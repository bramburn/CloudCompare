#!/usr/bin/env python3
"""
Rewrite every cross-doc relative link in website/docs/ to an absolute
`/docs/<path>` link, resolving the path against the current file's
folder so sibling links like `](gl)` in `docs/plugins/standard.md`
become `](/docs/plugins/gl)`.

Patterns handled:
  ](sibling)            in docs/foo/bar.md          -> ](/docs/foo/sibling)
  ](../sibling/foo)      in docs/foo/bar/baz.md      -> ](/docs/sibling/foo)
  ](./sibling)           in docs/foo/bar.md          -> ](/docs/foo/sibling)
  ](../../sibling)       in docs/foo/bar/baz.md      -> ](/docs/sibling)

Links that are already absolute (http/https, /, #, mailto, or
images that are already in /img/) are left alone.
"""

import re
import sys
from pathlib import Path

DOCS_DIR = Path("website/docs")
LINK_RE = re.compile(r"(!?\[)([^\]]*)\]\(([^)]+)\)")


def is_absolute_or_external(url: str) -> bool:
    return (
        url.startswith("http://")
        or url.startswith("https://")
        or url.startswith("/")
        or url.startswith("#")
        or url.startswith("mailto:")
    )


def normalise(url: str, docs_rel_folder: str) -> str:
    """Resolve a relative link against the current docs folder."""
    if is_absolute_or_external(url):
        return url

    # Split off anchor + query
    anchor_suffix = ""
    if "#" in url:
        url, anchor = url.split("#", 1)
        anchor_suffix = f"#{anchor}"
    if "?" in url:
        url = url.split("?", 1)[0]

    url = url.replace("\\", "/")

    # Resolve the path
    if url.startswith("/"):
        # Absolute within docs/
        rel = url.lstrip("/")
    else:
        # Relative to current docs folder
        rel = (Path(docs_rel_folder) / url).as_posix()

    # Normalise . and ..
    parts = []
    for seg in rel.split("/"):
        if seg in ("", "."):
            continue
        if seg == "..":
            if parts:
                parts.pop()
            continue
        parts.append(seg)
    return "/docs/" + "/".join(parts) + anchor_suffix


def rewrite(file_path: Path) -> bool:
    rel = file_path.relative_to(DOCS_DIR.parent).as_posix()
    # rel is "website/docs/<folder>/<file>" — strip "website/docs/"
    docs_rel = rel[len("website/docs/"):]
    docs_rel_folder = str(Path(docs_rel).parent).replace("\\", "/")
    if docs_rel_folder == ".":
        docs_rel_folder = ""

    text = file_path.read_text(encoding="utf-8")

    def replace(m: re.Match) -> str:
        prefix, alt, url = m.group(3), m.group(2), m.group(3)
        # Re-parse: group(1) is "[" or "![", group(2) is the alt, group(3) is the URL
        prefix = m.group(1)
        alt = m.group(2)
        url = m.group(3)
        new_url = normalise(url, docs_rel_folder)
        if new_url == url:
            return m.group(0)
        return f"{prefix}{alt}]({new_url})"

    new_text = LINK_RE.sub(replace, text)
    if new_text == text:
        return False
    file_path.write_text(new_text, encoding="utf-8")
    return True


def main() -> int:
    changed = 0
    for path in DOCS_DIR.rglob("*.md"):
        if rewrite(path):
            changed += 1
    for path in DOCS_DIR.rglob("*.mdx"):
        if rewrite(path):
            changed += 1
    print(f"{changed} files updated")
    return 0


if __name__ == "__main__":
    sys.exit(main())

#!/usr/bin/env python3
"""
Targeted fix-up pass for absolute `/docs/<short>` links that should
have been `/docs/<folder>/<short>`. Scoped to each file's docs-relative
folder so the same short name can be safely rewritten in one folder
without touching another.

Handles:
  /docs/<short>            in docs/<folder>/X.md  ->  /docs/<folder>/<short>
  /docs/<short>#anchor     in docs/<folder>/X.md  ->  /docs/<folder>/<short>#anchor

And the corrupted `/docs/ture/` (missing 'architec') and `/docs/started/`
(missing 'getting-') from the earlier buggy rewrite.

Intentionally leaves already-correct `/docs/<folder>/<short>` links alone.
"""

import re
import sys
from pathlib import Path

DOCS_DIR = Path("website/docs")
LINK_RE = re.compile(r"\]\(/docs/([^)#?]+)([#?][^)]*)?\)")
KNOWN_FOLDERS = {
    "getting-started",
    "build",
    "plugins",
    "architecture",
    "ci",
    "reference",
}


def is_already_correct(target: str) -> bool:
    if target == "intro":
        return True
    return any(target.startswith(f"{f}/") for f in KNOWN_FOLDERS)


def fix_file(file_path: Path) -> bool:
    rel = file_path.relative_to(DOCS_DIR.parent).as_posix()
    docs_rel = rel[len("website/docs/"):]
    folder = str(Path(docs_rel).parent).replace("\\", "/")
    if folder == ".":
        folder = ""

    text = file_path.read_text(encoding="utf-8")

    def replace(m: re.Match) -> str:
        target = m.group(1)
        suffix = m.group(2) or ""

        # Handle the corrupted forms
        if target.startswith("ture/") and folder == "architecture":
            target = "architecture/" + target[len("ture/"):]
        elif target.startswith("started/") and folder == "getting-started":
            target = "getting-started/" + target[len("started/"):]

        if is_already_correct(target):
            return m.group(0)

        # If the file is at the docs root (e.g. intro.md), keep `/docs/<target>`.
        if not folder:
            return m.group(0)

        # If the target already lives in a known folder, don't double-prefix.
        if any(target.startswith(f"{f}/") for f in KNOWN_FOLDERS):
            return m.group(0)

        return f"](/docs/{folder}/{target}{suffix})"

    new_text = LINK_RE.sub(replace, text)
    if new_text == text:
        return False
    file_path.write_text(new_text, encoding="utf-8")
    return True


def main() -> int:
    changed = 0
    for path in DOCS_DIR.rglob("*.md"):
        if fix_file(path):
            changed += 1
            print(f"fixed: {path}")
    for path in DOCS_DIR.rglob("*.mdx"):
        if fix_file(path):
            changed += 1
            print(f"fixed: {path}")
    print(f"\n{changed} files updated")
    return 0


if __name__ == "__main__":
    sys.exit(main())

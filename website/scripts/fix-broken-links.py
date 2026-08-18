#!/usr/bin/env python3
"""
Fix the broken `/docs/<short>` links that the previous rewrite pass
created: rewrite them to the correct `/docs/<folder>/<short>` based on
the current file's docs-relative folder.
"""

import re
import sys
from pathlib import Path

DOCS_DIR = Path("website/docs")
LINK_RE = re.compile(r"(!?\[)([^\]]*)\]\((/docs/)([^)#?]+)([#?][^)]*)?\)")
KNOWN_FOLDERS = {
    "getting-started",
    "build",
    "plugins",
    "architecture",
    "ci",
    "reference",
}


def is_already_correct(target: str) -> bool:
    return any(target.startswith(f"/docs/{f}/") for f in KNOWN_FOLDERS) or target == "/docs/intro"


def fix(file_path: Path) -> bool:
    rel = file_path.relative_to(DOCS_DIR.parent).as_posix()
    docs_rel = rel[len("website/docs/"):]
    docs_rel_folder = str(Path(docs_rel).parent).replace("\\", "/")
    if docs_rel_folder == ".":
        docs_rel_folder = ""
    text = file_path.read_text(encoding="utf-8")

    def replace(m: re.Match) -> str:
        prefix, alt, prefix_slash, target, suffix = m.groups()
        suffix = suffix or ""
        if is_already_correct(prefix_slash + target):
            return m.group(0)
        # Resolve the short target against the current folder
        if docs_rel_folder:
            new_target = f"/docs/{docs_rel_folder}/{target}"
        else:
            new_target = f"/docs/{target}"
        return f"{prefix}{alt}]({new_target}{suffix})"

    new_text = LINK_RE.sub(replace, text)
    if new_text == text:
        return False
    file_path.write_text(new_text, encoding="utf-8")
    return True


def main() -> int:
    changed = 0
    for path in DOCS_DIR.rglob("*.md"):
        if fix(path):
            changed += 1
            print(f"fixed: {path}")
    for path in DOCS_DIR.rglob("*.mdx"):
        if fix(path):
            changed += 1
            print(f"fixed: {path}")
    print(f"\n{changed} files updated")
    return 0


if __name__ == "__main__":
    sys.exit(main())

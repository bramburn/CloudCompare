#!/usr/bin/env python3
"""
Strip the `id:` line from the front-matter of every doc under
website/docs/ EXCEPT docs/intro.md (which needs id:intro and slug:/).
"""

import re
import sys
from pathlib import Path

DOCS_DIR = Path("website/docs")
KEEP_ID = {"website/docs/intro.md"}


def strip_id(file_path: Path) -> bool:
    rel = str(file_path).replace("\\", "/")
    if rel in KEEP_ID:
        return False

    text = file_path.read_text(encoding="utf-8")
    # Match: `id: something\n` immediately after the opening `---`
    new_text = re.sub(
        r"^(---)\s*\nid:[^\n]*\n",
        r"\1\n",
        text,
        count=1,
        flags=re.MULTILINE,
    )
    if new_text == text:
        return False
    file_path.write_text(new_text, encoding="utf-8")
    return True


def main() -> int:
    changed = 0
    for path in DOCS_DIR.rglob("*.md"):
        if strip_id(path):
            changed += 1
            print(f"stripped id: {path}")
    for path in DOCS_DIR.rglob("*.mdx"):
        if strip_id(path):
            changed += 1
            print(f"stripped id: {path}")
    print(f"\n{changed} files updated")
    return 0


if __name__ == "__main__":
    sys.exit(main())

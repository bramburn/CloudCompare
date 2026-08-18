#!/usr/bin/env python3
"""Downscale + convert hero illustrations to WebP for web delivery.

The 2K source PNGs from image_synthesize are 2-3 MB each — far too large
for inline hero use. This script produces a WebP twin per file with a
reasonable max-width and quality.
"""
from __future__ import annotations

import os
from pathlib import Path

from PIL import Image

STATIC = Path(__file__).resolve().parents[1] / "static" / "img"
TARGETS = [
    ("hero-illustration.png", 1600, 82),
    ("workflow-comparison.png", 1400, 82),
]


def main() -> int:
    for name, max_w, quality in TARGETS:
        src = STATIC / name
        if not src.exists():
            print(f"skip {name} (not found)")
            continue
        im = Image.open(src).convert("RGB")
        if im.width > max_w:
            ratio = max_w / im.width
            im = im.resize((int(im.width * ratio), int(im.height * ratio)), Image.LANCZOS)
        out = src.with_suffix(".webp")
        im.save(out, "WEBP", quality=quality, method=6)
        kb = os.path.getsize(out) / 1024
        print(f"{name} -> {out.name}: {im.size}, {kb:.1f} KB (q={quality})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

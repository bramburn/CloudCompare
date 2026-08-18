#!/usr/bin/env python3
"""
Optimize the AI-generated PNGs into web-friendly sizes, replacing the
old Docusaurus defaults in place so the build picks up the new assets:

  logo.png                  <- 256x256 RGBA optimized
  logo.svg                  <- 256x256 PNG embedded as base64 inside SVG
  social-card.png           <- 1200x630 (used as the OG image)
  favicon.ico               <- 16+32+48 multi-size .ico
  favicon-32.png            <- 32x32 PNG (modern browsers)
  hero-bg.png               <- 1920x900 hero background
  docusaurus-social-card.jpg <- preserved as a tiny JPG fallback
"""

import io
import base64
from pathlib import Path

try:
    from PIL import Image
except ImportError:
    print("Pillow is required:  pip install Pillow")
    raise


STATIC = Path("website/static/img")


def resize_square(src: Path, size: int, dest: Path) -> None:
    img = Image.open(src).convert("RGBA")
    img = img.resize((size, size), Image.LANCZOS)
    img.save(dest, "PNG", optimize=True)
    print(f"  {dest}  {dest.stat().st_size // 1024} KB  ({size}x{size})")


def resize_landscape(src: Path, size: tuple[int, int], dest: Path) -> None:
    img = Image.open(src).convert("RGBA")
    img = img.resize(size, Image.LANCZOS)
    img.save(dest, "PNG", optimize=True)
    print(f"  {dest}  {dest.stat().st_size // 1024} KB  ({size[0]}x{size[1]})")


def png_to_ico(src: Path, dest_ico: Path) -> None:
    img = Image.open(src).convert("RGBA")
    img.thumbnail((256, 256), Image.LANCZOS)
    img_16 = img.resize((16, 16), Image.LANCZOS)
    img_32 = img.resize((32, 32), Image.LANCZOS)
    img_48 = img.resize((48, 48), Image.LANCZOS)
    img_16.save(
        dest_ico,
        format="ICO",
        sizes=[(16, 16), (32, 32), (48, 48)],
        append_images=[img_32, img_48],
    )
    print(f"  {dest_ico}  {dest_ico.stat().st_size} bytes (16+32+48 ico)")


def png_to_svg_wrapper(png_path: Path, svg_path: Path) -> None:
    """Embed a PNG as a base64 data-URI inside a tiny SVG wrapper."""
    img = Image.open(png_path).convert("RGBA")
    img = img.resize((256, 256), Image.LANCZOS)
    buf = io.BytesIO()
    img.save(buf, "PNG", optimize=True)
    b64 = base64.b64encode(buf.getvalue()).decode("ascii")
    svg = (
        f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 256 256" '
        f'width="256" height="256" role="img" aria-label="CloudCompare logo">\n'
        f'  <image href="data:image/png;base64,{b64}" '
        f'x="0" y="0" width="256" height="256"/>\n'
        f'</svg>\n'
    )
    svg_path.write_text(svg, encoding="utf-8")
    print(f"  {svg_path}  {svg_path.stat().st_size // 1024} KB  (svg wrapper)")


def main() -> int:
    print("Optimizing logo:")
    if (STATIC / "logo.png").exists():
        resize_square(STATIC / "logo.png", 256, STATIC / "logo.png")
        png_to_svg_wrapper(STATIC / "logo.png", STATIC / "logo.svg")
    print()

    print("Optimizing social card:")
    if (STATIC / "social-card.png").exists():
        resize_landscape(STATIC / "social-card.png", (1200, 630), STATIC / "social-card.png")
    print()

    print("Optimizing favicon:")
    if (STATIC / "favicon-gen.png").exists():
        png_to_ico(STATIC / "favicon-gen.png", STATIC / "favicon.ico")
        resize_square(STATIC / "favicon-gen.png", 32, STATIC / "favicon-32.png")
    print()

    print("Optimizing hero background:")
    if (STATIC / "hero-bg-gen.png").exists():
        resize_landscape(STATIC / "hero-bg-gen.png", (1920, 900), STATIC / "hero-bg.png")
    print()

    return 0


if __name__ == "__main__":
    raise SystemExit(main())

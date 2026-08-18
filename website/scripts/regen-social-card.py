#!/usr/bin/env python3
"""Regenerate the OpenGraph / Twitter card with the actual CloudCompare logo.

Source of truth: `qCC/images/icon/cc_icon_256.png` (the upstream CloudCompare
"CC" mark, GPLv2-or-later — compatible with this fork's license).  We do not
generate the logo from scratch — the user wanted the *real* CC mark.

Output:  `website/static/img/social-card.png` (1200x630, RGBA, < 500 KB).

Layout:
  - Background: deep-navy gradient (`#0b1220` -> `#1e3a5f`) with subtle
    point-cloud / grid texture.
  - Left third: the actual CC logo at 320px, with a cyan glow.
  - Right two-thirds: "CloudCompare" title (sans-serif, white), tagline
    (cyan), and a small "Icelabz fork · bramburn/CloudCompare" badge.
"""
from __future__ import annotations

import math
import os
import sys
from pathlib import Path

from PIL import Image, ImageDraw, ImageFilter, ImageFont

REPO_ROOT = Path(__file__).resolve().parents[2]
LOGO_SRC = REPO_ROOT / "qCC" / "images" / "icon" / "cc_icon_256.png"
OUT_PATH = REPO_ROOT / "website" / "static" / "img" / "social-card.png"

CARD_W, CARD_H = 1200, 630
NAVY = (11, 18, 32, 255)          # #0b1220
NAVY_MID = (24, 42, 70, 255)
NAVY_LIGHT = (30, 58, 95, 255)    # #1e3a5f
CYAN = (6, 182, 212, 255)         # #06b6d4
CYAN_BRIGHT = (103, 232, 249, 255)
WHITE = (255, 255, 255, 255)
MUTED = (180, 200, 220, 230)


def find_font(candidates: list[str], size: int) -> ImageFont.FreeTypeFont:
    for c in candidates:
        if os.path.exists(c):
            try:
                return ImageFont.truetype(c, size)
            except OSError:
                continue
    return ImageFont.load_default()


def gradient_bg(w: int, h: int) -> Image.Image:
    """Diagonal navy gradient."""
    base = Image.new("RGBA", (w, h), NAVY)
    draw = ImageDraw.Draw(base)
    for y in range(h):
        # diagonal blend factor
        t = (y / h) * 0.55 + (math.sin(y / h * math.pi) * 0.15)
        r = int(NAVY[0] + (NAVY_LIGHT[0] - NAVY[0]) * t)
        g = int(NAVY[1] + (NAVY_LIGHT[1] - NAVY[1]) * t)
        b = int(NAVY[2] + (NAVY_LIGHT[2] - NAVY[2]) * t)
        draw.line([(0, y), (w, y)], fill=(r, g, b, 255))
    return base


def point_cloud_texture(w: int, h: int, density: int = 220, seed: int = 7) -> Image.Image:
    """Sparse point cloud — overlay layer."""
    import random

    rng = random.Random(seed)
    layer = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    draw = ImageDraw.Draw(layer)
    for _ in range(density):
        x = rng.randint(0, w)
        y = rng.randint(0, h)
        size = rng.choice([1, 1, 1, 2, 2, 3])
        alpha = rng.randint(40, 130)
        col = rng.choice([CYAN, (96, 165, 250, alpha), (148, 163, 184, alpha)])
        draw.ellipse([x - size, y - size, x + size, y + size], fill=col)
    return layer


def cyan_glow(w: int, h: int) -> Image.Image:
    """Two soft radial glows for visual depth."""
    layer = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    glow_a = Image.new("RGBA", (600, 600), (0, 0, 0, 0))
    ImageDraw.Draw(glow_a).ellipse([0, 0, 600, 600], fill=(6, 182, 212, 90))
    glow_a = glow_a.filter(ImageFilter.GaussianBlur(80))
    layer.alpha_composite(glow_a, dest=(-100, -150))

    glow_b = Image.new("RGBA", (500, 500), (0, 0, 0, 0))
    ImageDraw.Draw(glow_b).ellipse([0, 0, 500, 500], fill=(37, 99, 235, 100))
    glow_b = glow_b.filter(ImageFilter.GaussianBlur(80))
    layer.alpha_composite(glow_b, dest=(w - 380, h - 350))
    return layer


def paste_logo(card: Image.Image, logo: Image.Image) -> None:
    """Place the actual CC logo on the left with a subtle cyan glow."""
    target_h = 320
    ratio = target_h / logo.height
    target_w = int(logo.width * ratio)
    logo_resized = logo.resize((target_w, target_h), Image.LANCZOS)

    # glow underlay
    glow_layer = Image.new("RGBA", (target_w + 80, target_h + 80), (0, 0, 0, 0))
    glow_logo = logo_resized.copy()
    # add a cyan tint by alpha-compositing over a cyan silhouette
    cyan_silhouette = Image.new("RGBA", logo_resized.size, (CYAN[0], CYAN[1], CYAN[2], 200))
    masked = Image.composite(cyan_silhouette, Image.new("RGBA", logo_resized.size, (0, 0, 0, 0)), logo_resized.split()[3])
    glow_layer.alpha_composite(masked, dest=(40, 40))
    glow_layer = glow_layer.filter(ImageFilter.GaussianBlur(40))

    paste_x = 120
    paste_y = (card.height - target_h) // 2
    card.alpha_composite(glow_layer, dest=(paste_x - 40, paste_y - 40))
    card.alpha_composite(logo_resized, dest=(paste_x, paste_y))


def draw_text(card: Image.Image, fonts: dict[str, ImageFont.FreeTypeFont]) -> None:
    draw = ImageDraw.Draw(card)
    text_x = 540
    # Brand line
    draw.text(
        (text_x, 180),
        "CloudCompare",
        font=fonts["title"],
        fill=WHITE,
    )
    # Tagline
    draw.text(
        (text_x, 290),
        "3D point cloud & mesh processing",
        font=fonts["tagline"],
        fill=CYAN_BRIGHT,
    )
    # Divider
    draw.line([(text_x, 360), (text_x + 120, 360)], fill=CYAN, width=3)
    # Description (multiline)
    desc = (
        "Open-source 3D point cloud & mesh processing.\n"
        "C++17 · Qt 6 · OpenGL 2.1+ · CMake 3.10+."
    )
    draw.multiline_text(
        (text_x, 385),
        desc,
        font=fonts["body"],
        fill=MUTED,
        spacing=8,
    )
    # Footer badge
    badge_y = 530
    draw.rounded_rectangle(
        [(text_x, badge_y), (text_x + 400, badge_y + 46)],
        radius=23,
        fill=(6, 182, 212, 36),
        outline=CYAN,
        width=2,
    )
    draw.text(
        (text_x + 22, badge_y + 14),
        "Icelabz fork · bramburn/CloudCompare",
        font=fonts["badge"],
        fill=CYAN_BRIGHT,
    )


def main() -> int:
    if not LOGO_SRC.exists():
        print(f"FATAL: {LOGO_SRC} not found", file=sys.stderr)
        return 1

    card = gradient_bg(CARD_W, CARD_H)
    card.alpha_composite(cyan_glow(CARD_W, CARD_H))
    card.alpha_composite(point_cloud_texture(CARD_W, CARD_H))

    logo = Image.open(LOGO_SRC).convert("RGBA")
    paste_logo(card, logo)

    fonts = {
        "title": find_font(
            [
                "C:/Windows/Fonts/segoeuib.ttf",
                "C:/Windows/Fonts/arialbd.ttf",
                "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",
                "/System/Library/Fonts/Helvetica.ttc",
            ],
            88,
        ),
        "tagline": find_font(
            [
                "C:/Windows/Fonts/segoeui.ttf",
                "C:/Windows/Fonts/arial.ttf",
                "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
            ],
            36,
        ),
        "body": find_font(
            [
                "C:/Windows/Fonts/segoeui.ttf",
                "C:/Windows/Fonts/arial.ttf",
                "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
            ],
            24,
        ),
        "badge": find_font(
            [
                "C:/Windows/Fonts/segoeui.ttf",
                "C:/Windows/Fonts/consola.ttf",
                "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
            ],
            20,
        ),
    }
    draw_text(card, fonts)

    # optimize: convert to RGB for size; PNG compresses better without alpha
    final = card.convert("RGB")
    OUT_PATH.parent.mkdir(parents=True, exist_ok=True)
    final.save(OUT_PATH, "PNG", optimize=True)

    size_kb = OUT_PATH.stat().st_size / 1024
    print(f"OK social-card.png -> {OUT_PATH} ({size_kb:.1f} KB)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

---
title: Take a viewport screenshot
sidebar_label: Screenshot at fixed DPI
sidebar_position: 12
---

# Take a viewport screenshot

Export the current 3D view as an image at a specific resolution,
independent of the on-screen window size.

## Prerequisites

- The viewport showing what you want to capture

## Steps

1. `Display > Render to file`.
2. Pick a filename and output format (PNG recommended for reports).
3. Enter width and height in pixels. The output is rendered at
   exactly those dimensions.
4. The render uses the current camera angle, lighting, scalar field
   display, and color mapping — set these up before rendering.

## DPI guidelines

| Use | Recommended px width |
|---|---|
| Report / A4 print at 300 DPI | 3508 px |
| Report / A4 print at 150 DPI | 1754 px |
| Web / doc page hero | 1600 px |
| Thumbnail / social | 800 px |

Render at 3000–4000 px wide for print-quality deliverables.

## Next steps

- Include the screenshot in a report alongside
  [scalar field statistics](/docs/cookbook/scalar-field-colors) and
  [distance measurements](/docs/cookbook/measurement).

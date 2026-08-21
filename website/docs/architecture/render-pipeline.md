---
title: Render pipeline
sidebar_label: Render pipeline
sidebar_position: 6
---

# Render pipeline

How CloudCompare draws a frame. This page describes the full
sequence from a `QOpenGLWidget::paintGL()` call to the final image
on screen.

## Overview

The viewport is a `QOpenGLWidget`. Every frame triggers `paintGL()`,
which calls `doPaintGL()`. The render pipeline has five stages:

```
paintGL()
 └─ doPaintGL()
     ├─ 1. Init GL context
     ├─ 2. Full-render pass (FBO or on-screen)
     │   ├─ 2a. Background (sky / gradient)
     │   ├─ 2b. 3D entities (painter's algorithm)
     │   └─ 2c. Foreground (gizmos, pivot)
     ├─ 3. LOD refinement (optional)
     ├─ 4. GL post-processing (EDL / SSAO)
     └─ 5. 2D overlay (labels, cross, rulers)
```

## 1. Context init

`initPaintGL()` sets up the OpenGL state for the frame:
- `glViewport` — clip to the viewport rect
- `glClear` — colour and depth buffers
- `glEnable` — depth test, lighting, normalise
- `glMatrixMode` — modelview and projection
- Lights and materials from `ccGuiParameters`

The viewport is always portrait-aligned (rotation applied to the
modelview matrix, not the framebuffer).

## 2. Full-render pass

### 2a. Background

Clears the framebuffer with the current background colour. The sky
gradient is drawn as a full-screen quad in `drawBackground()`.

### 2b. 3D entities — painter's algorithm

The core rendering loop. Entities are sorted **back to front** by their
screen-space z-centre (the z-component of their bounding-box centre in
view space). Each entity calls `ccDrawableObject::draw()` with a
`CC_DRAW_CONTEXT` that carries the current GL state.

```
for each visible entity (sorted back-to-front):
    setEntityGLTransformation()
    setMaterialAndLighting()
    entity.draw(drawContext)
```

The `CC_DRAW_CONTEXT` carries:

| Field | Contents |
|---|---|
| `glFunctions` | `ccQOpenGLFunctions*` — Qt-wrapped GL function table |
| `_winSize` | Viewport width / height |
| `renderingParams` | Current LOD level, point size, light state |
| `display` | `ccGuiParameters` — per-entity visibility flags |

**Point clouds** render as `GL_POINTS` with per-point size from
`m_pointSize`. The shader path (`USE_SHADER`) draws points with
colour from the active scalar field; the fixed-function path falls back
to `glVertexPointer` + `glColorPointer`.

**Meshes** render as `GL_TRIANGLES`. Each mesh checks for a
`ccMaterial`; if present, it binds the texture and calls
`glMaterialfv` for Phong parameters.

### 2c. Foreground

Gizmos, pivot point, and the origin axis cross. Drawn last so they
always appear on top.

## 3. Level of Detail (LOD)

For clouds with more than `minLoDCloudSize` points, CloudCompare
renders a downsampled version first:

1. Render to the FBO at full resolution (the reference frame).
2. If the rendered image shows more than `minLoDCloudSize` points at
   the current zoom level, render a ½-resolution downsampled version.
3. If still too dense, render at ¼, and so on.
4. The first level where the point count drops below the threshold
   becomes the stable display level.

The `ccPointCloudLOD` class manages the per-level arrays and the
downsampling decision. LOD is enabled/disabled via
`ccGLWindowInterface::setLODEnabled()`.

## 4. GL post-processing (EDL / SSAO)

When a GL filter plugin (qEDL or qSSAO) is active, the frame is
rendered to an FBO first, then the filter applies a full-screen
post-processing pass:

### Eye-Dome Lighting (qEDL)

A screen-space shader that shades points based on depth discontinuities:

```glsl
// Pseudocode
for each screen pixel P:
    sample depth of 8 neighbours
    compute max depth delta
    shade factor = exp(-maxDelta * strength)
    output colour = baseColour * shadeFactor
```

EDL works purely on the depth buffer — no geometry is sent to the GPU.
It makes untextured point clouds look solid by adding the illusion of
surface structure at corners and edges.

### Screen-Space Ambient Occlusion (qSSAO)

Samples the depth buffer in a hemisphere around each pixel to estimate
how much the pixel is occluded by nearby geometry. Adds contact shadows
in crevices and under overhangs.

Both filters require FBO support (`ccFrameBufferObject` wraps the
`QOpenGLFramebufferObject`). If FBO is unavailable, GL filters are
silently disabled.

## 5. Stereo rendering

`ccGLWindowStereo` extends the pipeline with per-eye passes:

- **Anaglyph**: render left eye → red channel, right eye → cyan
  channel, composite into one image.
- **Page flip**: two on-screen buffers, alternate every vsync.
- **HDMI 3D**: left/right in top/bottom or side-by-side.
- **Interleaved**: NVIDIA 3D Vision interleaved rows.

The active stereo mode is `m_stereoModeEnabled`. When active,
`doPaintGL()` calls `fullRenderingPass()` twice (left + right) before
the overlay pass.

## 6. 2D overlay

Everything drawn in screen (pixel) space after the 3D pass:

- **Pivot point** — small sphere or cross at the rotation pivot.
- **Axis cross** — X/Y/Z arrows in the corner.
- **Hot zone labels** — "Scale bar", "Point count", etc. drawn via
  `renderText()` which uses `QPainter` to rasterise text into a texture
  and draws it as a billboard.
- **Rulers and annotations** — user-placed 2D labels.
- **Cross-hairs** — optional centred cross for alignment work.

## Render to file

`ccGLWindowInterface::renderToFile()` exports the current frame as an
image:

```cpp
QImage outputImage = renderToImage(zoomFactor, dontScaleFeatures, renderOverlayItems);
```

`renderToImage()` temporarily redirects the FBO or on-screen buffer to
a `QImage` via `glReadPixels`. The zoom factor scales the output
resolution independently of the on-screen size.

## What to read next

- [Plugin system](/docs/architecture/plugin-system) — how GL plugins hook into the pipeline.
- [UI](/docs/architecture/ui) — how the viewport widget is embedded in the main window.

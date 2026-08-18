---
title: GL plugins
sidebar_label: GL
sidebar_position: 6
---

# GL plugins

GL plugins are OpenGL post-filters. They hook into the 3D viewport's
framebuffer after the main scene has been drawn and add a screen-space
effect (eye-dome lighting, SSAO, color grading, …).

## The interface

```cpp
class IGLPlugin : public ccPluginInterface {
public:
  // Short name shown in the viewport's filter menu.
  virtual QString getName() const = 0;

  // True if the plugin is willing to draw something this frame.
  // Use this to early-out (e.g. when the filter is toggled off).
  virtual bool isEnabled() const = 0;

  // The actual draw. Use glFunctions() to get a QOpenGLFunctions
  // pointer; render into the viewport's current framebuffer.
  virtual void draw(CC_DRAW_CONTEXT& context) = 0;
};
```

`CC_DRAW_CONTEXT` is the structure passed to every GL callback. It
includes the viewport size, the camera, the list of visible entities,
and the active LOD level.

## A canonical example: qEDL

`qEDL` (Eye Dome Lighting) is the smallest useful GL plugin. The
relevant files:

```
plugins/core/GL/qEDL/
├── CMakeLists.txt
├── edl.h              // class EDLFilter : public QObject, public ccGLPluginInterface
├── edl.cpp
├── shaders/
│   ├── edl_vert.glsl
│   └── edl_frag.glsl
└── Resources/
    └── icon.png
```

`EDLFilter::draw` flow:

1. Bind a screen-space quad's FBO as the render target.
2. Pass 1: render the cloud's depth to a depth texture.
3. Pass 2: the fragment shader samples the depth texture at the current
   pixel and at eight surrounding pixels, computes a normal-ish gradient,
   and shades accordingly.
4. Composite the result back to the viewport's main framebuffer.

The full effect runs in under 1&nbsp;ms per frame for a 1080p viewport
on a modern GPU; this is why the fork enables `qEDL` by default.

## When to use the GL pattern

Reach for a GL plugin when your effect:

- Operates on the **viewport's framebuffer** (post-process), not the
  scene's geometry.
- Can be expressed as a **screen-space shader** (a vertex+fragment
  pair).
- Doesn't need to **read or write entity data** in the db-tree.

If your effect changes how entities are drawn (e.g. a different point
shape or a per-point size formula), look at the
`ccDrawable`/`ccGenericPrimitive` override mechanism instead. If it
generates new entities from the existing scene (e.g. a mesh-from-points
tool), it's a Standard plugin.

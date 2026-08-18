# libs/qCC_glWindow/AGENTS.md

The OpenGL 3D viewport library. Owns `ccGLWindowInterface` (the public viewport API) and `ccGLWindow` (the implementation).

**CMake target:** `QCC_GL_LIB` (SHARED, LGPL).
**Public headers:** `include/`.

Read [`../../AGENTS-libs.md`](../../AGENTS-libs.md) §4 for the public surface; [`../../AGENTS-ui.md`](../../AGENTS-ui.md) §7 for the runtime patterns.

## Folder layout

```
qCC_glWindow/
├── CMakeLists.txt
├── include/                       # public headers (the API)
├── src/                           # implementations
└── doc/                           # internal notes
```

## Key public headers

| Header | Why you care |
|---|---|
| `ccGLWindowInterface.h` | The public viewport API — render, pick, refresh, setPointSize, getViewportParameters. Reference this from `qCC/` and from plugins; never `ccGLWindow.h` directly. |
| `ccGLWindow.h` | The concrete implementation (private to the lib). |
| `ccRenderingTools.h` | Static helpers for 2D HUD drawing. |
| `ccGlFilter.h` | Base class for GL filter plugins (`ccGLPluginInterface::getFilter()` returns a `ccGlFilter*`). |
| `ccShader.h` | Base class for shader programs. |
| `ccGenericGLDisplay.h`, `ccGLUtils.h` | Foundational GL helpers (some in `qCC_db`). |

## Don't

- Don't reference `ccGLWindow` from outside the lib — use `ccGLWindowInterface`.
- Don't add new GL state to `ccGLWindow` without a matching accessor in `ccGLWindowInterface`.
- Don't use raw OpenGL calls in a plugin's `onNewSelection` — schedule a redraw via `ccGLWindowInterface::redraw()` or `m_app->refreshAll()`.

## See also

- Root [`../../AGENTS.md`](../../AGENTS.md)
- [`../../AGENTS-architecture.md`](../../AGENTS-architecture.md) §6 (rendering sketch)
- [`../../AGENTS-libs.md`](../../AGENTS-libs.md) §4
- [`../../AGENTS-ui.md`](../../AGENTS-ui.md) §7
- [`../../AGENTS-plugin-dev.md`](../../AGENTS-plugin-dev.md) §1.3 (how to add a GL plugin)

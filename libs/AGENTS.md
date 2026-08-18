# libs/AGENTS.md

The vendored dependencies that everything else builds on. **Don't add new code here unless you've read [`AGENTS-architecture.md`](../AGENTS-architecture.md) and confirmed a plugin can't do it.**

## What's here

| Folder | Target | Role | Deep dive |
|---|---|---|---|
| `qCC_db/` | `QCC_DB_LIB` | The 3D scene-graph (`ccHObject` hierarchy) | [`../AGENTS-libs.md`](../AGENTS-libs.md) §2 |
| `qCC_io/` | `QCC_IO_LIB` | The file-format I/O layer (`FileIOFilter` registry) | [`../AGENTS-libs.md`](../AGENTS-libs.md) §3 |
| `qCC_glWindow/` | `QCC_GL_LIB` | The OpenGL 3D viewport (`ccGLWindowInterface`) | [`../AGENTS-libs.md`](../AGENTS-libs.md) §4 |
| `CCFbo/` | `CC_FBO_LIB` | Tiny FBO helper | [`../AGENTS-libs.md`](../AGENTS-libs.md) §5 |
| `CCPluginAPI/` | `CCPluginAPI` | **Stable public surface** for plugins (`ccMainAppInterface`, …) | [`../AGENTS-libs.md`](../AGENTS-libs.md) §6 |
| `CCPluginStub/` | `CCPluginStub` | Plugin-side base classes (`ccStdPluginInterface`, …) | [`../AGENTS-libs.md`](../AGENTS-libs.md) §7 |
| `CCAppCommon/` | `CCAppCommon` | App scaffolding shared by `qCC` and `ccViewer` (plugin manager, 3D-mouse, recent files, dark style) | [`../AGENTS-libs.md`](../AGENTS-libs.md) §8 |

The vendored submodules (do not edit):
- `qCC_db/extern/CCCoreLib/` — pure-C++ algorithms (submodule).
- `CCAppCommon/QDarkStyleSheet/` — QSS stylesheets (submodule).
- `CCAppCommon/devices/3dConnexion/extern/hidapi/` — hidapi C API (submodule, **caps CMake at 4.3**).

## Rules for editing libs

- **The plugins' contract is `CCCoreLib + CCPluginAPI + CCPluginStub`.** Don't link `qCC_db`, `qCC_io`, `qCC_glWindow`, or `CCAppCommon` from a plugin.
- **Don't break ABI in `CCPluginAPI` lightly** — bump the `Q_DECLARE_INTERFACE` version string if you change a header.
- **The CCCoreLib submodule** has its own `CONTRIBUTING.md` and `AGENTS.md`. Read those before editing `libs/qCC_db/extern/CCCoreLib/`.
- **The hidapi submodule's `CMakeLists.txt`** caps CMake at 4.3 — see root [`AGENTS.md`](../AGENTS.md) gotcha #1.

## Sub-folder AGENTS.md

- [`libs/qCC_db/AGENTS.md`](qCC_db/AGENTS.md)
- [`libs/qCC_io/AGENTS.md`](qCC_io/AGENTS.md)
- [`libs/qCC_glWindow/AGENTS.md`](qCC_glWindow/AGENTS.md)
- [`libs/CCFbo/AGENTS.md`](CCFbo/AGENTS.md)
- [`libs/CCPluginAPI/AGENTS.md`](CCPluginAPI/AGENTS.md)
- [`libs/CCPluginStub/AGENTS.md`](CCPluginStub/AGENTS.md)
- [`libs/CCAppCommon/AGENTS.md`](CCAppCommon/AGENTS.md)

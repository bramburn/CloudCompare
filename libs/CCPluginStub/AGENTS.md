# libs/CCPluginStub/AGENTS.md

The plugin-side base classes. Plugins `#include` these headers and inherit from the appropriate interface.

**CMake target:** `CCPluginStub` (header-only-ish).
**Public headers:** `include/`.

Read [`../../AGENTS-libs.md`](../../AGENTS-libs.md) §7.

## Folder layout

```
CCPluginStub/
├── CMakeLists.txt
└── include/
    ├── ccPluginInterface.h          # v3.2 — base for every plugin
    ├── ccDefaultPluginInterface.h   # common helpers (info.json loading, icon, …)
    ├── ccStdPluginInterface.h       # v1.5 — Standard plugins
    ├── ccIOPluginInterface.h        # v1.3 — I/O plugins
    └── ccGLPluginInterface.h        # v1.4 — GL plugins
```

## The versioned interfaces

Every interface file ends with a `Q_DECLARE_INTERFACE(<Name>, "<org>.<interface>/<version>")` macro. The version string is the API contract — bump it when you change the interface in a way that would break existing plugins.

Current versions (don't change without reading [`../../AGENTS-plugin-dev.md`](../../AGENTS-plugin-dev.md) §6):
- `ccPluginInterface` — **3.2**
- `ccStdPluginInterface` — **1.5**
- `ccIOPluginInterface` — **1.3**
- `ccGLPluginInterface` — **1.4**

## Adding a new pure-virtual method

You **must**:
1. Give it a default `{}` body if you want old plugins to keep compiling.
2. Otherwise, bump the version string and update `ccPluginManager`'s plugin-version check.

Read [`../../AGENTS-architecture.md`](../../AGENTS-architecture.md) §3.2 first.

## See also

- Root [`../../AGENTS.md`](../../AGENTS.md)
- [`../../AGENTS-architecture.md`](../../AGENTS-architecture.md) §3
- [`../../AGENTS-libs.md`](../../AGENTS-libs.md) §7
- [`../../AGENTS-plugin-dev.md`](../../AGENTS-plugin-dev.md)

# plugins/example/AGENTS.md

Copy-paste templates for new plugins. **Do not enable any of these in a production build** — their CMakeLists all default to `OFF` and they exist only to be copied.

```
plugins/example/
├── ExamplePlugin/         # Standard plugin template
├── ExampleIOPlugin/       # I/O plugin template
└── ExampleGLPlugin/       # GL plugin template
```

## How to use

```powershell
# Pick the template that matches your plugin type.
# Standard:
cp -r plugins/example/ExamplePlugin plugins/core/Standard/qMyFeature
# I/O:
cp -r plugins/example/ExampleIOPlugin plugins/core/IO/qMyFormat
# GL:
cp -r plugins/example/ExampleGLPlugin plugins/core/GL/qMyFilter
```

Then follow the rename checklist in [`../../AGENTS-plugin-dev.md`](../../AGENTS-plugin-dev.md) §1.

## What each template contains

### `ExamplePlugin/` — Standard template
- `ExamplePlugin.h/.cpp` — the `ccStdPluginInterface`-derived class with one `QAction`.
- `ActionA.h/.cpp` — the action handler (one pair per menu item).
- `ExamplePlugin.qrc` — Qt resource file. **Must be renamed** to `<your-plugin-folder>.qrc`.
- `info.json` — `{"type":"Standard", …}`.
- `images/icon.png` — placeholder icon (replace with your own).
- `CMakeLists.txt`, `include/CMakeLists.txt`, `src/CMakeLists.txt` — wiring.

### `ExampleIOPlugin/` — I/O template
- `ExampleIOPlugin.h/.cpp` — the `ccIOPluginInterface`-derived class.
- `FooFilter.h/.cpp` — one `FileIOFilter` subclass (one per file format).
- `ExampleIOPlugin.qrc` — Qt resource file.
- `info.json` — `{"type":"I/O", …}`.
- `CMakeLists.txt`, `include/CMakeLists.txt`, `src/CMakeLists.txt` — wiring.

### `ExampleGLPlugin/` — GL template
- `ExampleGLPlugin.h/.cpp` — the `ccGLPluginInterface`-derived class.
- `Bilateral.h/.cpp` — the `ccGlFilter` subclass (one per shader).
- `shaders/` — GLSL files (referenced via `SHADER_FOLDER <name>` in `AddPlugin`).
- `ExampleGLPlugin.qrc` — Qt resource file.
- `info.json` — `{"type":"GL", …}`.
- `CMakeLists.txt`, `include/CMakeLists.txt`, `src/CMakeLists.txt` — wiring.

## Don't

- Don't `AddPlugin` these templates into a production build — they're meant to be copied, not enabled.
- Don't change the example's `info.json` `"type"` — each template is for one type only.
- Don't keep the `Example` prefix anywhere in your copied plugin. Rename everything.

## See also

- [`../../AGENTS-plugin-dev.md`](../../AGENTS-plugin-dev.md) §1 — the full rename recipe
- [`../AGENTS.md`](../AGENTS.md) — plugin contract

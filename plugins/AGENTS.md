# plugins/AGENTS.md

**This is where new features live.** Most changes to CloudCompare — adding an analysis tool, a new file format, a new GL effect — happen here, **not** in `qCC/` or `libs/`.

Plugins are Qt-loadable shared objects (`SHARED` library). See [`../AGENTS-architecture.md`](../AGENTS-architecture.md) §3 and [`../AGENTS-plugin-dev.md`](../AGENTS-plugin-dev.md) for the full picture.

## Folder layout

```
plugins/
├── CMakeLists.txt                  # scans subdirectories, links AddPlugin helper
├── cmake/                          # AddPlugin() cmake helper (the contract)
├── core/
│   ├── CMakeLists.txt
│   ├── IO/                         # file-format I/O plugins
│   ├── Standard/                   # analysis/UI-action plugins
│   └── GL/                         # OpenGL post-filter plugins
├── example/                        # copy-paste templates (OFF by default)
│   ├── ExamplePlugin/              # Standard template
│   ├── ExampleIOPlugin/            # I/O template
│   └── ExampleGLPlugin/            # GL template
└── private/                        # ⚠ not committed — your in-development plugins go here
```

> **`plugins/private/` is gitignored.** Copy a template there to scaffold a new plugin without polluting the public tree. Once it's stable, propose it upstream by moving it into `core/<TYPE>/`.

## Plugin contract (do not deviate)

Every plugin folder:
1. **MUST** have a `CMakeLists.txt` whose first line is `option(PLUGIN_<TYPE>_<NAME> "..." OFF)`.
2. **MUST** call `AddPlugin(NAME ${PROJECT_NAME} [TYPE io|gl|standard] [SHADER_FOLDER <name>])` inside the `if(...)` block.
3. **MUST** have a `.qrc` file named exactly `<PluginFolderName>.qrc`.
4. **MUST** have an `info.json` with `"type"` ∈ `{Standard, GL, I/O}`.
5. **MUST** declare its `Q_PLUGIN_METADATA(IID "ccorp.cloudcompare.plugin.<Name>" FILE "../info.json")` macro in the class derived from a plugin interface.
6. **MUST** default the `option()` to `OFF` unless the plugin has no external dependency and should always be available.

These rules are enforced by `plugins/cmake/Plugins.cmake::AddPlugin` (it errors out at configure time if any are missing).

## The three archetypes

| Type | When | Key method | Template |
|---|---|---|---|
| **Standard** | menu action / toolbar button on current selection | `QList<QAction*> getActions()` + `void onNewSelection(...)` | `example/ExamplePlugin/` |
| **I/O** | new file format | `FilterList getFilters()` (returns `FileIOFilter::Shared`s) | `example/ExampleIOPlugin/` |
| **GL** | OpenGL post-process shader | `ccGlFilter* getFilter()` | `example/ExampleGLPlugin/` |

See [`../AGENTS-plugin-dev.md`](../AGENTS-plugin-dev.md) §1 for the full recipe per type.

## Sub-folder AGENTS.md

- [`plugins/core/AGENTS.md`](core/AGENTS.md) — the shipped plugin index
- [`plugins/example/AGENTS.md`](example/AGENTS.md) — the templates (don't enable in production)
- [`plugins/cmake/Plugins.cmake`](cmake/Plugins.cmake) — the `AddPlugin` helper (read this before duplicating any of it)

## Adding a new plugin to the build

```cmake
# 1. Drop your folder under plugins/core/<TYPE>/<MyName>/
# 2. In plugins/core/<TYPE>/CMakeLists.txt append:
add_subdirectory(<MyName>)
# 3. In your cc-configure.cmd add:
-DPLUGIN_<TYPE>_<MYNAME>=ON
# 4. Re-configure with --fresh, rebuild, test.
```

The full verification checklist is in [`../AGENTS-plugin-dev.md`](../AGENTS-plugin-dev.md) §4.

## Don't

- Don't link `qCC` or `ccViewer` from a plugin. The contract is `CCCoreLib + CCPluginAPI + CCPluginStub` + your deps.
- Don't `add_subdirectory()` a plugin that doesn't have all of the contract files (it will fail at `cmake` time).
- Don't use a `"type"` other than `Standard`, `GL`, `I/O` in `info.json` (case-sensitive; the loader rejects it).
- Don't reuse an IID across plugins (`ccorp.cloudcompare.plugin.<Name>` must be unique).
- Don't put user-visible strings without `tr()` in a plugin.

## See also

- Root [`../AGENTS.md`](../AGENTS.md)
- [`../AGENTS-architecture.md`](../AGENTS-architecture.md) §3 (plugin model)
- [`../AGENTS-plugin-dev.md`](../AGENTS-plugin-dev.md) (the recipe)
- [`../AGENTS-libs.md`](../AGENTS-libs.md) §6, §7 (CCPluginAPI + CCPluginStub)

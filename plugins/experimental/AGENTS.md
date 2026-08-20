# plugins/experimental — Agent Instructions

> Read this **in addition to** the root [`AGENTS.md`](../../AGENTS.md). The
> root file has the build commands, plugin-development workflow, and repo
> conventions. This file adds the **plugin-experimental**-specific rules.

## Purpose

`plugins/experimental/` is the **scratch area for new CC plugins** —
throwaway or concept-test plugins that an LLM (or you) can use to try a
new feature, algorithm, or UI flow end-to-end inside a real CloudCompare
build, **without** the friction of being a "real" plugin in
`plugins/core/`.

Use it for:

- **New measurement or analysis tools** (registration UX, spherical
  views, scan-browser, custom filters, etc.)
- **New algorithms** (Horn ICP variants, custom segmentation, ML inference
  integration, etc.)
- **New UI flows** (overlay dialogs, multi-pane layouts, custom pickers)
- **New file-format experiments** (custom importers, ReCap-style scanners)
- **Concept demos** where the goal is to see whether the UX / API feels
  right before committing to a real plugin folder

Do **not** use it for:

- Bug fixes to existing plugins — go to the plugin's own folder under
  `plugins/core/Standard/`, `plugins/core/IO/`, or `plugins/core/GL/`.
- Core library work (`libs/qCC_*`, `CCCoreLib`) — those are vendored-ish
  and have their own conventions.
- Build-system changes — `cmake/` and `tools/cc-*.cmd` are tracked
  separately.

## Structure

```
plugins/experimental/
├── AGENTS.md                     ← this file
├── README.md                     ← short human-facing overview
├── CMakeLists.txt                ← aggregator: add_subdirectory each plugin
└── q<Name>/                      ← one folder per experiment
    ├── CMakeLists.txt            ← option(PLUGIN_EXPERIMENTAL_Q<NAME> ... OFF) ...
    ├── q<Name>.qrc               ← icon + info.json resource
    ├── info.json                 ← plugin metadata
    ├── images/icon.png
    ├── include/                  ← headers
    └── src/                      ← sources
```

Copy `qHelloCloud/` as a starting template for any new experiment — it
shows the minimum-viable plugin shape and the only public headers you
should be touching.

## Workflow (mandatory)

When you (the agent) are asked to "add a new X" or "prototype Y" inside
CloudCompare's GUI:

1. **Classify the request.**
   - **plugin-feature** → new menu/toolbar action, new dialog, new
     algorithm that runs in the GUI. **Go to `plugins/experimental/`.**
   - **plugin-io** → new file format reader/writer. **Go to
     `plugins/experimental/` first, then graduate to
     `plugins/core/IO/q<Format>/`.**
   - **plugin-gl** → new OpenGL post-process shader. **Go to
     `plugins/experimental/` first, then graduate to
     `plugins/core/GL/q<Filter>/`.**
   - **core-library** → algorithm that should live in `CCCoreLib` /
     `qCC_db`. Not this folder; talk to the user.
   - **core-bugfix** → bug in an existing plugin. **NOT this folder.**
     Open the existing plugin folder.
   - **build-system** → CMake / deploy / toolchain. **NOT this folder.**

2. **Search first.** Before creating `q<Name>/`, look at the existing
   experiments in this folder. If a similar concept exists, **extend**
   it rather than duplicating.

3. **Copy the template.** `cp -r qHelloCloud q<Name>` and rename
   `QObject` class, IID (`ccorp.cloudcompare.plugin.q<Name>`), and
   `.qrc` prefix. Touch only the public headers
   (`CCPluginAPI`, `CCPluginStub`, `qCC_db`, `CCCoreLib`). **Never**
   include private `qCC/`, `ccViewer/`, or `libs/qCC_io` internals.

4. **Add the option.** In your plugin's `CMakeLists.txt`:
   ```cmake
   option( PLUGIN_EXPERIMENTAL_Q<NAME> "..." OFF )
   if ( PLUGIN_EXPERIMENTAL_Q<NAME> )
       project( q<Name> )
       AddPlugin( NAME ${PROJECT_NAME} )
       add_subdirectory( include )
       add_subdirectory( src )
   endif()
   ```
   Default **OFF**. Wire it into `tools/cc-configure.cmd` only when
   you want it built.

5. **Implement minimally.** Buildable > feature-rich. One action is
   enough for the smoke test.

6. **Document in `info.json`.** Set `description` to say **what
   concept is being tested**, not "does nothing." The
   `plugins/experimental/README.md` table also has a row per experiment.

7. **Wire it into the parent aggregator** (`plugins/experimental/CMakeLists.txt`).

8. **Build incrementally.**
   ```powershell
   tools\cc-configure.cmd
   cmake --build build --parallel 16
   tools\cc-deploy-plugins.cmd
   .\build\qCC\deployqt\CloudCompare.exe
   ```
   The first build of a new plugin takes ~30s; subsequent edits are
   <5s on the plugin target alone.

9. **Graduate when ready.** When the concept is proven, copy the
   folder to `plugins/core/Standard/q<Name>/` (or `core/IO/`, `core/GL/`),
   delete the experimental copy, and add the option to the main
   configure script.

## Hard rules

- **Public headers only.** `CCPluginAPI`, `CCPluginStub`, `qCC_db`,
  `CCCoreLib`, and the public include directories they expose. Private
  `qCC/`, `ccViewer/`, or `libs/qCC_io` internals are off-limits from
  a plugin. If you need something that isn't exposed, **stop** and
  propose the API promotion to upstream first.
- **One CMake option per plugin, default OFF.** Naming:
  `PLUGIN_EXPERIMENTAL_Q<NAME>`. The aggregator enables it explicitly
  on the developer's local `tools/cc-configure.cmd`.
- **Throwaway by default.** If a plugin has been "experimental" for
  more than a few weeks without graduating, **delete the folder** and
  keep the lesson in the commit message. The dead experiment is more
  harmful than the missing folder.
- **Never link to vendored-only libs** from a plugin. Anything that
  needs a private vcpkg dep, an SDK install, or a non-public header
  belongs in a one-off branch, not here.
- **One plugin = one concept.** Don't bundle a spherical-view demo,
  a registration tool, and a point-coloring experiment into the same
  folder. Split them.

## Conventions

- **Class name:** `q<Name>` (matches folder). The class is in the
  `q<Name>` namespace only if it doesn't collide with the folder
  name; otherwise the convention from `ExamplePlugin` → `Example` is
  to drop the `q` prefix on the namespace.
- **Plugin IID:** `ccorp.cloudcompare.plugin.q<Name>`. Must be unique.
- **IID folder prefix in `.qrc`:** `/CC/plugin/q<Name>`. Must match
  the folder name; CMake's `AddPlugin()` macro reads it.
- **Commits:** Conventional Commits with `experimental:` scope:
  - `experimental(spherical-view): add orbit-camera pivot action`
  - `experimental(registration): wire ccPickingHub into a 2-view dialog`
  - `tools: add PLUGIN_EXPERIMENTAL_Q<NAME>=ON to local configure`
- **Status:** each plugin's `info.json` `description` field is the
  single source of truth for what concept is being tested. Update it
  on every meaningful change.

## The current experiments

See `plugins/experimental/README.md` for the live table. As of the
last update:

| Folder | Concept | Status |
|---|---|---|
| `qHelloCloud/` | hello world + load-and-display a point cloud | scaffolding |

When you add a new experiment, append a row to that table.

## Cross-references

- Root [`AGENTS.md`](../../AGENTS.md) — build commands, plugin IID
  format, header conventions, all of that.
- [`plugins/core/AGENTS.md`](../core/AGENTS.md) — what lives in
  `plugins/core/`, when to graduate here vs leave in experimental.
- [`cmake/AGENTS.md`](../../cmake/AGENTS.md) — `AddPlugin()` macro,
  how plugins get linked.
- Top-level [`experimental/AGENTS.md`](../../experimental/AGENTS.md) —
  the parallel experimental workspace for Rust ports and full-build
  concept tests. Different scope: that folder builds standalone apps
  outside the main CMake; this folder builds CC plugins. Use both as
  appropriate.

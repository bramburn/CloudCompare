# AGENTS-plugin-dev.md — Adding / editing / removing features

The most common code change to CloudCompare is **adding a plugin** (a new feature), **editing a plugin** (changing behavior of an existing feature), or **removing a plugin** (deleting a feature). This file is the step-by-step recipe.

> Read [`AGENTS-architecture.md`](AGENTS-architecture.md) first if you don't yet know what a "Standard" vs "IO" vs "GL" plugin is, or what `ccMainAppInterface` is. This file assumes that context.

---

## 0. The decision tree (pick the right plugin type)

| What you want to do | Plugin type | Template to copy |
|---|---|---|
| Add a new menu/toolbar action that operates on the current selection (filter, measure, transform, classify, segment, …) | **Standard** | `plugins/example/ExamplePlugin/` |
| Add a new file format (read and/or write) | **I/O** | `plugins/example/ExampleIOPlugin/` |
| Add a new OpenGL post-process shader (eye-dome lighting, SSAO, edge detection, …) | **GL** | `plugins/example/ExampleGLPlugin/` |

If you genuinely need to change behavior shared by **all** plugins or by `qCC` itself (e.g. add a new API to `ccMainAppInterface`), you are editing a **core lib** — see §6 at the bottom. **That should be rare.**

---

## 1. ADD — create a new plugin

### 1.1 Standard plugin

```text
Source: plugins/example/ExamplePlugin/
        ├── CMakeLists.txt
        ├── ExamplePlugin.qrc
        ├── info.json
        ├── images/icon.png
        ├── include/ExamplePlugin.h
        └── src/ExamplePlugin.cpp
```

**Step-by-step (assuming you want a plugin called `qMyFeature`):**

1. **Copy the template** — `cp -r plugins/example/ExamplePlugin plugins/core/Standard/qMyFeature`.
2. **Rename the folder** to `qMyFeature` (the `AddPlugin` macro will read `<folder>.qrc` so the folder name matters).
3. **Rename the `.qrc`** — `ExamplePlugin.qrc` → `qMyFeature.qrc`. Edit it: change `<qresource prefix="/CC/plugin/ExamplePlugin">` to `<qresource prefix="/CC/plugin/qMyFeature">`.
4. **Edit `info.json`** — change `"name"`, `"icon"` path, `"description"`, `"authors"`, `"maintainers"`, `"references"`, and most importantly `"type": "Standard"`.
5. **Edit `CMakeLists.txt`** — change every occurrence of `ExamplePlugin` → `qMyFeature`. The `option()` flag must follow the convention `PLUGIN_STANDARD_QMYFEATURE` (drop the leading lowercase `q`, uppercase the rest). Default to **`OFF`** unless the plugin needs no external dep.
6. **Edit `include/ExamplePlugin.h`** — change the class name `ExamplePlugin` → `qMyFeature`, the `Q_PLUGIN_METADATA IID` (`ccorp.cloudcompare.plugin.qMyFeature`), and add your `QAction*` members. Don't forget to keep `Q_OBJECT` and the two `Q_INTERFACES` macros.
7. **Edit `src/ExamplePlugin.cpp`** — change the constructor's resource path to `":/CC/plugin/qMyFeature/info.json"`. Replace `Example::performActionA(m_app)` with your own action handler. Add new `.cpp/.h` files for each separate action (the convention is one pair per top-level menu item, like `ActionA.h/.cpp` in the template).
8. **Update `include/CMakeLists.txt` and `src/CMakeLists.txt`** if you added new files — they list the source files explicitly (don't `file(GLOB)` at the plugin level).
9. **Wire it into the build:**
   - In `plugins/core/Standard/CMakeLists.txt`, append `add_subdirectory(qMyFeature)`.
   - In your local `cc-configure.cmd` add `-DPLUGIN_STANDARD_QMYFEATURE=ON`.
10. **Re-configure and build** (with `--fresh` to bust the cmake cache):
    ```powershell
    & C:\dev\tools\cc-configure.cmd
    & C:\dev\tools\cc-build.cmd
    ```
11. **Verify** — run the bundle (`build/qCC/deployqt/CloudCompare.exe`), open Help → About → Plugins, confirm `qMyFeature` is listed. Then exercise your action(s) end-to-end.

### 1.2 I/O plugin

```text
Source: plugins/example/ExampleIOPlugin/
        ├── CMakeLists.txt          (TYPE io)
        ├── ExampleIOPlugin.qrc
        ├── info.json               ("type": "I/O")
        ├── include/
        │   ├── ExampleIOPlugin.h
        │   └── FooFilter.h         (one filter class per file format)
        └── src/
            ├── ExampleIOPlugin.cpp
            └── FooFilter.cpp
```

**Step-by-step:**

1. Copy `plugins/example/ExampleIOPlugin/` to `plugins/core/IO/qMyFormat/`.
2. Rename folder to `qMyFormat`. Rename `.qrc` to `qMyFormat.qrc` and edit its `<qresource prefix="/CC/plugin/...">` accordingly.
3. Edit `info.json` — set `"type": "I/O"` (capital `I/O`).
4. Edit `CMakeLists.txt` — change `PLUGIN_EXAMPLE_IO` → `PLUGIN_IO_QMYFORMAT`, the project name to `QMYFORMAT_PLUGIN`, keep `AddPlugin(NAME ${PROJECT_NAME} TYPE io)`. Default `OFF`.
5. Rename `ExampleIOPlugin.h/.cpp` → `qMyFormat.h/.cpp`; change the class name `ExampleIOPlugin` → `qMyFormat`. Update the IID (`ccorp.cloudcompare.plugin.qMyFormat`). Update the constructor's resource path.
6. Rename `FooFilter.h/.cpp` → `MyFormatFilter.h/.cpp` (one filter class per format). In the constructor, build a `FilterInfo` struct:
   - `id` (human-readable name shown in menus)
   - `priority` — higher number = tried later. Use `DEFAULT_PRIORITY` unless you want to **override** a built-in.
   - import `extensions` (e.g. `{ "ply" }`)
   - default extension (`"ply"`)
   - import `filterStrings` (e.g. `{ "Polygon file (*.ply)" }`)
   - export `filterStrings` (omit if `Import`-only)
   - `features` (`Import`, `Export`, or `Import | Export`)
7. Override `loadFile()` and (if exporting) `saveToFile()` and `canSave()`. Return one of the `CC_FILE_ERROR` enum values.
8. In `getFilters()` return `{ FileIOFilter::Shared(new MyFormatFilter) }` (one per format you support).
9. **CLI support** — if you want your filter to be reachable from the command line (e.g. `CloudCompare -O -FB my_format file.foo`), override `registerCommands(ccCommandLineInterface*)` and add a `ccCommandLineInterface::Command::Shared`. Look at `plugins/core/IO/qCoreIO/src/qCoreIO.cpp` for the pattern (commands registered via `cmd->registerCommand(...)`).
10. Wire into the build: add `add_subdirectory(qMyFormat)` to `plugins/core/IO/CMakeLists.txt`; add `-DPLUGIN_IO_QMYFORMAT=ON` to `cc-configure.cmd`. Re-configure with `--fresh`, rebuild, test by opening a `.foo` file.

### 1.3 GL plugin

```text
Source: plugins/example/ExampleGLPlugin/
        ├── CMakeLists.txt          (TYPE gl, SHADER_FOLDER <name>)
        ├── ExampleGLPlugin.qrc
        ├── info.json               ("type": "GL")
        ├── images/icon.png
        ├── include/
        │   ├── Bilateral.h         (the actual ccGlFilter subclass)
        │   └── ExampleGLPlugin.h
        ├── src/
        │   ├── Bilateral.cpp
        │   └── ExampleGLPlugin.cpp
        └── shaders/                (GLSL files, copied at install)
```

**Step-by-step:**

1. Copy `plugins/example/ExampleGLPlugin/` to `plugins/core/GL/qMyFilter/`.
2. Rename folder to `qMyFilter`. Rename `.qrc` → `qMyFilter.qrc`; edit its `<qresource prefix="/CC/plugin/...">`.
3. Edit `info.json` — set `"type": "GL"`.
4. Edit `CMakeLists.txt` — change `PLUGIN_EXAMPLE_GL` → `PLUGIN_GL_QMYFILTER`, project to `QMYFILTER_GL_PLUGIN`. Keep `AddPlugin(NAME ${PROJECT_NAME} TYPE gl SHADER_FOLDER <name>)`. Default `OFF`.
5. Rename `ExampleGLPlugin.h/.cpp` → `qMyFilter.h/.cpp`; change class name and IID.
6. Replace `Bilateral.h/.cpp` with your own `ccGlFilter` subclass. Override `getName()`, `getDescription()`, `getIcon()`, `getSampleShader()` (or `getUniqueID()`), `init(ccGLWindowInterface*, QString&)`, `apply(ccGLWindowInterface*, unsigned, int, int)` (or the texture-based variant for newer filters).
7. Place `.glsl` (or `.vert`/`.frag`) files in `shaders/MyFilter/` — the `SHADER_FOLDER` argument tells `AddPlugin` to install them under `<prefix>/shaders/MyFilter/`.
8. In `getFilter()` return `MyFilter::getInstance()` (or `new MyFilter`).
9. Wire into the build: add `add_subdirectory(qMyFilter)` to `plugins/core/GL/CMakeLists.txt`; add `-DPLUGIN_GL_QMYFILTER=ON` to `cc-configure.cmd`. Re-configure, build, test by enabling the filter from Display → Shader & Filters.

---

## 2. EDIT — change behavior of an existing plugin (or core file)

### 2.1 Editing a plugin's behavior

Almost always **do this in place**:

1. Open the plugin folder (`plugins/core/<TYPE>/<NAME>/...`).
2. Edit the relevant `.cpp/.h` files.
3. If you added a new source file, append it to `src/CMakeLists.txt` and `include/CMakeLists.txt`.
4. Rebuild **incrementally** — `& C:\dev\tools\cc-build.cmd` (no need to `--fresh`).
5. Re-run and test.

The plugin DLL/.so/.dylib is **replaced in place** by the next build; you don't need to restart from scratch or re-configure.

### 2.2 Editing a dialog, db-tree node, or other GUI element in `qCC/`

Sometimes the change is not in a plugin but in a dialog shipped with the GUI itself (the "built-in" tools in `qCC/`). These are NOT plugins — they're part of `qCC/CloudCompare.exe`. Edit them in place:

- `qCC/<ToolName>Dlg.{h,cpp}` — dialog classes.
- `qCC/mainwindow.h/.cpp` — the `MainWindow` class (10,000+ lines; use the alphabetical `doAction*` method list as your index).
- `qCC/db_tree/` — db-tree root, properties tree, scalar-field editor.
- `qCC/ui_templates/` — `.ui` files for the dialogs above.

After editing, rebuild; the resulting `CloudCompare.exe` is replaced in place. The `deployqt/` self-contained bundle is refreshed by the `DeployQt` cmake target on each install (see `cmake/DeployQt.cmake`).

### 2.3 Editing a core lib (CCCoreLib, qCC_db, qCC_io, qCC_glWindow)

> See §6 below. **Don't** do this for a new feature; only when the change genuinely can't be a plugin.

---

## 3. REMOVE — delete a plugin (or core file)

### 3.1 Remove a plugin

**Step-by-step (using `qMyFeature` as example):**

1. **Delete the plugin folder** — `rm -rf plugins/core/Standard/qMyFeature`.
2. **Remove the `add_subdirectory(qMyFeature)` line** from `plugins/core/Standard/CMakeLists.txt`.
3. **Remove the `-DPLUGIN_STANDARD_QMYFEATURE=...` flag** from `cc-configure.cmd` (or comment it out).
4. **Search the codebase** for any reference to the plugin's class name, IID, or unique action ID:
   ```powershell
   rg -i "qMyFeature|QMYFEATURE_PLUGIN|ccorp.cloudcompare.plugin.qMyFeature" plugins qCC ccViewer libs
   ```
   If there are any references in `qCC/` or `ccViewer/`, decide case-by-case: usually a plugin is self-contained so nothing else needs to change.
5. **Re-configure with `--fresh`** — `& C:\dev\tools\cc-configure.cmd`. The `--fresh` is **mandatory** because the previous configure's `PLUGIN_STANDARD_QMYFEATURE=ON` will linger in `CMakeCache.txt` otherwise (see `BUILD-LOCAL.md` gotcha #4).
6. **Rebuild and run** — confirm the About → Plugins dialog no longer lists it.

### 3.2 Remove a dialog from `qCC/`

1. Find its class (`grep -rn "ccMyToolDlg" qCC/`) and trace:
   - The `.h/.cpp` files
   - The `.ui` template in `qCC/ui_templates/`
   - The `QAction` in `qCC/mainwindow.h` (member variable)
   - The action's `connect(...)` call in `qCC/mainwindow.cpp` constructor
   - The `doAction*` slot implementation
2. Delete the files, remove the connect / slot / member, remove the `.qrc` entry if present.
3. Rebuild.

### 3.3 Remove a core lib (rare)

If a whole library has become dead code (rare):

1. Remove `add_subdirectory(...)` for that lib from the parent `CMakeLists.txt`.
2. Remove `target_link_libraries(... <lib>)` lines from every downstream target.
3. Delete the folder.
4. Rebuild — expect a wall of "undefined reference" errors that will guide you to any missed wiring.

**Stop and reconsider** if you get non-linker errors. Removing a lib usually means breaking an API contract that plugins depend on.

---

## 4. The verification checklist (every change)

Before you mark a plugin edit/add/remove as done, all of these must be true:

- [ ] **Configured fresh** if you toggled plugin flags: `cmake -S . -B build --fresh ...` (or `cc-configure.cmd` which does this for you).
- [ ] **Built clean** — `cmake --build build --parallel` finished without warnings about your plugin. Warnings on other plugins (third-party deps) are OK.
- [ ] **clang-format clean** — `cmake --build build --target check-format` returned `0` (CI runs this on Windows). If it complains about your file, run `cmake --build build --target format` and re-commit.
- [ ] **Loaded** — `CloudCompare.exe` starts, your plugin appears in **Help → About → Plugins**.
- [ ] **No console errors at startup** — check the bottom console pane (or `ccLog::Print` output) for `[Plugin]` messages naming your plugin. A `[Plugin] Found: qMyFeature` line means it loaded.
- [ ] **Action / file format / GL filter works end-to-end** — click your action / open your file / toggle your filter with a representative test case. If the plugin is data-dependent, use a small synthetic input.
- [ ] **CLI works** (if you registered a command) — `CloudCompare -h` lists your command; `CloudCompare -COMMAND_NAME ...` runs without crashing.
- [ ] **Disable / re-enable cycle** — toggle the plugin off (`PLUGIN_*=OFF`), `--fresh`, rebuild; toggle on, `--fresh`, rebuild. Both states compile.
- [ ] **Submodule SHAs untouched** — `git submodule status` shows the same hashes as before your change.
- [ ] **Header** — every new `.cpp/.h` has the LGPL or GPL header (template in `CONTRIBUTING.md`).
- [ ] **`info.json` valid** — `"type"` is exactly `Standard`, `GL`, or `I/O` (case-sensitive; the loader's `IsMetaDataValid` rejects anything else).

---

## 5. CI parity

`.github/workflows/build.yml` builds this matrix:
- **Windows MSVC only** (via Conda, full plugin set).

> ⚠️ **macOS and Linux are not in the matrix.** The fork dropped both on 2026-08-24. The `qCC/Mac/` and `ccViewer/Mac/` bundle sources are kept for local macOS builds but are not exercised in CI. See [`AGENTS.md` §CI](AGENTS.md#ci) for the rationale.

For each platform, the CI toggles every plugin explicitly with `-DPLUGIN_*=ON/OFF` — that's the canonical list of plugin flags. When you add a plugin, **also add it to the CI matrix** with the right default (`ON` if no external dep, `OFF` otherwise). Mirror the changes for the `Windows MSVC` job.

CI also runs `cmake --build build --target check-format` on Windows — your code must pass.

---

## 6. When you genuinely need to change a core lib

If you find yourself needing to:

- Add a method to `ccMainAppInterface` (because plugins need a new capability)
- Add a field to `ccHObject` (because the data model is missing something)
- Add a new `FileIOFilter` to `InitInternalFilters` (because you want it always-on, not a plugin)
- Add a new enum to `CC_TYPES` (because nothing fits)

Then:

1. Open `AGENTS-architecture.md` to confirm the layer you're crossing.
2. Bump the `Q_DECLARE_INTERFACE` version string if you changed an interface header (e.g. `ccStdPluginInterface` v1.5 → v1.6). Update every shipped plugin's macro and the host side's loader.
3. Run **all three plugin types**' example templates against the change — they are the smallest possible smoke tests.
4. Document the API addition in `CONTRIBUTING.md` or in the relevant header's top-of-file comment.

This work is qualitatively different from "add a plugin" and should be reviewed by someone familiar with the plugin contract before merge.

---

## 7. Pointers / where to look next

- `plugins/example/ExamplePlugin/` — copy-paste skeleton for Standard plugins (read `ExamplePlugin.cpp` end-to-end; comments explain each step).
- `plugins/example/ExampleIOPlugin/` — I/O plugin skeleton (read `FooFilter.h/.cpp`).
- `plugins/example/ExampleGLPlugin/` — GL plugin skeleton (read `Bilateral.h/.cpp`).
- `plugins/cmake/Plugins.cmake` — the `AddPlugin` helper (what it does for you; read before duplicating any of it).
- `libs/CCPluginAPI/include/ccMainAppInterface.h` — the surface plugins call back into.
- `libs/CCPluginStub/include/cc{Std,IO,GL,Default}PluginInterface.h` — the base classes plugins subclass.
- `CONTRIBUTING.md` § "Designing a new qCC plugin" — the upstream walk-through.
- [`AGENTS-architecture.md`](AGENTS-architecture.md) — layer diagram, plugin discovery rules.
- [`AGENTS-coding-standards.md`](AGENTS-coding-standards.md) — naming, headers, formatting.
- [`AGENTS-libs.md`](AGENTS-libs.md) — what each core lib owns (so you know what's already available to import).

## 8. Worked example: `qManualRegistration` (in-flight)

There's one in-flight goal-level project that uses **all** the recipes above plus a few extras (creating plugin-owned GL windows, hooking two `ccPickingListener`s, computing a rigid transform from point pairs). If you're working on it, read the goal doc + layered context first:

- Goal doc: [`AGENTS_REGISTRATION.md`](AGENTS_REGISTRATION.md) — why, scope, constraints, testable milestones, decision (plugin vs core).
- Layered context: [`docs/context/registration/`](docs/context/registration/) — file map, data flow, picking, dual viewport, transform math.
- Single-window reference: `qCC/ccPointPairRegistrationDlg.{h,cpp}` + `qCC/ui_templates/pointPairRegistrationDlg.ui` — the existing manual registration, on one window. The dual-viewport version is a *supersetting* of this.

> **Rule of thumb:** any time a goal doc + a `docs/context/<goal>/` folder exists, the project is significant enough that you should read the goal doc *before* §1 of this file. It usually contains domain-specific constraints (e.g. "use `setGLTransformation` for preview, `applyRigidTransformation` for commit — never the reverse") that the standard plugin recipe doesn't know about.

# plugins/experimental — throwaway / concept-test plugins

This folder is the home for plugin experiments: things you want to try out
before deciding whether to graduate them into `plugins/core/...` or throw them
away. The goal is fast iteration, not production polish.

## What lives here

- One folder per concept, named `q<ConceptName>/` (e.g. `qHelloCloud/`,
  `qSphereView/`, `qRegistration/`).
- Each folder is a real CloudCompare plugin that compiles, loads, and runs
  inside `CloudCompare.exe` and `ccViewer.exe` like any other.
- No shared code between experiments — copy what you need; don't refactor
  across folders. If two experiments need the same helper, the helper has
  proven itself worth promoting somewhere else.

## Rules (for humans and LLMs)

1. **Public headers only.** Link against `CCPluginStub`, `CCPluginAPI`,
   `CCCoreLib`, and `qCC_db`. Never reach into `qCC/`, `ccViewer/`, or
   `libs/qCC_io` internals from an experimental plugin. If you think you
   need to, the answer is: no, promote the API into `CCPluginAPI` upstream
   first.
2. **One CMake option per plugin, default OFF.** Naming:
   `PLUGIN_EXPERIMENTAL_QXXX` (mirrors `PLUGIN_STANDARD_QXXX` /
   `PLUGIN_IO_QXXX`). Enable explicitly in your local
   `C:\dev\tools\cc-configure.cmd`.
3. **Throwaway by default.** Don't carry an experimental plugin forward
   unless it's clearly earning its keep. When in doubt, delete the folder
   and the option line.
4. **Document the intent.** Each plugin's `info.json` description should
   say what concept it's testing, not just "does nothing."

## Build

The experimental plugins are NOT enabled by default. To build one, add the
relevant `-DPLUGIN_EXPERIMENTAL_QXXX=ON` flag to your local configure
script and re-run `cc-configure.cmd` + `cc-build.cmd`.

```powershell
# Example: enable just the hello-world experiment
& C:\dev\tools\cc-configure.cmd   # with -DPLUGIN_EXPERIMENTAL_QHELLOCLOUD=ON
& C:\dev\tools\cc-build.cmd
& 'C:\dev\CloudCompare\build\qCC\deployqt\CloudCompare.exe'
```

The plugin's icon will appear in the plugin toolbar (or in the Plugins menu
if the plugin has multiple actions). Help → About → Plugins will list it
by name.

## Current experiments

| Folder | Concept | Status |
|---|---|---|
| `qHelloCloud/` | Smoke test: hello-world action + load-and-display a point cloud file | scaffolding |

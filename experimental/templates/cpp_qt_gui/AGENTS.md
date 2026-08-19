# Template: cpp_qt_gui

## Purpose

A **Qt 6 desktop app with a 3D OpenGL viewport** for visualising point clouds.

This is the "see what your code does" template. Replace the synthetic data
with a real point source (file loader, QProcess call to a Rust binary, or
CXX-linked Rust staticlib) and you have a working visual debug tool.

Use this when you want to:

- Visualise scalar field colour maps on a 3D point cloud
- Watch ICP iterations update in real time
- See octree subdivision
- Debug distance computations
- Show a colleague "what does the algorithm do"

## When NOT to use

- You don't need a GUI — use `cpp_qt_console/`
- You don't need OpenGL — just C++/Qt is enough for that
- You only need a headless unit test — use `rust_lib/`

## Structure

```
cpp_qt_gui/
├── CMakeLists.txt                ← find Qt6 Core/Gui/Widgets/OpenGLWidgets, link
├── src/
│   ├── main.cpp                  ← QApplication, QMainWindow, QComboBox for dataset
│   ├── pointcloudview.h/.cpp     ← QOpenGLWidget subclass with custom GLSL shaders
│   └── synthetic.h/.cpp          ← spiral / gaussian / helix point generators
├── AGENTS.md
└── README.md
```

## Build & run

```powershell
# Configure (assumes Qt 6.8.3 at C:/dev/tools/Qt/6.8.3/msvc2022_64)
cmake -S . -B build -G Ninja `
  -DCMAKE_PREFIX_PATH=C:/dev/tools/Qt/6.8.3/msvc2022_64

# Build
cmake --build build

# Run (Qt DLLs must be on PATH; see shared/scripts/get-qt-env.ps1)
$env:Path = 'C:\dev\tools\Qt\6.8.3\msvc2022_64\bin;' + $env:Path
.\build\demo_gui.exe
```

If the build fails with `LNK1181: cannot open d3d11.lib`, the Windows SDK
is not on the link path. The `CMakeLists.txt` in this template adds the
SDK lib path automatically when MSVC is the compiler; check that
`$env:WindowsSdkDir` is set (`. ../shared/scripts/get-vcvars.ps1`).

## Controls

| Input | Action |
|---|---|
| Left-mouse drag | Rotate |
| Right-mouse drag | Pan |
| Mouse wheel | Zoom |
| Reset View button | Return to default camera |
| Dataset combo | Switch between Spiral / Gaussian / Helix |

## Extension patterns

1. **Load a real file.** Add a QFileDialog and a `FileIOFilter`-style class
   that reads `.las` / `.laz` / `.obj` into `std::vector<float>` interleaved xyz.

2. **Call a Rust subprocess.** Use `QProcess` to launch a Rust binary that
   prints JSON to stdout; parse with `QJsonDocument`. For example, a Rust
   binary that loads a `.las` file and outputs ICP results.

3. **Call a Rust staticlib (advanced).** Add `cxx-ffi` to the linked
   `rust_cxx_app` crate. See `rust_cxx_app/AGENTS.md`. Requires MSVC.

4. **Add scalar-field colour map.** Pass a second vector (one float per
   point) to `setPoints` and use it as the colour input instead of z.

5. **Show ICP iteration in real time.** Animate the cloud update on a
   `QTimer`; re-upload points each frame.

6. **Add axes / grid.** Already in the template (RGB axes). For a grid,
   add another set of GL_LINES in `PointCloudView::drawAxes`.

## Common pitfalls

1. **Qt DLLs not found at runtime** — add `C:\dev\tools\Qt\6.8.3\msvc2022_64\bin` to PATH.
   Or use `windeployqt build\demo_gui.exe` to copy DLLs alongside the exe.
2. **`LNK1181: cannot open d3d11.lib`** — the CMakeLists in this template
   already handles this. If you copy it elsewhere, copy the `add_link_options`
   line.
3. **Black viewport on first run** — likely a shader compile error. Add
   `glGetShaderInfoLog` checks in `initializeGL`.
4. **Mouse controls feel inverted** — adjust the sign in `mouseMoveEvent`.

## Related

- Sibling: `../cpp_qt_console/` (Qt CLI, no GUI)
- Sibling: `../rust_lib/` (pure-Rust library)
- Sibling: `../rust_cxx_app/` (Rust ↔ C++ via CXX — link this to make a hybrid)
- Workflow: `../../AGENTS.md`

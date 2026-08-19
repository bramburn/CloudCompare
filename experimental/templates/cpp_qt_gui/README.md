# cpp_qt_gui template

Qt 6 desktop app with a 3D OpenGL viewport for point cloud visualisation.

## Quick start

```powershell
# Configure (assumes Qt 6.8.3 at C:/dev/tools/Qt/6.8.3/msvc2022_64)
cmake -S . -B build -G Ninja `
  -DCMAKE_PREFIX_PATH=C:/dev/tools/Qt/6.8.3/msvc2022_64

# Build
cmake --build build

# Run (Qt DLLs must be on PATH)
$env:Path = 'C:\dev\tools\Qt\6.8.3\msvc2022_64\bin;' + $env:Path
.\build\demo_gui.exe
```

You should see a window titled "Point Cloud Viewer — cpp_qt_gui template"
with a coloured spiral point cloud. Drag to rotate, wheel to zoom, right-drag
to pan. The Dataset combo switches between Spiral / Gaussian / Helix.

## What you get

| File | Purpose |
|---|---|
| `src/main.cpp` | `QApplication`, `QMainWindow`, `QComboBox` for dataset selection |
| `src/pointcloudview.{h,cpp}` | `QOpenGLWidget` subclass with custom GLSL shaders, mouse + wheel handlers |
| `src/synthetic.{h,cpp}` | `spiral(n)`, `gaussian_cloud(n)`, `helix(n)` — test data |
| `CMakeLists.txt` | Qt6 (Core, Gui, Widgets, OpenGLWidgets) + Windows SDK lib path fix for d3d11 |

## Controls

| Input | Action |
|---|---|
| Left-mouse drag | Rotate |
| Right-mouse drag | Pan |
| Mouse wheel | Zoom |
| Reset View | Return to default camera |
| Dataset combo | Switch between Spiral / Gaussian / Helix |

## Environment

| Var | Default | Used for |
|---|---|---|
| `CMAKE_PREFIX_PATH` | (none) | Where CMake looks for Qt 6 |
| `WindowsSdkDir` | (none) | Where CMake looks for d3d11.lib etc. |
| `Path` | system | Must include `C:\dev\tools\Qt\6.8.3\msvc2022_64\bin` for runtime DLLs |

## What to edit

1. `src/synthetic.cpp` — replace with a real file loader
2. `src/pointcloudview.cpp` — change colour mapping, add grid, add axes labels
3. `src/main.cpp` — add menus, file dialogs, status bar updates

## How to link Rust

See `AGENTS.md` "Extension patterns". Quick recipes:

- **Subprocess (easiest):** `QProcess` launches a Rust binary, parses JSON.
- **Staticlib (advanced):** Link the `rust_cxx_app` staticlib with `cxx-ffi`
  feature. Requires MSVC toolchain. See `rust_cxx_app/AGENTS.md`.

## See also

- `AGENTS.md` — when to use, structure, extension patterns, pitfalls
- `../../AGENTS.md` — workflow contract
- Sibling: `../cpp_qt_console/` (Qt CLI, no GUI)
- Sibling: `../rust_lib/` (pure-Rust library)
- Sibling: `../rust_cxx_app/` (Rust ↔ C++ via CXX)

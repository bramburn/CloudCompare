# Template: cpp_qt_console

## Purpose

A **C++ + Qt 6 console app** template. No GUI. No FFI. Just `QCoreApplication`,
`QCommandLineParser`, and Qt's logging.

Use this when you want to:

- Test file I/O using Qt's `QFile` / `QDir` / `QString`
- Parse command-line arguments with `QCommandLineParser`
- Use Qt's logging categories (`Q_LOGGING_CATEGORY`)
- Quickly try a Qt API before committing to a full GUI
- Run a CLI that uses Qt types but doesn't open any windows

## When NOT to use

- You need a window or viewport — use `cpp_qt_gui/`
- You need to call Rust — use `rust_cxx_app/` (or the `cxx-ffi` feature in this
  template's sibling; see "Hybrid" below)
- You don't need Qt at all — use plain C++ or Rust

## Structure

```
cpp_qt_console/
├── CMakeLists.txt          ← find Qt6 Core, add_executable, link Qt6::Core
├── src/
│   └── main.cpp            ← QCoreApplication + QCommandLineParser + Q_LOGGING_CATEGORY
├── AGENTS.md
└── README.md
```

## Build & run

```powershell
# Configure
cmake -S . -B build -G Ninja `
  -DCMAKE_PREFIX_PATH=C:/dev/tools/Qt/6.8.3/msvc2022_64

# Build
cmake --build build

# Run
.\build\demo_cli.exe
.\build\demo_cli.exe C:\path\to\file.txt
.\build\demo_cli.exe -v
```

If you have Qt in a different location, replace `C:/dev/tools/Qt/6.8.3/msvc2022_64`
with your Qt 6 install root.

If you don't have Ninja, drop `-G Ninja` and CMake will pick the default generator
(Visual Studio on Windows).

## Extension patterns

1. **Add Qt modules.** `find_package(Qt6 COMPONENTS Network Sql)`, then
   `target_link_libraries(demo_cli PRIVATE Qt6::Network Qt6::Sql)`.
2. **Add a sub-command.** Use `QCommandLineParser::addSubcommand` for git-style
   multi-command CLIs.
3. **Add tests.** `find_package(Qt6 COMPONENTS Test)`, create a test exe with
   `qt_add_executable`, link `Qt6::Test`, write `QTEST_MAIN` / `QTEST_GUILESS_MAIN`.

## Hybrid: call Rust from this template

If you also need Rust code (for fast point-cloud operations, say), this template
can launch a Rust subprocess and parse its output:

```cpp
QProcess rust;
rust.start("./rust_binary.exe", QStringList() << "--json");
rust.waitForFinished();
const QByteArray json = rust.readAllStandardOutput();
// parse with QJsonDocument
```

For tighter integration, use `rust_cxx_app/` to compile Rust as a staticlib and
link it here. The staticlib build needs the MSVC toolchain — see
`rust_cxx_app/AGENTS.md`.

## Common pitfalls

1. **Qt not found.** Set `CMAKE_PREFIX_PATH` to your Qt 6 install root.
2. **MSVC d3d11.lib not found.** Only matters if you link `Qt6::OpenGLWidgets`
   (i.e. in `cpp_qt_gui/`). The Qt-only Core module links nothing extra.
3. **Console window appears on Windows GUI apps.** If you want a GUI but no
   console, switch to `cpp_qt_gui/` and set `WIN32_EXECUTABLE` in CMake.

## Related

- Sibling: `../cpp_qt_gui/` (Qt with OpenGL viewport)
- Sibling: `../rust_cxx_app/` (Rust ↔ C++ via CXX)
- Workflow: `../../AGENTS.md`

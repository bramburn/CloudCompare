# cpp_qt_console template

Minimal Qt 6 console app. No GUI. No FFI.

## Quick start

```powershell
# Configure (assumes Qt 6.8.3 at C:/dev/tools/Qt/6.8.3/msvc2022_64)
cmake -S . -B build -G Ninja `
  -DCMAKE_PREFIX_PATH=C:/dev/tools/Qt/6.8.3/msvc2022_64

# Build
cmake --build build

# Run
.\build\demo_cli.exe            # prints hello world
.\build\demo_cli.exe C:\some\file.txt
```

## What you get

- `src/main.cpp` — `QCoreApplication` + `QCommandLineParser` + `Q_LOGGING_CATEGORY`
  + a tiny `countLines(file)` example
- `CMakeLists.txt` — `find_package(Qt6 Core)`, `add_executable`, `target_link_libraries`
- Logging via Qt's `QLoggingCategory` (filter with `QT_LOGGING_RULES`)

## Environment

| Var | Default | Used for |
|---|---|---|
| `QT_LOGGING_RULES` | (none) | Enable debug logging: `template.qt.console.debug=true` |
| `CMAKE_PREFIX_PATH` | (none) | Where CMake looks for Qt6 |

## What to edit

1. `CMakeLists.txt` — add more Qt components if needed (Network, Sql, etc.)
2. `src/main.cpp` — replace `countLines()` with your experiment
3. Add new subcommands with `QCommandLineParser::addSubcommand`

## See also

- `AGENTS.md` — when to use, structure, extension patterns
- `../../AGENTS.md` — workflow contract
- Sibling: `../cpp_qt_gui/` (Qt with OpenGL viewport)
- Sibling: `../rust_cxx_app/` (Rust ↔ C++ via CXX)

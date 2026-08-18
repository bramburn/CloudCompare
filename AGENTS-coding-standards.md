# AGENTS-coding-standards.md — Coding standards

The full style rules are in [`CONTRIBUTING.md`](CONTRIBUTING.md). This file restates them and adds the **unwritten conventions** that you'll trip over if no one tells you.

---

## 1. File headers (mandatory)

Every new `.h` and `.cpp` file **must** start with one of these headers. Use the **LGPL** version for `libs/*` and the **GPL** version for `qCC/`, `ccViewer/`, `plugins/`.

### LGPL (for libraries)

```cpp
//##########################################################################
//#                                                                        #
//#                              MODULE NAME                               #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU Library General Public License as       #
//#  published by the Free Software Foundation; version 2 of the License.  #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
//#  GNU General Public License for more details.                          #
//#                                                                        #
//#                             COPYRIGHT: XXX                             #
//#                                                                        #
//##########################################################################
```

### GPL (for `qCC/`, `ccViewer/`, plugins)

```cpp
//##########################################################################
//#                                                                        #
//#                            MODULE NAME                                 #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU General Public License as published by  #
//#  the Free Software Foundation; version 2 of the License.               #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
//#  GNU General Public License for more details.                          #
//#                                                                        #
//#                             COPYRIGHT: XXX                             #
//#                                                                        #
//##########################################################################
```

> `CONTRIBUTING.md` calls these "LGPL modules" (`CCLib`, etc.) and "GPL modules" (`qCC`, etc.). The same rule applies to plugins — they are GPL because they link into `qCC`. The `CCPluginAPI` lib headers use the GPL header for the same reason.

The CI does **not** enforce this header — be disciplined about it yourself.

---

## 2. Naming conventions (mandatory)

| Element | Convention | Example |
|---|---|---|
| Class | `cc` prefix + PascalCase | `ccConsole`, `ccPointCloud`, `ccMainAppInterface` |
| File | same as class (case-sensitive!) | `ccConsole.h`, `ccConsole.cpp` |
| Attribute / variable | `lowerCamelCase` | `numberOfPoints`, `ptsCount` |
| Static attribute / variable | `s_` prefix + lowerCamelCase | `s_defaultFilename` |
| Member attribute | `m_` prefix + lowerCamelCase | `m_selectedEntities`, `m_action` |
| Method | `lowerCamelCase()` | `getActions()`, `loadFile()` |
| Static method | PascalCase (yes, capital first letter) | `InitGLEW()`, `GetUniqueInstance()` |
| Structure | `lowerCamelCase` | `LoadParameters` (yes — no prefix) |
| Enumerator | `CC_` prefix + UPPER_SNAKE_CASE | `CC_OBJECT_FLAG`, `CC_STD_PLUGIN` |
| Macro (function-like) | `MACRO_` prefix + lowerCamelCase | `MACRO_SkipUnselected` |
| Const variable | `UPPER_SNAKE_CASE` | `NORMALS_QUANTIZE_LEVEL` |
| Const `#define` | same as const variable, prefer `const`/`constexpr` instead | |
| Qt slot | `doAction...()` for menu actions, otherwise `lowerCamelCase()` | `doActionSaveFile()` |
| Plugin IID | `ccorp.cloudcompare.plugin.<LowerCamelName>` | `ccorp.cloudcompare.plugin.qHPR` |
| `info.json` `type` | exactly one of `Standard`, `GL`, `I/O` (case-sensitive) | |
| Plugin folder / target name | PascalCase `_PLUGIN` suffix | `QHPR_PLUGIN`, `QCORE_IO_PLUGIN`, `QEDL_GL_PLUGIN` |
| Plugin CMake option | `PLUGIN_<TYPE>_<NAME>` | `PLUGIN_STANDARD_QHPR` |

Filenames:
- **No spaces.** Use underscores.
- **Case-sensitive** — on Linux/macOS the build will fail if `ccConsole.h` ≠ the include `"ccConsole.h"` in your `.cpp`.

---

## 3. Indentation & formatting

### 3.1 The hard rules

- **Tabs, not spaces.** Each tab = 4 columns. `.editorconfig` enforces this.
- **Blank line at end of every file.** `.editorconfig`'s `insert_final_newline = true`.
- **No trailing whitespace.** `.editorconfig`'s `trim_trailing_whitespace = true`.
- **UTF-8** charset (`.editorconfig`).
- All files matching `*.{hpp,cpp,h}` in `.editorconfig`.

### 3.2 clang-format (CI enforced)

`.clang-format` defines the full set. CI runs `cmake --build build --target check-format` on every PR. Run it before you commit:

```powershell
cmake --build C:\dev\CloudCompare\build --target check-format
# To auto-fix:
cmake --build C:\dev\CloudCompare\build --target format
```

Key style decisions encoded in `.clang-format`:
- `UseTab: ForIndentation` — tabs for indent, spaces for alignment.
- `BraceBreakStyle: Allman` — opening brace on its own line.
- `IndentWidth: 4`, `TabWidth: 4`.
- `PointerAlignment: Left` — `int* p`, not `int *p`.
- `SortIncludes: true` — `#include` blocks sorted, grouped (matching `IncludeBlocks: Regroup`).
- `MaxEmptyLinesToKeep: 1`.
- `NamespaceIndentation: All` — content inside namespaces is indented.
- `BreakBeforeBinaryOperators: NonAssignment` — operators break before.
- `AllowShortFunctionsOnASingleLine: false`, `AllowShortIfStatementsOnASingleLine: false` — keep code readable.
- `AlignConsecutiveAssignments: true`, `AlignConsecutiveDeclarations: true`, `AlignOperands: true`.
- `StatementMacros: ['Q_OBJECT']` — keep `Q_OBJECT` on its own line.

If you disagree with one of these, **don't fight it locally** — open an issue against `CONTRIBUTING.md`.

---

## 4. Includes

### 4.1 Order

Use the project's standard block order (clang-format will sort for you, but write them in order to start with):

1. **The matching header** (`#include "ccMyClass.h"` first thing in `ccMyClass.cpp`).
2. **CC-local headers** (`#include <ccHObject.h>`, `#include <FileIOFilter.h>`).
3. **Third-party headers** (Eigen, OpenGL, Boost, …).
4. **Qt headers**, grouped by module (`<QtCore>`, `<QtGui>`, `<QtWidgets>`).
5. **Standard library** (`<vector>`, `<string>`, `<memory>`).

Forward declarations instead of `#include` whenever you can in headers — see `qCC/mainwindow.h` for an extreme example.

### 4.2 The `#include "..."` vs `#include <...>` rule

- `#include "..."` for your own headers (CC, plugin, project).
- `#include <...>` for everything else (Qt, std, third-party).

### 4.3 Path style — **always forward slashes**, even on Windows

The codebase requires Unix-style paths in `#include` directives because the Linux/macOS builds need them. Never:

```cpp
#include "..\db\ccPointCloud.h"  // BAD
```

Always:

```cpp
#include "../db/ccPointCloud.h"  // GOOD
```

(This is from `CONTRIBUTING.md` § "Unix compliance". The CI doesn't enforce it but a Linux reviewer will catch it.)

---

## 5. C++ version & language rules

- **C++17.** Use `std::optional`, `std::variant`, structured bindings, `if constexpr`, etc. freely. Don't use C++20 features.
- **`auto` is allowed** but **don't** chain it through layers — readers should be able to see the type at a glance. Convention: `auto* ptr = ...` is OK; `auto x = someFunction();` is OK when the return type is documented in the function name.
- **`nullptr`, not `NULL`, not `0`.**
- **`override`** on every virtual override. **`final`** when you mean it.
- **`explicit`** on every single-argument constructor.
- **No exceptions** (`noexcept`) on small destructors and trivial getters.
- **No `using namespace std;`** or `using namespace cc;` in headers. Use namespace aliases in `.cpp` files if it helps.
- **`Q_OBJECT`** on every `QObject` subclass — required for signals/slots/meta-object.
- **No `slots:` in the old-style public/private/protected slots section** — use the new `Q_SIGNALS` / `Q_SLOTS` macros if you must, but CC mostly uses the old `public slots:` style. Match what you see in neighboring files.
- **`Q_DECLARE_METATYPE`** for any type you pass through Qt's queued signals or `QVariant`.

---

## 6. Qt conventions

- **`tr()`** for every user-visible string literal. Even single-word labels.
- **`QString::fromStdString(...)`** for std-string → QString; **`QStringLiteral("...")`** for constants (more efficient than `QString("...")`).
- **`Q_OBJECT`** + **`Q_DISABLE_COPY`** if the class isn't meant to be copied.
- **`QPointer<T>`** for non-owning pointers to `QObject`s that might be deleted (the host deletes overlay dialogs via `deleteLater`).
- **Forward-declare Qt classes** in headers (`class QWidget;`); include the full header in `.cpp`.
- **`#include <QtGui>`** at the top of any `.cpp` that uses `Qt::` enums and you can't be bothered to include the specific header.
- **`tr()` in constructors** is OK; just remember it's the class context, not the file context.
- **Avoid `connect(... SIGNAL(...) SLOT(...))`** — use the new pointer-to-member syntax:
  ```cpp
  connect(m_action, &QAction::triggered, this, &MyClass::onTriggered);
  ```
- **`qApp`** is Qt's `QCoreApplication::instance()`. CC's equivalent is **`ccApp`** (defined in `libs/CCAppCommon/include/ccApplicationBase.h`).

---

## 7. Memory & ownership

- **`ccHObject` and its subclasses** are managed by Qt parent-child ownership in the db-tree. Don't `delete` an `ccHObject` you didn't `new` — call `removeFromChildren()` or use `ccHObjectContext` and let the tree clean it up.
- **Plugins never own `ccHObject`s they add to the tree.** Hand them to `m_app->addToDB(obj, ...)` and the app owns them.
- **Dialogs (`QDialog` / `QWidget`) you create in a plugin** should have the app main window as parent (`m_app->getMainWindow()`), or be `deleteLater`-managed.
- **`QSharedPointer`** is used heavily in `qCC_io` (`FileIOFilter::Shared`). Match the pattern.
- **No `std::auto_ptr`** — C++17. Use `std::unique_ptr`.
- **No raw owning pointers in plugin interfaces.** If you return a new object, the caller owns it; if the caller passes you a pointer, you don't own it. State this explicitly in the doc-comment.

---

## 8. Strings

- Use `QString` everywhere in GUI code, `std::string` in `CCCoreLib`.
- Use `QStringLiteral("...")` for compile-time-known constants.
- Use `QObject::tr()` (or `tr()` from a `Q_OBJECT` member function) for translatable strings.
- Use `tr("Context|String", "context hint")` to disambiguate identical strings used in different places (the `Context|` syntax is Qt-isms; CC uses it in a few places).
- Avoid `QTextCodec`; UTF-8 is the project default.

---

## 9. Error reporting

- **GUI code** → `ccConsole::Error(...)`, `ccConsole::Warning(...)`, `ccConsole::Print(...)`.
- **Plugins** → `m_app->dispToConsole(message, level)` (or `dispToConsole(message)` on `ccStdPluginInterface`).
- **Algorithms (`CCCoreLib`)** → `ccLog::Error(...)`, etc. (the low-level logging used inside the libs).
- **`FileIOFilter::loadFile()`** → return one of the `CC_FILE_ERROR` values (in `libs/qCC_io/include/FileIOFilter.h`); do not throw.
- **Throw sparingly, only in `CCCoreLib` internals where the only caller can `try/catch` it.** Don't throw across library boundaries; convert to a return code at the boundary.

---

## 10. Testing

- The unit test target is `cc-test-lib` (built only when `BUILD_TESTING=ON`).
- Tests live in `libs/qCC_io/test/` (currently the only tests we have).
- Use any framework you like but keep new tests in `libs/qCC_io/test/`, not scattered.
- Run with `xvfb-run ctest --test-dir build --output-on-failure` on Linux. On Windows, just `ctest --test-dir build`.

---

## 11. PR / commit conventions

- **One PR per logical change.** Plugin additions are usually one PR (including CMakeLists + CI flag).
- **Commit message format** (upstream uses squash-merge):
  ```
  Short summary (max ~70 chars)
  
  Longer description if needed, wrapped to ~72 columns.
  
  Fixes #1234
  ```
- **No merge commits.** Rebase before push if needed.
- **Submodule SHAs** should stay at upstream's pinned values unless the change depends on a newer dep — coordinate that with the maintainers.
- **Don't push to `master` locally.** Branch from `master`, PR upstream, merge after review.
- **Don't commit `build/`, `.pi/`, `dist/`, or `BUILD-LOCAL.md`.** The `.gitignore` covers `build/` and `dist/`; add the others to your local excludes.

---

## 12. The "don't do this" list (consolidated)

- Don't include `qCC_db` headers into a plugin — only `CCPluginAPI` + `CCPluginStub` + `CCCoreLib` + your own deps.
- Don't `delete` an `ccHObject` you didn't `new`.
- Don't `throw` across library boundaries.
- Don't add a new dialog class to `qCC/` for a feature that should be a plugin.
- Don't add a new file format by editing `qCC_io/src/FileIOFilter.cpp` — make an I/O plugin.
- Don't use `dynamic_cast` on `ccHObject` — use `ccHObjectCaster::To*` helpers.
- Don't use `qDebug()` for user-visible messages — use `ccConsole`.
- Don't `std::move()` an `ccHObject` into the db — the db takes a raw pointer.
- Don't change a `Q_DECLARE_INTERFACE` version string without bumping it everywhere.
- Don't bypass `.clang-format` by hand-formatting — run `cmake --build build --target format` and trust it.
- Don't put `using namespace` in a header file.
- Don't put `Q_OBJECT` classes in `.cpp` files (must be in headers for `moc`).
- Don't put `m_action->setEnabled(false)` in `getActions()` — that's what `onNewSelection` is for.
- Don't forget the license header on new files.
- Don't forget `Q_PLUGIN_METADATA` in every plugin class.

---

## 13. Cross-refs

- [`CONTRIBUTING.md`](CONTRIBUTING.md) — upstream style guide (the source of truth for the rules).
- [`.clang-format`](.clang-format) — formatter config.
- [`.editorconfig`](.editorconfig) — editor config.
- [`AGENTS-architecture.md`](AGENTS-architecture.md) — where things belong.
- [`AGENTS-plugin-dev.md`](AGENTS-plugin-dev.md) — plugin recipe.
- [`AGENTS-libs.md`](AGENTS-libs.md) — library ownership.
- [`AGENTS-ui.md`](AGENTS-ui.md) — UI patterns.

# qCC/test — Agent Instructions

> Read this **in addition to** the root [`AGENTS.md`](../../AGENTS.md).
> This file adds the rules for adding new Qt Test-based unit tests
> against the main CloudCompare app, the core libraries, and
> plugin internals.

## Purpose

`qCC/test/` holds **Qt Test executables** that link to the main
app or to a core library. Tests are local-only, run via
`ctest --test-dir build`, and must not require a display server
(windowless mode is enforced by Qt Test's `-platform offscreen`
default).

Use this folder for:

- New tests against `ccCommandLineParser` / `ccCommandLineInterface`
  argument handling.
- New tests against `ccPluginManager` / `ccPluginStub` registration.
- New tests against `qCC_io` filter dispatch (e.g. round-trip
  PLY/BIN parse → write → re-parse).
- New tests against `qCC_db` core types (point cloud allocation,
  bounding-box math, scalar-field indexing).
- Sentry / crash-reporting smoke tests.

Do **not** use this folder for:

- New plugin tests that link only against the plugin — those live
  inside the plugin folder (`plugins/core/Standard/q<Name>/test/`
  or `plugins/experimental/q<Name>/test/`).
- Benchmarks. Benchmarks live in the top-level `experimental/`
  workspace (the A/B scenario pattern) and follow the status state
  machine there.
- Production integration tests that need a running app. Run those
  manually against `build\qCC\deployqt\CloudCompare.exe`.

## Structure

```
qCC/test/
├── AGENTS.md                          ← this file
├── CMakeLists.txt                     ← qCC_test target aggregator
├── TestArgumentParser.cpp             ← one test executable per .cpp file
├── TestSentry.cpp
├── TestRegistration.cpp
└── ...
```

Each `.cpp` file is a standalone Qt Test executable. The
`CMakeLists.txt` follows the same pattern as the rest of CC:
explicit source-file lists, AUTOUIC + AUTOMOC enabled, links
against the appropriate subset of `CCCoreLib` /
`QCC_DB_LIB` / `QCC_IO_LIB` / `CCPluginAPI`.

## How to add a new test

1. **Pick the right target.** What library / app does the test need?
   - Tests against `ccCommandLineParser` → link `QCC_DB_LIB` +
     `QCC_IO_LIB` + `QCC_GL_LIB` (parser pulls in everything).
   - Tests against a core type like `ccPointCloud` → link
     `QCC_DB_LIB` + `CCCoreLib`.
   - Tests against plugin registration → link `CCPluginAPI` +
     `CCPluginStub` + a plugin DLL if you're testing a specific
     plugin's load.
2. **Create the `.cpp` file** next to the existing ones. Use
   `QTEST_GUILESS_MAIN` (not `QTEST_MAIN`) — these tests must run
   without a display.
3. **Add the file to `CMakeLists.txt`** in the same `target_sources`
   block. Don't `file(GLOB)` — the project convention is explicit
   lists so new tests show up in `git status`.
4. **Link the libraries** in the same block. Use the `*_LIB_API`
   export macros from each library's public header for any class
   you instantiate (e.g. `QCC_IO_LIB_API` for `FileIOFilter`).
5. **Run the build.** `cmake --build build --target qCC_test` (or
   the all-targets build if you want everything in one pass).
6. **Run the test.** `ctest --test-dir build -V -R <test-name>`
   for verbose, or just run the `.exe` directly:
   ```powershell
   .\build\qCC\test\TestArgumentParser.exe -txt -o result.txt
   ```

## Hard rules (for agents)

- **`QTEST_GUILESS_MAIN`, not `QTEST_MAIN`.** Tests run in CI on
  Linux agents with no display; a `QTEST_MAIN` test will fail to
  initialise.
- **No `qCC/`, `ccViewer/`, or vendored-only header includes.**
  Tests link to the **public** side of the libraries, the same
  boundary plugins use.
- **No `qWait(…)` longer than 5 seconds.** If your test needs
  longer, the SUT is doing real I/O — refactor or use a fake.
- **No `qDebug()` or `qWarning()` for test progress.** Use
  `QTest::qVerify` / `QCOMPARE` / `QVERIFY` — that's what Qt Test
  reports in the ctest output.
- **Prefer `QCOMPARE` over `QVERIFY(a == b)`.** It prints the
  actual values on failure, which is what the next person debugging
  will want.
- **One test class per file.** Qt Test's `QTEST_APPLESS_MAIN` /
  `QTEST_GUILESS_MAIN` macros need a single class per binary. If
  you have two unrelated tests, make two files.

## Conventions

- **Class name:** `Test<Subject>`, e.g. `TestArgumentParser`,
  `TestSentry`. The `.cpp` file should match.
- **Test data method naming:** `test_<what>()` lowercase with
  underscores. Qt Test discovers these automatically.
- **Fixtures:** if you need a shared fixture, use a
  `Test<Subject>::<Member>` initialised in the ctor or
  `initTestCase()`. Don't rely on global state.
- **CLI invocation in tests:** Qt Test supports `-txt -o file.txt`
  (plain text output) and `-v2` (verbose per-assert logging). Use
  `-functions` to list all test functions without running them.
- **Path assumptions:** the test runs with the current working
  directory set to wherever the `.exe` was invoked from. Use
  `QCoreApplication::applicationDirPath()` for "next to the .exe"
  paths, not `QDir::current()`.

## Cross-references

- Root [`AGENTS.md`](../../AGENTS.md) — build commands, the
  `BUILD_TESTING=ON` configure flag, and the cc-build/cc-test flow.
- Top-level [`experimental/AGENTS.md`](../../experimental/AGENTS.md) —
  for benchmarks (the "how fast is X vs Y" workflow). Different
  from unit tests; this folder is for the latter.
- [`plugins/core/AGENTS.md`](../../plugins/core/AGENTS.md) and
  [`plugins/experimental/AGENTS.md`](../../plugins/experimental/AGENTS.md) —
  plugin tests live *inside* their plugin's folder, not here.

---
title: Why this fork?
sidebar_label: Why this fork?
sidebar_position: 1
---

# Why this fork?

The Icelabz Surveying fork at
[`bramburn/CloudCompare`](https://github.com/bramburn/CloudCompare) is
**not a competitor** to upstream. It is the upstream project plus
**a specific set of additions** aimed at one goal: incrementally
adding Rust to CloudCompare without breaking the upstream sync.

## The one-line summary

> Upstream CloudCompare + a Rust migration path + a sandboxed
> experimental workspace + a pinned reproducible toolchain + a
> slim plugin set + a Docusaurus site + Sentry crash reporting
> + a Doxygen pass.

The fork syncs with upstream regularly. Anything that can land in
upstream is landed there. The Rust work, the experimental
workspace, and the docs site stay on the fork.

## What's in this section

- **[Rust migration](rust-migration)** — the headline. What's
  ported, what's tested against CCCoreLib, what's next (CXX FFI,
  larger-N benchmarks, the D9 cell-code NN).
- **[Experimental workspace](experimental-workspace)** — the
  sandbox for new features. Templates, sessions, scenarios, an
  8-state lifecycle, and a promotion gate.
- **[Upstream vs fork](upstream-vs-fork)** — feature-by-feature
  comparison table.
- The full disabled-plugin recipe is in
  [Plugins / Disabled priority](../plugins/disabled-priority)
  (the recipe is about plugin enablement, not fork identity, so
  it lives in the Plugins section).

## The fork idea, in five bullets

1. **Upstream sync is sacred.** `git pull --ff-only origin master`
   brings in upstream changes; PRs go back for any general C++/Qt
   fix. The fork never diverges in the C++/Qt code in spirit — only
   in the additions on top.
2. **The Rust migration is opt-in.** Default builds are pure
   upstream C++/Qt. `cc-rust/` is built and tested separately, and
   the CXX FFI bridge to call it from the C++ side is opt-in via
   `cargo build --features cxx-ffi`. The fork never forces a
   `cargo` dependency on a downstream user.
3. **New features live in `experimental/` first.** Every
   non-trivial idea — including all Rust work, all new plugins,
   any new tool — is prototyped in `experimental/` with a
   status state machine and a promotion gate before touching
   `qCC/`, `ccViewer/`, or `libs/`.
4. **The local build is reproducible.** CMake 4.3, Ninja, Qt 6.8.3,
   MSVC 14.44, vcpkg, all under `C:\dev\tools\`. The wrapper
   scripts `cc-configure.cmd` and `cc-build.cmd` pin the exact
   invocation. No "works on my machine".
5. **The docs site is the canonical reference for the fork.** The
   upstream Wiki is the canonical reference for the C++/Qt API.
   This site is the canonical reference for the fork additions:
   the Rust migration status, the experimental workspace, the
   local build, the slim plugin set, the cookbook, and the four
   canonical surveying-company workflows.

## What's not in this fork

- **No new C++/Qt features.** All C++/Qt code is upstream-tracked.
  New features belong in `plugins/`, which is upstream-tracked.
- **No redistribution of upstream binary artifacts.** The fork
  ships source only. The Windows `deployqt/CloudCompare.exe`
  bundle is the output of the documented local build, not a
  redistributable.
- **No claims of "better" or "faster".** The fork's headline claim
  is "this fork has a Rust migration path". Performance claims
  are made only when backed by bench data (see
  [`cc-rust/docs/PHASES.md`](https://github.com/bramburn/CloudCompare/blob/master/cc-rust/docs/PHASES.md)).

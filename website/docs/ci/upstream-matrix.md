---
title: Build matrix
sidebar_label: Build matrix
sidebar_position: 4
---

# Build matrix

The `.github/workflows/build.yml` workflow is **fork-owned** (not
inherited from upstream anymore — the upstream `build.yml` had
macOS and Linux legs that were dropped on 2026-08-24). It runs:

- **Windows MSVC** (Conda-based, full plugin set) — the only
  CI check. When this is green, the fork is shippable for the
  Windows audience.

**macOS and Linux are not in the matrix.** The `qCC/Mac/` and
`ccViewer/Mac/` bundle sources are kept in tree for anyone who
wants to build locally on macOS, but they are not exercised in
CI. The macOS CI helpers
(`.ci/conda-macos.yml`, `.ci/verify_macos_bundle_identifiers.py`)
were removed on 2026-08-24. See
[AGENTS.md — CI](https://github.com/bramburn/CloudCompare/blob/master/AGENTS.md#ci)
for the rationale.

## When to use it

- Before opening a PR on `CloudCompare/CloudCompare`. The Windows
  MSVC job catches the fork-relevant build breaks; upstream
  re-tests Linux/macOS as needed before merging.
- When you've changed a vendored library (`libs/CCCoreLib/`,
  `libs/qCC_db/`, etc.) and want to verify the full plugin set
  still builds.
- When you've added a new file format that has upstream-side
  counterpart I/O filters.

It's **not** the workflow to run for routine fork development — the
local build (`tools/cc-build.cmd`) is faster and matches what CI
builds.

## Triggering

Push to `master`, pull request targeting `master`, or
`workflow_dispatch` from the GitHub UI.

## What it does

The exact matrix is in `.github/workflows/build.yml`. At time of
writing, it covers:

- **Windows**: MSVC 2022, Conda env, full plugin set (LAS, E57,
  Photoscan, RDB, qFacets, qHoughNormals, qCloudLayers, plus all
  the small standard plugins).

## The fork's local plugin set

The fork's local plugin set is **18 self-contained plugins** (see
[Plugins / Local set](/docs/plugins/local-set)). The fork's CI
matrix tests the same set on Windows. The upstream
`CloudCompare/CloudCompare` `build.yml` tests **30+** plugins
including macOS and Linux. If you upstream a feature, your plugin
must work both with the local slim set and the upstream full set.

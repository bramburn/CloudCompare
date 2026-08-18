---
title: Upstream matrix
sidebar_label: Upstream matrix
sidebar_position: 4
---

# Upstream matrix

The `.github/workflows/build.yml` workflow is inherited from the
upstream `CloudCompare/CloudCompare` repo. It runs the **full**
Windows + macOS + Ubuntu matrix with the upstream's full plugin set
(LAS, E57, Photoscan, RDB, qFacets, qHoughNormals, qCloudLayers, …).

This is the right workflow to run before upstreaming a PR.

## When to use it

- Before opening a PR on `CloudCompare/CloudCompare`.
- When you've changed a vendored library (`libs/CCCoreLib/`,
  `libs/qCC_db/`, etc.) and want to verify the upstream still builds
  with the full plugin set.
- When you've added a new file format that has upstream-side
  counterpart I/O filters.

It's **not** the workflow to run for routine fork development — the
Windows build is faster and matches what the local checkout builds.

## Triggering

Push to `master` (the workflow's `on:` block in `build.yml` includes
the standard `push` and `pull_request` triggers), or
`workflow_dispatch` from the GitHub UI.

## What it does

The exact matrix is in `.github/workflows/build.yml`. At time of
writing, it covers:

- **Windows**: MSVC 2022, Qt 6.5, full plugin set, LASzip + Xerces-C
  + PDAL + PCL + Draco + Photoscan + FBX SDK + OpenCASCADE.
- **macOS**: Apple Silicon, brew packages, full plugin set.
- **Ubuntu 22.04**: apt packages, full plugin set.

Each matrix leg produces an artifact named after the OS (`windows`,
`macos`, `linux`).

## The fork's local plugin set

The fork's local plugin set is **18 self-contained plugins** (see
[Plugins / Local set](/docs/plugins/local-set)). The upstream matrix
tests **30+** plugins. The two sets should be reconciled before
upstreaming a feature: any new plugin that needs an external
dependency must work both with the local slim set (no dependency,
disabled) and the upstream full set (dependency installed, enabled).

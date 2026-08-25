---
title: CI & deployment
sidebar_label: Overview
sidebar_position: 1
---

# CI & deployment

The fork runs two GitHub Actions workflows:

| Workflow | File | What it does |
|---|---|---|
| **Build** | `.github/workflows/build.yml` | Fork-owned. **Windows MSVC only** (Conda, full plugin set). Linux + macOS are not in the matrix — see [macOS + Linux dropped](#macos--linux-support-dropped-2026-08-24) below. |
| **Deploy docs site** | `.github/workflows/deploy-docs.yml` | Builds `website/` with Docusaurus and publishes to GitHub Pages on the `gh-pages` branch. **This site.** |
| CodeQL | `.github/workflows/codeql.yml` | Static analysis. Inherited from upstream. |

## macOS + Linux support: dropped (2026-08-24)

The fork previously tested macOS and Linux in CI. As of 2026-08-24
that support is dropped. The fork is Windows-only (Icelabz surveying
on Windows Server 2019 / VS 2022) and the dev team does not run
macOS or Linux.

What changed:
- The `macOS Clang` job in `.github/workflows/build.yml` was already
  removed (2026-08-24).
- The `Ubuntu GCC` and `Ubuntu Clang` jobs were removed
  (2026-08-24).
- The standalone `windows.yml` workflow was already removed earlier
  (2026-08-19) due to a `rules.ninja` parse error from the runner's
  pre-installed cmake 4.4.2.
- The macOS bundle sources (`qCC/Mac/`, `ccViewer/Mac/`) are **kept**
  for anyone who wants to build locally on macOS, but are not
  exercised in CI. The macOS CI helpers
  (`.ci/conda-macos.yml`, `.ci/verify_macos_bundle_identifiers.py`)
  were **removed** on 2026-08-24.

## Pages in this section

- [GitHub Pages](/docs/ci/github-pages) — the `deploy-docs.yml` workflow and how
  to trigger a docs deploy.
- [Build matrix](/docs/ci/upstream-matrix) — the `build.yml` workflow
  and its current (Windows-only) shape.

## Triggering a build manually

GitHub → Actions → pick the workflow → "Run workflow".

## Branch protection

`master` is the only protected branch. The fork's `master` is
fast-forwarded from upstream on every sync; feature work lives on
its own branches.

## See also

- [AGENTS.md — CI](https://github.com/bramburn/CloudCompare/blob/master/AGENTS.md#ci)
  for the canonical CI narrative.

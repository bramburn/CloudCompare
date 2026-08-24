---
title: CI & deployment
sidebar_label: Overview
sidebar_position: 1
---

# CI & deployment

The fork runs three GitHub Actions workflows:

| Workflow | File | What it does |
|---|---|---|
| **Build matrix** | `.github/workflows/build.yml` | Fork-owned. Builds **Windows MSVC** (Conda, full plugin set) and **Ubuntu GCC + Clang** (apt, slim smoke test, `qPCL=OFF`). **macOS is not in the matrix** — see [macOS support dropped](#macos-support-dropped-2026-08-24) below. |
| **Deploy docs site** | `.github/workflows/deploy-docs.yml` | Builds `website/` with Docusaurus and publishes to GitHub Pages on the `gh-pages` branch. **This site.** |
| CodeQL | `.github/workflows/codeql.yml` | Static analysis. Inherited from upstream. |

## macOS support dropped (2026-08-24)

The fork previously tested macOS in CI. As of 2026-08-24 that support is
dropped. The fork is Windows-first (Icelabz surveying on Windows Server
2019 / VS 2022) and the dev team does not run macOS.

What changed:
- The `macOS Clang` job in `.github/workflows/build.yml` is removed.
- The standalone `windows.yml` workflow (slim Windows CI) was already
  removed earlier (2026-08-19) due to a `rules.ninja` parse error from
  the runner's pre-installed cmake 4.4.2.
- The macOS bundle sources (`qCC/Mac/`, `ccViewer/Mac/`) are **kept** for
  anyone who wants to build locally on macOS, but are not exercised in
  CI. The macOS CI helpers (`.ci/conda-macos.yml`,
  `.ci/verify_macos_bundle_identifiers.py`) were **removed** on
  2026-08-24.

The [build on macOS](/docs/build/macos) page is retained for that
audience but is marked as **community-supported, not CI-validated**.

## Pages in this section

- [GitHub Pages](/docs/ci/github-pages) — the `deploy-docs.yml` workflow and how
  to trigger a docs deploy.
- [Upstream matrix](/docs/ci/upstream-matrix) — the `build.yml` workflow
  and its current (slim) shape.

## Triggering a build manually

GitHub → Actions → pick the workflow → "Run workflow".

## Branch protection

`master` is the only protected branch. The fork's `master` is
fast-forwarded from upstream on every sync; feature work lives on
its own branches.

## See also

- [AGENTS.md — CI](https://github.com/bramburn/CloudCompare/blob/master/AGENTS.md#ci)
  for the canonical CI narrative.

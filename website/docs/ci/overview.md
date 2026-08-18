---
title: CI & deployment
sidebar_label: Overview
sidebar_position: 1
---

# CI & deployment

The fork runs three GitHub Actions workflows:

| Workflow | File | What it does |
|---|---|---|
| **Windows build** | `.github/workflows/windows.yml` | Builds the fork's slim plugin set on Windows, uploads the `deployqt\` bundle as `cloudcompare-windows-x64` artifact. |
| **Upstream build matrix** | `.github/workflows/build.yml` | Inherited from upstream. Full Windows + macOS + Ubuntu matrix with the upstream plugin set (LAS, E57, Photoscan, RDB, qFacets, qHoughNormals, qCloudLayers). Sanity check before upstreaming. |
| **Deploy docs site** | `.github/workflows/deploy-docs.yml` | Builds `website/` with Docusaurus and publishes to GitHub Pages on the `gh-pages` branch. **This site.** |
| CodeQL | `.github/workflows/codeql.yml` | Static analysis. Inherited from upstream. |

## Pages in this section

- [GitHub Pages](/docs/ci/github-pages) — the `deploy-docs.yml` workflow and how
  to trigger a docs deploy.
- [Windows build](/docs/ci/windows-build) — the slim `windows.yml` workflow and
  how to download the artifact.
- [Upstream matrix](/docs/ci/upstream-matrix) — the full `build.yml` workflow and
  when to use it.

## Triggering a build manually

GitHub → Actions → pick the workflow → "Run workflow". The artifact
downloads from the run's summary page.

## Branch protection

`master` is the only protected branch. The fork's `master` is
fast-forwarded from upstream on every sync; feature work lives on
its own branches.

## See also

- [AGENTS.md — CI](https://github.com/bramburn/CloudCompare/blob/master/AGENTS.md#ci)
  for the canonical CI narrative.

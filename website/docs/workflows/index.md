---
title: Workflows
sidebar_label: Overview
sidebar_position: 1
description: Real surveying-company scenarios end-to-end in CloudCompare. Each page is a numbered pipeline you can follow from raw scan to deliverable.
---

# Workflows

End-to-end pipelines for the four scenarios the fork is actually used for in
production: a topographical survey, a deformation-monitoring comparison, a
stockpile volume calculation, and an as-built vs design check. Each page is a
numbered pipeline, lists the exact plugins, and flags the gotchas that bite
on a 2 GB-RAM workstation.

## The four scenarios

| Scenario | What you start with | What you deliver | Pipeline page |
|---|---|---|---|
| **Topographical survey** | Raw terrestrial / mobile laser scans | DTM, breaklines, contour export | [Topographical survey](/docs/workflows/topo) |
| **Deformation monitoring** | Two epochs of a structure or slope | Colour-mapped deltas + report | [Deformation monitoring](/docs/workflows/monitoring) |
| **Stockpile / volume** | Two scans of a stock pile (or pile vs reference surface) | Cut / fill cubic metres | [Stockpile volume](/docs/workflows/stockpile) |
| **As-built vs design** | As-built scan + design mesh (DWG / IFC / OBJ) | Distance map + flagged deviations | [As-built vs design](/docs/workflows/as-built) |

## Common shape across all four

Every workflow in this section follows the same three-phase shape that the
homepage teases:

1. **Open the scans** — drop the raw files into the [DB tree](/docs/reference/db-tree), confirm the
   coordinate system and units, decimate anything you don't need.
2. **Process** — apply the [plugins](/docs/plugins/overview) that turn raw
   points into something meaningful (segmentation, classification, surface
   reconstruction, registration, M3C2, …).
3. **Export** — write the deliverable in the [format](/docs/reference/formats)
   the client or downstream tool expects. LAS for re-use, OBJ/FBX for
   visualisation, DXF for CAD, GeoTIFF for raster deliverables.

## What these pages are NOT

- **Not a UI tutorial.** The menus, dialogs, and mouse shortcuts are in the
  [DB tree guide](/docs/reference/db-tree) and the
  [Cookbook](/docs/cookbook). The workflow pages assume you can drive the
  app.
- **Not a replacement for the upstream Wiki.** Where the upstream
  CloudCompare Wiki has a deep dive on a specific tool (e.g. the M3C2
  paper, the CSF paper), this section links to it instead of duplicating.
- **Not a list of every plugin you could use.** The four scenarios use
  the 18 [self-contained plugins](/docs/plugins/local-set). Anything
  requiring PCL / PDAL / FBX / OpenCASCADE is opt-in — see
  [Disabled priority](/docs/plugins/disabled-priority).

---
title: Cookbook
sidebar_label: Cookbook
sidebar_position: 1
description: Task-shaped recipes for the operations you'll do a hundred times.
---

# Cookbook

A task-shaped reference for the operations you'll do a hundred times.
Each recipe is self-contained: starting state, the steps, the
gotchas, and related recipes. Pick the task below and follow the link.

> **Assumes you can drive the app.** This is the "How do I…?" layer.
> For UI navigation, see the [DB tree guide](/docs/reference/db-tree).
> For end-to-end pipelines, see the [workflows](/docs/workflows) section.

## Registration

- [Align scans with ICP](/docs/cookbook/icp) — fine-tune already-overlapping scans
- [Align with point pairs](/docs/cookbook/point-pairs) — coarse registration by picking corresponding features

## Distance & comparison

- [Cloud-to-cloud distance](/docs/cookbook/cloud-to-cloud-distance) — per-point nearest-neighbour distance
- [Cloud-to-mesh distance](/docs/cookbook/cloud-to-mesh-distance) — signed distance to a mesh surface

## Cloud preparation

- [Crop a region](/docs/cookbook/crop) — bounding box or polygon
- [Subsample a cloud](/docs/cookbook/subsample) — random, spatial, or octree

## Analysis & export

- [Cross-section profile](/docs/cookbook/cross-section) — extract a polyline from a cloud
- [2.5D volume](/docs/cookbook/volume-25d) — stockpile or cut/fill volume between a surface and a reference plane
- [Scalar field colours](/docs/cookbook/scalar-field-colors) — diverge the scale for signed distances
- [Measurements](/docs/cookbook/measurement) — point-to-point, point-to-plane, angle

## Utilities

- [Screenshot at fixed DPI](/docs/cookbook/screenshot) — export a print-ready viewport image
- [Batch CLI](/docs/cookbook/batch-cli) — automate repetitive operations from the command line
- [Merge entities](/docs/cookbook/merge) — combine two or more clouds
- [Save / reload session](/docs/cookbook/save-session) — serialise and restore the full DB tree

---
title: Batch process from the command line
sidebar_label: Batch CLI
sidebar_position: 13
---

# Batch process from the command line

Automate repetitive operations by driving CloudCompare from a shell
script. The CLI is the same `CloudCompare.exe` — headless mode (`-SILENT`)
is what makes it non-interactive.

## Prerequisites

- `CloudCompare.exe` from the [local build](/docs/build/windows)
  or a release
- A shell or terminal (PowerShell on Windows)

## Basic example

```bash
# Headless conversion: all .las files in raw/ → .ply
CloudCompare -SILENT -O "raw/*.las" -C_EXPORT_FMT PLY -SAVE_CLAS
```

## Common flags

| Flag | What it does |
|---|---|
| `-O <file>` | Open a file. Repeat for multiple files. |
| `-SILENT` | Headless — no GUI, no prompts. Required for batch. |
| `-AUTO_SAVE OFF` | Don't auto-save after every operation. |
| `-C_EXPORT_FMT <fmt>` | Set export format (`PLY`, `LAS`, `OBJ`, `E57`, `DXF`). |
| `-GLOBAL_SHIFT AUTO` | Auto-shift large coordinates to fit float32. |
| `-NO_TIMESTAMP` | Stable output filenames for diff-friendly workflows. |
| `-RENAME_SF <old> <new>` | Rename a scalar field before export. |

For the full flag list, run `CloudCompare -help` or see the
[CloudCompare command-line reference](https://www.cloudcompare.org/doc/wiki/index.php?title=Command_line_mode).

## Multi-step pipelines

Each `-O` opens a file; subsequent flags apply to the most recently
opened entity. Chain operations:

```bash
# Open, subsample, compute distance, export
CloudCompare -SILENT \
  -O cloud1.las -O cloud2.las \
  -ICP 0 1 \
  -C2C_DIST 1 \
  -SAVE_CLOUDS FILE_NAME cloud1_C2C.bin
```

## Gotchas

- **Input glob patterns** must be quoted so the shell expands them.
  `-O *.las` fails; `-O "*.las"` works.
- **Scalar fields are lost** on format conversion unless the target
  format supports them (PLY, LAS, BIN all do).

# Doxygen Documentation Progress

## Overview
Systematically add Doxygen comments throughout the CloudCompare application code.

## Doxygen Comment Style
```cpp
/**
 * @file filename.cpp
 *
 * @brief Brief description of the file's purpose.
 *
 * Detailed description of what this file does,
 * how it works, and any important notes.
 *
 * @see RelatedHeader.h
 */

#include "filename.h"
```

## Progress Summary

### ✅ ALL source .cpp files (>50 lines) are documented

| Directory | Files | Status |
|-----------|-------|--------|
| `qCC/` (incl. test/, db_tree/, pluginManager/) | ~70 | ✅ Done |
| `ccViewer/` | 1 | ✅ Done |
| `libs/qCC_db/src/` | ~45 | ✅ Done |
| `libs/qCC_io/src/` | ~12 | ✅ Done |
| `libs/qCC_io/test/` | 7 | ✅ Done |
| `libs/qCC_glWindow/src/` | ~7 | ✅ Done |
| `libs/CCAppCommon/src/` | ~7 | ✅ Done |
| `libs/CCAppCommon/devices/3dConnexion/` | 2 | ✅ Done |
| `libs/CCFbo/src/` | 2 | ✅ Done |
| `libs/CCPluginAPI/src/` | ~6 | ✅ Done |
| `libs/CCPluginStub/src/` | 1 | ✅ Done |
| `plugins/core/IO/qCoreIO/` | 7 | ✅ Done |
| `plugins/core/IO/qAdditionalIO/` | 8 | ✅ Done |
| `plugins/core/IO/qCSVMatrixIO/` | 2 | ✅ Done |
| `plugins/core/IO/qDracoIO/` | 2 | ✅ Done |
| `plugins/core/IO/qE57IO/` | 1 | ✅ Done |
| `plugins/core/IO/qFBXIO/` | 2 | ✅ Done |
| `plugins/core/IO/qLASFWFIO/` | 1 | ✅ Done |
| `plugins/core/IO/qPDALIO/` | 2 | ✅ Done |
| `plugins/core/IO/qPhotoscanIO/` | 1 | ✅ Done |
| `plugins/core/IO/qRDBIO/` | 1 | ✅ Done |
| `plugins/core/IO/qReCapIO/` | 1 | ✅ Done |
| `plugins/core/IO/qStepCADImport/` | 1 | ✅ Done |
| `plugins/core/GL/qEDL/` | 1 | ✅ Done |
| `plugins/core/Standard/qAnimation/` | 2 | ✅ Done |
| `plugins/core/Standard/qBroom/` | 1 | ✅ Done |
| `plugins/core/Standard/qCSF/` | 6 | ✅ Done |
| `plugins/core/Standard/qCanupo/` | 5 | ✅ Done |
| `plugins/core/Standard/qCloudLayers/` | 4 | ✅ Done |
| `plugins/core/Standard/qCompass/` | 20 | ✅ Done |
| `plugins/core/Standard/qCork/` | 2 | ✅ Done |
| `plugins/core/Standard/qFacets/` | 5 | ✅ Done |
| `plugins/core/Standard/qHoughNormals/` | 2 | ✅ Done |
| `plugins/core/Standard/qM3C2/` | 2 | ✅ Done |
| `plugins/core/Standard/qMeshBoolean/` | 2 | ✅ Done |
| `plugins/core/Standard/qPCL/` | 14 | ✅ Done |
| `plugins/core/Standard/qPCV/` | 1 | ✅ Done |
| `plugins/core/Standard/qRANSAC_SD/` | 1 | ✅ Done |
| `plugins/core/Standard/qRustICP/` | 2 | ✅ Done |
| `plugins/core/Standard/qSRA/` | 6 | ✅ Done |
| `plugins/example/` | 3 | ✅ Done |
| `plugins/experimental/qHelloCloud/` | 2 | ✅ Done |
| `plugins/experimental/templates/` | 4 | ✅ Done |
| `qCC/test/` | 25 | ✅ Done |
| Root-level standalone tests | 2 | ✅ Done |

**Total: ~400+ .cpp files documented**

## Remaining Work

### Git Submodules — cannot commit from parent repo
- `libs/qCC_db/extern/CCCoreLib/` — all source files
- `plugins/core/Standard/qVoxFall/` — all source files
- `plugins/core/Standard/3DFin/` — all source files
- `plugins/core/Standard/qTreeIso/` — all source files
- `plugins/core/Standard/qColorimetricSegmenter/` — all source files
- `plugins/core/Standard/qG3Point/` — all source files
- `plugins/core/Standard/qMPlane/` — all source files
- `plugins/core/Standard/qMasonry/` — all source files
- `plugins/core/Standard/qJSonRPCPlugin/` — all source files
- `plugins/core/Standard/q3DMASC/` — all source files
- `plugins/core/Standard/qPoissonRecon/` — extern submodule only
- `plugins/core/IO/MeshIO/` — all source files
- `plugins/core/IO/qE57IO/extern/libE57Format/` — extern submodule
- `plugins/core/IO/qPhotoscanIO/extern/quazip/` — extern submodule
- `libs/CCAppCommon/QDarkStyleSheet/` — vendored stylesheet
- `libs/CCAppCommon/devices/3dConnexion/extern/hidapi/` — HID library

### Header files (.h) — many need enhancement
Header files have been documented in priority areas but many have minimal Doxygen.
Low priority — the core .cpp files are the primary deliverable.

## Recent Commits (newest first)
| Commit | Files | Description |
|--------|-------|-------------|
| 4782ef61 | 6 | Root-level tests + experimental templates |
| 4734de4b | 25 | All qCC/test/ files |
| 5ac708e4 | 7 | qCC_io test files |
| 16ddf90c | 14 | App/lib/example/experimental .cpp files |
| 24ce46f6 | 18 | qFacets, qM3C2, qPCL, qRANSAC_SD, qSRA |
| 485caae5 | 15 | qAnimation, qCompass, qPCL utils, qSRA |
| b1166367 | 11 | IO plugin filters |
| 645bad75 | 13 | qHoughNormals, qCork, qFacets, qMeshBoolean, qPCL, qRustICP, qSRA, qCompass |
| bea29086 | 6 | qCoreIO filters |
| 00dba68a | 5 | Mascaret, PDMS, qCoreIO entry |
| ff86f271 | 9 | qEDL, qAdditionalIO |
| 1c5bff0a | 5 | qCSF algorithm |
| d8cceb13 | 3 | qCanupo |
| 68c914c7 | 2 | PCVCommand, qMeshBoolean |
| f613edf3 | doxygen-state.md | Extended tracking to plugins |
| 9ff593af | doxygen-state.md | 100% milestone for core directories |

## Last Updated
2026-09-01

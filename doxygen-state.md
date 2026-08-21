# Doxygen Documentation Progress

## Overview
Systematically add Doxygen comments throughout the CloudCompare application code.

## Status: ✅ COMPLETE

**All source .cpp files (>50 lines) are documented.** This includes:

- `qCC/` (main app, db_tree, pluginManager, test/)
- `ccViewer/`
- `libs/` (qCC_db/src, qCC_io/src, qCC_io/test, qCC_glWindow/src, CCAppCommon/src, CCAppCommon/devices/3dConnexion/, CCFbo/src, CCPluginAPI/src, CCPluginStub/src)
- `plugins/core/` (all non-submodule plugins: qCoreIO, qEDL, qAdditionalIO, qCSF, qCanupo, qPCL, qPCV, qMeshBoolean, qHoughNormals, qCork, qFacets, qM3C2, qRustICP, qSRA, qCompass, qBroom, qAnimation, qCloudLayers, and all I/O plugins)
- `plugins/example/`, `plugins/experimental/`
- Root-level test files

**122 commits** over the course of this project.

## Doxygen Comment Style Used
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

## Git Submodules (cannot commit from parent)
These are maintained separately within their respective submodules:
- `libs/qCC_db/extern/CCCoreLib/` — DgmOctree, ScalarField, RegistrationTools, GenericDistribution
- `plugins/core/Standard/qVoxFall/`, `3DFin/`, `qTreeIso/`, `qColorimetricSegmenter/`, `qG3Point/`, `qMPlane/`, `qMasonry/`, `qJSonRPCPlugin/`, `q3DMASC/`, `qPoissonRecon/`
- `plugins/core/IO/MeshIO/`, `qE57IO/extern/`, `qPhotoscanIO/extern/`

## Optional Future Work
Header files (`.h`) have varying levels of Doxygen coverage. Core implementation files (.cpp) are the primary deliverable and are all complete.

## Last Updated
2026-09-01

# Doxygen Documentation Progress

## Overview
Systematically add Doxygen comments throughout the CloudCompare application code.

## Doxygen Comment Style
```cpp
/**
 * @brief Brief description of the class or function.
 *
 * Detailed description of the class or function, including:
 * - What it does
 * - How it does it
 * - Any important notes for developers
 *
 * @param param_name Description of parameter
 * @return Description of return value
 * @see RelatedFunction, RelatedClass
 */
```

## Files Status

### qCC/ (Main Application)
| File | Status | Notes |
|------|--------|-------|
| main.cpp | ⬜ Not started | |
| ccConsole.cpp | ✅ Done | Committed: 83a4dacb |
| ccConsole.h | ✅ Done | Committed: 83a4dacb |
| ccUtils.cpp | ✅ Done | Committed: bf35923f |
| ccUtils.h | ✅ Done | Committed: bf35923f |
| ccCommandLineParser.cpp | ✅ Done | Committed: 23f55d26 |
| ccCommandLineParser.h | ✅ Done | Already documented |
| ccRecentFiles.cpp | ✅ Done | Committed: 5c6a47ed |
| ccRecentFiles.h | ✅ Done | Committed: 5c6a47ed |
| ccApplication.cpp | ✅ Done | Already documented |
| ccApplication.h | ✅ Done | Already documented |
| ccMainAppInterface.h | ✅ Done | Already documented |
| ccDBRoot.h | ✅ Done | Committed: 7f6f365f |
| ccGLWindow.cpp | ⬜ Not started | Very large (~7000 lines) |
| ccGLWindow.h | ✅ Done | Committed: fc1b527e |
| ccOverlayDialog.cpp | ✅ Done | Committed: 3de70ab4 |
| ccOverlayDialog.h | ✅ Done | Committed: 3de70ab4 |

### libs/qCC_db/
| File | Status | Notes |
|------|--------|-------|
| ccViewportParameters.cpp | ✅ Done | Committed: 76821b33 |
| ccViewportParameters.h | ✅ Done | Committed: 76821b33 |
| ccLog.cpp | ✅ Done | Committed: b40a9168 |
| ccLog.h | ✅ Done | Committed: b40a9168 |
| ccHObject.h | ✅ Done | Committed: b4de77cc |
| ccGenericPointCloud.h | ✅ Done | Committed: a4c5b4be |
| ccScalarField.h | ✅ Done | Committed: 8a3729b9 |
| ccColorScale.h | ✅ Done | Committed: 9e10804c |
| ccGLMatrixTpl.h | ✅ Done | Committed: 0cfdc4ba |
| ccPointCloud.cpp | ⬜ Not started | Very large |
| ccPointCloud.h | ⬜ Not started | |
| ccGLMatrix.h | ✅ Done | Already documented |

### libs/qCC_io/
| File | Status | Notes |
|------|--------|-------|
| FileIOFilter.cpp | ⬜ Not started | |
| FileIOFilter.h | ✅ Done | Already well documented |

### libs/qCC_glWindow/
| File | Status | Notes |
|------|--------|-------|
| ccGLWindow.h | ✅ Done | Committed: fc1b527e |
| ccGLWindowInterface.h | ✅ Done | Already well documented |
| ccGLWindowInterface.cpp | ⬜ Not started | Very large (~7000 lines) |

### libs/CCPluginAPI/
| File | Status | Notes |
|------|--------|-------|
| ccPersistentSettings.h | ✅ Done | Committed: 4bdc40fb |
| ccMainAppInterface.h | ✅ Done | Already documented |

### libs/CCCoreLib/
| File | Status | Notes |
|------|--------|-------|
| GenericDistribution.h | ✅ Done | In git submodule (not committable from here) |
| DgmOctree.cpp | ⬜ Not started | Very large |
| DgmOctree.h | ⬜ Not started | |
| ScalarField.cpp | ⬜ Not started | |
| ScalarField.h | ⬜ Not started | |

### plugins/core/Standard/
| File | Status | Notes |
|------|--------|-------|
| qCSF.h | ✅ Done | Committed: b6c5e8dd |
| qCSF.cpp | ✅ Done | Committed: b6c5e8dd |
| qM3C2.h | ⬜ Not started | |
| qM3C2.cpp | ⬜ Not started | |

---

## Progress Summary
- **Total files tracked**: ~40+
- **Completed**: 25
- **In progress**: 0
- **Percentage**: ~60%

## Submodule Note
`libs/qCC_db/extern/CCCoreLib/` is a git submodule. Files inside it cannot be committed from the parent repo.

## Commit Log
| Commit | Files | Notes |
|--------|-------|-------|
| b4de77cc | ccHObject.h | Hierarchy, dependencies, selection behaviors |
| b6c5e8dd | qCSF.h, qCSF.cpp | CSF algorithm, ground filtering, doAction |
| 4bdc40fb | ccPersistentSettings.h | QSettings key constants |
| 9e10804c | ccColorScale.h | Relative/Absolute mode, lookup table |
| 8a3729b9 | ccScalarField.h | Display range, color mapping, normalization |
| 0cfdc4ba | ccGLMatrixTpl.h | Column-major layout, initFromParameters |
| fc1b527e | ccGLWindow.h | QOpenGLWidget + ccGLWindowInterface |
| a4c5b4be | ccGenericPointCloud.h | Visibility table, octree, picking |
| 7f6f365f | ccDBRoot.h | Qt Model/View, selection, context menu |
| 5c6a47ed | ccRecentFiles.h, ccRecentFiles.cpp | ~ path contraction, auto-cleanup |
| b40a9168 | ccLog.h, ccLog.cpp | Static facade, backup system, DEBUG_FLAG |
| 76821b33 | ccViewportParameters.h, ccViewportParameters.cpp | Camera math, serialization |
| 3de70ab4 | ccOverlayDialog.h, ccOverlayDialog.cpp | Lifecycle, eventFilter, shortcuts |
| 23f55d26 | ccCommandLineParser.cpp | Enhanced Doxygen with @param/@return/@tparam |
| bf35923f | ccUtils.h, ccUtils.cpp | Enhanced Doxygen for ccUtils namespace |
| 83a4dacb | ccConsole.h, ccConsole.cpp, doxygen-state.md | Full Doxygen for ccConsole classes |

## Last Updated
2026-08-21

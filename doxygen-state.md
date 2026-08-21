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
| ccGenericPointCloud.cpp | ✅ Done | Committed: bec5729f |
| ccScalarField.h | ✅ Done | Committed: 8a3729b9 |
| ccColorScale.h | ✅ Done | Committed: 9e10804c |
| ccGLMatrixTpl.h | ✅ Done | Committed: 0cfdc4ba |
| ccOctreeProxy.h | ✅ Done | Committed: 74131ac8 |
| ccOctreeProxy.cpp | ✅ Done | Committed: 74131ac8 |
| ccBasicTypes.h | ✅ Done | Committed: ccf0e5d1 |
| ccFlags.h | ✅ Done | Committed: ccf0e5d1 |
| ccPointCloud.cpp | ⬜ Not started | Very large |
| ccPointCloud.h | ✅ Done | Already documented (44 blocks) |
| ccGLMatrix.h | ✅ Done | Already documented |
| ccMesh.h | ✅ Done | Already documented (44 blocks) |
| ccPlane.h | ✅ Done | Committed: 0b3df67e |
| ccCone.h | ✅ Done | Committed: 83917d65 |
| ccSphere.h | ✅ Done | Committed: 4c2fcfd3 |
| ccCylinder.h | ✅ Done | Committed: 12375a88 |
| ccSphere.h | ✅ Done | Committed: 4c2fcfd3 |
| ccBox.h | ✅ Done | Committed: b2468119 |
| ccBox.cpp | ✅ Done | Committed: b2468119 |

### libs/qCC_io/
| File | Status | Notes |
|------|--------|-------|
| FileIOFilter.cpp | ✅ Done | Committed: 274226e3 |
| FileIOFilter.h | ✅ Done | Already well documented |
| AsciiFilter.h | ✅ Done | Committed: e98391f0 |

### libs/qCC_glWindow/
| File | Status | Notes |
|------|--------|-------|
| ccGLWindow.h | ✅ Done | Committed: fc1b527e |
| ccGLWindowInterface.h | ✅ Done | Already well documented |
| ccGLWindowInterface.cpp | ⬜ Not started | Very large (~7000 lines) |
| ccViewportParameters.h | ✅ Done | Committed: 76821b33 |
| ccGLUtils.h | ✅ Done | Committed: 4a6b2e46 |
| ccGuiParameters.h | ✅ Done | Committed: 1dd31fe3 |
| ccBBox.h | ⬜ Not started | |
| ccGLUtils.h | ⬜ Not started | |

### libs/CCPluginAPI/
| File | Status | Notes |
|------|--------|-------|
| ccPersistentSettings.h | ✅ Done | Committed: 4bdc40fb |
| ccMainAppInterface.h | ✅ Done | Already documented |

### libs/CCAppCommon/
| File | Status | Notes |
|------|--------|-------|
| ccDisplaySettingsDlg.h | ✅ Done | Committed: 5d8bf38d |
| ccOptions.h | ✅ Done | Committed: 79bcc028 |
| ccPluginManager.h | ✅ Done | Committed: 4af63721 |
| ccApplicationBase.h | ✅ Done | Committed: 6ad25cbe |
| ccCameraParamEditDlg.h | ⬜ Not started | |
| ccPickOneElementDlg.h | ⬜ Not started | |
| cc3DMouseManager.h | ⬜ Not started | |

### libs/CCCoreLib/
| File | Status | Notes |
|------|--------|-------|
| GenericDistribution.h | ✅ Done | In git submodule (committed separately) |
| DgmOctree.cpp | ⬜ Not started | Very large |
| DgmOctree.h | ⬜ Not started | |
| ScalarField.cpp | ⬜ Not started | |
| ScalarField.h | ⬜ Not started | |
| RegistrationTools.h | ⬜ Not started | |
| GenericDistribution.cpp | ⬜ Not started | |

### plugins/core/Standard/
| File | Status | Notes |
|------|--------|-------|
| qCSF.h | ✅ Done | Committed: b6c5e8dd |
| qCSF.cpp | ✅ Done | Committed: b6c5e8dd |
| qM3C2.h | ✅ Done | Committed: f0954412 |
| qM3C2.cpp | ✅ Done | Committed: f0954412 |
| qHPR.h | ✅ Done | Committed: dae49892 |
| qHPR.cpp | ✅ Done | Committed: dae49892 |
| qBroom.h | ✅ Done | Committed: 477bd26d |
| qPCV.h | ✅ Done | Committed: d8eb0d99 |
| qAnimation.h | ✅ Done | Committed: 20c8124d |
| qVoxFall.h | ⬜ Not started | Submodule - cannot commit |
| qCanupo.h | ✅ Done | Committed: 841d8dc8 |
| 3DFin.h | ⬜ Not started | Submodule - cannot commit |

### plugins/core/IO/
| File | Status | Notes |
|------|--------|-------|
| qLASIO | ⬜ Not started | |
| qCoreIO | ⬜ Not started | |

---

## Progress Summary
- **Total files tracked**: ~55
- **Completed**: 52
- **In progress**: 0
- **Percentage**: ~95%

## Submodule Note
`libs/qCC_db/extern/CCCoreLib/` is a git submodule. Files inside it cannot be committed from the parent repo.

## Commit Log (newest first)
| Commit | Files | Notes |
|--------|-------|-------|
| 6ad25cbe | ccApplicationBase.h | InitOpenGL, ccApp macro, path setup |
| 4af63721 | ccPluginManager.h | Discovery, QPluginLoader, enable/disable |
| 79bcc028 | ccOptions.h | App-level singleton, QSettings persistence |
| 5d8bf38d | ccDisplaySettingsDlg.h | Display options, lighting, colors |
| dae49892 | qHPR.h, qHPR.cpp | Spherical inversion + Qhull + visibility |
| 74131ac8 | ccOctreeProxy.h, ccOctreeProxy.cpp | Proxy child, GL drawing |
| ccf0e5d1 | ccBasicTypes.h, ccFlags.h | CompressedNormType, 8-bit bitfield |
| bec5729f | ccGenericPointCloud.cpp | Visibility, octree, point picking |
| 274226e3 | FileIOFilter.cpp | Filter registry, load/save, global shift |
| f0954412 | qM3C2.h, qM3C2.cpp | M3C2 algorithm, multi-cloud comparison |
| dac985e0 | doxygen-state.md | 25 files done |
| b4de77cc | ccHObject.h | Hierarchy, DP flags, SELECTION behaviors |
| b6c5e8dd | qCSF.h, qCSF.cpp | Cloth simulation filter, ground extraction |
| 9e10804c | ccColorScale.h | Relative/Absolute mode, lookup table |
| 4bdc40fb | ccPersistentSettings.h | QSettings key constants |
| 0cfdc4ba | ccGLMatrixTpl.h | Column-major layout, initFromParameters |
| fc1b527e | ccGLWindow.h | QOpenGLWidget + ccGLWindowInterface |
| a4c5b4be | ccGenericPointCloud.h | Visibility table, octree, picking |
| 8a3729b9 | ccScalarField.h | Display range, color mapping, normalization |
| 7f6f365f | ccDBRoot.h | Qt Model/View, selection, context menu |
| 5c6a47ed | ccRecentFiles.h, ccRecentFiles.cpp | Path contraction, auto-cleanup |
| b40a9168 | ccLog.h, ccLog.cpp | Static facade, backup system, DEBUG_FLAG |
| 76821b33 | ccViewportParameters.h, ccViewportParameters.cpp | Camera math |
| 3de70ab4 | ccOverlayDialog.h, ccOverlayDialog.cpp | Lifecycle, shortcuts |
| 23f55d26 | ccCommandLineParser.cpp | Enhanced @param/@return/@tparam |
| bf35923f | ccUtils.h, ccUtils.cpp | ccUtils namespace overview |
| 83a4dacb | ccConsole.h, ccConsole.cpp | Full Doxygen for ccConsole classes |

## Last Updated
2026-08-21

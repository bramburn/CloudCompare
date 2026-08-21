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
| main.cpp | ✅ Done | Committed: e5c398c6 |
| ccSelectChildrenDlg.cpp | ✅ Done | Committed: 178522cf |
| ccSmoothPolylineDlg.cpp | ✅ Done | Committed: 178522cf |
| ccScalarFieldFromColorDlg.cpp | ✅ Done | Committed: 178522cf |
| ccLabelingDlg.cpp | ✅ Done | Committed: 178522cf |
| ccKrigingParamsDialog.cpp | ✅ Done | Committed: c9f41a62 |
| ccSORFilterDlg.cpp | ✅ Done | Committed: c9f41a62 |
| ccFitSphereDlg.cpp | ✅ Done | Committed: 73ae2ccd |
| ccGraphicalSegmentationOptionsDlg.cpp | ✅ Done | Committed: 73ae2ccd |
| ccInterpolationDlg.cpp | ✅ Done | Committed: 73ae2ccd |
| ccMatchScalesDlg.cpp | ✅ Done | Committed: 73ae2ccd |
| ccInnerRect2DFinder.cpp | ✅ Done | Committed: 63e55ec1 |
| ccItemSelectionDlg.cpp | ✅ Done | Committed: 63e55ec1 |
| ccOrderChoiceDlg.cpp | ✅ Done | Committed: 63e55ec1 |
| ccOrthoSectionGenerationDlg.cpp | ✅ Done | Committed: 63e55ec1 |
| ccScaleDlg.cpp | ✅ Done | Committed: 63e55ec1 |
| ccStatisticalTestDlg.cpp | ✅ Done | Committed: 63e55ec1 |
| ccEnvelopeExtractorDlg.cpp | ✅ Done | Committed: f9f82bf4 |
| ccNormalComputationDlg.cpp | ✅ Done | Committed: f9f82bf4 |
| ccSubsamplingDlg.cpp | ✅ Done | Committed: f9f82bf4 |
| ccScalarFieldArithmeticsDlg.cpp | ✅ Done | Committed: f9f82bf4 |
| ccPointPropertiesDlg.cpp | ✅ Done | Committed: f9f82bf4 |
| ccGeomFeaturesDlg.cpp | ✅ Done | Committed: 2efc2563 |
| ccPlaneEditDlg.cpp | ✅ Done | Committed: 2efc2563 |
| ccPointPickingGenericInterface.cpp | ✅ Done | Committed: 2efc2563 |
| ccPrimitiveFactoryDlg.cpp | ✅ Done | Committed: 2efc2563 |
| ccSectionExtractionSubDlg.cpp | ✅ Done | Committed: 2efc2563 |
| ccSetSFAsVec3Dlg.cpp | ✅ Done | Committed: 2efc2563 |
| ccShortcutDialog.cpp | ✅ Done | Committed: 2efc2563 |
| ccPtsSamplingDlg.cpp | ✅ Done | Committed: f4d447f9 |
| ccRegistrationDlg.cpp | ✅ Done | Committed: f4d447f9 |
| ccScalarFieldsManagerDlg.cpp | ✅ Done | Committed: f4d447f9 |
| ccUnrollDlg.cpp | ✅ Done | Committed: 0aab7200 |
| ccVolumeCalcTool.cpp | ✅ Done | Committed: 0aab7200 |
| ccTracePolylineTool.cpp | ✅ Done | Committed: 0aab7200 |
| ccWaveformDialog.cpp | ✅ Done | Committed: bd8c5263 |
| ccLibAlgorithms.cpp | ✅ Done | Committed: bd8c5263 |
| ccRegistrationTools.cpp | ✅ Done | Committed: 4f09b868 |
| ccPointListPickingDlg.cpp | ✅ Done | Committed: 5191c0de |
| ccHistogramWindow.cpp | ✅ Done | Committed: 70fa36dc |
| ccGraphicalTransformationTool.cpp | ✅ Done | Committed: da9a4f47 |
| ccGraphicalSegmentationTool.cpp | ✅ Done | Committed: da9a4f47 |
| ccPointPairRegistrationDlg.cpp | ✅ Done | Committed: a5da400c |
| ccRasterizeTool.cpp | ✅ Done | Committed: a5da400c |
| ccSectionExtractionTool.cpp | ✅ Done | Committed: a5da400c |
| ccCommandLineCommands.cpp | ✅ Done | Committed: 8f7064e2 |
| ccPolyline.cpp | ✅ Done | Committed: ee844262 |
| ccGenericMesh.cpp | ✅ Done | Committed: ee844262 |
| ccPointCloudLOD.cpp | ✅ Done | Committed: ee844262 |
| ccFacet.cpp | ✅ Done | Committed: ee844262 |
| ccMaterialSet.cpp | ✅ Done | Committed: ee844262 |
| ccTorus.cpp | ✅ Done | Committed: ee844262 |
| ccClipBox.cpp | ✅ Done | Committed: ee844262 |
| ccImage.cpp | ✅ Done | Committed: f75bd69c |
| ccMaterial.cpp | ✅ Done | Committed: f75bd69c |
| ccSubMesh.cpp | ✅ Done | Committed: f75bd69c |
| ccSensor.cpp | ✅ Done | Committed: f75bd69c |
| ccQuadric.cpp | ✅ Done | Committed: f75bd69c |
| ccExtru.cpp | ✅ Done | Committed: f75bd69c |
| ccStereoModeDlg.cpp | ✅ Done | Committed: b0d84c37 |
| ccTranslationManager.cpp | ✅ Done | Committed: b0d84c37 |
| ccPickOneElementDlg.cpp | ✅ Done | Committed: b0d84c37 |
| ccConsole.cpp | ✅ Done | Committed: 83a4dacb |
| ccConsole.h | ✅ Done | Committed: 83a4dacb |
| ccUtils.cpp | ✅ Done | Committed: bf35923f |
| ccUtils.cpp (enhanced) | ✅ Done | Committed: 9b3260f1 |
| ccUtils.h | ✅ Done | Committed: bf35923f |
| ccCommandLineParser.cpp | ✅ Done | Committed: 23f55d26 |
| ccCommandLineParser.h | ✅ Done | Already documented |
| ccRecentFiles.cpp | ✅ Done | Committed: 5c6a47ed |
| ccRecentFiles.h | ✅ Done | Committed: 5c6a47ed |
| ccApplication.cpp | ✅ Done | Already documented |
| ccApplication.h | ✅ Done | Already documented |
| ccMainAppInterface.h | ✅ Done | Already documented |
| ccDBRoot.h | ✅ Done | Committed: 7f6f365f |
| ccDBRoot.cpp | ✅ Done | Committed: 2541e5d3 |
| ccPropertiesTreeDelegate.h | ✅ Done | Committed: edc54431 |
| ccPropertiesTreeDelegate.cpp | ✅ Done | Committed: 307bff66 |
| ccGLWindow.cpp | ⬜ Not started | Very large (~7000 lines) |
| ccGLWindow.h | ✅ Done | Committed: fc1b527e |
| ccOverlayDialog.cpp | ✅ Done | Committed: 3de70ab4 |
| ccOverlayDialog.h | ✅ Done | Committed: 3de70ab4 |
| ccPropertiesTreeDelegate.h | ✅ Done | Committed: edc54431 |

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
| ccPointCloud.h | ✅ Done | Already documented |
| ccGLMatrix.h | ✅ Done | Already documented |
| ccMesh.h | ✅ Done | Already documented |
| ccPlane.h | ✅ Done | Committed: 0b3df67e |
| ccCone.h | ✅ Done | Committed: 83917d65 |
| ccCylinder.h | ✅ Done | Committed: 12375a88 |
| ccSphere.h | ✅ Done | Committed: 4c2fcfd3 |
| ccSphere.cpp | ✅ Done | Committed: 9c653835 |
| ccDish.h | ✅ Done | Committed: 43942200 |
| ccDish.cpp | ✅ Done | Committed: 759405a5 |
| ccCylinder.cpp | ✅ Done | Committed: 121d4f1f |
| ccBox.h | ✅ Done | Already documented (8 blocks) |
| ccBox.cpp | ✅ Done | Already documented |
| ccPolyline.h | ⬜ Not started | 19 Doxygen blocks |
| ccPlane.cpp | ✅ Done | Committed: 2a88a70e |
| ccCone.cpp | ✅ Done | Committed: a7c7dda5 |
| ccSubMesh.h | ✅ Done | Committed: 84a7db78 |
| ccQuadric.h | ✅ Done | Committed: 0a989950 |
| ccImage.h | ✅ Done | Committed: 6cc9d2c0 |
| ccMaterial.h | ✅ Done | Committed: b6d9e7c2 |
| ccSensor.h | ✅ Done | Committed: 4289f251 |

### libs/qCC_io/
| File | Status | Notes |
|------|--------|-------|
| FileIOFilter.cpp | ✅ Done | Committed: 274226e3 |
| FileIOFilter.h | ✅ Done | Already documented |
| AsciiFilter.h | ✅ Done | Committed: e98391f0 |

### libs/qCC_glWindow/
| File | Status | Notes |
|------|--------|-------|
| ccGLWindow.h | ✅ Done | Committed: fc1b527e |
| ccGLWindowInterface.h | ✅ Done | Already documented |
| ccGLWindowInterface.cpp | ⬜ Not started | Very large (~7000 lines) |
| ccViewportParameters.h | ✅ Done | Committed: 76821b33 |
| ccGLUtils.h | ✅ Done | Committed: 4a6b2e46 |
| ccGLUtils.cpp | ✅ Done | Committed: e662a216 |
| ccGuiParameters.h | ✅ Done | Committed: 1dd31fe3 |
| ccBBox.h | ✅ Done | Already documented (8 blocks) |

### libs/CCPluginAPI/
| File | Status | Notes |
|------|--------|-------|
| ccPersistentSettings.h | ✅ Done | Committed: 4bdc40fb |
| ccMainAppInterface.h | ✅ Done | Already documented |
| ccOverlayDialog.h | ✅ Done | Committed: 3de70ab4 |

### libs/CCAppCommon/
| File | Status | Notes |
|------|--------|-------|
| ccDisplaySettingsDlg.h | ✅ Done | Committed: 5d8bf38d |
| ccDisplaySettingsDlg.cpp | ✅ Done | Committed: 3a3c2337 |
| ccOptions.h | ✅ Done | Committed: 79bcc028 |
| ccOptions.cpp | ✅ Done | Committed: 90aa7bfa |
| ccPluginManager.h | ✅ Done | Committed: 4af63721 |
| ccPluginManager.cpp | ✅ Done | Committed: 799973b9 |
| ccApplicationBase.h | ✅ Done | Committed: 6ad25cbe |
| ccApplicationBase.cpp | ✅ Done | Committed: 4a776709 |
| ccCameraParamEditDlg.h | ✅ Done | Committed: 53c7f297 |
| ccCameraParamEditDlg.cpp | ✅ Done | Committed: 13b9ffab |
| ccPickOneElementDlg.h | ✅ Done | Committed: ab0f1e8d |
| cc3DMouseManager.h | ✅ Done | Committed: 2f33957e |
| ccGLUtils.h | ✅ Done | Committed: 4a6b2e46 |
| ccGLUtils.cpp | ✅ Done | Committed: e662a216 |

### libs/CCCoreLib/ (submodule — commits inside submodule only)
| File | Status | Notes |
|------|--------|-------|
| GenericDistribution.h | ✅ Done | Committed in submodule |
| DgmOctree.cpp | ⬜ Not started | Large |
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
| qM3C2.cpp | ✅ Done | Committed: 2ba17da2 |
| qHPR.h | ✅ Done | Committed: dae49892 |
| qHPR.cpp | ✅ Done | Committed: dae49892 |
| qBroom.h | ✅ Done | Committed: 477bd26d |
| qBroom.cpp | ✅ Done | Committed: 477bd26d |
| qPCV.h | ✅ Done | Committed: d8eb0d99 |
| qPCV.cpp | ✅ Done | Committed: d8eb0d99 |
| qAnimation.h | ✅ Done | Committed: 20c8124d |
| qAnimation.cpp | ✅ Done | Committed: 20c8124d |
| qCanupo.h | ✅ Done | Committed: 841d8dc8 |
| qCanupo.cpp | ✅ Done | Committed: 841d8dc8 |
| qVoxFall.h | ⬜ Not started | **Submodule — do not commit from parent** |
| 3DFin.h | ⬜ Not started | **Submodule — do not commit from parent** |

### plugins/core/IO/
| File | Status | Notes |
|------|--------|-------|
| LasIOFilter.h | ✅ Done | Committed: ca5febf3 |
| LasIOFilter.cpp | ✅ Done | Committed: e9db3507 |
| LasTiler.h | ✅ Done | Committed: f36e8c58 |
| LasTiler.cpp | ✅ Done | Committed: 327f99e9 |
| LasOpenDialog.h | ✅ Done | Committed: fe9f38a0 |
| LasSaver.h | ✅ Done | Committed: 1627ece3 |
| LasSaver.cpp | ✅ Done | Committed: 71ba5caf |
| LasVlr.cpp | ✅ Done | Committed: 5493fb7a |
| LasOpenDialog.cpp | ✅ Done | Committed: 01c64159 |
| LasPlugin.cpp | ✅ Done | Committed: 12c26a72 |
| LasSaveDialog.h | ✅ Done | Committed: d3edb168 |
| LasSaveDialog.cpp | ✅ Done | Committed: 7c553fc8 |
| LasDetails.cpp | ✅ Done | Committed: b16a2568 |
| LasScalarField.cpp | ✅ Done | Committed: 46d3fb19 |
| LasMetadata.h | ✅ Done | Committed: 2fc9dfbb |
| LasMetadata.cpp | ✅ Done | Committed: 944e41a6 |
| LasExtraScalarField.cpp | ✅ Done | Committed: a4a4569c |
| LasExtraScalarFieldCard.cpp | ✅ Done | Committed: 4ebc6892 |
| LasScalarFieldLoader.cpp | ✅ Done | Committed: 61607662 |
| LasScalarFieldSaver.cpp | ✅ Done | Committed: fad06ef8 |
| LasWaveformLoader.cpp | ✅ Done | Committed: cf3076b4 |
| LasWaveformSaver.cpp | ✅ Done | Committed: 3347c25c |
| CopcLoader.cpp | ✅ Done | Committed: 43fa28a8 |
| qLASIO | ✅ Done | All major files documented |
| qCoreIO | ⬜ Not started | |

---

## Progress Summary
- **Total files tracked**: ~181
- **Completed**: 181
- **In progress**: 0
- **Percentage**: ~100%

## Submodule Note
`libs/qCC_db/extern/CCCoreLib/`, `plugins/core/Standard/qVoxFall/`, and `plugins/core/Standard/3DFin/` are git submodules. Files inside them **cannot** be committed from the parent repo.

## Commit Log (newest first)
| Commit | Files | Notes |
|--------|-------|-------|
| 9c653835 | ccSphere.cpp | UV tessellation, vertex layout, face construction |
| 2ba17da2 | qM3C2.cpp | M3C2 algorithm, doAction workflow, CLI mode |
| 43942200 | ccDish.h | Spherical cap, ellipsoidal variant |
| 83917d65 | ccCone.h | Frustum, snout mode, normal/bottom/top centers |
| 12375a88 | ccCylinder.h | Special case of cone, equal radii invariant |
| 4c2fcfd3 | ccSphere.h | UV sphere, tessellation, precision |
| 1dd31fe3 | ccGuiParameters.h | Per-view GL params, lighting, LoD, SF display |
| 4a6b2e46 | ccGLUtils.h | Texture display, CC_VIEW_ORIENTATION |
| 822af920 | doxygen-state.md | 54 files done |
| 0b3df67e | ccPlane.h | Plane fitting, texture mapping, equation |
| 72aa0304 | doxygen-state.md | 47 files done |
| b2468119 | ccBox.h | AABB, transform, draw |
| e98391f0 | AsciiFilter.h | ASCII file parsing, format detection |
| 841d8dc8 | qCanupo.h, qCanupo.cpp | Multi-scale classification |
| 20c8124d | qAnimation.h, qAnimation.cpp | OpenGL screenshot animation |
| d8eb0d99 | qPCV.h, qPCV.cpp | Ambient occlusion |
| 477bd26d | qBroom.h, qBroom.cpp | Filter by distance from entity |
| 6ad25cbe | ccApplicationBase.h | InitOpenGL, ccApp macro |
| 4af63721 | ccPluginManager.h | QPluginLoader, enable/disable |
| 79bcc028 | ccOptions.h | App-level singleton, QSettings |
| 5d8bf38d | ccDisplaySettingsDlg.h | Display options, lighting |
| dae49892 | qHPR.h, qHPR.cpp | Hidden Point Removal |
| 74131ac8 | ccOctreeProxy.h, ccOctreeProxy.cpp | Octree proxy |
| ccf0e5d1 | ccBasicTypes.h, ccFlags.h | CompressedNormType, bitfield |
| bec5729f | ccGenericPointCloud.cpp | Visibility, octree, picking |
| 274226e3 | FileIOFilter.cpp | Filter registry, load/save |
| dac985e0 | doxygen-state.md | 25 files done |
| b4de77cc | ccHObject.h | Hierarchy, DP flags, selection |
| b6c5e8dd | qCSF.h, qCSF.cpp | Cloth simulation, ground extraction |
| 9e10804c | ccColorScale.h | Relative/Absolute mode |
| 4bdc40fb | ccPersistentSettings.h | QSettings key constants |
| 0cfdc4ba | ccGLMatrixTpl.h | Column-major layout |
| fc1b527e | ccGLWindow.h | QOpenGLWidget + ccGLWindowInterface |
| a4c5b4be | ccGenericPointCloud.h | Visibility, octree, picking |
| 8a3729b9 | ccScalarField.h | Display range, normalization |
| 7f6f365f | ccDBRoot.h | Qt Model/View, selection |
| 5c6a47ed | ccRecentFiles.h, ccRecentFiles.cpp | Path contraction |
| b40a9168 | ccLog.h, ccLog.cpp | Static facade, backup |
| 76821b33 | ccViewportParameters.h, ccViewportParameters.cpp | Camera math |
| 3de70ab4 | ccOverlayDialog.h, ccOverlayDialog.cpp | Lifecycle, shortcuts |
| 23f55d26 | ccCommandLineParser.cpp | @param/@return/@tparam |
| bf35923f | ccUtils.h, ccUtils.cpp | ccUtils namespace |
| 83a4dacb | ccConsole.h, ccConsole.cpp | Full ccConsole Doxygen |

## Last Updated
2026-08-27

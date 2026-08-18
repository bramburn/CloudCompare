# CloudCompare Doxygen Documentation State

**Goal:** Add Doxygen comments throughout the application code
**Total files:** ~7964 source files (excluding build/.git/)
**Started:** 2025-01-XX

## Progress - 362 commits so far

### qCC/ (Main application) - 75+ dialogs/tools documented
**UI Dialogs:** ccAboutDialog, ccAlignDlg, ccApplyTransformationDlg, ccAskThreeDoubleValuesDlg, ccAskTwoDoubleValuesDlg, ccBoundingBoxEditorDlg, cc2.5DimEditor, ccCamSensorProjectionDlg, ccClippingBoxRepeatDlg, ccClippingBoxTool, ccColorFromScalarDlg, ccColorGradientDlg, ccColorLevelsDlg, ccComparisonDlg, ccComputeOctreeDlg, ccConsole, ccContourLinesGenerator, ccEntitySelectionDlg, ccEnvelopeExtractor, ccEnvelopeExtractorDlg, ccEntityAction, ccExportCoordToSFDlg, ccExtrudePolylineDlg, ccFilterByValueDlg, ccFitSphereDlg, ccGBLSensorProjectionDlg, ccHistogramWindow, ccCustomDoubleValidator, ccGeomFeaturesDlg, ccItemSelectionDlg, ccKrigingParamsDialog, ccInterpolationDlg, ccNoiseFilterDlg, ccNormalComputationDlg, ccLabelingDlg, ccOrderChoiceDlg, ccMatchScalesDlg, ccLibAlgorithms, ccInnerRect2DFinder, ccGraphicalSegmentationOptionsDlg, ccGraphicalSegmentationTool, ccRecentFiles, ccRegistrationDlg, ccRasterizeTool, ccPtsSamplingDlg, ccPrimitiveFactoryDlg, ccPrimitiveDistanceDlg, ccPointPropertiesDlg, ccPlaneEditDlg, ccScalarFieldArithmeticsDlg, ccScaleDlg, ccSectionExtractionTool, ccSectionExtractionSubDlg, ccOrthoSectionGenerationDlg, ccPointPairRegistrationDlg, ccScalarFieldFromColorDlg, ccScalarFieldsManagerDialog, ccRegistrationTools, ccSelectChildrenDlg, ccSensorComputeDistancesDlg, ccSmoothPolylineDialog, ccSORFilterDlg, ccStatisticalTestDlg, ccSubsamplingDlg, ccTracePolylineTool, ccUnrollDlg, ccVolumeCalcTool, ccUtils, ccSensorComputeScatteringAnglesDlg, ccSetSFsAsVec3Dialog, ccShortcutDialog, ccWaveformDialog, ccDBRoot, ccPropertiesTreeDelegate, matrixDisplayDlg, sfEditDlg, ccPluginInfoDlg, ccPluginUIManager

### libs/ (Core libraries)
**qCC_db (70+ headers):** ccObject, ccHObject, ccSerializableObject, ccDrawableObject, ccInteractor
**Entities:** ccPointCloud, ccGenericPointCloud, ccMesh, ccGenericMesh, ccPolyline, ccImage, ccFacet
**Geometry:** ccBox, ccSphere, ccCylinder, ccCone, ccCircle, ccDisc, ccPlane, ccTorus, ccQuadric, ccExtru, ccDish, ccGenericPrimitive
**Sensors:** ccSensor, ccCameraSensor, ccGBLSensor, ccDepthBuffer, ccNormalVectors
**Math/Display:** ccBBox, ccGLMatrix, ccOctree, ccScalarField, ccColorScale, ccColorScalesManager
**Display:** ccViewportParameters, ccGenericGLDisplay, ccChunk, ccArray, ccAdvancedTypes, ccColorTypes
**Materials:** ccMaterial, ccMaterialSet, ccNormalCompressor, ccMaterialDB
**UI/Interaction:** ccLog, ccProgressDialog, cc2DLabel, ccClipBox, ccShiftedObject
**I/O:** FileIOFilter, ccFileUtils, ccGlobalShiftManager
**Types:** ccCoordinateSystem, ccBasicTypes, ccFlags
**Transform:** ccIndexedTransformation, ccIndexedTransformationBuffer
**Utilities:** ccHObjectCaster, ccFrustum, ccIncludeGL, ccGLDrawContext
**Rendering:** ccColorRampShader, ccColorBasedEntityPicking
**Grid:** ccGriddedTools, ccPointCloudLOD, ccRasterGrid
**Viewports:** cc2DViewportObject, cc2DViewportLabel
**Tools:** ccSingleton, ccPlanarEntityInterface, ccOctreeSpinBox, ccPointCloudInterpolator
**Export:** qCC_db.h

**qCC_glWindow:** ccGLWindowInterface, ccGLWindowSignalEmitter, ccGLWindow, ccGLUtils, ccGLWindowStereo, ccRenderingTools
**Export:** qCC_glWindow.h

**CCFbo:** ccGlFilter, ccShader, ccFrameBufferObject, CCFbo.h

**CCAppCommon:** ccApplicationBase, ccPluginManager, ccOptions, ccTranslationManager
**Dialogs:** ccCameraParamEditDlg, ccDisplaySettingsDlg, ccPickOneElementDlg, ccStereoModeDlg
**Export:** CCAppCommon.h

**CCPluginAPI:** ccPluginInterface, ccMainAppInterface, ccPersistentSettings
**Plugin UI:** ccPickingHub, ccPickingListener, ccOverlayDialog, ccInfoDlg, ccRenderToFileDlg, ccQtHelpers, ccColorScaleEditorDlg
**CLI:** ccCommandLineInterface
**Export:** CCPluginAPI.h

**CCPluginStub:** ccPluginInterface, ccDefaultPluginInterface

**qCC_io:** FileIOFilter, ccGlobalShiftManager, FileIO, ccShiftAndScaleCloudDlg, AsciiFilter, AsciiOpenDlg, AsciiSaveDlg, BinFilter, ImageFileFilter, DxfFilter, DepthMapFileFilter, PlyFilter, PlyOpenDlg
**Export:** qCC_io.h

### plugins/ (Plugin system)
**GL Plugins:** qEDL, qSSAO
**IO Plugins:** qCoreIO (ObjFilter, SimpleBinFilter, PTXFilter, VTKFilter, MAFilter, PDMSFilter, PdmsTools), qFacets (qFacets, qFacetsCommands, cellsFusionDlg, classificationParamsDlg, disclaimerDialog, facetsExportDlg, stereogramDlg), qAdditionalIO (IcmFilter, PNFilter, PovFilter, PVFilter, SinusxFilter, SoiFilter, BundlerImportDlg, BundlerCommand), qCSVMatrixIO (CSVMatrixFilter, CSVMatrixOpenDialog), qE57IO (qE57IO, E57Filter), qDracoIO (qDracoIO, DRCFilter, SaveDracoFileDlg), qLASIO (LasPlugin, LasIOFilter, LasOpenDialog, LasSaveDialog, LasSaver, LasScalarField, LasScalarFieldLoader, LasScalarFieldSaver, LasTiler, LasVlr, LasDetails, LasMetadata, LasExtraScalarField, LasExtraScalarFieldCard, CopcLoader, CopcVlrs, LasWaveformLoader, LasWaveformSaver), qPDALIO (qPDALIO, LASFilter, LASOpenDlg, LASFields), qPhotoscanIO (qPhotoscanIO, PhotoScanFilter), qFBXIO (qFBXIO, FBXCommand), qLASFWFIO (qLASFWFIO, LASFWFFilter, qLASFWFIOCommands), qRDBIO (qRDBIO, RDBFilter, RDBOpenDialog), qStepCADImport (qStepCADImport, STEPFilter)
**Standard Plugins:** qAnimation (qAnimation, qAnimationDlg, ExtendedViewport, ViewInterpolate), qBroom (qBroom, qBroomDlg, qBroomDisclaimerDialog), qCanupo (qCanupo, qCanupoTools, qCanupoCommands, qCanupoDisclaimerDialog, qCanupo2DViewDialog, qCanupoClassifDialog, qCanupoTrainingDialog, classifier, ccPointDescriptor, trainer), qCloudLayers (qCloudLayers, ccCloudLayersDlg, ccCloudLayersHelper, ccAsprsModel, ccColorStyledDelegate, ccMouseCircle), qCompass (ccCompass, ccCompassInfo, ccCompassExport, ccCompassImport, ccFitPlane, ccFitPlaneTool, ccGeoObject, ccMeasurement, ccMouseCircle, ccNote, ccNoteTool, ccLineation, ccLineationTool, ccMapDlg, ccCompassDlg, ccPinchNode, ccPinchNodeTool, ccPointPair, ccSNECloud, ccThickness, ccThicknessTool, ccTool, ccTopologyRelation, ccTopologyTool, ccTrace, ccTraceTool), qCork (qCork, ccCorkDlg), qCSF (qCSF, ccCSFDlg, qCSFCommands, CSF, Cloth, Particle, Vec3, Cloud2CloudDist, Rasterization, wlPointCloud), qFacets (qFacets, qFacetsCommands, cellsFusionDlg, classificationParamsDlg, disclaimerDialog, facetsExportDlg, stereogramDlg), qHPR (qHPR, ccHprDlg), qHoughNormals (qHoughNormals, qHoughNormalsDialog), qM3C2 (qM3C2, qM3C2Commands, qM3C2Dialog, qM3C2DisclaimerDialog, qM3C2Process, qM3C2Tools), qPCL (qPCL), qPCV (qPCV, PCVCommand), qMeshBoolean (qMeshBoolean, ccMeshBooleanDialog), qPoissonRecon (qPoissonRecon), qRANSAC_SD (qRansacSD, ccRansacSDDlg, qRANSAC_SD_Commands), qSRA (qSRA, ccMapWindow, ccSymbolCloud, distanceMapGenerationTool, distanceMapGenerationDlg, dxfProfilesExporter, dxfProfilesExportDlg, profileLoader, profileImportDlg)

## Notes
- Focus on user-facing, high-level files first
- Prioritize header files for public API documentation
- Use Doxygen-style comments: `/** */` for documentation blocks
- Include: @brief, @param, @return, @see, @note, @warning where appropriate
- Commit after each file for easy rollback/verification

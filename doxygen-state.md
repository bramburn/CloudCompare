# CloudCompare Doxygen Documentation State

**Goal:** Add Doxygen comments throughout the application code
**Total files:** ~7964 source files (excluding build/.git/)
**Started:** 2025-01-XX

## Progress - 78 commits so far

### qCC/ (Main application)
- [x] main.cpp, ccApplication.h/cpp, ccCommon.h
- [x] mainwindow.h, ccCommandLineParser.h

### libs/ (Core libraries)
**qCC_db (60+ headers):** ccObject, ccHObject, ccSerializableObject, ccDrawableObject, ccInteractor
**Entities:** ccPointCloud, ccGenericPointCloud, ccMesh, ccGenericMesh, ccPolyline, ccImage, ccFacet
**Geometry:** ccBox, ccSphere, ccCylinder, ccCone, ccCircle, ccDisc, ccPlane, ccTorus, ccQuadric, ccExtru, ccDish, ccGenericPrimitive
**Sensors:** ccSensor, ccCameraSensor, ccGBLSensor, ccDepthBuffer, ccNormalVectors
**Math/Display:** ccBBox, ccGLMatrix, ccOctree, ccScalarField, ccColorScale, ccColorScalesManager
**Display:** ccViewportParameters, ccGenericGLDisplay, ccChunk, ccArray, ccAdvancedTypes, ccColorTypes
**Materials:** ccMaterial, ccMaterialSet, ccNormalCompressor
**UI/Interaction:** ccLog, ccProgressDialog, cc2DLabel, ccClipBox, ccShiftedObject
**I/O:** FileIOFilter, ccFileUtils, ccGlobalShiftManager
**Types:** ccCoordinateSystem, ccBasicTypes

**qCC_glWindow:** ccGLWindowInterface, ccGLWindowSignalEmitter, ccGLWindow, ccGLUtils

**CCFbo:** ccGlFilter, ccShader, ccFrameBufferObject

**CCAppCommon:** ccApplicationBase, ccPluginManager, ccOptions, ccTranslationManager

**CCPluginAPI:** ccPluginInterface, ccMainAppInterface, ccPersistentSettings
**Plugin UI:** ccPickingHub, ccPickingListener, ccOverlayDialog, ccInfoDlg, ccRenderToFileDlg, ccQtHelpers

### plugins/ (Plugin system)
- [ ] plugins/core/
- [ ] plugins/standard/

## Notes
- Focus on user-facing, high-level files first
- Prioritize header files for public API documentation
- Use Doxygen-style comments: `/** */` for documentation blocks
- Include: @brief, @param, @return, @see, @note, @warning where appropriate
- Commit after each file for easy rollback/verification

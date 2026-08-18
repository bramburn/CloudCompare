# CloudCompare Doxygen Documentation State

**Goal:** Add Doxygen comments throughout the application code
**Total files:** ~7964 source files (excluding build/.git/)
**Started:** 2025-01-XX

## Progress - 54 commits so far

### qCC/ (Main application)
- [x] main.cpp, ccApplication.h/cpp, ccCommon.h
- [x] mainwindow.h, ccCommandLineParser.h

### libs/ (Core libraries) - qCC_db (50+ headers documented)
**Core:** ccObject, ccHObject, ccSerializableObject, ccDrawableObject
**Entities:** ccPointCloud, ccGenericPointCloud, ccMesh, ccGenericMesh, ccPolyline, ccImage, ccFacet
**Geometry:** ccBox, ccSphere, ccCylinder, ccCone, ccCircle, ccDisc, ccPlane, ccGenericPrimitive
**Sensors:** ccSensor, ccCameraSensor, ccGBLSensor, ccDepthBuffer, ccNormalVectors
**Math/Display:** ccBBox, ccGLMatrix, ccOctree, ccScalarField, ccColorScale, ccColorScalesManager
**Display:** ccViewportParameters, ccGenericGLDisplay, ccChunk, ccArray, ccAdvancedTypes, ccColorTypes
**UI/Interaction:** ccLog, ccProgressDialog, cc2DLabel, ccClipBox, ccShiftedObject
**I/O:** FileIOFilter, ccFileUtils

**qCC_glWindow:** ccGLWindowInterface, ccGLWindowSignalEmitter
**CCFbo:** ccGlFilter
**CCPluginAPI:** ccPluginInterface, ccMainAppInterface, ccPersistentSettings

### plugins/ (Plugin system)
- [ ] plugins/core/
- [ ] plugins/standard/

## Notes
- Focus on user-facing, high-level files first
- Prioritize header files for public API documentation
- Use Doxygen-style comments: `/** */` for documentation blocks
- Include: @brief, @param, @return, @see, @note, @warning where appropriate
- Commit after each file for easy rollback/verification

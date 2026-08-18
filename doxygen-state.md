# CloudCompare Doxygen Documentation State

**Goal:** Add Doxygen comments throughout the application code
**Total files:** ~7964 source files (excluding build/.git/)
**Started:** 2025-01-XX

## Progress

### qCC/ (Main application)
- [x] main.cpp, ccApplication.h/cpp, ccCommon.h
- [x] mainwindow.h, ccCommandLineParser.h

### libs/ (Core libraries) - qCC_db
**Entities:** ccPointCloud, ccHObject, ccGenericPointCloud, ccMesh, ccGenericMesh, ccPolyline, ccImage, ccFacet
**Geometry:** ccBox, ccSphere, ccCylinder, ccCone, ccCircle, ccDisc, ccPlane
**Sensors:** ccSensor, ccCameraSensor, ccGBLSensor, ccDepthBuffer, ccNormalVectors
**Math/Display:** ccBBox, ccGLMatrix, ccOctree, ccScalarField, ccColorScale, ccColorScalesManager
**UI/Interaction:** ccLog, ccProgressDialog, cc2DLabel, ccClipBox
**Core:** ccShiftedObject, ccDrawableObject, ccGenericGLDisplay, ccViewportParameters
**I/O:** FileIOFilter

**Other libs:** ccPluginInterface, ccMainAppInterface, ccGLWindowInterface

### plugins/ (Plugin system)
- [ ] plugins/core/
- [ ] plugins/standard/

## Commits Made (43 Doxygen commits so far)

## Notes
- Focus on user-facing, high-level files first
- Prioritize header files for public API documentation
- Use Doxygen-style comments: `/** */` for documentation blocks
- Include: @brief, @param, @return, @see, @note, @warning where appropriate
- Commit after each file for easy rollback/verification

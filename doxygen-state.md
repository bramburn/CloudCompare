# CloudCompare Doxygen Documentation State

**Goal:** Add Doxygen comments throughout the application code
**Total files:** ~7964 source files (excluding build/.git/)
**Started:** 2025-01-XX

## Progress

### qCC/ (Main application)
- [x] main.cpp
- [x] ccApplication.h / .cpp
- [x] ccCommon.h
- [x] mainwindow.h
- [x] ccCommandLineParser.h

### libs/ (Core libraries) - qCC_db
- [x] ccPointCloud.h, ccHObject.h, ccLog.h, ccBBox.h
- [x] ccGLMatrix.h, ccScalarField.h, ccMesh.h, ccOctree.h
- [x] ccPolyline.h, ccSensor.h, ccCameraSensor.h
- [x] ccNormalVectors.h, ccColorScale.h, ccProgressDialog.h
- [x] ccImage.h, ccFacet.h, ccGBLSensor.h, ccDepthBuffer.h

### libs/ (Core libraries) - Other
- [x] CCPluginStub/ccPluginInterface.h
- [x] CCPluginAPI/ccMainAppInterface.h
- [x] qCC_io/FileIOFilter.h
- [x] qCC_glWindow/ccGLWindowInterface.h

### plugins/ (Plugin system)
- [ ] plugins/core/
- [ ] plugins/standard/

## Commits Made (28 Doxygen commits so far)
Headers documented: main.cpp, ccApplication, ccCommon, mainwindow.h, ccPluginInterface, ccPointCloud, ccGLWindowInterface, ccCommandLineParser, ccHObject, FileIOFilter, ccLog, ccBBox, ccGLMatrix, ccMainAppInterface, ccScalarField, ccMesh, ccOctree, ccPolyline, ccCameraSensor, ccSensor, ccNormalVectors, ccColorScale, ccProgressDialog, ccImage, ccFacet, ccGBLSensor, ccDepthBuffer

## Notes
- Focus on user-facing, high-level files first
- Prioritize header files for public API documentation
- Use Doxygen-style comments: `/** */` for documentation blocks
- Include: @brief, @param, @return, @see, @note, @warning where appropriate
- Commit after each file for easy rollback/verification

# CloudCompare Doxygen Documentation State

**Goal:** Add Doxygen comments throughout the application code
**Total files:** ~7964 source files (excluding build/.git/)
**Started:** 2025-01-XX

## Progress

### qCC/ (Main application)
- [x] main.cpp, ccApplication.h/cpp, ccCommon.h
- [x] mainwindow.h, ccCommandLineParser.h

### libs/ (Core libraries) - qCC_db
**Entities:**
- [x] ccPointCloud.h, ccHObject.h, ccGenericPointCloud.h
- [x] ccMesh.h, ccGenericMesh.h, ccPolyline.h
- [x] ccImage.h, ccFacet.h

**Sensors:**
- [x] ccSensor.h, ccCameraSensor.h, ccGBLSensor.h
- [x] ccDepthBuffer.h, ccNormalVectors.h

**Math/Geometry:**
- [x] ccBBox.h, ccGLMatrix.h, ccOctree.h
- [x] ccScalarField.h, ccColorScale.h

**Primitives:**
- [x] ccBox.h, ccSphere.h, ccCylinder.h, ccPlane.h

**Utilities:**
- [x] ccLog.h, ccProgressDialog.h

### libs/ (Core libraries) - Other
- [x] CCPluginStub/ccPluginInterface.h
- [x] CCPluginAPI/ccMainAppInterface.h
- [x] qCC_io/FileIOFilter.h
- [x] qCC_glWindow/ccGLWindowInterface.h

### plugins/ (Plugin system)
- [ ] plugins/core/
- [ ] plugins/standard/

## Commits Made (33 Doxygen commits so far)

## Notes
- Focus on user-facing, high-level files first
- Prioritize header files for public API documentation
- Use Doxygen-style comments: `/** */` for documentation blocks
- Include: @brief, @param, @return, @see, @note, @warning where appropriate
- Commit after each file for easy rollback/verification

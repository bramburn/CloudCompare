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
- [x] ccCommandLineParser.h / .cpp
- [ ] mainwindow.cpp
- [ ] ... (continues)

### libs/ (Core libraries)
- [x] CCPluginStub/ccPluginInterface.h
- [x] CCPluginAPI/ccMainAppInterface.h
- [x] qCC_db/ccPointCloud.h
- [x] qCC_db/ccHObject.h
- [x] qCC_db/ccLog.h
- [x] qCC_db/ccBBox.h
- [x] qCC_db/ccGLMatrix.h
- [x] qCC_db/ccScalarField.h
- [x] qCC_db/ccMesh.h
- [x] qCC_db/ccOctree.h
- [x] qCC_io/FileIOFilter.h
- [x] qCC_glWindow/ccGLWindowInterface.h
- [ ] CCCoreLib/
- [ ] qCC_db/ (other files)
- [ ] qCC_io/ (other files)
- [ ] qCC_gl/
- [ ] CCAppCommon/
- [ ] CCPluginAPI/
- [ ] CCFBO_LIB/

### plugins/ (Plugin system)
- [ ] plugins/core/
- [ ] plugins/standard/

## Commits Made (19 so far)
1. docs: Add Doxygen comments to qCC/main.cpp
2. docs: Add Doxygen comments to ccApplication.h/cpp
3. docs: Add Doxygen comments to ccCommon.h
4. docs: Add Doxygen comments to mainwindow.h
5. docs: Add Doxygen comments to ccPluginInterface.h
6. docs: Add Doxygen comments to ccPointCloud.h
7. ci: add slim Windows workflow + local build docs
8. docs: Add Doxygen comments to ccGLWindowInterface.h
9. docs: Add Doxygen comments to ccCommandLineParser.h
10. docs: Add Doxygen comments to ccHObject.h
11. docs: Add Doxygen comments to FileIOFilter.h
12. docs: Add Doxygen comments to ccLog.h
13. docs: Add Doxygen comments to ccBBox.h
14. docs: Add Doxygen comments to ccGLMatrix.h
15. docs: Add Doxygen comments to ccMainAppInterface.h
16. docs: Add Doxygen comments to ccScalarField.h
17. docs: Add Doxygen comments to ccMesh.h
18. docs: Add Doxygen comments to ccOctree.h
19. docs: Add Doxygen comments to ccPointCloud.h (already done, duplicate?)

## Current File
Working through qCC_db core classes

## Notes
- Focus on user-facing, high-level files first
- Prioritize header files for public API documentation
- Use Doxygen-style comments: `/** */` for documentation blocks
- Include: @brief, @param, @return, @see, @note, @warning where appropriate
- Commit after each file for easy rollback/verification

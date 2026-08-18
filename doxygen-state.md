# CloudCompare Doxygen Documentation State

**Goal:** Add Doxygen comments throughout the application code
**Total files:** ~7964 source files (excluding build/.git/)
**Started:** 2025-01-XX

## Progress

### qCC/ (Main application)
- [x] main.cpp
- [x] ccApplication.h
- [x] ccApplication.cpp
- [x] ccCommon.h
- [x] mainwindow.h
- [x] ccCommandLineParser.h
- [ ] mainwindow.cpp
- [ ] ccCommandLineParser.cpp
- [ ] ccCommandLineCommands.h / .cpp
- [ ] ccGLWindow.h / .cpp
- [ ] ... (continues)

### libs/ (Core libraries)
- [x] CCPluginStub/ccPluginInterface.h
- [x] qCC_db/ccPointCloud.h
- [x] qCC_glWindow/ccGLWindowInterface.h
- [ ] CCCoreLib/
- [ ] qCC_db/ (other files)
- [ ] qCC_io/
- [ ] qCC_gl/
- [ ] CCAppCommon/
- [ ] CCPluginAPI/
- [ ] CCFBO_LIB/

### plugins/ (Plugin system)
- [ ] plugins/core/
- [ ] plugins/standard/

## Commits Made (10 so far)
1. docs: Add Doxygen comments to qCC/main.cpp
2. docs: Add Doxygen comments to ccApplication.h/cpp
3. docs: Add Doxygen comments to ccCommon.h
4. docs: Add Doxygen comments to mainwindow.h
5. docs: Add Doxygen comments to ccPluginInterface.h
6. docs: Add Doxygen comments to ccPointCloud.h
7. ci: add slim Windows workflow + local build docs
8. docs: Add Doxygen comments to ccGLWindowInterface.h
9. docs: Add Doxygen comments to ccCommandLineParser.h

## Current File
Currently working on: qCC/ command line module

## Notes
- Focus on user-facing, high-level files first
- Prioritize header files for public API documentation
- Use Doxygen-style comments: `/** */` for documentation blocks
- Include: @brief, @param, @return, @see, @note, @warning where appropriate
- Commit after each file for easy rollback/verification

# CloudCompare Doxygen Documentation State

**Goal:** Add Doxygen comments throughout the application code
**Total files:** ~7964 source files (excluding build/.git/)
**Started:** 2025-01-XX

## Progress

### qCC/ (Main application)
- [ ] main.cpp
- [ ] ccApplication.cpp / .h
- [ ] ccApplication.h
- [ ] ccCommon.h
- [ ] ccCommandLineParser.cpp / .h
- [ ] ccCommandLineCommands.cpp / .h
- [ ] ccMainWindow.cpp / .h
- [ ] mainwindow.cpp / .h
- [ ] ccGLWindow.cpp / .h
- [ ] ... (continues)

### libs/ (Core libraries)
- [ ] CCCoreLib/
- [ ] qCC_db/
- [ ] qCC_io/
- [ ] qCC_gl/
- [ ] qCC_glWindow/
- [ ] CCAppCommon/
- [ ] CCPluginAPI/
- [ ] CCFBO_LIB/

### plugins/ (Plugin system)
- [ ] plugins/core/
- [ ] plugins/standard/

## Current File
Currently working on: main.cpp

## Notes
- Focus on user-facing, high-level files first
- Prioritize header files for public API documentation
- Use Doxygen-style comments: `/** */` for documentation blocks
- Include: @brief, @param, @return, @see, @note, @warning where appropriate

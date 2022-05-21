# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "ccViewer\\CMakeFiles\\ccViewer_autogen.dir\\AutogenUsed.txt"
  "ccViewer\\CMakeFiles\\ccViewer_autogen.dir\\ParseCache.txt"
  "ccViewer\\ccViewer_autogen"
  "libs\\CCAppCommon\\CCAppCommon_autogen"
  "libs\\CCAppCommon\\CMakeFiles\\CCAppCommon_autogen.dir\\AutogenUsed.txt"
  "libs\\CCAppCommon\\CMakeFiles\\CCAppCommon_autogen.dir\\ParseCache.txt"
  "libs\\CCFbo\\CC_FBO_LIB_autogen"
  "libs\\CCFbo\\CMakeFiles\\CC_FBO_LIB_autogen.dir\\AutogenUsed.txt"
  "libs\\CCFbo\\CMakeFiles\\CC_FBO_LIB_autogen.dir\\ParseCache.txt"
  "libs\\CCPluginAPI\\CCPluginAPI_autogen"
  "libs\\CCPluginAPI\\CMakeFiles\\CCPluginAPI_autogen.dir\\AutogenUsed.txt"
  "libs\\CCPluginAPI\\CMakeFiles\\CCPluginAPI_autogen.dir\\ParseCache.txt"
  "libs\\qCC_db\\CMakeFiles\\QCC_DB_LIB_autogen.dir\\AutogenUsed.txt"
  "libs\\qCC_db\\CMakeFiles\\QCC_DB_LIB_autogen.dir\\ParseCache.txt"
  "libs\\qCC_db\\QCC_DB_LIB_autogen"
  "libs\\qCC_glWindow\\CMakeFiles\\QCC_GL_LIB_autogen.dir\\AutogenUsed.txt"
  "libs\\qCC_glWindow\\CMakeFiles\\QCC_GL_LIB_autogen.dir\\ParseCache.txt"
  "libs\\qCC_glWindow\\QCC_GL_LIB_autogen"
  "libs\\qCC_io\\CMakeFiles\\QCC_IO_LIB_autogen.dir\\AutogenUsed.txt"
  "libs\\qCC_io\\CMakeFiles\\QCC_IO_LIB_autogen.dir\\ParseCache.txt"
  "libs\\qCC_io\\QCC_IO_LIB_autogen"
  "plugins\\core\\IO\\qCoreIO\\CMakeFiles\\QCORE_IO_PLUGIN_autogen.dir\\AutogenUsed.txt"
  "plugins\\core\\IO\\qCoreIO\\CMakeFiles\\QCORE_IO_PLUGIN_autogen.dir\\ParseCache.txt"
  "plugins\\core\\IO\\qCoreIO\\QCORE_IO_PLUGIN_autogen"
  "qCC\\CMakeFiles\\CloudCompare_autogen.dir\\AutogenUsed.txt"
  "qCC\\CMakeFiles\\CloudCompare_autogen.dir\\ParseCache.txt"
  "qCC\\CloudCompare_autogen"
  "qCC\\extern\\QCustomPlot\\CMakeFiles\\QCustomPlot_autogen.dir\\AutogenUsed.txt"
  "qCC\\extern\\QCustomPlot\\CMakeFiles\\QCustomPlot_autogen.dir\\ParseCache.txt"
  "qCC\\extern\\QCustomPlot\\QCustomPlot_autogen"
  )
endif()

# Install script for directory: C:/Users/bramburn/IdeaProject/CloudCompare/qCC

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/CloudCompareProjects")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "C:/Users/bramburn/AppData/Local/JetBrains/Toolbox/apps/CLion/ch-0/213.6777.58/bin/mingw/bin/objdump.exe")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/CloudCompare" TYPE DIRECTORY FILES "C:/Users/bramburn/IdeaProject/CloudCompare/cmake-build-debug/qCC/deployqt/")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/CloudCompare" TYPE EXECUTABLE FILES "C:/Users/bramburn/IdeaProject/CloudCompare/cmake-build-debug/qCC/CloudCompare.exe")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/CloudCompare/CloudCompare.exe" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/CloudCompare/CloudCompare.exe")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "C:/Users/bramburn/AppData/Local/JetBrains/Toolbox/apps/CLion/ch-0/213.6777.58/bin/mingw/bin/strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/CloudCompare/CloudCompare.exe")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/CloudCompare" TYPE FILE FILES "C:/Users/bramburn/IdeaProject/CloudCompare/CHANGELOG.md")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/CloudCompare" TYPE FILE FILES "C:/Users/bramburn/IdeaProject/CloudCompare/qCC/bin_other/license.txt")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/CloudCompare" TYPE FILE FILES "C:/Users/bramburn/IdeaProject/CloudCompare/qCC/bin_other/global_shift_list_template.txt")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/CloudCompare" TYPE FILE FILES "C:/Users/bramburn/IdeaProject/CloudCompare/qCC/bin_other/start.bat")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/CloudCompare/shaders/Bilateral" TYPE FILE FILES "C:/Users/bramburn/IdeaProject/CloudCompare/libs/CCFbo/shaders/Bilateral/bilateral.frag")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/CloudCompare/shaders/Bilateral" TYPE FILE FILES "C:/Users/bramburn/IdeaProject/CloudCompare/libs/CCFbo/shaders/Bilateral/bilateral.vert")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/CloudCompare/shaders/ColorRamp" TYPE FILE FILES "C:/Users/bramburn/IdeaProject/CloudCompare/qCC/shaders/ColorRamp/color_ramp.frag")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/CloudCompare/plugins" TYPE SHARED_LIBRARY FILES "C:/Users/bramburn/IdeaProject/CloudCompare/cmake-build-debug/plugins/core/IO/qCoreIO/libQCORE_IO_PLUGINd.dll")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/CloudCompare/plugins/libQCORE_IO_PLUGINd.dll" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/CloudCompare/plugins/libQCORE_IO_PLUGINd.dll")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "C:/Users/bramburn/AppData/Local/JetBrains/Toolbox/apps/CLion/ch-0/213.6777.58/bin/mingw/bin/strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/CloudCompare/plugins/libQCORE_IO_PLUGINd.dll")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/bramburn/IdeaProject/CloudCompare/cmake-build-debug/qCC/db_tree/cmake_install.cmake")
  include("C:/Users/bramburn/IdeaProject/CloudCompare/cmake-build-debug/qCC/pluginManager/cmake_install.cmake")
  include("C:/Users/bramburn/IdeaProject/CloudCompare/cmake-build-debug/qCC/translations/cmake_install.cmake")

endif()


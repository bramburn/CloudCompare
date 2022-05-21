# Install script for directory: C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/CCCoreLib" TYPE FILE FILES "C:/Users/bramburn/IdeaProject/CloudCompare/cmake-build-debug/libs/qCC_db/extern/CCCoreLib/exports/CCCoreLibExport.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/bramburn/IdeaProject/CloudCompare/cmake-build-debug/libs/qCC_db/extern/CCCoreLib/libCCCoreLibd.dll.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/CCCoreLib/CCCoreLibTargets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/CCCoreLib/CCCoreLibTargets.cmake"
         "C:/Users/bramburn/IdeaProject/CloudCompare/cmake-build-debug/libs/qCC_db/extern/CCCoreLib/CMakeFiles/Export/lib/cmake/CCCoreLib/CCCoreLibTargets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/CCCoreLib/CCCoreLibTargets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/CCCoreLib/CCCoreLibTargets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/CCCoreLib" TYPE FILE FILES "C:/Users/bramburn/IdeaProject/CloudCompare/cmake-build-debug/libs/qCC_db/extern/CCCoreLib/CMakeFiles/Export/lib/cmake/CCCoreLib/CCCoreLibTargets.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/CCCoreLib" TYPE FILE FILES "C:/Users/bramburn/IdeaProject/CloudCompare/cmake-build-debug/libs/qCC_db/extern/CCCoreLib/CMakeFiles/Export/lib/cmake/CCCoreLib/CCCoreLibTargets-debug.cmake")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/CCCoreLib" TYPE FILE FILES
    "C:/Users/bramburn/IdeaProject/CloudCompare/cmake-build-debug/libs/qCC_db/extern/CCCoreLib/CCCoreLibConfigVersion.cmake"
    "C:/Users/bramburn/IdeaProject/CloudCompare/cmake-build-debug/libs/qCC_db/extern/CCCoreLib/CCCoreLibConfig.cmake"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/bramburn/IdeaProject/CloudCompare/cmake-build-debug/libs/qCC_db/extern/CCCoreLib/include/cmake_install.cmake")
  include("C:/Users/bramburn/IdeaProject/CloudCompare/cmake-build-debug/libs/qCC_db/extern/CCCoreLib/src/cmake_install.cmake")

endif()


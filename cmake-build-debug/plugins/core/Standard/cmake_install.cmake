# Install script for directory: C:/Users/bramburn/IdeaProject/CloudCompare/plugins/core/Standard

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

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/bramburn/IdeaProject/CloudCompare/cmake-build-debug/plugins/core/Standard/qAnimation/cmake_install.cmake")
  include("C:/Users/bramburn/IdeaProject/CloudCompare/cmake-build-debug/plugins/core/Standard/qBroom/cmake_install.cmake")
  include("C:/Users/bramburn/IdeaProject/CloudCompare/cmake-build-debug/plugins/core/Standard/qCanupo/cmake_install.cmake")
  include("C:/Users/bramburn/IdeaProject/CloudCompare/cmake-build-debug/plugins/core/Standard/qCompass/cmake_install.cmake")
  include("C:/Users/bramburn/IdeaProject/CloudCompare/cmake-build-debug/plugins/core/Standard/qCork/cmake_install.cmake")
  include("C:/Users/bramburn/IdeaProject/CloudCompare/cmake-build-debug/plugins/core/Standard/qCSF/cmake_install.cmake")
  include("C:/Users/bramburn/IdeaProject/CloudCompare/cmake-build-debug/plugins/core/Standard/qFacets/cmake_install.cmake")
  include("C:/Users/bramburn/IdeaProject/CloudCompare/cmake-build-debug/plugins/core/Standard/qHoughNormals/cmake_install.cmake")
  include("C:/Users/bramburn/IdeaProject/CloudCompare/cmake-build-debug/plugins/core/Standard/qHPR/cmake_install.cmake")
  include("C:/Users/bramburn/IdeaProject/CloudCompare/cmake-build-debug/plugins/core/Standard/qM3C2/cmake_install.cmake")
  include("C:/Users/bramburn/IdeaProject/CloudCompare/cmake-build-debug/plugins/core/Standard/qPCL/cmake_install.cmake")
  include("C:/Users/bramburn/IdeaProject/CloudCompare/cmake-build-debug/plugins/core/Standard/qPCV/cmake_install.cmake")
  include("C:/Users/bramburn/IdeaProject/CloudCompare/cmake-build-debug/plugins/core/Standard/qPoissonRecon/cmake_install.cmake")
  include("C:/Users/bramburn/IdeaProject/CloudCompare/cmake-build-debug/plugins/core/Standard/qRANSAC_SD/cmake_install.cmake")
  include("C:/Users/bramburn/IdeaProject/CloudCompare/cmake-build-debug/plugins/core/Standard/qSRA/cmake_install.cmake")
  include("C:/Users/bramburn/IdeaProject/CloudCompare/cmake-build-debug/plugins/core/Standard/qColorimetricSegmenter/cmake_install.cmake")
  include("C:/Users/bramburn/IdeaProject/CloudCompare/cmake-build-debug/plugins/core/Standard/qMasonry/cmake_install.cmake")
  include("C:/Users/bramburn/IdeaProject/CloudCompare/cmake-build-debug/plugins/core/Standard/qMPlane/cmake_install.cmake")
  include("C:/Users/bramburn/IdeaProject/CloudCompare/cmake-build-debug/plugins/core/Standard/qJSonRPCPlugin/cmake_install.cmake")

endif()


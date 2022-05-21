# Install script for directory: C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/CCCoreLib" TYPE FILE FILES
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/AutoSegmentationTools.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/BoundingBox.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/CCConst.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/CCCoreLib.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/CCGeom.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/CCMath.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/CCMiscTools.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/CCPlatform.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/CCShareable.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/CCToolbox.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/CCTypes.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/ChamferDistanceTransform.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/CloudSamplingTools.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/ConjugateGradient.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/Delaunay2dMesh.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/DgmOctree.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/DgmOctreeReferenceCloud.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/DistanceComputationTools.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/ErrorFunction.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/FastMarching.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/FastMarchingForPropagation.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/Garbage.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/GenericCloud.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/GenericDistribution.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/GenericIndexedCloud.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/GenericIndexedCloudPersist.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/GenericIndexedMesh.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/GenericMesh.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/GenericOctree.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/GenericProgressCallback.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/GenericTriangle.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/GeometricalAnalysisTools.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/GridAndMeshIntersection.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/Grid3D.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/Jacobi.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/KdTree.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/LocalModel.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/ManualSegmentationTools.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/MathTools.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/MeshSamplingTools.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/Neighbourhood.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/NormalDistribution.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/ParallelSort.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/PointCloud.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/PointCloudTpl.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/PointProjectionTools.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/Polyline.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/RayAndBox.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/ReferenceCloud.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/RegistrationTools.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/SaitoSquaredDistanceTransform.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/ScalarField.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/ScalarFieldTools.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/SimpleMesh.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/SimpleTriangle.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/SquareMatrix.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/StatisticalTestingTools.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/TrueKdTree.h"
    "C:/Users/bramburn/IdeaProject/CloudCompare/libs/qCC_db/extern/CCCoreLib/include/WeibullDistribution.h"
    )
endif()


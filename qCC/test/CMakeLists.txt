find_package(Qt6 REQUIRED COMPONENTS Test)

#On Windows / MSVC, the Qt6 OpenGLWidgets component links d3d11.lib / dxgi.lib
#but the Windows SDK library directory is not on the           linker's default search path.
#CMake sets it up via the MSVC toolchain on full Visual Studio installs, but the
#Community edition + vcpkg toolchain needs an explicit / LIBPATH for the SDK.
    if (MSVC AND DEFINED CC_WINDOWS_SDK_LIB_DIR AND EXISTS "${CC_WINDOWS_SDK_LIB_DIR}")
        add_link_options("/LIBPATH:\"${CC_WINDOWS_SDK_LIB_DIR}\"")
            endif()

                add_executable(TestArgumentParser
                                   TestArgumentParser.cpp)

                    target_link_libraries(TestArgumentParser
                                              PRIVATE
                                                  CCPluginAPI
                                                      Qt6::Test)

                        add_test(NAME TestArgumentParser COMMAND TestArgumentParser)

#T1 - B : FileIOFilter core
                            add_executable(TestFileIOFilter
                                               TestFileIOFilter.cpp)

                                target_link_libraries(TestFileIOFilter
                                                          PRIVATE
                                                              QCC_IO_LIB
                                                                  QCC_DB_LIB
                                                                      CCCoreLib
                                                                          CCPluginAPI
                                                                              Qt6::Test)

                                    add_test(NAME TestFileIOFilter COMMAND TestFileIOFilter)

#T1 - G : Point cloud
                                        add_executable(TestPointCloud
                                                           TestPointCloud.cpp)

                                            target_link_libraries(TestPointCloud
                                                                      PRIVATE
                                                                          QCC_DB_LIB
                                                                              CCCoreLib
                                                                                  CCPluginAPI
                                                                                      Qt6::Test)

                                                add_test(NAME TestPointCloud COMMAND TestPointCloud)

#T1 - H : Shifted object(coordinate shift / scale)
                                                    add_executable(TestShiftedObject
                                                                       TestShiftedObject.cpp)

                                                        target_link_libraries(TestShiftedObject
                                                                                  PRIVATE
                                                                                      QCC_DB_LIB
                                                                                          CCCoreLib
                                                                                              CCPluginAPI
                                                                                                  Qt6::Test)

                                                            add_test(NAME TestShiftedObject COMMAND TestShiftedObject)

#T1 - C : Registration math(Horn, quaternion, SVD, transformation)
                                                                add_executable(TestRegistration
                                                                                   TestRegistration.cpp)

                                                                    target_link_libraries(TestRegistration
                                                                                              PRIVATE
                                                                                                  QCC_DB_LIB
                                                                                                      CCCoreLib
                                                                                                          CCPluginAPI
                                                                                                              Qt6::Test)

                                                                        add_test(NAME TestRegistration COMMAND TestRegistration)

#T1 - D : qM3C2 tools(ComputeStatistics, MakeNormalsHorizontal)
                                                                            add_executable(TestM3C2
                                                                                               TestM3C2.cpp)

                                                                                target_include_directories(TestM3C2 PRIVATE
                                                                                                               ${CMAKE_SOURCE_DIR}/plugins/core/Standard/qM3C2/include)

                                                                                    target_link_libraries(TestM3C2
                                                                                                              PRIVATE
                                                                                                                  QM3C2_PLUGIN
                                                                                                                      QCC_DB_LIB
                                                                                                                          CCCoreLib
                                                                                                                              CCPluginAPI
                                                                                                                                  Qt6::Test)

                                                                                        add_test(NAME TestM3C2 COMMAND TestM3C2)

#T1 - E : qCSF(Cloth Simulation Filter)
#CSF_lib is the STATIC library holding all pure - C++ algorithm classes.
#TestCSF links directly to it — no Qt, no MODULE plugin needed.
                                                                                            add_executable(TestCSF
                                                                                                               TestCSF.cpp)

                                                                                                target_include_directories(TestCSF PRIVATE
                                                                                                                               ${CMAKE_SOURCE_DIR}/plugins/core/Standard/qCSF/include)

                                                                                                    target_link_libraries(TestCSF
                                                                                                                              PRIVATE
                                                                                                                                  CSF_lib
                                                                                                                                      QCC_DB_LIB
                                                                                                                                          CCCoreLib
                                                                                                                                              CCPluginAPI
                                                                                                                                                  Qt6::Test)

                                                                                                        add_test(NAME TestCSF COMMAND TestCSF)

#T1 - F : ccHObject tree(addChild, removeChild, filterChildren, find, transfer, BB, etc.)
                                                                                                            add_executable(TestDBTree
                                                                                                                               TestDBTree.cpp)

                                                                                                                target_link_libraries(TestDBTree
                                                                                                                                          PRIVATE
                                                                                                                                              QCC_DB_LIB
                                                                                                                                                  CCCoreLib
                                                                                                                                                      CCPluginAPI
                                                                                                                                                          Qt6::Test)

                                                                                                                    add_test(NAME TestDBTree COMMAND TestDBTree)

#T2 - A : ScalarFieldTools
                                                                                                                        add_executable(TestScalarField
                                                                                                                                           TestScalarField.cpp)

                                                                                                                            target_link_libraries(TestScalarField
                                                                                                                                                      PRIVATE
                                                                                                                                                          QCC_DB_LIB
                                                                                                                                                              CCCoreLib
                                                                                                                                                                  CCPluginAPI
                                                                                                                                                                      Qt6::Test)

                                                                                                                                add_test(NAME TestScalarField COMMAND TestScalarField)

#T2 - B : ccGLMatrixd / SquareMatrix
                                                                                                                                    add_executable(TestGLMatrix
                                                                                                                                                       TestGLMatrix.cpp)

                                                                                                                                        target_link_libraries(TestGLMatrix
                                                                                                                                                                  PRIVATE
                                                                                                                                                                      QCC_DB_LIB
                                                                                                                                                                          CCCoreLib
                                                                                                                                                                              CCPluginAPI
                                                                                                                                                                                  Qt6::Test)

                                                                                                                                            add_test(NAME TestGLMatrix COMMAND TestGLMatrix)

#T2 - F : ccMaterial
                                                                                                                                                add_executable(TestMaterial
                                                                                                                                                                   TestMaterial.cpp)

                                                                                                                                                    target_link_libraries(TestMaterial
                                                                                                                                                                              PRIVATE
                                                                                                                                                                                  QCC_DB_LIB
                                                                                                                                                                                      CCCoreLib
                                                                                                                                                                                          CCPluginAPI
                                                                                                                                                                                              Qt6::Test)

                                                                                                                                                        add_test(NAME TestMaterial COMMAND TestMaterial)

#T2 - G : ccHistogramWindow / ScalarFieldTools
                                                                                                                                                            add_executable(TestHistogram
                                                                                                                                                                               TestHistogram.cpp)

                                                                                                                                                                target_link_libraries(TestHistogram
                                                                                                                                                                                          PRIVATE
                                                                                                                                                                                              QCC_DB_LIB
                                                                                                                                                                                                  CCCoreLib
                                                                                                                                                                                                      CCPluginAPI
                                                                                                                                                                                                          Qt6::Test)

                                                                                                                                                                    add_test(NAME TestHistogram COMMAND TestHistogram)

#T2 - H : ccRecentFiles
                                                                                                                                                                        add_executable(TestRecentFiles
                                                                                                                                                                                           TestRecentFiles.cpp)

                                                                                                                                                                            target_link_libraries(TestRecentFiles
                                                                                                                                                                                                      PRIVATE
                                                                                                                                                                                                          QCC_DB_LIB
                                                                                                                                                                                                              CCPluginAPI
                                                                                                                                                                                                                  Qt6::Test)

                                                                                                                                                                                add_test(NAME TestRecentFiles COMMAND TestRecentFiles)

#Sentry SDK smoke test
                                                                                                                                                                                    if (CC_SENTRY_AVAILABLE)
                                                                                                                                                                                        add_executable(TestSentry
                                                                                                                                                                                                           TestSentry.cpp)
                                                                                                                                                                                            target_link_libraries(TestSentry
                                                                                                                                                                                                                      PRIVATE
                                                                                                                                                                                                                          CCPluginAPI
                                                                                                                                                                                                                                  sentry::sentry
                                                                                                                                                                                                                                  Qt6::Test)
                                                                                                                                                                                                target_compile_definitions(TestSentry PRIVATE CC_USE_SENTRY)
                                                                                                                                                                                                    add_test(NAME TestSentry COMMAND TestSentry)
                                                                                                                                                                                                        endif()

#T2 - I : ccLibAlgorithms(GetDefaultCloudKernelSize, GetDensitySFName)
                                                                                                                                                                                                            add_executable(TestLibAlgorithms
                                                                                                                                                                                                                               TestLibAlgorithms.cpp)

                                                                                                                                                                                                                target_link_libraries(TestLibAlgorithms
                                                                                                                                                                                                                                          PRIVATE
                                                                                                                                                                                                                                              QCC_DB_LIB
                                                                                                                                                                                                                                                  CCCoreLib
                                                                                                                                                                                                                                                      CCPluginAPI
                                                                                                                                                                                                                                                          Qt6::Test)

                                                                                                                                                                                                                    add_test(NAME TestLibAlgorithms COMMAND TestLibAlgorithms)

#T2 - J : ccInnerRect2DFinder(largest empty rectangle in a point cloud)
                                                                                                                                                                                                                        add_executable(TestInnerRect
                                                                                                                                                                                                                                               TestInnerRect.cpp
                                                                                                                                                                                                                                               ${CMAKE_SOURCE_DIR}/qCC/ccInnerRect2DFinder.cpp)

                                                                                                                                                                                                                            target_link_libraries(TestInnerRect
                                                                                                                                                                                                                                                      PRIVATE
                                                                                                                                                                                                                                                          QCC_DB_LIB
                                                                                                                                                                                                                                                              CCPluginAPI
                                                                                                                                                                                                                                                                  Qt6::Test)

                                                                                                                                                                                                                                add_test(NAME TestInnerRect COMMAND TestInnerRect)

#T3 - B : qPCV(GCD, GenerateRays / Fibonacci sphere sampling)
                                                                                                                                                                                                                                    add_executable(TestPCV
                                                                                                                                                                                                                                                       TestPCV.cpp)

                                                                                                                                                                                                                                        target_include_directories(TestPCV PRIVATE
                                                                                                                                                                                                                                                                       ${CMAKE_SOURCE_DIR}/libs/qCC_db/extern/CCCoreLib/include)

                                                                                                                                                                                                                                            target_link_libraries(TestPCV
                                                                                                                                                                                                                                                                      PRIVATE
                                                                                                                                                                                                                                                                          QCC_DB_LIB
                                                                                                                                                                                                                                                                              CCCoreLib
                                                                                                                                                                                                                                                                                  CCPluginAPI
                                                                                                                                                                                                                                                                                      Qt6::Test)

                                                                                                                                                                                                                                                add_test(NAME TestPCV COMMAND TestPCV)

#T3 - C : qVoxFall(Grid2Index, Index2Grid, FindAdjacents, qVoxFallTransform)
                                                                                                                                                                                                                                                    add_executable(TestVoxFall
                                                                                                                                                                                                                                                                       TestVoxFall.cpp)

                                                                                                                                                                                                                                                        target_include_directories(TestVoxFall PRIVATE
                                                                                                                                                                                                                                                                                       ${CMAKE_SOURCE_DIR}/plugins/core/Standard/qVoxFall/include)

                                                                                                                                                                                                                                                            target_link_libraries(TestVoxFall
                                                                                                                                                                                                                                                                                      PRIVATE
                                                                                                                                                                                                                                                                                          QCC_DB_LIB
                                                                                                                                                                                                                                                                                              CCCoreLib
                                                                                                                                                                                                                                                                                                  CCPluginAPI
                                                                                                                                                                                                                                                                                                      Qt6::Test)

                                                                                                                                                                                                                                                                add_test(NAME TestVoxFall COMMAND TestVoxFall)

#T3 - D : qAnimation(ViewInterpolate : interpolate, nextView, setSmoothTrajectory)
#ExtendedViewportParameters + ViewInterpolate are embedded in the test file
#to avoid a hard dependency on the qAnimation                     plugin(PLUGIN_STANDARD_QANIMATION = OFF).
                                                                                                                                                                                                                                                                    add_executable(TestAnimation
                                                                                                                                                                                                                                                                                       TestAnimation.cpp)

                                                                                                                                                                                                                                                                        target_link_libraries(TestAnimation
                                                                                                                                                                                                                                                                                                  PRIVATE
                                                                                                                                                                                                                                                                                                      QCC_DB_LIB
                                                                                                                                                                                                                                                                                                          CCCoreLib
                                                                                                                                                                                                                                                                                                              CCPluginAPI
                                                                                                                                                                                                                                                                                                                  Qt6::Test)

                                                                                                                                                                                                                                                                            add_test(NAME TestAnimation COMMAND TestAnimation)

#T3 - E : NormalDistribution + WeibullDistribution(Gaussian PDF, erf, robust params, chi2)
                                                                                                                                                                                                                                                                                add_executable(TestDistributions
                                                                                                                                                                                                                                                                                                   TestDistributions.cpp)

                                                                                                                                                                                                                                                                                    target_link_libraries(TestDistributions
                                                                                                                                                                                                                                                                                                              PRIVATE
                                                                                                                                                                                                                                                                                                                  QCC_DB_LIB
                                                                                                                                                                                                                                                                                                                      CCCoreLib
                                                                                                                                                                                                                                                                                                                          CCPluginAPI
                                                                                                                                                                                                                                                                                                                              Qt6::Test)

                                                                                                                                                                                                                                                                                        add_test(NAME TestDistributions COMMAND TestDistributions)

#T3 - F : ccOctree(DgmOctree build, query, bounding box)
                                                                                                                                                                                                                                                                                            add_executable(TestOctree
                                                                                                                                                                                                                                                                                                               TestOctree.cpp)

                                                                                                                                                                                                                                                                                                target_link_libraries(TestOctree
                                                                                                                                                                                                                                                                                                                          PRIVATE
                                                                                                                                                                                                                                                                                                                              QCC_DB_LIB
                                                                                                                                                                                                                                                                                                                                  CCCoreLib
                                                                                                                                                                                                                                                                                                                                      CCPluginAPI
                                                                                                                                                                                                                                                                                                                                          Qt6::Test)

                                                                                                                                                                                                                                                                                                    add_test(NAME TestOctree COMMAND TestOctree)

#T3 - G : ccMesh(triangle area, normal, BB)
                                                                                                                                                                                                                                                                                                        add_executable(TestMesh
                                                                                                                                                                                                                                                                                                                           TestMesh.cpp)

                                                                                                                                                                                                                                                                                                            target_link_libraries(TestMesh
                                                                                                                                                                                                                                                                                                                                      PRIVATE
                                                                                                                                                                                                                                                                                                                                          QCC_DB_LIB
                                                                                                                                                                                                                                                                                                                                              CCCoreLib
                                                                                                                                                                                                                                                                                                                                                  CCPluginAPI
                                                                                                                                                                                                                                                                                                                                                      Qt6::Test)

                                                                                                                                                                                                                                                                                                                add_test(NAME TestMesh COMMAND TestMesh)

#T4 - C : StatisticalTestingTools(Chi2 fractile, probability, roundtrip)
                                                                                                                                                                                                                                                                                                                    add_executable(TestStatisticalTesting
                                                                                                                                                                                                                                                                                                                                       TestStatisticalTesting.cpp)

                                                                                                                                                                                                                                                                                                                        target_link_libraries(TestStatisticalTesting
                                                                                                                                                                                                                                                                                                                                                  PRIVATE
                                                                                                                                                                                                                                                                                                                                                      QCC_DB_LIB
                                                                                                                                                                                                                                                                                                                                                          CCCoreLib
                                                                                                                                                                                                                                                                                                                                                              CCPluginAPI
                                                                                                                                                                                                                                                                                                                                                                  Qt6::Test)

                                                                                                                                                                                                                                                                                                                            add_test(NAME TestStatisticalTesting COMMAND TestStatisticalTesting)

#T4 - G : ccConsole(Init, RefreshCycle, QtMessagesEnabled)
                                                                                                                                                                                                                                                                                                                                add_executable(TestConsole
                                                                                                                                                                                                                                                                                                                                                       TestConsole.cpp
                                                                                                                                                                                                                                                                                                                                                       ${CMAKE_SOURCE_DIR}/qCC/ccConsole.cpp)

                                                                                                                                                                                                                                                                                                                                    target_link_libraries(TestConsole
                                                                                                                                                                                                                                                                                                                                                              PRIVATE
                                                                                                                                                                                                                                                                                                                                                                  QCC_GL_LIB
                                                                                                                                                                                                                                                                                                                                                                      QCC_IO_LIB
                                                                                                                                                                                                                                                                                                                                                                          QCC_DB_LIB
                                                                                                                                                                                                                                                                                                                                                                              CCCoreLib
                                                                                                                                                                                                                                                                                                                                                                                  CCPluginAPI
                                                                                                                                                                                                                                                                                                                                                                                      CCFbo
                                                                                                                                                                                                                                                                                                                                                                                                      Qt6::Widgets
                                                                                                                                                                                                                                                                                                                                                                                                      Qt6::OpenGL
                                                                                                                                                                                                                                                                                                                                                                                                      Qt6::OpenGLWidgets
                                                                                                                                                                                                                                                                                                                                                                                                      Qt6::Test)

                                                                                                                                                                                                                                                                                                                                        add_test(NAME TestConsole COMMAND TestConsole)

#T4 - H : ccGLUtils pure math(view matrix trig, FOV, depth conversion)
                                                                                                                                                                                                                                                                                                                                            add_executable(TestGLUtils
                                                                                                                                                                                                                                                                                                                                                               TestGLUtils.cpp)

                                                                                                                                                                                                                                                                                                                                                target_link_libraries(TestGLUtils
                                                                                                                                                                                                                                                                                                                                                                          PRIVATE
                                                                                                                                                                                                                                                                                                                                                                              CCPluginAPI
                                                                                                                                                                                                                                                                                                                                                                                  Qt6::Test)

                                                                                                                                                                                                                                                                                                                                                    add_test(NAME TestGLUtils COMMAND TestGLUtils)

#T3 - I : ccSensor(spherical / cartesian conversion math)
                                                                                                                                                                                                                                                                                                                                                        add_executable(TestSensor
                                                                                                                                                                                                                                                                                                                                                                           TestSensor.cpp)

                                                                                                                                                                                                                                                                                                                                                            target_link_libraries(TestSensor
                                                                                                                                                                                                                                                                                                                                                                                      PRIVATE
                                                                                                                                                                                                                                                                                                                                                                                          QCC_DB_LIB
                                                                                                                                                                                                                                                                                                                                                                                              CCCoreLib
                                                                                                                                                                                                                                                                                                                                                                                                  CCPluginAPI
                                                                                                                                                                                                                                                                                                                                                                                                      Qt6::Test)

                                                                                                                                                                                                                                                                                                                                                                add_test(NAME TestSensor COMMAND TestSensor)

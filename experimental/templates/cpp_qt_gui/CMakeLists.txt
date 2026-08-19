# CMakeLists.txt for the cpp_qt_gui template.
#
# Qt 6 desktop app with an OpenGL viewport for 3D point cloud rendering.
#
# Build:
#   cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH=C:/dev/tools/Qt/6.8.3/msvc2022_64
#   cmake --build build
#
# Run (Qt DLLs must be on PATH):
#   $env:Path = 'C:\dev\tools\Qt\6.8.3\msvc2022_64\bin;' + $env:Path
#   .\build\demo_gui.exe

cmake_minimum_required(VERSION 3.16)
project(cpp_qt_gui_template LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt 6 — Core, Gui, Widgets, OpenGLWidgets. No QML.
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Widgets OpenGLWidgets)

qt_standard_project_setup()

# The OpenGL viewport in the GUI links Qt6::OpenGLWidgets, which in turn
# links d3d11.lib, dxgi.lib, d3d12.lib from the Windows SDK. The MSVC
# linker doesn't always find them — add the SDK lib path explicitly.
# See shared/cmake/QtConfig.cmake for the auto-detect helper.
if(MSVC AND EXISTS "$ENV{WindowsSdkDir}/Lib/$ENV{WindowsSDKVersion}/um/x64/d3d11.lib")
    message(STATUS "Adding Windows SDK lib path for OpenGLWidgets d3d11 link")
    add_link_options("/LIBPATH:\"$ENV{WindowsSdkDir}/Lib/$ENV{WindowsSDKVersion}/um/x64\"")
endif()

# Add the executable — qt_add_executable sets up MOC/UIC/RCC and respects
# WIN32 to suppress the console window.
qt_add_executable(demo_gui WIN32
    src/main.cpp
    src/pointcloudview.cpp
    src/synthetic.cpp
)

# Headers are picked up by AUTOMOC via target_include_directories.
target_include_directories(demo_gui PRIVATE src)

# Link Qt modules.
target_link_libraries(demo_gui PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::OpenGLWidgets
)

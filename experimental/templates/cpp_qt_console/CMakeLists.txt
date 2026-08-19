# CMakeLists.txt for the cpp_qt_console template.
#
# Minimal Qt 6 console app. Builds with cmake + ninja (or Visual Studio).
# Find this template's expected build pattern in shared/cmake/QtConfig.cmake
# (auto-detects Qt 6.8.3 on this machine).
#
# Usage:
#   cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH=C:/dev/tools/Qt/6.8.3/msvc2022_64
#   cmake --build build

cmake_minimum_required(VERSION 3.16)
project(cpp_qt_console_template LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt 6 — Core only (no GUI / Widgets / OpenGL)
find_package(Qt6 6.5 REQUIRED COMPONENTS Core)

qt_standard_project_setup()

# Add the executable
add_executable(demo_cli src/main.cpp)

target_link_libraries(demo_cli PRIVATE Qt6::Core)

# Windows: hide the console window for GUI apps. We are a console app,
# so we DON'T add /SUBSYSTEM:WINDOWS — leave the default console behaviour.

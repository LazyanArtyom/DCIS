cmake_minimum_required(VERSION 3.5)

# Drone Collective Intelligence System
project(DCIS VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# For NVIM tags
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# For MacOS build pass
set(CMAKE_OSX_SYSROOT "/Library/Developer/CommandLineTools/SDKs/MacOSX15.sdk")

find_package(Qt6 REQUIRED COMPONENTS Widgets Network Location Core Concurrent WebChannel WebEngineWidgets)
qt_standard_project_setup()

add_subdirectory(src/dcis)
add_subdirectory(src/core)
add_subdirectory(src/common)

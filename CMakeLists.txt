cmake_minimum_required(VERSION 3.5)

# Drone Collective Intelligence System
project(DCIS VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network)
qt_standard_project_setup()

add_subdirectory(src/dcis)
add_subdirectory(src/server)
add_subdirectory(src/common)

set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include/dcis")
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src/dcis")

include_directories(${INCLUDE_DIR})
include_directories(${SOURCE_DIR})

file(GLOB_RECURSE SOURCES
    "${INCLUDE_DIR}/*.h"
    "${SOURCE_DIR}/*.cpp"
)

add_executable(dcis
    ${SOURCES}
)

target_link_libraries(dcis PRIVATE
    common
    Qt6::Widgets)

set_target_properties(dcis PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

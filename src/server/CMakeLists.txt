set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include/server")
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src/server")

include_directories(${INCLUDE_DIR})
include_directories(${SOURCE_DIR})

file(GLOB_RECURSE SOURCES
    "${INCLUDE_DIR}/*.h"
    "${INCLUDE_DIR}/net/*.h"
    "${SOURCE_DIR}/*.cpp"
)

add_executable(server
    ${SOURCES}
)

target_link_libraries(server PRIVATE
    common
    Qt6::Widgets
    Qt6::Network
    )

set_target_properties(server PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

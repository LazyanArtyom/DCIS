set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/common")
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src/common")

include_directories(${INCLUDE_DIR})
include_directories(${SOURCE_DIR})

file(GLOB_RECURSE SOURCES
    "${INCLUDE_DIR}/*.h"
    "${INCLUDE_DIR}/utils/*.h"
    "${INCLUDE_DIR}/net/*.h"
    "${SOURCE_DIR}/*.cpp"
)

add_library(common STATIC
    ${SOURCES}
)

target_link_libraries(common PRIVATE Qt6::Widgets)
target_include_directories(common INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

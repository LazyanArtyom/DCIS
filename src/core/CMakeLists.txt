set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include/core")
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src/core")

include_directories(${INCLUDE_DIR})
include_directories(${SOURCE_DIR})

file(GLOB_RECURSE SOURCES
    "${INCLUDE_DIR}/*.h"
#   "${INCLUDE_DIR}/net/*.h"
#	"${INCLUDE_DIR}/graph/*.h"
    "${SOURCE_DIR}/*.cpp"
)

add_executable(core
    ${SOURCES}
)

target_link_libraries(core PRIVATE
    common
    Qt6::Widgets
    Qt6::Network
    )

set_target_properties(core PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

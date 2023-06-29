set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include/dcis")
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src/dcis")

include_directories(${INCLUDE_DIR})
include_directories(${SOURCE_DIR})

file(GLOB_RECURSE SOURCES
    "${INCLUDE_DIR}/*.h"
#    "${INCLUDE_DIR}/net/*.h"
#	"${INCLUDE_DIR}/graph/*.h"
    "${SOURCE_DIR}/*.cpp"
)

add_executable(dcis
    ${SOURCES}
)

# Resources:
set(dcis_icons
    "resources/upload.png"
    "resources/clear_cycles.png"
    "resources/graph.png"
    "resources/attack.png"
    "resources/drone.png"
    "resources/exploration.png"
)

qt_add_resources(dcis "dcis"
    PREFIX
        "/"
    FILES
        ${dcis_icons}
)

target_link_libraries(dcis PRIVATE
    common
    Qt6::Widgets
    Qt6::Network
    Qt6::Location
    )

set_target_properties(dcis PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

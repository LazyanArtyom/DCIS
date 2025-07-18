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
    "resources/load_graph.png"
    "resources/save_graph.png"
    "resources/grid.png"
    "resources/drone_cntrl.png"
    "resources/drone_sim.png"
    "resources/graph.png"
    "resources/attack.png"
    "resources/set_drone.png"
    "resources/set_attacker.png"
    "resources/set_target.svg"
    "resources/set_inner.png"
    "resources/set_corner.png"
    "resources/set_border.png"
    "resources/exploration.png"
    "resources/background.png"
)

set(dcis_webchannel
    "resources/qwebchannel.js"
    "resources/index.html"
)

qt_add_resources(dcis "dcis"
    PREFIX
        "/"
    FILES
        ${dcis_icons}
        ${dcis_webchannel}
)

target_link_libraries(dcis PRIVATE
    common
    Qt6::Widgets
    Qt6::Network
    Qt6::Location
    Qt6::WebChannel
    Qt6::WebEngineWidgets
    )

set_target_properties(dcis PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

include_directories("imgui/backends")
include_directories("/imgui")

file(GLOB SOURCES
        "imgui/*.cpp"
        "imgui/*.h"
)

add_library(imgui STATIC
        ${SOURCES}
        )

add_library(ImguiDependency
        INTERFACE
        )
target_link_libraries(ImguiDependency INTERFACE
        imgui
        imgui
        imgui_backends
        )
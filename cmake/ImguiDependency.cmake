include_directories("imgui/backends")
include_directories("/imgui")

file(GLOB SOURCES
        "imgui/*.cpp"
        "imgui/*.h"
        "imgui/backends/imgui_impl_sdl3.h"
        "imgui/backends/imgui_impl_sdl3.cpp"
        "imgui/backends/imgui_impl_sdlrenderer3.h"
        "imgui/backends/imgui_impl_sdlrenderer3.cpp"
)

include_directories("imgui")
include_directories("imgui/backends")

add_library(ImguiDependency STATIC
        ${SOURCES}
        ../src/Application/SaveGame.cpp
        ../include/Application/SaveGame.h
)
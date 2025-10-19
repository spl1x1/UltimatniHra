cmake_minimum_required(VERSION 3.31)
project(UltimatniHra)

set(CMAKE_CXX_STANDARD 20)

include(cmake/CPM.cmake)

set(SDLTTF_VENDORED ON CACHE BOOL "SDL_TTF - Bundeled dependencies" FORCE)
set(RMLUI_STATIC_LIB ON CACHE BOOL "RmlUi - static library" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Static libs" FORCE)

#sdl
CPMAddPackage("gh:libsdl-org/SDL#a8589a8")
CPMAddPackage("gh:libsdl-org/SDL_image#11154af")
CPMAddPackage("gh:libsdl-org/SDL_ttf#a1ce367")

#other libs
CPMAddPackage("gh:nlohmann/json#55f9368")
CPMAddPackage("gh:mikke89/RmlUi#58c7515")


add_executable(UltimatniHra
        main.cpp
        server/Server.cpp
        server/Server.h
        render/Window.cpp
        render/Window.h
        server/Event.h
)

add_executable(server_test
        server/World/generace_mapy.h
        server/World/generace_mapy.cpp
        server/World/test.cpp
)

target_link_libraries(UltimatniHra
        SDL3::SDL3
        SDL3_image::SDL3_image
        nlohmann_json::nlohmann_json
        SDL3_ttf::SDL3_ttf
        RmlUi::RmlUi
)

add_custom_command(TARGET UltimatniHra POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "${CMAKE_BINARY_DIR}/assets")


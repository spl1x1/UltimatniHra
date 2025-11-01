cmake_minimum_required(VERSION 3.31)
project(UltimatniHra)

set(CMAKE_CXX_STANDARD 20)

include(cmake/CPM.cmake)

set(RMLUI_STATIC_LIB ON CACHE BOOL "RmlUi - static library" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Static libs" FORCE)
set(RMLUI_LUA_BINDINGS ON "Build Lua bindings." FORCE)


add_compile_definitions(RMLUI_SDL_VERSION_MAJOR=3)
add_compile_definitions(SDL_VERSION_MAJOR=3)
add_compile_definitions(DEBUG)

#sdl
CPMAddPackage("gh:libsdl-org/SDL#a8589a8")
CPMAddPackage("gh:libsdl-org/SDL_image#11154af")
CPMAddPackage("https://download.savannah.gnu.org/releases/freetype/ft2141.zip")
#lua
CPMAddPackage("gh:lubgr/lua-cmake#master")

add_library(Freetype::Freetype ALIAS freetype)
add_library(SDL::SDL ALIAS SDL3-static )
add_library(SDL_image::SDL_image ALIAS SDL3_image-static)

add_library(Lua::Lua INTERFACE IMPORTED
        server/World/Biomes/Structure.h
        server/Server.cpp
        server/Server.h.h)

set_target_properties(Lua::Lua PROPERTIES
        INTERFACE_LINK_LIBRARIES "lua::lib"
)

#other libs
CPMAddPackage("gh:nlohmann/json#55f9368")
CPMAddPackage("gh:mikke89/RmlUi#58c7515")


add_subdirectory(
        ${CMAKE_BINARY_DIR}/_deps/rmlui-src/Backends
)

add_executable(UltimatniHra
        main.cpp
        MACROS.h
        render/Window.cpp
        render/Window.h
        server/Event.h
        render/World/WorldRender.cpp
        render/World/WorldRender.h
        server/World/generace_mapy.h
        server/World/generace_mapy.cpp
        render/Menu/RmlUi_Renderer_SDL.cpp
        render/Menu/RmlUi_Renderer_SDL.h
        render/Menu/RmlUi_Platform_SDL.cpp
        render/Menu/RmlUi_Platform_SDL.h
        render/ScaleFactorCalculator.h
        render/ScaleFactorCalculator.cpp
        server/World/WorldStructs.h
        server/World/WorldStructs.cpp
        server/Entities/EntityStructs.cpp
        server/Entities/EntityStructs.h
        server/Server.h
        server/Server.cpp
        server/World/Biomes/Biome.h
        server/World/Biomes/Water/WaterBiome.h
        server/World/Biomes/Water/WaterBiome.cpp
        render/Sprites/WaterSprite.cpp
        render/Sprites/WaterSprite.h
        render/Sprites/Sprite.h
)

add_executable(server_test
        server/World/generace_mapy.h
        server/World/generace_mapy.cpp
        server/World/test.cpp
        server/World/WorldStructs.h
        server/World/WorldStructs.cpp
        server/World/BossPathfinding.cpp
        server/World/BossPathfinding.h
)

target_link_libraries(UltimatniHra
        SDL3::SDL3
        SDL3_image::SDL3_image
        nlohmann_json::nlohmann_json
        RmlUi::RmlUi
        RmlUi::Lua
)

add_executable(ScalingTest
        render/ScaleFactorCalculator.h
        render/ScaleFactorCalculator.cpp
        render/ScalingFactorCalculator_test.cpp
        MACROS.h
)

add_custom_command(TARGET UltimatniHra POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "${CMAKE_BINARY_DIR}/assets")


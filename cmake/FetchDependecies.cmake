include(${CMAKE_CURRENT_LIST_DIR}/CPM.cmake)
add_library(LibsBundle INTERFACE)


#dependecies options
set(RMLUI_STATIC_LIB ON CACHE BOOL "RmlUi - static library" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Static libs" FORCE)
add_compile_definitions(RMLUI_SDL_VERSION_MAJOR=3)
add_compile_definitions(SDL_VERSION_MAJOR=3)

#sdl
CPMAddPackage("gh:libsdl-org/SDL#5b57263")
CPMAddPackage("gh:libsdl-org/SDL_image#11154af")
CPMAddPackage("https://download.savannah.gnu.org/releases/freetype/ft2141.zip")

#lua
CPMAddPackage("gh:lubgr/lua-cmake#master")
CPMAddPackage("gh:ThePhD/sol2#main")


add_library(Freetype::Freetype ALIAS freetype)
add_library(SDL::SDL ALIAS SDL3-static )
add_library(SDL_image::SDL_image ALIAS SDL3_image-static)

#other libs
CPMAddPackage("gh:nlohmann/json#55f9368")
CPMAddPackage("gh:mikke89/RmlUi#58c7515")

# HWINFO nefunguje pro Windows
if (NOT WIN32)
    CPMAddPackage("gh:C-And-Cpp-Libraries/hwinfo-machine-id#cf9fa44")
    target_link_libraries(LibsBundle INTERFACE lfreist-hwinfo::hwinfo)
else()
    target_link_libraries(LibsBundle INTERFACE dxgi)
endif()

target_link_libraries(LibsBundle INTERFACE
        SDL3::SDL3
        SDL3_image::SDL3_image
        nlohmann_json::nlohmann_json
        RmlUi::RmlUi
        lua::lib
        sol2::sol2
)





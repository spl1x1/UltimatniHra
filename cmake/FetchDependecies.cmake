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
#CPMAddPackage("gh:libsdl-org/SDL_ttf#5e1293a",)

add_library(Freetype::Freetype ALIAS freetype)
add_library(SDL::SDL ALIAS SDL3-static )
add_library(SDL_image::SDL_image ALIAS SDL3_image-static)

set(FREETYPE_FOUND TRUE)
set(FREETYPE_LIBRARY freetype)
set(FREETYPE_LIBRARIES freetype)
set(FREETYPE_INCLUDE_DIRS ${freetype_SOURCE_DIR}/include)

CPMAddPackage(
        NAME SDL3_ttf
        GITHUB_REPOSITORY libsdl-org/SDL_ttf
        GIT_TAG 5e1293a
        OPTIONS
        "SDL3TTF_VENDORED=OFF"
)

#other libs
CPMAddPackage("gh:simdjson/simdjson#8b69401")
CPMAddPackage("gh:mikke89/RmlUi#58c7515")
CPMAddPackage("gh:ocornut/imgui#683f916")
#CPMAddPackage("gh:ValveSoftware/GameNetworkingSockets#fa569cb") #nepouzivame sockets knihovna

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
        SDL3_ttf::SDL3_ttf
        RmlUi::RmlUi
        simdjson::simdjson
)





cmake_minimum_required(VERSION 3.31)
project(UltimatniHra)

set(CMAKE_CXX_STANDARD 20)


include(cmake/CPM.cmake)

CPMAddPackage("gh:libsdl-org/SDL#a8589a8")
CPMAddPackage("gh:libsdl-org/SDL_image#11154af")
CPMAddPackage("gh:libsdl-org/SDL_mixer#9cc5316")


add_executable(UltimatniHra main.cpp)

target_link_libraries(UltimatniHra
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_mixer::SDL3_mixer)

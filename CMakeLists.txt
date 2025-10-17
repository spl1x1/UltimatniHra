cmake_minimum_required(VERSION 3.31)
project(UltimatniHra)

set(CMAKE_CXX_STANDARD 20)

include(cmake/CPM.cmake)

CPMAddPackage("gh:libsdl-org/SDL#a8589a8")
CPMAddPackage("gh:libsdl-org/SDL_image#11154af")
CPMAddPackage("gh:nlohmann/json#55f9368")


add_executable(UltimatniHra
        main.cpp
        server/Server.cpp
        server/Server.h
)

target_link_libraries(UltimatniHra
        SDL3::SDL3
        SDL3_image::SDL3_image
        nlohmann_json::nlohmann_json
        )

add_custom_command(TARGET UltimatniHra POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3::SDL3>
        $<TARGET_FILE:SDL3_image::SDL3_image>
        ${CMAKE_BINARY_DIR})

add_custom_command(TARGET UltimatniHra POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "${CMAKE_BINARY_DIR}/assets")



cmake_minimum_required(VERSION 3.31)
project(UltimatniHra)

set(CMAKE_CXX_STANDARD 20)

add_custom_target(copy_assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "src/assets"
        "${CMAKE_BINARY_DIR}/src/assets"
        COMMENT "Copying assets directory")

add_subdirectory(src)
cmake_minimum_required(VERSION 3.31...4.1)
project(
        UltimatniHra
        VERSION 1.0
        DESCRIPTION "Semestralni projekt"
        LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 20)

include(cmake/FetchDependecies.cmake)
include(cmake/ImguiDependency.cmake)

add_custom_target(copy_assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "../assets"
        "${CMAKE_BINARY_DIR}/src/assets"
        COMMENT "Copying assets directory")

add_subdirectory(src)

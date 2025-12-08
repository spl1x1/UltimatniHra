cmake_minimum_required(VERSION 3.31)
project(UltimatniHra)
set(CMAKE_CXX_STANDARD 20)


include(cmake/FetchDependecies.cmake)

add_custom_target(copy_assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "../assets"
        "${CMAKE_BINARY_DIR}/src/assets"
        COMMENT "Copying assets directory")

add_subdirectory(src)
target_link_libraries(UltimatniHra LibsBundle)
add_dependencies(UltimatniHra copy_assets)

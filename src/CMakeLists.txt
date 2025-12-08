cmake_minimum_required(VERSION 3.31)
project(UltimatniHra)

set(CMAKE_CXX_STANDARD 20)


file(GLOB HEADERS
        "../include/*.h"
        "../include/**/*.h"
)

file(GLOB SOURCES
        "*.cpp"
        "**/*.cpp"
)

add_executable(UltimatniHra
        ${HEADERS}
        ${SOURCES}
)
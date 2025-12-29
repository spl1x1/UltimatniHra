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
        Application/SaveGame.cpp
        ../include/Application/SaveGame.h
)

target_link_libraries(ImguiDependency
        SDL3::SDL3)

target_link_libraries(UltimatniHra
        LibsBundle
        ImguiDependency
)


add_dependencies(UltimatniHra copy_assets)
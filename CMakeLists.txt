cmake_minimum_required(VERSION 3.12)

project(canary-ui C)

set(CMAKE_C_STANDARD 11)

add_subdirectory(lvgl)

find_package(SDL2 REQUIRED)

add_executable(canary-ui src/main.c)

target_link_libraries(canary-ui PRIVATE
    lvgl
    SDL2::SDL2
)

target_include_directories(canary-ui PRIVATE
    ${CMAKE_SOURCE_DIR}
)
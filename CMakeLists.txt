cmake_minimum_required(VERSION 3.10)

project(pac-man C)

find_package(raylib REQUIRED)

add_executable(main src/main.c src/dijkstra.c)

target_link_libraries(main raylib)

set_property(TARGET main PROPERTY C_STANDARD 99)

target_include_directories(main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

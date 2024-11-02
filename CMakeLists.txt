cmake_minimum_required(VERSION 3.10)
project(pac-man C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS_RELEASE "-Wextra -Wall -O3")
set(CMAKE_C_FLAGS_DEBUG "-Wextra -Wall -DDBG")

include(FetchContent)
find_package(raylib)
if(NOT raylib_FOUND)
    set(FETCHCONTENT_QUIET FALSE)
    set(BUILD_EXAMPLES OFF CACHE INTERNAL "")

    set(RAYLIB_VERSION 5.0)
    FetchContent_Declare(
        raylib
        URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
        FIND_PACKAGE_ARGS ${RAYLIB_VERSION} EXACT
    )

    FetchContent_MakeAvailable(raylib)
endif()

add_executable(main src/main.c src/dijkstra.c)
target_link_libraries(main raylib m)
set_property(TARGET main PROPERTY C_STANDARD 99)
target_include_directories(main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR/src})

cmake_minimum_required(VERSION 3.10)
project(pime)

set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(../../core core)

add_library(server_cpp SHARED
    world_server.cpp player_server.cpp
    networking.cpp elements_server.cpp
    craft.cpp

    tools/axe_blade.cpp
    tools/axe_handle.cpp
    tools/axe.cpp

    tools/knife_blade.cpp
    tools/knife_handle.cpp
    tools/knife.cpp
)

target_link_libraries(server_cpp pime)

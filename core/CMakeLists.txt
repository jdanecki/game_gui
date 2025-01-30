cmake_minimum_required(VERSION 3.10)
project(pime)

set(CMAKE_BUILD_TYPE Debug)

add_library(pime SHARED
    world.cpp player.cpp 
    tiles.cpp

    alchemist/elements.cpp
	alchemist/names.cpp
	alchemist/el_list.cpp
	)

target_link_libraries(pime)

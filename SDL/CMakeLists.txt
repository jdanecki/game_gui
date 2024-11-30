cmake_minimum_required(VERSION 3.10)
project(pime)

find_package(PkgConfig)
pkg_check_modules(SDL sdl2)
pkg_check_modules(SDL2_IMAGE SDL2_image)
pkg_check_modules(SDL2_TTF SDL2_ttf)
pkg_check_modules(SDL2_MIXER SDL2_mixer)

set(CMAKE_BUILD_TYPE Debug)

add_definitions(-DFUNNY_STUFF_FOR_SDL)
# uncomment this for single player game
#add_definitions(-DLOCAL_ONLY)

add_subdirectory(../core core)

add_executable(pime_SDL
	window.cpp music.cpp text.cpp  
	texture.cpp main.cpp menu.cpp
    engine_hooks.cpp networking.cpp
    
    implementations/alchemistSDL.cpp
    implementations/BeingSDL.cpp
)

# link_directories(${CMAKE_SOURCE_DIR}/core)

target_link_libraries(pime_SDL
    SDL2 SDL2_image SDL2_ttf SDL2_mixer
    ${SDL_LDFLAGS} ${SDL2_MIXER_LDFLAGS}
    -lm 
    pime
    /home/kamil/data/projects/the_game/the_game_net/target/debug/libthe_game_net.so
)

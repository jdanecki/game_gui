# The Game aka pime aka alchemist

## build

To build server in server directory run:

    cargo build

To build SDL client in SDL directory run:

    cmake -Bbuild
    cmake --build ./build

## run

### Server
In server directory

    cargo run

### SDL Client
In SDL directory

    ./build/pimeSDL

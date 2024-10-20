#ifndef BEING_SDL_H
#define BEING_SDL_H

#include "../../core/alchemist/elements.h"

class BeingSDL : public Being
{
    public:
        SDL_Texture * get_texture();
}

class PlantSDL : public Plant
{
    public:
        SDL_Texture * get_texture();

};

class AnimalSDL : public Animal
{
    public:
        SDL_Texture * get_texture();
        bool going_right;
}


#endif

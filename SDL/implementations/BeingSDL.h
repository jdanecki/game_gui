#ifndef BEING_SDL_H
#define BEING_SDL_H

#include "../../core/alchemist/elements.h"
#include "alchemistSDL.h"
#include "../texture.h"

class BeingSDL : public Being, public Renderable
{
    public:
        SDL_Texture * get_texture() override;
};

class PlantSDL : public Plant, public Renderable
{
    public:
        SDL_Texture * get_texture() override;

};

class AnimalSDL : public Animal, public Renderable
{
    public:
        SDL_Texture * get_texture() override;
        bool going_right;
};


#endif

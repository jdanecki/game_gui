#ifndef BEING_SDL_H
#define BEING_SDL_H

#include "../../core/alchemist/elements.h"
#include "../texture.h"
#include "alchemistSDL.h"

class BeingSDL : public Being, public Renderable
{
  public:
    SDL_Texture * get_texture() override;
};

class PlantSDL : public Plant, public Renderable
{
    int * padding1; // FIXME
  public:
    SDL_Texture * get_texture() override;
    PlantSDL(PlantData * data);
};

class AnimalSDL : public Animal, public Renderable
{
  public:
    SDL_Texture * get_texture() override;
    bool going_right;
    AnimalSDL(AnimalData * data);
    void render(SDL_Rect * rect)
    {
        flip = !going_right;
        Renderable::render(rect);
    }
    void update_item_location(ItemLocation & old_loc, ItemLocation & new_loc);
};

#endif

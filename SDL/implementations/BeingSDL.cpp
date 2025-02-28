#include "BeingSDL.h"

SDL_Texture * BeingSDL::get_texture()
{
    return nullptr;
}

SDL_Texture * AnimalSDL::get_texture()
{
    return animal_textures[get_id()];
}

AnimalSDL::AnimalSDL(AnimalData * data) : Animal(data->id)
{
    age->value = data->age;
    max_age->value = data->max_age;
}

void AnimalSDL::update_item_location(ItemLocation & old_loc, ItemLocation & new_loc)
{
    going_right = (old_loc.data.chunk.x < new_loc.data.chunk.x) ? true : false;
}

SDL_Texture * PlantSDL::get_texture()
{
    // FIXME combine plant textures in one file for each phase
    // if (grown)
    return plant_textures[get_id()];
}

PlantSDL::PlantSDL(PlantData * data) : Plant(data->id)
{
    grown = data->grown;
    age->value = data->age;
    max_age->value = data->max_age;
}

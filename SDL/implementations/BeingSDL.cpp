#include "BeingSDL.h"

SDL_Texture * BeingSDL::get_texture()
{
    return being_textures[type];
}

SDL_Texture * AnimalSDL::get_texture()
{
    return animal_textures[type];
}

AnimalSDL::AnimalSDL(AnimalData * data)
{
    age->value = data->age;
    max_age->value = data->max_age;
    type = data->id;
}

void AnimalSDL::update_item_location(ItemLocation & old_loc, ItemLocation & new_loc)
{
    going_right = (old_loc.data.chunk.x < new_loc.data.chunk.x) ? true : false;
}

SDL_Texture * PlantSDL::get_texture()
{
    if (grown)
        return grown_plant_textures[type];
    else
        return plant_textures[type];
}

PlantSDL::PlantSDL(PlantData * data)
{
    type = data->id;
    grown = data->grown;
    age->value = data->age;
    max_age->value = data->max_age;
}

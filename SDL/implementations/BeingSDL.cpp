#include "BeingSDL.h"

SDL_Texture * BeingSDL::get_texture()
{
    return being_textures[type];
}

SDL_Texture * AnimalSDL::get_texture()
{
    if (going_right)
        return animalr_textures[type];
    else
        return animall_textures[type];
    return animall_textures[type];
}

AnimalSDL::AnimalSDL(AnimalData * data)
{
    age->value = data->age;
    max_age->value = data->max_age;
    type = data->id;
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

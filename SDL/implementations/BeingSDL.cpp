#include "BeingSDL.h"

SDL_Texture * BeingSDL::get_texture()
{
    return being_textures[type];
}

SDL_Texture * Animal::get_texture()
{
    if (going_right)
        return animalr_textures[type];
    else
        return animall_textures[type];
    return animall_textures[type];
}

SDL_Texture * Plant::get_texture()
{
    if (grown)
        return grown_plant_textures[type];
    else
        return plant_textures[type];
}

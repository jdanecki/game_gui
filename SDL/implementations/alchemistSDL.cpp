#include "alchemistSDL.h"

SDL_Texture * ObjectSDL::get_texture()
{
    if (type == OBJECT_wall)
    {
        switch (base->id)
        {
            case ID_STONE:
                return object_textures[TEXTURE_stone_wall];
                break;
            case ID_LOG:
                return object_textures[TEXTURE_log_wall];
                break;
            case ID_LOG1:
                return object_textures[TEXTURE_log1_wall];
                break;
            case ID_LOG2:
                return object_textures[TEXTURE_log2_wall];
                break;
        }
    }
    return NULL;
}

ElementSDL::ElementSDL(int id) : Element(base_elements[id])
{
}

SDL_Texture * ElementSDL::get_texture()
{
    return items_textures[get_base()->id];
}

IngredientSDL::IngredientSDL(int id) : Ingredient(nullptr, (Ingredient_id)id, Form_none)
{
}

SDL_Texture * IngredientSDL::get_texture()
{
    return ing_textures[id];
}

ProductSDL::ProductSDL(int id) : Product(nullptr, nullptr, (Product_id)id, Form_none)
{
}

SDL_Texture * ProductSDL::get_texture()
{
    return prod_textures[id];
}

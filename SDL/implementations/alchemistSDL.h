#ifndef ALCHEMIST_SDL_H
#define ALCHEMIST_SDL_H

#include "../core/alchemist/elements.h"

class IngredientSDL : public Ingredient
{
    public:
        SDL_Texture * get_texture();
};

class ProductSDL : public Product
{
    public:
        SDL_Texture * get_texture();

};

class InventoryElementSDL : public InventoryElement
{
    public:
        virtual SDL_Texture * get_texture() {return NULL; }
};

class ObjectSDL : public Object
{
    public:
        SDL_Texture * get_texture();

};

class ElementSDL : public Element
{
    public:
        SDL_Texture * get_texture();
};

#endif


#ifndef ALCHEMIST_SDL_H
#define ALCHEMIST_SDL_H

#include "../../core/alchemist/elements.h"
#include "../texture.h"

class Renderable
{
  public:
    virtual SDL_Texture * get_texture()
    {
        return NULL;
    }
};

class IngredientSDL : public Ingredient, public Renderable
{
  public:
    SDL_Texture * get_texture();
    IngredientSDL(int id);
};

class ProductSDL : public Product, public Renderable
{
  public:
    SDL_Texture * get_texture();
    ProductSDL(int id);
};

class ObjectSDL : public Object, public Renderable
{
  public:
    SDL_Texture * get_texture();
    ObjectSDL(int id);
};

class ElementSDL : public Element, public Renderable
{
  public:
    SDL_Texture * get_texture();
    ElementSDL(int id);
};

#endif

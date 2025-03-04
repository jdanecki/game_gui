#ifndef MENU_H
#define MENU_H

#include "../core/alchemist/elements.h"
#include "text.h"

enum menu_actions
{
    MENU_CANCEL,
    MENU_SAVE_EXIT,
    MENU_EXIT,
    MENU_SAVE,
    MENU_LOAD,
    MENU_HELP,
    MENU_REGAIN,
    MENU_LOUDER,
    MENU_QUIETER,
    MENU_MUSIC,
    MENU_INV_SOLID,
    MENU_INV_LIGQUID,
    MENU_INV_GAS,
    MENU_CRAFT_AXE_BLADE,
    MENU_CRAFT_AXE_HANDLE,
    MENU_CRAFT_AXE,
    MENU_CRAFT_KNIFE_BLADE,
    MENU_CRAFT_KNIFE_HANDLE,
    MENU_CRAFT_KNIFE,
    MENU_GET_AXE,
    MENU_GET_KNIFE,
    MENU_GET_RANDOM_ELEMENT,
    MENU_GET_RANDOM_EDIBLE,
    MENU_BUILD_WALL,
    MENU_CATEGORIE,

    MENU_NPC,
    MENU_NPC_SAY,

    // must be the last
    MENU_ITEM = 0x1000,
};

class Menu
{
  public:
    const char * name;
    int options;
    int menu_pos;
    int added;
    SDL_Texture ** texture;
    const char ** entries;
    enum menu_actions * actions;
    int * values;
    Element ** el;
    bool show_texture;
    bool show_texture_literal;
    Menu(const char * n, int opt);
    void add(const char * e, enum menu_actions a);
    void add(const char * e, enum menu_actions a, int val);
    void add(const char * e, enum menu_actions a, Element * p_el);
    void add(const char * e, enum menu_actions a, SDL_Texture * _texture, int index, int item_id);
    int get_val(int v);
    int get_val();
    void show();
    void go_down();
    void go_up();
};

extern Menu * menu_main;
extern Menu * menu_energy;
extern Menu * menu_help;
extern Menu * menu_help2;
extern Menu * menu_crafting;
extern Menu * current_menu;
extern Menu * menu_npc;
extern Menu * menu_dialog;

int interact(enum menu_actions a);
void show_menu();
void create_menus();
int menu_interact(int key);

#endif

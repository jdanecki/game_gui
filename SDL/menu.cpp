#include "menu.h"
#include "../core/alchemist/elements.h"
#include "../core/player.h"
#include "../core/tiles.h"
#include "../core/world.h"
#include "music.h"
#include "networking.h"
#include "npc.h"
#include "text.h"
#include "texture.h"
#include "window.h"
#include <cstddef>
#include <cstdio>
#include <cstdlib>

extern class Player * player;
extern int active_hotbar;
extern const NetClient * client;

Menu * menu_music;
Menu * menu_main;
Menu * menu_help;
Menu * current_menu;
Menu * menu_inventory_categories;
Menu * menu_inventory;
Menu * menu_inventory_categories2;
Menu * menu_crafting;
Menu * menu_dev;
Menu * menu_build;
Menu * menu_npc;
Menu * menu_dialog;

void load(char with_player);
void save(char with_player);

Menu::Menu(const char * n, int opt)
{
    name = n;
    options = opt;
    menu_pos = 0;
    added = 0;
    entries = new Menu_entry *[options];
    show_texture = false;
}

Menu::~Menu()
{
    delete[] entries;
}
void Menu::add(const char * e, enum menu_actions a)
{
    entries[added] = new Menu_entry(e, a, 0, nullptr, nullptr);
    added++;
}

void Menu::add(const char * e, enum menu_actions a, int val)
{
    entries[added] = new Menu_entry(e, a, val, nullptr, nullptr);
    added++;
}

void Menu::add(const char * e, enum menu_actions a, SDL_Texture * t, int index, int item_id)
{
    // FIXME why is index here?
    entries[index] = new Menu_entry(e, a, item_id, nullptr, t);
    added++;
}

void Menu::add(const char * e, enum menu_actions a, Sentence * s, InventoryElement * p_el)
{
    entries[added] = new Menu_entry(e, a, s, p_el);
    added++;
}
void Menu::add(const char * e, enum menu_actions a, int val, InventoryElement * p_el)
{
    entries[added] = new Menu_entry(e, a, val, p_el, nullptr);
    added++;
}
void Menu::add(const char * e, enum menu_actions a, InventoryElement * p_el)
{
    entries[added] = new Menu_entry(e, a, 0, p_el, nullptr);
    added++;
    show_texture = false;
}

int Menu::get_val(int v)
{
    for (int i = 0; i < added; i++)
    {
        if (entries[i]->action == v)
            return entries[i]->value;
    }
    return 0;
}

int Menu::get_val()
{
    return entries[menu_pos]->value;
}

Sentence * Menu::get_sentence()
{
    return entries[menu_pos]->sentence;
}

InventoryElement * Menu::get_el()
{
    return entries[menu_pos]->el;
}

void Menu::show()
{
    int window_width;
    int window_height;
    int i;
    int game_size;

    SDL_GetWindowSize(main_window, &window_width, &window_height);

    if (window_width < window_height)
        game_size = window_width;
    else
        game_size = window_height;

    int menu_opt_size = game_size / 10;
    int mody;
    int mody2;

    if (options % 2)
    {
        mody = (game_size / 2) - (menu_opt_size * (options / 2) + menu_opt_size / 2);
        mody2 = (game_size / 2) + (menu_opt_size * (options / 2) + menu_opt_size / 2);
    }
    else
    {
        mody = (game_size / 2) - (menu_opt_size * (options / 2));
        mody2 = (game_size / 2) + (menu_opt_size * (options / 2));
    }

    int modx = int((game_size / 2)) - (0.4 * game_size);
    int modx2 = int((game_size / 2)) + (0.4 * game_size);

    SDL_Rect rect = {modx, mody, modx2 - modx, mody2 - mody};
    if (!(options < 10))
        rect = (SDL_Rect){modx, mody + (-menu_pos + options / 2) * menu_opt_size, modx2 - modx, menu_opt_size * options};
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 100);
    SDL_RenderFillRect(renderer, &rect);

    int mody3 = mody + (((menu_pos + 1) * menu_opt_size) - (menu_opt_size));
    int mody4 = mody + ((menu_pos + 1) * menu_opt_size);

    // THIS IS THE SELECT
    if (options < 10)
    {
        draw_rectangle(modx, mody3, modx2 - modx, mody4 - mody3, SDL_Color{0, 255, 255, 255}, SDL_Color{0, 255, 255, 255}, SDL_Color{}, SDL_Color{});
        draw_rectangle(modx, mody3, modx2 - modx, mody4 - mody3, SDL_Color{}, SDL_Color{}, SDL_Color{0, 255, 255, 255}, SDL_Color{0, 255, 255, 255});
    }
    else
    {
        if (options % 2)
        {
            draw_rectangle(modx, game_size / 2 - menu_opt_size / 2, modx2 - modx, menu_opt_size, SDL_Color{0, 255, 255, 255}, SDL_Color{0, 255, 255, 255}, SDL_Color{}, SDL_Color{});
            draw_rectangle(modx, game_size / 2 - menu_opt_size / 2, modx2 - modx, menu_opt_size, SDL_Color{}, SDL_Color{}, SDL_Color{0, 255, 255, 255}, SDL_Color{0, 255, 255, 255});
        }
        else
        {
            draw_rectangle(modx, game_size / 2, modx2 - modx, menu_opt_size, SDL_Color{0, 255, 255, 255}, SDL_Color{0, 255, 255, 255}, SDL_Color{}, SDL_Color{});
            draw_rectangle(modx, game_size / 2, modx2 - modx, menu_opt_size, SDL_Color{}, SDL_Color{}, SDL_Color{0, 255, 255, 255}, SDL_Color{0, 255, 255, 255});
        }
    }

    if (options < 10)
    {
        SDL_Rect rect3 = {modx, mody - menu_opt_size, modx2 - modx, mody4 - mody3};
        SDL_SetRenderDrawColor(renderer, 150, 0, 150, 100);
        SDL_RenderFillRect(renderer, &rect3);
        write_text(modx, mody - menu_opt_size, name, Yellow, game_size / 27, menu_opt_size);
    }
    else
    {
        SDL_Rect rect3 = {modx, mody + (-menu_pos + options / 2) * menu_opt_size - menu_opt_size, modx2 - modx, menu_opt_size};
        SDL_SetRenderDrawColor(renderer, 150, 0, 150, 100);
        SDL_RenderFillRect(renderer, &rect3);
        write_text(modx, mody + (-menu_pos + options / 2) * menu_opt_size - menu_opt_size, name, Yellow, game_size / 27, menu_opt_size);
    }
    for (i = 0; i < options; i++)
    {
        SDL_Rect rect;
        int text_y = mody + i * menu_opt_size;
        int text_x = modx;
        if (options < 10)
        {
            rect.x = modx;
            rect.y = mody + i * menu_opt_size;
            rect.w = menu_opt_size;
            rect.h = menu_opt_size;
        }
        else
        {
            rect.x = modx;
            rect.y = mody + (i - menu_pos + options / 2) * menu_opt_size;
            rect.w = menu_opt_size;
            rect.h = menu_opt_size;
        }
        if (entries[i]->texture)
        {
            SDL_RenderCopy(renderer, entries[i]->texture, NULL, &rect);
            text_x += menu_opt_size;
        }
        if (show_texture)
        {
            // TODO fix that
            // SDL_Texture *_texture = el[i]->get_texture();
            // SDL_RenderCopy(renderer, _texture, NULL, &rect);
            // text_x+=menu_opt_size;
        }
        if (options >= 10)
            text_y = mody + (i - menu_pos + options / 2) * menu_opt_size;

        if (entries[i])
            write_text(text_x, text_y, entries[i]->entry, White, game_size / 27, menu_opt_size);
    }
}

Menu_entry::Menu_entry(const char * e, enum menu_actions a, int v, InventoryElement * _el, SDL_Texture * t)
{
    texture = t;
    action = a;
    value = v;
    el = _el;
    if (el)
    {
        entry = new char[64];
        sprintf(entry, "%s %s", e, el->get_name());
        dynamic_entry = true;
    }
    else
    {
        entry = (char *)e;
        dynamic_entry = false;
    }
}

Menu_entry::Menu_entry(const char * e, menu_actions a, Sentence * s, InventoryElement * _el)
{
    texture = nullptr;
    action = a;
    el = _el;
    sentence = s;
    if (el)
    {
        entry = new char[64];
        sprintf(entry, "%s item?", e);
        dynamic_entry = true;
    }
    else
    {
        entry = (char *)e;
        dynamic_entry = false;
    }
}

Menu_entry::~Menu_entry()
{
    if (dynamic_entry)
        delete entry;
}

void create_menus()
{
    menu_main = new Menu("Main", 4);
    menu_main->add("Exit", MENU_EXIT);
    // add("Save & Exit", MENU_SAVE_EXIT);
    // add("Save", MENU_SAVE);
    // add("Load", MENU_LOAD);
    menu_main->add("Help", MENU_HELP);
    menu_main->add("Change music volume", MENU_MUSIC);
    menu_main->add("Cancel", MENU_CANCEL);

    menu_help = new Menu("Help 1", 12);
    menu_help->add("; - show item info", MENU_CANCEL);
    menu_help->add("f11 - resize", MENU_CANCEL);
    menu_help->add("1-9,0 - hotbar", MENU_CANCEL);
    //  menu_help->add("q - drop item", MENU_CANCEL);
    menu_help->add("` - previous item", MENU_CANCEL);
    menu_help->add("tab - next item", MENU_CANCEL);
    //   menu_help->add("minus - deselect hotbar", MENU_CANCEL);
    menu_help->add("esc - main menu", MENU_CANCEL);
    //   menu_help->add("l - devmenu", MENU_CANCEL);
    menu_help->add("c - crafting", MENU_CANCEL);
    menu_help->add("i - inventory", MENU_CANCEL);
    //  menu_help->add("v - clear statusline", MENU_CANCEL);
    //   menu_help->add("g - terrain break", MENU_CANCEL);
    //  menu_help->add("r - remove from hotbar", MENU_CANCEL);
    //  menu_help->add("= - select hotbar", MENU_CANCEL);
    //  menu_help->add("F5 - autoexplore", MENU_CANCEL);
    //  menu_help->add("F4 - item info at player", MENU_CANCEL);
    menu_help->add("e / enter - use", MENU_CANCEL);
    menu_help->add("shift/control - sneak/run", MENU_CANCEL);
    menu_help->add("wasd+arrows - move", MENU_CANCEL);
    menu_help->add("n - NPC", MENU_CANCEL);

    menu_music = new Menu("Music", 3);
    menu_music->add("+5 Volume", MENU_LOUDER);
    menu_music->add("-5 Volume", MENU_QUIETER);
    menu_music->add("Cancel", MENU_CANCEL);

    menu_inventory_categories = new Menu("Inventory categories", 4);
    menu_inventory_categories->add("Solid form", MENU_INV_SOLID, Form_solid);
    menu_inventory_categories->add("Liquid form", MENU_INV_LIGQUID, Form_liquid);
    menu_inventory_categories->add("Gas form", MENU_INV_GAS, Form_gas);
    menu_inventory_categories->add("Cancel", MENU_CANCEL);

    menu_crafting = new Menu("Crafting", 5);
    menu_crafting->add("Axe blade (1 ing.)", MENU_CRAFT_AXE_BLADE);
    menu_crafting->add("Axe handle (1 ing.)", MENU_CRAFT_AXE_HANDLE);
    menu_crafting->add("Axe (2 ing.)", MENU_CRAFT_AXE);
    menu_crafting->add("Wall (1 ing.)", MENU_CRAFT_WALL);
    menu_crafting->add("Cancel", MENU_CANCEL);
    // FIXME
    /*
        menu_crafting->add("Knife blade (1 ing.)", MENU_CRAFT_KNIFE_BLADE);
        menu_crafting->add("Knife handle (1 ing.)", MENU_CRAFT_KNIFE_HANDLE);
        menu_crafting->add("Knife (2 ing.)", MENU_CRAFT_KNIFE);
    */

    // FIXME
    /*
    menu_build = new Menu("build", 1);
    menu_build->add("Wall (1 ing.)", MENU_BUILD_WALL);

    menu_dev = new Menu("dev options", 4);
    menu_dev->add("axe", MENU_GET_AXE);
    menu_dev->add("knife", MENU_GET_KNIFE);
    menu_dev->add("random element", MENU_GET_RANDOM_ELEMENT);
    menu_dev->add("random edible", MENU_GET_RANDOM_EDIBLE);
    menu_dev->add("food/water +100", MENU_REGAIN);
*/
    menu_npc = new Menu("NPC", 2);
    menu_npc->add("Talk to NPC", MENU_NPC);
    menu_npc->add("Cancel", MENU_CANCEL);
}

Menu * create_inv_category_menu(enum Form f)
{
    int count = 0;
    InventoryElement ** elements_with_form = player->inventory->find_form(f, &count);
    if (!count)
        return nullptr;

    if (menu_inventory_categories2)
    {
        delete menu_inventory_categories2;
    }
    char * menu_name = new char[50];
    sprintf(menu_name, "Inventory: %s", Form_name[f]);

    menu_inventory_categories2 = new Menu(menu_name, count);
    // int menu_index = 0;
    for (int i = 0; i < count; i++)
    {
        // FIXME add texture
        // menu_inventory_categories2->add(base_elements[i]->name, MENU_CATEGORIE, items_textures[i], menu_index, i);
        menu_inventory_categories2->add("*", MENU_CATEGORIE, elements_with_form[i]);
        //        menu_index++;
    }
    return menu_inventory_categories2;
}
// FIXME
#if 0
void create_inv_menu(Item_id id)
{
    printf("szukam %d\n", id);
    int c = 0;
    InventoryElement ** i_el = player->inventory->find_id(id, &c);
    if (i_el)
    {
        printf("found %d elements\n", c);

        if (menu_inventory)
            delete menu_inventory;

        menu_inventory = new Menu("Which element?", c);
        Element ** el = (Element **)i_el;
        for (int i = 0; i < c; i++)
        {
            printf("%s\n", el[i]->get_name());
            menu_inventory->add("->", (menu_actions)(MENU_ITEM + i), el[i]);
        }
        free(el);
        current_menu = menu_inventory;
        return;
    }
    current_menu = nullptr;
}
#endif
void Menu::go_down()
{
    menu_pos++;
    if (menu_pos == options)
        menu_pos = 0;
}

void Menu::go_up()
{
    menu_pos--;
    if (menu_pos < 0)
        menu_pos = options - 1;
}

int menu_interact(int key)
{
    switch (key)
    {
        case SDLK_ESCAPE:
        {
            if (current_menu)
                current_menu = NULL;
            else
                current_menu = menu_main;
            return 1;
        }
        case SDLK_b:
        {
            if (!current_menu)
                current_menu = menu_build;
            else if (current_menu == menu_build)
                current_menu = NULL;

            return 1;
        }
        case SDLK_c:
        {
            if (!current_menu)
                current_menu = menu_crafting;
            else if (current_menu == menu_crafting)
                current_menu = NULL;
            return 1;
        }
        case SDLK_RETURN:
        case SDLK_e:
        {
            if (current_menu)
            {
                if (current_menu->interact())
                {
                    current_menu = NULL;
                    return 1;
                }
            }
            break;
        }
        case SDLK_i:
        {
            player->inventory->show();
            if (!current_menu)
                current_menu = menu_inventory_categories;
            else if (current_menu == menu_inventory_categories)
                current_menu = NULL;
            return 1;
        }
        case SDLK_l:
        {
            if (current_menu)
                current_menu = NULL;
            else
                current_menu = menu_dev;
            return 1;
        }
        case SDLK_n:
        {
            if (!current_menu)
                current_menu = menu_npc;
            else if (current_menu == menu_npc)
                current_menu = NULL;

            return 1;
        }

        case SDLK_DOWN:
        case SDLK_s:
        {
            if (current_menu)
                current_menu->go_down();
            break;
        }
        case SDLK_w:
        case SDLK_UP:
        {
            if (current_menu)
                current_menu->go_up();
            break;
        }
    }
    return current_menu ? 1 : 0;
}

int Menu::handle_item(int i)
{
    if (active_hotbar >= 0)
    {
        InventoryElement * el = entries[i]->el;

        for (int h = 0; h < 10; h++)
        {
            if (player->hotbar[h] == el)
                return 0;
        };
        player->hotbar[active_hotbar] = el;
    }
    return 1;
}

bool craft2elements(Product_id what)
{
    InventoryElement *el1 = nullptr, *el2 = nullptr;
    int i1, i2;
    for (int i = 0; i < 10; i++)
    {
        if (player->craftbar[i])
        {
            if (!el1)
            {
                el1 = player->hotbar[i];
                i1 = i;
            }
            else
            {
                el2 = player->hotbar[i];
                i2 = i;
                break;
            }
        }
    }
    if (el1 && el2)
    {
        size_t ingredients[2] = {el1->uid, el2->uid};
        send_packet_craft(client, ING_NUM + what, 2, ingredients);
        player->craftbar[i1] = 0;
        player->craftbar[i2] = 0;
        return true;
    }
    return false;
}

int craft(menu_actions a)
{
    //    InventoryElement * el = NULL;

    switch (a)
    {
            // FIXME
            /*
                    case MENU_CRAFT_KNIFE_BLADE: el = craft_knife_blade(); break;
                    case MENU_CRAFT_KNIFE_HANDLE: el = craft_knife_handle(); break;
                    case MENU_CRAFT_KNIFE: el = craft_knife(); break;
            */
        case MENU_CRAFT_AXE_BLADE:
            send_packet_craft(client, ING_AXE_BLADE, 1, &player->hotbar[active_hotbar]->uid);
            goto sent;

        case MENU_CRAFT_AXE_HANDLE:
            send_packet_craft(client, ING_AXE_HANDLE, 1, &player->hotbar[active_hotbar]->uid);
            goto sent;

        case MENU_CRAFT_AXE:
            if (craft2elements(PROD_AXE))
                goto sent;
            break;

        case MENU_CRAFT_WALL:
            send_packet_craft(client, ING_WALL, 1, &player->hotbar[active_hotbar]->uid);
            goto sent;

        default:
            break;
    }
    return 0;

sent:
    sprintf(status_line, "Starting crafting");
    return 1;
}

int Menu::interact()
{
    menu_actions a = entries[menu_pos]->action;

    if (a & MENU_ITEM)
        return menu_inventory->handle_item(a & ~MENU_ITEM);
    switch (a)
    {
            // FIXME - doesn't work for beings
        case MENU_CATEGORIE:
        { // FIXME don't use elements'id but c_id and type
            // create_inv_menu((Item_id)(menu_inventory_categories2->get_val()));
            return 0;
        }

        case MENU_BUILD_WALL:
        {
            sprintf(status_line, "Starting building");
            /*if (player.hotbar[active_hotbar] && player.hotbar[active_hotbar]->get_form() == Form_solid)
            {
                Object * ob = new Object();
                ob->base = ((Element *)(player.hotbar[active_hotbar]))->get_base();

                if (!(ob->base->id == ID_LOG || ob->base->id == ID_LOG1 || ob->base->id == ID_LOG2 || ob->base->id == ID_STONE))
                {
                    status_code = 0;
                    free(ob);
                    break;
                }

                player.inventory->remove(player.hotbar[active_hotbar]);
                player.hotbar[active_hotbar]=NULL;

                ob->type = OBJECT_wall;
                ob->set_posittion(player.x, player.y);

                for (int i = 0; i < CHUNK_SIZE*CHUNK_SIZE; i++)
                {
                    if (!world_table[player.map_y][player.map_x]->objects[i])
                    {
                        world_table[player.map_y][player.map_x]->objects[i] = ob;
                        break;
                    }
                }

                status_code = 1;
            }
            else {
                status_code = 0;
            }*/
            break;
        }

        /*case MENU_GET_AXE:
        {
            Element * el1=new Element(base_elements[0]);
            Element * el2=new Element(base_elements[1]);
            sprintf(status_line, "Creating axe...");
            status_code=1;
            Axe * axe=new Axe(el1, el2);
            player.inventory->add(axe);
            player.hotbar[active_hotbar]=axe;
            break;
        }
        case MENU_GET_KNIFE:
        {
            Element * el1=new Element(base_elements[0]);
            Element * el2=new Element(base_elements[1]);
            sprintf(status_line, "Creating knife...");
            status_code=1;
            Knife * knife=new Knife(el1, el2);
            player.inventory->add(knife);
            player.hotbar[active_hotbar]=knife;
            break;
        }
        case MENU_GET_RANDOM_ELEMENT:
        {
            Element * el=new Element(base_elements[rand() % BASE_ELEMENTS]);
            player.inventory->add(el);
            player.hotbar[active_hotbar]=el;
            break;
        }
        case MENU_GET_RANDOM_EDIBLE:
        {
            Element * el=new Element(base_elements[rand() % FOOD_ELEMENTS + NOT_FOOD_ELEMENTS]);
            player.inventory->add(el);
            player.hotbar[active_hotbar]=el;
            break;
        }
        case MENU_MUSIC:
            current_menu=menu_music;
            return 0;
        case MENU_REGAIN:
            player.hunger+=100;
            player.thirst+=100;
            return 0;*/
        case MENU_SAVE:
            save(1);
            break;

        case MENU_SAVE_EXIT:
            save(1);
        case MENU_EXIT:
            SDL_Quit();
            Mix_Quit();
            exit(0);
        case MENU_LOAD:
            load(1);
            break;

        case MENU_HELP:
            current_menu = menu_help;
            return 0;

        case MENU_INV_SOLID:
        case MENU_INV_LIGQUID:
        case MENU_INV_GAS:
            current_menu = create_inv_category_menu((Form)menu_inventory_categories->get_val());
            return 0;

        case MENU_LOUDER:
            Mix_Volume(0, Mix_Volume(0, -1) + 5);
            Mix_Volume(1, Mix_Volume(1, -1) + 5);
            printf("%d\n%d\n", Mix_Volume(1, -1), Mix_Volume(0, -1));
            return 0;
        case MENU_QUIETER:
            Mix_Volume(0, Mix_Volume(0, -1) - 5);
            Mix_Volume(1, Mix_Volume(1, -1) - 5);
            printf("%d\n%d\n", Mix_Volume(1, -1), Mix_Volume(0, -1));
            return 0;

        case MENU_CRAFT_AXE_BLADE:
        case MENU_CRAFT_AXE_HANDLE:
        case MENU_CRAFT_AXE:
        case MENU_CRAFT_KNIFE:
        case MENU_CRAFT_KNIFE_BLADE:
        case MENU_CRAFT_KNIFE_HANDLE:
        case MENU_CRAFT_WALL:
            return craft(a);

        case MENU_NPC:
            return npc();

        case MENU_NPC_SAY:
            return npc_say(menu_dialog->get_sentence());

        default:
            return 0;
    }
    return 1;
}

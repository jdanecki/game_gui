#include "craft.h"

#include "alchemist/axe_blade.h"
#include "alchemist/axe_handle.h"
#include "alchemist/axe.h"
#include "alchemist/knife_blade.h"
#include "alchemist/knife_handle.h"
#include "alchemist/knife.h"
#include "player.h"
#include <cstdio>
#include "networking.h"
#include "world.h"

#include "engine_hooks.h"

// TODO use real player
//extern class Player player;
//extern int active_hotbar;
int active_hotbar = 0;

void craft(int product_id, int ingredients_num, const size_t* ingredients_ids, Player* player)
{
    InventoryElement* crafted = nullptr;
    if (product_id < ING_NUM)
    {
        if (ingredients_num < 1)
            return;

        InventoryElement* el = player->get_item_by_uid(ingredients_ids[0]);
        if (!el)
        {
            printf("craft from invalid element");
            return;
        }
        player->inventory->remove(el);

        switch (product_id)
        {
        case ING_AXE_BLADE:
            crafted = craft_axe_blade(el);
            break;
        case ING_AXE_HANDLE:
            crafted = craft_axe_handle(el);
            break;
        }
    }
    if (crafted)
    {
#ifndef FUNNY_STUFF_FOR_SDL
        world_table[player->map_y][player->map_x]->add_object(crafted, player->x, player->y);
        objects_to_create.add(crafted);
#endif
        printf("crafted\n");
//        player->pickup(crafted);
    }
}

InventoryElement * craft_axe_blade(InventoryElement* el)
{
    if (el) {
        print_status("crafting: axe blade from %s", el->get_name());
    
        AxeBlade * axe_blade=new AxeBlade(el);
        if (axe_blade->craft()) {
            axe_blade->show();

            return axe_blade;
        } else delete axe_blade;
    }
    return NULL;
}

InventoryElement * craft_axe_handle(InventoryElement* el)
{
    if (el) {
        print_status("crafting: axe handle from %s", el->get_name());
    
        AxeHandle * axe_handle=new AxeHandle(el);
        if (axe_handle->craft())
        {
            axe_handle->show();

            return axe_handle;
        } else delete axe_handle;
    }
    return NULL;
}

InventoryElement * craft_axe(InventoryElement* el1, InventoryElement* el2)
{
    if (el1 && el2) 
    {
        print_status("crafting: axe from %s and %s", el1->get_name(), el2->get_name());
    
        Axe * axe=new Axe(el1, el2);
        if (axe->craft())
        {
            axe->show();
            return axe;
        } else delete axe;
    }
    return NULL;

}

//InventoryElement * craft_knife_blade(InventoryElement* el);
//InventoryElement * craft_knife_handle(InventoryElement* el);
//InventoryElement * craft_knife(InventoryElement* el1, InventoryElement* el2);


/*InventoryElement * craft_axe_blade(Player* player)
{
    InventoryElement * el = player->hotbar[active_hotbar];
    if (el) {
        print_status("crafting: axe blade from %s", el->get_name());
    
        AxeBlade * axe_blade=new AxeBlade(el);
        if (axe_blade->craft()) {
            axe_blade->show();
            axe_blade->set_posittion(player->x, player->y);
            player->inventory->remove(el);

            player->hotbar[active_hotbar]=NULL;
            return axe_blade;
        } else delete axe_blade;
    }
    return NULL;
}

InventoryElement * craft_axe_handle(Player* player)
{
    InventoryElement * el = player->hotbar[active_hotbar];
    if (el) {
        print_status("crafting: axe handle from %s", el->get_name());
    
        AxeHandle * axe_handle=new AxeHandle(el);
        if (axe_handle->craft())
        {
            axe_handle->show();
            axe_handle->set_posittion(player->x, player->y);
            player->inventory->remove(el);

            player->hotbar[active_hotbar]=NULL;
            return axe_handle;
        } else delete axe_handle;
    }
    return NULL;
}

InventoryElement * craft_axe(Player* player)
{
    InventoryElement *el1=NULL, *el2=NULL;
    for (int i =0; i< 10; i++)
    {
        if (player->craftbar[i]) 
        {
            player->craftbar[i]=0;
            if (!el1) {
                el1 = player->hotbar[i];
            } else {
                el2 = player->hotbar[i];
                player->hotbar[i]=NULL;
                break;
            }
            player->hotbar[i]=NULL;
        }
    }
    if (el1 && el2) 
    {
        print_status("crafting: axe from %s and %s", el1->get_name(), el2->get_name());
    
        Axe * axe=new Axe(el1, el2);
        if (axe->craft())
        {
            axe->show();
            axe->set_posittion(player->x, player->y);
            player->inventory->remove(el1);
            player->inventory->remove(el2);
            return axe;
        } else delete axe;
    }
    return NULL;
}





InventoryElement * craft_knife_blade(Player* player)
{
    InventoryElement * el = player->hotbar[active_hotbar];
    printf("FUN!\n");
    if (el) {
        print_status("crafting: knife blade from %s", el->get_name());
    
        KnifeBlade * knife_blade=new KnifeBlade(el);
        if (knife_blade->craft()) {
            knife_blade->show();
            knife_blade->set_posittion(player->x, player->y);
            player->inventory->remove(el);

            player->hotbar[active_hotbar]=NULL;
            return knife_blade;
        } else delete knife_blade;
    }
    return NULL;
}

InventoryElement * craft_knife_handle(Player* player)
{
    InventoryElement * el = player->hotbar[active_hotbar];
    if (el) {
        print_status("crafting: knife handle from %s", el->get_name());
    
        KnifeHandle * knife_handle=new KnifeHandle(el);
        if (knife_handle->craft()) {
            knife_handle->show();
            knife_handle->set_posittion(player->x, player->y);
            player->inventory->remove(el);

            player->hotbar[active_hotbar]=NULL;
            return knife_handle;
        } else delete knife_handle;
    }
    return NULL;
}

InventoryElement * craft_knife(Player* player)
{
    InventoryElement *el1=NULL, *el2=NULL;
    for (int i =0; i< 10; i++)
    {
        if (player->craftbar[i]) 
        {
            player->craftbar[i]=0;
            if (!el1) {
                el1 = player->hotbar[i];
            } else {
                el2 = player->hotbar[i];
                player->hotbar[i]=NULL;
                break;
            }
            player->hotbar[i]=NULL;
        }
    }
    if (el1 && el2) 
    {
        print_status("crafting: knife from %s and %s", el1->get_name(), el2->get_name());
    
        Knife * knife=new Knife(el1, el2);
        if (knife->craft()) {
            knife->show();
            knife->set_posittion(player->x, player->y);
            player->inventory->remove(el1);
            player->inventory->remove(el2);
            return knife;
        } else delete knife;
    }
    return NULL;
}*/

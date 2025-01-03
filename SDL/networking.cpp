#include "networking.h"
#include <stdio.h>
#include "../core/player.h"
#include "../core/tiles.h"
#include <cstring>
#include "../core/world.h"
#include "../core/alchemist/elements.h"
#include "implementations/alchemistSDL.h"
#include "implementations/BeingSDL.h"


#define PLAYER_NUM 16
extern Player* players[PLAYER_NUM];
extern Player* player;


InventoryElement* find_by_uid(size_t uid)
{
    ListElement* el = world_table[128][128]->objects.head;
    while (el)
    {
        if (el->el->uid == uid)
            return el->el;
        el = el->next;
    }
    return nullptr;
}



InventoryElementSDL* el_from_data(unsigned char* data)
{
    int offset = 0;

    size_t uid = *(size_t*)&data[offset];
    offset += sizeof(size_t);
    
    Class_id c_id = (Class_id)data[offset];
    offset += sizeof(Class_id);
    
    int ox = *((int*)&data[offset]);
    offset += sizeof(int);
    
    int oy = data[offset];
    offset += sizeof(int);
    
    int oz = data[offset];
    offset += sizeof(int);
    
    int id = data[offset];
    offset += sizeof(int);

    InventoryElementSDL* el = NULL;
    switch (c_id)
    {
        case Class_Element:
            el = new ElementSDL(id);
            printf("element(1) %ld - %d,%d - %d\n", uid, ox, oy, id);
            break;
        case Class_Ingredient:
            el = new IngredientSDL(id);
            printf("ingredient(2) %ld - %d,%d - %d\n", uid, ox, oy, id);
            break;
        case Class_Product:
            el = new ProductSDL(id);
            printf("product(3) %ld - %d,%d - %d\n", uid, ox, oy, id);
            break;
        case Class_Plant:
            el = new PlantSDL();
            printf("plant(4) %ld - %d,%d - %d\n", uid, ox, oy, id);
            break;
        case Class_Animal:
            el = new AnimalSDL();
            printf("animal(5) %ld - %d,%d - %d\n", uid, ox, oy, id);
            break;
        default:
            printf("something(%d) %ld - %d,%d - %d\n", c_id, uid, ox, oy, id);
            abort();
    }
    if (el)
    {
        el->uid = uid;
        //el->set_posittion(ox, oy);
        el->location.data.chunk.x = ox;
        el->location.data.chunk.y = oy;
    }
    return el;
}

extern "C" {

void update_player(uintptr_t id, int32_t map_x, int32_t map_y, int32_t x, int32_t y)
{
    if (id >= 0 && id < PLAYER_NUM)
    {
        if (!players[id])
        {
            players[id] = new Player(id);
        }
        /*for (int i = 0; i < 25; i++)
        {
            printf("%d, ", data[i]);
        }*/
        Player* player = players[id];
        player->map_x = map_x;
        player->map_y = map_y;

        if (x != player->x) 
            player->going_right = player->x > x ? 0 : 1;
        player->x = x;
        player->y = y;

        //printf("updated player %ld: %d %d %d %d\n", id, player->map_x, player->map_y, player->x, player->y);
    }
}

void update_chunk(int32_t x, int32_t y, uint8_t *data)
{
    if (!world_table[y][x])
    {
        //world_table[y][x] = (chunk*)calloc(1, sizeof(chunk));
        //world_table[y][x]->objects = InvList();
        world_table[y][x] = new chunk(x, y);
        memcpy(world_table[y][x]->table, &data[0], CHUNK_SIZE * CHUNK_SIZE * sizeof(game_tiles));
        /*printf("got %d items\n[", item_num);
        for (int i = 0; i < 1027 + item_num*20; i++)
        {
            printf("%d, ", data[i]);
        }
        printf("\n");*/
    }
}

void got_id(uintptr_t id, int64_t seed)
{
    //my_id = id;
    players[id] = new Player(id);
    player = players[id];

    printf("seed: %ld\n", seed);
    srand(seed);
    init_elements();
    printf("got id %ld\n", id);

}

void update_inventory(uint8_t *data)
{
    int offset = 1;
    int item_num = *(int*)&data[offset];
    for (int i = 0; i < item_num; i++)
    {
        InventoryElement* el = el_from_data(&data[5 + i*28]);
        
        if (el)
        {
            player->inventory->add(el);
            printf("invent %d %ld\n", el->c_id, el->uid);
        }
        else
        {
            printf("invalid item\n");
        }
    }
}

void update_objects(uint8_t *data)
{
    int offset = 1;
    int obj_num = *(int*)&data[offset];
    offset += sizeof(int);
    for (int i = 0; i < obj_num; i++)
    {
        size_t uid = *(size_t*)&data[offset];
        offset += sizeof(size_t);
        
        Class_id c_id = (Class_id)data[offset];
        offset += sizeof(Class_id);
        
        int ox = *((int*)&data[offset]);
        offset += sizeof(int);
        
        int oy = data[offset];
        offset += sizeof(int);
        
        int oz = data[offset];
        offset += sizeof(int);
        
        int id = data[offset];
        offset += sizeof(int);

        InventoryElement * el = find_by_uid(uid);
        if (el && el->c_id == c_id)
        {
            //el->set_posittion(ox, oy);
            el->location.data.chunk.x = ox;
            el->location.data.chunk.y = oy;
            printf("%ld moved to %d %d\n", uid, ox, oy);
        }
        else
        {
            if (el)
                printf("bad data for update object %ld %d real %d\n", uid, c_id, el->c_id);
            else
                printf("non existing object for update object %ld %d\n", uid, c_id);
        }
    }
}

void item_picked_up(uintptr_t iid, uintptr_t pid)
{
    /*if (pid >= 0 && pid < PLAYER_NUM && players[pid])
    {
        InventoryElement* item = find_by_uid(iid);
        item->parent_chunk->remove_object(item);
        if (item)
        {
            players[pid]->pickup(item);
            if (players[pid] == player)
            {
                for (int i = 0; i<10; i++) {
                    if (!player->hotbar[i])
                    {
                        player->hotbar[i]=item;
                        break;
                    }
                }
            }
            printf("player %ld picked up %ld\n", pid, iid);
            player->inventory->show();
        }   
        else 
            printf("picked inexisting item\n");
    }
    else
        printf("inexisting player\n");*/
}

void item_dropped(uintptr_t iid, uintptr_t pid) 
{
    /*if (pid < 0 || pid >= PLAYER_NUM || !players[pid])
    {
        printf("inexisting player drop");
        return;
    }
    InventoryElement* el = players[pid]->get_item_by_uid(iid);
    if (!el)
    {
        printf("inexisting item drop");
        return;
    }
    Player* p = players[pid];
    world_table[p->map_y][p->map_x]->add_object(el, p->x, p->y);
    p->inventory->remove(el);*/

}

void item_used_on_object(uintptr_t iid, uintptr_t oid, uintptr_t pid)
{
    printf("item used LOL\n");
}

void update_item_location(int32_t updates_number, uint8_t *data)
{
    int offset = 0;
    for (int i = 0; i < updates_number; i++)
    {
        size_t id = *((size_t*)&data[offset]);
        offset += sizeof(size_t);
        ItemLocation old_location = *((ItemLocation*)&data[offset]);
        offset += sizeof(ItemLocation);
        ItemLocation new_location = *((ItemLocation*)&data[offset]);
        offset += sizeof(ItemLocation);

        InventoryElement* el;
        switch (old_location.type)
        {
            case LOCATION_CHUNK:
                {
                printf("removed %ld from chunk %d %d\n", id, old_location.data.chunk.map_x, old_location.data.chunk.map_y);
                el = world_table[old_location.data.chunk.map_y][old_location.data.chunk.map_x]->find_by_id(id);
                world_table[old_location.data.chunk.map_y][old_location.data.chunk.map_x]->remove_object(el);
                break;
                }
            case LOCATION_PLAYER_INV:
                {
                printf("removed %ld from player %d\n", id, old_location.data.player.id);
                el = players[old_location.data.player.id]->get_item_by_uid(id);
                players[old_location.data.player.id]->drop(el);
                }

        }
        if (!el)
        {
            printf("not found item to remove %d %d\n", old_location.data.chunk.map_x, old_location.data.chunk.map_y);
            return;
        }
        switch (new_location.type)
        {
            case LOCATION_CHUNK:
            {
                printf("added %ld to chunk %d %d\n", id, new_location.data.chunk.map_x, new_location.data.chunk.map_y);
                world_table[new_location.data.chunk.map_y][new_location.data.chunk.map_x]->add_object(el, new_location.data.chunk.x, new_location.data.chunk.y);
                break;
            }
            case LOCATION_PLAYER_INV:
            {
                printf("added %ld to player %d\n", id, new_location.data.player.id);
                players[new_location.data.player.id]->pickup(el);
                if (new_location.data.player.id == player->get_id())
                {
                    for (int i = 0; i<10; i++) {
                        if (!player->hotbar[i])
                        {
                            player->hotbar[i]=el;
                            break;
                        }
                    }
                }
            }
        }

    }
}

void create_objects_in_chunk(int32_t x, int32_t y, uint32_t num, uint8_t *data)
{
    if (world_table[y][x])
    {
        for (int i = 0; i < num; i++)
        {
            InventoryElementSDL* el = el_from_data(&data[i*28]);
            if (el)
            {
                int item_x = el->location.data.chunk.x;
                int item_y = el->location.data.chunk.y;
                //el->get_posittion(&item_x,&item_y);
                world_table[y][x]->add_object(el, item_x, item_y);
            }
        }
    } else {
        printf("inexisting chunk\n");
    }
}

void destroy_object(uintptr_t id, uint8_t *data)
{
    ItemLocation* loc = (ItemLocation*)data;
    InventoryElement* el = nullptr;
    switch(loc->type)
    {
        case LOCATION_CHUNK:
            el = world_table[loc->data.chunk.map_y][loc->data.chunk.map_x]->find_by_id(id);
            world_table[loc->data.chunk.map_y][loc->data.chunk.map_x]->remove_object(el);
            break;
        case LOCATION_PLAYER_INV:
            el = players[loc->data.player.id]->get_item_by_uid(id);
            players[loc->data.player.id]->drop(el);
            break;
    }

    if (el)
        delete el;
    else
        printf("deleting inexisting item %ld\n", id);
}

void update_location(InventoryElement* el, ItemLocation location)
{

}

}

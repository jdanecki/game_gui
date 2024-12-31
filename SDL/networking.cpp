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
            printf("element %d - %d,%d\n", id, ox, oy);
            break;
        case Class_Ingredient:
            el = new IngredientSDL(id);
            printf("ingredient %d - %d,%d\n", id, ox, oy);
            break;
        case Class_Product:
            el = new ProductSDL(id);
            printf("product %d - %d,%d\n", id, ox, oy);
            break;
        case Class_Plant:
            el = new PlantSDL();
            printf("plant %d - %d,%d\n", id, ox, oy);
            break;
        case Class_Animal:
            el = new AnimalSDL();
            printf("animal %d - %d,%d\n", id, ox, oy);
            break;
        default:
            printf("something %d - %d,%d\n", c_id, ox, oy);
    }
    if (el)
    {
        el->uid = uid;
        el->set_posittion(ox, oy);
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
            players[id] = new Player();
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
        world_table[y][x] = new chunk;
        memcpy(world_table[y][x]->table, &data[0], CHUNK_SIZE * CHUNK_SIZE * sizeof(game_tiles));
        unsigned int item_num = data[1024];
        /*printf("got %d items\n[", item_num);
        for (int i = 0; i < 1027 + item_num*20; i++)
        {
            printf("%d, ", data[i]);
        }
        printf("\n");*/
        for (int i = 0; i < item_num; i++)
        {
            InventoryElementSDL* el = el_from_data(&data[1028 + i*28]);
            int item_x, item_y;
            if (el)
            {
                el->get_posittion(&item_x,&item_y);
                world_table[y][x]->add_object(el, item_x, item_y);
            }
        }
    }
}

void got_id(uintptr_t id, int64_t seed)
{
    //my_id = id;
    players[id] = new Player;
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
            el->set_posittion(ox, oy);
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
    if (pid >= 0 && pid < PLAYER_NUM && players[pid])
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
        printf("inexisting player\n");
}

}

#include "networking.h"
#include "../core/alchemist/elements.h"
#include "../core/player.h"
#include "../core/tiles.h"
#include "../core/world.h"
#include "implementations/BeingSDL.h"
#include "implementations/alchemistSDL.h"
#include "text.h"
#include <cstring>
#include <stdio.h>

#define PLAYER_NUM 16
extern Player * players[PLAYER_NUM];
extern Player * player;

InventoryElement * find_by_uid(size_t uid)
{
    ListElement * el = world_table[128][128]->objects.head;
    while (el)
    {
        if (el->el->uid == uid)
            return el->el;
        el = el->next;
    }
    return nullptr;
}

// InventoryElement* el_from_data(unsigned char* data)
// {
//     int offset = 0;

//     size_t uid = *(size_t*)&data[offset];
//     offset += sizeof(size_t);

//     Class_id c_id = (Class_id)data[offset];
//     offset += sizeof(Class_id);

//     /*int ox = *((int*)&data[offset]);
//     offset += sizeof(int);

//     int oy = data[offset];
//     offset += sizeof(int);

//     int oz = data[offset];
//     offset += sizeof(int);*/
//     ItemLocation* location = ((ItemLocation*)&data[offset]);
//     offset += sizeof(ItemLocation);

//     int id = -1;

//     InventoryElement* el = NULL;
//     switch (c_id)
//     {
//         case Class_Element:
//         {
//             offset += sizeof(unsigned int) * 7;
//             int id = data[offset];
//             offset += sizeof(int);
//             el = new ElementSDL(id);
//             printf("element(1) %ld - %d,%d - %d\n", uid, location->data.chunk.x, location->data.chunk.y, id);
//             break;
//         }
//         case Class_Ingredient:
//         {
//             offset += sizeof(unsigned int) * 3;
//             int id = data[offset];
//             offset += sizeof(int);
//             el = new IngredientSDL(id);
//             printf("ingredient(2) %ld - %d,%d - %d\n", uid, location->data.chunk.x, location->data.chunk.y, id);
//             break;
//         }
//         case Class_Product:
//         {
//             offset += sizeof(unsigned int) * 3;
//             int id = data[offset];
//             offset += sizeof(int);
//             el = new ProductSDL(id);
//             printf("product(3) %ld - %d,%d - %d\n", uid, location->data.chunk.x, location->data.chunk.y, id);
//             break;
//         }
//         case Class_Plant:
//         {
//             PlantSDL* p = new PlantSDL();
//             p->type = *((enum plant_types*)&data[offset]);
//             offset += sizeof(p->type);
//             p->phase = *((Plant_phase*)&data[offset]);
//             offset += sizeof(p->phase);
//             p->grown = *((bool*)&data[offset]);
//             offset += sizeof(p->grown);
//             el = p;
//             printf("plant(4) %ld - %d,%d - %d\n", uid, location->data.chunk.x, location->data.chunk.y, p->type);
//             break;
//         }
//         case Class_Animal:
//             el = new AnimalSDL();
//             printf("animal(5) %ld - %d,%d - %d\n", uid, location->data.chunk.x, location->data.chunk.y, id);
//             break;
//         default:
//             printf("something(%d) %ld - %d,%d - %d\n", c_id, uid, location->data.chunk.x, location->data.chunk.y, id);
//             abort();
//     }
//     if (el)
//     {
//         el->uid = uid;
//         //el->set_posittion(ox, oy);
//         //el->location.data.chunk.x = ox;
//         //el->location.data.chunk.y = oy;
//         el->location = *location;
//     }
//     return el;
// }

InventoryElement * el_from_data(ObjectData data)
{
    InventoryElement * el = nullptr;
    switch (data.tag)
    {
        case ObjectData::Tag::InvElement:
            break;
        case ObjectData::Tag::Element:
            el = new ElementSDL(data.element.data.id);
            break;
        case ObjectData::Tag::Ingredient:
            el = new IngredientSDL(data.ingredient.data.id);
            break;
        case ObjectData::Tag::Product:
            el = new ProductSDL(data.product.data.id);
            break;
        case ObjectData::Tag::Plant:
        {
            Plant * p = new PlantSDL();
            p->type = data.plant.data.id;
            p->grown = data.plant.data.grown;
            el = p;
            break;
        }
        case ObjectData::Tag::Animal:
            el = new AnimalSDL();
            break;
    }
    if (el)
    {
        el->uid = data.inv_element.data.uid;
        el->location.type = LOCATION_CHUNK; //(ItemLocationType)data.inv_element.data.location.tag;
        el->location.data.chunk.map_x = 128;
        el->location.data.chunk.map_y = 128;
        el->location.data.chunk.x = data.inv_element.data.location.chunk.x;
        el->location.data.chunk.y = data.inv_element.data.location.chunk.y;
    }
    return el;
}
//FIXME - remove/add only one element
void update_hotbar()
{
    for (int i = 0; i < 10; i++)
        player->hotbar[i] = nullptr;
    ListElement * le = player->inventory->head;
    int i = 0;
    while (le)
    {
        if (i >= 10)
            break;
        if (le->el)
            player->hotbar[i] = le->el;
        le = le->next;
        i++;
    }
}

extern "C"
{

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
            Player * player = players[id];
            player->map_x = map_x;
            player->map_y = map_y;

            if (x != player->x)
                player->going_right = player->x > x ? 0 : 1;
            player->x = x;
            player->y = y;

            // printf("updated player %ld: %d %d %d %d\n", id, player->map_x, player->map_y, player->x, player->y);
        }
    }

    void update_chunk(int32_t x, int32_t y, const chunk_table * data)
    {
        if (!world_table[y][x])
        {
            // world_table[y][x] = (chunk*)calloc(1, sizeof(chunk));
            // world_table[y][x]->objects = InvList();
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
        // my_id = id;
        players[id] = new Player(id);
        player = players[id];

        printf("seed: %ld\n", seed);
        srand(seed);
        init_elements();
        printf("got id %ld\n", id);
    }

    void update_inventory(uint8_t * data)
    {
        printf("THAT HAPPENED\n");
        /*int offset = 1;
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
        }*/
    }

    void update_object(ObjectData data)
    {
        size_t uid = data.inv_element.data.uid;
        Class_id c_id = data.inv_element.data.el_type;

        InventoryElement * el = find_by_uid(data.inv_element.data.uid);

        if (el && el->c_id == c_id)
        {
            switch (c_id)
            {
                case Class_Element:
                    break;
                case Class_Ingredient:
                    break;
                case Class_Product:
                    break;
                case Class_Plant:
                    Plant * p = dynamic_cast<Plant *>(el);
                    p->phase = data.plant.data.phase;
                    p->grown = data.plant.data.grown;
                    break;
            }
            // printf("%s updated\n", el->get_name());
        }
        else
        {
            if (el)
                printf("bad data for update object %ld %d real %d\n", uid, c_id, el->c_id);
            else
                printf("non existing object for update object %ld %d\n", uid, c_id);
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

    void update_item_location(int32_t updates_number, uint8_t * data)
    {
        int offset = 0;
        for (int i = 0; i < updates_number; i++)
        {
            printf("LOL\n");
            size_t id = *((size_t *)&data[offset]);
            offset += sizeof(size_t);
            ItemLocation old_location = *((ItemLocation *)&data[offset]);
            offset += sizeof(ItemLocation);
            ItemLocation new_location = *((ItemLocation *)&data[offset]);
            offset += sizeof(ItemLocation);

            InventoryElement * el;
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
                    if (old_location.data.player.id == player->get_id())
                    {
                        update_hotbar();  //FIXME - remove only one element  from hotbar
                    }
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
                    print_status("added %s to inventory", el->get_name());
                    players[new_location.data.player.id]->pickup(el);
                    //FIXME - don't add automatically to hotbar
                    if (new_location.data.player.id == player->get_id())
                    {
                        update_hotbar();
                    }
                }
            }
        }
    }

    // void create_objects_in_chunk(int32_t x, int32_t y, uint32_t num, uint8_t *data)
    void create_object(ObjectData data)
    {
        int x = 128;
        int y = 128;
        if (world_table[y][x])
        {
            int offset = 0;
            // for (int i = 0; i < num; i++)
            //{
            //   if (offset + 30 > num)
            //     break;
            InventoryElement * el = el_from_data(data);
            if (el)
            {
                int item_x = el->location.data.chunk.x;
                int item_y = el->location.data.chunk.y;
                // el->get_posittion(&item_x,&item_y);
                world_table[y][x]->add_object(el, item_x, item_y);
                print_status("%s created", el->get_name());
                // offset += el->get_packet_size();
            }
            //}
        }
        else
        {
            printf("inexisting chunk\n");
        }
    }

    void destroy_object(uintptr_t id, uint8_t * data)
    {
        ItemLocation * loc = (ItemLocation *)data;
        InventoryElement * el = nullptr;
        switch (loc->type)
        {
            case LOCATION_CHUNK:
                el = world_table[loc->data.chunk.map_y][loc->data.chunk.map_x]->find_by_id(id);
                world_table[loc->data.chunk.map_y][loc->data.chunk.map_x]->remove_object(el);
                break;
            case LOCATION_PLAYER_INV:
                el = players[loc->data.player.id]->get_item_by_uid(id);
                players[loc->data.player.id]->drop(el);
                if (loc->data.player.id == player->get_id())
                {
                    update_hotbar();  //FIXME - remove only one element
                }
                break;
        }

        if (el)
        {
            printf("delete %ld\n", id);
            delete el;
        }
        else
            printf("deleting inexisting item %ld\n", id);
    }

    void failed_craft()
    {
        printf("failed craft\n");
        print_status("failed craft");
    }
}

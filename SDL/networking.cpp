#include "networking.h"
#include <stdio.h>
#include "../core/player.h"
#include "../core/tiles.h"
#include <cstring>
#include "../core/world.h"
#include "../core/alchemist/elements.h"
#include "implementations/alchemistSDL.h"
#include "implementations/BeingSDL.h"

ENetHost* host;
ENetPeer* server;
unsigned int my_id;

#define PLAYER_NUM 16
extern Player* players[PLAYER_NUM];
extern Player* player;

int init_enet()
{
    if (enet_initialize () != 0)
    {
        printf("An error occurred while initializing ENet.\n");
        return 1;
    }
     
    host = enet_host_create (NULL /* create a client host */,
                1 /* only allow 1 outgoing connection */,
                2 /* allow up 2 channels to be used, 0 and 1 */,
                0 /* assume any amount of incoming bandwidth */,
                0 /* assume any amount of outgoing bandwidth */);
     
    if (host == NULL)
    {
        fprintf (stderr, 
                 "An error occurred while trying to create an ENet client host.\n");
        return 1;
    }
    ENetAddress address;
     
    enet_address_set_host (& address, "localhost");
    address.port = 1234;
     
    /* Initiate the connection, allocating the two channels 0 and 1. */
    server = enet_host_connect (host, &address, 2, 0);    
     
    if (server == NULL)
    {
       fprintf (stderr, 
                "No available peers for initiating an ENet connection.\n");
       return 1;
    }

    /* Wait up to 5 seconds for the connection attempt to succeed. */
    ENetEvent event;
    if (enet_host_service (host, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
    {
        printf("Connection to server succeeded.");
    }
    else
    {
        /* Either the 5 seconds are up or a disconnect event was */
        /* received. Reset the peer in the event the 5 seconds   */
        /* had run out without any significant event.            */
        enet_peer_reset (server);
     
        printf("Connection failed.");
        return 1;
    }
    for(;;)
    {
        if (enet_host_service (host, &event, 500) > 0 && event.type == ENET_EVENT_TYPE_RECEIVE)
        {
            if (event.packet->data[0] == PACKET_PLAYER_ID)
            {
                my_id = (unsigned int)event.packet->data[1];
                players[my_id] = new Player;
                player = players[my_id];
                int seed = *((int*)&event.packet->data[9]);
                printf("seed: %d\n", seed);
                srand(seed);
                init_elements();
                printf("got id %d\n", my_id);
                return 0;
            }
        }
    }
    return 0;
}

void update_player(unsigned char* data)
{
    size_t id = data[1];
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
        player->map_x = (int)data[9];
        player->map_y = (int)data[13];

        int nx = (int)data[17];
        if (nx != player->x) 
            player->going_right = player->x > nx ? 0 : 1;
        player->x = nx;
        player->y = (int)data[21];

        //printf("updated player %d: %d %d %d %d\n", id, player->map_x, player->map_y, player->x, player->y);
    }
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

void update_chunk(unsigned char* data)
{
    unsigned int x = data[1];
    unsigned int y = data[2];
    if (!world_table[y][x])
    {
        //world_table[y][x] = (chunk*)calloc(1, sizeof(chunk));
        //world_table[y][x]->objects = InvList();
        world_table[y][x] = new chunk;
        memcpy(world_table[y][x]->table, &data[3], CHUNK_SIZE * CHUNK_SIZE * sizeof(game_tiles));
        unsigned int item_num = data[1027];
        /*printf("got %d items\n[", item_num);
        for (int i = 0; i < 1027 + item_num*20; i++)
        {
            printf("%d, ", data[i]);
        }
        printf("\n");*/
        for (int i = 0; i < item_num; i++)
        {
            InventoryElementSDL* el = el_from_data(&data[1031 + i*28]);

            if (el)
            {
                world_table[y][x]->add_object(el);
            }
        }
    }
}

void update_inventory(unsigned char* data)
{
    int offset = 1;
    int item_num = *(int*)&data[offset];
    for (int i = 0; i < item_num; i++)
    {
        InventoryElement* el = el_from_data(&data[5 + i*28]);
        
        if (el)
        {
            players[my_id]->inventory->add(el);
            printf("invent %d %d", el->c_id, el->uid);
        }
        else
        {
            printf("invalid item\n");
        }
    }
}

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

void update_objects(unsigned char * data)
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

int network_tick()
{
    ENetEvent event;
    while (enet_host_service(host, &event, 0) > 0)
    {
        switch (event.type)
        {
            case ENET_EVENT_TYPE_RECEIVE:
            {
                unsigned char * data = event.packet->data;
                switch (data[0])
                {
                    case PACKET_PLAYER_UPDATE:
                        if (event.packet->dataLength == 25)
                            update_player(data);
                        else
                            printf("bad packet");
                        break;
                    case PACKET_CHUNK_UPDATE:
                        //if (event.packet->dataLength == CHUNK_SIZE * CHUNK_SIZE * sizeof(game_tiles) + 3)
                            update_chunk(data);
                        //else
                          //  printf("bad c update");
                        break;
                    case PACKET_SEND_INVENTORY:
                        update_inventory(data);
                        break;
                    case PACKET_OBJECTS_UPDATE:
                        update_objects(data);
                        break;

                }
                //printf("%\n", event.packet->data);

                break;
            }
            case ENET_EVENT_TYPE_DISCONNECT:
                return 1;
        }

    }
    return 0; 
}

void send_packet_move(int x, int y)
{
    char buf[3];
    buf[0] = PACKET_MOVE;
    buf[1] = x;
    buf[2] = y;
    
    ENetPacket* packet = enet_packet_create(buf, sizeof(buf), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(server, 0, packet);
}

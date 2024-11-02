#include "networking.h"
#include <stdio.h>
#include "../core/player.h"
#include "../core/tiles.h"
#include <cstring>
#include "../core/world.h"
#include "../core/alchemist/elements.h"
#include "implementations/alchemistSDL.h"

ENetHost* host;
ENetPeer* server;
unsigned int my_id;

#define PLAYER_NUM 16
extern Player* players[PLAYER_NUM];

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
        Player* player = players[id];
        player->map_x = (int)data[9];
        player->map_y = (int)data[13];
        player->x = (int)data[17];
        player->y = (int)data[21];

        printf("updated player %d: %d %d %d %d\n", id, player->map_x, player->map_y, player->x, player->y);
    }
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
        printf("got %d items\n", item_num);
        for (int i = 0; i < item_num; i++)
        {
            int ox = data[1027+i*16];
            int oy = data[1027+i*16 + 4];
            int oz = data[1027+i*16 + 8];
            int id = data[1027+i*16 + 12];

            ElementSDL* el = new ElementSDL(id);
            el->set_posittion(ox, oy);

            world_table[y][x]->add_object(el);
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

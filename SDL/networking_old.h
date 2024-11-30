#ifndef NETWORKING_H
#define NETWORKING_H

#include <enet/enet.h>
#include "../core/networking.h"

extern ENetHost* host;
extern ENetPeer* server;
extern unsigned int my_id;

enum ClientEvent
{
    MOVE,
    WHATEVER,
};

enum ServerEvent
{
    UPDATE_PLAYER,
    COS
};

int init_enet();
int network_tick();
void send_packet_move(int x, int y);

void send_packet(char* format, ...);
// this function will be called by other functions


#endif

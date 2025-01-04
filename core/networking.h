#ifndef NETWORKING_CORE_H
#define NETWORKING_CORE_H

#include "alchemist/el_list.h"

extern InvList objects_to_update;
extern InvList objects_to_update_reliable;

#ifndef FUNNY_STUFF_FOR_SDL
extern InvList objects_to_create;
#endif

enum PacketType {
    PACKET_PLAYER_UPDATE = 0,
    PACKET_PLAYER_ID,
    PACKET_MOVE,
    PACKET_CHUNK_UPDATE,
    PACKET_SEND_INVENTORY,
    PACKET_OBJECTS_UPDATE,
};

#endif

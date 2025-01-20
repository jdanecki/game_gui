#ifndef PACKET_TYPE_H
#define PACKET_TYPE_H

using PacketType = unsigned char;

constexpr static const PacketType PACKET_JOIN_REQUEST = 0;

constexpr static const PacketType PACKET_PLAYER_UPDATE = 1;

constexpr static const PacketType PACKET_PLAYER_ID = 2;

constexpr static const PacketType PACKET_PLAYER_MOVE = 3;

constexpr static const PacketType PACKET_CHUNK_UPDATE = 4;

constexpr static const PacketType PACKET_INVENTORY_UPDATE = 5;

constexpr static const PacketType PACKET_OBJECTS_UPDATE = 6;

constexpr static const PacketType PACKET_PLAYER_ACTION_PICKUP = 7;

constexpr static const PacketType PACKET_PLAYER_ACTION_DROP = 8;

constexpr static const PacketType PACKET_PLAYER_ACTION_USE_ITEM_ON_OBJECT = 9;

constexpr static const PacketType PACKET_PLAYER_ACTION_CRAFT = 10;

constexpr static const PacketType PACKET_LOCATION_UPDATES = 11;

constexpr static const PacketType PACKET_CREATE_OBJECTS_IN_CHUNK = 12;

constexpr static const PacketType PACKET_DESTROY_OBJECT = 13;

constexpr static const PacketType PACKET_FAILED_CRAFT = 14;

constexpr static const PacketType PACKET_PLAYER_ACTION_USE_ITEM_ON_TILE = 15;

#endif

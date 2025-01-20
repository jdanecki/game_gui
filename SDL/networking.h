#ifndef NETWORKING_H
#define NETWORKING_H
class UdpSocket {};
#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <new>

struct NetClient {
  UdpSocket socket;
};

using PacketType = uint8_t;

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

extern "C" {

const NetClient *init();

void foo(const NetClient *a);

void network_tick(const NetClient *client);

extern void update_player(uintptr_t id, int32_t map_x, int32_t map_y, int32_t x, int32_t y);

extern void update_chunk(int32_t x, int32_t y, uint8_t *data);

extern void got_id(uintptr_t id, int64_t seed);

extern void update_inventory(uint8_t *data);

extern void update_objects(uint8_t *data);

extern void item_picked_up(uintptr_t iid, uintptr_t pid);

extern void item_dropped(uintptr_t iid, uintptr_t pid);

extern void item_used_on_object(uintptr_t iid, uintptr_t oid, uintptr_t pid);

extern void update_item_location(int32_t updates_number, uint8_t *data);

extern void create_objects_in_chunk(int32_t x, int32_t y, uint32_t num, uint8_t *data);

extern void destroy_object(uintptr_t id, uint8_t *data);

extern void failed_craft();

void send_packet_move(const NetClient *client, int32_t x, int32_t y);

void send_packet_pickup(const NetClient *client, uintptr_t id);

void send_packet_drop(const NetClient *client, uintptr_t id);

void send_packet_item_used_on_object(const NetClient *client, uintptr_t iid, uintptr_t oid);

void send_packet_item_used_on_tile(const NetClient *client,
                                   uintptr_t iid,
                                   int32_t map_x,
                                   int32_t map_y,
                                   int32_t x,
                                   int32_t y);

void send_packet_craft(const NetClient *client,
                       uintptr_t prod_id,
                       uintptr_t ingredients_num,
                       const uintptr_t *iid);

}  // extern "C"
#endif

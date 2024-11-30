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

extern "C" {

const NetClient *init();

void foo(const NetClient *a);

void network_tick(const NetClient *client);

void send_packet_move(const NetClient *client, int32_t x, int32_t y);

extern void update_player(uintptr_t id, int32_t map_x, int32_t map_y, int32_t x, int32_t y);

extern void update_chunk(int32_t x, int32_t y, uint8_t *data);

extern void got_id(uintptr_t id, int64_t seed);

extern void update_inventory(uint8_t *data);

extern void update_objects(uint8_t *data);

}  // extern "C"
#endif

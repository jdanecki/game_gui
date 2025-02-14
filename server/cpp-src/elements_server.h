#ifndef ELEMENTS_SERVER_H
#define ELEMENTS_SERVER_H

#include "../../core/alchemist/elements.h"
#include "../../core/tiles.h"

class ToBytes
{
  public:
};

void to_bytes_binding(InventoryElement * el, unsigned char * buf);
unsigned int get_packet_size_binding(InventoryElement * el);

extern "C"
{
    void update_location(size_t id, ItemLocation old_loc, ItemLocation new_loc);
    void notify_destroy(size_t id, ItemLocation location);
}

void destroy(InventoryElement * el);

const int max_delay_move = 10;  // 1 sec.
const int max_delay_grow = 600; // 1 min.

class AnimalServer : public Animal
{
    int delay_for_move;
    int dst_loc_x, dst_loc_y;

  public:
    void move();
    bool tick() override;
    AnimalServer()
    {
        delay_for_move = max_delay_move; // 600 * 100ms -> 1min
        dst_loc_x = rand() % CHUNK_SIZE;
        dst_loc_y = rand() % CHUNK_SIZE;
    }
};

class PlantServer : public Plant
{
    int * padding; // FIXME
    int delay_for_grow;

  public:
    bool grow() override;
    // bool tick() override;
    PlantServer();
};

#endif

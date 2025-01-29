#ifndef ELEMENTS_SERVER_H
#define ELEMENTS_SERVER_H

#include "../../core/alchemist/elements.h"

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

const int max_delay_move = 10;
const int max_delay_grow = 600;

class AnimalServer : public Animal
{
    int delay_for_move;

  public:
    void move();
    bool tick() override;
    AnimalServer()
    {
        delay_for_move = max_delay_move; // 600 * 100ms -> 1min
    }
};

class PlantServer : public Plant
{
    int delay_for_grow;

  public:
    bool grow() override;
    // bool tick() override;
    PlantServer();
};

#endif

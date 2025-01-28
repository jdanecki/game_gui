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

class AnimalServer : public Animal
{
    int delay_for_move;

  public:
    void move();
    bool tick() override;
    AnimalServer()
    {
        delay_for_move = 10;
    }
};

class PlantServer : public Plant
{
  public:
    bool grow() override;
    bool tick() override;
};

#endif

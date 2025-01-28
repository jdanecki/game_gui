#include "elements_server.h"
#include "networking.h"
#include "world_server.h"

void to_bytes_binding(InventoryElement * el, unsigned char * buf)
{
    el->to_bytes(buf);
}

unsigned int get_packet_size_binding(InventoryElement * el)
{
    return el->get_packet_size();
}

void destroy(InventoryElement * el)
{
    notify_destroy(el->get_uid(), el->location);
    if (el->location.type == LOCATION_CHUNK)
    {
        world_table[el->location.data.chunk.map_y][el->location.data.chunk.map_x]->remove_object(el);
    }
    delete el;
}

void AnimalServer::move()
{
    ItemLocation l = location;
    int _x = location.data.chunk.x;
    int _y = location.data.chunk.y;

    switch (rand() % 4)
    {
        case 0: // up
            _y--;
            break;
        case 1: // down
            _y++;
            break;
        case 2: // left
            // going_right=false;
            _x--;
            break;
        case 3: // right
            // going_right=true;
            _x++;
            break;
    }

    if (_x >= CHUNK_SIZE)
        _x = CHUNK_SIZE - 1;
    if (_y >= CHUNK_SIZE)
        _y = CHUNK_SIZE - 1;
    if (_y < 0)
        _y = 0;
    if (_y < 0)
        _y = 0;
    if (_x < 0)
        _x = 0;

    location.data.chunk.x = _x;
    location.data.chunk.y = _y;

    update_location(uid, l, location);
}

bool AnimalServer::tick()
{
    Animal::tick();
    move();
    return true;
}

bool PlantServer::grow()
{
    if (grown)
        return false;
    if (!water)
        return !grown;
    // water--;
    age++;

    if (age >= max_age)
    {
        if (phase != Plant_fruits)
        {
            grown = true;
            change_phase(Plant_fruits);
            objects_to_update.add(this);
        }
        return !grown;
    }
    if (age >= flowers_time)
    {
        if (phase != Plant_flowers)
        {
            change_phase(Plant_flowers);
            objects_to_update.add(this);
        }
        return !grown;
    }
    if (age >= growing_time)
    {
        if (phase != Plant_growing)
        {
            change_phase(Plant_growing);
            objects_to_update.add(this);
        }
        return !grown;
    }
    if (age >= seedling_time)
    {
        if (phase != Plant_seedling)
        {
            change_phase(Plant_seedling);
            objects_to_update.add(this);
        }
        return !grown;
    }
    return !grown;
}

bool PlantServer::tick()
{
    grow();
    Plant::tick();
    return true;
}

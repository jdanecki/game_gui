#include "tiles.h"

void chunk::add_object(InventoryElement* object, int x, int y)
{
    objects.add(object);
#ifndef FUNNY_STUFF_FOR_SDL
    if (Being* being = (Being*)object)
    {
        beings.add(object);
    }
#endif

    ItemLocation location;
    location.type = LOCATION_CHUNK;
    location.data.chunk.map_y = map_y;
    location.data.chunk.map_x = map_x;
    location.data.chunk.x = x;
    location.data.chunk.y = y;
    object->location = location;

#ifndef FUNNY_STUFF_FOR_SDL
    object->to_bytes();
#endif

}

// move object to chunk from other location
void chunk::move_object(InventoryElement* object, int x, int y)
{
    ItemLocation location;
    location.type = LOCATION_CHUNK;
    location.data.chunk.map_y = map_y;
    location.data.chunk.map_x = map_x;
    location.data.chunk.x = x;
    location.data.chunk.y = y;
    update_location(object, location);
}


void chunk::remove_object(InventoryElement* object)
{
    objects.remove(object);
    beings.remove(object);
}

InventoryElement* chunk::find_by_id(size_t id)
{
    ListElement* el = objects.head;
    while (el)
    {
        if (el->el->uid == id)
            return el->el;
        el = el->next;
    }
    return nullptr;
}

chunk::chunk(int map_x, int map_y): map_x(map_x), map_y(map_y)
{
}

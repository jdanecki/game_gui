#include "tiles.h"

void chunk::add_object(InventoryElement * object, int x, int y)
{
    objects.add(object);
    if (Being * being = dynamic_cast<Being *>(object))
    {
        beings.add(object);
    }

    ItemLocation location;
    location.type = LOCATION_CHUNK;
    location.data.chunk.map_y = map_y;
    location.data.chunk.map_x = map_x;
    location.data.chunk.x = x;
    location.data.chunk.y = y;
    object->location = location;
}

void chunk::remove_object(InventoryElement * object)
{
    objects.remove(object);
    beings.remove(object);
}

InventoryElement * chunk::find_by_id(size_t id)
{
    ListElement * el = objects.head;
    while (el)
    {
        if (el->el->uid == id)
            return el->el;
        el = el->next;
    }
    return nullptr;
}

chunk::chunk(int map_x, int map_y) : map_x(map_x), map_y(map_y)
{

}

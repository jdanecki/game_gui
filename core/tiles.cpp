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
    object->set_posittion(x, y);
    object->parent_chunk = this;
}

void chunk::remove_object(InventoryElement* object)
{
    objects.remove(object);
    beings.remove(object);
    object->parent_chunk = nullptr;
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

chunk::chunk()
{
}

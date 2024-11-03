#include "tiles.h"

void chunk::add_object(InventoryElement* object)
{
    objects.add(object);
#ifndef FUNNY_STUFF_FOR_SDL
    if (Being* being = (Being*)object)
    {
        beings.add(object);
    }
#endif
#ifdef LOCAL_ONLY
    beings.add(object);
#endif
}

chunk::chunk()
{
}

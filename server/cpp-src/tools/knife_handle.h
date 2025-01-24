#ifndef KNIFE_HANDLE_H
#define KNIFE_HANDLE_H

#include "../elements_server.h"

class KnifeHandle : public Ingredient
{
  public:
    KnifeHandle(InventoryElement * from);
};
#endif

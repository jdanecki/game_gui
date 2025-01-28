#ifndef CRATFING_H
#define CRATFING_H

#include "elements_server.h"
#include "player_server.h"

bool craft(int product_id, int ingredients_num, const size_t * ingredients_ids, Player * player);

InventoryElement * craft_axe_blade(InventoryElement * el);
InventoryElement * craft_axe_handle(InventoryElement * el);
InventoryElement * craft_axe(InventoryElement * el1, InventoryElement * el2);

// InventoryElement * craft_knife_blade(InventoryElement* el);
// InventoryElement * craft_knife_handle(InventoryElement* el);
// InventoryElement * craft_knife(InventoryElement* el1, InventoryElement* el2);

/*InventoryElement * craft_axe_blade(Player* player);
InventoryElement * craft_axe_handle(Player* player);
InventoryElement * craft_axe(Player* player);

InventoryElement * craft_knife_blade(Player* player);
InventoryElement * craft_knife_handle(Player* player);
InventoryElement * craft_knife(Player* player);*/
#endif

#include "player.h"
#include "tiles.h"

void Player::pickup(InventoryElement* item)
{
    inventory->add(item);

    ItemLocation location;
    location.type = LOCATION_PLAYER_INV;
    location.data.player.id = id;
    item->location = location;
}

void Player::drop(InventoryElement* item)
{
    inventory->remove(item);
}

InventoryElement* Player::get_item_by_uid(size_t id)
{
    ListElement* cur = inventory->head;
    while (cur)
    {
        if (cur->el->uid == id)
            return cur->el;
        cur = cur->next;
    }
    return NULL;
}



Player::Player(int id): id(id)
{
    hunger=500;
    thirst=250;
    map_x = WORLD_CENTER;
    map_y = WORLD_CENTER;
    inventory = new InvList("inventory");
    direction = direction::right;

    x = 0;
    y = 0;

    for (int i=0; i < 10; i++)
    {
        hotbar[i]=NULL;
        craftbar[i]=0;
    }
}

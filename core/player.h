#ifndef PLAYER_H
#define PLAYER_H
#include "alchemist/el_list.h"

enum class direction
{
    up,
    right,
    down,
    left,
};

class Player
{
    int id;

public:
    int map_y;
    int map_x;
    int y;
    int x;
    char running;
    char sneaking;
    char going_right;
    enum direction direction;
    int thirst;
    int hunger;
    InvList * inventory; 
    InventoryElement * hotbar[10];
    int craftbar[10];
    void pickup(InventoryElement* item);
    void drop(InventoryElement* item);
    InventoryElement* get_item_by_uid(size_t id);
    int get_id() {return id;}
	  Player(int id);

};



#endif

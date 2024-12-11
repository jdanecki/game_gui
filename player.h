#ifndef PLAYER_H
#define PLAYER_H
#include "alchemist/el_list.h"

enum location
{
    LOC_WORLD,
    LOC_DUNGEON,
    LOC_CAVE
};


enum class direction
{
    up,
    right,
    down,
    left,
};

class Player
{
  	void check_and_move(int new_map_x, int new_map_y, int new_x, int new_z);

public:
    int map_y;
    int map_x;
    int y;
    int x;
  	int back_y;
  	int back_x;
    char running;
    char sneaking;
    char going_right;
    enum direction direction;
    enum location in;
    int health;
    int max_health; // TODO skill tree -> increase max health
    int energy;
    int max_energy; // TODO skill tree -> increase max energy
    int run_usage; // TODO skill tree -> decrease run energy usage
                   // TODO if run_usage is negative, use it as chance to not use energy
    InvList * inventory; 
    InventoryElement * hotbar[10];
    int craftbar[10];
	void move(int x, int z);
	Player();

};



#endif

#include "npc.h"

// defined in SDL/main.h
// #define PLAYER_NUM 16

int npcs_count;

Npc::Npc() : Player(16 + (++npcs_count))
{
    c_id = Class_Npc;
    name = create_name(rand() % 3 + 3);

    Class_id i = Class_BaseElement;

    ElementsTable * list = dynamic_cast<ElementsTable *>(known_elements->find(&i));
    list->set_all_known();

    i = Class_BaseAnimal;
    list = dynamic_cast<ElementsTable *>(known_elements->find(&i));
    list->set_all_known();

    i = Class_BasePlant;
    list = dynamic_cast<ElementsTable *>(known_elements->find(&i));
    list->set_all_known();
}

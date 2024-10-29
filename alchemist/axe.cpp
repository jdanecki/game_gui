#include "axe.h"
#include <cstddef>
#include <cstdio>
#include "../player.h"
#include "../world.h"
#include "elements.h"

Axe::Axe(InventoryElement *el1, InventoryElement *el2): Product(el1, el2, PROD_AXE, Form_solid)
{

}

bool Axe::use(int map_x, int map_y, int x, int y)
{
    for (int i = 0; i < CHUNK_SIZE*CHUNK_SIZE; i++) {
        Plant * p = world_table[map_y][map_x]->plants[i];
        if (p)
        {
            int px,py;
            p->get_posittion(&px, &py);
            if (x == px && y == py && p->grown)
            {
                if (p->type == PLANTID_tree || p->type == PLANTID_tree1 || p->type == PLANTID_tree2)
                {
                    
                    switch (p->type) {
                        case PLANTID_tree:
                        {
                            Element * el = new Element(base_elements[ID_LOG]);
                            el->set_posittion(x, y);
                            set_item_at(el, map_x, map_y, x, y);
                            Element * el_seed = new Element(base_elements[ID_ACORN]);
                            el_seed->set_posittion(x, y);
                            set_item_at(el_seed, map_x, map_y, x, y);
                            break;
                        }
                        case PLANTID_tree1:
                        {
                            Element * el = new Element(base_elements[ID_LOG1]);
                            el->set_posittion(x, y);
                            set_item_at(el, map_x, map_y, x, y);
                            Element * el_seed = new Element(base_elements[ID_ACORN1]);
                            el_seed->set_posittion(x, y);
                            set_item_at(el_seed, map_x, map_y, x, y);
                            break;
                        }
                        case PLANTID_tree2:
                        {
                            Element * el = new Element(base_elements[ID_LOG2]);
                            el->set_posittion(x, y);
                            set_item_at(el, map_x, map_y, x, y);
                            Element * el_seed = new Element(base_elements[ID_ACORN2]);
                            el_seed->set_posittion(x, y);
                            set_item_at(el_seed, map_x, map_y, x, y);
                            break;
                        }
                    }
                    free(p);
                    p=NULL;
                    world_table[map_y][map_x]->plants[i]=NULL;
                    return true;
                }
                // TODO    break when used too much
            }
        }

        Object * o = world_table[map_y][map_x]->objects[i];
        if (o && o->type == OBJECT_wall)
        {
            int ox,oy;
            o->get_posittion(&ox, &oy);
            if (ox == x && oy == y)
            {
                Element * el = new Element(o->base);
                el->set_posittion(x, y);
                set_item_at(el, map_x, map_y, x, y);

                free(o);
                o=NULL;
                world_table[map_y][map_x]->objects[i]=NULL;
                return true;
            }
        }
    }
    return false;
}

bool Axe::check_ing()
{
    int id1 = ings[0]->get_id();
    int id2 = ings[1]->get_id();

    if (
            (id1 == ING_AXE_BLADE && id2 == ING_AXE_HANDLE) || 
            (id1 == ING_AXE_HANDLE && id2 == ING_AXE_BLADE)
       ) return true;
    printf("wrong ingredients\n");
    return false;
}

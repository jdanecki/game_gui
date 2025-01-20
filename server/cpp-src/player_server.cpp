#include "player_server.h"
#include "world_server.h"
#include "networking.h"
#include "elements_server.h"

void PlayerServer::check_and_move(int new_map_x, int new_map_y, int new_x, int new_y)
{
    map_x = new_map_x;
    map_y = new_map_y;
    x = new_x;
    y = new_y;
    hunger -= 3;
    thirst--;
}

void PlayerServer::move(int dx, int dy)
{
    int new_x = x + dx;
    int new_y = y + dy;
    int new_map_x = map_x;
    int new_map_y = map_y;
    
    if (! ((new_x >= 0 && new_x < CHUNK_SIZE) && (new_y >= 0 && new_y < CHUNK_SIZE))) 
    {
        if (new_x < 0 || new_x >= CHUNK_SIZE) {
                new_map_x += dx;
                new_x += -CHUNK_SIZE * dx;
            if (!load_chunk(new_map_x, new_map_y)) return;
        }
    
        if (new_y < 0 || new_y >= CHUNK_SIZE)
        {
                new_map_y += dy;
                new_y += -CHUNK_SIZE * dy;
            if (!load_chunk(new_map_x, new_map_y)) return;
        }
    }
    check_and_move(new_map_x, new_map_y, new_x, new_y);
}

bool PlayerServer::use_item_on_object(InventoryElement* item, InventoryElement* object)
{
    Product* i = (Product*)item;

    printf("used %d on %ld", i->get_id(), object->uid);
    return i->use(object);
}


bool PlayerServer::plant_with_seed(InventoryElement * el, int map_x, int map_y, int x, int y)
{
    if (get_tile_at_ppos(this) == TILE_GRASS || get_tile_at_ppos(this) == TILE_DIRT)
    {
        int id = el->get_id();
        if (id == ID_ACORN || id == ID_ACORN1 || id == ID_ACORN2)
        {
            Plant* p = new Plant();
            switch (id)
            {
                case ID_ACORN:
                    p->type = PLANTID_tree;
                    break;
                case ID_ACORN1:
                    p->type = PLANTID_tree1;
                    break;
                case ID_ACORN2:
                    p->type = PLANTID_tree2;
                    break;
                case ID_PUMPKIN_SEEDS:
                    p->type = PLANTID_pumpkin;
                    break;
                case ID_WATERMELON_SEEDS:
                    p->type = PLANTID_watermelon;
                    break;
                case ID_STRAWBERRY_SEEDS:
                    p->type = PLANTID_strawberry;
                    break;
                default:
                    delete p;
                    p = nullptr; 
            }
            if (!p)
                return false;
            ListElement* le = world_table[map_y][map_x]->objects.head;
            while (le)
            {
                if (le->el->location.data.chunk.x == x && le->el->location.data.chunk.y == y)
                    return false;
                le = le->next;
            }
            world_table[map_y][map_x]->add_object(p, x, y);
            objects_to_create.add(p);
            
            p->phase=Plant_seed;
            p->grown=false;
            p->age=1;

            drop(el);
            destroy(el);
            return true;
        }
    }
    return false;
}

bool PlayerServer::pickup(InventoryElement* item)
{
    ItemLocation old_location = item->location;
    remove_from_chunks(item);
    Player::pickup(item);
    update_location(item->get_uid(), old_location, item->location);
    return true;
}

PlayerServer::PlayerServer(int id) : Player(id)
{}

#include "world_server.h"
#include "elements_server.h"

void generator()
{
    load_chunk(WORLD_CENTER, WORLD_CENTER);
    printf("generated: %d items\n", world_table[128][128]->objects.nr_elements);
}

void create_biome_plains(chunk * chunk)
{
// FIXME
#if 0
    for (int y = 0; y < CHUNK_SIZE; y++)
    {
        for (int x = 0; x < CHUNK_SIZE; x++)
        {
            chunk->table[y][x].tile = TILE_GRASS;
        }
        // printf("\n");
    }
    if (rand() % 2)
    {
        for (int i = 0; i < 16; i++)
        {
            int b = rand() % BASE_ELEMENTS;
            Element * o = new Element(base_elements[b]);
            int x = rand() % CHUNK_SIZE;
            int y = rand() % CHUNK_SIZE;

            chunk->add_object(o, x, y);
        }
    }
    if (rand() % 2)
    {
        for (int i = 0; i < 16; i++)
        {
            Plant * p = new PlantServer();
            int x = rand() % CHUNK_SIZE;
            int y = rand() % CHUNK_SIZE;

            p->type = PLANTID_strawberry;

            chunk->add_object(p, x, y);
        }
    }
    if (rand() % 2)
    {
        for (int i = 0; i < 16; i++)
        {
            Plant * p = new PlantServer();
            int x = rand() % CHUNK_SIZE;
            int y = rand() % CHUNK_SIZE;

            p->type = PLANTID_watermelon;

            chunk->add_object(p, x, y);
        }
    }
    if (rand() % 2)
    {
        for (int i = 0; i < 16; i++)
        {
            Plant * p = new PlantServer();
            int x = rand() % CHUNK_SIZE;
            int y = rand() % CHUNK_SIZE;

            p->type = PLANTID_pumpkin;

            chunk->add_object(p, x, y);
        }
    }
    if (/*rand() % 2*/ 1)
    {

        for (int i = 0; i < 2; i++)
        {
            Animal * a = new AnimalServer();
            int x = rand() % CHUNK_SIZE;
            int y = rand() % CHUNK_SIZE;

            a->type = ANIMAL_ID_pig;

            chunk->add_object(a, x, y);
        }
    }
#endif
}

void create_biome_forest(chunk * chunk)
{
    for (int y = 0; y < CHUNK_SIZE; y++)
    {
        for (int x = 0; x < CHUNK_SIZE; x++)
        {
            chunk->table[y][x].tile = TILE_GRASS;
        }
        // printf("\n");
    }
#if 0
    Element *o= new Element(base_elements[0]);
    o->show(true);
    chunk->add_object(o);
#else
    for (int i = 0; i < 16; i++)
    {
        Element * o = new Element();
        int x = rand() % CHUNK_SIZE;
        int y = rand() % CHUNK_SIZE;

        chunk->add_object(o, x, y);
    }

    if (/*rand() % 2*/ 1)
    {

        for (int i = 0; i < 5; i++)
        {
            chunk->add_object(new AnimalServer());
        }
    }

    for (int i = 0; i < 10; i++)
    {
        chunk->add_object(new PlantServer());
    }
#endif
}

void create_biome_desert(chunk * chunk)
{
    // FIXME
#if 0
    for (int y = 0; y < CHUNK_SIZE; y++)
    {
        for (int x = 0; x < CHUNK_SIZE; x++)
        {
            chunk->table[y][x].tile = TILE_SAND;
        }
        // printf("\n");
    }

    for (int i = 0; i < 4; i++)
    {
        int b = rand() % BASE_ELEMENTS;
        Element * o = new Element(base_elements[b]);
        int x = rand() % CHUNK_SIZE;
        int y = rand() % CHUNK_SIZE;

        chunk->add_object(o, x, y);
    }
#endif
}

void generate_chunk(chunk * chunk, int chunk_x, int chunk_y)
{
    //  enum biomes random_biome = (enum biomes)(rand() % BIOMES);

    create_biome_forest(chunk);
    /*
    switch (random_biome)
    {
        case BIOME_FOREST:
            create_biome_forest(chunk);
            break;
        case BIOME_DESERT:
            create_biome_desert(chunk);
            break;
        case BIOME_PLAINS:
            create_biome_plains(chunk);
            break;
    }
*/
}

char load_chunk(int x, int y)
{
    // FIXME when more chunks enabled
    if (x != 128)
        return 0;
    if (y != 128)
        return 0;

    if (x >= 0 && x < WORLD_SIZE && y >= 0 && y < WORLD_SIZE)
    {
        if (world_table[y][x] == NULL)
        {
            chunk * c = new chunk(x, y);
            printf("load_chunk: %d %d\n", x, y);
            generate_chunk(c, x, y);
            world_table[y][x] = c;
        }
        return 1;
    }
    return 0;
}

void update()
{
    // TODO update more chunks
    ListElement * el = world_table[128][128]->beings.head;
    while (el)
    {
        el->el->tick();
        el = el->next;
    }
    // TODO kill animals
    /*            if (!a->alive)
        {
            int x,y;
            a->get_posittion(&x, &y);

            Element * el;
            switch (a->type)
            {
                case ANIMALID_pig:
                    el = new Element(base_elements[ID_RAW_HAM]);
            }
            el->set_posittion(x, y);
            set_item_at(el, player.map_x, player.map_y, x, y);

            free(a);
            a=NULL;
            world_table[player.map_y][player.map_x]->animals[i]=NULL;
        }*/
}

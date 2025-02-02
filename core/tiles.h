#ifndef TILES_H
#define TILES_H

#include "alchemist/el_list.h"
#include "alchemist/elements.h"

enum game_tiles
{
    TILE_AIR,
    TILE_STONE,
    TILE_DIRT,
    TILE_SAND,
    TILE_SANDSTONE,
    TILE_SWEET_TREE,
    TILE_SWEET_BUSH,
    TILE_SWEET_FLOWER,
    TILE_GRASS,
    TILE_SWEET_GRASS,
    TILE_WATER,
    TILE_MAX_NUM
};

enum biomes
{
    BIOME_DESERT, // yellow
    BIOME_FOREST, // green with trees
    BIOME_PLAINS, // green
};
#define BIOMES 3

#define CHUNK_SIZE 16
#define WORLD_SIZE 256
#define WORLD_CENTER WORLD_SIZE / 2
#define WORLD_SCALE 100.0

struct tile
{
    int tile;
};

typedef struct tile chunk_table[CHUNK_SIZE][CHUNK_SIZE];

class chunk
{
    int map_x, map_y;

  public:
    chunk_table table;
    InvList objects;
    InvList beings;
    void add_object(InventoryElement * object, int x, int y);
    void remove_object(InventoryElement * object);
    InventoryElement * find_by_id(size_t id);
    chunk(int map_x, int map_y);
};

#endif

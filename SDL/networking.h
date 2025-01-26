#ifndef NETWORKING_H
#define NETWORKING_H
class UdpSocket
{
};
#include "../core/packet_types.h"
#include "../core/world.h"
#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <new>
#include <ostream>

struct NetClient
{
    UdpSocket socket;
};

struct ItemLocationLol
{
    enum class Tag
    {
        Chunk,
        Player,
    };

    struct Chunk_Body
    {
        int32_t x;
        int32_t y;
    };

    struct Player_Body
    {
        uintptr_t id;
    };

    Tag tag;
    union
    {
        Chunk_Body chunk;
        Player_Body player;
    };
};

struct InventoryElementData
{
    uintptr_t uid;
    Class_id el_type;
    ItemLocationLol location;
};

struct ElementData
{
    InventoryElementData base;
    int32_t id;
    uint32_t sharpness;
    uint32_t smoothness;
    uint32_t mass;
    uint32_t length;
    uint32_t width;
    uint32_t height;
    uint32_t volume;
};

struct IngredientData
{
    InventoryElementData base;
    Ingredient_id id;
    int32_t quality;
    int32_t resilience;
    int32_t usage;
};

struct ProductData
{
    InventoryElementData base;
    Product_id id;
    int32_t quality;
    int32_t resilience;
    int32_t usage;
};

struct PlantData
{
    InventoryElementData base;
    plant_types id;
    Plant_phase phase;
    bool grown;
};

struct ObjectData
{
    enum class Tag
    {
        InvElement,
        Element,
        Ingredient,
        Product,
        Plant,
        Animal,
    };

    struct InvElement_Body
    {
        InventoryElementData data;
    };

    struct Element_Body
    {
        ElementData data;
    };

    struct Ingredient_Body
    {
        IngredientData data;
    };

    struct Product_Body
    {
        ProductData data;
    };

    struct Plant_Body
    {
        PlantData data;
    };

    struct Animal_Body
    {
        InventoryElementData data;
    };

    Tag tag;
    union
    {
        InvElement_Body inv_element;
        Element_Body element;
        Ingredient_Body ingredient;
        Product_Body product;
        Plant_Body plant;
        Animal_Body animal;
    };
};

extern "C"
{

    const NetClient * init();

    void network_tick(const NetClient * client);

    extern void update_player(uintptr_t id, int32_t map_x, int32_t map_y, int32_t x, int32_t y);

    extern void update_chunk(int32_t x, int32_t y, const chunk_table * data);

    extern void got_id(uintptr_t id, int64_t seed);

    extern void update_inventory(uint8_t * data);

    extern void update_object(ObjectData data);

    extern void item_picked_up(uintptr_t iid, uintptr_t pid);

    extern void item_dropped(uintptr_t iid, uintptr_t pid);

    extern void item_used_on_object(uintptr_t item_id, uintptr_t object_id, uintptr_t player_id);

    extern void update_item_location(int32_t updates_number, uint8_t * data);

    extern void create_object_in_chunk(ObjectData data);

    extern void destroy_object(uintptr_t id, uint8_t * data);

    extern void failed_craft();

    void send_packet_move(const NetClient * client, int32_t x, int32_t y);

    void send_packet_pickup(const NetClient * client, uintptr_t id);

    void send_packet_drop(const NetClient * client, uintptr_t id);

    void send_packet_item_used_on_object(const NetClient * client, uintptr_t iid, uintptr_t oid);

    void send_packet_item_used_on_tile(const NetClient * client, uintptr_t iid, int32_t map_x, int32_t map_y, int32_t x, int32_t y);

    void send_packet_craft(const NetClient * client, uintptr_t prod_id, uintptr_t ingredients_num, const uintptr_t * iid);

    ObjectData foo();

} // extern "C"
#endif

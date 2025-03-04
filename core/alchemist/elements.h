#ifndef __ELEMENTS__H
#define __ELEMENTS__H

#include "names.h"
#include <cstdio>
#include <cstdlib>

class Edible
{
  public:
    unsigned int irrigation;
    unsigned int poison;
    unsigned int caloric;

    Edible();
    void show();
};

class Solid
{
  public:
    unsigned int stretching; // <-->
    unsigned int squeezing;  //  >--<
    unsigned int bending;
    unsigned int fragility;  // kruchosc
    unsigned int solubility; // rozpuszczalnosc
    Solid();
    void show();
};

enum Form
{
    Form_none = 0,
    Form_solid = 1,
    Form_liquid,
    Form_gas,
};

extern const char * Form_name[];

enum Class_id
{
    Class_Element = 1,
    Class_Ingredient,
    Class_Product,
    Class_Plant,
    Class_Animal,
    Class_Npc,
};

enum Item_id
{
    // SOLID START HERE
    ID_STONE,
    ID_LOG,
    ID_LOG1,
    ID_LOG2,
    ID_SAND,
    ID_STICK,
    ID_STRAWBERRY_SEEDS,
    ID_ACORN,
    ID_ACORN1,
    ID_ACORN2,
    ID_PUMPKIN_SEEDS,
    ID_WATERMELON_SEEDS,
    // LIQUID START HERE
    ID_WATER,
    // GAS START HERE
    ID_HELIUM,
    // FOOD START HERE
    ID_PUMPKIN,
    ID_WATERMELON,
    ID_CHERRY,
    ID_STRAWBERRY,
    ID_RAW_HAM
};

class BaseElement
{
  public:
    const char * name;

    int id; // texture id
    unsigned int density;
    Edible * edible;
    Form form;
    Solid * solid;

    BaseElement(int index);
    void show(bool details = true);
};

enum ItemLocationType
{
    LOCATION_CHUNK,
    LOCATION_PLAYER_INV,
};

union ItemLocationData
{
    struct
    {
        int map_x, map_y, x, y, z;
    } chunk;
    struct
    {
        int id;
    } player;
};

class ItemLocation
{
  public:
    enum ItemLocationType type;
    ItemLocationData data;
};
class chunk;
class InventoryElement
{
    // int x, y, z;
  protected:
    const char * name;

  public:
    ItemLocation location;
    size_t uid;
    Class_id c_id;
    Form req_form;
    bool known;
    InventoryElement()
    {
        req_form = Form_none;
        known = true;
        uid = (size_t)this;
        name = nullptr;
    }
    virtual bool use(int map_x, int map_y, int x, int y)
    {
        return false;
    }
    virtual bool use(InventoryElement * object)
    {
        return false;
    }
    virtual void show(bool details = true)
    {
    }
    virtual bool tick()
    {
        return false;
    }
    virtual Form get_form()
    {
        return req_form;
    }
    virtual const char * get_name()
    {
        return name;
    }
    virtual const char * get_form_name()
    {
        return Form_name[req_form];
    }
    virtual int get_id();
    virtual Edible * get_edible()
    {
        return NULL;
    }
    virtual BaseElement * get_base()
    {
        return NULL;
    }
    virtual bool craft()
    {
        printf("missing craft function\n");
        return false;
    }
    int get_x()
    {
        return location.data.chunk.x;
    }
    int get_y()
    {
        return location.data.chunk.y;
    }
    size_t get_uid()
    {
        return uid;
    }
    virtual unsigned int get_packet_size();
    virtual void to_bytes(unsigned char * buf);
};

enum object_types
{
    OBJECT_wall,
};

extern const char * object_names[];

#define OBJECTS 1

class Object : public InventoryElement
{
  public:
    BaseElement * base;
    enum object_types type;
    void * information;
    Form get_form()
    {
        return Form_solid;
    }
    const char * get_form_name()
    {
        return Form_name[Form_solid];
    }
    const char * get_name()
    {
        return object_names[type];
    }
    void show()
    {
        printf("Object type: %s", get_name());
        base->show();
    }
};

class Element : public InventoryElement
{
    BaseElement * base;

  public:
    unsigned int sharpness;
    unsigned int smoothness;
    unsigned int mass; // density*volume
    unsigned int length;
    unsigned int width;
    unsigned int height;
    unsigned int volume; // lenght*width*height

    virtual BaseElement * get_base()
    {
        return base;
    }
    Edible * get_edible()
    {
        return base->edible;
    }

    void show(bool details = true);

    Element(BaseElement * b);
    Form get_form()
    {
        return base->form;
    }
    const char * get_name()
    {
        return known ? base->name : "unknown";
    }
    const char * get_form_name()
    {
        return Form_name[base->form];
    }
    int get_id() override
    {
        return base->id;
    }
    unsigned int get_packet_size() override;
    void to_bytes(unsigned char * buf) override;
};

enum Ingredient_id
{
    ING_AXE_BLADE,
    ING_AXE_HANDLE,

    ING_KNIFE_BLADE,
    ING_KNIFE_HANDLE,

    ING_NUM,

};

enum Product_id
{
    PROD_AXE,
    PROD_KNIFE
};

extern const char * Ingredient_name[];
extern const char * Product_name[];
extern const char * items_name[];
extern const char * food_name[];

class Ingredient : public InventoryElement
{
    int * padding; // FIXME

  public:
    int quality;    //[0..100] slaby..najlepszy
    int resilience; // [0..100] wytrzymały..słaby
    int usage;      // [0..100] łatwy..trudny

    Ingredient_id id;
    InventoryElement * el; // available only in server , move to IngredientServer class

    int get_id()
    {
        return id;
    }
    bool craft();
    Edible * get_edible()
    { // FIXME
        return nullptr;
        // return el->get_edible();
    }
    Ingredient(InventoryElement * from, Ingredient_id i, Form f);
    void show(bool details = true);
    unsigned int get_packet_size() override;
    void to_bytes(unsigned char * buf) override;
};

class Product : public InventoryElement
{
    int * padding; // FIXME
    void init(Product_id i, int c, Form f);

  public:
    int quality;    //[0..100] slaby..najlepszy
    int resilience; // [0..100] wytrzymały..słaby
    int usage;      // [0..100] łatwy..trudny

    Product_id id;

    // available only in server , move to IngredientServer class
    int ing_count;
    InventoryElement ** ings;
    // above only in server

    int get_id()
    {
        return id;
    }
    Product(InventoryElement * el1, InventoryElement * el2, Product_id i, Form f);
    Product(InventoryElement ** from, int count, Product_id i, Form f);

    bool craft();
    virtual bool check_ing()
    {
        return false;
    }
    void show(bool details = true);
    unsigned int get_packet_size() override;
    void to_bytes(unsigned char * buf) override;
};

enum being_types
{
};
#define BEINGS 0

enum plant_types
{
    PLANTID_strawberry,
    PLANTID_tree,
    PLANTID_tree1,
    PLANTID_tree2,
    PLANTID_pumpkin,
    PLANTID_watermelon,
};

#define PLANTS 6

class Being : public InventoryElement
{
    int * padding; // FIXME
  public:
    unsigned int age;
    unsigned int max_age;
    bool alive;
    enum being_types type;

    virtual bool grow()
    {
        if (!alive)
            return false;
        age++;
        if (age >= max_age)
            alive = false;
        return alive;
    }
    Being()
    {
        alive = true;
        max_age = 1 + rand() % 36000; // 100 years
        age = rand() % max_age;
        name = create_name(5);
    }
    bool is_alive()
    {
        return alive;
    }
    void show(bool details = true)
    {
        printf("Being %s age=%d/%d alive=%d\n", name, age, max_age, alive);
    }
    bool tick()
    {
        return grow();
    }
};

enum animal_types
{
    ANIMALID_pig
};

#define ANIMALS 1

class Animal : public Being
{
  public:
    enum animal_types type;
    Animal();
    void show(bool details = true)
    {
        printf("Animal %s age=%d/%d alive=%d\n", name, age, max_age, alive);
    }
    bool tick()
    {
        return grow();
    }
};

class Npc : public Being
{
  public:
    Npc();
    void show(bool details = true)
    {
        printf("Npc %s age=%d/%d alive=%d\n", name, age, max_age, alive);
    }
};

enum Plant_phase
{
    Plant_seed = 0,
    Plant_seedling,
    Plant_growing,
    Plant_flowers,
    Plant_fruits
};

extern const char * Plant_phase_name[];

class Plant : public Being
{
  protected:
    unsigned int seedling_time;
    unsigned int growing_time;
    unsigned int flowers_time;

  public:
    bool planted;
    bool grown;
    int water;
    enum plant_types type;
    Plant_phase phase;
    Plant();
    unsigned int get_packet_size() override;
    void to_bytes(unsigned char * buf) override;
    void show(bool details = true)
    {
        printf("Plant -> %d name=%s age=%d/%d grown=%d\n", c_id, name, age, max_age, grown);
        if (details)
        {
            printf("phase=%s planted=%d times=%d/%d/%d/%d water=%d\n", Plant_phase_name[phase], planted, seedling_time, growing_time, flowers_time, max_age, water);
        }
    }
    void sow()
    {
        planted = 1;
    }
    void change_phase(Plant_phase p)
    {
        if (phase != p)
        {
            printf("%s growing: %s -> %s\n", name, Plant_phase_name[phase], Plant_phase_name[p]);
        }
        phase = p;
    }
};

#define SOLID_ELEMENTS 12
#define FOOD_ELEMENTS 5
#define LIQUID_ELEMENTS 1
#define GAS_ELEMENTS 1

#define NOT_FOOD_ELEMENTS (SOLID_ELEMENTS + LIQUID_ELEMENTS + GAS_ELEMENTS)

#define BASE_ELEMENTS (SOLID_ELEMENTS + FOOD_ELEMENTS + LIQUID_ELEMENTS + GAS_ELEMENTS)

#define ING_ELEMENTS 4
#define PROD_ELEMENTS 2

extern BaseElement * base_elements[BASE_ELEMENTS];

void init_elements();
void show_base_elements(bool details);

#endif

#ifndef __ELEMENTS__H
#define __ELEMENTS__H

#include "names.h"
#include <cstdio>
#include <cstdlib>

class Property
{
  public:
    const char * name;
    unsigned int value;
    Property(const char * n, unsigned int v)
    {
        name = n;
        value = v;
    }
    void show()
    {
        printf("%s = %u\n", name, value);
    }
};

class Edible
{
  public:
    Property * irrigation;
    Property * poison;
    Property * caloric;

    Edible();
    ~Edible();
    void show();
};

class Solid
{
  public:
    Property * stretching; // <-->
    Property * squeezing;  //  >--<
    Property * bending;
    Property * fragility;  // kruchosc
    Property * solubility; // rozpuszczalnosc
    Solid();
    ~Solid();
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

extern const char * Class_names[];

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
    Property * density;
    Edible * edible;
    Form form;
    Solid * solid;

    BaseElement(int index);
    ~BaseElement()
    {
        delete density;
    }
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
    const char * get_class_name()
    {
        return Class_names[c_id];
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
#ifndef CORE_FOR_CLIENT

    virtual bool craft()
    {
        printf("missing craft function\n");
        return false;
    }
#endif
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

    virtual char * get_description()
    {
        char * buf = new char[128];
        sprintf(buf, "%s: %s (%s)", get_class_name(), get_form_name(), get_name());
        return buf;
    }
    virtual Property ** get_properties(int * count)
    {
        *count = 0;
        return nullptr;
    }
    virtual ~InventoryElement()
    {
    }
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
    Property * sharpness;
    Property * smoothness;
    Property * mass; // density*volume
    Property * length;
    Property * width;
    Property * height;
    Property * volume; // lenght*width*height

    Property ** get_properties(int * count)
    {
        Property ** props = new Property *[7];
        props[0] = sharpness;
        props[1] = smoothness;
        props[2] = mass;
        props[3] = length;
        props[4] = width;
        props[5] = height;
        props[6] = volume;
        *count = 7;
        return props;
    }
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
    ~Element()
    {
        delete sharpness;
        delete smoothness;
        delete mass;
        delete length;
        delete width;
        delete height;
        delete volume;
    }
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
// extern const char * food_name[];

class Ingredient : public InventoryElement
{
    int * padding; // FIXME

  public:
    Property * quality;    //[0..100] slaby..najlepszy
    Property * resilience; // [0..100] wytrzymały..słaby
    Property * usage;      // [0..100] łatwy..trudny

    Ingredient_id id;

    int get_id()
    {
        return id;
    }
#ifdef CORE_FOR_CLIENT
    Ingredient(Ingredient_id i);
    Property ** get_properties(int * count)
    {
        Property ** props = new Property *[3];
        props[0] = quality;
        props[1] = resilience;
        props[2] = usage;

        *count = 3;
        return props;
    }

#else
    InventoryElement * el; // available only in server , move to IngredientServer class
    bool craft();
    Ingredient(InventoryElement * from, Ingredient_id i, Form f);
#endif
    Edible * get_edible()
    { // FIXME
        return nullptr;
        // return el->get_edible();
    }

    ~Ingredient()
    {
        delete quality;
        delete resilience;
        delete usage;
    }
    void show(bool details = true);
    unsigned int get_packet_size() override;
    void to_bytes(unsigned char * buf) override;
    char * get_description()
    {
        char * buf = new char[128];
        sprintf(buf, "%s: (%s)", get_class_name(), get_name());
        return buf;
    }
};

class Product : public InventoryElement
{
    int * padding; // FIXME
#ifndef CORE_FOR_CLIENT
    void init(Product_id i, int c, Form f);
#endif
  public:
    Property * quality;    //[0..100] slaby..najlepszy
    Property * resilience; // [0..100] wytrzymały..słaby
    Property * usage;      // [0..100] łatwy..trudny

    Product_id id;
    int get_id()
    {
        return id;
    }
#ifdef CORE_FOR_CLIENT
    Product(Product_id i);
    Property ** get_properties(int * count)
    {
        Property ** props = new Property *[3];
        props[0] = quality;
        props[1] = resilience;
        props[2] = usage;

        *count = 3;
        return props;
    }
#else
    int ing_count;
    InventoryElement ** ings;
    bool craft();

    Product(InventoryElement * el1, InventoryElement * el2, Product_id i, Form f);
    Product(InventoryElement ** from, int count, Product_id i, Form f);
#endif
    ~Product()
    {
        delete quality;
        delete resilience;
        delete usage;
    }
    virtual bool check_ing()
    {
        return false;
    }
    void show(bool details = true);

    char * get_description()
    {
        char * buf = new char[128];
        sprintf(buf, "%s: (%s)", get_class_name(), get_name());
        return buf;
    }
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
    // shared with client
    Property * age;
    Property * max_age;

    bool alive;
    bool can_talk;
    enum being_types type;

    virtual bool grow()
    {
        if (!alive)
        {
            //  printf("%s is dead\n", get_name());
            return false;
        }
        age->value++;
        // printf("%s:%s growing\n", get_class_name(), get_name());
        if (age->value >= max_age->value)
        {
            alive = false;
            printf("%s is dying\n", get_name());
        }
        return alive;
    }
    Being()
    {
        alive = true;

        max_age = new Property("max age", 1 + rand() % 36000); // 100 years
        age = new Property("age", rand() % max_age->value);

        name = create_name(5);
        printf("new Being: name=%s uid=%lx\n", name, uid);
        req_form = Form_solid;
        can_talk = false;
    }
    Property ** get_properties(int * count)
    {
        Property ** props = new Property *[2];
        props[0] = age;
        props[1] = max_age;

        *count = 2;
        return props;
    }
    ~Being()
    {
        delete age;
        delete max_age;
    }
    bool is_alive()
    {
        return alive;
    }
    void show(bool details = true)
    {
        printf("Being %s alive=%d uid=%lx\n", name, alive, uid);
        age->show();
        max_age->show();
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
    int padding; // FIXME
  public:
    // shared with client
    enum animal_types type;

    Animal();
    void show(bool details = true)
    {
        printf("Animal %s alive=%d uid=%lx\n", name, alive, uid);
        age->show();
        max_age->show();
    }
    /* bool tick()
     {
         return grow();
     }*/
};

class Npc : public Being
{
  public:
    Npc();
    void show(bool details = true)
    {
        printf("Npc %s alive=%d\n", name, alive);
        age->show();
        max_age->show();
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
    int padding; // FIXME

  protected:
    unsigned int seedling_time;
    unsigned int growing_time;
    unsigned int flowers_time;

  public:
    bool planted;
    int water;

    // shared with client
    enum plant_types type;
    Plant_phase phase;
    bool grown;

    Plant();
    unsigned int get_packet_size() override;
    void to_bytes(unsigned char * buf) override;
    void show(bool details = true)
    {
        printf("Plant -> %d name=%s grown=%d uid=%lx\n", c_id, name, grown, uid);
        age->show();
        max_age->show();
        if (details)
        {
            printf("phase=%s planted=%d times=%d/%d/%d/%d water=%d\n", Plant_phase_name[phase], planted, seedling_time, growing_time, flowers_time, max_age->value, water);
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
            printf("%s changing phase: %s -> %s age=%u/%u\n", name, Plant_phase_name[phase], Plant_phase_name[p], age->value, max_age->value);
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

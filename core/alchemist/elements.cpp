#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "../tiles.h"
#include "elements.h"
#include "names.h"

ElementsList * base_list;

const char * Form_name[]{"unknown", "solid form", "liquid", "gas"};

const char * Ingredient_name[] = {
    "Axe_blade",
    "Axe_handle",

    "Knife_blade",
    "Knife_handle",
};

const char * Product_name[] = {
    "Axe",
    "Knife",
};

const char * object_names[] = {"wall"};
const char * Plant_phase_name[] = {"Seed", "Seedling", "Growing", "Flowers", "Fruits"};
const char * Class_names[] = {"unknown", "BaseElement", "BaseAnimal", "BasePlant", "Element", "Ingredient", "Product", "Being", "Plant", "Animal", "Player", "Npc"};

Edible::Edible()
{
    caloric = new Property("caloric", rand() % 1000);
    ;
    irrigation = new Property("irrigation", rand() % 500);
    poison = new Property("poison", 0);
    if (rand() % 100 < 10)
    { // 10%
        poison->value = 1 + rand() % 250;
    }
}

Edible::~Edible()
{
    delete irrigation;
    delete poison;
    delete caloric;
}

void Edible::show()
{
    printf("      *** Edible ***\n");
    caloric->show();
    irrigation->show();
    poison->show();
}

Solid::Solid()
{
    stretching = new Property("stretching", 1 + rand() % 10000);
    // styropian: 1
    // beton:     5
    // cyna:     14
    // drewno:   800
    // diament: 1800
    // żelazo:  3800
    // stal:   10000

    squeezing = new Property("squezzing", 1 + rand() % 20000);
    // styropian:     1
    // beton:        50
    // kość:        150
    // kamień: 100-5000
    // diament:   17000

    fragility = new Property("fragility", 1000 * stretching->value / squeezing->value);
    // < 100 kruche
    // > spręzyste

    bending = new Property("bending", 1 + rand() % 100);
    // 1 łatwo zginalne
    // 100 trudno
    solubility = new Property("solubility", 1 + rand() % 100);
}

Solid::~Solid()
{
    delete stretching;
    delete squeezing;
    delete bending;
    delete fragility;
    delete solubility;
}

void Solid::show()
{
    printf("      *** Solid ***\n");

    stretching->show(); // rozciąganie
    squeezing->show();  // ściskanie
    bending->show();    // zginanie
    fragility->show();  // kruchość
    solubility->show(); // rozpuszczalność
}

BaseElement::BaseElement(Form f, int index) : Base(index, Class_BaseElement)
{
    solid = NULL;
    edible = NULL;
    density = nullptr;
    form = f;
    switch (f)
    {
        case Form_solid:
            solid = new Solid;
            density = new Property("density", 50 + rand() % 2000);
            break;
        case Form_liquid:
            density = new Property("density", 500 + rand() % 500);
            break;
        case Form_gas:
            density = new Property("density", 1);
            break;
    }
    if (rand() % 100 < 10) // 10% edible
        edible = new Edible;

    name = create_name(5 - form);
}

void BaseElement::show(bool details)
{
    Base::show(details);
    printf("BaseElement form=%s edible=%s\n", Form_name[form], edible ? "yes" : "no");
    if (!details)
        return;
    density->show(); // gęstość
    printf("   form = %s\n", Form_name[form]);
    switch (form)
    {
        case Form_solid:
            solid->show();
            break;
        default:
            break;
    }
    if (edible)
        edible->show();
}

#ifndef CORE_TUI
unsigned int InventoryElement::get_packet_size()
{
    // if (get_base())
    return sizeof(size_t) + sizeof(Class_id) + sizeof(ItemLocation);
    // return sizeof(int)*4;
}

void InventoryElement::to_bytes(unsigned char * buf)
{
    //    unsigned char* bytes = (unsigned char*)malloc(get_packet_size());
    int offset = 0;
    memcpy(&buf[offset], &uid, sizeof(uid));
    offset += sizeof(uid);
    memcpy(&buf[offset], &c_id, sizeof(Class_id));
    offset += sizeof(Class_id);

    memcpy(&buf[offset], &location, sizeof(location));
    offset += sizeof(location);

    printf("class %d - uid: %ld - %d %d\n", c_id, uid, location.data.chunk.x, location.data.chunk.y);
}
#endif
Element::Element()
{
    Class_id id = Class_BaseElement;
    BaseTable * table = dynamic_cast<BaseTable *>(base_list->find(&id));
    init(dynamic_cast<BaseElement *>(table->get_random()));
}
Element::Element(BaseElement * b)
{
    init(b);
}

Element::Element(int i)
{
    Class_id id = Class_BaseElement;
    BaseTable * table = dynamic_cast<BaseTable *>(base_list->find(&id));
    init(dynamic_cast<BaseElement *>(table->get(i)));
}

void Element::init(BaseElement * b)
{
    c_id = Class_Element;
    base = b;
    sharpness = new Property("sharpeness", rand() % 100);
    smoothness = new Property("smoothness", rand() % 100);
    length = new Property("length", 1 + rand() % 100);
    width = new Property("width", 1 + rand() % 100);
    height = new Property("height", 1 + rand() % 100);
    volume = new Property("volume", length->value * width->value * height->value);
    mass = new Property("mass", b->density->value * volume->value / 1000);
}

void Element::show(bool details)
{
    printf("%s: base=%s form=%s uid=%lx\n", Class_names[c_id], base->name, get_form_name(), uid);
    if (!details)
        return;
    sharpness->show();  // ostrość
    smoothness->show(); // gładkość
    length->show();
    width->show();
    height->show();
    volume->show();
    mass->show();

    base->show(details);
}

#ifndef CORE_TUI
unsigned int Element::get_packet_size()
{
    return InventoryElement::get_packet_size() + sizeof(unsigned int) * 7 + sizeof(int);
}

void Element::to_bytes(unsigned char * buf)
{
    InventoryElement::to_bytes(buf);
    int offset = InventoryElement::get_packet_size();
    // printf("packet size %d\n", offset);

    memcpy(&buf[offset], &sharpness, sizeof(sharpness->value));
    offset += sizeof(sharpness);
    memcpy(&buf[offset], &smoothness, sizeof(smoothness->value));
    offset += sizeof(smoothness);
    memcpy(&buf[offset], &mass, sizeof(mass->value));
    offset += sizeof(mass);
    memcpy(&buf[offset], &length, sizeof(length->value));
    offset += sizeof(length);
    memcpy(&buf[offset], &width, sizeof(width->value));
    offset += sizeof(width);
    memcpy(&buf[offset], &height, sizeof(height->value));
    offset += sizeof(height);
    memcpy(&buf[offset], &volume, sizeof(volume->value));
    offset += sizeof(volume);

    memcpy(&buf[offset], &base->id, sizeof(base->id));
    offset += sizeof(base->id);
    printf("element %d\n", base->id);
}
#endif

#ifdef CORE_FOR_CLIENT
Ingredient::Ingredient(Ingredient_id i)
{
    c_id = Class_Ingredient;
    name = Ingredient_name[i];
    id = i;

    quality = new Property("quality", 0);
    resilience = new Property("resilience", 0);
    usage = new Property("usage", 0);
    crafted = true;
}
#else
Ingredient::Ingredient(InventoryElement * from, Ingredient_id i, Form f)
{
    c_id = Class_Ingredient;
    el = from;
    name = Ingredient_name[i];
    id = i;
    req_form = f;
    quality = nullptr;
    resilience = nullptr;
    usage = nullptr;
}
bool Ingredient::craft()
{
    if (req_form != el->get_form())
    {
        printf("form != %d\n", req_form);
        return false;
    }

    quality = new Property("quality", rand() % 100);
    resilience = new Property("resilience", rand() % 100);
    usage = new Property("usage", rand() % 100);
    crafted = true;
    return true;
}
#endif

void Ingredient::show(bool details)
{
    printf("%s ->%d\n", name, c_id);
    if (!details)
        return;
    quality->show();
    resilience->show();
    usage->show();
    printf("form = %s", Form_name[req_form]);
}
#ifndef CORE_TUI
unsigned int Ingredient::get_packet_size()
{
    return InventoryElement::get_packet_size() + sizeof(Ingredient_id) + sizeof(int) * 3;
}

void Ingredient::to_bytes(unsigned char * buf)
{
    InventoryElement::to_bytes(buf);
    int offset = InventoryElement::get_packet_size();

    memcpy(&buf[offset], &quality, sizeof(quality->value));
    offset += sizeof(quality);
    memcpy(&buf[offset], &resilience, sizeof(resilience->value));
    offset += sizeof(resilience);
    memcpy(&buf[offset], &usage, sizeof(usage->value));
    offset += sizeof(usage);

    memcpy(&buf[offset], &id, sizeof(id));
    offset += sizeof(id);
}
#endif

#ifdef CORE_FOR_CLIENT
Product::Product(Product_id i)
{
    id = i;
    c_id = Class_Product;
    name = Product_name[i];
    quality = new Property("quality", 0);
    resilience = new Property("resilience", 0);
    usage = new Property("usage", 0);
    crafted = true;
}
#else
void Product::init(Product_id i, int c, Form f)
{
    name = Product_name[i];
    id = i;
    ing_count = c;
    req_form = f;
}

Product::Product(InventoryElement * el1, InventoryElement * el2, Product_id i, Form f)
{
    c_id = Class_Product;
    ings = (InventoryElement **)calloc(2, sizeof(InventoryElement));
    ings[0] = el1;
    ings[1] = el2;
    init(i, 2, f);
}

Product::Product(InventoryElement ** from, int count, Product_id i, Form f)
{
    c_id = Class_Product;
    ings = from;
    init(i, count, f);
}

bool Product::craft() // executed only on server
{
    for (int i = 0; i < ing_count; i++)
    {
        if (req_form != ings[i]->get_form())
        {
            printf("form != %d for inq[%d]\n", req_form, i);
            return false;
        }
    }
    if (!check_ing())
        return false;

    quality = new Property("quality", rand() % 100);
    resilience = new Property("resilience", rand() % 100);
    usage = new Property("usage", rand() % 100);
    crafted = true;
    return true;
}
#endif

void Product::show(bool details)
{
    printf("%s -> %d\n", name, c_id);
    if (!details)
        return;
    quality->show();
    resilience->show();
    usage->show();
}

void init_elements()
{
    base_list = new ElementsList("base list");

    BaseTable * base_elements = new BaseTable(BASE_ELEMENTS, Class_BaseElement);

    int i = 0;

    for (; i < SOLID_ELEMENTS; i++)
    {
        base_elements->add(i, new BaseElement(Form_solid, i));
    }
    for (; i < SOLID_ELEMENTS + LIQUID_ELEMENTS; i++)
    {
        base_elements->add(i, new BaseElement(Form_liquid, i));
    }
    for (; i < SOLID_ELEMENTS + LIQUID_ELEMENTS + GAS_ELEMENTS; i++)
    {
        base_elements->add(i, new BaseElement(Form_gas, i));
    }
    base_list->add(base_elements);

    BaseTable * base_animals = new BaseTable(BASE_ANIMALS, Class_BaseAnimal);

    for (i = 0; i < BASE_ANIMALS; i++)
    {
        base_animals->add(i, new BaseAnimal(i));
    }
    base_list->add(base_animals);

    BaseTable * base_plants = new BaseTable(BASE_PLANTS, Class_BasePlant);

    for (i = 0; i < BASE_PLANTS; i++)
    {
        base_plants->add(i, new BasePlant(i));
    }
    base_list->add(base_plants);
}

void show_base_table(Class_id id, bool details)
{
    BaseTable * table = dynamic_cast<BaseTable *>(base_list->find(&id));
    table->show(details);
}

void Animal::init(BaseAnimal * b)
{
    c_id = Class_Animal;
    base = b;
    alive = true;
    max_age = new Property("max age", 1 + rand() % 36000); // 100 years
    age = new Property("age", rand() % max_age->value);
    name = create_name(rand() % 2 + 2);
    can_talk = false;
}

Animal::Animal(BaseAnimal * b)
{
    init(b);
}

Animal::Animal()
{
    Class_id id = Class_BaseAnimal;
    BaseTable * table = dynamic_cast<BaseTable *>(base_list->find(&id));
    init(dynamic_cast<BaseAnimal *>(table->get_random()));
}

Animal::Animal(int i)
{
    Class_id id = Class_BaseAnimal;
    BaseTable * table = dynamic_cast<BaseTable *>(base_list->find(&id));
    init(dynamic_cast<BaseAnimal *>(table->get(i)));
}

Plant::Plant()
{
    Class_id id = Class_BasePlant;
    BaseTable * table = dynamic_cast<BaseTable *>(base_list->find(&id));
    init(dynamic_cast<BasePlant *>(table->get_random()));
}

Plant::Plant(int i)
{
    Class_id id = Class_BasePlant;
    BaseTable * table = dynamic_cast<BaseTable *>(base_list->find(&id));
    init(dynamic_cast<BasePlant *>(table->get(i)));
}

// FIXME split Plant to sertver/client side
void Plant::init(BasePlant * b)
{
    c_id = Class_Plant;
    base = b;
    seedling_time = 7 + rand() % 14;
    growing_time = seedling_time + rand() % 150;
    flowers_time = growing_time + rand() % 30;
    max_age = new Property("max age", flowers_time + rand() % 100);
    phase = (Plant_phase)(rand() % (Plant_fruits + 1));
    grown = false;
    water = rand() % 100;
    age = new Property("age", 0);
    can_talk = false;
    switch (phase)
    {
        case Plant_seed:
            age->value = 1;
            planted = false;
            break;
        case Plant_seedling:
            age->value = seedling_time;
            planted = true;
            break;
        case Plant_growing:
            age->value = growing_time;
            planted = true;
            break;
        case Plant_flowers:
            age->value = flowers_time;
            planted = true;
            break;
        case Plant_fruits:
            age->value = max_age->value;
            grown = true;
            planted = true;
            break;
    }
    name = create_name(rand() % 3 + 5);
}

Plant::Plant(BasePlant * b)
{
    init(b);
}

#ifndef CORE_TUI
unsigned int Plant::get_packet_size()
{
    return InventoryElement::get_packet_size() + sizeof(base->id) + sizeof(phase) + sizeof(grown);
}

void Plant::to_bytes(unsigned char * buf)
{
    InventoryElement::to_bytes(buf);
    int offset = InventoryElement::get_packet_size();

    memcpy(&buf[offset], &base->id, sizeof(base->id));
    memcpy(&buf[offset], &phase, sizeof(phase));
    offset += sizeof(phase);
    memcpy(&buf[offset], &grown, sizeof(grown));
    offset += sizeof(grown);
}
#endif

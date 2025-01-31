#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "../tiles.h"
#include "elements.h"
#include "names.h"

BaseElement * base_elements[BASE_ELEMENTS];

const char * Form_name[]{"nothing", "solid form", "liquid", "gas"};

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
const char * Class_names[] = {"unknown", "Element", "Ingredient", "Product", "Plant", "Animal", "Npc"};

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

BaseElement::BaseElement(int index)
{
    solid = NULL;
    edible = NULL;
    id = index;

    form = Form_solid;
    solid = new Solid;
    density = new Property("density", 50 + rand() % 2000);
    if (id >= SOLID_ELEMENTS)
    { // generate liquid
        form = Form_liquid;
        density->value = 500 + rand() % 500;
    }
    if (id >= SOLID_ELEMENTS + LIQUID_ELEMENTS)
    { // generate gas
        form = Form_gas;
        density->value = 1;
    }
    if (id >= SOLID_ELEMENTS + LIQUID_ELEMENTS + GAS_ELEMENTS) // generate food
    {
        form = Form_solid;
        density->value = 50 + rand() % 1000;
        solid = new Solid;
        edible = new Edible;
    }
    name = create_name(5 - form);
}

void BaseElement::show(bool details)
{
    printf("BaseElement name=%s form=%s id=%d\n", name, Form_name[form], id);
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

int InventoryElement::get_id()
{
    return -1;
}

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

Element::Element(BaseElement * b)
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
    printf("Element -> %d: base=%s form=%s\n", c_id, base->name, get_form_name());
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

#ifdef CORE_FOR_CLIENT
Ingredient::Ingredient(Ingredient_id i)
{
    c_id = Class_Ingredient;
    name = Ingredient_name[i];
    id = i;

    quality = new Property("quality", 0);
    resilience = new Property("resilience", 0);
    usage = new Property("usage", 0);
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
    if (req_form != get_form())
    {
        printf("form != %d\n", req_form);
        return false;
    }

    quality = new Property("quality", rand() % 100);
    resilience = new Property("resilience", rand() % 100);
    usage = new Property("usage", rand() % 100);
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

#ifdef CORE_FOR_CLIENT
Product::Product(Product_id i)
{
    id = i;
    c_id = Class_Product;
    name = Product_name[i];
    quality = new Property("quality", 0);
    resilience = new Property("resilience", 0);
    usage = new Property("usage", 0);
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
    for (int i = 0; i < BASE_ELEMENTS; i++)
    {
        base_elements[i] = new BaseElement(i);
    }
}

void show_base_elements(bool details)
{
    for (int i = 0; i < BASE_ELEMENTS; i++)
    {
        base_elements[i]->show(details);
    }
}

Animal::Animal()
{
    c_id = Class_Animal;
    alive = true;
    max_age = new Property("max age", 1 + rand() % 36000); // 100 years
    age = new Property("age", rand() % max_age->value);
    name = create_name(rand() % 2 + 2);
    can_talk = false;
}

Npc::Npc()
{
    c_id = Class_Npc;
    alive = true;
    max_age = new Property("max age", 1 + rand() % 18000); // 50 years
    age = new Property("age", rand() % max_age->value);
    name = create_name(rand() % 3 + 3);
    can_talk = true;
}

// FIXME split Plant to server/client side
Plant::Plant()
{
    c_id = Class_Plant;
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
}

unsigned int Plant::get_packet_size()
{
    return InventoryElement::get_packet_size() + sizeof(type) + sizeof(phase) + sizeof(grown);
}

void Plant::to_bytes(unsigned char * buf)
{
    InventoryElement::to_bytes(buf);
    int offset = InventoryElement::get_packet_size();

    memcpy(&buf[offset], &type, sizeof(type));
    offset += sizeof(type);
    memcpy(&buf[offset], &phase, sizeof(phase));
    offset += sizeof(phase);
    memcpy(&buf[offset], &grown, sizeof(grown));
    offset += sizeof(grown);
}

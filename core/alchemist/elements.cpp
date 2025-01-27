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

const char * items_name[] = {
    "Stone",
    "Log",
    "Sand",
    "Stick",
};

const char * object_names[] = {"wall"};

const char * food_name[] = {"Pumpkin", "Watermelon"};

const char * Plant_phase_name[] = {"Seed", "Seedling", "Growing", "Flowers", "Fruits"};

Edible::Edible()
{
    caloric = rand() % 1000;
    irrigation = rand() % 500;
    poison = 0;
    if (rand() % 100 < 10)
    { // 10%
        poison = 1 + rand() % 250;
    }
}

void Edible::show()
{
    printf("      *** Edible ***\n");
    printf("      caloric = %u\n", caloric);
    printf("      irrigation = %u\n", irrigation);
    if (poison)
    {
        printf("           *** Poison ***\n");
        printf("           poison = %u\n", poison);
    }
}

Solid::Solid()
{
    stretching = 1 + rand() % 10000;
    // styropian: 1
    // beton:     5
    // cyna:     14
    // drewno:   800
    // diament: 1800
    // żelazo:  3800
    // stal:   10000

    squeezing = 1 + rand() % 20000;
    // styropian:     1
    // beton:        50
    // kość:        150
    // kamień: 100-5000
    // diament:   17000

    fragility = 1000 * stretching / squeezing;
    // < 100 kruche
    // > spręzyste

    bending = 1 + rand() % 100;
    // 1 łatwo zginalne
    // 100 trudno
    solubility = 1 + rand() % 100;
}

void Solid::show()
{
    printf("      *** Solid ***\n");
    printf("      stretching = %u\n", stretching); // rozciąganie
    printf("      squeezing = %u\n", squeezing);   // ściskanie
    printf("      bending = %u\n", bending);       // zginanie
    printf("      fragility = %u\n", fragility);   // kruchość
    printf("      solubility = %u\n", solubility); // rozpuszczalność
}

BaseElement::BaseElement(int index)
{
    solid = NULL;
    edible = NULL;
    id = index;

    form = Form_solid;
    solid = new Solid;
    density = 50 + rand() % 2000;
    if (id >= SOLID_ELEMENTS)
    { // generate liquid
        form = Form_liquid;
        density = 500 + rand() % 500;
    }
    if (id >= SOLID_ELEMENTS + LIQUID_ELEMENTS)
    { // generate gas
        form = Form_gas;
        density = 1;
    }
    if (id >= SOLID_ELEMENTS + LIQUID_ELEMENTS + GAS_ELEMENTS) // generate food
    {
        form = Form_solid;
        density = 50 + rand() % 1000;
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
    printf("   density = %u\n", density); // gęstość
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
    sharpness = rand() % 100;
    smoothness = rand() % 100;
    length = 1 + rand() % 100;
    width = 1 + rand() % 100;
    height = 1 + rand() % 100;
    volume = length * width * height;
    mass = b->density * volume / 1000;
}

void Element::show(bool details)
{
    printf("Element -> %d: base=%s form=%s\n", c_id, base->name, get_form_name());
    if (!details)
        return;
    printf("sharpness = %u\n", sharpness);   // ostrość
    printf("smoothness = %u\n", smoothness); // gładkość
    printf("mass = %u: l=%u w=%u h=%u \n", mass, length, width, height);
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

    memcpy(&buf[offset], &sharpness, sizeof(sharpness));
    offset += sizeof(sharpness);
    memcpy(&buf[offset], &smoothness, sizeof(smoothness));
    offset += sizeof(smoothness);
    memcpy(&buf[offset], &mass, sizeof(mass));
    offset += sizeof(mass);
    memcpy(&buf[offset], &length, sizeof(length));
    offset += sizeof(length);
    memcpy(&buf[offset], &width, sizeof(width));
    offset += sizeof(width);
    memcpy(&buf[offset], &height, sizeof(height));
    offset += sizeof(height);
    memcpy(&buf[offset], &volume, sizeof(volume));
    offset += sizeof(volume);

    memcpy(&buf[offset], &base->id, sizeof(base->id));
    offset += sizeof(base->id);
    printf("element %d\n", base->id);
}

Ingredient::Ingredient(InventoryElement * from, Ingredient_id i, Form f)
{
    c_id = Class_Ingredient;
    el = from;
    name = Ingredient_name[i];
    id = i;
    req_form = f;
}

bool Ingredient::craft()
{
    if (req_form != get_form())
    {
        printf("form != %d\n", req_form);
        return false;
    }

    quality = rand() % 100;
    resilience = rand() % 100;
    usage = rand() % 100;
    return true;
}

void Ingredient::show(bool details)
{
    printf("%s ->%d\n", name, c_id);
    if (!details)
        return;
    printf("quality = %d\n", quality);
    printf("resilience = %d\n", resilience);
    printf("usage = %d\n", usage);
}

unsigned int Ingredient::get_packet_size()
{
    return InventoryElement::get_packet_size() + sizeof(Ingredient_id) + sizeof(int) * 3;
}

void Ingredient::to_bytes(unsigned char * buf)
{
    InventoryElement::to_bytes(buf);
    int offset = InventoryElement::get_packet_size();

    memcpy(&buf[offset], &quality, sizeof(quality));
    offset += sizeof(quality);
    memcpy(&buf[offset], &resilience, sizeof(resilience));
    offset += sizeof(resilience);
    memcpy(&buf[offset], &usage, sizeof(usage));
    offset += sizeof(usage);

    memcpy(&buf[offset], &id, sizeof(id));
    offset += sizeof(id);
}

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

bool Product::craft()
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

    quality = rand() % 100;
    resilience = rand() % 100;
    usage = rand() % 100;
    return true;
}

void Product::show(bool details)
{
    printf("%s -> %d\n", name, c_id);
    if (!details)
        return;
    printf("quality = %d\n", quality);
    printf("resilience = %d\n", resilience);
    printf("usage = %d\n", usage);
}

unsigned int Product::get_packet_size()
{
    return InventoryElement::get_packet_size() + sizeof(Product_id) + sizeof(int) * 3;
}

void Product::to_bytes(unsigned char * buf)
{
    InventoryElement::to_bytes(buf);
    int offset = InventoryElement::get_packet_size();

    memcpy(&buf[offset], &quality, sizeof(quality));
    offset += sizeof(quality);
    memcpy(&buf[offset], &resilience, sizeof(resilience));
    offset += sizeof(resilience);
    memcpy(&buf[offset], &usage, sizeof(usage));
    offset += sizeof(usage);

    memcpy(&buf[offset], &id, sizeof(id));
    offset += sizeof(id);
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
    max_age = 1 + rand() % 36000; // 100 years
    age = rand() % max_age;
    name = create_name(rand() % 2 + 2);
}

Npc::Npc()
{
    c_id = Class_Animal;
    alive = true;
    max_age = 1 + rand() % 18000; // 50 years
    age = rand() % max_age;
    name = create_name(rand() % 3 + 3);
}

Plant::Plant()
{
    c_id = Class_Plant;
    seedling_time = 7 + rand() % 14;
    growing_time = seedling_time + rand() % 150;
    flowers_time = growing_time + rand() % 30;
    max_age = flowers_time + rand() % 100;
    phase = (Plant_phase)(rand() % (Plant_fruits + 1));
    grown = false;
    water = rand() % 100;
    switch (phase)
    {
        case Plant_seed:
            age = 1;
            planted = false;
            break;
        case Plant_seedling:
            age = seedling_time;
            planted = true;
            break;
        case Plant_growing:
            age = growing_time;
            planted = true;
            break;
        case Plant_flowers:
            age = flowers_time;
            planted = true;
            break;
        case Plant_fruits:
            age = max_age;
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

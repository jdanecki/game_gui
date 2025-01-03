#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "elements.h"
#include "names.h"
#include "../tiles.h"
#include "../networking.h"

BaseElement *base_elements[BASE_ELEMENTS];

const char * Form_name[]
{
    "nothing",
    "solid form",
    "liquid",
    "gas"
};

const char * Ingredient_name[]=
{
    "Axe_blade",
    "Axe_handle",

    "Knife_blade",
    "Knife_handle",
};

const char * Product_name[]=
{
    "Axe",
    "Knife",
};

const char * items_name[]=
{
    "Stone",
    "Log",
    "Sand",
    "Stick",
};

const char * object_names[]=
{
    "wall"
};

const char * food_name[]=
{
    "Pumpkin",
    "Watermelon"
};

const char * Plant_phase_name[]=
{
    "Seed",
    "Seedling",
    "Growing",
    "Flowers",
    "Fruits"
};

Edible::Edible()
{
   caloric=rand() % 1000;
   irrigation=rand() % 500;
   poison=0;
   if (rand() %100 < 10) { // 10%
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
    stretching=1 + rand() % 10000;
            //styropian: 1
            //beton:     5
            //cyna:     14
            //drewno:   800
            //diament: 1800
            //żelazo:  3800
            //stal:   10000

    squeezing = 1 + rand() % 20000;
            //styropian:     1
            //beton:        50
            //kość:        150
            //kamień: 100-5000
            //diament:   17000

    fragility = 1000 * stretching / squeezing;
        // < 100 kruche
        // > spręzyste

    bending = 1 + rand() % 100;
            //1 łatwo zginalne 
            //100 trudno
    solubility=1+rand() % 100;
}

void Solid::show()
{
    printf("      *** Solid ***\n");
    printf("      stretching = %u\n", stretching); // rozciąganie
    printf("      squeezing = %u\n", squeezing); //ściskanie
    printf("      bending = %u\n", bending); // zginanie
    printf("      fragility = %u\n", fragility); //kruchość
    printf("      solubility = %u\n", solubility); //rozpuszczalność

}

BaseElement::BaseElement(int index)
{
    solid = NULL;
    edible = NULL;
    id = index;

    form = Form_solid;
    solid = new Solid;
    density = 50 + rand() % 2000;
    if (id >= SOLID_ELEMENTS) { // generate liquid
        form = Form_liquid;
        density = 500 + rand() % 500;
    } 
    if (id >= SOLID_ELEMENTS+LIQUID_ELEMENTS) { // generate gas
        form = Form_gas;
        density = 1;
    }
    if (id >= SOLID_ELEMENTS+LIQUID_ELEMENTS+GAS_ELEMENTS) // generate food
    {
        form = Form_solid;
        density = 50 + rand() % 1000;
        solid =new Solid;
        edible=new Edible;
    }
    name = create_name(5 - form);
}

void BaseElement::show(bool details)
{
    printf("BaseElement name=%s form=%s id=%d\n", name, Form_name[form],id); 
    if (!details) return;
    printf("   density = %u\n", density); //gęstość
    printf("   form = %s\n", Form_name[form]);
    switch(form)
    {
        case Form_solid:
            solid->show();
            break;
        default: break;
    }
    if (edible) edible->show();
}

unsigned int InventoryElement::get_packet_size()
{
    //if (get_base())
        return sizeof(size_t) + sizeof(int) * 4 + sizeof(Class_id);
    //return sizeof(int)*4;
}

unsigned char* InventoryElement::to_bytes()
{
    unsigned char* bytes = (unsigned char*)malloc(get_packet_size());
    int offset = 0;
    memcpy(&bytes[offset], &uid, sizeof(uid));
    offset += sizeof(uid);
    memcpy(&bytes[offset], &c_id, sizeof(Class_id));
    offset += sizeof(Class_id);
    memcpy(&bytes[offset], &location.data.chunk.x, sizeof(location.data.chunk.x));
    offset += sizeof(location.data.chunk.x);
    memcpy(&bytes[offset], &location.data.chunk.y, sizeof(location.data.chunk.y));
    offset += sizeof(location.data.chunk.y);
    memcpy(&bytes[offset], &location.data.chunk.z, sizeof(location.data.chunk.z));
    offset += sizeof(location.data.chunk.z);

    printf("class %d - uid: %ld - %d %d", c_id, uid, location.data.chunk.x, location.data.chunk.y);
    if (get_base())
    {
        memcpy(&bytes[offset], &get_base()->id, sizeof(int));
        offset += sizeof(int);
        printf(", id %d", get_base()->id);
    }
    offset += sizeof(int);
    printf("\n");
    return bytes;
}

Element::Element(BaseElement *b)
{
    c_id=Class_Element;
    base = b;
    sharpness = rand() % 100;
    smoothness = rand() % 100;
    length = 1 + rand() % 100;
    width = 1 + rand() % 100;
    height = 1 + rand() % 100;
    volume = length * width * height;
    mass = b->density * volume/1000;
    
}

void Element::show(bool details)
{
    printf("Element -> %d: base=%s form=%s\n", c_id, base->name, get_form_name());
    if (!details) return;
    printf("sharpness = %u\n", sharpness); //ostrość
    printf("smoothness = %u\n", smoothness); //gładkość
    printf("mass = %u: l=%u w=%u h=%u \n", mass, length, width, height);
    base->show(details);
}

Ingredient::Ingredient(InventoryElement * from, Ingredient_id i, Form f)
{
    c_id=Class_Ingredient;
    el = from;
    name = Ingredient_name[i];
    id = i;
    req_form = f;
}
        
bool Ingredient::craft()
{
    if (req_form != get_form()) {
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
    if (!details) return;
    printf("quality = %d\n", quality);
    printf("resilience = %d\n", resilience);
    printf("usage = %d\n", usage);
    el->show(details);
}
        
void Product::init(Product_id i, int c, Form f)
{
    name = Product_name[i];
    id = i;
    ing_count=c;
    req_form=f;
}

Product::Product(InventoryElement * el1, InventoryElement *el2, Product_id i, Form f)
{
    c_id=Class_Product;
    ings = (InventoryElement**) calloc(2, sizeof(InventoryElement));
    ings[0]=el1;
    ings[1]=el2;
    init(i, 2, f);
}

Product::Product(InventoryElement ** from, int count, Product_id i, Form f)
{
    c_id=Class_Product;
    ings = from;
    init(i, count, f);
}

bool Product::craft()
{
    for (int i=0; i < ing_count; i++)
    {
        if (req_form != ings[i]->get_form()) {
            printf("form != %d for inq[%d]\n", req_form, i);       
            return false;
        }
    }
    if (!check_ing()) return false;

    quality = rand() % 100;
    resilience = rand() % 100;
    usage = rand() % 100;
    return true;
}

void Product::show(bool details)
{
    printf("%s -> %d\n", name, c_id);
    if (!details) return;
    printf("quality = %d\n", quality);
    printf("resilience = %d\n", resilience);
    printf("usage = %d\n", usage);

    for (int i=0; i < ing_count; i++)
    {
    }
}

Form Product::get_form()
{
    int solid=0;
    int liq=0;
    int gas=0;

    for (int i=0; i < ing_count; i++)
    {
        switch (ings[i]->get_form())
        {
            case Form_solid: solid++; break;
            case Form_liquid: liq++; break;
            case Form_gas: gas++; break;
            default: return Form_none;
        }
    }
    if (solid) return Form_solid;
    if (gas == ing_count) return Form_gas;
    if (liq) return Form_liquid;
    return Form_none;
}

const char * Product::get_form_name() 
{ 
    return Form_name[get_form()]; 
}
void init_elements()
{
    for (int i=0; i < BASE_ELEMENTS; i++)
    {
        base_elements[i] = new BaseElement(i);
    }
}

void show_base_elements(bool details)
{
    for (int i=0; i < BASE_ELEMENTS; i++)
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

void Animal::move()
{
    // TODO: send info about movement for animations
    int _x = location.data.chunk.x;
    int _y =  location.data.chunk.y;

    //get_posittion(&_x, &_y);

    switch (rand() % 4)
    {
        case 0: // up
            _y--;
            break;
        case 1: // down
            _y++;
            break;
        case 2: // left
            //going_right=false;
            _x--;
            break;
        case 3: // right
            //going_right=true;
            _x++;
            break;
    }

    if (_x >= CHUNK_SIZE) _x=CHUNK_SIZE-1;
    if (_y >= CHUNK_SIZE) _y=CHUNK_SIZE-1;
    if (_y < 0) _y=0;
    if (_y < 0) _y=0;
    if (_x < 0) _x=0;

    //set_posittion(_x, _y);
    location.data.chunk.x = 7;
    location.data.chunk.y = 0;

    objects_to_update.add(this);
}

Plant::Plant()
{
    c_id=Class_Plant;
    seedling_time=7 + rand() % 14;
    growing_time=seedling_time + rand() % 150;
    flowers_time=growing_time + rand() % 30;
    max_age=flowers_time + rand() % 100;
    phase = (Plant_phase) (rand() %  (Plant_fruits+1));
    grown=false;
    water=rand() % 100; 
    switch (phase)
    {
        case Plant_seed: age = 1; planted = false; break;
        case Plant_seedling: age = seedling_time;  planted = true; break;
        case Plant_growing: age = growing_time; planted = true; break;
        case Plant_flowers: age = flowers_time; planted = true; break;
        case Plant_fruits: age = max_age; grown=true; planted = true; break;
    }
}

bool Plant::grow()
{
    if (grown) return false;
    if (!water) return !grown;
    water--;
    age++;
    if (age >= max_age) grown = true;
    if (age >= max_age) { change_phase(Plant_fruits); return !grown; }
    if (age >= flowers_time) { change_phase(Plant_flowers); return !grown; }
    if (age >= growing_time) { change_phase(Plant_growing); return !grown; }
    if (age >= seedling_time) { change_phase(Plant_seedling); return !grown; }
    return !grown;
}

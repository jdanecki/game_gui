#include "plants.h"
#include "show_list.h"

extern InvList * inventory;
extern InvList * elements;
InvList * plants;

void sow_plant()
{
    ListElement * cur = elements->head;
    while (cur)
    {
        if (cur->el->c_id != Class_Plant)
            cur->disable();
        cur = cur->next;
    }

    InventoryElement * el = select_element(elements);
    elements->enable_all();
    if (!el)
        return;
    elements->remove(el);
    plants->add(el);
    Plant * p = (Plant *)el;
    p->sow();
    printf("%s planted\n", el->get_name());
}

void harvest_plant()
{
    ListElement * cur = plants->head;
    while (cur)
    {
        Plant * p = (Plant *)cur->el;
        if (!p->planted)
            cur->disable();
        cur = cur->next;
    }

    InventoryElement * el = select_element(plants);
    plants->enable_all();
    if (!el)
        return;
    inventory->add(el);
    plants->remove(el);
    printf("plant: %s harvested to inventory\n", el->get_name());
}

void add_new_plant()
{
    Plant * p = new Plant();
    if (p->phase == Plant_seed)
    {
        elements->add(p);
        printf("new Plant seed %s found\n", p->get_name());
    }
    else
    {
        plants->add(p);
        printf("new Plant %s found\n", p->get_name());
    }
}

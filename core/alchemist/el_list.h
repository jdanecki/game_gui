#ifndef EL_LIST_H
#define EL_LIST_H

#include "elements.h"

class ListElement
{
    bool enabled;

  public:
    InventoryElement * el;
    ListElement * next;

    void add(ListElement * entry);
    void disable()
    {
        enabled = false;
    };
    void enable()
    {
        enabled = true;
    };
    bool is_enabled()
    {
        return enabled;
    };
    virtual void show(bool details = true);
    virtual bool tick()
    {
        return el->tick();
    };
    ListElement(InventoryElement * entry);
    ListElement() : el(nullptr), next(nullptr)
    {
        enable();
    }
};

class InvList
{
  public:
    const char * name;
    int nr_elements;
    ListElement * head;
    ListElement * tail;

    InvList(const char * n);
    InvList();
    ListElement * find(void * what);
    bool virtual find_check(ListElement * el, void * what)
    {
        return el == what;
    }
    //    bool virtual find_at_check(ListElement *el, void * pos) { return false; }
    InventoryElement ** find_form(enum Form f, int * count);
    InventoryElement ** find_id(enum Item_id id, int * count);
    void show(bool details = true);
    void add(InventoryElement * el);
    void add(ListElement * el);
    void remove(InventoryElement * el);
    int get_count(InventoryElement * el);
    void tick();
    void enable_all();
    int size();
};

#endif // EL_LIST_H

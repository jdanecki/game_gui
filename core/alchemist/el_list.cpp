#include "el_list.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <sys/ioctl.h>
#include <unistd.h>

ListElement::ListElement(InventoryElement * entry)
{
    el = entry;
    next = NULL;
    enabled = true;
}

void ListElement::add(ListElement * entry)
{
    next = entry;
}

void ListElement::show(bool details)
{
    el->show(details);
}

InvList::InvList(const char * n)
{
    name = n;
    nr_elements = 0;
    tail = NULL;
    head = NULL;
}

InvList::InvList()
{
    name = "foo";
    nr_elements = 0;
    tail = NULL;
    head = NULL;
}

ListElement * InvList::find(void * what)
{
    ListElement * cur = head;
    while (cur)
    {
        if (find_check(cur, what))
            return cur;
        cur = cur->next;
    }
    return NULL;
}

InventoryElement ** InvList::find_form(enum Form f, int * count)
{
    ListElement * cur = head;
    InventoryElement ** a = (InventoryElement **)calloc(nr_elements, sizeof(InventoryElement *));
    int c = 0;
    while (cur)
    {
        if (cur->el->get_form() == f)
        {
            a[c] = cur->el;
            c++;
        }
        cur = cur->next;
    }
    if (!c)
    {
        free(a);
        return NULL;
    }
    else
    {
        *count = c;
        return a;
    }
}

InventoryElement ** InvList::find_id(enum Item_id id, int * count)
{
    ListElement * cur = head;
    InventoryElement ** a = (InventoryElement **)calloc(nr_elements, sizeof(InventoryElement *));
    int c = 0;

    while (cur)
    {
        if (cur && cur->el && cur->el->get_base() && cur->el->get_base()->id == id)
        {
            a[c] = cur->el;
            c++;
        }
        cur = cur->next;
    }

    if (!c)
    {
        free(a);
        return NULL;
    }
    else
    {
        if (count)
            *count = c;
        return a;
    }
}

void InvList::show(bool details)
{
    ListElement * cur = head;
    printf("--- %s (%d) ---\n", name, nr_elements);
    while (cur)
    {
        cur->show(details);
        cur = cur->next;
    }
}

void InvList::enable_all()
{
    ListElement * cur = head;

    while (cur)
    {
        cur->enable();
        cur = cur->next;
    }
}

int InvList::size()
{
    int size = 0;
    ListElement * cur = head;
    while (cur)
    {
        size++;
        cur = cur->next;
    }
    return size;
}

void InvList::tick()
{
    ListElement * cur = head;

    while (cur)
    {
        ListElement * next = cur->next;
        bool alive = cur->tick();
        if (!alive)
        {
            remove(cur->el);
        }
        cur = next;
    }
}

void InvList::add(InventoryElement * el)
{
    ListElement * entry = new ListElement(el);
    add(entry);
}

void InvList::add(ListElement * entry)
{
    if (!entry)
    {
        printf("adding NULL pointer\n");
    }
    if (nr_elements)
    {
        tail->add(entry);
        tail = entry;
    }
    else
    {
        head = entry;
        tail = entry;
    }
    nr_elements++;
}

void InvList::remove(InventoryElement * el)
{
    if (!head)
        return;
    ListElement * cur = head;
    ListElement * tmp;
    if (head->el == el)
    {
        tmp = head->next;
        if (!tail)
        {
            printf("!!! tail is null %s\n", el->get_name());
            exit(0);
        }
        if (tail->el == el) // only 1 element on the list
        {
            if (head == tail)
                tail = NULL;
        }
        free(head);
        nr_elements--;
        head = tmp;
        return;
    }
    while (cur) // more then 1 element on the list
    {
        if (!cur->next)
            break;
        if (cur->next->el == el)
        {
            tmp = cur->next;
            cur->next = cur->next->next;
            if (tail->el == el)
            {
                tail = cur;
            }
            free(tmp);
            nr_elements--;
            return;
        }
        cur = cur->next;
    }
}

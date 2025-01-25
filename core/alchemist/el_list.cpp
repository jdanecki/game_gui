#include "el_list.h"
#include "game_time.h"
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
            //   printf("--- %s (%d) ---\n", name, nr_elements);
            //  printf("removing last element %s ", el->get_name());
            if (head == tail)
                tail = NULL;
        }
        free(head);
        nr_elements--;
        // printf("    removing first element %s\n", el->get_name());
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
            //  printf("--- %s (%d) ---\n", name, nr_elements);
            //  printf("removing element %s\n", el->get_name());
            return;
        }
        cur = cur->next;
    }
}

const char * colorGray = "\033[1;30m";
const char * colorRed = "\033[2;31m";
const char * colorRedBold = "\033[1;31m";
const char * colorGreen = "\033[2;32m";
const char * colorGreenBold = "\033[1;32m";
const char * colorYellow = "\033[2;33m";
const char * colorYellowBold = "\033[1;33m";
const char * colorBlue = "\033[2;34m";
const char * colorMagenta = "\033[2;35m";
const char * colorCyan = "\033[2;36m";
const char * colorWhite = "\033[1;37m";
const char * colorNormal = "\033[0m";
const char * clrscr = "\033[H\033[J";

int kbhit()
{
    int lf;

    if (ioctl(0, FIONREAD, &lf) == 0)
        return lf;
    else
        return -1;
}

char wait_key(char prompt)
{
    printf("\r%s [%c] > ", game_time->get_time(), prompt);
    while (!kbhit())
    {
        usleep(1000);
    }
    char c;
    if (read(0, &c, 1) == 1)
    {
        printf("%c\n", c);
        return c;
    }
    else
        return 0;
}

Show_el::Show_el(char _c, ListElement * _el)
{
    c = _c;
    l_el = _el;
    selected = false;
}

void Show_el::show(bool details)
{
    printf("%c (%c) - ", c, selected ? '*' : ' ');
    l_el->show(details);
}

bool Show_list::find_check(ListElement * _el, void * what)
{
    Show_el * s_el = (Show_el *)_el;
    char * c = (char *)what;
    if (s_el->c == *c)
        return true;
    return false;
}

ListElement * Show_list::select_el()
{
    char c = wait_key(prompt);
    Show_el * f = (Show_el *)find(&c);
    if (f)
        return f->l_el;
    return nullptr;
}

bool Show_list::multi_select()
{
    char c = 0;
    bool sel = 0;
    printf("z - zakończ selekcje\n");
    while (1)
    {
        c = wait_key(prompt);
        if (c == 'z')
            break;
        Show_el * f = (Show_el *)find(&c);
        if (f)
        {
            f->selected ^= true;
            sel = true;
        }
        show(false);
        printf("z - zakończ selekcje\n");
    }
    return sel;
}

void Show_list::unselect_all()
{
    ListElement * cur = head;
    while (cur)
    {
        Show_el * _el = (Show_el *)cur;
        _el->selected = false;
        cur = cur->next;
    }
}

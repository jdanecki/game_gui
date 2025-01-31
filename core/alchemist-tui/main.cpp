#include "../alchemist/el_list.h"
#include "game_time.h"
#include "npc_talk.h"
#include "plants.h"
#include "show_list.h"
#include "test_axe.h"
#include <cstdio>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

InvList * inventory;
InvList * elements;
InvList * animals;
InvList * npcs;

void (*callback_daily)();

void daily_call()
{
    printf("daily call\n");
    plants->tick();
    animals->tick();
}

void notify_destroy(size_t id, ItemLocation location)
{
}

void update_location(size_t id, ItemLocation old_loc, ItemLocation new_loc)
{
}

void help()
{
    printf("%sESC/e/?- clear screen/Exit/Help\n", colorWhite);
    printf("t - Test\n");
    printf("c - Craft\n");
    printf("s - Show\n");
    printf("h - Harvest plant\n");
    printf("o - sOw plant\n");
    printf("@ - change clock\n");
    printf("f - Find\n");
    printf("p - Pickup up element\n");
    printf("d - Drop element\n");
    printf("# - conversation\n");
}

void show()
{
    printf("%sb/B - base elements (details off/on)\n", colorCyan);
    printf("e/E - elements (details off/on)\n");
    printf("i/I - inventory (details off/on)\n");
    printf("p/P - plants (details off/on)\n");
    printf("a/A - animals (details off/on)\n");
    printf("n/N - npcs (details off/on)\n");
    printf("%s%s", colorNormal, colorGreenBold);

    char c = wait_key('s');
    switch (c)
    {
        case 'b':
            show_base_elements(false);
            break;
        case 'i':
            inventory->show(false);
            break;
        case 'e':
            elements->show(false);
            break;
        case 'a':
            animals->show(false);
            break;
        case 'p':
            plants->show(false);
            break;
        case 'n':
            npcs->show(false);
            break;
        case 'B':
            show_base_elements(true);
            break;
        case 'I':
            inventory->show(true);
            break;
        case 'E':
            elements->show(true);
            break;
        case 'A':
            animals->show(true);
            break;
        case 'P':
            plants->show(true);
            break;
        case 'N':
            npcs->show(true);
            break;
    }
}
void add_new_element()
{
    Element * el = new Element(base_elements[rand() % BASE_ELEMENTS]);
    elements->add(el);
    printf("new Element %s found\n", el->get_name());
}

void add_new_animal()
{
    Animal * el = new Animal;
    animals->add(el);
    printf("new Animal %s found\n", el->get_name());
}

void add_new_npc()
{
    Npc * el = new Npc;
    npcs->add(el);
    printf("new NPC %s found\n", el->get_name());
}

void find_new()
{
    printf("%se - Element\n", colorCyan);
    printf("p - Plant\n");
    printf("a - Animal\n");
    printf("n - Npc\n");
    printf("%s%s", colorNormal, colorGreenBold);

    char c = wait_key('f');
    switch (c)
    {
        case 'e':
            add_new_element();
            break;
        case 'p':
            add_new_plant();
            break;
        case 'a':
            add_new_animal();
            break;
        case 'n':
            add_new_npc();
            break;
    }
}

void change_clock()
{
    printf("%sh - add 1 hour\n", colorCyan);
    printf("d - add 1 day\n");
    printf("m - add 30 days\n");
    printf("q - add 90 days\n");
    printf("y - add 1 year\n");
    printf("%s%s", colorNormal, colorGreenBold);

    char c = wait_key('@');
    switch (c)
    {
        case 'h':
            game_time->update_time(3600);
            break;
        case 'd':
            game_time->update_time(24 * 3600);
            break;
        case 'm':
            game_time->update_time(30 * 24 * 3600);
            break;
        case 'q':
            game_time->update_time(90 * 24 * 3600);
            break;
        case 'y':
            game_time->update_time(360 * 24 * 3600);
            break;
    }
}

void test()
{
    printf("%sa - test axe\n", colorCyan);

    printf("%s%s", colorNormal, colorGreenBold);

    char c = wait_key('t');
    switch (c)
    {
        case 'a':
        {
            Axe * axe = test_axe();
            if (axe)
                inventory->add(axe);
        }
        break;
    }
}

void craft()
{
    printf("%sa - craft axe\n", colorCyan);
    printf("b - craft axe blade\n");
    printf("h - craft axe handle\n");

    printf("%s%s", colorNormal, colorGreenBold);

    char c = wait_key('c');
    switch (c)
    {
        case 'a':
        {
            InventoryElement *el1 = nullptr, *el2 = nullptr;
            if (!select_inventory2(&el1, &el2))
                return;
            Axe * axe = new Axe(el1, el2);
            if (!axe->craft())
                return;
            inventory->add(axe);
            inventory->remove(el1);
            inventory->remove(el2);
            printf("axe added to inventory\n");
        }
        break;
        case 'b':
        {
            InventoryElement * el = select_element(elements);
            if (!el)
                return;
            AxeBlade * axe_blade = new AxeBlade(el);
            if (!axe_blade->craft())
                return;
            inventory->add(axe_blade);
            elements->remove(el);
            printf("axe blade added to inventory\n");
        }
        break;
        case 'h':
        {
            InventoryElement * el = select_element(elements);
            if (!el)
                return;
            AxeHandle * axe_handle = new AxeHandle(el);
            if (!axe_handle->craft())
                return;
            inventory->add(axe_handle);
            elements->remove(el);
            printf("axe handle added to inventory\n");
        }
        break;
    }
}

void pickup()
{
    InventoryElement * el = select_element(elements);
    if (!el)
        return;
    inventory->add(el);
    elements->remove(el);
    printf("%s added to inventory\n", el->get_name());
}

void drop()
{
    InventoryElement * el = select_element(inventory);
    if (!el)
        return;
    printf("%s dropped from inventory\n", el->get_name());
    elements->add(el);
    inventory->remove(el);
}

void play()
{
    help();
    while (1)
    {
        char c = wait_key('>');

        switch (c)
        {
            case 't':
                test();
                break;
            case 'c':
                craft();
                break;
            case 's':
                show();
                break;
            case 'e':
                return;
            case 27:
                printf("%s", clrscr);
                break;
            case '?':
                help();
                break;
            case '@':
                change_clock();
                break;
            case 'h':
                harvest_plant();
                break;
            case 'o':
                sow_plant();
                break;
            case 'f':
                find_new();
                break;
            case '#':
                talk();
                break;
            case 'p':
                pickup();
                break;
            case 'd':
                drop();
                break;
        }
        game_time->update_time(1);
        plants->tick();
        animals->tick();
        npcs->tick();
    }
}

int main()
{
    struct termios old_stdin, stdin_tty;

    setbuf(stdout, nullptr);
    printf("%s", clrscr);
    tcgetattr(0, &old_stdin);
    stdin_tty = old_stdin;
    stdin_tty.c_lflag &= ~(ECHO | ICANON | ECHOE | ISIG);
    stdin_tty.c_cc[VMIN] = 1;
    stdin_tty.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &stdin_tty);

    tcflush(0, TCIFLUSH);
    ioctl(0, TCXONC, 1);

    srandom(time(nullptr));
    init_elements();
    game_time = new Game_time;

    inventory = new InvList("inventory");
    elements = new InvList("elements");
    plants = new InvList("plants");
    animals = new InvList("animals");
    npcs = new InvList("npcs");

    for (int i = 0; i < 5; i++)
        add_new_element();

    for (int i = 0; i < 2; i++)
    {
        add_new_plant();
    }

    for (int i = 0; i < 2; i++)
    {
        add_new_animal();
    }

    for (int i = 0; i < 5; i++)
    {
        add_new_npc();
    }

    init_sentences();

    callback_daily = daily_call;
    play();

    tcsetattr(0, TCSANOW, &old_stdin);
    return 0;
}

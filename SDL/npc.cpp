#include "npc.h"
#include "../core/player.h"
#include "menu.h"
extern Player * player;
extern int active_hotbar;

int npc()
{
    if (menu_dialog)
    {
        delete menu_dialog;
    }
    int menu_entries = 3;
    if (player->hotbar[active_hotbar])
        menu_entries++;

    menu_dialog = new Menu("Talk to NPC", menu_entries);
    menu_dialog->add("Hello", MENU_NPC_SAY, NPC_Say_hello);
    menu_dialog->add("How do you do?", MENU_NPC_SAY, NPC_Say_how_do_you_do);

    if (player->hotbar[active_hotbar])
    {
        menu_dialog->add("Do you know", MENU_NPC_SAY, NPC_Ask_do_you_know, player->hotbar[active_hotbar]);
    }

    menu_dialog->add("Cancel", MENU_CANCEL);
    current_menu = menu_dialog;
    return 0;
}

void npc_say(enum Npc_say s)
{
    switch (s)
    {
        case NPC_Say_hello:
            print_status("NPC says: Hello");
            break;
        case NPC_Say_how_do_you_do:
            if (rand() % 3)
                print_status("NPC says: I'm fine");
            else
                print_status("NPC says: I'm not fine, could be better");
            break;
        case NPC_Ask_do_you_know:
            InventoryElement * el = menu_dialog->get_el();
            print_status("NPC says: I don't know %s", el->get_name());
            break;
    }
}

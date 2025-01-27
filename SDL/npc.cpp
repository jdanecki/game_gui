#include "npc.h"
#include "menu.h"

int npc()
{
    if (menu_dialog)
    {
        free(menu_dialog->texture);
        delete menu_dialog;
    }
    menu_dialog = new Menu("Talk to NPC", 2);
    menu_dialog->add("Hello", MENU_NPC_SAY, Say_hello);
    menu_dialog->add("How do you do?", MENU_NPC_SAY, Say_how_do_you_do);
   // menu_dialog->add("Do you know?", MENU_NPC_SAY, Say_how_do_you_do);
    current_menu= menu_dialog;
    return 0;
}

void npc_say(enum Npc_say s)
{
    switch (s)
    {
        case Say_hello:
            print_status("NPC says: Hello");
            break;
        case Say_how_do_you_do:
            if (rand() % 3) print_status("NPC says: I'm fine");
            else print_status("NPC says: I'm not fine, could be better");
            break;
    }
}

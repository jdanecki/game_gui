#include "npc_talk.h"

InvList * sentences;
extern InvList * inventory;
extern InvList * npcs;

void talk()
{
    Npc * el = dynamic_cast<Npc *>(select_element(npcs));
    if (el)
    {
        printf("talking to %s\n", el->get_name());
        while (true)
        {
            Sentence * s = dynamic_cast<Sentence *>(select_list_element(sentences));
            if (!s)
                break;
            if (!npc_say(s))
                break;
        }
    }
}

bool npc_say(Sentence * s)
{
    printf("%s\n", s->text);
    switch (s->id)
    {
        case NPC_Say_hello:
            printf("NPC says: Hello\n");
            break;
        case NPC_Say_how_do_you_do:
            if (rand() % 3)
                printf("NPC says: I'm fine\n");
            else
                printf("NPC says: I'm not fine, could be better\n");
            break;
        case NPC_Ask_do_you_know:
        {
            InventoryElement * el = select_element(inventory);
            if (el)
            {
                if (rand() % 3)
                {
                    printf("NPC says: I know %s. It's %s\n", el->get_name(), el->get_class_name());
                }
                else
                    printf("NPC says: I don't know %s\n", el->get_name());
            }
            else
                printf("NPC says: I don't know what you asking for\n");
            break;
        }
        case NPC_Say_bye:
            printf("NPC says: Bye\n");
            return false;
    }
    return true;
}

void init_sentences()
{
    sentences = new InvList("sentences");
    sentences->add(new Sentence("Hello", NPC_Say_hello));
    sentences->add(new Sentence("How do you do?", NPC_Say_how_do_you_do));
    sentences->add(new Sentence("Do you know", NPC_Ask_do_you_know));
    sentences->add(new Sentence("Bye", NPC_Say_bye));
}

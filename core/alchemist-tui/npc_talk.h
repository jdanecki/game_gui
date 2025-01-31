#ifndef NPC_H
#define NPC_H

#include "show_list.h"

enum Npc_say
{
    NPC_Say_hello,
    NPC_Say_how_do_you_do,
    NPC_Say_bye,
    NPC_Ask_do_you_know,
};

class Sentence : public ListElement
{
  public:
    const char * text;
    enum Npc_say id;
    Sentence(const char * t, enum Npc_say i)
    {
        text = t;
        id = i;
    }
    void show(bool details = true)
    {
        printf("%s\n", text);
    }
};

extern InvList * sentences;

extern bool npc_say(Sentence * s);
extern void talk();
extern void init_sentences();

#endif // NPC_H

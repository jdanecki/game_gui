#ifndef NPC_H
#define NPC_H

extern int npc();

enum Npc_say
{
    NPC_Say_hello,
    NPC_Say_how_do_you_do,
    NPC_Ask_do_you_know,

};

extern void npc_say(enum Npc_say s);

#endif // NPC_H

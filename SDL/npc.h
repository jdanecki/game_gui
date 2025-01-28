#ifndef NPC_H
#define NPC_H

extern int npc();

enum Npc_say
{
    Say_hello,
    Say_how_do_you_do,
    Ask_do_you_know,

};

extern void npc_say(enum Npc_say s);

#endif // NPC_H

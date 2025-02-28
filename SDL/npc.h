#ifndef SDL_NPC_H
#define SDL_NPC_H
#include "../core/alchemist/npc_talk.h"
#include "../core/npc.h"

extern int npc();
extern int npc_say(Sentence * s);
extern Npc * current_npc;
#endif // NPC_H

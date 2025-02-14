#ifndef NPC_TALK_H
#define NPC_TALK_H

#include "el_list.h"

enum Npc_say
{
    NPC_Say_nothing,
    NPC_Say_hello,
    NPC_Say_how_are_you,
    NPC_Say_bye,
    NPC_Say_I_dont_know_you,
    NPC_Say_Im_not_fine,
    NPC_Say_I_know,
    NPC_Say_I_dont_know,

    NPC_Ask_do_you_know_item,
    NPC_Ask_do_we_know_each_other,
    NPC_Ask_do_you_really_care,
};

class Sentence : public ListElement
{
  public:
    const char * question;
    enum Npc_say id;
    Sentence * answers[3];
    Sentence(const char * t, enum Npc_say i)
    {
        question = t;
        id = i;
        answers[0] = nullptr;
        answers[1] = nullptr;
        answers[2] = nullptr;
    }
    void show(bool details = true)
    {
        printf("%s\n", question);
    }
    bool check(void * what)
    {
        enum Npc_say * i = (enum Npc_say *)what;
        return id == *i;
    }
    void add(Sentence * a1, Sentence * a2, Sentence * a3)
    {
        answers[0] = a1;
        answers[1] = a2;
        answers[2] = a3;
    }
    Sentence * get_answer()
    {
        int a = rand() % 3;
        return answers[a];
    }
};

class SentencesList : public ElementsList
{
  public:
    SentencesList(const char * n) : ElementsList(n)
    {
    }
    Sentence * add(ListElement * el)
    {
        return (Sentence *)ElementsList::add(el);
    }
    void disable(enum Npc_say id);
    void enable(enum Npc_say id);
};

extern SentencesList * sentences;

extern void init_sentences();

#endif // NPC_TALK_H

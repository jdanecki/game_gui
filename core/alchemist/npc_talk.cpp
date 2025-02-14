#include "npc_talk.h"

SentencesList * sentences;

void init_sentences()
{
    sentences = new SentencesList("sentences");
    Sentence * hello = sentences->add(new Sentence("Hello", NPC_Say_hello));
    hello->add(new Sentence("Hello", NPC_Say_nothing), new Sentence("Do we know each other?", NPC_Ask_do_we_know_each_other), new Sentence("I don't know you!", NPC_Say_I_dont_know_you));

    Sentence * how = sentences->add(new Sentence("How are you?", NPC_Say_how_are_you));
    how->add(new Sentence("I'm fine", NPC_Say_nothing), new Sentence("I'm not fine, could be better", NPC_Say_Im_not_fine), new Sentence("Do you really care?", NPC_Ask_do_you_really_care));

    Sentence * know = sentences->add(new Sentence("Do you know item?", NPC_Ask_do_you_know_item));
    know->add(new Sentence("I know", NPC_Say_I_know), new Sentence("I don't know", NPC_Say_I_dont_know), new Sentence("I don't know what you asking for", NPC_Say_nothing));

    Sentence * bye = sentences->add(new Sentence("Bye", NPC_Say_bye));
    bye->add(new Sentence("Bye", NPC_Say_nothing), new Sentence("See you next time", NPC_Say_nothing), new Sentence("See you later", NPC_Say_nothing));
}

void SentencesList::disable(Npc_say id)
{
    Sentence * s = dynamic_cast<Sentence *>(find(&id));
    s->disable();
}

void SentencesList::enable(Npc_say id)
{
    Sentence * s = dynamic_cast<Sentence *>(find(&id));
    s->enable();
}

#include "jamultypes.h"

typedef struct monsnotes_t {
	byte monster;
	char* notes;
} monsnotes_t;

static monsnotes_t notes[] = {
	{1,		// Bouapha															| cutoff
	 "Hello, and welcome to SpaceManiac's monster commentary! Since Sleepless "
	 "Hollow wasn't originally intended to have an editor, Jamul ripped out all "
	 "the monster notes, so I'm writing some myself. Various monsters are from "
	 "Supreme so I won't be repeating anything said in their Supreme notes. "
	 "This is Bouapha, the hero of the Dr. Lunatic and Adventures of Bouapha "
	 "series! His hammer mechanics are different here than in Supreme, as noted."},
	{255,
	 "No notes on monster"}
};

char* MonsterNotes(byte type)
{
	int i;
	for (i=0; notes[i].monster != 255 && notes[i].monster != type; ++i);
	return notes[i].notes;
}

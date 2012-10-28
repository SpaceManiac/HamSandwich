#include "jamultypes.h"
#include "monster.h"

typedef struct monsnotes_t {
	byte monster;
	char* notes;
} monsnotes_t;

static monsnotes_t notes[] = {
	{MONS_BOUAPHA,															//--| cutoff
	 "Hello, and welcome to SpaceManiac's monster commentary! Since Sleepless "
	 "Hollow wasn't originally intended to have an editor, Jamul ripped out all "
	 "the monster notes, so I'm writing some myself. Various monsters are from "
	 "Supreme so I won't be repeating anything said in their Supreme notes. "
	 "This is Bouapha, the hero of the Dr. Lunatic and Adventures of Bouapha "
	 "series! His hammer mechanics are different here than in Supreme, as noted."},
	{MONS_BONEHEAD,
	 "Boneheads have a long history with Hamumu Software, having been featured "
	 "in about every 2.5d game released since Kid Mystic. Sleepless Hollow uses "
	 "the big-head graphics from Loonyland, as it does for a few other monsters."},
	{MONS_LAZYBONE,
	 "These guys are a pain to deal with in the very early game, and can serve "
	 "as effective deterrant for a brief time, especially in confined spaces, "
	 "until the player is strong enough to take them on. Lazybones originally "
	 "appeared in Supreme, but here use LL1 graphics."},
	{MONS_CRAZYBONE,
	 "Aside from graphics from Loonyland, these guys are straight from Supreme. "
	 "They have a % chance of reviving on death, but here's a protip: they "
	 "can't come back if you or another monster stands on their corpse. They'll "
	 "still come back via SH's normal respawn methods, though, so you can't "
	 "eliminate their danger quite as completely as you could in Supreme."},
	{MONS_MINECART,
	 "This \"monster\" wasn't originally included in Sleepless Hollow at all "
	 "and was one of the few I decided to bring the graphics in from Supreme "
	 "for. Its theme and mechanics fit in the Sleepless Hollow mood and it was "
	 "the only vechicle out of three not in the game, so I thought it made "
	 "sense and was worth the space to bring it in."},
	{MONS_TURRETD,
	 "Actually called Turret in the code, the laser is one of Sleepless "
	 "Hollow's defining foes. Impenetrable except to its own beam, getting past "
	 "a laser invariably requires clever use if mirrors and/or the Lightreaver. "
	 "This specific one points down."},
	{MONS_TURRETL,
	 "This is a left laser. Lasers are actually one of my favorite obstacles "
	 "since they're new to Sleepless Hollow, having not previously appeared in "
	 "any Hamumu game."},
	{MONS_TURRETR,
	 "Lasers are unique but their graphics aren't - they're actually composed "
	 "several AK-8087 bullets with adjusted glowiness, since Sleepless Hollow "
	 "doesn't make any changes to the bullet or item spritesheets."},
	{MONS_BOBBER,
	 "This \"monster\" is Bouapha's fishing bobber. I haven't tried summoning "
	 "it or anything yet, so get back to me on that."},
	{MONS_BIGBODY,
	 "This is the underlying body of Big Heads Rod, Maude, and Todd and is "
	 "responsible for handling the movement and facing of the boss. The Big "
	 "Heads themselves are, like the Iron Skull, using Richie Lich graphics, "
	 "which first appeared in Spooky Castle."},
	{MONS_DARKNESS,
	 "These guys are highly unique to Sleepless Hollow in their mechanics. "
	 "Internally they're take on the size and graphics of Bouapha himself "
	 "but render only as a black inflection on the terrain, which required "
	 "a few changes to the lighting system to work right."},
	{MONS_STAREYBAT,
	 "Like the Supreme monsters, these guys push you back Blower-style. "
	 "Unlike Supreme, they push much stronger and are indestructible without "
	 "the right tools."},
	{MONS_BAT,
	 "Fairly standard bat fare, straight from Supreme. Did you know Scary Bats "
	 "were one of the first monsters in Dr. Lunatic, with ID 3 only after "
	 "Bouapha and Boneheads?"},
	{MONS_SCAREDYBAT,
	 "Since good old Buddy Bunnies didn't fit the Sleepless Theme, but Jamul "
	 "loves his allied creature escort missions, these guys were invented. A "
	 "bit atypically, they run away instead of towards you, useful for puzzles "
	 "where you want to require careful positioning."},
	{255,
	 "No notes on monster"}
};

char* MonsterNotes(byte type)
{
	int i;
	for (i=0; notes[i].monster != 255 && notes[i].monster != type; ++i);
	return notes[i].notes;
}

#include "jamultypes.h"
#include "monster.h"

typedef struct monsnotes_t {
	byte monster;
	const char* notes;
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
	{MONS_YERFDOG,
	 "Yerf yerf! This guy was brought over from Supreme as part of the update "
	 "since he's really useful for conveying information to the player."},
	{MONS_YUGO,
	 "Vroom vroom! This was included in Sleepless Hollow for the scene at the "
	 "beginning of the game, but you don't usually get the opportunity to drive "
	 "it anywhere."},
	{MONS_GOODBONE,
	 "A fair amount of code was changed so that checks like pushing and picking "
	 "up items worked for monsters with the Funnybone AI type as well as the "
	 "typical Bouapha AI type. This guy's pretty fancy in my opinion - "
	 "basically a better version of puzzles I've seen in Supreme that involve "
	 "multiple Bouaphas."},
	{MONS_RAFT,
	 "The classic raft! Used nowhere that I remember except Brayka Lake, which "
	 "is a pun about breaking legs in case that wasn't completely obvious."},
	{MONS_GOODTURRET,
	 "A good turret! It's, uh, good. Turret graphics were included for lasers "
	 "so this guy was an obvious choice to make accessible."},
	{MONS_MISLTURRET,
	 "Missle turret! It shoots missiles. This required some special treatment "
	 "to include since it usually shares AI routines with the Turret and Death "
	 "Turret (the routine checks its AI type to determine which kind of turret "
	 "it is). So strictly speaking it's in monster waves 5."},
	{MONS_DEATHTURRET,
	 "Like the missile turret, but worst. In case you were wondering, the "
	 "monster waves are: Dr. Lunatic; Expando-Pak; Fun Pack; Supreme; and "
	 "Sleepless Hollow. The lengthiest of these is Supreme, followed closely "
	 "by the original Dr. Lunatic."},
	{MONS_POLTERGUY,
	 "A throwback to Polterguy from Loonyland, Sir Spookley Hauntmore (oft "
	 "misnamed by way of extra Ss) is the Underhollow boss that requires the "
	 "Planetsmasher. He moves between worlds when he's damaged too much and "
	 "you have to switch sides to keep fighting him."},
	{MONS_POLTERGUY2,
	 "This is the invincible form of Sir Spookley Hauntmore, for when he's "
	 "shifted to the opposite world."},
	{MONS_BATGUARD,
	 "These guys are the bodyguards to Von Spatula, and are positioned in a "
	 "loop around the Countess by a special routine. Placing them manually is "
	 "a bad idea."},
	{MONS_RADISH,
	 "If it isn't the Horseradish himself! This guy's not that tough to beat "
	 "if you can survive long enough to lead him to the head, since by then "
	 "you've already proven yourself by beating all six of the Underhollow "
	 "bosses."},
	{MONS_PUMPKINFLY2,
	 "The Horseradish releases these guys to pester you. They do burn down "
	 "the trees in the way of where the head is kept, so at least that's "
	 "helpful."},
	{MONS_SKULL,
	 "The iron skull is pretty darn iron, with a whopping 20,000 hit points. "
	 "Reflect his megabeams (the staple attack of liches and giant spiders "
	 "throughout the Dr. Lunatic universe) with the Lightreaver and he falls "
	 "fast."},
	{MONS_COUNTESS,
	 "Von Spatula! Replacing the ordinary countess's position, she's protected "
	 "by an entourage of Royal Batguards that absorb hits. Use the earsplitter "
	 "to pierce her defenses."},
	{MONS_SPEEDY,
	 "The Sparkthrower boss! Only Sparkthrower bullets  are quick enough to "
	 "catch the Speedy."},
	{MONS_BLASTER,
	 "One of the three baddies using the Lightswitch graphics from Loonyland, "
	 "this one just shoots at you. It doesn't have much health but like many "
	 "other monsters is prone to respawning."},
	{MONS_LIGHTSLIDE,
	 "The second of the Lightswitch enemies, the Charge Coil slides around at "
	 "Bouapha's behest and is activated by a spark passing over it. Very useful "
	 "for puzzles involving sparks."},
	{MONS_LIGHTSWITCH,
	 "The final of the Lightswitch enemies, the Lightswitch itself is rendered "
	 "awake by any good bullet hitting it. If it's around a corner, "
	 "Sparkthrower trickshots are likely to be required."},
	{MONS_SPARKY,
	 "A less-speedy, more candle-bearing version of Speedy Gonzales. Based on "
	 "the wolves from Supreme and \"Sparky\" being a fairly common dog's name. "
	 "Three of them are featured in the northwest quadrant of the Tomb of "
	 "sparks, each holding a joyously bright candle for the player to murder "
	 "for. They don't respawn for this reason."},
	{255,
	 "No notes on monster."}
};

const char* MonsterNotes(byte type)
{
	int i;
	for (i=0; notes[i].monster != 255 && notes[i].monster != type; ++i);
	return notes[i].notes;
}

int MonsterNotesPercent()
{
	int total = 0;
	for (int i=0; i<NUM_MONSTERS; ++i)
		if (!(GetMonsterType(i)->theme & MT_NOGFX))
			++total;
	int done = 0;
	for (done=0; notes[done].monster != 255; ++done);
	return done * 100 / total;
}

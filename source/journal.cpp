#include "journal.h"
#include "monster.h"
#include "fishing.h"
#include "player.h"

char journal[NUM_PAGES][600]={
	// 0 - blank
	"Welcome to Sleepless Hollow! ^ ^ Use the arrow keys to move, and press Z or CTRL to throw hammers, should "
	"you find some.  ^ If you happen to get a mystical hammer, you can use its mystical powers with SHIFT or X. "
	"Of course, what are the odds you'd find something like that?  ^ You can also play with a gamepad or mouse. "
	"^ Press ESC to begin playing now, and press it any other time to pause the game. "
#ifdef DEMO
	"By the way, you should buy this game."
#endif
	,
	// 1 - intro
	"If you are reading this, then you are in danger!  The town is overrun with monsters, all because "
	"of the Headless Horseradish!  We've all fled, and I urge you to do the same.  If for some contrived "
	"reason you stay here, seek out the scattered pages of this journal.  They got rather conveniently "
	"lost as I fled.  I have been studying Sleepless Hollow for years, and my notes may just save your life. "
#ifdef DEMO
	"As could paying for this game."
#endif
	,
	// 2 - hollow
	"I'm afraid I couldn't discover much about the Headless Horseradish.  His legend stretches back for "
	"centuries, but all the details are quite sketchy.  I moved here originally just to research it, but I "
	"never imagined he might actually appear!  On an unrelated note, the longer you go without throwing a hammer, "
	"the more you can throw at once.  Collecting more hammers should let you charge up faster, just as collecting "
	"pants (logically) makes you throw them faster. "
#ifdef DEMO
	"And buying this game lets you get more of those items!"
#endif
	,
	// 3 - sleep inn
	"The legend of the Headless Horseradish has a myriad of complex facets, and it is difficult to separate "
	"myth from history.  I suppose I would've called it all myth before he appeared!  Nonetheless, the ledger "
	"in this inn seems to be where it all started.  As a joke, a teenager entered \"Headless Horseradish\", and "
	"the beast itself appeared!  Needless to say, that is when the screaming and running began. "
#ifdef DEMO
	"Just as you should scream and run to your wallet to get out the credit card and buy this!"
#endif
	,
	// 4 - yonder barn
	"The Headless Horseradish seemed to stir up the local wildlife with his arrival, not to mention the local "
	"dead.  We are being inundated with undead monstrosities and rampaging animals as we flee, and as I write this. "
	"Pardon my hasty handwriting!  I have just a handful of pages left... the animals accosted me and seem to have "
	"run off with the rest, resulting in an exciting challenge for you. "
#ifdef DEMO
	"Boy, one exciting challenge would sure be to buy this game.  Can you accomplish it?"
#endif
	,
	// 5 - graveyard
	"Sleepless Hollow's largest attraction (for the macabre crowd) has always been its vast graveyard.  The six "
	"tombs here have been the main focus of my investigation.  Each one houses a mystical hammer, rather than the "
	"more traditional corpse.  The hammers are sealed off behind a vast array of traps and puzzles.  It is said "
	"that when the Horseradish is unleashed, only the combined force of these six hammers can seal him away again. "
#ifdef DEMO
	"But you can't get all six unless you buy!"
#endif
	,
	// 6 - town hall
	"I moved to Sleepless Hollow a few years ago, and discovered the residents to be a strange lot.  They seem "
	"obsessed with puzzles.  If you are clever, you may find many valuable items hidden around the town and woods. "
	"A town alderman told me a clue to one such puzzle, located in the Town Hall.  He said simply \"The 31st state.\" "
	"I presume he meant the 31st state of the United States, but I never solved the puzzle myself.  Good luck! "
#ifdef DEMO
	"Better luck if you buy this game!"
#endif
	,
	// 7 - brayka lake
	"Barring the occasional sea serpent attack, Brayka Lake has always been a popular place for the locals to relax. "
	"But alas, like the other animals, the frogs here have now been infused with some sort of madness!  The serpents too "
	"I suppose, but really, they always act this way.  I found a golden figurine in one of the toad holes, engraved with "
	"a series of numbers.  I don't know what it means.  In any case, the numbers are 4-2-7-8-1-3.  Another of the villagers' "
	"puzzles?",
	// 8 - tomb of flames
	"The Flamebringer!  This mystical hammer emits a superheated bolt that sets monsters on fire, gradually "
	"burning them up.  It's said that the mystical hammers grow in power as you use them.  I wouldn't know, I "
	"never got close enough to touch it!  To employ the mystical power, press SHIFT or X.  It costs Energy, "
	"which for some unrealistic reason is collected from monsters.  It's the blue stuff. "
#ifdef DEMO
	"Spend some green stuff and buy this game now."
#endif
	,
	// 9 - tomb of mirrors
	"The Lightreaver!  This mystical hammer can emit a shield that reflects incoming energy and even has "
	"healing properties.  I imagine it would be quite handy against the lasers this tomb contains, if I could get some "
	"Energy to fuel it!  To use the mystical power, press SHIFT or X. "
	"When you get more than one mystical hammer, you can switch between them with the menu in this very journal. "
	"You know, I hope, that you can return to the journal anytime with the ESC key? "
#ifdef DEMO
	"You also know that you can buy this game for a small pittance?"
#endif
	,
	// 10 - tomb of darkness
	"The Planetsmasher!  This mystical hammer fires off a mini black hole that can suck monsters in and crush them! "
	"I've also discovered mysterious Gateways - glowing purple tiles that seem tied in some way to this "
	"hammer.  I'm not sure what they do, but if I could obtain the Planetsmasher and fire it while standing on "
	"one, I know it would do something!",
	// 11 - tomb of sparks
	"The Sparkthrower!  As the name implies, this hammer flings sparks that are electrostatically attracted to "
	"monsters.  I imagine that this homing capability would allow the sparks to hit targets you simply couldn't "
	"otherwise.  By the way, did you know you can quickly switch hammers with the number keys on your keyboard?",
	// 12 - tomb of noise
	"The Earsplitter!  This hammer creates a deafening focused roar that will devastate monsters even through walls. "
	"Earplugs are recommended when using it.  While tremendously annoying from a distance, it's harmless except at "
	"very short range.",
	// 13 - tomb of bones
	"Ah, the Bonecrusher.  It is said this hammer can call forth the walking dead, and control their every "
	"action!  That's surely a myth, and even if it were true, I'm sure the raised corpse would gradually decay over "
	"time, and that you'd only be able to control one at a time, and it would explode if you tried to summon another.",
	// 14 - sleepy woods
	"Formerly sleepy, these woods have become more of a nightmare since all the creatures have been awakened!  See what "
	"I did there?  Sleepy... nightmare?  It's like a metaphor, or a simile or something.  Oh, I'm glad I wrote that down! "
	"Hope I don't somehow lose this page.",
	// 15 - dumb side
	"Tales tell of a strange dimension outside of our own, known as The Dumb Side. "
	"The self-proclaimed mystics telling me about this say that if a person were to go there, "
	"their physical form would not quite mesh with that world.  Instead, any harm done to them would simply drain energy from them, "
	"eventually causing them to lose their connection to The Dumb Side, snapping them back to reality.  I suspect these mystics to "
	"be under the influence of narcotics.",
	// 16 - underhollow
	"The Underhollow is a dark secret that the townspeople kept from me.  It was only through my own investigations and luck that "
	"I stumbled across this vile place.  Monstrosities lurk within these depths.  I hear terrible moanings and... well, I think I'll "
	"go investigate something else now.  But I did find a strange door.  It says only the wise may enter.  I wonder if that has some "
	"connection to the 100 Zombie Brains that are rather oddly scattered around this land?",
	// 17 - cabin
	"Mr. Klein is the only friendly zombie I've ever seen.  His home is terribly strange.  It seems to exist "
	"in multiple parallel (and perpendicular) dimensions at once.  Finding my way around it has proven quite "
	"disorienting.  Perhaps the zombie mind is more capable than ours in some way?  Or perhaps Klein just never leaves his spot behind "
	"the counter, saving himself the trouble of getting lost.  This Mr. Klein has a truly odd fascination with candles.  Perhaps he'd trade his strange goods for some.",
	// 18 - horseradish hill
	"Tales say that the Headless Horseradish makes its lair atop a place called Horseradish Hill, which can only be reached via "
	"an extremely dangerous underground passage.  What does this Horseradish want?  Why is it besieging this poor town?  We can only "
	"hope someone bald who's good with hammers will come along and save us all, that the poor villagers may return. "
	"As for me, I'm done with this place!  See you in Tahiti! ^ ^ Ichabod Steamshovel ^ 10/28/07",
	// 19 - happy glade
	"Ah, just doing some doodling...",
	// 20 - fishing journal
	"The waters here are polluted with minerals.  This makes electrofishing easy with an Electroreel!  It attaches to "
	"your Red Hammer, allowing you to magnetically catch fish!",
};

void FishingPage(int offX,int offY)
{
	int i,t,r;
	char s[64];

	offY+=120;
	t=0;
	r=0;
	PrintUnGlow(offX+330,offY+70,"You have caught:",1);
	for(i=0;i<10;i++)
	{
		if(player.journal[30+i])	// have caught some
		{
			sprintf(s,"%s (%d Caught, %d lbs. max)",FishName((byte)i),player.journal[30+i],player.journal[40+i]);
			PrintUnGlow(offX+330,offY+90+i*20,s,1);
			t+=player.journal[30+i];
		}
		else
			PrintUnGlow(offX+330,offY+90+i*20,"-",1);
		if(player.journal[40+i]>r)
			r=player.journal[40+i];
	}
	if(player.journal[54])
		t++;

	sprintf(s,"Total caught: %d    Record: %d lbs.",t,r);
	PrintUnGlow(offX+330,offY+90+12*20,s,1);
	if(player.journal[54])	// have caught the unwanted goldfish
		PrintUnGlow(offX+330,offY+90+10*20,"...and an Unwanted Goldfish!",1);
}

void ShowJournalPage(int offX,int offY,byte p)
{
	char s[32];
	int x;

	if(offX>310)
		return;

	if(p>0)
	{
		sprintf(s,"%d",p);
		x=635-GetStrLength(s,2);
		PrintUnGlow(offX+x,offY+2,s,2);
	}
	PrintUnGlowRect(offX+330,offY+30,offX+635,offY+480,24,journal[p],2);
	if(p==19)
		InstaRenderMonster(480,offY+320,MONS_STICKTREE,-32,GetDisplayMGL());
	if(p==20)
		FishingPage(offX,offY);
}

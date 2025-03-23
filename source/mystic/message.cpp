#include "message.h"
#include "sound.h"
#include "game.h"
#include "intface.h"
#include "options.h"

typedef struct SpeechDef
{
	char text[4][64];
	byte nextSpeech;
} SpeechDef;

std::vector<SpeechDef> speechDef = {
	{
		// 0
		"Welcome to Beginnerton!  This town is",
		"really a tutorial on some basic game",
		"features. There's a spell you might",
		"want in here.",
		0,
	},
	{
		// 1
		"Fireballs are the weapon of choice.",
		"Each Energy Orb you collect increases",
		"the number you can launch at once.",
		"Each Speed Gem raises the rate of fire.",
		13,
	},
	{
		// 2
		"Spellbooks teach you new spells.",
		"Casting them uses up your mana (the",
		"blue bar). As you gain levels, your",
		"spells become more powerful.",
		14,
	},
	{
		// 3
		"Psst, hey... thousands of years ago,",
		"a great hero used a weapon known as",
		"the Armageddon Sword to defeat a",
		"monstrous evil.  To keep this sword",
		4,
	},
	{
		// 4
		"from falling into evil hands, it was",
		"broken into four pieces and hidden",
		"throughout the land.  Four pieces,",
		"four chapters... think about it.",
		0,
	},
	{
		// 5
		"The passage to Spooky Castle is",
		"blocked right now.  I have a feeling",
		"that it'll be cleared by the time you",
		"recover the orbs from X more places.",
		0,
	},
	{
		// 6
		"The passage to Spooky Castle is",
		"wide open now.  I can't think of a",
		"logical reason why, but it is.",
		"Good luck, Kid Mystic!!",
		0,
	},
	{
		// 7
		"My village was attacked by Mushies!",
		"As the village librarian, it's my",
		"job to remain here in case any heroes",
		"happen along that want the key to",
		8,
	},
	{
		// 8
		"the village library.  You look",
		"pretty heroic.  Here!",
		" ",
		" ",
		0,
	},
	{
		// 9
		"I hope you found something of",
		"interest in our library.  I guess",
		"I'll stay here and chronicle the",
		"Mushie occupation of our village.",
		0,
	},
	{
		// 10
		"Welcome to the Spooky Dance-a-thon!",
		"This nifty book is the prize for",
		"whoever can stay dancing and/or",
		"alive the longest!  Good luck!",
		0,
	},
	{
		// 11
		"You've won the Dance-a-thon!!",
		"Way to go!  The book's all yours.",
		" ",
		" ",
		0,
	},
	{
		// 12
		"To the left is knowledge, and to",
		"the right is wealth.  Choose wisely,",
		"you can't have both!",
		" ",
		0,
	},
	{
		// 13
		"You can launch fireballs by pressing",
		"the Fire button. Shocking, I know.",
		"Fireball upgrades only last for the",
		"level you are in, unlike Spells.",
		16,
	},
	{
		// 14
		"To cast a spell, press the Cast button.",
		"You can use the specific Spell buttons",
		"to switch to other spells, or use",
		"the Next and Previous buttons.",
		15,
	},
	{
		// 15
		"In the options menu, you can change",
		"between just selecting spells with those",
		"buttons, or instantly casting them. Play",
		"your own way!",
		0,
	},
	{
		// 16
		"One other thing... you can't leave",
		"any area that you enter until you",
		"collect all the mystical green orbs",
		"in it.  After all, that's what you're",
		17,
	},
	{
		// 17
		"here to do, right?  The Orbometer in",
		"the lower right of the screen will",
		"give you an idea of how many remain.",
		" ",
		0,
	},
	{
		// 18
		"You've just caught a fairy! Fairies",
		"assist you with magical powers, but",
		"some of them also have drawbacks.  You",
		"can choose which fairy to equip by",
		19,
	},
	{
		// 19
		"pressing the Spell key when you are in",
		"the overworld.  Pick the one you want",
		"to use and press Fire.  Pressing Spell",
		"again lets you go fairyless.",
		0,
	},
	{
		// 20
		"Welcome to Kid Mystic!  Remember the",
		"handy controls below. You can also",
		"change them, and many other things,",
		"in the Options menu on the title screen.",
		0,
	},
	{
		// 21
		"",
		"",
		"",
		"",
		0,
	},
	{
		// 22
		"Welcome to Madcap Mode!  You can",
		"now go through the entire adventure",
		"again, keeping all the powerups and",
		"levels you already earned... but",
		23,
	},
	{
		// 23
		"that won't be nearly enough!  Don't",
		"forget to see what's new in the Shop,",
		"because you will need it badly.",
		"Good luck!",
		0,
	},
	{
		// 24
		"Congratulations!  You found all the",
		"secret items in the game, so you've",
		"unlocked Challenge Mode!  Try it out",
		"on the main menu.",
		0,
	},
	{
		// 25
		"Welcome back to Madcap Mode!  You",
		"can keep exploring this mode all",
		"you want, or start a new game, or",
		"whatever!  Sorry, no more prizes!",
		0,
	},
	{
		// 26
		"You're playing in Modern Mode! That",
		"means you get Skill Points when you",
		"level up. Press ESC or START to spend",
		"your skill points.",
		27,
	},
	{
		// 27
		"You can reset your skills anytime as",
		"long as you are in the Overworld!",
		"You don't get stronger with levels",
		"anymore, so don't forget your skills!",
		0,
	},
	{
		// 28
		"I'm Farley the Ghost Bat! I can fly",
		"you to previous chapters. Don't worry,",
		"I can also fly you back.",
		"",
		0,
	},
	{
		// 29
		"This book describes the Runic Tongue.",
		"It looks like every rune has a letter",
		"that it's equivalent to. There's even",
		"a handy chart!",
		0,
	},
	{
		// 30
		"Hmm, according to this book, ancient",
		"numerologists assigned numbers to",
		"each rune in addition to their",
		"alphabetical value.",
		0,
	},
	{
		// 31
		"Ugh, fine you caught me. I had hoped",
		"I could just leave the lights off, but",
		"you stumbled right into me. Yes, it is",
		"I, Professor Dusseldorf, and I am",
		32,
	},
	{
		// 32
		"in fact reading romance novels. It's my",
		"guilty pleasure. Look, if you don't spill",
		"my secret, I will tell you another secret",
		"about this library. Deal?",
		33,
	},
	{
		// 33
		"I'll assume that blank stare means yes.",
		"Okay. In the Fiction section, there are",
		"a series of shelves you can move. If you",
		"push them all up, in order from left to",
		34,
	},
	{
		// 34
		"right, you will find a little surprise!",
		"If you screw it up, just push them all",
		"back down. Now let me get back to Lady",
		"Cordelia and the Strapping Lad.",
		0,
	},
	{
		// 35
		"Go to the Fiction section, I'm trying",
		"to read here!",
		"Ugh, just push up the shelves from",
		"left to right.",
		0,
	},
	{
		// 36
		"Oh dearie me! I'm just a helpless old",
		"lady! Would you assist me in making my",
		"famous Swamp Stew, young man? I need",
		"you to throw these ingredients in",
		37,
	},
	{
		// 37
		"the pot in exactly this order. Please",
		"don't make a mistake! Swamp Stew can",
		"be a trifle... temperamental.",
		"",
		0,
	},
	{
		// 38
		"Did you forget the recipe? Sometimes I",
		"mix it up too! Almost seems like it's",
		"always different. Here it is again: ",
		"",
		0,
	},
	{
		// 39
		"Delightful, that's all the ingredients!",
		"Now to cook it. Step on the button",
		"there to light the fire. The longer you",
		"stay on the button, the hotter it gets.",
		40,
	},
	{
		// 40
		"Once it's lit, there's enough wood for",
		"10 seconds of cooking. You need to boil",
		"it for 3 seconds and then keep it at a",
		"simmer until it's done.",
		0,
	},
	{
		// 41
		"Oh no, the Octon Juice isn't even",	// tentacle attack
		"cooked! This is definitely going to be",
		"a problem.",
		"",
		0,
	},
	{
		// 42
		"The stew is overheated! This can't be",	// magmazoids
		"good!",
		"",
		"",
		0,
	},
	{
		// 43
		"Wait, what order did you put the",	// just explodes
		"ingredients in? This is a disaster!",
		"",
		"",
		0,
	},
	{
		// 44
		"Ack, these are the wrong ingredients!",	// mushie attack
		"This is mushroom feed, not Swamp Stew!",
		"",
		"",
		0,
	},
	{
		// 45
		"Delicious, this is perfect Swamp Stew!",
		"Okay, full disclosure. I'm actually a",
		"witch. I know, I know, I don't look a",
		"day over hag. Anyway, since you were",
		0,
	},
	{
		// 46
		"so helpful, I'm gonna hex you up a nice",
		"treat - a Rune Pouch! That will let you",
		"equip one more rune at a time. Thanks",
		"again, young lad.",
		0,
	},
	{
		// 47
		"Well, you aren't exactly a chef, but",
		"I do appreciate you trying to help.",
		"Full disclosure: I'm actually a witch.",
		"Crazy, right? So since you have been",	// wraps back up to 46
		0,
	},
	{
		// 48
		"Get outta here, you ruined my swamp",
		"hovel! No stew for you! Or me, come",
		"to think of it!",
		"",
		0,
	},
	{
		// 49
		"I hope to see you again, Kid Mystic.",
		"You're a good egg.",
		"",
		"",
		0,
	},
	{
		// 50
		"so helpful, I'll give you a big old",
		"smooch. Come here cutie pie!",
		"",
		"Just kidding. Consent and all that.",
		0,
	},
	{
		// 51
		"Spellbooks teach you new spells.",
		"Casting them uses up your mana (the",
		"blue bar). As you gain levels, you get",
		"skill points you can spend to improve",
		52,
	},
	{
		// 52
		"your spells as well as yourself. I",
		"know this spell seems pretty sad",
		"right now, but check it out at level",
		"five! Pause the game to adjust skills.",
		14,
	},
	{
		// 53
		"The Monster Wilderness is full of tricks",
		"and traps. Sometimes there are walls",
		"you can push, or buttons to step on,",
		"or walls you can shoot.",
		54,
	},
	{
		// 54
		"I've created a very very subtle obstacle",
		"course for you here. Good luck figuring",
		"out which walls are the trick ones!",
		"I'm sneaky!",
		0,
	},
	{
		// 55
		"Oh, I see you're admiring my collection!",
		"Yes, I have my own personal Fairy Bell",
		"and Reflect Crystal! I keep them locked",
		"up for safety.",
		56,
	},
	{
		// 56
		"In fact, they're so securely stored that",
		"I just continue facing the other",
		"direction. I feel no need to check that",
		"they're still there.",
		0,
	},
	{
		// 57
		"They're still there, right? I'll just keep",
		"facing this way and assume they are.",
		"You'd let me know if they were gone, for",
		"sure. Right? I can trust you.",
		0,
	},
	{
		// 58
		"Ooh, Kid Mystic! Just who I was hoping",
		"to see! I don't know if you've noticed, but",
		"these woods are drained of magic. There's",
		"ghosts a-roaming, and they feed on it.",
		59,
	},
	{
		// 59
		"Now, ghosts are invisible, as you know.",
		"But you might not know that if you",
		"try to cast a spell around one, it will",
		"make their ectoplasm glow. Use that to",
		60,
	},
	{
		// 60
		"track them. I've made a batch of Ghost",
		"Traps, so once you think you know where",
		"a ghost is headed, press Next and Prev",
		"at the same time to drop a trap.",
		61,
	},
	{
		// 61
		"It takes two seconds for a trap to arm,",
		"so think ahead! The trap will burn out",
		"two seconds later, so you better have",
		"good aim. And one last little thing...",
		62,
	},
	{
		// 62
		"Each trap is powered by an Energy Orb, so",
		"every time you put one out, you'll lose",
		"one Fireball! Don't waste any traps!",
		"Good luck, we need our magic back!",
		0,
	},
	{
		// 63
		"What are you waiting for, there's ghosts",
		"afoot! Who you gonna call, me? No,",
		"you do it! You want your magic back,",
		"don't you?",
		0,
	},
	{
		// 64
		"We did it! The ghosts are trapped. I",
		"definitely won't use them for any",
		"nefarious purposes. I'm just glad the",
		"magic is back. Come see me for a gift!",
		0,
	},
	{
		// 65
		"Thanks again, Kid. We make a great team.",
		"",
		"",
		"",
		0,
	},
	{
		// 66
		"Psst, Kid! Come over here! I've found some-",
		"thing. So you know how big evil monsters",
		"can split their soul into pieces and store",
		"it away so you can't truly destroy",
		67,
	},
	{
		// 67
		"them? Well I think I just found Bobby",
		"Khan's Horkbox collection! That's what",
		"the soul storage boxes are called. It's",
		"a perfectly cromulent word.",
		68,
	},
	{
		// 68
		"And if you don't destroy these horkboxes,",
		"Bobby Khan can just come back in one",
		"of them! So get in there. And I have no",
		"twisted agenda that relies on using the",
		69,
	},
	{
		// 69
		"empty horkboxes for myself, so don't",
		"worry about that. Just go smash them",
		"all!",
		"",
		0,
	},
	{
		// 70
		"Hey Kid! Good work in there. I'm not",
		"hanging around here to collect and use",
		"the horkboxes for myself, so feel free",
		"to ignore me and move on.",
		0,
	},
	{
		// 71
		"Okay, that's a Horkbox right in there.",
		"All you need to do is shoot it to",
		"destabilize it. But it will take a minute",
		"before it explodes...",
		72,
	},
	{
		// 72
		"and it will definitely be spewing",
		"monstrosities and wild magic the whole",
		"time, so be ready! Each Horkbox has a",
		"different kind of magic, and it's all bad.",
		0,
	},
	{
		// 73
		"Make sure you find all the Horkboxes!",
		"If even one is left, Bobby Khan could",
		"escape. They're powered by Mystic Orbs,",
		"so you're gonna want those anyway!",
		0,
	},
	{
		// 74
		"There's a poem of sorts scrawled on the",
		"wall here, presumably by a deranged",
		"person who does not know much about",
		"anatomy.",
		0,
	},
	{
		// 75
		"Hey Kid, it's me again, Madam Kromch.",
		"Now that all the horkboxes are gone,",
		"why don't you come meet me in the",
		"central chamber for your 'reward'?",
		0,
	},
	{
		// 76
		"Muahahaha... it's time, Kid Mystic.",
		"Time that I demonstrated my true power",
		"to you! With all the handy help you've",
		"given me, my power has only grown!",
		77,
	},
	{
		// 77
		"And by that, I mean the power of",
		"FRIENDSHIP! Thank you so much for all",
		"things you've done to help me out when",
		"you had problems of your own.",
		78,
	},
	{
		// 78
		"Sorry, I don't know why I accidentally",
		"put reward in quote marks. I have a",
		"diagnosed speech impediment where I",
		"always sound like a villain.",
		79,
	},
	{
		// 79
		"I do have an actual reward for you.",
		"It's my last Rune Pouch, and I want you",
		"to have it. You've been a great friend",
		"and we'll have to have lunch sometime!",
		0,
	},
};

byte speechX,speechY,curSpeech;

message_t bigMessage;
message_t message;
byte romanceTalk = 0;
byte speechStartedAt = 0;

void InitSpeechSystem(void)
{
	romanceTalk = 0;
}

void InitMessage(void)
{
	message.msg[0]='\0';
}

void NewBigMessage(const char *txt,int time)
{
	strncpy(bigMessage.msg,txt,32);
	bigMessage.x=HALFWID-GetStrLength(bigMessage.msg,0)/2;
	bigMessage.y=-100;
	bigMessage.dy=0;
	bigMessage.timer=time;
	bigMessage.bright=-32;
	bigMessage.brightDir=2;
}

void NewMessage(const char *txt,int time)
{
	strncpy(message.msg,txt,32);
	message.x=HALFWID-GetStrLength(message.msg,2)/2;
	message.y=SCRHEI+4;
	message.dy=-8;
	message.timer=time;
	message.bright=-32;
	message.brightDir=2;
}

void NoRepeatNewMessage(const char *txt,int time)
{
	if(!strncmp(message.msg,txt,32))
		return;	// don't reset if showing the same message
	NewMessage(txt,time);
	MakeNormalSound(SND_MESSAGE);
}

void UpdateBigMessage(void)
{
	bigMessage.y+=bigMessage.dy;
	bigMessage.dy+=2;
	bigMessage.bright+=bigMessage.brightDir;

	if(bigMessage.timer)
		bigMessage.timer--;
	else
		bigMessage.brightDir=-2;

	// while time still remains, don't start falling offscreen
	if(bigMessage.timer)
	{
		if(bigMessage.y>200)
		{
			bigMessage.y=200;
			bigMessage.dy=-bigMessage.dy/2;
			if(bigMessage.dy>-2)
				bigMessage.dy=0;
		}
		if(bigMessage.bright>=32)
			bigMessage.brightDir=-1;
		if(bigMessage.brightDir<0 && bigMessage.bright<0)
		{
			bigMessage.brightDir=0;
			bigMessage.bright=0;
		}
	}
	else	// go ahead and fall
	{
		if(bigMessage.y>480)
		{
			bigMessage.msg[0]='\0';
			bigMessage.y=0;
			bigMessage.dy=0;
		}
	}
}

void UpdateMessage(void)
{
	UpdateBigMessage();

	message.y+=message.dy;
	message.dy+=1;
	message.bright+=message.brightDir;

	if(message.timer)
		message.timer--;
	else
		message.brightDir=-2;

	// while time still remains, don't start falling offscreen
	if(message.timer)
	{
		if(message.dy>0)
			message.dy=0;

		if(message.bright>=32)
			message.brightDir=-2;
		if(message.brightDir<0 && message.bright<0)
		{
			message.brightDir=0;
			message.bright=0;
		}
	}
	else	// go ahead and fall
	{
		if(message.y>480)
		{
			message.msg[0]='\0';
			message.y=0;
			message.dy=0;
		}
	}
}

void RenderMessage(void)
{
	char b;

	b=message.bright/2;
	PrintBright(message.x,message.y,message.msg,b,2);
	b=bigMessage.bright/2;
	Print(bigMessage.x,bigMessage.y,bigMessage.msg,b,0);
}

void InitSpeech(byte spc)
{
	if(spc==7 && player.keys[2]==1)
		spc=9;

	if(spc==5)
	{
		// this guy tells you how many levels remain to clear.
		if(player.levelsPassed>=9)
			spc=6;
		else
		{

			speechDef[5].text[3][22]= '9' - player.levelsPassed;

			if(player.levelsPassed==8)
			{
				speechDef[5].text[3][34]='.';
				speechDef[5].text[3][35]=' ';
			}
			else
			{
				speechDef[5].text[3][34]='s';
				speechDef[5].text[3][35]='.';
			}
		}
	}
	if(spc==10)
	{
		// check to see if the player has won the Dance-a-thon yet
		if(!MonsterExists(255))
			spc=11;
	}
	if (spc == 31)
	{
		if (romanceTalk)
			spc = 35;
	}
	speechStartedAt = spc;
	curSpeech=spc;
	speechX=0;
	speechY=0;
	EnterSpeechMode();
}

void FinishSpeech(void)
{
	if (speechStartedAt == 76)
	{
		CurrentMap()->GetTile(31, 102)->item = ITM_RUNEPOUCH;
		CurrentMap()->GetTile(31, 100)->item = ITM_BRAIN;
		GetFirstFriendly()->type = MONS_NONE;	// she runs off
	}
	if (speechStartedAt == 39)
		SwampEnableCooking();

	if (speechStartedAt == 41)
	{
		// uncooked octon means big tentacle attack
		SwampDestroyCauldron(0);
	}
	if (speechStartedAt == 42)	// overheated stew means magmazoids
	{
		SwampDestroyCauldron(1);
	}
	if (speechStartedAt == 43) // wrong order, stew explodes
	{
		SwampDestroyCauldron(2);
	}
	if (speechStartedAt == 44) // wrong amounts, mushies
	{
		SwampDestroyCauldron(3);
	}
	if (speechStartedAt == 46)
	{
		// get rune pouch
		CurrentMap()->GetTile(60, 62)->item = ITM_RUNEPOUCH;
		CurrentMap()->BrightTorch(60, 62, 24, 8);
		MakeNormalSound(SND_MEGABEAMHIT);
		FloaterParticles((60 * TILE_WIDTH + TILE_WIDTH / 2) * FIXAMT, (62 * TILE_HEIGHT + TILE_HEIGHT / 2) * FIXAMT, 6, 20, 2, 20);
	}
	if (speechStartedAt == 31 && romanceTalk == 0)
	{
		CurrentMap()->LightSpecial(GetGoodguy()->x / (TILE_WIDTH * FIXAMT), GetGoodguy()->y / (TILE_HEIGHT * FIXAMT), 0, 20);
		romanceTalk = 1;
	}
	if (speechStartedAt == 7)
	{
		MakeNormalSound(SND_GETKEY);
		player.keys[2] = 1;
	}
	if (speechStartedAt == 28)
		EnterFarleyMode();
}

byte UpdateSpeech(MGLDraw *mgl)
{
	if(ButtonTapped(CONTROL_B1|CONTROL_B2))
	{
		if(speechY<4)
			speechY=4;
		else
		{
			if (curSpeech == 20)
			{
				if (!ClassicMode())
				{
					curSpeech = 26;
					speechX = 0;
					speechY = 0;
					return 0;
				}
				else
				{
					FinishSpeech();
					return 1;
				}
			}
			if (curSpeech == 45 || curSpeech == 47) // both victory speeches go to the end victory speech
			{
				if (GotRunePouchInLevel(player.worldNum, player.levelNum))
					curSpeech = 50;
				else
					curSpeech = 46;
				speechX = 0;
				speechY = 0;
				return 0;
			}

			if (speechDef[curSpeech].nextSpeech != 0)
			{
				curSpeech = speechDef[curSpeech].nextSpeech;
				speechX = 0;
				speechY = 0;
				return 0;
			}
			else
			{
				FinishSpeech();
				return 1;
			}
		}
	}

	if(speechY<4)
	{
		if((speechX&1)==0)
			MakeNormalSound(SND_CHAT);
		speechX++;
		if(speechX>=strlen(speechDef[curSpeech].text[speechY]))
		{
			speechX=0;
			speechY++;
		}
	}

	if (ButtonTapped(CONTROL_ESCAPE))
	{
		SetInMenu(false);
		UpdateControls();
		FinishSpeech();
		return 1;	// ESC cancels the speech
	}

	return 0;
}

void RenderSpeech(void)
{
	char s[64];
	int i;

	GetIntfaceSpr(68)->Draw(180,86,GetDisplayMGL());

	for(i=0;(i<=speechY && i<4);i++)
	{
		strcpy(s,speechDef[curSpeech].text[i]);
		if(i==speechY)
			s[speechX+1]='\0';

		PrintGlow(HALFWID-64*8/2+5,i*20+95,s,2);
	}

	if (curSpeech == 29)	// show the rune chart!
	{
		RenderSkillBox(170-2, 200-2, SCRWID - 170+2, 460+2, 16, 0);
		RenderSkillBox(170, 200, SCRWID - 170, 460, 16, 27);
		int rx, ry;
		rx = 190;
		ry = 215;
		s[1] = '\0';
		byte c;
		for (int i = 0; i < 24; i++)
		{
			c = i;
			if (c >= 16)
				c++;	// we want to skip Q, so we have the whole alphabet except Q and Z
			BlitIcon(100 + i, rx-12, ry, 0, -31);
			s[0] = 'A' + c;
			int len = GetStrLength(s, 0);
			Print(rx + 35-len/2, ry, s, -31, 0);
			rx += 72;
			if (rx > SCRWID - 180 - 40)
			{
				rx = 190;
				ry += 40;
			}
		}
	}
	if (curSpeech == 30)	// show the rune number chart!
	{
		RenderSkillBox(170 - 2, 200 - 2, SCRWID - 170 + 2, 460 + 2, 16, 0);
		RenderSkillBox(170, 200, SCRWID - 170, 460, 16, 27);
		int rx, ry;
		rx = 190;
		ry = 215;
		for (int i = 0; i < 24; i++)
		{
			BlitIcon(100 + i, rx - 12, ry, 0, -31);
			sprintf(s, "%d", i+1);
			int len = GetStrLength(s, 0);
			Print(rx + 35 - len / 2, ry, s, -31, 0);
			rx += 72;
			if (rx > SCRWID - 180 - 40)
			{
				rx = 190;
				ry += 40;
			}
		}
	}
	if (curSpeech >= 37 && curSpeech <= 40)	// show the swamp recipe
	{
		RenderSwampRecipe();
	}
	if (curSpeech == 0 || curSpeech==13 || curSpeech==18 || curSpeech==19 ||
		curSpeech==20 || curSpeech==26 || curSpeech==27)	// show basic controls
	{
		RenderBasicControls();
	}
	if (curSpeech == 14 || curSpeech == 15 || curSpeech==21 || curSpeech==51 || curSpeech==52 || curSpeech==60)	// show spell buttons
	{
		RenderSpellControls();
	}
	if (curSpeech == 74)
	{
		RenderSkillBox(60 - 2, 200 - 2, SCRWID - 60 + 2, 200+140 + 2, 32*2+4, 32*2+14);
		RenderSkillBox(60, 200, SCRWID - 60, 200+140, 32*2+4, 32*2+14);
		CenterPrint(HALFWID, 200 + 10, "Legbone's connected to the rib bone,", -31, 2);
		CenterPrint(HALFWID, 200 + 30, "Rib bone's connected to the teeth bones", -31, 2);
		CenterPrint(HALFWID, 200 + 50, "Teeth bones are stuck to the round bone", -31, 2);
		CenterPrint(HALFWID, 200 + 70, "Round bone's connected to the skull bone", -31, 2);
		CenterPrint(HALFWID, 200 + 90, "Skull bone's connected to the foot bone", -31, 2);
		CenterPrint(HALFWID, 200 + 110, "Foot bone's connected to this wall here", -31, 2);
	}
}

byte farleyCursor;
byte farleyTicker;
void InitFarley(void)
{
	farleyCursor = player.worldNum;
}

byte UpdateFarley(MGLDraw *mgl)
{
	farleyTicker+=2;
	int top = (int)HighestWorldReached();
	if (AutoRepeatTapped(CONTROL_UP))
	{
		farleyCursor--;
		if (farleyCursor > 200)
			farleyCursor = top;
		MakeNormalSound(SND_BATEYES);
	}
	if (AutoRepeatTapped(CONTROL_DN))
	{
		farleyCursor++;
		if (farleyCursor > top)
			farleyCursor = 0;
		MakeNormalSound(SND_BATEYES);
	}
	if (AutoRepeatTapped(CONTROL_B1))
	{
		if (farleyCursor == player.worldNum)
			MakeNormalSound(SND_BATEYES);
		else
			MakeNormalSound(SND_GOTOMAP);
		SetInMenu(false);
		UpdateControls();
		return 1;	// we're gonna stay here
	}

	if (AutoRepeatTapped(CONTROL_B2|CONTROL_ESCAPE))
	{
		MakeNormalSound(SND_BATEYES);
		farleyCursor = player.worldNum;
		SetInMenu(false);
		UpdateControls();
		return 1;
	}
	return 0;
}

void OutlineFarleySprite(word spr, int x, int y, char bright)
{
	for(int i=x-1;i<=x+1;i++)
		for (int j = y - 1; j <= y + 1; j++)
		{
			if(i!=x || j!=y)
				GetItemSprite(spr)->DrawBright(i, j, GetDisplayMGL(), (bright==0)?-31:31);
		}
	GetItemSprite(spr)->DrawBright(x, y, GetDisplayMGL(), bright);
}

void RenderFarley(void)
{
	char s[32];
	int top = (int)HighestWorldReached();
	word w = 1;
	for (int i = 0; i <= top; i++)
	{
		GetIntfaceSpr(68)->DrawBright(180, 30+i*110, GetDisplayMGL(),-10+(farleyCursor==i)*10);
		sprintf(s, "Chapter %d", i + 1);
		CenterPrintGlow(30 + i * 110 + 20, s, 0);
		switch (i)
		{
			case 0:
				strcpy(s, "Over the River");
				break;
			case 1:
				strcpy(s, "Through The Woods");
				break;
			case 2:
				strcpy(s, "Spooky Castle");
				break;
			case 3:
				strcpy(s, "Beneath The Castle");
				break;
		}
		CenterPrintGlow(30 + i * 110 + 60, s, 2);
		if(i==player.worldNum)
			CenterPrintGlow(30 + i * 110 + 80, "You are here!", 1);
		if (farleyCursor == i)
		{
			BlitIconBit(558, 437, 558 + 12, 437 + 13, 50 + (Cosine(farleyTicker) * 4 >> FIXSHIFT), 30 + i * 110 - 10 + (Cosine(farleyTicker) * 4 >> FIXSHIFT), 255, 0);
			BlitIconBit(558, 467, 558 + 13, 467 + 13, 50 + (Cosine(farleyTicker) * 4 >> FIXSHIFT), 30 + i * 110 + 98 - (Cosine(farleyTicker) * 4 >> FIXSHIFT), 255, 0);

			BlitIconBit(588, 437, 588 + 12, 437 + 12, 50+520 - (Cosine(farleyTicker) * 4 >> FIXSHIFT), 30 + i * 110-10 + (Cosine(farleyTicker) * 4 >> FIXSHIFT), 255, 0);
			BlitIconBit(588, 467, 588 + 12, 467 + 13, 50+520 - (Cosine(farleyTicker) * 4 >> FIXSHIFT), 30 + i * 110+98 - (Cosine(farleyTicker) * 4 >> FIXSHIFT), 255, 0);
		}
		byte fairyCt=0;
		for (int j = 0; j < 4; j++)
		{
			if (player.haveFairy & w)
				fairyCt++;
			w *= 2;
		}
		for (int j = 3; j >= 0; j--)
			OutlineFarleySprite(173,64 + j * 16, 30 + i * 110 + 85, 0 - 31 * (j >= fairyCt));

		fairyCt = 0;
		for (int j = 0; j < MAX_MAPS; j++)
			if (GotRuneInLevel(i,j))
				fairyCt++;

		for (int j = 5; j >= 0; j--)
			OutlineFarleySprite(281,570 - 16 * 6 + j * 16, 30 + i * 110 + 85, 0 - 31 * (j >= fairyCt));

		fairyCt = 0;
		byte totalSpells = 0;
		for (int j = 0; j < MAX_MAPS; j++)
		{
			byte b = SpellBookForThisLevel(j, i);
			if (b != 255)
			{
				totalSpells++;
				if (GotSpellInLevel(i, j))
					fairyCt++;
			}
		}

		for (int j = totalSpells-1; j >= 0; j--)
			OutlineFarleySprite(45, 565 - 16 * totalSpells + j * 16, 30 + i * 110 + 45, 0 - 31 * (j >= fairyCt));

		OutlineFarleySprite(131,160,30+i*110+85, 0-31*(player.swordPiece[i]==0));
	}
}

byte FarleyWorldChoice(void)
{
	return farleyCursor;
}

void RenderBasicControl(int y, const char *s, byte btn)
{
	int x = 170 + 100;
	bool started = false;
	RightPrint(170 + 80+1, y+1, s, -31, 1);
	RightPrint(170 + 80, y, s, 0, 1);
	if (opt.key[btn][0] != 0)
	{
		Print(x+1, y+1, ScanCodeText(opt.key[btn][0]), -31, 1);
		Print(x, y, ScanCodeText(opt.key[btn][0]), 0, 1);
		x += GetStrLength(ScanCodeText(opt.key[btn][0]),1)+4;
		started = true;
	}
	if (opt.key[btn][1] != 0)
	{
		if (started)
		{
			Print(x+1, y+1, "/", -31, 1);
			Print(x, y, "/", 0, 1);
			x += 10;
		}
		Print(x+1, y+1, ScanCodeText(opt.key[btn][1]), -31, 1);
		Print(x, y, ScanCodeText(opt.key[btn][1]), 0, 1);
		x += GetStrLength(ScanCodeText(opt.key[btn][1]),1) + 4;
		started = true;
	}
	if(started)
	{
		Print(x+1, y+1, "/", -31, 1);
		Print(x, y, "/", 0, 1);
		x += 10;
	}
	RenderGamepadButton(x, y-3, opt.joyCtrl[btn]);
}

void RenderBasicControls(void)
{
	RenderSkillBox(170 - 2, 200 - 2, SCRWID - 170 + 2, 380 + 2, 31, 0);
	RenderSkillBox(170, 200, SCRWID - 170, 380, 31, 6);

	CenterPrint(HALFWID+1, 200 + 5+1, "CONTROLS", -31, 2);
	CenterPrint(HALFWID, 200 + 5, "CONTROLS", 0, 2);

	int x = 170 + 100;
	int y = 200 + 40;
	RightPrint(170 + 80+1, y+1, "Movement:", -31, 1);
	RightPrint(170 + 80, y, "Movement:", 0, 1);
	if (opt.key[CTL_ID_UP][0] != 0)
	{
		for (int i = 0; i < 4; i++)
		{
			Print(x+1, y+1, ScanCodeText(opt.key[CTL_ID_UP + i][0]), -31, 1);
			Print(x, y, ScanCodeText(opt.key[CTL_ID_UP+i][0]), 0, 1);
			x += GetStrLength(ScanCodeText(opt.key[CTL_ID_UP + i][0]),1) + 4;
			if (i < 3)
			{
				Print(x+1, y+1, "/", -31, 1);
				Print(x, y, "/", 0, 1);
				x += 10;
			}
		}
		y += 20;
	}
	if (opt.key[CTL_ID_UP][1] != 0)
	{
		x = 170 + 100;
		for (int i = 0; i < 4; i++)
		{
			Print(x+1, y+1, ScanCodeText(opt.key[CTL_ID_UP + i][1]), -31, 1);
			Print(x, y, ScanCodeText(opt.key[CTL_ID_UP + i][1]), 0, 1);
			x += GetStrLength(ScanCodeText(opt.key[CTL_ID_UP + i][1]), 1) + 4;
			if (i < 3)
			{
				Print(x + 1, y + 1, "/", -31, 1);
				Print(x, y, "/", 0, 1);
				x += 10;
			}
		}
		y += 20;
	}
	x = 170 + 100;
	byte dpadOK = opt.dpadToMove;
	opt.dpadToMove = false;
	for (int i = 0; i < 4; i++)
	{
		RenderGamepadButton(x, y-3, opt.joyCtrl[CTL_ID_UP + i]);
		if(dpadOK)
			RenderGamepadButton(x, y+20-3, SDL_CONTROLLER_BUTTON_DPAD_UP+i);
		x += 20;
	}
	opt.dpadToMove = dpadOK;
	y += 20+dpadOK*20+10;
	RenderBasicControl(y, "Fire:", CTL_ID_B1);
	RenderBasicControl(y+20, "Cast:", CTL_ID_B2);
	RenderBasicControl(y+40, "Pause:", CTL_ID_ESCAPE);
}

void RenderSpellControls(void)
{
	RenderSkillBox(170 - 2, 200 - 2, SCRWID - 170 + 2, 460 + 2, 31, 0);
	RenderSkillBox(170, 200, SCRWID - 170, 460, 31, 6);

	CenterPrint(HALFWID + 1, 200 + 5 + 1, "SPELL CONTROLS", -31, 2);
	CenterPrint(HALFWID, 200 + 5, "SPELL CONTROLS", 0, 2);

	int x = 170 + 100;
	int y = 200 + 30;
	RenderBasicControl(y, "Cast:", CTL_ID_B2);
	y += 17;
	RenderBasicControl(y, "Prev Spell:", CTL_ID_B3);
	y += 17;
	RenderBasicControl(y, "Next Spell:", CTL_ID_B4);
	y += 17;
	for (int i = 0; i < 10; i++)
	{
		char s[32];
		sprintf(s, "Spell #%d:", i + 1);
		RenderBasicControl(y, s, CTL_ID_QC_1+i);
		y += 17;
	}
}

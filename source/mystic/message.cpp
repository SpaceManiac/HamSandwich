#include "message.h"
#include "sound.h"
#include "game.h"
#include "intface.h"
#include "options.h"

char speech[58*4][64]={
	// 0
	"Welcome to Beginnerton!  This town is",
	"really a tutorial on some basic game",
	"features. There's a spell you might",
	"want in here.",
	// 1
	"Fireballs are the weapon of choice.",
	"Each Energy Orb you collect increases",
	"the number you can launch at once.",
	"Each Speed Gem raises the rate of fire.",
	// 2
	"Spellbooks teach you new spells.",
	"Casting them uses up your mana (the",
	"blue bar). As you gain levels, your",
	"spells become more powerful.",
	// 3
	"Psst, hey... thousands of years ago,",
	"a great hero used a weapon known as",
	"the Armageddon Sword to defeat a",
	"monstrous evil.  To keep this sword",
	// 4
	"from falling into evil hands, it was",
	"broken into four pieces and hidden",
	"throughout the land.  Four pieces,",
	"four chapters... think about it.",
	// 5
	"The passage to Spooky Castle is",
	"blocked right now.  I have a feeling",
	"that it'll be cleared by the time you",
	"recover the orbs from X more places.",
	// 6
	"The passage to Spooky Castle is",
	"wide open now.  I can't think of a",
	"logical reason why, but it is.",
	"Good luck, Kid Mystic!!",
	// 7
	"My village was attacked by Mushies!",
	"As the village librarian, it's my",
	"job to remain here in case any heroes",
	"happen along that want the key to",
	// 8
	"the village library.  You look",
	"pretty heroic.  Here!",
	" ",
	" ",
	// 9
	"I hope you found something of",
	"interest in our library.  I guess",
	"I'll stay here and chronicle the",
	"Mushie occupation of our village.",
	// 10
	"Welcome to the Spooky Dance-a-thon!",
	"This nifty book is the prize for",
	"whoever can stay dancing and/or",
	"alive the longest!  Good luck!",
	// 11
	"You've won the Dance-a-thon!!",
	"Way to go!  The book's all yours.",
	" ",
	" ",
	// 12
	"To the left is knowledge, and to",
	"the right is wealth.  Choose wisely,",
	"you can't have both!",
	" ",
	// 13
	"You can launch fireballs by pressing",
	"the Fire button. Shocking, I know.",
	"Fireball upgrades only last for the",
	"level you are in, unlike Spells.",
	// 14
	"To cast a spell, press the Cast button.",
	"You can use the specific Spell buttons",
	"to switch to other spells, or use",
	"the Next and Previous buttons.",
	// 15
	"In the options menu, you can change",
	"between just selecting spells with those",
	"buttons, or instantly casting them. Play",
	"your own way!",
	// 16
	"One other thing... you can't leave",
	"any area that you enter until you",
	"collect all the mystical green orbs",
	"in it.  After all, that's what you're",
	// 17
	"here to do, right?  The Orbometer in",
	"the lower right of the screen will",
	"give you an idea of how many remain.",
	" ",
	// 18
	"You've just caught a fairy! Fairies",
	"assist you with magical powers, but",
	"some of them also have drawbacks.  You",
	"can choose which fairy to equip by",
	// 19
	"pressing the Spell key when you are in",
	"the overworld.  Pick the one you want",
	"to use and press Fire.  Pressing Spell",
	"again lets you go fairyless.",
	// 20
	"Welcome to Kid Mystic!  Remember the",
	"handy controls below. You can also",
	"change them, and many other things,",
	"in the Options menu on the title screen.",
	// 21
	"",
	"",
	"",
	"",
	// 22
	"Welcome to Madcap Mode!  You can",
	"now go through the entire adventure",
	"again, keeping all the powerups and",
	"levels you already earned... but",
	// 23
	"that won't be nearly enough!  Don't",
	"forget to see what's new in the Shop,",
	"because you will need it badly.",
	"Good luck!",
	// 24
	"Congratulations!  You found all the",
	"secret items in the game, so you've",
	"unlocked Challenge Mode!  Try it out",
	"on the main menu.",
	// 25
	"Welcome back to Madcap Mode!  You",
	"can keep exploring this mode all",
	"you want, or start a new game, or",
	"whatever!  Sorry, no more prizes!",
	// 26
	"You're playing in Modern Mode! That",
	"means you get Skill Points when you",
	"level up. Press ESC or START to spend",
	"your skill points.",
	// 27
	"You can reset your skills anytime as",
	"long as you are in the Overworld!",
	"You don't get stronger with levels",
	"anymore, so don't forget your skills!",
	// 28
	"I'm Farley the Ghost Bat! I can fly",
	"you to previous chapters. Don't worry,",
	"I can also fly you back.",
	"",
	// 29
	"This book describes the Runic Tongue.",
	"It looks like every rune has a letter",
	"that it's equivalent to. There's even",
	"a handy chart!",
	// 30
	"Hmm, according to this book, ancient",
	"numerologists assigned numbers to",
	"each rune in addition to their",
	"alphabetical value.",
	// 31
	"Ugh, fine you caught me. I had hoped",
	"I could just leave the lights off, but",
	"you stumbled right into me. Yes, it is",
	"I, Professor Dusseldorf, and I am",
	// 32
	"in fact reading romance novels. It's my",
	"guilty pleasure. Look, if you don't spill",
	"my secret, I will tell you another secret",
	"about this library. Deal?",
	// 33
	"I'll assume that blank stare means yes.",
	"Okay. In the Fiction section, there are",
	"a series of shelves you can move. If you",
	"push them all up, in order from left to",
	// 34
	"right, you will find a little surprise!",
	"If you screw it up, just push them all",
	"back down. Now let me get back to Lady",
	"Cordelia and the Strapping Lad.",
	// 35
	"Go to the Fiction section, I'm trying",
	"to read here!",
	"Ugh, just push up the shelves from",
	"left to right.",
	// 36
	"Oh dearie me! I'm just a helpless old",
	"lady! Would you assist me in making my",
	"famous Swamp Stew, young man? I need",
	"you to throw these ingredients in",
	// 37
	"the pot in exactly this order. Please",
	"don't make a mistake! Swamp Stew can",
	"be a trifle... temperamental.",
	"",
	// 38
	"Did you forget the recipe? Sometimes I",
	"mix it up too! Almost seems like it's",
	"always different. Here it is again: ",
	"",
	// 39
	"Delightful, that's all the ingredients!",
	"Now to cook it. Step on the button",
	"there to light the fire. The longer you",
	"stay on the button, the hotter it gets.",
	// 40
	"Once it's lit, there's enough wood for",
	"10 seconds of cooking. You need to boil",
	"it for 3 seconds and then keep it at a",
	"simmer until it's done.",
	// 41
	"Oh no, the Octon Juice isn't even",	// tentacle attack
	"cooked! This is definitely going to be",
	"a problem.",
	"",
	// 42
	"The stew is overheated! This can't be",	// magmazoids
	"good!",
	"",
	"",
	// 43
	"Wait, what order did you put the",	// just explodes
	"ingredients in? This is a disaster!",
	"",
	"",
	// 44
	"Ack, these are the wrong ingredients!",	// mushie attack
	"This is mushroom feed, not Swamp Stew!",
	"",
	"",
	// 45
	"Delicious, this is perfect Swamp Stew!",
	"Okay, full disclosure. I'm actually a",
	"witch. I know, I know, I don't look a",
	"day over hag. Anyway, since you were",
	// 46
	"so helpful, I'm gonna hex you up a nice",
	"treat - a Rune Pouch! That will let you",
	"equip one more rune at a time. Thanks",
	"again, young lad.",
	// 47
	"Well, you aren't exactly a chef, but",
	"I do appreciate you trying to help.",
	"Full disclosure: I'm actually a witch.",
	"Crazy, right? So since you have been",	// wraps back up to 46
	// 48
	"Get outta here, you ruined my swamp",
	"hovel! No stew for you! Or me, come",
	"to think of it!",
	"",
	// 49
	"I hope to see you again, Kid Mystic.",
	"You're a good egg.",
	"",
	"",
	// 50
	"so helpful, I'll give you a big old",
	"smooch. Come here cutie pie!",
	"",
	"Just kidding. Consent and all that.",
	// 51
	"Spellbooks teach you new spells.",
	"Casting them uses up your mana (the",
	"blue bar). As you gain levels, you get",
	"skill points you can spend to improve",
	// 52
	"your spells as well as yourself. I",
	"know this spell seems pretty sad",
	"right now, but check it out at level",
	"five! Pause the game to adjust skills.",
	// 53
	"The Monster Wilderness is full of tricks",
	"and traps. Sometimes there are walls",
	"you can push, or buttons to step on,",
	"or walls you can shoot.",
	// 54
	"I've created a very very subtle obstacle",
	"course for you here. Good luck figuring",
	"out which walls are the trick ones!",
	"I'm sneaky!",
	// 55
	"Oh, I see you're admiring my collection!",
	"Yes, I have my own personal Fairy Bell",
	"and Reflect Crystal! I keep them locked",
	"up for safety.",
	// 56
	"In fact, they're so securely stored that",
	"I just continue facing the other",
	"direction. I feel no need to check that",
	"they're still there.",
	// 57
	"They're still there, right? I'll just keep",
	"facing this way and assume they are.",
	"You'd let me know if they were gone, for",
	"sure. Right? I can trust you.",
};

byte speechX,speechY,curSpeech;

message_t bigMessage;
message_t message;
byte romanceTalk = 0;

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

/* this is for NPC speech */

void InitSpeech(byte spc)
{
	if(spc==7 && player.keys[2]==1)
		spc=9;

	if(spc==5)
	{
		// this guy tells you how many levels remain to clear.
		if(player.levelsPassed==9)
			spc=6;
		else
		{
			speech[23][22]='9'-player.levelsPassed;
			if(player.levelsPassed==8)
			{
				speech[23][34]='.';
				speech[23][35]=' ';
			}
			else
			{
				speech[23][34]='s';
				speech[23][35]='.';
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
	curSpeech=spc;
	speechX=0;
	speechY=0;
	EnterSpeechMode();
}

byte UpdateSpeech(MGLDraw *mgl)
{
	if(ButtonTapped(CONTROL_B1|CONTROL_B2))
	{
		if(speechY<4)
			speechY=4;
		else
		{
			if(curSpeech==1)
			{
				curSpeech=13;
				speechX=0;
				speechY=0;
				return 0;
			}
			if(curSpeech==13)
			{
				curSpeech=16;
				speechX=0;
				speechY=0;
				return 0;
			}
			if(curSpeech==16)
			{
				curSpeech=17;
				speechX=0;
				speechY=0;
				return 0;
			}
			if (curSpeech == 51)
			{
				curSpeech = 52;
				speechX = 0;
				speechY = 0;
				return 0;
			}
			if (curSpeech == 52)
			{
				curSpeech = 14;
				speechX = 0;
				speechY = 0;
				return 0;
			}
			if (curSpeech == 53)
			{
				curSpeech = 54;
				speechX = 0;
				speechY = 0;
				return 0;
			}
			if (curSpeech == 55)
			{
				curSpeech = 56;
				speechX = 0;
				speechY = 0;
				return 0;
			}
			if(curSpeech==2)
			{
				curSpeech=14;
				speechX=0;
				speechY=0;
				return 0;
			}
			if(curSpeech==14)
			{
				curSpeech=15;
				speechX=0;
				speechY=0;
				return 0;
			}
			if(curSpeech==18)
			{
				curSpeech=19;
				speechX=0;
				speechY=0;
				return 0;
			}
			if(curSpeech==20)
			{
				if (!ClassicMode())
					curSpeech = 26;
				else
					return 1;
				speechX=0;
				speechY=0;
				return 0;
			}
			if (curSpeech == 21 && !ClassicMode())
			{
				curSpeech = 26;
				speechX = 0;
				speechY = 0;
				return 0;
			}
			if (curSpeech == 26)
			{
				curSpeech = 27;
				speechX = 0;
				speechY = 0;
				return 0;
			}
			if(curSpeech==22)
			{
				curSpeech=23;
				speechX=0;
				speechY=0;
				return 0;
			}
			if(curSpeech==3 || curSpeech==7)
			{
				curSpeech++;
				speechX=0;
				speechY=0;
				return 0;
			}
			if (curSpeech == 36)
			{
				curSpeech = 37;
				speechX = 0;
				speechY = 0;
				return 0;
			}
			if (curSpeech == 39)
			{
				SwampEnableCooking();
				curSpeech = 40;
				speechX = 0;
				speechY = 0;
				return 0;
			}
			if (curSpeech == 41)
			{
				// uncooked octon means big tentacle attack
				SwampDestroyCauldron(0);
				return 1;
			}
			if (curSpeech == 42)	// overheated stew means magmazoids
			{
				SwampDestroyCauldron(1);
				return 1;
			}
			if (curSpeech == 43) // wrong order, stew explodes
			{
				SwampDestroyCauldron(2);
				return 1;
			}
			if (curSpeech == 44) // wrong amounts, mushies
			{
				SwampDestroyCauldron(3);
				return 1;
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
			if (curSpeech == 46)
			{
				// get rune pouch
				CurrentMap()->GetTile(60, 62)->item = ITM_RUNEPOUCH;
				CurrentMap()->BrightTorch(60, 62, 24,8);
				MakeNormalSound(SND_MEGABEAMHIT);
				FloaterParticles((60*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT, (62*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT, 6, 20, 2, 20);
				return 1;
			}
			
			if (curSpeech >= 31 && curSpeech<=33)
			{
				if (curSpeech == 31)
				{
					CurrentMap()->LightSpecial(GetGoodguy()->x / (TILE_WIDTH * FIXAMT), GetGoodguy()->y / (TILE_HEIGHT * FIXAMT), 0, 20);
					romanceTalk = 1;
				}
				curSpeech = curSpeech+1;
				speechX = 0;
				speechY = 0;
				return 0;
			}
			else
			{
				if(curSpeech==8)
				{
					MakeNormalSound(SND_GETKEY);
					player.keys[2]=1;
				}
				if (curSpeech == 28)
					EnterFarleyMode();
				return 1;
			}
		}
	}

	if(speechY<4)
	{
		if((speechX&1)==0)
			MakeNormalSound(SND_CHAT);
		speechX++;
		if(speechX>=strlen(speech[curSpeech*4+speechY]))
		{
			speechX=0;
			speechY++;
		}
	}

	if (ButtonTapped(CONTROL_ESCAPE))
	{
		SetInMenu(false);
		UpdateControls();
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
		strcpy(s,speech[curSpeech*4+i]);
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
	if (curSpeech == 14 || curSpeech == 15 || curSpeech==21 || curSpeech==51 || curSpeech==52)	// show spell buttons
	{
		RenderSpellControls();
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

void RenderBasicControl(int y, char *s, byte btn)
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

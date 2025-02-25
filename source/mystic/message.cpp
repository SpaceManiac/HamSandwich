#include "message.h"
#include "sound.h"
#include "game.h"
#include "intface.h"

char speech[30*4][64]={
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
	"Casting them uses up your mana (spell",
	"power). As you gain levels, your spells",
	"become more powerful.",
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
	"You can launch fireballs by",
	"pressing the Fire key or button 1",
	"on the joystick.",
	" ",
	// 14
	"To cast a spell, press the Spell key or",
	"button 2 on the joystick.  You can",
	"switch which spell to cast with the",
	"Next and Previous keys, or button 3 & 4",
	// 15
	"joystick, or by directly pushing",
	"the number key for the spell you",
	"want.  This Energy Barrage spell",
	"is number 1.",
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
	"You've just caught a fairy!  Fairies",
	"assist you with magical powers, but",
	"some of them also have drawbacks.  You",
	"can choose which fairy to equip by",
	// 19
	"pressing the Spell key when you are in",
	"the overworld.  Pick the one you want",
	"to use and press Fire.  Pressing Spell",
	"again lets you go fairyless.",
	// 20
	"Welcome to Kid Mystic!  Control the Kid",
	"with the arrow keys or gamepad.",
	"Fire: CTRL, Z, or Gamepad Button 1",
	"Spell: SHIFT, X, or Gamepad Button 2",
	// 21
	"Previous Spell: A or Gamepad Button 3",
	"Next Spell: S or Gamepad Button 4",
	"Pause: ESC or Start",
	"Visit Beginnerton above to learn more!",
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
	"can keep repeating this mode all",
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
};

byte speechX,speechY,curSpeech;

message_t bigMessage;
message_t message;
static byte oldc;

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
	curSpeech=spc;
	speechX=0;
	speechY=0;
	EnterSpeechMode();
	oldc=255;
}

byte UpdateSpeech(MGLDraw *mgl)
{
	byte c;

	c=GetControls();

	if((c&CONTROL_B1) && (!(oldc&CONTROL_B1)))
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
				oldc=c;
				return 0;
			}
			if(curSpeech==13)
			{
				curSpeech=16;
				speechX=0;
				speechY=0;
				oldc=c;
				return 0;
			}
			if(curSpeech==16)
			{
				curSpeech=17;
				speechX=0;
				speechY=0;
				oldc=c;
				return 0;
			}
			if(curSpeech==2)
			{
				curSpeech=14;
				speechX=0;
				speechY=0;
				oldc=c;
				return 0;
			}
			if(curSpeech==14)
			{
				curSpeech=15;
				speechX=0;
				speechY=0;
				oldc=c;
				return 0;
			}
			if(curSpeech==18)
			{
				curSpeech=19;
				speechX=0;
				speechY=0;
				oldc=c;
				return 0;
			}
			if(curSpeech==20)
			{
				curSpeech=21;
				speechX=0;
				speechY=0;
				oldc=c;
				return 0;
			}
			if (curSpeech == 21 && !ClassicMode())
			{
				curSpeech = 26;
				speechX = 0;
				speechY = 0;
				oldc = c;
				return 0;
			}
			if (curSpeech == 26)
			{
				curSpeech = 27;
				speechX = 0;
				speechY = 0;
				oldc = c;
				return 0;
			}
			if(curSpeech==22)
			{
				curSpeech=23;
				speechX=0;
				speechY=0;
				oldc=c;
				return 0;
			}
			if(curSpeech==3 || curSpeech==7)
			{
				curSpeech++;
				speechX=0;
				speechY=0;
				oldc=c;
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
	UpdateGamepadStartAndSelect();
	if (mgl->LastKeyPressed() == 27 || GamepadStartTapped() || GamepadSelectTapped())
	{
		oldc = c;
		return 1;	// ESC cancels the speech
	}
	oldc=c;
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
}

byte farleyCursor;
byte farleyTicker;
void InitFarley(void)
{
	farleyCursor = player.worldNum;
	oldc = 255;	
}

byte UpdateFarley(MGLDraw *mgl)
{
	byte c = GetControls();
	farleyTicker+=2;
	int top = (int)HighestWorldReached();
	if ((c & CONTROL_UP) && !(oldc & CONTROL_UP))
	{
		farleyCursor--;
		if (farleyCursor > 200)
			farleyCursor = top;
	}
	if ((c & CONTROL_DN) && !(oldc & CONTROL_DN))
	{
		farleyCursor++;
		if (farleyCursor > top)
			farleyCursor = 0;
	}
	if ((c & CONTROL_B1) && !(oldc & CONTROL_B1))
	{
		return 1;	// we're gonna stay here
	}
	oldc = c;
	return 0;
}

void RenderFarley(void)
{
	char s[32];
	int top = (int)HighestWorldReached();
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
	}
}

byte FarleyWorldChoice(void)
{
	return farleyCursor;
}

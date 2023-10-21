#include "textgame.h"
#include "goal.h"

// rooms
#define TR_ASYLDOOR		0
#define TR_FOOTHILL		1
#define TR_DESERT		2
#define TR_DESERT2		3
#define TR_DESERT3		4
#define TR_DESERT4		5
#define TR_DESERT5		6
#define TR_OASIS		7
#define TR_HILL			8
#define TR_SPHINX		9
#define TR_CLEARING		10
#define TR_ASYLUM		11
#define TR_PEAK			12
#define TR_ICYPATH		13
#define TR_CHASM		14
#define TR_LEDGE		15
#define TR_YETI			16
#define TR_CAVE			17
#define TR_BANANA		18
#define TR_FOREST		19
#define TR_CAVE2		20
#define TR_CAVE3		21
#define TR_TUNNEL		22
#define TR_VENDING		23
#define TR_PIRATE		24
#define TR_THING		25
#define TR_HOLEROOM		26
#define TR_THING2		27
#define TR_HOLEROOM2	28
#define TR_SPRING		29
#define TR_SPIDERS		30
#define TR_SPIDERS2		31
#define TR_MATILDA		32
#define TR_MAX			33

// items
#define TI_NONE			0
#define TI_JAR			1
#define TI_FULLJAR		2
#define TI_LOONYKEY		3
#define TI_ASYLDOOR		4
#define TI_SAND			5
#define TI_WATER		6
#define TI_PICK			7
#define TI_BOULDER		8
#define TI_HOLE			9
#define TI_SPHINXTER	10
#define TI_BUGSPRAY		11
#define TI_POT			12
#define TI_SEED			13
#define TI_BLOCK		14
#define TI_ICE			15
#define TI_COIN			16
#define TI_SVEN			17
#define TI_BJORN		18
#define TI_OLAF			19
#define TI_KONGOR		20
#define TI_THING		21
#define TI_MATILDA		22
#define TI_BANANA		23
#define TI_HOOK			24
#define TI_BOOTS		25
#define TI_VEND			26
#define TI_SPIDERS		27
#define TI_WEBS			28
#define TI_SILK			29
#define TI_GEAR			30
#define TI_CANDY		31
#define TI_FROZENCANDY	32
#define TI_PEEL			33
#define TI_MONEY		34
#define TI_GHOST		35
#define TI_LUNATIC		36
#define TI_VINE			37
#define TI_DOOR			38
#define TI_MAX			39

// flags
#define TIF_NOGET		(1)
#define TIF_NOLIST		(2)	// not listed in the room
#define TIF_WATER		(4)	// requires jar to get, instead of getting it, it turns the jar from empty to full

#define MAX_LINELEN	(80)
#define MAX_LINES	(20)
#define HELPLEN		(14)
#define INV_SIZE	(TI_MAX+4)

typedef struct txtRoom_t
{
	char name[MAX_LINELEN];
	char desc[8][MAX_LINELEN];
	byte item[TI_MAX+4];
	byte exit[4];
} txtRoom_t;

typedef struct txtItem_t
{
	char typeName[4][16];	// the name as it must be typed in, up to 4 options
	char name[32];			// the name as described in game
	char desc[4][MAX_LINELEN];
	dword flags;
} txtItem_t;

static const char helpTxt[HELPLEN][MAX_LINELEN]={
	"Type 2-word commands to do things, always 'VERB NOUN'.  For example,",
	"'GET TORCH' or 'OPEN DOOR'.  There are several handy shortcuts - ",
	"just type 'N' to move North, or 'W', 'S', or 'E' for the other ",
	"directions.  Type 'I' to see what your inventory contains.  Type ",
	"'L' to look at the room you're in again.  Type 'G' to repeat your ",
	"previous action.  Type 'Z' to wait a little while. You can talk to",
	"characters by typing 'TALK NAME'... but replace NAME with their ",
	"name!  Type 'VERBS' for a complete list of all verbs the game can ",
	"understand.  That will help you solve the puzzles!  It will also ",
	"help to 'LOOK ITEM' where ITEM is the name of an item in the room",
	"or in your inventory.  If the game doesn't know what you're talking",
	"about, odds are you don't need to do anything with it.",
	"Type 'SCORE' to see your current score.",
	"Press the ESCAPE key to quit the game at any time.",
};

#include "textitems.inl"
#include "textrooms.inl"

static txtItem_t items[TI_MAX];
static txtRoom_t rooms[TR_MAX];

char inputTxt[MAX_LINELEN];
char lastInput[MAX_LINELEN];

static byte room,heat,riddle,yetiSleep,score,oasis,filledJar,usedBoots,climbedPeak,openedAsylum,silkYet,cubeExists;
static byte cubeMelt,candyMelt,frozenBar,chippedIce,matildaKey,gotBananas,haunted,scaredThing,bugsSprayed,lunaticCaught;
static byte vineGrown;
static byte inv[INV_SIZE];
static byte *backgd;
char textbuf[MAX_LINES][MAX_LINELEN];

namespace
{
	dword lastGamepad;
	bool gamepadMode;
	constexpr int KEYBOARD_WIDTH = 10;
	constexpr int KEYBOARD_HEIGHT = 4;
	const char KEYBOARD[KEYBOARD_HEIGHT][KEYBOARD_WIDTH+1] = {
		"1234567890",
		"QWERTYUIOP",
		"ASDFGHJKL~", // backspace
		" ZXCVBNM~~", // space, enter
	};
	int curRow = 0, curCol = 0;
}

void TypeLine(const char *line)
{
	int i;

	if(strlen(line)>MAX_LINELEN)
		TypeLine("LINE TOO LONG!");

	for(i=0;i<MAX_LINES-1;i++)
		strcpy(textbuf[i],textbuf[i+1]);
	strcpy(textbuf[MAX_LINES-1],line);
}

void EchoInput(void)
{
	char tmp[MAX_LINELEN + 2];

	sprintf(tmp,"> %s",inputTxt);
	TypeLine("");
	TypeLine(tmp);
	TypeLine("");
}

void DescribeItem(byte itm)
{
	int i;

	TypeLine(items[itm].name);
	TypeLine("");
	for(i=0;i<4;i++)
	{
		if(items[itm].desc[i][0]!='\0')
			TypeLine(items[itm].desc[i]);
	}
}

void DescribeRoom(void)
{
	int i;
	char itm[MAX_LINELEN];
	byte itms,cnt;

	TypeLine(rooms[room].name);
	TypeLine("");
	for(i=0;i<8;i++)
	{
		if(rooms[room].desc[i][0]!='\0')
			TypeLine(rooms[room].desc[i]);
	}
	itm[0]='\0';
	itms=0;
	for(i=0;i<TI_MAX+4;i++)
		if(rooms[room].item[i] && !(items[rooms[room].item[i]].flags&TIF_NOLIST))
			itms++;

	if(itms)
	{
		cnt=0;
		for(i=0;i<TI_MAX+4;i++)
		{
			if(rooms[room].item[i] && !(items[rooms[room].item[i]].flags&TIF_NOLIST))
			{
				if(strlen(itm)==0)
				{
					strcpy(itm,"You see ");
				}
				else
				{
					if(strlen(itm)+strlen(", ")>MAX_LINELEN-3)
					{
						strcat(itm,",");
						TypeLine(itm);
						itm[0]='\0';
					}
					else
						strcat(itm,", ");
				}
				if(itms>1 && cnt==itms-1)
				{
					if(strlen(itm)+strlen("and ")>MAX_LINELEN-3)
					{
						TypeLine(itm);
						itm[0]='\0';
					}
					strcat(itm,"and ");
				}
				if(strlen(itm)+strlen(items[rooms[room].item[i]].name)>MAX_LINELEN-3)
				{
					TypeLine(itm);
					itm[0]='\0';
				}
				strcat(itm,items[rooms[room].item[i]].name);
				cnt++;
			}
		}
		if(strlen(itm)+strlen(" here.")>MAX_LINELEN-3)
		{
			TypeLine(itm);
			itm[0]='\0';
		}
		strcat(itm," here.");
		TypeLine(itm);
	}
}

void ListInventory(void)
{
	int i;
	byte c;
	char s[48];

	TypeLine("In your pockets, on your head, shoved down your shirt, and in your socks,");
	TypeLine("you are carrying:");
	TypeLine("");

	c=0;
	for(i=0;i<INV_SIZE;i++)
	{
		if(inv[i]==TI_LOONYKEY)
			c++;
	}
	if(c>0)
	{
		if(c==1)
			sprintf(s,"%d Key Of Lunacy",c);
		else
			sprintf(s,"%d Keys Of Lunacy",c);
		TypeLine(s);
	}

	for(i=0;i<INV_SIZE;i++)
	{
		if(inv[i] && inv[i]!=TI_LOONYKEY)
		{
			TypeLine(items[inv[i]].name);
			c++;
		}
	}
	if(c==0)
		TypeLine("Absolutely NOTHING!");
}

void ShowScore(void)
{
	char s[48];

	sprintf(s,"You have %d out of 100 possible points.",score);
	TypeLine(s);
}

void AddScore(byte amt)
{
	char s[48];

	score+=amt;
	sprintf(s,"Your score just went up by %d!",amt);
	TypeLine(s);
	ShowScore();
}

byte HaveLoonyKeys(void)
{
	int i;
	byte c;

	c=0;
	for(i=0;i<INV_SIZE;i++)
	{
		if(inv[i]==TI_LOONYKEY)
			c++;
	}
	return (c==4);
}

void CopyRoom(byte src,byte dst)
{
	int i;

	memcpy(rooms[dst].name,rooms[src].name,MAX_LINELEN);
	for(i=0;i<8;i++)
		memcpy(rooms[dst].desc[i],rooms[src].desc[i],MAX_LINELEN);
}

byte TextHaveItem(byte itm)
{
	int i;

	for(i=0;i<INV_SIZE;i++)
	{
		if(inv[i]==itm)
			return 1;
	}
	return 0;
}


void Move(byte dir)
{
	int i,j;

	if(rooms[room].exit[dir]==255)
	{
		EchoInput();
		TypeLine("You can't go that way... who said you could?  I know I didn't.  Just chill out");
		TypeLine("and pick a legitimate direction.");
		return;
	}
	else
	{
		// special move conditions can be checked here
		if(rooms[room].exit[dir]==TR_YETI)
		{
			if(!vineGrown)
			{
				EchoInput();
				TypeLine("It would be unwise to attempt to walk across a bottomless chasm unaided.");
				return;
			}
			else
				TypeLine("You crawl across the vine cautiously...");
		}
		if(rooms[room].exit[dir]==TR_MATILDA)
		{
			if(!bugsSprayed)
			{
				EchoInput();
				TypeLine("You try to go through the webs, but the little spiders are rebuilding");
				TypeLine("them as fast as you can tear them down, and you only succeed in getting");
				TypeLine("all sticky.");
				return;
			}
			else if(!silkYet)
			{
				silkYet=1;
				EchoInput();
				TypeLine("You plow through the webs recklessly.  You make it, but you end up");
				TypeLine("wrapped in layers of silk, which you spend a few minutes peeling off.");
				room=rooms[room].exit[dir];
				DescribeRoom();
				return;
			}
		}
		if(rooms[room].exit[dir]==TR_ASYLUM)
		{
			if(!HaveLoonyKeys())
			{
				EchoInput();
				TypeLine("You're not getting that door open until you have 4 Keys Of Lunacy!");
				return;
			}
			else
			{
				EchoInput();
				if(!openedAsylum)
				{
					TypeLine("You unlock all 4 padlocks, and swing the door open...");
					openedAsylum=1;
					AddScore(8);
				}
				room=rooms[room].exit[dir];
				DescribeRoom();
				return;
			}
		}
		if(rooms[room].exit[dir]==TR_ICYPATH)
		{
			if(!TextHaveItem(TI_BOOTS))
			{
				EchoInput();
				TypeLine("The path is too icy!  You go slip-sliding right back down.");
				return;
			}
			else
			{
				EchoInput();
				TypeLine("Carefully donning your spikey pirate boots, you traverse the slippery slope.");
				if(!usedBoots)
				{
					usedBoots=1;
					AddScore(3);
				}
				room=rooms[room].exit[dir];
				DescribeRoom();
				return;
			}
		}
		if(rooms[room].exit[dir]==TR_PEAK && room!=TR_LEDGE)
		{
			if(!TextHaveItem(TI_GEAR))
			{
				EchoInput();
				TypeLine("That's way too steep to climb!  You'll need climbing gear.");
				return;
			}
			else
			{
				EchoInput();
				TypeLine("You whip your handy hook up to the top and climb the silk!");
				if(!climbedPeak)
				{
					climbedPeak=1;
					AddScore(2);
				}
				room=rooms[room].exit[dir];
				DescribeRoom();
				return;
			}
		}
		if(room==TR_PEAK && rooms[room].exit[dir]!=TR_LEDGE)
		{
			if(!TextHaveItem(TI_GEAR))
			{
				EchoInput();
				TypeLine("You're not just gonna drop off this mountain!");
				return;
			}
			else
			{
				EchoInput();
				TypeLine("You rappel down the mountain with your climbing gear.");
				room=rooms[room].exit[dir];
				DescribeRoom();
				return;
			}
		}
		EchoInput();
		for(i=0;i<TI_MAX+4;i++)
		{
			if(rooms[room].item[i]==TI_GHOST)
			{
				// the ghost comes with you
				if(rooms[room].exit[dir]==TR_CAVE)
				{
					TypeLine("The ghost howls in fury.  \"Drat, I can't be followin' ye into yonder sunlight!");
					TypeLine("YARRR!!\"");
					if(haunted==0)
					{
						haunted=1;
						AddScore(5);
					}
				}
				else
				{
					// bring the ghost along
					rooms[room].item[i]=0;
					for(j=0;j<TI_MAX+4;j++)
					{
						if(rooms[rooms[room].exit[dir]].item[j]==0)
						{
							rooms[rooms[room].exit[dir]].item[j]=TI_GHOST;
							switch(Random(3))
							{
								case 0:
									TypeLine("The ghost follows you, whistling an old sea shanty.");
									break;
								case 1:
									TypeLine("The ghost follows you, clanking invisible chains.");
									break;
								case 2:
									TypeLine("The ghost follows you, mumbling about how a ghost's work");
									TypeLine("is never done.");
									break;
							}
							break;
						}
					}
				}
			}
		}
		room=rooms[room].exit[dir];
		DescribeRoom();
		if(room==TR_OASIS && oasis==0)
		{
			oasis=1;
			AddScore(5);
		}
		if(room==TR_THING && !scaredThing)
		{
			for(j=0;j<TI_MAX+4;j++)
			{
				if(rooms[room].item[j]==TI_GHOST)
				{
					scaredThing=1;
					TypeLine("");
					TypeLine("As soon as you step in the door, The Thing sees the ghost following");
					TypeLine("you, shrieks in terror, and smashes out through the back wall!");
					TypeLine("The ghost chuckles.");
					AddScore(10);
					CopyRoom(TR_THING2,TR_THING);
					for(j=0;j<TI_MAX+4;j++)
					{
						if(rooms[room].item[j]==TI_THING)
						{
							rooms[room].item[j]=0;
							return;
						}
					}
				}
			}
		}
	}
}

byte ActuallyGetItem(byte itm)
{
	// some items can't be gotten!
	int i;
	char s[MAX_LINELEN];

	if(items[itm].flags&TIF_NOGET)
	{
		TypeLine("You can't pick that up!");
		return 0;
	}
	if(items[itm].flags&TIF_WATER)
	{
		for(i=0;i<INV_SIZE;i++)
			if(inv[i]==TI_JAR)
			{
				inv[i]=TI_FULLJAR;
				strcpy(items[TI_JAR].typeName[0],"");
				strcpy(items[TI_JAR].typeName[1],"");
				strcpy(items[TI_FULLJAR].typeName[0],"JAR");
				strcpy(items[TI_FULLJAR].typeName[1],"GLASS");
				strcpy(items[TI_FULLJAR].typeName[2],"WATER");

				TypeLine("You fill your jar with some of the water!");
				if(!filledJar)
				{
					filledJar=1;
					AddScore(2);
				}
				return 0;
			}
			else if(inv[i]==TI_FULLJAR)
			{
				TypeLine("Your jar is already full of water!");
				return 0;
			}
		TypeLine("As talented as you are, you can't carry water in your hands.");
		return 0;
	}

	if(room==TR_MATILDA)
	{
		if(itm==TI_LOONYKEY)
		{
			TypeLine("Matilda snaps at you with claws significantly bigger than");
			TypeLine("your head, so you step back.");
			return 0;
		}
	}
	if(room==TR_BANANA && !gotBananas)
	{
		if(itm==TI_BANANA)
		{
			TypeLine("Kongor thumps his chest and gives you a dirty look.");
			return 0;
		}
	}
	if(room==TR_THING && !scaredThing)
	{
		if(itm==TI_LOONYKEY)
		{
			TypeLine("The Thing smacks you back with a tentacle.  No Key Of");
			TypeLine("Lunacy for you!");
			return 0;
		}
	}
	if(room==TR_YETI && !yetiSleep)
	{
		if(itm==TI_LOONYKEY)
		{
			TypeLine("A simultaneous menacing growl from the three yetis keeps");
			TypeLine("you firmly in place.");
			return 0;
		}
	}
	if(room==TR_PIRATE && (itm==TI_HOOK || itm==TI_BOOTS))
	{
		for(i=0;i<TI_MAX+4;i++)
		{
			if(rooms[room].item[i]==TI_GHOST)
			{
				TypeLine("\"Yarr!!  Ye shan't be touching me booty, scalliwag!\"");
				TypeLine("The pirate floats the objects through the air just out of your");
				TypeLine("reach.");
				return 0;
			}
		}
	}
	for(i=0;i<INV_SIZE;i++)
	{
		if(inv[i]==TI_NONE)
		{
			inv[i]=itm;
			sprintf(s,"You pick up %s.",items[itm].name);
			TypeLine(s);
			return 1;
		}
	}
	return 0;
}

byte TextGetItem(char *name)
{
	int i,j,k;

	if(name==NULL)
	{
		TypeLine("You have to specify what you want to get!");
		return 0;
	}
	if(!strcmp(name,"ALL") || !strcmp(name,"EVERYTHING"))
	{
		for(i=0;i<TI_MAX+4;i++)
		{
			if(rooms[room].item[i] && !(items[rooms[room].item[i]].flags&(TIF_NOGET)))
			{
				TextGetItem(items[rooms[room].item[i]].typeName[0]);
			}
		}
		return 1;
	}
	for(i=0;i<TI_MAX;i++)
	{
		for(j=0;j<4;j++)
			if(!strcmp(name,items[i].typeName[j]))
			{
				// you are trying to get this item
				for(k=0;k<TI_MAX+4;k++)
				{
					if(rooms[room].item[k]==i)
					{
						if(ActuallyGetItem(i))
						{
							rooms[room].item[k]=TI_NONE;
							return 1;
						}
						else
							return 0;
					}
				}
				for(k=0;k<INV_SIZE;k++)
				{
					if(inv[k]==i)
					{
						TypeLine("You've already got that!");
						return 0;
					}
				}
			}
	}
	TypeLine("I don't know what you're trying to get.");
	return 0;
}

void TextLook(char *name)
{
	int i,j,k;

	if(name==NULL)
	{
		DescribeRoom();
		return;
	}

	for(i=0;i<TI_MAX;i++)
	{
		for(j=0;j<4;j++)
			if(!strcmp(name,items[i].typeName[j]))
			{
				// you are trying to look at this item
				for(k=0;k<TI_MAX+4;k++)
				{
					if(rooms[room].item[k]==i)
					{
						DescribeItem(i);
						return;
					}
				}
				for(k=0;k<INV_SIZE;k++)
				{
					if(inv[k]==i)
					{
						DescribeItem(i);
						return;
					}
				}
			}
	}
	TypeLine("I don't know what you're trying to look at!");
}

byte TextDropItem(char *name)
{
	int i,j,k,m;
	char s[MAX_LINELEN];
	byte dontHave;

	dontHave=0;
	if(name==NULL)
	{
		TypeLine("You have to specify what you want to drop!");
		return 0;
	}
	if(!strcmp(name,"ALL") || !strcmp(name,"EVERYTHING"))
	{
		k=0;
		for(i=0;i<INV_SIZE;i++)
		{
			if(inv[i])
			{
				k++;
				for(j=0;j<TI_MAX+4;j++)
				{
					if(rooms[room].item[j]==0)
					{
						rooms[room].item[j]=inv[i];
						inv[i]=0;
						sprintf(s,"You drop %s.",items[rooms[room].item[j]].name);
						TypeLine(s);
						j=TI_MAX+4;
					}
				}
			}

		}
		if(k==0)
		{
			TypeLine("You don't have anything to drop!");
			return 0;
		}
		return 1;
	}
	for(i=0;i<TI_MAX;i++)
	{
		for(j=0;j<4;j++)
			if(!strcmp(name,items[i].typeName[j]))
			{
				// you are trying to drop this item
				for(k=0;k<INV_SIZE;k++)
				{
					if(inv[k]==i)
					{
						if(i==TI_SAND && room==TR_SPHINX)
						{
							riddle=2;
							inv[k]=TI_LOONYKEY;
							TypeLine("You place the sand on Sphinxter's paw and he looks at you in amazement.");
							TypeLine("\"Nobody's gotten that riddle before!  Wow!  Alright, I promised a key, ");
							TypeLine("so here you go!\"");
							TypeLine("Sphinxter hands you a Key Of Lunacy, and goes back to glaring at you.");
							AddScore(10);
							return 1;
						}
						if(i==TI_CANDY && room==TR_LEDGE)
						{
							candyMelt=0;
							inv[k]=0;

							for(m=0;m<TI_MAX+4;m++)
							{
								if(rooms[room].item[m]==0)
								{
									rooms[room].item[m]=TI_FROZENCANDY;
									break;
								}
							}
							TypeLine("You set the candy bar down on the block of ice.  In moments, it's ");
							TypeLine("frozen solid!  That is some cold ice.");
							if(!frozenBar)
							{
								frozenBar=1;
								AddScore(2);
							}
							return 1;
						}
						if(i==TI_CANDY && room==TR_MATILDA)
						{
							TypeLine("\"No thanks - if I'm exchanging the incredibly valuable Key Of Lunacy.");
							TypeLine("I at least want to get a FROZEN candy bar for it.  Mmm, I love 'em");
							TypeLine("frozen.\"");
						}
						if(i==TI_FROZENCANDY && room==TR_MATILDA)
						{
							inv[k]=TI_LOONYKEY;
							TypeLine("Matilda grabs the candy bar and munches it down in 3 seconds flat.");
							TypeLine("\"Alright, a deal's a deal.  Man, that was some good chocolate.");
							TypeLine("Here's my Key Of Lunacy.  Have fun with it!\"");
							matildaKey=1;
							for(m=0;m<TI_MAX+4;m++)
							{
								if(rooms[room].item[m]==TI_LOONYKEY)
								{
									rooms[room].item[m]=0;
									break;
								}
							}
							AddScore(10);
							return 1;
						}
						if(i==TI_BANANA && room==TR_YETI && !yetiSleep)
						{
							yetiSleep=1;
							TypeLine("The yetis fall on the bananas and mow through them like hungry");
							TypeLine("yetis eating bananas.  A minute later, all 3 are snoring loudly.");
							TypeLine("All that remains of the carnage is a single banana peel.");
							AddScore(10);
							inv[k]=0;
							for(m=0;m<TI_MAX+4;m++)
							{
								if(rooms[room].item[m]==0)
								{
									rooms[room].item[m]=TI_PEEL;
									break;
								}
							}
							return 1;
						}
						if(i==TI_MONEY && room==TR_BANANA && !gotBananas)
						{
							gotBananas=1;
							TypeLine("\"Thank!  Have bananas!\"");
							TypeLine("Kongor hands you the largest bunch of bananas you have ever seen.");
							AddScore(3);
							inv[k]=TI_BANANA;
							for(m=0;m<TI_MAX+4;m++)
							{
								if(rooms[room].item[m]==TI_BANANA)
								{
									rooms[room].item[m]=0;
									break;
								}
							}
							return 1;
						}
						if(i==TI_COIN && room==TR_VENDING)
						{
							TypeLine("You put the quarter in the vending machine.  There's a clunk, then");
							TypeLine("nothing happens.  You hit the machine.  You hit it again.  You");
							TypeLine("pummel the machine mercilessly and shake it all around.  Suddenly,");
							TypeLine("it gives in and breaks down, dumping out a candy bar and a bunch of");
							TypeLine("spare change!");
							AddScore(5);
							inv[k]=0;
							for(m=0;m<TI_MAX+4;m++)
							{
								if(rooms[room].item[m]==0)
								{
									rooms[room].item[m]=TI_MONEY;
									break;
								}
							}
							for(m=0;m<TI_MAX+4;m++)
							{
								if(rooms[room].item[m]==0)
								{
									rooms[room].item[m]=TI_CANDY;
									break;
								}
							}
							return 1;
						}
						if(i==TI_PEEL && room==TR_ASYLUM)
						{
							TypeLine("You casually toss down the banana peel.  Dr. Lunatic is so intent");
							TypeLine("on scrambling around crazily, he doesn't notice it, and he slips");
							TypeLine("on it!  He falls to the ground and is knocked unconscious.  YOU WIN!!");
							AddScore(10);
							lunaticCaught=1;
							CompleteGoal(71);
							SaveProfile();
							return 1;
						}
						if(i==TI_SEED && room==TR_CHASM)
						{
							TypeLine("You gently plant the magical seed in the rather enormous pot.");
							for(m=0;m<TI_MAX+4;m++)
							{
								if(rooms[room].item[m]==0)
								{
									rooms[room].item[m]=inv[k];
									inv[k]=0;
									return 1;
								}
							}
							return 1;
						}

						for(m=0;m<TI_MAX+4;m++)
						{
							if(rooms[room].item[m]==0)
							{
								rooms[room].item[m]=inv[k];
								inv[k]=0;
								sprintf(s,"You drop %s.",items[i].name);
								TypeLine(s);
								return 1;
							}
						}
					}
				}
				dontHave=1;
			}
	}
	if(dontHave)
		TypeLine("You don't have that!");
	else
		TypeLine("I don't know what you're trying to drop!");
	return 0;
}

byte TextDrink(char *noun)
{
	int i;

	if(noun==NULL)
	{
		TypeLine("Drink what?");
		return 0;
	}
	if(strcmp(noun,"WATER"))
	{
		TypeLine("You can't drink that!");
		return 0;
	}
	if(room==TR_OASIS)
	{
		TypeLine("You take a big ol' slurp from the oasis.  It's very soothing.");
		heat=0;
		return 1;
	}
	if(room==TR_SPRING)
	{
		TypeLine("You gulp down some fresh spring water.  Wow, tastes just like bottled!");
		return 1;
	}
	if(room==TR_PIRATE)
	{
		TypeLine("Just looking into that fetid swamp makes your stomach churn.  No drinking!");
		return 0;
	}
	for(i=0;i<INV_SIZE;i++)
	{
		if(inv[i]==TI_FULLJAR)
		{
			inv[i]=TI_JAR;
			TypeLine("You down the entire jar of water in one gulp.  Aaaah!");
			heat=0;
			strcpy(items[TI_FULLJAR].typeName[0],"");
			strcpy(items[TI_FULLJAR].typeName[1],"");
			strcpy(items[TI_FULLJAR].typeName[2],"");
			strcpy(items[TI_JAR].typeName[0],"JAR");
			strcpy(items[TI_JAR].typeName[1],"GLASS");
			return 0;	// count it as no time passed, just to avoid instantly seeing "you are thirsty"
		}
	}

	TypeLine("You don't have any water to drink!");
	return 0;
}

byte TextFill(char *noun)
{
	int i;

	if(noun==NULL)
	{
		TypeLine("What do you want to fill?");
		return 0;
	}

	if(strcmp(noun,"GLASS") && strcmp(noun,"JAR"))
	{
		TypeLine("You can't fill that.");
		return 0;
	}

	for(i=0;i<INV_SIZE;i++)
	{
		if(inv[i]==TI_FULLJAR)
		{
			TypeLine("Your jar is already full!");
			return 0;
		}
		if(inv[i]==TI_JAR)
		{
			if(room==TR_PIRATE)
			{
				TypeLine("You really don't want this water, seriously.");
				return 0;
			}
			else if(room!=TR_OASIS && room!=TR_SPRING)
			{
				TypeLine("There's nothing here to fill the jar with!");
				return 0;
			}
			else
			{
				TypeLine("You fill the jar with water.");
				if(!filledJar)
				{
					filledJar=1;
					AddScore(2);
				}
				inv[i]=TI_FULLJAR;
				strcpy(items[TI_JAR].typeName[0],"");
				strcpy(items[TI_JAR].typeName[1],"");
				strcpy(items[TI_FULLJAR].typeName[0],"JAR");
				strcpy(items[TI_FULLJAR].typeName[1],"GLASS");
				strcpy(items[TI_FULLJAR].typeName[2],"WATER");
				return 1;
			}
		}
	}

	TypeLine("You don't have a jar to fill!");
	return 0;
}

byte TextHit(char *noun)
{
	int i,j;

	if(noun==NULL)
	{
		TypeLine("You sock yourself in the nose for forgetting to specify a target.");
		return 1;
	}
	for(i=0;i<TI_MAX+4;i++)
	{
		for(j=0;j<4;j++)
			if(!strcmp(items[rooms[room].item[i]].typeName[j],noun))
			{
				if(rooms[room].item[i]==TI_BOULDER)
				{
					if(TextHaveItem(TI_PICK))
					{
						TypeLine("You draw back and smash the pick into the cracked boulder with");
						TypeLine("all your might!  There's a resounding SNAP! and the boulder falls");
						TypeLine("into bits all over the place, revealing a hole in the ground to");
						TypeLine("the North.");
						rooms[room].item[i]=TI_HOLE;
						AddScore(3);
						CopyRoom(TR_HOLEROOM2,TR_HOLEROOM);
						rooms[room].exit[3]=TR_HOLEROOM;
						return 1;
					}
					else
					{
						TypeLine("You give the rock a light slap, and there's no effect.  You're not");
						TypeLine("dumb enough to hit a boulder with your bare hands.");
						return 1;
					}
				}
				if(rooms[room].item[i]==TI_BLOCK)
				{
					if(TextHaveItem(TI_PICK))
					{
						if(cubeExists)
						{
							TypeLine("No need, you already have an ice cube!");
							return 0;
						}
						TypeLine("You crack the block of ice with your pick and a neat little cube");
						TypeLine("of ice chips off into your grasp.  Yow, it's cold.");
						for(i=0;i<INV_SIZE;i++)
						{
							if(inv[i]==0)
							{
								inv[i]=TI_ICE;
								cubeExists=1;
								cubeMelt=0;
								break;
							}
						}
						if(!chippedIce)
						{
							chippedIce=1;
							AddScore(2);
						}
						return 1;
					}
					else
					{
						TypeLine("Smacking the ice with your bare hand only manages to give you");
						TypeLine("a minor case of frostbite.");
						return 1;
					}
				}
				else
				{
					TypeLine("You probably don't want to just go around hitting stuff willy-nilly.");
					return 0;
				}
			}
	}

	TypeLine("I don't know what you are trying to hit!");
	return 0;
}

byte TextTalk(char *noun)
{
	int i,j;

	if(noun==NULL)
	{
		TypeLine("You babble incoherently to no one in particular.");
		return 1;
	}
	for(i=0;i<TI_MAX+4;i++)
	{
		for(j=0;j<4;j++)
			if(!strcmp(items[rooms[room].item[i]].typeName[j],noun))
			{
				if(rooms[room].item[i]==TI_SPHINXTER)
				{
					if(riddle==0)
					{
						TypeLine("Sphinxter says:");
						TypeLine("\"Behold mortal, if you can place the answer to my riddle in my");
						TypeLine("outstretched paw, I shall give you my Key Of Lunacy.  Here goes:");
						TypeLine("");
						TypeLine("  What builds up castles, tears down mountains,");
						TypeLine("     makes some blind, helps others to see?\"");
						riddle=1;
						return 1;
					}
					else if(riddle==1)
					{
						TypeLine("Sphinxter says:");
						TypeLine("\"Got rocks in your head?  I gave you a riddle!  I shall repeat:");
						TypeLine("");
						TypeLine("  What builds up castles, tears down mountains,");
						TypeLine("     makes some blind, helps others to see?");
						TypeLine("");
						TypeLine("Place the answer to my riddle in my paw, and I'll give you my Key Of Lunacy.\"");
						return 1;
					}
					else
					{
						TypeLine("Sphinxter says:");
						TypeLine("\"Good job on that riddle!  Sorry, it's the only one I know, or I'd");
						TypeLine("share some more with you!\"");
						return 1;
					}
				}
				else if(rooms[room].item[i]==TI_SVEN)
				{
					if(yetiSleep)
						TypeLine("Shh!  He's asleep!");
					else
						TypeLine("\"Man, I could sure go for a banana.\"");
					return 1;
				}
				else if(rooms[room].item[i]==TI_BJORN)
				{
					if(yetiSleep)
						TypeLine("Shh!  He's asleep!");
					else
					{
						TypeLine("\"Don't even look funny at our Key Of Lunacy!  We're guarding it for Dr.");
						TypeLine("Lunatic.\"");
					}
					return 1;
				}
				else if(rooms[room].item[i]==TI_OLAF)
				{
					if(yetiSleep)
						TypeLine("Shh!  He's asleep!");
					else
						TypeLine("\"Bananas sure make me sleepy. . . Huh... Why did I say that?\"");
					return 1;
				}
				else if(rooms[room].item[i]==TI_THING)
				{
					TypeLine("\"You don't scare me, little man!  Although ghosts sure do.  But hey,");
					TypeLine("that's neither here nor there.  Point is, you ain't getting my key!\"");
					return 1;
				}
				else if(rooms[room].item[i]==TI_GHOST)
				{
					switch(Random(5))
					{
						case 0:
							TypeLine("\"Ahoy!!!\"");
							break;
						case 1:
							TypeLine("\"YARR!!!\"");
							break;
						case 2:
							TypeLine("\"Avast, landlubber!\"");
							break;
						case 3:
							TypeLine("\"Moan, groan, etc.\"");
							break;
						case 4:
							TypeLine("\"BOOO!!  As long as yer in me cavernous lair, you're mine, bucko!\"");
							break;
					}
					return 1;
				}
				else if(rooms[room].item[i]==TI_MATILDA)
				{
					if(!matildaKey)
					{
						TypeLine("\"Looks like I'm fresh out of candy bars!  If you bring me one, I'll");
						TypeLine("trade you my Key Of Lunacy for it.\"");
						return 1;
					}
					else
					{
						TypeLine("\"Thanks for the candy bar.  You get any more, you bring em my way!\"");
						return 1;
					}
				}
				else if(rooms[room].item[i]==TI_KONGOR)
				{
					if(!gotBananas)
					{
						TypeLine("\"You want banana?  I give much banana, you give five dollar.  Fair trade.\"");
						return 1;
					}
					else
					{
						TypeLine("\"Me go buy Backstreet Boys CD.\"");
						return 1;
					}
				}
				else if(rooms[room].item[i]==TI_LUNATIC)
				{
					TypeLine("\"You'll never catch me, Bouapha!  HAHA!\"");
					return 1;
				}
				else
				{
					TypeLine("You can't talk to that without appearing to be quite odd.");
					return 0;
				}

			}
	}

	TypeLine("I don't know what you want to talk to!");
	return 0;
}

byte TextEat(char *noun)
{
	int i,j;

	if(noun==NULL)
	{
		TypeLine("You sound like a caveman.");
		return 1;
	}
	for(i=0;i<TI_MAX+4;i++)
	{
		for(j=0;j<4;j++)
			if(!strcmp(items[rooms[room].item[i]].typeName[j],noun))
			{
				if(i==TI_BANANA)
				{
					if(room==TR_BANANA)
					{
						TypeLine("Kongor thumps his chest and gives you a dirty look.");
						return 1;
					}
					else
					{
						TypeLine("You eat one from the huge bunch, but there's LOTS more!");
						return 1;
					}
				}
				else if(i==TI_CANDY || i==TI_FROZENCANDY)
				{
					TypeLine("No way, it's got coconut!");
					return 0;
				}
				else
				{
					TypeLine("You can't eat that, and besides, yuck!");
					return 0;
				}
			}
	}
	for(i=0;i<INV_SIZE;i++)
	{
		if(inv[i])
		{
			for(j=0;j<4;j++)
				if(!strcmp(items[inv[i]].typeName[j],noun))
				{
					if(inv[i]==TI_BANANA)
					{
						TypeLine("You eat one from the huge bunch, but there's LOTS more!");
						return 1;
					}
					else if(inv[i]==TI_CANDY || inv[i]==TI_FROZENCANDY)
					{
						TypeLine("No way, it's got coconut!");
						return 0;
					}
					else
					{
						TypeLine("You can't eat that, and besides, yuck!");
						return 0;
					}
				}
		}
	}

	TypeLine("I don't know what you want to eat!");
	return 0;
}

byte TextTie(char *noun)
{
	int i,j;

	if(noun==NULL)
	{
		TypeLine("Tie what?");
		return 1;
	}

	for(i=0;i<TI_MAX;i++)
	{
		for(j=0;j<4;j++)
			if(!strcmp(items[i].typeName[j],noun))
			{
				if(i==TI_HOOK || i==TI_SILK)
				{
					if(TextHaveItem(TI_HOOK) && TextHaveItem(TI_SILK))
					{
						TypeLine("You dextrously knot the silk around the pirate hook,");
						TypeLine("forming a crude set of climbing gear.");
						AddScore(5);
						for(i=0;i<INV_SIZE;i++)
						{
							if(inv[i]==TI_HOOK)
								inv[i]=0;
							if(inv[i]==TI_SILK)
								inv[i]=TI_GEAR;
						}
						return 1;
					}
					else if(i==TI_HOOK)
					{
						TypeLine("You don't have anything to tie it with!");
						return 0;
					}
					else
					{
						TypeLine("You don't have anything to tie it to!");
						return 0;
					}
				}
			}
	}

	TypeLine("I don't know what you want to tie!");
	return 0;
}

byte TextSpray(char *noun)
{
	if(!TextHaveItem(TI_BUGSPRAY))
	{
		TypeLine("You have nothing with spraying capability on hand.");
		return 0;
	}
	if(room!=TR_SPIDERS || bugsSprayed)
	{
		TypeLine("No sense wasting the bug spray when there are no bugs!");
		return 0;
	}

	bugsSprayed=1;
	TypeLine("You spray the bug spray all over the place like mad, while dancing around");
	TypeLine("and stomping and jumping.  In a few minutes, it's all over, and the");
	TypeLine("spiders are history.");
	AddScore(3);
	CopyRoom(TR_SPIDERS2,TR_SPIDERS);
	return 1;
}

byte TextPour(char *noun)
{
	int i,j;

	if(!TextHaveItem(TI_FULLJAR))
	{
		TypeLine("You have nothing to pour.");
		return 0;
	}
	j=0;
	for(i=0;i<TI_MAX+4;i++)
		if(rooms[room].item[i]==TI_SEED)
			j=1;
	if(room!=TR_CHASM || (room==TR_CHASM && !j))
	{
		TypeLine("You dump the water out on the ground.");
		for(i=0;i<INV_SIZE;i++)
		{
			if(inv[i]==TI_FULLJAR)
			{
				inv[i]=TI_JAR;
				strcpy(items[TI_FULLJAR].typeName[0],"");
				strcpy(items[TI_FULLJAR].typeName[1],"");
				strcpy(items[TI_FULLJAR].typeName[2],"");
				strcpy(items[TI_JAR].typeName[0],"JAR");
				strcpy(items[TI_JAR].typeName[1],"GLASS");
			}
		}
		return 1;
	}
	if(room==TR_CHASM && j==1)
	{
		TypeLine("You water the seed.  Nothing happens.  You feel very silly now.");
		TypeLine("");
		TypeLine("Then suddenly, the earth shakes and a giant vine begins to grow out of the");
		TypeLine("pot.  It stretches across the chasm, creating an amazingly convenient");
		TypeLine("bridge.");
		vineGrown=1;
		for(i=0;i<INV_SIZE;i++)
		{
			if(inv[i]==TI_FULLJAR)
			{
				inv[i]=TI_JAR;
				strcpy(items[TI_FULLJAR].typeName[0],"");
				strcpy(items[TI_FULLJAR].typeName[1],"");
				strcpy(items[TI_FULLJAR].typeName[2],"");
				strcpy(items[TI_JAR].typeName[0],"JAR");
				strcpy(items[TI_JAR].typeName[1],"GLASS");
			}
		}
		for(i=0;i<TI_MAX+4;i++)
		{
			if(rooms[room].item[i]==TI_SEED)
				rooms[room].item[i]=0;
			if(rooms[room].item[i]==TI_POT)
				rooms[room].item[i]=TI_VINE;
		}
		AddScore(2);
		return 1;
	}

	return 0;
}

byte HandleInput(void)
{
	char *verb,*noun,*extra;
	char tmpInput[MAX_LINELEN];
	int i;

	strcpy(tmpInput,inputTxt);
	verb=strtok(tmpInput," ");
	noun=strtok(NULL," ");
	extra=strtok(NULL," ");
	if(extra)
	{
		EchoInput();
		TypeLine("I only understand 2-word sentences! Cut me some slack, I'm only a computer!");
		return 0;
	}
	if(!verb)
	{
		EchoInput();
		TypeLine("Eh?  Wha?!");
		return 0;
	}
	if(!strcmp(verb,"G"))
	{
		strcpy(inputTxt,lastInput);
		HandleInput();
	}
	else if(!strcmp(verb,"E") || !strcmp(verb,"EAST") ||
			(!strcmp(verb,"GO") && noun && (!strcmp(noun,"E") || !strcmp(noun,"EAST"))))
	{
		Move(0);
	}
	else if(!strcmp(verb,"S") || !strcmp(verb,"SOUTH") ||
			(!strcmp(verb,"GO") && noun && (!strcmp(noun,"S") || !strcmp(noun,"SOUTH"))))
	{
		Move(1);
	}
	else if(!strcmp(verb,"W") || !strcmp(verb,"WEST") ||
			(!strcmp(verb,"GO") && noun && (!strcmp(noun,"W") || !strcmp(noun,"WEST"))))
	{
		Move(2);
	}
	else if(!strcmp(verb,"N") || !strcmp(verb,"NORTH") ||
			(!strcmp(verb,"GO") && noun && (!strcmp(noun,"N") || !strcmp(noun,"NORTH"))))
	{
		Move(3);
	}
	else if(!strcmp(verb,"Q") || !strcmp(verb,"QUIT"))
	{
		EchoInput();
		TypeLine("Try pressing ESC.  QUITTER!");
		return 0;
	}
	else if(!strcmp(verb,"I") || !strcmp(verb,"INVENTORY"))
	{
		EchoInput();
		ListInventory();
		return 0;
	}
	else if(!strcmp(verb,"GET") || !strcmp(verb,"TAKE"))
	{
		EchoInput();
		return TextGetItem(noun);
	}
	else if(!strcmp(verb,"DROP") || !strcmp(verb,"PUT") || !strcmp(verb,"GIVE"))
	{
		EchoInput();
		return TextDropItem(noun);
	}
	else if(!strcmp(verb,"L") || !strcmp(verb,"LOOK"))
	{
		EchoInput();
		TextLook(noun);
		return 0;
	}
	else if(!strcmp(verb,"DRINK"))
	{
		EchoInput();
		return TextDrink(noun);
	}
	else if(!strcmp(verb,"FILL"))
	{
		EchoInput();
		return TextFill(noun);
	}
	else if(!strcmp(verb,"HIT"))
	{
		EchoInput();
		return TextHit(noun);
	}
	else if(!strcmp(verb,"TALK"))
	{
		EchoInput();
		return TextTalk(noun);
	}
	else if(!strcmp(verb,"TIE"))
	{
		EchoInput();
		return TextTie(noun);
	}
	else if(!strcmp(verb,"EAT"))
	{
		EchoInput();
		return TextEat(noun);
	}
	else if(!strcmp(verb,"SPRAY"))
	{
		EchoInput();
		return TextSpray(noun);
	}
	else if(!strcmp(verb,"POUR"))
	{
		EchoInput();
		return TextPour(noun);
	}
	else if(!strcmp(verb,"Z") || !strcmp(verb,"WAIT"))
	{
		EchoInput();
		TypeLine("You wait a little while...");
		return 1;
	}
	else if(!strcmp(verb,"VERBS"))
	{
		EchoInput();
		TypeLine("I know the following verbs:");
		TypeLine("Go, Take, Get, Put, Drop, Give, Fill, Pour, Drink, Eat, Hit, Talk,");
		TypeLine("Tie, Spray, Wait.");
		return 0;
	}
	else if(!strcmp(verb,"HELP"))
	{
		EchoInput();
		TypeLine("How To Play:");
		for(i=0;i<HELPLEN;i++)
			TypeLine(helpTxt[i]);
		return 0;
	}
	else if(!strcmp(verb,"SCORE"))
	{
		EchoInput();
		ShowScore();
		return 0;
	}
	else
	{
		EchoInput();
		TypeLine("I have NO idea what you're talking about.  Type 'VERBS' for a list of all");
		TypeLine("the verbs I can understand, or 'HELP' for general help.");
		return 0;
	}
	return 1;
}

void InitTextGame(MGLDraw *mgl)
{
	int i;

	mgl->LoadBMP("graphics/profmenu.bmp");
	backgd=(byte *)malloc(640*480);

	for(i=0;i<480;i++)
		memcpy(&backgd[i*640],&mgl->GetScreen()[i*mgl->GetWidth()],640);
	for(i=0;i<MAX_LINES;i++)
		textbuf[i][0]='\0';

	strcpy(lastInput,"");
	TypeLine("Bouapha's Text Adventure!");
	TypeLine("");
	TypeLine("Type 'HELP' for instructions.");
	TypeLine("");
	room=TR_ASYLDOOR;
	heat=0;
	riddle=0;
	yetiSleep=0;
	score=0;
	oasis=0;
	filledJar=0;
	usedBoots=0;
	climbedPeak=0;
	openedAsylum=0;
	cubeMelt=0;
	candyMelt=0;
	frozenBar=0;
	chippedIce=0;
	matildaKey=0;
	gotBananas=0;
	haunted=0;
	scaredThing=0;
	bugsSprayed=0;
	silkYet=0;
	lunaticCaught=0;
	vineGrown=0;
	cubeExists=0;
	for(i=0;i<INV_SIZE;i++)
		inv[i]=TI_NONE;

	memcpy(items,baseItems,sizeof(txtItem_t)*TI_MAX);
	memcpy(rooms,baseRooms,sizeof(txtRoom_t)*TR_MAX);

	DescribeRoom();

	lastGamepad = ~0;
	gamepadMode = false;
	GetTaps();
}

void ExitTextGame(void)
{
	free(backgd);
}

void TimePassed(void)
{
	int i;

	if(room>=TR_DESERT2 && room<=TR_DESERT5)
	{
		heat++;
		switch(heat)
		{
			case 1:
				TypeLine("");
				TypeLine("You're getting really thirsty.");
				break;
			case 2:
				TypeLine("");
				TypeLine("You're dying of thirst in this heat!");
				break;
			case 3:
				TypeLine("");
				TypeLine("If you don't drink something right now, you're toast.");
				break;
			case 4:
				TypeLine("");
				TypeLine("In a heat-induced stupor, you stumble around aimlessly, parched.");
				TypeLine("Eventually you collapse, and awaken later to find yourself on...");
				TypeLine("");
				room=TR_DESERT;
				DescribeRoom();
				heat=0;
				break;
		}
	}
	else
		heat=0;

	if(room==TR_ASYLUM && !lunaticCaught)
	{
		TypeLine("");
		TypeLine("Dr. Lunatic sprints all around the room at ridiculous speed.");
	}
	if(room==TR_SPHINX)
	{
		switch(Random(7))
		{
			case 1:
				TypeLine("");
				TypeLine("Sphinxter sneezes from all the dust in the air.  You barely escape alive when");
				TypeLine("giant rocks come tumbling from his huge nostrils and cascade all over the");
				TypeLine("landscape.");
				break;
			case 2:
				TypeLine("");
				TypeLine("Sphinxter glares at you in a sphinxy way.");
				break;
			case 3:
				TypeLine("");
				TypeLine("Sphinxter appears to be measuring you up for a quick squishing.");
				break;
		}
	}

	if(room==TR_YETI && yetiSleep==0)
	{
		switch(Random(7))
		{
			case 1:
				TypeLine("");
				TypeLine("The yetis argue about who loves bananas the most.");
				break;
			case 2:
				TypeLine("");
				TypeLine("Olaf warms his paws by the fire.");
				break;
			case 3:
				TypeLine("");
				TypeLine("Sven tells Bjorn to scoot over a little.");
				break;
		}
	}
	if(room==TR_YETI && yetiSleep==1)
	{
		switch(Random(7))
		{
			case 1:
				TypeLine("");
				TypeLine("Olaf rolls over in his sleep.");
				break;
			case 2:
				TypeLine("");
				TypeLine("Sven mumbles something in his sleep.");
				break;
			case 3:
				TypeLine("");
				TypeLine("Bjorn snores like a sleeping yeti.");
				break;
			case 4:
				TypeLine("");
				TypeLine("The brothers are sleeping soundly.");
				break;
		}
	}

	if(TextHaveItem(TI_FROZENCANDY))
	{
		if(heat>0)
		{
			candyMelt=0;
			for(i=0;i<INV_SIZE;i++)
			{
				if(inv[i]==TI_FROZENCANDY)
					inv[i]=TI_CANDY;
			}
			TypeLine("");
			TypeLine("Your candy bar has thawed.  You're lucky it didn't completely melt!");
		}
		else if(room!=TR_LEDGE && !TextHaveItem(TI_ICE))
		{
			candyMelt++;
			if(candyMelt<5)
			{
				TypeLine("");
				TypeLine("Your candy bar is thawing.");
			}
			else
			{
				TypeLine("");
				TypeLine("Your candy bar is now completely thawed.");
				candyMelt=0;
				for(i=0;i<INV_SIZE;i++)
				{
					if(inv[i]==TI_FROZENCANDY)
						inv[i]=TI_CANDY;
				}
			}
		}
	}
	if(TextHaveItem(TI_ICE))
	{
		if(heat>0)
		{
			cubeMelt=0;
			for(i=0;i<INV_SIZE;i++)
			{
				if(inv[i]==TI_ICE)
					inv[i]=0;
			}
			TypeLine("");
			TypeLine("Your ice cube has evaporated into nothingness in the scorching heat.");
			cubeExists=0;
		}
		else if(room!=TR_LEDGE)
		{
			cubeMelt++;
			if(cubeMelt<4)
			{
				TypeLine("");
				TypeLine("Your ice cube is melting.");
			}
			else
			{
				TypeLine("");
				TypeLine("Your ice cube has melted into a puddle on the floor.");
				cubeMelt=0;
				cubeExists=0;
				for(i=0;i<INV_SIZE;i++)
				{
					if(inv[i]==TI_ICE)
						inv[i]=0;
				}
			}
		}
	}

	for(i=0;i<TI_MAX+4;i++)
	{
		if(rooms[room].item[i]==TI_GHOST)
		{
			switch(Random(7))
			{
				case 0:
					TypeLine("");
					TypeLine("The ghost capers about.");
					break;
				case 1:
					TypeLine("");
					TypeLine("The ghost moans about the old days at sea.");
					break;
				case 2:
					TypeLine("");
					TypeLine("The ghost laments the ease with which piracy can be committed in this");
					TypeLine("digital age.");
					break;
				case 3:
					TypeLine("");
					TypeLine("The ghost makes spooky faces at you.");
					break;
			}
		}
	}
}

byte UpdateTextGame(int *lastTime,MGLDraw *mgl)
{
	char c = mgl->LastKeyPressed();
	dword gamepad = GetGamepadButtons();
	byte taps = GetTaps();

	if (gamepad & ~lastGamepad & (1 << SDL_CONTROLLER_BUTTON_A))
	{
		if (curRow == 2 && curCol == 9)
		{
			c = SDLK_BACKSPACE;
		}
		else if (curRow == 3 && curCol == 8)
		{
			c = SDLK_RETURN;
		}
		else if (KEYBOARD[curRow][curCol] != '~')
		{
			c = KEYBOARD[curRow][curCol];
		}
	}
	if (taps & CONTROL_LF)
	{
		if (curCol > 0)
			--curCol;
	}
	if (taps & CONTROL_RT)
	{
		if (curCol < KEYBOARD_WIDTH - 1)
			++curCol;
	}
	if (taps & CONTROL_UP)
	{
		if (curRow > 0)
			--curRow;
	}
	if (taps & CONTROL_DN)
	{
		if (curRow < KEYBOARD_HEIGHT - 1)
			++curRow;
	}
	if (curRow == 3 && curCol == 9)
		curCol = 8;

	if (gamepad & ~lastGamepad & (1 << SDL_CONTROLLER_BUTTON_X))
		mgl->StartTextInput(35, 440, 640-35, 440+28);
	if (gamepad)
		gamepadMode = true;

	if(c == SDLK_ESCAPE || (gamepad & ~lastGamepad & (1 << SDL_CONTROLLER_BUTTON_BACK)))
		return 1;

	if(c == SDLK_RETURN || (gamepad & ~lastGamepad & (1 << SDL_CONTROLLER_BUTTON_Y)))	// enter
	{
		MakeNormalSound(SND_MENUSELECT);
		if(lunaticCaught)
		{
			TypeLine("You've won the game!  Press ESC, you fool!");
		}
		else
		{
			if(HandleInput())
				TimePassed();
			mgl->StartTextInput(35, 440, 640-35, 440+28);
		}

		strcpy(lastInput,inputTxt);
		inputTxt[0]='\0';
	}
	else if((c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9') || c==' ')
	{
		if((c>='a' && c<='z'))
			c+='A'-'a';
		if(strlen(inputTxt)<50)
		{
			inputTxt[strlen(inputTxt)+1]='\0';
			inputTxt[strlen(inputTxt)]=c;
			MakeNormalSound(SND_MENUCLICK);
		}
	}
	else if(c == SDLK_BACKSPACE || (gamepad & ~lastGamepad & (1 << SDL_CONTROLLER_BUTTON_B)))
	{
		if(strlen(inputTxt)>0)
		{
			inputTxt[strlen(inputTxt)-1]='\0';
			MakeNormalSound(SND_MENUCLICK);
		}
	}

	if(*lastTime>TIME_PER_FRAME*5)
		*lastTime=TIME_PER_FRAME*5;
	while(*lastTime>=TIME_PER_FRAME)
	{
		mgl->Process();
		*lastTime-=TIME_PER_FRAME;
	}

	lastGamepad = gamepad;
	return 0;
}

void RenderTextGame(MGLDraw *mgl)
{
	int i;
	static byte flash=0;

	bool showGamepadTyper = gamepadMode && !SDL_HasScreenKeyboardSupport();

	flash=1-flash;

	for(i=0;i<480;i++)
		memcpy(&mgl->GetScreen()[i*mgl->GetWidth()],&backgd[i*640],640);

	for (i = (showGamepadTyper ? 4 : 0); i<MAX_LINES; i++)
		PrintGlow(20,20+i*20,textbuf[i],0,2);

	PrintGlow(20,440,"> ",0,2);
	if(flash)
	{
		inputTxt[strlen(inputTxt)+1]='\0';
		inputTxt[strlen(inputTxt)]='*';
	}
	PrintGlow(35,440,inputTxt,0,2);
	if(flash)
		inputTxt[strlen(inputTxt)-1]='\0';

	if (showGamepadTyper)
	{
		for (int row = 0; row < KEYBOARD_HEIGHT; ++row)
		{
			for (int col = 0; col < KEYBOARD_WIDTH; ++col)
			{
				char ch = KEYBOARD[row][col];
				std::string_view msg;
				int w = 40-4;
				if (ch == ' ')
				{
					msg = "__";
				}
				else if (ch != '~')
				{
					msg = { &ch, 1 };
				}
				else if (row == 2)
				{
					msg = "Del";
				}
				else if (row == 3 && col == 8)
				{
					msg = "Enter";
					w = 80-4;
				}

				if (!msg.empty())
				{
					int x = 100 + col * 40, y = 10 + row * 23;
					if (curRow == row && curCol == col)
					{
						mgl->FillBox(x, y, x+w, y+20, 32+8);
						mgl->Box(x, y, x+w, y+20, 32+31);
					}
					else
						mgl->Box(x, y, x+w, y+20, 32+16);

					PrintGlow(x + (w - GetStrLength(msg, 2)) / 2, y + 2, msg, 0, 2);
				}
			}
		}
	}
}

TASK(void) TextGame(MGLDraw *mgl)
{
	byte done=0;
	int lastTime;

	InitTextGame(mgl);
	SDL_SetHint(SDL_HINT_RETURN_KEY_HIDES_IME, "0");
	mgl->StartTextInput(35, 440, 640-35, 440+28);

	lastTime=1;
	while(!done)
	{
		lastTime+=TimeLength();
		StartClock();
		done=UpdateTextGame(&lastTime,mgl);
		RenderTextGame(mgl);
		AWAIT mgl->Flip();

		if(!mgl->Process())
			done=1;
		EndClock();
	}

	ExitTextGame();
}

/* scoring

  100 total

  5		get to oasis
  10	answer riddle
  3		smash rock
  2		filling jar the first time
  5		ditch ghost in desert
  10	scare the thing
  10	put yetis to sleep
  10	bribe matilda
  3		get a banana
  2		freeze candy bar
  2		chip ice
  3		spray bugs
  5		make climbing gear
  5		buy candy bar
  3		wearing boots
  2		climbed peak
  8		opened asylum
  10	catch lunatic
  2		grew vine
  --
  100
  */

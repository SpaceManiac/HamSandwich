#include "title.h"
#include "game.h"
#include "jamulfmv.h"
#include "pause.h"
#include "options.h"
#include "pause.h"
#include "plasma.h"
#include "palettes.h"

// special codes in the credits:
// @ = use GirlsRWeird font
// # = draw a major horizontal line
// % = draw a minor horizontal line
// $ = last line of the whole deal
char credits[][32]={
	"@LoonyLand:",
	"",
	"@Halloween Hill",
	"",
	"",
	"Copyright 2004, Hamumu Software",
	"#",
	"Original Concept",
	"Mike Hommel",
	"%",
	"Programming",
	"Mike Hommel",
	"%",
	"Character Design",
	"Mike Hommel",
	"%",
	"Level Design",
	"Mike Hommel",
	"%",
	"3D Graphics",
	"Mike Hommel",
	"%",
	"2D Graphics",
	"Mark Stevens",
	"Mike Hommel",
	"%",
	"Sound Effects",
	"Solange Hunt",
	"Mike Hommel",
	"%",
	"\"Music\"",
	"Mike Hommel",
	"%",
	"Testing",
	"Eric Arevalo",
	"Solange Hunt",
	"BJ McRae",
	"Barry Pennington",
	"Mark Stevens",
	"Carol \"Baba\" VanderMolen",
	"%",
	"Muse",
	"Solange Hunt",
	"%",
	"This game goes out as",
	"a special thanks to the",
	"Dr. Lunatic fans!!",
	"Keep me fed, and I'll keep",
	"making these things...",
	"",
	"#",
	"Stop by www.hamumu.com!",
	"","","","","","","","","","","","","","","","","","","",
	"P.S.... watch for",
	"Loonyland: Insanity",
	"and",
	"Sol Hunt",
	"coming soon!!",
	"$"
	};

char victoryTxt[][64]={
	"",
	"",
	"After a little time to rest and wipe off a whole",
	"",
	"lot of mud, Loony got back to his feet and returned",
	"",
	"to Luniton, a hero at last.",
	"",
	"",
	"",
	"#",
	"",
	"",
	"All around the land of Halloween Hill, life was",
	"",
	"returning to normal.  The frogs stopped licking",
	"",
	"so viciously, the mummies settled down and returned",
	"",
	"to their work in the toilet paper industry - Even",
	"",
	"the werewolves went back to their den to take a nap.",
	"",
	"Yes, everything was back to being as peaceful as it",
	"",
	"gets in Halloween Hill.  Which, by the way, is not",
	"",
	"terribly peaceful.",
	"",
	"",
	"",
	"",
	"",
	"#",
	"",
	"",
	"There was just one thing wrong...",
	"",
	"The trees in the south of Wicked Woods were still",
	"",
	"hung with stick figures.  Almost as if the evil",
	"",
	"presence there hadn't been the result of the",
	"",
	"Evilizer at all... and was still lurking somewhere.",
	"$"
	};

char cheatTxt[][64]={
	"@Congratulations on getting",
	"",
	"",
	"@all 40 Merit Badges!",
	"",
	"",
	"#",
	"So since you've proven you don't need them,",
	"here are the cheat codes to the game!",
	"",
	"These ones are to be typed in while playing:",
	"",
	"%",
	"SUPERBOOM",
	"Causes tremendous explosions to wipe out enemies",
	"",
	"%",
	"GETALIFE",
	"Heals you to your full health",
	"",
	"%",
	"BANGZOOM",
	"Renders you invulnerable for a long time",
	"",
	"%",
	"GOGOGO",
	"Gives you double speed for a while",
	"",
	"",
	"#",
	"These cheats must be entered in the Badges screen,",
	"while holding down both fire buttons:",
	"",
	"%",
	"GIMME",
	"Gives you the merit badge you have highlighted",
	"If you already have it, it removes it instead",
	"",
	"%",
	"GIVEALL",
	"Gives you every merit badge",
	"",
	"%",
	"CLEAR",
	"Removes all merit badges, letting you make a fresh",
	"start if you want to earn them all again - also",
	"marks all bosses un-beaten, for Boss Bash mode",
	"",
	"%",
	"BOSSBASH",
	"Mark all bosses beaten for purposes of playing Boss",
	"Bash mode",
	"",
	"#",
	"",
	"Oh, and one other thing... start a new game as",
	"Loony or Werewolf Loony for a special surprise!",
	"",
	"#",
	"",
	"@Thank you for playing Loonyland!",
	"",
	"",
	"@Look for more good stuff coming soon at",
	"",
	"",
	"@WWW.HAMUMU.COM",
	"$",
};

// once the credits have scrolled to END_OF_CREDITS pixels, they end
#define END_OF_CREDITS 480*4-330
#define END_OF_VICTORY 480*2
#define END_OF_CHEATS 480*3-100

// the most custom worlds it will handle
#define MAX_CUSTOM (64-5)

#define VERSION_NO	"Version 2.0"

sprite_set_t *planetSpr;
static int numRunsToMakeUp;

static byte oldc=0;

static byte keyAnim=0;
char lvlName[32];
static byte cursor;
byte *backScr;
int numRuns;
byte demoTextCounter;
byte songRestart;

static byte loadingGame;

typedef struct menu_t
{
	char txt[64];
	byte known;
	char bright;
} menu_t;

menu_t menu[MENU_CHOICES]={
	{"New Game",1,-32},
	{"Load Game",1,-32},
	{"Bowling",0,-32},
	{"Survival",0,-32},
	{"Boss Bash",0,-32},
	{"Loony Ball",0,-32},
	{"Remix",0,-32},
	{"Badges",0,-32},
	{"Hi Scores",1,-32},
	{"Options",1,-32},
	{"Editor",1,-32},
	{"Exit",1,-32},
	{"!",0,-32}
};

menu_t saves[5];
byte choosingDiffFor;

byte HandleTitleKeys(MGLDraw *mgl)
{
	char k;

	k=mgl->LastKeyPressed();

	if(k=='e')
		return 1;	// go to the editor
	if(k==27)
		return 2;	// exit
	else
		return 0;	// play the game
}

void SetSongRestart(byte b)
{
	songRestart=b;
}

byte WhichGameToLoad(void)
{
	return cursor;
}

void GetSavesForMenu(void)
{
	FILE *f;
	int i;
	char txt[64];
	float pct;
	player_t p;

	for(i=0;i<5;i++)
	{
		sprintf(txt,"save%d.sav",i+1);
		f=fopen(txt,"rb");
		if(!f)
		{
			pct=0.0;
			sprintf(txt,"Unused");
			saves[i].known=0;
		}
		else
		{
			fread(&p,sizeof(player_t),1,f);
			fclose(f);
			pct=CalcPercent(&p);
			if(p.worldNum==WORLD_NORMAL)
				strcpy(txt,p.areaName);
			else
				sprintf(txt,"*%s",p.areaName);
			saves[i].known=1;
		}
		sprintf(saves[i].txt,"%s - %0.1f%%",txt,pct);
		saves[i].bright=-32;
	}
}

byte LunaticTitle(MGLDraw *mgl)
{
	mgl->LoadBMP("graphics/title.bmp");
	mgl->Flip();
	while(!mgl->LastKeyPeek())
	{
		if(!mgl->Process())
			return 2;
		mgl->Flip();
	}
	return HandleTitleKeys(mgl);
}

void ShowMenuOption(int x,int y,menu_t m,byte on)
{
	int i;

	if(m.known)
	{
		PrintGlow(x,y,m.txt,m.bright,2);
		if(on && m.bright>-20)
		{
			PrintGlow(x-4+Random(9),y-4+Random(9),m.txt,-20,2);
		}
	}
	else // if this option is unknown
	{
		// bouncy question marks!
		if(on)
		{
			for(i=0;i<8;i++)
			{
				PrintGlow(x+i*16-2+Random(5),y-2+Random(5),"?",m.bright,2);
			}
		}
		else
		{
			PrintGlow(x,y,"????????",m.bright,2);
		}
	}
}

void ShowSavedGame(int x,int y,menu_t m,byte on)
{
	PrintGlow(x,y,m.txt,m.bright,0);
	if(on && m.bright>-20)
	{
		PrintGlow(x-2+Random(5),y-2+Random(5),m.txt,-20,0);
	}
}

void MainMenuDisplay(MGLDraw *mgl)
{
	int i;
	int x,y;

	for(i=0;i<480;i++)
		memcpy(mgl->GetScreen()+mgl->GetWidth()*i,&backScr[i*640],640);

	// version #:
	Print(555,3,VERSION_NO,1,1);
	Print(554,2,VERSION_NO,0,1);
	// Copyright:
	PrintGlow(395,467,"Copyright 2004, Hamumu Software",-10,1);

	// menu options
	x=5;
	y=320;
	for(i=0;i<MENU_CHOICES;i++)
	{
		if(menu[i].txt[0]!='!')
			ShowMenuOption(x,y-menu[i].bright/4,menu[i],(cursor==i));
		x+=160;
		if(x>640-150)
		{
			x=5;
			y+=45;
		}
	}
}

void DiffChooseDisplay(MGLDraw *mgl)
{
	int i;

	char diffName[5][16]={"Beginner","Normal","Challenge","Mad","Loony"};

	char diffDesc[][128]={
		// beginner
		"Enemies never do more than 1 Heart of damage, and move slower than normal.  You",
		"begin with 15 Hearts, and do extra damage.  Enemies drop more items than normal.",
		"",
		// normal
		"The standard difficulty.  Good for someone who has an idea of what they're doing!",
		"You begin with 10 Hearts.",
		"",
		// challenge
		"If you're looking for a bigger challenge, this is it!  Enemies do more damage, and",
		"you do less. You begin with 5 Hearts, and enemies drop fewer items.",
		"",
		// mad
		"A special difficulty to inject more skillful play into the late-game, although it makes",
		"the early going very tough.  Only for Loonyland aficionados.  You begin with 3 Hearts",
		"and enemies are much tougher to beat.",
		// Loony
		"For masochists.  Enemies do double damage, you do 1/4 damage, you begin with ONE",
		"Heart, poison is twice as deadly, enemies move faster, and enemies only drop items",
		"when Combo'd!"
	};

	for(i=0;i<480;i++)
		memcpy(mgl->GetScreen()+mgl->GetWidth()*i,&backScr[i*640],640);

	// version #:
	Print(560,3,VERSION_NO,1,1);
	Print(559,2,VERSION_NO,0,1);

	PrintGlow(5,330,"Difficulty Level:",0,2);

	if(opt.difficulty>0)
		PrintGlow(280,330,"<<<",0,2);

	CenterPrintGlow(440,330,diffName[opt.difficulty],0,2);
	CenterPrintGlow(440-4+Random(9),330-4+Random(9),diffName[opt.difficulty],-20,2);

	if(opt.difficulty<4)
		PrintGlow(560,330,">>>",0,2);

	PrintGlow(5,450,"The chosen difficulty applies to all game modes.  You can change it at any time",0,1);
	PrintGlow(5,465,"in the Options menu on the title screen, but that doesn't affect saved games.",0,1);

	PrintGlow(310,318,"Select with arrow keys and press Enter",0,1);
	for(i=0;i<3;i++)
	{
		PrintGlow(5,390+i*15,diffDesc[opt.difficulty*3+i],0,1);
	}
}


void LoadGameDisplay(MGLDraw *mgl)
{
	int i;
	int x,y;

	for(i=0;i<480;i++)
		memcpy(mgl->GetScreen()+mgl->GetWidth()*i,&backScr[i*640],640);

	// version #:
	Print(560,3,VERSION_NO,1,1);
	Print(559,2,VERSION_NO,0,1);
	// Copyright:
	PrintGlow(395,467,"Copyright 2004, Hamumu Software",-10,1);

	PrintGlow(5,360,"Select a game to load",0,0);
	PrintGlow(5,390,"Or press ESC to cancel",0,0);
	// menu options
	x=320;
	y=320;
	for(i=0;i<5;i++)
	{
		ShowSavedGame(x,y-menu[i].bright/4,saves[i],(cursor==i));
		y+=26;
	}
}

byte MainMenuUpdate(int *lastTime,MGLDraw *mgl)
{
	byte c;
	static byte reptCounter=0;
	int i;

	if(*lastTime>TIME_PER_FRAME*30)
		*lastTime=TIME_PER_FRAME*30;

	while(*lastTime>=TIME_PER_FRAME)
	{
		for(i=0;i<MENU_CHOICES;i++)
		{
			if(cursor==i)
			{
				if(menu[i].bright<0)
					menu[i].bright++;
			}
			else
			{
				if(menu[i].bright>-23)
					menu[i].bright--;
				else if(menu[i].bright<-23)
					menu[i].bright++;
			}
		}

		// now real updating
		c=GetControls()|GetArrows();

		if((c&CONTROL_UP) && !(oldc&CONTROL_UP))
		{
			cursor-=4;
			if(cursor>=MENU_CHOICES)
				cursor+=MENU_CHOICES;
			if(menu[cursor].txt[0]=='!')
			{
				cursor-=4;
				if(cursor>=MENU_CHOICES)
					cursor+=MENU_CHOICES;
			}
			MakeNormalSound(SND_MENUCLICK);
		}
		if((c&CONTROL_DN) && !(oldc&CONTROL_DN))
		{
			cursor+=4;
			if(cursor>=MENU_CHOICES)
				cursor-=MENU_CHOICES;
			if(menu[cursor].txt[0]=='!')
			{
				cursor-=8;
				if(cursor>=MENU_CHOICES)
					cursor-=MENU_CHOICES;
			}
			MakeNormalSound(SND_MENUCLICK);
		}
		if((c&CONTROL_LF) && !(oldc&CONTROL_LF))
		{
			cursor--;
			if(cursor>=MENU_CHOICES)
				cursor+=MENU_CHOICES;
			if(cursor%4==3)
			{
				cursor+=4;
				if(cursor>=MENU_CHOICES)
					cursor-=MENU_CHOICES;
			}
			if(menu[cursor].txt[0]=='!')
			{
				cursor--;
			}
			MakeNormalSound(SND_MENUCLICK);
		}
		if((c&CONTROL_RT) && !(oldc&CONTROL_RT))
		{
			cursor++;
			if(cursor>=MENU_CHOICES)
				cursor-=MENU_CHOICES;
			if(cursor%4==0)
			{
				cursor-=4;
				if(cursor>=MENU_CHOICES)
					cursor+=MENU_CHOICES;
			}
			if(menu[cursor].txt[0]=='!')
			{
				cursor-=3;
			}
			MakeNormalSound(SND_MENUCLICK);
		}

		if((c&(CONTROL_B1|CONTROL_B2)) && !(oldc&(CONTROL_B1|CONTROL_B2)))
		{
			if(menu[cursor].known)
			{
				MakeNormalSound(SND_MENUSELECT);
				return cursor+1;
			}
			else
				MakeNormalSound(SND_MENUCANCEL);
		}

		if(c)	// hitting any key prevents credits
			numRuns=0;

		oldc=c;

		c=mgl->LastKeyPressed();
		if(c==27)
			return MENU_EXIT+1;
#ifdef _DEBUG
		if(c=='e')
			return MENU_EDITOR+1;
#endif
		*lastTime-=TIME_PER_FRAME;
		numRuns++;
	}
	return 0;
}

byte LoadGameUpdate(int *lastTime,MGLDraw *mgl)
{
	byte c;
	int i;

	if(*lastTime>TIME_PER_FRAME*30)
		*lastTime=TIME_PER_FRAME*30;

	while(*lastTime>=TIME_PER_FRAME)
	{
		for(i=0;i<5;i++)
		{
			if(cursor==i)
			{
				if(saves[i].bright<0)
					saves[i].bright++;
			}
			else
			{
				if(saves[i].bright>-23)
					saves[i].bright--;
				else if(saves[i].bright<-23)
					saves[i].bright++;
			}
		}

		// now real updating
		c=GetControls()|GetArrows();

		if((c&CONTROL_UP) && !(oldc&CONTROL_UP))
		{
			cursor--;
			if(cursor>4)
				cursor=4;
			MakeNormalSound(SND_MENUCLICK);
		}
		if((c&CONTROL_DN) && !(oldc&CONTROL_DN))
		{
			cursor++;
			if(cursor>4)
				cursor=0;
			MakeNormalSound(SND_MENUCLICK);
		}

		if((c&(CONTROL_B1|CONTROL_B2)) && !(oldc&(CONTROL_B1|CONTROL_B2)))
		{
			if(saves[cursor].known)
			{
				MakeNormalSound(SND_MENUSELECT);
				return 2;
			}
			else
				MakeNormalSound(SND_MENUCANCEL);
		}

		// no credits while sitting here
		numRuns=0;

		oldc=c;

		c=mgl->LastKeyPressed();
		if(c==27)
		{
			MakeNormalSound(SND_MENUCANCEL);
			return 0;
		}

		*lastTime-=TIME_PER_FRAME;
		numRuns++;
	}
	return 1;
}

byte ChooseDiffUpdate(int *lastTime,MGLDraw *mgl)
{
	byte c;

	if(*lastTime>TIME_PER_FRAME*30)
		*lastTime=TIME_PER_FRAME*30;

	while(*lastTime>=TIME_PER_FRAME)
	{
		// now real updating
		c=GetControls()|GetArrows();

		if((c&CONTROL_LF) && !(oldc&CONTROL_LF))
		{
			if(opt.difficulty>0)
				opt.difficulty--;
			MakeNormalSound(SND_MENUCLICK);
		}
		if((c&CONTROL_RT) && !(oldc&CONTROL_RT))
		{
			if(opt.difficulty<4)
				opt.difficulty++;
			MakeNormalSound(SND_MENUCLICK);
		}

		if((c&(CONTROL_B1|CONTROL_B2)) && !(oldc&(CONTROL_B1|CONTROL_B2)))
		{
			MakeNormalSound(SND_MENUSELECT);
			SaveOptions();
			return 2;
		}

		// no credits while sitting here
		numRuns=0;

		oldc=c;

		c=mgl->LastKeyPressed();
		if(c==27)
		{
			MakeNormalSound(SND_MENUCANCEL);
			return 0;
		}

		*lastTime-=TIME_PER_FRAME;
		numRuns++;
	}
	return 1;
}

byte MainMenu(MGLDraw *mgl)
{
	byte b=0;
	int i;
	int lastTime=1;

	if(songRestart)
	{
		JamulSoundPurge();
		LoopingSound(SND_HAMUMU);
	}

	for(i=0;i<MENU_CHOICES;i++)
		menu[i].bright=-32;

	mgl->LoadBMP("graphics/title.bmp");
	if(opt.cheats[CH_VINTAGE])
		GreyPalette(mgl);
	mgl->LastKeyPressed();
	oldc=255;//CONTROL_B1|CONTROL_B2;

	backScr=(byte *)malloc(640*480);
	if(!backScr)
		FatalError("Out of memory!");

	for(i=0;i<480;i++)
		memcpy(&backScr[i*640],mgl->GetScreen()+mgl->GetWidth()*i,640);

	if(opt.modes[4])
		menu[MENU_BADGES].known=1;
	if(opt.modes[3])
		menu[MENU_BOWLING].known=1;
	if(opt.modes[2])
		menu[MENU_LOONYBALL].known=1;
	if(opt.modes[1])
		menu[MENU_BOSSATTACK].known=1;
	if(opt.modes[0])
		menu[MENU_SURVIVAL].known=1;
	if(opt.expando[0])
		menu[MENU_REMIX].known=1;

	cursor=0;
	loadingGame=0;
	GetTaps();
	while(b==0)
	{
		lastTime+=TimeLength();
		StartClock();

		if(loadingGame==0)
		{
			b=MainMenuUpdate(&lastTime,mgl);
			MainMenuDisplay(mgl);
		}
		else if(loadingGame==1)
		{
			b=LoadGameUpdate(&lastTime,mgl);
			LoadGameDisplay(mgl);
			if(b==0)
			{
				for(i=0;i<MENU_CHOICES;i++)
					menu[i].bright=-32;

				cursor=1;
				loadingGame=0;
			}
			else if(b==2)
			{
				free(backScr);
				return MENU_LOADGAME+1;
			}
			b=0;
		}
		else
		{
			b=ChooseDiffUpdate(&lastTime,mgl);
			DiffChooseDisplay(mgl);
			if(b==0)
			{
				cursor=0;
				loadingGame=0;
			}
			else if(b==2)
			{
				free(backScr);
				if(choosingDiffFor==0)
					return MENU_ADVENTURE+1;
				else
					return MENU_REMIX+1;
			}
			b=0;
		}

		mgl->Flip();
		if(!mgl->Process())
		{
			free(backScr);
			return 255;
		}
		EndClock();
		if(!loadingGame && numRuns>=30*15)
		{
			GetTaps();
			oldc=255;
			Credits(mgl,1);
			mgl->LoadBMP("graphics/title.bmp");
			numRuns=0;
		}
		if(!loadingGame && b==MENU_LOADGAME+1)
		{
			loadingGame=1;
			GetSavesForMenu();
			cursor=0;
			b=0;
			oldc=255;
		}
		if(!loadingGame && b==MENU_ADVENTURE+1)
		{
			loadingGame=2;
			choosingDiffFor=0;
			cursor=0;
			b=0;
			oldc=255;
		}
		if(!loadingGame && b==MENU_REMIX+1)
		{
			loadingGame=2;
			choosingDiffFor=1;
			cursor=0;
			b=0;
			oldc=255;
		}
	}
	free(backScr);

	return b;
}

void CreditsRender(int y)
{
	int i,ypos;
	char *s;
	char b;

	i=0;

	ypos=0;
	while(credits[i][0]!='$')
	{
		s=credits[i];
		if(ypos-y>-60)
		{
			if(ypos-y>240)
				b=-((ypos-y)-300)/10;
			else
				b=0;

			if(b>0)
				b=0;

			if(s[0]=='@')
			{
				CenterPrintColor(320,ypos-y,&s[1],4,b,2);
			}
			else if(s[0]=='#')
			{
				DrawFillBox(320-200,ypos-y+8,320+200,ypos-y+11,4*32+31+b);
			}
			else if(s[0]=='%')
			{
				DrawFillBox(320-70,ypos-y+8,320+70,ypos-y+9,4*32+31+b);
			}
			else
				CenterPrintColor(320,ypos-y,s,4,b,1);
		}
		ypos+=20;
		i++;
		if(ypos-y>=480)
			return;
	}
}

void Credits(MGLDraw *mgl,byte init)
{
	int y=-470;
	int lastTime;
	int wid;
	int pos;
	int i;
	dword hangon;

	EndClock();
	hangon=TimeLength();

	mgl->LastKeyPressed();
	mgl->LoadBMP("graphics/title.bmp");
	lastTime=1;
	if(init)
		InitPlasma(4);

	hangon=0;
	for(i=0;i<40;i++)
		if(opt.meritBadge[i])
			hangon++;

	if(hangon==40)
		CheatText(mgl,0);

	while(1)
	{
		lastTime+=TimeLength();
		StartClock();

		wid=mgl->GetWidth();
		pos=(int)mgl->GetScreen()+0*wid;
		for(i=0;i<480;i++)
		{
			memset((byte *)pos,4*32+2,640);
			pos+=wid;
		}

		RenderPlasma2(mgl);
		CreditsRender(y);

		if(lastTime>TIME_PER_FRAME*30)
			lastTime=TIME_PER_FRAME*30;

		while(lastTime>=TIME_PER_FRAME)
		{
			y+=1;
			UpdatePlasma();
			lastTime-=TIME_PER_FRAME;
		}

		mgl->Flip();
		EndClock();

		if(!mgl->Process())
		{
			ExitPlasma();
			ResetClock(hangon);
			return;
		}
		if(mgl->LastKeyPressed())
		{
			ExitPlasma();
			ResetClock(hangon);
			return;
		}
		if(y==END_OF_CREDITS)
		{
			ExitPlasma();
			ResetClock(hangon);
			return;
		}
	}
	ResetClock(hangon);
}

void CheatsRender(int y)
{
	int i,ypos;
	char *s;
	char b;

	i=0;

	ypos=0;
	while(cheatTxt[i][0]!='$')
	{
		s=cheatTxt[i];
		if(ypos-y>-60)
		{
			if(ypos-y>240)
				b=-((ypos-y)-300)/10;
			else
				b=0;

			if(b>0)
				b=0;

			if(s[0]=='@')
			{
				CenterPrintColor(320,ypos-y,&s[1],4,b,2);
			}
			else if(s[0]=='#')
			{
				DrawFillBox(320-200,ypos-y+8,320+200,ypos-y+11,4*32+31+b);
			}
			else if(s[0]=='%')
			{
				DrawFillBox(320-70,ypos-y+8,320+70,ypos-y+9,4*32+31+b);
			}
			else
				CenterPrintColor(320,ypos-y,s,4,b,1);
		}
		ypos+=20;
		i++;
		if(ypos-y>=480)
			return;
	}
}

void CheatText(MGLDraw *mgl,byte init)
{
	int y=-470;
	int lastTime;
	int wid;
	int pos;
	int i;
	dword hangon;

	EndClock();
	hangon=TimeLength();

	mgl->LastKeyPressed();
	mgl->LoadBMP("graphics/title.bmp");
	lastTime=1;
	if(init)
		InitPlasma(4);
	while(1)
	{
		lastTime+=TimeLength();
		StartClock();

		wid=mgl->GetWidth();
		pos=(int)mgl->GetScreen()+0*wid;
		for(i=0;i<480;i++)
		{
			memset((byte *)pos,4*32+2,640);
			pos+=wid;
		}

		RenderPlasma2(mgl);
		CheatsRender(y);

		if(lastTime>TIME_PER_FRAME*30)
			lastTime=TIME_PER_FRAME*30;

		while(lastTime>=TIME_PER_FRAME)
		{
			y+=1;
			UpdatePlasma();
			lastTime-=TIME_PER_FRAME;
		}

		mgl->Flip();
		EndClock();

		if(!mgl->Process())
		{
			ExitPlasma();
			ResetClock(hangon);
			return;
		}
		if(mgl->LastKeyPressed())
		{
			ExitPlasma();
			ResetClock(hangon);
			return;
		}
		if(y==END_OF_CHEATS)
		{
			ExitPlasma();
			ResetClock(hangon);
			return;
		}
	}
	ResetClock(hangon);
}

void VictoryTextRender(int y)
{
	int i,ypos;
	char *s;
	char b;

	i=0;

	ypos=0;
	while(victoryTxt[i][0]!='$')
	{
		s=victoryTxt[i];
		if(ypos-y>-60)
		{
			if(ypos-y>240)
				b=-((ypos-y)-300)/10;
			else
				b=0;

			if(b>0)
				b=0;

			if(s[0]=='@')
			{
				CenterPrintColor(320,ypos-y,&s[1],4,b,2);
			}
			else if(s[0]=='#')
			{
				DrawFillBox(320-200,ypos-y+8,320+200,ypos-y+11,4*32+31+b);
			}
			else if(s[0]=='%')
			{
				DrawFillBox(320-70,ypos-y+8,320+70,ypos-y+9,4*32+31+b);
			}
			else
				CenterPrintColor(320,ypos-y,s,4,b,0);
		}

		ypos+=20;
		i++;
		if(ypos-y>=480)
			return;
	}
}

void VictoryText(MGLDraw *mgl)
{
	int y=-470;
	int lastTime;
	int wid;
	int pos;
	int i;
	dword hangon;

	EndClock();
	hangon=TimeLength();
	lastTime=1;
	mgl->LastKeyPressed();
	mgl->LoadBMP("graphics/title.bmp");
	InitPlasma(4);
	while(1)
	{
		lastTime+=TimeLength();
		StartClock();

		wid=mgl->GetWidth();
		pos=(int)mgl->GetScreen()+0*wid;
		for(i=0;i<480;i++)
		{
			memset((byte *)pos,4*32+2,640);
			pos+=wid;
		}

		RenderPlasma2(mgl);

		VictoryTextRender(y);

		if(lastTime>TIME_PER_FRAME*30)
			lastTime=TIME_PER_FRAME*30;

		while(lastTime>=TIME_PER_FRAME)
		{
			y++;
			UpdatePlasma();
			lastTime-=TIME_PER_FRAME;
		}
		mgl->Flip();
		EndClock();
		if(!mgl->Process())
		{
			ResetClock(hangon);
			return;
		}
		if(mgl->LastKeyPressed()==27)
		{
			ResetClock(hangon);
			return;
		}
		if(y==END_OF_VICTORY)
		{
			ResetClock(hangon);
			return;
		}
	}
	ResetClock(hangon);
}

byte SpeedSplash(MGLDraw *mgl,const char *fname)
{
	int i,j,clock;
	RGB desiredpal[256],curpal[256];
	byte mode,done;
	byte c,oldc;


	for(i=0;i<256;i++)
	{
		curpal[i].r=0;
		curpal[i].g=0;
		curpal[i].b=0;
	}
	mgl->SetPalette(curpal);
	mgl->RealizePalette();

	mgl->LastKeyPressed();
	oldc=GetControls()|GetArrows();

	if (!mgl->LoadBMP(fname, desiredpal))
		return 0;

	mode=0;
	clock=0;
	done=0;
	while(!done)
	{
		mgl->Flip();
		if(!mgl->Process())
			return 0;
		c=mgl->LastKeyPressed();

		if(c==27)
			return 0;
		else if(c)
			mode=2;

		c=GetControls()|GetArrows();
		if((c&(CONTROL_B1|CONTROL_B2)) && (!(oldc&(CONTROL_B1|CONTROL_B2))))
			mode=2;
		oldc=c;

		clock++;
		switch(mode)
		{
			case 0:	// fading in
				for(j=0;j<16;j++)
					for(i=0;i<256;i++)
					{
						if(curpal[i].r<desiredpal[i].r)
							curpal[i].r++;
						if(curpal[i].g<desiredpal[i].g)
							curpal[i].g++;
						if(curpal[i].b<desiredpal[i].b)
							curpal[i].b++;
					}
				mgl->SetPalette(curpal);
				mgl->RealizePalette();
				if(clock>16)
				{
					mode=1;
					clock=0;
				}
				break;
			case 1:
				// sit around
				break;
			case 2:	// fading out
				clock=0;
				for(j=0;j<16;j++)
					for(i=0;i<256;i++)
					{
						if(curpal[i].r>0)
							curpal[i].r--;
						else
							clock++;
						if(curpal[i].g>0)
							curpal[i].g--;
						else
							clock++;
						if(curpal[i].b>0)
							curpal[i].b--;
						else
							clock++;
					}
				mgl->SetPalette(curpal);
				mgl->RealizePalette();
				if(clock==256*3*16)
					done=1;
				break;
		}
	}
	mgl->ClearScreen();
	mgl->Flip();
	return 1;
}

void HelpScreens(MGLDraw *mgl)
{
	int i;
	char name[32];

	for(i=0;i<5;i++)
	{
		sprintf(name,"docs/help%d.bmp",i+1);
		if(!SpeedSplash(mgl,name))
			return;
	}
}

void DemoSplashScreens(MGLDraw *mgl)
{
	if(!SpeedSplash(mgl,"graphics/advert.bmp"))
		return;
}

void SplashScreen(MGLDraw *mgl,const char *fname,int delay,byte sound)
{
	int i,j,clock;
	RGB desiredpal[256],curpal[256];
	byte mode,done;
	dword tick;

	for(i=0;i<256;i++)
	{
		curpal[i].r=0;
		curpal[i].g=0;
		curpal[i].b=0;
	}
	mgl->SetPalette(curpal);
	mgl->RealizePalette();

	mgl->LastKeyPressed();

	if (!mgl->LoadBMP(fname, desiredpal))
		return;

	StartClock();
	mode=0;
	clock=0;
	done=0;
	tick=0;
	while(!done)
	{

		mgl->Flip();
		if(!mgl->Process())
			return;
		if(mgl->LastKeyPressed())
			mode=2;
		EndClock();
		tick+=TimeLength();
		StartClock();
		if(tick>1000/30)
		{
			clock++;
			tick-=1000/30;
			switch(mode)
			{
				case 0:	// fading in
					for(j=0;j<8;j++)
						for(i=0;i<256;i++)
						{
							if(curpal[i].r<desiredpal[i].r)
								curpal[i].r++;
							if(curpal[i].g<desiredpal[i].g)
								curpal[i].g++;
							if(curpal[i].b<desiredpal[i].b)
								curpal[i].b++;
						}
					mgl->SetPalette(curpal);
					mgl->RealizePalette();
					if(clock==32)
					{
						//if(sound==2)
							//MakeNormalSound(SND_HAMUMU);
					}
					if(clock>64)
					{
						mode=1;
						clock=0;
					}
					break;
				case 1:
					if(clock>delay)
					{
						mode=2;
						clock=0;
					}
					break;
				case 2:	// fading out
					clock=0;
					for(j=0;j<8;j++)
						for(i=0;i<256;i++)
						{
							if(curpal[i].r>0)
								curpal[i].r--;
							else
								clock++;
							if(curpal[i].g>0)
								curpal[i].g--;
							else
								clock++;
							if(curpal[i].b>0)
								curpal[i].b--;
							else
								clock++;
						}
					mgl->SetPalette(curpal);
					mgl->RealizePalette();
					if(clock==256*3*8)
						done=1;
					break;
			}
		}
	}
	mgl->ClearScreen();
	mgl->Flip();
}

#include "title.h"
#include "game.h"
#include "jamulfmv.h"
#include "pause.h"
#include "options.h"
#include "pause.h"
#include "plasma.h"
#include "palettes.h"
#include "appdata.h"
#include "steam.h"

#define VERSION_NO         "Version 3.0"
#define COPYRIGHT_YEARS    "2001-2024"
#define COPYRIGHT_COMPANY  "Hamumu Games, Inc."

// special codes in the credits:
// @ = use GirlsRWeird font
// # = draw a major horizontal line
// % = draw a minor horizontal line
// $ = last line of the whole deal
static const char* credits[] = {
	"@LoonyLand:",
	"",
	"@Halloween Hill",
	"",
	"",
	("Copyright " COPYRIGHT_YEARS ", " COPYRIGHT_COMPANY),
	"#",
	"Original Concept",
	"Mike Hommel",
	"%",
	"Programming",
	"Mike Hommel",
	"Tad \"SpaceManiac\" Hardesty",
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

static const char* victoryTxt[] = {
	"",
	"",
	"&After a little time to rest and wipe off a whole",
	"",
	"&lot of mud, Loony got back to his feet and returned",
	"",
	"&to Luniton, a hero at last.",
	"",
	"",
	"",
	"#",
	"",
	"",
	"&All around the land of Halloween Hill, life was",
	"",
	"&returning to normal.  The frogs stopped licking",
	"",
	"&so viciously, the mummies settled down and returned",
	"",
	"&to their work in the toilet paper industry - Even",
	"",
	"&the werewolves went back to their den to take a nap.",
	"",
	"&Yes, everything was back to being as peaceful as it",
	"",
	"&gets in Halloween Hill.  Which, by the way, is not",
	"",
	"&terribly peaceful.",
	"",
	"",
	"",
	"",
	"",
	"#",
	"",
	"",
	"&There was just one thing wrong...",
	"",
	"&The trees in the south of Wicked Woods were still",
	"",
	"&hung with stick figures.  Almost as if the evil",
	"",
	"&presence there hadn't been the result of the",
	"",
	"&Evilizer at all... and was still lurking somewhere.",
	"$"
	};

static const char* cheatTxt[] = {
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

static byte oldc=0;

static byte cursor;
byte *backScr;
int numRuns;
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
};

menu_t saves[5];
byte choosingDiffFor;

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
		f=AppdataOpen(txt);
		if(!f)
		{
			sprintf(saves[i].txt, "%d: Unused", i+1);
			saves[i].known=0;
		}
		else
		{
			fread(&p,sizeof(player_t),1,f);
			fclose(f);

			DescribeSave(saves[i].txt, &p);
			saves[i].known=1;
		}
		saves[i].bright=-32;
	}
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

	// Workshop status
	const char* workshopStatus = Steam()->DescribeWorkshopStatus();
	Print(3, 3, workshopStatus, 1, 1);
	Print(3-1, 3-1, workshopStatus, 0, 1);
	// version #:
	Print(560,3,VERSION_NO,1,1);
	Print(559,2,VERSION_NO,0,1);
	// Copyright:
	RightPrintGlow(641, 467, "Copyright " COPYRIGHT_YEARS ", " COPYRIGHT_COMPANY, -10, 1);

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

	const static char diffDesc[][128]={
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

	// Workshop status
	const char* workshopStatus = Steam()->DescribeWorkshopStatus();
	Print(3, 3, workshopStatus, 1, 1);
	Print(3-1, 3-1, workshopStatus, 0, 1);
	// version #:
	Print(560,3,VERSION_NO,1,1);
	Print(559,2,VERSION_NO,0,1);

	PrintGlow(5,330,"Difficulty Level:",0,2);

	if(opt.difficulty>0)
		PrintGlow(280,330,"<<<",0,2);

	CenterPrintGlow(440,330,DifficultyName(opt.difficulty),0,2);
	CenterPrintGlow(440-4+Random(9),330-4+Random(9),DifficultyName(opt.difficulty),-20,2);

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

	// Workshop status
	const char* workshopStatus = Steam()->DescribeWorkshopStatus();
	Print(3, 3, workshopStatus, 1, 1);
	Print(3-1, 3-1, workshopStatus, 0, 1);
	// version #:
	Print(560,3,VERSION_NO,1,1);
	Print(559,2,VERSION_NO,0,1);

	PrintGlow(5,467,"Select a game to load or press ESC to cancel",0,1);
	// menu options
	x=20;
	y=320;
	for(i=0;i<5;i++)
	{
		ShowSavedGame(x+saves[i].bright/4,y,saves[i],(cursor==i));
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
#ifndef NDEBUG
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

TASK(byte) MainMenu(MGLDraw *mgl)
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

	if(opt.modes[MODE_BADGES])
		menu[MENU_BADGES].known=1;
	if(opt.modes[MODE_BOWLING])
		menu[MENU_BOWLING].known=1;
	if(opt.modes[MODE_LOONYBALL])
		menu[MENU_LOONYBALL].known=1;
	if(opt.modes[MODE_BOSSBASH])
		menu[MENU_BOSSATTACK].known=1;
	if(opt.modes[MODE_SURVIVAL])
		menu[MENU_SURVIVAL].known=1;
	if(opt.remixMode)
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
				CO_RETURN MENU_LOADGAME+1;
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
					CO_RETURN MENU_ADVENTURE+1;
				else
					CO_RETURN MENU_REMIX+1;
			}
			b=0;
		}

		AWAIT mgl->Flip();
		if(!mgl->Process())
		{
			free(backScr);
			CO_RETURN 255;
		}
		EndClock();
		if(!loadingGame && numRuns>=30*15)
		{
			GetTaps();
			oldc=255;
			AWAIT Credits(mgl,1);
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

	CO_RETURN b;
}

// true = continue, false = document is now offscreen
bool CreditsRender(int y, const char* const* document)
{
	int i,ypos;
	const char *s;
	char b;

	i=0;

	ypos=0;
	while(document[i][0]!='$')
	{
		s=document[i];
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
			else if(s[0]=='&')
			{
				CenterPrintColor(320,ypos-y,&s[1],4,b,0);
			}
			else
				CenterPrintColor(320,ypos-y,s,4,b,1);
		}
		ypos+=20;
		i++;
		if(ypos-y>=480)
			return true;  // went off the bottom before document ended
	}

	// document ended, was it off the top?
	return ypos-y>-60;
}

static TASK(void) DoCredits(MGLDraw *mgl, const char* const* document)
{
	int y = -470;
	dword lastTime = 0;

	GetTaps();
	mgl->LastKeyPressed();
	mgl->LoadBMP("graphics/title.bmp");

	StartClock();
	while (true)
	{
		EndClock();
		lastTime += TimeLength();
		StartClock();

		lastTime = std::min(lastTime, TIME_PER_FRAME * 30u);
		while (lastTime >= TIME_PER_FRAME)
		{
			if (GetControls() & CONTROL_UP)
				y = std::max(-470, y - 3);
			else if (GetControls() & (CONTROL_DN | CONTROL_B1))
				y += 3;
			else
				y += 1;
			UpdatePlasma();
			lastTime -= TIME_PER_FRAME;
		}

		memset(mgl->GetScreen(), 4*32 + 2, mgl->GetWidth() * mgl->GetHeight());
		RenderPlasma2(mgl);
		bool stillGoing = CreditsRender(y, document);
		AWAIT mgl->Flip();

		if(!mgl->Process() || mgl->LastKeyPressed() == SDLK_ESCAPE || (GetTaps() & CONTROL_B2) || !stillGoing)
		{
			break;
		}
	}
	ExitPlasma();
}

TASK(void) Credits(MGLDraw *mgl, byte init)
{
	if(init)
		InitPlasma(4);

	int hangon=0;
	for(int i=0;i<40;i++)
		if(opt.meritBadge[i])
			hangon++;

	if(hangon==40)
		AWAIT CheatText(mgl,0);

	AWAIT DoCredits(mgl, credits);
}

TASK(void) CheatText(MGLDraw *mgl,byte init)
{
	if(init)
		InitPlasma(4);

	AWAIT DoCredits(mgl, cheatTxt);
}

TASK(void) VictoryText(MGLDraw *mgl)
{
	InitPlasma(4);
	AWAIT DoCredits(mgl, victoryTxt);
}

TASK(void) SplashScreen(MGLDraw *mgl,const char *fname,int delay,byte sound)
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
		CO_RETURN;

	StartClock();
	mode=0;
	clock=0;
	done=0;
	tick=0;
	while(!done)
	{

		AWAIT mgl->Flip();
		if(!mgl->Process())
			CO_RETURN;
		if(mgl->LastKeyPressed() || GetJoyButtons())
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
	AWAIT mgl->Flip();
}

#include "title.h"
#include "game.h"
#include "jamulfmv.h"
#include "pause.h"
#include "appdata.h"

// special codes in the credits:
// @ = use GirlsRWeird font
// # = draw a major horizontal line
// % = draw a minor horizontal line
// $ = last line of the whole deal
char credits[][32]={
	"THE ADVENTURES OF BOUAPHA:",
	"@SPOOKY CASTLE",
	"",
	"",
	"Copyright 1998-2004,",
	"Hamumu Software",
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
	"Mike Hommel",
	"%",
	"Sound Effects",
	"Brent Christian",
	"Mike Hommel",
	"(Surprise!)",
	"%",
	"Producer/Designer/Director",
	"Mike Hommel",
	"%",
	"Gaffer",
	"Mike Hommel",
	"%",
	"QA Director",
	"Angela Finer",
	"%",
	"Testing",
	"Brent Christian",
	"Jim Crawford",
	"Chris Dillman",
	"Angela Finer",
	"Tim Finer",
	"Dawn Genge",
	"Mattie Goodman",
	"Matt Guest",
	"Suzanne Hommel",
	"Brad Kasten",
	"Geoff Michell",
	"Britt Morris",
	"Trevor Strohman",
	"%",
	"Technical Assistance",
	"Trevor Strohman",
	"%",
	"Special Thanks",
	"Junebug Superspy &",
	"Too Much Hot Sauce",
	"All the SpisFans",
	"(both of them that is)",
	"Ketmany Bouapha (NO, Bouapha!)",
	"Rinley \"Dirty\" Deeds",
	"%",
	"#",
	"Stop by www.hamumu.com!",
	"","","","","","","","","","","","","","","","",
	"@THE END",
	"$"
	};

char victoryTxt[][64]={
	"@After destroying the evil monster",
	"",
	"",
	"@that was leasing the castle for its",
	"",
	"",
	"@nefarious schemes, Bouapha not only",
	"",
	"",
	"@returned all the missing brains to",
	"",
	"",
	"@the organ bank, but even cleaned up",
	"",
	"",
	"@the place so that, had the monster",
	"",
	"",
	"@lived, it could have recovered the",
	"",
	"",
	"@security deposit.  Bouapha is",
	"",
	"",
	"@nothing if not a thoughtful man.",
	"",
	"",
	"#",
	"",
	"",
	"@And so Bouapha returned home for some",
	"",
	"",
	"@much-deserved rest, and checked out",
	"",
	"",
	"@the classifieds for some more",
	"",
	"",
	"@adventures on which to embark...",
	"",
	"",
	"#",
	"",
	"",
	"If you enjoyed this little adventure,",
	"check out Dr. Lunatic Supreme With Cheese",
	"for hundreds more!  Seriously, HUNDREDS!",
	"",
	"$"
	};

// once the credits have scrolled to END_OF_CREDITS pixels, they end
#define END_OF_CREDITS 480*4
#define END_OF_VICTORY 480*2

// the most custom worlds it will handle
#define MAX_CUSTOM 64

typedef struct title_t
{
	int bouaphaX;
	int blueY;
	char titleBright;
	char titleDir;
	int moonY;
	int optionsX;
	byte cursor;
	byte savecursor;
	float percent[3];
} title_t;

sprite_set_t *planetSpr;
static int numRunsToMakeUp;
byte pickerpos;
char pickeroffset;
byte offsetdir;
byte curCustom;

static byte oldc=0;
mfont_t pickerFont;
char customName[MAX_CUSTOM][32];
char customFname[MAX_CUSTOM][32];

static byte keyAnim=0;

byte starColorTable[]={214,81,63,49,33,21,32,83,93};

void MainMenuDisplay(MGLDraw *mgl,title_t title)
{
	int i,color,deltaColor;
	byte *scrn;

	mgl->ClearScreen();

	// blueness effect at top of screen
	color=12*65536;
	deltaColor=(12*65536)/(title.blueY);
	scrn=mgl->GetScreen();
	if(title.blueY>0)
		memset(scrn,0,640*title.blueY);

	for(i=0;i<title.blueY;i++)
	{
		memset(scrn,color/65536+96,640);
		scrn+=640;
		color-=deltaColor;
		if(color<0)
			color=0;
	}

	// draw Bouapha
	planetSpr->GetSprite(2)->Draw(title.bouaphaX,220,mgl);
	// draw moon
	planetSpr->GetSprite(9)->Draw(10,title.moonY,mgl);

	// draw the title parts
	// Adventures of Bouapha:
	planetSpr->GetSprite(0)->DrawBright(180,10,mgl,title.titleBright);
	// SPOOKY CASTLE:
	planetSpr->GetSprite(1)->DrawBright(70,45,mgl,title.titleBright);
	// Copyright:
	//planetSpr->GetSprite(10)->Draw(2,463,mgl);
	Print(2-1,463-1,"Copyright 1998-2004, Hamumu Software",-32,1);
	Print(2+1,463+1,"Copyright 1998-2004, Hamumu Software",-32,1);
	Print(2,463,"Copyright 1998-2004, Hamumu Software",0,1);

	// now the menu options
	planetSpr->GetSprite(3+(title.cursor==0))->Draw(title.optionsX+20,240,mgl);
	planetSpr->GetSprite(5+(title.cursor==1))->Draw(title.optionsX,310,mgl);
	planetSpr->GetSprite(7+(title.cursor==2))->Draw(title.optionsX+40,380,mgl);
}

byte MainMenuUpdate(MGLDraw *mgl,title_t *title,int *lastTime)
{
	byte c;
	static byte reptCounter=0;

	while(*lastTime>=TIME_PER_FRAME)
	{
		// update graphics
		title->titleBright+=title->titleDir;
		if(title->titleBright>31)
		{
			title->titleDir=-1;
			title->titleBright=31;
		}
		if(title->titleDir<0 && title->titleBright==0)
			title->titleDir=0;

		if(title->bouaphaX>360)
			title->bouaphaX-=5;
		if(title->optionsX<160)
			title->optionsX+=8;

		if(title->moonY>20)
			title->moonY-=8;

		if(title->blueY<200)
			title->blueY+=8;

		// now real updating
		c=GetControls();

		reptCounter++;
		if((!oldc) || (reptCounter>10))
			reptCounter=0;

		if((c&CONTROL_UP) && (!reptCounter))
		{
			(title->cursor)--;
			if(title->cursor==255)
				title->cursor=2;
			MakeNormalSound(SND_MENUCLICK);
		}
		if((c&CONTROL_DN) && (!reptCounter))
		{
			(title->cursor)++;
			if(title->cursor==3)
				title->cursor=0;
			MakeNormalSound(SND_MENUCLICK);
		}
		if(((c&CONTROL_B1) && (!(oldc&CONTROL_B1))) ||
		   ((c&CONTROL_B2) && (!(oldc&CONTROL_B2))))
		{
			MakeNormalSound(SND_MENUSELECT);
			return 1;
		}
		oldc=c;

		c=mgl->LastKeyPressed();
		if(c==27)
		{
			MakeNormalSound(SND_MENUSELECT);
			return 2;
		}
#ifdef _DEBUG
		if(c=='e')
		{
			title->cursor=3;
			return 1;
		}
#endif
		*lastTime-=TIME_PER_FRAME;
	}
	return 0;
}

TASK(byte) MainMenu(MGLDraw *mgl)
{
	dword startTime,now;
	byte b=0;
	title_t title;
	int lastTime=1;

	mgl->LoadBMP("graphics/title.bmp");
	mgl->LastKeyPressed();
	mgl->ClearScreen();
	oldc=CONTROL_B1|CONTROL_B2;
	planetSpr=new sprite_set_t("graphics/titlespr.jsp");

	title.bouaphaX=640;
	title.optionsX=-300;
	title.titleBright=-32;
	title.titleDir=4;
	title.cursor=0;
	title.blueY=1;
	title.moonY=480;
	startTime=timeGetTime();
	StartClock();
	while(b==0)
	{
		lastTime+=TimeLength();
		StartClock();
		b=MainMenuUpdate(mgl,&title,&lastTime);
		MainMenuDisplay(mgl,title);
		AWAIT mgl->Flip();
		if(!mgl->Process())
		{
			delete planetSpr;
			CO_RETURN 255;
		}
		if(b==1 && title.cursor==1)	// selected Continue
		{
			if(!AWAIT GameSlotPicker(mgl,&title))	// pressed ESC on the slot picker
			{
				b=0;
			}
		}
		now=timeGetTime();
		if(now-startTime>1000*10)
		{
			AWAIT Credits(mgl);
			startTime=timeGetTime();
		}
		EndClock();
	}
	delete planetSpr;
	if(b==1)	// something was selected
	{
		if(title.cursor==2)	// exit
			CO_RETURN 255;
		else
			CO_RETURN title.cursor;
	}
	else
		CO_RETURN 255;	// ESC was pressed
}

void GameSlotPickerDisplay(MGLDraw *mgl,title_t title)
{
	int i,j,color,deltaColor,x;
	byte *scrn;
	char txt[8];

	mgl->ClearScreen();

	// blueness effect at top of screen
	color=12*65536;
	deltaColor=(12*65536)/(title.blueY);
	scrn=mgl->GetScreen();
	if(title.blueY>0)
		memset(scrn,0,640*title.blueY);

	for(i=0;i<title.blueY;i++)
	{
		memset(scrn,color/65536+96,640);
		scrn+=640;
		color-=deltaColor;
		if(color<0)
			color=0;
	}

	// draw Bouapha
	planetSpr->GetSprite(2)->Draw(title.bouaphaX,220,mgl);
	// draw moon
	planetSpr->GetSprite(9)->Draw(10,title.moonY,mgl);

	// draw the title parts
	// Adventures of Bouapha:
	planetSpr->GetSprite(0)->DrawBright(180,10,mgl,title.titleBright);
	// SPOOKY CASTLE:
	planetSpr->GetSprite(1)->DrawBright(70,45,mgl,title.titleBright);
	// Copyright:
	//planetSpr->GetSprite(10)->Draw(2,463,mgl);
	Print(2-1,463-1,"Copyright 1998-2004, Hamumu Software",-32,1);
	Print(2+1,463+1,"Copyright 1998-2004, Hamumu Software",-32,1);
	Print(2,463,"Copyright 1998-2004, Hamumu Software",0,1);

	// now the game slots
	for(i=0;i<3;i++)
	{
		x=80-i*10;
		planetSpr->GetSprite(11+(title.savecursor==i))->Draw(x,260+50*i,mgl);
		x+=planetSpr->GetSprite(11)->width+12;
		planetSpr->GetSprite(17+i*2+(title.savecursor==i))->Draw(x,260+50*i,mgl);
		x+=planetSpr->GetSprite(17+i*2)->width+4;
		planetSpr->GetSprite(13+(title.savecursor==i))->Draw(x,260+50*i,mgl);
		x+=planetSpr->GetSprite(13)->width+8;

		if(title.percent[i]>99.9)
			sprintf(txt,"100");
		else
			sprintf(txt,"%03.1f",title.percent[i]);

		// draw out the percentage
		j=0;
		while(txt[j])
		{
			if(txt[j]=='.')
			{
				planetSpr->GetSprite(35+(title.savecursor==i))->Draw(x-8,260+50*i,mgl);
				x+=planetSpr->GetSprite(35)->width+4;
			}
			else
			{
				planetSpr->GetSprite(15+(txt[j]-'0')*2+(title.savecursor==i))->Draw(x,260+50*i,mgl);
				x+=planetSpr->GetSprite(15+(txt[j]-'0')*2)->width+4;
			}
			j++;
		}
		// and draw a %
		planetSpr->GetSprite(37+(title.savecursor==i))->Draw(x,260+50*i,mgl);
	}
}

byte GameSlotPickerUpdate(MGLDraw *mgl,title_t *title,int *lastTime)
{
	byte c;
	static byte reptCounter=0;

	while(*lastTime>=TIME_PER_FRAME)
	{
		// update graphics
		title->titleBright+=title->titleDir;
		if(title->titleBright>31)
		{
			title->titleDir=-1;
			title->titleBright=31;
		}
		if(title->titleDir<0 && title->titleBright==0)
			title->titleDir=0;

		if(title->bouaphaX>360)
			title->bouaphaX-=8;
		if(title->optionsX<160)
			title->optionsX+=8;

		if(title->moonY>20)
			title->moonY-=10;

		if(title->blueY<200)
			title->blueY+=8;

		// now real updating
		c=GetControls();

		reptCounter++;
		if((!oldc) || (reptCounter>10))
			reptCounter=0;

		if((c&CONTROL_UP) && (!reptCounter))
		{
			(title->savecursor)--;
			if(title->savecursor==255)
				title->savecursor=2;
			MakeNormalSound(SND_MENUCLICK);
		}
		if((c&CONTROL_DN) && (!reptCounter))
		{
			(title->savecursor)++;
			if(title->savecursor==3)
				title->savecursor=0;
			MakeNormalSound(SND_MENUCLICK);
		}
		if(((c&CONTROL_B1) && (!(oldc&CONTROL_B1))) ||
		   ((c&CONTROL_B2) && (!(oldc&CONTROL_B2))))
		{
			MakeNormalSound(SND_MENUSELECT);
			return 1;
		}
		oldc=c;
		*lastTime-=TIME_PER_FRAME;
	}

	if(mgl->LastKeyPressed()==27)
	{
		MakeNormalSound(SND_MENUSELECT);
		return 2;
	}
	return 0;
}

void InitGameSlotPicker(MGLDraw *mgl,title_t *title)
{
	FILE *f;
	player_t p;

	f=AppdataOpen("loony.sav");
	if(!f)
	{
		title->percent[0]=0.0;
		title->percent[1]=0.0;
		title->percent[2]=0.0;
	}
	else
	{
		fread(&p,sizeof(player_t),1,f);
		title->percent[0]=CalcTotalPercent(&p)*100;
		fread(&p,sizeof(player_t),1,f);
		title->percent[1]=CalcTotalPercent(&p)*100;
		fread(&p,sizeof(player_t),1,f);
		title->percent[2]=CalcTotalPercent(&p)*100;
		fclose(f);
	}
	mgl->LastKeyPressed();
	oldc=CONTROL_B1|CONTROL_B2;
}

TASK(byte) GameSlotPicker(MGLDraw *mgl,title_t *title)
{
	byte b=0;
	int lastTime=1;

	title->savecursor=0;
	InitGameSlotPicker(mgl,title);

	while(b==0)
	{
		lastTime+=TimeLength();
		StartClock();
		b=GameSlotPickerUpdate(mgl,title,&lastTime);
		GameSlotPickerDisplay(mgl,*title);
		AWAIT mgl->Flip();
		if(!mgl->Process())
			CO_RETURN 0;
		EndClock();
	}
	if(b==1)	// something was selected
	{
		InitPlayer(INIT_GAME,0,0);
		PlayerLoadGame(title->savecursor);
		// make it remember which was picked so the pause menu will start on the same
		SetSubCursor(title->savecursor);
		CO_RETURN 1;
	}
	else
		CO_RETURN 0;
}

void CreditsRender(int y)
{
	int i,ypos;
	char *s;

	i=0;

	ypos=0;
	while(credits[i][0]!='$')
	{
		s=credits[i];
		if(ypos-y>-60)
		{
			if(s[0]=='@')
			{
				CenterPrint(ypos-y,&s[1],0,0);
			}
			else if(s[0]=='#')
			{
				DrawFillBox(320-200,ypos-y+8,320+200,ypos-y+11,255);
			}
			else if(s[0]=='%')
			{
				DrawFillBox(320-70,ypos-y+8,320+70,ypos-y+9,255);
			}
			else
				CenterPrint(ypos-y,s,0,1);
		}
		ypos+=20;
		i++;
		if(ypos-y>=480)
			return;
	}
}

TASK(void) Credits(MGLDraw *mgl)
{
	int y=-470;
	dword lastTime;

	mgl->LastKeyPressed();
	mgl->LoadBMP("graphics/title.bmp");
	lastTime=1;
	while(1)
	{
		lastTime+=TimeLength();
		StartClock();
		while(lastTime>=TIME_PER_FRAME)
		{
			y++;
			lastTime-=TIME_PER_FRAME;
		}
		mgl->ClearScreen();
		CreditsRender(y);

		AWAIT mgl->Flip();
		if(!mgl->Process())
			CO_RETURN;
		if(mgl->LastKeyPressed())
			CO_RETURN;
		if(y==END_OF_CREDITS)
			CO_RETURN;
		EndClock();
	}
}

void VictoryTextRender(int y)
{
	int i,ypos;
	char *s;

	i=0;

	ypos=0;
	while(victoryTxt[i][0]!='$')
	{
		s=victoryTxt[i];
		if(ypos-y>-60)
		{
			if(s[0]=='@')
			{
				CenterPrint(ypos-y,&s[1],0,0);
			}
			else if(s[0]=='#')
			{
				DrawFillBox(320-200,ypos-y+8,320+200,ypos-y+11,255);
			}
			else if(s[0]=='%')
			{
				DrawFillBox(320-70,ypos-y+8,320+70,ypos-y+9,255);
			}
			else
				CenterPrint(ypos-y,s,0,1);
		}

		ypos+=20;
		i++;
		if(ypos-y>=480)
			return;
	}
}

TASK(void) VictoryText(MGLDraw *mgl)
{
	int y=-470;
	dword lastTime;

	mgl->LastKeyPressed();
	mgl->LoadBMP("graphics/title.bmp");
	lastTime=1;
	while(1)
	{
		lastTime+=TimeLength();
		StartClock();
		while(lastTime>=TIME_PER_FRAME)
		{
			y++;
			lastTime-=TIME_PER_FRAME;
		}
		mgl->ClearScreen();
		VictoryTextRender(y);
		AWAIT mgl->Flip();
		if(!mgl->Process())
			CO_RETURN;
		if(mgl->LastKeyPressed()==27)
			CO_RETURN;
		if(y==END_OF_VICTORY)
			CO_RETURN;
		EndClock();
	}
}

/*
byte SpecialLoadBMP(char *name,MGLDraw *mgl,palette_t *pal)
{
	FILE *f;
	BITMAPFILEHEADER bmpFHead;
	BITMAPINFOHEADER bmpIHead;
	RGBQUAD	pal2[256];

	int i;
	byte *scr;

	f=AssetOpen(name);
	if(!f)
		return FALSE;

	fread(&bmpFHead,sizeof(BITMAPFILEHEADER),1,f);
	fread(&bmpIHead,sizeof(BITMAPINFOHEADER),1,f);

	// 8-bit BMPs only
	if(bmpIHead.biBitCount!=8)
		return FALSE;

	fread(pal2,sizeof(pal2),1,f);
	for(i=0;i<256;i++)
	{
		pal[i].red=pal2[i].rgbRed;
		pal[i].green=pal2[i].rgbGreen;
		pal[i].blue=pal2[i].rgbBlue;
	}

	for(i=0;i<bmpIHead.biHeight;i++)
	{
		scr=(byte *)((int)mgl->GetScreen()+(bmpIHead.biHeight-1-i)*640);
		fread(scr,bmpIHead.biWidth,1,f);
	}
	fclose(f);
	return TRUE;
}

void SplashScreen(MGLDraw *mgl,char *fname,int delay,byte sound)
{
	int i,j,clock;
	palette_t desiredpal[256],curpal[256];
	byte mode,done;

	for(i=0;i<256;i++)
	{
		curpal[i].red=0;
		curpal[i].green=0;
		curpal[i].blue=0;
	}
	mgl->SetPalette(curpal);
	mgl->RealizePalette();

	mgl->LastKeyPressed();

	SpecialLoadBMP(fname,mgl,desiredpal);

	mode=0;
	clock=0;
	done=0;
	while(!done)
	{
		mgl->Flip();
		if(!mgl->Process())
			return;
		if(mgl->LastKeyPressed())
			mode=2;

		clock++;
		switch(mode)
		{
			case 0:	// fading in
				for(j=0;j<8;j++)
					for(i=0;i<256;i++)
					{
						if(curpal[i].red<desiredpal[i].red)
							curpal[i].red++;
						if(curpal[i].green<desiredpal[i].green)
							curpal[i].green++;
						if(curpal[i].blue<desiredpal[i].blue)
							curpal[i].blue++;
					}
				mgl->SetPalette(curpal);
				mgl->RealizePalette();
				if(clock==32)
				{
					if(sound==2)
						MakeNormalSound(SND_HAMUMU);
					else if(sound==1)
						MakeNormalSound(SND_XGAMES);
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
						if(curpal[i].red>0)
							curpal[i].red--;
						else
							clock++;
						if(curpal[i].green>0)
							curpal[i].green--;
						else
							clock++;
						if(curpal[i].blue>0)
							curpal[i].blue--;
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
	mgl->ClearScreen();
	mgl->Flip();

}
*/

TASK(void) SplashScreen(MGLDraw *dispmgl,const char *fname,int delay,byte sound,byte specialdeal)
{
	int i,j,clock;
	dword tick,tock;
	RGB desiredpal[256],curpal[256];
	byte mode,done;

	for(i=0;i<256;i++)
	{
		curpal[i].r=0;
		curpal[i].g=0;
		curpal[i].b=0;
	}
	dispmgl->SetPalette(curpal);
	dispmgl->RealizePalette();

	dispmgl->LastKeyPressed();

	if(!dispmgl->LoadBMP(fname,desiredpal))
		CO_RETURN;

	mode=0;
	clock=0;
	done=0;
	while(!done)
	{
		tick=timeGetTime();
		AWAIT dispmgl->Flip();
		if(!dispmgl->Process())
			CO_RETURN;

		// let it fade in & out at max speed, but the sitting still needs to be timed
		if(mode==1)
		{
			tock=timeGetTime();
			while(tock-tick<(1000/30))
				tock=timeGetTime();
		}

		if((!specialdeal) || clock>delay)
		{
			if(dispmgl->LastKeyPressed())
				mode=2;
		}

		clock++;
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
				dispmgl->SetPalette(curpal);
				dispmgl->RealizePalette();
				if(clock==32)
				{
					MakeNormalSound(sound);
				}
				if(clock>32)
				{
					mode=1;
					clock=0;
				}
				break;
			case 1:
				// if specialdeal==1, then it sits until a keypress, no matter what
				if(clock>delay && specialdeal==0)
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
				dispmgl->SetPalette(curpal);
				dispmgl->RealizePalette();
				if(clock==256*3*8)
					done=1;
				break;
		}
	}
	dispmgl->ClearScreen();
	AWAIT dispmgl->Flip();
}

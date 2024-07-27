#include "options.h"
#include "control.h"
#include "display.h"
#include "tile.h"
#include "backgd.h"
#include "status.h"
#include "game.h"
#include "random.h"
#include "sound.h"
#include "appdata.h"

#include <stdio.h>
#include <string.h>

options_t options;
int optmsx,optmsy;
static byte cursorOn;
static byte keys,oldKeys;

void InitOptions(void)
{
	FILE *f;

	// load the hi score file if you can
	f=AppdataOpen_Stdio("options.cfg");
	if(f)
	{
		fread(&options,1,sizeof(options_t),f);
		fclose(f);
	}
	else
	{
		// initialize default values
		options.lives=3;
		options.shroomReset=0;
		options.difficulty=DIFF_EASY;
		options.sound=1;
		options.music=1;

		options.hiScore[0]=10000;
		options.hiScore[1]=5000;
		options.hiScore[2]=2500;
		options.hiScore[3]=1250;
		options.hiScore[4]=5;

		strcpy(options.hiScoreName[0],"Eddie");
		strcpy(options.hiScoreName[1],"Galaxy");
		strcpy(options.hiScoreName[2],"Jamul");
		strcpy(options.hiScoreName[3],"Ralph");
		strcpy(options.hiScoreName[4],"Ixtatl");
	}
	options.cheating=0;
}

void ExitOptions(void)
{
	FILE *f;

	f=AppdataOpen_Write_Stdio("options.cfg");
	if(f)
	{
		fwrite(&options,1,sizeof(options_t),f);
		fclose(f);
	}
}

byte Cheating(void)
{
	return options.cheating;
}

void SetCheat(byte on)
{
	options.cheating=on;
}

char *GetHiScoreName(byte w)
{
	return options.hiScoreName[w];
}

int GetHiScore(byte w)
{
	return options.hiScore[w];
}

void SetHiScore(byte w,int score)
{
	options.hiScore[w]=score;
}

byte GetDifficulty(void)
{
	return options.difficulty;
}

byte GetStartLives(void)
{
	return options.lives;
}

byte SoundIsOn(void)
{
	return options.sound;
}

byte MusicIsOn(void)
{
	return options.music;
}

byte GetShroomReset(void)
{
	return options.shroomReset;
}

void ToggleSound(void)
{
	options.sound=1-options.sound;
	VolumeSong(options.sound==0);
}

void ToggleMusic(void)
{
	options.music=1-options.music;
	if(!options.music)
		StopSong();
	else
		ReplaySong();
}

//---------------------------------------------------------------------------------------

byte OptionsScreenRun(int *lastTime)
{
	int msx,msy;
	int oldCur;
	int i;

	while(*lastTime>=TIME_PER_FRAME)
	{
		UpdateMouse();
		GetMouse(&msx,&msy);
		oldKeys=keys;
		keys=GetControls();

		JamulSoundUpdate();
		optmsx+=msx;
		optmsy+=msy;

		if((keys&CONTROL_UP) && (!(oldKeys&CONTROL_UP)))
		{
			if(optmsy>139)
				optmsy-=50;
		}
		if((keys&CONTROL_DN) && (!(oldKeys&CONTROL_DN)))
		{
			if(optmsy<390)
				optmsy+=50;
		}

		if(optmsx<10)
			optmsx=10;
		if(optmsy<10)
			optmsy=10;
		if(optmsx>460)
			optmsx=460;
		if(optmsy>450)
			optmsy=450;

		oldCur=cursorOn;
		cursorOn=0;

		if(optmsy>89 && optmsy<140)
			cursorOn=1;
		if(optmsy>139 && optmsy<190)
			cursorOn=2;
		if(optmsy>189 && optmsy<240)
			cursorOn=3;
		if(optmsy>239 && optmsy<290)
			cursorOn=4;
		if(optmsy>289 && optmsy<340)
			cursorOn=5;
		if(optmsy>339 && optmsy<390)
			cursorOn=6;
		if(optmsy>389 && optmsy<440)
			cursorOn=7;

		if(cursorOn!=0 && oldCur!=cursorOn)	// you moved onto a different one, make a noise
			MakeSound(SND_TYPING,1000);

		if(mainmgl->MouseTap() || ((keys&CONTROL_B1)&&(!(oldKeys&CONTROL_B1))))
		{
			if(cursorOn!=0)
				MakeSound(SND_MENUSELECT,1100);
			switch(cursorOn)
			{
				case 0:
					// not on anything
					break;
				case 1:
					// obs. reset
					options.shroomReset=1-options.shroomReset;
					break;
				case 2:
					// difficulty
					options.difficulty++;
					if(options.difficulty>DIFF_HARD)
						options.difficulty=DIFF_EASY;
					break;
				case 3:
					// lives
					options.lives++;
					if(options.lives>9)
						options.lives=1;
					break;
				case 4:
					// sound
					ToggleSound();
					break;
				case 5:
					// music
					ToggleMusic();
					break;
				case 6:
					MakeSound(SND_NUKEBOOM,1200);
					// reset hi scores
					for(i=0;i<5;i++)
					{
						options.hiScore[i]=0;
						options.hiScoreName[i][0]='\0';
					}
					break;
				case 7:
					// exit
					return 0;
					break;
			}
		}
		*lastTime-=TIME_PER_FRAME;
	}
	if(mainmgl->LastKeyPressed()==27)
		return 0;

	return 1;
}

TASK(void) OptionsScreenRender(void)
{
	int i;

	DrawBackgd();
	RenderHiScoreDisplay();
	// show the title: "OPTIONS"
	ShowSprite(SPR_OPTION,0,160,14);
	// show the names of all the options
	ShowSprite(SPR_OPTION,2+(cursorOn==1),20,90);
	ShowSprite(SPR_OPTION,4+(cursorOn==2),20,140);
	ShowSprite(SPR_OPTION,6+(cursorOn==3),20,190);
	ShowSprite(SPR_OPTION,8+(cursorOn==4),20,240);
	ShowSprite(SPR_OPTION,10+(cursorOn==5),20,290);
	ShowSprite(SPR_OPTION,12+(cursorOn==6),20,340);
	ShowSprite(SPR_OPTION,14+(cursorOn==7),20,390);

	// show individual option values
	// Obstacle Reset: On/Off
	ShowSprite(SPR_OPTION,16+(cursorOn==1)+2*(options.shroomReset==0),320,90);
	// difficulty: easy/normal/hard
	ShowSprite(SPR_OPTION,22+(cursorOn==2)+2*(options.difficulty),320,140);
	// lives: 1-9
	for(i=0;i<9;i++)
		ShowSprite(SPR_OPTION,20+(options.lives>i),280+i*20,190);
	// sound: on/off
	ShowSprite(SPR_OPTION,16+(cursorOn==4)+2*(options.sound==0),320,240);
	// music: on/off
	ShowSprite(SPR_OPTION,16+(cursorOn==5)+2*(options.music==0),320,290);

	ShowSprite(SPR_OPTION,1,optmsx,optmsy);
	AWAIT FlipScreen();
}

TASK(void) OptionsScreen(void)
{
	byte quit;
	int lastTime;
	dword clock,endclock;

	quit=0;
	clock=0;
	endclock=0;
	lastTime=TIME_PER_FRAME;

	LoadBackgd("graphics/options.bmp");
	SetBackScroll(0);
	optmsx=SCR_WIDTH/2;
	optmsy=SCR_HEIGHT/2;
	cursorOn=0;

	mainmgl->LastKeyPressed();
	keys=0xFF;

	while(!quit)
	{
		lastTime+=endclock-clock;
		clock=GetTime();

		if(OptionsScreenRun(&lastTime)==0)
			quit=1;

		AWAIT OptionsScreenRender();

		endclock=GetTime();
	}
}

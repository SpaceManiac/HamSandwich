#include "highscore.h"
#include "display.h"
#include "player.h"
#include "fireworks.h"
#include "badge.h"
#include "bossbash.h"
#include "options.h"
#include "endgame.h"
#include "plasma.h"
#include "steam.h"

static byte position;
static highScore_t *me;
static char buffer[32];
static byte txtpos;
static byte flip;
static byte oldc;
static byte delCursor;
static byte deleting;

static const char modeText[][32]={
	"Adventure Mode",
	"Survival Mode",
	"Slingshot Mode",
	"Loonyball Mode",
	"Bowling Mode",
	"Boss Bash Mode",
	"Infinite Survival Mode",
	"Remix Mode",
};

//---------------------------------------
// entering your name when you get a high score

void InitNameEntry(void)
{
	GetDisplayMGL()->LastKeyPressed();
	GetTaps();
	InitFireworks();
	txtpos=0;
	memset(buffer, 0, sizeof(buffer));
	flip=0;
	MakeNormalSound(SND_HISCORE);
}

void ExitNameEntry(void)
{
	ExitFireworks();
	SaveOptions();
}

byte UpdateNameEntry(int *lastTime)
{
	char c;
	static byte fwClock=0;

	if(*lastTime>TIME_PER_FRAME*30)
		*lastTime=TIME_PER_FRAME*30;

	while(*lastTime>=TIME_PER_FRAME)
	{
		flip=1-flip;
		byte taps = GetTaps();

		UpdateFireworks();

		if(fwClock>0)
		{
			fwClock--;
			if(fwClock==0)
			{
				if(Random(100)<30)
					LaunchShell(10);
			}
		}
		else
			fwClock=10;

		c=GetDisplayMGL()->LastKeyPressed();

		// Up and down edit the last character, inserting a new one if empty.
		// Gamepads are limited to A thru Z for ease of nagivation.
		// B1 duplicates the last character, or finishes if full.
		// Characters add to the end.
		// B2 or Backspace deletes the last character.
		// Enter finishes.
		// ESC resets to "???" and quits.

		if(c==8 || (taps & CONTROL_B2)) // backspace
		{
			if(txtpos>0)
			{
				txtpos--;
				buffer[txtpos]='\0';
			}
			ham_strcpy(me->name, buffer);
		}
		else if(c==27)  // escape
		{
			ham_strcpy(me->name, "???");
			return 0;
		}
		else if(c==13)  // enter
		{
			ham_strcpy(me->name, "???");
			return 0;
		}
		else if(txtpos<3 && c >= ' ' && c <= '~')
		{
			buffer[txtpos]=c;
			txtpos++;
			buffer[txtpos]='\0';
		}
		else if (taps & CONTROL_DN)
		{
			if (txtpos == 0)
			{
				buffer[0] = 'Z';  // will be ++'d into A
				txtpos++;
			}
			buffer[txtpos - 1]++;
			if (buffer[txtpos - 1] < 'A' || buffer[txtpos - 1] > 'Z')
				buffer[txtpos - 1] = 'A';
		}
		else if (taps & CONTROL_UP)
		{
			if (txtpos == 0)
			{
				buffer[0] = 'A';  // will be --'d into Z
				txtpos++;
			}
			buffer[txtpos - 1]--;
			if (buffer[txtpos - 1] < 'A' || buffer[txtpos - 1] > 'Z')
				buffer[txtpos - 1] = 'Z';
		}
		else if (taps & CONTROL_B1)
		{
			if (txtpos >= 3)
			{
				// End if you ran out of space or if you just mashed A.
				ham_strcpy(me->name, buffer);
				return 0;
			}
			else if (txtpos > 0)
			{
				buffer[txtpos]=buffer[txtpos-1];
				txtpos++;
				buffer[txtpos]='\0';
			}
		}

		*lastTime-=TIME_PER_FRAME;
	}
	return 1;
}

void RenderNameEntry(MGLDraw *mgl)
{
	char s[40];

	mgl->ClearScreen();
	RenderFireworks(mgl);

	CenterPrint(320,2,"CONGRATULATIONS!",0,2);
	if(me->mode!=WORLD_BOSSBASH)
		sprintf(s,"You've earned the #%d high score!",position+1);
	else
		sprintf(s,"You've earned the high score!");
	CenterPrint(320,100,s,0,2);

	CenterPrint(320,150,"Enter your initials:",0,2);

	if(flip)
	{
		sprintf(s,"%s-",buffer);
		Print(280,200,s,0,2);
	}
	else
	{
		sprintf(s,"%s",buffer);
		Print(280,200,s,0,2);
	}
}

TASK(void) NameEntry(MGLDraw *mgl)
{
	byte b;
	dword now,then,hangon;
	int lastTime=1;

	EndClock();
	hangon=TimeLength();
	now=timeGetTime();

	InitNameEntry();
	b=1;
	while(b==1)
	{
		lastTime+=TimeLength();
		StartClock();
		b=UpdateNameEntry(&lastTime);
		RenderNameEntry(mgl);
		AWAIT mgl->Flip();
		if(!mgl->Process())
		{
			CO_RETURN;
		}
		EndClock();
	}
	ExitNameEntry();
	then=timeGetTime();
	ResetClock(hangon);
}

//----------------------------------------
// the high score pages themselves

static byte curMode;
static byte curScore;

void InitHighScore(void)
{
	GetDisplayMGL()->LastKeyPressed();
	GetTaps();
	curMode=WORLD_NORMAL;
	curScore=0;
	oldc=255;
	deleting=0;
	InitPlasma(0);
}

void ExitHighScore(void)
{
	SaveOptions();
}

byte UpdateHighScore(int *lastTime)
{
	byte c;
	int i;
	byte colors[]={0,1,2,3,4,6,5,7,8};

	if(*lastTime>TIME_PER_FRAME*30)
		*lastTime=TIME_PER_FRAME*30;

	while(*lastTime>=TIME_PER_FRAME)
	{
		flip=1-flip;
		c=GetControls()|GetArrows();
		byte taps = c & ~oldc;

		if(!deleting)
		{
			if(taps & CONTROL_UP)
			{
				curScore--;
				if(curScore>14)
					curScore=14;
				MakeNormalSound(SND_MENUCLICK);
			}
			if(taps & CONTROL_DN)
			{
				curScore++;
				if(curScore>14)
					curScore=0;
				MakeNormalSound(SND_MENUCLICK);
			}
			if(taps & CONTROL_LF)
			{
				while(1)
				{
					curMode--;
					if(curMode>WORLD_REMIX)
						curMode=WORLD_REMIX;
					if(curMode==WORLD_REMIX && opt.remixMode)
						break;
					if(curMode==WORLD_SURVIVAL && opt.modes[MODE_SURVIVAL])
						break;
					if(curMode==WORLD_INFSURV && opt.modes[MODE_SURVIVAL] && opt.meritBadge[BADGE_SURVCOMBO])
						break;
					if(curMode==WORLD_BOSSBASH && opt.modes[MODE_BOSSBASH])
						break;
					if(curMode==WORLD_LOONYBALL && opt.modes[MODE_LOONYBALL])
						break;
					if(curMode==WORLD_BOWLING && opt.modes[MODE_BOWLING])
						break;
					if(curMode==WORLD_NORMAL)
						break;
				}
				InitPlasma(colors[curMode]);
				MakeNormalSound(SND_MENUCLICK);
			}
			if(taps & CONTROL_RT)
			{
				while(1)
				{
					curMode++;
					if(curMode>WORLD_REMIX)
						curMode=WORLD_NORMAL;
					if(curMode==WORLD_REMIX && opt.remixMode)
						break;
					if(curMode==WORLD_SURVIVAL && opt.modes[MODE_SURVIVAL])
						break;
					if(curMode==WORLD_INFSURV && opt.modes[MODE_SURVIVAL] && opt.meritBadge[BADGE_SURVCOMBO])
						break;
					if(curMode==WORLD_BOSSBASH && opt.modes[MODE_BOSSBASH])
						break;
					if(curMode==WORLD_LOONYBALL && opt.modes[MODE_LOONYBALL])
						break;
					if(curMode==WORLD_BOWLING && opt.modes[MODE_BOWLING])
						break;
					if(curMode==WORLD_NORMAL)
						break;
				}
				InitPlasma(colors[curMode]);
				MakeNormalSound(SND_MENUCLICK);
			}
			if(taps & CONTROL_B1)
			{
				if(opt.score[curMode][curScore].mode==255)
					MakeNormalSound(SND_MENUCANCEL);
				else
				{
					MakeNormalSound(SND_MENUSELECT);
					deleting=1;
					delCursor=1;
				}
			}
		}
		else
		{
			if(taps & (CONTROL_UP | CONTROL_DN))
			{
				delCursor=1-delCursor;
				MakeNormalSound(SND_MENUCLICK);
			}
			if(taps & CONTROL_B1)
			{
				if(delCursor==0)
				{
					MakeNormalSound(SND_SWAMPDIE);
					if(curMode!=WORLD_BOSSBASH)
					{
						for(i=curScore;i<14;i++)
						{
							opt.score[curMode][i]=opt.score[curMode][i+1];
						}
						opt.score[curMode][14].mode=255;
					}
					else	// boss bash is not an ordered list
						opt.score[curMode][curScore].mode=255;
				}
				else
					MakeNormalSound(SND_MENUCANCEL);
				deleting=0;
			}
		}

		oldc=c;

		c=GetDisplayMGL()->LastKeyPressed();
		if(c==27 || (taps & CONTROL_B2))
		{
			if(deleting)
			{
				deleting=0;
			}
			else
				return 0;
		}

		UpdatePlasma();

		*lastTime-=TIME_PER_FRAME;
	}
	return 1;
}

void RenderHighScore(MGLDraw *mgl)
{
	int i;
	char s[32];
	int wid;
	byte* pos;
	byte colors[]={0,1,2,3,4,6,5,7,8};

	wid=mgl->GetWidth();
	pos=mgl->GetScreen()+20*wid;
	for(i=20;i<480-20;i++)
	{
		memset(pos,colors[curMode]*32+2,640);
		pos+=wid;
	}

	RenderPlasma(mgl,0);
	RenderPlasma3(mgl,1);

	CenterPrintGlow(320,-2,"High Scores",0,2);
	DrawBox(0,85,639,86,31);

	// only even suggest the presence of multimodes if there are some
	if(opt.modes[MODE_SURVIVAL])
	{
		DrawBox(0,54,639,54,31);
		CenterPrintGlow(320,56,modeText[curMode],0,0);
		if(flip)
		{
			Print(2,56,"<<<",0,0);
			RightPrint(638,56,">>>",0,0);
		}
		else
		{
			PrintColor(2,56,"<<<",colors[curMode],-10,0);
			RightPrintColor(638,56,">>>",colors[curMode],-10,0);
		}
		PrintGlow(2,375,"Left or right switches mode",0,0);
	}
	PrintGlow(2,400,"Press up or down to move",0,0);
	PrintGlow(2,425,"Enter to delete score",0,0);
	PrintGlow(2,450,ShowGamepadText() ? "Weapon to exit" : "ESC to exit",0,0);
	for(i=0;i<15;i++)
	{
		if(opt.score[curMode][i].mode==curMode)
		{
			sprintf(s,"%2d.",i+1);
			if(curScore!=i)
			{
				RightPrintColor(295,i*25+90,s,colors[curMode],-10,0);
				PrintColor(300,i*25+90,opt.score[curMode][i].name,colors[curMode],-10,0);
			}
			else
			{
				RightPrintGlow(295,i*25+90,s,0,0);
				PrintGlow(300,i*25+90,opt.score[curMode][i].name,0,0);
			}
			ShowHighScore(390,i*25+90,opt.score[curMode][i].score,(curScore==i),colors[curMode],mgl);
			if(curScore==i)
				ShowScoreStats(2,90,&opt.score[curMode][i],mgl);
		}
		else
		{
			sprintf(s,"%2d.",i+1);
			RightPrintColor(295,i*25+90,s,colors[curMode]-colors[curMode]*(curScore==i),-10+10*(curScore==i),0);
			PrintColor(300,i*25+90,"---",colors[curMode]-colors[curMode]*(curScore==i),-10+10*(curScore==i),0);
		}
	}

	if(deleting)
	{
		DrawFillBox(200,150,440,270,0);
		DrawBox(200,150,440,270,31);
		CenterPrintGlow(320,152,"Delete this high score?",0,0);
		CenterPrintColor(320,200,"Yes",colors[curMode]-colors[curMode]*(delCursor==0),-10+10*(delCursor==0),0);
		CenterPrintColor(320,225,"No",colors[curMode]-colors[curMode]*(delCursor==1),-10+10*(delCursor==1),0);
	}
}

TASK(void) HighScore(MGLDraw *mgl)
{
	byte b;
	dword now,then,hangon;
	int lastTime=1;

	EndClock();
	hangon=TimeLength();
	now=timeGetTime();

	InitHighScore();
	b=1;
	while(b==1)
	{
		lastTime+=TimeLength();
		StartClock();
		b=UpdateHighScore(&lastTime);
		RenderHighScore(mgl);
		AWAIT mgl->Flip();
		if(!mgl->Process())
		{
			CO_RETURN;
		}
		EndClock();
	}
	ExitHighScore();
	then=timeGetTime();
	ResetClock(hangon);
}

//-----------------------------------------------
// other high score functions, like calculating

void ResetHighScores(void)
{
	int i,j;

	for(i=0;i<8;i++)
		for(j=0;j<15;j++)
			opt.score[i][j].mode=255;
}

TASK(void) CheckForHighScore(highScore_t myScore)
{
	int i,j;

	if(myScore.mode!=WORLD_BOSSBASH)
	{
		for(i=0;i<15;i++)
		{
			if(myScore.score>=opt.score[myScore.mode][i].score || opt.score[myScore.mode][i].mode==255)
			{
				// shift the scores down
				for(j=14;j>i;j--)
				{
					opt.score[myScore.mode][j]=opt.score[myScore.mode][j-1];
				}
				opt.score[myScore.mode][i]=myScore;
				position=i;
				me=&opt.score[myScore.mode][i];
				Steam()->UploadHighScore(me, 255);
				AWAIT NameEntry(GetDisplayMGL());
				CO_RETURN;
			}
		}
	}
	else
	{
		// replace only the specific one
		i=player.levelNum-1;
		if(myScore.score>=opt.score[myScore.mode][i].score || opt.score[myScore.mode][i].mode==255)
		{
			opt.score[myScore.mode][i]=myScore;
			position=i;
			me=&opt.score[myScore.mode][i];
			Steam()->UploadHighScore(me, player.levelNum);
			AWAIT NameEntry(GetDisplayMGL());
			CO_RETURN;
		}
	}
}

void MakeHighScore(highScore_t *me,int score)
{
	if(player.worldNum==WORLD_SURVIVAL && (player.cheatsOn&PC_INFSURV))
		me->mode=WORLD_INFSURV;
	else
		me->mode=player.worldNum;

	switch(me->mode)
	{
		case WORLD_NORMAL:
		case WORLD_REMIX:
			me->score=score;
			me->time=player.timeToFinish;
			me->spclValue=(int)(CalcPercent(&player)*FIXAMT);
			me->monsterPoints=player.monsterPoints;
			me->gemsGotten=player.gemsGotten;
			me->bestCombo=player.bestCombo;
			me->guysKilled=player.guysKilled;
			me->shotsFired=player.shotsFired;
			me->specialShotsFired=player.specialShotsFired;
			me->numSaves=player.numSaves;
			me->hitsTaken=player.hitsTaken;
			break;
		case WORLD_INFSURV:
		case WORLD_SURVIVAL:
			me->score=score;
			me->numSaves=player.numSaves;
			me->time=player.timeToFinish;
			me->monsterPoints=player.monsterPoints;
			me->gemsGotten=player.gemsGotten;
			me->bestCombo=player.bestCombo;
			me->guysKilled=player.guysKilled;
			me->shotsFired=player.shotsFired;
			me->specialShotsFired=player.specialShotsFired;
			me->hitsTaken=player.hitsTaken;
			break;
		case WORLD_BOWLING:
			me->score=score;
			me->time=player.timeToFinish;
			me->monsterPoints=player.monsterPoints;
			me->gemsGotten=player.gemsGotten;
			me->specialShotsFired=player.specialShotsFired;
			me->shotsFired=player.shotsFired;
			break;
		case WORLD_LOONYBALL:
			me->score=score;
			me->time=player.timeToFinish;
			me->gemsGotten=player.gemsGotten;
			me->numSaves=player.numSaves;
			me->bestCombo=player.levelNum;
			me->guysKilled=player.guysKilled;
			break;
		case WORLD_BOSSBASH:
			me->score=score;
			me->time=player.timeToFinish;
			me->gemsGotten=BashPower();
			me->hitsTaken=player.hitsTaken;
			me->spclValue=player.levelNum-1;
			break;
	}
}

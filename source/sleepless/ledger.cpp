#include "winpch.h"
#include "ledger.h"
#include "progress.h"
#include "player.h"
#include "goal.h"
#include "guy.h"
#include "special.h"
#include "shop.h"

static byte *backgd;
static char entry[48];
static byte cursorBright;

void InitLedger(MGLDraw *mgl)
{
	int i;

	entry[0]='\0';
	cursorBright=0;

	mgl->LoadBMP("graphics/pause.bmp");
	backgd=(byte *)malloc(640*480);

	for(i=0;i<480;i++)
		memcpy(&backgd[i*640],&mgl->GetScreen()[i*mgl->GetWidth()],640);
}

void ExitLedger(void)
{
	free(backgd);
}

void LedgerHappen(void)
{
	int i,m;
	char tmp[32];
	Guy *g;

#ifdef DEMO
	byte summonable[]={MONS_BONEHEAD,MONS_BAT,MONS_SPIDER,MONS_BIGSPDR,MONS_ZOMBIE,MONS_FROG,
						MONS_PUMPKIN,MONS_GLASSJAW,MONS_FROG2,MONS_FIREBUG,MONS_FIREBAT,
						MONS_GHOST,MONS_MADCOW,
						MONS_LAZYBONE,255};
#else
	byte summonable[]={MONS_BONEHEAD,MONS_BAT,MONS_SPIDER,MONS_BIGSPDR,MONS_ZOMBIE,MONS_FROG,
						MONS_PUMPKIN,MONS_BABYTHING,MONS_GLASSJAW,MONS_FROG2,MONS_FIREBUG,MONS_FIREBAT,
						MONS_STICKTREE,MONS_ROLLER,MONS_BIGHEAD,MONS_SPARK,MONS_GHOST,MONS_SPARKY,MONS_MADCOW,
						MONS_ROLLER2,MONS_DARKNESS,MONS_BLASTER,MONS_CRAZYBONE,MONS_LAZYBONE,MONS_STAREYBAT,
						MONS_BIGHEAD2,MONS_BIGHEAD3,MONS_BONEHEAD2,MONS_PATCH,MONS_PATCH2,MONS_BEETLE,MONS_SHOCKGHOST,255};
#endif
	for(i=0;i<(int)strlen(entry);i++)
		if(entry[i]>='a' && entry[i]<='z')
			entry[i]+='A'-'a';

	if(!strcmp(entry,"CANDLE"))
	{
		if(player.journal[62]==0)
		{
			curMap->map[7+70*curMap->width].item=ITM_CANDLE;
			player.journal[62]=1;
			NewMessage("Mr. Candle, I presume?",60,1);
			return;
		}
	}
	if(!strcmp(entry,"HEADLESS HORSERADISH"))
	{
		NewMessage("Meh, it's been done.",60,1);
		return;
	}
	if(!strcmp(entry,"ICHABOD STEAMSHOVEL"))
	{
		CompleteGoal(19);
		NewMessage("Sorry, he can't make it.",60,1);
		return;
	}
	if(!strcmp(entry,"MR. KLEIN"))
	{
		NewMessage("He can't find his way here.",60,1);
		return;
	}
	m=0;
	while(summonable[m]!=255)
	{
		strcpy(tmp,MonsterName(summonable[m]));
		for(i=0;i<=(int)strlen(tmp);i++)
			if(tmp[i]>='a' && tmp[i]<='z')
				tmp[i]+='A'-'a';
		if(!strcmp(tmp,entry))
		{
			g=AddGuy((8*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
					   (70*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,
					   350*FIXAMT,summonable[m],2);
			if(g)
			{
				if(g->CanWalk(g->x,g->y,curMap,&curWorld))
				{
					g->item=ITM_NONE;
					MakeSound(SND_TELEPORT,g->x,g->y,SND_CUTOFF,1500);
					curMap->map[8+70*curMap->width].templight=34;
					g->bright=34;
				}
				else
					g->type=MONS_NONE;
			}
			return;
		}
		m++;
	}
}

byte UpdateLedger(int *lastTime,MGLDraw *mgl)
{
	char c;

	if(*lastTime>TIME_PER_FRAME*5)
		*lastTime=TIME_PER_FRAME*5;

	while(*lastTime>=TIME_PER_FRAME)
	{
		mgl->Process();

		cursorBright=1-cursorBright;

		*lastTime-=TIME_PER_FRAME;
	}

	c=mgl->LastKeyPressed();
	if(c==13)	// enter
	{
		MakeNormalSound(SND_MENUSELECT);
		LedgerHappen();
		return 1;
	}
	else if(c==27)
	{
		MakeNormalSound(SND_MENUSELECT);
		return 1;
	}
	else if((c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9') || c==' ' || c=='.' || c=='\'' || c=='!')
	{
		if(strlen(entry)<31)
		{
			entry[strlen(entry)+1]='\0';
			if(ShiftState())
			{
				if(c>='a' && c<='z')
					c+='A'-'a';
			}
			entry[strlen(entry)]=c;
			MakeNormalSound(SND_MENUCLICK);
		}
	}
	else if(c==8)
	{
		if(strlen(entry)>0)
		{
			entry[strlen(entry)-1]='\0';
			MakeNormalSound(SND_MENUCLICK);
		}
	}

	return 0;
}

static void RenderLine(int y,const char *txt,int num)
{
	PrintUnGlow(20,y,txt,2);
}

void RenderLedger(MGLDraw *mgl)
{
	int i;

	for(i=0;i<480;i++)
		memcpy(&mgl->GetScreen()[i*mgl->GetWidth()],&backgd[i*640],640);

	RenderLine(20,"Welcome to the Sleepless Inn!",0);
	RenderLine(60,"Check-in time:  Nowish",1);
	RenderLine(90,"Check-out time: NEVER!",2);

	PrintUnGlowRect(330,320,635,479,30,"NOTE: The management will not be responsible for any "
					"decapitation which occurs on or about our premises.  Guest agrees to assume all risk.",2);

	RenderLine(280,"Please sign our ledger:",3);

	mgl->Box(20,320,300,350,0);
	PrintUnGlow(25,322,entry,2);

	if(cursorBright)
		PrintUnGlow(25+GetStrLength(entry,2)+1,322,"_",2);
}

//----------------

void Ledger(MGLDraw *mgl)
{
	byte done=0;
	int lastTime=1;

	InitLedger(mgl);

	while(!done)
	{
		lastTime+=TimeLength();
		StartClock();

		done=UpdateLedger(&lastTime,mgl);
		RenderLedger(mgl);
		mgl->Flip();

		if(!mgl->Process())
			done=1;
		EndClock();
	}

	ExitLedger();
}

#include "worldselect.h"
#include "world.h"
#include "winpch.h"
#include "clock.h"
#include "game.h"
#include "music.h"
#include "dialogbits.h"
#include "shop.h"
#include "hiscore.h"
#include "lsdir.h"

#define WS_CONTINUE	0
#define WS_EXIT		1
#define WS_PLAY		2

#define MODE_PICKWORLD	0
#define MODE_SCROLL		1
#define MODE_MENU		2
#define MODE_VERIFY		3
#define MODE_VERIFY2	4

#define NAME_X	20
#define AUTH_X	300
#define PERCENT_X 580
#define GAP_HEIGHT	18

#define SCROLLBAR_HEIGHT 320
#define WORLDS_PER_SCREEN 18
#define CLICK_SCROLL_AMT (WORLDS_PER_SCREEN*3/4)

#define WBTN_HEIGHT		19

typedef struct worldDesc_t
{
	char fname[32];
	char name[32];
	char author[32];
	float percentage;
	byte complete;
	byte dimmed;
} worldDesc_t;

static char curName[32];
static byte mode;
static worldDesc_t *list;
static int numWorlds,worldDescSize,listPos,choice;
static byte *backgd;
static byte sortType,sortDir;
static int scrollY,scrollHeight,scrollOffset;
static int msx,msy;
static byte mouseB=255;
static sprite_set_t *wsSpr;
static char msBright,msDBright;
static score_t top3[3];
static byte level,scoreMode,numScores,noScoresAtAll;
static world_t tmpWorld;
static int mouseZ;
#ifdef WTG
static byte showFilenames = 1;
#elif defined(_DEBUG)
static byte showFilenames = 1;
#endif

#ifdef LEVELLIST
static FILE *levelF,*level2F,*authorF;
static int totalLCount;
#endif

void FlipEm(worldDesc_t *me,worldDesc_t *you)
{
	worldDesc_t tmp;

	memcpy(&tmp,me,sizeof(worldDesc_t));
	memcpy(me,you,sizeof(worldDesc_t));
	memcpy(you,&tmp,sizeof(worldDesc_t));
}

byte Compare(worldDesc_t *me,worldDesc_t *you,byte field,byte bkwds)
{
	byte f;
	char tmp1[32],tmp2[32];

	if(me->dimmed!=you->dimmed)
		return 0;	// no flipping between ones of different dim status
	if(!bkwds)
	{
		if(!strcmp(me->fname,"tutorial.hsw"))	// the tutorial comes before all else
			return 0;
		if(!strcmp(you->fname,"tutorial.hsw"))
		{
			FlipEm(me,you);
			return 1;
		}
	}
	else
	{
		if(!strcmp(you->fname,"tutorial.hsw"))	// the tutorial comes before all else (vice versa when backwards, so still on top)
			return 0;
		if(!strcmp(me->fname,"tutorial.hsw"))
		{
			FlipEm(me,you);
			return 1;
		}
	}

	f=0;
	switch(field)
	{
		case 0:
			strcpy(tmp1,me->name);
			strcpy(tmp2,you->name);
			f=(strcasecmp(tmp1,tmp2)>0);
			break;
		case 1:
			strcpy(tmp1,me->author);
			strcpy(tmp2,you->author);
			f=(strcasecmp(tmp1,tmp2)>0);
			break;
		case 2:
			f=(me->percentage>you->percentage);
			break;
	}

	if(f)
		FlipEm(me,you);

	return f;
}

void SortWorlds(byte field,byte backwards)
{
	int i;
	int flipped;

	flipped=1;

	while(flipped)
	{
		flipped=0;
		for(i=0;i<numWorlds-1;i++)
		{
			if(backwards)
			{
				if(Compare(&list[i+1],&list[i],field,1))
				{
					flipped++;
					if(choice==i)
						choice=i+1;
					else if(choice==i+1)
						choice=i;
				}
			}
			else
			{
				if(Compare(&list[i],&list[i+1],field,0))
				{
					flipped++;
					if(choice==i)
						choice=i+1;
					else if(choice==i+1)
						choice=i;
				}
			}
			if(list[i].dimmed && !list[i+1].dimmed)
			{
				flipped++;
				FlipEm(&list[i],&list[i+1]);
			}
		}
	}

	if(choice<listPos)
	{
		listPos=choice;
		if(listPos<0)
			listPos=0;
		if(listPos>numWorlds-WORLDS_PER_SCREEN)
			listPos=numWorlds-WORLDS_PER_SCREEN;
	}
	if(choice>=listPos+WORLDS_PER_SCREEN)
	{
		listPos=choice-WORLDS_PER_SCREEN+1;
		if(listPos<0)
			listPos=0;
		if(listPos>numWorlds-WORLDS_PER_SCREEN)
			listPos=numWorlds-WORLDS_PER_SCREEN;
	}
}

void InputWorld(const char *fname)
{
	char fullname[64];
	worldData_t *w;

	strcpy(list[numWorlds].fname,fname);
	sprintf(fullname,"worlds/%s",fname);

#ifdef LEVELLIST
	int i;
	world_t wor;
	dword d;

	LoadWorld(&wor,fullname);

	fprintf(authorF,"%s\n",wor.author);
	fprintf(levelF,"W:%s\n",wor.map[0]->name);
	for(i=0;i<wor.numMaps;i++)
	{
		//fprintf(levelF,"lvl%04d: %s\n",totalLCount++,wor.map[i]->name);
		d=ChecksumMap(wor.map[i]);
		fwrite(&d,sizeof(dword),1,level2F);
		if(!(wor.map[i]->flags&MAP_HUB))
			fprintf(levelF,"L: %s|%d\n",wor.map[i]->name,d);
	}


	FreeWorld(&wor);
#endif
	GetWorldName(fullname,list[numWorlds].name,list[numWorlds].author);
	w=GetWorldProgressNoCreate(list[numWorlds].fname);

	if(w)
		list[numWorlds].percentage=w->percentage;
	else
		list[numWorlds].percentage=0.0f;

	list[numWorlds].dimmed=(!CanPlayWorld(fname));

	numWorlds++;
	if(numWorlds==worldDescSize)
	{
		worldDescSize+=16;
		list=(worldDesc_t *)realloc(list,sizeof(worldDesc_t)*worldDescSize);
	}
	profile.progress.totalWorlds=numWorlds;
}

void ScanWorlds(void)
{
	int count,done;

#ifdef LEVELLIST
	levelF=fopen("levellist.txt","wt");
	level2F=fopen("worlds/levels.dat","wt");
	authorF=fopen("authorlist.txt","wt");
	totalLCount=0;
#endif
	// count up how many there are to deal with
	count=0;

	for (const char* name : filterdir("worlds", ".hsw", 32))
	{
		// rule out the backup worlds, so they don't show up
		if((strcmp(name,"backup_load.hsw")) &&
		   (strcmp(name,"backup_exit.hsw")) &&
		   (strcmp(name,"backup_save.hsw")))
			count++;
	}

	done=0;

	dword start = timeGetTime();
	for (const char* name : filterdir("worlds", ".hsw", 32))
	{
		// rule out the backup worlds, so they don't show up
		if((strcmp(name,"backup_load.hsw")) &&
		   (strcmp(name,"backup_exit.hsw")) &&
		   (strcmp(name,"backup_save.hsw")))
		{
			InputWorld(name);
			done++;

			dword now = timeGetTime();
			if (now - start > 50)  // 50 ms = 20 fps
			{
				start = now;
				GetDisplayMGL()->FillBox(20,440,20+(done*600)/count,450,32*1+16);
				GetDisplayMGL()->Flip();
			}
		}
	}
#ifdef LEVELLIST
	fclose(levelF);
	fclose(level2F);
	fclose(authorF);
#endif
}

void CalcScrollBar(void)
{
	scrollHeight=SCROLLBAR_HEIGHT*WORLDS_PER_SCREEN/numWorlds;
	if(scrollHeight<10)
		scrollHeight=10;
	if(scrollHeight>=SCROLLBAR_HEIGHT)
		scrollHeight=SCROLLBAR_HEIGHT-1;

	scrollY=SCROLLBAR_HEIGHT*listPos/numWorlds;
	if(scrollY+scrollHeight>SCROLLBAR_HEIGHT)
		scrollY=SCROLLBAR_HEIGHT-scrollHeight;
}

void ReverseCalcScrollBar(void)
{
	if(scrollY<0)
		scrollY=0;
	if(scrollY+scrollHeight+1>SCROLLBAR_HEIGHT)
		scrollY=SCROLLBAR_HEIGHT-scrollHeight-1;

	if(scrollY>0)
		listPos=scrollY*numWorlds/SCROLLBAR_HEIGHT+1;
	else
		listPos=0;
}

void FetchScores(byte backwards)
{
	byte curL;

	if(noScoresAtAll)
		return;

	if(level>=tmpWorld.numMaps)
		level=0;

	curL=level;

	while(1)
	{
		if(scoreMode==0)
			numScores=GetTopScores(top3,tmpWorld.map[level]);
		else
			numScores=GetTopTimes(top3,tmpWorld.map[level]);
		if(numScores)
			break;

		if(!backwards)
		{
			level++;
			if(level>=tmpWorld.numMaps)
				level=0;
			if(level==curL)
			{
				noScoresAtAll=1;
				break;
			}
			if(tmpWorld.map[level]->flags&MAP_HUB)
			{
				level++;
				if(level>=tmpWorld.numMaps)
					level=0;
			}
		}
		else
		{
			level--;
			if(level>=tmpWorld.numMaps)
				level=tmpWorld.numMaps-1;
			if(level==curL)
			{
				noScoresAtAll=1;
				break;
			}
			if(tmpWorld.map[level]->flags&MAP_HUB)
			{
				level--;
				if(level>=tmpWorld.numMaps)
					level=tmpWorld.numMaps-1;
			}
		}
		if(level==curL)
		{
			noScoresAtAll=1;
			break;
		}
	}
}

void SelectLastWorld(void)
{
	int i;
	char s[64];

	choice=0;

	for(i=0;i<numWorlds;i++)
		if(!strcmp(list[i].fname,profile.lastWorld))
			choice=i;

	if(listPos<=choice-WORLDS_PER_SCREEN)
		listPos=choice-WORLDS_PER_SCREEN+1;
	if(listPos>choice)
		listPos=choice;

	sprintf(s,"worlds/%s",list[choice].fname);
	LoadWorld(&tmpWorld,s);
	level=0;
	scoreMode=0;
	noScoresAtAll=0;
	FetchScores(0);
}

void MoveToNewWorld(void)
{
	char s[64];

	FreeWorld(&tmpWorld);
	sprintf(s,"worlds/%s",list[choice].fname);
	LoadWorld(&tmpWorld,s);
	level=0;
	noScoresAtAll=0;
	FetchScores(0);
}

void InitWorldSelect(MGLDraw *mgl)
{
	int i;
	char s[512];
	char dyk[][512]={"Due to the graphics engine's color liminations, the color orange does not appear in-game.",
	"Bouapha is from the town of of Merced, CA.",
	"The four keychains are references to enemies/items/etc. from the original SPISPOPD.",
	"Currently here are a total of [insert # of monsters] monsters in this version of Supreme With Cheese.",
	"5",
	"6",
	"7",
	"8",
	"9",
	"10",
	"11",
	"12",
	"13",
	"14",};

	mgl->LoadBMP("graphics/profmenu.bmp");
	backgd=(byte *)malloc(640*480);

	for(i=0;i<480;i++)
		memcpy(&backgd[i*640],&mgl->GetScreen()[i*mgl->GetWidth()],640);

	PrintGlow(20,20,"Loading...",0,2);
	PrintGlow(20,360,"Did You Know?",0,2);
	sprintf(s,dyk[Random(14)]);
	PrintGlow(20,380,s,0,1);
	mgl->Flip();

	sortType=0;
	sortDir=0;
	listPos=0;
	worldDescSize=16;
	numWorlds=0;

	list=(worldDesc_t *)malloc(sizeof(worldDesc_t)*16);
	ScanWorlds();
	SortWorlds(sortType,sortDir);
	SelectLastWorld();
	CalcScrollBar();
	mgl->GetMouse(&msx,&msy);
	mode=MODE_PICKWORLD;
	wsSpr=new sprite_set_t("graphics/pause.jsp");
	msBright=0;
	msDBright=1;
	PlaySongForce("SWC_02_worldpicker.ogg");
	mouseZ=mgl->mouse_z;
}

void ExitWorldSelect(void)
{
	free(backgd);
	free(list);
	FreeWorld(&tmpWorld);
	delete wsSpr;
}

byte UpdateWorldSelect(int *lastTime,MGLDraw *mgl)
{
	static byte oldc=255;
	char c;
	int i;

	if(*lastTime>TIME_PER_FRAME*5)
		*lastTime=TIME_PER_FRAME*5;

	mgl->GetMouse(&msx,&msy);

	while(*lastTime>=TIME_PER_FRAME)
	{
		msBright+=msDBright;
		if(msBright>10)
			msDBright=-1;
		if(msBright<-2)
			msDBright=1;
		mgl->Process();
		*lastTime-=TIME_PER_FRAME;
	}

	c=GetControls()|GetArrows();

	if(mode==MODE_PICKWORLD)
	{
		if((c&CONTROL_UP) && !(oldc&CONTROL_UP))
		{
			if(choice>0)
			{
				choice--;
				MoveToNewWorld();
			}
			if(choice<listPos)
			{
				listPos--;
				CalcScrollBar();
			}
		}
		if((c&CONTROL_DN) && !(oldc&CONTROL_DN))
		{
			if(choice<numWorlds-1)
			{
				choice++;
				MoveToNewWorld();
			}
			if(choice>listPos+17)
			{
				listPos++;
				CalcScrollBar();
			}
		}
		if((c&CONTROL_B1) && !(oldc&CONTROL_B1))
		{
			if(list[choice].dimmed)
				MakeNormalSound(SND_TURRETBZZT);
			else
			{
				oldc=255;
				return WS_PLAY;
			}
		}

		int mv=mouseZ-mgl->mouse_z;
		mouseZ=mgl->mouse_z;
		if(mv<0)
		{
			listPos+=mv;
			if(listPos<0)
				listPos=0;
			CalcScrollBar();
		}
		else if(mv>0)
		{
			listPos+=mv;
			if(listPos>numWorlds-WORLDS_PER_SCREEN)
				listPos=numWorlds-WORLDS_PER_SCREEN;
			if(listPos<0)
				listPos=0;
			CalcScrollBar();
		}

		if((mgl->mouse_b&1) && !(mouseB&1))
		{
			// clicking on a world
			for(i=0;i<18;i++)
			{
				if(i+listPos<numWorlds)
				{
					if(PointInRect(msx,msy,17,39+i*GAP_HEIGHT,599,39+GAP_HEIGHT+i*GAP_HEIGHT-1))
					{
						if(i+listPos!=choice)
						{
							choice=i+listPos;
							MoveToNewWorld();
						}
						else
						{
							if(list[choice].dimmed)
								MakeNormalSound(SND_TURRETBZZT);
							else
							{
								oldc=255;
								return WS_PLAY;
							}
						}
					}
				}
			}
			// clicking on the sort possibilities
			if(PointInRect(msx,msy,NAME_X,20,AUTH_X-20,37))
			{
				if(sortType==0)
					sortDir=1-sortDir;
				else
				{
					sortType=0;
					sortDir=0;
				}
				SortWorlds(sortType,sortDir);
				CalcScrollBar();
			}
			if(PointInRect(msx,msy,AUTH_X,20,PERCENT_X-GetStrLength("Complete",2)-20,37))
			{
				if(sortType==1)
					sortDir=1-sortDir;
				else
				{
					sortType=1;
					sortDir=0;
				}
				SortWorlds(sortType,sortDir);
				CalcScrollBar();
			}
			if(PointInRect(msx,msy,PERCENT_X-GetStrLength("Complete",2),20,PERCENT_X+4,37))
			{
				if(sortType==2)
					sortDir=1-sortDir;
				else
				{
					sortType=2;
					sortDir=0;
				}
				SortWorlds(sortType,sortDir);
				CalcScrollBar();
			}
			// scroll bar click
			if(PointInRect(msx,msy,605,40,620,42+SCROLLBAR_HEIGHT))
			{
				if(msy>=scrollY+41 && msy<=41+scrollY+scrollHeight-1)
				{
					mode=MODE_SCROLL;
					scrollOffset=(msy-scrollY);
				}
				else if(msy<scrollY+41)
				{
					listPos-=CLICK_SCROLL_AMT;
					if(listPos<0)
						listPos=0;
					CalcScrollBar();
				}
				else if(msy>41+scrollY+scrollHeight-1)
				{
					listPos+=CLICK_SCROLL_AMT;
					if(listPos>numWorlds-WORLDS_PER_SCREEN)
						listPos=numWorlds-WORLDS_PER_SCREEN;
					if(listPos<0)
						listPos=0;
					CalcScrollBar();
				}
			}

			// play world
			if(PointInRect(msx,msy,20,371,20+150,371+WBTN_HEIGHT))
			{
				if(list[choice].dimmed)
					MakeNormalSound(SND_TURRETBZZT);
				else
				{
					oldc=255;
					return WS_PLAY;
				}
			}
			// reset world
			else if(PointInRect(msx,msy,20,395,20+150,395+WBTN_HEIGHT))
			{
				if(list[choice].dimmed)
					MakeNormalSound(SND_TURRETBZZT);
				else
				{
					mode=MODE_VERIFY;
				}
			}
			// reset high scores
			else if(PointInRect(msx,msy,20,419,20+150,419+WBTN_HEIGHT))
			{
				if(list[choice].dimmed)
					MakeNormalSound(SND_TURRETBZZT);
				else
				{
					mode=MODE_VERIFY2;
				}
			}
			// exit to menu
			else if(PointInRect(msx,msy,20,443,20+150,443+WBTN_HEIGHT))
			{
				oldc=255;
				return WS_EXIT;
			}

			// hi score buttons
			if(PointInRect(msx,msy,180,371,180+150,371+WBTN_HEIGHT) && !list[choice].dimmed)
			{
				scoreMode=1-scoreMode;
				noScoresAtAll=0;
				FetchScores(0);
			}
			else if(PointInRect(msx,msy,335,371,335+20,371+WBTN_HEIGHT) && !list[choice].dimmed)
			{
				level--;
				if(level>=tmpWorld.numMaps)
					level=tmpWorld.numMaps-1;
				FetchScores(1);
			}
			else if(PointInRect(msx,msy,592,371,592+20,371+WBTN_HEIGHT) && !list[choice].dimmed)
			{
				level++;
				if(level>=tmpWorld.numMaps)
					level=0;
				FetchScores(0);
			}
		}
	}
	else if(mode==MODE_SCROLL)
	{
		if((mgl->mouse_b&1)==0)
			mode=MODE_PICKWORLD;
		else
		{
			scrollY=msy-scrollOffset;
			ReverseCalcScrollBar();
		}
	}
	else if(mode==MODE_VERIFY || mode==MODE_VERIFY2)
	{
		if((mgl->mouse_b&1) && !(mouseB&1))
		{
			if(PointInRect(msx,msy,70,270,70+50,270+WBTN_HEIGHT))
			{
				MakeNormalSound(SND_MENUSELECT);
				if(mode==MODE_VERIFY)
					EraseWorldProgress(list[choice].fname);
				else
					EraseHighScores(&tmpWorld);
				ExitWorldSelect();
				InitWorldSelect(mgl);
			}
			else if(PointInRect(msx,msy,600-30-50,270,600-30-50+50,270+WBTN_HEIGHT))
			{
				MakeNormalSound(SND_MENUSELECT);
				mode=MODE_PICKWORLD;
			}
		}
	}

	oldc=c;

	c=mgl->LastKeyPressed();

#if defined(WTG) || defined(_DEBUG)
	if (c == 'A')
		showFilenames = !showFilenames;
#endif

	if(c==27)
	{
		oldc=255;
		return WS_EXIT;
	}

	mouseB=mgl->mouse_b;
	return WS_CONTINUE;
}

void RenderWorldSelectButton(int x,int y,int wid,const char *txt,MGLDraw *mgl)
{
	if(PointInRect(msx,msy,x,y,x+wid,y+WBTN_HEIGHT))
	{
		mgl->Box(x,y,x+wid,y+WBTN_HEIGHT,32+31);
		mgl->FillBox(x+1,y+1,x+wid-1,y+WBTN_HEIGHT-1,32+8);
	}
	else
		mgl->Box(x,y,x+wid,y+WBTN_HEIGHT,32+16);

	PrintGlow(x+2,y+2,txt,0,2);
}

void RenderWorldSelect(MGLDraw *mgl)
{
	int i,msx2,msy2;
	char s[16],b;

	for(i=0;i<480;i++)
		memcpy(&mgl->GetScreen()[i*mgl->GetWidth()],&backgd[i*640],640);

	PrintGlow(NAME_X,20,"World",6,2);
	PrintGlow(AUTH_X,20,"Author",6,2);
	PrintGlow(PERCENT_X-GetStrLength("Complete",2),20,"Complete",6,2);

	// lines to separate stuff
	mgl->FillBox(20,37,620,37,32+16);
	mgl->FillBox(20,364,620,364,32+16);

	// the world list
	for(i=0;i<18;i++)
	{
		if(i+listPos<numWorlds)
		{
			if(choice==i+listPos)
				mgl->FillBox(17,39+i*GAP_HEIGHT,599,39+GAP_HEIGHT+i*GAP_HEIGHT-1,32+8);

			if(PointInRect(msx,msy,17,39+i*GAP_HEIGHT,599,39+GAP_HEIGHT+i*GAP_HEIGHT-1))
			{
				mgl->Box(17,39+i*GAP_HEIGHT,599,39+GAP_HEIGHT+i*GAP_HEIGHT-1,32+16);
			}
			if(list[i+listPos].dimmed)
				b=-10;
			else
				b=0;

#if defined(WTG) || defined(_DEBUG)
			if (showFilenames)
			{
				PrintGlow(NAME_X,40+i*GAP_HEIGHT,list[i+listPos].name,b,1);
				PrintGlow(AUTH_X-70,40+i*GAP_HEIGHT,list[i+listPos].fname,b,1);
				PrintGlow(AUTH_X+100,40+i*GAP_HEIGHT,list[i+listPos].author,b,1);
			}
			else
#endif
			{
				PrintGlow(NAME_X,40+i*GAP_HEIGHT,list[i+listPos].name,b,2);
				PrintGlow(AUTH_X,40+i*GAP_HEIGHT,list[i+listPos].author,b,2);
			}
			if(list[i+listPos].percentage==0.0f)
				strcpy(s,"0%");
			else if(list[i+listPos].percentage==100.0f)
				strcpy(s,"100%");
			else
				sprintf(s,"%0.1f%%",list[i+listPos].percentage);
			PrintGlow(PERCENT_X-GetStrLength(s,2),40+i*GAP_HEIGHT,s,b,2);
		}
		else
			break;
	}

	// the scrollbar
	mgl->Box(605,39,620,39+3+SCROLLBAR_HEIGHT,32+16);
	mgl->FillBox(606,40,619,39+2+SCROLLBAR_HEIGHT,32+6);
	mgl->FillBox(607,41+scrollY,618,41+scrollY+scrollHeight,32+20);

	// buttons
	RenderWorldSelectButton(20,371,150,"Play This World",mgl);
	RenderWorldSelectButton(20,395,150,"Reset This World",mgl);
	RenderWorldSelectButton(20,419,150,"Reset High Scores",mgl);
	RenderWorldSelectButton(20,443,150,"Exit To Menu",mgl);

	if(list[choice].dimmed)
	{
		PrintGlow(200,411,"You need to buy this world in the SpisMall to play it!",0,2);
	}
	else
	{
		// high score section
		if(scoreMode==1)
			RenderWorldSelectButton(180,371,150,"Record Times",mgl);
		else
			RenderWorldSelectButton(180,371,150,"Record Scores",mgl);

		if(!noScoresAtAll)
		{
			RenderWorldSelectButton(335,371,20,"<<",mgl);
			PrintGlowLimited(359,373,590,tmpWorld.map[level]->name,0,2);
			RenderWorldSelectButton(592,371,20,">>",mgl);

			// now the scores themselves
			for(i=0;i<3;i++)
			{
				s[2]='\0';
				s[1]='.';
				s[0]=i+'1';	// equivalent to sprintf(s,"%d.",i);
				PrintGlow(335,395+i*20,s,0,2);
				if(i>=numScores)
					PrintGlow(350,395+i*20,"?????",0,2);
				else
					PrintGlow(350,395+i*20,top3[i].name,0,2);
				if(scoreMode==1)
				{
					if(i>=numScores)
						strcpy(s,"0:00.00");
					else
						sprintf(s,"%lu:%02lu.%02lu",top3[i].score/(30*60),(top3[i].score/30)%60,(top3[i].score%30)*100/30);
				}
				else
				{
					if(i>=numScores)
						strcpy(s,"0");
					else
						sprintf(s,"%lu",top3[i].score);
				}
				PrintGlow(615-GetStrLength(s,2),395+i*20,s,0,2);
			}
		}
	}

	if(mode==MODE_VERIFY || mode==MODE_VERIFY2)
	{
		mgl->FillBox(40,150,600,300,32*1+4);
		mgl->Box(40,150,600,300,32*1+16);
		if(mode==MODE_VERIFY)
			PrintGlowRect(50,160,590,250,18,"Are you sure you want to reset this world?  That will erase all of your progress "
										   "in the world, but leave high scores unchanged.",2);
		else
			PrintGlowRect(50,160,590,250,18,"Are you sure you want to reset the high scores for this world?  That will erase "
										   "the high scores, but keep your progress in the world.",2);
		RenderWorldSelectButton(70,270,50,"Yes",mgl);
		RenderWorldSelectButton(600-30-50,270,50,"No",mgl);
	}

	SetSpriteConstraints(13,13,627,467);
	msx2=msx;
	msy2=msy;
	if(msx2<13)
		msx2=13;
	if(msy2<13)
		msy2=13;
	if(msx2>622)
		msx2=622;
	if(msy2>462)
		msy2=462;
	wsSpr->GetSprite(0)->DrawBright(msx2,msy2,mgl,msBright/2);
	SetSpriteConstraints(0,0,639,479);
}

byte WorldSelectMenu(MGLDraw *mgl)
{
	byte done=WS_CONTINUE;
	int lastTime=1;
	char fname[32];

	InitWorldSelect(mgl);

	if(numWorlds==0)
		return 1;	// just skip it if there are no worlds!

	while(done==WS_CONTINUE)
	{
		lastTime+=TimeLength();
		StartClock();
		done=UpdateWorldSelect(&lastTime,mgl);
		RenderWorldSelect(mgl);
		mgl->Flip();

		if(!mgl->Process())
			done=WS_EXIT;

		if(done==WS_PLAY)
		{
			strcpy(fname,list[choice].fname);
			ExitWorldSelect();
			if(PlayWorld(mgl,fname)==0)
			{
				return 1;
			}
			InitWorldSelect(mgl);
			done=WS_CONTINUE;
		}
		EndClock();
	}

	ExitWorldSelect();
	return 1;
}

const char *WorldFName(void)
{
	return curName;
}

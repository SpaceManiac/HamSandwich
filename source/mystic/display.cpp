#include "display.h"
#include "particle.h"
#include "jamulfmv.h"
#include "game.h"
#include "title.h"

mfont_t  *gameFont[3]={NULL,NULL,NULL};
MGLDraw  *mgl=NULL;

int scrx=320,scry=240,scrdx=0,scrdy=0;
int rscrx=320<<FIXSHIFT,rscry=240<<FIXSHIFT;

byte shakeTimer=0;

DisplayList *dispList;
static byte gammaCorrection=0;

static char cursor[] = "}";

MGLDraw *GetDisplayMGL(void)
{
	return mgl;
}

bool InitDisplay(MGLDraw *mainmgl)
{
	mgl=mainmgl;
	if(!mgl)
		return false;
	gameFont[0]=(mfont_t *)malloc(sizeof(mfont_t));
	if(!gameFont[0])
		return false;
	FontInit(mgl);
	if(FontLoad("graphics/listiumbig.jft",gameFont[0])!=FONT_OK)
		return false;

	gameFont[1]=(mfont_t *)malloc(sizeof(mfont_t));
	if(!gameFont[1])
		return false;

	if(FontLoad("graphics/verdana.jft",gameFont[1])!=FONT_OK)
		return false;
	cursor[0] = RightBraceHack(gameFont[1]);

	gameFont[2]=(mfont_t *)malloc(sizeof(mfont_t));
	if(!gameFont[2])
		return false;

	if(FontLoad("graphics/listium.jft",gameFont[2])!=FONT_OK)
		return false;

	dispList=new DisplayList();

	return true;
}

void ExitDisplay(void)
{
	if(gameFont[0])
	{
		FontFree(gameFont[0]);
		free(gameFont[0]);
	}
	if(gameFont[1])
	{
		FontFree(gameFont[1]);
		free(gameFont[1]);
	}
	if(gameFont[2])
	{
		FontFree(gameFont[2]);
		free(gameFont[2]);
	}

	delete dispList;
}

void ShowVictoryAnim(byte world)
{
	dword start,end;

	start=timeGetTime();
	mgl->ClearScreen();
	mgl->Flip();
	switch(world)
	{
		case 0:
			PlaySong(SONG_CHAP12MAP);
			FLI_play("graphics/CH1.FLC",0,60,mgl);
			break;
		case 1:
			PlaySong(SONG_CHAP12MAP);
			FLI_play("graphics/CH2.FLC",0,60,mgl);
			break;
		case 2:
			PlaySong(SONG_CHAP34MAP);
			FLI_play("graphics/CH3.FLC",0,60,mgl);
			break;
		case 3:
			PlaySong(SONG_CHAP34MAP);
			FLI_play("graphics/CH4.FLC",0,60,mgl);
			break;
		case 4:
			// the final victory!
			PlaySong(SONG_BEATNIK);
			FLI_play("graphics/ENDING.FLC",0,100,mgl);
			if(player.nightmare)
				VictoryText(mgl,1);
			else
			{
				VictoryText(mgl,0);
				Credits(mgl,1);
			}
			break;
		case 5:
			// winning the demo
			PlaySong(SONG_SHOP);
			VictoryText(mgl,2);
			break;
		case 10:
			StopSong();
			MakeNormalSound(SND_ARMAGEDDON);
			FLI_play("graphics/SWORD.FLC",0,60,mgl);
			ReplaySong();
			break;
		case 11:
			//PlaySong(SONG_INTRO);
			StopSong();
			MakeNormalSound(SONG_INTRO);
			FLI_play("graphics/Intro.flc",0,60,mgl);
			JamulSoundStop(SONG_INTRO);
			if (CurrentSong())
				ReplaySong();
			break;
	}
	mgl->ClearScreen();
	mgl->Flip();
	mgl->LoadBMP("graphics/pal.bmp");

	end=timeGetTime();
	AddGarbageTime(end-start);
}

void EndingMovie(int frm)
{
}

byte *GetDisplayScreen(void)
{
	return mgl->GetScreen();
}

byte GetGamma(void)
{
	return gammaCorrection;
}

void SetGamma(byte g)
{
	gammaCorrection=g;
}

void GetCamera(int *x,int *y)
{
	*x=scrx;
	*y=scry;
}

void PutCamera(int x,int y)
{
	rscrx=x;
	rscry=y;
	scrdx=0;
	scrdy=0;

	scrx=(rscrx>>FIXSHIFT);
	scry=(rscry>>FIXSHIFT);
}

void UpdateCamera(int x,int y,byte facing,Map *map)
{
	int desiredX,desiredY;

	desiredX=((x<<FIXSHIFT)+Cosine(facing)*80)>>FIXSHIFT;
	desiredY=((y<<FIXSHIFT)+Sine(facing)*60)>>FIXSHIFT;

	rscrx+=scrdx;
	rscry+=scrdy;

	if(rscrx<320<<FIXSHIFT)
		rscrx=320<<FIXSHIFT;
	if(rscrx>((map->width*TILE_WIDTH-320)<<FIXSHIFT))
		rscrx=(map->width*TILE_WIDTH-320)<<FIXSHIFT;
	if(rscry<(240-TILE_HEIGHT)<<FIXSHIFT)
		rscry=(240-TILE_HEIGHT)<<FIXSHIFT;
	if(rscry>((map->height*TILE_HEIGHT-240)<<FIXSHIFT))
		rscry=(map->height*TILE_HEIGHT-240)<<FIXSHIFT;

	if(scrx>desiredX+20)
		scrdx=-((scrx-(desiredX+20))*FIXAMT/16);
	if(scrx<desiredX-20)
		scrdx=(((desiredX-20)-scrx)*FIXAMT/16);
	if(scry>desiredY+20)
		scrdy=-((scry-(desiredY+20))*FIXAMT/16);
	if(scry<desiredY-20)
		scrdy=(((desiredY-20)-scry)*FIXAMT/16);

	Dampen(&scrdx,1<<FIXSHIFT);
	Dampen(&scrdy,1<<FIXSHIFT);

	scrx=(rscrx>>FIXSHIFT);
	scry=(rscry>>FIXSHIFT);
}

void Print(int x,int y,const char *s,char bright,byte font)
{
	if(font==0)
		FontPrintStringBright(x,y,s,gameFont[0],bright);
	else
	{
		if(bright==0)
			FontPrintString(x,y,s,gameFont[font]);
		else
			FontPrintStringSolid(x,y,s,gameFont[font],0);
	}
}

void PrintBright(int x,int y,const char *s,char bright,byte font)
{
	FontPrintStringBright(x,y,s,gameFont[font],bright);
}

void PrintGlow(int x,int y,const char *s,byte font)
{
	FontPrintStringGlow(x,y,s,gameFont[font]);
}

void RightPrintGlow(int x,int y,const char *s,byte font)
{
	FontPrintStringGlow(x-FontStrLen(s,gameFont[font]),y,s,gameFont[font]);
}

void PrintBrightGlow(int x,int y,const char *s,char brt,byte font)
{
	FontPrintStringBrightGlow(x,y,s,brt,gameFont[font]);
}

void CenterPrint(int midx,int y,const char *s,char bright,byte font)
{
	int x;

	if(font==0)
	{
		x=midx-FontStrLen(s,gameFont[0])/2;
		FontPrintStringBright(x,y,s,gameFont[0],bright);
	}
	else
	{
		x=midx-FontStrLen(s,gameFont[font])/2;
		if(bright==0)
			FontPrintString(x,y,s,gameFont[font]);
		else
			FontPrintStringSolid(x,y,s,gameFont[font],0);
	}
}

void CenterPrintGlow(int y,const char *s,byte font)
{
	int x;

	x=320-FontStrLen(s,gameFont[font])/2;
	FontPrintStringGlow(x,y,s,gameFont[font]);
}

int GetStrLength(const char *s,byte fnt)
{
	return FontStrLen(s,gameFont[fnt]);
}

void DrawMouseCursor(int x,int y)
{
	FontPrintStringSolid(x-1,y,cursor,gameFont[1],0);
	FontPrintStringSolid(x+1,y,cursor,gameFont[1],0);
	FontPrintStringSolid(x,y-1,cursor,gameFont[1],0);
	FontPrintStringSolid(x,y+1,cursor,gameFont[1],0);
	FontPrintStringSolid(x,y,cursor,gameFont[1],31);
}

void ShakeScreen(byte howlong)
{
	shakeTimer=howlong;
}

void RenderItAll(world_t *world,Map *map,byte flags)
{
	if(shakeTimer)
	{
		shakeTimer--;
		scrx-=2+MGL_random(5);
		scry-=2+MGL_random(5);
	}
	map->Render(world,scrx,scry,flags);

	scrx-=320;
	scry-=240;
	dispList->Render();
	dispList->ClearList();
	scrx+=320;
	scry+=240;
}

void SprDraw(int x,int y,int z,byte hue,char bright,sprite_t *spr,byte flags)
{
	// this call returns whether it worked or not, but frankly, we don't care
	dispList->DrawSprite(x,y,z,hue,bright,spr,flags);
}

void WallDraw(int x,int y,word wall,word floor,const char *light,byte flags)
{
	// this call returns whether it worked or not, but frankly, we don't care
	dispList->DrawSprite(x,y,wall,floor,0,(sprite_t *)light,flags);
}

void RoofDraw(int x,int y,word roof,const char *light,byte flags)
{
	// this call returns whether it worked or not, but frankly, we don't care
	dispList->DrawSprite(x,y,TILE_HEIGHT,roof,0,(sprite_t *)light,flags);
}

void ParticleDraw(int x,int y,int z,byte color,byte size,byte flags)
{
	// this call returns whether it worked or not, but frankly, we don't care
	dispList->DrawSprite(x,y,z,color,size,(sprite_t *)1,flags);
}

//---------------------------------------------------------------------------------------
// from here on out it's class DISPLAYLIST

DisplayList::DisplayList(void)
{
	ClearList();
}

DisplayList::~DisplayList(void)
{
	// nothin
}

int DisplayList::GetOpenSlot(void)
{
	int i;

	for(i=0;i<MAX_DISPLAY_OBJS;i++)
	{
		if(dispObj[i].flags==0)
			return i;
	}

	return -1;
}

void DisplayList::HookIn(int me)
{
	int i;

	if(head==-1)
	{
		head=me;
		dispObj[me].prev=-1;
		dispObj[me].next=-1;
		return;
	}
	else
	{
		// shadows go on the head of the list always, drawn before anything else
		// (and the order of shadows doesn't matter, of course)
		if(dispObj[me].flags&DISPLAY_SHADOW)
		{
			dispObj[me].next=head;
			dispObj[head].prev=me;
			dispObj[me].prev=-1;
			head=me;
			return;
		}

		i=head;
		while(i!=-1)
		{
			if((!(dispObj[i].flags&DISPLAY_SHADOW)) &&
				(dispObj[i].y>dispObj[me].y || (dispObj[i].y==dispObj[me].y && dispObj[i].z>dispObj[me].z)))
			{
				dispObj[me].prev=dispObj[i].prev;
				dispObj[me].next=i;
				if(dispObj[me].prev!=-1)
					dispObj[dispObj[me].prev].next=me;
				dispObj[i].prev=me;
				if(head==i)
					head=me;
				return;
			}
			if(dispObj[i].next==-1)
			{
				dispObj[i].next=me;
				dispObj[me].prev=i;
				dispObj[me].next=-1;
				return;
			}
			i=dispObj[i].next;
		}
		return; // this would be bad, but hopefully can't occur
	}
}

bool DisplayList::DrawSprite(int x,int y,int z,word hue,char bright,sprite_t *spr,byte flags)
{
	int i;

	if((x-scrx+320)<-DISPLAY_XBORDER || (x-scrx+320)>640+DISPLAY_XBORDER ||
	   (y-scry+240)<-DISPLAY_YBORDER || (y-scry+240)>480+DISPLAY_YBORDER)
		return true;

	i=GetOpenSlot();
	if(i==-1)
		return false;

	dispObj[i].hue=hue;
	dispObj[i].bright=bright;
	dispObj[i].flags=flags;
	dispObj[i].spr=spr;
	dispObj[i].x=x;
	dispObj[i].y=y;
	dispObj[i].z=z;
	if(dispObj[i].flags&(DISPLAY_WALLTILE|DISPLAY_ROOFTILE))
		memcpy(dispObj[i].light,dispObj[i].spr,9);
	HookIn(i);
	return true;
}

void DisplayList::ClearList(void)
{
	int i;

	for(i=0;i<MAX_DISPLAY_OBJS;i++)
	{
		dispObj[i].prev=-1;
		dispObj[i].next=-1;
		dispObj[i].flags=0;
	}
	head=-1;
	nextfree=0;
}

void DisplayList::Render(void)
{
	int i;

	i=head;

	while(i!=-1)
	{
		if((dispObj[i].flags&DISPLAY_DRAWME) && (dispObj[i].spr))
		{
			if(dispObj[i].flags&DISPLAY_WALLTILE)
			{
				if(dispObj[i].flags&DISPLAY_TRANSTILE)
				{

					RenderWallTileFancy(dispObj[i].x-scrx,dispObj[i].y-scry,dispObj[i].z,dispObj[i].light);
					RenderRoofTileFancy(dispObj[i].x-scrx,dispObj[i].y-scry-TILE_HEIGHT,dispObj[i].hue,1,1,dispObj[i].light);
				}
				else
				{
					RenderWallTileFancy(dispObj[i].x-scrx,dispObj[i].y-scry,dispObj[i].z,dispObj[i].light);
					RenderRoofTileFancy(dispObj[i].x-scrx,dispObj[i].y-scry-TILE_HEIGHT,dispObj[i].hue,0,1,dispObj[i].light);
				}
			}
			else if(dispObj[i].flags&DISPLAY_ROOFTILE)
			{
				if(dispObj[i].flags&DISPLAY_TRANSTILE)
					RenderRoofTileFancy(dispObj[i].x-scrx,dispObj[i].y-scry-TILE_HEIGHT,dispObj[i].hue,1,1,dispObj[i].light);
				else
					RenderRoofTileFancy(dispObj[i].x-scrx,dispObj[i].y-scry-TILE_HEIGHT,dispObj[i].hue,0,1,dispObj[i].light);
			}
			else if(dispObj[i].flags&DISPLAY_SHADOW)
			{
				dispObj[i].spr->DrawShadow(dispObj[i].x-scrx,dispObj[i].y-scry-dispObj[i].z,mgl);
			}
			else if(dispObj[i].flags&DISPLAY_PARTICLE)
			{
				RenderParticle(dispObj[i].x-scrx,dispObj[i].y-scry-dispObj[i].z,mgl->GetScreen(),
								(byte)dispObj[i].hue,dispObj[i].bright);
			}
			else
			{
				if(dispObj[i].flags&DISPLAY_GHOST)
				{
					dispObj[i].spr->DrawGhost(dispObj[i].x-scrx,dispObj[i].y-scry-dispObj[i].z,mgl,
							dispObj[i].bright);
				}
				else if(dispObj[i].flags&DISPLAY_GLOW)
				{
					dispObj[i].spr->DrawGlow(dispObj[i].x-scrx,dispObj[i].y-scry-dispObj[i].z,mgl,
							dispObj[i].bright);
				}
				else
				{
					if(dispObj[i].hue==255)	// no special coloring
					{
						dispObj[i].spr->DrawBright(dispObj[i].x-scrx,dispObj[i].y-scry-dispObj[i].z,mgl,
							dispObj[i].bright);
					}
					else	// draw special color
					{
						dispObj[i].spr->DrawColored(dispObj[i].x-scrx,dispObj[i].y-scry-dispObj[i].z,mgl,
							(byte)dispObj[i].hue,dispObj[i].bright);
					}
				}
			}
		}
		i=dispObj[i].next;
	}
}

void MakeItFlip(void)
{
	mgl->Flip();
}

void DrawBox(int x,int y,int x2,int y2,byte c)
{
	mgl->Box(x,y,x2,y2,c);
}

void DrawDebugBox(int x,int y,int x2,int y2)
{
	x-=scrx-320;
	y-=scry-240;
	x2-=scrx-320;
	y2-=scry-240;
	mgl->Box(x,y,x2,y2,255);
	mgl->Flip();
}

void DrawFillBox(int x,int y,int x2,int y2,byte c)
{
	mgl->FillBox(x,y,x2,y2,c);
}

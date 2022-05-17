#include "display.h"
#include "particle.h"
#include "jamulfmv.h"
#include "game.h"
#include "options.h"

mfont_t  *gameFont[3]={NULL,NULL,NULL};
MGLDraw  *mgl=NULL;

int scrx=320,scry=240,scrdx=0,scrdy=0;
int rscrx=320<<FIXSHIFT,rscry=240<<FIXSHIFT;

byte shakeTimer=0;
byte filmWiggleTimer=0;

DisplayList *dispList;
static byte gammaCorrection=0;

MGLDraw *GetDisplayMGL(void)
{
	return mgl;
}

void SetDisplayMGL(MGLDraw *mainmgl)
{
	mgl=mainmgl;
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
	if(FontLoad("graphics\\impact.jft",gameFont[0])!=FONT_OK)
		return false;

	gameFont[1]=(mfont_t *)malloc(sizeof(mfont_t));
	if(!gameFont[1])
		return false;

	if(FontLoad("graphics\\verdana.jft",gameFont[1])!=FONT_OK)
		return false;

	gameFont[2]=gameFont[0];

	dispList=new DisplayList();

	return true;
}

void ExitDisplay(void)
{
	if(gameFont[0])
	{
		FontFree(gameFont[0]);
		free(gameFont[0]);
		gameFont[0]=NULL;
	}
	if(gameFont[1])
	{
		FontFree(gameFont[1]);
		free(gameFont[1]);
		gameFont[1]=NULL;
	}
	gameFont[2]=NULL;

	delete dispList;
}

void ShowVictoryAnim(byte world)
{
	dword start,end;

	start=timeGetTime();

	JamulSoundPurge();
	switch(world)
	{
		case 0:
			//FLI_play("graphics\\ending.flc",0,60,mgl,MovieSounds);
			break;
	}
	mgl->ClearScreen();
	mgl->Flip();
	mgl->LoadBMP("graphics\\title.bmp");
	JamulSoundPurge();
	end=timeGetTime();
	AddGarbageTime(end-start);
}

void ShowImageOrFlic(char *str)
{
	dword start,end;
	int speed;

	char *fname;
	char *other;
	char nm[64];

	fname=strtok(str,",\n");
	if(!fname)
		return;

	other=strtok(NULL,",\n");

	// BMP loading
	if((fname[strlen(fname)-3]=='b' || fname[strlen(fname)-3]=='B') &&
	   (fname[strlen(fname)-2]=='m' || fname[strlen(fname)-2]=='M') &&
	   (fname[strlen(fname)-1]=='p' || fname[strlen(fname)-1]=='P'))
	{
		EnterPictureDisplay();
		sprintf(nm,"graphics\\%s",fname);
		GetDisplayMGL()->LoadBMP(nm);
		return;
	}

	if(other)
		speed=atoi(other);
	else
		speed=60;

	sprintf(nm,"graphics\\%s",fname);

	start=timeGetTime();
	FLI_play(nm,0,speed,mgl,NULL);
	mgl->LoadBMP("graphics\\title.bmp");
	end=timeGetTime();
	AddGarbageTime(end-start);
}

void DarkenScreen(byte dark)
{
	byte *scrn;
	byte b;
	int i;

	scrn=mgl->GetScreen();

	for(i=0;i<640*480;i++)
	{
		b=*scrn;
		if((b&31)<dark)
			*scrn=(b&(~31));
		else
			*scrn-=dark;
		scrn++;
	}
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

void UpdateCamera(int x,int y,byte facing,Map *map)
{
	rscrx=320*FIXAMT;
	rscry=240*FIXAMT;
	scrx=320;
	scry=240;
}

void Print(int x,int y,const char *s,char bright,byte font)
{
	if(font!=1)
		FontPrintStringBright(x,y,s,gameFont[font],bright);
	else
	{
		if(bright==0)
			FontPrintString(x,y,s,gameFont[1]);
		else
			FontPrintStringSolid(x,y,s,gameFont[1],0);
	}
}

void PrintOutline(int x,int y,const char *s,char bright,byte font)
{
	FontPrintStringBright(x-1,y-1,s,gameFont[font],-32);
	FontPrintStringBright(x+1,y+1,s,gameFont[font],-32);
	FontPrintStringBright(x-1,y+1,s,gameFont[font],-32);
	FontPrintStringBright(x+1,y-1,s,gameFont[font],-32);
	FontPrintStringBright(x,y,s,gameFont[font],bright);
}

void CenterPrintOutline(int x,int y,const char *s,char bright,byte font)
{
	x=x-FontStrLen(s,gameFont[font])/2;
	FontPrintStringBright(x-1,y-1,s,gameFont[font],-32);
	FontPrintStringBright(x+1,y+1,s,gameFont[font],-32);
	FontPrintStringBright(x-1,y+1,s,gameFont[font],-32);
	FontPrintStringBright(x+1,y-1,s,gameFont[font],-32);
	FontPrintStringBright(x,y,s,gameFont[font],bright);
}

void RvsCenterPrintOutline(int x,int y,const char *s,char bright,byte font)
{
	char s2[64];
	word i;

	for(i=1;i<=strlen(s);i++)
	{
		s2[strlen(s)-i]=s[i-1];
	}
	s2[strlen(s)]='\0';
	x=x-FontStrLen(s2,gameFont[font])/2;
	FontPrintStringBright(x-1,y-1,s2,gameFont[font],-32);
	FontPrintStringBright(x+1,y+1,s2,gameFont[font],-32);
	FontPrintStringBright(x-1,y+1,s2,gameFont[font],-32);
	FontPrintStringBright(x+1,y-1,s2,gameFont[font],-32);
	FontPrintStringBright(x,y,s2,gameFont[font],bright);
}

void RightPrintOutline(int x,int y,const char *s,char bright,byte font)
{
	x=x-FontStrLen(s,gameFont[font]);
	FontPrintStringBright(x-1,y-1,s,gameFont[font],-32);
	FontPrintStringBright(x+1,y+1,s,gameFont[font],-32);
	FontPrintStringBright(x-1,y+1,s,gameFont[font],-32);
	FontPrintStringBright(x+1,y-1,s,gameFont[font],-32);
	FontPrintStringBright(x,y,s,gameFont[font],bright);
}

void PrintLimited(int x,int y,int maxX,const char *s,char bright,byte font)
{
	FontPrintStringBrightLimit(x,y,maxX,s,gameFont[font],bright);
}

void PrintColor(int x,int y,const char *s,byte color,char bright,byte font)
{
	FontPrintStringColor(x,y,s,gameFont[font],color,bright);
}

void PrintGlow(int x,int y,const char *s,char bright,byte font)
{
	FontPrintStringGlow(x,y,s,gameFont[font],bright);
}

void PrintUnGlow(int x,int y,const char *s,byte font)
{
	FontPrintStringUnGlow(x,y,s,gameFont[font]);
}

void PrintUnGlowSideways(int x,int y,int minY,const char *s,byte font)
{
	FontPrintStringUnGlowSideways(x,y,minY,s,gameFont[font]);
}

void CenterPrintUnGlow(int x,int y,const char *s,byte font)
{
	x=x-FontStrLen(s,gameFont[font])/2;
	FontPrintStringUnGlow(x,y,s,gameFont[font]);
}

void RightPrintUnGlow(int x,int y,const char *s,byte font)
{
	x=x-FontStrLen(s,gameFont[font]);
	FontPrintStringUnGlow(x,y,s,gameFont[font]);
}

void PrintAngleUnGlow(int x,int y,const char *s,byte font)
{
	FontPrintStringAngleUnGlow(x,y,s,gameFont[font]);
}

void PrintAngle2(int x,int y,const char *s,byte font)
{
	FontPrintStringAngle2(x,y,s,gameFont[font]);
}

void CenterPrintGlow(int x,int y,const char *s,char bright,byte font)
{
	x=x-FontStrLen(s,gameFont[font])/2;
	FontPrintStringGlow(x,y,s,gameFont[font],bright);
}

void RightPrintGlow(int x,int y,const char *s,char bright,byte font)
{
	x=x-FontStrLen(s,gameFont[font]);
	FontPrintStringGlow(x,y,s,gameFont[font],bright);
}

void CenterPrint(int x,int y,const char *s,char bright,byte font)
{
	if(font!=1)
	{
		x=x-FontStrLen(s,gameFont[font])/2;
		FontPrintStringBright(x,y,s,gameFont[font],bright);
	}
	else
	{
		x=x-FontStrLen(s,gameFont[1])/2;
		if(bright==0)
			FontPrintString(x,y,s,gameFont[1]);
		else if(bright!=16)
			FontPrintStringSolid(x,y,s,gameFont[1],0);
		else
			FontPrintStringSolid(x,y,s,gameFont[1],16);
	}
}

void CenterPrintLimit(int x,int y,int maxX,const char *s,char bright,byte font)
{
	x=x-FontStrLen(s,gameFont[font])/2;
	FontPrintStringBrightLimit(x,y,maxX,s,gameFont[font],bright);
}

void RightPrint(int x,int y,const char *s,char bright,byte font)
{
	if(font!=1)
	{
		x=x-FontStrLen(s,gameFont[font]);
		FontPrintStringBright(x,y,s,gameFont[font],bright);
	}
	else
	{
		x=x-FontStrLen(s,gameFont[1]);
		if(bright==0)
			FontPrintString(x,y,s,gameFont[1]);
		else if(bright!=16)
			FontPrintStringSolid(x,y,s,gameFont[1],0);
		else
			FontPrintStringSolid(x,y,s,gameFont[1],16);
	}
}

void RightPrintColor(int x,int y,const char *s,byte color,char bright,byte font)
{
	x=x-FontStrLen(s,gameFont[font]);
	FontPrintStringColor(x,y,s,gameFont[font],color,bright);
}

void CenterPrintColor(int x,int y,const char *s,byte color,char bright,byte font)
{
	x=x-FontStrLen(s,gameFont[font])/2;
	FontPrintStringColor(x,y,s,gameFont[font],color,bright);
}

int GetStrLength(const char *s,byte f)
{
	return FontStrLen(s,gameFont[f]);
}

void DrawMouseCursor(int x,int y)
{
	FontPrintStringSolid(x-1,y,"}",gameFont[1],0);
	FontPrintStringSolid(x+1,y,"}",gameFont[1],0);
	FontPrintStringSolid(x,y-1,"}",gameFont[1],0);
	FontPrintStringSolid(x,y+1,"}",gameFont[1],0);
	FontPrintStringSolid(x,y,"}",gameFont[1],31);
}

void ShakeScreen(byte howlong)
{
	shakeTimer=howlong;
}

void FilmWiggle(byte howlong)
{
	filmWiggleTimer=howlong;
}

void RenderItAll(world_t *world,Map *map,byte flags)
{
	/*
	if(!GM_doDraw)
		return;
	*/

	if(shakeTimer)
	{
		shakeTimer--;
		scrx+=-2+Random(5);
		scry+=-2+Random(5);
	}
	if(filmWiggleTimer)
	{
		filmWiggleTimer--;
		if(filmWiggleTimer>4)
			scry+=-filmWiggleTimer/4+Random(filmWiggleTimer/2);
	}

	if(map)
		map->Render(world,scrx,scry,flags);

	scrx-=320;
	scry-=240;
	dispList->Render();
	dispList->ClearList();
	scrx+=320;
	scry+=240;
}

void SprDraw(int x,int y,int z,byte hue,char bright,sprite_t *spr,word flags)
{
	// this call returns whether it worked or not, but frankly, we don't care
	dispList->DrawSprite(x,y,z,0,hue,bright,spr,flags);
}

void OffSprDraw(int x,int y,int z,byte hue,byte hue2,char bright,sprite_t *spr,word flags)
{
	// this call returns whether it worked or not, but frankly, we don't care
	dispList->DrawSprite(x,y,z,hue2,hue,bright,spr,flags);
}

void WallDraw(int x,int y,word wall,word floor,char light,word flags)
{
	// this call returns whether it worked or not, but frankly, we don't care
	dispList->DrawSprite(x,y,0,wall,floor,light,(sprite_t *)1,flags);
}

void RoofDraw(int x,int y,word roof,char light,word flags)
{
	// this call returns whether it worked or not, but frankly, we don't care
	dispList->DrawSprite(x,y,TILE_HEIGHT,0,roof,light,(sprite_t *)1,flags);
}

void ParticleDraw(int x,int y,int z,byte color,byte size,word flags)
{
	// this call returns whether it worked or not, but frankly, we don't care
	dispList->DrawSprite(x,y,z,0,color,size,(sprite_t *)1,flags);
}

void LightningDraw(int x,int y,int x2,int y2,byte bright,char range)
{
	dispList->DrawSprite(x,y+32,x2,y2+32,bright,range,(sprite_t *)1,DISPLAY_DRAWME|DISPLAY_LIGHTNING);

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

bool DisplayList::DrawSprite(int x,int y,int z,int z2,word hue,char bright,sprite_t *spr,word flags)
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
	dispObj[i].z2=z2;
	HookIn(i);
	if(dispObj[i].flags&DISPLAY_LIGHTNING)
	{
		dispObj[i].y-=32;
		dispObj[i].z2-=32;
	}
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
					RenderWallTileTrans(dispObj[i].x-scrx,dispObj[i].y-scry,(word)dispObj[i].z2,dispObj[i].hue,dispObj[i].bright);
				else
					RenderWallTile(dispObj[i].x-scrx,dispObj[i].y-scry,(word)dispObj[i].z2,dispObj[i].hue,dispObj[i].bright);
			}
			else if(dispObj[i].flags&DISPLAY_ROOFTILE)
			{
				if(dispObj[i].flags&DISPLAY_TRANSTILE)
					RenderFloorTileTrans(dispObj[i].x-scrx,dispObj[i].y-scry-TILE_HEIGHT,dispObj[i].hue,dispObj[i].bright);
				else
					RenderFloorTile(dispObj[i].x-scrx,dispObj[i].y-scry-TILE_HEIGHT,dispObj[i].hue,dispObj[i].bright);
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
			else if(dispObj[i].flags&DISPLAY_CIRCLEPART)
			{
				if(dispObj[i].flags&DISPLAY_GLOW)
					RenderGlowCircleParticle(dispObj[i].x-scrx,dispObj[i].y-scry-dispObj[i].z,dispObj[i].bright,
									(byte)dispObj[i].hue,mgl->GetScreen());
				else
					RenderCircleParticle(dispObj[i].x-scrx,dispObj[i].y-scry-dispObj[i].z,dispObj[i].bright,
									(byte)dispObj[i].hue,mgl->GetScreen());
			}
			else if(dispObj[i].flags&DISPLAY_TEXTPART)
			{
				RenderTextParticle(dispObj[i].x,dispObj[i].y,(char)dispObj[i].bright,(byte)dispObj[i].hue);
			}
			else if(dispObj[i].flags&DISPLAY_ARROW)
			{
				RenderGhostStar(dispObj[i].bright+35,dispObj[i].x*FIXAMT,(dispObj[i].y-dispObj[i].z)*FIXAMT,(char)dispObj[i].hue,mgl);
			}
			else if(dispObj[i].flags&DISPLAY_RINGPART)
			{
				RenderRingParticle(dispObj[i].x-scrx,dispObj[i].y-scry-dispObj[i].z,dispObj[i].bright,
									(byte)dispObj[i].hue,mgl->GetScreen());
			}
			else if(dispObj[i].flags&DISPLAY_LIGHTNING)
			{
				RenderLightningParticle(dispObj[i].x-scrx,dispObj[i].y-scry,dispObj[i].z-scrx,
								dispObj[i].z2-scry,dispObj[i].bright,(byte)dispObj[i].hue,mgl->GetScreen());
			}
			else if(dispObj[i].flags&DISPLAY_OFFCOLOR)
			{
				dispObj[i].spr->DrawOffColor(dispObj[i].x-scrx,dispObj[i].y-scry-dispObj[i].z,mgl,
							(byte)dispObj[i].hue,(byte)dispObj[i].z2,dispObj[i].bright);
				if(profile.colorblind && (dispObj[i].flags&DISPLAY_COLORBLIND))
				{
					switch(dispObj[i].z2)
					{
						case 1:
							PrintOutline(dispObj[i].x-scrx,dispObj[i].y-scry-dispObj[i].z-8,"G",0,1);
							break;
						case 2:
							PrintOutline(dispObj[i].x-scrx,dispObj[i].y-scry-dispObj[i].z-8,"O",0,1);
							break;
						case 3:
							PrintOutline(dispObj[i].x-scrx,dispObj[i].y-scry-dispObj[i].z-8,"B",0,1);
							break;
						case 4:
							PrintOutline(dispObj[i].x-scrx,dispObj[i].y-scry-dispObj[i].z-8,"R",0,1);
							break;
						case 5:
							PrintOutline(dispObj[i].x-scrx,dispObj[i].y-scry-dispObj[i].z-8,"Y",0,1);
							break;
						case 6:
							PrintOutline(dispObj[i].x-scrx,dispObj[i].y-scry-dispObj[i].z-8,"P",0,1);
							break;
						case 7:
							PrintOutline(dispObj[i].x-scrx,dispObj[i].y-scry-dispObj[i].z-8,"A",0,1);
							break;
					}
				}
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

void PlotPixel(int x,int y,byte c,byte *scrn,int pitch)
{
	if(x>=0 && x<640 && y>=0 && y<480)
	{
		scrn[x+y*pitch]=c;
	}
}

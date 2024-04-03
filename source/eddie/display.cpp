#include "display.h"
#include "jamulfont.h"
#include "backgd.h"
#include "tile.h"
#include "sound.h"
#include "clock.h"

static MGLDraw *dispmgl;
sprite_set_t *spr[MAX_SPR];
mfont_t font[3];

DisplayList *dispList;
byte logoPos;

void InitDisplay(MGLDraw *mgl)
{
	FontInit(mgl);
	dispmgl=mgl;
	dispmgl->LoadBMP("graphics/status.bmp");

	dispList=new DisplayList();

#ifdef DEMO
	spr[SPR_ROCKET]=new sprite_set_t("graphics/rocket.jsp");
	spr[SPR_PEDE1]=NULL;
	spr[SPR_PEDE2]=NULL;
	spr[SPR_PEDE3]=NULL;
	spr[SPR_PEDE4]=NULL;
	spr[SPR_PEDE5]=NULL;
	spr[SPR_PEDE6]=NULL;
	spr[SPR_PEDE7]=new sprite_set_t("graphics/pede8.jsp");
	spr[SPR_PEDE8]=NULL;
	spr[SPR_PEDE9]=NULL;
	spr[SPR_BULLET]=new sprite_set_t("graphics/bullets.jsp");
	spr[SPR_SHROOM]=new sprite_set_t("graphics/shrooms.jsp");
	spr[SPR_MONSTER]=new sprite_set_t("graphics/monster.jsp");
	spr[SPR_STATUS]=new sprite_set_t("graphics/status.jsp");
	spr[SPR_OPTION]=new sprite_set_t("graphics/options.jsp");
	spr[SPR_LOGO]=new sprite_set_t("graphics/logo.jsp");
	spr[SPR_TITLE]=new sprite_set_t("graphics/title.jsp");
	spr[SPR_QUEEN]=NULL;
#elif TRIAL
	spr[SPR_ROCKET]=new sprite_set_t("graphics/rocket.jsp");
	spr[SPR_PEDE1]=new sprite_set_t("graphics/pede1.jsp");
	spr[SPR_PEDE2]=new sprite_set_t("graphics/pede2.jsp");
	spr[SPR_PEDE3]=new sprite_set_t("graphics/pede3.jsp");
	spr[SPR_PEDE4]=NULL;
	spr[SPR_PEDE5]=NULL;
	spr[SPR_PEDE6]=NULL;
	spr[SPR_PEDE7]=NULL;
	spr[SPR_PEDE8]=NULL;
	spr[SPR_PEDE9]=NULL;
	spr[SPR_BULLET]=new sprite_set_t("graphics/bullets.jsp");
	spr[SPR_SHROOM]=new sprite_set_t("graphics/shrooms.jsp");
	spr[SPR_MONSTER]=new sprite_set_t("graphics/monster.jsp");
	spr[SPR_STATUS]=new sprite_set_t("graphics/status.jsp");
	spr[SPR_OPTION]=new sprite_set_t("graphics/options.jsp");
	spr[SPR_LOGO]=new sprite_set_t("graphics/logo.jsp");
	spr[SPR_TITLE]=new sprite_set_t("graphics/title.jsp");
	spr[SPR_QUEEN]=NULL;
#elif JUNIOR
	spr[SPR_ROCKET]=new sprite_set_t("graphics/rocket.jsp");
	spr[SPR_PEDE1]=new sprite_set_t("graphics/pede1.jsp");
	spr[SPR_PEDE2]=new sprite_set_t("graphics/pede2.jsp");
	spr[SPR_PEDE3]=new sprite_set_t("graphics/pede3.jsp");
	spr[SPR_PEDE4]=new sprite_set_t("graphics/pede4.jsp");
	spr[SPR_PEDE5]=new sprite_set_t("graphics/pede5.jsp");
	spr[SPR_PEDE6]=NULL;
	spr[SPR_PEDE7]=NULL;
	spr[SPR_PEDE8]=NULL;
	spr[SPR_PEDE9]=NULL;
	spr[SPR_BULLET]=new sprite_set_t("graphics/bullets.jsp");
	spr[SPR_SHROOM]=new sprite_set_t("graphics/shrooms.jsp");
	spr[SPR_MONSTER]=new sprite_set_t("graphics/monster.jsp");
	spr[SPR_STATUS]=new sprite_set_t("graphics/status.jsp");
	spr[SPR_OPTION]=new sprite_set_t("graphics/options.jsp");
	spr[SPR_LOGO]=new sprite_set_t("graphics/logo.jsp");
	spr[SPR_TITLE]=new sprite_set_t("graphics/title.jsp");
	spr[SPR_QUEEN]=NULL;
#else
	spr[SPR_ROCKET]=new sprite_set_t("graphics/rocket.jsp");
	spr[SPR_PEDE1]=new sprite_set_t("graphics/pede1.jsp");
	spr[SPR_PEDE2]=new sprite_set_t("graphics/pede2.jsp");
	spr[SPR_PEDE3]=new sprite_set_t("graphics/pede3.jsp");
	spr[SPR_PEDE4]=new sprite_set_t("graphics/pede4.jsp");
	spr[SPR_PEDE5]=new sprite_set_t("graphics/pede5.jsp");
	spr[SPR_PEDE6]=new sprite_set_t("graphics/pede9.jsp");
	spr[SPR_PEDE7]=new sprite_set_t("graphics/pede8.jsp");
	spr[SPR_PEDE8]=new sprite_set_t("graphics/pede6.jsp");
	spr[SPR_PEDE9]=new sprite_set_t("graphics/pede7.jsp");
	spr[SPR_BULLET]=new sprite_set_t("graphics/bullets.jsp");
	spr[SPR_SHROOM]=new sprite_set_t("graphics/shrooms.jsp");
	spr[SPR_MONSTER]=new sprite_set_t("graphics/monster.jsp");
	spr[SPR_STATUS]=new sprite_set_t("graphics/status.jsp");
	spr[SPR_OPTION]=new sprite_set_t("graphics/options.jsp");
	spr[SPR_LOGO]=new sprite_set_t("graphics/logo.jsp");
	spr[SPR_TITLE]=new sprite_set_t("graphics/title.jsp");
	spr[SPR_QUEEN]=new sprite_set_t("graphics/queen.jsp");
#endif
	FontLoad("graphics/verdana.jft",&font[0]);
	FontLoad("graphics/silicon.jft",&font[1]);
	FontLoad("graphics/siliconbig.jft",&font[2]);
	logoPos=0;
}

void ExitDisplay(void)
{
	int i;

	FontFree(&font[0]);
	FontFree(&font[1]);
	FontFree(&font[2]);
	FontExit();
	delete dispList;

	for(i=0;i<MAX_SPR;i++)
	{
		if(spr[i]!=NULL)
			delete spr[i];
	}
}

void GetPaletteFromBMP(const char *fname)
{
	dispmgl->LoadBMP(fname);
}

void Print(int x,int y,const char *s,byte f)
{
	FontPrintString(x,y,s,&font[f]);
}

void PrintBright(int x,int y,const char *s,char brt,byte f)
{
	FontPrintStringBright(x,y,s,&font[f],brt);
}

void PrintGreen(int x,int y,const char *s)
{
	FontPrintStringColor(x,y,s,&font[1],32);
}

int StringLength(const char *s,byte f)
{
	return FontStrLen(s,&font[f]);
}

void PrintCursor(int x,int y,const char *s,byte pos,byte blink,char bright,byte f)
{
	FontPrintStringCursorLit(x,y,pos,blink,s,&font[f],bright);
}

void CenterPrint(int y,const char *s,byte f,char brt)
{
	// this centers in the play window, not the full screen
	FontPrintStringBright(240-(FontStrLen(s,&font[f])/2),y,s,&font[f],brt);
}

void ColorCenterPrint(int y,const char *s,byte f,byte col)
{
	// this centers in the play window, not the full screen
	FontPrintStringDropShadow(240-(FontStrLen(s,&font[f])/2),y,s,&font[f],col,2);
}

void CenterPrint2(int y,char *s,byte f,char brt)
{
	int len;

	len=(FontStrLen(s,&font[f])/2);
	s[strlen(s)-1]='\0';	// strip the last character
	// this centers in the play window, not the full screen
	FontPrintStringBright(240-len,y,s,&font[f],brt);
}

TASK(byte) FlipScreen(void)
{
	AWAIT dispmgl->Flip();
	if(!dispmgl->Process())
		CO_RETURN 0;
	CO_RETURN 1;
}

void AdvanceLogo(void)
{
	if(logoPos<8)
		logoPos++;
}

void RetractLogo(void)
{
	if(logoPos>0)
		logoPos--;
}

void RenderLogo(void)
{
	if(logoPos>0)
		spr[SPR_LOGO]->GetSprite(logoPos-1)->Draw(240,240,dispmgl);
}

void ShowSprite(byte set,int s,int x,int y)
{
	spr[set]->GetSprite(s)->Draw(x,y,dispmgl);
}

void GhostSprite(byte set,int s,int x,int y,char bright)
{
	spr[set]->GetSprite(s)->DrawGhost(x,y,dispmgl,bright);
}

void GlowSprite(byte set,int s,int x,int y,char bright)
{
	spr[set]->GetSprite(s)->DrawGlow(x,y,dispmgl,bright);
}

void DrawStatus(void)
{
	RenderStatusBar(dispmgl->GetScreen());
}

void ClearScreen(void)
{
	dispmgl->ClearScreen();
}

void DrawBackgd(void)
{
	RenderBackgd(dispmgl->GetScreen());
}

void SpecialDrawBackgd(int y)
{
	SpecialRenderBackgd(dispmgl->GetScreen(),y);
}

void SpriteRect(byte set,int s,int x,int y,int *rx,int *ry,int *rx2,int *ry2)
{
	*rx=x-spr[set]->GetSprite(s)->ofsx;
	*ry=y-spr[set]->GetSprite(s)->ofsy;

	*rx2=*rx+spr[set]->GetSprite(s)->width;
	*ry2=*ry+spr[set]->GetSprite(s)->height;
}

void Box(int x,int y,int x2,int y2,byte c)
{
	dispmgl->FillBox(x,y,x2,y2,c);
}

void SprDraw(int x,int y,int z,byte hue,char bright,byte set,int s,byte flags)
{
	// this call returns whether it worked or not, but frankly, we don't care
	dispList->DrawSprite(x,y,z,hue,bright,spr[set]->GetSprite(s),flags);
}

void RenderAllSprites(void)
{
	dispList->Render();
	dispList->ClearList();
}

void RenderParticle(int x,int y,byte color,byte size)
{
	byte c1,c2;
	byte *scrn;

	if(x<0 || x>639 || y<0 || y>479)
		return;

	scrn=dispmgl->GetScreen();

	switch(size)
	{
		case 2:	// big particle
			if(x<2 || x>637 || y<2 || y>477)
				return;

			if((color&31)>1)
				c1=color-2;	// only do this if subtracting 2 keeps it in the same color group
			else
				c1=color;
			if((c1&31)>1)
				c2=c1-2;	// only do this if subtracting 4 keeps it in the same color group
			else
				c2=c1;

			scrn+=(x+(y-2)*640);
			*scrn=c2;
			scrn+=639;
			*scrn++=c1;
			*scrn++=color;
			*scrn=c1;
			scrn+=637;
			*scrn++=c2;
			*scrn++=c1;
			*scrn++=color;
			*scrn++=c1;
			*scrn=c2;
			scrn+=637;
			*scrn++=c1;
			*scrn++=color;
			*scrn=c1;
			*(scrn+639)=c2;
			break;
		case 1:	// normal particle
			if(x<1 || x>638 || y<1 || y>478)
				return;
			if(color&31)
				c1=color-1;	// only do this if subtracting 1 keeps it in the same color group
			else
				c1=color;
			scrn+=(x+(y-1)*640);
			*scrn=c1;
			scrn+=639;
			*scrn++=c1;
			*scrn++=color;
			*scrn=c1;
			scrn+=639;
			*scrn=c1;
			break;
		case 0:	// tiny particle (1 pixel)
			scrn[x+y*640]=color;
			break;
	}
}

void GlowSpot(byte *scrn,byte color)
{
	*scrn&=31;
	*scrn+=color+160;
	if(*scrn>31+160)
		*scrn=31+160;

}

void RenderGlowParticle(int x,int y,byte color,byte size)
{
	byte c1,c2;
	byte *scrn;

	if(x<0 || x>639 || y<0 || y>479)
		return;

	scrn=dispmgl->GetScreen();

	switch(size)
	{
		case 3:	// macho grande particle
			if(x<3 || x>636 || y<3 || y>476)
				return;

			if((color&31)>1)
				c1=color-2;	// only do this if subtracting 2 keeps it in the same color group
			else
				c1=color;
			if((c1&31)>1)
				c2=c1-2;	// only do this if subtracting 4 keeps it in the same color group
			else
				c2=c1;

			scrn+=(x-1+(y-3)*640);
			GlowSpot(scrn,c2);
			GlowSpot(scrn+1,c2);
			GlowSpot(scrn+2,c2);
			scrn+=639;
			GlowSpot(scrn,c2);
			GlowSpot(scrn+1,c1);
			GlowSpot(scrn+2,c1);
			GlowSpot(scrn+3,c1);
			GlowSpot(scrn+4,c2);
			scrn+=639;
			GlowSpot(scrn,c2);
			GlowSpot(scrn+1,c1);
			GlowSpot(scrn+2,c1);
			GlowSpot(scrn+3,color);
			GlowSpot(scrn+4,c1);
			GlowSpot(scrn+5,c1);
			GlowSpot(scrn+6,c2);
			scrn+=640;
			GlowSpot(scrn,c2);
			GlowSpot(scrn+1,c1);
			GlowSpot(scrn+2,color);
			GlowSpot(scrn+3,color);
			GlowSpot(scrn+4,color);
			GlowSpot(scrn+5,c1);
			GlowSpot(scrn+6,c2);
			scrn+=640;
			GlowSpot(scrn,c2);
			GlowSpot(scrn+1,c1);
			GlowSpot(scrn+2,c1);
			GlowSpot(scrn+3,color);
			GlowSpot(scrn+4,c1);
			GlowSpot(scrn+5,c1);
			GlowSpot(scrn+6,c2);
			scrn+=641;
			GlowSpot(scrn,c2);
			GlowSpot(scrn+1,c1);
			GlowSpot(scrn+2,c1);
			GlowSpot(scrn+3,c1);
			GlowSpot(scrn+4,c2);
			scrn+=641;
			GlowSpot(scrn,c2);
			GlowSpot(scrn+1,c2);
			GlowSpot(scrn+2,c2);
			break;
		case 2:	// big particle
			if(x<2 || x>637 || y<2 || y>477)
				return;

			if((color&31)>1)
				c1=color-2;	// only do this if subtracting 2 keeps it in the same color group
			else
				c1=color;
			if((c1&31)>1)
				c2=c1-2;	// only do this if subtracting 4 keeps it in the same color group
			else
				c2=c1;

			scrn+=(x+(y-2)*640);
			GlowSpot(scrn,c2);
			scrn+=639;
			GlowSpot(scrn,c1);
			GlowSpot(scrn+1,color);
			GlowSpot(scrn+2,c1);
			scrn+=639;
			GlowSpot(scrn,c2);
			GlowSpot(scrn+1,c1);
			GlowSpot(scrn+2,color);
			GlowSpot(scrn+3,c1);
			GlowSpot(scrn+4,c2);
			scrn+=641;
			GlowSpot(scrn,c1);
			GlowSpot(scrn+1,color);
			GlowSpot(scrn+2,c1);
			GlowSpot(scrn+641,c2);
			break;
		case 1:	// normal particle
			if(x<1 || x>638 || y<1 || y>478)
				return;
			if(color&31)
				c1=color-1;	// only do this if subtracting 1 keeps it in the same color group
			else
				c1=color;
			scrn+=(x+(y-1)*640);
			GlowSpot(scrn,c1);
			scrn+=639;
			GlowSpot(scrn,c1);
			GlowSpot(scrn+1,color);
			GlowSpot(scrn+2,c1);
			GlowSpot(scrn+641,c1);
			break;
		case 0:	// tiny particle (1 pixel)
			GlowSpot(&scrn[x+y*640],color);
			break;
	}
}

TASK(void) SplashScreen(const char *fname,int delay,byte sound,byte specialdeal)
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
			if(dispmgl->LastKeyPressed() || dispmgl->MouseTap() || dispmgl->RMouseTap())
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
					MakeSound(sound,3000);
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

bool DisplayList::DrawSprite(int x,int y,int z,byte hue,char bright,sprite_t *spr,byte flags)
{
	int i;

	if(x<-DISPLAY_XBORDER || x>640+DISPLAY_XBORDER ||
	   y<-DISPLAY_YBORDER || y>480+DISPLAY_YBORDER)
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
			if(dispObj[i].flags&DISPLAY_SHADOW)
			{
				dispObj[i].spr->DrawShadow(dispObj[i].x,dispObj[i].y-dispObj[i].z,dispmgl);
			}
			else
			{
				if(dispObj[i].flags&DISPLAY_GHOST)
				{
					dispObj[i].spr->DrawGhost(dispObj[i].x,dispObj[i].y-dispObj[i].z,dispmgl,
							dispObj[i].bright);
				}
				else if(dispObj[i].flags&DISPLAY_GLOW)
				{
					dispObj[i].spr->DrawGlow(dispObj[i].x,dispObj[i].y-dispObj[i].z,dispmgl,
							dispObj[i].bright);
				}
				else
				{
					if(dispObj[i].hue==255)	// no special coloring
					{
						dispObj[i].spr->DrawBright(dispObj[i].x,dispObj[i].y-dispObj[i].z,dispmgl,
							dispObj[i].bright);
					}
					else	// draw special color
					{
						dispObj[i].spr->DrawColored(dispObj[i].x,dispObj[i].y-dispObj[i].z,dispmgl,
							dispObj[i].hue,dispObj[i].bright);
					}
				}
			}
		}
		i=dispObj[i].next;
	}
}

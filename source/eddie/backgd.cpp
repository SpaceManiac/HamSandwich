#include "backgd.h"
#include "mgldraw.h"
#include "appdata.h"

byte *backgd;
byte *statusBar;
int backgdY;
byte backScroll;

void InitBackgd(void)
{
	backgdY=0;
	backScroll=0;
	backgd=(byte *)malloc(480*480);
	if(!backgd)
		FatalError("Out of memory!");
	statusBar=(byte *)malloc(160*480);
	if(!statusBar)
		FatalError("Out of memory!");
	LoadStatus();
}

void ExitBackgd(void)
{
	free(backgd);
	free(statusBar);
}

void SetBackScroll(byte on)
{
	backScroll=on;
}

void LoadBackgd(const char *name)
{
	owned::SDL_Surface bmp = owned::SDL_LoadBMP_RW(AssetOpen_SDL_Owned(name));
	memcpy(backgd, bmp->pixels, bmp->h * 480);
	backgdY=0;
}

void LoadStatus(void)
{
	owned::SDL_Surface bmp = owned::SDL_LoadBMP_RW(AssetOpen_SDL_Owned("graphics/status.bmp"));
	memcpy(statusBar, bmp->pixels, bmp->h * 160);
}

void RenderBackgd(byte *scr)
{
	int i;
	byte *src,*dst;

	src=backgd;
	dst=scr+backgdY*640;

	for(i=backgdY;i<480;i++)
	{
		memcpy(dst,src,480);
		src+=480;
		dst+=640;
	}
	dst=scr;
	for(i=0;i<backgdY;i++)
	{
		memcpy(dst,src,480);
		src+=480;
		dst+=640;
	}
}

void SpecialRenderBackgd(byte *scr,int y)
{
	int i;
	byte *src,*dst;

	src=backgd;
	dst=scr+y*640;

	for(i=y;i<480;i++)
	{
		memcpy(dst,src,480);
		src+=480;
		dst+=640;
	}
	dst=scr;
	for(i=0;i<y;i++)
	{
		memset(dst,0,480);
		src+=480;
		dst+=640;
	}
}

void UpdateBackgd(void)
{
	if(backScroll)
	{
		backgdY++;
		if(backgdY==480)
			backgdY=0;
	}
}

void RenderStatusBar(byte *scr)
{
	int i;
	byte *src,*dst;

	src=statusBar;
	dst=scr+640-160;

	for(i=0;i<480;i++)
	{
		memcpy(dst,src,160);
		src+=160;
		dst+=640;
	}
}

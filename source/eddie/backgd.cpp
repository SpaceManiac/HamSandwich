#include "backgd.h"
#include "mgldraw.h"

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
		MGL_fatalError("Out of memory!");
	statusBar=(byte *)malloc(160*480);
	if(!statusBar)
		MGL_fatalError("Out of memory!");
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

void LoadBackgd(char *name)
{
	FILE *f;
	BITMAPFILEHEADER bmpFHead;
	BITMAPINFOHEADER bmpIHead;
	RGBQUAD	pal2[256];
	
	int i;
	byte *scr;

	f=fopen(name,"rb");
	if(!f)
		return;

	fread(&bmpFHead,sizeof(BITMAPFILEHEADER),1,f);
	fread(&bmpIHead,sizeof(BITMAPINFOHEADER),1,f);

	fread(pal2,sizeof(pal2),1,f);	

	for(i=0;i<bmpIHead.biHeight;i++)
	{
		scr=(byte *)((int)backgd+(bmpIHead.biHeight-1-i)*480);
		fread(scr,bmpIHead.biWidth,1,f);
	}
	fclose(f);
	backgdY=0;
}

void LoadStatus(void)
{
	FILE *f;
	BITMAPFILEHEADER bmpFHead;
	BITMAPINFOHEADER bmpIHead;
	RGBQUAD	pal2[256];
	
	int i;
	byte *scr;

	f=fopen("graphics\\status.bmp","rb");
	if(!f)
		return;

	fread(&bmpFHead,sizeof(BITMAPFILEHEADER),1,f);
	fread(&bmpIHead,sizeof(BITMAPINFOHEADER),1,f);

	fread(pal2,sizeof(pal2),1,f);	

	for(i=0;i<bmpIHead.biHeight;i++)
	{
		scr=(byte *)((int)statusBar+(bmpIHead.biHeight-1-i)*160);
		fread(scr,bmpIHead.biWidth,1,f);
	}
	fclose(f);
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
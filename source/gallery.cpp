#include "gallery.h"
#include "display.h"
#include "game.h"
#include "control.h"

galpic_t *galpix;
char tmp[1024];
byte bumpedPic;
static byte *image;

int GetNextTextBit(int start,char *s)
{
	char endChar;
	int pos;

	pos=0;

	if(s[start]=='"')
	{
		start++;
		endChar='"';
	}
	else
		endChar=',';

	while(1)
	{
		tmp[pos]=s[pos+start];
		if(s[pos+start]==endChar)
		{
			if(endChar==',')
			{
				tmp[pos]='\0';
				return start+pos+1;
			}
			else
			{
				if(s[pos+start+1]==endChar)	// double quote=way of showing single quote
				{
					start++;
				}
				else
				{
					tmp[pos]='\0';
					return start+pos+2;
				}
			}
		}
		if(pos+start>=(int)strlen(s))
		{
			tmp[pos]='\0';
			return pos+start;
		}
		pos++;
	}
	return start;
}

void ScanGalleryLine(char *txt,byte pic)
{
	int start;

	start=0;
	start=GetNextTextBit(start,txt);
	strcpy(galpix[pic].fname,tmp);
	start=GetNextTextBit(start,txt);
	strcpy(galpix[pic].title,tmp);
	start=GetNextTextBit(start,txt);
	strcpy(galpix[pic].artist,tmp);
	start=GetNextTextBit(start,txt);
	galpix[pic].x=atoi(tmp);
	start=GetNextTextBit(start,txt);
	galpix[pic].y=atoi(tmp);
	start=GetNextTextBit(start,txt);
	strcpy(galpix[pic].desc,tmp);
}

void SetupGalPix(Map *map)
{
	int i;

	for(i=0;i<100;i++)
	{
		if(profile.progress.goal[i])
			map->GetTile(galpix[i].x,galpix[i].y)->floor=i+268;
	}
}

void InitGallery(Map *map)
{
	FILE *f;
	int pos;
	char *s;

	galpix=new galpic_t[100];
	pos=0;
	f=fopen("gallery\\galpix.dat","rt");
	if(!f)
		return;

	s=new char[2048];

	while(fgets(s,2048,f))
	{
		ScanGalleryLine(s,pos);
		pos++;
		if(pos==100)
			break;
	}
	delete s;
	fclose(f);

	SetupGalPix(map);
}

void ExitGallery(void)
{
	delete[] galpix;
}
//--------------------------------
static byte showMode;

byte GalleryBump(int x,int y)
{
	int i;

	for(i=0;i<100;i++)
	{
		if(x==galpix[i].x && y==galpix[i].y)
		{
			bumpedPic=i;
			return 1;
		}
	}

	return 0;
}

void SetupShow(void)
{
	int i;
	char s[128];

	GetDisplayMGL()->ClearScreen();
	GetDisplayMGL()->Flip();

	if(profile.progress.goal[bumpedPic]==0)
	{
		DrawFillBox(0,0,639,479,32*1-4);
		sprintf(s,"This painting will be unveiled when you complete Gallery Goal #%d!",bumpedPic+1);
		PrintUnGlow(320-GetStrLength(s,2)/2,220,s,2);
		PrintUnGlow(170,440,"Press Fire, Weapon, or ESC to exit",2);
	}
	else if(showMode==0)
	{
		GetDisplayMGL()->LoadBMP("graphics\\title.bmp");
		DrawFillBox(0,0,639,479,32*1-4);

		sprintf(s,"#%03d",bumpedPic+1);
		i=GetStrLength(s,2)/2;
		PrintUnGlow(320-i,5,s,2);
		i=GetStrLength(galpix[bumpedPic].title,2)/2;
		PrintUnGlow(320-i,40,galpix[bumpedPic].title,2);
		DrawFillBox(320-i,60,320+i,30,0);
		sprintf(s,"By %s",galpix[bumpedPic].artist);
		i=GetStrLength(s,2)/2;
		PrintUnGlow(320-i,70,s,2);

		PrintUnGlowRect(10,120,630,400,16,galpix[bumpedPic].desc,1);

		PrintUnGlow(10,440,"Press Fire to view artwork!",2);
		PrintUnGlow(400,440,"Press Weapon or ESC to exit",2);
	}
	else
	{
		sprintf(s,"gallery\\%s",galpix[bumpedPic].fname);
		GetDisplayMGL()->LoadBMP(s);
	}
	for(i=0;i<480;i++)
		memcpy(&image[i*640],&GetDisplayMGL()->GetScreen()[i*GetDisplayMGL()->GetWidth()],640);
}

void InitShowPic(MGLDraw *mgl)
{

	image=new byte[640*480];
	showMode=0;
	SetupShow();

	GetTaps();
	GetArrowTaps();
	mgl->LastKeyPressed();
}

void ExitShowPic(void)
{
	GetDisplayMGL()->ClearScreen();
	GetDisplayMGL()->Flip();

	if(showMode==1)
	{
		GetDisplayMGL()->LoadBMP("graphics\\title.bmp");
	}

	delete image;
}

byte UpdateShowPic(int *lastTime,MGLDraw *mgl)
{
	byte t;
	char k;

	k=mgl->LastKeyPressed();
	t=GetTaps()|GetArrowTaps();

	if(*lastTime>TIME_PER_FRAME*5)
		*lastTime=TIME_PER_FRAME*5;

	while(*lastTime>=TIME_PER_FRAME)
	{
		mgl->Process();
		*lastTime-=TIME_PER_FRAME;
	}

	if(k==27)
		return 1;

	if(t&CONTROL_B1)
	{
		if(!profile.progress.goal[bumpedPic])
			return 1;
		showMode=1-showMode;
		SetupShow();
	}
	if(t&CONTROL_B2)
		return 1;

	return 0;
}

void RenderShowPic(MGLDraw *mgl)
{
	int i;

	for(i=0;i<480;i++)
		memcpy(&mgl->GetScreen()[i*mgl->GetWidth()],&image[i*640],640);
}

void ShowGalleryPic(void)
{
	byte done=0;
	int lastTime=1;

	InitShowPic(GetDisplayMGL());

	while(!done)
	{
		lastTime+=TimeLength();
		StartClock();
		done=UpdateShowPic(&lastTime,GetDisplayMGL());
		RenderShowPic(GetDisplayMGL());

		GetDisplayMGL()->Flip();

		if(!GetDisplayMGL()->Process())
			done=1;
		EndClock();
	}

	ExitShowPic();
}

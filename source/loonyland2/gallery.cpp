#include "gallery.h"
#include "mgldraw.h"
#include "control.h"
#include "display.h"
#include "player.h"
#include "title.h"
#include "pause.h"
#include "music.h"
#include "options.h"

static byte subcursor=0;
static char lastKey=0;
static byte oldc;
static byte reptCounter=0;
byte picToDrop=0;
static byte *backScr;

void ExitGallery(void)
{
#ifdef DIRECTORS
	int i;

	GetDisplayMGL()->ClearScreen();
	GetDisplayMGL()->Flip();
	GetDisplayMGL()->LoadBMP(TITLEBMP);
	GetDisplayMGL()->LastKeyPressed();
	for(i=0;i<480;i++)
		memcpy(&backScr[i*640],GetDisplayMGL()->GetScreen()+GetDisplayMGL()->GetWidth()*i,640);
	GetTaps();
	free(backScr);
#endif
}

void LoadGalPic(byte p,byte *backScr)
{
	int i;
	char s[64];

	GetDisplayMGL()->ClearScreen();
	GetDisplayMGL()->Flip();
	sprintf(s,"graphics/glr%02d.bmp",p);
	GetDisplayMGL()->LoadBMP(s);
	GetDisplayMGL()->LastKeyPressed();
	for(i=0;i<480;i++)
		memcpy(&backScr[i*640],GetDisplayMGL()->GetScreen()+GetDisplayMGL()->GetWidth()*i,640);
}

void InitGallery(void)
{
#ifdef DIRECTORS
	backScr=(byte *)malloc(640*480);
	if(!backScr)
		FatalError("Out of memory!");

	GetTaps();
	MakeNormalSound(SND_PAUSE);
	lastKey=0;
	subcursor=0;
	while(subcursor<GALLERY_PIX && opt.galPix[subcursor]==0)
		subcursor++;
	if(subcursor==GALLERY_PIX)
		subcursor=0;

	LoadGalPic(subcursor+1,backScr);

	reptCounter=5;
	oldc=255;
#endif
}

void HandleGalleryKeyPresses(MGLDraw *mgl)
{
	char k;

	k=mgl->LastKeyPressed();
	if(k)
		lastKey=k;
}

byte UpdateGallery(MGLDraw *mgl)
{
#ifdef DIRECTORS
	byte c;
	byte was;

	c=GetControls()|GetArrows();

	reptCounter++;
	if((!oldc) || (reptCounter>10))
		reptCounter=0;

	if((c&CONTROL_LF) && (!reptCounter))
	{
		was=subcursor;
		subcursor--;
		while(1)
		{
			if(subcursor<0 || subcursor>=GALLERY_PIX)
				subcursor=GALLERY_PIX-1;
			else if(subcursor==was)
				break;
			else if(opt.galPix[subcursor])
				break;
			else
				subcursor--;
		}
		LoadGalPic(subcursor+1,backScr);
		MakeNormalSound(SND_MENUSELECT);
	}
	if((c&CONTROL_RT) && (!reptCounter))
	{
		was=subcursor;
		subcursor++;
		while(1)
		{
			if(subcursor<0 || subcursor>=GALLERY_PIX)
				subcursor=0;
			else if(subcursor==was)
				break;
			else if(opt.galPix[subcursor])
				break;
			else
				subcursor++;
		}
		LoadGalPic(subcursor+1,backScr);
		MakeNormalSound(SND_MENUSELECT);
	}
	if((c&CONTROL_B1) && (!(oldc&CONTROL_B1)))
	{
		return 1;
	}
	if((c&CONTROL_B2) && (!(oldc&CONTROL_B2)))
	{
		// exit the gallery
		return 1;
	}

	oldc=c;

	HandleGalleryKeyPresses(mgl);
	if(lastKey==27)	// hit ESC to exit
	{
		return 1;
	}
#endif
	return 0;
}

void RenderGallery(byte *backScr)
{
	int i;

	for(i=0;i<480;i++)
		memcpy(GetDisplayMGL()->GetScreen()+GetDisplayMGL()->GetWidth()*i,&backScr[i*640],640);
}

void Gallery(MGLDraw *mgl)
{
#ifdef DIRECTORS
	byte done=0;
	int lastTime;

	InitGallery();
	lastTime=1;

	while(!done)
	{
		lastTime+=TimeLength();
		StartClock();
		if(lastTime>TIME_PER_FRAME*30)
			lastTime=TIME_PER_FRAME*30;

		while(lastTime>=TIME_PER_FRAME && !done)
		{
			done=UpdateGallery(mgl);
			lastTime-=TIME_PER_FRAME;
		}
		RenderGallery(backScr);
		mgl->Flip();
		EndClock();

		if(!mgl->Process())
			done=1;
	}

	ExitGallery();
#endif
}

byte HaveGallery(void)
{
	int i,c;

	c=0;
#ifdef DIRECTORS
	for(i=0;i<GALLERY_PIX;i++)
		if(opt.galPix[i])
			c++;
#endif
	return (c>0);
}

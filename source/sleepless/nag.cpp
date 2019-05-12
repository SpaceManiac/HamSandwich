#include "nag.h"
#include "display.h"
#include "game.h"
#include "jamulsound.h"

static byte *backScr,*shots;
static byte cursor;
static byte nagSize;
static byte ben[3];
static byte buyNow;
static int scrX,scrY,scrDX,scrDY;
static byte ang;

#define NUM_BENEFITS (7)
char benefit[NUM_BENEFITS][64]={
	"* Six Mystical Hammers to wield!",
	"* 20 Comic Goals to solve!",
	"* Unlock a ridiculous comic book!",
	"* Dozens of intense puzzles!",
	"* A secret so stupid, it will hurt ",
	"  your brain!",
	"* Upload your goals and earn trophies!",
};

static char nagBrt=0,nagBrtDir=1;
static int msx,msy;

byte MiniLoadBMP(byte w,byte pos)
{
	BITMAP *b;
	RGB newpal[256];
	int i,y;
	char s[64];
	byte *scrn;
	int pitch;

	sprintf(s,"graphics/preview%d.bmp",w);
	b=load_bitmap(s,newpal);
	if(b==NULL)
		return FALSE;

	y=(pos)*240;

	pitch=GetDisplayMGL()->GetWidth();
	scrn=GetDisplayMGL()->GetScreen();
	for(i=0;i<b->h;i++)
	{
		memcpy(&shots[(y+i)*320],b->line[i],b->w);
	}
	destroy_bitmap(b);
	return TRUE;
}

void ShiftPicsUp(void)
{
	int i,j;

	for(i=0;i<320;i++)
	{
		for(j=0;j<240;j++)
		{
			shots[i+j*320]=shots[i+(j+240)*320];
			shots[i+(j+240)*320]=shots[i+(j+480)*320];
		}
	}
	ben[0]=ben[1];
	ben[1]=ben[2];
	ben[2]=(byte)Random(7);
	while(ben[2]==ben[1] || ben[2]==ben[0])
		ben[2]=(byte)Random(7);
	MiniLoadBMP(ben[2],2);
}

void InitNag(void)
{
	int i;

	scrX=0;
	scrY=0;
	scrDX=0;
	scrDY=0;

	PlaySong("mr-tiny.ogg");
	JamulSoundPurge();
	GetDisplayMGL()->LastKeyPressed();
	GetTaps();
	GetArrowTaps();
	backScr=(byte *)malloc(640*480);
	if(!backScr)
		FatalError("Out of memory!");

	if(nagSize)
	{
		shots=(byte *)malloc(320*240*3);
		for(i=0;i<3;i++)	// pick 3 random shots
		{
			ben[i]=(byte)Random(7);
			if(i==1 && ben[i]==ben[0])
				i=0;
			if(i==2 && (ben[i]==ben[0] || ben[i]==ben[1]))
				i=1;
		}
		MiniLoadBMP(ben[0],0);
		MiniLoadBMP(ben[1],1);
		MiniLoadBMP(ben[2],2);
		GetDisplayMGL()->LoadBMP("graphics/nag.bmp");
		for(i=0;i<480;i++)
			memcpy(&backScr[i*640],GetDisplayMGL()->GetScreen()+GetDisplayMGL()->GetWidth()*i,640);
	}

	buyNow=0;
	cursor=0;
}

void ExitNag(void)
{
	free(backScr);
}

byte UpdateNag(int *lastTime,MGLDraw *mgl)
{
	char c;
	byte t;
	static byte fwClock;

	if(*lastTime>TIME_PER_FRAME*3)
		*lastTime=TIME_PER_FRAME*3;

	mgl->GetMouse(&msx,&msy);

	while(*lastTime>=TIME_PER_FRAME)
	{
		nagBrt+=nagBrtDir;
		if(nagBrt>10)
			nagBrtDir=-1;
		if(nagBrt<-5)
			nagBrtDir=1;

		scrY++;
		if(scrY==240)
		{
			ShiftPicsUp();
			scrY=0;
		}

		c=mgl->LastKeyPressed();
		t=GetTaps()|GetArrowTaps();

		if(t&CONTROL_UP)
		{
			cursor++;
			if(cursor>1)
				cursor=0;
			MakeNormalSound(SND_MENUCLICK);
		}
		if(t&CONTROL_DN)
		{
			cursor--;
			if(cursor>1)
				cursor=1;
			MakeNormalSound(SND_MENUCLICK);
		}

		if(msx>=340 && msx<=620 && msy>=330 && msy<=390)
		{
			if(cursor!=0)
				MakeNormalSound(SND_MENUCLICK);
			cursor=0;
		}
		if(msx>=340 && msx<=620 && msy>=400 && msy<=460)
		{
			if(cursor!=1)
				MakeNormalSound(SND_MENUCLICK);
			cursor=1;
		}

		if((t&(CONTROL_B1|CONTROL_B2)))
		{
			switch(cursor)
			{
				case 0:	// buy now
					buyNow=1;
					MakeNormalSound(SND_MENUSELECT);
					return 0;
					break;
				case 1:	// quit
					buyNow=0;
					MakeNormalSound(SND_MENUSELECT);
					return 0;
					break;
			}
		}

		if(mgl->MouseTap())
		{
			switch(cursor)
			{
				case 0:	// buy now
					if(msx>=340 && msx<=620 && msy>=330 && msy<=390)
					{
						buyNow=1;
						MakeNormalSound(SND_MENUSELECT);
						return 0;
					}
					break;
				case 1:	// quit
					if(msx>=340 && msx<=620 && msy>=400 && msy<=460)
					{
						buyNow=0;
						MakeNormalSound(SND_MENUSELECT);
						return 0;
					}
					break;
			}
		}

		if(c==27)
		{
			// continue playing
			buyNow=0;
			return 0;
		}
		*lastTime-=TIME_PER_FRAME;
	}
	return 1;
}

static void RenderButton(int x,int y,char *txt,byte on)
{
	/*
	DrawBox(x,y,x+100,y+30,0);
	DrawBox(x+1,y+1,x+99,y+29,10);
	DrawBox(x+2,y+2,x+98,y+28,20);
	DrawFillBox(x+3,y+3,x+97,y+27,31);
	CenterPrint(x+50,y+5,txt,-1,1);
	*/
	if(on)
		CenterPrint(x,y,txt,nagBrt,0);
	else
	{
		CenterPrint(x,y,txt,-32,0);
		CenterPrint(x-5,y-5,txt,0,0);
	}
}

void RenderNagBig(MGLDraw *mgl)
{
	int i;
	byte *data;

	for(i=0;i<480;i++)
	{
		data=mgl->GetScreen()+mgl->GetWidth()*i+320;
		memcpy(data,&backScr[i*640+320],320);
		data-=320;
		memcpy(data,&shots[(i+scrY)*320],320);
	}

	for(i=0;i<NUM_BENEFITS;i++)
	{
		PrintGlow(350,i*30+100,benefit[i],0,1);
	}
	RenderButton(320+160,330,"Order Now!",(cursor==0));
	RenderButton(320+160,400,"Exit",(cursor==1));
	DrawMouseCursor(msx,msy);
}

byte Nag(MGLDraw *mgl,byte full)
{
	byte b;
	int lastTime=1;
	dword now,then,hangon;

	nagSize=full;
	EndClock();
	hangon=TimeLength();

	now=timeGetTime();
	InitNag();
	b=1;
	while(b==1)
	{
		lastTime+=TimeLength();
		StartClock();
		b=UpdateNag(&lastTime,mgl);
		RenderNagBig(mgl);

		mgl->Flip();
		if(!mgl->Process())
		{
			ExitNag();
			return 0;
		}
		EndClock();
	}
	then=timeGetTime();
	ExitNag();

	return buyNow;
}

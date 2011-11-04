#include "mgldraw.h"
#include "game.h"
#include "sound.h"
#include <allegro.h>
#include <winalleg.h>
#include "internet.h"

MGLDraw *_globalMGLDraw;
byte needPalRealize=0;
byte firstStartup;
static byte exitNow=0;
HWND myHwnd;
HINSTANCE hInst;

bool CreateMyOwnWindow(HINSTANCE hInsta,char *wndName)
{
	WNDCLASS wc;
	char className[]="supremeWindow";

	hInst=hInsta;
	// first thing needed is to make a window, so directsound can have it
	wc.style=CS_HREDRAW|CS_VREDRAW;
	wc.lpfnWndProc=MyWindowsEventHandler;
	wc.cbClsExtra=0;
	wc.cbWndExtra=0;
	wc.hInstance=hInst;
	wc.hIcon=LoadIcon(hInst,IDI_APPLICATION);
	wc.hCursor=LoadCursor(NULL,IDC_ARROW);
	wc.hbrBackground=NULL;
	wc.lpszMenuName=NULL;
	wc.lpszClassName=className;
	if(!RegisterClass(&wc))
		return FALSE;

	myHwnd=CreateWindowEx(
		WS_EX_APPWINDOW,
		className,
		wndName,
		WS_SYSMENU|WS_CAPTION|WS_MINIMIZEBOX,
		0,
		0,
		650,
		540,
		NULL,
		NULL,
		hInst,
		NULL);

	if(!myHwnd)
		return FALSE;

	ShowWindow(myHwnd,SW_SHOWNORMAL);
	ShowCursor(0);
	UpdateWindow(myHwnd);

	return TRUE;
}

void DestroyMyWindow(void)
{
	DestroyWindow(myHwnd);
	UnregisterClass("supremeWindow",hInst);
}

MGLDraw::MGLDraw(HINSTANCE hInst,char *name,int xRes,int yRes,int bpp,bool window)
{
	RECT rect;
	int result;

	//if(!CreateMyOwnWindow(hInst,name))
	//	FatalError("Couldn't create window!");

	//win_set_window(myHwnd);
	allegro_init();
	install_keyboard();
	install_mouse();
	SeedRNG();

	if(JamulSoundInit(512))
		SoundSystemExists();

	set_color_conversion(COLORCONV_NONE);
	set_color_depth(bpp);
	if(window)
		result=set_gfx_mode(GFX_AUTODETECT_WINDOWED,xRes,yRes,0,0);
	else
		result=set_gfx_mode(GFX_AUTODETECT_FULLSCREEN,xRes,yRes,0,0);

	set_display_switch_mode(SWITCH_BACKGROUND);
	set_display_switch_callback(SWITCH_OUT, PauseGame);
	set_window_title(name);

	if(result<0)
		FatalError("Unable to initialize video mode!");

	readyToQuit=false;
	_globalMGLDraw=this;

	// gimme windows colors
	this->xRes=xRes;
	this->yRes=yRes;
	this->bpp=bpp;
	this->pitch=xRes;
	this->buffer=create_bitmap(xRes, yRes);
	this->scrn=new byte[xRes*yRes];
	if(!this->scrn)
		FatalError("Out of memory!");

	this->windowed=window;
	this->hInst=hInst;

	if(window)
	{
		GetWindowRect(win_get_window(),&rect);
		windowX=rect.left+3;
		windowY=rect.top+23;
	}
	else
	{
		windowX=0;
		windowY=0;
	}

	mouseDown=0;
	rMouseDown=0;

	lastRawCode=0;
	SetLastKey(0);
	SeedRNG();
	tapTrack=mouse_b;
}

MGLDraw::~MGLDraw(void)
{
	exitNow=1;
	JamulSoundExit();
	destroy_bitmap(buffer);
	delete[] this->scrn;
	allegro_exit();
//	DestroyMyWindow();
}

void MGLDraw::GetMouse(int *x,int *y)
{
	*x=mouse_x;
	*y=mouse_y;
}

void MGLDraw::SetMouse(int x,int y)
{
	position_mouse(x,y);
}

void MGLDraw::ResetTimer(void)
{
	elapsedTime=0;
	now=timeGetTime();
	numFrames=0;
}

void MGLDraw::TickTimer(void)
{
	dword then;

	then=now;
	now=timeGetTime();
	elapsedTime+=(now-then);
	numFrames++;
}

float MGLDraw::FrameRate(void)
{
	return (float)(((float)numFrames*1000)/(float)elapsedTime);
}

bool MGLDraw::Process(void)
{
	return (!readyToQuit);
}

HWND MGLDraw::GetHWnd(void)
{
	return win_get_window();
}

int MGLDraw::FormatPixel(int x,int y)
{
	byte b = scrn[y*xRes+x];
	return makecol32((*thePal)[b].r*4, (*thePal)[b].g*4, (*thePal)[b].b*4);
}

void MGLDraw::PseudoCopy(int y,int x,byte* data,int len)
{
	for(int i = 0; i < len; ++i, ++x)
		putpixel(buffer, x, y, makecol32((*thePal)[data[i]].r*4, (*thePal)[data[i]].g*4, (*thePal)[data[i]].b*4));
}

void MGLDraw::FinishFlip(void)
{
	vsync();
	acquire_screen();
	blit(buffer, screen, 0, 0, 0, 0, xRes, yRes);
	release_screen();
}

void MGLDraw::Flip(void)
{
	int i;

	vsync();
	// blit to the screen
	for(i=0;i<yRes;i++)
		PseudoCopy(i,0,&scrn[i*xRes],xRes);
	FinishFlip();
}

void MGLDraw::WaterFlip(int v)
{
	int i;
	char table[24]={ 0, 1, 1, 1, 2, 2, 2, 2,
		             2, 2, 1, 1, 0,-1,-1,-1,
				    -1,-2,-2,-2,-2,-1,-1,-1};
	v=v%24;

	// blit to the screen
	for(i=0;i<yRes;i++)
	{
		if(table[v]==0)
			PseudoCopy(i,0,&scrn[i*xRes],xRes);
		else if(table[v]<0)
		{
			PseudoCopy(i,0,&scrn[i*xRes-table[v]],xRes+table[v]);
			PseudoCopy(i,xRes+table[v],&scrn[i*xRes],-table[v]);
		}
		else
		{
			PseudoCopy(i,0,&scrn[i*xRes+xRes-table[v]],table[v]);
			PseudoCopy(i,table[v],&scrn[i*xRes],xRes-table[v]);
		}
		if(i&1)
		{
			v++;
			if(v>23)
				v=0;
		}
	}
	FinishFlip();
}

void MGLDraw::TeensyFlip(void)
{
	int i,j,x,y;

	//clear_bitmap(screen);
	x=640/4;
	y=480/4;
	// blit to the screen
	for(i=0;i<yRes/2;i++)
	{
		for(j=0;j<xRes/2;j++)
			putpixel(buffer,x+j,y,FormatPixel(j*2,i*2));
		y++;
	}
	FinishFlip();
}

void MGLDraw::TeensyWaterFlip(int v)
{
	int i,j,x,y;
	char table[24]={ 0, 1, 1, 1, 2, 2, 2, 2,
		             2, 2, 1, 1, 0,-1,-1,-1,
				    -1,-2,-2,-2,-2,-1,-1,-1};
	v=v%24;

	x=640/4;
	y=480/4;
	// blit to the screen
	for(i=0;i<yRes/2;i++)
	{
		putpixel(buffer,x-1-table[v],y,0);
		putpixel(buffer,x+xRes/2-table[v],y,0);
		for(j=0;j<xRes/2;j++)
		{
			putpixel(buffer,x+j-table[v],y,FormatPixel(j*2,i*2));
		}
		if(i&1)
		{
			v++;
			if(v>23)
				v=0;
		}
		y++;
	}
	FinishFlip();
}


void MGLDraw::RasterFlip(void)
{
	int i,j;

	// blit to the screen
	for(i=0;i<yRes;i++)
	{
		if(!(i&1))
		{
			PseudoCopy(i,0,&scrn[i*xRes],xRes);
		}
		else
		{
			for(j=0;j<xRes;j++)
				putpixel(buffer,j,i,0);
		}
	}
	FinishFlip();
}

void MGLDraw::RasterWaterFlip(int v)
{
	int i,j;
	char table[24]={ 0, 1, 1, 1, 2, 2, 2, 2,
		             2, 2, 1, 1, 0,-1,-1,-1,
				    -1,-2,-2,-2,-2,-1,-1,-1};
	v=v%24;

	// blit to the screen
	for(i=0;i<yRes;i++)
	{
		if(!(i&1))
		{
			if(table[v]==0)
				PseudoCopy(i,0,&scrn[i*xRes],xRes);
			else if(table[v]<0)
			{
				PseudoCopy(i,0,&scrn[i*xRes-table[v]],xRes+table[v]);
				PseudoCopy(i,xRes+table[v],&scrn[i*xRes],-table[v]);
			}
			else
			{
				PseudoCopy(i,0,&scrn[i*xRes+xRes-table[v]],table[v]);
				PseudoCopy(i,table[v],&scrn[i*xRes],xRes-table[v]);
			}
		}
		else
		{
			for(j=0;j<xRes;j++)
				putpixel(buffer,j,i,0);
		}
		if(i&1)
		{
			v++;
			if(v>23)
				v=0;
		}
	}
	FinishFlip();
}

void MGLDraw::ClearScreen(void)
{
	memset(scrn,0,640*480);
}

byte *MGLDraw::GetScreen(void)
{
	return scrn;
}

int MGLDraw::GetWidth(void)
{
	return pitch;
}

int MGLDraw::GetHeight(void)
{
	return yRes;
}

void MGLDraw::Quit(void)
{
	readyToQuit=true;
}

typedef struct palfile_t
{
	char r,g,b;
} palfile_t;

bool MGLDraw::LoadPalette(char *name)
{
	FILE *f;
	palfile_t p[256];
	int i;

	f=fopen(name,"rb");
	if(!f)
		return false;

	if(fread(p,sizeof(palfile_t),256,f)!=256)
	{
		fclose(f);
		return false;
	}

	for(i=0;i<256;i++)
	{
		pal[i].r=p[i].r;
		pal[i].g=p[i].g;
		pal[i].b=p[i].b;
	}

	RealizePalette();

	fclose(f);
	return true;
}

void MGLDraw::SetPalette(PALETTE pal2)
{
	int i;

	for(i=0;i<256;i++)
	{
		pal[i].r=pal2[i].r/4;
		pal[i].g=pal2[i].g/4;
		pal[i].b=pal2[i].b/4;
	}
}

RGB *MGLDraw::GetPalette(void)
{
	return pal;
}

void MGLDraw::RealizePalette(void)
{
	thePal = &pal;
}

void MGLDraw::WaterPalette(byte b)
{
	int i,j;

	for(i=0;i<8;i++)
		for(j=0;j<32;j++)
		{
			pal2[i*32+j]=pal[i*32+j];
			if(b==0)
			{
				pal2[i*32+j].b+=40;
				if(pal2[i*32+j].b>63)
				{
					pal2[i*32+j].b=63;
				}
			}
			else
			{
				pal2[i*32+j].r+=40;
				if(pal2[i*32+j].r>63)
				{
					pal2[i*32+j].r=63;
				}
			}
		}

	thePal = &pal2;
}

// 8-bit graphics only
void MGLDraw::Box(int x,int y,int x2,int y2,byte c)
{
	int i;
	byte noleft=0,noright=0;
	byte notop=0,nobottom=0;

	if(x>x2)
	{
		i=x;
		x=x2;
		x2=i;
	}
	if(y>y2)
	{
		i=y;
		y=y2;
		y2=i;
	}
	if(x<0)
	{
		noleft=1;
		x=0;
	}
	if(x>=xRes)
		return;

	if(y<0)
	{
		notop=1;
		y=0;
	}
	if(y>=yRes)
		return;
	if(x2<0)
		return;
	if(x2>=xRes)
	{
		noright=1;
		x2=xRes-1;
	}
	if(y2<0)
		return;
	if(y2>=yRes)
	{
		nobottom=1;
		y2=yRes-1;
	}

	if(!notop)
		memset(&scrn[x+y*pitch],c,x2-x+1);
	if(!nobottom)
		memset(&scrn[x+y2*pitch],c,x2-x+1);
	for(i=y;i<=y2;i++)
	{
		if(!noleft)
			scrn[x+i*pitch]=c;
		if(!noright)
			scrn[x2+i*pitch]=c;
	}
}

void MGLDraw::FillBox(int x,int y,int x2,int y2,byte c)
{
	int i;

	if(y>=yRes)
		return;

	if(x<0)
		x=0;
	if(x>=xRes)
		return;

	if(y<0)
		y=0;
	if(y>=yRes)
		y=yRes-1;
	if(x2<0)
		return;
	if(x2>=xRes)
		x2=xRes-1;
	if(y2<0)
		return;
	if(y2>=yRes)
		y2=yRes-1;

	for(i=y;i<=y2;i++)
	{
		memset(&scrn[x+i*pitch],c,x2-x+1);
	}
}

void MGLDraw::SelectLineH(int x,int x2,int y,byte ofs)
{
	int i;
	byte col=ofs;
	byte *scr;

	if(x>x2)
	{
		i=x;
		x=x2;
		x2=i;
	}

	if(x<0)
		x=0;
	if(x>=xRes)
		x=xRes-1;
	if(y<0)
		return;
	if(y>=yRes)
		return;
	if(x2<0)
		return;
	if(x2>=xRes)
		x2=xRes-1;

	scr=&scrn[x+y*pitch];
	for(i=x;i<x2;i++)
	{
		if(col<4)
			*scr=31;	// white
		else
			*scr=0;		// black
		col++;
		if(col>7)
			col=0;
		scr++;
	}
}

void MGLDraw::SelectLineV(int x,int y,int y2,byte ofs)
{
	int i;
	byte col=ofs;
	byte *scr;

	if(y>y2)
	{
		i=y;
		y=y2;
		y2=i;
	}

	if(x<0)
		return;
	if(x>=xRes)
		return;
	if(y<0)
		y=0;
	if(y>=yRes)
		y=yRes-1;
	if(y2<0)
		return;
	if(y2>=yRes)
		y2=yRes-1;

	scr=&scrn[x+y*pitch];
	for(i=y;i<y2;i++)
	{
		if(col<4)
			*scr=31;	// white
		else
			*scr=0;		// black
		col++;
		if(col>7)
			col=0;
		scr+=pitch;
	}
}

void MGLDraw::SetLastKey(char c)
{
	lastKeyPressed=c;
}

int MGLDraw::LastRawCode(void)
{
	int r;

	r=lastRawCode;
	lastRawCode=0;
	return r;
}
char MGLDraw::LastKeyPressed(void)
{
	if(keypressed())
	{
		lastRawCode=readkey();
		return (lastRawCode&0xff);
	}
	else
		return 0;
}

void MGLDraw::ClearKeys(void)
{
	while(keypressed())
		readkey();
	lastRawCode=0;
}

void MGLDraw::SetMouseDown(byte w)
{
	mouseDown=w;
}

void MGLDraw::SetRMouseDown(byte w)
{
	rMouseDown=w;
}

byte MGLDraw::MouseTap(void)
{
	byte b,mb;

	mb=mouse_b&1;

	if((mb&1) && !(tapTrack&1))
		b=1;
	else
		b=0;

	tapTrack&=2;
	tapTrack|=mb;
	return b;
}

byte MGLDraw::RMouseTap(void)
{
	byte b,mb;

	mb=mouse_b&2;

	if((mb&2) && !(tapTrack&2))
		b=1;
	else
		b=0;

	tapTrack&=1;
	tapTrack|=mb;
	return b;
}

byte MGLDraw::MouseDown(void)
{
	return ((mouse_b&1)!=0);
}

byte MGLDraw::RMouseDown(void)
{
	return ((mouse_b&2)!=0);
}

char MGLDraw::LastKeyPeek(void)
{
	return lastKeyPressed;
}

bool MGLDraw::LoadBMP(char *name)
{
	BITMAP *b;
	RGB newpal[256];
	int i,w;

	b=load_bitmap(name,newpal);
	if(b==NULL)
		return FALSE;

	for(i=0;i<256;i++)
	{
		pal[i].r=newpal[i].r;
		pal[i].g=newpal[i].g;
		pal[i].b=newpal[i].b;
	}
	RealizePalette();

	w=b->w;
	if(w>SCRWID)
		w=SCRWID;

	for(i=0;i<b->h;i++)
	{
		if(i<SCRHEI)
			memcpy(&scrn[i*pitch],b->line[i],w);
	}

	destroy_bitmap(b);
	return TRUE;
}

bool MGLDraw::LoadBMPNoPalette(char *name)
{
	BITMAP *b;
	RGB newpal[256];
	int i,w;

	b=load_bitmap(name,newpal);
	if(b==NULL)
		return FALSE;

	w=b->w;
	if(w>SCRWID)
		w=SCRWID;

	for(i=0;i<b->h;i++)
	{
		if(i<SCRHEI)
			memcpy(&scrn[i*pitch],b->line[i],w);
	}
	destroy_bitmap(b);
	return TRUE;
}

//--------------------------------------------------------------------------
// these are all external stuff

byte fatalDie=0;
char errMsg[128];

HWND MGLGetHWnd(void)
{
	return _globalMGLDraw->GetHWnd();
}

void SeedRNG(void)
{
  srand(timeGetTime());
}

dword Random(dword range)
{
	return (rand()%range);
}

void FatalError(char *msg)
{
	_globalMGLDraw->Quit();
	fatalDie=1;
	strcpy(errMsg,msg);
}

void FatalErrorQuit(void)
{
	if(fatalDie)
		MessageBox(NULL,errMsg,"Fatal Error!!",MB_ICONERROR|MB_OK);
}

long FAR PASCAL MyWindowsEventHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	byte b;

	switch(message)
    {
		case WM_VSCROLL:
		//case WM_MOUSEWHEEL:
			// scroll wheel moved
			b=1;
			break;
		default:
			// Hand off unprocessed messages to DefWindowProc
			return DefWindowProc(hwnd,message,wParam,lParam);
			break;
	}
	return DefWindowProc(hwnd,message,wParam,lParam);
}

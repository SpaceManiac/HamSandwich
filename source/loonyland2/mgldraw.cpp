#include "mgldraw.h"
#include "game.h"
#include "sound.h"
#include "music.h"

#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_image.h>

MGLDraw *_globalMGLDraw;

int KEY_MAX;
const Uint8* key = SDL_GetKeyboardState(&KEY_MAX);

MGLDraw::MGLDraw(char *name,int xRes,int yRes,int bpp,bool windowed)
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
	SeedRNG();

	if(JamulSoundInit(512))
		SoundSystemExists();

	Uint32 flags = windowed ? 0 : SDL_WINDOW_FULLSCREEN;
	window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, xRes, yRes, flags);
	if (!window) {
		printf("SDL_CreateWindow: %s\n", SDL_GetError());
		FatalError("Failed to create window");
		return;
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	if (!renderer) {
		printf("SDL_CreateWindow: %s\n", SDL_GetError());
		FatalError("Failed to create renderer");
		return;
	}

	SDL_SetWindowTitle(window, name);
	SDL_ShowCursor(SDL_DISABLE);

	readyToQuit=false;
	_globalMGLDraw=this;

	// gimme windows colors
	this->xRes=xRes;
	this->yRes=yRes;
	this->bpp=bpp;
	this->pitch=xRes;
	this->texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, xRes, yRes);
	this->scrn=new byte[xRes*yRes];
	if(!this->scrn)
		FatalError("Out of memory!");

	this->windowed=windowed;

	mouseDown=0;
	rMouseDown=0;
	mouse_x = xRes/2;
	mouse_y = yRes/2;
	mouse_b = 0;
	mouse_z = 0;

	lastRawCode=0;
	SetLastKey(0);
	SeedRNG();
	tapTrack=mouse_b;
}

MGLDraw::~MGLDraw(void)
{
	JamulSoundExit();
	delete[] scrn;
//	DestroyMyWindow();
}

void MGLDraw::GetMouse(int *x,int *y)
{
	*x=mouse_x;
	*y=mouse_y;
}

void MGLDraw::SetMouse(int x,int y)
{
	SDL_WarpMouseInWindow(window, x, y);
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
	UpdateMusic();
	return (!readyToQuit);
}

#ifdef WIN32
HWND MGLDraw::GetHWnd(void)
{
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	SDL_GetWindowWMInfo(window, &info);
	return info.info.win.window;
}
#endif

int makecol32(int r, int g, int b)
{
	return (r << 24) | (g << 16) | (b << 8);
}

#define putpixel(buffer, x, y, value) (buffer)[(y)*pitch + (x)] = (value)

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

void MGLDraw::StartFlip(void)
{
	void* p;
	SDL_LockTexture(texture, NULL, &p, &pitch);
	buffer = (int*) p;
	pitch /= sizeof(int);
}

void MGLDraw::FinishFlip(void)
{
	SDL_UnlockTexture(texture);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
	UpdateMusic();
	buffer = nullptr;

	SDL_Event e;
	while(SDL_PollEvent(&e)) {
		if (e.type == SDL_KEYDOWN) {
			ControlKeyDown(e.key.keysym.scancode);
			lastRawCode = e.key.keysym.scancode;
			if (!(e.key.keysym.sym & ~0xff))
			{
				lastKeyPressed = e.key.keysym.sym;
				if (e.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
				{
					lastKeyPressed = toupper(lastKeyPressed);
				}
			}
		} else if (e.type == SDL_KEYUP) {
			ControlKeyUp(e.key.keysym.scancode);
		} else if (e.type == SDL_MOUSEMOTION) {
			mouse_x = e.motion.x;
			mouse_y = e.motion.y;
		} else if (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP) {
			int flag = 0;
			if (e.button.button == 1)
				flag = 1;
			else if (e.button.button == 3)
				flag = 2;
			if (e.button.state == SDL_PRESSED)
				mouse_b |= flag;
			else
				mouse_b &= ~flag;
		} else if (e.type == SDL_QUIT) {
			readyToQuit = 1;
		} else if (e.type == SDL_WINDOWEVENT) {
			if (e.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
				SetGameIdle(true);
			} else if (e.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
				SetGameIdle(false);
			}
		}
	}
}

void MGLDraw::Flip(void)
{
	int i;

	StartFlip();
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

	StartFlip();
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

	x=640/4;
	y=480/4;
	// blit to the screen
	StartFlip();
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
	StartFlip();
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
	StartFlip();
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
	StartFlip();
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

struct palfile_t
{
	char r,g,b;
};

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

void MGLDraw::DarkBox(int x,int y,int x2,int y2,byte amt)
{
	int i,j;
	byte b;

	if(y>=yRes)
		return;

	if(x<0)
		x=0;
	if(x>=xRes)
		x=xRes-1;
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

	for(j=y;j<=y2;j++)
	{
		for(i=x;i<x2;i++)
		{
			b=scrn[i+j*pitch];
			if((b&31)<amt)
				scrn[i+j*pitch]=0;
			else
				scrn[i+j*pitch]=b-amt;
		}
	}
}

void MGLDraw::BrightBox(int x,int y,int x2,int y2,byte amt)
{
	int i,j;
	byte b;

	if(y>=yRes)
		return;

	if(x<0)
		x=0;
	if(x>=xRes)
		x=xRes-1;
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

	for(j=y;j<=y2;j++)
	{
		for(i=x;i<=x2;i++)
		{
			b=scrn[i+j*pitch];
			if((b&31)>(31-amt))
				scrn[i+j*pitch]=31;
			else
				scrn[i+j*pitch]=b+amt;
		}
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

int MGLDraw::LastKeyPressed(void)
{
	int i = lastKeyPressed;
	lastKeyPressed = 0;
	return i;
}

void MGLDraw::ClearKeys(void)
{
	lastKeyPressed = 0;
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
	return LoadBMP(name, pal);
}

bool MGLDraw::LoadBMP(char *name, PALETTE pal)
{
	int i,w;

	SDL_Surface* b = IMG_Load(name);
	if (!b) {
		printf("%s: %s\n", name, SDL_GetError());
		return FALSE;
	}

	if(pal && b->format->palette)
		for(i=0; i<256 && i < b->format->palette->ncolors; i++)
		{
			pal[i].r = b->format->palette->colors[i].r/4;
			pal[i].g = b->format->palette->colors[i].g/4;
			pal[i].b = b->format->palette->colors[i].b/4;
		}
	RealizePalette();

	w=b->w;
	if(w>SCRWID)
		w=SCRWID;

	SDL_LockSurface(b);
	for(i=0;i<b->h;i++)
	{
		if(i<SCRHEI)
			memcpy(&scrn[i*pitch], &((byte*) b->pixels)[b->pitch * i], w);
	}

	SDL_UnlockSurface(b);
	SDL_FreeSurface(b);
	return TRUE;
}

//--------------------------------------------------------------------------
// these are all external stuff

byte fatalDie=0;
char errMsg[128];

void SeedRNG(void)
{
	srand(timeGetTime());
}

dword Random(dword range)
{
	return rand() * range / (RAND_MAX+1);
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
	{
		fprintf(stderr, "%s\n", errMsg);
	}
}

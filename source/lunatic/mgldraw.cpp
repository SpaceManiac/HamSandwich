#include "mgldraw.h"
#include "winpch.h"
#include "game.h"	// for SetGameIdle and GetGameIdle only
#include "sound.h"
#include "music.h"
#include "ctype.h"
#include <stdio.h>
#include <random>

#include <sys/stat.h>

#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_image.h>

// Appdata shenanigans
#ifdef _WIN32
#include <shlobj.h> // for SHGetFolderPath

#ifdef _MSC_VER
#include <direct.h>
#endif

FILE* AppdataOpen(const char* file, const char* mode)
{
	char buffer[MAX_PATH];
	SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, buffer);
	sprintf(buffer + strlen(buffer), "\\Hamumu");
	mkdir(buffer);
	sprintf(buffer + strlen(buffer), "\\DrLunatic");
	mkdir(buffer);
	sprintf(buffer + strlen(buffer), "\\%s", file);
	return fopen(buffer, mode);
}

#else

FILE* AppdataOpen(const char* file, const char* mode)
{
    return fopen(file, mode);
}

#endif

// Replacements for missing MGL functions
std::mt19937_64 mersenne;

int MGL_random(int max)
{
	return std::uniform_int_distribution<int>(0, max - 1)(mersenne);
}

void MGL_srand(int seed)
{
	mersenne.seed(seed);
}

long MGL_randoml(long max)
{
	return std::uniform_int_distribution<long>(0, max - 1)(mersenne);
}

MGLDraw* _globalMGLDraw;

void MGL_fatalError(const char* txt)
{
	_globalMGLDraw->Quit();
	printf("%s\n", txt);
	exit(1);
}

MGLDraw::MGLDraw(const char *name, int xRes, int yRes, bool windowed)
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);

	if(JamulSoundInit(512))
		SoundSystemExists();

	Uint32 flags = windowed ? 0 : SDL_WINDOW_FULLSCREEN;
	window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, xRes, yRes, flags);
	if (!window) {
		printf("SDL_CreateWindow: %s\n", SDL_GetError());
		MGL_fatalError("Failed to create window");
		return;
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	if (!renderer) {
		printf("SDL_CreateWindow: %s\n", SDL_GetError());
		MGL_fatalError("Failed to create renderer");
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
	this->scrn=new byte[xRes * yRes];
	this->windowed=windowed;

	mouseDown=0;
	rMouseDown=0;
	mouse_x = xRes/2;
	mouse_y = yRes/2;
	mouse_b = 0;
	mouse_z = 0;

	lastRawCode=0;
	SetLastKey(0);
	MGL_srand(timeGetTime());
	tapTrack=mouse_b;
}

MGLDraw::~MGLDraw()
{
	JamulSoundExit();
	delete[] scrn;
}

void MGLDraw::GetMouse(int *x,int *y)
{
	*x=mouse_x;
	*y=mouse_y;
}

void MGLDraw::TeleportMouse(int x,int y)
{
	SDL_WarpMouseInWindow(window, x, y);
}

bool MGLDraw::Process(void)
{
	//UpdateMusic();
	return (!readyToQuit);
}

#ifdef _WIN32
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
	return makecol32(pal[b].r, pal[b].g, pal[b].b);
}

void MGLDraw::PseudoCopy(int y,int x,byte* data,int len)
{
	for(int i = 0; i < len; ++i, ++x)
		putpixel(buffer, x, y, makecol32(pal[data[i]].r, pal[data[i]].g, pal[data[i]].b));
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
	//UpdateMusic();
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

void MGLDraw::ClearScreen(void)
{
	memset(&scrn[0],0,xRes*yRes);
}

byte *MGLDraw::GetScreen(void)
{
	return &scrn[0];
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

bool MGLDraw::LoadPalette(const char *name)
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

	fclose(f);
	return true;
}

void MGLDraw::SetPalette(const RGB *pal2)
{
	int i;

	for(i=0;i<256;i++)
	{
		pal[i].r=pal2[i].r;
		pal[i].g=pal2[i].g;
		pal[i].b=pal2[i].b;
	}
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

void MGLDraw::SetLastKey(char c)
{
	lastKeyPressed=c;
}

char MGLDraw::LastKeyPressed(void)
{
	int i = lastKeyPressed;
	lastKeyPressed = 0;
	return i;
}

void MGLDraw::SetMouseDown(byte w)
{
	mouseDown=w;
}

byte MGLDraw::MouseDown(void)
{
	return ((mouse_b&1)!=0);
}

char MGLDraw::LastKeyPeek(void)
{
	return lastKeyPressed;
}

bool MGLDraw::LoadBMP(const char *name)
{
	return LoadBMP(name, pal);
}

bool MGLDraw::LoadBMP(const char *name, PALETTE pal)
{
	int i,w;

	SDL_Surface* b = IMG_Load(name);
	if (!b) {
		printf("%s: %s\n", name, SDL_GetError());
		return false;
	}

	if(pal && b->format->palette)
		for(i=0; i<256 && i < b->format->palette->ncolors; i++)
		{
			pal[i].r = b->format->palette->colors[i].r;
			pal[i].g = b->format->palette->colors[i].g;
			pal[i].b = b->format->palette->colors[i].b;
		}

	w=b->w;
	if(w>xRes)
		w=xRes;

	SDL_LockSurface(b);
	for(i=0;i<b->h;i++)
	{
		if(i<yRes)
			memcpy(&scrn[i*pitch], &((byte*) b->pixels)[b->pitch * i], w);
	}

	SDL_UnlockSurface(b);
	SDL_FreeSurface(b);
	return true;
}

void MGLDraw::GammaCorrect(byte gamma)
{
	int i;
	int r, g, b;
	RGB temp[256];

	memcpy(temp, pal, sizeof (RGB)*256);
	for (i = 0; i < 256; i++)
	{
		r = pal[i].r;
		g = pal[i].g;
		b = pal[i].b;
		r = (r * (gamma + 4)) / 4;
		g = (g * (gamma + 4)) / 4;
		b = (b * (gamma + 4)) / 4;
		if (r > 255)
			r = 255;
		if (g > 255)
			g = 255;
		if (b > 255)
			b = 255;
		pal[i].r = r;
		pal[i].g = g;
		pal[i].b = b;
	}
	memcpy(pal, temp, sizeof (RGB)*256);
}

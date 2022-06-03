#include "mgldraw.h"
#include "clock.h"
#include "jamulsound.h"
#include "hammusic.h"
#include "log.h"
#include "softjoystick.h"
#include "appdata.h"
#include "extern.h"
#include <time.h>
#include <random>
#include <algorithm>

#include <SDL_image.h>
#ifdef _WIN32
	#include <SDL_syswm.h>
#endif  // _WIN32

#ifdef __EMSCRIPTEN__
	#include <emscripten.h>
#endif  // _EMSCRIPTEN__

// in control.cpp
void ControlKeyDown(byte scancode);
void ControlKeyUp(byte scancode);

static const RGB BLACK = {0, 0, 0, 0};

static MGLDraw *_globalMGLDraw = nullptr;

static bool pixelMode = false;

MGLDraw::MGLDraw(const char *name, int xRes, int yRes, bool windowed)
	: mouse_x(xRes / 2)
	, mouse_y(yRes / 2)
	, mouse_z(0)
	, mouse_b(0)
	, windowed(windowed)
	, readyToQuit(false)
	, idle(false)
	, xRes(xRes)
	, yRes(yRes)
	, pitch(xRes)
	, winWidth(xRes)
	, winHeight(yRes)
	, scrn(nullptr)
	, tapTrack(0)
	, lastKeyPressed(0)
	, lastRawCode(0)
	, buffer(nullptr)
{
	_globalMGLDraw = this;

#ifdef __EMSCRIPTEN__
	// These don't actually do anything under Emscripten, and they log a debug
	// message "Calling stub instead of sigaction()", so turn them off.
	SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1");

	// Emscripten builds don't meaningfully receive argv, so they should not
	// start fullscreen by default.
	this->windowed = windowed = true;
#endif  // __EMSCRIPTEN__

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK)) {
		LogError("SDL_Init(VIDEO|JOYSTICK): %s", SDL_GetError());
		FatalError("Failed to initialize SDL");
		return;
	}

#ifdef __ANDROID__
	// On Android, tabbing out of the application may generate an SDL_Quit,
	// which is received once the application is re-entered.
	SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
#endif

	if(JamulSoundInit(512) && g_HamExtern.SoundSystemExists)
		g_HamExtern.SoundSystemExists();

	Uint32 flags = windowed ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP;
	window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, xRes, yRes, flags);
	if (!window) {
		LogError("SDL_CreateWindow: %s", SDL_GetError());
		FatalError("Failed to create window");
		return;
	}
	LogDebug("window format: %s", SDL_GetPixelFormatName(SDL_GetWindowPixelFormat(window)));
	SDL_GetWindowSize(window, &winWidth, &winHeight);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	if (!renderer) {
		LogError("SDL_CreateRenderer: %s", SDL_GetError());
		LogDebug("Trying software renderer...");
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
		if (!renderer) {
			LogError("SDL_CreateRenderer: %s", SDL_GetError());
			FatalError("Failed to create renderer");
			return;
		}
	}

#if !defined(_WIN32) && !defined(__clang__)
	// Icon embedding for non-Windows platforms.
	// `tools/build/rescomp.py` produces a .cpp file containing these symbols.
	// Marked `__attribute__((weak))` for platforms or projects which do not
	// use this icon embedding method.
	extern __attribute__((weak)) const size_t embed_game_icon_size;
	extern __attribute__((weak)) const unsigned char embed_game_icon[];
	if (&embed_game_icon && &embed_game_icon_size) {
		SDL_Surface *surface = IMG_Load_RW(
			SDL_RWFromConstMem(embed_game_icon, embed_game_icon_size),
			1
		);
		SDL_SetWindowIcon(window, surface);
		SDL_FreeSurface(surface);
	}
#endif

#ifndef NDEBUG
	SDL_RendererInfo info;
	SDL_GetRendererInfo(renderer, &info);
	LogDebug("renderer: %s", info.name);

	char flagbuf[64] = "  flags:";
	if (info.flags & SDL_RENDERER_SOFTWARE)
		strcat(flagbuf, " software");
	if (info.flags & SDL_RENDERER_ACCELERATED)
		strcat(flagbuf, " accelerated");
	if (info.flags & SDL_RENDERER_PRESENTVSYNC)
		strcat(flagbuf, " vsync");
	if (info.flags & SDL_RENDERER_TARGETTEXTURE)
		strcat(flagbuf, " ttex");
	if (!info.flags)
		strcat(flagbuf, " 0");
	LogDebug("%s", flagbuf);
	LogDebug("  texture: (%d, %d)", info.max_texture_width, info.max_texture_height);
	LogDebug("  formats (%d):", info.num_texture_formats);
	for (Uint32 i = 0; i < info.num_texture_formats; ++i) {
		LogDebug("    %s", SDL_GetPixelFormatName(info.texture_formats[i]));
	}
#endif

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, pixelMode ? "0" : "1");
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, xRes, yRes);
	if (!texture) {
		LogError("SDL_CreateTexture: %s", SDL_GetError());
		FatalError("Failed to create texture");
		return;
	}

	SDL_SetWindowTitle(window, name);
	SDL_ShowCursor(SDL_DISABLE);

	scrn = new byte[xRes * yRes];
	buffer = new RGB[xRes * yRes];
	thePal = pal;
	SeedRNG();

#ifdef __ANDROID__
	softJoystick = new SoftJoystick(this);
#else
	softJoystick = nullptr;
#endif
}

MGLDraw::~MGLDraw(void)
{
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	JamulSoundExit();
	delete[] buffer;
	delete[] scrn;
	delete softJoystick;
}

int MGLDraw::GetWidth()
{
	return pitch;
}

int MGLDraw::GetHeight()
{
	return yRes;
}

byte *MGLDraw::GetScreen()
{
	return scrn;
}

void MGLDraw::ClearScreen()
{
	memset(scrn, 0, xRes * yRes);
}

void MGLDraw::GetMouse(int *x,int *y)
{
	*x=mouse_x;
	*y=mouse_y;
}

void MGLDraw::SetMouse(int x,int y)
{
	if (idle)
		return;

	int scale = std::max(1, std::min(winWidth / xRes, winHeight / yRes));
	x = (winWidth - xRes * scale) / 2 + x * scale;
	y = (winHeight - yRes * scale) / 2 + y * scale;
	SDL_WarpMouseInWindow(window, x, y);
}

bool MGLDraw::Process(void)
{
	UpdateMusic();
	return (!readyToQuit);
}

void MGLDraw::Quit()
{
	readyToQuit = true;
}

inline void MGLDraw::putpixel(int x, int y, RGB value)
{
	buffer[y * pitch + x] = value;
}

inline RGB MGLDraw::FormatPixel(int x,int y)
{
	return thePal[scrn[y * xRes + x]];
}

void MGLDraw::PseudoCopy(int y,int x,byte* data,int len)
{
	RGB* target = &buffer[y * pitch + x];
	for(int i = 0; i < len; ++i)
		*target++ = thePal[*data++];
}

inline void MGLDraw::StartFlip(void)
{
}

void MGLDraw::ResizeBuffer(int w, int h)
{
	SDL_DestroyTexture(texture);
	delete[] buffer;
	delete[] scrn;

	xRes = pitch = w;
	yRes = h;

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, xRes, yRes);
	if (!texture) {
		LogError("SDL_CreateTexture: %s", SDL_GetError());
		FatalError("Failed to create texture");
		return;
	}
	scrn = new byte[xRes * yRes];
	buffer = new RGB[xRes * yRes];
}

static void TranslateKey(SDL_Keysym* sym)
{
	if (sym->scancode == SDL_SCANCODE_AC_BACK)
	{
		sym->scancode = SDL_SCANCODE_ESCAPE;
		sym->sym = SDLK_ESCAPE;
		return;
	}
#ifdef __EMSCRIPTEN__
	if (sym->scancode == SDL_SCANCODE_GRAVE)
	{
		sym->scancode = SDL_SCANCODE_ESCAPE;
		sym->sym = SDLK_ESCAPE;
		return;
	}
#endif  // __EMSCRIPTEN__
}

TASK(void) MGLDraw::FinishFlip(void)
{
	float scale = std::max(1.0f, std::min((float)winWidth / xRes, (float)winHeight / yRes));
	SDL_Rect dest = {
		(int)((winWidth - xRes * scale) / 2),
		(int)((winHeight - yRes * scale) / 2),
		(int)(xRes * scale),
		(int)(yRes * scale),
	};

	SDL_UpdateTexture(texture, NULL, buffer, pitch * sizeof(RGB));
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, &dest);
	if (softJoystick) {
		softJoystick->update(this, scale);
		softJoystick->render(renderer);
	}
	SDL_RenderPresent(renderer);
	UpdateMusic();

	SDL_Event e;
	while(SDL_PollEvent(&e)) {
		if (e.type == SDL_KEYDOWN) {
			TranslateKey(&e.key.keysym);
			ControlKeyDown(e.key.keysym.scancode);
			lastRawCode = e.key.keysym.scancode;
			if (!(e.key.keysym.sym & ~0xff))
			{
				lastKeyPressed = e.key.keysym.sym;
			}

			if (e.key.keysym.scancode == SDL_SCANCODE_F11)
			{
#ifndef __EMSCRIPTEN__
				windowed = !windowed;
				if (windowed) {
					SDL_SetWindowFullscreen(window, 0);
				} else {
					SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
				}
#else  // __EMSCRIPTEN__
				EM_ASM(
					Module.requestFullscreen();
				);
#endif  // __EMSCRIPTEN__
			}
			else if (e.key.keysym.scancode == SDL_SCANCODE_F10)
			{
				pixelMode = !pixelMode;
				SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, pixelMode ? "0" : "1");
				SDL_Texture* newTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, xRes, yRes);
				if (newTexture)
				{
					texture = newTexture;
				}
			}
			else if (e.key.keysym.scancode == SDL_SCANCODE_F12)
			{
				time_t timeobj;
				time(&timeobj);
				tm* clock = localtime(&timeobj);

				char fname[128];
				sprintf(fname, "Screenshot %04d-%02d-%02d %02d:%02d:%02d.bmp", 1900 + clock->tm_year, 1 + clock->tm_mon, clock->tm_mday, clock->tm_hour, clock->tm_min, clock->tm_sec);
				SaveBMP(fname);
			}
		} else if (e.type == SDL_TEXTINPUT) {
			if (strlen(e.text.text) == 1)
			{
#ifdef __EMSCRIPTEN__
				if (e.text.text[0] == '`')
				{
					continue;
				}
#endif
				lastKeyPressed = e.text.text[0];
			}
		} else if (e.type == SDL_KEYUP) {
			TranslateKey(&e.key.keysym);
			ControlKeyUp(e.key.keysym.scancode);
		} else if (e.type == SDL_MOUSEMOTION) {
			mouse_x = (e.motion.x - dest.x) / scale;
			mouse_y = (e.motion.y - dest.y) / scale;
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
		} else if (e.type == SDL_MOUSEWHEEL) {
			mouse_z += e.wheel.y;
		} else if (e.type == SDL_QUIT) {
			readyToQuit = 1;
		} else if (e.type == SDL_WINDOWEVENT) {
			if (e.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
				idle = true;
				if (g_HamExtern.SetGameIdle)
					g_HamExtern.SetGameIdle(true);
			} else if (e.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
				if (g_HamExtern.SetGameIdle)
					g_HamExtern.SetGameIdle(false);
				idle = false;
			} else if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
				winWidth = e.window.data1;
				winHeight = e.window.data2;
			}
		}
		if (softJoystick) {
			softJoystick->handle_event(this, e);
		}
	}

	AWAIT coro::next_frame();
}

TASK(void) MGLDraw::Flip(void)
{
	StartFlip();

	// blit to the screen
	int limit = pitch * yRes;
	byte* src = scrn;
	RGB* target = buffer;

	for(int i = 0; i < limit; ++i)
		*target++ = thePal[*src++];

	AWAIT FinishFlip();
}

TASK(void) MGLDraw::WaterFlip(int v)
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
	AWAIT FinishFlip();
}

TASK(void) MGLDraw::TeensyFlip(void)
{
	int i,j,x,y;

	x=640/4;
	y=480/4;
	// blit to the screen
	StartFlip();
	for(i=0;i<yRes/2;i++)
	{
		for(j=0;j<xRes/2;j++)
			putpixel(x+j,y,FormatPixel(j*2,i*2));
		y++;
	}
	AWAIT FinishFlip();
}

TASK(void) MGLDraw::TeensyWaterFlip(int v)
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
		putpixel(x-1-table[v],y,BLACK);
		putpixel(x+xRes/2-table[v],y,BLACK);
		for(j=0;j<xRes/2;j++)
		{
			putpixel(x+j-table[v],y,FormatPixel(j*2,i*2));
		}
		if(i&1)
		{
			v++;
			if(v>23)
				v=0;
		}
		y++;
	}
	AWAIT FinishFlip();
}

TASK(void) MGLDraw::RasterFlip(void)
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
				putpixel(j,i,BLACK);
		}
	}
	AWAIT FinishFlip();
}

TASK(void) MGLDraw::RasterWaterFlip(int v)
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
				putpixel(j,i,BLACK);
		}
		if(i&1)
		{
			v++;
			if(v>23)
				v=0;
		}
	}
	AWAIT FinishFlip();
}

void MGLDraw::SetPalette(PALETTE newpal)
{
	memcpy(pal, newpal, sizeof(PALETTE));
}

const RGB *MGLDraw::GetPalette(void)
{
	return pal;
}

void MGLDraw::RealizePalette(void)
{
	thePal = pal;
}

void MGLDraw::SetSecondaryPalette(PALETTE newpal)
{
	memcpy(pal2, newpal, sizeof(PALETTE));
	thePal = pal2;
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

char MGLDraw::LastKeyPressed(void)
{
	char i = lastKeyPressed;
	lastKeyPressed = 0;
	return i;
}

char MGLDraw::LastKeyPeek(void)
{
	return lastKeyPressed;
}

void MGLDraw::ClearKeys(void)
{
	lastKeyPressed = 0;
}

bool MGLDraw::MouseTap(void)
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

bool MGLDraw::RMouseTap(void)
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

bool MGLDraw::MouseDown(void)
{
	return ((mouse_b&1)!=0);
}

bool MGLDraw::RMouseDown(void)
{
	return ((mouse_b&2)!=0);
}

bool MGLDraw::MouseDown3(void)
{
	return ((mouse_b&4)!=0);
}

bool MGLDraw::LoadBMP(const char *name)
{
	return LoadBMP(name, pal);
}

bool MGLDraw::LoadBMP(const char *name, PALETTE pal)
{
	int i,w;

	SDL_RWops* rw = AssetOpen_SDL(name);
	if (!rw) {
		// Asset stack printed error already
		return false;
	}

#ifdef __EMSCRIPTEN__
	// Under Emscripten, IMG_Load can't load some files which SDL_LoadBMP can.
	SDL_Surface* b = SDL_LoadBMP_RW(rw, SDL_FALSE);
	if (!b)
	{
		b = IMG_Load_RW(rw, SDL_FALSE);
	}
	SDL_RWclose(rw);
#else  // __EMSCRIPTEN__
	SDL_Surface* b = IMG_Load_RW(rw, SDL_TRUE);
#endif  // __EMSCRIPTEN__

	if (!b) {
		LogError("%s: %s", name, SDL_GetError());
		return false;
	}

	if(pal && b->format->palette)
		for(i=0; i<256 && i < b->format->palette->ncolors; i++)
		{
			pal[i].r = b->format->palette->colors[i].r;
			pal[i].g = b->format->palette->colors[i].g;
			pal[i].b = b->format->palette->colors[i].b;
		}
	RealizePalette();

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

bool MGLDraw::SaveBMP(const char *name)
{
#if SDL_VERSION_ATLEAST(2, 0, 5)
	SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, xRes, yRes, 8, SDL_PIXELFORMAT_INDEX8);
#else
	SDL_Surface* surface = SDL_CreateRGBSurface(0, xRes, yRes, 8, 0, 0, 0, 0);
#endif
	SDL_LockSurface(surface);
	memcpy(surface->pixels, scrn, xRes * yRes);
	SDL_UnlockSurface(surface);
	for (int i = 0; i < 256; ++i)
	{
		surface->format->palette->colors[i] = { thePal[i].r, thePal[i].g, thePal[i].b, thePal[i].a };
	}
	SDL_SaveBMP(surface, name);
	SDL_FreeSurface(surface);
	return true;
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

//--------------------------------------------------------------------------
// Fatal error

void FatalError(const char *msg)
{
	fprintf(stderr, "FATAL: %s\n", msg);
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal Error", msg, nullptr);
	if (_globalMGLDraw)
		_globalMGLDraw->Quit();
}

//--------------------------------------------------------------------------
// Global RNG

std::mt19937_64 mersenne;

void SeedRNG(void)
{
	mersenne.seed(timeGetTime());
}

dword Random(dword range)
{
	return std::uniform_int_distribution<dword>(0, range - 1)(mersenne);
}

int MGL_random(int range)
{
	return std::uniform_int_distribution<int>(0, range - 1)(mersenne);
}

long MGL_randoml(long range)
{
	return std::uniform_int_distribution<int>(0, range - 1)(mersenne);
}

void MGL_srand(int seed)
{
	mersenne.seed(seed);
}

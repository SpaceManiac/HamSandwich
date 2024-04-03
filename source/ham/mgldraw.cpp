#include "mgldraw.h"
#include "clock.h"
#include "jamulsound.h"
#include "hammusic.h"
#include "log.h"
#include "softjoystick.h"
#include "appdata.h"
#include "extern.h"
#include "ico.h"
#include "openurl.h"
#include <time.h>
#include <random>
#include <algorithm>

#include <SDL_image.h>
#ifdef _WIN32
	#include <SDL_syswm.h>
#endif  // _WIN32

#ifdef __EMSCRIPTEN__
	#include <emscripten.h>
#endif  // __EMSCRIPTEN__

// in control.cpp
void ControlKeyDown(byte scancode);
void ControlKeyUp(byte scancode);
void ControlHandleNewGamepad(int which);

static const RGB BLACK = {0, 0, 0, 0};

static MGLDraw *_globalMGLDraw = nullptr;

static bool pixelMode = false;

static bool idleGame = false;
bool GetGameIdle()
{
	if (idleGame)
	{
		idleGame = false;
		return true;
	}
	return false;
}

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

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER)) {
		LogError("SDL_Init(VIDEO|JOYSTICK): %s", SDL_GetError());
		FatalError("Failed to initialize SDL");
		return;
	}

#ifdef __ANDROID__
	// On Android, tabbing out of the application may generate an SDL_Quit,
	// which is received once the application is re-entered.
	SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
#endif

	JamulSoundInit(512);

	Uint32 flags = windowed ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP;
	flags |= SDL_WINDOW_HIDDEN;  // Hide the window on creation, and show it a little later, to avoid flicker if SDL_CreateRenderer recreates the window.
	window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, xRes, yRes, flags);
	if (!window) {
		LogError("SDL_CreateWindow: %s", SDL_GetError());
		FatalError("Failed to create window");
		return;
	}
	SDL_SetWindowMinimumSize(window, xRes, yRes);
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
		owned::SDL_Surface surface = ReadIcoFile(owned::SDL_RWFromConstMem(embed_game_icon, embed_game_icon_size), -1);
		SDL_SetWindowIcon(window, surface.get());
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
	SDL_ShowWindow(window);

	scrn = std::make_unique<byte[]>(xRes * yRes);
	buffer = std::make_unique<RGB[]>(xRes * yRes);
	thePal = pal;
	SeedRNG();

#ifdef __ANDROID__
	softJoystick = std::make_unique<SoftJoystick>(this);
#else
	softJoystick = nullptr;
#endif
}

MGLDraw::~MGLDraw(void)
{
	softJoystick.reset();
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	JamulSoundExit();
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
	return scrn.get();
}

void MGLDraw::ClearScreen()
{
	memset(scrn.get(), 0, xRes * yRes);
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

	if (SDL_GetRelativeMouseMode())
	{
		mouse_x = x;
		mouse_y = y;
		return;
	}

	float scale = std::max(1.0f, std::min((float)winWidth / xRes, (float)winHeight / yRes));
	int destX = (int)((winWidth - xRes * scale) / 2);
	int destY = (int)((winHeight - yRes * scale) / 2);
	x = destX + x * scale;
	y = destY + y * scale;
	SDL_WarpMouseInWindow(window, x, y);

	// After warping, update mouse_x and mouse_y immediately so a following
	// call to GetMouse will correctly confirm the completed warp.
	SDL_PumpEvents();
	if (!idle)
	{
		SDL_GetMouseState(&x, &y);
		mouse_x = (x - destX) / scale;
		mouse_y = (y - destY) / scale;
	}
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

bool MGLDraw::IsWindowed()
{
	return windowed;
}

void MGLDraw::SetWindowed(bool newWindowed)
{
	if (windowed == newWindowed)
		return;

	windowed = newWindowed;
#ifndef __EMSCRIPTEN__
	if (windowed)
	{
		SDL_SetWindowFullscreen(window, 0);

		int px, py;
		SDL_GetWindowPosition(window, &px, &py);
		px -= (xRes - winWidth) / 2;
		py -= (yRes - winHeight) / 2;
		px = std::max(0, px);
		py = std::max(0, py);
		SDL_SetWindowPosition(window, px, py);

		SDL_SetWindowSize(window, xRes, yRes);
	}
	else
	{
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	}
#else  // __EMSCRIPTEN__
	if (!windowed)
	{
		EM_ASM(
			Module.requestFullscreen();
		);
	}
#endif  // __EMSCRIPTEN__
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

void MGLDraw::ResizeBuffer(int w, int h, bool clamp)
{
	if (xRes == w && yRes == h)
		return;

	if (clamp)
	{
		// Clamp the requested width/height to be no more than the display size.
		SDL_DisplayMode mode = {};
		SDL_GetWindowDisplayMode(window, &mode);
		if (mode.w > 640 && mode.w < w)
			w = mode.w;
		if (mode.h > 480 && mode.h < h)
			h = mode.h;
	}

	// Resize the 8-bit buffer, truecolor buffer, and GPU texture.
	SDL_DestroyTexture(texture);

	xRes = pitch = w;
	yRes = h;

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, xRes, yRes);
	if (!texture) {
		LogError("SDL_CreateTexture: %s", SDL_GetError());
		FatalError("SDL_CreateTexture failed in ResizeBuffer");
		return;
	}
	scrn = std::make_unique<byte[]>(xRes * yRes);
	buffer = std::make_unique<RGB[]>(xRes * yRes);

	// Resize the window, but only on increases.
	if (windowed && (xRes > winWidth || yRes > winHeight))
	{
		int px, py;
		SDL_GetWindowPosition(window, &px, &py);
		px -= std::max(0, xRes - winWidth) / 2;
		py -= std::max(0, yRes - winHeight) / 2;
		px = std::max(0, px);
		py = std::max(0, py);
		SDL_SetWindowPosition(window, px, py);
	}
	SDL_SetWindowMinimumSize(window, xRes, yRes);

	// Update mouse_x and mouse_y.
	float scale = std::max(1.0f, std::min((float)winWidth / xRes, (float)winHeight / yRes));
	int destX = (int)((winWidth - xRes * scale) / 2);
	int destY = (int)((winHeight - yRes * scale) / 2);
	int mx, my;
	SDL_GetMouseState(&mx, &my);
	mouse_x = (mx - destX) / scale;
	mouse_y = (my - destY) / scale;
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

	SDL_UpdateTexture(texture, NULL, buffer.get(), pitch * sizeof(RGB));
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, &dest);
	bool enableTouchMouse = true;
	if (softJoystick)
	{
		softJoystick->update(this, scale);
		softJoystick->render(renderer);
		enableTouchMouse = softJoystick->enableTouchMouse();
	}
	SDL_RenderPresent(renderer);
	UpdateMusic();
	if (g_HamExtern.AfterFlip)
		g_HamExtern.AfterFlip();

	SDL_Event e;
	while(SDL_PollEvent(&e))
	{
		if (e.type == SDL_KEYDOWN)
		{
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
				SetWindowed(!windowed);
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
				sprintf(fname, "Screenshot %04d-%02d-%02d %02d-%02d-%02d.bmp", 1900 + clock->tm_year, 1 + clock->tm_mon, clock->tm_mday, clock->tm_hour, clock->tm_min, clock->tm_sec);
				SaveBMP(fname);
			}
		}
		else if (e.type == SDL_TEXTINPUT)
		{
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
		}
		else if (e.type == SDL_KEYUP)
		{
			TranslateKey(&e.key.keysym);
			ControlKeyUp(e.key.keysym.scancode);
		}
		else if (e.type == SDL_MOUSEMOTION)
		{
			if (enableTouchMouse || e.motion.which != SDL_TOUCH_MOUSEID)
			{
				if (SDL_GetRelativeMouseMode())
				{
					mouse_x += e.motion.xrel;
					mouse_y += e.motion.yrel;
				}
				else
				{
					mouse_x = (e.motion.x - dest.x) / scale;
					mouse_y = (e.motion.y - dest.y) / scale;
				}
			}
		}
		else if (e.type == SDL_MOUSEBUTTONUP)
		{
			// Always accept mouseups.
			int flag = 0;
			if (e.button.button == SDL_BUTTON_LEFT)
				flag = 1;
			else if (e.button.button == SDL_BUTTON_RIGHT)
				flag = 2;
			mouse_b &= ~flag;
		}
		else if (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP)
		{
			// Only accept mousedowns that are inside the real scene.
			// Also ignore mousedowns if soft joystick is active.
			SDL_Point p = {e.button.x, e.button.y};
			if (SDL_PointInRect(&p, &dest) && (e.motion.which != SDL_TOUCH_MOUSEID || enableTouchMouse))
			{
				int flag = 0;
				if (e.button.button == SDL_BUTTON_LEFT)
					flag = 1;
				else if (e.button.button == SDL_BUTTON_RIGHT)
					flag = 2;
				mouse_b |= flag;
			}
		}
		else if (e.type == SDL_MOUSEWHEEL)
		{
			mouse_z += e.wheel.y;
		}
		else if (e.type == SDL_QUIT)
		{
			readyToQuit = 1;
		}
		else if (e.type == SDL_CONTROLLERDEVICEADDED)
		{
			ControlHandleNewGamepad(e.cdevice.which);
		}
		else if (e.type == SDL_CONTROLLERDEVICEREMOVED)
		{
			idle = true;
			idleGame = true;
		}
		else if (e.type == SDL_WINDOWEVENT)
		{
			if (e.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
			{
				idle = true;
				idleGame = true;
			}
			else if (e.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
			{
				idleGame = false;
				idle = false;
			}
			else if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
			{
				winWidth = e.window.data1;
				winHeight = e.window.data2;
			}
		}

		if (softJoystick)
		{
			softJoystick->handle_event(this, e);
		}
	}

	AWAIT coro::next_frame();
}

void MGLDraw::BufferFlip()
{
	// blit to the screen
	int limit = pitch * yRes;
	byte* src = scrn.get();
	RGB* target = buffer.get();

	for(int i = 0; i < limit; ++i)
		*target++ = thePal[*src++];
}

TASK(void) MGLDraw::Flip(void)
{
	StartFlip();
	BufferFlip();
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
	byte mb = mouse_b & 1;
	bool result = mb && !(tapTrack & 1);
	tapTrack &= ~1;
	tapTrack |= mb;
	return result;
}

bool MGLDraw::RMouseTap(void)
{
	byte mb = mouse_b & 2;
	bool result = mb && !(tapTrack & 2);
	tapTrack &= ~2;
	tapTrack |= mb;
	return result;
}

bool MGLDraw::MouseTap3()
{
	byte mb = mouse_b & 4;
	bool result = mb && !(tapTrack & 4);
	tapTrack &= ~4;
	tapTrack |= mb;
	return result;
}

bool MGLDraw::MouseDown(void)
{
	return mouse_b & 1;
}

bool MGLDraw::RMouseDown(void)
{
	return mouse_b & 2;
}

bool MGLDraw::MouseDown3(void)
{
	return mouse_b & 4;
}

bool MGLDraw::LoadBMP(const char *name)
{
	if (!LoadBMP(name, pal))
		return false;
	RealizePalette();
	return true;
}

bool MGLDraw::LoadBMP(const char *name, PALETTE pal)
{
	owned::SDL_RWops rw = AssetOpen_SDL_Owned(name);
	if (!rw) {
		// Asset stack printed error already
		return false;
	}

#ifdef __EMSCRIPTEN__
	// Under Emscripten, IMG_Load can't load some files which SDL_LoadBMP can.
	SDL_Surface* b = SDL_LoadBMP_RW(rw.get(), SDL_FALSE);
	if (!b)
	{
		b = IMG_Load_RW(rw.get(), SDL_FALSE);
	}
	rw.reset();
#else  // __EMSCRIPTEN__
	SDL_Surface* b = IMG_Load_RW(rw.release(), SDL_TRUE);
#endif  // __EMSCRIPTEN__

	if (!b) {
		LogError("%s: %s", name, SDL_GetError());
		return false;
	}

	if(pal && b->format->palette)
		for(int i = 0; i < 256 && i < b->format->palette->ncolors; i++)
		{
			pal[i].r = b->format->palette->colors[i].r;
			pal[i].g = b->format->palette->colors[i].g;
			pal[i].b = b->format->palette->colors[i].b;
		}

	if (b->format->BitsPerPixel != 8)
	{
		// Trying to load a truecolor image or something. Convert it to the palette.
		SDL_PixelFormat* format = SDL_AllocFormat(SDL_PIXELFORMAT_INDEX8);
		format->palette = SDL_AllocPalette(256);
		for (int i = 0; i < 256; ++i)
		{
			format->palette->colors[i].r = thePal[i].r;
			format->palette->colors[i].g = thePal[i].g;
			format->palette->colors[i].b = thePal[i].b;
			format->palette->colors[i].a = 255;
		}

		SDL_Surface* b2 = SDL_ConvertSurface(b, format, 0);
		SDL_FreeSurface(b);
		b = b2;

		SDL_FreeFormat(format);
	}

	// 8-bit image suitable for loading directly into `srcn`.
	// Either we saved the palette above or threw it away.
	int w = std::min(b->w, xRes);
	int h = std::min(b->h, yRes);

	SDL_LockSurface(b);
	for(int i = 0; i < h; i++)
	{
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
	for (int y = 0; y < yRes; ++y)
		memcpy(&((uint8_t*)surface->pixels)[y * surface->pitch], &scrn[y * pitch], xRes);
	SDL_UnlockSurface(surface);
	for (int i = 0; i < 256; ++i)
	{
		surface->format->palette->colors[i] = { thePal[i].r, thePal[i].g, thePal[i].b, thePal[i].a };
	}
	SDL_SaveBMP_RW(surface, AppdataOpen_Write_SDL(name).release(), SDL_TRUE);
	SDL_FreeSurface(surface);
	return true;
}

bool MGLDraw::SavePNG(const char* name)
{
#if SDL_VERSION_ATLEAST(2, 0, 5)
	SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, xRes, yRes, 8, SDL_PIXELFORMAT_INDEX8);
#else
	SDL_Surface* surface = SDL_CreateRGBSurface(0, xRes, yRes, 8, 0, 0, 0, 0);
#endif
	SDL_LockSurface(surface);
	for (int y = 0; y < yRes; ++y)
		memcpy(&((uint8_t*)surface->pixels)[y * surface->pitch], &scrn[y * pitch], xRes);
	SDL_UnlockSurface(surface);
	for (int i = 0; i < 256; ++i)
	{
		surface->format->palette->colors[i] = { thePal[i].r, thePal[i].g, thePal[i].b, thePal[i].a };
	}
	// NB: Unlike SaveBMP, this expects an absolute path. Maybe slightly surprising.
	bool ok = IMG_SavePNG_RW(surface, SDL_RWFromFile(name, "wb"), SDL_TRUE) == 0;
	SDL_FreeSurface(surface);
	return ok;
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

void MGLDraw::StartTextInput(int x, int y, int x2, int y2)
{
	float scale = std::max(1.0f, std::min((float)winWidth / xRes, (float)winHeight / yRes));
	int destX = (int)((winWidth - xRes * scale) / 2);
	int destY = (int)((winHeight - yRes * scale) / 2);

	SDL_Rect rect = { x, y, x2-x, y2-y };
	rect.x = destX + rect.x * scale;
	rect.y = destY + rect.y * scale;
	rect.w *= scale;
	rect.h *= scale;

	SDL_SetTextInputRect(&rect);

	if (SDL_GetHintBoolean("SteamDeck", SDL_FALSE))
	{
		SDL_EventState(SDL_TEXTINPUT, SDL_ENABLE);
		SDL_EventState(SDL_TEXTEDITING, SDL_ENABLE);

		// The original addition of Steam Deck keyboard support doesn't set the
		// rect https://github.com/libsdl-org/SDL/pull/6515 so do it ourselves.
		// https://partner.steamgames.com/doc/api/ISteamUtils#ShowFloatingGamepadTextInput
		// > The text field position is specified in pixels relative the origin
		// > of the game window and is used to position the floating keyboard
		// > in a way that doesn't cover the text field.
		char deeplink[128];
		SDL_snprintf(deeplink, sizeof(deeplink),
			"steam://open/keyboard?XPosition=%d&YPosition=%d&Width=%d&Height=%d&Mode=%d",
			rect.x,
			rect.y,
			rect.w,
			rect.h,
			SDL_GetHintBoolean(SDL_HINT_RETURN_KEY_HIDES_IME, SDL_FALSE) ? 0 : 1);
		SDL_OpenURL(deeplink);
	}
	else
	{
		SDL_StartTextInput();
	}
}

void MGLDraw::StopTextInput()
{
	SDL_Rect rect = { 0, 0, 0, 0 };
	SDL_SetTextInputRect(&rect);
	SDL_StopTextInput();
}

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

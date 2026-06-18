#include "mgldraw.h"
#include <time.h>
#include <random>
#include <algorithm>
#include "clock.h"
#include "jamulsound.h"
#include "log.h"
#include "softjoystick.h"
#include "appdata.h"
#include "extern.h"
#include "ico.h"
#include "string_extras.h"

#include <SDL3_image/SDL_image.h>

#ifdef __EMSCRIPTEN__
	#include <emscripten.h>
#endif  // __EMSCRIPTEN__

// in control.cpp
void ControlHandleEvent(const SDL_Event &event);

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
	, prevTexture(nullptr)
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

	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMEPAD)) {
		LogError("SDL_Init: %s", SDL_GetError());
		FatalError("Failed to initialize SDL");
		return;
	}

#ifdef __ANDROID__
	// On Android, tabbing out of the application may generate an SDL_Quit,
	// which is received once the application is re-entered.
	SDL_FlushEvents(SDL_EVENT_FIRST, SDL_EVENT_LAST);
#endif

	JamulSoundInit(512);

	Uint32 flags = windowed ? 0 : SDL_WINDOW_FULLSCREEN;
	flags |= SDL_WINDOW_HIDDEN;  // Hide the window on creation, and show it a little later, to avoid flicker if SDL_CreateRenderer recreates the window.
	window = SDL_CreateWindow(name, xRes, yRes, flags);
	if (!window) {
		LogError("SDL_CreateWindow: %s", SDL_GetError());
		FatalError("Failed to create window");
		return;
	}
	SDL_SetWindowMinimumSize(window, xRes, yRes);
	LogDebug("window format: %s", SDL_GetPixelFormatName(SDL_GetWindowPixelFormat(window)));
	SDL_GetWindowSize(window, &winWidth, &winHeight);

	renderer = SDL_CreateRenderer(window, nullptr);
	if (!renderer) {
		LogError("SDL_CreateRenderer: %s", SDL_GetError());
		FatalError("Failed to create renderer");
		return;
	}
	if (!SDL_SetRenderVSync(renderer, 1)) {
		LogError("SDL_SetRenderVSync: %s", SDL_GetError());
	}

#if !defined(_WIN32)
	// Icon embedding for non-Windows platforms.
	// `tools/build/rescomp.py` produces a .cpp file containing these symbols.
	// Marked `__attribute__((weak))` for platforms or projects which do not
	// use this icon embedding method.
	extern __attribute__((weak)) const size_t embed_game_icon_size;
	extern __attribute__((weak)) const unsigned char embed_game_icon[];
	if (&embed_game_icon && &embed_game_icon_size) {
		owned::SDL_Surface surface = ReadIcoFile(owned::SDL_IOFromConstMem(embed_game_icon, embed_game_icon_size), -1);
		SDL_SetWindowIcon(window, surface.get());
	}
#endif

#ifndef NDEBUG
	LogDebug("renderer: %s", SDL_GetRendererName(renderer));
	SDL_PropertiesID props = SDL_GetRendererProperties(renderer);
	LogDebug("  texture: %" SDL_PRIs64, SDL_GetNumberProperty(props, SDL_PROP_RENDERER_MAX_TEXTURE_SIZE_NUMBER, 0));
	const SDL_PixelFormat* formats = (const SDL_PixelFormat*)SDL_GetPointerProperty(props, SDL_PROP_RENDERER_TEXTURE_FORMATS_POINTER, nullptr);
	LogDebug("  formats:");
	while (formats && *formats != SDL_PIXELFORMAT_UNKNOWN) {
		LogDebug("    %s", SDL_GetPixelFormatName(*formats));
		++formats;
	}
#endif

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, xRes, yRes);
	if (!texture) {
		LogError("SDL_CreateTexture: %s", SDL_GetError());
		FatalError("Failed to create texture");
		return;
	}
	SDL_SetTextureScaleMode(texture, pixelMode ? SDL_SCALEMODE_PIXELART : SDL_SCALEMODE_LINEAR);

	SDL_SetWindowTitle(window, name);
	SDL_HideCursor();
	SDL_ShowWindow(window);
	SDL_SetWindowResizable(window, true);  // Set after showing so i3 treats it as non-resizable to start.

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

	if (SDL_GetWindowRelativeMouseMode(window))
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
		float fx, fy;
		SDL_GetMouseState(&fx, &fy);
		mouse_x = (fx - destX) / scale;
		mouse_y = (fy - destY) / scale;
	}
}

void MGLDraw::SetMouseRelative(bool relative)
{
	SDL_SetWindowRelativeMouseMode(window, relative);
}

bool MGLDraw::Process(void)
{
	UpdateMusic();

	// Peep the queue to get quit events immediately without having to drain it.
	SDL_PumpEvents();
	if (SDL_PeepEvents(nullptr, 1, SDL_PEEKEVENT, SDL_EVENT_QUIT, SDL_EVENT_QUIT))
		readyToQuit = true;

	return !readyToQuit;
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
		int px, py;
		SDL_GetWindowPosition(window, &px, &py);

		SDL_SetWindowFullscreen(window, false);
		SDL_SetWindowResizable(window, true);  // Set now in case we started fullscreen.

		px -= (xRes - winWidth) / 2;
		py -= (yRes - winHeight) / 2;
		px = std::max(0, px);
		py = std::max(0, py);
		SDL_SetWindowPosition(window, px, py);

		SDL_SetWindowSize(window, xRes, yRes);
	}
	else
	{
		SDL_SetWindowFullscreen(window, true);
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
	if (w < 0 && h < 0)
	{
		// (-1, -1) means use window size.
		w = winWidth;
		h = winHeight;
	}
	else if (w < 0)
	{
		// (-1, 480) means fixed height, full-window width.
		w = h * winWidth / winHeight;
	}
	else if (h < 0)
	{
		// (640, -1) means fixed width, full-window height.
		h = w * winHeight / winWidth;
	}

	if (xRes == w && yRes == h)
		return;

	if (clamp)
	{
		// Clamp the requested width/height to be no more than the display size.
		const SDL_DisplayMode *mode = SDL_GetWindowFullscreenMode(window);
		if (mode && mode->w > 640 && mode->w < w)
			w = mode->w;
		if (mode && mode->h > 480 && mode->h < h)
			h = mode->h;
	}

	// Resize the 8-bit buffer, truecolor buffer, and GPU texture.
	// Save the texture and delete it at the start of the next frame to avoid
	// having to flush the GPU command queue here just to delete it.
	prevTexture = texture;
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
	float mx, my;
	SDL_GetMouseState(&mx, &my);
	mouse_x = (mx - destX) / scale;
	mouse_y = (my - destY) / scale;
}

static void TranslateKey(SDL_KeyboardEvent* sym)
{
	if (sym->scancode == SDL_SCANCODE_AC_BACK)
	{
		sym->scancode = SDL_SCANCODE_ESCAPE;
		sym->key = SDLK_ESCAPE;
		return;
	}
#ifdef __EMSCRIPTEN__
	if (sym->scancode == SDL_SCANCODE_GRAVE)
	{
		sym->scancode = SDL_SCANCODE_ESCAPE;
		sym->key = SDLK_ESCAPE;
		return;
	}
#endif  // __EMSCRIPTEN__
}

TASK(void) MGLDraw::FinishFlip(void)
{
	float scale = std::max(1.0f, std::min((float)winWidth / xRes, (float)winHeight / yRes));
	SDL_FRect dest = {
		(float)(int)((winWidth - xRes * scale) / 2),
		(float)(int)((winHeight - yRes * scale) / 2),
		(float)(int)(xRes * scale),
		(float)(int)(yRes * scale),
	};

	SDL_UpdateTexture(texture, NULL, buffer.get(), pitch * sizeof(RGB));
	SDL_RenderClear(renderer);
	SDL_RenderTexture(renderer, texture, NULL, &dest);
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
		if (e.type == SDL_EVENT_KEY_DOWN || e.type == SDL_EVENT_KEY_UP)
		{
			TranslateKey(&e.key);
		}

		ControlHandleEvent(e);

		if (e.type == SDL_EVENT_KEY_DOWN)
		{
			lastRawCode = e.key.scancode;
			if (!(e.key.key & ~0xff))
			{
				lastKeyPressed = SDL_GetKeyFromScancode(e.key.scancode, e.key.mod, false);
			}

			if (e.key.scancode == SDL_SCANCODE_F11)
			{
#ifndef __EMSCRIPTEN__
				if (windowed)
				{
					// decrease scale by 1
					int scale = std::min((winWidth - 1) / xRes, (winHeight - 1) / yRes);
					if (scale < 1)
					{
						// Already at minimum size, just go fullscreen
						SetWindowed(false);
					}
					else
					{
						// bump down window size
						int newWidth = xRes * scale, newHeight = yRes * scale;
						int px, py;
						SDL_GetWindowPosition(window, &px, &py);
						px -= (newWidth - winWidth) / 2;
						py -= (newHeight - winHeight) / 2;
						px = std::max(0, px);
						py = std::max(0, py);
						SDL_SetWindowPosition(window, px, py);

						SDL_SetWindowSize(window, newWidth, newHeight);
					}
				}
				else
				{
					// go to biggest window size that will fit on desktop
					const SDL_DisplayMode *mode;
					int index = SDL_GetDisplayForWindow(window);
					if (index < 0)
					{
						LogError("SDL_GetDisplayForWindow: %s", SDL_GetError());
						SetWindowed(true);
					}
					else if (mode = SDL_GetDesktopDisplayMode(index); !mode)
					{
						LogError("SDL_GetDesktopDisplayMode: %s", SDL_GetError());
						SetWindowed(true);
					}
					else
					{
						int scale = std::min((mode->w - 1) / xRes, (mode->h - 1) / yRes);
						int newWidth = xRes * scale, newHeight = yRes * scale;
						int px, py;
						SDL_GetWindowPosition(window, &px, &py);

						SDL_SetWindowFullscreen(window, 0);
						SDL_SetWindowResizable(window, true);  // Set now in case we started fullscreen.
						windowed = true;

						px -= (newWidth - winWidth) / 2;
						py -= (newHeight - winHeight) / 2;
						px = std::max(0, px);
						py = std::max(0, py);
						SDL_SetWindowPosition(window, px, py);

						SDL_SetWindowSize(window, newWidth, newHeight);
					}
				}
#else  // __EMSCRIPTEN__
				EM_ASM(
					Module.requestFullscreen();
				);
#endif  // __EMSCRIPTEN__
			}
			else if (e.key.scancode == SDL_SCANCODE_F10)
			{
				pixelMode = !pixelMode;
				SDL_SetTextureScaleMode(texture, pixelMode ? SDL_SCALEMODE_PIXELART : SDL_SCALEMODE_LINEAR);
			}
			else if (e.key.scancode == SDL_SCANCODE_F12)
			{
				time_t timeobj;
				time(&timeobj);
				tm* clock = localtime(&timeobj);

				char fname[128];
				ham_sprintf(fname, "Screenshot %04d-%02d-%02d %02d-%02d-%02d.bmp", 1900 + clock->tm_year, 1 + clock->tm_mon, clock->tm_mday, clock->tm_hour, clock->tm_min, clock->tm_sec);
				SaveBMP(fname);
			}
		}
		else if (e.type == SDL_EVENT_TEXT_INPUT)
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
		else if (e.type == SDL_EVENT_MOUSE_MOTION)
		{
			if (enableTouchMouse || e.motion.which != SDL_TOUCH_MOUSEID)
			{
				if (SDL_GetWindowRelativeMouseMode(window))
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
		else if (e.type == SDL_EVENT_MOUSE_BUTTON_UP)
		{
			// Always accept mouseups.
			int flag = 0;
			if (e.button.button == SDL_BUTTON_LEFT)
				flag = 1;
			else if (e.button.button == SDL_BUTTON_RIGHT)
				flag = 2;
			mouse_b &= ~flag;
		}
		else if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN || e.type == SDL_EVENT_MOUSE_BUTTON_UP)
		{
			// Only accept mousedowns that are inside the real scene.
			// Also ignore mousedowns if soft joystick is active.
			SDL_FPoint p = {e.button.x, e.button.y};
			if (SDL_PointInRectFloat(&p, &dest) && (e.motion.which != SDL_TOUCH_MOUSEID || enableTouchMouse))
			{
				int flag = 0;
				if (e.button.button == SDL_BUTTON_LEFT)
					flag = 1;
				else if (e.button.button == SDL_BUTTON_RIGHT)
					flag = 2;
				mouse_b |= flag;
			}
		}
		else if (e.type == SDL_EVENT_MOUSE_WHEEL)
		{
			mouse_z += e.wheel.y;
		}
		else if (e.type == SDL_EVENT_QUIT)
		{
			readyToQuit = 1;
		}
		else if (e.type == SDL_EVENT_GAMEPAD_REMOVED)
		{
			idle = true;
			idleGame = true;
		}
		else if (e.type == SDL_EVENT_WINDOW_FOCUS_LOST)
		{
			idle = true;
			idleGame = true;
		}
		else if (e.type == SDL_EVENT_WINDOW_FOCUS_GAINED)
		{
			idleGame = false;
			idle = false;
		}
		else if (e.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED)
		{
			winWidth = e.window.data1;
			winHeight = e.window.data2;
		}
		else if (e.type == SDL_EVENT_WINDOW_EXPOSED)
		{
			// On some platforms (Linux i3) we don't get SIZE_CHANGED when
			// we go fullscreen, but we do get an EXPOSED, and this works.
			SDL_GetWindowSize(window, &winWidth, &winHeight);
			SDL_Rect rect = { 0, 0, winWidth, winHeight };
			SDL_SetRenderViewport(renderer, &rect);
		}

		if (softJoystick)
		{
			softJoystick->handle_event(this, e);
		}
	}

	if (prevTexture)
	{
		SDL_DestroyTexture(prevTexture);
		prevTexture = nullptr;
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

bool MGLDraw::LoadBMPResize(const char *name)
{
	if (!LoadBMP(name, pal, true))
		return false;
	RealizePalette();
	return true;
}

bool MGLDraw::LoadBMP(const char *name, PALETTE pal, bool resize)
{
	owned::SDL_IOStream rw = AppdataOpen(name);
	if (!rw) {
		// Asset stack printed error already
		return false;
	}

#ifdef __EMSCRIPTEN__
	// Under Emscripten, IMG_Load can't load some files which SDL_LoadBMP can.
	owned::SDL_Surface b { SDL_LoadBMP_IO(rw.get(), false) };
	if (!b)
	{
		b.reset(IMG_Load_IO(rw.get(), false));
	}
	rw.reset();
#else  // __EMSCRIPTEN__
	owned::SDL_Surface b { IMG_Load_IO(rw.release(), true) };
#endif  // __EMSCRIPTEN__

	if (!b) {
		LogError("%s: %s", name, SDL_GetError());
		return false;
	}

	SDL_Palette *palette = SDL_GetSurfacePalette(b.get());
	if(pal && palette)
	{
		for(int i = 0; i < 256 && i < palette->ncolors; i++)
		{
			pal[i].r = palette->colors[i].r;
			pal[i].g = palette->colors[i].g;
			pal[i].b = palette->colors[i].b;
			pal[i].a = palette->colors[i].a;
		}
	}

	if (SDL_GetPixelFormatDetails(b->format)->bits_per_pixel != 8)
	{
		// Trying to load a truecolor image or something. Convert it to the palette.
		owned::SDL_Palette newPalette = owned::SDL_CreatePalette(256);
		for (int i = 0; i < 256; ++i)
		{
			newPalette->colors[i].r = thePal[i].r;
			newPalette->colors[i].g = thePal[i].g;
			newPalette->colors[i].b = thePal[i].b;
			newPalette->colors[i].a = 255;
		}
		b.reset(SDL_ConvertSurfaceAndColorspace(b.get(), SDL_PIXELFORMAT_INDEX8, newPalette.get(), SDL_COLORSPACE_UNKNOWN, 0));
	}

	if (resize)
	{
		ResizeBuffer(b->w, b->h);
	}

	// 8-bit image suitable for loading directly into `srcn`.
	// Either we saved the palette above or threw it away.
	int w = std::min(b->w, xRes);
	int h = std::min(b->h, yRes);

	SDL_LockSurface(b.get());
	for(int i = 0; i < h; i++)
	{
		memcpy(&scrn[i*pitch], &((byte*) b->pixels)[b->pitch * i], w);
	}
	SDL_UnlockSurface(b.get());

	return true;
}

bool MGLDraw::SaveBMP(const char *name)
{
	SDL_Surface* surface = SDL_CreateSurface(xRes, yRes, SDL_PIXELFORMAT_INDEX8);
	SDL_LockSurface(surface);
	uint8_t* pixels = (uint8_t*)surface->pixels;
	for (int y = 0; y < yRes; ++y)
		memcpy(&pixels[y * surface->pitch], &scrn[y * pitch], xRes);
	SDL_UnlockSurface(surface);
	SDL_Palette* palette = SDL_CreateSurfacePalette(surface);
	for (int i = 0; i < 256; ++i)
	{
		palette->colors[i] = { thePal[i].r, thePal[i].g, thePal[i].b, thePal[i].a };
	}
	SDL_SaveBMP_IO(surface, AppdataOpen_Write(name).release(), true);
	AppdataSync();
	SDL_DestroySurface(surface);
	return true;
}

bool MGLDraw::SavePNG(const char* name)
{
	SDL_Surface* surface = SDL_CreateSurface(xRes, yRes, SDL_PIXELFORMAT_INDEX8);
	SDL_LockSurface(surface);
	uint8_t* pixels = (uint8_t*)surface->pixels;
	for (int y = 0; y < yRes; ++y)
		memcpy(&pixels[y * surface->pitch], &scrn[y * pitch], xRes);
	SDL_UnlockSurface(surface);
	SDL_Palette* palette = SDL_CreateSurfacePalette(surface);
	for (int i = 0; i < 256; ++i)
	{
		palette->colors[i] = { thePal[i].r, thePal[i].g, thePal[i].b, thePal[i].a };
	}
	// NB: Unlike SaveBMP, this expects an absolute path. Maybe slightly surprising.
	bool ok = IMG_SavePNG_IO(surface, SDL_IOFromFile(name, "wb"), true);
	SDL_DestroySurface(surface);
	return ok;
}

#ifdef _WIN32
HWND MGLDraw::GetHWnd(void)
{
	return (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
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

	SDL_SetTextInputArea(window, &rect, 0);

	if (SDL_GetHintBoolean("SteamDeck", false))
	{
		SDL_SetEventEnabled(SDL_EVENT_TEXT_INPUT, true);
		SDL_SetEventEnabled(SDL_EVENT_TEXT_EDITING, true);

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
			SDL_GetHintBoolean(SDL_HINT_RETURN_KEY_HIDES_IME, false) ? 0 : 1);
		SDL_OpenURL(deeplink);
	}
	else
	{
		SDL_StartTextInput(window);
	}
}

void MGLDraw::StopTextInput()
{
	SDL_Rect rect = { 0, 0, 0, 0 };
	SDL_SetTextInputArea(window, &rect, 0);
	SDL_StopTextInput(window);
}

//--------------------------------------------------------------------------
// Fatal error

void FatalError(const char *msg)
{
	LogError("FATAL: %s", msg);
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
	if (range < 1) range = 1;
	return std::uniform_int_distribution<dword>(0, range - 1)(mersenne);
}

int MGL_random(int range)
{
	if (range < 1) range = 1;
	return std::uniform_int_distribution<int>(0, range - 1)(mersenne);
}

long MGL_randoml(long range)
{
	if (range < 1) range = 1;
	return std::uniform_int_distribution<int>(0, range - 1)(mersenne);
}

void MGL_srand(int seed)
{
	mersenne.seed(seed);
}

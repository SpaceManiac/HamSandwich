/* MGLDraw

   A moderately cute little wrapper around MGL.

   -- No longer actually an MGL wrapper! --
   -- Currently using PixelToaster --

   For quick reference, to use MGL in a program, you must:

   link in gm.lib and mglfx.lib as the first two things in the list of things to link in.

*/

#include "mgldraw.h"
#include "winpch.h"
#include "game.h"	// even sorrier about this crap (it's for MGLDraw_Activate only)
#include "sound.h"
#include "music.h"
#include "ctype.h"

// Allegro shenanigans
static char prevKey[KEY_MAX];
static bool closeButtonPressed;
void closeButtonCallback() { closeButtonPressed = true; }

// Replacements for missing MGL functions
int MGL_random(int max) {
    return rand() % max;
}
long MGL_randoml(long max) {
    return rand() % max;
}
void MGL_fatalError(const char* txt) {
    set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
    allegro_message(txt);
    exit(0);
}

MGLDraw::MGLDraw(const char *name,int xRes,int yRes,int bpp,bool window,HINSTANCE hInst)
{
    allegro_init();
    install_keyboard();
    install_mouse();
    set_color_depth(32);

    if (set_gfx_mode(window ? GFX_AUTODETECT_WINDOWED : GFX_AUTODETECT_FULLSCREEN, xRes, yRes, 0, 0) != 0) {
        char buf[256];
        sprintf(buf, "Unable to set graphics mode: %s", allegro_error);
        MGL_fatalError(buf);
    }
    set_window_title(name);
    set_close_button_callback(&closeButtonCallback);

	// this used to have to be in a very specific place but now it doesn't, hooray!
	if(JamulSoundInit(hInst,name,512))
		SoundSystemExists();

	readyToQuit=false;

	// gimme windows colors
	this->xRes=xRes;
	this->yRes=yRes;
	this->bpp=bpp;
	this->pitch=xRes;
	this->scrn=new byte[xRes * yRes];
    buffer = create_bitmap(xRes, yRes);

	mouseDown=0;
}

MGLDraw::~MGLDraw(void)
{
	JamulSoundExit();
    destroy_bitmap(buffer);
    delete[] scrn;
}

void MGLDraw::FatalError(char *msg)
{
	MGL_fatalError(msg);
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
    blit(buffer, screen, 0, 0, 0, 0, xRes, yRes);

    while (keypressed()) {
        int k = readkey();
        SetLastKey((char) (k & 0xff));
    }

    for (int i = 0; i < KEY_MAX; ++i) {
        if (key[i] && !prevKey[i]) {
            ControlKeyDown(i);
        } else if (!key[i] && prevKey[i]) {
            ControlKeyUp(i);
        }
        prevKey[i] = key[i];
    }

    SetMouse(mouse_x, mouse_y);
    SetMouseDown(mouse_b & 3);

    if (closeButtonPressed) {
        readyToQuit = true;
    }

	return (!readyToQuit);
}

HWND MGLDraw::GetHWnd(void)
{
    return win_get_window();
}

void MGLDraw::Flip(void)
{
	if(GetGameIdle())
		GameIdle();

    // This is nice and fast, thankfully
    for (int i = 0; i < xRes * yRes; ++i) {
        palette_t c = pal[scrn[i]];
        putpixel(buffer, i%xRes, i/xRes, makecol(c.red, c.green, c.blue));
    }
    Process();
}

void MGLDraw::ClearScreen(void)
{
    memset(scrn, 0, xRes * yRes);
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
		pal[i].red=p[i].r;
		pal[i].green=p[i].g;
		pal[i].blue=p[i].b;
		pal[i].alpha=0;
	}

	RealizePalette();

	fclose(f);
	return true;
}

void MGLDraw::SetPalette(palette_t *pal2)
{
	memcpy(pal,pal2,sizeof(palette_t)*256);
}

void MGLDraw::RealizePalette(void)
{
	//GM_setPalette(pal,256,0);
	//GM_realizePalette(256,0,true);
}

// Doesn't appear to be used.
void MGLDraw::DarkPalette(void)
{
    /*
	palette_t darkpal[256];
	int i;

	for(i=0;i<256;i++)
	{
		darkpal[i].blue=(byte)(pal[i].blue>>1);
		darkpal[i].red=(byte)(pal[i].red>>1);
		darkpal[i].green=(byte)(pal[i].green>>1);
		darkpal[i].alpha=pal[i].alpha;
	}
	darkpal[175].blue=pal[175].blue;
	darkpal[175].red=pal[175].red;
	darkpal[175].green=pal[175].green;
	darkpal[255].blue=pal[255].blue;
	darkpal[255].red=pal[255].red;
	darkpal[255].green=pal[255].green;
	GM_setPalette(darkpal,256,0);
	GM_realizePalette(256,0,true);
    */
}

// 8-bit graphics only
void MGLDraw::Box(int x,int y,int x2,int y2,byte c)
{
	int i;

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
	memset(&scrn[x+y*pitch],c,x2-x+1);
	memset(&scrn[x+y2*pitch],c,x2-x+1);
	for(i=y;i<=y2;i++)
	{
		scrn[x+i*pitch]=c;
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
	char c=lastKeyPressed;
	lastKeyPressed=0;
	return c;
}

void MGLDraw::SetMouseDown(byte w)
{
	mouseDown=w;
}

byte MGLDraw::MouseDown(void)
{
	return mouseDown;
}

void MGLDraw::SetMouse(int x, int y)
{
    mousex = x;
    mousey = y;
}

void MGLDraw::TeleportMouse(int x, int y)
{
    POINT pt = { x, y };
    ClientToScreen(GetHWnd(), &pt);
    SetCursorPos(pt.x, pt.y);
    SetMouse(x, y);
}

void MGLDraw::GetMouse(int *x, int *y)
{
    *x = mousex;
    *y = mousey;
}

char MGLDraw::LastKeyPeek(void)
{
	return lastKeyPressed;
}

bool MGLDraw::LoadBMP(const char *name)
{
	FILE *f;
	BITMAPFILEHEADER bmpFHead;
	BITMAPINFOHEADER bmpIHead;
	RGBQUAD	pal2[256];

	int i;
	byte *scr;

	f=fopen(name,"rb");
	if(!f)
		return FALSE;

	fread(&bmpFHead,sizeof(BITMAPFILEHEADER),1,f);
	fread(&bmpIHead,sizeof(BITMAPINFOHEADER),1,f);

	// 8-bit BMPs only
	if(bmpIHead.biBitCount!=8)
		return FALSE;

    // Non-RLE BMPs only
    if (bmpIHead.biCompression != 0) {
        printf("bitmap %s is compressed (%lu)\n", name, bmpIHead.biCompression);
        return FALSE;
    }

	fread(pal2,sizeof(pal2),1,f);
	for(i=0;i<256;i++)
	{
		pal[i].red=pal2[i].rgbRed;
		pal[i].green=pal2[i].rgbGreen;
		pal[i].blue=pal2[i].rgbBlue;
	}

	this->RealizePalette();

	for(i=0;i<bmpIHead.biHeight;i++)
	{
		scr = scrn + (bmpIHead.biHeight-1-i) * pitch;
		fread(scr,1,bmpIHead.biWidth,f);
	}
	fclose(f);
	return TRUE;
}

void MGLDraw::GammaCorrect(byte gamma)
{
	int i;
	int r,g,b;
	palette_t temp[256];

	memcpy(temp,pal,sizeof(palette_t)*256);
	for(i=0;i<256;i++)
	{
		r=pal[i].red;
		g=pal[i].green;
		b=pal[i].blue;
		r=(r*(gamma+4))/4;
		g=(g*(gamma+4))/4;
		b=(b*(gamma+4))/4;
		if(r>255)
			r=255;
		if(g>255)
			g=255;
		if(b>255)
			b=255;
		pal[i].red=r;
		pal[i].green=g;
		pal[i].blue=b;
	}
	this->RealizePalette();
	memcpy(pal,temp,sizeof(palette_t)*256);
}

//--------------------------------------------------------------------------
// these are all the event handling functions

long FAR PASCAL MGLDraw_EventHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case MM_MCINOTIFY:	// this is just tossed in because I have CD audio in there
			CDNeedsUpdating();
			break;
 		default:
			return DefWindowProc(hwnd,message,wParam,lParam);
	}
	return 0L;
}

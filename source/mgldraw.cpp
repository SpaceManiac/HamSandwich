/* MGLDraw

   A moderately cute little wrapper around MGL.

   -- No longer actually an MGL wrapper! --
   -- Currently using PixelToaster --

   For quick reference, to use MGL in a program, you must:

   link in gm.lib and mglfx.lib as the first two things in the list of things to link in.

*/

#include "mgldraw.h"
#include "game.h"	// even sorrier about this crap (it's for MGLDraw_Activate only)
#include "sound.h"
#include "music.h"
#include "ctype.h"

using namespace PixelToaster;

// Replacement for missing MGL functions
int MGL_random(int max) {
    return rand() % max;
}
long MGL_randoml(long max) {
    return rand() % max;
}
void MGL_fatalError(const char* txt) {
    fprintf(stderr, "%s", txt);
    exit(0);
}

// I'm very sorry about this.  It has to do with that whole no-function-pointers-to-member-functions
// thing I mentioned earlier.
MGLDraw *_globalMGLDraw;
byte needPalRealize=0;

// Pixel toaster callback thingies
class PtListener : public Listener {
    MGLDraw* mgldraw;
    bool shift;
public:
    PtListener(MGLDraw* mgldraw) : mgldraw(mgldraw), shift(false) {}

    bool defaultKeyHandlers() const { return false; }
    void onActivate(DisplayInterface & display, bool active);
    void onKeyDown( DisplayInterface & display, Key key );
    void onKeyPressed( DisplayInterface & display, Key key );
    void onKeyUp( DisplayInterface & display, Key key );
    void onMouseButtonDown( DisplayInterface & display, Mouse mouse ) { mgldraw->SetMouseDown(1); }
    void onMouseButtonUp( DisplayInterface & display, Mouse mouse ) { mgldraw->SetMouseDown(0); }
    void onMouseMove( DisplayInterface & display, Mouse mouse ) { mgldraw->SetMouse(mouse.x, mouse.y); }
    bool onClose( DisplayInterface & display ) { mgldraw->Quit(); return false; }
};

void PtListener::onKeyDown(DisplayInterface & display, Key key) {
    if(key == Key::Shift) shift = true;
    ControlKeyDown((char) key);
}

void PtListener::onKeyPressed(DisplayInterface & display, Key key) {
    char chkey = (char) key;
    if (!shift) {
        chkey = tolower(chkey);
    }
    mgldraw->SetLastKey(chkey);
}

void PtListener::onKeyUp(DisplayInterface & display, Key key) {
    if(key == Key::Shift) shift = false;
    ControlKeyDown((char) key);
}

void PtListener::onActivate(DisplayInterface & display, bool active) {
    if(active) {
		SetGameIdle(0);
		needPalRealize=1;
	} else
		SetGameIdle(1);
}

MGLDraw::MGLDraw(const char *name,int xRes,int yRes,int bpp,bool window,HINSTANCE hInst)
{
    //GM_modeInfo	info;
	// the driver options are pretty much set up to go for directdraw only.  Pretty  much.
	//GM_driverOptions driverOpt =
	//{
	//	true,			/* UseWinDirect		*/
	//	true,			/* UseDirectDraw	*/
	//	true,			/* UseVGA			*/
	//	true,			/* UseVGAX			*/
	//	true,			/* UseVBE			*/
	//	true,			/* UseVBEAF			*/
	//	true,			/* UseLinear		*/
	//    true,			/* UseFullscreenDIB	*/
	//	false,			/* UseHWOpenGL		*/
	//	MGL_GL_AUTO,	/* OpenGLType		*/
	//	GM_MODE_8BPP,	/* modeFlags		*/
	//};

    ptListener = new PtListener(this);
    ptDisplay.open(name, xRes, yRes, window ? Output::Windowed : Output::Fullscreen, Mode::TrueColor);
    ptDisplay.listener(ptListener);

    if (!ptDisplay.open()) {
        MGL_fatalError("Couldn't set display mode :V");
    }

	// must initialize sound after that stuff and before switching video modes,
	// so it has to be in this constructor!  Sorry
	if(JamulSoundInit(hInst,name,512))
		SoundSystemExists();

	//GM_registerEventProc(MGLDraw_EventHandler);
	//GM_setModeSwitchFunc(MGLDraw_SwitchModes);
	//GM_setAppActivate(MGLDraw_Activate);
	//GM_setSuspendAppCallback(MGLDraw_Suspend);

	/*
	//------------------------------- CRAAZY DEBUGGING SHIT
	{
		FILE *f;
		int i;

		f=fopen("log.txt","wt");
		i=0;
		while(gm->modeList[i].mode!=0 && i<grMAXMODE)
		{
			fprintf(f,"Mode: %3d X:%4d Y:%4d B:%3d Page:%2d Flag: %12d Str: %d WSt: %d Name: %s\n",
				gm->modeList[i].mode,gm->modeList[i].xRes,gm->modeList[i].yRes,
				gm->modeList[i].bitsPerPixel,gm->modeList[i].pages,gm->modeList[i].flags,
				gm->modeList[i].stretch,gm->modeList[i].windowedStretch,gm->modeList[i].driverName);
			i++;
		}
		fclose(f);
	}
	//----------------------------------
	*/



	/*if (!GM_findMode(&info,xRes,yRes,bpp))
		MGL_fatalError("Unable to find graphics mode!");
	if (!GM_setMode(&info,window,2,true))
		MGL_fatalError(MGL_errorMsg(MGL_result()));*/

	readyToQuit=false;
	_globalMGLDraw=this;

	// gimme windows colors
	this->xRes=xRes;
	this->yRes=yRes;
	this->bpp=bpp;
	this->pitch=xRes;
	this->scrn=new byte[xRes * yRes];
    ptBuffer = new TrueColorPixel[xRes * yRes];

	mouseDown=0;
}

MGLDraw::~MGLDraw(void)
{
	JamulSoundExit();
    delete ptListener;
    delete[] ptBuffer;
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
	return (!readyToQuit);
}

HWND MGLDraw::GetHWnd(void)
{
    // uh, fix later...?
	//return (HWND)gm->mainWindow;
    return NULL;
}

void MGLDraw::Flip(void)
{
	if(GetGameIdle())
		GameIdle();

    // This is probably super slow...
    for (int i = 0; i < xRes * yRes; ++i) {
        ptBuffer[i].r = pal[scrn[i]].red;
        ptBuffer[i].g = pal[scrn[i]].green;
        ptBuffer[i].b = pal[scrn[i]].blue;
    }

    ptDisplay.update(ptBuffer);

	/*if(needPalRealize)
	{
		needPalRealize=0;
		if(_globalMGLDraw)
			_globalMGLDraw->RealizePalette();
	}*/
}

void MGLDraw::ClearScreen(void)
{
    memset(ptBuffer, 0, 256*sizeof(TrueColorPixel));
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

void MGLDraw::DarkPalette(void)
{
    return;
    // TODO: fix? doesn't seem to be called anywhere.

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
	//GM_setPalette(darkpal,256,0);
	//GM_realizePalette(256,0,true);
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
    printf("Set last key to %d = %c\n", (int)c, c);
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
    SetCursorPos(x, y);
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
    printf("Imma event handler: %d\n", message);
	switch(message)
	{
		case WM_LBUTTONDOWN:
			_globalMGLDraw->SetMouseDown(1);
			break;
		case WM_LBUTTONUP:
			_globalMGLDraw->SetMouseDown(0);
			break;
		case WM_KEYUP:
			ControlKeyUp((char)((lParam>>16)&255));
			break;
		case WM_CHAR:
			_globalMGLDraw->SetLastKey((char)wParam);
			break;
		case WM_KEYDOWN:
			ControlKeyDown((char)((lParam>>16)&255));
			break;
		case MM_MCINOTIFY:	// this is just tossed in because I have CD audio in there
			CDNeedsUpdating();
			break;
 		default:
			return DefWindowProc(hwnd,message,wParam,lParam);
	}
	return 0L;
}

HWND MGLGetHWnd(void) // "augh"
{
	return _globalMGLDraw->GetHWnd();
}

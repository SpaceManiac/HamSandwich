/* MGLDraw

   A moderately cute little wrapper around MGL.

   For quick reference, to use MGL in a program, you must:

   link in gm.lib and mglfx.lib as the first two things in the list of things to link in.

*/

#include "mgldraw.h"
#include "music.h"	 //-- very sorry about this, stupid windows
#include "game.h"	// even sorrier about this crap (it's for MGLDraw_Activate only)

// I'm very sorry about this.  It has to do with that whole no-function-pointers-to-member-functions
// thing I mentioned earlier.
MGLDraw *_globalMGLDraw;

MGLDraw::MGLDraw(char *name,int xRes,int yRes,int bpp,bool window,HINSTANCE hInst)
{
    GM_modeInfo	info;
	// the driver options are pretty much set up to go for directdraw only.  Pretty  much.
	GM_driverOptions driverOpt =
	{
		false,			/* UseWinDirect		*/
		true,			/* UseDirectDraw	*/
		false,			/* UseVGA			*/
		false,			/* UseVGAX			*/
		true,			/* UseVBE			*/
		true,			/* UseVBEAF			*/
		true,			/* UseLinear		*/
	    true,			/* UseFullscreenDIB	*/
		true,			/* UseHWOpenGL		*/
		MGL_GL_AUTO,	/* OpenGLType		*/
		GM_MODE_8BPP,	/* modeFlags		*/
	};

	GM_setDriverOptions(&driverOpt);
	if ((gm = GM_init(name)) == NULL)
		MGL_fatalError(MGL_errorMsg(MGL_result()));

	// must initialize sound after that stuff and before switching video modes,
	// so it has to be in this constructor!  Sorry
	if(!JamulSoundInit(hInst,name,255))
		MGL_fatalError("Couldn't initialize sound!");

	GM_registerEventProc(MGLDraw_EventHandler);
	GM_setModeSwitchFunc(MGLDraw_SwitchModes);
	GM_setAppActivate(MGLDraw_Activate);
	//GM_setSuspendAppCallback(MGLDraw_Suspend);

	if (!GM_findMode(&info,xRes,yRes,bpp))
		MGL_fatalError("Unable to find graphics mode!");
	if (!GM_setMode(&info,window,2,true))
		MGL_fatalError(MGL_errorMsg(MGL_result()));

	readyToQuit=false;
	_globalMGLDraw=this;

	// gimme windows colors
	GM_initSysPalNoStatic(true);
	this->xRes=xRes;
	this->yRes=yRes;
	this->bpp=bpp;
	this->pitch=gm->dc->mi.bytesPerLine;
	this->scrn=(byte *)gm->dc->surface;

	mouseDown=0;
	MS_hide();
}

MGLDraw::~MGLDraw(void)
{
	GM_cleanup();
	JamulSoundExit();
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
	GM_processEventsWin();
	scrn=(byte *)gm->dc->surface;
	return (!readyToQuit);
}

HWND MGLDraw::GetHWnd(void)
{
	return (HWND)gm->mainWindow;
}

void MGLDraw::Flip(void)
{
	if(GetGameIdle())
		GameIdle();
	GM_swapBuffers(MGL_waitVRT);
	scrn=(byte *)gm->dc->surface;
}

void MGLDraw::ClearScreen(void)
{
	MGL_clearDevice();
}

MGLDC *MGLDraw::GetDC(void)
{
	return gm->dc;
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
	GM_setPalette(pal,256,0);
	GM_realizePalette(256,0,true);
}

void MGLDraw::DarkPalette(void)
{
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

char MGLDraw::LastKeyPeek(void)
{
	return lastKeyPressed;
}

bool MGLDraw::LoadBMP(char *name)
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
		scr=(byte *)((int)scrn+(bmpIHead.biHeight-1-i)*pitch);
		fread(scr,bmpIHead.biWidth,1,f);
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
		case WM_LBUTTONDOWN:
			_globalMGLDraw->SetMouseDown(1);
			break;
		case WM_LBUTTONUP:
			_globalMGLDraw->SetMouseDown(0);
			break;
		case WM_KEYUP:
			ControlKeyUp((char)wParam);
			break;
		case WM_CHAR:
			_globalMGLDraw->SetLastKey((char)wParam);
			break;
		case WM_KEYDOWN:
			ControlKeyDown((char)wParam);
			break;
		case MM_MCINOTIFY:	// this is just tossed in because I have CD audio in there
			CDNeedsUpdating();
			break;
 		default:
			return DefWindowProc(hwnd,message,wParam,lParam);
	}
	return 0L;
}

void MGLDraw_SwitchModes(GM_modeInfo *mode,bool windowed)
{
	// need to do anything when switching from windowed to full-screen?
	_globalMGLDraw->RealizePalette();
}

void MGLDraw_Activate(bool active)
{
	// stuff to do whenever the program is switched away from or to, such as pausing CD music
	if(active)
		SetGameIdle(0);
	else
		SetGameIdle(1);
}

int ASMAPI MGLDraw_Suspend(MGLDC *dc,int flags)
{
	if (flags == MGL_DEACTIVATE)
	{
		/* We are switching back to GDI mode, so put code in here to disable
		 * stuff when switched away from fullscreen mode. Note that this
		 * function may get once to determine if the switch should occur,
		 * and again when the switch actually happens.
		 */
	}
	else if (flags == MGL_REACTIVATE)
	{
		/* We are now back in fullscreen mode, so put code in here to renable
		 * stuff for fullscreen modes.
		 */
		_globalMGLDraw->RealizePalette();
	}
	return MGL_NO_SUSPEND_APP;
}

HWND MGLGetHWnd(void)
{
	return _globalMGLDraw->GetHWnd();
}

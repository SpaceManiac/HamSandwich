/* MGLDraw

   A moderately cute little wrapper around MGL.

   For quick reference, to use MGL in a program, you must:

   link in gm.lib and mglfx.lib as the first two things in the list of things to link in.

*/

#include "mgldraw.h"
#include "game.h"	// even sorrier about this crap (it's for MGLDraw_Activate only)
#include "sound.h"
#include <ddraw.h>
#include "internet.h"

// I'm very sorry about this.  It has to do with that whole no-function-pointers-to-member-functions
// thing I mentioned earlier.
MGLDraw *_globalMGLDraw;
byte needPalRealize=0;
byte firstStartup;
static byte exitNow=0;

MGLDraw::MGLDraw(char *name,int xRes,int yRes,int bpp,bool window,HINSTANCE hInst)
{
    GM_modeInfo	info;
	RECT rect;

	exitNow=0;
	firstStartup=1;
	GM_initSysPalNoStatic(true);
	
	// the driver options are pretty much set up to go for directdraw only.  Pretty  much.
	GM_driverOptions driverOpt = 
	{
		false,			/* UseWinDirect		*/
		true,			/* UseDirectDraw	*/
		true,			/* UseVGA			*/
		true,			/* UseVGAX			*/
		true,			/* UseVBE			*/
		true,			/* UseVBEAF			*/
		true,			/* UseLinear		*/
	    true,			/* UseFullscreenDIB	*/
		false,			/* UseHWOpenGL		*/
		MGL_GL_AUTO,	/* OpenGLType		*/
		GM_MODE_8BPP,	/* modeFlags		*/
	};

	GM_setDriverOptions(&driverOpt);
	if ((gm = GM_init(name)) == NULL)
		MGL_fatalError(MGL_errorMsg(MGL_result()));
	
	// must initialize sound after that stuff and before switching video modes,
	// so it has to be in this constructor!  Sorry
	if(JamulSoundInit(hInst,name,512))
		SoundSystemExists();
	
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

	if (!GM_findMode(&info,xRes,yRes,bpp))
		MGL_fatalError("Unable to find graphics mode!");
	if (!GM_setMode(&info,window,2,true))
		MGL_fatalError(MGL_errorMsg(MGL_result()));

	readyToQuit=false;
	_globalMGLDraw=this;

	// gimme windows colors
	this->xRes=xRes;
	this->yRes=yRes;
	this->bpp=bpp;
	this->pitch=gm->dc->mi.bytesPerLine;
	this->scrn=(byte *)gm->dc->surface;
	this->windowed=window;
	this->hInst=hInst;

	reversePal=0;
	colorblind=0;
	if(window)
	{
		GetWindowRect(gm->mainWindow,&rect);
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

	MS_hide();
	SeedRNG();
	
	GM_setModeSwitchFunc(MGLDraw_SwitchModes);
	GM_setAppActivate(MGLDraw_Activate);
	GM_setSuspendAppCallback(MGLDraw_Suspend);
}

MGLDraw::~MGLDraw(void)
{
	exitNow=1;
	JamulSoundExit();
	GM_cleanup();
}

void MGLDraw::GetMouse(int *x,int *y)
{
	MS_getPos(x,y);

	*x-=windowX;
	*y-=windowY;
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
	GM_processEventsWin();
	if(gm)
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
	if(gm)
		scrn=(byte *)gm->dc->surface;

	if(needPalRealize)
	{
		needPalRealize--;
		if(!needPalRealize)
		{
			RealizePalette();
		}
	}
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
	int i,j;
	palette_t pal2[256];

	if(reversePal)
	{
		for(i=0;i<8;i++)
			for(j=0;j<32;j++)
			{
				pal2[i*32+j]=pal[i*32+(31-j)];
			}
		
		GM_setPalette(pal2,256,0);
		GM_realizePalette(256,0,false);
		return;
	}
	GM_setPalette(pal,256,0);
	GM_realizePalette(256,0,false);

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

void MGLDraw::GreyPalette(void)
{
	int i;

	// put the greys in all the colors
	for(i=32;i<256;i++)
	{
		pal[i]=pal[i&31];
	}
	RealizePalette();
}

void MGLDraw::CyclePalette(void)
{
	int i;
	palette_t p;

	p=pal[0];

	for(i=0;i<255;i++)
	{
		pal[i]=pal[i+1];
	}
	pal[255]=p;
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
	char c=lastKeyPressed;
	lastKeyPressed=0;
	return c;
}

void MGLDraw::SetMouseDown(byte w)
{
	mouseDown=w;
}

void MGLDraw::SetRMouseDown(byte w)
{
	rMouseDown=w;
}


byte MGLDraw::MouseDown(byte w)
{
	if(w==0)
		return mouseDown;
	else
		return rMouseDown;
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
		case WM_RBUTTONDOWN:
			_globalMGLDraw->SetRMouseDown(1);
			break;
		case WM_RBUTTONUP:
			_globalMGLDraw->SetRMouseDown(0);
			break;
		case WM_KEYUP:
			ControlKeyUp((char)((lParam>>16)&255));
			break;
		case WM_CHAR:
			_globalMGLDraw->SetLastKey((char)wParam);
			break;
		case WM_PALETTECHANGED:
			if((HWND)wParam!=hwnd)
				_globalMGLDraw->RealizePalette();
			break;
		case WM_QUERYNEWPALETTE:
			_globalMGLDraw->RealizePalette();
			return TRUE;
			break;
		case WM_KEYDOWN:
			ControlKeyDown((char)((lParam>>16)&255));
			break;
		case INTERNET_EVENT:
			Web_HandleEvent((SOCKET)wParam,WSAGETSELECTEVENT(lParam),WSAGETSELECTERROR(lParam));
			break;
		case MM_MCINOTIFY:	// this is just tossed in because I have CD audio in there
			break;
		case WM_QUIT:
			_globalMGLDraw->readyToQuit=1;
			break;
 		default:
			return DefWindowProc(hwnd,message,wParam,lParam);
	}
	return 0L;
}

void MGLDraw_SwitchModes(GM_modeInfo *mode,int windowed)
{
	// need to do anything when switching from windowed to full-screen?
	_globalMGLDraw->RealizePalette();
	_globalMGLDraw->windowed=windowed;
	needPalRealize=30;
}

void MGLDraw_Activate(int active)
{
//	int xR,yR,bpp,win;
//	HINSTANCE hInst;

	// stuff to do whenever the program is switched away from or to, such as pausing CD music
	if(active)
	{
		needPalRealize=30;

		/*
		xR=_globalMGLDraw->xRes;
		yR=_globalMGLDraw->yRes;
		bpp=_globalMGLDraw->bpp;
		win=_globalMGLDraw->windowed;
		hInst=_globalMGLDraw->hInst;
		delete _globalMGLDraw;
		_globalMGLDraw=new MGLDraw("Stockboy",xR,yR,bpp,win,hInst);
		SetDisplayMGL(_globalMGLDraw);
		SetGameMGL(_globalMGLDraw);*/
		SetGameIdle(0);
	}
	else
	{
		if(!exitNow)
			WaitMessage();
		//SetGameIdle(1);
	}
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
		//SetGameIdle(1);
		if(!exitNow)
			WaitMessage();
		return MGL_SUSPEND_APP;
	}
	else if (flags == MGL_REACTIVATE) 
	{
		/* We are now back in fullscreen mode, so put code in here to renable
		 * stuff for fullscreen modes.
		 */
		SetGameIdle(0);
		needPalRealize=30;
		return MGL_NO_SUSPEND_APP;
	}
	return MGL_NO_SUSPEND_APP;
}

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
	return MGL_randoml(range);
} 

void MGLDraw::SetReversePal(byte on)
{
	reversePal=on;
}

void MGLDraw::SetColorblind(byte on)
{
	colorblind=on;
}

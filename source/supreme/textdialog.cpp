#include "textdialog.h"
#include "editor.h"
#include "dialogbits.h"

static char title[64];
static char text[128];
static byte exitCode;
static byte maxLength;

void InitTextDialog(const char *title2,const char *contents,byte maxLen)
{
	exitCode=0;

	maxLength=maxLen;
	strcpy(title,title2);
	strcpy(text,contents);
}

void ExitTextDialog(void)
{
}

void RenderTextDialog(int msx,int msy,MGLDraw *mgl)
{
	static byte flash=0;

	flash=1-flash;

	// box for the whole dialog
	mgl->FillBox(50,200,590,300,8);
	mgl->Box(50,200,590,300,31);
	// the box to enter your text
	mgl->Box(52,230,588,246,16);
	mgl->FillBox(53,231,587,245,0);
	Print(54,232,text,0,1);
	if(flash)
	{
		int x;

		x=54+GetStrLength(text,1);
		mgl->Box(x,232+2,x+4,232+10,31);
	}
	CenterPrint(320,204,title,0,1);

	RenderButtonImage(msx,msy,320-140-60,270,60,15,"Okay");
	RenderButtonImage(msx,msy,320+140,270,60,15,"Cancel");
}

static void RenderGrnButtonImage(int mouseX,int mouseY,int x,int y,int width,int height,const char *txt)
{
	if(PointInRect(mouseX,mouseY,x,y,x+width,y+height))
		DrawFillBox(x,y,x+width,y+height,8+32*1);
	DrawBox(x,y,x+width,y+height,32*1+16);
	PrintLimited(x+2,y+2,x+width-1,txt,0,1);
}

void RenderTextDialog2(int msx,int msy,MGLDraw *mgl)
{
	static byte flash=0;

	flash=1-flash;

	// box for the whole dialog
	mgl->FillBox(50,200,590,300,32*1+4);
	mgl->Box(50,200,590,300,32*1+16);
	// the box to enter your text
	mgl->Box(52,230,588,246,32*1+16);
	mgl->FillBox(53,231,587,245,0);
	PrintLimited(54,232,586,text,0,1);
	if(flash)
	{
		int x;

		x=54+GetStrLength(text,1);
		mgl->Box(x,232+2,x+4,232+10,32*1+16);
	}
	PrintGlow(320-GetStrLength(title,2)/2,204,title,0,2);

	RenderGrnButtonImage(msx,msy,320-140-60,270,60,15,"Okay");
	RenderGrnButtonImage(msx,msy,320+140,270,60,15,"Cancel");
}

byte TextDialogKey(char key)
{
	int len;

	if(key==27)	// esc
	{
		exitCode=TM_CANCEL;
		return 0;
	}

	if(key==8)	// backspace
	{
		if(strlen(text)>0)
			text[strlen(text)-1]='\0';
		return 1;
	}

	if(key==13)	// enter
	{
		exitCode=TM_OKAY;
		return 1;	// accept the text
	}

	if(strlen(text)<maxLength)
	{
		len=strlen(text);
		text[len]=key;
		text[len+1]='\0';
	}

	return 1;
}

byte TextDialogClick(int msx,int msy)
{
	if(exitCode)
		return exitCode;

	if(PointInRect(msx,msy,320-140-60,270,320-140,285))
	{
		exitCode=TM_OKAY;
		return TM_OKAY;
	}

	if(PointInRect(msx,msy,320+140,270,320+140+60,285))
	{
		exitCode=TM_CANCEL;
		return TM_CANCEL;
	}

	return 0;
}

const char *GetText(void)
{
	return text;
}

byte TextDialogCommand(void)
{
	return exitCode;
}

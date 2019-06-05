#include "yesnodialog.h"
#include "editor.h"
#include "dialogbits.h"
#include "display.h"

static char text[128],choice[2][32];
static byte exitCode;

void InitYesNoDialog(const char *text2,const char *choice1,const char *choice2)
{
	exitCode=0;

	strcpy(text,text2);
	strcpy(choice[0],choice1);
	strcpy(choice[1],choice2);
}

void ExitYesNoDialog(void)
{
}

void RenderYesNoDialog(int msx,int msy,MGLDraw *mgl)
{
	// box for the whole dialog
	mgl->FillBox(150,200,490,300,8);
	mgl->Box(150,200,490,300,31);

	CenterPrint(320,204,text,0,1);

	RenderButtonImage(msx,msy,320-100-60,270,60,15,choice[0]);
	RenderButtonImage(msx,msy,320+100,270,60,15,choice[1]);
}

static void RenderGrnButtonImage(int mouseX,int mouseY,int x,int y,int width,int height,char *txt)
{
	if(PointInRect(mouseX,mouseY,x,y,x+width,y+height))
		DrawFillBox(x,y,x+width,y+height,31);
	DrawBox(x,y,x+width,y+height,31);
	PrintGlowLimited(x+2,y+2,x+width-1,txt,0,1);
}

void RenderYesNoDialog2(int msx,int msy,MGLDraw *mgl)
{
	// box for the whole dialog
	mgl->FillBox(150,200,490,300,32*2+20);
	mgl->Box(150,200,490,300,31);

	PrintUnGlow(320-GetStrLength(text,2)/2,204,text,2);

	RenderGrnButtonImage(msx,msy,320-100-60,270,60,15,choice[0]);
	RenderGrnButtonImage(msx,msy,320+100,270,60,15,choice[1]);
}

void RenderYesNoDialog3(int msx,int msy,MGLDraw *mgl)
{
	// box for the whole dialog
	mgl->FillBox(150,200,490,300,32*1+4);
	mgl->Box(150,200,490,300,32*1+16);

	PrintGlowRect(152,202,488,260,18,text,2);

	RenderGrnButtonImage(msx,msy,320-100-60,270,60,15,choice[0]);
	RenderGrnButtonImage(msx,msy,320+100,270,60,15,choice[1]);
}

byte YesNoDialogKey(char key)
{
	if(key==27 || key=='n' || key=='N')	// esc
	{
		exitCode=YNM_NO;
		return 0;
	}

	if(key==13 || key=='y' || key=='Y')	// enter
	{
		exitCode=YNM_YES;
		return 0;
	}

	return 1;
}

byte YesNoDialogClick(int msx,int msy)
{
	if(exitCode)
		return exitCode;

	if(PointInRect(msx,msy,320-100-60,270,320-100,285))
	{
		exitCode=YNM_YES;
		return YNM_YES;
	}

	if(PointInRect(msx,msy,320+100,270,320+100+60,285))
	{
		exitCode=YNM_NO;
		return YNM_NO;
	}

	return 0;
}

byte YesNoDialogCommand(void)
{
	return exitCode;
}

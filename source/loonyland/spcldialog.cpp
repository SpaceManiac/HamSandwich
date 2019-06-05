#include "spcldialog.h"
#include "editor.h"

static byte mapNum;
static world_t *world;
special_t *spcl;
static int spclnum;

void InitSpclDialog(special_t *theSpcl,byte spnum,world_t *wrld,byte currentMap)
{
	world=wrld;
	mapNum=currentMap;
	spcl=theSpcl;
	spclnum=spnum;
}

void ExitSpclDialog(void)
{
}

static void RenderCheckbox(int x,int y,int v,const char *txt)
{
	if(v)
		DrawFillBox(x,y,x+11,y+11,16);
	else
		DrawBox(x,y,x+11,y+11,16);
	Print(x+13,y,txt,0,1);
}

void RenderValueAdjuster(const char *name,int value,int x,int y)
{
	char s[8];

	Print(x,y,name,0,1);
	DrawBox(x,y+14,x+20,y+28,16);
	Print(x+5,y+16,"--",0,1);
	DrawBox(x+22,y+14,x+32,y+28,16);
	Print(x+25,y+16,"-",0,1);

	sprintf(s,"%03d",value);
	Print(x+38,y+16,s,0,1);

	DrawBox(x+68,y+14,x+78,y+28,16);
	Print(x+70,y+16,"+",0,1);
	DrawBox(x+80,y+14,x+100,y+28,16);
	Print(x+83,y+16,"++",0,1);
}

void RenderSpclDialog(int msx,int msy,MGLDraw *mgl)
{
	char s[6];

	// box for the whole dialog
	DrawFillBox(60,10,480,370,8);
	DrawBox(60,10,480,370,16);

	sprintf(s,"#%02d",spclnum);
	Print(450,12,s,0,1);
	// the trigger checkboxes
	Print(62,12,"Trigger",0,1);

	RenderCheckbox(62,26,spcl->trigger&TRG_STEP,"Player step");
	RenderCheckbox(62,40,spcl->trigger&TRG_ENEMYSTEP,"Enemy step");
	RenderCheckbox(62,54,spcl->trigger&TRG_NEAR,"Player within N tiles");
	RenderCheckbox(62,68,spcl->trigger&TRG_KILLALL,"Kill all monster N");
	RenderCheckbox(62,82,spcl->trigger&TRG_SHOOT,"Shoot item/wall");
	RenderCheckbox(62,96,spcl->trigger&TRG_CHAIN,"Chain off neighbor");
	RenderCheckbox(62,110,spcl->trigger&TRG_TIMED,"Once Every N seconds");
	RenderCheckbox(62,124,spcl->trigger&TRG_RANDOM,"Random (N% per second)");
	RenderCheckbox(220,54,spcl->trigger&TRG_VARON,"Var N = N2");
	RenderCheckbox(220,68,spcl->trigger&TRG_VAROFF,"Var N != N2");
	RenderCheckbox(220,82,spcl->trigger&TRG_GETITEM,"Get Item");
	RenderCheckbox(220,96,spcl->trigger&TRG_DELAYCHAIN,"Delayed Chain");
	RenderCheckbox(300,96,spcl->trigger&TRG_ITEMHERE,"Item Here");

	RenderCheckbox(62,148,spcl->trigger&TRG_REPEATABLE,"Multiple use");
	RenderCheckbox(62,162,spcl->trigger&TRG_MESSAGE,"Show message");

	// the box where the text is entered
	Print(62,341,"Message",0,1);
	DrawBox(62,355,362,368,16);
	DrawFillBox(63,356,361,367,0);
	Print(64,357,spcl->msg,0,1);

	// the effect choices
	Print(62,180,"Effect",0,1);
	DrawBox(62,194,132,208,16+15*(spcl->effect==SPC_NONE));
	Print(64,196,"None",0,1);
	DrawBox(62,210,132,224,16+15*(spcl->effect==SPC_SUMMON));
	Print(64,212,"Summon",0,1);
	DrawBox(62,226,132,240,16+15*(spcl->effect==SPC_ZAPWALL));
	Print(64,228,"Zap Wall",0,1);
	DrawBox(62,242,132,256,16+15*(spcl->effect==SPC_RAISEWALL));
	Print(64,244,"MakeWall",0,1);
	DrawBox(62,258,132,272,16+15*(spcl->effect==SPC_TOGGLEWALL));
	Print(64,260,"TogglWall",0,1);
	DrawBox(62,274,132,288,16+15*(spcl->effect==SPC_COPYMAP));
	Print(64,276,"CopyMap",0,1);
	DrawBox(62,290,132,304,16+15*(spcl->effect==SPC_DROPITEM));
	Print(64,292,"DropItem",0,1);
	DrawBox(62,306,132,320,16+15*(spcl->effect==SPC_SWAPMAP));
	Print(64,308,"SwapMap",0,1);

	DrawBox(134,194,204,208,16+15*(spcl->effect==SPC_TELEPORT));
	Print(136,196,"Teleport",0,1);
	DrawBox(134,210,204,224,16+15*(spcl->effect==SPC_LIGHT));
	Print(136,212,"Light",0,1);
	DrawBox(134,226,204,240,16+15*(spcl->effect==SPC_GOTOMAP));
	Print(136,228,"Goto Map",0,1);
	DrawBox(134,242,204,256,16+15*(spcl->effect==SPC_SETVAR));
	Print(136,244,"Set Var",0,1);
	DrawBox(134,258,204,272,16+15*(spcl->effect==SPC_PICTURE));
	Print(136,260,"Show Pic",0,1);
	DrawBox(134,274,204,288,16+15*(spcl->effect==SPC_PLAYSOUND));
	Print(136,276,"Play Snd",0,1);
	DrawBox(134,290,204,304,16+15*(spcl->effect==SPC_TEMPLIGHT));
	Print(136,292,"TmpLght",0,1);
	DrawBox(134,306,204,320,16+15*(spcl->effect==SPC_CHGTILE));
	Print(134,308,"ChgTile",0,1);

	DrawBox(206,306,276,320,16+15*(spcl->effect==SPC_CHAT));
	Print(206,308,"Chat",0,1);
	DrawBox(206,290,276,304,16+15*(spcl->effect==SPC_LIGHTBALL));
	Print(206,292,"Lghtball",0,1);

	// trigger value
	RenderValueAdjuster("Trigger Value",spcl->trigValue,210,12);
	// trigger value #2
	RenderValueAdjuster("Other Value",spcl->trigValue2,340,12);
	// trigger X
	RenderValueAdjuster("Trigger X",spcl->trigX,340,42);
	// trigger Y
	RenderValueAdjuster("Trigger Y",spcl->trigY,340,72);
	// value
	RenderValueAdjuster("Effect Value",spcl->value,210,180);
	// target tag
	RenderValueAdjuster("Target Tag",spcl->effectTag,370,270);
	// targetX
	RenderValueAdjuster("Target X",spcl->effectX,370,300);
	// targetY
	RenderValueAdjuster("Target Y",spcl->effectY,370,330);
}

byte SpclDialogKey(char key)
{
	int len;

	if(key==27)	// esc
		return 0;

	if(key==8)	// backspace
	{
		if(strlen(spcl->msg)>0)
			spcl->msg[strlen(spcl->msg)-1]='\0';
		return 1;
	}

	if(key==13)	// enter
	{
		return 1;
	}

	if(strlen(spcl->msg)<31)
	{
		len=strlen(spcl->msg);
		spcl->msg[len]=key;
		spcl->msg[len+1]='\0';
	}
	return 1;
}

void CheckValueAdjustClick(int msx,int msy,int x,int y,int *value,int min,int max)
{
	if(msx>x-1 && msx<x+21 && msy>y+13 && msy<y+29)
		*value-=10;
	if(msx>x+21 && msx<x+33 && msy>y+13 && msy<y+29)
		*value-=1;
	if(msx>x+67 && msx<x+79 && msy>y+13 && msy<y+29)
		*value+=1;
	if(msx>x+79 && msx<x+101 && msy>y+13 && msy<y+29)
		*value+=10;

	if(*value<min)
		*value=max;
	if(*value>max)
		*value=min;
}

byte SpclDialogClick(int msx,int msy)
{
	int i;

	// checkbox toggles
	if(msx>61 && msx<72 && msy>25 && msy<26+14)
		spcl->trigger^=TRG_STEP;
	if(msx>61 && msx<72 && msy>39 && msy<40+14)
		spcl->trigger^=TRG_ENEMYSTEP;
	if(msx>61 && msx<72 && msy>53 && msy<54+14)
		spcl->trigger^=TRG_NEAR;
	if(msx>61 && msx<72 && msy>67 && msy<68+14)
		spcl->trigger^=TRG_KILLALL;
	if(msx>61 && msx<72 && msy>81 && msy<82+14)
		spcl->trigger^=TRG_SHOOT;
	if(msx>61 && msx<72 && msy>95 && msy<96+14)
		spcl->trigger^=TRG_CHAIN;
	if(msx>61 && msx<72 && msy>109 && msy<110+14)
		spcl->trigger^=TRG_TIMED;
	if(msx>61 && msx<72 && msy>123 && msy<124+14)
		spcl->trigger^=TRG_RANDOM;
	if(msx>61 && msx<72 && msy>147 && msy<148+14)
		spcl->trigger^=TRG_REPEATABLE;
	if(msx>61 && msx<72 && msy>161 && msy<162+14)
		spcl->trigger^=TRG_MESSAGE;

	if(msx>219 && msx<230 && msy>53 && msy<54+14)
		spcl->trigger^=TRG_VARON;
	if(msx>219 && msx<230 && msy>67 && msy<68+14)
		spcl->trigger^=TRG_VAROFF;
	if(msx>219 && msx<230 && msy>81 && msy<82+14)
		spcl->trigger^=TRG_GETITEM;
	if(msx>219 && msx<230 && msy>95 && msy<96+14)
		spcl->trigger^=TRG_DELAYCHAIN;

	if(msx>299 && msx<310 && msy>95 && msy<96+14)
		spcl->trigger^=TRG_ITEMHERE;

	// effect choices
	if(msx>61 && msx<133 && msy>193 && msy<209)
		spcl->effect=SPC_NONE;
	if(msx>61 && msx<133 && msy>209 && msy<227)
		spcl->effect=SPC_SUMMON;
	if(msx>61 && msx<133 && msy>227 && msy<243)
		spcl->effect=SPC_ZAPWALL;
	if(msx>61 && msx<133 && msy>243 && msy<257)
		spcl->effect=SPC_RAISEWALL;
	if(msx>61 && msx<133 && msy>257 && msy<271)
		spcl->effect=SPC_TOGGLEWALL;
	if(msx>61 && msx<133 && msy>273 && msy<287)
		spcl->effect=SPC_COPYMAP;
	if(msx>61 && msx<133 && msy>289 && msy<303)
		spcl->effect=SPC_DROPITEM;
	if(msx>61 && msx<133 && msy>305 && msy<319)
		spcl->effect=SPC_SWAPMAP;

	if(msx>133 && msx<205 && msy>193 && msy<209)
		spcl->effect=SPC_TELEPORT;
	if(msx>133 && msx<205 && msy>209 && msy<227)
		spcl->effect=SPC_LIGHT;
	if(msx>133 && msx<205 && msy>227 && msy<243)
		spcl->effect=SPC_GOTOMAP;
	if(msx>133 && msx<205 && msy>241 && msy<257)
		spcl->effect=SPC_SETVAR;
	if(msx>133 && msx<205 && msy>257 && msy<271)
		spcl->effect=SPC_PICTURE;
	if(msx>133 && msx<205 && msy>273 && msy<287)
		spcl->effect=SPC_PLAYSOUND;
	if(msx>133 && msx<205 && msy>289 && msy<303)
		spcl->effect=SPC_TEMPLIGHT;
	if(msx>133 && msx<205 && msy>305 && msy<319)
		spcl->effect=SPC_CHGTILE;
	if(msx>205 && msx<277 && msy>305 && msy<319)
		spcl->effect=SPC_CHAT;
	if(msx>205 && msx<277 && msy>289 && msy<303)
		spcl->effect=SPC_LIGHTBALL;

	// trigger value
	i=spcl->trigValue;
	CheckValueAdjustClick(msx,msy,210,12,&i,0,2000);
	spcl->trigValue=i;
	// trigger value #2
	i=spcl->trigValue2;
	CheckValueAdjustClick(msx,msy,340,12,&i,0,2000);
	spcl->trigValue2=i;
	// trigger X
	i=spcl->trigX;
	CheckValueAdjustClick(msx,msy,340,42,&i,0,2000);
	spcl->trigX=i;
	// trigger Y
	i=spcl->trigY;
	CheckValueAdjustClick(msx,msy,340,72,&i,0,2000);
	spcl->trigY=i;
	// value
	i=spcl->value;
	CheckValueAdjustClick(msx,msy,210,180,&i,-255,2000);
	spcl->value=i;
	// effect tag
	i=spcl->effectTag;
	CheckValueAdjustClick(msx,msy,370,270,&i,0,255);
	spcl->effectTag=i;
	// effect X
	i=spcl->effectX;
	CheckValueAdjustClick(msx,msy,370,300,&i,0,2000);
	spcl->effectX=i;
	// effect Y
	i=spcl->effectY;
	CheckValueAdjustClick(msx,msy,370,330,&i,0,2000);
	spcl->effectY=i;

	return 1;
}

#include "dialogbits.h"
#include "display.h"
#include "string_extras.h"

constexpr int MAX_BUTTONS = 128;
constexpr int BTN_TXT_LEN = 32;

struct button_t
{
	byte type;
	int id;
	byte on;
	int x,y;
	int width,height;
	char text[BTN_TXT_LEN];
	buttonFunc callback;
};

static button_t button[MAX_BUTTONS];
static byte stopChecking;

byte PointInRect(int x,int y,int rx1,int ry1,int rx2,int ry2)
{
	return (x>=rx1 && x<=rx2 && y>=ry1 && y<=ry2);
}

void ClearButtons(void)
{
	int i;

	stopChecking=1;

	for(i=0;i<MAX_BUTTONS;i++)
		button[i].type=BTN_NONE;
}

void ClearButtons(int minID,int maxID)
{
	int i;

	for(i=0;i<MAX_BUTTONS;i++)
	{
		if(button[i].id>=minID && button[i].id<=maxID)
			button[i].type=BTN_NONE;
	}
}


byte MakeButton(ButtonType type,int id,byte on,int x,int y,int width,int height,std::string_view txt,buttonFunc func)
{
	int i;

	for(i=0;i<MAX_BUTTONS;i++)
	{
		if(button[i].type==BTN_NONE)
		{
			button[i].type=type;
			button[i].id=id;
			button[i].on=on;
			button[i].x=x;
			button[i].y=y;
			button[i].width=width;
			button[i].height=height;
			StringDestination(button[i].text).assign(txt);
			button[i].callback=func;
			return i;
		}
	}
	return 255;
}

void SetButtonState(int id,byte on)
{
	int i;

	for(i=0;i<MAX_BUTTONS;i++)
		if(button[i].id==id)
			button[i].on=on;
}

byte GetButtonState(int id)
{
	int i;

	for(i=0;i<MAX_BUTTONS;i++)
		if(button[i].id==id)
			return button[i].on;

	return 0;
}

byte CheckButton(int mouseX,int mouseY,int id)
{
	int i;

	for(i=0;i<MAX_BUTTONS;i++)
	{
		if(button[i].id==id && button[i].type!=BTN_NONE && PointInRect(mouseX,mouseY,button[i].x,button[i].y,button[i].x+button[i].width,
			button[i].y+button[i].height))
		{
			if(button[i].callback!=NULL)
				button[i].callback(button[i].id);
			return 1;
		}
	}
	return 0;
}

byte CheckButtonCallback(int mouseX,int mouseY,int id,buttonFunc func)
{
	int i;

	for(i=0;i<MAX_BUTTONS;i++)
	{
		if(button[i].id==id && button[i].type!=BTN_NONE && PointInRect(mouseX,mouseY,button[i].x,button[i].y,button[i].x+button[i].width,
			button[i].y+button[i].height))
		{
			if(func!=NULL)
			{
				func(button[i].id);
				return 1;
			}
		}
	}

	return 0;
}

void CheckButtons(int mouseX,int mouseY)
{
	int i;

	stopChecking=0;

	for(i=0;i<MAX_BUTTONS;i++)
	{
		if(button[i].type && button[i].type!=BTN_NONE && PointInRect(mouseX,mouseY,button[i].x,button[i].y,button[i].x+button[i].width,
			button[i].y+button[i].height))
		{
			if(button[i].callback!=NULL)
				button[i].callback(button[i].id);
			if(stopChecking)
				return;
		}
	}
}

void RenderCheckbox(byte light,int x,int y,int v,std::string_view txt,MGLDraw *editmgl)
{
	editmgl->Box(x,y,x+11,y+11,31);

	if(light)
		editmgl->Box(x-1,y-1,x+12,y+12,16+32*1);
	if(v==CHECK_ON)
		editmgl->FillBox(x+1,y+1,x+10,y+10,16+32*1);
	else if(v==CHECK_MIXED)
	{
		for(int i=2;i<11;i+=2)
			editmgl->FillBox(x+1,y+i,x+10,y+i,16+32*1);
	}

	Print(x+13,y,txt,0,1);
}

void RenderButton(int mouseX,int mouseY,int id,MGLDraw *mgl)
{
	int i;

	for(i=0;i<MAX_BUTTONS;i++)
	{
		if(button[i].id==id)
		{
			switch(button[i].type)
			{
				case BTN_NORMAL:
					if(PointInRect(mouseX,mouseY,button[i].x,button[i].y,button[i].x+button[i].width,button[i].y+button[i].height))
						mgl->FillBox(button[i].x,button[i].y,button[i].x+button[i].width,button[i].y+button[i].height,8+32*1);
					mgl->Box(button[i].x,button[i].y,button[i].x+button[i].width,button[i].y+button[i].height,31);
					Print(button[i].x+2,button[i].y+2,button[i].text,0,1);
					break;
				case BTN_RADIO:
					if(button[i].on==CHECK_ON)
						RenderButtonImageLit(mouseX,mouseY,button[i].x,button[i].y,button[i].width,button[i].height,button[i].text);
					else
						RenderButtonImage(mouseX,mouseY,button[i].x,button[i].y,button[i].width,button[i].height,button[i].text);
					break;
				case BTN_STATIC:
					Print(button[i].x+2,button[i].y+2,button[i].text,0,1);
					break;
				case BTN_CHECK:
					if(PointInRect(mouseX,mouseY,button[i].x,button[i].y,button[i].x+button[i].width,button[i].y+button[i].height))
						RenderCheckbox(1,button[i].x,button[i].y,button[i].on,button[i].text,mgl);
					else
						RenderCheckbox(0,button[i].x,button[i].y,button[i].on,button[i].text,mgl);
					break;
				case BTN_TILE:
				case BTN_TILE2:
					if(PointInRect(mouseX,mouseY,button[i].x,button[i].y,button[i].x+button[i].width,button[i].y+button[i].height))
					{
						mgl->FillBox(button[i].x,button[i].y,button[i].x+button[i].width,button[i].y+button[i].height,8+32*1);
						if(button[i].type==BTN_TILE || (int)(button[i].text[0]+button[i].text[1]*128)!=0)
							RenderFloorTile(button[i].x+1,button[i].y+1,(int)(button[i].text[0]+button[i].text[1]*128),8);
						else
						{
							Print(button[i].x+2,button[i].y+2,"N",0,1);
							Print(button[i].x+10,button[i].y+6,"O",0,1);
							Print(button[i].x+18,button[i].y+10,"N",0,1);
							Print(button[i].x+26,button[i].y+14,"E",0,1);
						}
					}
					else
					{
						if(button[i].type==BTN_TILE || (int)(button[i].text[0]+button[i].text[1]*128)!=0)
							RenderFloorTile(button[i].x+1,button[i].y+1,(int)(button[i].text[0]+button[i].text[1]*128),0);
						else
						{
							Print(button[i].x+2,button[i].y+2,"N",0,1);
							Print(button[i].x+10,button[i].y+6,"O",0,1);
							Print(button[i].x+18,button[i].y+10,"N",0,1);
							Print(button[i].x+26,button[i].y+14,"E",0,1);
						}
					}
					mgl->Box(button[i].x,button[i].y,button[i].x+button[i].width,button[i].y+button[i].height,31);
					break;
			}
		}
	}
}

void RenderButtonImage(int mouseX,int mouseY,int x,int y,int width,int height, std::string_view txt)
{
	if(PointInRect(mouseX,mouseY,x,y,x+width,y+height))
		DrawFillBox(x,y,x+width,y+height,8+32*1);
	DrawBox(x,y,x+width,y+height,31);
	PrintLimited(x+2,y+2,x+width-1,txt,0,1);
}

void RenderButtonImageLit(int mouseX,int mouseY,int x,int y,int width,int height, std::string_view txt)
{
	if(PointInRect(mouseX,mouseY,x,y,x+width,y+height))
		DrawFillBox(x,y,x+width,y+height,31);
	else
		DrawFillBox(x,y,x+width,y+height,24+32*1);
	DrawBox(x,y,x+width,y+height,31);
	PrintLimited(x+2,y+2,x+width-1,txt,-32,1);
}

void RenderSliderImage(int mouseX,int mouseY,int x,int y,byte setting)
{
	DrawFillBox(x,y,x+63,y+1,16);

	if(PointInRect(mouseX,mouseY,x,y-4,x+63,y+5))
	{
		DrawFillBox(x+setting-2,y-4,x+setting+2,y+5,16+32*1);
		DrawBox(x+setting-2,y-4,x+setting+2,y+5,31);
		DrawBox(x+setting-4,y-6,x+setting+4,y+7,31);
	}
	else
	{
		DrawFillBox(x+setting-2,y-4,x+setting+2,y+5,(setting/2));
		DrawBox(x+setting-2,y-4,x+setting+2,y+5,31);
	}
}

void RenderButtons(int mouseX,int mouseY,MGLDraw *mgl)
{
	int i;

	for(i=0;i<MAX_BUTTONS;i++)
	{
		switch(button[i].type)
		{
			case BTN_NORMAL:
				RenderButtonImage(mouseX,mouseY,button[i].x,button[i].y,button[i].width,button[i].height,button[i].text);
				break;
			case BTN_RADIO:
				if(button[i].on==CHECK_ON)
					RenderButtonImageLit(mouseX,mouseY,button[i].x,button[i].y,button[i].width,button[i].height,button[i].text);
				else
					RenderButtonImage(mouseX,mouseY,button[i].x,button[i].y,button[i].width,button[i].height,button[i].text);
				break;
			case BTN_STATIC:
				Print(button[i].x+2,button[i].y+2,button[i].text,0,1);
				break;
			case BTN_CHECK:
				if(PointInRect(mouseX,mouseY,button[i].x,button[i].y,button[i].x+button[i].width,button[i].y+button[i].height))
					RenderCheckbox(1,button[i].x,button[i].y,button[i].on,button[i].text,mgl);
				else
					RenderCheckbox(0,button[i].x,button[i].y,button[i].on,button[i].text,mgl);
				break;
			case BTN_TILE:
			case BTN_TILE2:
				if(PointInRect(mouseX,mouseY,button[i].x,button[i].y,button[i].x+button[i].width,button[i].y+button[i].height))
					{
						mgl->FillBox(button[i].x,button[i].y,button[i].x+button[i].width,button[i].y+button[i].height,8+32*1);
						if(button[i].type==BTN_TILE || (int)(button[i].text[0]+button[i].text[1]*128)!=0)
							RenderFloorTile(button[i].x+1,button[i].y+1,(int)(button[i].text[0]+button[i].text[1]*128),8);
						else
						{
							Print(button[i].x+2,button[i].y+2,"N",0,1);
							Print(button[i].x+10,button[i].y+6,"O",0,1);
							Print(button[i].x+18,button[i].y+10,"N",0,1);
							Print(button[i].x+26,button[i].y+14,"E",0,1);
						}
					}
					else
					{
						if(button[i].type==BTN_TILE || (int)(button[i].text[0]+button[i].text[1]*128)!=0)
							RenderFloorTile(button[i].x+1,button[i].y+1,(int)(button[i].text[0]+button[i].text[1]*128),0);
						else
						{
							Print(button[i].x+2,button[i].y+2,"N",0,1);
							Print(button[i].x+10,button[i].y+6,"O",0,1);
							Print(button[i].x+18,button[i].y+10,"N",0,1);
							Print(button[i].x+26,button[i].y+14,"E",0,1);
						}
					}
					mgl->Box(button[i].x,button[i].y,button[i].x+button[i].width,button[i].y+button[i].height,31);
				break;
			case BTN_COLOR:
				if(PointInRect(mouseX,mouseY,button[i].x,button[i].y,button[i].x+button[i].width,button[i].y+button[i].height))
				{
					mgl->FillBox(button[i].x,button[i].y,button[i].x+button[i].width,button[i].y+button[i].height,8+32*1);
					mgl->FillBox(button[i].x+2,button[i].y+2,button[i].x+button[i].width-2,button[i].y+button[i].height-2,19+32*(byte)button[i].text[0]);
				}
				else
					mgl->FillBox(button[i].x+2,button[i].y+2,button[i].x+button[i].width-2,button[i].y+button[i].height-2,16+32*(byte)button[i].text[0]);
				mgl->Box(button[i].x,button[i].y,button[i].x+button[i].width,button[i].y+button[i].height,31);
				break;
		}
	}
}

void RadioOn(int id,int minID,int maxID)
{
	int i;

	for(i=0;i<MAX_BUTTONS;i++)
	{
		if(button[i].type==BTN_RADIO && button[i].id>=minID && button[i].id<=maxID)
		{
			if(button[i].id==id)
				button[i].on=CHECK_ON;
			else
				button[i].on=CHECK_OFF;
		}
	}
}

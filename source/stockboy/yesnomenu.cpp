#include "yesnomenu.h"

YesNoMenu::YesNoMenu(MGLDraw *mymgl)
{
	x=200;
	y=200;
	active=0;
	mgl=mymgl;
	answer=ANSWER_NONE;
}

YesNoMenu::~YesNoMenu(void)
{
}

void YesNoMenu::Activate(char *settitle,char *settitle2,char *setyes,char *setno)
{
	strcpy(title,settitle);
	strcpy(title2,settitle2);
	strcpy(yes,setyes);
	strcpy(no,setno);
	active=1;
}

void YesNoMenu::Render(int msx,int msy)
{
	if(!active)
		return;

	if(title[0]=='!')
	{
		// showing keebie shortcuts
		msx-=x;
		msy-=y;
		DrawFillBox(100,100,539,379,255-32+8);
		DrawBox(101,101,538,378,255-32+16);
		CenterPrintGlow(320,105,"Keyboard Shortcuts",0,0);
		DrawBox(220,125,420,126,255-32+16);

		CenterPrintGlow(160,140,"TAB",0,0);
		CenterPrintGlow(400,140,"Show/Hide Menus",0,0);

		CenterPrintGlow(160,170,"G",0,0);
		CenterPrintGlow(400,170,"'Pick up' what's under the cursor",0,0);
		CenterPrintGlow(400,190,"(picks up items when in item",0,0);
		CenterPrintGlow(400,210,"mode, floors in floor mode, etc)",0,0);
		
		CenterPrintGlow(160,240,"T",0,0);
		CenterPrintGlow(400,240,"Next Tool type",0,0);

		CenterPrintGlow(160,270,"ESC",0,0);
		CenterPrintGlow(400,270,"Exit editor",0,0);

		CenterPrintGlow(160,300,"-, +",0,0);
		CenterPrintGlow(400,300,"Change current Tool",0,0);

		CenterPrintGlow(160,330,"Arrows",0,0);
		CenterPrintGlow(400,330,"Change color and 'facing'",0,0);
		return;
	}
	msx-=x;
	msy-=y;

	editspr->GetSprite(20)->Draw(x,y,mgl);
	CenterPrintGlow(x+100,y+8,title,0,2);
	CenterPrintGlow(x+100,y+30,title2,0,2);
	
	if(PointInRect(msx,msy,6,52,96,74))
		editspr->GetSprite(22)->Draw(x+6,y+52,mgl);
	else
		editspr->GetSprite(21)->Draw(x+6,y+52,mgl);
	if(PointInRect(msx,msy,105,52,195,74))
		editspr->GetSprite(22)->Draw(x+105,y+52,mgl);
	else
		editspr->GetSprite(21)->Draw(x+105,y+52,mgl);

	CenterPrintGlow(x+51,x+54,yes,0,2);
	CenterPrintGlow(x+150,x+54,no,0,2);
}

// returns whether or not the click occurred in this menu, and hence needs no more processing
byte YesNoMenu::Click(int msx,int msy,byte btn)
{
	if(!active)
		return 0;

	msx-=x;
	msy-=y;
	
	if(title[0]=='!')
	{
		answer=ANSWER_NO;
		editMode=EDITMODE_EDIT;
		active=0;
		return 1;
	}
	if(PointInRect(msx,msy,6,52,96,74))
	{
		answer=ANSWER_YES;
		editMode=EDITMODE_EDIT;
		active=0;
	}
	if(PointInRect(msx,msy,105,52,195,74))
	{
		answer=ANSWER_NO;
		editMode=EDITMODE_EDIT;
		active=0;
	}
	// if the yesnomenu is active, NOTHING ELSE MAY BE CLICKED!
	return 1;
}

byte YesNoMenu::KeyPress(char c)
{
	if(!active)
		return 0;

	if(title[0]=='!')
	{
		if(c)
		{
			answer=ANSWER_NO;
			editMode=EDITMODE_EDIT;
			active=0;
			return 1;
		}
	}

	if(c==27 || c=='n')	// ESC or N, same as saying no
	{
		answer=ANSWER_NO;
		editMode=EDITMODE_EDIT;
		active=0;
		return 1;
	}
	if(c==13 || c=='y')	// Enter or Y... treat it as yes
	{
		answer=ANSWER_YES;
		editMode=EDITMODE_EDIT;
		active=0;
		return 1;
	}
	return 0;
}

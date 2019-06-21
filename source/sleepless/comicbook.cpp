#include "comicbook.h"
#include "display.h"
#include "player.h"
#include "progress.h"
#include "goal.h"

static byte pageNo,maxPage;

void ComicPage(void)
{
	char txt[64];

	sprintf(txt,"graphics/comic%d.bmp",pageNo);
	GetDisplayMGL()->LoadBMP(txt);
	PrintUnGlow(20,450,"Press arrows or left click to navigate, right click or ESC to exit",2);
	GetTaps();
	GetArrowTaps();
	GetDisplayMGL()->MouseTap();
	GetDisplayMGL()->RMouseTap();
	GetDisplayMGL()->LastKeyPressed();
}

void PageChg(char dir)
{
	pageNo+=dir;
	if(pageNo==255)
		pageNo=maxPage-1;
	if(pageNo>=maxPage)
		pageNo=0;
	ComicPage();
}

TASK(void) ComicBook(void)
{
	int i;
	char k;
	byte c;

	maxPage=0;
	for(i=0;i<NUM_GOALS;i++)
		if(profile.progress.goal[i])
			maxPage++;

	pageNo=0;
	ComicPage();

	while(GetDisplayMGL()->Process())
	{
		AWAIT GetDisplayMGL()->Flip();
		k=GetDisplayMGL()->LastKeyPressed();
		c=GetTaps()|GetArrowTaps();
		if(k==27)
			break;
		if(GetDisplayMGL()->RMouseTap())
			break;
		if(GetDisplayMGL()->MouseTap())
			PageChg(1);
		if(c&CONTROL_B2)
			break;
		if(c&(CONTROL_B1|CONTROL_RT))
			PageChg(1);
		if(c&(CONTROL_LF))
			PageChg(-1);
	}
	GetDisplayMGL()->ClearScreen();
	AWAIT GetDisplayMGL()->Flip();
}

#include "pause.h"
#include "player.h"
#include "options.h"
#include "music.h"

byte cursor=0;
static char lastKey=0;
static byte darkness;
static int offX;
static byte oldc,oldMsBtn;
static int oldMsx,oldMsy;
static sprite_set_t *blowSpr;

void RenderPauseMenu(MGLDraw *mgl)
{
	char s[48];

	if(!GM_doDraw)
		return;

	DarkenScreen(darkness);

	if(cursor!=0)
	{
		CenterPrintOutline(320,150,"Return To Game",0,0);
		CenterPrintColor(320,150,"Return To Game",4,-20,0);
	}
	else
		CenterPrintOutline(320,150,"Return To Game",0,0);

	if(cursor!=1)
	{
		CenterPrintOutline(320,180,"Retry Level",0,0);
		CenterPrintColor(320,180,"Retry Level",4,-20,0);
	}
	else
		CenterPrintOutline(320,180,"Retry Level",0,0);

	if(cursor!=2)
	{
		CenterPrintOutline(320,210,"Self Destruct",0,0);
		CenterPrintColor(320,210,"Self Destruct",4,-20,0);
	}
	else
		CenterPrintOutline(320,210,"Self Destruct",0,0);

	sprintf(s,"Song Playing: %s",player.songName);

	if(cursor!=3)
	{
		CenterPrintOutline(320,240,s,0,0);
		CenterPrintColor(320,240,s,4,-20,0);
	}
	else
		CenterPrintOutline(320,240,s,0,0);

	if(cursor!=4)
	{
		CenterPrintOutline(320,270,"Exit To Menu",0,0);
		CenterPrintColor(320,270,"Exit To Menu",4,-20,0);
	}
	else
		CenterPrintOutline(320,270,"Exit To Menu",0,0);

	blowSpr->GetSprite(9)->Draw(oldMsx,oldMsy,mgl);
}

void HandlePauseKeyPresses(MGLDraw *mgl)
{
	char k;

	k=mgl->LastKeyPressed();
	if(k)
		lastKey=k;
}

void InitPauseMenu(void)
{
	Music_Pause();
	MakeNormalSound(SND_PAUSE);
	lastKey=0;
	cursor=0;
	darkness=0;
	offX=400;
	oldc=255;
	oldMsBtn=1;
	GetDisplayMGL()->GetMouse(&oldMsx,&oldMsy);
	blowSpr=new sprite_set_t("graphics\\blowout.jsp");
}

void ExitPauseMenu(void)
{
	delete blowSpr;
}

byte UpdatePauseMenu(MGLDraw *mgl)
{
	byte c,sng;
	static byte reptCounter=0;
	int i,msx,msy;

	if(darkness<8)
		darkness++;
	if(offX>0)
	{
		offX-=32;
		if(offX<0)
			offX=0;
	}

	c=GetControls()|GetArrows();

	reptCounter++;
	if((!oldc) || (reptCounter>10))
		reptCounter=0;

	if((c&CONTROL_UP) && (!reptCounter))
	{
		cursor--;
		if(cursor==255)
			cursor=4;
	}
	if((c&CONTROL_DN) && (!reptCounter))
	{
		cursor++;
		if(cursor==5)
			cursor=0;
	}
	if(((c&CONTROL_B1) && (!(oldc&CONTROL_B1))) || 
	   ((c&CONTROL_B2) && (!(oldc&CONTROL_B2))))
	{
		switch(cursor)
		{
			case 0: // cancel
				Music_Play();
				SetPlayerNoBomb();
				return PAUSE_RETURN;
				break;
			case 1:	// retry level
				return PAUSE_RETRY;
				break;
			case 2:	// self destruct
				Music_Play();
				PlayerExplode(curMap);
				SetPlayerNoBomb();
				return PAUSE_RETURN;
				break;
			case 3:
				i=0;
				while(i++<300)
				{
					sng=SONG_PLAY+(byte)Random(5);
					if(profile.songChoice[sng][0]!='\0' && sng!=player.songNum &&
						Music_Load(profile.songChoice[sng]))
					{
						player.songNum=sng;
						Music_Play();
						break;
					}
				}
				break;
			case 4:	// quit
				return PAUSE_EXITGAME;
		}
	}

	mgl->GetMouse(&msx,&msy);
	if(msx!=oldMsx || msy!=oldMsy)
	{
		if(PointInRect(msx,msy,320-60,150,320+60,178))
			cursor=0;
		if(PointInRect(msx,msy,320-50,180,320+50,180+28))
			cursor=1;
		if(PointInRect(msx,msy,320-50,210,320+50,210+28))
			cursor=2;
		if(PointInRect(msx,msy,320-90,240,320+90,240+28))
			cursor=3;
		if(PointInRect(msx,msy,320-50,270,320+50,270+28))
			cursor=4;

		oldMsx=msx;
		oldMsy=msy;
	}
	if(mgl->MouseDown(0))
	{
		if(!oldMsBtn)
		{
			switch(cursor)
			{
				case 0: // cancel
					Music_Play();
					SetPlayerNoBomb();
					return PAUSE_RETURN;
					break;
				case 1:	// retry level
					return PAUSE_RETRY;
					break;
				case 2:	// self destruct
					Music_Play();
					PlayerExplode(curMap);
					SetPlayerNoBomb();
					return PAUSE_RETURN;
					break;
				case 3:
					i=0;
					while(i++<300)
					{
						sng=SONG_PLAY+(byte)Random(5);
						if(profile.songChoice[sng][0]!='\0' && sng!=player.songNum &&
							Music_Load(profile.songChoice[sng]))
						{
							player.songNum=sng;
							Music_Play();
							break;
						}
					}
					break;
				case 4:	// quit
					return PAUSE_EXITGAME;
			}
		}
		oldMsBtn=1;
	}
	else
		oldMsBtn=0;
	
	oldc=c;

	HandlePauseKeyPresses(mgl);
	if(lastKey==27)	// hit ESC to exit pause menu
	{
		Music_Play();
		return PAUSE_RETURN;
	}
	return PAUSE_PAUSING;
}

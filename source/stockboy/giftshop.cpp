#include "giftshop.h"
#include "profile.h"
#include "options.h"
#include "clock.h"
#include "game.h"
#include "title.h"
#include "sound.h"
#include "music.h"

#define GIFT_MINX	80
#define GIFT_MINY	80
#define GIFT_DX		100
#define GIFT_DY		100

static char giftName[][16]={
	"Level",
	"Editor",
	"Inventory",
	"Clearance",
	"Summer",
	"Blowout",
	"Pest",
	"Control",
	"War",
	"Pig",
	"Goth",
	"Girl",
	"Chunky",
	"Cherub",
	"Ro-Bot",
	"Machine",
	"Saint",
	"Nicky",
	"Shooper",
	"Shroom",
	"The Month",
	"Of Smarch",
	"Freako",
	"McFreak",
	"Music",
	"Composer",
	"Parallel",
	"Universe",
	"Happy",
	"Stick Boy",
	"Exit",
	""
};
static int giftCost[]={
	77,	// level editor
	43,	// inv. clearance
	54,	// super blowout
	92,	// pest control

	18,	// war pig
	17, // goth girl
	21, // cherub
	26, // robot

	23, // santa
	28,  // shroom
	78, // smarch
	34,	// freako

	144, // music composer
	237, // parallel
	107, // stick boy
	0,	 // exit
};

// this tells how many stars you can earn once you buy this thing
static int giftPayoff[]={
	0,	// level editor
	5,	// inv. clearance
	40,	// super blowout
	45,	// pest control

	0,	// war pig
	0, // goth girl
	0, // cherub
	0, // robot

	0, // santa
	0,  // shroom
	30, // smarch
	0,	// freako

	0, // music composer
	370, // parallel
	0, // stick boy
	0,	 // exit
};

static byte sprNum[]={
	1,		// lvl edit
	1,		// inv. clearance
	1,		// blowout
	1,		// pest control
	15,		// war pig
	3,		// goth girl
	5,		// cherub
	7,		// robot
	9,		// santa
	13,		// shroom
	1,		// smarch
	17,		// freako
	1,		// music
	1,		// parallel
	11,		// happy stick
	1,		// exit
};

static char giftMsg[][90]={
	"Create your own levels, and play those made by other people!",	// level editor
	"Use magic Acid to clear the screen of pesky Crates.",	// inv. clearance
	"Keep blowing up items that fall from the sky until they overwhelm you!", // blowout
	"Exterminate those pesky critters in many different scenarios.",	// pest control
	"Select this character in the profile menu to look pig-headed.",	// war pig
	"Select this character in the profile menu to look spooky and surly.",	// gothgirl
	"Select this character in the profile menu to look angelic and portly.",	// cherub
	"Select this character in the profile menu to look robotic and clanky.",	// robot
	"Select this character in the profile menu to look jolly.",					// santa
	"Select this character in the profile menu to look shooper shroomy.",		// shroom
	"Lousy Smarch weather... a new HIDDEN month due to a calendar misprint.",	// smarch
	"Select this character in the profile menu to look freaky weird.",		// freako
	"Compose your own background music to blow up barrels by!!", // music
	"Visit an eerie alternate version of the Stockroom!",				// parallel
	"The ultimate in secret characters.  DO NOT DEFY THE HAPPY STICK!",		// happy stick
	"Thank you, come again!",			// exit
};

#define GIFT_MODE	0
#define GIFT_CHAR	1
#define GIFT_SPECIAL 2
#define GIFT_HIDEDITOR	3

static byte giftType[]={
	GIFT_HIDEDITOR,GIFT_MODE,GIFT_MODE,GIFT_MODE,
	GIFT_CHAR,GIFT_CHAR,GIFT_CHAR,GIFT_CHAR,
	GIFT_CHAR,GIFT_CHAR,GIFT_SPECIAL,GIFT_CHAR,
	GIFT_SPECIAL,GIFT_MODE,GIFT_CHAR,GIFT_MODE,
};

static byte inStock[16];

static byte cursor;
static byte oldc;
static dword oldBtn;
static sprite_set_t *giftSpr;
static byte oldMouseBtn;
static byte *backScr;
static int oldmsx,oldmsy;
static int msx,msy;
static int startx,starty;
static int starx,stary;
static int stardx,stardy;
static byte starAng;

void InitGiftShop(void)
{
	int i;

	JamulSoundPurge();	// each level, that should be good
	Music_Load(profile.songChoice[SONG_MENU]);
	Music_Play();
	oldc=255;
	cursor=0;
	oldMouseBtn=1;

	LoadOptions();

	giftSpr=new sprite_set_t("graphics\\giftshop.jsp");

	backScr=(byte *)malloc(640*480);
	if(!backScr)
		FatalError("Out of memory!");

	GetDisplayMGL()->LoadBMP("graphics\\giftshop.bmp");

	for(i=0;i<480;i++)
		memcpy(&backScr[i*640],GetDisplayMGL()->GetScreen()+GetDisplayMGL()->GetWidth()*i,640);

	starx=320*FIXAMT;
	stary=240*FIXAMT;
	starAng=0;
	stardx=0;
	stardy=0;
	startx=GIFT_MINX*FIXAMT;
	starty=GIFT_MINY*FIXAMT;

	profile.giftShop[15]=1;	// you can always exit the gift shop, duh
	GetDisplayMGL()->GetMouse(&oldmsx,&oldmsy);
}

void ExitGiftShop(void)
{
	SaveOptions();
	ResetClock(0);
	free(backScr);
	delete giftSpr;
}

void CalcStock(void)
{
	int starPotential;
	int i,j,remain;
	byte avail[16],items,best;

	// smarch and parallel interact to determine their value
	if(profile.giftShop[GIFT_PARALLEL])
		giftPayoff[GIFT_SMARCH]=60;
	else
		giftPayoff[GIFT_SMARCH]=30;
	if(profile.giftShop[GIFT_SMARCH])
		giftPayoff[GIFT_PARALLEL]=400;
	else
		giftPayoff[GIFT_PARALLEL]=370;

	// calculate how many stars the player could possibly have right now
	starPotential=479; // 479 is the amount you can earn without buying anything
	for(i=0;i<15;i++)
		if(profile.giftShop[i])
		{
			inStock[i]=1;
			starPotential+=giftPayoff[i]-giftCost[i];
		}
	inStock[15]=1;	// exit is always in stock!

	for(i=0;i<15;i++)
	{
		if(!profile.giftShop[i])	// only checking things you don't own
		{
			// remain is how much you'd be left if you bought it
			remain=starPotential-giftCost[i]+giftPayoff[i];
			// now we make a list of which items would remain to buy after buying that
			// only counting ones that give more stars
			items=0;
			for(j=0;j<15;j++)
			{
				if(j==i)
					avail[j]=0;
				else if(profile.giftShop[j])
					avail[j]=0;
				else if(giftPayoff[j]==0)
					avail[j]=0;
				else
				{
					items++;
					avail[j]=1;
				}
			}
			while(items>0)
			{
				best=255;
				// find which of the remaining items is the most profitable purchase
				// (cheapest, counting the payoff)
				for(j=0;j<15;j++)
				{
					if(avail[j])
					{
						if(best==255 || (giftPayoff[j]-giftCost[j])>(giftPayoff[best]-giftCost[best]))
							best=j;
					}
				}
				// 'buy' that item, and then loop around and see if you could buy the rest
				if(best!=255)
				{
					if(remain>=giftCost[best])
					{
						remain+=giftPayoff[best]-giftCost[best];
						items--;
						avail[best]=0;
					}
					else
						break;
				}
			}
			if(items>0)	// couldn't afford everything
				inStock[i]=0;
			else
				inStock[i]=1;
		}
	}
}

void UpdateGiftStars(void)
{
	starx+=stardx;
	stary+=stardy;

	if(starx/FIXAMT!=startx/FIXAMT)
	{
		if(stardx>0 && starx>startx)
		{
			stardx=0;
			starx=startx;
		}
		else if(stardx<0 && starx<startx)
		{
			stardx=0;
			starx=startx;
		}
		else
			stardx=(startx-starx)/8;
	}
	else
		stardx=0;

	if(stary/FIXAMT!=starty/FIXAMT)
	{
		if(stardy>0 && stary>starty)
		{
			stardy=0;
			stary=starty;
		}
		else if(stardy<0 && stary<starty)
		{
			stardy=0;
			stary=starty;
		}
		else
			stardy=(starty-stary)/8;
	}
	else
		stardy=0;

	starAng+=4;
}

void SetGiftStars(void)
{
	startx=(GIFT_MINX+(cursor%4)*GIFT_DX)*FIXAMT;
	starty=(GIFT_MINY+(cursor/4)*GIFT_DY)*FIXAMT;
	stardx=0;
	stardy=0;
}

void BuyItem(void)
{
	if(profile.giftShop[cursor] || profile.starsLeft<giftCost[cursor] || !inStock[cursor])
	{
		MakeNormalSound(SND_MENUCANCEL);
		return;	// can't buy what you already have, or buy what you can't afford
	}
	else
	{
		MakeNormalSound(SND_MENUSELECT);
		profile.starsLeft-=giftCost[cursor];
		profile.giftShop[cursor]=1;
		SaveProfile(opt.curProfile);
		CalcStock();
	}
}

static byte GiftShopMouseCheck(MGLDraw *mgl)
{
	int i,j;
	byte b;
	byte oldcursor;

	mgl->GetMouse(&msx,&msy);
	b=mgl->MouseDown();

	oldcursor=cursor;

	if(msx!=oldmsx || msy!=oldmsy)
	{
		for(i=0;i<4;i++)
		{
			for(j=0;j<4;j++)
			{
				if(PointInRect(msx,msy,GIFT_MINX+GIFT_DX*i-35,
									   GIFT_MINY+GIFT_DY*j-35,
									   GIFT_MINX+GIFT_DX*i+35,
									   GIFT_MINY+GIFT_DY*j+35))
				{
					cursor=i+j*4;
					if(cursor!=oldcursor)
						SetGiftStars();
				}
			}
		}
	}
	if(!b)	// not clicking
	{
		oldMouseBtn=b;
		return 0;
	}
	else	// mouse is down!
	{
		if(!oldMouseBtn) // only taps allowed here
		{
			for(i=0;i<4;i++)
			{
				for(j=0;j<4;j++)
				{
					if(PointInRect(msx,msy,GIFT_MINX+GIFT_DX*i-35,
										   GIFT_MINY+GIFT_DY*j-35,
										   GIFT_MINX+GIFT_DX*i+35,
										   GIFT_MINY+GIFT_DY*j+35))
					{
						cursor=i+j*4;
						if(cursor!=oldcursor)
							SetGiftStars();
						if(cursor!=15)
						{
							BuyItem();
							oldMouseBtn=b;
							return 0;
						}
						else
						{
							oldMouseBtn=b;
							return 1;
						}
					}
				}
			}
		}
	}

	oldMouseBtn=b;
	return 0;
}

byte UpdateGiftShop(int *lastTime,MGLDraw *mgl)
{
	char c;
	byte c2;

	JamulSoundUpdate();

	if(*lastTime>TIME_PER_FRAME*MAX_RUNS)
		*lastTime=TIME_PER_FRAME*MAX_RUNS;
	while(*lastTime>=TIME_PER_FRAME)
	{
		Music_Update();

		UpdateGiftStars();
		if(GiftShopMouseCheck(mgl))
			return 1;

		c=mgl->LastKeyPressed();
		c2=GetArrows()|GetControls();

		if((c2&CONTROL_UP) && !(oldc&CONTROL_UP))
		{
			cursor-=4;
			if(cursor>15)
				cursor+=16;
			SetGiftStars();
		}
		if((c2&CONTROL_DN) && !(oldc&CONTROL_DN))
		{
			cursor+=4;
			if(cursor>15)
				cursor-=16;
			SetGiftStars();
		}
		if((c2&CONTROL_LF) && !(oldc&CONTROL_LF))
		{
			cursor--;
			if((cursor%4)==3)
				cursor+=4;
			SetGiftStars();
		}
		if((c2&CONTROL_RT) && !(oldc&CONTROL_RT))
		{
			cursor++;
			if((cursor%4)==0)
				cursor-=4;
			SetGiftStars();
		}
		if((c2&CONTROL_B1) && !(oldc&CONTROL_B1))
		{
			if(cursor!=15)
				BuyItem();
			else
				return 1;
		}

		if(c==27)
		{
			return 1;
		}

		oldmsx=msx;
		oldmsy=msy;
		oldc=c2;

		*lastTime-=TIME_PER_FRAME;
	}

	return 0;
}

void RenderProfileStars2(int x,int y,profile_t *prof,MGLDraw *mgl)
{
	char s[10];

	RenderStar(0,5,x*FIXAMT,y*FIXAMT,mgl);
	sprintf(s,"%03d/%03d",prof->starsLeft,prof->stars);
	PrintUnGlow(x+25,y-8,s,0);
}

void RenderGiftShop(MGLDraw *mgl)
{
	int i;
	int msx,msy;
	char typeName[][16]={
		"New Game",
		"Mode",
		"New",
		"Character",
		"Special",
		"Feature",
		"Level",
		"Editor",
		"Out Of",
		"Stock!",
	};
	char s[10];

	Music_Update();

	/*
	if(!GM_doDraw)
		return;
	*/

	for(i=0;i<480;i++)
		memcpy(mgl->GetScreen()+mgl->GetWidth()*i,&backScr[i*640],640);

	msx=GIFT_MINX;
	msy=GIFT_MINY;

	for(i=0;i<16;i++)
	{
		if(!inStock[i])
		{
			giftSpr->GetSprite(0+sprNum[i])->Draw(msx,msy,mgl);
			CenterPrintOutline(msx,msy-20,typeName[4*2],0,0);
			CenterPrintOutline(msx,msy+10,typeName[4*2+1],0,0);
		}
		else if(profile.giftShop[i])
		{
			giftSpr->GetSprite(1+sprNum[i])->Draw(msx,msy,mgl);
			CenterPrintOutline(msx,msy-20,giftName[i*2],0,0);
			CenterPrintOutline(msx,msy+10,giftName[i*2+1],0,0);
		}
		else
		{
			giftSpr->GetSprite(0+sprNum[i])->Draw(msx,msy,mgl);
			CenterPrintOutline(msx,msy-30,typeName[giftType[i]*2],0,0);
			CenterPrintOutline(msx,msy-10,typeName[giftType[i]*2+1],0,0);
			RenderStar(0,5,(msx-20)*FIXAMT,(msy+30)*FIXAMT,mgl);
			sprintf(s,"%03d",giftCost[i]);
			PrintOutline(msx,msy+20,s,0,0);
		}
		msx+=GIFT_DX;
		if(msx>GIFT_MINX+GIFT_DX*3)
		{
			msx=GIFT_MINX;
			msy+=GIFT_DY;
		}
	}

	CenterPrintOutline(560,240,"You have",0,0);
	RenderStar(0,5,(530)*FIXAMT,(280)*FIXAMT,mgl);
	sprintf(s,"%03d",profile.starsLeft);
	PrintOutline(550,270,s,0,0);
	CenterPrintOutline(560,300,"to spend.",0,0);

	if(profile.giftShop[cursor])
		PrintOutline(50,460,giftMsg[cursor],0,0);

	for(i=0;i<6;i++)
	{
		msx=starx+Cosine(starAng+i*(256/6))*40;
		msy=stary+Sine(starAng+i*(256/6))*40;
		RenderStar(5,4,msx,msy,mgl);
	}

	giftSpr->GetSprite(0)->Draw(oldmsx,oldmsy,mgl);
}

void GiftShop(MGLDraw *mgl)
{
	byte done=0;
	int lastTime;

	InitGiftShop();
	CalcStock();
	lastTime=1;

	while(!done)
	{
		lastTime+=TimeLength();
		StartClock();
		done=UpdateGiftShop(&lastTime,mgl);
		RenderGiftShop(mgl);
		mgl->Flip();
		EndClock();

		if(!mgl->Process())
			done=1;

	}

	ExitGiftShop();
}

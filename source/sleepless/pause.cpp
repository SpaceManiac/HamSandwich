#include "pause.h"
#include "player.h"
#include "progress.h"
#include "editor.h"
#include "intface.h"
#include "shop.h"
#include "dialogbits.h"
#include "journal.h"
#include "customworld.h"

#define PE_CONTINUE	0	// back to gameplay
#define PE_RETRY	1	// retry this level
#define PE_GIVEUP	2	// give up, go back to hub
#define PE_WORLDSEL 3	// world select screen
#define PE_EXIT		4	// exit entirely, to main menu or editor, wherever you came from
#define PE_SNDVOL	5	// sound volume adjust
#define PE_MUSVOL	6	// music volume adjust
#define PE_MUSIC	7	// music options menu
#define PE_CHEAT	8	// go to the cheat menu
#define PE_DONE		9	// a marker to indicate the list is done
#define PE_MORECHEATS 10	// next page of cheats
#define PE_MUSICMODE 11	// random/sequential/repeat
#define PE_SONG		12	// which song is playing
#define PE_MUSICTYPE 13 // music type adjust (which playlist, or official music)
#define PE_PREVCHEATS 14
#define PE_BZZT		15		// an invalid option
#define PE_SHOP		16	// go shopping, go back to playing
#define PE_WEAPON	17	// change hammer
#define PE_JOURNAL  18

#define PE_CHEATS	50

#define INV_Y	(300)

typedef struct pauseItem_t
{
	byte effect;
	char text[32];
} pauseItem_t;

static pauseItem_t gamePause[]={
	{PE_CONTINUE,"Continue"},
	{PE_WEAPON,"Hammer: None"},
	{PE_RETRY,"Retry"},
	{PE_JOURNAL,"< Journal >"},
	{PE_SNDVOL,""},
	{PE_MUSVOL,""},
	{PE_EXIT,"Save & Exit"},
	{PE_DONE,""}
};

static pauseItem_t shopPause[]={
	{PE_CONTINUE,"Continue"},
	{PE_SNDVOL,""},
	{PE_MUSIC,"Music Options"},
	{PE_SHOP,"Quit & Play"},
	{PE_EXIT,"Main Menu"},
	{PE_DONE,""}
};

static pauseItem_t editPause[]={
	{PE_CONTINUE,"Continue"},
	{PE_WEAPON,"Hammer: None"},
	{PE_RETRY,"Retry"},
	{PE_JOURNAL,"< Journal >"},
	{PE_SNDVOL,""},
	{PE_MUSVOL,""},
	{PE_CHEAT,"Cheats!!"},
	{PE_GIVEUP,"Reset Level"},
	{PE_EXIT,"Editor"},
	{PE_DONE,""}
};

static pauseItem_t gameCheatPause[]={
	{PE_CONTINUE,"Continue"},
	{PE_RETRY,"Retry"},
	{PE_GIVEUP,"Give Up"},
	{PE_WORLDSEL,"World Select"},
	{PE_SNDVOL,""},
	{PE_MUSIC,"Music Options"},
	{PE_CHEAT,"Cheats!!"},
	{PE_SHOP,"Quit & Shop"},
	{PE_EXIT,"Exit Game"},
	{PE_DONE,""}
};

static pauseItem_t cheatPause[]={
	{PE_CHEAT,"Stop Cheating!"},
	{PE_CHEATS+CHEAT_WINLEVEL,"Win Level"},
	{PE_CHEATS+CHEAT_HAMMERUP,"UltraHammerUp"},
	{PE_CHEATS+CHEAT_LIFE,"Max Health"},
	{PE_CHEATS+CHEAT_RAGE,"Max Rage"},
	{PE_CHEATS+CHEAT_SHIELD,"Energy Barrier"},
	{PE_CHEATS+CHEAT_BOOM,"KABLOOIE!"},
	{PE_CHEATS+CHEAT_FREEZE,"Freeze Enemies"},
	{PE_CHEATS+CHEAT_BRAINS,"All Brains"},
	{PE_CHEATS+CHEAT_KEYS,"All Keys"},
	{PE_MORECHEATS,"More Cheats"},
	{PE_DONE,""},
};

static pauseItem_t cheatPause2[]={
	{PE_PREVCHEATS,"Prev. Cheats"},
	{PE_CHEATS+CHEAT_SCANNER,"Free Scanner"},
	{PE_CHEATS+CHEAT_AMMO,"Ammo Crate"},
	{PE_CHEATS+CHEAT_LIGHT,"Lights On"},
	{PE_CHEATS+CHEAT_WATER,"Walk On Water"},
	{PE_CHEATS+CHEAT_OXYGEN,"Infinite Oxygen"},
	{PE_CHEATS+CHEAT_NOSKID,"No-Skid Boots"},
	{PE_CHEATS+CHEAT_SPEED,"Super Speed"},
	{PE_CHEAT,"Stop Cheating!"},
	{PE_DONE,""},
};

static pauseItem_t musicPause[]={
	{PE_MUSVOL,""},
	{PE_MUSICTYPE,""},
	{PE_MUSICMODE,""},
	{PE_SONG,""},
	{PE_MUSIC,"Back To Menu"},
	{PE_DONE,""},
};

static invItem_t inventory[]={
	{0,ITM_PANTS,INVT_PANTS,0,10,"Pants Of Power","Throw hammers faster."},
	{1,ITM_HAMMERUP,INVT_HAMMERS,0,10,"Hammers","Charge up the multi-shot faster."},
	{2,153,INVT_MYSTIC,0,10,"Flamebringer","Sets monsters on fire!"},
	{3,162,INVT_MYSTIC,1,10,"Lightreaver","Reflects shots and heals!"},
	{4,163,INVT_MYSTIC,2,10,"Planetsmasher","Creates black holes and portals!"},
	{5,169,INVT_MYSTIC,3,10,"Sparkthrower","Spews monster-seeking sparks!"},
	{6,170,INVT_MYSTIC,4,10,"Earsplitter","Flattens monsters through walls!"},
	{7,171,INVT_MYSTIC,5,10,"Bonecrusher","Summons and controls a skeleton!"},
	{8,ITM_BRAIN,INVT_BRAINS,0,100,"Zombie Brains","Collect them for some reason."},
	{9,ITM_CANDLE,INVT_CANDLES,0,100,"Candles","Might have a use?"},
	{10,150,INVT_JOURNAL,0,NUM_PAGES-1,"Journal Pages","Learn more of the legend."},
	{11,178,INVT_HFLAGS,HMR_BIONIC,0,"Bionic Arm","Hammers fly faster.",2,0},
	{12,177,INVT_HFLAGS,HMR_BLAST,0,"Blasting Cap","10% chance of hammers exploding."},
	{13,ITM_REVERSE,INVT_HFLAGS,HMR_REVERSE,0,"Reverse Hammer","25% chance to throw hammers backwards."},
	{14,ITM_REFLECT,INVT_HFLAGS,HMR_REFLECT,0,"Sproingy Spring","Hammers have 25% chance to bounce."},
	{15,176,INVT_ABILITY,ABIL_SHIELD,0,"Solid Shield","10% chance of blocking damage."},
	{16,180,INVT_ABILITY,ABIL_TRAINING,0,"Training Guide","25% chance of earning double XP."},
	{17,174,INVT_ABILITY,ABIL_SOLAR,0,"Solar Collector","Gain 1 Energy per second."},
	{18,175,INVT_ABILITY,ABIL_FIRSTAID,0,"First Aid Kit","Heal 1 Life per 3 seconds."},
	{19,173,INVT_ABILITY,ABIL_BRAIN,0,"Brain Detector","Stand still to scan the area for brains."},
	{20,ITM_KEYR,INVT_KEYS,1,0,"Red Key","Opens red doors."},
	{21,ITM_KEYG,INVT_KEYS,2,0,"Green Key","Opens green doors."},
	{22,ITM_KEYB,INVT_KEYS,3,0,"Blue Key","Opens blue doors."},
	{23,ITM_KEY,INVT_KEYS,0,3,"Yellow Key","Opens yellow doors. Breaks when used."},
	{31,186,INVT_ABILITY,ABIL_FISH,0,"Electroreel","Electrofish with the Classic Red Hammer.",0,-8},
	{255,0,0,0,0,"",""}
};

byte cursor=0;
static char lastKey=0;
static byte menuMode;
static int pauseX=640,pauseY=480;
static byte numItems;
static pauseItem_t menu[15];
static int msx,msy;
static char msBright,msDBright;
static byte oldc=255;
static byte *backgd=NULL;

byte volumeSpot[]={0,26,51,77,102,128,153,179,204,230,255};

void RenderPauseButton(byte b,int x,int y,int wid,char *txt,MGLDraw *mgl)
{
	PrintUnGlow(x+2,y+2,txt,2);
	if(cursor==b)
	{
		PrintUnGlow(x+1,y+1,txt,2);
		PrintUnGlow(x+3,y+3,txt,2);
		PrintUnGlow(x+1,y+3,txt,2);
		PrintUnGlow(x+3,y+1,txt,2);
		PrintGlow(x+2,y+2,txt,0,2);
	}

}

void RenderUnpaused(void)
{
	if(pauseY<480)
		RenderPauseMenu();
}

void PrintPauseVal(const char *txt,int v,int max,int x,int y)
{
	char s[32];
	int len;

	PrintUnGlow(pauseX+x,pauseY+y,txt,1);
	if(max==-1)
		strcpy(s,"MAX!");
	else if(max==0)
	{
		sprintf(s,"%d",v);

	}
	else
		sprintf(s,"%d/%d",v,max);
	len=GetStrLength(s,1);
	PrintUnGlow(pauseX+x+140-len,pauseY+y,s,1);
}

void PrintInv(int amt,int max,int x,int y)
{
	char s[10];

	sprintf(s,"%d",amt);
	x-=GetStrLength(s,2)/2;
	PrintUnGlow(pauseX+x-1,pauseY+y+1,s,2);
	PrintUnGlow(pauseX+x-1,pauseY+y-1,s,2);
	PrintUnGlow(pauseX+x+1,pauseY+y-1,s,2);
	PrintUnGlow(pauseX+x+1,pauseY+y+1,s,2);
	PrintGlow(pauseX+x,pauseY+y,s,0,2);
}

void BlackOutlineItem(byte item,int x,int y)
{
	InstaRenderItem(x-2,y-2,item,-32,GetDisplayMGL());
	InstaRenderItem(x+2,y-2,item,-32,GetDisplayMGL());
	InstaRenderItem(x-2,y+2,item,-32,GetDisplayMGL());
	InstaRenderItem(x+2,y+2,item,-32,GetDisplayMGL());
}

void WhiteOutlineItem(byte item,int x,int y)
{
	InstaRenderItem(x-1,y-1,item,32,GetDisplayMGL());
	InstaRenderItem(x+1,y-1,item,32,GetDisplayMGL());
	InstaRenderItem(x-1,y+1,item,32,GetDisplayMGL());
	InstaRenderItem(x+1,y+1,item,32,GetDisplayMGL());
}

void InvItem(byte item,int x,int y,int amt,int max,char brAdjust,char *name,char *desc)
{

	if(amt>0)
	{
		BlackOutlineItem(item,x,y);

		if(amt>=max)
			WhiteOutlineItem(item,x,y);
		InstaRenderItem(x,y,item,brAdjust,GetDisplayMGL());
		if(max>0)
			PrintInv(amt,max,x+12,y-8);
		if(msx>=x-15 && msx<=x+15 && msy>=y-20 && msy<y+10)
		{
			// this item is hovered
			PrintUnGlow(5,440,name,1);
			PrintUnGlow(5,460,desc,1);
		}
	}
	else
		InstaRenderItem(x,y,item,-32,GetDisplayMGL());
}

void RenderPauseMenu(void)
{
	int i,jcount;
	byte pnts;

	if(pauseY>=480)
		return;

	if(backgd==NULL)
	{
		GetDisplayMGL()->LoadBMP("graphics/pause.bmp");
		backgd=(byte *)malloc(640*480);
		if(!backgd)
			FatalError("Out of memory!");

		for(i=0;i<480;i++)
			memcpy(&backgd[i*640],&GetDisplayMGL()->GetScreen()[i*GetDisplayMGL()->GetWidth()],640);
	}

	for(i=0;i<480;i++)
	{
		if(pauseY+i>=0 && pauseY+i<480)
			memcpy(&GetDisplayMGL()->GetScreen()[(pauseY+i)*GetDisplayMGL()->GetWidth()],&backgd[i*640],640);
	}

	ShowJournalPage(pauseX,pauseY,player.jpage);
	for(i=0;i<numItems;i++)
	{
		RenderPauseButton(i,pauseX+10,pauseY+4+24*i,125,menu[i].text,GetDisplayMGL());
	}

	jcount=0;
	for(i=1;i<NUM_PAGES;i++)
		if(player.journal[i])
			jcount++;

	i=16;
	pnts=0;
	while(i>(int)player.hamSpeed)
	{
		pnts++;
		if(i>4)
			i-=2;
		else if(i>0)
			i--;
	}

	// inventory items
	/*
	InvItem(ITM_PANTS,24,pauseY+INV_Y,pnts,10,0,"Pants Of Power","Throw hammers faster.");
	InvItem(ITM_HAMMERUP,24+38,pauseY+INV_Y,player.hammers,10,-5+15*(player.weapon==0),"Hammers","Charge up the multi-shot faster.");

	byte weaponItmNum[]={153,162,163,169,170,171};

	for(i=0;i<6;i++)
	{
		InvItem(weaponItmNum[i],24+38*(i+2),pauseY+INV_Y,player.weaponLvl[i],10,-5+(player.weapon==i+1)*15,WeaponName(i+1),WeaponDesc(i+1));
	}

	InvItem(ITM_BRAIN,24,pauseY+INV_Y+50,player.brains,100,0,"Zombie Brains","Collect them for some reason.");
	InvItem(ITM_CANDLE,24+38,pauseY+INV_Y+50,player.candles,100,0,"Candles","Might have a use?");
	InvItem(150,24+38*2,pauseY+INV_Y+50,jcount,NUM_PAGES-1,0,"Journal Pages","Learn more of the legend.");	// journal

	InvItem(178,24+38*3+4,pauseY+INV_Y+50,(player.hammerFlags&HMR_BIONIC)>0,0,0,"Bionic Arm","Hammers fly faster.");	// bionic arm
	InvItem(177,24+38*4,pauseY+INV_Y+50,(player.hammerFlags&HMR_BLAST)>0,0,0,"Blasting Cap","10% chance of hammers exploding.");	// blasting cap
	InvItem(ITM_REVERSE,24+38*5,pauseY+INV_Y+50,(player.hammerFlags&HMR_REVERSE)>0,0,0,"Reverse Hammer","25% chance to throw hammers backwards.");	// reverse
	InvItem(ITM_REFLECT,24+38*6,pauseY+INV_Y+50,(player.hammerFlags&HMR_REFLECT)>0,0,0,"Sproingy Spring","Hammers have 25% chance to bounce.");	// reflect
	InvItem(176,24+38*7,pauseY+INV_Y+50,(player.ability[ABIL_SHIELD])>0,0,0,"Solid Shield","10% chance of blocking damage.");	// shield
	InvItem(180,24+38*0,pauseY+INV_Y+50*2,(player.ability[ABIL_TRAINING])>0,0,0,"Training Guide","25% chance of earning double XP.");	// training guide
	InvItem(174,24+38*1,pauseY+INV_Y+50*2,(player.ability[ABIL_SOLAR])>0,0,0,"Solar Collector","Gain 1 Energy per second.");	// solar collector
	InvItem(175,24+38*2,pauseY+INV_Y+50*2,(player.ability[ABIL_FIRSTAID])>0,0,0,"First Aid Kit","Heal 1 Life per 3 seconds.");	// first aid
	InvItem(173,24+38*3,pauseY+INV_Y+50*2,(player.ability[ABIL_BRAIN])>0,0,0,"Brain Detector","Stand still to scan the area for brains.");	// brain detector
	InvItem(ITM_KEYR,24+38*4,pauseY+INV_Y+50*2,(player.keys[1]>0),0,0,"Red Key","Opens red doors.");
	InvItem(ITM_KEYG,24+38*5,pauseY+INV_Y+50*2,(player.keys[2]>0),0,0,"Green Key","Opens green doors.");
	InvItem(ITM_KEYB,24+38*6,pauseY+INV_Y+50*2,(player.keys[3]>0),0,0,"Blue Key","Opens blue doors.");
	InvItem(ITM_KEY,24+38*7,pauseY+INV_Y+50*2,player.keys[0],3,0,"Yellow Key","Opens yellow doors. Breaks when used.");
	InvItem(186,24+38*7,pauseY+INV_Y+50*3-8,(player.ability[ABIL_FISH]>0),0,0,"Electroreel","Electrofish with the Classic Red Hammer.");
	*/
	invItem_t* items = IsCustomWorld() && CustomInventory() ? CustomInventory() : inventory;
	for(i=0; items[i].position!=255 && i<64; ++i)
	{
		int x = 24 + 38 * (items[i].position % 8) + items[i].dx;
		int y = pauseY + INV_Y + 50 * (items[i].position / 8) + items[i].dy;
		int amount = 0, bright = 0, subtype = items[i].subtype;

		switch(items[i].type)
		{
			case INVT_PANTS:
				amount = pnts;
				break;
			case INVT_HAMMERS:
				amount = player.hammers;
				bright = -5+(player.weapon==0)*15;
				break;
			case INVT_MYSTIC:
				amount = player.weaponLvl[subtype];
				bright = -5+(player.weapon==subtype+1)*15;
				break;
			case INVT_BRAINS:
				amount = player.brains;
				break;
			case INVT_CANDLES:
				amount = player.candles;
				break;
			case INVT_JOURNAL:
				if (subtype == 0)
					amount = jcount;
				else
					amount = player.journal[subtype];
				break;
			case INVT_HFLAGS:
				amount = (player.hammerFlags & subtype) > 0;
				break;
			case INVT_ABILITY:
				amount = (player.ability[subtype]) > 0;
				break;
			case INVT_KEYS:
				amount = player.keys[subtype];
				break;
			case INVT_LVAR:
				amount = player.var[subtype];
				break;
			case INVT_GVAR:
				amount = GetWorldProgress(profile.lastWorld)->var[subtype];
				break;
		}

		InvItem(items[i].item, x, y, amount, items[i].max, bright, items[i].name, items[i].desc);
	}

	if(player.level==20)
		PrintPauseVal("XP",-1,-1,2,205);
	else
		PrintPauseVal("XP",player.xp,player.needXP,2,205);
	PrintPauseVal("Level",player.level,0,165,205);
	PrintPauseVal("Life",player.life,PlayerMaxLife(),2,225);
	PrintPauseVal("Energy",player.ammo,100,165,225);

	if(player.weapon>0)
	{
		if(player.weaponLvl[player.weapon-1]==10)
			PrintPauseVal("Mystic XP",-1,-1,2,245);
		else
			PrintPauseVal("Mystic XP",player.wpnXP[player.weapon-1],NeedWeaponXP(player.weapon),2,245);
		PrintPauseVal("Mystic Lvl.",player.weaponLvl[player.weapon-1],0,165,245);
	}
	if(player.ability[ABIL_FISH])
		PrintPauseVal("Fishing",player.journal[55],100,2,265);

	// mouse cursor

	if(pauseY<480)
		DrawMouseCursor(msx,msy);
}

void HandlePauseKeyPresses(MGLDraw *mgl)
{
	char k;

	k=mgl->LastKeyPressed();
	if(k)
		lastKey=k;
}

void SetupSoundItems(void)
{
	int i;

	for(i=0;i<numItems;i++)
	{
		switch(menu[i].effect)
		{
			case PE_SNDVOL:
				sprintf(menu[i].text,"Sound: %d%%",(profile.sound*100/255));
				break;
			case PE_MUSVOL:
				sprintf(menu[i].text,"Music: %d%%",(profile.music*100/255));
				break;
			case PE_WEAPON:
				if(player.weapon>0)
					sprintf(menu[i].text,"Hammer: %s",WeaponName(player.weapon));
				else if(player.hammers==0)
					sprintf(menu[i].text,"Hammer: None");
				else
					sprintf(menu[i].text,"Hammer: Classic Red");
				ColorizeBouapha();
				break;
		}
	}
}

void FillPauseMenu(pauseItem_t *src)
{
	int i;

	for(i=0;i<15;i++)
	{
		if(src[i].effect==PE_DONE)
		{
			numItems=i;
			break;
		}
		else
		{
			memcpy(&menu[i],&src[i],sizeof(pauseItem_t));
			if(menu[i].effect>=PE_CHEATS)
			{
				// squigglize cheats that aren't owned
				if(!ItemPurchased(SHOP_CHEAT,menu[i].effect-PE_CHEATS) && !editing)
				{
					strcpy(menu[i].text,"????????");
					menu[i].effect=PE_BZZT;
				}
			}
		}
	}
	SetupSoundItems();
}

void InitPauseMenu(void)
{
	int i;
	lastKey=0;
	cursor=0;
	oldc=255;

	if(!editing)
	{
		if(shopping)
			FillPauseMenu(shopPause);
		//else if(ItemPurchased(SHOP_MAJOR,MAJOR_CHEATMENU))
		//	FillPauseMenu(gameCheatPause);
		else
			FillPauseMenu(gamePause);

	}
	else
	{
		FillPauseMenu(editPause);
	}

	MakeNormalSound(SND_PAUSE);
	pauseX=0;
	pauseY=480;
	menuMode=0;
	msBright=0;
	msDBright=1;
	GetDisplayMGL()->MouseTap();

	if(backgd==NULL)
	{
		GetDisplayMGL()->LoadBMP("graphics/pause.bmp");
		backgd=(byte *)malloc(640*480);
		if(!backgd)
			FatalError("Out of memory!");

		for(i=0;i<480;i++)
			memcpy(&backgd[i*640],&GetDisplayMGL()->GetScreen()[i*GetDisplayMGL()->GetWidth()],640);
		GetDisplayMGL()->ClearScreen();
	}
}

void ExitPauseMenu(void)
{
	pauseY=480;
	if(backgd!=NULL)
	{
		free(backgd);
		backgd=NULL;
	}
}

void UpdateUnpaused(void)
{
	//if(pauseX<640)
	//	pauseX+=15;
	if(pauseY<480)
		pauseY+=20;
}

byte NextVolumeSpot(byte v)
{
	int i;

	for(i=0;i<10;i++)
	{
		if(v==volumeSpot[i])
			return volumeSpot[i+1];
	}
	if(v==volumeSpot[10])
		return volumeSpot[0];

	return 0;
}
byte PrevVolumeSpot(byte v)
{
	int i;

	for(i=1;i<11;i++)
	{
		if(v==volumeSpot[i])
			return volumeSpot[i-1];
	}
	if(v==volumeSpot[0])
		return volumeSpot[10];
	return 0;
}

void ChangeWeapon(char dir)
{
	byte prev;

	prev=player.weapon;

	while(1)
	{
		player.weapon+=dir;
		if(player.weapon>6)
		{
			if(dir<0)
				player.weapon=6;
			else
				player.weapon=0;
		}
		if(player.weapon==prev)
			return;
		if(player.weapon==0)
			return;
		if(player.weaponLvl[player.weapon-1]>0)
			return;
	}
}

byte UpdatePauseMenu(MGLDraw *mgl)
{
	byte c;
	int i;
	static byte reptCounter=0;
	byte msOkay;

	mgl->GetMouse(&msx,&msy);

	msBright+=msDBright;
	if(msBright>10)
		msDBright=-1;
	if(msBright<-2)
		msDBright=1;

	pauseX=0;
	if(pauseY>0)
	{
		pauseY-=48;
		if(pauseY<0)
			pauseY=0;
	}


	msOkay=0;
	for(i=0;i<numItems;i++)
	{
		if(PointInRect(msx,msy,pauseX+10,pauseY+4+24*i,pauseX+10+125,pauseY+4+24*i+23))
		{
			cursor=i;
			msOkay=1;
		}
	}

	c=GetControls()|GetArrows();

	reptCounter++;
	if((!oldc) || (reptCounter>10))
		reptCounter=0;

	if((c&CONTROL_UP) && (!reptCounter))
	{
		cursor--;
		if(cursor==255)
			cursor=numItems-1;

		MakeNormalSound(SND_MENUCLICK);
	}
	if((c&CONTROL_DN) && (!reptCounter))
	{
		cursor++;
		if(cursor==numItems)
			cursor=0;

		MakeNormalSound(SND_MENUCLICK);
	}

	if((c&CONTROL_LF) && (!reptCounter))
	{
		switch(menu[cursor].effect)
		{
			case PE_SNDVOL:
				MakeNormalSound(SND_MENUSELECT);
				profile.sound=PrevVolumeSpot(profile.sound);
				JamulSoundVolume(profile.sound);
				break;
			case PE_MUSVOL:
				MakeNormalSound(SND_MENUSELECT);
				profile.music=PrevVolumeSpot(profile.music);
				SetMusicVolume(profile.music);
				break;
			case PE_SONG:
				PlayPrevSong();
				break;
			case PE_JOURNAL:
				player.jpage--;
				if(player.jpage==255)
					player.jpage=NUM_PAGES-1;
				while(player.journal[player.jpage]==0)
				{
					player.jpage--;
					if(player.jpage==255)
						player.jpage=NUM_PAGES-1;
				}
				break;
			case PE_WEAPON:
			case PE_CONTINUE:
				ChangeWeapon(-1);
				break;
		}
		SetupSoundItems();
	}
	if((c&CONTROL_RT) && (!reptCounter))
	{
		switch(menu[cursor].effect)
		{
			case PE_SNDVOL:
				MakeNormalSound(SND_MENUSELECT);
				profile.sound=NextVolumeSpot(profile.sound);
				JamulSoundVolume(profile.sound);
				break;
			case PE_MUSVOL:
				MakeNormalSound(SND_MENUSELECT);
				profile.music=NextVolumeSpot(profile.music);
				SetMusicVolume(profile.music);
				break;
			case PE_SONG:
				PlayNextSong();
				break;
			case PE_JOURNAL:
				player.jpage++;
				if(player.jpage>=NUM_PAGES)
					player.jpage=0;
				while(player.journal[player.jpage]==0)
				{
					player.jpage++;
					if(player.jpage>=NUM_PAGES)
						player.jpage=0;
				}
				break;
			case PE_WEAPON:
			case PE_CONTINUE:
				ChangeWeapon(1);
				break;
		}
		SetupSoundItems();
	}

	i=0;
	if(((c&CONTROL_B1) && (!(oldc&CONTROL_B1))) ||
	   ((c&CONTROL_B2) && (!(oldc&CONTROL_B2))))
	   i=1;
	if(mgl->MouseTap())
		i=2;
	if(i>0)
	{
		MakeNormalSound(SND_MENUSELECT);
		if(i==2)	// only on mouse click
		{
			for(i=0;i<7;i++)
			{
				if(msx>=24+38*(i+1)-15 && msx<=24+38*(i+1)+15 && msy>=pauseY+INV_Y-20 && msy<=pauseY+INV_Y+10)
				{
					if(i==0 || player.weaponLvl[i-1]>0)
					{
						player.weapon=i;
						i=1000;
						break;
					}

				}
			}
			if(i!=1000)
				i=2;
		}
		if(i==1 || (i==2 && msOkay))
		{
			i=0;
			switch(menu[cursor].effect)
			{
				case PE_CONTINUE:
					return PAUSE_CONTINUE;
				case PE_RETRY:
					goodguy->GetShot(0,0,255,curMap,&curWorld);
					return PAUSE_CONTINUE;
				case PE_GIVEUP:
					return PAUSE_GIVEUP;
				case PE_WEAPON:
					ChangeWeapon(1);
					break;
				case PE_WORLDSEL:
					//strcpy(profile.lastWorld,player.worldName);
					player.worldProg->levelOn=player.levelNum;
					SaveProfile();
					return PAUSE_WORLDSEL;
				case PE_SNDVOL:
					profile.sound=NextVolumeSpot(profile.sound);
					JamulSoundVolume(profile.sound);
					break;
				case PE_MUSVOL:
					profile.music=NextVolumeSpot(profile.music);
					SetMusicVolume(profile.music);
					break;
				case PE_EXIT:
					pauseY=480;
					if(!shopping && !editing)
					{
						//strcpy(profile.lastWorld,player.worldName);
						player.worldProg->levelOn=player.levelNum;
					}
					SaveProfile();
					SaveState();
					return PAUSE_EXIT;
				case PE_SHOP:
					if(!shopping)
					{
						//strcpy(profile.lastWorld,player.worldName);
						player.worldProg->levelOn=player.levelNum;
					}
					SaveProfile();
					return PAUSE_SHOP;
				case PE_CHEAT:
					cursor=0;
					menuMode=1-menuMode;
					if(menuMode==1)
					{
						FillPauseMenu(cheatPause);
					}
					else
					{
						if(!editing)
						{
							// if cheats are available, use cheat game list instead
							if(ItemPurchased(SHOP_MAJOR,MAJOR_CHEATMENU))
								FillPauseMenu(gameCheatPause);
							else
								FillPauseMenu(gamePause);
						}
						else
						{
							FillPauseMenu(editPause);
						}
					}
					i=100;
					break;
				case PE_MORECHEATS:
					cursor=0;
					FillPauseMenu(cheatPause2);
					i=100;
					break;
				case PE_PREVCHEATS:
					cursor=0;
					FillPauseMenu(cheatPause);
					i=100;
					break;
				case PE_BZZT:
					MakeNormalSound(SND_TURRETBZZT);
					break;
				case PE_JOURNAL:
					player.jpage++;
					if(player.jpage>=NUM_PAGES)
						player.jpage=0;
					while(player.journal[player.jpage]==0)
					{
						player.jpage++;
						if(player.jpage>=NUM_PAGES)
							player.jpage=0;
					}
					break;
			}
		}
		if(i==0 && menu[cursor].effect>=PE_CHEATS)
		{
			DoCheat(menu[cursor].effect-PE_CHEATS);
			return PAUSE_CONTINUE;
		}
		SetupSoundItems();
	}


	HandlePauseKeyPresses(mgl);
	if(lastKey==27 || ((c&CONTROL_B3) && !(oldc&CONTROL_B3)))	// hit ESC to exit pause menu
	{
		MakeNormalSound(SND_MENUSELECT);
		lastKey=0;
		return PAUSE_CONTINUE;
	}

	oldc=c;

	return PAUSE_PAUSED;
}

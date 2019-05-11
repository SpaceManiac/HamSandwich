#include "services.h"
#include "pause.h"
#include "player.h"
#include "options.h"
#include "quest.h"
#include "skill.h"
#include "equip.h"
#include "alchemy.h"
#include "arena.h"
#include "leveldef.h"
#include "allusionary.h"
#include "achieve.h"

static byte cursor=0;
static byte shopType,shopMode,arenaPage=0;
static byte msPick;
static char lastKey=0;
static byte darkness;
static byte oldc;
static char shopName[32];
static byte potionPick[3],curPot;
static byte svcStuff[SHOP_WID*SHOP_HEI];
static equip_t msmithItem;

word CalcSmithCost(equip_t *e1,equip_t *e2)
{
	word cost;

	cost=(e1->value+e2->value+e1->speed+e2->speed);
	if(e1->magic>=1 && e1->magic<=30)	// regular magic
		cost*=2;
	else if(e1->magic>=31 && e1->magic<=35)	// elite magic
		cost*=5;
	else if(e1->magic>=46 && e1->magic<=54)	// cursed magic
		cost*=3;
	if(e2->magic>=1 && e2->magic<=30)	// regular magic
		cost*=2;
	else if(e2->magic>=31 && e2->magic<=35)	// elite magic
		cost*=5;
	else if(e2->magic>=46 && e2->magic<=54)	// cursed magic
		cost*=3;

	cost*=8;
	return cost;
}

void RenderServiceAlchemy(MGLDraw *mgl)
{
	int i;

	PauseBox(180,100,460,270,234);

	for(i=0;i<3;i++)
	{
		RenderInvItem(240+i*50,110,potionPick[i],1,(byte)(curPot==i));
	}
	for(i=0;i<6;i++)
	{
		if(EquipCount(IT_HERBA+i))
			RenderInvItem(194+i*42,160,IT_HERBA+i,EquipCount(IT_HERBA+i),(byte)(cursor==i));
		else
			RenderInvItem(194+i*42,160,IT_NONE,1,(byte)(cursor==i));
	}
	CenterPrintGlow(320,210,"Left & Right choose herbs, Fire to",0,1);
	CenterPrintGlow(320,224,"select, Jump to cancel. You can't use",0,1);
	CenterPrintGlow(320,238,"the same herb twice in one potion",0,1);
	CenterPrintGlow(320,252,"for safety reasons.",0,1);
}

void RenderServiceArena(MGLDraw *mgl)
{
	int i,cost;
	char s[16];

	PauseBox(120,40,520,320,234);

#ifdef DIRECTORS
	if(arenaPage==11)
		strcpy(s,"Bonus Matches");
	else
		sprintf(s,"Rank %d Matches",arenaPage);
#else
	sprintf(s,"Rank %d Matches",arenaPage);
#endif
	CenterPrintGlow(320,45,s,0,0);

	for(i=0;i<5;i++)
	{
		if(cursor!=i)
			PauseBox(125,80+i*32,515,80+i*32+28,240);
		else
			PauseBox(125,80+i*32,515,80+i*32+28,32*5+16);
		if(arenaMatch[i+arenaPage*5].var!=0 && player.var[arenaMatch[i+arenaPage*5].var]==0)
		{
			if(arenaMatch[i+arenaPage*5].var==VAR_MADCAP)
				PrintDark(130,80+i*32+2,"(Madcap Only!)",0);
			else
				PrintDark(130,80+i*32+2,"(Arena Closed)",0);
			s[0]='\0';
		}
		else
		{
			PrintDark(130,80+i*32+2,arenaMatch[i+arenaPage*5].name,0);
			cost=arenaMatch[i+arenaPage*5].cost;
			if(ModifierOn(MOD_ARENA))
				cost*=5;
			if(player.var[VAR_MADCAP])
				cost*=2;
			sprintf(s,"$%d",cost);
		}
		RightPrintDark(510,80+i*32+2,s,0);
	}
	i=5;
	if(cursor!=5)
		PauseBox(125,80+i*32,515,80+i*32+28,240);
	else
		PauseBox(125,80+i*32,515,80+i*32+28,32*5+16);
	PrintDark(130,80+i*32+2,"Exit",0);

	CenterPrintGlow(320,280,"Left & Right to change Ranks, Up & Down to",0,1);
	CenterPrintGlow(320,292,"select a match.  Fire to begin, Jump to exit.",0,1);

	PauseBox(120,322,520,378,234);
	if(cursor<5)
	{
		if(arenaMatch[cursor+arenaPage*5].var!=0 && player.var[arenaMatch[cursor+arenaPage*5].var]==0)
		{
			if(arenaMatch[cursor+arenaPage*5].var==VAR_MADCAP)
			{
				PrintGlow(125,326,"This bonus match is only available in Madcap Mode.",0,1);
			}
			else
			{
				PrintGlow(125,326,"We are still negotiating the crystal ball rights for this",0,1);
				PrintGlow(125,338,"match.  Come back later.",0,1);
			}
		}
		else
		{
			PrintGlow(125,326,arenaMatch[cursor+arenaPage*5].desc[0],0,1);
			PrintGlow(125,338,arenaMatch[cursor+arenaPage*5].desc[1],0,1);
			cost=arenaMatch[cursor+arenaPage*5].prize;
			if(ModifierOn(MOD_ARENA))
				cost*=5;
			if(player.var[VAR_MADCAP])
				cost*=2;
			sprintf(s,"Prize: $%d",cost);
			PrintGlow(125,360,s,0,1);
		}
	}
	else
	{
		PrintGlow(125,326,"Give up like a puny infant!",0,1);
	}
}

void RenderServiceBalloon(MGLDraw *mgl)
{
	int i;
	char s[48];
	char placeName[6][16]={
		"Eisburg",
		"Tina's Arena",
		"Baskerville",
		"Frosty Cliffs",
		"Haunted Highway",
		"Glacial Gorge",
	};

	PauseBox(140,40,500,350,234);

	sprintf(s,"Oh, The Places You've Been");
	CenterPrintGlow(320,45,s,0,0);

	for(i=0;i<6;i++)
	{
		if(cursor!=i)
			PauseBox(145,80+i*32,495,80+i*32+28,240);
		else
			PauseBox(145,80+i*32,495,80+i*32+28,32*5+16);
		if(player.var[VAR_VISITED+i])
			PrintDark(150,80+i*32+2,placeName[i],0);
		else
			PrintDark(150,80+i*32+2,"???",0);
	}
	i=6;
	if(cursor!=6)
		PauseBox(145,80+i*32,495,80+i*32+28,240);
	else
		PauseBox(145,80+i*32,495,80+i*32+28,32*5+16);
	PrintDark(150,80+i*32+2,"Cancel",0);

	CenterPrintGlow(320,310,"Up & Down to choose a destination, Fire to fly",0,1);
	CenterPrintGlow(320,330,"there, Jump to exit.",0,1);
}

void RenderServiceSharpen(MGLDraw *mgl)
{
	byte c;

	PauseBox(140,40,500,230,234);

	CenterPrintGlow(320,45,"Sharpen this axe?",0,0);

	ConvertEquipmentForShow(1);
	RenderInvItem(150,80,player.axe.type,1,0);
	ConvertEquipmentForShow(0);
	Print(195,85,EquipName(&player.axe),0,1);

	if(cursor==0)
		c=32*5+16;
	else
		c=240;

	PauseBox(145,130,310,160,c);
	CenterPrintDark((310-145)/2+145,135,"Yes",0);

	if(cursor==1)
		c=32*5+16;
	else
		c=240;
	PauseBox(330,130,495,160,c);
	CenterPrintDark((495-330)/2+330,135,"No",0);

	CenterPrintGlow(320,180,"Left & Right to choose, Fire to accept.",0,1);
	CenterPrintGlow(320,200,"Woody can only sharpen your equipped axe.",0,1);
}

void RenderClockJunkStuff(MGLDraw *mgl)
{
	int i,j;
	char s[16];

	PauseBox(1,32,200,428,234);
	for(i=0;i<SHOP_WID;i++)
		for(j=0;j<SHOP_HEI;j++)
		{
			if((shopType==SVC_JUNKSMITH  && i+j*SHOP_WID<(int)(4+TalentBonus(TLT_JUNKSMITH))) ||
				(shopType==SVC_CLOCKWORK && i+j*SHOP_WID<(int)(5+TalentBonus(TLT_CLOCKWORK))))
				RenderInvItem(7+i*43,38+j*43,svcStuff[i+j*SHOP_WID],1,(cursor==(i+j*4+100)));
		}

	if(cursor>=100)
	{
		if(svcStuff[cursor-100]!=IT_NONE)
		{
			equip_t eq;

			eq.type=svcStuff[cursor-100];
			eq.count=1;
			DescribeItem(206-208,304-352,&eq);
		}
	}

	if(shopType==SVC_JUNKSMITH)
	{
		if(svcStuff[0] && svcStuff[1] && svcStuff[2] && svcStuff[3])	// have 4 items at least
			strcpy(s,"Smith It Good");
		else
			strcpy(s,"Need 4 Items");
	}
	else
	{
		if(svcStuff[0] && svcStuff[1] && svcStuff[2] && svcStuff[3] && svcStuff[4])	// have 5 items at least
			strcpy(s,"Build Clockbot");
		else
			strcpy(s,"Need 5 Items");
	}
	if(cursor==130)
	{
		PauseBox(7,304,194,304+30,234+14);
		CenterPrintDark(100,307,s,0);
	}
	else
	{
		PauseBox(7,304,194,304+30,234);
		CenterPrintGlow(100,307,s,0,0);
	}

	if(cursor==131)
	{
		PauseBox(7,340,194,340+30,234+14);
		CenterPrintDark(100,342,"Cancel",0);
	}
	else
	{
		PauseBox(7,340,194,340+30,234);
		CenterPrintGlow(100,342,"Cancel",0,0);
	}

	Print(6,384,"Press directions to select,",0,1);
	Print(6,396,"Fire to add/remove, or",0,1);
	Print(6,408,"Jump to cancel",0,1);
}

void RenderYourJunkJunk(MGLDraw *mgl)
{
	int i,j,x,y;

	x=0;
	y=0;
	PauseBox(202,32,638,298,234);
	PauseBox(202,300,638,378,234);

	ConvertEquipmentForShow(1);
	for(i=0;i<10;i++)
		for(j=0;j<6;j++)
		{
			if(player.items[i+j*10].type>=IT_LOG && player.items[i+j*10].type<=IT_DIAMOND)
				RenderInvItem(206+i*43,38+j*43,player.items[i+j*10].type,player.items[i+j*10].count,(cursor==i+j*10));
			else
				RenderUnusableInvItem(206+i*43,38+j*43,player.items[i+j*10].type,player.items[i+j*10].count,(cursor==i+j*10));
		}
	ConvertEquipmentForShow(0);
	if(cursor<60)
	{
		if(player.items[cursor].type!=IT_NONE)
		{
			DescribeItem(206-208,304-352,&player.items[cursor]);
			if(player.items[cursor].type>=IT_LOG && player.items[cursor].type<=IT_DIAMOND)
			{
				RightPrint(636,304,"Press Fire to add",0,1);
			}
			else
				RightPrint(636,304,"Can't be used here",0,1);
		}
	}
}

void RenderYourJunkMagicSmith(MGLDraw *mgl,byte type)
{
	int i,j,x,y;

	x=0;
	y=0;
	PauseBox(202,32,638,298,234);
	PauseBox(202,300,638,378,234);

	ConvertEquipmentForShow(1);
	for(i=0;i<10;i++)
		for(j=0;j<6;j++)
		{
			if((player.items[i+j*10].type==IT_MAGICAXE || player.items[i+j*10].type==IT_MAGICPARKA ||
				player.items[i+j*10].type==IT_MAGICAMULET) && (type==0 || type==player.items[i+j*10].type))
				RenderInvItem(206+i*43,38+j*43,player.items[i+j*10].type,player.items[i+j*10].count,(cursor==i+j*10));
			else
				RenderUnusableInvItem(206+i*43,38+j*43,player.items[i+j*10].type,player.items[i+j*10].count,(cursor==i+j*10));
		}
	ConvertEquipmentForShow(0);
	if(cursor<60)
	{
		if(player.items[cursor].type!=IT_NONE)
		{
			DescribeItem(206-208,304-352,&player.items[cursor]);
			if((player.items[cursor].type==IT_MAGICAXE || player.items[cursor].type==IT_MAGICPARKA ||
				player.items[cursor].type==IT_MAGICAMULET) && (type==0 || type==player.items[cursor].type))
			{
				if(type==0)
					RightPrint(636,304,"Press Fire to select",0,1);
				else
				{
					char s[32];

					sprintf(s,"Press Fire to smith for $%d",CalcSmithCost(&msmithItem,&player.items[cursor]));
					RightPrint(636,304,s,0,1);
				}
			}
			else
				RightPrint(636,304,"Can't be smithed",0,1);
		}
	}
}

void RenderServiceMagicSmith(MGLDraw *mgl)
{
	RenderYourJunkMagicSmith(mgl,msmithItem.type);

	PauseBox(1,32,200,378,234);
	RenderInvItem(7+1*43,80,msmithItem.type,1,0);
	RenderInvItem(7+2*43,80,IT_NONE,1,0);

	Print(6,336,"Press directions to select,",0,1);
	Print(6,348,"Fire to choose an item, or",0,1);
	Print(6,360,"Jump to cancel",0,1);
}


void RenderServiceJunksmith(MGLDraw *mgl)
{
	RenderClockJunkStuff(mgl);
	RenderYourJunkJunk(mgl);
}

void RenderServiceClockwork(MGLDraw *mgl)
{
	RenderClockJunkStuff(mgl);
	RenderYourJunkJunk(mgl);
}

void RenderServiceOnion(MGLDraw *mgl)
{
	int i;
	char txt[2][32]={"Join The Onion Ring","Get Beaten Senseless"};

	PauseBox(140,70,500,160,234);

	for(i=0;i<2;i++)
	{
		if(cursor!=i)
			PauseBox(145,80+i*32,495,80+i*32+28,240);
		else
			PauseBox(145,80+i*32,495,80+i*32+28,32*5+16);
		PrintDark(150,80+i*32+2,txt[i],0);
	}

	CenterPrintGlow(320,310,"Up & Down to choose your response, Fire to accept",0,1);
	CenterPrintGlow(320,330,"your choice.  Choose wisely!",0,1);
}

void RenderServiceAllusionary(MGLDraw *mgl)
{
	int i;
	char s[32];

	PauseBox(0,32,318,330,234);
	PauseBox(322,32,638,330,31);
	PauseBox(0,334,638,357,234);

	PrintDark(5,340,"Up and Down to choose Allusions.  Left and Right to flip pages.  Jump to exit.",1);

	for(i=0;i<9;i++)
	{
		if((cursor%9)==i)
			PauseBox(8,40+i*32,310,40+i*32+28,32*5+16);
		else
			PauseBox(8,40+i*32,310,40+i*32+28,240);
		PrintDark(13,40+i*32+2,All_EntryName(i+(cursor/9)*9),0);
	}

	PrintDark(328,40,All_EntryName(cursor),0);
	PrintRectBlack2(328,70,633,460,All_EntryDesc(cursor),13,1);

	sprintf(s,"Page %d",(cursor/9)+1);
	PrintDark(328,315,s,1);
	sprintf(s,"Allusion %d/99",cursor+1);
	RightPrintDark(633,315,s,1);
}

void RenderService(MGLDraw *mgl)
{
	char s[10];

	DarkenScreen(darkness);

	PauseBox(10,0,629,30,234);
	CenterPrintGlow(320,5,shopName,0,0);
	sprintf(s,"$%04d",player.money);
	RightPrintGlow(624,5,s,0,0);

	switch(shopType)
	{
		case SVC_ALCHEMY:
			RenderServiceAlchemy(mgl);
			break;
		case SVC_ARENA:
			RenderServiceArena(mgl);
			RenderChatNoName(mgl);
			break;
		case SVC_BALLOON:
			RenderServiceBalloon(mgl);
			RenderChatNoName(mgl);
			break;
		case SVC_SHARPEN:
			RenderServiceSharpen(mgl);
			RenderChatNoName(mgl);
			break;
		case SVC_ONION:
			RenderServiceOnion(mgl);
			RenderChatNoName(mgl);
			break;
		case SVC_JUNKSMITH:
			RenderServiceJunksmith(mgl);
			break;
		case SVC_CLOCKWORK:
			RenderServiceClockwork(mgl);
			break;
		case SVC_MAGICSMITH:
			RenderServiceMagicSmith(mgl);
			RenderChatNoName(mgl);
			break;
		case SVC_ALLUS:
			RenderServiceAllusionary(mgl);
			break;
	}
}

void HandleServiceKeyPresses(MGLDraw *mgl)
{
	char k;

	k=mgl->LastKeyPressed();
	if(k)
		lastKey=k;
}

void ClearJunkpile(void)
{
	int i;

	for(i=0;i<SHOP_WID*SHOP_HEI;i++)
		svcStuff[i]=0;
}

void ReturnJunkpile(void)
{
	int i;

	MessageDisable(1);
	for(i=0;i<SHOP_WID*SHOP_HEI;i++)
	{
		if(svcStuff[i])
			EquipAdd(svcStuff[i],0,0);
		svcStuff[i]=0;
	}
	MessageDisable(0);
	SortEquipment();
}

void SortJunkpile(void)
{
	byte swap;
	int i;

	swap=1;
	while(swap)
	{
		swap=0;
		for(i=0;i<SHOP_WID*SHOP_HEI-1;i++)
		{
			if((svcStuff[i]==IT_NONE && svcStuff[i+1]!=IT_NONE) ||
				(svcStuff[i]>svcStuff[i+1] && svcStuff[i+1]!=IT_NONE))
			{
				byte b;

				b=svcStuff[i];
				svcStuff[i]=svcStuff[i+1];
				svcStuff[i+1]=b;
				swap=1;
			}
		}
	}
}

byte AddJunk(byte type)
{
	int i;
	byte maxPos;

	if(shopType==SVC_JUNKSMITH)
		maxPos=(int)(4+TalentBonus(TLT_JUNKSMITH));
	else
		maxPos=(int)(5+TalentBonus(TLT_CLOCKWORK));

	for(i=0;i<maxPos;i++)
		if(svcStuff[i]==0)
		{
			svcStuff[i]=type;
			SortJunkpile();
			return 1;
		}

	return 0;
}

void JunksmithItem(void)
{
	int seed,lvl,i;
	word seedVal[]={
		1,	// log
		2,	// gear
		4,	// can
		8,	// stuffing
		16,	// sock
		32,	// ice cube
		64, // seed
		128, // junk2
		256, // junk3
		512, // junk4
		1024, // silver
		2048, // gold
		4096, // ruby
		8192, // emerald
		16384, // diamond
	};
	byte lvlVal[]={
		1,	// log
		1,	// gear
		1,	// can
		1,	// stuffing
		1,	// sock
		2,	// ice cube
		2,  // seed
		3,  // rock
		2,  // ecto
		3,  // claw
		1,  // silver
		2,  // gold
		3,  // ruby
		4,  // emerald
		8,  // diamond
	};
	byte type;
	int val1,val2;
	bullet_t *b;

	seed=0;
	lvl=0;
	for(i=0;i<SHOP_WID*SHOP_HEI;i++)
	{
		if(svcStuff[i])
		{
			seed+=seedVal[svcStuff[i]-IT_LOG];
			lvl+=lvlVal[svcStuff[i]-IT_LOG];
		}
	}
	if((rand()%100)>75+(int)TalentBonus(TLT_JUNKSMITH))
	{
		NewMessage("You broke the junk!",0);
		MakeNormalSound(SND_BOOM);
		return;
	}
	TalentPoint(TLT_JUNKSMITH,1);
	srand((dword)seed);
	type=0;
	while(type<IT_AXE || type>IT_GOLDENPARKA)
	{
		EquipCreateItem((byte)lvl,&type,&val1,&val2);
	}
	if((type==IT_MAGICAXE || type==IT_MAGICPARKA || type==IT_MAGICAMULET) && player.talentLevel[TLT_JUNKSMITH]<4)
	{
		NewMessage("The junk exploded!",0);
		MakeNormalSound(SND_BOOM);
		return;
	}
	if((type==IT_GOLDENAXE || type==IT_GOLDENPARKA || type==IT_GOLDENAMULET))
	{
		if(player.talentLevel[TLT_JUNKSMITH]<7)
		{
			NewMessage("The junk exploded!",0);
			MakeNormalSound(SND_BOOM);
			return;
		}
		else if((val1/65536)>=EM_ARTSTART && (val1/65536)<=EM_ARTEND && player.talentLevel[TLT_JUNKSMITH]<10)	// it's an artifact!
		{
			NewMessage("The junk exploded!",0);
			MakeNormalSound(SND_BOOM);
			return;
		}
	}
	srand(timeGetTime());
	b=FireBullet(goodguy->ID,goodguy->x,goodguy->y,FIXAMT*20,rand()%256,BLT_ITEM,0);
	if(b)
	{
		b->speed=0;
		b->anim=type;
		b->target=val1;
		b->owner=val2;
	}
	MakeNormalSound(SND_SMITH);
}

void MakeClockBot(void)
{
	int i;

	DeleteMonsterByType(MONS_YOURBOT);
	for(i=0;i<15;i++)
		player.var[VAR_CLOCKBOT+i]=0;	// clear out clock settings

	for(i=0;i<SHOP_WID*SHOP_HEI;i++)
	{
		if(svcStuff[i])
		{
			player.var[VAR_CLOCKBOT-IT_LOG+svcStuff[i]]++;
		}
	}
	PlayerCalcStats();
	AddGuy(goodguy->x,goodguy->y,goodguy->z,MONS_YOURBOT);
	i=MonsterHP(MONS_YOURBOT,GOOD);
	player.var[VAR_CLOCKLIFE]=(i&0xFF);
	player.var[VAR_CLOCKLIFE2]=(i>>8);

	MakeNormalSound(SND_SMITH);
	MakeNormalSound(SND_PUPPETHAT);
}

void InitService(byte type)
{
	shopType=type;
	shopMode=0;
	MakeNormalSound(SND_PAUSE);
	lastKey=0;
	cursor=0;
	darkness=0;
	oldc=255;

	switch(shopType)
	{
		case SVC_SHARPEN:
			strcpy(shopName,"Woody's Sharpening Service");
			break;
		case SVC_MAGICSMITH:
			strcpy(shopName,"The Magicsmith");
			msmithItem.type=IT_NONE;
			break;
		case SVC_BALLOON:
			strcpy(shopName,"Phileas The Balloonist");
			break;
		case SVC_ARENA:
			strcpy(shopName,"Tina's Arena");
			break;
		case SVC_ALCHEMY:
			strcpy(shopName,"Alchemy");
			potionPick[0]=potionPick[1]=potionPick[2]=0;
			curPot=0;
			break;
		case SVC_ONION:
			strcpy(shopName,"A Tough Decision");
			break;
		case SVC_JUNKSMITH:
			strcpy(shopName,"Junksmithing");
			ClearJunkpile();
			break;
		case SVC_CLOCKWORK:
			strcpy(shopName,"Clockwork");
			ClearJunkpile();
			break;
		case SVC_ALLUS:
			strcpy(shopName,"The Allusionary");
			break;
	}
}

static byte ButtonPressedClockJunk(byte btn)
{
	byte maxPos;

	if(shopType==SVC_JUNKSMITH)
		maxPos=(int)(4+TalentBonus(TLT_JUNKSMITH));
	else
		maxPos=(int)(5+TalentBonus(TLT_CLOCKWORK));

	switch(btn)
	{
		case CONTROL_UP:
			if(cursor<60)
			{
				cursor-=10;
				if(cursor>=60)
					cursor+=60;
			}
			else
			{
				if((cursor-100)/SHOP_WID==0)
				{
					cursor=131;
				}
				else if(cursor==130)
				{
					cursor=100+((maxPos-1)/SHOP_WID)*SHOP_WID;
				}
				else if(cursor==131)
					cursor=130;
				else
					cursor-=SHOP_WID;
			}
			break;
		case CONTROL_DN:
			if(cursor<60)
			{
				cursor+=10;
				if(cursor>=60)
					cursor-=60;
			}
			else if(cursor==130)
				cursor=131;
			else if(cursor==131)
				cursor=100;
			else
			{
				cursor+=4;
				if(cursor>=100+maxPos)
					cursor=130;
			}
			break;
		case CONTROL_LF:
			if(cursor<60)
			{
				cursor--;
				if(cursor==255 || (cursor%10)==9)
					cursor+=10;
				if((cursor%10)==9)
				{
					cursor=(cursor/10)*4+3+100;	// move into shop part
					if(cursor>=100+maxPos)
						cursor=100+maxPos-1;
				}
			}
			else if(cursor==130 || cursor==131)
				cursor=59;
			else
			{
				if(((cursor-100)%4)==0)
					cursor=((cursor-100)/4)*10+9;
				else
					cursor--;
			}
			break;
		case CONTROL_RT:
			if(cursor<60)
			{
				cursor++;
				if((cursor%10)==0)
					cursor-=10;
				if((cursor%10)==0)
				{
					cursor=(cursor/10)*4+0+100;	// move into shop part
					if(cursor>=100+maxPos)
						cursor=100+((maxPos-1)/4)*4;
				}
			}
			else if(cursor==130 || cursor==131)
				cursor=50;
			else
			{
				if(((cursor-100)%4)==3 || cursor==100+maxPos-1)
					cursor=((cursor-100)/4)*10+0;
				else
				{
					cursor++;
				}
			}
			break;
		case CONTROL_B1:
			if(cursor<60)
			{
				if(player.items[cursor].type>=IT_LOG && player.items[cursor].type<=IT_DIAMOND)
				{
					if(AddJunk(player.items[cursor].type))
					{
						DropItem(cursor);
						SortEquipment();
						MakeNormalSound(SND_MENUSELECT);
					}
					else
						MakeNormalSound(SND_MENUCANCEL);
				}
				else
					MakeNormalSound(SND_MENUCANCEL);
			}
			else if(cursor==130)
			{
				// smith it
				if(shopType==SVC_JUNKSMITH)
				{
					if(svcStuff[0] && svcStuff[1] && svcStuff[2] && svcStuff[3])	// have 4 items at least
					{
						JunksmithItem();
						MakeNormalSound(SND_MENUSELECT);
						return 0;
					}
					else
						MakeNormalSound(SND_MENUCANCEL);
				}
				else
				{
					if(svcStuff[0] && svcStuff[1] && svcStuff[2] && svcStuff[3] && svcStuff[4])	// have 5 items at least
					{
						MakeClockBot();
						MakeNormalSound(SND_MENUSELECT);
						return 0;
					}
					else
						MakeNormalSound(SND_MENUCANCEL);
				}

			}
			else if(cursor==131)
			{
				// exit
				ReturnJunkpile();
				return 0;
			}
			else
			{
				if(svcStuff[cursor-100]==IT_NONE)
					MakeNormalSound(SND_MENUCANCEL);
				else
				{
					MessageDisable(1);
					EquipAdd(svcStuff[cursor-100],0,0);
					svcStuff[cursor-100]=0;
					SortJunkpile();
					MakeNormalSound(SND_MENUSELECT);
					MessageDisable(0);
				}
			}
			break;
		case CONTROL_B2:
			ReturnJunkpile();
			return 0;
			break;
	}
	return 1;
}

static byte ButtonPressedAllusionary(byte btn)
{
	switch(btn)
	{
		case CONTROL_UP:
			if((cursor%9)==0)
				cursor+=8;
			else
				cursor--;
			break;
		case CONTROL_DN:
			if((cursor%9)==8)
				cursor-=8;
			else
				cursor++;
			break;
		case CONTROL_LF:
			if(cursor<=8)
				cursor+=90;
			else
				cursor-=9;
			break;
		case CONTROL_RT:
			if(cursor>=90)
				cursor-=90;
			else
				cursor+=9;
			break;
		case CONTROL_B1:
			break;
		case CONTROL_B2:
			return 0;
			break;
	}
	return 1;
}

static byte ButtonPressedMagicSmith(byte btn)
{
	switch(btn)
	{
		case CONTROL_UP:
			cursor-=10;
			if(cursor>=60)
				cursor+=60;
			break;
		case CONTROL_DN:
			cursor+=10;
			if(cursor>=60)
				cursor-=60;
			break;
		case CONTROL_LF:
			cursor--;
			if(cursor==255 || (cursor%10)==9)
				cursor+=10;
			break;
		case CONTROL_RT:
			cursor++;
			if((cursor%10)==0)
				cursor-=10;
			break;
		case CONTROL_B1:
			if(msmithItem.type==0)
			{
				if(player.items[cursor].type==IT_MAGICAXE || player.items[cursor].type==IT_MAGICPARKA || player.items[cursor].type==IT_MAGICAMULET)
				{
					msmithItem=player.items[cursor];
					DropItem(cursor);
					SortEquipment();
					MakeNormalSound(SND_MENUSELECT);
				}
				else
					MakeNormalSound(SND_MENUCANCEL);
			}
			else	// already have one item
			{
				if(player.items[cursor].type==msmithItem.type && player.money>=CalcSmithCost(&msmithItem,&player.items[cursor]))
				{
					player.money-=CalcSmithCost(&msmithItem,&player.items[cursor]);
					SmithItems(&msmithItem,&player.items[cursor]);
					DropItem(cursor);
					SortEquipment();
					MakeNormalSound(SND_PUPPETHAT);
					return 0;
				}
				else
					MakeNormalSound(SND_MENUCANCEL);
			}
			break;
		case CONTROL_B2:
			if(msmithItem.type!=0)
			{
				SimpleEquipAdd(&msmithItem);
				msmithItem.type=IT_NONE;
				SortEquipment();
				MakeNormalSound(SND_MENUCANCEL);
			}
			else
			{
				MakeNormalSound(SND_MENUCANCEL);
				return 0;
			}
			break;
	}
	return 1;
}

static byte ButtonPressedAlchemy(byte btn)
{
	switch(btn)
	{
		case CONTROL_UP:
			break;
		case CONTROL_DN:
			break;
		case CONTROL_LF:
			if(cursor==0)
				cursor=5;
			else
				cursor--;
			break;
		case CONTROL_RT:
			if(cursor==5)
				cursor=0;
			else
				cursor++;
			break;
		case CONTROL_B1:
			if(EquipCount(IT_HERBA+cursor))
			{
				if(curPot>0 && potionPick[0]==IT_HERBA+cursor)
				{
					MakeNormalSound(SND_MENUCANCEL);
					return 1;
				}
				if(curPot>1 && potionPick[1]==IT_HERBA+cursor)
				{
					MakeNormalSound(SND_MENUCANCEL);
					return 1;
				}
				MakeNormalSound(SND_MENUSELECT);
				DropItemOfType(IT_HERBA+cursor,1);
				potionPick[curPot]=IT_HERBA+cursor;
				curPot++;
				if(curPot==3)
				{
					MixPotion(potionPick[0],potionPick[1],potionPick[2]);
					return 0;
				}
			}
			else
				MakeNormalSound(SND_MENUCANCEL);
			break;
		case CONTROL_B2:
			if(curPot==0)
				return 0;
			else
			{
				curPot--;
				AddItemOfType(potionPick[curPot],1);
				potionPick[curPot]=0;
				MakeNormalSound(SND_MENUCANCEL);
			}
			break;
	}
	return 1;
}

static byte ButtonPressedArena(byte btn)
{
	switch(btn)
	{
		case CONTROL_UP:
			if(cursor==0)
				cursor=5;
			else
				cursor--;
			MakeNormalSound(SND_MENUCLICK);
			break;
		case CONTROL_DN:
			if(cursor==5)
				cursor=0;
			else
				cursor++;
			MakeNormalSound(SND_MENUCLICK);
			break;
		case CONTROL_LF:
#ifdef DIRECTORS
			if(arenaPage==0)
			{
				arenaPage=11;
			}
			else if(arenaPage==11)
				arenaPage=(int)TalentBonus(TLT_GLADIATOR);
			else
				arenaPage--;
#else
			if(arenaPage==0)
			{
				arenaPage=(int)TalentBonus(TLT_GLADIATOR);
			}
			else
				arenaPage--;
#endif
			MakeNormalSound(SND_MENUCLICK);
			break;
		case CONTROL_RT:
#ifdef DIRECTORS
			if(arenaPage==11)
				arenaPage=0;
			else if(arenaPage>=(int)TalentBonus(TLT_GLADIATOR))
				arenaPage=11;
			else
			{
				arenaPage++;
			}
#else
			if(arenaPage>=(int)TalentBonus(TLT_GLADIATOR))
				arenaPage=0;
			else
			{
				arenaPage++;
			}
#endif
			MakeNormalSound(SND_MENUCLICK);
			break;
		case CONTROL_B1:
			int w;

			w=arenaMatch[cursor+arenaPage*5].cost;
			if(ModifierOn(MOD_ARENA))
				w*=5;
			if(player.var[VAR_MADCAP])
				w*=2;

			if(cursor==5)	// exit
			{
				MakeNormalSound(SND_MENUCANCEL);
				return 0;
			}
			else if(player.money>=w &&
					(arenaMatch[cursor+arenaPage*5].var==0 || player.var[arenaMatch[cursor+arenaPage*5].var]))
			{
				MakeNormalSound(SND_MENUSELECT);
				player.money-=w;
				player.arenaLvl=cursor+arenaPage*5;
				SendMessageToGame(MSG_GOTOMAP,arenaMatch[player.arenaLvl].map);
				return 0;	// enter the match here
			}
			else
			{
				MakeNormalSound(SND_MENUCANCEL);
			}
			break;
		case CONTROL_B2:
			MakeNormalSound(SND_MENUCANCEL);
			return 0;
			break;
	}
	return 1;
}

byte balloonData[]={
		7,9,LVL_EISBURG,	// Eisburg
		103,19,LVL_TUNDRA,		// Arena
		88,61,LVL_BASKERVILLE,	// baskerville
		79,51,LVL_CLIFFS,		// cliffs
		15,113,LVL_HIGHWAY,	// highway
		151,15,LVL_GORGE,				// gorge
	};


void BalloonTrip(byte where)
{
	player.destx=balloonData[where*3];
	player.desty=balloonData[where*3+1];
	SendMessageToGame(MSG_GOTOMAP,balloonData[where*3+2]);
}

static byte ButtonPressedBalloon(byte btn)
{
	switch(btn)
	{
		case CONTROL_UP:
			if(cursor==0)
				cursor=6;
			else
				cursor--;
			break;
		case CONTROL_DN:
			if(cursor==6)
				cursor=0;
			else
				cursor++;
			break;
		case CONTROL_B1:
			if(cursor==6)	// exit
			{
				MakeNormalSound(SND_MENUCANCEL);
				return 0;
			}
			else if(player.money>=100 && player.var[VAR_VISITED+cursor])
			{
				if(balloonData[cursor*3+2]==player.levelNum)
				{
					NewChat(108);
					MakeNormalSound(SND_MENUCANCEL);
				}
				else
				{
					MakeNormalSound(SND_MENUSELECT);
					player.money-=100;
					BalloonTrip(cursor);
					return 0;
				}
			}
			else if(player.money<100)
			{
				NewChat(54);
				MakeNormalSound(SND_MENUCANCEL);
			}
			else if(player.var[VAR_VISITED+cursor]==0)
			{
				NewChat(109);
				MakeNormalSound(SND_MENUCANCEL);
			}
			else
			{
				MakeNormalSound(SND_MENUCANCEL);
			}
			break;
		case CONTROL_B2:
			MakeNormalSound(SND_MENUCANCEL);
			return 0;
			break;
	}
	return 1;
}


static byte ButtonPressedSharpen(byte btn)
{
	switch(btn)
	{
		case CONTROL_LF:
		case CONTROL_RT:
			cursor=1-cursor;
			break;
		case CONTROL_B1:
			if(cursor==0)	// yes
			{
				if(player.axe.sharpened==0 && player.money>=50)
				{
					player.axe.sharpened=1;
					player.money-=50;
					player.axe.value++;
					PlayerCalcStats();
					MakeNormalSound(SND_SHARPEN);
					NewMessage("Your axe was sharpened!",0);
					return 0;
				}
				else
				{
					if(player.axe.sharpened)
						NewMessage("That axe is as sharp as it gets!",0);
					else if(player.money<50)
						NewMessage("You can't afford it!",0);
					MakeNormalSound(SND_MENUCANCEL);
					return 0;
				}
			}
			else
			{
				MakeNormalSound(SND_MENUSELECT);
				return 0;
			}
			break;
		case CONTROL_B2:
			MakeNormalSound(SND_MENUCANCEL);
			return 0;
			break;
	}
	return 1;
}

static byte ButtonPressedOnion(byte btn)
{
	switch(btn)
	{
		case CONTROL_UP:
		case CONTROL_DN:
			cursor=1-cursor;
			break;
		case CONTROL_B1:
			if(cursor==0)
			{
				MakeNormalSound(SND_MENUSELECT);
				player.chatClock=0;
				PlayerSetVar(VAR_SNUGGLY,0);
				BeginChatting(24);
				NewChat(175);
				return 1;
			}
			else
			{
				MakeNormalSound(SND_MENUSELECT);
				player.chatClock=0;
				PlayerSetVar(VAR_SNUGGLY,1);
				BeginChatting(24);
				NewChat(176);
				return 1;
			}
			break;
	}
	return 1;
}

static byte ButtonPressed(byte btn)
{
	switch(shopType)
	{
		case SVC_ALCHEMY:
			return ButtonPressedAlchemy(btn);
		case SVC_ARENA:
			return ButtonPressedArena(btn);
		case SVC_BALLOON:
			return ButtonPressedBalloon(btn);
		case SVC_SHARPEN:
			return ButtonPressedSharpen(btn);
		case SVC_ONION:
			return ButtonPressedOnion(btn);
		case SVC_MAGICSMITH:
			return ButtonPressedMagicSmith(btn);
		case SVC_JUNKSMITH:
		case SVC_CLOCKWORK:
			return ButtonPressedClockJunk(btn);
		case SVC_ALLUS:
			return ButtonPressedAllusionary(btn);
	}
	return 0;
}

byte UpdateService(MGLDraw *mgl)
{
	byte c;
	static byte reptCounter=0;

	UpdateChat(1);
	if(darkness<30)
		darkness++;

	c=GetControls()|GetArrows();

	reptCounter++;
	if((!oldc) || (reptCounter>10))
		reptCounter=0;

	if(shopMode==0)	// not in any submenu
	{
		if((c&CONTROL_UP) && (!reptCounter))
		{
			ButtonPressed(CONTROL_UP);
		}
		if((c&CONTROL_DN) && (!reptCounter))
		{
			ButtonPressed(CONTROL_DN);
		}
		if((c&CONTROL_LF) && (!reptCounter))
		{
			ButtonPressed(CONTROL_LF);
		}
		if((c&CONTROL_RT) && (!reptCounter))
		{
			ButtonPressed(CONTROL_RT);
		}
		if(((c&CONTROL_B1) && (!(oldc&CONTROL_B1))))
		{
			if(!ButtonPressed(CONTROL_B1))
				return 0;
		}
		if(((c&CONTROL_B2) && (!(oldc&CONTROL_B2))))
		{
			if(!ButtonPressed(CONTROL_B2))
				return 0;
		}
	}

	oldc=c;

	HandleServiceKeyPresses(mgl);
	if(lastKey==27)	// hit ESC to exit shop
	{
		if(shopType==SVC_ALCHEMY)
		{
			while(curPot>0)
			{
				curPot--;
				AddItemOfType(potionPick[curPot],1);
				potionPick[curPot]=0;
			}
		}
		else if(shopType==SVC_CLOCKWORK || shopType==SVC_JUNKSMITH)
		{
			ReturnJunkpile();
		}
		else if(shopType==SVC_MAGICSMITH)
		{
			if(msmithItem.type!=0)
			{
				SimpleEquipAdd(&msmithItem);
				SortEquipment();
			}
		}
		return 0;
		lastKey=0;
	}

	return 1;
}

void MadcapArena(void)
{
	arenaPage=0;
}

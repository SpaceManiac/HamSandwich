#include "shop.h"
#include "display.h"
#include "player.h"
#include "sound.h"
#include "fairy.h"
#include "challenge.h"
#include "achieves.h"

sprite_set_t *shopSpr;
static byte armaBrt=0;

static const char hatName[5][16]={
	"Paper Hat",
	"Felt Hat",
	"Moon Hat",
	"Checkered Hat",
	"Steel Hat"
};

static const char staffName[5][16]={
	"Balsa Staff",
	"Pine Staff",
	"Oak Staff",
	"Ironwood Staff",
	"Mystical Staff"
};

static const char bootName[5][18]={
	"Orthopedic Boots",
	"Leather Boots",
	"Doc Merlins",
	"Lucky Boots",
	"Air Lancelots"
};

static const char otherName[12][18]={
	"Heart Pendant",
	"Soul Pendant",
	"Moon Amulet",
	"Sun Amulet",
	"Lantern",
	"Light Of Wisdom",
	"Pointy Hat",
	"Magnetic Headband",
	"Magic Compass",
	"Bouncy Crystal",
	"Chicken Feathers",
	"Shootin' Socks",
};

static const char shopName[29][18]={
	"Felt Hat",
	"Moon Hat",
	"Checkered Hat",
	"Steel Hat",
	"Pointy Hat",
	"Pine Staff",
	"Oak Staff",
	"Ironwood Staff",
	"Mystical Staff",
	"Orb Compass",
	"Leather Boots",
	"Doc Merlins",
	"Lucky Boots",
	"Air Lancelots",
	"Chicken Feathers",
	"Heart Pendant",
	"Moon Amulet",
	"Lantern",
	"Bouncy Crystal",
	"Shootin' Socks",
	"Soul Pendant",
	"Sun Amulet",
	"Light Of Wisdom",
	"Magnetic Headband",
	"Exit",
	"Power Stone",
	"Shield Stone",
	"Spell Stone",
	"Skill Stone"};

static const word price[25]={30, 120, 400,2000,1000, // hats (and a bonus)
				70, 180, 600,3500,400, // staves (and a bonus)
				20, 100, 250,2000,350, // boots (and a bonus)
				800,1200,220,2200,700, // heart moon lantern, bonus bonus
				900,750,1500,450, 0};  // heart2, moon2, lantern2, bonus, exit

static const word shopPic[25]={14,15,16,17,32, // hats (and pointy)
				  4, 5, 6, 7, 27, // staves (and compass)
				  9, 10,11,12,29, // boots (and feathers)
				  18,19,20,28,30, // heart moon lantern, crystal socks
				  25,23,21,31,0}; // heart2, moon2, lantern2, headband, exit

static const dword gearTag[]={0,0,0,0,GEAR_POINTY,
		        0,0,0,0,GEAR_COMPASS,
				0,0,0,0,GEAR_FEATHER,
				GEAR_HEART,GEAR_MOON,GEAR_LAMP,GEAR_BOUNCY,GEAR_SOCKS,
				GEAR_SOUL,GEAR_SUN,GEAR_WISDOM,GEAR_MAGNET,0};

static const char itemDesc[33][4][64]={
	"Mmmm, soft. This hat offers",
	"slightly better protection",
	"than the paper hat.",
	"",

	"The magical moon emblem",
	"offers superior defensive",
	"capability.",
	"",

	"For some reason, putting a",
	"checkered pattern on the",
	"hat makes it even more",
	"durable.",

	"This hat is metal. It doesn't",
	"get much better than that.",
	"",
	"",

	"These handy spikes attach to",
	"your hat and fire at enemies",
	"anytime you are hit.",
	"",

	"The wood of the pine tree",
	"generates a more powerful",
	"fireball than ordinary",
	"balsa wood.",

	"Oak wood offers still",
	"better magical power.",
	"",
	"",

	"Ironwood is the most",
	"magical of woods. Highly",
	"recommended!",
	"",

	"This staff isn't even made of",
	"wood. It's made of amazing",
	"magical stuff, though",
	"we don't know what.",

	"This handy compass points",
	"the way to the nearest",
	"Orb or Fairy, if you haven't",
	"gotten one in a while.",

	"Much more comfortable",
	"than Orthopedic Boots.",
	"",
	"",

	"All the rage with the kids",
	"up in Seattleton. The rubber",
	"soles of these boots will",
	"help you run faster.",

	"They appear to be ordinary",
	"boots with yellow stars,",
	"but by sheer luck, they'll",
	"make you move really fast.",

	"Special air coils make these",
	"boots decrease your weight,",
	"causing you to move at",
	"incredible speed.",

	"Attach these to your boots",
	"to show your true colors.",
	"Then you can run from any",
	"battle without penalty!",

	"This pendant will double",
	"your maximum life! You",
	"can't beat that.",
	"",

	"This amulet doubles your",
	"maximum mana. And it",
	"really brings out your eyes",
	"too!",

	"This will really assist",
	"you in exploring dark",
	"areas.",
	"",

	"This crystal attaches to",
	"your staff, making your",
	"fireballs bounce off of",
	"enemies and hit others!",

	"With these comfy socks on,",
	"you'll be able run around",
	"while you launch fireballs!",
	"",

	"This pendant regenerates",
	"your health slowly over",
	"time.",
	"",

	"This amulet makes your",
	"magic slowly regenerate.",
	"",
	"",

	"This incredible orb actually",
	"makes you learn faster! With",
	"it, you will level up more",
	"quickly.",

	"This headband will attract",
	"coins toward you from a",
	"distance, making collection",
	"quite succulent!",

	"Continue adventuring.",
	"",
	"",
	"",

	"You can collect up to 99 of",
	"these stones. Each one will",
	"block 1% of the damage you",
	"take.",

	"You can collect up to 99 of",
	"these stones. Each one will",
	"add 10% to your fireball",
	"damage.",

	"You can collect up to 99 of",
	"these stones. Each one will",
	"add 10% to your spell",
	"damage.",

	"You can collect up to 99 of",
	"these stones. Each one will",
	"grant you a Skill Point.",
	"",

	"This ancient timepiece will",
	"give you 20% more time in",
	"Challenges. Magically.",
	"",

	"You can collect up to 99 of",
	"these stones. Each one will",
	"improve your armor, up to",
	"99% damage reduction.",

	"You can collect up to 99 of",
	"these stones. Each one will",
	"increase your fireball",
	"damage, by up to 990%.",

	"You can collect up to 99 of",
	"these stones. Each one will",
	"increase your spell damage,",
	"by up to 990%.",
};

byte shopCursor=0;
byte *shopScr;

void InitShop(void)
{
	shopSpr = new sprite_set_t("graphics/shop.jsp");
}

void ExitShop(void)
{
	delete shopSpr;
}

sprite_t *GetShopSpr(byte spr)
{
	return shopSpr->GetSprite(spr);
}


void EnterShop(void)
{
	PlaySong(SONG_SHOP);
	GetDisplayMGL()->LoadBMP("graphics/shop.bmp");
	shopScr=(byte *)malloc(SCRWID*SCRHEI);
	if(!shopScr)
		FatalError("Out of memory!!");
	memcpy(shopScr,GetDisplayMGL()->GetScreen(),SCRWID*SCRHEI);
	GetDisplayMGL()->LastKeyPressed();
	UpdateControls();
	SetInMenu(true);

	if (VeryClassicMode() && shopCursor % 5 == 4 || shopCursor >= 20)
	{
		shopCursor = 0;
	}
}

void LeaveShop(void)
{
	SetInMenu(false);
	free(shopScr);
	ResetInterface();
}

word Price(byte which)
{
	if (VeryClassicMode())
	{
		// 1999 mode: best hat, staff, and boots are cheaper.
		if (which == 3)
			return 1500; // down from 2000
		else if (which == 8)
			return 2500; // down from 3500
		else if (which == 13)
			return 1000; // down from 2000
	}
	return price[which];
}

word NightmarePrice(byte which)
{
	if (which == 0)	// shield stones
		return 50 * (player.shieldStones + 2);
	else if (which == 5)	// power stones
		return 50 * (player.powerStones + 2);
	else if (which == 10)
		return 50 * (player.spellStones + 2);
	else if (which == 15 && !ClassicMode())
		return 50 * (player.skillStones + 2);
	else
		return Price(which);
}

void GearOutline(byte spr, int x, int y)
{
	for(int i=x-1;i<=x+1;i++)
		for (int j = y - 1; j <= y + 1; j++)
		{
			if(i!=x || j!=y)
				shopSpr->GetSprite(spr)->DrawBright(i, j, GetDisplayMGL(), -31);
		}
	shopSpr->GetSprite(spr)->Draw(x, y, GetDisplayMGL());
}

void RenderGearSlot(int x, int y, const char* line1, const char* line2, byte spr)
{
	// the box for the item
	shopSpr->GetSprite(1)->Draw(x,y, GetDisplayMGL());
	// the item
	GearOutline(spr, x,y);
	if (line1[0] == '\0')	// this is a one-liner
	{
		Print(x+60+2, y+15+2, line2, 1, 2);
		Print(x+60, y+15, line2, 0, 2);
	}
	else
	{
		Print(x + 60+2, y + 5+2, line1, 1, 2);
		Print(x + 60, y + 5, line1, 0, 2);
		Print(x + 60+2, y + 25+2, line2, 1, 2);
		Print(x + 60, y + 25, line2, 0, 2);
	}
}

void RenderPlayerGear(int x,byte brt)
{
	char s[32],s2[32];
	char b;

	shopSpr->GetSprite(33)->DrawGhost(x,250,GetDisplayMGL(),0);

	if(player.nightmare || BrutalMode())
	{
		int cx = 13;
		if (ClassicMode())
		{
			shopSpr->GetSprite(28)->DrawColored(x - 50, 5, GetDisplayMGL(), 0, 4);
			sprintf(s, "%02d", player.shieldStones);
			CenterPrint(x - 35+cx - 1, 21 - 1, s, -32, 2);
			CenterPrint(x - 35+cx + 1, 21 + 1, s, -32, 2);
			CenterPrint(x - 35+cx, 21, s, 0, 2);
			shopSpr->GetSprite(28)->DrawColored(x + 30, 5, GetDisplayMGL(), 4, 4);
			sprintf(s, "%02d", player.powerStones);
			CenterPrint(x + 45+cx - 1, 21 - 1, s, -32, 2);
			CenterPrint(x + 45+cx + 1, 21 + 1, s, -32, 2);
			CenterPrint(x + 45+cx, 21, s, 0, 2);
			shopSpr->GetSprite(28)->DrawColored(x + 110, 5, GetDisplayMGL(), 1, 4);
			sprintf(s, "%02d", player.spellStones);
			CenterPrint(x + 125+cx - 1, 21 - 1, s, -32, 2);
			CenterPrint(x + 125+cx + 1, 21 + 1, s, -32, 2);
			CenterPrint(x + 125+cx, 21, s, 0, 2);
		}
		else
		{
			shopSpr->GetSprite(28)->DrawColored(x - 60, 5, GetDisplayMGL(), 0, 4);
			sprintf(s, "%02d", player.shieldStones);
			CenterPrint(x - 45+cx - 1, 21 - 1, s, -32, 2);
			CenterPrint(x - 45+cx + 1, 21 + 1, s, -32, 2);
			CenterPrint(x - 45+cx, 21, s, 0, 2);
			shopSpr->GetSprite(28)->DrawColored(x + 0, 5, GetDisplayMGL(), 4, 4);
			sprintf(s, "%02d", player.powerStones);
			CenterPrint(x + 15+cx - 1, 21 - 1, s, -32, 2);
			CenterPrint(x + 15+cx + 1, 21 + 1, s, -32, 2);
			CenterPrint(x + 15+cx, 21, s, 0, 2);
			shopSpr->GetSprite(28)->DrawColored(x + 60, 5, GetDisplayMGL(), 1, 4);
			sprintf(s, "%02d", player.spellStones);
			CenterPrint(x + 75+cx - 1, 21 - 1, s, -32, 2);
			CenterPrint(x + 75+cx + 1, 21 + 1, s, -32, 2);
			CenterPrint(x + 75+cx, 21, s, 0, 2);
			shopSpr->GetSprite(28)->DrawColored(x + 120, 5, GetDisplayMGL(), 7, 4);
			sprintf(s, "%02d", player.skillStones);
			CenterPrint(x + 135+cx - 1, 21 - 1, s, -32, 2);
			CenterPrint(x + 135+cx + 1, 21 + 1, s, -32, 2);
			CenterPrint(x + 135+cx, 21, s, 0, 2);
		}
	}
	// hat

	if (player.gear & GEAR_POINTY)
	{
		if (player.gear & GEAR_MAGNET)
			sprintf(s, "Pointy Magnetic");
		else
			sprintf(s, "Pointy");
	}
	else if (player.gear & GEAR_MAGNET)
		sprintf(s, "Magnetic");
	else
		s[0] = '\0';
	RenderGearSlot(x - 80, 50, s, hatName[player.hat], 13 + player.hat);

	if (player.gear & GEAR_HEART)
	{
		if (player.gear & GEAR_SOUL)
			RenderGearSlot(x - 80, 106, "Heart & Soul", "Pendant", 26);
		else
			RenderGearSlot(x - 80, 106, "", "Heart Pendant", 18);
	}
	else if (player.gear & GEAR_SOUL)
		RenderGearSlot(x - 80, 106, "", "Soul Pendant", 25);

	if (player.gear & GEAR_MOON)
	{
		if (player.gear & GEAR_SUN)
			RenderGearSlot(x - 80, 162, "Sun & Moon", "Amulet", 24);
		else
			RenderGearSlot(x - 80, 162, "", "Moon Amulet", 19);
	}
	else if (player.gear & GEAR_SUN)
		RenderGearSlot(x - 80, 162, "", "Sun Amulet", 23);

	if (player.gear & GEAR_LAMP)
	{
		if (player.gear & GEAR_WISDOM)
			RenderGearSlot(x - 80, 218, "Lantern Of", "Wisdom", 22);
		else
			RenderGearSlot(x - 80, 218, "", "Lantern", 20);
	}
	else if (player.gear & GEAR_WISDOM)
		RenderGearSlot(x - 80, 218, "", "Light Of Wisdom", 21);

	s[0] = '\0';
	if (player.gear & GEAR_BOUNCY)
	{
		if (player.gear & GEAR_COMPASS)
			sprintf(s, "Bouncy Seekin'");
		else
			sprintf(s, "Bouncy");
	}
	else if (player.gear & GEAR_COMPASS)
		sprintf(s, "Seekin'");
	if (PlayerHasSword())
		RenderGearSlot(x-80, 274, s, "Armageddon Swd", 35);
	else
		RenderGearSlot(x-80, 274, s, staffName[player.staff], 3 + player.staff);

	s[0] = '\0';
	if (player.gear & GEAR_FEATHER)
	{
		if (player.gear & GEAR_SOCKS)
		{
			if (Challenging())
				sprintf(s, "Tickin' Shootin'");
			else
				sprintf(s, "Chicken Shootin'");
		}
		else
		{
			if (Challenging())
				sprintf(s, "Timely");
			else
				sprintf(s, "Feathered");
		}
	}
	else if (player.gear & GEAR_SOCKS)
		sprintf(s, "Shootin'");
	RenderGearSlot(x-80, 330, s, bootName[player.boots], 8 + player.boots);

	// life
	if(!ClassicMode() && player.barrier>0)
		sprintf(s,"Life: %03d/%03d +%d",player.life,player.maxLife,player.barrier);
	else
		sprintf(s, "Life: %03d/%03d", player.life, player.maxLife);
	Print(x-80+2,384+2,s,1,2);
	Print(x-80,384,s,0,2);
	// runestones
	if (!ClassicMode())
	{
		BlitIconBit(2, 467, 13, 477, x-80+2+180,384+5, 255,-31);
		BlitIconBit(2, 467, 13, 477, x - 80 + 180, 384+5, 255, 0);
		sprintf(s, "%d", player.runeStones);
		Print(x - 80 + 2 + 180 + 16, 384 + 2, s, 1, 2);
		Print(x - 80 + 180 + 16, 384, s, 0, 2);
	}
	// mana
	sprintf(s,"Mana: %03d/%03d",player.mana,player.maxMana);
	Print(x-80+2,404+2,s,1,2);
	Print(x-80,404,s,0,2);
	// experience
	if(player.level<50)
	{
		sprintf(s,"XP: %04d/%04d",player.experience,player.needExp);
		Print(x-80+2,424+2,s,1,2);
		Print(x-80,424,s,0,2);
	}
	else
	{
		sprintf(s,"XP now gives coins!");
		Print(x-80+2,424+2,s,1,2);
		Print(x-80,424,s,0,2);
	}
	// money
	sprintf(s,"Money: $%d",TotalMoney());
	Print(x-80+2,444+2,s,1,2);
	Print(x-80,444,s,0,2);

	// armageddon stuff
	b=abs(((brt/2)&31)-16)-8;
	GetIntfaceSpr(67)->Draw(x+175,476,GetDisplayMGL());
	if(player.swordPiece[0])
		GetIntfaceSpr(63)->DrawBright(x+175,476,GetDisplayMGL(),b);
	if(player.swordPiece[3])
		GetIntfaceSpr(64)->DrawBright(x+175,476,GetDisplayMGL(),b);
	if(player.swordPiece[1])
		GetIntfaceSpr(65)->DrawBright(x+175,476,GetDisplayMGL(),b);
	if(player.swordPiece[2])
		GetIntfaceSpr(66)->DrawBright(x+175,476,GetDisplayMGL(),b);
}

byte Owned(byte w)
{
	if((player.nightmare || BrutalMode()) && w == 0)
		return (player.shieldStones==99);
	else if((player.nightmare || BrutalMode())  && w == 5)
		return (player.powerStones==99);
	else if((player.nightmare || BrutalMode()) && w == 10)
		return (player.spellStones==99);
	else if ((player.nightmare || BrutalMode()) && w == 15 && !ClassicMode())
		return (player.skillStones == 99);
	else if(w>=0 && w<=3)
		return (player.hat>=w+1);
	else if(w>=5 && w<=8)
		return (player.staff>=w-4);
	else if(w>=10 && w<=13)
		return (player.boots>=w-9);
	else
	{
		return ((player.gear&(gearTag[w]))!=0);
	}

	return 0;
}

void RenderShop(void)
{
	int i;
	int j;
	MGLDraw *mgl;
	char s[32];

	mgl=GetDisplayMGL();

	memcpy(mgl->GetScreen(),shopScr,SCRWID*SCRHEI);

	int SHOPX = VeryClassicMode() ? 30 : 10;
	int SHOPY = VeryClassicMode() ? 51 : 40;
	int SHOPDX = VeryClassicMode() ? 80 : 70;
	int SHOPDY = VeryClassicMode() ? 80 : 65;
	int shopSize = VeryClassicMode() ? 4 : 5;

	for(i=0;i<shopSize;i++)
		for(j=0;j<shopSize;j++)
		{
			if(player.nightmare || BrutalMode())
			{
				shopSpr->GetSprite(0 + Owned(i+j*5))->Draw((i)*SHOPDX+SHOPX,SHOPY+j*SHOPDY,mgl);
				if(i==0 && j==0)	// shield stone
				{
					shopSpr->GetSprite(28)->DrawColored((i)*SHOPDX+SHOPX,SHOPY+j*SHOPDY,mgl,0,4);
				}
				else if(i==0 && j==1)	// power stone
				{
					shopSpr->GetSprite(28)->DrawColored((i)*SHOPDX+SHOPX,SHOPY+j*SHOPDY,mgl,4,4);
				}
				else if(i==0 && j==2)	// spell stone
				{
					shopSpr->GetSprite(28)->DrawColored((i)*SHOPDX+SHOPX,SHOPY+j*SHOPDY,mgl,1,4);
				}
				else if (i == 0 && j == 3 && !ClassicMode())	// skill stone
				{
					shopSpr->GetSprite(28)->DrawColored((i)*SHOPDX + SHOPX, SHOPY + j * SHOPDY, mgl, 7, 4);
				}
				else if (i == 4 && j == 2 && Challenging()) // hourglass
				{
					shopSpr->GetSprite(38)->Draw((i)*SHOPDX + SHOPX, SHOPY + j * SHOPDY, mgl);
				}
				else
				{
					if(shopPic[i+j*5]!=0)
						shopSpr->GetSprite(shopPic[i+j*5])->Draw((i)*SHOPDX+SHOPX,SHOPY+j*SHOPDY,mgl);
					else // exit
						PrintGlow(SHOPDX*i+SHOPX+8,SHOPY+SHOPDY*j+18,"Exit",2);
				}
			}
			else
			{
				shopSpr->GetSprite(0 + Owned(i + j * 5))->Draw((i)*SHOPDX + SHOPX, SHOPY + j * SHOPDY, mgl);
				if (i == 4 && j == 2 && Challenging())	// hourglass
					shopSpr->GetSprite(38)->Draw((i)*SHOPDX + SHOPX, SHOPY + j * SHOPDY, mgl);
				else if(shopPic[i+j*5]!=0 && !(VeryClassicMode() && i == 3 && j == 3))
					shopSpr->GetSprite(shopPic[i+j*5])->Draw((i)*SHOPDX+SHOPX,SHOPY+j*SHOPDY,mgl);
				else // exit
					PrintGlow(SHOPDX*i+SHOPX+8,SHOPY+SHOPDY*j+18,"Exit",2);
			}
		}

	// cursor
	shopSpr->GetSprite(2)->Draw((shopCursor-(shopCursor/5*5))*SHOPDX+SHOPX,(shopCursor/5)*SHOPDY+SHOPY,mgl);

	RenderPlayerGear(450,armaBrt);
	Print(2,2,"ITEM SHOP",0,0);

	if(shopCursor==14 && Challenging())
		Print(5, 100 + 5 * 56 - 20, "Magic Hourglass", 0, 2);
	else if(player.nightmare || BrutalMode())
	{
		if(shopCursor==0)
			Print(5,100+5*56-20,"Shield Stone",0,2);
		else if(shopCursor==5)
			Print(5,100+5*56-20,"Power Stone",0,2);
		else if(shopCursor==10)
			Print(5,100+5*56-20,"Spell Stone",0,2);
		else if (shopCursor == 15 && !ClassicMode())
			Print(5, 100 + 5 * 56 - 20, "Skill Stone", 0, 2);
		else
			Print(5,100+5*56-20,shopName[(shopCursor)],0,2);
	}
	else if (VeryClassicMode() && shopCursor == 18)
		Print(5,100+5*56-20,shopName[24],0,2);
	else
		Print(5,100+5*56-20,shopName[(shopCursor)],0,2);
	if(shopCursor != (VeryClassicMode() ? 18 : 24))
	{
		if(Owned(shopCursor))
			sprintf(s,"N/A");
		else
		{
			if(player.nightmare || BrutalMode())
			{
				if(player.fairyOn==FAIRY_HAGGLY)
					sprintf(s,"$%d",NightmarePrice(shopCursor)*9/10);
				else
					sprintf(s,"$%d",NightmarePrice(shopCursor));
			}
			else
			{
				if(player.fairyOn==FAIRY_HAGGLY)
					sprintf(s,"$%d",Price(shopCursor)*9/10);
				else
					sprintf(s,"$%d",Price(shopCursor));
			}
		}
		RightPrintGlow(10+345,100+5*56-20,s,2);
	}

	if(Challenging() && shopCursor == 14)
		i = 29;
	else if(player.nightmare || BrutalMode())
	{
		if (shopCursor == 0)
			i = 25;
		else if (shopCursor == 5)
			i = 26;
		else if (shopCursor == 10)
			i = 27;
		else if (shopCursor == 15 && !ClassicMode())
			i = 28;
		else
			i = shopCursor;
	}
	else if (VeryClassicMode() && shopCursor == 18)
		i = 24;
	else
		i = shopCursor;
	if (!ClassicMode() && i >= 25 && i <= 27)
		i += 5;	// in modern mode, the description of the 3 % upgrade stones changes
	PrintBrightGlow(5,110+5*56,itemDesc[i][0],-4,2);
	PrintBrightGlow(5,110+20+5*56,itemDesc[i][1],-4,2);
	PrintBrightGlow(5,110+40+5*56,itemDesc[i][2],-4,2);
	PrintBrightGlow(5,110+60+5*56,itemDesc[i][3],-4,2);
}

void BuyGear(dword w)
{
	player.gear|=w;
	switch(w)
	{
		case GEAR_HEART:
			PlayerUpdateLife();
			PlayerHeal(128);
			return;
		case GEAR_MOON:
			PlayerUpdateLife();
			return;
		default:
			return;
	}
}

void CheckStonePurchases(void)
{
	if (player.shieldStones == 99 && player.powerStones == 99 && player.spellStones == 99)
	{
		if (!ClassicMode() && player.skillStones < 99)
			return;
		EarnAchieve(Achievement::MASTER_MADCAP);
	}
}

void Buy(byte which)
{
	int prc;

	if((player.nightmare || BrutalMode())  && shopCursor == 0)
	{
		prc=NightmarePrice(shopCursor);
		if(player.fairyOn==FAIRY_HAGGLY)
			prc=prc*9/10;
		if(player.shieldStones==99)
		{
			MakeNormalSound(SND_UNAVAILABLE);
			return;
		}
		GainMoney(-prc);
		player.shieldStones++;
		MakeNormalSound(SND_PURCHASE);
		CheckStonePurchases();
		return;
	}
	else if((player.nightmare || BrutalMode()) && shopCursor == 5)
	{
		prc=NightmarePrice(shopCursor);
		if(player.fairyOn==FAIRY_HAGGLY)
			prc=prc*9/10;
		if(player.powerStones==99)
		{
			MakeNormalSound(SND_UNAVAILABLE);
			return;
		}
		GainMoney(-prc);
		player.powerStones++;
		MakeNormalSound(SND_PURCHASE);
		CheckStonePurchases();
		return;
	}
	else if((player.nightmare || BrutalMode()) && shopCursor == 10)
	{
		prc=NightmarePrice(shopCursor);
		if(player.fairyOn==FAIRY_HAGGLY)
			prc=prc*9/10;
		if(player.spellStones==99)
		{
			MakeNormalSound(SND_UNAVAILABLE);
			return;
		}
		GainMoney(-prc);
		player.spellStones++;
		MakeNormalSound(SND_PURCHASE);
		CheckStonePurchases();
		return;
	}
	else if ((player.nightmare || BrutalMode()) && shopCursor == 15 && !ClassicMode())
	{
		prc = NightmarePrice(shopCursor);
		if (player.fairyOn == FAIRY_HAGGLY)
			prc = prc * 9 / 10;
		if (player.skillStones == 99)
		{
			MakeNormalSound(SND_UNAVAILABLE);
			return;
		}
		GainMoney(-prc);
		player.skillStones++;
		player.skillPts++;
		MakeNormalSound(SND_PURCHASE);
		CheckStonePurchases();
		return;
	}

	prc=Price(shopCursor);
	if(player.fairyOn==FAIRY_HAGGLY)
		prc=prc*9/10;

	if(gearTag[which]==0)
	{
		if(which<4)	// hats
		{
			if(which+1>player.hat)
			{
				GainMoney(-prc);
				player.hat=which+1;
				SetPlayerDefense();
				MakeNormalSound(SND_PURCHASE);
				return;
			}
		}
		else if(which<9) //staves
		{
			if(which-4>player.staff)
			{
				GainMoney(-prc);
				player.staff=which-4;
				SetPlayerDamage();
				MakeNormalSound(SND_PURCHASE);
				return;
			}
		}
		else if(which<14) // boots
		{
			if(which-9>player.boots)
			{
				GainMoney(-prc);
				player.boots=which-9;
				SetPlayerSpeed();
				MakeNormalSound(SND_PURCHASE);
				return;
			}
		}
	}
	else
	{
		if((player.gear&gearTag[which])==0)
		{
			GainMoney(-prc);
			BuyGear(gearTag[which]);
			MakeNormalSound(SND_PURCHASE);
			return;
		}
	}
	// couldn't buy, make a buzz
	MakeNormalSound(SND_UNAVAILABLE);
}

byte UpdateShop(MGLDraw *mgl)
{
	dword c;
	int prc;

	armaBrt++;

	int shopWidth = VeryClassicMode() ? 4 : 5;
	int shopTotal = VeryClassicMode() ? 20 : 25;

	if(AutoRepeatTapped(CONTROL_UP))
	{
		MakeNormalSound(SND_MENUCLICK);
		shopCursor-=5;
		if(shopCursor>=shopTotal)
			shopCursor+=shopTotal;
	}
	if(AutoRepeatTapped(CONTROL_DN))
	{
		MakeNormalSound(SND_MENUCLICK);
		shopCursor+=5;
		if(shopCursor>=shopTotal)
			shopCursor-=shopTotal;
	}
	if(AutoRepeatTapped(CONTROL_LF))
	{
		MakeNormalSound(SND_MENUCLICK);
		if (shopCursor % 5 == 0)
			shopCursor += shopWidth - 1;
		else
			shopCursor--;
	}
	if(AutoRepeatTapped(CONTROL_RT))
	{
		MakeNormalSound(SND_MENUCLICK);
		if (shopCursor % 5 == shopWidth - 1)
			shopCursor -= shopWidth - 1;
		else
			shopCursor++;
	}
	if(ButtonTapped(CONTROL_B1))
	{
		if (shopCursor == (VeryClassicMode() ? 18 : 24))
			return 1;	// exit

		if(player.nightmare || BrutalMode())
		{
			prc=NightmarePrice(shopCursor);
			if(player.fairyOn==FAIRY_HAGGLY)
				prc=prc*9/10;
		}
		else
		{
			prc=Price(shopCursor);
			if(player.fairyOn==FAIRY_HAGGLY)
				prc=prc*9/10;
		}

		if(CanAffordMoney(prc))
			Buy(shopCursor);
		else
			MakeNormalSound(SND_UNAVAILABLE);
	}

	if (ButtonTapped(CONTROL_ESCAPE | CONTROL_B2))
	{
		UpdateControls();
		LockOutControl(CONTROL_ESCAPE | CONTROL_B2, true);
		return 1;
	}
	return 0;
}

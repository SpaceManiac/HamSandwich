#include "shop.h"
#include "display.h"
#include "player.h"
#include "sound.h"
#include "fairy.h"

#define SHOPX 10
#define SHOPY 40
#define SHOPDX 70
#define SHOPDY 65

sprite_set_t *shopSpr;
static byte armaBrt=0;

char hatName[5][16]={
	"Paper Hat",
	"Felt Hat",
	"Moon Hat",
	"Checkered Hat",
	"Steel Hat"
};

char staffName[5][16]={
	"Balsa Staff",
	"Pine Staff",
	"Oak Staff",
	"Ironwood Staff",
	"Mystical Staff"
};

char bootName[5][18]={
	"Orthopedic Boots",
	"Leather Boots",
	"Doc Merlins",
	"Lucky Boots",
	"Air Lancelots"
};

char otherName[12][18]={
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

char shopName[28][18]={
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
	"Spell Stone"};

word price[25]={30, 120, 400,2000,1000, // hats (and a bonus)
				70, 180, 600,3500,400, // staves (and a bonus)
				20, 100, 250,2000,350, // boots (and a bonus)
				800,1200,220,2200,700, // heart moon lantern, bonus bonus
				900,750,1500,450, 0};  // heart2, moon2, lantern2, bonus, exit

word shopPic[25]={14,15,16,17,32, // hats (and pointy)
				  4, 5, 6, 7, 27, // staves (and compass)
				  9, 10,11,12,29, // boots (and feathers)
				  18,19,20,28,30, // heart moon lantern, crystal socks
				  25,23,21,31,0}; // heart2, moon2, lantern2, headband, exit

dword gearTag[]={0,0,0,0,GEAR_POINTY,
		        0,0,0,0,GEAR_COMPASS,
				0,0,0,0,GEAR_FEATHER,
				GEAR_HEART,GEAR_MOON,GEAR_LAMP,GEAR_BOUNCY,GEAR_SOCKS,
				GEAR_SOUL,GEAR_SUN,GEAR_WISDOM,GEAR_MAGNET,0};

char itemDesc[28*4][64]={
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
	"these stones.  Each one will",
	"block 1% of the damage you",
	"take.",

	"You can collect up to 99 of",
	"these stones.  Each one will",
	"add 10% to your fireball",
	"damage.",

	"You can collect up to 99 of",
	"these stones.  Each one will",
	"add 10% to your spell",
	"damage.",
};

byte shopCursor=0;
byte *shopScr;
static byte oldc=0;
static byte reptCounter=0;

void InitShop(void)
{
	shopSpr=new sprite_set_t("graphics\\shop.jsp");
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
	GetDisplayMGL()->LoadBMP("graphics\\shop.bmp");
	shopScr=(byte *)malloc(640*480);
	if(!shopScr)
		FatalError("Out of memory!!");
	memcpy(shopScr,GetDisplayMGL()->GetScreen(),640*480);
	oldc=255;
	GetDisplayMGL()->LastKeyPressed();
	reptCounter=3;
}

void LeaveShop(void)
{
	free(shopScr);
}

word NightmarePrice(byte which)
{
	if(which==0)	// shield stones
		return 200*(player.shieldStones+2);
	else if(which==5)	// power stones
		return 200*(player.powerStones+2);
	else if(which==10)
		return 200*(player.spellStones+2);
	else
		return price[which];
}

void RenderPlayerGear(int x,byte brt)
{
	char s[32];
	char b;

	shopSpr->GetSprite(33)->DrawGhost(x,250,GetDisplayMGL(),0);

	if(player.nightmare)
	{
		shopSpr->GetSprite(28)->DrawColored(x-50,5,GetDisplayMGL(),0,4);
		sprintf(s,"%02d",player.shieldStones);
		Print(x-32-1,21-1,s,-32,2);
		Print(x-32+1,21+1,s,-32,2);
		Print(x-32,21,s,0,2);
		shopSpr->GetSprite(28)->DrawColored(x+30,5,GetDisplayMGL(),4,4);
		sprintf(s,"%02d",player.powerStones);
		Print(x+48-1,21-1,s,-32,2);
		Print(x+48+1,21+1,s,-32,2);
		Print(x+48,21,s,0,2);
		shopSpr->GetSprite(28)->DrawColored(x+110,5,GetDisplayMGL(),1,4);
		sprintf(s,"%02d",player.spellStones);
		Print(x+128-1,21-1,s,-32,2);
		Print(x+128+1,21+1,s,-32,2);
		Print(x+128,21,s,0,2);
	}
	// hat
	shopSpr->GetSprite(1)->Draw(x-80,50,GetDisplayMGL());
	shopSpr->GetSprite(13+player.hat)->Draw(x-80,50,GetDisplayMGL());
	if(player.gear&(GEAR_POINTY|GEAR_MAGNET))
	{
		if(player.gear&GEAR_POINTY)
		{
			if(player.gear&GEAR_MAGNET)
			{
				Print(x-18,62,"Pointy Magnetic",1,2);
				Print(x-20,60,"Pointy Magnetic",0,2);
			}
			else
			{
				Print(x-18,62,"Pointy",1,2);
				Print(x-20,60,"Pointy",0,2);
			}
		}
		else
		{
			Print(x-18,62,"Magnetic",1,2);
			Print(x-20,60,"Magnetic",0,2);
		}
		Print(x-18,82,hatName[player.hat],1,2);
		Print(x-20,80,hatName[player.hat],0,2);
	}
	else
	{
		Print(x-18,72,hatName[player.hat],1,2);
		Print(x-20,70,hatName[player.hat],0,2);
	}

	// heart pendant
	if((player.gear&GEAR_HEART) && !(player.gear&GEAR_SOUL))
	{
		shopSpr->GetSprite(1)->Draw(x-80,106,GetDisplayMGL());
		shopSpr->GetSprite(18)->Draw(x-80,106,GetDisplayMGL());
		Print(x-18,128,"Heart Pendant",1,2);
		Print(x-20,126,"Heart Pendant",0,2);
	}
	else if(!(player.gear&GEAR_HEART) && (player.gear&GEAR_SOUL))
	{
		shopSpr->GetSprite(1)->Draw(x-80,106,GetDisplayMGL());
		shopSpr->GetSprite(25)->Draw(x-80,106,GetDisplayMGL());
		Print(x-18,128,"Soul Pendant",1,2);
		Print(x-20,126,"Soul Pendant",0,2);
	}
	else if((player.gear&GEAR_HEART) && (player.gear&GEAR_SOUL))
	{
		shopSpr->GetSprite(1)->Draw(x-80,106,GetDisplayMGL());
		shopSpr->GetSprite(26)->Draw(x-80,106,GetDisplayMGL());
		Print(x-18,114,"Heart & Soul",1,2);
		Print(x-18,134,"Pendant",1,2);
		Print(x-20,112,"Heart & Soul",0,2);
		Print(x-20,132,"Pendant",0,2);
	}

	// moon amulet
	if((player.gear&GEAR_MOON) && !(player.gear&GEAR_SUN))
	{
		shopSpr->GetSprite(1)->Draw(x-80,162,GetDisplayMGL());
		shopSpr->GetSprite(19)->Draw(x-80,162,GetDisplayMGL());
		Print(x-18,184,"Moon Amulet",1,2);
		Print(x-20,182,"Moon Amulet",0,2);
	}
	else if(!(player.gear&GEAR_MOON) && (player.gear&GEAR_SUN))
	{
		shopSpr->GetSprite(1)->Draw(x-80,162,GetDisplayMGL());
		shopSpr->GetSprite(23)->Draw(x-80,162,GetDisplayMGL());
		Print(x-18,184,"Sun Amulet",1,2);
		Print(x-20,182,"Sun Amulet",0,2);
	}
	else if((player.gear&GEAR_MOON) && (player.gear&GEAR_SUN))
	{
		shopSpr->GetSprite(1)->Draw(x-80,162,GetDisplayMGL());
		shopSpr->GetSprite(24)->Draw(x-80,162,GetDisplayMGL());
		Print(x-18,170,"Sun & Moon",1,2);
		Print(x-18,190,"Amulet",1,2);

		Print(x-20,168,"Sun & Moon",0,2);
		Print(x-20,188,"Amulet",0,2);
	}

	// lantern
	if((player.gear&GEAR_LAMP) && !(player.gear&GEAR_WISDOM))
	{
		shopSpr->GetSprite(1)->Draw(x-80,218,GetDisplayMGL());
		shopSpr->GetSprite(20)->Draw(x-80,218,GetDisplayMGL());
		Print(x-18,240,"Lantern",1,2);
		Print(x-20,238,"Lantern",0,2);
	}
	else if(!(player.gear&GEAR_LAMP) && (player.gear&GEAR_WISDOM))
	{
		shopSpr->GetSprite(1)->Draw(x-80,218,GetDisplayMGL());
		shopSpr->GetSprite(21)->Draw(x-80,218,GetDisplayMGL());
		Print(x-18,240,"Light Of Wisdom",1,2);
		Print(x-20,238,"Light Of Wisdom",0,2);
	}
	else if((player.gear&GEAR_LAMP) && (player.gear&GEAR_WISDOM))
	{
		shopSpr->GetSprite(1)->Draw(x-80,218,GetDisplayMGL());
		shopSpr->GetSprite(22)->Draw(x-80,218,GetDisplayMGL());
		Print(x-18,226,"Lantern Of",1,2);
		Print(x-18,246,"Wisdom",1,2);

		Print(x-20,224,"Lantern Of",0,2);
		Print(x-20,244,"Wisdom",0,2);
	}
	// staff
	shopSpr->GetSprite(1)->Draw(x-80,274,GetDisplayMGL());
	if(player.gear&(GEAR_BOUNCY|GEAR_COMPASS))
	{
		if(player.gear&GEAR_BOUNCY)
		{
			if(player.gear&GEAR_COMPASS)
			{
				Print(x-18,286,"Bouncy Seekin'",1,2);
				Print(x-20,284,"Bouncy Seekin'",0,2);
			}
			else
			{
				Print(x-18,286,"Bouncy",1,2);
				Print(x-20,284,"Bouncy",0,2);
			}
		}
		else
		{
			Print(x-18,286,"Seekin'",1,2);
			Print(x-20,284,"Seekin'",0,2);
		}

		if(PlayerHasSword())
		{
			shopSpr->GetSprite(35)->Draw(x-80,274,GetDisplayMGL());
			Print(x-18,306,"Armageddon Swd",1,2);
			Print(x-20,304,"Armageddon Swd",0,2);
		}
		else
		{
			shopSpr->GetSprite(3+player.staff)->Draw(x-80,274,GetDisplayMGL());
			Print(x-18,306,staffName[player.staff],1,2);
			Print(x-20,304,staffName[player.staff],0,2);
		}
	}
	else
	{
		if(PlayerHasSword())
		{
			shopSpr->GetSprite(35)->Draw(x-80,274,GetDisplayMGL());
			Print(x-18,296,"Armageddon Swd",1,2);
			Print(x-20,294,"Armageddon Swd",0,2);
		}
		else
		{
			shopSpr->GetSprite(3+player.staff)->Draw(x-80,274,GetDisplayMGL());
			Print(x-18,296,staffName[player.staff],1,2);
			Print(x-20,294,staffName[player.staff],0,2);
		}
	}
	// boots
	shopSpr->GetSprite(1)->Draw(x-80,330,GetDisplayMGL());
	shopSpr->GetSprite(8+player.boots)->Draw(x-80,330,GetDisplayMGL());
	if(player.gear&(GEAR_FEATHER|GEAR_SOCKS))
	{
		if(player.gear&GEAR_SOCKS)
		{
			if(player.gear&GEAR_FEATHER)
			{
				Print(x-18,342,"Chicken Shootin'",1,2);
				Print(x-20,340,"Chicken Shootin'",0,2);
			}
			else
			{
				Print(x-18,342,"Shootin'",1,2);
				Print(x-20,340,"Shootin'",0,2);
			}
		}
		else
		{
			Print(x-18,342,"Feathered",1,2);
			Print(x-20,340,"Feathered",0,2);
		}
		Print(x-18,362,bootName[player.boots],1,2);
		Print(x-20,360,bootName[player.boots],0,2);
	}
	else
	{
		Print(x-18,352,bootName[player.boots],1,2);
		Print(x-20,350,bootName[player.boots],0,2);
	}

	// life
	sprintf(s,"Life: %03d/%03d",player.life,player.maxLife);
	Print(x-80+2,384+2,s,1,2);
	Print(x-80,384,s,0,2);
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
	sprintf(s,"Money: $%d",player.money);
	Print(x-80+2,444+2,s,1,2);
	Print(x-80,444,s,0,2);

	// armageddon stuff
	b=abs(((brt/2)&31)-16)-8;
	GetIntfaceSpr(67)->Draw(x+175,476,GetDisplayMGL());
	if(player.keychain[0])
		GetIntfaceSpr(63)->DrawBright(x+175,476,GetDisplayMGL(),b);
	if(player.keychain[3])
		GetIntfaceSpr(64)->DrawBright(x+175,476,GetDisplayMGL(),b);
	if(player.keychain[1])
		GetIntfaceSpr(65)->DrawBright(x+175,476,GetDisplayMGL(),b);
	if(player.keychain[2])
		GetIntfaceSpr(66)->DrawBright(x+175,476,GetDisplayMGL(),b);
}

byte Owned(byte w)
{
	if(player.nightmare && w==0)
		return (player.shieldStones==99);
	else if(player.nightmare && w==5)
		return (player.powerStones==99);
	else if(player.nightmare && w==10)
		return (player.spellStones==99);
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

	memcpy(mgl->GetScreen(),shopScr,640*480);

	for(i=0;i<5;i++)
		for(j=0;j<5;j++)
		{
			if(player.nightmare)
			{
				shopSpr->GetSprite(0+Owned(i+j*5))->Draw((i)*SHOPDX+SHOPX,SHOPY+j*SHOPDY,mgl);
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
				shopSpr->GetSprite(0+Owned(i+j*5))->Draw((i)*SHOPDX+SHOPX,SHOPY+j*SHOPDY,mgl);
				if(shopPic[i+j*5]!=0)
					shopSpr->GetSprite(shopPic[i+j*5])->Draw((i)*SHOPDX+SHOPX,SHOPY+j*SHOPDY,mgl);
				else // exit
					PrintGlow(SHOPDX*i+SHOPX+8,SHOPY+SHOPDY*j+18,"Exit",2);
			}
		}

	// cursor
	shopSpr->GetSprite(2)->Draw((shopCursor-(shopCursor/5*5))*SHOPDX+SHOPX,(shopCursor/5)*SHOPDY+SHOPY,mgl);

	RenderPlayerGear(450,armaBrt);
	Print(2,2,"ITEM SHOP",0,0);

	if(player.nightmare)
	{
		if(shopCursor==0)
			Print(5,100+5*56-20,"Shield Stone",0,2);
		else if(shopCursor==5)
			Print(5,100+5*56-20,"Power Stone",0,2);
		else if(shopCursor==10)
			Print(5,100+5*56-20,"Spell Stone",0,2);
		else
			Print(5,100+5*56-20,shopName[(shopCursor)],0,2);
	}
	else
		Print(5,100+5*56-20,shopName[(shopCursor)],0,2);
	if(shopCursor<24)
	{
		if(Owned(shopCursor))
			sprintf(s,"N/A");
		else
		{
			if(player.nightmare)
			{
				if(player.fairyOn==FAIRY_HAGGLY)
					sprintf(s,"$%d",NightmarePrice(shopCursor)*9/10);
				else
					sprintf(s,"$%d",NightmarePrice(shopCursor));
			}
			else
			{
				if(player.fairyOn==FAIRY_HAGGLY)
					sprintf(s,"$%d",price[shopCursor]*9/10);
				else
					sprintf(s,"$%d",price[shopCursor]);
			}
		}
		RightPrintGlow(10+345,100+5*56-20,s,2);
	}

	if(player.nightmare)
	{
		if(shopCursor==0)
			i=25*4;
		else if(shopCursor==5)
			i=26*4;
		else if(shopCursor==10)
			i=27*4;
		else
			i=shopCursor*4;
	}
	else
		i=shopCursor*4;
	PrintBrightGlow(5,110+5*56,itemDesc[i],-4,2);
	PrintBrightGlow(5,110+20+5*56,itemDesc[i+1],-4,2);
	PrintBrightGlow(5,110+40+5*56,itemDesc[i+2],-4,2);
	PrintBrightGlow(5,110+60+5*56,itemDesc[i+3],-4,2);
}

void BuyGear(dword w)
{
	player.gear|=w;
	switch(w)
	{
		case GEAR_HEART:
			player.maxLife=28+player.level*2;
			PlayerHeal(128);
			return;
		case GEAR_MOON:
			player.maxMana=28+player.level*2;
			player.mana=28+player.level*2;
			return;
		default:
			return;
	}
}

void Buy(byte which)
{
	int prc;

	if(player.nightmare && shopCursor==0)
	{
		prc=NightmarePrice(shopCursor);
		if(player.fairyOn==FAIRY_HAGGLY)
			prc=prc*9/10;
		if(player.shieldStones==99)
		{
			MakeNormalSound(SND_UNAVAILABLE);
			return;
		}
		player.money-=prc;
		player.shieldStones++;
		MakeNormalSound(SND_PURCHASE);
		return;
	}
	else if(player.nightmare && shopCursor==5)
	{
		prc=NightmarePrice(shopCursor);
		if(player.fairyOn==FAIRY_HAGGLY)
			prc=prc*9/10;
		if(player.powerStones==99)
		{
			MakeNormalSound(SND_UNAVAILABLE);
			return;
		}
		player.money-=prc;
		player.powerStones++;
		MakeNormalSound(SND_PURCHASE);
		return;
	}
	else if(player.nightmare && shopCursor==10)
	{
		prc=NightmarePrice(shopCursor);
		if(player.fairyOn==FAIRY_HAGGLY)
			prc=prc*9/10;
		if(player.spellStones==99)
		{
			MakeNormalSound(SND_UNAVAILABLE);
			return;
		}
		player.money-=prc;
		player.spellStones++;
		MakeNormalSound(SND_PURCHASE);
		return;
	}

	prc=price[shopCursor];
	if(player.fairyOn==FAIRY_HAGGLY)
		prc=prc*9/10;

	if(gearTag[which]==0)
	{
		if(which<4)	// hats
		{
			if(which+1>player.hat)
			{
				player.money-=prc;
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
				player.money-=prc;
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
				player.money-=prc;
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
			player.money-=prc;
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
	byte c;
	int prc;

	armaBrt++;
	c=GetControls();

	reptCounter++;
	if((!oldc) || (reptCounter>10))
		reptCounter=0;

	if((c&CONTROL_UP) && (!reptCounter))
	{
		MakeNormalSound(SND_MENUCLICK);
		shopCursor-=5;
		if(shopCursor>=25)
			shopCursor+=25;
	}
	if((c&CONTROL_DN) && (!reptCounter))
	{
		MakeNormalSound(SND_MENUCLICK);
		shopCursor+=5;
		if(shopCursor>=25)
			shopCursor-=25;
	}
	if((c&CONTROL_LF) && (!reptCounter))
	{
		MakeNormalSound(SND_MENUCLICK);
		shopCursor--;
		if((shopCursor%5)==4)
			shopCursor+=5;
		if(shopCursor>50)
			shopCursor=4;
	}
	if((c&CONTROL_RT) && (!reptCounter))
	{
		MakeNormalSound(SND_MENUCLICK);
		shopCursor++;
		if((shopCursor%5)==0)
			shopCursor-=5;
	}
	if((c&CONTROL_B1) && (!(oldc&CONTROL_B1)))
	{
		if(shopCursor==24)
			return 1;	// exit

		if(player.nightmare)
		{
			prc=NightmarePrice(shopCursor);
			if(player.fairyOn==FAIRY_HAGGLY)
				prc=prc*9/10;
		}
		else
		{
			prc=price[shopCursor];
			if(player.fairyOn==FAIRY_HAGGLY)
				prc=prc*9/10;
		}

		if(player.money>=prc)
		{
			Buy(shopCursor);
		}
		else
			MakeNormalSound(SND_UNAVAILABLE);
	}
	oldc=c;

	JamulSoundUpdate();

	if(mgl->LastKeyPressed()==27)
		return 1;
	else return 0;
}

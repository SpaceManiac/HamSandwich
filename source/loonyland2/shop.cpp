#include "shop.h"
#include "pause.h"
#include "player.h"
#include "options.h"
#include "quest.h"
#include "skill.h"
#include "equip.h"
#include "services.h"
#include "help.h"
#include "achieve.h"

static byte cursor=0;
static byte shopType,shopMode;
static byte yourMode,yourPage,yourItems,msPick;
static char lastKey=0;
static byte darkness;
static byte oldc;
static char shopName[32];

void RenderYourStuff(MGLDraw *mgl)
{
	int i,j,x,y;
	char s[16];

	x=0;
	y=0;
	PauseBox(202,32,638,298,234);
	PauseBox(202,300,638,378,234);

	ConvertEquipmentForShow(1);
	for(i=0;i<10;i++)
		for(j=0;j<6;j++)
		{
			RenderInvItem(206+i*43,38+j*43,player.items[i+j*10].type,player.items[i+j*10].count,(cursor==i+j*10));
		}
	ConvertEquipmentForShow(0);
	if(cursor<60)
	{
		if(player.items[cursor].type!=IT_NONE)
		{
			DescribeItem(206-208,304-352,&player.items[cursor]);
			if(shopType!=SHOP_STASH)
			{
				if(ItemCost(&player.items[cursor],1)>0)
				{
					sprintf(s,"Sell for $%d",ItemCost(&player.items[cursor],1));
					RightPrint(636,304,s,0,1);
				}
				else
					RightPrint(636,304,"Can't be sold",0,1);
			}
		}
	}
}

void RenderShopStuff(MGLDraw *mgl)
{
	int i,j;
	char s[16];

	if(shopType!=SHOP_BLACKMARKET)
		ConvertShopForShow(1,shopType);
	PauseBox(1,32,200,378,234);
	for(i=0;i<SHOP_WID;i++)
		for(j=0;j<SHOP_HEI;j++)
		{
			RenderInvItem(7+i*43,38+j*43,player.shopInv[shopType][i+j*4].type,player.shopInv[shopType][i+j*4].count,(cursor==(i+j*4+100)));
		}
	if(shopType!=SHOP_BLACKMARKET)
		ConvertShopForShow(0,shopType);
	if(cursor>=100)
	{
		if(player.shopInv[shopType][cursor-100].type!=IT_NONE)
		{
			if(shopType==SHOP_BLACKMARKET)
			{
				if(player.shopInv[shopType][cursor-100].type==IT_MAGICAXE)
					Print(206,304,"Some Kinda Magic Axe",0,1);
				else if(player.shopInv[shopType][cursor-100].type==IT_GOLDENAXE)
					Print(206,304,"Some Kinda Golden Axe",0,1);
				if(player.shopInv[shopType][cursor-100].type==IT_MAGICAMULET)
					Print(206,304,"Some Kinda Magic Amulet",0,1);
				else if(player.shopInv[shopType][cursor-100].type==IT_GOLDENAMULET)
					Print(206,304,"Some Kinda Golden Amulet",0,1);
				if(player.shopInv[shopType][cursor-100].type==IT_MAGICPARKA)
					Print(206,304,"Some Kinda Magic Parka",0,1);
				else if(player.shopInv[shopType][cursor-100].type==IT_GOLDENPARKA)
					Print(206,304,"Some Kinda Golden Parka",0,1);
				Print(206,304+24,"Ya gotta buy it to see the details!",0,1);
				sprintf(s,"Buy for $%d",ItemCost(&player.shopInv[shopType][cursor-100],0)*3);
				RightPrint(636,304,s,0,1);
			}
			else
			{
				DescribeItem(206-208,304-352,&player.shopInv[shopType][cursor-100]);
				if(shopType!=SHOP_STASH)
				{
					sprintf(s,"Buy for $%d",ItemCost(&player.shopInv[shopType][cursor-100],0));
					RightPrint(636,304,s,0,1);
				}
			}
		}
	}
	Print(6,334,"Press directions to select,",0,1);
	Print(6,346,"Fire to buy or sell, or",0,1);
	Print(6,358,"Jump to exit",0,1);
}

void RenderShop(MGLDraw *mgl)
{
	char s[10];

	if(shopType>=SERVICES)
	{
		RenderService(mgl);
		return;
	}

	DarkenScreen(darkness);

	PauseBox(10,0,629,30,234);
	CenterPrintGlow(320,5,shopName,0,0);
	sprintf(s,"$%04d",player.money);
	RightPrintGlow(624,5,s,0,0);

	RenderYourStuff(mgl);
	RenderShopStuff(mgl);

	RenderChatNoName(mgl);
}

void HandleShopKeyPresses(MGLDraw *mgl)
{
	char k;

	k=mgl->LastKeyPressed();
	if(k)
		lastKey=k;
}

void InitShop(byte type)
{
	if(type>=SERVICES)
	{
		InitService(type);
		shopType=type;
		return;
	}

	shopType=type;
	shopMode=0;
	MakeNormalSound(SND_PAUSE);
	lastKey=0;
	cursor=0;
	darkness=0;
	oldc=255;

	switch(shopType)
	{
		case SHOP_BLACKMARKET:
			strcpy(shopName,"The Black Market");
			break;
		case SHOP_AXES:
			strcpy(shopName,"Axes R Us");
			break;
		case SHOP_TAILOR:
			strcpy(shopName,"Taylor's Knits");
			break;
		case SHOP_MAGIC:
			strcpy(shopName,"The Magic Box");
			break;
		case SHOP_ROPES:
			strcpy(shopName,"Ropes n' Ropes");
			break;
		case SHOP_SHROOM:
			strcpy(shopName,"The Black Heart Of Commerce");
			break;
		case SHOP_STASH:
			strcpy(shopName,"Your Friendly Mimic");
			break;
	}
}

byte ButtonPressed(byte btn)
{
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
				if((cursor-100)/4==0)
					cursor+=4*5;
				else
					cursor-=4;
			}
			break;
		case CONTROL_DN:
			if(cursor<60)
			{
				cursor+=10;
				if(cursor>=60)
					cursor-=60;
			}
			else
			{
				if((cursor-100)/4==5)
					cursor-=4*5;
				else
					cursor+=4;
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
				}
			}
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
					cursor=(cursor/10)*4+0+100;	// move into shop part
			}
			else
			{
				if(((cursor-100)%4)==3)
					cursor=((cursor-100)/4)*10+0;
				else
					cursor++;
			}
			break;
		case CONTROL_B1:
			if(cursor<60)	// in your inventory, trying to sell
			{
				if(shopType==SHOP_STASH)	// free, just dump it in!
				{
					if(player.items[cursor].type)
					{
						if(StashGive(cursor))
						{
							MakeNormalSound(SND_JUNKGET);
							player.items[cursor].count--;
							if(player.items[cursor].count==0)
								player.items[cursor].type=IT_NONE;
							SortEquipment();
						}
						else
							MakeNormalSound(SND_CANTBUY);
					}
					else
						MakeNormalSound(SND_CANTBUY);
				}
				else
				{
					if(ItemCost(&player.items[cursor],1)==0)
					{
						MakeNormalSound(SND_CANTBUY);
						NewChat(65);	// can't sell that
					}
					else
					{
						PlayerGetMoney(ItemCost(&player.items[cursor],1));
						DropItem(cursor);
						MakeNormalSound(SND_SELLITEM);
						SortEquipment();
						NewChat(66);
					}
				}
			}
			else	// in the shop, trying to buy
			{
				int cost;

				if(shopType==SHOP_STASH)	// free, take it
				{
					if(player.shopInv[shopType][cursor-100].type)
					{
						if(StashTake(&player.shopInv[shopType][cursor-100]))
						{
							MakeNormalSound(SND_JUNKGET);
							player.shopInv[shopType][cursor-100].count--;
							if(player.shopInv[shopType][cursor-100].count==0)
								player.shopInv[shopType][cursor-100].type=IT_NONE;
							SortShop(SHOP_STASH);
						}
						else
							MakeNormalSound(SND_CANTBUY);
					}
					else
						MakeNormalSound(SND_CANTBUY);
				}
				else
				{
					cost=ItemCost(&player.shopInv[shopType][cursor-100],0);
					if(shopType==SHOP_BLACKMARKET)
						cost*=3;	// triple prices here!

					if(cost==0)
					{
						MakeNormalSound(SND_CANTBUY);	// can't buy nothing
					}
					else if(cost>player.money)
					{
						MakeNormalSound(SND_CANTBUY);
						NewChat(54);	// can't afford it
					}
					else
					{
						int v1,v2;

						v1=player.shopInv[shopType][cursor-100].magic*65536+player.shopInv[shopType][cursor-100].speed*256+player.shopInv[shopType][cursor-100].value;
						v2=player.shopInv[shopType][cursor-100].skill[2]*65536+player.shopInv[shopType][cursor-100].skill[1]*256+player.shopInv[shopType][cursor-100].skill[0];

						if(player.shopInv[shopType][cursor-100].type!=IT_GLASSES)
						{
							if(EquipAdd(player.shopInv[shopType][cursor-100].type,v1,v2))
							{
								MakeNormalSound(SND_JUNKGET);
								player.money-=cost;
								TalentPoint(TLT_HAGGLING,cost);
								player.shopInv[shopType][cursor-100].type=IT_NONE;
								NewChat(66);
							}
							else
							{
								MakeNormalSound(SND_CANTBUY);
								NewChat(67);	// can't carry it
							}
						}
						else
						{
							player.money-=cost;
							player.var[VAR_GLASSES]=1;
							TipEvent(TIP_GLASSES);
							MakeNormalSound(SND_SPECIALGET);
							player.shopInv[shopType][cursor-100].type=IT_NONE;
							NewChat(66);
						}

					}
				}
			}
			break;
		case CONTROL_B2:
			return 0;
			break;
		case CONTROL_B3:
			byte ok;

			if(cursor<60)	// in your inventory
			{
				if(shopType==SHOP_STASH)	// free, just dump it in!
				{
					if(player.items[cursor].type)
					{
						ok=0;
						while(StashGive(cursor))
						{
							ok=1;
							player.items[cursor].count--;
							if(player.items[cursor].count==0)
							{
								player.items[cursor].type=IT_NONE;
								break;
							}
						}
						if(ok)
							MakeNormalSound(SND_JUNKGET);
						else
							MakeNormalSound(SND_CANTBUY);
						SortEquipment();
					}
					else
						MakeNormalSound(SND_CANTBUY);
				}
			}
			else
			{
				if(shopType==SHOP_STASH)	// free, take it
				{
					if(player.shopInv[shopType][cursor-100].type)
					{
						ok=0;
						while(StashTake(&player.shopInv[shopType][cursor-100]))
						{
							ok=1;
							player.shopInv[shopType][cursor-100].count--;
							if(player.shopInv[shopType][cursor-100].count==0)
							{
								player.shopInv[shopType][cursor-100].type=IT_NONE;
								break;
							}
						}
						if(ok)
							MakeNormalSound(SND_JUNKGET);
						else
							MakeNormalSound(SND_CANTBUY);
						SortShop(SHOP_STASH);
					}
					else
						MakeNormalSound(SND_CANTBUY);
				}
			}
			break;
	}
	return 1;
}

byte UpdateShop(MGLDraw *mgl)
{
	byte c;
	static byte reptCounter=0;

	if(shopType>=SERVICES)
	{
		return UpdateService(mgl);
	}

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
		if(((c&CONTROL_B3) && (!(oldc&CONTROL_B3))))
		{
			if(!ButtonPressed(CONTROL_B3))
				return 0;
		}
	}

	oldc=c;

	HandleShopKeyPresses(mgl);
	if(lastKey==27)	// hit ESC to exit shop
	{
		return 0;
		lastKey=0;
	}

	return 1;
}

void ChangeShopItem(byte shop,byte pos)
{
	byte type;
	int v1,v2;

	switch(shop)
	{
		case SHOP_BLACKMARKET:
			type=0;
			while(type!=IT_MAGICAXE && type!=IT_GOLDENAXE &&
				type!=IT_MAGICPARKA && type!=IT_GOLDENPARKA &&
				type!=IT_MAGICAMULET && type!=IT_GOLDENAMULET)
				EquipCreateItem((15+(rand()%70))*(1+(player.var[VAR_MADCAP])),&type,&v1,&v2);	// make magic or golden items of random quality
			player.shopInv[SHOP_BLACKMARKET][pos].type=type;
			player.shopInv[SHOP_BLACKMARKET][pos].value=v1%256;
			player.shopInv[SHOP_BLACKMARKET][pos].speed=((v1)/256)%256;
			player.shopInv[SHOP_BLACKMARKET][pos].magic=(v1)/65536;
			player.shopInv[SHOP_BLACKMARKET][pos].sharpened=0;
			player.shopInv[SHOP_BLACKMARKET][pos].count=1;
			player.shopInv[SHOP_BLACKMARKET][pos].skill[0]=(v2)%256;
			player.shopInv[SHOP_BLACKMARKET][pos].skill[1]=((v2)/256)%256;
			player.shopInv[SHOP_BLACKMARKET][pos].skill[2]=((v2)/65536)%256;
			break;
		case SHOP_AXES:
			type=0;
			if(ModifierOn(MOD_MERCHANT))
			{
				while(type!=IT_AXE && type!=IT_MAGICAXE && type!=IT_GOLDENAXE)
					EquipCreateItem((pos+1)*2*(1+(player.var[VAR_MADCAP])),&type,&v1,&v2);	// make normal or magic axes of gradually increasing level
			}
			else
			{
				while(type!=IT_AXE && type!=IT_MAGICAXE)
					EquipCreateItem((pos+1)*2*(1+(player.var[VAR_MADCAP])),&type,&v1,&v2);	// make normal or magic axes of gradually increasing level
			}
			player.shopInv[SHOP_AXES][pos].type=type;
			player.shopInv[SHOP_AXES][pos].value=v1%256;
			player.shopInv[SHOP_AXES][pos].speed=((v1)/256)%256;
			player.shopInv[SHOP_AXES][pos].magic=(v1)/65536;
			player.shopInv[SHOP_AXES][pos].sharpened=0;
			player.shopInv[SHOP_AXES][pos].count=1;
			player.shopInv[SHOP_AXES][pos].skill[0]=(v2)%256;
			player.shopInv[SHOP_AXES][pos].skill[1]=((v2)/256)%256;
			player.shopInv[SHOP_AXES][pos].skill[2]=((v2)/65536)%256;
			break;
		case SHOP_TAILOR:
			type=0;
			if(ModifierOn(MOD_MERCHANT))
			{
				while(type!=IT_PARKA && type!=IT_MAGICPARKA && type!=IT_GOLDENPARKA)
					EquipCreateItem((pos+1)*2*(1+(player.var[VAR_MADCAP])),&type,&v1,&v2);	// make normal or magic parkas of gradually increasing level
			}
			else
			{
				while(type!=IT_PARKA && type!=IT_MAGICPARKA)
					EquipCreateItem((pos+1)*2*(1+(player.var[VAR_MADCAP])),&type,&v1,&v2);	// make normal or magic parkas of gradually increasing level
			}
			player.shopInv[SHOP_TAILOR][pos].type=type;
			player.shopInv[SHOP_TAILOR][pos].value=v1%256;
			player.shopInv[SHOP_TAILOR][pos].speed=((v1)/256)%256;
			player.shopInv[SHOP_TAILOR][pos].magic=(v1)/65536;
			player.shopInv[SHOP_TAILOR][pos].sharpened=0;
			player.shopInv[SHOP_TAILOR][pos].count=1;
			player.shopInv[SHOP_TAILOR][pos].skill[0]=(v2)%256;
			player.shopInv[SHOP_TAILOR][pos].skill[1]=((v2)/256)%256;
			player.shopInv[SHOP_TAILOR][pos].skill[2]=((v2)/65536)%256;
			break;
		case SHOP_MAGIC:
			type=0;
			if(ModifierOn(MOD_MERCHANT))
			{
				while(type!=IT_AMULET && type!=IT_MAGICAMULET && type!=IT_GOLDENAMULET && (type<IT_POT1 || type>IT_POT20))
					EquipCreateItem((pos+1)*2*(1+(player.var[VAR_MADCAP])),&type,&v1,&v2);	// make normal or magic amulets and potions of gradually increasing level
			}
			else
			{
				while(type!=IT_AMULET && type!=IT_MAGICAMULET && (type<IT_POT1 || type>IT_POT20))
					EquipCreateItem((pos+1)*2*(1+(player.var[VAR_MADCAP])),&type,&v1,&v2);	// make normal or magic amulets and potions of gradually increasing level
			}
			player.shopInv[SHOP_MAGIC][pos].type=type;
			player.shopInv[SHOP_MAGIC][pos].value=v1%256;
			player.shopInv[SHOP_MAGIC][pos].speed=((v1)/256)%256;
			player.shopInv[SHOP_MAGIC][pos].magic=(v1)/65536;
			player.shopInv[SHOP_MAGIC][pos].sharpened=0;
			player.shopInv[SHOP_MAGIC][pos].count=1;
			player.shopInv[SHOP_MAGIC][pos].skill[0]=(v2)%256;
			player.shopInv[SHOP_MAGIC][pos].skill[1]=((v2)/256)%256;
			player.shopInv[SHOP_MAGIC][pos].skill[2]=((v2)/65536)%256;
			break;
		case SHOP_ROPES:
			player.shopInv[SHOP_ROPES][pos].type=IT_ROPE;
			player.shopInv[SHOP_ROPES][pos].value=0;
			player.shopInv[SHOP_ROPES][pos].speed=0;
			player.shopInv[SHOP_ROPES][pos].magic=0;
			player.shopInv[SHOP_ROPES][pos].sharpened=0;
			player.shopInv[SHOP_ROPES][pos].count=1;
			player.shopInv[SHOP_ROPES][pos].skill[0]=0;
			player.shopInv[SHOP_ROPES][pos].skill[1]=0;
			player.shopInv[SHOP_ROPES][pos].skill[2]=0;
			break;
		case SHOP_SHROOM:
			type=0;
			if(ModifierOn(MOD_MERCHANT))
			{
				while(type!=IT_MAGICAXE && type!=IT_AXE && type!=IT_GOLDENAXE &&
					type!=IT_MAGICPARKA && type!=IT_PARKA && type!=IT_GOLDENPARKA &&
					type!=IT_MAGICAMULET && type!=IT_AMULET && type!=IT_GOLDENAMULET && (type<IT_POT1 || type>IT_POT20))
					EquipCreateItem((10+(rand()%30))*(1+(player.var[VAR_MADCAP])),&type,&v1,&v2);	// make regular or magic items of random quality up to level 40
			}
			else
			{
				while(type!=IT_MAGICAXE && type!=IT_AXE &&
					type!=IT_MAGICPARKA && type!=IT_PARKA &&
					type!=IT_MAGICAMULET && type!=IT_AMULET && (type<IT_POT1 || type>IT_POT20))
					EquipCreateItem((10+(rand()%30))*(1+(player.var[VAR_MADCAP])),&type,&v1,&v2);	// make regular or magic items of random quality up to level 40
			}
			player.shopInv[SHOP_SHROOM][pos].type=type;
			player.shopInv[SHOP_SHROOM][pos].value=v1%256;
			player.shopInv[SHOP_SHROOM][pos].speed=((v1)/256)%256;
			player.shopInv[SHOP_SHROOM][pos].magic=(v1)/65536;
			player.shopInv[SHOP_SHROOM][pos].sharpened=0;
			player.shopInv[SHOP_SHROOM][pos].count=1;
			player.shopInv[SHOP_SHROOM][pos].skill[0]=(v2)%256;
			player.shopInv[SHOP_SHROOM][pos].skill[1]=((v2)/256)%256;
			player.shopInv[SHOP_SHROOM][pos].skill[2]=((v2)/65536)%256;
			break;
		case SHOP_STASH:	// empty!  Go to default
		default:
			player.shopInv[shop][pos].type=IT_NONE;
			player.shopInv[shop][pos].count=0;
			break;
	}
}

void SortShop(byte shop)
{
	byte swap;
	int i;
	equip_t e;

	swap=1;
	while(swap==1)
	{
		swap=0;
		for(i=0;i<SHOP_SIZE-1;i++)
		{
			if(shop==SHOP_STASH)
			{
				if((player.shopInv[shop][i].type>player.shopInv[shop][i+1].type && player.shopInv[shop][i+1].type!=IT_NONE) || (player.shopInv[shop][i].type==IT_NONE && player.shopInv[shop][i+1].type!=IT_NONE))	// sort by item type order
				{
					e=player.shopInv[shop][i];
					player.shopInv[shop][i]=player.shopInv[shop][i+1];
					player.shopInv[shop][i+1]=e;
					swap=1;
				}
				else if(player.shopInv[shop][i].type==player.shopInv[shop][i+1].type && player.shopInv[shop][i].type!=IT_NONE)	// same item type, so...
				{
					if(player.shopInv[shop][i].value<player.shopInv[shop][i+1].value)	// sort by value
					{
						e=player.shopInv[shop][i];
						player.shopInv[shop][i]=player.shopInv[shop][i+1];
						player.shopInv[shop][i+1]=e;
						swap=1;
					}
					else if(player.shopInv[shop][i].value==player.shopInv[shop][i+1].value && player.shopInv[shop][i].speed<player.shopInv[shop][i+1].speed)	// if same, then finally sort by speed
					{
						e=player.shopInv[shop][i];
						player.shopInv[shop][i]=player.shopInv[shop][i+1];
						player.shopInv[shop][i+1]=e;
						swap=1;
					}
				}
			}
			else if(ItemCost(&player.shopInv[shop][i],0)>ItemCost(&player.shopInv[shop][i+1],0))
			{
				swap=1;	// swap these
				e=player.shopInv[shop][i];
				player.shopInv[shop][i]=player.shopInv[shop][i+1];
				player.shopInv[shop][i+1]=e;
			}
		}
	}
}

void SetupShopLists(void)
{
	int i,j;

	for(j=0;j<SHOP_MAX;j++)
	{
		for(i=0;i<SHOP_SIZE;i++)
		{
			ChangeShopItem(j,i);
		}
		if(j==SHOP_MAGIC)
		{
			player.shopInv[j][0].type=IT_GLASSES;
			player.shopInv[j][0].count=1;
		}
		SortShop(j);
	}
}

void UpdateShopInventories(void)
{
	int i,j;

	if((player.timeToFinish%(30*10))==0)	// only update shops once every 10 seconds
	{
		for(j=0;j<SHOP_MAX;j++)
		{
			if(j!=SHOP_STASH)
			{
				for(i=0;i<SHOP_SIZE;i++)
				{
					// 10% chance for empty spot to be refilled, 1% chance for filled spot to change
					if(((player.shopInv[j][i].type==IT_NONE && (rand()%100)<10) ||
						(player.shopInv[j][i].type!=IT_NONE && (rand()%100)==0)) && player.shopInv[j][i].type!=IT_GLASSES)
						ChangeShopItem(j,i);
				}
				SortShop(j);
			}
		}
	}
}

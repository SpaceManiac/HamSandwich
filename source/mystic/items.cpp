#include "items.h"
#include "display.h"
#include "particle.h"
#include "challenge.h"

sprite_set_t *itmSpr;
static byte glowism;

void InitItems(void)
{
	itmSpr=new sprite_set_t("graphics/items.jsp");
	glowism=0;
}

void ExitItems(void)
{
	delete itmSpr;
}

void UpdateItems(void)
{
	glowism++;
}

sprite_t *GetItemSprite(word spr)
{
	return itmSpr->GetSprite(spr);
}

void RenderItem(int x,int y,byte type,char bright)
{
	byte b;

	switch(type)
	{
		case ITM_NONE:
			break;
		case ITM_HAMMERUP:
			SprDraw(x,y,0,255,bright+abs((glowism&7)-3),itmSpr->GetSprite(0),DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case ITM_PANTS:
			SprDraw(x,y,8,255,bright,itmSpr->GetSprite(59+(glowism&7)),DISPLAY_DRAWME|DISPLAY_GLOW);
			SprDraw(x,y,0,255,bright,itmSpr->GetSprite(59+(glowism&7)),DISPLAY_DRAWME|DISPLAY_SHADOW);
			//SprDraw(x,y,0,255,bright,itmSpr->GetSprite(3),DISPLAY_DRAWME);
			break;
		case ITM_REVERSE:
			SprDraw(x,y,8,255,bright,itmSpr->GetSprite(67+(glowism&7)),DISPLAY_DRAWME|DISPLAY_GLOW);
			SprDraw(x,y,0,255,bright,itmSpr->GetSprite(67+(glowism&7)),DISPLAY_DRAWME|DISPLAY_SHADOW);
			//SprDraw(x,y,0,255,bright,itmSpr->GetSprite(4),DISPLAY_DRAWME);
			break;
		case ITM_REFLECT:
			SprDraw(x,y,8,255,bright,itmSpr->GetSprite(75+(glowism&7)),DISPLAY_DRAWME|DISPLAY_GLOW);
			SprDraw(x,y,0,255,bright,itmSpr->GetSprite(75+(glowism&7)),DISPLAY_DRAWME|DISPLAY_SHADOW);
			//SprDraw(x,y,0,255,bright,itmSpr->GetSprite(5),DISPLAY_DRAWME);
			break;
		case ITM_SPELLBOOK:
			SprDraw(x,y,4,255,bright,itmSpr->GetSprite(43+((glowism/2)&15)),DISPLAY_DRAWME);
			SprDraw(x,y,0,255,bright,itmSpr->GetSprite(43+((glowism/2)&15)),DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case ITM_BRAIN:
			SprDraw(x,y,6,255,bright-6+abs((glowism&7)-3),itmSpr->GetSprite(7),DISPLAY_DRAWME|DISPLAY_GLOW);
			break;
		case ITM_BOX:
			SprDraw(x,y,0,255,bright,itmSpr->GetSprite(1),DISPLAY_DRAWME);
			break;
		case ITM_TREE:
			SprDraw(x,y,0,255,bright,itmSpr->GetSprite(2),DISPLAY_DRAWME);
			break;
		case ITM_SKLDOOR2:
			SprDraw(x,y,0,255,bright,itmSpr->GetSprite(40),DISPLAY_DRAWME|DISPLAY_SHADOW);
			SprDraw(x,y,0,255,bright,itmSpr->GetSprite(40),DISPLAY_DRAWME);
			break;
		case ITM_SKLDOOR:
			SprDraw(x,y,0,255,bright,itmSpr->GetSprite(39),DISPLAY_DRAWME|DISPLAY_SHADOW);
			SprDraw(x,y,0,255,bright,itmSpr->GetSprite(39),DISPLAY_DRAWME);
			break;
		case ITM_IGLOO:
			SprDraw(x,y,0,255,bright,itmSpr->GetSprite(42),DISPLAY_DRAWME);
			break;
		case ITM_HEALTHPOT:
			SprDraw(x,y,0,255,bright+((glowism&7)==0)*2,itmSpr->GetSprite(156),DISPLAY_DRAWME);
			break;
		case ITM_MANAPOT:
			SprDraw(x,y,0,255,bright+((glowism&7)==0)*2,itmSpr->GetSprite(155),DISPLAY_DRAWME);
			break;
		case ITM_BIGCOIN:
			SprDraw(x,y,0,255,bright,itmSpr->GetSprite(159+((glowism/4)&7)),DISPLAY_DRAWME);
			break;
		case ITM_COIN:
			SprDraw(x,y,0,255,bright,itmSpr->GetSprite(147+((glowism/4)&7)),DISPLAY_DRAWME);
			break;
		case ITM_FLAME:
			SprDraw(x,y,0,255,bright,itmSpr->GetSprite(34),DISPLAY_DRAWME);
			break;
		case ITM_KEYCH1:
			SprDraw(x,y,8,255,bright-4+abs((glowism&7)-3),itmSpr->GetSprite(83+(glowism&15)),DISPLAY_DRAWME);
			SprDraw(x,y,0,255,bright,itmSpr->GetSprite(83+(glowism&15)),DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case ITM_KEYCH2:
			SprDraw(x,y,8,255,bright-4+abs((glowism&7)-3),itmSpr->GetSprite(99+(glowism&15)),DISPLAY_DRAWME);
			SprDraw(x,y,0,255,bright,itmSpr->GetSprite(99+(glowism&15)),DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case ITM_KEYCH3:
			SprDraw(x,y,8,255,bright-4+abs((glowism&7)-3),itmSpr->GetSprite(115+(glowism&15)),DISPLAY_DRAWME);
			SprDraw(x,y,0,255,bright,itmSpr->GetSprite(115+(glowism&15)),DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case ITM_KEYCH4:
			SprDraw(x,y,8,255,bright-4+abs((glowism&7)-3),itmSpr->GetSprite(131+(glowism&15)),DISPLAY_DRAWME);
			SprDraw(x,y,0,255,bright,itmSpr->GetSprite(131+(glowism&15)),DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case ITM_KEY:
			SprDraw(x,y,0,255,bright,itmSpr->GetSprite(17),DISPLAY_DRAWME);
			break;
		case ITM_KEYR:
			SprDraw(x,y,0,255,bright,itmSpr->GetSprite(18),DISPLAY_DRAWME);
			break;
		case ITM_KEYG:
			SprDraw(x,y,0,255,bright,itmSpr->GetSprite(19),DISPLAY_DRAWME);
			break;
		case ITM_KEYB:
			SprDraw(x,y,0,255,bright,itmSpr->GetSprite(20),DISPLAY_DRAWME);
			break;
		case ITM_LOONYKEY:
			b=abs(16-(glowism&31));
			SprDraw(x,y,0,glowism/32,bright+b,itmSpr->GetSprite(25),DISPLAY_DRAWME);
			break;
		case ITM_SMLROCKS:
			SprDraw(x,y,0,255,bright,itmSpr->GetSprite(32),DISPLAY_DRAWME);
			break;
		case ITM_HOLETREE:
			//SprDraw(x,y,0,255,bright,itmSpr->GetSprite(26),DISPLAY_DRAWME|DISPLAY_SHADOW);
			SprDraw(x,y,0,255,bright,itmSpr->GetSprite(26),DISPLAY_DRAWME);
			break;
		case ITM_STUMP:
			//SprDraw(x,y,0,255,bright,itmSpr->GetSprite(27),DISPLAY_DRAWME|DISPLAY_SHADOW);
			SprDraw(x,y,0,255,bright,itmSpr->GetSprite(27),DISPLAY_DRAWME);
			break;
		case ITM_CHAIR1:
			SprDraw(x,y,0,255,bright,itmSpr->GetSprite(36),DISPLAY_DRAWME|DISPLAY_SHADOW);
			SprDraw(x,y,0,255,bright,itmSpr->GetSprite(36),DISPLAY_DRAWME);
			break;
		case ITM_CHAIR2:
			SprDraw(x,y,0,255,bright,itmSpr->GetSprite(37),DISPLAY_DRAWME|DISPLAY_SHADOW);
			SprDraw(x,y,0,255,bright,itmSpr->GetSprite(37),DISPLAY_DRAWME);
			break;
		case ITM_BUSH:
			SprDraw(x,y,0,255,bright,itmSpr->GetSprite(30),DISPLAY_DRAWME);
			break;
		case ITM_BIGROCKS:
			//SprDraw(x,y,0,255,bright,itmSpr->GetSprite(31),DISPLAY_DRAWME|DISPLAY_SHADOW);
			SprDraw(x,y,0,255,bright,itmSpr->GetSprite(31),DISPLAY_DRAWME);
			break;
		case ITM_POST:
			SprDraw(x,y,0,255,bright,itmSpr->GetSprite(28),DISPLAY_DRAWME|DISPLAY_SHADOW);
			SprDraw(x,y,0,255,bright,itmSpr->GetSprite(28),DISPLAY_DRAWME);
			break;
		case ITM_SIGN:
			SprDraw(x,y,0,255,bright,itmSpr->GetSprite(29),DISPLAY_DRAWME|DISPLAY_SHADOW);
			SprDraw(x,y,0,255,bright,itmSpr->GetSprite(29),DISPLAY_DRAWME);
			break;
		case ITM_TARGET:
			SprDraw(x-18,y,0,255,bright,itmSpr->GetSprite(182),DISPLAY_DRAWME|DISPLAY_SHADOW);
			SprDraw(x-18,y,0,255,bright,itmSpr->GetSprite(182),DISPLAY_DRAWME);
			break;
		case ITM_DOOR1:
			SprDraw(x-14,y+11,0,255,bright,itmSpr->GetSprite(9),DISPLAY_DRAWME);
			break;
		case ITM_DOOR1R:
			SprDraw(x-14,y+11,0,255,bright,itmSpr->GetSprite(10),DISPLAY_DRAWME);
			break;
		case ITM_DOOR1G:
			SprDraw(x-14,y+11,0,255,bright,itmSpr->GetSprite(11),DISPLAY_DRAWME);
			break;
		case ITM_DOOR1B:
			SprDraw(x-14,y+11,0,255,bright,itmSpr->GetSprite(12),DISPLAY_DRAWME);
			break;
		case ITM_DOOR2:
			SprDraw(x-14,y+11,0,255,bright,itmSpr->GetSprite(13),DISPLAY_DRAWME);
			break;
		case ITM_DOOR2R:
			SprDraw(x-14,y+11,0,255,bright,itmSpr->GetSprite(14),DISPLAY_DRAWME);
			break;
		case ITM_DOOR2G:
			SprDraw(x-14,y+11,0,255,bright,itmSpr->GetSprite(15),DISPLAY_DRAWME);
			break;
		case ITM_DOOR2B:
			SprDraw(x-14,y+11,0,255,bright,itmSpr->GetSprite(16),DISPLAY_DRAWME);
			break;
		case ITM_HUT:
			SprDraw(x,y,0,255,bright,itmSpr->GetSprite(157),DISPLAY_DRAWME);
			SprDraw(x,y,0,255,bright,itmSpr->GetSprite(157),DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case ITM_SHOP:
			SprDraw(x,y,0,255,bright,itmSpr->GetSprite(158),DISPLAY_DRAWME);
			SprDraw(x,y,0,255,bright,itmSpr->GetSprite(158),DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case ITM_FAIRYBELL:
			SprDraw(x-16,y+12,0,255,bright,itmSpr->GetSprite(167+((glowism/4)%15)),DISPLAY_DRAWME);
			SprDraw(x-16,y+12,0,255,bright,itmSpr->GetSprite(167+((glowism/4)%15)),DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;
		case ITM_LETTERM:
		case ITM_LETTERY:
		case ITM_LETTERS:
		case ITM_LETTERT:
		case ITM_LETTERI:
		case ITM_LETTERC:
			if(CurrentLetter()==type-ITM_LETTERM)
			{
				SprDraw(x-18,y,0,255,bright,itmSpr->GetSprite(183+((type-ITM_LETTERM)*16)+((glowism/4)%16)),DISPLAY_DRAWME|DISPLAY_SHADOW);
				SprDraw(x-18,y,0,255,bright,itmSpr->GetSprite(183+((type-ITM_LETTERM)*16)+((glowism/4)%16)),DISPLAY_DRAWME);
			}
			else if(CurrentLetter()<type-ITM_LETTERM)
				SprDraw(x-18,y,0,255,bright,itmSpr->GetSprite(183+((type-ITM_LETTERM)*16)+((glowism/4)%16)),DISPLAY_DRAWME|DISPLAY_GHOST);
			// don't render at all if this letter is already gotten
			break;
		case ITM_CHLGCRYSTAL:
			SprDraw(x,y,8,1,bright+(glowism&3),itmSpr->GetSprite(59+(glowism&7)),DISPLAY_DRAWME);
			SprDraw(x,y,0,255,bright,itmSpr->GetSprite(59+(glowism&7)),DISPLAY_DRAWME|DISPLAY_SHADOW);
			break;

	}
}

void InstaRenderItem(int x,int y,byte type,char bright,MGLDraw *mgl)
{
	switch(type)
	{
		case ITM_NONE:
			break;
		case ITM_HAMMERUP:
			itmSpr->GetSprite(0)->DrawBright(x,y,mgl,bright);
			break;
		case ITM_PANTS:
			itmSpr->GetSprite(3)->DrawBright(x,y,mgl,bright);
			break;
		case ITM_REVERSE:
			itmSpr->GetSprite(4)->DrawBright(x,y,mgl,bright);
			break;
		case ITM_REFLECT:
			itmSpr->GetSprite(5)->DrawBright(x,y,mgl,bright);
			break;
		case ITM_SPELLBOOK:
			itmSpr->GetSprite(43)->DrawBright(x,y,mgl,bright);
			break;
		case ITM_HEALTHPOT:
			itmSpr->GetSprite(156)->DrawBright(x,y,mgl,bright);
			break;
		case ITM_MANAPOT:
			itmSpr->GetSprite(155)->DrawBright(x,y,mgl,bright);
			break;
		case ITM_BIGCOIN:
			itmSpr->GetSprite(159)->DrawBright(x,y,mgl,bright);
			break;
		case ITM_COIN:
			itmSpr->GetSprite(147)->DrawBright(x,y,mgl,bright);
			break;
		case ITM_FLAME:
			itmSpr->GetSprite(34)->DrawBright(x,y,mgl,bright);
			break;
		case ITM_BRAIN:
			itmSpr->GetSprite(7)->DrawBright(x,y,mgl,bright);
			break;
		case ITM_KEYCH1:
			itmSpr->GetSprite(24)->DrawBright(x,y,mgl,bright);
			break;
		case ITM_KEYCH2:
			itmSpr->GetSprite(21)->DrawBright(x,y,mgl,bright);
			break;
		case ITM_KEYCH3:
			itmSpr->GetSprite(22)->DrawBright(x,y,mgl,bright);
			break;
		case ITM_KEYCH4:
			itmSpr->GetSprite(23)->DrawBright(x,y,mgl,bright);
			break;
		case ITM_KEY:
			itmSpr->GetSprite(17)->DrawBright(x,y,mgl,bright);
			break;
		case ITM_KEYR:
			itmSpr->GetSprite(18)->DrawBright(x,y,mgl,bright);
			break;
		case ITM_KEYG:
			itmSpr->GetSprite(19)->DrawBright(x,y,mgl,bright);
			break;
		case ITM_KEYB:
			itmSpr->GetSprite(20)->DrawBright(x,y,mgl,bright);
			break;
		case ITM_LOONYKEY:
			itmSpr->GetSprite(25)->DrawBright(x,y,mgl,bright);
			break;
		case ITM_BOX:
			itmSpr->GetSprite(1)->DrawBright(x,y,mgl,bright);
			break;
		case ITM_TREE:
			itmSpr->GetSprite(2)->DrawBright(x,y,mgl,bright);
			break;
		case ITM_SKLDOOR:
			Print(x,y,"SKLDOOR",0,2);
			break;
		case ITM_SKLDOOR2:
			Print(x,y,"SKLDOOR2",0,2);
			break;
		case ITM_IGLOO:
			itmSpr->GetSprite(42)->DrawBright(x,y,mgl,bright);
			break;
		case ITM_DOOR1:
			itmSpr->GetSprite(9)->DrawBright(x-14,y+11,mgl,bright);
			break;
		case ITM_DOOR1R:
			itmSpr->GetSprite(10)->DrawBright(x-14,y+11,mgl,bright);
			break;
		case ITM_DOOR1G:
			itmSpr->GetSprite(11)->DrawBright(x-14,y+11,mgl,bright);
			break;
		case ITM_DOOR1B:
			itmSpr->GetSprite(12)->DrawBright(x-14,y+11,mgl,bright);
			break;
		case ITM_DOOR2:
			itmSpr->GetSprite(13)->DrawBright(x-14,y+11,mgl,bright);
			break;
		case ITM_DOOR2R:
			itmSpr->GetSprite(14)->DrawBright(x-14,y+11,mgl,bright);
			break;
		case ITM_DOOR2G:
			itmSpr->GetSprite(15)->DrawBright(x-14,y+11,mgl,bright);
			break;
		case ITM_DOOR2B:
			itmSpr->GetSprite(16)->DrawBright(x-14,y+11,mgl,bright);
			break;
		case ITM_HOLETREE:
			itmSpr->GetSprite(26)->DrawBright(x,y,mgl,bright);
			break;
		case ITM_STUMP:
			itmSpr->GetSprite(27)->DrawBright(x,y,mgl,bright);
			break;
		case ITM_POST:
			itmSpr->GetSprite(28)->DrawBright(x,y,mgl,bright);
			break;
		case ITM_SIGN:
			itmSpr->GetSprite(29)->DrawBright(x,y,mgl,bright);
			break;
		case ITM_TARGET:
			itmSpr->GetSprite(182)->DrawBright(x-18,y,mgl,bright);
			break;
		case ITM_BUSH:
			itmSpr->GetSprite(30)->DrawBright(x,y,mgl,bright);
			break;
		case ITM_BIGROCKS:
			itmSpr->GetSprite(31)->DrawBright(x,y,mgl,bright);
			break;
		case ITM_SMLROCKS:
			itmSpr->GetSprite(32)->DrawBright(x,y,mgl,bright);
			break;
		case ITM_CHAIR1:
			itmSpr->GetSprite(36)->DrawBright(x,y,mgl,bright);
			break;
		case ITM_CHAIR2:
			itmSpr->GetSprite(37)->DrawBright(x,y,mgl,bright);
			break;
		case ITM_HUT:
			itmSpr->GetSprite(157)->DrawBright(x,y,mgl,bright);
			break;
		case ITM_SHOP:
			itmSpr->GetSprite(158)->DrawBright(x,y,mgl,bright);
			break;
		case ITM_FAIRYBELL:
			itmSpr->GetSprite(167)->DrawBright(x-16,y+12,mgl,bright);
			break;
		case ITM_LETTERM:
		case ITM_LETTERY:
		case ITM_LETTERS:
		case ITM_LETTERT:
		case ITM_LETTERI:
		case ITM_LETTERC:
			itmSpr->GetSprite(183+((type-ITM_LETTERM)*16))->DrawBright(x-18,y,mgl,bright);
			break;
		case ITM_CHLGCRYSTAL:
			itmSpr->GetSprite(59)->DrawColored(x,y,mgl,1,bright);
			break;
	}
}

void DrawRedX(int x,int y,MGLDraw *mgl)
{
	itmSpr->GetSprite(8)->Draw(x,y,mgl);
}

#include "items.h"
#include "display.h"

sprite_set_t *itmSpr;
static byte glowism;
static byte itmLight = 0;

void InitItems(void)
{
	itmSpr = new sprite_set_t("graphics\\items.jsp");
	glowism = 0;
}

void ExitItems(void)
{
	delete itmSpr;
}

void RenderItem(int x, int y, byte type, char bright)
{
	byte b;

	x -= 3;

	if (itmLight)
		bright = 10;
	switch (type) {
		case ITM_NONE:
			break;
		case ITM_HAMMERUP:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(0), DISPLAY_DRAWME);
			break;
		case ITM_PANTS:
			SprDraw(x, y - 2, 0, 255, bright, itmSpr->GetSprite(3), DISPLAY_DRAWME);
			break;
		case ITM_REVERSE:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(4), DISPLAY_DRAWME);
			break;
		case ITM_REFLECT:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(5), DISPLAY_DRAWME);
			break;
		case ITM_MISSILES:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(6), DISPLAY_DRAWME);
			break;
		case ITM_BRAIN:
			SprDraw(x, y - 2, 0, 255, bright, itmSpr->GetSprite(7), DISPLAY_DRAWME);
			break;
		case ITM_BOX:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(1), DISPLAY_DRAWME);
			break;
		case ITM_TREE:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(2), DISPLAY_DRAWME);
			break;
		case ITM_PINE:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(40), DISPLAY_DRAWME);
			break;
		case ITM_PALM:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(41), DISPLAY_DRAWME | DISPLAY_SHADOW);
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(41), DISPLAY_DRAWME);
			break;
		case ITM_IGLOO:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(42), DISPLAY_DRAWME);
			break;
		case ITM_AK8087:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(33), DISPLAY_DRAWME);
			break;
		case ITM_TAKEOUT:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(38), DISPLAY_DRAWME);
			break;
		case ITM_SHIELD:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(39), DISPLAY_DRAWME | DISPLAY_GLOW);
			break;
		case ITM_BOMBS:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(35), DISPLAY_DRAWME);
			break;
		case ITM_FLAME:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(34), DISPLAY_DRAWME);
			break;
		case ITM_KEYCH1:
			SprDraw(x, y - 1, 0, 255, bright, itmSpr->GetSprite(24), DISPLAY_DRAWME);
			break;
		case ITM_KEYCH2:
			SprDraw(x, y - 1, 0, 255, bright, itmSpr->GetSprite(21), DISPLAY_DRAWME);
			break;
		case ITM_KEYCH3:
			SprDraw(x, y - 1, 0, 255, bright, itmSpr->GetSprite(22), DISPLAY_DRAWME);
			break;
		case ITM_KEYCH4:
			SprDraw(x, y - 1, 0, 255, bright, itmSpr->GetSprite(23), DISPLAY_DRAWME);
			break;
		case ITM_KEY:
			SprDraw(x, y - 1, 0, 255, bright, itmSpr->GetSprite(17), DISPLAY_DRAWME);
			break;
		case ITM_KEYR:
			SprDraw(x, y - 1, 0, 255, bright, itmSpr->GetSprite(18), DISPLAY_DRAWME);
			break;
		case ITM_KEYG:
			SprDraw(x, y - 1, 0, 255, bright, itmSpr->GetSprite(19), DISPLAY_DRAWME);
			break;
		case ITM_KEYB:
			SprDraw(x, y - 1, 0, 255, bright, itmSpr->GetSprite(20), DISPLAY_DRAWME);
			break;
		case ITM_LOONYKEY:
			glowism++;
			b = abs(16 - (glowism & 31));
			SprDraw(x, y, 0, glowism / 32, bright + b, itmSpr->GetSprite(25), DISPLAY_DRAWME);
			break;
		case ITM_SMLROCKS:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(32), DISPLAY_DRAWME);
			break;
		case ITM_HOLETREE:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(26), DISPLAY_DRAWME);
			break;
		case ITM_STUMP:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(27), DISPLAY_DRAWME);
			break;
		case ITM_CHAIR1:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(36), DISPLAY_DRAWME | DISPLAY_SHADOW);
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(36), DISPLAY_DRAWME);
			break;
		case ITM_CHAIR2:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(37), DISPLAY_DRAWME | DISPLAY_SHADOW);
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(37), DISPLAY_DRAWME);
			break;
		case ITM_BUSH:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(30), DISPLAY_DRAWME);
			break;
		case ITM_BIGROCKS:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(31), DISPLAY_DRAWME);
			break;
		case ITM_POST:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(28), DISPLAY_DRAWME | DISPLAY_SHADOW);
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(28), DISPLAY_DRAWME);
			break;
		case ITM_SIGN:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(29), DISPLAY_DRAWME | DISPLAY_SHADOW);
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(29), DISPLAY_DRAWME);
			break;
		case ITM_DOOR1:
			SprDraw(x - 13, y + 9, 0, 255, bright, itmSpr->GetSprite(9), DISPLAY_DRAWME);
			break;
		case ITM_DOOR1R:
			SprDraw(x - 13, y + 9, 0, 255, bright, itmSpr->GetSprite(10), DISPLAY_DRAWME);
			break;
		case ITM_DOOR1G:
			SprDraw(x - 13, y + 9, 0, 255, bright, itmSpr->GetSprite(11), DISPLAY_DRAWME);
			break;
		case ITM_DOOR1B:
			SprDraw(x - 13, y + 9, 0, 255, bright, itmSpr->GetSprite(12), DISPLAY_DRAWME);
			break;
		case ITM_DOOR2:
			SprDraw(x - 13, y + 9, 0, 255, bright, itmSpr->GetSprite(13), DISPLAY_DRAWME);
			break;
		case ITM_DOOR2R:
			SprDraw(x - 13, y + 9, 0, 255, bright, itmSpr->GetSprite(14), DISPLAY_DRAWME);
			break;
		case ITM_DOOR2G:
			SprDraw(x - 13, y + 9, 0, 255, bright, itmSpr->GetSprite(15), DISPLAY_DRAWME);
			break;
		case ITM_DOOR2B:
			SprDraw(x - 13, y + 9, 0, 255, bright, itmSpr->GetSprite(16), DISPLAY_DRAWME);
			break;
		case ITM_PWRARMOR:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(43), DISPLAY_DRAWME);
			break;
		case ITM_BIGAXE:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(44), DISPLAY_DRAWME);
			break;
		case ITM_LIGHTNING:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(46), DISPLAY_DRAWME);
			break;
		case ITM_SPEAR:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(45), DISPLAY_DRAWME);
			break;
		case ITM_MACHETE:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(47), DISPLAY_DRAWME);
			break;
		case ITM_MINES:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(48), DISPLAY_DRAWME);
			break;
		case ITM_GARLIC:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(49), DISPLAY_DRAWME);
			break;
		case ITM_ORBITER:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(58), DISPLAY_DRAWME);
			break;
		case ITM_ACCEL:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(59), DISPLAY_DRAWME);
			break;
		case ITM_WEB:
			SprDraw(x + 5, y - 6, 0, 255, bright, itmSpr->GetSprite(54), DISPLAY_DRAWME);
			break;
		case ITM_WEB2:
			SprDraw(x, y - 10, 0, 255, bright, itmSpr->GetSprite(55), DISPLAY_DRAWME);
			break;
		case ITM_WEB3:
			SprDraw(x - 2, y - 8, 0, 255, bright, itmSpr->GetSprite(56), DISPLAY_DRAWME);
			break;
		case ITM_WEB4:
			SprDraw(x, y - 9, 0, 255, bright, itmSpr->GetSprite(57), DISPLAY_DRAWME);
			break;
		case ITM_DEADTREE:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(52), DISPLAY_DRAWME);
			break;
		case ITM_DEADTREE2:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(53), DISPLAY_DRAWME);
			break;
		case ITM_FATPALM:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(60), DISPLAY_DRAWME);
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(60), DISPLAY_DRAWME | DISPLAY_SHADOW);
			break;
		case ITM_GRASS:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(61), DISPLAY_DRAWME);
			break;
		case ITM_GRASS2:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(62), DISPLAY_DRAWME);
			break;
		case ITM_TREE2:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(63), DISPLAY_DRAWME);
			break;
		case ITM_WALLGRASS:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(66), DISPLAY_DRAWME);
			break;
		case ITM_VILLAGE:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(65), DISPLAY_DRAWME);
			break;
		case ITM_MINEBLOCK:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(64), DISPLAY_DRAWME);
			break;
		case ITM_TURRETWPN:
			SprDraw(x - 1, y - 2, 0, 255, bright, itmSpr->GetSprite(68), DISPLAY_DRAWME);
			break;
		case ITM_MINDCONTROL:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(69), DISPLAY_DRAWME);
			break;
		case ITM_REFLECTOR:
			SprDraw(x - 1, y, 0, 255, bright, itmSpr->GetSprite(67), DISPLAY_DRAWME);
			break;
		case ITM_INVIS:
			SprDraw(x - 1, y, 0, 255, bright, itmSpr->GetSprite(88), DISPLAY_DRAWME | DISPLAY_GLOW);
			break;
		case ITM_JETPACK:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(71), DISPLAY_DRAWME);
			break;
		case ITM_UNHAMMER:
			SprDrawOff(x, y, 0, 4, 0, bright, itmSpr->GetSprite(0), DISPLAY_DRAWME);
			break;
		case ITM_UNPANTS:
			SprDrawOff(x, y, 0, 3, 0, bright, itmSpr->GetSprite(3), DISPLAY_DRAWME);
			break;
		case ITM_SWAPGUN:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(70), DISPLAY_DRAWME);
			break;
		case ITM_BADCHINESE:
			SprDrawOff(x, y, 0, 4, 1, bright, itmSpr->GetSprite(38), DISPLAY_DRAWME);
			break;
		case ITM_BARREL:
			SprDraw(x, y - 4, 0, 255, bright, itmSpr->GetSprite(72), DISPLAY_DRAWME);
			SprDraw(x, y - 4, 0, 255, bright, itmSpr->GetSprite(72), DISPLAY_DRAWME | DISPLAY_SHADOW);
			break;
		case ITM_BARREL2:
			SprDraw(x, y - 4, 0, 255, bright, itmSpr->GetSprite(73), DISPLAY_DRAWME);
			SprDraw(x, y - 4, 0, 255, bright, itmSpr->GetSprite(73), DISPLAY_DRAWME | DISPLAY_SHADOW);
			break;
		case ITM_BARREL3:
			SprDraw(x, y - 4, 0, 255, bright, itmSpr->GetSprite(74), DISPLAY_DRAWME);
			SprDraw(x, y - 4, 0, 255, bright, itmSpr->GetSprite(74), DISPLAY_DRAWME | DISPLAY_SHADOW);
			break;
		case ITM_TRASHCAN:
			SprDraw(x - 1, y - 2, 0, 255, bright, itmSpr->GetSprite(75), DISPLAY_DRAWME);
			SprDraw(x - 1, y - 2, 0, 255, bright, itmSpr->GetSprite(76), DISPLAY_DRAWME | DISPLAY_SHADOW);
			break;
		case ITM_TRASHCAN2:
			SprDraw(x - 1, y - 2, 0, 255, bright, itmSpr->GetSprite(76), DISPLAY_DRAWME);
			SprDraw(x - 1, y - 2, 0, 255, bright, itmSpr->GetSprite(76), DISPLAY_DRAWME | DISPLAY_SHADOW);
			break;
		case ITM_CRATE:
			SprDraw(x - 3, y - 2, 0, 255, bright, itmSpr->GetSprite(77), DISPLAY_DRAWME);
			break;
		case ITM_CRATE2:
			SprDraw(x - 3, y - 2, 0, 255, bright, itmSpr->GetSprite(78), DISPLAY_DRAWME);
			break;
		case ITM_BUSH2:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(79), DISPLAY_DRAWME);
			break;
		case ITM_BUSH3:
			SprDraw(x - 2, y - 3, 0, 255, bright, itmSpr->GetSprite(80), DISPLAY_DRAWME);
			SprDraw(x - 2, y - 3, 0, 255, bright, itmSpr->GetSprite(80), DISPLAY_DRAWME | DISPLAY_SHADOW);
			break;
		case ITM_BRWNROCK:
			SprDraw(x, y - 4, 0, 255, bright, itmSpr->GetSprite(87), DISPLAY_DRAWME);
			break;
		case ITM_SIGN2:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(81), DISPLAY_DRAWME);
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(81), DISPLAY_DRAWME | DISPLAY_SHADOW);
			break;
		case ITM_SIGN3:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(82), DISPLAY_DRAWME);
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(82), DISPLAY_DRAWME | DISPLAY_SHADOW);
			break;
		case ITM_SIGN4:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(83), DISPLAY_DRAWME);
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(83), DISPLAY_DRAWME | DISPLAY_SHADOW);
			break;
		case ITM_SIGN5:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(84), DISPLAY_DRAWME);
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(84), DISPLAY_DRAWME | DISPLAY_SHADOW);
			break;
		case ITM_SIGN6:
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(85), DISPLAY_DRAWME);
			SprDraw(x, y, 0, 255, bright, itmSpr->GetSprite(85), DISPLAY_DRAWME | DISPLAY_SHADOW);
			break;
		case ITM_BIGROCK:
			SprDraw(x, y - 3, 0, 255, bright, itmSpr->GetSprite(86), DISPLAY_DRAWME);
			SprDraw(x, y - 3, 0, 255, bright, itmSpr->GetSprite(86), DISPLAY_DRAWME | DISPLAY_SHADOW);
			break;
	}
}

void InstaRenderItem(int x, int y, byte type, char bright, MGLDraw *mgl)
{
	x -= 3;

	switch (type) {
		case ITM_NONE:
			break;
		case ITM_HAMMERUP:
			itmSpr->GetSprite(0)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_PANTS:
			itmSpr->GetSprite(3)->DrawBright(x, y - 2, mgl, bright);
			break;
		case ITM_REVERSE:
			itmSpr->GetSprite(4)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_REFLECT:
			itmSpr->GetSprite(5)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_MISSILES:
			itmSpr->GetSprite(6)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_AK8087:
			itmSpr->GetSprite(33)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_TAKEOUT:
			itmSpr->GetSprite(38)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_SHIELD:
			itmSpr->GetSprite(39)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_BOMBS:
			itmSpr->GetSprite(35)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_FLAME:
			itmSpr->GetSprite(34)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_BRAIN:
			itmSpr->GetSprite(7)->DrawBright(x, y - 2, mgl, bright);
			break;
		case ITM_KEYCH1:
			itmSpr->GetSprite(24)->DrawBright(x, y - 1, mgl, bright);
			break;
		case ITM_KEYCH2:
			itmSpr->GetSprite(21)->DrawBright(x, y - 1, mgl, bright);
			break;
		case ITM_KEYCH3:
			itmSpr->GetSprite(22)->DrawBright(x, y - 1, mgl, bright);
			break;
		case ITM_KEYCH4:
			itmSpr->GetSprite(23)->DrawBright(x, y - 1, mgl, bright);
			break;
		case ITM_KEY:
			itmSpr->GetSprite(17)->DrawBright(x, y - 1, mgl, bright);
			break;
		case ITM_KEYR:
			itmSpr->GetSprite(18)->DrawBright(x, y - 1, mgl, bright);
			break;
		case ITM_KEYG:
			itmSpr->GetSprite(19)->DrawBright(x, y - 1, mgl, bright);
			break;
		case ITM_KEYB:
			itmSpr->GetSprite(20)->DrawBright(x, y - 1, mgl, bright);
			break;
		case ITM_LOONYKEY:
			itmSpr->GetSprite(25)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_BOX:
			itmSpr->GetSprite(1)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_TREE:
			itmSpr->GetSprite(2)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_PINE:
			itmSpr->GetSprite(40)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_PALM:
			itmSpr->GetSprite(41)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_IGLOO:
			itmSpr->GetSprite(42)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_DOOR1:
			itmSpr->GetSprite(9)->DrawBright(x - 13, y + 9, mgl, bright);
			break;
		case ITM_DOOR1R:
			itmSpr->GetSprite(10)->DrawBright(x - 13, y + 9, mgl, bright);
			break;
		case ITM_DOOR1G:
			itmSpr->GetSprite(11)->DrawBright(x - 13, y + 9, mgl, bright);
			break;
		case ITM_DOOR1B:
			itmSpr->GetSprite(12)->DrawBright(x - 13, y + 9, mgl, bright);
			break;
		case ITM_DOOR2:
			itmSpr->GetSprite(13)->DrawBright(x - 13, y + 9, mgl, bright);
			break;
		case ITM_DOOR2R:
			itmSpr->GetSprite(14)->DrawBright(x - 13, y + 9, mgl, bright);
			break;
		case ITM_DOOR2G:
			itmSpr->GetSprite(15)->DrawBright(x - 13, y + 9, mgl, bright);
			break;
		case ITM_DOOR2B:
			itmSpr->GetSprite(16)->DrawBright(x - 13, y + 9, mgl, bright);
			break;
		case ITM_HOLETREE:
			itmSpr->GetSprite(26)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_STUMP:
			itmSpr->GetSprite(27)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_POST:
			itmSpr->GetSprite(28)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_SIGN:
			itmSpr->GetSprite(29)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_BUSH:
			itmSpr->GetSprite(30)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_BIGROCKS:
			itmSpr->GetSprite(31)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_SMLROCKS:
			itmSpr->GetSprite(32)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_CHAIR1:
			itmSpr->GetSprite(36)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_CHAIR2:
			itmSpr->GetSprite(37)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_PWRARMOR:
			itmSpr->GetSprite(43)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_BIGAXE:
			itmSpr->GetSprite(44)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_LIGHTNING:
			itmSpr->GetSprite(46)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_SPEAR:
			itmSpr->GetSprite(45)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_MACHETE:
			itmSpr->GetSprite(47)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_MINES:
			itmSpr->GetSprite(48)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_GARLIC:
			itmSpr->GetSprite(49)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_ORBITER:
			itmSpr->GetSprite(58)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_ACCEL:
			itmSpr->GetSprite(59)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_WEB:
			itmSpr->GetSprite(54)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_WEB2:
			itmSpr->GetSprite(55)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_WEB3:
			itmSpr->GetSprite(56)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_WEB4:
			itmSpr->GetSprite(57)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_DEADTREE:
			itmSpr->GetSprite(52)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_DEADTREE2:
			itmSpr->GetSprite(53)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_FATPALM:
			itmSpr->GetSprite(60)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_GRASS:
			itmSpr->GetSprite(61)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_GRASS2:
			itmSpr->GetSprite(62)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_TREE2:
			itmSpr->GetSprite(63)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_WALLGRASS:
			itmSpr->GetSprite(66)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_VILLAGE:
			itmSpr->GetSprite(65)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_MINEBLOCK:
			itmSpr->GetSprite(64)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_TURRETWPN:
			itmSpr->GetSprite(68)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_MINDCONTROL:
			itmSpr->GetSprite(69)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_REFLECTOR:
			itmSpr->GetSprite(67)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_INVIS:
			itmSpr->GetSprite(88)->DrawGlow(x, y, mgl, bright);
			break;
		case ITM_JETPACK:
			itmSpr->GetSprite(71)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_UNHAMMER:
			itmSpr->GetSprite(0)->DrawOffColor(x, y, mgl, 4, 0, bright);
			break;
		case ITM_UNPANTS:
			itmSpr->GetSprite(3)->DrawOffColor(x, y, mgl, 3, 0, bright);
			break;
		case ITM_SWAPGUN:
			itmSpr->GetSprite(70)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_BADCHINESE:
			itmSpr->GetSprite(38)->DrawOffColor(x, y, mgl, 4, 1, bright);
			break;
		case ITM_BARREL:
			itmSpr->GetSprite(72)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_BARREL2:
			itmSpr->GetSprite(73)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_BARREL3:
			itmSpr->GetSprite(74)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_TRASHCAN:
			itmSpr->GetSprite(75)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_TRASHCAN2:
			itmSpr->GetSprite(76)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_CRATE:
			itmSpr->GetSprite(77)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_CRATE2:
			itmSpr->GetSprite(78)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_BUSH2:
			itmSpr->GetSprite(79)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_BUSH3:
			itmSpr->GetSprite(80)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_BRWNROCK:
			itmSpr->GetSprite(87)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_SIGN2:
			itmSpr->GetSprite(81)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_SIGN3:
			itmSpr->GetSprite(82)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_SIGN4:
			itmSpr->GetSprite(83)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_SIGN5:
			itmSpr->GetSprite(84)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_SIGN6:
			itmSpr->GetSprite(85)->DrawBright(x, y, mgl, bright);
			break;
		case ITM_BIGROCK:
			itmSpr->GetSprite(86)->DrawBright(x, y, mgl, bright);
			break;
	}
}

void DrawRedX(int x, int y, MGLDraw *mgl)
{
	itmSpr->GetSprite(8)->Draw(x, y, mgl);
}

void ItemLightUp(void)
{
	itmLight = 1 - itmLight;
}

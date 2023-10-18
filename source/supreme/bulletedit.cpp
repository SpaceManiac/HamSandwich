#include "bulletedit.h"

#include "bullet.h"
#include "dialogbits.h"
#include "editor.h"
#include "specialedit.h"

static byte rememberMode;
static bool realClick = false;

#define ID_PICKBULLET  500

void BulletPickClick(int id)
{
	if (!realClick) return;

	PickedTile(id - ID_PICKBULLET);
	MakeNormalSound(SND_MENUCLICK);
	SetEditMode(rememberMode);
}

void BulletEditSetupButtons(bool allowAnything)
{
	ClearButtons();

	const int start = allowAnything ? BLT_NONE : BLT_HAMMER;
	const int width = 160;
	int x = 0, y = 0;
	for (byte i = start; i < BLT_NUM; ++i)
	{
		std::string name = BulletName(i);
		MakeButton(BTN_NORMAL, ID_PICKBULLET + i, true, x, y, width, 16, name.c_str(), BulletPickClick);
		y += 16;

		if (y > 480 - 32) {
			y = 0;
			x += width;
		}
	}
}

void BulletEdit_Init(byte modeFrom, bool allowAnything)
{
	rememberMode = modeFrom;
	GetDisplayMGL()->MouseTap();
	BulletEditSetupButtons(allowAnything);
}

void BulletEdit_Exit()
{

}

void BulletEdit_Update(int mouseX, int mouseY, MGLDraw* mgl)
{
	realClick = mgl->MouseTap();
	CheckButtons(mouseX, mouseY);
}

void BulletEdit_Render(int mouseX, int mouseY, MGLDraw* mgl)
{
	mgl->ClearScreen();

	RenderButtons(mouseX, mouseY, mgl);
}

void BulletEdit_Key(char k)
{
	if (k == 27) {
		PickedTile(-1);
		SetEditMode(rememberMode);
	}
}

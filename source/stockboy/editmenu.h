#ifndef EDITMENU_H
#define EDITMENU_H

#include "editor.h"
#include "yesnomenu.h"
#include "toolmenu.h"

// which menu is active
#define MENU_FILE	0
#define MENU_SETOPT	1
#define MENU_LVLOPT	2
#define MENU_FLOOR	3
#define MENU_ITEMS	4
#define MENU_CRITTERS 5
#define MENU_EXTRA	6
#define MENU_SCORE  7

#define FNAMELEN	32

typedef class EditMenu
{
	public:
		EditMenu(Map *map,YesNoMenu *yesNoMenu,ToolMenu *toolMenu,MGLDraw *mymgl);
		~EditMenu(void);

		void Render(int msx,int msy);
		void RenderFileWindow(int msx,int msy);
		void RenderSetWindow(int msx,int msy);
		void RenderLevelWindow(int msx,int msy);
		void RenderFloorWindow(int msx,int msy);
		void RenderCritterWindow(int msx,int msy);
		void RenderItemWindow(int msx,int msy);
		void RenderExtraWindow(int msx,int msy);
		void RenderScoreWindow(int msx,int msy);

		byte Click(int msx,int msy,byte btn);
		void FileClick(int msx,int msy,byte btn);
		void LevelOptClick(int msx,int msy,byte btn);
		void SetOptClick(int msx,int msy,byte btn);
		void FloorClick(int msx,int msy,byte btn);
		void ExtraClick(int msx,int msy,byte btn);
		void ScoreClick(int msx,int msy,byte btn);

		void MouseHold(int msx,int msy,byte btn);

		void DraggedByMouse(int msx,int msy);
		byte KeyPress(char c);
		byte AlwaysKeyPress(char c,byte x,byte y);	// even when hidden

		void GetAnswer(void);

		void SetScoreTxt(void);
		void SetScores(void);

		void ScanFiles(void);
		byte CanEditFile(const char *fname);

		byte ShowGuys(void);
		byte ShowItems(void);
		byte DelMode(void);

		byte minmax;
		byte winminmax;
		byte menu;
		int x,y;
		int relmsx,relmsy;
		MGLDraw *mgl;
		int numFiles;
		int maxFiles;
		char *filename;
		int curFile;
		int filePos;
		int curLvl;
		int lvlPos;
		byte askingFor;
		char curFname[FNAMELEN+2];
		char scoreTxt[3][16];
		byte curFlash;
		byte extraMode;
		byte scoreMode;

		byte showGuys,showItems,delMode;

		YesNoMenu *yesno;
		ToolMenu *toolmenu;
		byte msClock;
		byte msRep;
} EditMenu;

#endif

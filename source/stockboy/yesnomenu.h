#ifndef YESNOMENU_H
#define YESNOMENU_H

#include "editor.h"

#define ANSWER_NONE	0
#define ANSWER_NO	1
#define ANSWER_YES	2

typedef class YesNoMenu
{
	public:
		YesNoMenu(MGLDraw *mymgl);
		~YesNoMenu(void);

		void Activate(char *settitle,char *settitle2,char *setyes,char *setno);
		void Render(int msx,int msy);
		
		byte Click(int msx,int msy,byte btn);
		byte KeyPress(char c);

		int x,y;
		int active;
		char title[64];
		char title2[64];
		char yes[16];
		char no[16];
		byte answer;
		MGLDraw *mgl;
} YesNoMenu;

#endif
#include "winpch.h"
#include "mgldraw.h"
#include "jamulfont.h"
#include "jamulsound.h"
#include <shellapi.h>
#include "leakcheck.h"
#include "config.h"

#include "game.h"
#include "editor.h"
#include "tile.h"
#include "sound.h"
#include "monster.h"
#include "title.h"
#include "options.h"
#include "sound.h"
#include "music.h"

MGLDraw *mainmgl;

// SPISPOPD helper for atkins!
/*
void GetPal(void)
{
	FILE *f;
	byte *b;

	b=(byte *)malloc(320*200+768);
	f=fopen("c:\\work\\fini.256","rb");
	fread(b,320*200+768,1,f);
	fclose(f);

	f=fopen("c:\\work\\spis_fight.PspPalette","wt");
	fprintf(f,"JASC-PAL\n0100\n256\n");
	for(int i=0;i<256;i++)
	{
		fprintf(f,"%d %d %d\n",b[320*200+i*3+0]*4,b[320*200+i*3+1]*4,b[320*200+i*3+2]*4);
	}
	fclose(f);
	free(b);
}
*/

int main(int argc, char* argv[])
{
	EnableLeakCheck();

	bool windowedGame=false;
	for (int i = 1; i < argc; ++i)
	{
		if (!strcmp(argv[i], "window"))
			windowedGame=true;
	}
	LoadConfig();
	mainmgl=new MGLDraw("Loonyland 2",640,480,8,windowedGame);

	if(!mainmgl)
		return 0;

	LunaticInit(mainmgl);
	//NewComputerSpriteFix("graphics\\villager.jsp",0,-6);
	//set_keyboard_rate(400,80);
	while(1)
	{
		player.var[VAR_COMMENTARY]=0;
		Song(SONG_FACTORY);
		switch(MainMenu(mainmgl))
		{
			case 255:	// quit
			case MENU_EXIT:
				LunaticExit();
				delete mainmgl;
				FatalErrorQuit();
				return 0;
				break;
			case MENU_NEWCHAR:	// new game
				//set_keyboard_rate(0,0);
				LunaticGame(mainmgl,0,WORLD_NORMAL);
				//set_keyboard_rate(400,80);
				break;
			case MENU_PLAY:	// continue
				//set_keyboard_rate(0,0);
				LunaticGame(mainmgl,1,WORLD_NORMAL);
				//set_keyboard_rate(400,80);
				JamulSoundPurge();
				break;
			case MENU_EDITOR:	// editor
				LunaticEditor(mainmgl);
				mainmgl->LastKeyPressed();
				break;
		}
	}
}

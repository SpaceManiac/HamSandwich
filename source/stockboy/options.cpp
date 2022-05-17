#include "options.h"
#include "profile.h"
#include "appdata.h"

options_t opt;

void LoadOptions(void)
{
	FILE *f;
	int i;

	f=AppdataOpen("stock.cfg");
	if(!f)
	{
		opt.sound=1;

		opt.control[0][0]=SDL_SCANCODE_UP;	// up
		opt.control[0][1]=SDL_SCANCODE_DOWN;	// down
		opt.control[0][2]=SDL_SCANCODE_LEFT;	// left
		opt.control[0][3]=SDL_SCANCODE_RIGHT;	// right
		opt.control[0][4]=SDL_SCANCODE_LCTRL;	// hammer: CTRL
		opt.control[0][5]=SDL_SCANCODE_LSHIFT;	// special: shift
		opt.control[1][0]=0;	// up
		opt.control[1][1]=0;	// down
		opt.control[1][2]=0;	// left
		opt.control[1][3]=0;	// right
		opt.control[1][4]=SDL_SCANCODE_Z;	// hammer: Z
		opt.control[1][5]=SDL_SCANCODE_X;	// special: X

		opt.joyCtrl[0]=0;
		opt.joyCtrl[1]=1;

		opt.blowoutDiff=2;
		opt.clearanceDiff=2;
		opt.pestDiff=2;

		opt.curProfile=0;

		for(i=0;i<NUM_GAMES;i++)
			if(i!=GAME_ADDON)
				ResetHighScores(i);
	}
	else
	{
		fread(&opt,sizeof(options_t),1,f);
		fclose(f);
	}
	InitProfile();
}

void SaveOptions(void)
{
	FILE *f;

	f=AppdataOpen_Write("stock.cfg");
	fwrite(&opt,sizeof(options_t),1,f);
	fclose(f);
}

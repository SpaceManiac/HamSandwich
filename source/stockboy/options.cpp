#include "options.h"
#include "profile.h"

options_t opt;

void LoadOptions(void)
{
	FILE *f;
	int i;

	f=fopen("stock.cfg","rb");
	if(!f)
	{
		opt.sound=1;

		opt.control[0][0]=72;	// up
		opt.control[0][1]=80;	// down
		opt.control[0][2]=75;	// left
		opt.control[0][3]=77;	// right
		opt.control[0][4]=29;	// hammer: CTRL
		opt.control[0][5]=42;	// special: shift
		opt.control[1][0]=0;	// up
		opt.control[1][1]=0;	// down
		opt.control[1][2]=0;	// left
		opt.control[1][3]=0;	// right
		opt.control[1][4]=44;	// hammer: Z
		opt.control[1][5]=45;	// special: X
		
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

	f=fopen("stock.cfg","wb");
	fwrite(&opt,sizeof(options_t),1,f);
	fclose(f);
}

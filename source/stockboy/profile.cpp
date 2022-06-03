#include "profile.h"
#include "options.h"
#include "monster.h"
#include "appdata.h"
#include "control.h"

profile_t profile;

void ApplyControlSettings(void)
{
	byte joyBinds[2] = { 0, 1 };
	SetKeyboardBindings(0, 5, profile.keys);
	SetJoystickBindings(2, joyBinds);
}

void InitProfile(void)
{
	if(!LoadProfile(opt.curProfile))
	{
		// try to load profile #0 then
		if(!LoadProfile(0))
		{
			// okay, there appear to be little or no profiles...
			DefaultProfile(&profile);
			SaveProfile(0);
			opt.curProfile=0;
		}
		else
		{
			// if it's there, then load it and make it active
			opt.curProfile=0;
		}
	}
	ApplyControlSettings();
	SetupPlayerSprite();
}

byte LoadProfile(byte n)
{
	FILE *f;
	char name[32];

	sprintf(name,"profile%03d.pro",n);
	f=AppdataOpen(name);
	if(!f)
		return 0;

	if(fread(&profile,sizeof(profile_t),1,f)!=1)
	{
		fclose(f);
		return 0;
	}
	fclose(f);
	SetVolumes(profile.sound,profile.music);
	return 1;
}

byte FindOpenProfile(void)
{
	int i,thisNum;
	byte used[MAX_PROFILES];
	char numtxt[5];

	for(i=0;i<MAX_PROFILES;i++)
		used[i]=0;

	for (const auto& file : ListDirectory(".", ".pro"))
	{
		strncpy(numtxt,&file[7],3);
		thisNum=atoi(numtxt);
		if(thisNum<MAX_PROFILES)
			used[thisNum]=1;
	}

	for(i=0;i<MAX_PROFILES;i++)
		if(!used[i])
			return i;

	return 255;
}

byte SaveProfile(byte n)
{
	FILE *f;
	char name[32];

	sprintf(name,"profile%03d.pro",n);
	f=AppdataOpen_Write(name);
	if(!f)
		return 0;

	if(fwrite(&profile,sizeof(profile_t),1,f)!=1)
	{
		fclose(f);
		return 0;
	}
	fclose(f);
	return 1;
}

void DefaultProfile(profile_t *p)
{
	int i,j;

	strcpy(p->name,"Kiddo");
	p->character=0;

	for(i=0;i<10;i++)
		p->training[i]=0;
	for(i=0;i<13;i++)
	{
		p->empMonth[i]=0;
		p->empMonth2[i]=0;
		p->zodiac[i]=0;
		p->zodiac2[i]=0;
	}
	p->sound=255;
	p->music=255-64;
	p->colorblind=0;

	p->empYear=0;
	p->empYear2=0;
	p->keys[0]=SDL_SCANCODE_UP;	// up arrow
	p->keys[1]=SDL_SCANCODE_DOWN;	// down arrow
	p->keys[2]=SDL_SCANCODE_LEFT;	// left arrow
	p->keys[3]=SDL_SCANCODE_RIGHT;	// right arrow
	p->keys[4]=SDL_SCANCODE_LCTRL;	// control
	p->color=7;
	p->bright=0;
	for(i=0;i<16;i++)
		p->giftShop[i]=0;
	p->stars=0;
	p->starsLeft=0;
	p->bestBounce=0;

	for(i=0;i<NUM_GAMES;i++)
		for(j=0;j<MAX_SCORES;j++)
		{
			p->starGot[i][j]=0;
		}
	strcpy(p->songChoice[SONG_TITLE],"title.sng");
	strcpy(p->songChoice[SONG_MENU],"menu.sng");
	strcpy(p->songChoice[SONG_PLAY],"action1.sng");
	strcpy(p->songChoice[SONG_PLAY2],"action2.sng");
	strcpy(p->songChoice[SONG_PLAY3],"action3.sng");
	strcpy(p->songChoice[SONG_PLAY4],"action4.sng");
	strcpy(p->songChoice[SONG_PLAY5],"action5.sng");
	SetVolumes(p->sound,p->music);
}

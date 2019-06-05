#include "music.h"
#include "mgldraw.h"
#include "progress.h"
#include "config.h"
#include "editor.h"
#include <SDL2/SDL_mixer.h>

Mix_Music *curStream=NULL;
char curSongName[64];
int musVolume=255;
byte lastSong=255;
byte dontcallback=0;

void PickSongToPlay(void)
{
	if(!config.music)
		return;

	if(!strcmp(curSongName,"hicksong.ogg"))
		PlaySongForce("unspeakable.ogg");
	else
		PlaySongForce(curSongName);
}

void UpdateMusic(void)
{
	if (config.music && curStream && !Mix_PlayingMusic())
		if (!dontcallback)
			PickSongToPlay();
}

void PlaySong(const char *fname)
{
	char fullname[64];

	if(!config.music)
		return;

	if(!strcmp(curSongName,fname) && fname[0]!='\0')
		return;	// no need!

	if(fname[0]=='\0')
	{
		StopSong();
		return;
	}

	PlaySongForce(fname);
}

void PlaySongForce(const char *fname)
{
	char fullname[64];

	if(!config.music)
		return;

	strcpy(curSongName,fname);
	sprintf(fullname,"music/%s",fname);
	StopSong();

	SDL_RWops* rw = SDL_RWFromFile(fullname, "rb");
	if(!rw)
	{
		printf("%s: %s\n", fullname, SDL_GetError());
		return;
	}

	curStream=Mix_LoadMUS_RW(rw, 1);
	if(!curStream)
	{
		printf("%s: %s\n", fullname, Mix_GetError());
		return;
	}

	Mix_VolumeMusic(musVolume / 2);
	Mix_PlayMusic(curStream, 1);
	UpdateMusic();
}

void StopSong(void)
{
	if(!config.music)
		return;

	dontcallback=1;
	if(curStream)
	{
		Mix_HaltMusic();
		Mix_FreeMusic(curStream);
		curStream=NULL;
	}
	dontcallback=0;
}

void SetMusicVolume(int vol)
{
	if(!config.music)
		return;

	musVolume=vol;
	if(curStream)
	{
		Mix_VolumeMusic(musVolume / 2);
	}
}

const char *CurSongTitle(void)
{
	return curSongName;
}

void PlayNextSong(void)
{
	byte pl;

	if(!config.music)
		return;

	if((profile.musicMode&MUSIC_LISTBITS)==MUSIC_OFFICIAL)
		return;	// can't

	pl=(profile.musicMode&MUSIC_LISTBITS)-1;

	lastSong++;
	if(lastSong>=profile.playList[pl].numSongs)
	{
		lastSong=0;
		if(profile.playList[pl].numSongs==0)
		{
			StopSong();
			return;
		}
	}
	PlaySongForce(&profile.playList[pl].song[lastSong*SONGNAME_LEN]);
}

void PlayPrevSong(void)
{
	byte pl;

	if(!config.music)
		return;

	if((profile.musicMode&MUSIC_LISTBITS)==MUSIC_OFFICIAL)
		return;	// can't

	pl=(profile.musicMode&MUSIC_LISTBITS)-1;

	lastSong--;
	if(lastSong>250)
	{
		lastSong=profile.playList[pl].numSongs-1;
		if(profile.playList[pl].numSongs==0)
		{
			StopSong();
			return;
		}
	}
	PlaySongForce(&profile.playList[pl].song[lastSong*SONGNAME_LEN]);
}

#include "music.h"
#include "mgldraw.h"
#include "options.h"
#include "editor.h"
#include <SDL2/SDL_mixer.h>

Mix_Music *curStream=NULL;
char curSongName[64];
int musVolume=255;
byte lastSong=255;
byte dontcallback=0;

void Song(byte w)
{
	char s[32];

	if(w==lastSong)
		return;	// no need, it's already playing our song

	sprintf(s,"song%03d.wav",w-SONG_UNDERWORLD);

	PlaySong(s);
	lastSong=w;
}

void UpdateMusic(void)
{
	if (curStream && !Mix_PlayingMusic())
	{
		if (!dontcallback)
			PlaySong(curSongName);
	}
}

void PlaySong(char *fname)
{
	char fullname[64];

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

void StopSong2(void)
{
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
	musVolume=vol;
	if(curStream)
	{
		Mix_VolumeMusic(musVolume / 2);
	}
}

char *CurSongTitle(void)
{
	return curSongName;
}

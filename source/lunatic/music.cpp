#include "music.h"
#include "mgldraw.h"
#include "options.h"
#include <SDL2/SDL_mixer.h>

byte currentMode;
Mix_Music *curStream=NULL;
int isPlaying;
int trackNum;

byte MusicInit(void)
{
	currentMode = CD_OFF;
	curStream = nullptr;
	return true;
}

void MusicExit(void)
{
	CDStop();
}

// SDL2_Mixer audio stuff

void CDPlay(int track)
{
	if (trackNum == track && curStream && isPlaying)
	{
		return; // Already playing that track
	}

	char fullname[32];
	sprintf(fullname, "sound/mus%03d.ogg", track);
	trackNum = track;
	if (curStream)
	{
		Mix_HaltMusic();
		Mix_FreeMusic(curStream);
		curStream=NULL;
	}

	SDL_RWops* rw = SDL_RWFromFile(fullname, "rb");
	if(!rw)
	{
		printf("%s: %s\n", fullname, SDL_GetError());
		return;
	}

	curStream=Mix_LoadMUS_RW(rw, 1);
	if(!curStream)
	{
		printf("Mix_LoadMUS(%s): %s\n", fullname, Mix_GetError());
		return;
	}

	Mix_VolumeMusic(128);
	Mix_PlayMusic(curStream, 1);
}

void CDNeedsUpdating(void)
{
}

void CDPlayerUpdate(byte mode)
{
	isPlaying = 0;
	if (curStream)
		isPlaying = Mix_PlayingMusic();

	bool modeChanged = currentMode != mode;
	currentMode = mode;

	if (!isPlaying || modeChanged)
	{
		switch (currentMode)
		{
			case CD_LOOPTRACK:
				CDPlay(trackNum);
				break;
			case CD_INTROLOOP:
				CDPlay(trackNum + 1);
				currentMode = CD_LOOPTRACK;
				break;
			case CD_RANDOM:
				CDPlay(3 + MGL_random(15));
				break;
			case CD_NORMAL:
				if (!isPlaying)
				{
					int newTrack = trackNum + 1;
					if (newTrack > 18) newTrack = 3;
					CDPlay(newTrack);
				}
				break;
			case CD_OFF:
			default:
				if (isPlaying)
				{
					CDStop();
				}
				break;
		}
	}
}

void CDStop(void)
{
	if (curStream)
	{
		Mix_HaltMusic();
		Mix_FreeMusic(curStream);
		curStream=NULL;
	}
}

byte CDLoaded(void)
{
	return 1;
}

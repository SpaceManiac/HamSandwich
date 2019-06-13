#include "hammusic.h"
#include "mgldraw.h"
#include "log.h"
#include <stdio.h>

#ifdef SDL_UNPREFIXED
	#include <SDL_mixer.h>
#else  // SDL_UNPREFIXED
	#include <SDL2/SDL_mixer.h>
#endif  // SDL_UNPREFIXED

static Mix_Music* curStream = nullptr;
static int musVolume = 255;

void UpdateMusic()
{
	if (ConfigMusicEnabled() && curStream && !Mix_PlayingMusic())
	{
		Mix_PlayMusic(curStream, 1);
		ChooseNextSong();
	}
}

void SetMusicVolume(int vol)
{
	musVolume = vol;
	if (curStream)
	{
		Mix_VolumeMusic(musVolume / 2);
	}
}

void PlaySongFile(const char* fullname)
{
	if (!ConfigMusicEnabled())
		return;

	StopSong();

	SDL_RWops* rw = SDL_RWFromFile(fullname, "rb");
	if (!rw)
	{
		LogError("Open(%s): %s", fullname, SDL_GetError());
		return;
	}

	curStream = Mix_LoadMUS_RW(rw, 1);
	if (!curStream)
	{
		LogError("LoadMUS(%s): %s", fullname, Mix_GetError());
		return;
	}

	Mix_VolumeMusic(musVolume / 2);
	Mix_PlayMusic(curStream, 1);
	UpdateMusic();
}

void StopSong()
{
	if (curStream)
	{
		Mix_HaltMusic();
		Mix_FreeMusic(curStream);
		curStream = nullptr;
	}
}

bool IsSongPlaying()
{
	return curStream && Mix_PlayingMusic();
}

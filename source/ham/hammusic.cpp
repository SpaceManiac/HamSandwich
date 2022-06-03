#include "hammusic.h"
#include "mgldraw.h"
#include "log.h"
#include "appdata.h"
#include "extern.h"
#include <stdio.h>

#include <SDL_mixer.h>

static Mix_Music* curStream = nullptr;
static int musVolume = 255;

void UpdateMusic()
{
	if (g_HamExtern.ConfigMusicEnabled && g_HamExtern.ConfigMusicEnabled() && curStream && !Mix_PlayingMusic())
	{
		Mix_PlayMusic(curStream, 1);
		if (g_HamExtern.ChooseNextSong)
			g_HamExtern.ChooseNextSong();
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
	if (!g_HamExtern.ConfigMusicEnabled || !g_HamExtern.ConfigMusicEnabled())
		return;

	StopSong();

	SDL_RWops* rw = AssetOpen_SDL(fullname);
	if (!rw)
	{
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

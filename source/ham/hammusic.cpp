#include "hammusic.h"
#include <stdio.h>
#include <utility>
#include <SDL_mixer.h>
#include "mgldraw.h"
#include "log.h"
#include "appdata.h"
#include "extern.h"
#include "owned_mixer.h"

namespace
{
	bool musicEnabled = true;

	owned::Mix_Music curStream = nullptr;
	int musVolume = 255;
}

void SetHamMusicEnabled(bool enabled)
{
	musicEnabled = enabled;
}

void UpdateMusic()
{
	if (musicEnabled && curStream && !Mix_PlayingMusic())
	{
		Mix_PlayMusic(curStream.get(), 1);
		if (g_HamExtern.ChooseNextSong)
			g_HamExtern.ChooseNextSong();
	}
}

void SetMusicVolume(int vol)
{
	musVolume = vol;
	Mix_VolumeMusic(musVolume / 2);
}

void PlaySongFile(const char* fullname)
{
	if (!musicEnabled)
		return;

	StopSong();

	owned::SDL_RWops rw = AppdataOpen(fullname);
	if (!rw)
	{
		return;
	}

	curStream = owned::Mix_LoadMUS_RW(std::move(rw));
	if (!curStream)
	{
		LogError("Mix_LoadMUS(%s): %s", fullname, Mix_GetError());
		return;
	}

	Mix_VolumeMusic(musVolume / 2);
	if (Mix_PlayMusic(curStream.get(), 1))
	{
		LogError("Mix_PlayMusic(%s): %s", fullname, Mix_GetError());
	}
	UpdateMusic();
}

void StopSong()
{
	if (curStream)
	{
		Mix_HaltMusic();
		curStream.reset();
	}
}

bool IsSongPlaying()
{
	return curStream && Mix_PlayingMusic();
}

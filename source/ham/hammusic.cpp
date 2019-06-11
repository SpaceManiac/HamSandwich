#include "hammusic.h"
#include "mgldraw.h"

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
		printf("UpdateMusic() noticed music is done\n");
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
		printf("Open(%s): %s\n", fullname, SDL_GetError());
		ChooseNextSong();  // On playlists, one error shouldn't stop everything.
		return;
	}

	curStream = Mix_LoadMUS_RW(rw, 1);
	if (!curStream)
	{
		printf("LoadMUS(%s): %s\n", fullname, Mix_GetError());
		ChooseNextSong();  // On playlists, one error shouldn't stop everything.
		return;
	}

	printf("OK: %s\n", fullname);
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

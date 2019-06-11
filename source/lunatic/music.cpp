#include "music.h"
#include "mgldraw.h"
#include "options.h"
#include "player.h"
#include "hammusic.h"

static int trackNum = 0;

void CDPlay(int track)
{
	if (trackNum == track)
	{
		return; // Already playing that track
	}

	char fullname[64];
	sprintf(fullname, "sound/mus%03d.ogg", track);
	trackNum = track;
	PlaySongFile(fullname);
}

void ChooseNextSong()
{
	switch (PlayerGetMusicSettings())
	{
		case MUSIC_ON:
			// Default behavior is to loop.
			break;

		case MUSIC_RAND:
			// Pick the next song at random.
			CDPlay(3 + MGL_random(15));
			break;

		// Dropped modes:
		// - introloop:
		// - normal: add 1 to track, if > 18 loop to 3

		case MUSIC_OFF:
		default:
			StopSong();
			break;
	}
}

void CDStop(void)
{
	StopSong();
}

bool ConfigMusicEnabled()
{
	return true;
}

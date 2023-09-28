#include "sound.h"
#include <stdio.h>
#include "jamulsound.h"
#include "hammusic.h"
#include "options.h"

void PlaySound(int sound, int priority)
{
	if (ConfigGetSoundEnabled())
	{
		GoPlaySound(sound, 0, 0, SND_CUTOFF, priority);
	}
}

void PlaySong(int song)
{
	if (ConfigGetMusicEnabled())
	{
		char buf[64];
		snprintf(buf, sizeof(buf), "sound/snd%03d.wav", song);
		PlaySongFile(buf);
	}
}

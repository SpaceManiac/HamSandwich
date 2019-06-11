#include "sound.h"
#include "display.h"
#include "options.h"
#include "music.h"

byte soundAvailable = 0;

void SoundSystemExists(void)
{
	soundAvailable = 1;
}

void InitSound(void)
{
	JamulSoundPurge();
}

void ExitSound(void)
{
	// don't need to do nothing, it's handled by jamulsoundexit
}

void MakeSound(int snd, int x, int y, int flags, int priority)
{
	long pan, vol;
	int cx, cy;

	if (!soundAvailable)
		return;
	if (!opt.sound)
		return;

	GetCamera(&cx, &cy);
	x >>= FIXSHIFT;
	y >>= FIXSHIFT;
	pan = 127 + (x - cx)*127/800; // (x-cx)*2 in range -1600 to 1600, this is 0-255
	vol = -((x - cx)*(x - cx)+(y - cy)*(y - cy)) / 128;
	if (vol<-5000)
		return; // too quiet to play
	vol = vol * 255 / 5000 + 255;
	GoPlaySound(snd, pan, vol, (byte) flags, priority);
}

void MakeNormalSound(int snd)
{
	if (!soundAvailable)
		return;

	if (!opt.sound)
		return;

	GoPlaySound(snd, 0, 255, SND_MAXPRIORITY | SND_CUTOFF | SND_ONE, MAX_SNDPRIORITY);
}

bool ConfigSoundEnabled()
{
	return 1;
}

int ConfigNumSounds()
{
	return 32;
}

SDL_RWops* SoundLoadOverride(int which)
{
	return nullptr;
}

#include "jamulsound.h"
#include "mgldraw.h"

const int MAX_FEW_SOUNDS = 2;	// most copies of a SND_FEW sound that can play at once
const int MAX_SOUNDS_AT_ONCE = 16;

// internal sound playing flags
enum {
	SOUND_LOOP = 1,		// loop the sound indefinitely (actually does nothing)
	SOUND_CUTOFF = 2	// if the copy of the sound is busy, cut it off and restart
};

// a loaded sound buffer
struct soundbuf_t
{
	SAMPLE *sample;
};

static int soundbufSize = 0;
static soundbuf_t *soundbuf = NULL;

// a sound currently playing
struct sound_t
{
	int voice; // allegro handle
	int soundNum; // which game sound number
	int priority;
	long pan, vol;
	byte flags;
};

sound_t playBuffer[MAX_SOUNDS_AT_ONCE];

bool JamulSoundInit(int numBuffers)
{
	int i;

	soundbufSize = numBuffers;
	soundbuf = new soundbuf_t[numBuffers];
	for (i = 0; i < numBuffers; i++)
	{
		soundbuf[i].sample = NULL;
	}

	for (i = 0; i < MAX_SOUNDS_AT_ONCE; i++)
	{
		playBuffer[i].soundNum = -1;
		playBuffer[i].voice = -1;
		playBuffer[i].flags = 0;
	}
	return TRUE;
}

void JamulSoundDestroyBuffer(int which)
{
	if (soundbuf[which].sample)
	{
		destroy_sample(soundbuf[which].sample);
		soundbuf[which].sample = NULL;
	}
}

void JamulSoundExit(void)
{
	int i;

	if (soundbuf)
	{
		for (i = 0; i < soundbufSize; i++)
			JamulSoundDestroyBuffer(i);
		delete[] soundbuf;
	}
}

bool JamulSoundPlay(int voice, long pan, long vol, byte playFlags)
{
	// if this copy is in use, can't play it
	if (voice_get_position(voice) > 0)
	{
		if (playFlags & SOUND_CUTOFF)
		{
			voice_set_position(voice, 0);
			// keep going to handle the rest of the stuff
		}
		else
			return FALSE; // can't play if it's playing
	}

	// set the pan and volume and start the voice
	voice_set_volume(voice, vol);
	voice_set_pan(voice, pan);
	voice_start(voice);

	return TRUE;
}

// now here is all the big sound manager stuff, that allows multiple sounds at once

void JamulSoundUpdate(void)
{
	int i;

	for (i = 0; i < MAX_SOUNDS_AT_ONCE; i++)
	{
		if (playBuffer[i].voice != -1 && playBuffer[i].flags & SND_PLAYING)
		{
			if (voice_get_position(playBuffer[i].voice) == -1)
			{
				playBuffer[i].flags &= (~SND_PLAYING);
			}
		}
	}
}

void JamulSoundPurge(void)
{
	int i;

	for (i = 0; i < soundbufSize; i++)
	{
		JamulSoundDestroyBuffer(i);
	}
}

void GoPlaySound(int num, long pan, long vol, byte flags, int priority)
{
	char txt[32];
	int i, best, count;

	// load the sample if it isn't already
	if (soundbuf[num].sample == NULL)
	{
		sprintf(txt, "sound\\snd%03d.wav", num);
		soundbuf[num].sample = load_sample(txt);
		if (soundbuf[num].sample == NULL)
			return; // can't play the sound, it won't load for some reason
	}

	priority += vol; // the quieter a sound, the lower the priority
	if (flags & SND_MAXPRIORITY)
		priority = MAX_SNDPRIORITY;

	if (flags & SND_ONE)
	{
		for (i = 0; i < MAX_SOUNDS_AT_ONCE; i++)
			if (playBuffer[i].soundNum == num)
			{
				// if you want to cut it off, or it isn't playing, then start anew
				if ((flags & SND_CUTOFF) || (!(playBuffer[i].flags & SND_PLAYING)))
				{
					playBuffer[i].pan = pan;
					playBuffer[i].vol = vol;
					playBuffer[i].flags = flags | SND_PLAYING;
					playBuffer[i].priority = priority;
					JamulSoundPlay(playBuffer[i].voice, playBuffer[i].pan, playBuffer[i].vol, SOUND_CUTOFF);
					return; // good job
				}
				else
					return; // can't be played because can't cut it off
			}
		// if you fell through to here, it isn't playing, so go ahead as normal
	}
	if (flags & SND_FEW)
	{
		count = 0;
		for (i = 0; i < MAX_SOUNDS_AT_ONCE; i++)
			if (playBuffer[i].soundNum == num && (playBuffer[i].flags & SND_PLAYING))
				count++;

		if (count >= MAX_FEW_SOUNDS)
		{
			for (i = 0; i < MAX_SOUNDS_AT_ONCE; i++)
				if (playBuffer[i].soundNum == num)
				{
					if ((flags & SND_CUTOFF) && (playBuffer[i].flags & SND_PLAYING))
					{
						playBuffer[i].pan = pan;
						playBuffer[i].vol = vol;
						playBuffer[i].flags = flags | SND_PLAYING;
						playBuffer[i].priority = priority;
						JamulSoundPlay(playBuffer[i].voice, playBuffer[i].pan, playBuffer[i].vol, SOUND_CUTOFF);
						return; // good job
					}
				}
			return; // failed for some reason
		}
	}
	best = -1;
	for (i = 0; i < MAX_SOUNDS_AT_ONCE; i++)
	{
		if (playBuffer[i].soundNum == -1 || (!(playBuffer[i].flags & SND_PLAYING)))
		{
			best = i;
			break; // can't beat that
		}
		if ((playBuffer[i].priority < priority) || (playBuffer[i].soundNum == num && (flags & SND_CUTOFF)))
		{
			if (best == -1 || playBuffer[i].priority < playBuffer[best].priority)
				best = i;
		}
	}
	if (best == -1)
		return; // sound is not worthy to be played

	if (playBuffer[best].soundNum != num) // if it was already playing that sound, don't waste time
	{
		playBuffer[best].soundNum = num;
		if (playBuffer[best].voice != -1)
		{
			deallocate_voice(playBuffer[best].voice); // slash & burn
		}
		playBuffer[best].voice = allocate_voice(soundbuf[num].sample);
	}
	else
	{
		voice_set_position(playBuffer[best].voice, 0);
	}

	if (playBuffer[best].voice == -1)
		return; // can't play it
	playBuffer[best].priority = priority;
	playBuffer[best].pan = pan;
	playBuffer[best].vol = vol;
	playBuffer[best].flags = flags | SND_PLAYING;

	JamulSoundPlay(playBuffer[best].voice, playBuffer[best].pan, playBuffer[best].vol, 0);
}

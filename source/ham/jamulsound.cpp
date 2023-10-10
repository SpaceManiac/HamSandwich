#include "jamulsound.h"
#include "hammusic.h"
#include "log.h"
#include "audiofx.h"
#include "appdata.h"
#include "extern.h"
#include <stdio.h>
#include <limits.h>
#include <memory>

#include <SDL.h>
#include <SDL_mixer.h>

#include "owned_mixer.h"

struct soundList_t
{
	owned::Mix_Chunk sample;
};

struct schannel_t
{
	int soundNum;
	int priority;
	int voice;
	owned::Mix_Chunk sample;
};

namespace
{
	bool soundEnabled = true;
	int configNumSounds = 32;

	int sndVolume;

	int NUM_SOUNDS = 0;

	bool soundIsOn = false;
	int bufferCount;
	std::vector<soundList_t> soundList;
	std::vector<schannel_t> schannel;
}

void SetJamulSoundEnabled(bool enable, int numSounds)
{
	soundEnabled = enable;
	configNumSounds = numSounds;
}

bool JamulSoundInit(int numBuffers)
{
	int i;

	if (!soundEnabled)
	{
		LogDebug("sound disabled in config");
		return false;
	}
	if (SDL_Init(SDL_INIT_AUDIO) != 0)
	{
		LogError("SDL_Init(AUDIO): %s", SDL_GetError());
		return false;
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) != 0)
	{
		LogError("Mix_OpenAudio: %s", Mix_GetError());
		return false;
	}
	if (Mix_Init(MIX_INIT_OGG | MIX_INIT_MID) != (MIX_INIT_OGG | MIX_INIT_MID))
	{
		LogError("Mix_Init: %s", Mix_GetError());
	}

#ifndef __EMSCRIPTEN__
	int frequency, channels;
	uint16_t format;
	Mix_QuerySpec(&frequency, &format, &channels);
	LogDebug("audio format: freq=%d, channels=%d, format=0x%x\n", frequency, channels, format);
#endif

	NUM_SOUNDS = configNumSounds;  // Only copied over here to avoid problems if SetJamulSoundEnabled is called after init.
	Mix_AllocateChannels(NUM_SOUNDS + 1);

	bufferCount=numBuffers;
	soundList.clear();
	soundList.reserve(bufferCount);
	schannel.resize(NUM_SOUNDS+1);
	for(i=0;i<NUM_SOUNDS;i++)
	{
		schannel[i].priority=INT_MIN;
		schannel[i].soundNum=-1;
		schannel[i].voice=-1;
	}
	sndVolume=128;
	return soundIsOn = true;
}

void JamulSoundExit(void)
{
	if(soundIsOn)
	{
		soundIsOn = false;
		JamulSoundPurge();
		StopSong();
		schannel.clear();
		soundList.clear();
		Mix_CloseAudio();
	}
}

bool SoundIsAvailable()
{
	return soundIsOn;
}

void JamulSoundUpdate(void)
{
	int i;

	if(!soundIsOn)
		return;

	for(i=0;i<NUM_SOUNDS;i++)
	{
		if(schannel[i].soundNum>-1)
		{
			if(!Mix_Playing(schannel[i].voice))
			{
				schannel[i].soundNum=-1;
				schannel[i].priority=INT_MIN;
				schannel[i].voice=-1;
			}
		}
	}
}

bool JamulSoundPlay(int which,long pan,long vol,int playFlags,int priority)
{
	char s[32];
	int i,chosen,lowpriority;

	if(!soundIsOn)
		return false;

	JamulSoundUpdate();

	vol=((vol + 255) * sndVolume) / 255;
	pan+=128;

	if (vol < 0)
		return false;

	size_t wantSize = which + 1;
	if (soundList.size() < wantSize)
	{
		soundList.resize(wantSize);
	}

	if(soundList[which].sample==NULL)
	{
		// See if sound loading is overridden for this sound...
		owned::SDL_RWops rw = g_HamExtern.SoundLoadOverride ? g_HamExtern.SoundLoadOverride(which) : nullptr;
		if (!rw)
		{
			// If not, try to load it from a file instead
			sprintf(s,"sound/snd%03d.wav",which);
			rw = AssetOpen_SDL_Owned(s);
			if (!rw)
			{
				return false;
			}
		}

		// Now try to load it
		soundList[which].sample = owned::Mix_LoadWAV_RW(std::move(rw));
		if(soundList[which].sample==NULL)
		{
			LogError("LoadWAV(%d): %s", which, Mix_GetError());
			return false;
		}
	}

	if(playFlags&SND_ONE)
	{
		for(i=0;i<NUM_SOUNDS;i++)
			if(schannel[i].soundNum==which)
			{
				// JamulSoundUpdate above ensures that sounds that finish are
				// erased from schannel.
				if (playFlags&SND_CUTOFF)
				{
					Mix_HaltChannel(schannel[i].voice);
					schannel[i].soundNum=-1;
					schannel[i].priority=INT_MIN;
					schannel[i].voice=-1;
				}
				else
					return false;
			}
	}

	if(playFlags & SND_MAXPRIORITY)
		priority = MAX_SNDPRIORITY;

	chosen = -1;
	lowpriority = priority;
	// see if there are any spots of lower priority,
	for (i = 0; i < NUM_SOUNDS; i++)
	{
		if (schannel[i].soundNum == -1)
		{
			chosen = i;
			break; // can't beat that
		}
		else if (schannel[i].priority <= lowpriority || (chosen == -1 && (playFlags & SND_CUTOFF) && schannel[i].soundNum == which))
		{
			chosen = i;
			lowpriority = schannel[i].priority;
		}
	}

	if(chosen==-1)	// no sounds of lower priority to kick out, give up
		return false;

	// if you're replacing a sound, stop it first
	if(schannel[chosen].soundNum!=-1)
		Mix_HaltChannel(schannel[chosen].voice);

	Mix_Chunk* playing = soundList[which].sample.get();
#ifndef __EMSCRIPTEN__
	if (playFlags & SND_RANDOM)
	{
		schannel[chosen].sample = FxRandomPitch(playing);
		playing = schannel[chosen].sample.get();
	}
	if (playFlags & SND_BACKWARDS)
	{
		schannel[chosen].sample = FxBackwards(playing);
		playing = schannel[chosen].sample.get();
	}
	if (playFlags & SND_DOUBLESPEED)
	{
		schannel[chosen].sample = FxDoubleSpeed(playing);
		playing = schannel[chosen].sample.get();
	}
#endif

	i=Mix_PlayChannel(-1, playing, (playFlags & SND_LOOPING) ? -1 : 0);
	if (i == -1)
		return false;

	Mix_Volume(i, vol / 2);
	Mix_SetPanning(i, 255 - pan, pan);

	schannel[chosen].soundNum=which;
	schannel[chosen].priority=priority;
	schannel[chosen].voice=i;

	return true;
}

bool JamulSoundStop(int which)
{
	int i;

	if(!soundIsOn)
		return true;

	for(i=0;i<NUM_SOUNDS;i++)
	{
		if(schannel[i].soundNum==which)
		{
			Mix_HaltChannel(schannel[i].voice);
			schannel[i].soundNum=-1;
			schannel[i].priority=INT_MIN;
			schannel[i].voice=-1;
		}
	}

	return true;
}

// now here is all the big sound manager stuff, that allows multiple sounds at once
void JamulSoundPurge(void)
{
	if (!soundIsOn)
		return;

	for (int i = 0; i < NUM_SOUNDS; i++)
	{
		if (schannel[i].soundNum != -1)
			Mix_HaltChannel(schannel[i].voice);
		schannel[i].soundNum = -1;
		schannel[i].priority = INT_MIN;
		schannel[i].voice = -1;
		schannel[i].sample.reset();
	}

	for (size_t i = 0; i < soundList.size(); i++)
	{
		if (soundList[i].sample)
		{
			soundList[i].sample.reset();
		}
	}
}

void GoPlaySound(int num,long pan,long vol,int flags,int priority)
{
	if(!soundIsOn)
		return;
	JamulSoundPlay(num,pan,vol,flags,priority);
}

void JamulSoundVolume(int v)
{
	sndVolume=v;
}

#include "jamulsound.h"
#include "hammusic.h"
#include "log.h"
#include "audiofx.h"
#include "appdata.h"
#include <stdio.h>
#include <memory>

#include <SDL.h>
#include <SDL_mixer.h>

// Game-provided
extern bool ConfigSoundEnabled();
extern int ConfigNumSounds();
extern SDL_RWops* SoundLoadOverride(int num);

struct ChunkDeleter {
	inline void operator()(Mix_Chunk* ptr) { return Mix_FreeChunk(ptr); }
};

typedef std::unique_ptr<Mix_Chunk, ChunkDeleter> OwnedChunk;

typedef struct soundList_t
{
	OwnedChunk sample;
} soundList_t;

typedef struct schannel_t
{
	int soundNum;
	int priority;
	int voice;
	OwnedChunk sample;
} schannel_t;

static int sndVolume;

static int NUM_SOUNDS = 0;

static byte soundIsOn=0;
static int bufferCount;
static soundList_t *soundList;
static schannel_t *schannel;

bool JamulSoundInit(int numBuffers)
{
	int i;

	if (!ConfigSoundEnabled()) {
		LogDebug("sound disabled in config");
		return false;
	}
	if (SDL_Init(SDL_INIT_AUDIO) != 0) {
		LogError("SDL_Init(AUDIO): %s", SDL_GetError());
		return false;
	}
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024) != 0) {
		LogError("Mix_OpenAudio: %s", Mix_GetError());
		return false;
	}
	if (Mix_Init(MIX_INIT_OGG) != MIX_INIT_OGG) {
		LogError("Mix_Init: %s", Mix_GetError());
	}

#ifndef __EMSCRIPTEN__
	int frequency, channels;
	uint16_t format;
	Mix_QuerySpec(&frequency, &format, &channels);
	printf("audio format: freq=%d, channels=%d, format=0x%x\n", frequency, channels, format);
#endif

	NUM_SOUNDS = ConfigNumSounds();
	Mix_AllocateChannels(NUM_SOUNDS + 1);

	soundIsOn=1;
	bufferCount=numBuffers;
	soundList = new soundList_t[bufferCount];
	schannel = new schannel_t[NUM_SOUNDS+1];
	for(i=0;i<bufferCount;i++)
		soundList[i].sample=NULL;
	for(i=0;i<NUM_SOUNDS;i++)
	{
		schannel[i].priority=0;
		schannel[i].soundNum=-1;
		schannel[i].voice=-1;
	}
	sndVolume=128;
	return true;
}

void JamulSoundExit(void)
{
	if(soundIsOn)
	{
		soundIsOn = false;
		JamulSoundPurge();
		StopSong();
		Mix_CloseAudio();
		delete[] schannel;
		delete[] soundList;
	}
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
				schannel[i].priority=0;
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
		return 0;

	JamulSoundUpdate();

	vol=vol+sndVolume;
	pan+=128;

	if(soundList[which].sample==NULL)
	{
		// See if sound loading is overridden for this sound...
		SDL_RWops* rw = SoundLoadOverride(which);
		if (!rw)
		{
			// If not, try to load it from a file instead
			sprintf(s,"sound/snd%03d.wav",which);
			rw = AssetOpen_SDL(s, "rb");
			if(!rw) {
				return 0;
			}
		}

		// Now try to load it
		soundList[which].sample.reset(Mix_LoadWAV_RW(rw, 1));
		if(soundList[which].sample==NULL) {
			LogError("LoadWAV(%d): %s", which, Mix_GetError());
			return 0;
		}
	}

	if(playFlags&SND_ONE)
	{
		for(i=0;i<NUM_SOUNDS;i++)
			if(schannel[i].soundNum==which)
			{
				// TODO: only cut this sound off if it is not playing or SND_CUTOFF is set
				Mix_HaltChannel(schannel[i].voice);
				schannel[i].soundNum=-1;
				schannel[i].priority=0;
				schannel[i].voice=-1;
			}
	}

	if(playFlags & SND_MAXPRIORITY)
		priority = MAX_SNDPRIORITY;

	chosen=-1;
	lowpriority=priority;
	// see if there are any spots of lower priority,
	for(i=0;i<NUM_SOUNDS;i++)
	{
		if(schannel[i].priority<=lowpriority || ((playFlags & SND_CUTOFF) && schannel[i].soundNum == which))
		{
			chosen=i;
			lowpriority=schannel[i].priority;
		}
	}

	if(chosen==-1)	// no sounds of lower priority to kick out, give up
		return 0;

	// if you're replacing a sound, stop it first
	if(schannel[chosen].soundNum!=-1)
		Mix_HaltChannel(schannel[chosen].voice);

	Mix_Chunk* playing = soundList[which].sample.get();
#ifndef __EMSCRIPTEN__
	if (playFlags & SND_RANDOM)
	{
		schannel[chosen].sample.reset(FxRandomPitch(playing));
		playing = schannel[chosen].sample.get();
	}
	if (playFlags & SND_BACKWARDS)
	{
		schannel[chosen].sample.reset(FxBackwards(playing));
		playing = schannel[chosen].sample.get();
	}
	if (playFlags & SND_DOUBLESPEED)
	{
		schannel[chosen].sample.reset(FxDoubleSpeed(playing));
		playing = schannel[chosen].sample.get();
	}
#endif

	i=Mix_PlayChannel(-1, playing, (playFlags & SND_LOOPING) ? -1 : 0);
	if(i!=-1)
	{
		Mix_Volume(i, vol / 2);
		Mix_SetPanning(i, 255 - pan, pan);

		schannel[chosen].soundNum=which;
		schannel[chosen].priority=priority;
		schannel[chosen].voice=i;
	}
	else
		return false;

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
			schannel[i].priority=0;
			schannel[i].voice=-1;
		}
	}

	return true;
}

// now here is all the big sound manager stuff, that allows multiple sounds at once
void JamulSoundPurge(void)
{
	int i;

	if(!soundIsOn)
		return;

	for(i=0;i<NUM_SOUNDS;i++)
	{
		if(schannel[i].soundNum!=-1)
			Mix_HaltChannel(schannel[i].voice);
		schannel[i].soundNum=-1;
		schannel[i].priority=0;
		schannel[i].voice=-1;
		schannel[i].sample.reset();
	}
	for(i=0;i<bufferCount;i++)
	{
		if(soundList[i].sample)
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

#include "jamulsound.h"
#include "mgldraw.h"
#include "sound.h"
#include "options.h"
#include <SDL2/SDL_mixer.h>

typedef struct soundList_t
{
	Mix_Chunk *sample;
} soundList_t;

typedef struct schannel_t
{
	int soundNum;
	int priority;
	int voice;
} schannel_t;

static int sndVolume;

const int NUM_SOUNDS = 32;

static byte soundIsOn=0;
static int bufferCount;
static soundList_t *soundList;
static schannel_t *schannel;

bool JamulSoundInit(int numBuffers)
{
	int i;

	if (!opt.soundVol)
		return false;
	if (SDL_Init(SDL_INIT_AUDIO) != 0) {
		printf("init audio failed: %s\n", SDL_GetError());
		return false;
	}
	Mix_Init(MIX_INIT_OGG);
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024) != 0) {
		printf("Mix_OpenAudio: %s\n", Mix_GetError());
		return false;
	}
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

bool JamulSoundPlay(int which,long pan,long vol,byte playFlags,int priority)
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
		sprintf(s,"sound/snd%03d.wav",which);
		SDL_RWops* rw = SDL_RWFromFile(s, "rb");
		if(!rw)
			return 0;
		soundList[which].sample = Mix_LoadWAV_RW(rw, 1);
		if(soundList[which].sample==NULL)
			return 0;
	}

	if(playFlags&SND_ONE)
	{
		for(i=0;i<NUM_SOUNDS;i++)
			if(schannel[i].soundNum==which)
			{
				Mix_HaltChannel(schannel[i].voice);
				schannel[i].soundNum=-1;
				schannel[i].priority=0;
				schannel[i].voice=-1;
			}
	}

	chosen=-1;
	lowpriority=priority;
	// see if there are any spots of lower priority,
	for(i=0;i<NUM_SOUNDS;i++)
	{
		if(schannel[i].priority<=lowpriority)
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

	i=Mix_PlayChannel(-1, soundList[which].sample, 0);
	if(i!=-1)
	{
		Mix_Volume(i, vol / 2);
		Mix_SetPanning(i, 255 - pan, pan);

		schannel[chosen].soundNum=which;
		schannel[chosen].priority=priority;
		schannel[chosen].voice=i;
#if 0  // TODO
		if(playFlags&SND_RANDOM)
		{
			voice_stop(i);
			int freq=voice_get_frequency(i);
			freq=freq-(freq/5)+(rand()%((freq/5)*2+1));
			voice_set_frequency(i, freq);
			voice_start(i);
		}
		if(profile.progress.purchase[modeShopNum[MODE_REVERSE]]&SIF_ACTIVE)
		{
			voice_stop(i);
			voice_set_position(i, soundList[which].sample->len - 1);
			voice_set_frequency(i, -voice_get_frequency(i));
			voice_start(i);
		}
		if(profile.progress.purchase[modeShopNum[MODE_MANIC]]&SIF_ACTIVE)
		{
			voice_stop(i);
			voice_set_frequency(i, 2 * voice_get_frequency(i));
			voice_start(i);
		}
#endif
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

	soundIsOn = 0;

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
	}
	for(i=0;i<bufferCount;i++)
	{
		if(soundList[i].sample)
		{
			Mix_FreeChunk(soundList[i].sample);
			soundList[i].sample=NULL;
		}
	}
}

void GoPlaySound(int num,long pan,long vol,byte flags,int priority)
{
	if(!soundIsOn)
		return;
	JamulSoundPlay(num,pan,vol,flags,priority);
}

void JamulSoundVolume(int v)
{
	sndVolume=v;
}

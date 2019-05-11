#include "jamulsound.h"
#include <mmsystem.h>
#include <dsound.h>
#include "mgldraw.h"
#include "sound.h"
#include <fmod.h>
#include "music.h"

typedef struct soundList_t
{
	FSOUND_SAMPLE *sample;
} soundList_t;

typedef struct schannel_t
{
	int soundNum;
	int priority;
} schannel_t;

static int sndVolume;

static byte soundIsOn=0;
static int bufferCount;
static soundList_t *soundList;
static schannel_t *schannel;

bool JamulSoundInit(int numBuffers)
{
	int i;

	if(FSOUND_Init(44100,config.numSounds+1,0))
	{
		soundIsOn=1;
		bufferCount=numBuffers;
		soundList=(soundList_t *)malloc(sizeof(soundList_t)*bufferCount);
		schannel=(schannel_t *)malloc(sizeof(schannel_t)*(config.numSounds+1));
		for(i=0;i<bufferCount;i++)
			soundList[i].sample=NULL;
		for(i=0;i<config.numSounds;i++)
		{
			schannel[i].priority=0;
			schannel[i].soundNum=-1;
		}
		sndVolume=255;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void JamulSoundExit(void)
{
	if(soundIsOn)
	{
		free(schannel);
		JamulSoundPurge();
		StopSong();
		FSOUND_Close();
		free(soundList);
	}
}

void JamulSoundUpdate(void)
{
	int i;

	if(!soundIsOn)
		return;

	for(i=0;i<config.numSounds;i++)
	{
		if(schannel[i].soundNum>-1)
		{
			if(!FSOUND_IsPlaying(i))
			{
				schannel[i].soundNum=-1;
				schannel[i].priority=0;
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
	if(vol<0)
		return 1;	// inaudible!

	pan+=128;

	if(soundList[which].sample==NULL)
	{
		sprintf(s,"sound\\snd%03d.wav",which);
		soundList[which].sample=FSOUND_Sample_Load(FSOUND_UNMANAGED,s,FSOUND_NORMAL,0);
		if(soundList[which].sample==NULL)
			return 0;
	}

	if(playFlags&SND_ONE)
	{
		for(i=0;i<config.numSounds;i++)
			if(schannel[i].soundNum==which)
			{
				FSOUND_StopSound(i);
				schannel[i].soundNum=-1;
				schannel[i].priority=0;
			}
	}

	chosen=-1;
	lowpriority=priority;
	// see if there are any spots of lower priority,
	for(i=0;i<config.numSounds;i++)
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
		FSOUND_StopSound(chosen);

	i=FSOUND_PlaySound(chosen,soundList[which].sample);
	if(i!=-1)
	{
		schannel[chosen].soundNum=which;
		schannel[chosen].priority=priority;
		FSOUND_SetVolume(chosen,vol);
		FSOUND_SetPan(chosen,pan);
		if(playFlags&SND_RANDOM)
		{
			int freq;
			FSOUND_SetPaused(chosen,TRUE);
			freq=FSOUND_GetFrequency(chosen);
			freq=freq-(freq/5)+(rand()%((freq/5)*2+1));
			FSOUND_SetFrequency(chosen,freq);
			FSOUND_SetPaused(chosen,FALSE);
		}
	}
	else
		return FALSE;

	return TRUE;
}

bool JamulSoundStop(int which)
{
	int i;

	if(!soundIsOn)
		return TRUE;

	for(i=0;i<config.numSounds;i++)
	{
		if(schannel[i].soundNum==which)
		{
			FSOUND_StopSound(i);
			schannel[i].soundNum=-1;
			schannel[i].priority=0;
		}
	}

	return TRUE;
}

// now here is all the big sound manager stuff, that allows multiple sounds at once
void JamulSoundPurge(void)
{
	int i;

	if(!soundIsOn)
		return;

	for(i=0;i<config.numSounds;i++)
	{
		if(schannel[i].soundNum!=-1)
			FSOUND_StopSound(i);
		schannel[i].soundNum=-1;
		schannel[i].priority=0;
	}
	for(i=0;i<bufferCount;i++)
	{
		if(soundList[i].sample)
		{
			FSOUND_Sample_Free(soundList[i].sample);
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

void JamulSoundMusicVolume(int v)
{
	SetMusicVolume(v);

}

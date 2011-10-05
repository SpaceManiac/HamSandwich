#include "jamulsound.h"
#include <mmsystem.h>
#include <dsound.h>
#include "mgldraw.h"
#include "sound.h"
#include <fmod.h>
#include "music.h"
#include "progress.h"
#include "shop.h"
#include "config.h"

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

	if(config.sound && FSOUND_Init(44100,config.numSounds+1,0))
	{
		soundIsOn=1;
		bufferCount=numBuffers;
		soundList=(soundList_t *)malloc(sizeof(soundList_t)*bufferCount);
		schannel=(schannel_t *)malloc(sizeof(schannel_t)*config.numSounds+1);
		for(i=0;i<bufferCount;i++)
			soundList[i].sample=NULL;
		for(i=0;i<config.numSounds;i++)
		{
			schannel[i].priority=0;
			schannel[i].soundNum=-1;
		}
		sndVolume=0;
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
		if(profile.progress.purchase[modeShopNum[MODE_REVERSE]]&SIF_ACTIVE)
		{
			FSOUND_SetPaused(chosen,TRUE);
			FSOUND_SetCurrentPosition(chosen,FSOUND_Sample_GetLength(soundList[which].sample)-1);
			FSOUND_SetFrequency(chosen,-FSOUND_GetFrequency(chosen));
			FSOUND_SetPaused(chosen,FALSE);
		}
		if(profile.progress.purchase[modeShopNum[MODE_MANIC]]&SIF_ACTIVE)
		{
			FSOUND_SetPaused(chosen,TRUE);
			FSOUND_SetFrequency(chosen,FSOUND_GetFrequency(chosen)*2);
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

	if(num>CUSTOM_SND_START+GetNumCustomSounds())
		return;

	if(num>=CUSTOM_SND_START)
	{
		if(GetCustomSound(num-CUSTOM_SND_START)==NULL)
			return;

		if(soundList[num].sample==NULL)
		{
			soundList[num].sample=FSOUND_Sample_Load(FSOUND_UNMANAGED,
				(char *)GetCustomSound(num-CUSTOM_SND_START),
				FSOUND_LOADMEMORY,GetCustomLength(num-CUSTOM_SND_START));
			if(soundList[num].sample==NULL)
				return;
		}
	}
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

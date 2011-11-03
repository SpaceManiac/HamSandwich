#include "jamulsound.h"
#include <mmsystem.h>
#include "mgldraw.h"
#include "sound.h"
#include <allegro.h>
#include "music.h"
#include "progress.h"
#include "shop.h"
#include "config.h"
#include "mempackf.h"

typedef struct soundList_t
{
	SAMPLE *sample;
} soundList_t;

typedef struct schannel_t
{
	int soundNum;
	int priority;
	int voice;
} schannel_t;

static int sndVolume;

static byte soundIsOn=0;
static int bufferCount;
static soundList_t *soundList;
static schannel_t *schannel;

bool JamulSoundInit(int numBuffers)
{
	int i;

	if(config.sound && (reserve_voices(config.numSounds+1, 0), (install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, "allegro.cfg") == 0)))
	{
		soundIsOn=1;
		bufferCount=numBuffers;
		soundList = new soundList_t[bufferCount];
		schannel = new schannel_t[config.numSounds+1];
		for(i=0;i<bufferCount;i++)
			soundList[i].sample=NULL;
		for(i=0;i<config.numSounds;i++)
		{
			schannel[i].priority=0;
			schannel[i].soundNum=-1;
			schannel[i].voice=-1;
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
		delete[] schannel;
		JamulSoundPurge();
		StopSong();
		remove_sound();
		delete[] soundList;
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
			if(!voice_check(schannel[i].voice))
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
		sprintf(s,"sound\\snd%03d.wav",which);
		soundList[which].sample=load_sample(s);
		if(soundList[which].sample==NULL)
			return 0;
	}

	if(playFlags&SND_ONE)
	{
		for(i=0;i<config.numSounds;i++)
			if(schannel[i].soundNum==which)
			{
				deallocate_voice(schannel[i].voice);
				schannel[i].soundNum=-1;
				schannel[i].priority=0;
				schannel[i].voice=-1;
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
		deallocate_voice(chosen);

	i=play_sample(soundList[which].sample, vol, pan, 1000, 0);
	if(i!=-1)
	{
		schannel[chosen].soundNum=which;
		schannel[chosen].priority=priority;
		schannel[chosen].voice=i;
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
			deallocate_voice(schannel[i].voice);
			schannel[i].soundNum=-1;
			schannel[i].priority=0;
			schannel[i].voice=-1;
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
			deallocate_voice(schannel[i].voice);
		schannel[i].soundNum=-1;
		schannel[i].priority=0;
		schannel[i].voice=-1;
	}
	for(i=0;i<bufferCount;i++)
	{
		if(soundList[i].sample)
		{
			destroy_sample(soundList[i].sample);
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
			MemoryPackfile mem(GetCustomSound(num-CUSTOM_SND_START), GetCustomLength(num-CUSTOM_SND_START));
			PACKFILE* pf = mem.open();
			soundList[num].sample = load_wav_pf(pf);
			pack_fclose(pf);
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

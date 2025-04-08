#include "sound.h"
#include "display.h"
#include "player.h"
#include "hammusic.h"
#include "options.h"

static int curSong;
static byte soundsThisFrame[MAX_SNDS_PER_FRAME];
static short soundVolThisFrame[MAX_SNDS_PER_FRAME];
void InitSound(void)
{
	JamulSoundPurge();
	curSong=0;
	memset(soundsThisFrame, 255, sizeof(byte) * MAX_SNDS_PER_FRAME);
}

void ExitSound(void)
{
	// don't need to do nothing, it's handled by jamulsoundexit
}

bool SoundAlreadyPlayed(byte num,int vol)
{
	for (int i = 0; i < MAX_SNDS_PER_FRAME; i++)
		if (soundsThisFrame[i] == num)
		{
			if (vol > soundVolThisFrame[i])	// we need to turn it up to the new volume
			{
				soundVolThisFrame[i] = vol;
				JamulSoundChangeSoundVolume(num, vol * 255 / 5000);
			}
			return true;
		}

	return false;
}

void MarkSoundPlayed(byte num,int vol)
{
	for (int i = 0; i < MAX_SNDS_PER_FRAME; i++)
		if (soundsThisFrame[i] == 255)
		{
			soundsThisFrame[i] = num;
			soundVolThisFrame[i] = (short)vol;
			return;
		}
}

void UpdateSounds(void)
{
	JamulSoundUpdate();
	for (int i = 0; i < MAX_SNDS_PER_FRAME; i++)
		soundsThisFrame[i] = 255;
}

void MakeSound(int snd,int x,int y,int flags,int priority)
{
	long pan,vol;
	int cx,cy;

	if(!SoundIsAvailable())
		return;
	GetCamera(&cx, &cy);
	x >>= FIXSHIFT;
	y >>= FIXSHIFT;
	pan = (x - cx) * 2;

	vol = -((x - cx) * (x - cx) + (y - cy) * (y - cy)) / 128;
	if (vol < -5000)
		return;	// too quiet to play
	if (SoundAlreadyPlayed((byte)snd,vol))
		return;
		
	GoPlaySound(snd,pan * 127 / SCRWID,vol * 255 / 5000,(byte)flags,priority);
	MarkSoundPlayed(snd,vol);
}

void MakeNormalSound(int snd)
{
	if(!SoundIsAvailable())
		return;
	if (SoundAlreadyPlayed((byte)snd,0))
		return;

	GoPlaySound(snd,0,0,SND_MAXPRIORITY|SND_CUTOFF|SND_ONE,MAX_SNDPRIORITY);
	MarkSoundPlayed(snd,0);
}

void ReplaySong()
{
	if (!SoundIsAvailable() || opt.musicVol==0)
		return;

	char s[64];
	sprintf(s, "sound/snd%03d.wav", curSong);
	PlaySongFile(s);
}

void PlaySong(int sng)
{
	if(!SoundIsAvailable())
		return;

	if(sng == curSong)
		return;	// no need, it's already playing our song

	curSong = sng;
	ReplaySong();
}

void StopPlayingSong(void)
{
	StopSong();
	curSong = 0;
}

void VolumeSong(byte hi)
{
	if(!SoundIsAvailable())
		return;

	switch(hi)
	{
		case 0:
			StopSong();
			break;
		case 1:
			SetMusicVolume(16);
			break;
		case 2:
			SetMusicVolume(64);
			break;
		case 3:
			SetMusicVolume(128);
			break;
		case 4:
			SetMusicVolume(192);
			break;
		case 5:
			SetMusicVolume(255);
			break;
	}
}

void VolumeSound(byte hi)
{
	if(!SoundIsAvailable())
		return;

	switch(hi)
	{
		case 0:
			JamulSoundVolume(0);
			break;
		case 1:
			JamulSoundVolume(16);
			break;
		case 2:
			JamulSoundVolume(64);
			break;
		case 3:
			JamulSoundVolume(128);
			break;
		case 4:
			JamulSoundVolume(192);
			break;
		case 5:
			JamulSoundVolume(255);
			break;
	}
}

int CurrentSong(void)
{
	return curSong;
}

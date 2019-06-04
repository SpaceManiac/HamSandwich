#include "sound.h"
#include "display.h"
#include "player.h"
#include "music.h"

int curSong,prvSong;
static int songVol,soundVol;
int soundExists=0;

void InitSound(void)
{
	JamulSoundPurge();
	curSong=0;
	prvSong=0;
	songVol=-700;
	soundVol=0;
}

void ExitSound(void)
{
	// don't need to do nothing, it's handled by jamulsoundexit
}

void MakeSound(int snd,int x,int y,int flags,int priority)
{
	long pan,vol;
	int cx,cy;

	if(!soundExists)
		return;

	GetCamera(&cx,&cy);
	x>>=FIXSHIFT;
	y>>=FIXSHIFT;
	pan=(x-cx)*2;
	vol=-((x-cx)*(x-cx)+(y-cy)*(y-cy))/128;
	if(vol<-5000)
		return;	// too quiet to play
	GoPlaySound(snd,pan,vol,(byte)flags,priority);
}

void MakeNormalSound(int snd)
{
	if(!soundExists)
		return;

	GoPlaySound(snd,0,0,SND_MAXPRIORITY|SND_CUTOFF|SND_ONE,MAX_SNDPRIORITY);
}

void PlaySong(int sng)
{
	if(!soundExists)
		return;

	prvSong=curSong;
	curSong=sng;
	if(PlayerGetMusicSettings()>0)
	{
		Song(sng);
	}
	else
		prvSong=curSong;
}

void StopSong(void)
{
	if(!soundExists)
		return;

	StopSong2();
	prvSong=curSong;
	curSong=0;
}

void ReplaySong(void)
{
	if(!soundExists)
		return;

	if(PlayerGetMusicSettings()>0)
	{
		curSong=prvSong;
		Song(curSong);
	}
}

void VolumeSong(byte hi)
{
	if(!soundExists)
		return;

	switch(hi)
	{
		case 0:
			StopSong();
			break;
		case 1:
			songVol=85;
			SetMusicVolume(85);
			break;
		case 2:
			songVol=170;
			SetMusicVolume(170);
			break;
		case 3:
			songVol=255;
			SetMusicVolume(255);
			break;
	}
}

void VolumeSound(byte hi)
{
	if(!soundExists)
		return;

	switch(hi)
	{
		case 0:
			soundVol=0;
			JamulSoundVolume(0);
			break;
		case 1:
			soundVol=85;
			JamulSoundVolume(85);
			break;
		case 2:
			soundVol=170;
			JamulSoundVolume(170);
			break;
		case 3:
			soundVol=255;
			JamulSoundVolume(255);
			break;
	}
}

int CurrentSong(void)
{
	return curSong;
}

void SoundSystemExists(void)
{
	soundExists=1;
}

bool ConfigSoundEnabled()
{
	return 1;
}

int ConfigNumSounds()
{
	return 32;
}

void KillSong()
{
	StopSong();
}

SDL_RWops* SoundLoadOverride(int which)
{
	return nullptr;
}

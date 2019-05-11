#include "sound.h"
#include "display.h"
#include "player.h"

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
		JamulSoundSong(sng,songVol);
	}
	else
		prvSong=curSong;
}

void StopSong(void)
{
	if(!soundExists)
		return;

	JamulSoundStopSong();
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
		JamulSoundSong(curSong,songVol);
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
			songVol=-1400;
			SetSongVolume(-1400);
			break;
		case 2:
			songVol=-700;
			SetSongVolume(-700);
			break;
		case 3:
			songVol=0;
			SetSongVolume(0);
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
			soundVol=-5000;
			SetSoundVolume(-5000);
			break;
		case 1:
			soundVol=-1400;
			SetSoundVolume(-1400);
			break;
		case 2:
			soundVol=-700;
			SetSoundVolume(-700);
			break;
		case 3:
			soundVol=0;
			SetSoundVolume(0);
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

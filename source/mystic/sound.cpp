#include "sound.h"
#include "display.h"
#include "player.h"
#include "hammusic.h"

static int curSong;

void InitSound(void)
{
	JamulSoundPurge();
	curSong=0;
}

void ExitSound(void)
{
	// don't need to do nothing, it's handled by jamulsoundexit
}

void MakeSound(int snd,int x,int y,int flags,int priority)
{
	long pan,vol;
	int cx,cy;

	if(!SoundIsAvailable())
		return;

	GetCamera(&cx,&cy);
	x>>=FIXSHIFT;
	y>>=FIXSHIFT;
	pan=(x-cx)*2;
	vol=-((x-cx)*(x-cx)+(y-cy)*(y-cy))/128;
	if(vol<-5000)
		return;	// too quiet to play
	GoPlaySound(snd,pan * 127 / 640,vol * 255 / 5000,(byte)flags,priority);
}

void MakeNormalSound(int snd)
{
	if(!SoundIsAvailable())
		return;

	GoPlaySound(snd,0,0,SND_MAXPRIORITY|SND_CUTOFF|SND_ONE,MAX_SNDPRIORITY);
}

void ReplaySong()
{
	if (!PlayerGetMusicSettings())
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
			SetMusicVolume(85);
			break;
		case 2:
			SetMusicVolume(170);
			break;
		case 3:
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
			JamulSoundVolume(85);
			break;
		case 2:
			JamulSoundVolume(170);
			break;
		case 3:
			JamulSoundVolume(255);
			break;
	}
}

int CurrentSong(void)
{
	return curSong;
}

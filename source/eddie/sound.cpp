#include "sound.h"
#include <stdio.h>
#include "options.h"
#include "hammusic.h"

byte curSong;

void JamulSoundSong(int sng, int x)
{
	char buf[64];
	sprintf(buf, "sound/snd%03d.wav", sng);
	PlaySongFile(buf);
	SetMusicVolume(x == -700 ? 128 : 255);
}

void MakeSound(int snd,int priority)
{
	if(SoundIsOn())
		GoPlaySound(snd,0,0,SND_CUTOFF,priority);
}

void PlaySong(int sng)
{
	curSong=sng;
	if(MusicIsOn())
	{
		if(SoundIsOn())
			JamulSoundSong(sng,-700);
		else
			JamulSoundSong(sng,0);
	}
}

void ReplaySong(void)
{
	if(MusicIsOn())
	{
		if(SoundIsOn())
			JamulSoundSong(curSong,-700);
		else	// when sound is off, music goes full blast
			JamulSoundSong(curSong,0);
	}
}

void VolumeSong(byte hi)
{
	if(hi)
		SetMusicVolume(255);
	else
		SetMusicVolume(128);
}

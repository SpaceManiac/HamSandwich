#include "sound.h"
#include "options.h"

byte curSong;

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

void StopSong(void)
{
	JamulSoundStopSong();
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
		SetSongVolume(0);
	else
		SetSongVolume(-700);
}

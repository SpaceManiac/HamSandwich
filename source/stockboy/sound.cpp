#include "sound.h"
#include "display.h"
#include "options.h"
#include "hammusic.h"

void InitSound(void)
{
	JamulSoundPurge();
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
	if(!opt.sound)
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
	if(!SoundIsAvailable())
		return;

	if(!opt.sound)
		return;

	GoPlaySound(snd,0,0,SND_MAXPRIORITY|SND_CUTOFF|SND_ONE,MAX_SNDPRIORITY);
}

void MakeNormalSound2(int snd)
{
	if(!SoundIsAvailable())
		return;

	if(!opt.sound)
		return;

	GoPlaySound(snd,0,0,SND_MAXPRIORITY|SND_CUTOFF,MAX_SNDPRIORITY);
}

void SetVolumes(int sndvol,int musvol)
{
	JamulSoundVolume(sndvol);
	SetMusicVolume(musvol);
}

void PlayInstrument(int ins,int note,long vol,int flags,byte seq)
{
	if (flags & SND_SUSTAIN)
		flags = flags & ~SND_ONE & ~SND_CUTOFF;
	flags |= SND_MUSICVOLUME;
	GoPlaySound(200 + ins*15 + note, 0, vol, flags, 0);
#if 0
	char txt[32];
	int i,best;
	byte playFlags;
	int insBase;
	int num;

	if(musVolume==MIN_VOL)
		return;

	insBase=ins*15+200;
	num=200+ins*15+note;

	vol+=musVolume;
	if(vol>0)
		vol=0;

	if(!(flags&SND_LOOPING))
		playFlags=0;
	else
		playFlags=SOUND_LOOP;

	if(soundHandle[num]==-1)
	{
		sprintf(txt,"sound/snd%03d.wav",num);
		soundHandle[num]=JamulSoundLoad(txt);
		if(soundHandle[num]==-1)
			return;	// can't play the sound, it won't load for some reason
	}

	if(!(flags&SND_SUSTAIN))
	{
		// if it's not to be sustained, then cut off any existing ones of this instrument
		for(i=0;i<MAX_SOUNDS_AT_ONCE;i++)
			if(playBuffer[i].seqNum==seq && playBuffer[i].dsHandle!=-1)
				JamulSoundStop(playBuffer[i].dsHandle);
	}

	best=-1;
	for(i=0;i<MAX_SOUNDS_AT_ONCE;i++)
	{
		if(playBuffer[i].soundNum==num && (!(playBuffer[i].flags&SND_PLAYING)))
		{
			best=i;
			break;	// can't beat that
		}
		if(playBuffer[i].soundNum==-1 || (!(playBuffer[i].flags&SND_PLAYING)))
		{
			best=i;
		}
		if(playBuffer[i].priority<MAX_SNDPRIORITY)
		{
			if(best==-1 || playBuffer[i].priority<playBuffer[best].priority)
				best=i;
		}
	}
	if(best==-1)
		return;	// sound is not worthy to be played

	if(playBuffer[best].soundNum!=num)	// if it was already playing that sound, don't waste time
	{
		playBuffer[best].soundNum=num;
		if(playBuffer[best].dsHandle!=-1)
		{
			JamulSoundDestroyBuffer(playBuffer[best].dsHandle);	// slash & burn
		}
		playBuffer[best].dsHandle=JamulSoundCopy(soundHandle[num]);
	}
	else
	{
		JamulSoundRewind(playBuffer[best].dsHandle);
	}

	if(playBuffer[best].dsHandle==-1)
		return;	// can't play it
	playBuffer[best].priority=MAX_SNDPRIORITY;
	playBuffer[best].pan=0;
	playBuffer[best].vol=vol;
	playBuffer[best].seqNum=seq;
	playBuffer[best].flags=flags|SND_PLAYING;

	JamulSoundPlay(playBuffer[best].dsHandle,playBuffer[best].pan,playBuffer[best].vol,playFlags);
	soundNumber=playBuffer[best].dsHandle;
#endif
}

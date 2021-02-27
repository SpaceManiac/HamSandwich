#include "sound.h"
#include "display.h"
#include "options.h"

const double PAN_MULTIPLIER = 0.1984375;

int sndNum;

void InitSound(void)
{
	JamulSoundPurge();
	sndNum=-1;
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

	if(opt.cheats[CH_VINTAGE])
		return;

	GetCamera(&cx,&cy);
	x>>=FIXSHIFT;
	y>>=FIXSHIFT;
	pan=(x-cx)*2;
	vol=-((x-cx)*(x-cx)+(y-cy)*(y-cy))/128;
	if(vol<-5000)
		return;	// too quiet to play
	GoPlaySound(snd,pan * PAN_MULTIPLIER,vol,(byte)flags,priority);
}

void MakeNormalSound(int snd)
{
	if(!SoundIsAvailable())
		return;

	if(!opt.sound)
		return;

	if(opt.cheats[CH_VINTAGE] && snd!=SND_ENTERMAP && snd!=SND_FILM)
		return;

	GoPlaySound(snd,0,0,SND_MAXPRIORITY|SND_CUTOFF|SND_ONE,MAX_SNDPRIORITY);
}

void MakeNormalSound2(int snd)
{
	if(!SoundIsAvailable())
		return;

	if(!opt.sound)
		return;

	if(opt.cheats[CH_VINTAGE] && snd!=SND_ENTERMAP && snd!=SND_FILM)
		return;

	GoPlaySound(snd,0,0,SND_MAXPRIORITY|SND_CUTOFF,MAX_SNDPRIORITY);
}

void MakeSpaceSound(int snd,int priority)
{
	if(!SoundIsAvailable())
		return;

	if(!opt.sound)
		return;

	if(opt.cheats[CH_VINTAGE] && snd!=SND_ENTERMAP && snd!=SND_FILM)
		return;

	GoPlaySound(snd,0,0,SND_CUTOFF,priority);
}

void LoopingSound(int snd)
{
	if(!SoundIsAvailable())
		return;

	if(!opt.sound)
		return;

	if(opt.cheats[CH_VINTAGE] && snd!=SND_ENTERMAP && snd!=SND_FILM)
		return;

	GoPlaySound(snd,0,0,SND_MAXPRIORITY|SND_CUTOFF|SND_ONE|SND_LOOPING,MAX_SNDPRIORITY);
	sndNum=snd;
}

void KillSong(void)
{
	if(sndNum!=-1)
		JamulSoundStop(sndNum);
	sndNum=-1;
}

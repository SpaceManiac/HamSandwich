#ifndef JAMULSOUND_H
#define JAMULSOUND_H

#include "mgldraw.h"

// external fun sound playing flags for everyone to use
#define SND_CUTOFF		1	// cut off same sound if needed
#define SND_MAXPRIORITY 2	// ignore priority value, this sound is a must-have
#define SND_ONE			4	// only one copy may play at once
#define SND_PLAYING		8	// well, it's not for everyone, but it goes here
#define SND_LOOPING		16
#define SND_SUSTAIN		32	// only used by instruments
#define SND_RANDOM		64	// randomize pitch up or down 10%

#define MAX_SNDPRIORITY	65535

typedef struct sound_t
{
	int dsHandle;	// directsound handle
	int soundNum;	// which game sound number
	int priority;
	long pan,vol;
	byte seqNum;
	byte flags;
} sound_t;

extern int soundNumber;

bool JamulSoundInit(int numBuffers);
void JamulSoundExit(void);

// pass in the sound # you wish to play
bool JamulSoundPlay(int which,long pan,long vol,byte playFlags,int priority,int seq);
// pass in the sound # you're sick of hearing
bool JamulSoundStop(int which);
// if you're REALLY sick of hearing a certain sound
void JamulSoundDestroyBuffer(int which);

// call this to wipe the sounds from memory
void JamulSoundPurge(void);

// call this a lot, it plays sounds
void GoPlaySound(int num,long pan,long vol,byte flags,int priority);

void JamulSoundVolume(int v);
void JamulSoundMusicVolume(int v);
void JamulSoundUpdate(void);

#endif

#ifndef SOUND_H
#define SOUND_H

#include "jamulsound.h"

// a list of defines to make sound playing calls more comprehensible

#define SND_CLICK 0	// Interface click
#define SND_SHOOT 1	// regular gun firing
#define SND_POW   2	// small explosion
#define SND_MISSILE 3 // WHOOSH! Missile firing
#define SND_PLUNK 4 // weak little hit, mushroom getting hit
#define SND_DIE	  5	// player blowing up
#define SND_SHIELDHIT 6	// player getting hit while having a shield on
#define SND_TYPING 7	// typing sound for world intros
#define SND_BLIP   8	// end of line for typing sound
#define SND_SPIDER 9	// spider makes this noise continually
#define SND_CHAINGUN 10
#define SND_LASER	11
#define SND_NUKELAUNCH 12
#define SND_NUKEBOOM 13
#define SND_SHOTGUN	 14
#define SND_MISSION  15	// picking up a walkie-talkie
#define SND_WINMISSION 16 // accomplished a mission
#define SND_MISSIONGOOD 17	// did one thing towards finishing the mission
#define SND_LOSEMISSION 18
#define SND_MISSILEHIT  19	// a missile exploding
#define SND_FLYSPIT		20
#define SND_FLYARRIVE	21	// a fly buzz for whenever a fly appears
#define SND_RIBBIT		22
#define SND_TURRETFIRE  23	// turret shooting
#define SND_REPAIRBOT	24	// repairbot being happy
#define SND_TURRETFIXED	25
#define SND_LOBSTER		26	// the lobster makes this sound continually
#define SND_EVILLASER	27
#define SND_FIREBALL	28
#define SND_LOBCHARGE	29	// charging a laser to fire
#define SND_POWERUP		30	// got a powerup
#define SND_CHEAT		31
#define SND_TRICERAROAR 32	// triceratops roar upon arriving
#define SND_PTEROSCREECH 33	// pterodactyl screeching for power dive
#define SND_PTEROFLAP	34	// pterodactyl wing flap
#define SND_SLURP		35	// eyeball splitting in two
#define SND_EYEGUYFIRE	36	// eye guy shooting flame
#define SND_SCORPION	37	// scorpion shooting bullet
#define SND_SHOOT1		38	// spaceship shoot noise 1
#define SND_SHOOT2		39	// spaceship shoot noise 2
#define SND_SCORPSHOOT	40
#define SND_BABYLEAP	41	// baby alien leaping at you (or out of its egg)
#define SND_BABYDIE		42	// baby alien squealing to death
#define SND_FLYALIENSPIT 43
#define SND_CLINK		44	// bullet not doing any damage
#define SND_QUEENROAR	45
#define SND_QUEENHEADDIE 46
#define SND_QUEENDIE	47
#define SND_QUEENLOB	48
#define SND_ALIENBIRTH	49
#define SND_MENUSELECT	50
#define SND_SPARKYFIRE  51
#define SND_VOLCANOFIRE 52
#define SND_PAUSE		53
#define SND_UNPAUSE		54
#define SND_SANDWORM	55
#define SND_CONTYES		56
#define SND_CONTNO		57	// choosing to continue or not
#define SND_GAMEOVER	58
#define SND_CONTINUE	59
#define SND_BOOM		60
#define SND_BOOM2		61
#define SND_BOOM3		62

// the songs
#define SND_SONG1		100

void MakeSound(int snd,int priority);
void PlaySong(int sng);
void StopSong(void);
void ReplaySong(void);
void VolumeSong(byte hi);

#endif
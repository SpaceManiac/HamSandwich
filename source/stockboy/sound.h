#ifndef SOUND_H
#define SOUND_H

#include "jamulsound.h"
#include "jamultypes.h"

#define SND_MENUCLICK	0
#define SND_MENUCANCEL	1
#define SND_MENUSELECT	2

// sounds for Ro-Bot
#define SND_ROBOTSTEP	3
#define SND_ROBOTPUSH	4
#define SND_ROBOTDIE	5
#define SND_ROBOTBORED	6

// generic player sounds
#define SND_FIRELASER	7
#define SND_BOMBDROP	8
#define SND_BOMBBOOM	9
#define SND_BOMBBOOM2	10
#define SND_BOMBBOOM3	11
#define SND_BOMBFIZZ	12
#define SND_BOMBFIZZ2	13
#define SND_ITEMGET		14

// tally sounds
#define SND_STARGO		15
#define SND_STARSTOP	16

// other gameplay sounds
#define SND_FALLINPIT	17
#define SND_FALLINHOLE	18
#define SND_TELEPORT	19
#define SND_NOTELEPORT	20
#define SND_GENERATE	21

// monster sounds
#define SND_WALK_BLOBBY	22
#define SND_WALK_FLUFFY	23
#define SND_WALK_INCHY	24
#define SND_WALK_PIGGY	25

#define SND_MONS_BUMP	26
#define SND_MONS_PUSH	27
#define SND_MONS_SHOOT	28
#define SND_MONS_BOMB	29	// lighting a bomb
#define SND_MONS_EAT	30	// vaporize an object
#define SND_FREEZE		31  // freeze an object

#define SND_GETGOODIE	32	// earn a hidden goodie
#define SND_EMPMONTH	33	// earning an emp. of month
#define SND_EMPYEAR		34	// earning an emp. of year

#define SND_NOITEM		35	// try to use an item you don't have, or drop a bomb where you can't

// surfboy sounds
#define SND_SURFSTEP	36
#define SND_SURFPUSH	37
#define SND_SURFDIE		38
#define SND_SURFBORED	39

// gothgirl sounds
#define SND_GOTHSTEP	40
#define SND_GOTHPUSH	41
#define SND_GOTHDIE		42
#define SND_GOTHBORED	43

// hippie sounds
#define SND_HIPSTEP		44
#define SND_HIPPUSH		45
#define SND_HIPDIE		46
#define SND_HIPBORED	47

// santa sounds
#define SND_SANTASTEP	48
#define SND_SANTAPUSH	49
#define SND_SANTADIE	50
#define SND_SANTABORED	51

// stick sounds
#define SND_STICKSTEP	52
#define SND_STICKPUSH	53
#define SND_STICKDIE	54
#define SND_STICKBORED	55

// cherub sounds
#define SND_CHERUBSTEP	56
#define SND_CHERUBPUSH	57
#define SND_CHERUBDIE	58
#define SND_CHERUBBORED	59

// shroom sounds
#define SND_SHROOMSTEP	60
#define SND_SHROOMPUSH	61
#define SND_SHROOMDIE	62
#define SND_SHROOMBORED	63

// pig sounds
#define SND_PIGSTEP		64
#define SND_PIGPUSH		65
#define SND_PIGDIE		66
#define SND_PIGBORED	67

// freak sounds
#define SND_FREAKSTEP	68
#define SND_FREAKPUSH	69
#define SND_FREAKDIE	70
#define SND_FREAKBORED	71

#define SND_BULLETPOP	72
#define SND_SWITCHFLIP	73
#define SND_CRANKTURN	74
#define SND_RESCUE		75
#define SND_SHIELD		76
#define SND_TIMERBEEP	77
#define SND_ACID		78
#define SND_DETONATOR	79
#define SND_PAUSE		80
#define SND_HAMUMU		81
#define SND_SHIELDDOWN	82
#define SND_GOALUP		83

#define SND_WINLEVEL	99	// little win song

//--------------------------------

void InitSound(void);
void ExitSound(void);
void MakeSound(int snd,int x,int y,int flags,int priority);
void MakeNormalSound(int snd);
void MakeNormalSound2(int snd);
void LoopingSound(int snd);
void SoundSystemExists(void);
void KillSong(void);
void SetVolumes(int sndvol,int musvol);

void PlayInstrument(int ins,int note,long vol,byte flags,byte seq);

#endif

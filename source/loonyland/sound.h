#ifndef SOUND_H
#define SOUND_H

#include "jamulsound.h"
#include "jamultypes.h"

// bonehead
#define SND_SKELOUCH	0
#define SND_SKELDIE		1
#define SND_SKELKICK	2
#define SND_SKELSHOOT	3
#define SND_SKELGAS		4	// gangrene gas attack
#define SND_SKELSUMMON  5	// numbskull summoning
#define SND_SKELBLAST	6	// numbskull flamewall attack
// zombie
#define SND_ZOMBIEOUCH  7
#define SND_ZOMBIEDIE	8
#define SND_ZOMBIELEAP	9
#define SND_ZOMBIESTOMP 10
// player
#define SND_LOONYOUCH	11
#define SND_LOONYDIE	12
#define SND_LOONYSHOOT	13
#define SND_LOONYBORED	14
// villager
#define SND_VILLAGERSCRATCH 15
#define SND_VILLAGERYAWN	16	// female
#define SND_VILLAGERHELLO	17	// male
#define SND_VILLAGERHELLO2	18	// female
#define SND_VILLAGERHELLO3	19	// other male
// frog
#define SND_FROGOUCH	20
#define SND_FROGDIE		21
#define SND_FROGLICK	22
#define SND_STEAM		23	// steam when water hits Toasty
// mummy
#define SND_MUMMYSMACK	24
#define SND_MUMMYSUMMON 25
// evil tree
#define SND_TREEOUCH	26
#define SND_TREEDIE		27
#define SND_TREEFROG	28
// bat
#define SND_BATOUCH		29
#define SND_BATDIE		30
#define SND_BATEYES		31
#define SND_BATSHOOT	32
#define SND_BATDIVE		33
// wolfman
#define SND_WOLFOUCH	34
#define SND_WOLFDIE		35
#define SND_WOLFSPIT	36
#define SND_WOLFHOWL	37
#define SND_WOLFPOUNCE	38
#define SND_WOLFCHARGE	39
// swampdog
#define SND_SWAMPOUCH	40
#define SND_SWAMPDIE	41
#define SND_SWAMPALERT	42
#define SND_SWAMPJAB	43
#define SND_SWAMPCHARGE	44
#define SND_SWAMPSHOOT	45
// ghost
#define	SND_GHOSTOUCH	46
#define SND_GHOSTDIE	47
#define SND_GHOSTSPIN	48
#define SND_GHOSTPUNCH	49
#define SND_GHOSTSCREAM	50
#define SND_GHOSTTELEPORT 51
// part of vampire
#define SND_VAMPSUMMON	52
#define SND_VAMPSHOOT	53
// stick witch
#define SND_STICKOUCH	54
#define SND_STICKDIE	55
#define SND_STICKATTACK	56
// other junk
#define SND_BLOCKPUSH	57
#define SND_GETQUEST	58
#define SND_FINISHQUEST 59
#define SND_ENTERMAP	60

// weapons
#define SND_WHOOPEE1	61
#define SND_WHOOPEE2	62
#define SND_WHOOPEE3	63
#define SND_CACTUS		64
// other junk
#define SND_POWERUP		65
#define SND_HEAL		66
#define SND_MEOW		67
#define SND_GRATE		68	// a bridge grating open
#define SND_MUSHGET		69
#define SND_WPNGET		70
#define SND_GEMGET		71
#define SND_BIGGEMGET	72
#define SND_ZOMBIESDEAD 73	// cleaned out a crypt
#define SND_DOOROPEN	74
#define SND_KEYGET		75
#define SND_SWITCH		76
#define SND_SWITCH2		77	// the rolly switch grating
#define SND_MUCK		78	// walking in muck
#define SND_STONEBONK	79	// tumblestone hitting a wall
#define SND_PLANTGROW	80
#define SND_LIGHTSMACK	81	// hitting a LightBall
#define SND_BOOM		82
#define SND_BIGBOOM		83
#define SND_AUTODOOROPEN 84
#define SND_AUTODOORSHUT 85
// Frankenjulie
#define SND_FRANKOUCH	86
#define SND_FRANKDIE	87
#define SND_FRANKHEAD	88
#define SND_FRANKARMS	89
#define SND_FRANKMISSILE 90
// Polterguy
#define SND_POLTEROUCH	91
#define SND_POLTERDIE	92
#define SND_POLTERSNEEZE 93
#define SND_POLTERGET	94	// activating a chair/barrel/etc

#define SND_GRENADE		95	// orb grenade being launched
#define SND_SAVEGAME	96
#define SND_LOADGAME	97
#define SND_PAUSE		98

// pumpkin
#define SND_PUMPKINOUCH	99
#define SND_PUMPKINDIE	100
// vampire
#define SND_VAMPOUCH	101
#define SND_VAMPDIE		102
#define SND_VAMPSHOVE	103
#define SND_MUMMYSHOOT	104
// other junk
#define SND_ZAP			105
// elder vampires
#define SND_ELDEROUCH	106
#define SND_ELDERDIE	107
#define SND_ELDERSHOOT	108
#define SND_ELDERCHARGE 109
#define SND_ELDERSUMMON 110
#define SND_BUBBLEPOP	111
#define SND_FREEZE		112
#define SND_FILM		113	// film ticking noise for vintage mode
#define SND_VILLDIE1	114	// male villager die
#define SND_VILLDIE2	115	// female villager die
// bonkula
#define SND_BONKOUCH	116
#define SND_BONKDIE		117
#define SND_BONKSWING	118
#define SND_BONKHIT		119
#define SND_BONKHURRY	120
#define SND_BONKHURRY2	121
//other
#define SND_WITCHHI		122	// when stick witch first appears
#define SND_BADGEGET	123	// get a badge! happy song!
#define SND_SCARYBOSS	124	// the scary boss song
//fireworks
#define SND_FWORK		125
#define SND_FWORK2		126
#define SND_CRUMP		127
#define SND_CRUMP2		128
//loonyball
#define SND_PENALTY		129
#define SND_PENALTYOPEN 130
#define SND_GOAL		131
#define SND_WIN			132
#define SND_BALLKICK	133
#define SND_BALLBOUNCE	134
#define SND_BALLDRIBBLE 135

#define SND_MODEGET		136	// song for earning a new mode!
#define SND_WITCHOUT	137	// witch out of magic
#define SND_MAGICUP		138
// kick the cat
#define SND_CATBOUNCE	139
#define SND_CATBOUNCE2	140
#define SND_CATKICK		141
#define SND_CATDRIBBLE	142
#define SND_CATDRIBBLE2	143
// toasty water
#define SND_WATERSPURT	144
#define SND_WATERSPLASH	145
// other
#define SND_HISCORE		146
#define SND_MENUCLICK	147
#define SND_MENUSELECT	148
#define SND_MENUCANCEL	149
#define SND_FRANKOUCH2	150
#define SND_GRAVESMASH	151
#define SND_CATTAILSMASH 152
#define SND_EVILIZER	153	// the evilizer running
#define SND_METALSMACK	154
// witch spells
#define SND_BUZZSAW		155
#define SND_SPEED		156

// movie sounds
#define SND_FROGSPLAT	157	// frog hitting the ground
#define SND_LOONYFLY	158
#define SND_MINIRIBBIT	159
#define SND_LOONYSPLAT	160
#define SND_MOVIELICK	161
#define SND_EVILSPLAT	162
#define SND_EVILFALL	163
#define SND_THUMBSUP	164
#define SND_MOVIEOOF	165
#define SND_MOVIESONG	166
// witch
#define SND_WITCHBORED	167

// enviro-sound
#define SND_SWAMPGURGLE 168
#define SND_CLIFFSWIND	173
#define SND_WOODSLIFE	178

// wolf-larry
#define SND_LARRYOUCH	183
#define SND_LARRYDARTS	184
#define SND_LARRYPOUNCE	185
#define SND_LARRYHOWL	186
#define SND_LARRYNORMAL	187	// return to normal
#define SND_FIREBURN	188	// fire crackle for hot pants

// the hamumu sound
#define SND_HAMUMU		200
#define SND_ENDSONG		201

//--------------------------------

void InitSound(void);
void ExitSound(void);
void MakeSound(int snd,int x,int y,int flags,int priority);
void MakeNormalSound(int snd);
void MakeNormalSound2(int snd);
void MakeSpaceSound(int snd,int priority);
void LoopingSound(int snd);
void KillSong(void);

#endif

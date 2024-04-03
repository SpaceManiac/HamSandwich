#ifndef SOUND_H
#define SOUND_H

#include "jamulsound.h"
#include "jamultypes.h"

#define SND_MENUCLICK	0
#define SND_MENUSELECT  1
#define SND_PAUSE		2
#define SND_SAVEGAME	3
#define SND_LOADGAME	4
#define SND_HAMMERBONK	5
#define SND_HAMMERREFLECT 6
#define SND_BULLETREFLECT 7
#define SND_BOMBREFLECT 8
#define SND_MISSILEBOOM 9
#define SND_ACIDSPLAT	10
#define SND_ENERGYBONK  11	// little red bullets hitting the wall
#define SND_BOMBBOOM	12
#define SND_BULLETHIT	13
#define SND_MEGABEAMHIT 14
#define SND_BOMBTHROW   15
#define SND_MISSILELAUNCH 16
#define SND_FLAMEGO		17	// each piece of flame being thrown does this
#define SND_BULLETFIRE  18	// machine gun (1 shot)
#define SND_KOOLKAT		19	// when you get all the brains
#define SND_MEGABEAM	20	// being fired
#define SND_HAMMERTOSS  21
#define SND_CHEATWIN	22	// skip level via cheat code
#define SND_HEALTHCHEAT 23
#define SND_INVINCCHEAT 24	// doesn't exist yet, but someday
#define SND_DROWN		25
#define SND_ITEMDROP	26
#define SND_GOTOMAP		27
#define SND_WINLEVEL	28
#define SND_WALLDOWN	29
#define SND_WALLUP		30
#define SND_LIGHTSON	31
#define SND_TELEPORT	32
#define SND_MESSAGE		33
// now the frigging monster sounds...
// bonehead
#define SND_SKELKICK	34
#define SND_SKELSHOOT	35
#define SND_SKELOUCH	36
#define SND_SKELDIE		37
// scary bat
#define SND_BATEYES		38
#define SND_BATDIVE		39
#define SND_BATOUCH		40
#define SND_BATDIE		41
// eensy weensy spider
#define SND_SPD1OUCH	42
#define SND_SPD1DIE		43
#define SND_SPD1GNAW	44
// spitter spider
#define SND_SPD2OUCH	45
#define SND_SPD2DIE		46
#define SND_SPD2SPIT	47
// zombie
#define SND_ZOMBIEOUCH	48
#define SND_ZOMBIEDIE	49
#define SND_ZOMBIELEAP	50
#define SND_ZOMBIEMOAN	51	// not implemented yet, but random moans from the zombies while they wander
// egg sac
#define SND_EGGSACDIE	52
#define SND_EGGSACBIRTH	53
// mama spider
#define SND_SPD3OUCH	54
#define SND_SPD3DIE		55
#define SND_SPD3CHOMP	56
#define SND_SPD3SPEW	57
// pygmy
#define SND_PYGMYOUCH	58
#define SND_PYGMYDIE	59
#define SND_PYGMYYAWN	60
#define SND_PYGMYSPIN	61
#define SND_PYGMYSTAB	62
#define SND_PYGMYANGRY  63
// aquazoid
#define SND_SERPENTOUCH	64
#define SND_SERPENTDIE	65
#define SND_SERPENTSPIT	66
// Matilda!!!!!
#define SND_MATTIEOUCH  67
#define SND_MATTIETRUEDIE 68
#define SND_MATTIEDIE	69	// this is when her head or skull dies
#define SND_MATTIECLAW	70
#define SND_MATTIESHOOT 71
#define SND_MATTIESTOMP 72
// Ninjabread
#define SND_GINGEROUCH	73
#define SND_GINGERDIE	74
#define SND_GINGERJKICK	75
#define SND_GINGERCOMBO	76
// pumpkin
#define SND_PUMPKINDIE	77	// no ouch sound- they usually die in one hit, if not who cares
// sudden leap back into non-monster sounds
// item pickup sounds
#define SND_HAMMERUP	78
#define SND_PANTS		79
#define SND_REVERSE		79
#define SND_REFLECT		79

#define SND_WEAPON		82
#define SND_FOOD		83
#define SND_SHIELD		84
#define SND_GETBRAIN	85
#define SND_GETKEY		86
#define SND_GETKEYCHAIN	87
#define SND_LOONYKEY	88
#define SND_BOUAPHAOUCH	89
#define SND_BOUAPHADIE	90
#define SND_BOUAPHABORED 91
#define SND_DOOROPEN	92
#define SND_WORLDTURN	93
#define SND_WORLDPICK	94
#define SND_BLOCKPUSH	95
// sounds for Mush
#define SND_MUSHMAD		96
#define SND_MUSHMISSILE 97
#define SND_MUSHSPORES	98
#define SND_MUSHOUCH	99
#define SND_MUSHDIE		100
// Super zombie sounds
#define SND_SZGRAB		101
// thingie snore
#define SND_BABYSNORE	102
// Happy stick man
#define SND_HAPPYFLEX	103
#define SND_HAPPYOUCH	104
#define SND_HAPPYDIE	105
#define SND_HAPPYCHOMP	106
// The Thing
#define SND_THINGVOMIT	107	// spitting up a baby that is
// zomboni
#define SND_ZOMBONIZOOM 108
// baby SEAL
#define SND_SEALOUCH	109
#define SND_SEALDIE		110
// pengulon
#define SND_PENGDIE		111
#define SND_PENGPECK	112
// snowguy
#define SND_SNOWOUCH	113
#define SND_SNOWDIE		114
#define SND_SNOWTHROW	115
// yeti bros
#define SND_SVENHI		116
#define SND_BJORNHI		117
#define SND_YETIOUCH	118
#define SND_YETIDIE		119
#define SND_YETIYODEL	120
// cacton
#define SND_CACTONSHOOT 121
#define SND_CACTONDIE	122
// djinni
#define SND_DJINNIOUCH	123
#define SND_DJINNIDIE	124
#define SND_DJINNIPUNCH	125
#define SND_DJINNIBOO	126
// magic lamp
#define SND_LAMPPOOF	127
#define SND_LAMPDIE		128
// mumble
#define SND_MUMBLECRUSH 129
// roly poly
#define SND_ROLYPOLYWALL 130
#define SND_ROCKBOUNCE	131	// rocks bouncing off wall, not really roly poly, but kind of
// sphinx
#define SND_SPHINXSNEEZE 132
#define SND_SPHINXSTOMP	133
// wacko
#define SND_WACKOSTAB	134
#define SND_WACKOOUCH	135
#define SND_WACKODIE	136
// boiler
#define SND_BOILERHIT	137
// great pumpkin
#define SND_GREATPKOUCH	138
#define SND_GREATPKDIE	139
#define SND_GREATPKVOMIT 140
// Dr. L
#define SND_DRLLAUGH	141
#define SND_DRLFEAR		142
#define SND_DRLOUCH		143
#define SND_DRLDIE		144
// SDZL
#define SND_SDZLOUCH	145
#define SND_SDZLDIE		146
#define SND_SDZLSLASH	147
#define SND_SDZLPOUND	148
#define SND_SDZLSPIT	149
// Santa
#define SND_SANTAOUCH	150
#define SND_SANTADIE	151
#define SND_SANTAHIT	152
// splash screen sounds
#define SND_HAMUMU		153
#define SND_XGAMES		154
#define SND_FINAL		155

#define SND_MONEY		156
#define SND_ATTACKED	157
#define SND_CHAT		158
#define SND_PTEROSHOOT  159
#define SND_PTERODIE    160
#define SND_PTEROSUMMON	161
#define SND_LEVELUP		162
#define SND_FROZEN		163
#define SND_ICEBEAM		164
#define SND_BERSERK		165
#define SND_STONESKIN   166
#define SND_INFERNAL	167
#define SND_EYEGUYSHT	168
#define SND_PEEPDIE		169
#define SND_PEEPOUCH	170
#define SND_PEEPSHOOT	171
#define SND_EYEGUYOUCH  172
#define SND_EYEGUYDIE	173
#define SND_BIGRUMBLE	174
#define SND_TOWEROUCH	175
#define SND_TOWERDIE	176
#define SND_LOOKEYOUCH  177
#define SND_LOOKEYDIE	178
#define SND_SCARIEROUCH 179
#define SND_SCARIERDIE	180
#define SND_SCARIERSPIT 181
#define SND_UBEROUCH	182
#define SND_UBERDIE		183
#define SND_UBERSPIT	184
#define SND_GOLEMPOUND	185
#define SND_GOLEMOUCH	186
#define SND_GOLEMDIE	187
#define SND_BOBBYOUCH	188
#define SND_BOBBYDIE	189
#define SND_BOBBYSLASH	190
#define SND_BOBBYSTOMP	191
#define SND_BOBBYSPIN	192
#define SND_BOBBYBLOCK	193
#define SND_ARMAGEDDON  194
#define SND_INTRO1		195
#define SND_INTRO2		196
#define SND_TRAIL1		197
#define SND_TRAIL2		198
#define SND_BOBBYLAUGH  199
//--------------------------------

// song list
#define SONG_CHAP12FIGHT 200
#define SONG_CHAP12LEVEL 201
#define SONG_CHAP12MAP	 202
#define SONG_BEATNIK	 203
#define SONG_SHOP	     204
#define SONG_INTRO		 205

#define SONG_CHAP34MAP	 208
#define SONG_CHAP34LEVEL 207
#define SONG_CHAP34FIGHT 206

// back to sounds for some reason
#define SND_OCTONSHOOT	209
#define SND_OCTONSPLASH	210
#define SND_OCTONOUCH	211
#define SND_OCTONDIE	212
#define SND_BUSHDIE		213
#define SND_BUSHOUCH	214
#define SND_FAIRYGET	215
#define SND_INCABEAM	216
#define SND_INCAGEN		217
#define SND_INCAHIT		218
#define SND_INCADIE		219
#define SND_INCADIE2	220
#define SND_INCATONGUE	221
#define SND_RESURRECT	222
#define SND_PURCHASE	223
#define SND_UNAVAILABLE 224
#define SND_FAILSPELL	225
#define SND_FLUSH		226
#define SND_SNAILHIT	227
#define SND_SNAILDIE	228
#define SND_SNAILINSHELL 229
#define SND_SNAILOUTSHELL 230
#define SND_GOATSHOOT	231
#define SND_GOATHIT		232
#define SND_GOATDIE		233
#define SND_GOATRUN		234
#define SND_GOATDIZZY	235
#define SND_CHLGCRYSTAL	236
#define SND_ALLCHLGCRYSTAL 237
#define SND_PEEPALARM	238
#define SND_BALLBOUNCE  239
#define SND_CHICKEN		240
#define SND_TRIVIA		241
#define SONG_HAPPYSTICK 242

// set this when done
#define MAX_SOUND 100	// the most sounds used

void InitSound(void);
void ExitSound(void);
void MakeSound(int snd,int x,int y,int flags,int priority);
void MakeNormalSound(int snd);
void PlaySong(int sng);
void StopSong(void);
void ReplaySong(void);
void VolumeSong(byte hi);
void VolumeSound(byte hi);
int CurrentSong(void);

#endif

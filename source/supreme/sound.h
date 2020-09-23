#ifndef SOUND_H
#define SOUND_H

#include "jamulsound.h"
#include "jamultypes.h"
#include <stdio.h>

//s
#define SND_NONE		0
#define SND_MENUCLICK	51
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
#define SND_BATOUCH		41
#define SND_BATDIE		40
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
#define SND_REVERSE		80
#define SND_REFLECT		81
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
#define SND_CRAZYSTAB	134
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

// EXPANSION SOUNDS
// minecart
#define SND_MINECART	153

// ------------- NOT AN EXPANSION SOUND:::  THIS IS THE HAMUMU THEME
#define SND_HAMUMU		154
// vampire
#define SND_VAMPATTK	155
#define SND_VAMPFEAR	156
#define SND_VAMPOUCH	157
#define SND_VAMPDIE		158

// pwr armor
#define SND_ARMORSHOOT	159

// triceroid
#define SND_TRICERAROAR 160
// kongor
#define SND_CHESTPOUND	161
#define SND_ROBOOUCH	162
#define SND_ROBODIE		163
#define SND_FACTORYDIE	164
#define SND_COFFIN		165
#define SND_GHOSTOUCH	166
#define SND_GHOSTDIE	167
#define SND_GHOSTTPORT	168
#define SND_GHOSTYELL	169
#define SND_LEFTYOUCH	170
#define SND_LEFTYDIE	171
#define SND_LEFTYWAVE   172 // "hello" muffled
#define SND_UNGADANCE	173
#define SND_SHAMANOUCH	174
#define SND_SHAMANDIE	175
#define SND_SHAMANZAP	176
#define SND_SHAMANDANCE	177
#define SND_PKSTEINBUMP 178
#define SND_GLASSBREAK	179
#define SND_GLASSHIT	180
#define SND_GLASSDIE	181
#define SND_GLASSBLOCK	182
#define SND_TRICEROUCH	183
#define SND_TRICERDIE	184
#define SND_TRICERBUMP	185
#define SND_COUNTOUCH	186
#define SND_COUNTDIE	187
#define SND_COUNTHELLO	188
#define SND_COUNTSUMMON 189
#define SND_COUNTSHOOT	190
#define SND_COUNTCHARGE 191
#define SND_EGGOUCH		192
#define SND_EGGDIE		193
#define SND_EGGOPEN		194
#define SND_ALIENOUCH	195
#define SND_ALIENDIE	196
#define SND_ALIENSPIT	197
#define SND_INVIDCHARGE 198
#define SND_ROBOSHOOT	199
#define SND_FACTORYGO	200
#define SND_KONGOUCH	201
#define SND_KONGDIE		202
#define SND_KONGHELLO	203
// bouapha weapons
#define SND_LIGHTNING	204
#define SND_SLASH		205
#define SND_CLANG		206
#define SND_MINELAY		207
#define SND_RAGE		208		// NOT JUST IN EXPANSION!
#define SND_SPEEDUP		209		// get particle accelerator
#define SND_ROBOBOUAPHAOUCH 210
#define SND_ROBOBOUAPHAON 211
#define SND_ULTRAPYGMYOUCH 212
#define SND_ULTRAPYGMYDIE  213
#define SND_ULTRAPYGMYYAWN 214
#define SND_ULTRAPYGMYSTAB 215
#define SND_ULTRAPYGMYANGRY 216
#define SND_ROBOSTOMP	217
#define SND_SHAMANANGRY 218
#define SND_ZAP			219
#define SND_BALLLIGHTNING 220
#define SND_VICTORY		221
#define SND_COINGET		222
#define SND_BATSTARE	223
#define SND_CANDLEGET	224
#define SND_ALLCANDLE	225
#define SND_CRAZYDASH	226
#define SND_CRAZYSPIN	227
#define SND_PYGMYCASE	228
#define SND_CRAZYMAD	229
#define SND_YERFDOG		230
#define SND_SCUBA		231
#define SND_BUBBLE		232
#define SND_DIVERANGRY	233
#define SND_DIVERDIE	234
#define SND_GLUP		235	// mini-drown sound, like GLUP!
#define SND_OLAFHI		236
#define SND_FOLLOWHI	237	// follow bunny awaken
#define SND_CARSTART	238
#define SND_CARGO		239
#define SND_JELLOOUCH	240
#define SND_KINGDIE		241
#define SND_KINGCONES	242
#define SND_KINGSCOFF	243
#define SND_FISHDIE		244
#define SND_SUBSTART	245
#define SND_SUBSHOOT	246
#define SND_SUBMINE		247
#define SND_OYOYOY		248
#define SND_CUKEDANCE	249
#define SND_RUFFIAN		250
#define SND_HAVEATYOU	251
#define SND_CUKEDIE		252
#define SND_CUKEOUCH	253
#define SND_WILLYIDLE	254
#define SND_WILLYIDLE2	255
#define SND_BUNNYDIE	256
#define SND_MINDWIPE	257
#define SND_SHARKATTACK	258
#define SND_SHARKOUCH	259
#define SND_SHARKDIE	260
#define SND_WIZHIT		261
#define SND_WIZOUCH		262
#define SND_WIZDIE		263
#define SND_HAMMERDN	264
#define SND_PANTSDN		265
#define SND_LAYEGG		266
#define SND_GNOMESEE	267
#define SND_GNOMELAUGH	268
#define SND_GENERATE	269
#define SND_CLONEOUCH	270
#define SND_CLONEDIE	271
#define SND_TURRETBZZT	272
#define SND_SKELALIVE	273
#define SND_WILLYOUCH	274
#define SND_WILLYDIE	275
#define SND_CRABCLICK	276
#define SND_PATCHDIE	277
#define SND_KINGTICKLE	278
#define SND_PATTYOUCH	279
#define SND_PATTYDIE	280
#define SND_BLOWBUBBLE	281
#define SND_PATTYSUMMON	282
#define SND_DOZERSPIN	283
#define SND_DOZEROUCH	284
#define SND_AMMOCRATE	285
#define SND_BUBBLEPOP	286
#define SND_SCARIEROUCH 287
#define SND_SCARIERDIE	288
#define SND_SCARIERSPIT 289
#define SND_UBEROUCH	290
#define SND_UBERDIE		291
#define SND_UBERSPIT	292
#define SND_WOLFOUCH	293
#define SND_WOLFDIE		294
#define SND_WOLFHOWL	295
#define SND_WOLFPOUNCE	296
#define SND_WOLFCHARGE	297
#define SND_CLAP		298

#define SND_MORONRIGHT	299
#define SND_MORONRIGHT2 300
#define SND_MORONWRONG	301
#define SND_MORONHELLO	302
#define SND_MORONWIN	303

#define SND_FREEZE		304
#define SND_LUNASHOOT	305
#define SND_LUNAOUCH	306
#define SND_LUNADIE		307
#define SND_LUNADROWN	308
#define SND_LUNABORED	309
#define SND_LUNABRAINS	310
#define SND_BUNNYTHEME	311
#define SND_HAPPYWEEN	312
#define SND_MARKMOVIE	313
#define SND_TZONE		314
#define SND_BOING		315
#define SND_BOING2		316
#define SND_BOING3		317
#define SND_TALLY		318
#define SND_SHOPBELL	319
#define SND_LOCKER		320
#define SND_BUYITEM		321
#define SND_TURNEVIL	322
#define SND_TURNGOOD	323
#define SND_ALLKEYCHAIN 324
#define SND_MINIZOIDSPIT 325
#define SND_MINIZOIDDIE	326
#define SND_MINIWACKOOUCH	327
#define SND_MINIWACKODIE	328
#define SND_JACKATTACK	329
#define SND_JACKDIE		330

//kid mystic sounds
#define SND_KOOLKATKM	331 	//kid mystic kool kat bad
#define SND_DROWNKM		332		//kid mystic drown
#define SND_WINKM		333		//kid mystic win level
#define SND_KMOUCH		334		//kid mystic ouch
#define SND_KMDIE		335		//kid mystic die
//peeper... oh man
#define SND_PEEPDIE		336		//peeper
#define SND_PEEPOUCH	337
#define SND_PEEPSHOOT	338
//eyeguy
#define SND_EYEGUYOUCH  339		//eye guy
#define SND_EYEGUYDIE	340
//tower
#define SND_TOWEROUCH	341
#define SND_TOWERDIE	342
//lookey loo & xeno empress
#define SND_LOOKEYOUCH  343		//also used by xeno empress
#define SND_LOOKEYDIE	344
//grumpy golem
#define SND_GOLEMPOUND	345		//grumpy golem
#define SND_GOLEMOUCH	346
#define SND_GOLEMDIE	347
//bobby khan
#define SND_BOBBYOUCH	348
#define SND_BOBBYDIE	349
#define SND_BOBBYSLASH	350		//also used by sdzl's charge
#define SND_BOBBYSTOMP	351		//also used by sdzl's throwup
#define SND_BOBBYSPIN	149 	//basically the same sound as sdz's spit attack
#define SND_BOBBYBLOCK	352
//back to eye guy
#define SND_EYEGUYSHT	353
//octon
#define SND_OCTONSHOOT	354
#define SND_OCTONSPLASH	355
#define SND_OCTONOUCH	356
#define SND_OCTONDIE	357
//bush
#define SND_BUSHDIE		358
#define SND_BUSHOUCH	359
//the incas
#define SND_INCABEAM	360
#define SND_INCAGEN		361
#define SND_INCAHIT		362
#define SND_INCADIE		363
#define SND_INCATONGUE	364
//snail
#define SND_SNAILHIT	365
#define SND_SNAILDIE	366
//goat
#define SND_GOATSHOOT	367
#define SND_GOATHIT		368
#define SND_GOATDIE		369
#define SND_GOATRUN		370
#define SND_GOATDIZZY	371
//peeping bomb
#define SND_PEEPALARM	372

//ah, time for loonyland sounds
#define SND_SKELGAS		373		// gangrene gas attack
#define SND_SKELSUMMON  374		// numbskull summoning
// player
#define SND_LOONYOUCH	375
#define SND_LOONYDIE	376
#define SND_LOONYBORED	377
// Villager
#define SND_VILLAGERSCRATCH	378
#define SND_VILLAGERYAWN	379	// female
#define SND_VILLDIE1	380	// male villager die
#define SND_VILLDIE2	381	// female villager die
// Swampdog
#define SND_SWAMPOUCH	382
#define SND_SWAMPDIE	383
#define SND_SWAMPALERT	384
#define SND_SWAMPJAB	385
#define SND_SWAMPCHARGE	386
#define SND_SWAMPSHOOT	387
// Ghostly
#define SND_GHOSTSPIN	388
#define SND_GHOSTPUNCH	389
// Happy Stick Witch
#define SND_STICKTAUNT	390
#define SND_STICKOUCH	391
#define SND_STICKDIE	392
// Frankenjulie
#define SND_FRANKOUCH	393
#define SND_FRANKDIE	394
#define SND_FRANKHEAD	395
#define SND_FRANKARMS	396
#define SND_FRANKMISSILE 397
// Polterguy/Spookley
#define SND_POLTEROUCH	398
#define SND_POLTERDIE	399
#define SND_POLTERSNEEZE 400
#define SND_POLTERGET	401	// activating a chair/barrel/etc
//PUMPKIN!
#define SND_PUMPKINOUCH	402
// Mummy
#define SND_MUMMYSHOOT	403
#define SND_MUMMYSUMMON 404
// Evil tree
#define SND_TREEOUCH	405
#define SND_TREEDIE		406
#define SND_TREEFROG	407
// Elder vampires
#define SND_ELDEROUCH	408
#define SND_ELDERDIE	409
#define SND_ELDERSHOOT	410
#define SND_ELDERCHARGE 411
#define SND_ELDERSUMMON 412
// Bonkula
#define SND_BONKOUCH	413
#define SND_BONKDIE		414
#define SND_BONKSWING	415
#define SND_BONKHIT		416
// Evilizer (and water)
#define SND_EVILIZER	417	// the evilizer running
#define SND_WATERSPURT	418
#define SND_WATERSPLASH	419
// Larry
#define SND_LARRYOUCH	420
#define SND_LARRYDARTS	421
#define SND_LARRYPOUNCE	422
#define SND_LARRYHOWL	423
// Forg
#define SND_FROGOUCH	424
#define SND_FROGDIE		425
#define SND_FROGLICK	426
// Cow
#define SND_COWMOO		427
#define SND_COWOUCH		428
#define SND_COWDIE		429
// other
#define SND_FRANKOUCH2 	68
#define SND_CHEER 		430
#define SND_DEATHRAY 	431
#define SND_INFERNAL 	432
#define SND_SEKRIT 		433
#define SND_BEAM2 		434
// hologram dr. lunatic
#define SND_HOLOLAUGH 	435
#define SND_HOLOSHOOT 	436
#define SND_HOLODIE 	437
#define SND_HOLOOUCH 	438
#define SND_LOONYBRAINS	439
// new sphinxter noises
#define SND_SPHINXOUCH	440
#define SND_SPHINXDIE	441
// yerfdog boss!
#define SND_YERFOUCH	442
#define SND_YERFLEAVE	443
//classic
#define SND_PUMPKDIE	444
#define SND_PUMPKOUCH	445
// zombie
#define SND_SZOMBIEOUCH	446
#define SND_SZOMBIEDIE	447
#define SND_SZOMBIELEAP	512

#define NUM_ORIG_SOUNDS	448
#define MAX_SOUNDS		512		// absolute maximum? @448, add sounds after 512 (513+)
#define MAX_CUSTOM_SOUNDS 64
#define CUSTOM_SND_START (MAX_SOUNDS-MAX_CUSTOM_SOUNDS)



//--------------------------------

#define ST_INTFACE		1
#define ST_PLAYER		2
#define ST_MONSTER		4
#define ST_EFFECT		8
#define ST_VOCAL		16
#define ST_CUSTOM		32

typedef struct soundDesc_t
{
	word num;
	char name[32];
	byte theme;
} soundDesc_t;

void InitSound(void);
void ExitSound(void);
void MakeSound(int snd,int x,int y,int flags,int priority);
void MakeNormalSound(int snd);
void SoundSystemExists(void);
soundDesc_t *GetSoundInfo(int snd);
int GetNumSounds(void);
byte *GetCustomSound(int n);
long GetCustomLength(int n);
void ClearCustomSounds(void);
byte AddCustomSound(const char *fname);
int GetNumCustomSounds(void);
void DeleteCustomSound(int n);
byte ReplaceCustomSound(int n,const char *fname);
void LoadCustomSounds(FILE *f);
void SaveCustomSounds(FILE *f);

void MakeCustomSound(int snd,int x,int y,int flags,int priority);
void MakeNormalCustomSound(int snd);
int GetCustomSoundByName(const char *name);
void MakeSpaceSound(int snd,int priority);

int AppendCustomSounds(FILE *f);

#endif

#ifndef MONSTER_H
#define MONSTER_H

/* this file does the AI and everything else for each monster type.
   It knows all about the different types, as opposed to guy.cpp which
   just sort of keeps track of the list of guys. */

#include "jamulspr.h"
#include "display.h"

// the monster types
#define MONS_NONE	  0
#define MONS_BOUAPHA  1
#define MONS_BONEHEAD 2
#define MONS_BAT	  3
#define MONS_SPIDER	  4
#define MONS_BIGSPDR  5
#define MONS_ZOMBIE	  6
#define MONS_EGGSAC	  7
#define MONS_MAMASPDR 8
#define MONS_PYGMY	  9
#define MONS_SERPENT  10
// the parts of Mattie
#define MONS_MATHEAD  11
#define MONS_MATSKULL 12
#define MONS_MATBRAIN 13
#define MONS_MATBODY  14
#define MONS_MATCLAW1 15
#define MONS_MATCLAW2 16
#define MONS_MATTAIL  17
// more normal monsters
#define MONS_GINGER	  18
#define MONS_PUMPKIN  19
#define MONS_BABYTHING 20
#define MONS_MOSS	  21
#define MONS_MOSSGRANDE 22
#define MONS_MAGMAZOID 23
#define MONS_SHROOM	  24
#define MONS_MUSH	  25
// the parts of The Thing
#define MONS_THING	  26
#define MONS_THINGTENT 27
#define MONS_THINGTENTTIP 28
// normal monsters again
#define MONS_SUPERZOMBIE 29
#define MONS_STICKMAN 30
#define MONS_BABYSEAL 31
#define MONS_ISOZOID  32
#define MONS_SNOWGUY  33
#define MONS_PENGUIN  34
#define MONS_ZOMBONI  35
// the Yeti Bros.
#define MONS_SVEN	  36
#define MONS_BJORN	  37
// normal monsters again
#define MONS_GEOZOID  38
#define MONS_MUMBLE	  39
#define MONS_DJINNI	  40
#define MONS_LAMP	  41
#define MONS_CACTUS	  42
#define MONS_ROLLER	  43
#define MONS_LICH	  44
#define MONS_DUSTDEVIL 45
#define MONS_MECHABOUAPHA 46
// Sphinx parts
#define MONS_SPHXARM1 47
#define MONS_SPHXARM2 48
#define MONS_SPHINX   49
// more normal monsters
#define MONS_FREAKAZOID 50
#define MONS_CENTIBODY 51
#define MONS_CENTIHEAD 52
#define MONS_WACKO	  53
#define MONS_BOILER	  54
#define MONS_GREATPK  55
#define MONS_ULTRAZOID 56
// the Asylum bosses
#define MONS_DRL	  57
#define MONS_SDZL	  58
#define MONS_SANTA	  59

// expansion "monsters"
#define MONS_MINECART 60	// mine cart Bouapha rides on
#define MONS_RAFT	  61	// raft Bouapha rides on
#define MONS_PWRBOUAPHA 62	// Bouapha in power armor

// expansion monsters
#define MONS_VAMPIRE  63
#define MONS_COFFIN	  64
#define MONS_GHOST	  65
#define MONS_BURNER	  66
#define MONS_LEFTY	  67
#define MONS_PYGMY2	  68
#define MONS_PYGMY3	  69
#define MONS_PKSTEIN  70
#define MONS_KNIGHT	  71
#define MONS_TRICEROID 72
#define MONS_COUNTESS 73
#define MONS_ALIENEGG 74
#define MONS_BABYALIEN 75
#define MONS_ALIEN	  76
#define MONS_ROBOPK	  77
#define MONS_SHOCKTR  78
#define MONS_ROBOT1	  79
#define MONS_ROBOT2	  80
#define MONS_ROBOFACTORY 81
#define MONS_TURRET	  82
#define MONS_BUNNY	  83
#define MONS_KONGOR	  84
#define MONS_SQUASH	  85
#define MONS_MINIPYGMY 86
#define MONS_LOONYBOT 87
#define MONS_LOONYCORE 88
#define MONS_LOONYGUN 89
#define MONS_LOONYSHIP 90

// -- fun pack
#define MONS_FRIENDLY   91
#define MONS_GOODTURRET 92
#define MONS_ROLLER2	93
#define MONS_ALIENEGG2	94

#define MONS_MAT2HEAD	95
#define MONS_MAT2SKULL	96
#define MONS_MAT2BRAIN	97
#define MONS_MAT2BODY	98
#define MONS_MAT2TAIL	99

#define MONS_SHAMAN2	100
#define MONS_JALAPENO	101

#define MONS_GENERATOR1	102
#define MONS_GENERATOR2	103
#define MONS_GENERATOR3	104
#define MONS_GENERATOR4	105

#define MONS_SHARK		106
#define MONS_MADBUG		107
#define MONS_WIZARD		108
#define MONS_EVILCLONE	109
#define MONS_BOB		110
#define MONS_MULTIMOSS  111
#define MONS_MOSS2		112
#define MONS_SNOWBALL	113
#define MONS_SNOWBALL2	114
#define MONS_SNOWBLOW	115
#define MONS_BOOMKIN	116
#define MONS_MUMBLE2	117
#define MONS_GOODROBOT	118
#define MONS_GOODROBOT2 119
#define MONS_XENOMAMA	120
#define MONS_ROLLER3	121
#define MONS_ROLLER4	122
#define MONS_DARKVAMP	123
#define MONS_GNOME		124
#define MONS_NOBODY		125
#define MONS_FRIENDLY2	126
#define MONS_TROOPER2	127
#define MONS_PUMPKIN2	128
#define MONS_CRAZYBONE	129
#define MONS_CREEPAZOID 130
// -- SWC
#define MONS_LAZYBONE	131
#define MONS_STAREYBAT	132
#define MONS_MADMAN		133
#define MONS_PYGMYACCT	134
#define MONS_CRAZYPANTS	135
#define MONS_YERFDOG	136
#define MONS_ELITESEAL	137
#define MONS_CONE		138
#define MONS_PYGMYDIVER	139
#define MONS_PUFFYFISH	140
#define MONS_PUFFYFISH2 141
#define MONS_OLAF		142
#define MONS_SPIKEY		143
#define MONS_BOMBOT		144
#define MONS_FOLLOWBUNNY 145
#define MONS_AUTOZOID	146
#define MONS_CHEST		147
#define MONS_JELLOFISH	148
#define MONS_KINGCONE	149
#define MONS_MAILBOXER	150
#define MONS_PIRANHA	151
#define MONS_YUGO		152
#define MONS_MINISUB	153	// bouapha in minisub
#define MONS_PUNKIN		154
#define MONS_SEACUKE	155
#define MONS_SNKYSHRK2	156
#define MONS_STARFISH	157
#define MONS_UNDERZOID	158
#define MONS_WETSEAL	159
#define MONS_WETWILLY	160
#define MONS_CRABPUFF	161
#define MONS_PATCH		162
#define MONS_PATCH2		163	// quickly producing patch
#define MONS_PATCH3		164	// boomkin patch
#define MONS_PATCH4		165	// squash patch
#define MONS_PATTY		166
#define MONS_DOZER		167
#define MONS_DARKCOFFIN	168
#define MONS_MUTANT		169
#define MONS_WHATSIT	170
#define MONS_SHOCKGHOST 171
#define MONS_MECHAZOID	172
#define MONS_SUCKER1	173	// sucks from anywhere in the world
#define MONS_SUCKER2	174	// sucks harder
#define MONS_SUCKER3	175	// sucks over limited range
#define MONS_SUCKER4	176	// sucks harder
#define MONS_BLOWER1	177	// blows from anywhere in the world
#define MONS_BLOWER2	178	// blows harder
#define MONS_BLOWER3	179	// blows over limited range
#define MONS_BLOWER4	180	// blows harder
#define MONS_BEETLE		181
#define MONS_SCARAB		182
#define MONS_FROSTBITER 183
#define MONS_MISLTURRET 184
#define MONS_DEATHTURRET 185
#define MONS_UNDERMAGMA	186
#define MONS_PARKEDCAR	187
#define MONS_PARKED2	188
#define MONS_TRAFFIC	189
#define MONS_TRAFFIC2	190
#define MONS_PLAYMECHA	191
#define MONS_SHROOMLORD	192
#define MONS_BIGBAT		193
#define MONS_HUGEBAT	194
#define MONS_WOLF		195
#define MONS_WOLF2		196
#define MONS_PATROLLR	197
#define MONS_PATROLUD	198
#define MONS_DPATROLLR	199
#define MONS_DPATROLUD	200
#define MONS_WEATHERMAN	201
#define MONS_XENOEGG	202
#define MONS_PLAYSHROOM 203
#define MONS_PUNKBUNNY  204
#define MONS_LUNACHICK	205
#define MONS_SKELETON	206
#define MONS_HOTSHROOM	207
#define MONS_MINIZOID	208
#define MONS_MINIWACKO	209
#define MONS_JACKFROST	210

//kid mystic exclusive enemies
#define MONS_MYSTIC   	211
#define MONS_EYEGUY   	212
#define MONS_PEEPER   	213
#define MONS_TOWER    	214
#define MONS_LOOKEYLOO 	215
#define MONS_PINKEYE 	216
#define MONS_MICRO  	217
#define MONS_GOLEM 		218
#define MONS_BOBBY  	219
#define MONS_CRAZYBUSH 	220
#define MONS_OCTOPUS  	221
#define MONS_INCABOT 	222
#define MONS_INCAGEN  	223
#define MONS_INCAGOLD	224
#define MONS_INCAGOLD2	225
#define MONS_LOG	  	226
#define MONS_INCABOSS  	227
#define MONS_INCATONGUE 228
#define MONS_PEEPBOMB	229
#define MONS_PEEPBOMB2	230 //	reverse peeping bomb
#define MONS_TRAPAZOID	231
#define MONS_TRAPAZOID2 232 // hiding trapazoid, not meant for use on its own
#define MONS_WHACKAZOID	233
#define MONS_SLUG   	234
#define MONS_SNAIL	 	235
#define MONS_BALL		236
#define MONS_OCTOBOSS	237
#define MONS_OCTOTENT	238
#define MONS_OCTOTENT2	239
#define MONS_GOAT1		240
#define MONS_GOAT2		241
#define MONS_GOAT3		242
#define MONS_STKSHROOM	243
#define MONS_STKSPIDER	244
#define MONS_STKCORPSE  245
#define MONS_STKBAT		246
#define MONS_DANCER		247

//sleepless hollow exclusive enemies
#define MONS_SPIDER2	248
#define MONS_FROG	  	249
#define MONS_PUMPKINFLY	250
#define MONS_DUMBLE 	251
#define MONS_GLASSJAW 	252
#define MONS_FROG2	  	253
#define MONS_FIREBUG  	254
#define MONS_FIREBAT  	255
#define MONS_STICKTREE	256
#define MONS_GOODBONE 	257
#define MONS_RADISH	  	258
#define MONS_BIGHEAD1  	259
#define MONS_BIGHEAD2  	260
#define MONS_BIGHEAD3  	261
#define MONS_BIGBODY  	262
#define MONS_SPOOKLEY 	263
#define MONS_SPOOKLEY2 	264
#define MONS_SPARK		265
#define MONS_SPARKY	  	266
#define MONS_MEGABURNER 267
#define MONS_SKULL	 	268
#define MONS_LIGHTSWITCH 269
#define MONS_KNIGHT2	270	
#define MONS_MADCOW	  	271
#define MONS_MADCOW2	272
#define MONS_SPATULA	273
#define MONS_BATGUARD	274
#define MONS_TURRETR	275
#define MONS_TURRETD	276
#define MONS_TURRETL	277
#define MONS_TURRETU	278
#define MONS_REFLECTION	279
#define MONS_DARKNESS	280
#define MONS_SCAREDYBAT 281
#define MONS_BLASTER	282
#define MONS_SPEEDY		283
#define MONS_GRUE		284
#define MONS_MARSHBONE	285
#define MONS_PATCH5		286 //toad hole
#define MONS_PATCH6		287 //beetle nest
#define MONS_PUMPKINFLY2 288 //simulated head

//loonyland exclusive enemies
#define MONS_REDBONE		289
#define MONS_GANGRENE		290
#define MONS_NUMBSKULL		291
#define MONS_BOMBIE			292
#define MONS_ZOMBIELORD		293
#define MONS_VILLAGERM		294
#define MONS_VILLAGERF		295
#define MONS_FROG3			296
#define MONS_RAGMAN			297
#define MONS_PHARAOHF		298
#define MONS_PHARAOHG		299
#define MONS_TERRORTREE		300
#define MONS_SWAMPSTUMP		301
#define MONS_DINGBAT		302
#define MONS_GARGOYLE		303
#define MONS_VAMPBAT		304
#define MONS_DIREWOLF		305
#define MONS_LARRY			306
#define MONS_SWAMPDOG		307
#define MONS_SWAMPDOG2		308
#define MONS_SWAMPDOG3		309
#define MONS_GHOST2			310
#define MONS_GHOST3			311
#define MONS_DRACULITE		312
#define MONS_VAMPLORD		313
#define MONS_FRANK			314
#define MONS_HAND			315	// Frankenjulie's hand
#define MONS_BIGGHOST		316	// polterguy!
#define MONS_ELDER			317	// wind elder vampire
#define MONS_ELDER2			318	// earth elder vampire
#define MONS_ELDER3			319	// water elder vampire
#define MONS_ELDER4			320	// fire elder vampire
#define MONS_BONKULA		321
#define MONS_EVILIZER		322
#define MONS_EVILPUMP		323
#define MONS_SKULLY			324
#define MONS_ELDER5			325
#define MONS_HARRY			326
#define MONS_FROGURT		327
#define MONS_TORTREE		328
#define MONS_GASBAT			329
#define MONS_SWAMPDOG4		330
#define MONS_KINGTOOT		331
#define MONS_GHASTLY		332
#define MONS_BOMBIELORD		333
#define MONS_MUTANTPUMKIN	334
#define MONS_MINIFRANK		335
#define MONS_MECHABONK		336
#define MONS_ARCTICWOLF		337
#define MONS_GRABBYBAT		338
#define MONS_HOTDOG			339
#define MONS_STICKWITCH		340

//new stuff
#define MONS_PLAYWOLF		341
#define MONS_PLAYWIZARD		342
#define MONS_ICEPUMPKN		343 // implement
#define MONS_NIGHTVAMP		344
#define MONS_REVENGECLONE	345
#define MONS_THINGY4		346
#define MONS_GRUESML		347
#define MONS_PYGMYBOOMER	348
#define MONS_DEATHCAP		349
#define MONS_BURNBUSH		350 //implement
#define MONS_ATOMBOT		351 //implement
#define MONS_NOODLENODE		352
#define MONS_SANDMAN		353 //fix
#define MONS_DECOY			354
#define MONS_DEADEYE		355
#define MONS_PEABRAIN		356
#define MONS_ANUBIS			357
#define MONS_HOLOGRAM		358
#define MONS_CRABSHELL		359 //implement
#define MONS_PATROLZOID		360
#define MONS_METALPUMPKIN	361
#define MONS_KABOOMKIN		362
#define MONS_CHEF			363
#define MONS_KELVOZOID		364
#define MONS_MISSOZOID		365
#define MONS_FROZOMBIE		366
#define MONS_SUMUZOMBIE		367
#define MONS_HOTFOOT		368
#define MONS_EMPRESS		369
#define MONS_EMPRESSBODY	370
#define MONS_EMPRESSTAIL	371
#define MONS_LUNABOSS		372
#define MONS_FLYINGZOMBIE	373
#define MONS_GINGERSNAP		374
#define MONS_ATOMICSEAL		375
#define MONS_WETGHOST		376
#define MONS_MINIMATTIE  	377
#define MONS_MINIMATBODY  	378
#define MONS_MINIMATCLAW1 	379
#define MONS_MINIMATCLAW2 	380
#define MONS_MINIMATTAIL  	381
#define MONS_MALLOWMAN  	382
#define MONS_MALLOWMAN2  	383
//left out some stuff...
#define MONS_LIGHTSLIDE  	384
#define MONS_JUNK		  	385
//okay back to stuff
#define MONS_PYGMYFIRE  	386
#define MONS_ATOMICZOMB  	387
#define MONS_DEATHBURNER	388
#define MONS_LUNAMECHA		389
#define MONS_YOUNGLOONY		390 //playable loonyland loony
#define MONS_PATCH7			391 //flying pumpkin patch
#define MONS_GOURD			392
#define MONS_ESHKAH			393 //bonus boss
#define MONS_ROCKGUARD		394
#define MONS_STKSNOW		395
#define MONS_STKFISH  		396 //implement
#define MONS_YERFBOSS  		397
#define MONS_IMITATER  		398 //secret final boss
#define MONS_SPHINX2  		399
#define MONS_THING2  		400
#define MONS_YETIX  		401
//heres some stuff for the eldritch
#define MONS_ELDARM1 		402
#define MONS_ELDARM2		403
#define MONS_THING2TENT 	404
#define MONS_THING2TENTTIP	405
//back to new enemies
#define MONS_KONGOR2  		406
#define MONS_VAMPJR			407
#define MONS_CENTIBBODY 	408
#define MONS_CENTIBHEAD 	409
#define MONS_FREEZER	 	410
#define MONS_MULTISHROOM	411
#define MONS_MUMBLE3		412

#define MONS_PUSHERH		413
#define MONS_PUSHERV		414
#define MONS_HEALER		  	415 // medic shroom
#define MONS_BUDDYPUMPKIN	416 // buddy pumpkin
#define MONS_MINECARTEVIL	417 // evil minecart
#define MONS_BONERIDER		418 // bonehead riding cart
#define MONS_PYGMYRIDER		419 // pygmy riding cart
#define MONS_CHESTRIDER		420 // treasure c
#define MONS_SUPRBOMBIE		421

#define NUM_MONSTERS  422    // Can grow as needed
#define NUM_PROFILE_MONSTERS 211    // Do not change without redoing the profile format

// fake monster codes
#define MONS_GOODGUY	(-1)	// any goodguy
#define MONS_BADGUY		(-2)	// any badguy
#define MONS_ANYBODY	(-3)	// anybody
#define MONS_PLAYER		(-6)	// the player
#define MONS_NONPLAYER	(-5)	// anybody but the player
#define MONS_TAGGED		(-4)	// the tagged monster
#define MONS_BOSS		(-7)	// the boss

// the animations
#define ANIM_IDLE	0
#define ANIM_MOVE	1
#define ANIM_ATTACK 2
#define ANIM_DIE	3
#define ANIM_A1		4
#define ANIM_A2		5
#define ANIM_A3		6
#define ANIM_A4		7
#define ANIM_A5		8
#define ANIM_A6		9
#define NUM_ANIMS	10

#define ANIM_LENGTH	64

// flags
#define MF_FLYING	 1
#define MF_WATERWALK 2
#define MF_ONEFACE	 4
#define MF_ENEMYWALK 8	// other enemies can stomp all over this one (but not Bouapha)
#define MF_NOMOVE	 16 // doesn't move when hit
#define MF_AQUATIC	 32	// can ONLY move on water/lava, not land
#define MF_INVINCIBLE 64 // totally invulnerable to harm
#define MF_SPRITEBOX 128 // use the sprite's rect for collision checks instead of standard size-box method
#define MF_FACECMD	 256 // this monster's "facing" value should just be added to the sprite number,
						 // it's calculated by his AI (only useful for MF_ONEFACE monsters)
#define MF_NOGRAV	 512
#define MF_FREEWALK	 1024 // Bouapha can walk right through it
#define MF_WALLWALK	 2048 // walk through walls
#define MF_NOSHADOW	 4096 // doesn't cast a shadow
#define MF_GHOST	 8192 // draw using ghost draw
#define MF_NOHIT	 16384	// bullets pass through it
#define MF_GLOW		 32768 // draw using glow draw
#define MF_IMMOBILE	(1<<16) // cannot move!

// themes
#define MT_NONE		(0)
#define MT_GOOD		(1<<0)
#define MT_EVIL		(1<<1)
#define MT_FOREST	(1<<2)
#define MT_DESERT	(1<<3)
#define MT_ARCTIC	(1<<4)
#define MT_CAVES	(1<<5)
#define MT_SPACE	(1<<6)
#define MT_MANSION	(1<<7)
#define MT_ISLAND	(1<<8)
#define MT_URBAN 	(1<<9)
#define MT_UNDERSEA	(1<<10)
#define MT_ASYLUM	(1<<11)
#define MT_TRAP		(1<<12)
#define MT_HUMAN	(1<<13)
#define MT_ANIMAL	(1<<14)
#define MT_VEGGIE 	(1<<15)
#define MT_PUMPKIN  (1<<16)
#define MT_ZOMBIE	(1<<17)
#define MT_SKELETON	(1<<18)
#define MT_VAMPIRE	(1<<19)
#define MT_ZOID		(1<<20)
#define MT_ALIEN	(1<<21)
#define MT_GENERATE	(1<<22)
#define MT_VEHICLE	(1<<23)
#define MT_FLYING	(1<<24)
#define MT_AQUATIC	(1<<25)
#define MT_SPIDER	(1<<26)
#define MT_BITS		(1<<27)
#define MT_BOSS		(1<<28)
#define MT_MINIBOSS	(1<<29)
#define MT_WACKY	(1<<30)
#define NUM_MONSTHEMES	(31)
// themes/groups
#define MG_ALL		(1<<0)
#define MG_DRL		(1<<1)
#define MG_EXPANDO	(1<<2)
#define MG_FUN		(1<<3)
#define MG_SUPREME	(1<<4)
#define MG_MYSTIC	(1<<5)
#define MG_HOLLOW	(1<<6)
#define MG_LOONY	(1<<7)
#define MG_NEW		(1<<8)
// themes/colors
#define MC_ALL		(1<<0)
#define MC_GREY		(1<<1)
#define MC_GREEN	(1<<2)
#define MC_BROWN	(1<<3)
#define MC_BLUE		(1<<4)
#define MC_RED		(1<<5)
#define MC_YELLOW	(1<<6)
#define MC_VIOLET	(1<<7)
#define MC_AQUA 	(1<<8)
#define MC_GLOW 	(1<<9)

typedef void (*Monster_AIFunc)(Guy *,Map *,world_t *,Guy *);

typedef struct monsterType_t
{
	char name[32];
	byte fromCol,toCol;
	char brtChg;
	Monster_AIFunc AI;
	byte size;
	byte framesPerDir;
	word hp;
	word points;
	char sprName[32];
	sprite_set_t *spr;
	word flags;
	dword theme;
	dword group;
	dword color;
	byte anim[NUM_ANIMS][ANIM_LENGTH];
} monsterType_t;

void InitMonsters(void);
void ExitMonsters(void);

void PurgeMonsterSprites(void);

monsterType_t *GetMonsterType(dword type);

void ChangeOffColor(dword type,byte from,byte to);
byte MonsterSize(dword type);
byte *MonsterAnim(dword type,byte anim);
word MonsterFlags(dword type,byte aiType);
dword MonsterTheme(dword type);
dword MonsterGroup(dword type);
dword MonsterColor(dword type);
void SetMonsterFlags(dword type,word flags);
byte MonsterFrames(dword type);
byte MonsterFrames(dword type,byte aiType);
word MonsterHP(dword type);
word MonsterPoints(dword type);
char *MonsterName(short type);
void MonsterDraw(int x,int y,int z,dword type,dword aiType,byte seq,byte frm,byte facing,char bright,byte ouch,byte poison,byte frozen,byte weak,byte strong,byte ignited,byte confuse,byte special,sprite_set_t* set);
void InstaRenderMonster(int x,int y,dword type,char bright,MGLDraw *mgl);
int InstaRenderMonsterAnimated(int x,int y,dword type,char bright,MGLDraw *mgl);
void InstaRenderMonsterAnimated2(int x,int y,dword type,char bright,int tm,MGLDraw *mgl);
int InstaRenderScannedMonster(int x,int y,dword type,char bright,MGLDraw *mgl);
sprite_t *GetMonsterSprite(dword type,byte seq,byte frm,byte facing);
int RangeToTarget(Guy *me,Guy *goodguy);

// ai functions for each monster type
void AI_Bonehead(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Bat(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Spider(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_BigSpider(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Zombie(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_EggSac(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MamaSpider(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Pygmy(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Serpent(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Ginger(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Pumpkin(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_BabyThing(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Moss(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MossGrande(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Magmazoid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Shroom(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Mush(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_SuperZombie(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_StickMan(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_BabySeal(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Isozoid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Snowguy(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Penguin(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Zomboni(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Geozoid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Mumble(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Djinni(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MagicLamp(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Cactus(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Roller(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Lich(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_DustDevil(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MechaBouapha(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Freakazoid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_CentiBody(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_CentiHead(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Wacko(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Boiler(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_GreatPumpkin(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Ultrazoid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Santa(Guy *me,Map *map,world_t *world,Guy *goodguy);

	//Matilda's AIs
void AI_MattieBrain(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MattieSkullOrHead(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MattieClaw(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MattieTail(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MattieBody(Guy *me,Map *map,world_t *world,Guy *goodguy);
	// The Thing's AIs
void AI_TheThing(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_ThingTentacle(Guy *me,Map *map,world_t *world,Guy *goodguy);
	// Sven & Bjorn's AI
void AI_Yeti(Guy *me,Map *map,world_t *world,Guy *goodguy);
	// Sphinxter's AIs
void AI_SphinxArm(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Sphinx(Guy *me,Map *map,world_t *world,Guy *goodguy);
	// Asylum bosses' AIs
void AI_DrLunatic(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_SDZL(Guy *me,Map *map,world_t *world,Guy *goodguy);

// expansion monsters
void AI_MineCart(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Raft(Guy *me,Map *map,world_t *world,Guy *goodguy);

void AI_Vampire(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Coffin(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Ghost(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Burner(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Lefty(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Pygmy2(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Pygmy3(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Pumpkinstein(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Knight(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Triceroid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Countess(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_AlienEgg(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_BabyAlien(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Alien(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Robopumpkin(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_ShockTrooper(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Minibot(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Meaniebot(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Robofactory(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Turret(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Bunny(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Kongor(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Squash(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_UltraPygmy(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_LoonyShip(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_LoonyGun(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_LoonyBot(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_LoonyCore(Guy *me,Map *map,world_t *world,Guy *goodguy);

// fun pack monsters
void AI_GoodTurret(Guy *me,Map *map,world_t *world,Guy *goodguy);

void AI_Mattie2Brain(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Mattie2SkullOrHead(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Mattie2Tail(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Mattie2Body(Guy *me,Map *map,world_t *world,Guy *goodguy);

void AI_BuddyBunny(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_PygmyQueen(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Jalapeno(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Generator(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Shark(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MadBug(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Wizard(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_EvilClone(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Bob(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MultiMoss(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MossRapido(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Snowball(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Snowblower(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_XenoMama(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Gnome(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Nobody(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_BigPumpkin(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_RockTrooper(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_CrazyBone(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Creepazoid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Lazybones(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_StareyBat(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Madman(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_PygmyAcct(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Crazypants(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Yerfdog(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_EliteSeal(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Cone(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_PygmyDiver(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Puffyfish(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Yeti2(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Spikey(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Bombot(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_FollowBunny(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Autozoid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Chest(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Jellofish(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_KingCone(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Mailboxer(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Piranha(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Yugo(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Punkin(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_SeaCuke(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Sneaky2(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Starfish(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Underzoid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_WetSeal(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_WetWilly(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_CrabPuff(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Patch(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Patty(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Dozer(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MutantZombie(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Whatsit(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_ShockGhost(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Mechazoid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Sucker(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Blower(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Beetle(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Scarab(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Frostbiter(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_UnderMagmazoid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_ParkedCar(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Traffic(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_ShroomLord(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_BigBat(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_HugeBat(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Werewolf(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Patrol(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Weatherman(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_HunterEgg(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_PunkBunny(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Skeleton(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Microzoid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_HotShroom(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MiniWacko(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_JackFrost(Guy *me,Map *map,world_t *world,Guy *goodguy);

//kid mystic
void AI_Eyeguy(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Peeper(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Tower(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Pinkeye(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Lookeyloo(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Golem(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Microfish(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_BobbyKhan(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Crazybush(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Octopus(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Incabot(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Incagen(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Incagold(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Incaboss(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Incatongue(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Incatongue(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Trapazoid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_PeepBomb(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Whackazoid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Slug(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Snail(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Ball(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_OctoBoss(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_OctoTentacle(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Rammy(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Hoppy(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Gruffy(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_StickShroom(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_StickSpider(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_StickCorpse(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_StickBat(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Dancer(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Log(Guy *me,Map *map,world_t *world,Guy *goodguy);

// hollow
void AI_Frog(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MadCow(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MadCow2(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_PumpkinFly(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Reflection(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Goodbone(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Darkness(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Spark(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_LightSwitch(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_LightBlaster(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_LightSlide(Guy *me,Map *map,world_t *world,Guy *goodguy); //broken
void AI_BigHead(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_BigHead2(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_BigHeadBod(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_ScaredyBat(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Tree(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Horseradish(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Sparky(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Grue(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Spook(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Speedy(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_IronSkull(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Spatula(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_BatGuard(Guy *me,Map *map,world_t *world,Guy *goodguy);

//loonyland
void AI_Villager(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Bat2(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Mummy(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_EvilTree(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Larry(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Swampdog(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_LLGhost(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_StickWitch(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Frankenjulie(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Hand(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_WindElder(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_EarthElder(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_WaterElder(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_FireElder(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_SummonElder(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Bonkula(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Evilizer(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_EvilPump(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Polterguy(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Ghastly(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Junk(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Harry(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MiniFrankenjulie(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MechaBonkula(Guy *me,Map *map,world_t *world,Guy *goodguy);

//other
void AI_CloneArmor(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_NightTerror(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_PygmyBoomer(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_ScreamQueen(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Firedancer(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_GrueJr(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_PatrolZoid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Decoy(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Hologram(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Gingersnap(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Hotfoot(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Crabshell(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Noodlenode(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Galvigourd(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_XenoEmpressHead(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_XenoEmpressBody(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_XenoEmpressTail(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Lunachick(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Minitilda(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MarshmallowMan(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_RollerKiller(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_FlyingPatch(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_WinterGourd(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MultiShroom(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_StickSnowman(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_StickFish(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Yerfboss(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MechaLuna(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_ChefLiem(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_CountEshkah(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Shapeshifter(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_SuperMutantZombie(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_SuperBombie(Guy *me,Map *map,world_t *world,Guy *goodguy);

//The upgraded
void AI_Sphinx2(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Thing2(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Yeti3(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_SDZL2(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Kongor2(Guy *me,Map *map,world_t *world,Guy *goodguy);

//more new stuff
void AI_Pusher(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MinecartBad(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Deadeye(Guy *me,Map *map,world_t *world,Guy *goodguy);

#endif

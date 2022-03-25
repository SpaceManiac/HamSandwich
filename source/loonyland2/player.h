#ifndef PLAYER_H
#define PLAYER_H

#include "jamultypes.h"
#include "guy.h"
#include "control.h"
#include "game.h"
#include "display.h"
#include "bullet.h"
#include "world.h"
#include "intface.h"
#include "equip.h"
#include "talents.h"
#include "shop.h"

#define MAX_MONEY	99999
#define COMBO_TIME	30*2

#define MAX_LOG	(30)
#define LOG_LEN	(64)

// initializing constants (pass to InitPlayer)
#define INIT_GAME  2
#define INIT_WORLD 1
#define INIT_LEVEL 0

// var constants to keep things clear
	// quests
#define VAR_QUESTASSIGN 0	// 0-49 = quest (N) has been assigned
#define VAR_QUESTDONE	50	// 50-99 = quest (N) is complete
	// not quests
#define VAR_PROVEKILLS	100	// # of kills you've gotten while on a Prove Yourself quest
#define VAR_PRESENT		101	// 387-485 = have you collected present (N-387)?

#define VAR_MAGICHAT	200	// 200=killed magic hat
#define VAR_STONEROSE	201	// 201=killed stone rose
#define VAR_VISITED		202	// 202-216=have been to different places
#define VAR_WIFEUNLOCK	217	// have unlocked Bob's wife
#define VAR_FACTORY		218	// factory destroyed
#define VAR_STINKYSOCK	219	// killed stinky sock boss (also exists as a normal guy, later on, either one works)
#define VAR_TREECHOP	220	// 220-221 = the two choppable trees have been chopped
#define VAR_RATS		222	 // how many rats remain in Mickey's cellar.  If 0, you can cash in the quest

	// temp variables
#define VAR_TEMP		240	// 240-255 = temporary variables - cleared when leaving a level
#define VAR_CHEST		256	// 256-305 = treasure chests opened
#define VAR_KLONKDEAD	306	// 306 = klonk has been killed
#define VAR_TOYSDEAD	307	// 307 = toys killed for mickey's quest
#define VAR_BEARSDEAD	308	// 308 = bears killed for hank's quest
#define VAR_SNUGGLY		309	// 309 = 0 for Onion side, 1 for Snuggly side
#define VAR_CHAMPION	310	// 310 = beat the champion of the arena
#define VAR_GLASSES		311	// 311 = have gotten the fancy glasses
#define VAR_SWORDS		312	// 312 = swords collected in arena collection level
#define VAR_ROPED		313	// 313-320 = rope bridges have been built (313+tag number)
#define VAR_RATBOSS		321	// 321 = ratatouille dead
#define VAR_ONIONCHAT	322	// 322=have had the chat with onion to decide
#define VAR_FACTORY1	323	// 323=have cleared first floor of factory
#define VAR_BUSHDEAD	324	// 324=killed burning bush
#define VAR_TRIGUNDEAD	325	// 325=trigun is dead
#define VAR_SHAKEDOWN	326	// 326-329=merchants have been shaken down (the 0th one is also used by the Snugglies for their quest)
#define VAR_SUPPLIES	330	// 330=supply base destroyed
#define VAR_RUMBLE		331	// 331=have won the rumble
#define VAR_PRISONBREAK 332	// 332=have broken out of prison
#define VAR_JUNKDEAD	333	// 333=have killed the junksmith
#define VAR_HIGHWAY		334	// 334=have successfully done the highway mission (snuggly side)
#define VAR_CLOCKBOT	335	// 335-349=how many of each item are used in your clockwork bot
#define VAR_CLOCKLIFE	350	// 350=clockwork bot's current health for traipsing between areas
#define VAR_GURUTALENT	351	// 351=which talent you are currently talking to the guru about
#define VAR_BOOKGIVEN	352	// 352=have given the library book to historian
#define VAR_BARRIER		353	// 353-357=have opened the barriers of the same tag
#define VAR_GOURDDEAD	358	// 358=killed gourdzilla
#define VAR_DEFILE		359	// 359-360=bodzhas left alive on the two sides of shroom forest
#define VAR_YOUTH		361	// 361=Shrumford has youngified
#define VAR_BOKFOUND	362	// 362-371=bokbok picked up
#define VAR_BOKGIVEN	372	// 372=number of found bokboks you've delivered
#define VAR_FURNACE		373	// 373=furnace lit
#define VAR_JUSTHEF		374	// 374=used by the justhef axe as a timer
#define VAR_TITANIC		375	// 375-384=titanic titans have been slain
#define VAR_CAPTAIN2	385	// 385=1, captain has arrived at furnace, 2=is active, 3=is dead
#define VAR_TIP			386	// 386-410= tips have been shown
#define VAR_FROSTGATE	411	// 411= has frostburn front door been smashed
#define VAR_PUPPETLIFE  412	// 412-416=current life of helper puppets
#define VAR_BARONDEAD	417	// the baron has been beaten
#define VAR_SUPERTITAN	418	// melty titan has been beaten
#define VAR_SWITCH		419	// 419-422=have flipped the switches in Frostburn
#define VAR_TAMED		423	// 423=have tamed a mimic for magicsmith (bit 1 and 2 for hers and yours respectively)
#define VAR_SWITCH2		424	// 424-427=castle floor 2 switches
#define VAR_SPELLSET	428	// 428-432=which spell is currently in place in each of the 5 spots of spell menu
#define VAR_BIRDCHAT	433	// the bird has spoken to you
#define VAR_MADCAP		434	// in madcap mode
#define VAR_MADGEM		435	// 435-445=how many of each type of madcap gem you have
#define VAR_MODIFIER	446	// 446-449=the three modifiers you are using
#define VAR_CPOINTS		450	// 450=crystal points on hand
#define VAR_PARRYCLOCK	451	// 451=how long until you can parry again
#define VAR_CLOCKLIFE2	452	// 452=other half of clocklife var, since a byte won't do it
#define VAR_CONTROLS	453	// 453-472=control settings for this profile
#define VAR_SOUND		483	// 483=player sound volume
#define VAR_MUSIC		484	// 484=player music volume
#define VAR_COMMENTARY	485	// 485=commentary is on... only in collector's edition!
#define VAR_KILLCOUNTS	486	// 486-511=kill counters (two bytes each)
#define VAR_ARENAWINS	512	// 512=arena wins
#define VAR_DEATHS		513	// 513=deaths
#define VAR_FLAMEPAID	514	// 514=have paid for the flame skill this attack
#define VAR_KLONKSMASH	515	// 515=how many trees klonk has smashed

#define PLAYER_SKILLS	(50)	// the most skills the player can have.  Presumably the map will be setup appropriately
#define MAX_LEVEL		(50)	// the highest level you can level up to

// special moves
#define SM_NONE		(0)
#define SM_NOVA		(1)
#define SM_WHIRL	(2)
#define SM_TURRET	(3)

// places you can visit (added to VAR_VISITED)
#define PLC_EISBURG	(0)
#define PLC_ARENA	(1)
#define PLC_BASKER	(2)
#define PLC_CLIFFS	(3)
#define PLC_HIGHWAY	(4)
#define PLC_GORGE	(5)

// kill count numbers
#define KC_GHOST	(0)
#define KC_ICE		(2)
#define KC_BEAR		(4)
#define KC_MICE		(6)
#define KC_BODZHA	(8)
#define KC_PUPPET	(10)
#define KC_MONKEY	(12)
#define KC_STONE	(14)
#define KC_SHROOM	(16)
#define KC_SOLDIER	(18)
#define KC_PLANT	(20)
#define KC_ONION	(22)
#define KC_BUNNY	(24)

#pragma pack(push, 1)
typedef struct player_t
{
	char profile[16];
	int money;
	byte worldNum;
	byte levelNum;
	int  boredom;

	word hearts;
	word maxHearts;
	//byte hearts;
	//byte maxHearts;
	//byte firePower;
	//byte fireRate;
	byte fireFlags;
	byte fireRange;

	byte reload;
	byte pushPower;	// for pushing pushy blocks
	byte chatClock;
	byte saveClock;
	byte stone;

	int  invinc;
	int  destx,desty;	// where the player should be put, used between levels, and for
						// saved games
	byte speed;	// accelerated

	byte difficulty;
	dword monsterPoints;
	dword batLevel;
	int  monsType;
	word startHearts;
	dword cheatsOn;		// flags for which whole-game-only cheats are on in this game
	dword xtraVar;		// an extra variable for some characters
	byte crystalBall;
	byte xtraByte;		// an extra byte for some characters
	byte clockRunning;	// if the time limit clock should be going

	dword timeLimit;	// time passed for certain short-term timed things, like boss-killing
	// end-game stats
	dword timeToFinish;
	dword bestCombo;
	dword shotsFired;
	dword specialShotsFired;
	dword shouldMonsters;
	dword gemsGotten;
	dword hitsTaken;
	byte  rank;
	// vars
	byte  var[982];

	char addonName[12];
	word  arenaHearts,arenaMagic,arenaStamina;
	dword playClock;
	dword potionsDrunk;
	dword kills;
	dword herbsPicked;
	dword holesDug;
	dword damageDone;

	// new stuff, don't remove
	dword xp,xpNeed;
	byte level;
	byte skillPts;

	byte damage;	// the sum of your axe damage and any boosts from skills
	byte axeSpeed;	// the sum of your axe speed and skill boosts
	byte axeMode;	// 0=melee, 1=throw
	byte armor;		// defense, from parka plus boosts from skills

	byte magic,maxMagic,startMagic;
	byte stamina,maxStamina;	// maxStamina is always your current health, plus a bonus from any stamina-boosting skills (which just add to the percent of your original health you can count)
	byte staminaClock,magicClock;

	equip_t axe;
	equip_t parka;
	equip_t amulet;

	byte lens[2];

	equip_t items[MAX_EQUIP];
	equip_t shopInv[SHOP_MAX][SHOP_SIZE];

	byte skillHave[PLAYER_SKILLS];
	byte skillLvl[PLAYER_SKILLS];
	word skillClock[PLAYER_SKILLS];

	word talentPts[MAX_TALENTS];	// how many points you have toward the next level of the talent
	byte talentLevel[MAX_TALENTS];

	byte parry;	// while this is non-zero, anything that would hit you is blocked, and bullets are reflected
	byte spclMove;	// what special move you're doing now
	byte whirlClock;
	byte spell;
	byte lastTown;	// when you load a saved game, you appear in the last town you visited
	// timers for various spells
	word shockClock;
	word berserkClock;
	word parkaClock;	// timer for parka's magic power if needed
	word amuletClock;	// timer for amulet's magic power if needed

	word potionClock;
	byte potionType;
	byte potionPower;

	// message log
	char log[MAX_LOG][LOG_LEN];
	byte logBright,logTime;

	// arena stuff
	byte arenaLvl;	// which match is it?
	word arenaTime;
	byte arenaWave;	// which wave you are on if multiple
	word arenaSpawn;
} player_t;
#pragma pack(pop)

extern player_t player;

void InitPlayer(byte initWhat,byte world,byte level);
void ExitPlayer(void);
void PlayerControlMe(Guy *me,Map *map,mapTile_t *mapTile,world_t *world);

void DoPlayerFacing(byte c,Guy *me);
byte PlayerGetItem(byte itm,int val1,int val2);
void PlayerWinLevel(byte w,byte l,byte isSecret);
void SetPlayerHP(int hp);
void SetPlayerGlow(byte v);
byte GetPlayerGlow(void);
byte PlayerPushMore(void);
void SetTportClock(byte tp);
byte GetTportClock(void);
float CalcPercent(player_t *p);
void PlayerSetVar(int v,int val);
void PlayerClearTempVars(void);
void PlayerGetMonsterPoints(int level);
void PlayerGetBossPoints(int level);

void PlayerGetExactXP(int amt);

void PlayerCalcStats(void);
void PlayerBackToLastTown(byte *mapNum);
void Dig(Map *map,int x,int y);
void PlayerGetMoney(int amt);

void MadcapPlayer(void);
void PlayerAddKill(byte type);

#endif

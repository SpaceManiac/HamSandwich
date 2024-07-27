#ifndef BULLET_H
#define BULLET_H

#include "winpch.h"
#include "jamulspr.h"
#include "map.h"
#include "world.h"
#include "display.h"
#include "cossin.h"
#include "sound.h"
#include "particle.h"

// bullet traits
#define BF_HITEVIL	(1)
#define BF_HITGOOD	(2)
#define BF_INVIS	(4)	// don't render it
#define BF_HITMANY	(8)	// when finding targets, don't stop at one, hit everyone you can touch
#define BF_NOREHIT	(16)	// mark the last guy hit so as not to hit the same guy twice in a row
#define BF_RANDHITDIR (32)	// bounce hit victims in a random direction instead of my own
#define BF_SHADOW	(64)
#define BF_GLOW		(128)
#define BF_ITEM		(256)	// rendering is determined extry special, since this can be any gettable item
#define BF_LIGHT	(512)	// makes light around itself (and is accordingly drawn darker than normal to even it out)
#define BF_FLOOR	(1024)	// render this at the shadow level - under everything else
#define BF_FREEZE	(2048)	// freezes enemies hit for 2 seconds
#define BF_STICK	(4096)	// stay on your owner, positionally
#define BF_PLAYERONLY (8192)	// only hits the player, can't hit anyone else, good or bad
#define BF_POISON	(16384)	// poisons target for 3 seconds
#define BF_GROUND	(32768)	// vanishes with a splash if it hits water (when on the ground), and slides down cliffs and sky, if on the ground, and collides like a monster, not a bullet

// hit effects
#define BH_NONE		(0)	// do nothing
#define BH_RNDBOUNCE (1)	// bounce in a random direction, V=amount of speed to keep, 255=all
#define BH_BOUNCE	(2)	// bounce in the appropriate way, V=speed to keep, 255=all
#define BH_BECOME	(3)	// vanish, replaced by bullet type V (v=0, no bullet)
#define BH_STOP		(4)	// stop moving
#define BH_SUMMON	(5)	// summon a monster!

// spawn methods
#define BS_NONE		(0)	// don't
#define BS_RANDOM	(1)	// in random directions
#define BS_SAMEDIR	(2)	// in same direction as me
#define BS_RANDDIR	(3)	// same direction as me, randomly adjusted a whee
#define BS_RADIUS	(4)	// randomly scattered over your hit radius
#define BS_CIRCLE	(5)	// in a ring, evenly spaced around you

#pragma pack(push, 1)
typedef struct bulletDef_t
{
	dword flags;
	short gravity;	// how much gravity to apply per frame

	word  damage;
	byte  size;		// hitting size
	short hitForce;	// how much it pushes a hit victim (in subpixels, so FIXAMT would push them 1 pixel)
	word  speed;	// movement per frame
	short dspeed;	// change in speed per frame
	short beginDZ;	// initial dz value
	word timer;		// how long it lasts before ending

	byte homing;	// homing power, 0-255

	word animSpeed;	// every 256 is 1 frame, so a value of 256 would be 1 frame changed per game frame, 512 would skip a frame, 128 would hold each for 2.
	word firstFrm,lastFrm;	// graphics
	byte facings;	// 0=don't draw, otherwise number=how many different facings there are.  say it's 4 and firstFrm=105.  Then 105-108 are the first facing gfx, 109-112 are the second facing, etc.
	// what happens if it dies without hitting anything
	byte endEffect;
	byte endValue;
	byte endPart;
	byte endSound;
	// what happens if it hits an enemy/friend
	byte hitEffect;
	byte hitValue;
	byte hitPart;
	byte hitSound;
	byte hitRate;	// after you hit somebody, how long until you may hit somebody again?
	// what happens if it hits the floor
	byte flrEffect;
	byte flrValue;
	byte flrPart;
	byte flrSound;
	byte flrFriction;	// actually anti-friction: 255 means keep all speed, 0 means lose all speed
	// and a wall
	byte walEffect;
	byte walValue;
	byte walPart;
	byte walSound;
	// spawning
	byte spawnDelay;	// 0=don't spawn, otherwise ticks until spawn
	byte spawnType;		// type of bullet to spawn
	byte spawnMethod;	// how to spawn, above
	byte spawnCount;	// number of them to spawn
	byte spawnRate;		// delay before spawning again (0=just do it once)
	short spawnDmg;		// damage of spawned projectiles
	// particles
	byte partDelay;
	byte partType;
	byte partRate;	// same features as spawning, but the type is an explosion type, covering how many and what direction by itself
} bulletDef_t;
static_assert(sizeof(bulletDef_t) == 55);
#pragma pack(pop)

#define BLT_NONE    0
#define BLT_AXE		1
#define BLT_ITEM	2	// special bullet type for items dropped by enemies.  Anim contains what item type it is, target and owner are used to define Equipment.
#define BLT_FLAME	3	// flame first being born
#define BLT_FLAME2	4	// flame sitting there, timer dependent on Flame skill
#define BLT_FLAME3	5	// flame sputtering out, harmless
#define BLT_STONESPIKE 6
#define BLT_NOVA	7
#define BLT_MINIAXE	8	// mini axes for triple skill
#define BLT_AXEBOOM	9	// explosion for air strike skill
#define BLT_BOOMAXE 10	// the special axe you throw for air-strike
#define BLT_AXESHARD 11	// shrapnel from Shatter skill
#define BLT_BONEBOOM 12	// explosion from Bone Bomb skill
#define BLT_FIREBALL 13	// basic fireball for enemies, 2 damage
#define BLT_FLAMEB  14	// flame same as before, but fixed length - 2 seconds
#define BLT_FLAMEB2 15
#define BLT_FLAMEB3 16
#define BLT_CRYO	17	// ice bolts from the Cryozoid
#define BLT_IGNITER 18	// the shot that causes ignition
#define BLT_ALIENBLD 19	// alien blood, from drinking the potion
#define BLT_KABOOM  20	// a generic explosion of fixed, pretty big, size for anyone's needs
#define BLT_WOODSHARD 21	// wood chunk for klonk smashing trees
#define BLT_DRAIN	22	// the ghosts that get sent to you by Drain
#define BLT_DRAINMASTER	23	// invisible bullet that tracks the drain spell's time and who is involved and all
#define BLT_PFIREBALL 24	// the player fireball - actually fire colored
#define BLT_SMALLBOOM 25	// a much smaller version of KABOOM
#define BLT_GRENADE	26		// energy grenades lobbed by some enemies
#define BLT_BRAMBLE1 27		// brambles that are growing
#define BLT_BRAMBLE2 28		// brambles sitting around, having their effect
#define BLT_BRAMBLE3 29		// brambles that are dying off
#define BLT_POISON	30		// enemy fireball, but it poisons
#define BLT_GALE1	31		// invisible Gale shot, tries to hit enemies to pin them with Gale2 projectiles
#define BLT_GALE2	32		// invisible Gale shot that is attached to a victim.  If it smashes him into a wall or guy, it hurts him
#define BLT_CRYSTALSHOCK 33	// electrozaps fired by power crystals
#define BLT_BERRY	34		// life berry
#define BLT_SPORE	35		// poisonous dandylion spores
#define BLT_STUN	36		// the stars you get for being stunned
#define BLT_CRITICAL 37		// splash for critical hit
#define BLT_FLAMETRAIL 38	// leaves a string of flames as it goes
#define BLT_BURNINATE 39
#define BLT_BURNFLAME 40	// regular flame, timer dependent on burninate skill
#define BLT_BURNFLAME2 41
#define BLT_BURNFLAME3 42
#define BLT_TORNADO 43
#define BLT_TORNADO2 44		// used by Bunnyboss in his spin for looks
#define BLT_SMOKE	45		// used to give the smoke particle something to reference
#define BLT_COLDWAVE 46
#define BLT_CLOCKBULLET 47	// player fireball, as used by clockbot (for different values)
#define BLT_CLOCKBOOM 48	// smallboom used by clockbot for exploding hits (because radius changes)
#define BLT_ROSESEED 49
#define BLT_EVILSPIKE 50	// stone spike for enemies
#define BLT_EVILSPIKE2 51	// faster stone spike for scary enemies
#define BLT_SHOCKORB 52		// for shock orb spell
#define BLT_ICECUBE 53		// ice cube falling from the sky
#define BLT_BLIZZARD 54		// the cloud that makes the ice cubes
#define BLT_HOMER	55		// big homing shots from Sock Gorillas
#define BLT_TURRET	56		// big bullet that doesn't home, for turrets
#define BLT_BLOCK	57
#define BLT_BLOCK2	58
#define BLT_BLOCK3	59
#define BLT_ICEMINE	60
#define BLT_CRYO2	61
#define BLT_BLOCK4	62	// blocks just flying out everywhere
#define BLT_SKELSHOT 63	// repeat of enemy fireball, for use by your skellies
#define BLT_ROCK	64
#define BLT_ROCK2	65	// for the arena, bounces off walls and such
#define BLT_PFIREBALL2 66
#define BLT_CLOCKBOOM2 67
#define BLT_BERRY2	68
#define BLT_BADBURNIN 69
#define BLT_TYPES	70

#define MAX_BULLETS 400

// the special fire flags for different powerups
#define FF_REVERSE 1
#define FF_REFLECT 2
#define FF_WPNLOCK 4	// when on, you can't switch special weapons
#define FF_HOMING  8
#define FF_GUIDED  16
#define	FF_TRIPLE  32
#define FF_HELPER  64	// if you have a helper bat or not
#define FF_HELPERHERE 128	// is the helper bat present?

#pragma pack(push, 1)
typedef struct bullet_t
{
	int x,y,z;
	int dx,dy,dz;
	short speed;
	word timer;
	int target;
	int owner;

	word anim;
	word frame;
	byte facing;
	byte type;
	byte bright;
	byte team;

	word damage;

	byte nohit;	// a timer until you are allowed to hit again
	int canthit;	// the ID of a guy you're not allowed to hit (initially set to the guy who fired it, later used to prevent rehits if needed)

	word pSpawnTimer;
	word bSpawnTimer;
	byte bounces,pierces;
} bullet_t;
static_assert(sizeof(bullet_t) == 57);
#pragma pack(pop)

extern bulletDef_t bulDef[BLT_TYPES];
extern byte bulletHittingType;

void InitBullets(void);
void ExitBullets(void);

void SetupAxeBullet(void);

void UpdateBullets(Map *map,world_t *world);
void RenderBullets(void);

bullet_t *FireBullet(int owner,int x,int y,int z,byte facing,byte type,word damage);
byte Bulletable(bullet_t *me,Map *map,int x,int y);
void BulletRanOut(bullet_t *me,Map *map,world_t *world);	// prototyped for interior use
void BulletSwap(int sx,int sy,int width,int height,int dx,int dy);
byte BulletCanGo(bullet_t *me,int xx,int yy,Map *map,byte size);
byte Intersect(int rx,int ry,int rx2,int ry2,int r2x,int r2y,int r2x2,int r2y2);
void DestroyBullets(int x,int y,byte team,int radius);
void SmashTrees(int owner,int x,int y,int radius,Map *map);
byte NotBeingDrained(int who);
void RenderBullet(bullet_t *me);
byte EatItem(int x,int y);

void LoadBullets(SDL_RWops *f);
void SaveBullets(SDL_RWops *f);

#endif

#include "bullet.h"
#include "guy.h"
#include "player.h"
#include "quest.h"
#include "options.h"
#include "skill.h"
#include "ch_loony.h"
#include "alchemy.h"
#include "leveldef.h"
#include "madcap.h"

bullet_t bullet[MAX_BULLETS];
sprite_set_t *bulletSpr;
byte bulControl;

byte bulletHittingType;

bulletDef_t bulDef[BLT_TYPES]={
	// 0 - none
	{0},
	// 1 - BLT_AXE
	{BF_HITEVIL|BF_NOREHIT|BF_SHADOW,	// flags
	 0,1,12,FIXAMT*3,FIXAMT*8,-1,0,30*2,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 256,0,9,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,PART_WHITEPOP,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_BECOME,0,PART_WHITEPOP,0,5,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_BECOME,0,PART_WHITEPOP,0,0,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_BECOME,0,PART_WHITEPOP,SND_RICOCHET,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 2 - BLT_ITEM
	{BF_HITGOOD|BF_ITEM|BF_HITMANY|BF_GROUND,	// flags
	 -FIXAMT/4,0,12,0,FIXAMT*3,-FIXAMT/16,FIXAMT*2,30*60,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 0,0,0,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,PART_WHITEPOP,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_BECOME,0,PART_WHITEPOP,0,0,		// effect,value,particle,sound, hitrate
	 // hit the floor effect
	 BH_RNDBOUNCE,128,0,0,128,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_RNDBOUNCE,64,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 30*2,PART_FLOATYPOP,10,				// delay,type,rate
	},
	// 3 - BLT_FLAME
	{BF_GLOW|BF_LIGHT|BF_GROUND,	// flags
	 0,0,0,0,0,0,0,12,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 128,33,38,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,BLT_FLAME2,0,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_NONE,0,PART_FIREPOP,0,10,		// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_NONE,0,0,0,128,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_RNDBOUNCE,255,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 4 - BLT_FLAME2
	{BF_HITEVIL|BF_HITMANY|BF_GLOW|BF_LIGHT|BF_GROUND,	// flags
	 0,1,16,0,0,0,0,30,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 128,39,43,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,BLT_FLAME3,0,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_NONE,0,PART_FIREPOP,0,20,		// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_NONE,0,0,0,128,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_RNDBOUNCE,255,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 5 - BLT_FLAME3
	{BF_GLOW|BF_LIGHT|BF_GROUND,	// flags
	 0,0,16,0,0,0,0,8,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 128,44,47,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,0,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_NONE,0,0,0,0,		// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_NONE,0,0,0,128,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_RNDBOUNCE,255,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 6 - BLT_STONESPIKE
	{BF_HITEVIL|BF_HITMANY|BF_NOREHIT|BF_GROUND,	// flags
	 0,5,14,FIXAMT*3,FIXAMT*22,-FIXAMT*22,0,44,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 128,10,32,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,PART_NONE,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_NONE,0,PART_NONE,0,60,		// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_NONE,0,0,0,128,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_BECOME,0,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 5,BLT_STONESPIKE,BS_RANDDIR,1,0,5,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 7 - BLT_NOVA
	{BF_GLOW|BF_FLOOR,	// flags
	 0,0,0,0,0,0,0,9,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 256,48,56,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,PART_NONE,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_NONE,0,PART_NONE,0,0,		// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_NONE,0,0,0,0,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_BECOME,0,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 8 - BLT_MINIAXE
	{BF_HITEVIL|BF_NOREHIT|BF_SHADOW,	// flags
	 0,1,6,5,FIXAMT*8,-1,0,30*2,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 256,322,331,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,PART_WHITEPOP,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_BECOME,0,PART_WHITEPOP,0,5,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_BECOME,0,PART_WHITEPOP,0,0,	// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_BECOME,0,PART_WHITEPOP,SND_RICOCHET,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 9 - BLT_AXEBOOM
	{BF_HITEVIL|BF_HITMANY|BF_GLOW|BF_RANDHITDIR,	// flags
	 -FIXAMT,1,16,FIXAMT*8,0,0,0,13,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 256,222,234,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,0,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_NONE,0,0,0,15,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_NONE,0,0,0,0,	// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_NONE,0,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 10 - BLT_BOOMAXE
	{BF_GLOW|BF_SHADOW,	// flags
	 0,0,0,0,FIXAMT*8,-1,-FIXAMT*5,30*2,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 384,0,9,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,BLT_AXEBOOM,0,SND_BOOM,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_NONE,0,0,0,0,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_BECOME,BLT_AXEBOOM,0,SND_BOOM,0,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_BECOME,BLT_AXEBOOM,0,SND_BOOM,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 3,PART_FIRE,3,				// delay,type,rate
	},
	// 11 - BLT_AXESHARD
	{BF_HITEVIL|BF_NOREHIT|BF_SHADOW,	// flags
	 0,1,8,FIXAMT*1,FIXAMT*12,0,0,15,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 128,276,276,16,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,PART_NONE,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_BECOME,0,PART_WHITEPOP,0,44,		// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_NONE,0,0,0,0,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_RNDBOUNCE,200,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 12 - BLT_BONEBOOM
	{BF_HITEVIL|BF_HITMANY|BF_GLOW|BF_RANDHITDIR,	// flags
	 -FIXAMT,1,16,FIXAMT*8,0,0,0,13,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 256,222,234,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,0,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_NONE,0,0,0,15,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_NONE,0,0,0,0,	// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_NONE,0,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 13 - BLT_FIREBALL
	{BF_HITEVIL|BF_NOREHIT|BF_SHADOW|BF_GLOW,	// flags
	 0,4,12,FIXAMT*2,FIXAMT*10,-1,0,30*2,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 256,171,174,8,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,PART_GREENPOP,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_BECOME,0,PART_GREENPOP,0,5,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_BECOME,0,PART_GREENPOP,0,0,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_BECOME,0,PART_GREENPOP,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 14 - BLT_FLAMEB
	{BF_GLOW|BF_LIGHT|BF_GROUND,	// flags
	 -FIXAMT,0,0,0,FIXAMT*8,-FIXAMT/8,0,12,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 128,33,38,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,BLT_FLAMEB2,0,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_NONE,0,PART_FIREPOP,0,10,		// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_NONE,0,0,0,128,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_RNDBOUNCE,255,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 15 - BLT_FLAMEB2
	{BF_HITEVIL|BF_HITMANY|BF_GLOW|BF_LIGHT|BF_GROUND,	// flags
	 0,1,16,0,0,0,0,30,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 128,39,43,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,BLT_FLAMEB3,0,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_NONE,0,PART_FIREPOP,0,20,		// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_NONE,0,0,0,128,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_RNDBOUNCE,255,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 16 - BLT_FLAMEB3
	{BF_GLOW|BF_LIGHT|BF_GROUND,	// flags
	 0,0,16,0,0,0,0,8,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 128,44,47,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,0,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_NONE,0,0,0,0,		// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_NONE,0,0,0,128,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_RNDBOUNCE,255,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 17 - BLT_CRYO
	{BF_HITEVIL|BF_NOREHIT|BF_SHADOW|BF_GLOW|BF_FREEZE,	// flags
	 -FIXAMT/4,2,12,FIXAMT*2,FIXAMT*10,-1,FIXAMT*2,40,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 256,244,247,8,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,PART_WHITEPOP,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_BECOME,0,PART_WHITEPOP,0,5,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_BECOME,0,PART_WHITEPOP,0,0,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_BECOME,0,PART_WHITEPOP,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 4,PART_WHITEPOP,4,				// delay,type,rate
	},
	// 18 - BLT_IGNITER
	{BF_HITEVIL|BF_NOREHIT|BF_SHADOW|BF_GLOW|BF_INVIS,	// flags
	 0,0,12,0,FIXAMT*14,0,0,30,8,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 256,200,203,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,PART_SPARKPOP,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_BECOME,0,PART_SPARKPOP,SND_IGNITE,5,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_BECOME,0,PART_SPARKPOP,0,0,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_BECOME,0,PART_SPARKPOP,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 1,PART_SPARKPOP,2,				// delay,type,rate
	},
	// 19 - BLT_ALIENBLD
	{BF_HITEVIL|BF_NOREHIT|BF_SHADOW|BF_INVIS,	// flags
	 -FIXAMT/2,2,12,FIXAMT,FIXAMT*6,-1,FIXAMT*2,30,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 256,0,9,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,PART_GREENPOP,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_BECOME,0,PART_GREENPOP,0,5,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_BECOME,0,PART_GREENPOP,0,0,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_BECOME,0,PART_GREENPOP,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 2,PART_GREENPOP,2,				// delay,type,rate
	},
	// 20 - BLT_KABOOM
	{BF_HITEVIL|BF_HITMANY|BF_GLOW|BF_RANDHITDIR,	// flags
	 -FIXAMT,1,128,FIXAMT*8,0,0,0,19,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 256,203,221,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,0,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_NONE,0,0,0,19,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_NONE,0,0,0,0,	// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_NONE,0,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 21 - BLT_WOODSHARD
	{BF_HITEVIL|BF_NOREHIT|BF_SHADOW,	// flags
	 0,1,8,FIXAMT*1,FIXAMT*10,0,0,60,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 128,276,276,16,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,PART_NONE,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_BECOME,0,PART_WHITEPOP,SND_PUPPETOUCH,44,		// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_NONE,0,0,0,0,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_BECOME,0,PART_WHITEPOP,SND_PUPPETOUCH,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 22 - BLT_DRAIN
	{BF_HITEVIL|BF_GLOW|BF_PLAYERONLY|BF_LIGHT,	// flags
	 0,0,20,FIXAMT*1,FIXAMT*5,FIXAMT/2,0,60,10,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 128,243,243,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,PART_NONE,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_BECOME,0,PART_NONE,0,0,		// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_NONE,0,0,0,0,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_NONE,0,PART_NONE,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 2,PART_GHOST,2,				// delay,type,rate
	},
	// 23 - BLT_DRAINMASTER
	{BF_STICK,	// flags
	 0,0,8,FIXAMT*1,FIXAMT*4,0,0,60,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 128,300,304,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,PART_NONE,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_BECOME,0,PART_WHITEPOP,SND_PUPPETOUCH,44,		// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_NONE,0,0,0,0,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_NONE,0,PART_NONE,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 24 - BLT_PFIREBALL
	{BF_HITEVIL|BF_NOREHIT|BF_SHADOW|BF_GLOW,	// flags
	 0,4,12,FIXAMT*2,FIXAMT*10,-1,0,30*2,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 256,123,128,8,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,PART_FIREPOP,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_BECOME,0,PART_FIREPOP,0,5,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_BECOME,0,PART_FIREPOP,0,0,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_BECOME,0,PART_FIREPOP,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 25 - BLT_SMALLBOOM
	{BF_HITEVIL|BF_HITMANY|BF_GLOW|BF_RANDHITDIR,	// flags
	 -FIXAMT,1,80,FIXAMT*8,0,0,0,13,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 256,222,234,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,0,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_NONE,0,0,0,15,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_NONE,0,0,0,0,	// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_NONE,0,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 26 - BLT_GRENADE
	{BF_GLOW|BF_SHADOW,	// flags
	 -FIXAMT/4,1,0,0,FIXAMT*5,0,FIXAMT*8,90,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 128,235,242,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,BLT_SMALLBOOM,0,SND_BOOM,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_NONE,0,0,0,15,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_BECOME,BLT_SMALLBOOM,0,SND_BOOM,0,	// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_STOP,0,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 27- BLT_BRAMBLE1
	{BF_FLOOR|BF_GROUND,	// flags
	 -FIXAMT,1,40,0,0,0,0,22,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 128,57,67,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,BLT_BRAMBLE2,0,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_NONE,0,0,0,15,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_NONE,0,0,0,0,	// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_NONE,0,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 28 - BLT_BRAMBLE2
	{BF_FLOOR|BF_HITEVIL|BF_HITMANY|BF_GROUND,	// flags
	 -FIXAMT,0,40,0,0,0,0,30,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 128,68,68,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,BLT_BRAMBLE3,0,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_NONE,0,0,0,29,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_NONE,0,0,0,0,	// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_NONE,0,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 29 - BLT_BRAMBLE3
	{BF_FLOOR|BF_GROUND,	// flags
	 -FIXAMT,1,40,0,0,0,0,12,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 128,69,74,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,BLT_NONE,0,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_NONE,0,0,0,15,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_NONE,0,0,0,0,	// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_NONE,0,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 30 - BLT_POISON
	{BF_HITEVIL|BF_NOREHIT|BF_SHADOW|BF_GLOW|BF_POISON,	// flags
	 0,4,12,FIXAMT*2,FIXAMT*10,-1,0,30*2,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 256,171,174,8,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,PART_GREENPOP,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_BECOME,0,PART_GREENPOP,0,5,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_BECOME,0,PART_GREENPOP,SND_POISON,0,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_BECOME,0,PART_GREENPOP,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 31 - BLT_GALE1
	{BF_HITEVIL|BF_HITMANY|BF_INVIS,	// flags
	 0,0,24,0,FIXAMT*22,0,0,12,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 256,0,0,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,0,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_NONE,0,0,0,5,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_NONE,0,0,0,0,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_BECOME,0,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 32 - BLT_GALE2
	{BF_INVIS,	// flags
	 0,0,1,0,0,FIXAMT,0,40,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 256,1,3,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,0,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_NONE,0,0,0,5,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_NONE,0,0,0,0,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_NONE,0,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 33 - BLT_CRYSTALSHOCK
	{BF_INVIS,	// flags
	 0,0,16,0,0,0,0,30,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 256,1,1,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,BLT_SMALLBOOM,0,SND_BOOM,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_NONE,0,0,0,5,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_NONE,0,0,0,0,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_NONE,0,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 5,PART_WHITEPOP,5,				// delay,type,rate
	},
	// 34 - BLT_BERRY
	{BF_HITEVIL|BF_HITGOOD|BF_NOREHIT|BF_SHADOW|BF_GROUND,	// flags
	 -FIXAMT/2,2,8,0,FIXAMT*6,-1,FIXAMT*6,30*5,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 256,313,313,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,PART_REDPOP,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_BECOME,0,PART_REDPOP,0,5,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_BOUNCE,0,0,0,48,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_BOUNCE,0,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 35 - BLT_SPORE
	{BF_HITEVIL|BF_NOREHIT|BF_INVIS|BF_HITMANY|BF_POISON,	// flags
	 -FIXAMT/4,2,14,0,FIXAMT*5,-1,FIXAMT,30,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 256,0,0,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,PART_SNOW,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_NONE,0,0,0,5,		// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_BECOME,0,PART_SNOW,0,0,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_BECOME,0,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 5,PART_SNOW,5,				// delay,type,rate
	},
	// 36 - BLT_STUN
	{BF_STICK,	// flags
	 0,0,0,0,0,0,0,32,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 256,292,299,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,PART_NONE,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_NONE,0,PART_NONE,0,0,		// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_NONE,0,0,0,0,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_BECOME,0,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 37 - BLT_CRITICAL
	{BF_GLOW,	// flags
	 0,0,0,0,0,0,0,20,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 64,300,304,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,PART_NONE,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_NONE,0,PART_NONE,0,0,		// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_NONE,0,0,0,0,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_BECOME,0,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 38 - BLT_FLAMETRAIL
	{BF_INVIS,	// flags
	 -FIXAMT/4,2,14,0,FIXAMT*5,-1,FIXAMT,30,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 256,0,0,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,0,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_NONE,0,0,0,5,		// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_BECOME,0,0,0,0,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_BECOME,0,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 15,BLT_FLAMEB,BS_RANDOM,1,15,10,	// delay,type,method,count,rate,damage
	 // spawn particles
	 10,PART_FIREPOP,10,				// delay,type,rate
	},
	// 39 - BLT_BURNINATE
	{BF_HITEVIL|BF_HITMANY|BF_NOREHIT|BF_GLOW,	// flags
	 0,1,30,FIXAMT*10,FIXAMT*6,0,0,30*4,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 128,75,80,8,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,BLT_KABOOM,0,SND_BOOM,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_BECOME,BLT_KABOOM,0,SND_BOOM,5,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_BECOME,0,PART_WHITEPOP,0,0,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_BECOME,BLT_KABOOM,0,SND_BOOM,	// effect,value,particle,sound
	 // spawn bullets
	 5,BLT_BURNFLAME,BS_RANDOM,1,5,1,	// delay,type,method,count,rate,damage
	 // spawn particles
	 10,PART_FIREPOP,10,				// delay,type,rate
	},
	// 40 - BLT_BURNFLAME
	{BF_GLOW|BF_LIGHT|BF_GROUND,	// flags
	 0,0,0,0,0,FIXAMT/2,FIXAMT/16,12,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 128,33,38,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,BLT_BURNFLAME2,0,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_NONE,0,PART_FIREPOP,0,10,		// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_NONE,0,0,0,128,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_RNDBOUNCE,255,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 41 - BLT_BURNFLAME2
	{BF_HITEVIL|BF_HITMANY|BF_GLOW|BF_LIGHT|BF_GROUND,	// flags
	 0,1,16,0,0,0,0,30,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 128,39,43,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,BLT_BURNFLAME3,0,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_NONE,0,PART_FIREPOP,0,20,		// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_NONE,0,0,0,128,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_RNDBOUNCE,255,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 42 - BLT_BURNFLAME3
	{BF_GLOW|BF_LIGHT|BF_GROUND,	// flags
	 0,0,16,0,0,0,0,8,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 128,44,47,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,0,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_NONE,0,0,0,0,		// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_NONE,0,0,0,128,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_RNDBOUNCE,255,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 43 - BLT_TORNADO
	{BF_GLOW,				// flags
	 0,0,0,0,FIXAMT*4,-FIXAMT/16,0,60,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 128,305,312,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,PART_SMOKE,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_NONE,0,0,0,0,		// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_NONE,0,0,0,0,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_RNDBOUNCE,255,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 4,PART_SMOKESINGLE,8,				// delay,type,rate
	},
	// 44 - BLT_TORNADO2
	{BF_GLOW|BF_STICK,				// flags
	 0,0,0,0,0,0,0,60,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 128,305,312,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,PART_SMOKE,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_NONE,0,0,0,0,		// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_NONE,0,0,0,0,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_RNDBOUNCE,255,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 4,PART_SMOKESINGLE,8,				// delay,type,rate
	},
	// 45 - BLT_SMOKE
	{BF_GLOW,				// flags
	 0,0,0,0,0,0,0,30,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 128,314,321,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,0,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_NONE,0,0,0,0,		// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_NONE,0,0,0,0,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_RNDBOUNCE,255,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 46 - BLT_COLDWAVE
	{BF_HITEVIL|BF_NOREHIT|BF_HITMANY|BF_SHADOW|BF_GLOW|BF_FREEZE,	// flags
	 0,2,16,FIXAMT*1,FIXAMT*7,-4,0,80,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 256,244,247,8,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,PART_WHITEPOP,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_NONE,0,PART_WHITEPOP,0,5,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_BECOME,0,PART_WHITEPOP,0,0,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_BECOME,0,PART_WHITEPOP,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 47 - BLT_CLOCKBULLET
	{BF_HITEVIL|BF_NOREHIT|BF_SHADOW|BF_GLOW,	// flags
	 0,4,12,FIXAMT*2,FIXAMT*10,-1,0,30*2,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 256,123,128,8,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,PART_FIREPOP,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_BECOME,0,PART_FIREPOP,0,5,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_BECOME,0,PART_FIREPOP,0,0,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_BECOME,0,PART_FIREPOP,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 48 - BLT_CLOCKBOOM
	{BF_HITEVIL|BF_HITMANY|BF_GLOW|BF_RANDHITDIR,	// flags
	 -FIXAMT,1,80,FIXAMT*8,0,0,0,13,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 256,222,234,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,0,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_NONE,0,0,0,15,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_NONE,0,0,0,0,	// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_NONE,0,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 49 - BLT_ROSESEED
	{BF_SHADOW|BF_LIGHT,	// flags
	 -FIXAMT/2,0,0,0,FIXAMT*5,0,FIXAMT*21,90,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 128,235,242,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_SUMMON,MONS_ROSE3,0,SND_PLANTSURFACE,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_NONE,0,0,0,15,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_SUMMON,MONS_ROSE3,0,SND_PLANTSURFACE,0,		// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_STOP,0,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 50 - BLT_EVILSPIKE
	{BF_HITEVIL|BF_HITMANY|BF_NOREHIT|BF_GROUND,	// flags
	 0,5,14,FIXAMT*3,FIXAMT*22,-FIXAMT*22,0,44,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 128,10,32,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,PART_NONE,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_NONE,0,PART_NONE,0,60,		// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_NONE,0,0,0,128,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_BECOME,0,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 5,BLT_EVILSPIKE,BS_RANDDIR,1,0,5,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 51 - BLT_EVILSPIKE2
	{BF_HITEVIL|BF_HITMANY|BF_NOREHIT|BF_GROUND,	// flags
	 0,5,14,FIXAMT*3,FIXAMT*22,-FIXAMT*22,0,22,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 256,10,32,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,PART_NONE,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_NONE,0,PART_NONE,0,60,		// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_NONE,0,0,0,128,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_BECOME,0,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 3,BLT_EVILSPIKE2,BS_RANDDIR,1,0,5,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 52 - BLT_SHOCKORB
	{BF_GLOW|BF_LIGHT,	// flags
	 0,1,0,0,FIXAMT*1,0,0,90,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 128,235,242,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,PART_WHITEPOP,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_NONE,0,0,0,15,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_NONE,0,0,0,0,	// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_BECOME,0,PART_WHITEPOP,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 53 - BLT_ICECUBE
	{BF_GLOW|BF_SHADOW|BF_HITEVIL|BF_FREEZE,	// flags
	 -FIXAMT,1,18,0,0,0,-FIXAMT,90,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 0,0,0,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,PART_WHITEPOP,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_BECOME,0,PART_WHITEPOP,SND_CUBEHIT,15,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_BECOME,0,PART_WHITEPOP,SND_CUBEHIT,0,	// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_BECOME,0,PART_WHITEPOP,SND_CUBEHIT,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 54 - BLT_BLIZZARD
	{BF_INVIS,	// flags
	 0,1,32,0,0,0,0,90,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 128,235,242,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,0,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_NONE,0,0,0,0,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_NONE,0,0,0,0,	// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_STOP,0,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 1,BLT_ICECUBE,BS_RADIUS,3,10,1,	// delay,type,method,count,rate,damage
	 // spawn particles
	 1,PART_CLOUD,1,				// delay,type,rate
	},
	// 55 - BLT_HOMER
	{BF_HITEVIL|BF_NOREHIT|BF_SHADOW|BF_GLOW,	// flags
	 0,4,18,FIXAMT*6,FIXAMT*6,0,0,30*3,64,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 128,235,242,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,BLT_SMALLBOOM,0,SND_BOOM,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_BECOME,BLT_SMALLBOOM,0,SND_BOOM,5,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_BECOME,0,PART_GREENPOP,0,0,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_BECOME,BLT_SMALLBOOM,0,SND_BOOM,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 56 - BLT_TURRET
	{BF_HITEVIL|BF_NOREHIT|BF_SHADOW,	// flags
	 0,4,18,FIXAMT*16,FIXAMT*10,0,0,30*3,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 128,332,332,8,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,PART_SMOKESINGLE,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_BECOME,0,PART_SMOKESINGLE,0,5,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_BECOME,0,PART_SMOKESINGLE,0,0,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_BECOME,0,PART_SMOKESINGLE,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 57 - BLT_BLOCK
	{BF_SHADOW,	// flags
	 FIXAMT,0,18,FIXAMT*16,0,0,0,10,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 128,340,344,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,BLT_BLOCK2,0,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_BECOME,0,0,0,0,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_NONE,0,0,0,0,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_NONE,0,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 58 - BLT_BLOCK2 - sit for a while in the air, pointing at player secretly
	{BF_SHADOW,	// flags
	 0,0,18,FIXAMT*16,0,0,0,45,128,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 128,344,344,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,BLT_BLOCK3,0,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_BECOME,0,0,0,0,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_NONE,0,0,0,0,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_NONE,0,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 59 - BLT_BLOCK3
	{BF_HITEVIL|BF_SHADOW,	// flags
	 0,1,18,FIXAMT*10,FIXAMT*10,0,0,120,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 256,344,349,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,PART_SMOKE,SND_STONEHIT,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_BECOME,0,PART_SMOKE,SND_STONEHIT,0,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_NONE,0,0,0,0,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_BECOME,0,PART_SMOKE,SND_STONEHIT,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 60 - BLT_ICEMINE
	{BF_HITEVIL|BF_SHADOW|BF_GLOW,	// flags
	 0,4,32,FIXAMT*6,0,0,0,30*3,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 128,235,242,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,0,SND_CRYSTALZAP,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_BECOME,BLT_SMALLBOOM,0,SND_BOOM,5,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_NONE,0,0,0,0,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_NONE,0,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 30*3-2,BLT_CRYO2,BS_CIRCLE,12,20,20,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 61 - BLT_CRYO2
	{BF_HITEVIL|BF_NOREHIT|BF_SHADOW|BF_GLOW|BF_FREEZE,	// flags
	 0,2,12,FIXAMT*2,FIXAMT*10,-1,0,40,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 256,244,247,8,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,PART_WHITEPOP,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_BECOME,0,PART_WHITEPOP,0,5,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_BECOME,0,PART_WHITEPOP,0,0,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_BECOME,0,PART_WHITEPOP,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 4,PART_WHITEPOP,4,				// delay,type,rate
	},
	// 62 - BLT_BLOCK4
	{BF_SHADOW,	// flags
	 -FIXAMT/2,1,18,FIXAMT*10,FIXAMT*4,0,FIXAMT*8,120,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 256,344,349,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,PART_SMOKESINGLE,SND_STONEHIT,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_BECOME,0,PART_SMOKESINGLE,SND_STONEHIT,0,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_BECOME,0,PART_SMOKESINGLE,0,0,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_RNDBOUNCE,200,PART_SMOKESINGLE,SND_STONEHIT,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 63 - BLT_SKELSHOT
	{BF_HITEVIL|BF_NOREHIT|BF_SHADOW|BF_GLOW,	// flags
	 0,4,12,FIXAMT*2,FIXAMT*10,-1,0,30*2,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 256,171,174,8,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,PART_GREENPOP,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_BECOME,0,PART_GREENPOP,0,5,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_BECOME,0,PART_GREENPOP,0,0,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_BECOME,0,PART_GREENPOP,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 64 - BLT_ROCK
	{BF_HITEVIL|BF_NOREHIT|BF_SHADOW,	// flags
	 -FIXAMT/2,4,22,FIXAMT*16,FIXAMT*7,0,-FIXAMT*5,30*5,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 256,350,355,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,PART_DIRT,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_BECOME,0,PART_DIRT,SND_BOOM,10,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_BOUNCE,255,PART_DIRT,SND_STONEHIT,255,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_BECOME,0,PART_DIRT,SND_BOOM,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 65 - BLT_ROCK2
	{BF_HITEVIL|BF_NOREHIT|BF_SHADOW,	// flags
	 -FIXAMT/2,4,22,FIXAMT*16,FIXAMT*7,0,-FIXAMT*5,30*3,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 256,350,355,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,PART_DIRT,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_BECOME,0,PART_DIRT,SND_BOOM,10,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_BOUNCE,255,PART_DIRT,SND_STONEHIT,255,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_BOUNCE,255,PART_DIRT,SND_STONEHIT,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 66 - BLT_PFIREBALL2
	{BF_HITEVIL|BF_NOREHIT|BF_SHADOW|BF_GLOW,	// flags
	 0,4,12,FIXAMT*2,FIXAMT*10,-1,0,30*2,5,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 256,123,128,8,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,PART_FIREPOP,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_BECOME,0,PART_FIREPOP,0,5,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_BECOME,0,PART_FIREPOP,0,0,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_BECOME,0,PART_FIREPOP,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 67 - BLT_CLOCKBOOM2
	{BF_HITEVIL|BF_HITMANY|BF_GLOW|BF_RANDHITDIR,	// flags
	 -FIXAMT,1,80,FIXAMT*8,0,0,0,13,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 256,222,234,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,0,0,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_NONE,0,0,0,15,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_NONE,0,0,0,0,	// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_NONE,0,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 68 - BLT_BERRY2
	{BF_SHADOW|BF_GROUND,	// flags
	 -FIXAMT/2,2,8,0,FIXAMT*6,-1,FIXAMT*6,30*2,0,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 256,313,313,1,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_SUMMON,MONS_FRUITPUNCH,PART_REDPOP,0,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_NONE,0,0,0,0,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_BOUNCE,0,0,0,48,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_BOUNCE,0,0,0,	// effect,value,particle,sound
	 // spawn bullets
	 0,0,BS_NONE,0,0,0,	// delay,type,method,count,rate,damage
	 // spawn particles
	 0,0,0,				// delay,type,rate
	},
	// 40 - BLT_BADBURNIN
	{BF_HITEVIL|BF_HITMANY|BF_NOREHIT|BF_GLOW,	// flags
	 0,1,30,FIXAMT*10,FIXAMT*6,0,0,30*4,6,	// gravity,damage,hit size,hitForce,speed,dspeed,beginDZ,timer,homing
	 128,75,80,8,			// anim speed, first frame, last frame, # of facings
	 // run out of time effect
	 BH_BECOME,BLT_KABOOM,0,SND_BOOM,		// effect,value,particle,sound
	 // hit an enemy effect
	 BH_BECOME,BLT_KABOOM,0,SND_BOOM,5,	// effect,value,particle,sound,hitrate
	 // hit the floor effect
	 BH_BECOME,0,PART_WHITEPOP,0,0,					// effect,value,particle,sound,friction
	 // hit a wall effect
	 BH_BECOME,BLT_KABOOM,0,SND_BOOM,	// effect,value,particle,sound
	 // spawn bullets
	 5,BLT_FLAMEB,BS_RANDOM,1,5,10,	// delay,type,method,count,rate,damage
	 // spawn particles
	 10,PART_FIREPOP,10,				// delay,type,rate
	},
};

static byte itmBright;
static char itmDBright;

void SetupAxeBullet(void)
{
	bulDef[BLT_AXE].damage=((((float)player.damage)/2.0f+0.5f));
	if(bulDef[BLT_AXE].damage<1)
		bulDef[BLT_AXE].damage=1;

	bulDef[BLT_AXE].speed=FIXAMT*7+(FIXAMT/2)*(word)SpecificSkillVal(0,SKILL_THROWING);
	bulDef[BLT_AXE].timer=20+(word)SpecificSkillVal(0,SKILL_THROWING)*10;
	bulDef[BLT_AXE].homing=(byte)SpecificSkillVal(0,SKILL_HOMING);
	bulDef[BLT_AXE].partDelay=CalcShockRate((byte)SpecificSkillVal(0,SKILL_STATIC));

	bulDef[BLT_BOOMAXE].damage=DamageAmt(SpecificSkillVal(0,SKILL_AIRSTRIKE),GOOD);
	bulDef[BLT_AXEBOOM].size=(byte)SpecificSkillVal(1,SKILL_AIRSTRIKE);

	bulDef[BLT_AXE].hitForce=3*FIXAMT+(short)TalentBonus(TLT_TOSSING);

	bulDef[BLT_AXESHARD].damage=DamageAmt(1,GOOD);
	if(SpecificSkillVal(0,SKILL_RICOCHET))
	{
		bulDef[BLT_AXE].walEffect=BH_BOUNCE;
		if(SpecificSkillVal(0,SKILL_RICOCHET)>10)
			bulDef[BLT_AXE].walValue=(byte)255;
		else
			bulDef[BLT_AXE].walValue=(byte)(255-100+SpecificSkillVal(0,SKILL_RICOCHET)*10);
	}
	else
	{
		bulDef[BLT_AXE].walEffect=BH_BECOME;
		bulDef[BLT_AXE].walValue=0;
	}
	// and mini axe
	float f;


	f=SpecificSkillVal(0,SKILL_TRIPLE);
	bulDef[BLT_MINIAXE].damage=(byte)((float)bulDef[BLT_AXE].damage*f);
	if(bulDef[BLT_MINIAXE].damage<1)
		bulDef[BLT_MINIAXE].damage=1;

	bulDef[BLT_MINIAXE].speed=(word)(FIXAMT*7+(float)((FIXAMT/2)*(word)SpecificSkillVal(0,SKILL_THROWING))*f);
	bulDef[BLT_MINIAXE].timer=(word)(20+(SpecificSkillVal(0,SKILL_THROWING)*10*f));
	bulDef[BLT_MINIAXE].homing=(byte)(SpecificSkillVal(0,SKILL_HOMING)*f);
	bulDef[BLT_MINIAXE].partDelay=CalcShockRate((byte)SpecificSkillValDiminished(0,SKILL_STATIC,f));

	bulDef[BLT_MINIAXE].hitForce=(short)((3*FIXAMT+TalentBonus(TLT_TOSSING))*f);

	if(SpecificSkillVal(0,SKILL_RICOCHET))
	{
		bulDef[BLT_MINIAXE].walEffect=BH_BOUNCE;
		if(SpecificSkillVal(0,SKILL_RICOCHET)>10)
			bulDef[BLT_MINIAXE].walValue=(byte)255;
		else
			bulDef[BLT_MINIAXE].walValue=(byte)(255-100+SpecificSkillVal(0,SKILL_RICOCHET)*10);
	}
	else
	{
		bulDef[BLT_MINIAXE].walEffect=BH_BECOME;
		bulDef[BLT_MINIAXE].walValue=0;
	}

	bulDef[BLT_BONEBOOM].size=(byte)SpecificSkillVal(1,SKILL_BONEBOMB);

	if(player.axe.magic==EM_WHISTLING)
	{
		bulDef[BLT_AXE].speed=bulDef[BLT_AXE].speed*3/2;
		bulDef[BLT_MINIAXE].speed=bulDef[BLT_MINIAXE].speed*3/2;
	}
	if(player.axe.magic==EM_FLAMING)
	{
		bulDef[BLT_AXE].spawnCount=1;
		bulDef[BLT_AXE].spawnDelay=10;
		bulDef[BLT_AXE].spawnType=BLT_PFIREBALL;
		bulDef[BLT_AXE].spawnMethod=BS_RANDOM;
		bulDef[BLT_AXE].spawnRate=20;
		bulDef[BLT_AXE].spawnDmg=DamageAmt(player.axe.value,GOOD)/4;
		if(bulDef[BLT_AXE].spawnDmg<1)
			bulDef[BLT_AXE].spawnDmg=1;
	}
	else if(player.axe.magic==EM_SCORCHING)
	{
		bulDef[BLT_AXE].spawnCount=1;
		bulDef[BLT_AXE].spawnDelay=4;
		bulDef[BLT_AXE].spawnType=BLT_PFIREBALL;
		bulDef[BLT_AXE].spawnMethod=BS_RANDOM;
		bulDef[BLT_AXE].spawnRate=7;
		bulDef[BLT_AXE].spawnDmg=DamageAmt(player.axe.value,GOOD)/4;
		if(bulDef[BLT_AXE].spawnDmg<1)
			bulDef[BLT_AXE].spawnDmg=1;
	}
	else
		bulDef[BLT_AXE].spawnDelay=0;

	bulDef[BLT_DRAINMASTER].timer=(int)SpecificSkillVal(1,SKILL_DRAIN);
	bulDef[BLT_BRAMBLE2].timer=(int)SpecificSkillVal(1,SKILL_BRAMBLES);

	bulDef[BLT_GALE2].speed=(word)SpecificSkillVal(0,SKILL_GALE);
	bulDef[BLT_GALE2].dspeed=(word)SpecificSkillVal(1,SKILL_GALE);

	bulDef[BLT_BURNFLAME2].timer=(int)SpecificSkillVal(1,SKILL_BURNINATE);
	bulDef[BLT_BURNINATE].damage=(int)(SpecificSkillVal(0,SKILL_BURNINATE)*(SpellDamageBoost(SC_FIRE))/100.0f);

	bulDef[BLT_TORNADO].timer=30*(int)SpecificSkillVal(1,SKILL_TORNADO);
	bulDef[BLT_COLDWAVE].damage=(int)(SpecificSkillVal(0,SKILL_COLDWAVE)*(SpellDamageBoost(SC_ICE))/100.0f);

	bulDef[BLT_SHOCKORB].timer=(int)SpecificSkillVal(1,SKILL_SHOCK);
	bulDef[BLT_SHOCKORB].partDelay=CalcShockRate(30);

	bulDef[BLT_BLIZZARD].size=(byte)SpecificSkillVal(1,SKILL_ICECUBE);
	bulDef[BLT_BLIZZARD].spawnDmg=(byte)((SpecificSkillLevel(SKILL_ICECUBE))*(SpellDamageBoost(SC_ICE))/100.0f);
	bulDef[BLT_BLIZZARD].timer=(word)(SpecificSkillVal(0,SKILL_ICECUBE)*30);
	bulDef[BLT_BLIZZARD].spawnRate=10-(SpecificSkillLevel(SKILL_ICECUBE)/2);
	// clockbot
	bulDef[BLT_CLOCKBULLET].hitForce=player.var[VAR_CLOCKBOT-IT_LOG+IT_STUFFING]*6;
	bulDef[BLT_CLOCKBOOM].size=20+player.var[VAR_CLOCKBOT-IT_LOG+IT_DIAMOND]*10;
}

void InitBullets(void)
{
	bulletSpr=new sprite_set_t("graphics/bullets.jsp");

	memset(bullet,0,MAX_BULLETS*sizeof(bullet_t));
	itmBright=0;
	itmDBright=1;
}

void ExitBullets(void)
{
	delete bulletSpr;
}

byte Bulletable(bullet_t *me,Map *map,int x,int y)
{
	if(bulDef[me->type].flags&BF_GROUND)
	{
		if(map->map[x+y*map->width].wall ||
			(curWorld.terrain[map->map[x+y*map->width].floor].flags&(TF_SOLID|TF_NOBULLET)) ||
			((ItemFlags(map->map[x+y*map->width].item)&IF_OBSTACLE) && (map->map[x+y*map->width].item<IT_CHEST1 || map->map[x+y*map->width].item>IT_CHEST5)))
			return 0;
	}
	else
	{
		if(me->z>FIXAMT*80)	// high enough to travel over anything but a wall, or of course a "nobullet" zone
		{
			if(map->map[x+y*map->width].wall ||
			(curWorld.terrain[map->map[x+y*map->width].floor].flags&TF_NOBULLET))
				return 0;
		}
		else if(map->map[x+y*map->width].wall ||
			(ItemFlags(map->map[x+y*map->width].item)&IF_TALL) ||
			(curWorld.terrain[map->map[x+y*map->width].floor].flags&TF_NOBULLET))
		{
			return 0;
		}
	}

	return 1;
}

byte BulletCanGo(bullet_t *me,int xx,int yy,Map *map,byte size)
{
	byte result;
	int mapx,mapy,mapx1,mapx2,mapy1,mapy2;

	if(bulDef[me->type].walEffect==BH_NONE)
		return 1;

	xx>>=FIXSHIFT;
	yy>>=FIXSHIFT;

	mapx=xx/TILE_WIDTH;
	mapy=yy/TILE_HEIGHT;
	mapx1=(xx-size)/TILE_WIDTH;
	mapy1=(yy-size)/TILE_HEIGHT;
	mapx2=(xx+size)/TILE_WIDTH;
	mapy2=(yy+size)/TILE_HEIGHT;

	result=(xx>=0 && yy>=0 && mapx2<map->width && mapy2<map->height &&
		(Bulletable(me,map,mapx,mapy1)) &&
		(Bulletable(me,map,mapx,mapy2)) &&
		(Bulletable(me,map,mapx1,mapy)) &&
		(Bulletable(me,map,mapx2,mapy)) &&
		(Bulletable(me,map,mapx,mapy)) &&
		(Bulletable(me,map,mapx1,mapy1)) &&
		(Bulletable(me,map,mapx2,mapy1)) &&
		(Bulletable(me,map,mapx2,mapy2)) &&
		(Bulletable(me,map,mapx1,mapy2)));

	if((!result) && me->team==GOOD)
		SpecialShootCheck(map,mapx,mapy);

	return result;
}

void BurninateSpew(bullet_t *me)
{
	int i,x,y;

	for(i=0;i<SpecificSkillLevel(SKILL_BURNINATE)*5;i++)
	{
		x=me->x-64*FIXAMT+(rand()%(128*FIXAMT+1));
		y=me->y-64*FIXAMT+(rand()%(128*FIXAMT+1));

		FireBullet(me->owner,x,y,FIXAMT*10,rand()%256,BLT_BURNFLAME,CurseDmg(1,GOOD));
	}
}

void BulletHitWall(byte y,bullet_t *me,Map *map,world_t *world)
{
	byte newT;
	bullet_t *b;

	if(bulDef[me->type].walEffect==BH_NONE)
		return;	// go right through

	newT=me->type;

	if(y)
		me->y-=me->dy;
	else
		me->x-=me->dx;

	switch(bulDef[me->type].walEffect)
	{
		case BH_NONE:
			break;
		case BH_RNDBOUNCE:	// bounce in a random direction, V=amount of speed to keep, 255=all
			me->speed=(short)((int)(me->speed*bulDef[me->type].walValue)/255);
			if(y)
			{
				if(me->dy<0)
					me->facing=(byte)Random(128);	// an angle on the bottom side
				else
					me->facing=(byte)Random(128)+128;	// an angle on the top side
			}
			else
			{
				if(me->dx<0)
					me->facing=(byte)Random(128)+192;	// an angle on the right side
				else
					me->facing=(byte)Random(128)+64;	// an angle on the left side
			}
			me->dx=Cosine(me->facing)*me->speed/FIXAMT;
			me->dy=Sine(me->facing)*me->speed/FIXAMT;
			me->canthit=-1;
			if(me->bounces==0)
				newT=BLT_NONE;
			else
				me->bounces--;
			break;
		case BH_BOUNCE:	// bounce in the appropriate way, V=speed to keep, 255=all
			me->speed=(short)((int)(me->speed*bulDef[me->type].walValue)/255);
			if(y)
			{
				me->facing=256-me->facing;
			}
			else
			{
				if(me->facing<128)
				{
					me->facing=(128-me->facing);
				}
				else
				{
					me->facing=(byte)(384-me->facing);
				}
			}
			me->dx=Cosine(me->facing)*me->speed/FIXAMT;
			me->dy=Sine(me->facing)*me->speed/FIXAMT;
			me->canthit=-1;
			if(me->bounces==0)
				newT=BLT_NONE;
			else
				me->bounces--;
			break;
		case BH_BECOME:	// vanish, replaced by bullet type V (v=0, no bullet)
			if(bulDef[me->type].walValue==0)
				newT=0;
			else
			{
				b=FireBullet(me->owner,me->x,me->y,me->z,me->facing,bulDef[me->type].walValue,me->damage);
				if(b)
					b->canthit=me->canthit;
				newT=0;
			}
			break;
		case BH_STOP:	// stop moving
			me->speed=0;
			me->dx=0;
			me->dy=0;
			break;
	}

	// shatter on impact
	if(me->type==BLT_AXE && newT==0)
	{
		if(SpecificSkillVal(0,SKILL_SHATTER))
		{
			bullet_t *b;
			int i;
			for(i=0;i<SpecificSkillVal(0,SKILL_SHATTER);i++)
			{
				b=FireBullet(me->owner,me->x,me->y,me->z,rand()%256,BLT_AXESHARD,bulDef[BLT_AXESHARD].damage);
				if(b)
					b->canthit=me->canthit;
			}
		}
	}
	else if(me->type==BLT_MINIAXE && newT==0)
	{
		if(SpecificSkillValDiminished(0,SKILL_SHATTER,SpecificSkillVal(0,SKILL_TRIPLE)))
		{
			bullet_t *b;
			int i;

			for(i=0;i<SpecificSkillValDiminished(0,SKILL_SHATTER,SpecificSkillVal(0,SKILL_TRIPLE));i++)
			{
				b=FireBullet(me->owner,me->x,me->y,me->z,rand()%256,BLT_AXESHARD,bulDef[BLT_AXESHARD].damage);
				if(b)
					b->canthit=me->canthit;
			}
		}
	}
	else if(me->type==BLT_BURNINATE && newT==BLT_KABOOM)
	{
		// spew flames
		BurninateSpew(me);
	}

	if(bulDef[me->type].walSound!=0)
		MakeSound(bulDef[me->type].walSound,me->x,me->y,SND_CUTOFF|SND_RANDOM,100);
	if(bulDef[me->type].walPart!=0)
		ParticleBoom(bulDef[me->type].walPart,me->x,me->y,me->z);
	me->type=newT;
}

void BulletHitFloor(bullet_t *me,Map *map,world_t *world)
{
	byte newT;
	bullet_t *b;

	newT=me->type;
	me->z=0;

	switch(bulDef[me->type].flrEffect)
	{
		case BH_NONE:
			me->speed=(short)((int)(me->speed*bulDef[me->type].flrFriction)/255);
			break;
		case BH_RNDBOUNCE:	// bounce in a random direction, V=amount of speed to keep, 255=all
			me->speed=(short)((int)(me->speed*bulDef[me->type].flrFriction)/255);
			me->dz=-(short)((int)(me->dz*bulDef[me->type].flrValue)/255);
			me->facing=(byte)Random(256);
			me->dx=Cosine(me->facing)*me->speed/FIXAMT;
			me->dy=Sine(me->facing)*me->speed/FIXAMT;
			break;
		case BH_BOUNCE:	// bounce in the appropriate way, V=speed to keep, 255=all
			me->speed=(short)((int)(me->speed*bulDef[me->type].flrFriction)/255);
			me->dz=-(short)((int)(me->dz*bulDef[me->type].flrValue)/255);
			break;
		case BH_SUMMON:
			newT=0;
			AddGuy(me->x,me->y,me->z,bulDef[me->type].flrValue);
			break;
		case BH_BECOME:	// vanish, replaced by bullet type V (v=0, no bullet)
			if(bulDef[me->type].flrValue==0)
				newT=0;
			else
			{
				b=FireBullet(me->owner,me->x,me->y,me->z,me->facing,bulDef[me->type].flrValue,me->damage);
				if(b)
				{
					b->canthit=me->canthit;
					b->team=me->team;
				}
				newT=0;
			}
			break;
		case BH_STOP:	// stop moving
			me->speed=0;
			me->dx=0;
			me->dy=0;
			me->dz=0;
			break;
	}

	if(bulDef[me->type].flrSound!=0)
		MakeSound(bulDef[me->type].flrSound,me->x,me->y,SND_CUTOFF|SND_RANDOM,100);
	if(bulDef[me->type].flrPart!=0)
		ParticleBoom(bulDef[me->type].flrPart,me->x,me->y,me->z);
	me->type=newT;

	byte mapx,mapy;
	mapx=me->x/(TILE_WIDTH*FIXAMT);
	mapy=me->y/(TILE_HEIGHT*FIXAMT);
	if((bulDef[me->type].flags&BF_GROUND) && mapx>=0 && mapy>=0 && mapx<map->width && mapy<map->height && (world->terrain[map->map[mapx+mapy*map->width].floor].flags&TF_WATER))
	{
		ExplodeParticles2(PART_WATER,me->x,me->y,0,40,6);
		MakeSound(SND_SPLASH,me->x,me->y,SND_CUTOFF|SND_RANDOM,1200);
		me->type=BLT_NONE;
	}
	if((bulDef[me->type].flags&BF_GROUND) && mapx>=0 && mapy>=0 && mapx<map->width && mapy<map->height && (world->terrain[map->map[mapx+mapy*map->width].floor].flags&TF_LAVA))
	{
		ParticleBoom(PART_SMOKE,me->x,me->y,0);
		me->type=BLT_NONE;
	}
}

void BulletRanOut(bullet_t *me,Map *map,world_t *world)
{
	switch(bulDef[me->type].endEffect)
	{
		case BH_NONE:
			break;
		case BH_RNDBOUNCE:	// ignore this, no bouncing to be done if you're gone
			break;
		case BH_BOUNCE:
			break;
		case BH_SUMMON:
			AddGuy(me->x,me->y,me->z,bulDef[me->type].endValue);
			break;
		case BH_BECOME:	// vanish, replaced by bullet type V (v=0, no bullet)
			if(bulDef[me->type].endValue!=0)
			{
				bullet_t *b;

				if(me->type==BLT_BURNINATE)
					BurninateSpew(me);

				b=FireBullet(me->owner,me->x,me->y,me->z,me->facing,bulDef[me->type].endValue,me->damage);
				if(me->type==BLT_FLAME && b)	// length of flame2 depends on who the owner is
				{
					if(me->owner==goodguy->ID)
					{
						b->timer=(word)SpecificSkillVal(0,SKILL_FLAME);
					}
				}
				if(b)
					b->canthit=me->canthit;
			}
			break;
		case BH_STOP:	// stop moving
			break;
	}

	if(bulDef[me->type].endSound!=0)
		MakeSound(bulDef[me->type].endSound,me->x,me->y,SND_CUTOFF|SND_RANDOM,100);
	if(bulDef[me->type].endPart!=0)
		ParticleBoom(bulDef[me->type].endPart,me->x,me->y,me->z);
	me->type=0;
}

void BulletHitAGuy(bullet_t *me,Map *map,world_t *world)
{
	bullet_t *b;
	byte newT;

	newT=me->type;

	switch(bulDef[me->type].hitEffect)
	{
		case BH_NONE:
			break;
		case BH_RNDBOUNCE:	// bounce in a random direction, V=amount of speed to keep, 255=all
			me->speed=(short)((int)(me->speed*bulDef[me->type].hitValue)/255);
			me->facing=me->facing+128-64+(byte)Random(129);	// any direction that is within 90 degrees either way of the opposite of your original heading
			me->dx=Cosine(me->facing)*me->speed/FIXAMT;
			me->dy=Sine(me->facing)*me->speed/FIXAMT;
			break;
		case BH_BOUNCE:	// bounce in the appropriate way, V=speed to keep, 255=all
			me->speed=(short)((int)(me->speed*bulDef[me->type].hitValue)/255);
			me->facing=me->facing+128;	// just opposite your previous heading
			me->dx=Cosine(me->facing)*me->speed/FIXAMT;
			me->dy=Sine(me->facing)*me->speed/FIXAMT;
			break;
		case BH_BECOME:	// vanish, replaced by bullet type V (v=0, no bullet)
			if(me->type==BLT_ITEM)
			{
				if(PlayerGetItem((byte)me->anim,me->target,me->owner))
					return;	// don't disappear!  maybe you'll get picked up later
			}
			if(me->type==BLT_BURNINATE)
				BurninateSpew(me);
			if(bulDef[me->type].hitValue==0)
				newT=0;
			else
			{
				b=FireBullet(me->owner,me->x,me->y,me->z,me->facing,bulDef[me->type].hitValue,me->damage);
				if(b)
					b->canthit=me->canthit;
				newT=0;
			}
			break;
		case BH_STOP:	// stop moving
			me->speed=0;
			me->dx=0;
			me->dy=0;
			break;
	}

	// shatter on impact
	if(me->type==BLT_AXE)
	{
		if(SpecificSkillVal(0,SKILL_SHATTER) || player.axe.magic==EM_THORNY)
		{
			bullet_t *b;
			int i,n;

			n=(int)SpecificSkillVal(0,SKILL_SHATTER);
			if(player.axe.magic==EM_THORNY)
				n+=2;

			for(i=0;i<n;i++)
			{
				b=FireBullet(me->owner,me->x,me->y,me->z,rand()%256,BLT_AXESHARD,bulDef[BLT_AXESHARD].damage);
				if(b)
					b->canthit=me->canthit;
			}
		}
	}
	else if(me->type==BLT_MINIAXE)
	{
		if(SpecificSkillValDiminished(0,SKILL_SHATTER,SpecificSkillVal(0,SKILL_TRIPLE)) || player.axe.magic==EM_THORNY)
		{
			bullet_t *b;
			int i,n;

			n=(int)SpecificSkillValDiminished(0,SKILL_SHATTER,SpecificSkillVal(0,SKILL_TRIPLE));
			if(player.axe.magic==EM_THORNY)
				n+=2;

			for(i=0;i<n;i++)
			{
				b=FireBullet(me->owner,me->x,me->y,me->z,rand()%256,BLT_AXESHARD,bulDef[BLT_AXESHARD].damage);
				if(b)
					b->canthit=me->canthit;
			}
		}
	}
	else if(me->type==BLT_CLOCKBULLET)
	{
		if(player.var[VAR_CLOCKBOT-IT_LOG+IT_CLAW])
		{
			bullet_t *b;
			int i;

			for(i=0;i<player.var[VAR_CLOCKBOT-IT_LOG+IT_CLAW];i++)
			{
				b=FireBullet(me->owner,me->x,me->y,me->z,rand()%256,BLT_AXESHARD,bulDef[BLT_AXESHARD].damage);
				if(b)
					b->canthit=me->canthit;
			}
		}
		if(player.var[VAR_CLOCKBOT-IT_LOG+IT_DIAMOND])
		{
			bullet_t *b;
			b=FireBullet(me->owner,me->x,me->y,me->z,rand()%256,BLT_CLOCKBOOM,me->damage);
			if(b)
				b->canthit=me->canthit;
		}
	}

	me->nohit=bulDef[me->type].hitRate;
	if(bulDef[me->type].hitSound!=0)
		MakeSound(bulDef[me->type].hitSound,me->x,me->y,SND_CUTOFF|SND_RANDOM,100);
	if(bulDef[me->type].hitPart!=0)
		ParticleBoom(bulDef[me->type].hitPart,me->x,me->y,me->z);
	me->type=newT;
}

byte GuyLacksGale(int id)
{
	int i;

	if(MonsterFlags(GetGuy(id)->type)&MF_NOMOVE)
		return 0;	// can't gale an immobile monster
	for(i=0;i<MAX_BULLETS;i++)
		if(bullet[i].owner==id && bullet[i].type==BLT_GALE2)
			return 0;

	return 1;
}

void GaleBullets(bullet_t *me)
{
	int i;

	for(i=0;i<MAX_BULLETS;i++)
	{
		if(bullet[i].type!=BLT_GALE1 && bullet[i].type!=BLT_GALE2 && abs(bullet[i].x-me->x)<30*FIXAMT && abs(bullet[i].y-me->y)<30*FIXAMT)
		{
			bullet[i].facing=me->facing;	// get blown!
		}
	}
}

void HitBadguys(bullet_t *me,Map *map,world_t *world)
{
	int i,start;
	byte team,f;
	short dx,dy;

	if(me->type==BLT_ITEM && me->z>0)
		return;	// items can only be gotten when on the ground

	bulletHittingType=me->type;

	team=0;
	if(bulDef[me->type].flags&BF_HITEVIL)
	{
		if(me->team==GOOD)
			team+=EVIL;
		else
			team+=GOOD;
	}
	if(bulDef[me->type].flags&BF_HITGOOD)
	{
		if(me->team==GOOD)
			team+=GOOD;
		else
			team+=EVIL;
	}
	if(team==0)
		return;	// can't hit anybody if you can't hit anybody

	if(me->type==BLT_GALE1)
		GaleBullets(me);

	start=0;
	while(1)
	{
		i=FindVictim(me->x,me->y,me->x/(TILE_WIDTH*FIXAMT),me->y/(TILE_HEIGHT*FIXAMT),bulDef[me->type].size,me->canthit,team,start,map,world);
		if(i==0)
			return;
		start=i;	// next pass start over at the next guy

		if(i==me->canthit)
			continue;

		//if(me->z>GetGuy(i-1)->z+monsType[GetGuy(i-1)->type].height*FIXAMT)
		//	continue;	// up too high to hit

		if((bulDef[me->type].flags&BF_PLAYERONLY) && ((i-1)!=goodguy->ID))
			continue;	// can only hit the player

		if(me->type==BLT_COLDWAVE && GetGuy(i-1)->frozen)
			continue;	// can't hit frozen guys

		if(bulDef[me->type].flags&BF_RANDHITDIR)
		{
			f=(byte)Random(256);
		}
		else
			f=me->facing;

		if(me->type!=BLT_ITEM && me->type!=BLT_BERRY)
		{
			dx=Cosine(f)*bulDef[me->type].hitForce/FIXAMT;
			dy=Sine(f)*bulDef[me->type].hitForce/FIXAMT;
			if(goodguy && (i-1)==goodguy->ID && ((player.parry && player.var[VAR_PARRYCLOCK]==0) || player.potionType==POT_DEFLECTOR))
			{
				// parried it
				MakeSound(SND_PARRY,me->x,me->y,SND_CUTOFF|SND_RANDOM,600);
				me->team=goodguy->team;
				me->facing=(me->facing+128)&255;
				player.parry=0;
				player.var[VAR_PARRYCLOCK]=(byte)(30*2-SpecificSkillVal(1,SKILL_PARRY));
				return;
			}

			if(me->type==BLT_AXE || me->type==BLT_MINIAXE)
			{
				if(me->type==BLT_AXE)
					LoonyHitGuyThrow(1.0f,bulDef[me->type].damage,goodguy,i,world);
				else
					LoonyHitGuyThrow(SpecificSkillVal(0,SKILL_TRIPLE),bulDef[me->type].damage,goodguy,i,world);
				if(me->pierces>0)
				{
					me->pierces--;
					bulDef[me->type].hitEffect=BH_NONE;
					me->canthit=i;
				}
				else
					bulDef[me->type].hitEffect=BH_BECOME;
			}
			else if(me->type==BLT_CLOCKBULLET)
			{
				meleeAttacker=NULL;
				if(GetGuy(i-1)->GetShot(dx,dy,me->damage,map,world))
				{
					if(player.var[VAR_CLOCKBOT-IT_LOG+IT_ICECUBE])
						GetGuy((word)(i-1))->GetFrozen(player.var[VAR_CLOCKBOT-IT_LOG+IT_ICECUBE]*15);
					if(player.var[VAR_CLOCKBOT-IT_LOG+IT_ECTO])
						GetGuy((word)(i-1))->GetPoisoned(player.var[VAR_CLOCKBOT-IT_LOG+IT_ECTO]*30);
					if(player.var[VAR_CLOCKBOT-IT_LOG+IT_SILVER])
						GetGuy((word)(i-1))->GetStunned(player.var[VAR_CLOCKBOT-IT_LOG+IT_SILVER]*30/4);
				}
			}
			else
			{
				meleeAttacker=NULL;
				if(GetGuy(i-1)->GetShot(dx,dy,me->damage,map,world))
				{
					// other effects like freezing could occur here
					if(bulDef[me->type].flags&BF_FREEZE)
					{
						if(me->type==BLT_COLDWAVE)
							GetGuy(i-1)->GetFrozen((int)(30.0f*SpecificSkillVal(1,SKILL_COLDWAVE)));
						else
							GetGuy(i-1)->GetFrozen(30*2);
					}
					if(bulDef[me->type].flags&BF_POISON)
					{
						GetGuy(i-1)->GetPoisoned(30*3);
					}
					if(me->type==BLT_BRAMBLE2)
					{
						GetGuy(i-1)->GetPoisoned(30);
						GetGuy(i-1)->GetStunned(1);
					}
				}
				if(me->type==BLT_IGNITER)
				{
					GetGuy(i-1)->ignited=(word)SpecificSkillVal(0,SKILL_IGNITE);
				}
				if(me->type==BLT_DRAIN)
					HealGoodguy((byte)(1.0f*SpellDamageBoost(SC_DEATH)/100.0f));	// heal you on impact
				if(me->type==BLT_GALE1)
				{
					if(GuyLacksGale(i-1))	// only gale this guy if he's not already being galed
						FireBullet(i-1,GetGuy(i-1)->x,GetGuy(i-1)->y,0,me->facing,BLT_GALE2,0);
				}
			}
		}
		if(me->type==BLT_BERRY)
		{
			MakeSound(SND_BERRYEAT,me->x,me->y,SND_CUTOFF|SND_RANDOM,500);
			if(GetGuy(i-1)->type!=player.monsType)
			{
				GetGuy(i-1)->hp+=me->damage;
				if(GetGuy(i-1)->hp>MonsterHP(GetGuy(i-1)->type,GetGuy(i-1)->team))
					GetGuy(i-1)->hp=MonsterHP(GetGuy(i-1)->type,GetGuy(i-1)->team);
				if(GetGuy(i-1)->type==MONS_YOURBOT)
				{
					player.var[VAR_CLOCKLIFE]=GetGuy(i-1)->hp&0xFF;
					player.var[VAR_CLOCKLIFE2]=GetGuy(i-1)->hp>>8;
				}

				NumberParticle(me->damage,GetGuy(i-1)->x,GetGuy(i-1)->y+1,1);
			}
			else
			{
				if(player.magic<player.maxMagic)
				{
					goodguy->x-=20*FIXAMT;
					HealGoodguy(me->damage);
					goodguy->x+=20*FIXAMT;
					player.magic++;
					NumberParticle(1,goodguy->x+FIXAMT*20,goodguy->y+1,6);
				}
				else
					HealGoodguy(me->damage);
			}
		}

		if(bulDef[me->type].flags&BF_NOREHIT)
			me->canthit=i;

		if(me->type!=BLT_ITEM || GetGuy(i-1)->type==player.monsType || GetGuy(i-1)->type==MONS_BONEHEAD)
		{
			BulletHitAGuy(me,map,world);	// only boneheads and loony can pick up items
			if(GetGuy(i-1)->team==EVIL && (me->type==BLT_FLAME2 || me->type==BLT_FLAMEB2))
			{
				// the rehit on flames is modified if they are your flames, by Combustion talent
				me->nohit=bulDef[me->type].hitRate-(byte)TalentBonus(TLT_COMBUSTION);
			}
		}
		if(me->type==0)
			return;	// all gone!

		if(!(bulDef[me->type].flags&BF_HITMANY))
			return;	// 1 hit is the limit!
	}
}

void SpawnFromMe(bullet_t *me)
{
	byte f,d;
	int i;

	for(i=0;i<bulDef[me->type].spawnCount;i++)
	{
		switch(bulDef[me->type].spawnMethod)
		{
			case BS_RANDOM:
				f=rand()%256;
				break;
			case BS_SAMEDIR:
				f=me->facing;
				break;
			case BS_RANDDIR:
				f=me->facing-16+(rand()%33);
				break;
			case BS_RADIUS:
				f=rand()%256;
				d=rand()%(bulDef[me->type].size);
				FireBullet(me->owner,me->x+Cosine(f)*d,me->y+Sine(f)*d,me->z,f,bulDef[me->type].spawnType,bulDef[me->type].spawnDmg);
				continue;
				break;
			case BS_CIRCLE:
				f=(256/bulDef[me->type].spawnCount)*i;
				break;
		}
		if(me->type==BLT_STONESPIKE || me->type==BLT_EVILSPIKE || me->type==BLT_EVILSPIKE2)
		{
			if(me->x/(TILE_WIDTH*FIXAMT)>=0 && me->y/(TILE_HEIGHT*FIXAMT)>=0 && me->x/(TILE_WIDTH*FIXAMT)<curMap->width &&
				me->y/(TILE_HEIGHT*FIXAMT)<curMap->height && (curWorld.terrain[curMap->map[(me->x/(TILE_WIDTH*FIXAMT))+(me->y/(TILE_HEIGHT*FIXAMT))*curMap->width].floor].flags&(TF_LAVA|TF_WATER|TF_SKY)))
			{
				return;	// can't make new spikes here
			}
			MakeSound(SND_STONERUMBLE,me->x,me->y,SND_CUTOFF|SND_RANDOM,1);
			bulDef[me->type].spawnDmg=me->damage;
		}
		if(me->type==BLT_FLAMETRAIL)
			bulDef[me->type].spawnDmg=me->damage;

		FireBullet(me->owner,me->x,me->y,me->z,f,bulDef[me->type].spawnType,bulDef[me->type].spawnDmg);
	}
}

void UpdateBullet(bullet_t *me,Map *map,world_t *world)
{
	int mapx,mapy;

	meleeAttacker=NULL;
	bulletHittingType=me->type;

	if(!me->type)
		return;

	// bullets can't live when far away, except items
	if((abs(me->x-goodguy->x)>FIXAMT*600 || abs(me->y-goodguy->y)>FIXAMT*400) && me->type!=BLT_ITEM && me->type!=BLT_ROSESEED && me->type!=BLT_ROCK)
	{
		me->type=BLT_NONE;
		return;
	}

	if(me->team==EVIL && goodguy && (rand()%100)<5 && HeatShieldOn() && Distance(me->x,me->y,goodguy->x,goodguy->y)<FIXAMT*60)
	{
		ParticleBoom(PART_FIRE,me->x,me->y,me->z);
		me->type=BLT_NONE;
		return;
	}

	me->dx=Cosine(me->facing)*me->speed/FIXAMT;
	me->dy=Sine(me->facing)*me->speed/FIXAMT;

	me->speed+=bulDef[me->type].dspeed;
	if(me->speed<0)
		me->speed=0;

	mapx=(me->x/TILE_WIDTH)>>FIXSHIFT;
	mapy=(me->y/TILE_HEIGHT)>>FIXSHIFT;
	if(mapx>=0 && mapy>=0 && mapx<map->width && mapy<map->height)
		me->bright=map->map[mapx+mapy*map->width].templight;
	else
		me->bright=-32;

	if(bulDef[me->type].flags&BF_LIGHT)
	{
		me->bright-=5;
		map->BrightTorch(mapx,mapy,5,5);
	}

	me->x+=me->dx;
	if(!BulletCanGo(me,me->x,me->y,map,8))
		BulletHitWall(0,me,map,world);
	else
	{
		me->y+=me->dy;
		if(!BulletCanGo(me,me->x,me->y,map,8))
			BulletHitWall(1,me,map,world);
	}

	if(me->type==0)
		return;

	me->z+=me->dz;
	if(me->z<0)
		BulletHitFloor(me,map,world);

	if(me->z<=0 && (bulDef[me->type].flags&BF_GROUND))
	{
		if(mapx>=0 && mapy>=0 && mapx<map->width && mapy<map->height && (world->terrain[map->map[mapx+mapy*map->width].floor].flags&(TF_SKY|TF_CLIFF)))
		{
			me->facing=64;
			me->speed+=FIXAMT;
		}
	}

	if(bulDef[me->type].flags&BF_STICK)
	{
		Guy *g;

		g=GetGuy(me->owner);
		if(g)
		{
			me->x=g->x;
			me->y=g->y;
		}
	}
	if(me->type==0)
		return;

	if(me->type==BLT_ITEM)
	{
		me->bright+=itmBright;
	}
	// all gravity-affected bullets, get gravitized
	me->dz+=bulDef[me->type].gravity;

	me->timer--;
	if(!me->timer)
	{
		if(me->type==BLT_STUN)
			me->timer=32;
		else
			BulletRanOut(me,map,world);
	}

	if(me->type==BLT_STUN)
	{
		me->z=GetGuy(me->owner)->z+monsType[GetGuy(me->owner)->type].height*FIXAMT;
		me->y=GetGuy(me->owner)->y+1;
		if(GetGuy(me->owner)->hp==0 || (GetGuy(me->owner)->stun<3 && GetGuy(me->owner)->stun>=0))
			me->type=0;	// go away if he is dead or not stunned anymore
	}

	if(me->type==0)
		return;

	if(me->type==BLT_TORNADO)
	{
		TornadoGuys(me->team,me->x,me->y,(int)SpecificSkillVal(0,SKILL_TORNADO)*FIXAMT,map,world);
	}
	if(me->type==BLT_TORNADO2)
	{
		TornadoGuys(me->team,me->x,me->y,150*FIXAMT,map,world);
	}

	if(me->type==BLT_GALE2)
	{
		// drag your owner along for the ride
		if(!GetGuy(me->owner)->WalkCheck(me->x,me->y,map,world))
		{
			mapx=(byte)(((me->speed)/(FIXAMT*2)+1)*SpellDamageBoost(SC_WIND)/100.0f);
			GetGuy(me->owner)->GetShot(me->dx,me->dy,mapx,map,world);
			if(mapx>20)
				mapx=20;
			GetGuy(me->owner)->GetStunned(mapx*2);
			me->type=BLT_NONE;
		}
		else
		{
			GetGuy(me->owner)->x=me->x;
			GetGuy(me->owner)->y=me->y;
		}
		if(me->speed>FIXAMT*22)
			me->speed=FIXAMT*22;
	}
	if(me->type==BLT_DRAINMASTER)
	{
		if(GetGuy(me->owner)->type==0 || GetGuy(me->owner)->hp==0)
		{
			me->type=0;
			return;
		}
		//if(me->timer%2)
		//	FireBullet(me->owner,me->x,me->y,me->z,(byte)(rand()%256),BLT_DRAIN,0);
		if((me->timer%45)==44)
		{
			GetGuy(me->owner)->GetShotReal(0,0,me->damage,map,world);
			FireBullet(me->owner,me->x,me->y,me->z,(byte)(rand()%256),BLT_DRAIN,0);
			MakeSound(SND_DRAIN,me->x,me->y,SND_CUTOFF|SND_RANDOM,600);
		}
	}
	if(me->type==BLT_CRYSTALSHOCK)
	{
		if(mapx>=0 && mapy>=0 && mapx<map->width && mapy<map->height)
		{
			map->map[mapx+mapy*map->width].templight=(30-me->timer);
			if(GetGuy(me->owner)->hp==0)
				me->type=BLT_NONE;
			else
				LightningBolt(me->x,me->y,GetGuy(me->owner)->x,GetGuy(me->owner)->y-GetGuy(me->owner)->z-FIXAMT*40);
		}
	}
	if(bulDef[me->type].homing>0)
	{
		if(me->target==0 || GetGuy(me->target-1)->hp==0 || me->target==me->canthit || GetGuy(me->target-1)->type==MONS_NONE ||
			GetGuy(me->target-1)->team==me->team)	// don't have a target
		{
			if(bulDef[me->type].flags&BF_PLAYERONLY)
				me->target=goodguy->ID+1;
			else
				me->target=BulletFindTarget(me->team,me->facing,me->x,me->y,me->canthit,map,world);
		}
		if(me->target!=0)
		{
			byte f;

			f=AngleFrom(me->x,me->y,GetGuy(me->target-1)->x,GetGuy(me->target-1)->y);
			me->facing=TurnToward(me->facing,f,bulDef[me->type].homing);
		}
	}
	if(bulDef[me->type].spawnMethod!=BS_NONE)
	{
		if(me->bSpawnTimer)
		{
			me->bSpawnTimer--;
			if(!me->bSpawnTimer)
			{
				me->bSpawnTimer=bulDef[me->type].spawnRate;
				SpawnFromMe(me);
			}
		}
	}
	// and particle spawn
	if(bulDef[me->type].partType!=0)
	{
		if(me->pSpawnTimer)
		{
			me->pSpawnTimer--;
			if(!me->pSpawnTimer)
			{
				me->pSpawnTimer=bulDef[me->type].partRate;
				ParticleBoom(bulDef[me->type].partType,me->x,me->y,me->z);
			}
		}
	}
	else if(me->type==BLT_AXE || me->type==BLT_MINIAXE)
	{
		if(me->pSpawnTimer)
		{
			me->pSpawnTimer--;
			if(!me->pSpawnTimer)
			{
				int t;

				me->pSpawnTimer=bulDef[me->type].partDelay;
				t=BulletFindTargetClosest(me->team,me->x,me->y,0,100*FIXAMT,map,world);
				if(t!=0 && curMap->CheckLOS(mapx,mapy,8,GetGuy(t-1)->mapx,GetGuy(t-1)->mapy))
				{
					bulletHittingType=me->type;
					meleeAttacker=NULL;
					GetGuy(t-1)->GetShotReal(0,0,ShockDmg((word)SpecificSkillVal(1,SKILL_STATIC),GOOD),map,world);
					TalentPoint(TLT_ELECTROCUTION,(word)SpecificSkillVal(1,SKILL_STATIC));
					LightningBolt(me->x,me->y-me->z,GetGuy(t-1)->x,GetGuy(t-1)->y-FIXAMT*20);
				}
			}
		}
	}

	if(me->type==BLT_SHOCKORB)
	{
		if(me->pSpawnTimer)
		{
			me->pSpawnTimer--;
			if(!me->pSpawnTimer)
			{
				int t;
				me->pSpawnTimer=bulDef[me->type].partDelay;
				t=BulletFindTargetClosest(me->team,me->x,me->y,0,160*FIXAMT,map,world);
				LightningBolt(me->x,me->y-me->z,me->x-10*FIXAMT+(rand()%(20*FIXAMT+1)),me->y-me->z-10*FIXAMT+(rand()%(20*FIXAMT+1)));
				curMap->BrightTorch(mapx,mapy,9,7);
				if(t!=0 && curMap->CheckLOS(mapx,mapy,8,GetGuy(t-1)->mapx,GetGuy(t-1)->mapy))
				{
					bulletHittingType=me->type;
					meleeAttacker=NULL;
					GetGuy(t-1)->GetShot(0,0,me->damage,map,world);
					TalentPoint(TLT_ELECTROCUTION,me->damage);
					LightningBolt(me->x,me->y-me->z,GetGuy(t-1)->x,GetGuy(t-1)->y-FIXAMT*20);
				}
			}
		}
	}

	// animate
	me->anim+=bulDef[me->type].animSpeed;
	while(me->anim>255)
	{
		me->frame++;
		me->anim-=256;
		if(me->frame>bulDef[me->type].lastFrm)
			me->frame=bulDef[me->type].firstFrm;
	}
	if(me->nohit)
		me->nohit--;
	else
		HitBadguys(me,map,world);
}

void RenderBullet(bullet_t *me)
{
	sprite_t *curSpr;
	byte facing,facewid;
	word spr;
	word flags;
	byte b;

	if(bulDef[me->type].flags&BF_INVIS)
		return;	// nope!

	if(me->type==BLT_ITEM)
	{
		if(me->timer<30 && (me->timer&1)==0)
			return;	// flicker
		b=me->target/65536;
		if(b>=EM_ARTSTART && b<=EM_ARTEND)
		{
			if(me->anim==IT_GOLDENAXE)
				RenderItem(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,(byte)IT_ARTAXE,0,me->bright);
			else if(me->anim==IT_GOLDENPARKA)
				RenderItem(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,(byte)IT_ARTPARKA,0,me->bright);
			else
				RenderItem(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,(byte)IT_ARTAMMY,0,me->bright);
		}
		else
			RenderItem(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,(byte)me->anim,0,me->bright);
		return;
	}
	if(me->type==BLT_ICECUBE)
	{
		itemInfo[IT_ICECUBE].flags|=IF_GLOW;
		RenderItem(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,IT_ICECUBE,0,me->bright);
		itemInfo[IT_ICECUBE].flags&=(~IF_GLOW);
		return;
	}

	facewid=(256/bulDef[me->type].facings);
	if(facewid==0)
		facing=0;
	else
	{
		facing=(me->facing+facewid/2)/facewid;
		if(facing>=bulDef[me->type].facings)
			facing=0;
	}
	spr=me->frame+facing*(bulDef[me->type].lastFrm-bulDef[me->type].firstFrm+1);
	curSpr=bulletSpr->GetSprite(spr);

	if(bulDef[me->type].flags&BF_SHADOW)
		SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,0,255,me->bright,curSpr,DISPLAY_DRAWME|DISPLAY_SHADOW);

	flags=DISPLAY_DRAWME;
	b=me->bright;
	if(bulDef[me->type].flags&BF_GLOW)
	{
		flags|=DISPLAY_GLOW;
		b-=5;
	}
	if(bulDef[me->type].flags&BF_FLOOR)
		flags|=DISPLAY_BOTTOM;

	if(me->type==BLT_STONESPIKE || me->type==BLT_EVILSPIKE || me->type==BLT_EVILSPIKE2)
	{
		if(player.levelNum<LVL_GEYSER || player.levelNum>LVL_TITANS)	// overworld, brown spikes (maybe add exceptions for grey ones, like tundra, grey caves)
			SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,b,curSpr,flags);
		else
			OffSprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,2,6,b,curSpr,flags|DISPLAY_OFFCOLOR);
	}
	else
		SprDraw(me->x>>FIXSHIFT,me->y>>FIXSHIFT,me->z>>FIXSHIFT,255,b,curSpr,flags);
}

void UpdateBullets(Map *map,world_t *world)
{
	int i;

	itmBright+=itmDBright;
	if(itmBright>4 || itmBright==0)
		itmDBright=-itmDBright;

	for(i=0;i<MAX_BULLETS;i++)
		if(bullet[i].type)
		{
			if(bullet[i].team==GOOD)
				UpdateBullet(&bullet[i],map,world);
			else
			{
				// can speed up or slow down enemy bullets here
				UpdateBullet(&bullet[i],map,world);
			}
		}
}

void RenderBullets(void)
{
	int i;

	for(i=0;i<MAX_BULLETS;i++)
		if(bullet[i].type)
			RenderBullet(&bullet[i]);
}

void FireMe(bullet_t *me,int x,int y,int z,int owner,byte facing,byte type,word damage)
{
	me->type=type;
	me->bounces=255;
	if(me->type==BLT_AXE)
	{
		me->bounces=(byte)SpecificSkillVal(0,SKILL_RICOCHET);
		me->pierces=(byte)SpecificSkillVal(0,SKILL_PIERCE);
	}
	else if(me->type==BLT_MINIAXE)
	{
		me->bounces=(byte)(SpecificSkillVal(0,SKILL_RICOCHET)*SpecificSkillVal(0,SKILL_TRIPLE));
		me->pierces=(byte)(SpecificSkillVal(0,SKILL_PIERCE)*SpecificSkillVal(0,SKILL_TRIPLE));
	}
	me->damage=damage;
	me->x=x;
	me->y=y;
	me->z=z;
	me->facing=facing;
	me->bright=0;
	me->dx=0;
	me->dy=0;
	me->dz=bulDef[me->type].beginDZ;
	me->canthit=0;
	me->speed=bulDef[me->type].speed;
	me->timer=bulDef[me->type].timer;
	me->target=0;
	me->owner=owner;
	me->anim=0;
	me->frame=bulDef[me->type].firstFrm;
	me->team=GetGuy(owner)->team;
	me->nohit=0;
	me->bSpawnTimer=bulDef[me->type].spawnDelay;
	me->pSpawnTimer=bulDef[me->type].partDelay;
}

bullet_t *FireBullet(int owner,int x,int y,int z,byte facing,byte type,word damage)
{
	int i;

	for(i=0;i<MAX_BULLETS;i++)
		if(!bullet[i].type)
		{
			FireMe(&bullet[i],x,y,z,owner,facing,type,damage);
			return &bullet[i];
			break;
		}
	return NULL;
}

void BulletSwap(int sx,int sy,int width,int height,int dx,int dy)
{
	int i;

	sx*=(TILE_WIDTH*FIXAMT);
	sy*=(TILE_HEIGHT*FIXAMT);
	dx*=(TILE_WIDTH*FIXAMT);
	dy*=(TILE_HEIGHT*FIXAMT);
	width*=(TILE_WIDTH*FIXAMT);
	height*=(TILE_HEIGHT*FIXAMT);

	for(i=0;i<MAX_BULLETS;i++)
	{
		if(bullet[i].type)
		{
			if(bullet[i].x>=sx && bullet[i].y>=sy && bullet[i].x<=(sx+width) && bullet[i].y<=(sy+height))
			{
				// in target area, swap
				bullet[i].x+=(-sx+dx);
				bullet[i].y+=(-sy+dy);
			}
			else if(bullet[i].x>=dx && bullet[i].y>=dy && bullet[i].x<=(dx+width) && bullet[i].y<=(dy+height))
			{
				// in other target area, swap
				bullet[i].x+=(-dx+sx);
				bullet[i].y+=(-dy+sy);
			}
		}
	}
}

byte Intersect(int rx,int ry,int rx2,int ry2,int r2x,int r2y,int r2x2,int r2y2)
{
	return (rx<r2x2 && rx2>r2x && ry<r2y2 && ry2>r2y);
}

void DestroyBullets(int x,int y,byte team,int radius)
{
	int i;

	for(i=0;i<MAX_BULLETS;i++)
	{
		if(bullet[i].type && bullet[i].team==team && Distance(bullet[i].x,bullet[i].y,x,y)<radius)
		{
			if(player.amulet.magic==EM_MIRRORED)
			{
				bullet[i].facing+=128;
				bullet[i].team=goodguy->team;
			}
			else
			{
				bullet[i].type=BLT_NONE;
				ParticleBoom(PART_WHITEPOP,bullet[i].x,bullet[i].y,bullet[i].z);
			}
		}
	}
}

void SaveBullets(SDL_RWops *f)
{
	int i;
	word b;

	b=0;
	for(i=0;i<MAX_BULLETS;i++)
	{
		if(bullet[i].type)
			b++;
	}

	SDL_RWwrite(f,&b,sizeof(word),1);
	for(i=0;i<MAX_BULLETS;i++)
	{
		if(bullet[i].type)
			SDL_RWwrite(f,&bullet[i],sizeof(bullet_t),1);
	}
}

void LoadBullets(SDL_RWops *f)
{
	int i;
	word b;

	b=0;
	for(i=0;i<MAX_BULLETS;i++)
	{
		if(bullet[i].type)
			bullet[i].type=BLT_NONE;
	}

	SDL_RWread(f,&b,sizeof(word),1);
	for(i=0;i<b;i++)
	{
		SDL_RWread(f,&bullet[i],sizeof(bullet_t),1);
	}
}

void SmashTrees(int owner,int x,int y,int radius,Map *map)
{
	byte n;
	int mapx1,mapx2,mapy1,mapy2;
	int i,j;

	x>>=FIXSHIFT;
	y>>=FIXSHIFT;

	mapx1=(x-radius)/TILE_WIDTH;
	mapy1=(y-radius)/TILE_HEIGHT;
	mapx2=(x+radius)/TILE_WIDTH;
	mapy2=(y+radius)/TILE_HEIGHT;

	// boundary conditions
	if(mapx1<0)
		mapx1=0;
	if(mapx2>=map->width)
		mapx2=map->width-1;
	if(mapy1<0)
		mapy1=0;
	if(mapy2>=map->height)
		mapy2=map->height-1;

	if(mapx1>mapx2)
		return;	// nothing to smash
	if(mapy1>mapy2)
		return;

	for(i=mapx1;i<=mapx2;i++)
		for(j=mapy1;j<=mapy2;j++)
		{
			if(map->map[i+j*map->width].item>=IT_TREE1 && map->map[i+j*map->width].item<=IT_TREE3)
			{
#ifdef DIRECTORS
				if(player.var[VAR_KLONKSMASH]<20)
					player.var[VAR_KLONKSMASH]++;
				else
				{
					picToDrop=6;
					if(opt.galPix[picToDrop-1]==0 && player.var[VAR_TEMP+15]==0)
					{
						bullet_t *b;

						b=FireBullet(goodguy->ID,goodguy->x,goodguy->y,FIXAMT*15,64,BLT_ITEM,0);
						if(b)
						{
							player.var[VAR_TEMP+15]=1;
							b->anim=IT_PICTURE;
							b->target=picToDrop;
							NewMessage("Klonk smashed 20 trees!",0);
						}
					}
					picToDrop=0;
				}
#endif
				map->map[i+j*map->width].item=IT_PINESTUMP;
				for(n=0;n<10;n++)
				{
					FireBullet(owner,(i*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(j*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,FIXAMT*20,(rand()%256),BLT_WOODSHARD,DamageAmt(2,GetGuy(owner)->team));
				}
			}
		}
}

byte NotBeingDrained(int who)
{
	int i;

	for(i=0;i<MAX_BULLETS;i++)
	{
		if(bullet[i].type==BLT_DRAINMASTER && bullet[i].owner==who)
			return 0;
	}
	return 1;
}

byte EatItem(int x,int y)
{
	int i;

	for(i=0;i<MAX_BULLETS;i++)
	{
		if(bullet[i].type==BLT_ITEM && Distance(bullet[i].x,bullet[i].y,x,y)<FIXAMT*30)
		{
			bullet[i].type=BLT_NONE;
			return 1;
		}
	}
	return 0;
}

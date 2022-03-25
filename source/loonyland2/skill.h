#ifndef SKILL_H
#define SKILL_H

#include "mgldraw.h"

// skill types
#define ST_PASSIVE	(0)	// always affects things
#define ST_ATTACKP	(1)	// passive, but labeled as the Attack type (and only affects you in Attack mode)
#define ST_THROWP	(2)	// passive, but labeled as the Throw type (and only affects you in Throw mode)
#define ST_MAGICP	(3)	// passive, but labeled as Magic
#define ST_ATTACK	(4)	// a special attack that must be activated a certain way (only works in Attack mode)
#define ST_THROW	(5)	// a special throw that must be activated (only works in Throw mode)
#define ST_MAGIC	(6)	// a magic spell you can pick from the ring of 8
#define ST_CRAFT	(7)	// a crafting skill, activates a menu in the pause menu
#define ST_CRAFTP	(8)	// a crafting skill that's actually a passive skill

// how to activate the skill
#define SA_NONE		(0)	// it's passive, no activation
#define SA_ATTACKJUMP (1)	// press jump while in attack animation
#define SA_JUMPJUMP	(2)	// jump in midair
#define SA_JUMPATTACK (3)	// jump and attack
#define SA_ONHIT	 (4)	// triggers when you hit a guy

// skill effects
#define SE_NONE		(0)	// no effect!?
#define SE_LIFE		(1)	// max life increase by val0
#define SE_ARMOR	(2)	// -val0 damage when hit
#define SE_DODGE	(3)	// val0% chance of not being hit when you're hit
#define SE_RECOVER	(4)	// gain val0 life every val1 ticks
#define SE_CANNIBAL	(5)	// val0% chance of gaining val1 life every kill
#define SE_XP		(6) // add 0-val0% more XP per kill, randomly
#define SE_STRENGTH	(7)	// damage per hit +val0
#define SE_STAMINA	(8)	// stamina meter increase by val0% of your missing health
#define SE_MAGIC	(9)	// magic meter increase by val0
#define SE_RADAR	(10)	// show a radar of offscreen enemies, range val0, refreshes every val1 ticks
#define SE_LUCK		(11)	// val0% increase in % of finding items from enemies, val1 more coins possible to find per kill
#define SE_SPEED	(12)	// attack speed increase by val0
#define SE_STUN		(13)	// stun enemy hit for val0 ticks (decreased by toughness of enemy)
#define SE_CRITICAL (14)	// val0% chance of inflicting val1% extra damage
#define SE_KNOCKBACK (15)	// knock back enemies val0 pixels when hit
#define SE_POISON	(16)	// add val0 seconds of poison to enemy hit
#define SE_SHOCK	(17)	// once every val0 ticks, do val1 damage to the nearest enemy (within a fairly small range) with a jolt
#define SE_SPIKE	(18)	// enter a state where when you hit the ground, you'll launch val0 stone spikes forward
#define SE_WHIRL	(19)	// whirl around hitting all guys around you every val0 frames. Lasts val1 frames.
#define SE_FIRE		(20)	// makes a fire bullet at the spot that lasts val0 ticks
#define SE_VAMP		(21)	// val0% chance to gain val1 life
#define SE_PARRY	(22)	// parry hits for val0 ticks
#define SE_NOVA		(23)	// makes a nova boom that does val0 damage and hits in a radius of val1 (stun and knockback relative to val0)
#define SE_PIERCE	(24)	// thrown axes go through val0 enemies before stopping
#define SE_HOME		(25)	// home in on enemies with val0 accuracy
#define SE_TRIPLE	(26)	// throw mini axes alongside main one.  Rate of fire is val0, not your normal rate
#define SE_RICOCHET (27)	// bounce off val0 walls per throw
#define SE_AIRSTRIKE (28)	// toss an explosive axe that does val0 damage to everyone in val1 radius
#define SE_PERSUADE	(29)	// val0% chance to convert enemies hit to your team for val1 seconds (decreased by enemy level)
#define SE_SHATTER	(30)	// axes release val0 shards (1 dmg each) on impact to hit others
#define SE_MEDITATE	(31)	// recover val0 magic every val1 ticks that you don't move (in addition to normal recovery)
#define SE_RESTORE	(32)	// heal val0 damage, invinc for val1 ticks, cure all, and destroy nearby bullets
#define SE_STOP		(33)	// stop everything evil for val0 ticks
#define SE_SHOCKSHIELD (34)	// do val0 damage to enemies in a radius, pushing them out.  Lasts val1 ticks
#define SE_SUMMON	(35)	// summon a bonehead.  Kills an existing one if you have val0 already
#define SE_SUMBOMB	(36)	// explode all summoned boneheads for val0 damage over val1 radius each
#define SE_BERSERK	(37)	// move double speed for val0 ticks, and have maxed out stamina that whole time
#define SE_TURRET	(38)	// create a turret where you are, with val0 life
#define SE_CRYOZOID (39)	// summon a cryozoid that goes away after val0 seconds
#define SE_BLAH		(40)	// I give up on using SE's... the rest of the skills will be SpecificSkillVal()'d

#define SKILL_NONE		(0)
// passive
#define SKILL_DEFENSE	(1)
#define SKILL_ENERGY	(2)
#define SKILL_TOUGHNESS	(3)
#define SKILL_SURVIVAL	(4)
#define SKILL_DODGE		(5)
#define SKILL_TRAINING	(6)
#define SKILL_STRENGTH	(7)
#define SKILL_ENDURANCE (8)
#define SKILL_TRACKING  (9)
#define SKILL_LUCK		(10)
// attack
#define SKILL_AXEMASTERY (11)
#define SKILL_STUN		(12)
#define SKILL_BASH		(13)
#define SKILL_CRITICAL	(14)
#define SKILL_PERSUADE	(15)
#define SKILL_FLAME		(16)
#define SKILL_VAMPIRE	(17)
#define SKILL_PARRY		(18)
#define SKILL_STOMP		(19)
#define SKILL_WHIRL		(20)
// throwing
#define SKILL_THROWING	(21)
#define SKILL_PIERCE	(22)
#define SKILL_HOMING	(23)
#define SKILL_TRIPLE	(24)
#define SKILL_RICOCHET	(25)
#define SKILL_AIRSTRIKE (26)
#define SKILL_STATIC	(27)
#define SKILL_POISON	(28)
#define SKILL_SHATTER	(29)
#define SKILL_TURRET	(30)
// magic
#define SKILL_SHROOM	(31)
#define SKILL_SPIKE		(32)
#define SKILL_BRAMBLES	(33)
#define SKILL_LIFEBERRY	(34)
#define SKILL_BURNINATE	(35)
#define SKILL_IGNITE	(36)
#define SKILL_TOASTIES	(37)
#define SKILL_HEATSHIELD (38)
#define SKILL_SHOCK		(39)
#define SKILL_BERSERK	(40)
// magic pg2
#define SKILL_GALE		(41)
#define SKILL_TORNADO	(42)
#define SKILL_RESTORE	(43)
#define SKILL_CRYOZOID	(44)
#define SKILL_COLDWAVE	(45)
#define SKILL_ICECUBE	(46)
#define SKILL_DRAIN		(47)
#define SKILL_BONEHEAD	(48)
#define SKILL_BONEBOMB	(49)
#define SKILL_PLAGUE	(50)

#define MAX_SKILLS	(50)

// magic schools
#define SC_NATURE	(0)
#define SC_FIRE		(1)
#define SC_WIND		(2)
#define SC_ICE		(3)
#define SC_DEATH	(4)

#pragma pack(push, 1)
typedef struct skill_t
{
	char name[16];
	char desc[5][64];
	byte type;
	byte activate;	// how the skill is triggered - 1-8 for spells for the 8 spell slots
	byte effect;	// what exactly does the skill do?
	float stamina,staminaUp;	// cost in stamina to use (if passive, cost is paid when attacking in the right mode)
	float magic,magicUp;		// cost in magic to use (if passive, cost is paid when casting any spell)
	float val[2];	// the up to two values this skill uses (value here is what it is with 1 point in it)
	float valUp[2];	// how much they change per level (each extra point adds this much)
	byte capLevel[2];	// the highest level possible for this stat of this skill
} skill_t;
#pragma pack(pop)

void InitialSkills(void);
byte SkillDamageBonus(void);
byte SkillSpeedBonus(void);
byte SkillLifeBonus(void);
byte SkillStaminaBonus(void);
byte SkillMagicBonus(void);
byte SkillArmorBonus(void);
void SkillsUpdate(void);

byte StaminaCost(void);
byte MagicCost(void);

char *GetSkillName(byte n,byte longName);
skill_t *GetSkill(byte n);
byte PickSkillToBoost(byte type);

void LearnSkill(byte s);
byte SkillDodgeRate(void);
word SkillTrainingAmount(void);
word SkillStunAmount(void);
byte SkillHaveASpell(void);
byte SkillHave(byte s);

byte PlagueOn(void);
byte HeatShieldOn(void);
byte HaveTwoOfASchool(void);

float SpecificSkillVal(byte w,byte s);
float SpecificSkillCost(byte w,byte s);
float SpecificSkillValDiminished(byte w,byte s,float f);
byte SpecificSkillLevel(byte s);

#endif

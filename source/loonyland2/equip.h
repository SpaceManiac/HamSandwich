#ifndef EQUIP_H
#define EQUIP_H

#include "mgldraw.h"

#define MAX_EQUIP		(60)

// magical powers
#define EM_NONE		(0)
// axe
#define EM_HANDY 	 (1)	// stamina costs are -1
#define EM_WHISTLING (2)	// flies 50% faster when thrown
#define EM_POISONED	 (3)	// adds 3 seconds of poison on hit
#define EM_PIERCING	 (4)	// penetrates armor 20%
#define EM_FLAMING	 (5)	// tosses out fireballs on attack
#define EM_FROSTY	 (6)	// freezes 1 second on hit
#define EM_VAMPIRIC	 (7)	// 5% chance of sucking 1 health per hit
#define EM_SMASHING	 (8)	// adds a little stun and knockback to hits
#define EM_THORNY	 (9)	// fire out a couple thorns (same as Shatter skill) on hit
#define EM_BEEFY	 (10)	// +5 max stamina
// parka
#define EM_HEALTHY	 (11)	// recover 1 health per 20 seconds
#define EM_ENERGETIC (12)	// recover 1 stamina per 5 seconds
#define EM_STONE	 (13)	// 5% chance to take no damage when hit
#define EM_THORNY2	 (14)	// shoot thorns when you get hit
#define EM_FROSTY2	 (15)	// enemies that hit you in melee are frozen for 1 second
#define EM_SWIFT	 (16)	// increase your movement speed a bit
#define EM_VIBRANT	 (17)	// max life +5
#define EM_CURING	 (18)	// half duration of poison
#define EM_WARM		 (19)	// half duration of freezing
#define EM_LUCKY	 (20)	// equivalent to 1 level in Luck (but stacks with Luck even if Luck is maxed)
// amulet
#define EM_POWERFUL	 (21)	// +5 max magic
#define EM_CHARGED	 (22)	// recover 1 magic per 20 seconds
#define EM_MIGHTY	 (23)	// offensive spells do +20% damage
#define EM_ABSORBING (24)	// gain 1 magic when hit
#define EM_BONEY	 (25)	// boneheads shoot fireballs instead of stabbing, and regenerate health 1 per 10 seconds
#define EM_GUARDIAN	 (26)	// become invincible for 1 second when hit
#define EM_SHOCKING  (27)	// zap very close enemies for 2 damage every 5 seconds, like low-grade shock shield
#define EM_SAFETY	 (28)	// if you have more than 1hp, and a hit would kill you, it leaves you with 1hp
#define EM_MIRRORED	 (29)	// Restoration spell reflects bullets instead of vaporizing them
#define EM_HOTFOOT	 (30)	// Berserk spell leaves flames behind once per second that last 2 seconds
// elite axe
#define EM_POISON2	 (31)	// adds 5 seconds of poison on hit
#define EM_PIERCE2	 (32)	// pierces 40% armor
#define EM_SCORCHING (33)	// fires 3 fireballs at once on attack
#define EM_FREEZING	 (34)	// freezes for 2 seconds on hit
#define EM_VAMPIRIC2 (35)	// 10% chance to suck 1 health per hit
// elite parka
#define EM_HEALTHY2	 (36)	// recover 1 health per 15 seconds
#define EM_STONE2	 (37)	// 10% chance of no damage when hit
#define EM_EXPLOSIVE (38)	// explode when hit, 4 damage
#define EM_WARM2	 (39)	// immune to freezing
#define EM_ELECTRIC	 (40)	// shocks enemies that hit you for 5
// elite amulet
#define EM_ABSORB2	 (41)	// gain 2 magic when hit
#define EM_NATURE	 (42)	// Earth magic costs 2 less magic to cast (min 1)
#define EM_HEALING	 (43)	// healing spells and potions give 25% more benefit
#define EM_FRIGID	 (44)	// casting ice spells freezes all enemies on screen for 1 second
#define EM_THINKING  (45)	// recover 1 magic per 15 seconds
// cursed axe
#define EM_DEFEND	 (46)	// does -3 damage, adds 3 armor
#define EM_MAGICAL	 (47)	// swings -3 speed, +10 Magic
#define EM_MYSTIC	 (48)	// stamina recovers half as fast, magic recovers 50% faster
// cursed parka
#define EM_VIOLENT	 (49)	// -3 armor, +3 damage
#define EM_SICKENING (50)	// -10 Life, your poison does 2 damage per hit
#define EM_ITCHY	 (51)	// -3 armor, +10 Magic
// cursed amulet
#define EM_ANGRY	 (52)	// -3 Magic, +2 damage
#define EM_SOLID	 (53)	// +3 armor, 10% chance for spells to fail but still cost
#define EM_CURSED	 (54)	// lose 1 Magic when hit, +2 armor

#define EM_ARTSTART (55)
// artifacts
#define EM_NEBROGUB	 (55)	// Nebro Gubular Axe - dam12, spd7, randomly freezes, stuns, poisons, or lights a fire on hit,
#define EM_REGEN	 (56)	// Regeneratorizer Amulet - mag7, pwr5, heals all friendlies 2hp per 10 seconds
#define EM_COOLGUY	 (57)	// Coolguy Parka - sta7, def9, gain magic when frozen instead of slowing down
#define EM_LINKIN	 (58)	// Linkin Parka - sta10,def7, 20% to go berserk briefly when hit
#define EM_GHOSTLY	 (59)	// Ghostly Axe - dam7,spd3, makes ghosts with each swing
#define EM_IMAJICA	 (60)	// Imajica Amulet - mag10,pwr10, magic dmg +50%, attack damage/2
#define EM_NUCULAR	 (61)	// Nucular Axe - dam10, spd1, makes a huge explosion on impact doing 10 damage
#define EM_TRAINEE	 (62)	// Trainee Parka - sta2, def2, +50% XP from enemies
#define EM_FATCROW	 (63)	// Fat Crow Amulet - mag10,pwr10, does 100 damage per second to flying enemies.
#define EM_QUESTION	 (64)	// Question Axe - dam10,spd10, 10% better prices
#define EM_PURPLEKOO (65)	// Purplekoo Parka - sta1, def15, makes you purple
#define EM_DEATH	 (66)	// Death Amulet - mag10,pwr10, boney amulet and +1 to all death magic skills
#define EM_JUSTHEF	 (67)	// Justhef Axe - dam5,spd5, gives the stats of the last monster hit onscreen
#define EM_PARALLEL	 (68)	// Parallel Parka - sta10,def6, 1% chance to split enemy in two on hit (non-boss, non-tagged only, tags clone 255)
#define EM_BOKBOK	 (69)	// Bokbok Amulet - mag8, pwr8, each spell cast also summons a suicide bomber Bokbok per level of the spell (dmg 3)
#define EM_WHIRLING (70)	// Whirling Axe - dam8,spd4, extends radius of Whirl skill
#define EM_DARK		(71)	// Dark Parka - sta12, def12, but you can't see much somehow
#define EM_SUMMONERS (72)	// Summoner's Amulet - mag9,pwr8, summoned guys do +3 damage
#define EM_JORD		(73)	// Jord Axe - dmg1,spd1, spells do half damage, earn double XP
#define EM_PARACHUTE (74)	// Parachute Parka - sta10,def10, no damage from falling/water
#define EM_EVILEYE	(75)	// Evil Eye Amulet - mag5, pwr12, sets fire to enemies you look at once per second (1 second burn)
#define EM_PERSUADER (76)	// Persuadertron Axe - dmg3,spd4, 100% success on persuasion
#define EM_OLDLACE	(77)	// Old Lace Parka - sta10, def10, +5 armor when poisoned
#define EM_SUNANGEL	(78)	// Sun Angel Amulet - mag9,pwr9, fire does 2 damage per tick to enemies
#ifdef DIRECTORS
#define EM_DIRECTORS (79)	// Director's Cutter Axe - dam20,spd20, half stamina cost
#define EM_OMELET	(80)	// Omelet Amulet - mag20,pwr20, spell costs halved
#define EM_DOOM		(81)	// Doom Parka - sta20,def20, enemies that hit you suffer horribly
#define EM_BANHAMMER (82)	// Banhammer Axe - dam15,spd1, 5% chance of banning enemies other than bosses
#define EM_SP		(83)	// Sp Amulet - mag10,pwr10, spams scorching fireballs nonstop
#define EM_IMAGINARY (84)	// Imaginary Parka - sta0, def0, 50% chance to dodge
#define EM_ARTEND	(84)
#define MAX_EQMAGIC	 (85)
#else
#define EM_ARTEND	(78)
#define MAX_EQMAGIC	 (79)
#endif

#pragma pack(push, 1)
typedef struct equip_t
{
	byte count;	// how many of this same one do you have?
	byte type;
	byte value;	// damage for axes, defense for parkas, magic boost for rings
	byte sharpened;	// has been upgraded, can't be again
	byte magic;		// magical bonus on magic/golden items
	byte speed;
	byte skill[3];	// which skills it boosts
} equip_t;
#pragma pack(pop)

extern byte bossDrop;

void InitialEquip(void);
void DescribeItem(int x,int y,equip_t *eq);
char *EquipName(equip_t *eq);
byte EquipAdd(byte item,int val1,int val2);
void SortEquipment(void);
void SmithItems(equip_t *e1,equip_t *e2);

void EquipCreateItem(byte level,byte *type,int *val1,int *val2);

void EquipTest(void);
byte EquipCount(byte item);
byte GotItem(byte item);
byte EquipSkillBoost(byte skill);
byte DropItemOfType(byte type,byte count);
byte AddItemOfType(byte type,byte count);
void DropItem(byte itm);
word ItemCost(equip_t *e,byte sell);
char *PotionPrefix(equip_t *eq);
byte SimpleEquipAdd(equip_t *e);
byte JunkCount(void);
void ConvertEquipmentForShow(byte on);
void ConvertShopForShow(byte on,byte shop);
byte StashGive(int i);
byte StashTake(equip_t *e);
byte AllPotionCheck(void);
byte GenerateArtifact(void);

#endif

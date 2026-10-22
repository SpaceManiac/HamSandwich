#include "upgrade.h"
#include <stdio.h>

upgrade_t upgrades[MAX_UPGRADES]={
	{	// 0
		"Explorer",
		255,
		{255,255},
		{"  (Skill) Tracking: Detect creatures and items",
		 "  (Skill) Survival: Passively recover health",
		 "(Talent) Mobility: Increased movement speed",
		 ""}
	},
	{	// 1
		"Merchant",
		255,
		{255,255},
		{"  (Skill) Luck: Enemies drop more items and money",
		 "(Talent) Haggling: Buy from shops at lower prices",
		 "(Talent) Miser: Sell to shops at higher prices",
		 "(Talent) Packrat: Enemies drop more Junk"}
	},
	{	// 2
		"Craftsman",
		255,
		{255,255},
		{"  (Skill) Training: Enemies give bonus XP",
		 "(Talent) Clockwork: Build friendly Clockbots",
		 "(Talent) Junksmith: Turn Junk into items",
		 "(Talent) Alchemy: Brew herbs into potions"}
	},
	{	// 3
		"Expert",
		255,
		{2,255},
		{"  (Skill) Toughness: Increased maximum health",
		 "(Talent) Mining: Find more items when digging",
		 "(Talent) Herbalism: Find more herbs",
		 "(Talent) Mixology: Potions are more effective"}
	},
	{	// 4
		"Scholar",
		255,
		{255,255},
		{"  (Skill) Energy: Increased max magic and magic recovery",
		 "(Talent) Meditation: Increased magic recovery when idle",
		 "(Talent) Mental Focus: Increased spell damage",
		 ""}
	},
	{	// 5
		"Duelist",
		1,
		{255,255},
		{"  (Skill) Stun: Jump attacks stun enemies",
		 "  (Skill) Critical: Melee attacks chance for bonus damage",
		 "(Talent) Chopping: Melee attacks pierce enemy armor",
		 ""}
	},
	{	// 6
		"Duelist",
		2,
		{5,255},
		{"  (Skill) Bash: Jump attacks deal extra damage, knockback",
		 "  (Skill) Strength: Melee attacks deal extra damage",
		 "",
		 ""}
	},
	{	// 7
		"Duelist",
		3,
		{6,255},
		{"  (Skill) Defense: Enemy attacks deal less damage",
		 "(Talent) Criticism: Critical hits pierce enemy armor",
		 "",
		 ""}
	},
	{	// 8
		"Duelist",
		4,
		{7,255},
		{"  (Skill) Poison: Thrown axes poison enemies",
		 "  (Skill) Parry: Melee swings can deflect an enemy attack",
		 "",
		 ""}
	},
	{	// 9
		"Duelist",
		5,
		{8,255},
		{"  (Skill) Persuasion: Jump attacks can beguile enemies",
		 "(Talent) Finesse: Stun enemies who are Dodged or Parried",
		 "",
		 ""}
	},
	{	// 10
		"Gladiator",
		1,
		{255,255},
		{"  (Skill) Axe Mastery: Melee attacks faster, less stamina",
		 "  (Skill) Vampirism: Melee hits might recover health",
		 "(Talent) Sneak Attack: Bonus damage vs. undamaged foes",
		 ""}
	},
	{	// 11
		"Gladiator",
		2,
		{10,255},
		{"  (Skill) Whirl: Damages enemies around you",
		 "  (Skill) Dodge: Chance to avoid enemy attacks",
		 "",
		 ""}
	},
	{	// 12
		"Gladiator",
		3,
		{11,255},
		{"  (Skill) Endurance: Keep stamina when hit, fast recovery",
		 "(Talent) Stoneskin: Increases your defense",
		 "",
		 ""}
	},
	{	// 13
		"Gladiator",
		4,
		{12,255},
		{"  (Skill) Stomp: Area effect damage, stun, and knockback",
		 "  (Skill) Flame: Jump and Whirl attacks create flames",
		 "",
		 ""}
	},
	{	// 14
		"Gladiator",
		5,
		{13,255},
		{"  (Skill) Air Strike: Jump throw for massive area damage",
		 "(Talent) Knockout: Extra duration when stunning enemies",
		 "",
		 ""}
	},
	{	// 15
		"Marksman",
		1,
		{255,255},
		{"  (Skill) Throwing: Throw attacks faster, less stamina",
		 "  (Skill) Piercing: Thrown axes pass through enemies",
		 "(Talent) Scavenging: Chance to gain health when killing",
		 ""}
	},
	{	// 16
		"Marksman",
		2,
		{15,255},
		{"  (Skill) Static: Thrown axes zap nearby enemies",
		 "  (Skill) Triple: Axes are thrown with bonus smaller axes",
		 "",
		 ""}
	},
	{	// 17
		"Marksman",
		3,
		{16,255},
		{"  (Skill) Shatter: Thrown axes spray shards when hitting",
		 "(Talent) Vivacity: All attacks have decreased stamina cost",
		 "",
		 ""}
	},
	{	// 18
		"Marksman",
		4,
		{17,255},
		{"  (Skill) Ricochet: Axes rebound off walls",
		 "  (Skill) Axe Trap: Plant turrets that throw axes",
		 "",
		 ""}
	},
	{	// 19
		"Marksman",
		5,
		{18,255},
		{"  (Skill) Homing: Thrown axes seek nearby enemies",
		 "(Talent) Tossing: Thrown axes push enemies back",
		 "",
		 ""}
	},
	{	// 20
		"Enchant",
		1,
		{255,255},
		{" (Spell) Lifeberries: Bushes spray healing berries",
		 " (Spell) Ignite: Enemies have reduce armor, drop flames",
		 "(Talent) Green Thumb: Casting Nature spells recovers HP",
		 "(Talent) Combustion: Flames damage enemies more often"}
	},
	{	// 21
		"Enchant",
		2,
		{20,25},
		{" (Spell) Restoration: Recover health and status, unhittable",
		 " (Spell) Brambles: Poisons enemies and interrupts attacks",
		 "(Talent) Green Thumb: Casting Nature spells recovers HP",
		 "(Talent) Arsenic: Poisoned enemies have reduced armor"}
	},
	{	// 22
		"Enchant",
		3,
		{21,26},
		{" (Spell) Cold Wave: Damages and freezes enemies",
		 " (Spell) Tornado: Sucks enemies into a vortex",
		 "(Talent) Freezing: Frozen enemies take damage over time",
		 "(Talent) Windbag: Casting Wind spells costs less magic"}
	},
	{	// 23
		"Enchant",
		4,
		{22,27},
		{" (Spell) Berserk: Move at double speed, limitless stamina",
		 " (Spell) Heat Shield: Hurt attackers, burn shots, cheap Fire",
		 "(Talent) Scorching: Casting Fire spells sprays fireballs",
		 ""}
	},
	{	// 24
		"Enchant",
		5,
		{23,28},
		{" (Spell) Plague: Poisons enemies, dying enemies create rats",
		 " (Spell) Shroom: Turns enemies into helpless Shrooms",
		 "(Talent) Arsenic: Poisoned enemies have reduced armor",
		 "(Talent) Green Thumb: Casting Nature spells recovers HP"}
	},
	{	// 25
		"Sorcery",
		1,
		{255,255},
		{" (Spell) Cryozoid: Summoned beast damages and freezes",
		 " (Spell) Drain: Deals damage through armor, heals you",
		 "(Talent) Freezing: Frozen enemies take damage over time",
		 "(Talent) Spookiness: Killing with Death Magic creates ghosts"}
	},
	{	// 26
		"Sorcery",
		2,
		{20,25},
		{" (Spell) Bonehead: Summoned skeletons attack enemies",
		 " (Spell) Toasties: Summoned spirits shoot enemies, explode",
		 "(Talent) Spookiness: Killing with Death Magic creates ghosts",
		 "(Talent) Scorching: Casting Fire spells sprays fireballs"}
	},
	{	// 27
		"Sorcery",
		3,
		{21, 26},
		{" (Spell) Gale: Blows away enemies and projectiles",
		 " (Spell) Bone Bomb: Boneheads extra damage, can explode",
		 "(Talent) Windbag: Casting Wind spells costs less magic",
		 "(Talent) Spookiness: Killing with Death Magic creates ghosts"}
	},
	{	// 28
		"Sorcery",
		4,
		{22,27},
		{" (Spell) Stone Spike: Waves of rock damage enemies",
		 " (Spell) Shock Orb: Slow-moving orb that zaps enemies",
		 "(Talent) Electrocution: All electric attacks zap faster",
		 ""}
	},
	{	// 29
		"Sorcery",
		5,
		{23,28},
		{" (Spell) Ice Cube: Long-lasting area damage and freeze",
		 " (Spell) Burninate: Huge fireball sprays flames everywhere",
		 "(Talent) Freezing: Frozen enemies take damage over time",
		 "(Talent) Combustion: Flames damage enemies more often"}
	},
};


void PrintUpgradeName(byte n, char* s)
{
	upgrade_t* u;

	if (n >= MAX_UPGRADES)
	{
		sprintf(s, "????");
		return;
	}
	
	u = GetUpgrade(n);
	if (u->level == 255)
		sprintf(s, u->name);
	else
		sprintf(s, "%s %d", u->name, u->level);
}

void PrintRequirements(byte n, char* s)
{
	upgrade_t* u;
	char s1[16];
	char s2[16];

	if (n >= MAX_UPGRADES)
	{
		sprintf(s, "????");
		return;
	}

	u = GetUpgrade(n);
	if (u->req[0] == 255)
		sprintf(s, "No Requirements");
	else if (u->req[1] == 255)
	{
		PrintUpgradeName(u->req[0], s1);
		sprintf(s, "Requires %s", s1);
	}
	else
	{
		PrintUpgradeName(u->req[0], s1);
		PrintUpgradeName(u->req[1], s2);
		sprintf(s, "Requires %s or %s", s1, s2);
	}
}

upgrade_t* GetUpgrade(byte n)
{
	if (n >= MAX_UPGRADES)
		return NULL;
	return &upgrades[n];
}

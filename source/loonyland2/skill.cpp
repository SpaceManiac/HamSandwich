#include "skill.h"
#include "player.h"
#include "guy.h"
#include "alchemy.h"
#include "glasses.h"
#include "madcap.h"
#include "achieve.h"

skill_t skills[MAX_SKILLS+1]={
	{
		"None",
		{"Indescribable.",
		 "",
		 "",
		 "",
		 ""},
		 ST_PASSIVE,
	},
	{	// #1
		"Defense",
		{"With practice, you can toughen yourself up against attack.",
		 "Or just put points into this skill!",
		 "",
		 "",
		 "(-1 damage taken per level)"},
		ST_PASSIVE,
		SA_NONE,
		SE_ARMOR,
		0,0,	// stamina
		0,0,	// magic
		{1,0},
		{1,0},
		{18,18},	// max level
	},
	{	// #2
		"Energy",
		{"Magic is all about balancing unstable ethereal forces with",
		 "the power of your mind.  Increase that power with this",
		 "skill!",
		 "",
		 "(+2 Magic and Magic regenerates 5% faster per level)"},
		ST_PASSIVE,
		SA_NONE,
		SE_MAGIC,
		0,0,	// stamina
		0,0,	// magic
		{2,5},
		{2,5},
		{18,18},	// max level
	},
	{	// #3
		"Toughness",
		{"With a proper training regimen, anyone can increase their",
		 "maximum life.  That is, they could if there was such a",
		 "thing as 'maximum life'.",
		 "",
		 "(+3 per level)"},
		ST_PASSIVE,
		SA_NONE,
		SE_LIFE,
		0,0,	// stamina
		0,0,	// magic
		{3,0},
		{3,0},
		{18,18},	// max level
	},
	{	// #4
		"Survival",
		{"Learn to eat twigs and snow, improving your ability to",
		 "survive in this harsh climate.  With the right seasoning,",
		 "bark tastes just like chicken!",
		 "(Heals every 25 seconds, 2 seconds less per level,",
		 "Heals 1 Life plus 1/4 per level)"},
		ST_PASSIVE,
		SA_NONE,
		SE_RECOVER,
		0,0,	// stamina
		0,0,	// magic
		{1,25*30},
		{0.25f,-60},
		{18,13},	// max level
	},
	{	// #5
		"Dodge",
		{"Leap aside when attacked, so swiftly that you don't",
		 "appear to be moving at all.  It's amazing!",
		 "",
		 "",
		 "(+2% chance per level of avoiding attacks)"},
		ST_PASSIVE,
		SA_NONE,
		SE_DODGE,
		0,0,	// stamina
		0,0,	// magic
		{2,0},
		{2,0},
		{18,18},	// max level
	},
	{	// #6
		"Training",
		{"A skilled hunter studies himself and learns something from",
		 "every experience.  You want to be a skilled hunter, don't",
		 "you?",
		 "",
		 "(Gain random extra XP per monster, up to 20% per level)"},
		ST_PASSIVE,
		SA_NONE,
		SE_XP,
		0,0,	// stamina
		0,0,	// magic
		{20,0},
		{20,0},
		{18,18},	// max level
	},
	{	// #7
		"Strength",
		{"By training your physical might, you will eventually become",
		 "strong enough to press the Fire button and level up this",
		 "skill, which increases the damage you do with axes, both",
		 "attacking and throwing (half as much when throwing).",
		 "(+1 Damage per level, +1 Stamina cost per level)"},
		ST_PASSIVE,
		SA_NONE,
		SE_STRENGTH,
		0,0,	// stamina
		0,0,	// magic
		{1,0},
		{1,0},
		{18,18},	// max level
	},
	{	// #8
		"Endurance",
		{"With training, you can become resistant to the pain of",
		 "injuries, and recover better when you are tired.  This skill",
		 "makes you lose less Stamina when injured, and recover",
		 "Stamina more quickly.",
		 "(10% less per level, and slightly faster)"},
		ST_PASSIVE,
		SA_NONE,
		SE_STAMINA,
		0,0,	// stamina
		0,0,	// magic
		{10,1.5f},
		{10,1.5f},
		{10,12},	// max level
	},
	{	// #9
		"Tracking",
		{"A true woodsman is aware of every snapped twig and",
		 "rustling branch.  Not being a true woodsman, you can",
		 "just look at the big glowing dots around the edges of the",
		 "screen to find monsters.  Finds other stuff at high",
		 "levels. (Range and accuracy increase with level)"},
		ST_CRAFTP,
		SA_NONE,
		SE_RADAR,
		0,0,	// stamina
		0,0,	// magic
		{20,30*3+1},
		{20,-10},
		{18,10},	// max level
	},
	{	// #10
		"Luck",
		{"The skill of luck?!  Increase your odds of finding items on",
		 "enemies, and the amount of money you find.",
		 "",
		 "",
		 "(25% better chance per level)"},
		ST_PASSIVE,
		SA_NONE,
		SE_LUCK,
		0,0,	// stamina
		0,0,	// magic
		{25,1},
		{25,0.5f},
		{18,18},	// max level
	},
	{	// #11
		"Axe Mastery",
		{"Become one with your axe.  As you begin to understand",
		 "its special needs, and begin to call it Beowulf, you'll",
		 "realize you're going snow-crazy.  But you'll be able to",
		 "swing it faster and more easily.",
		 "(+1 Speed per level, -1 Stamina cost per level)"},
		ST_ATTACKP,
		SA_NONE,
		SE_SPEED,
		-1,-1,	// stamina
		0,0,	// magic
		{1,0},
		{1,0},
		{18,18},	// max level
	},
	{	// #12
		"Stun",
		{"Stun enemies with your jumping attacks.",
		 "Increases the Stamina cost of jump attacks.",
		 "Tougher enemies will not stay stunned as long.",
		 "",
		 "(1/8 second per level, +1/2 Stamina cost per level)"},
		ST_ATTACKP,
		SA_JUMPATTACK,
		SE_STUN,
		0.5f,0.5f,	// stamina
		0,0,	// magic
		{3.25f,0},
		{3.25f,0},
		{18,18},	// max level
	},
	{	// #13
		"Bash",
		{"Smash enemies away with your jumping attacks.",
		 "Increases the Stamina cost of jump attacks.",
		 "Does extra damage too!",
		 "(1 foot per level, costs 1 Stamina per level,",
		 "+1 Damage per level)"},
		ST_ATTACKP,
		SA_JUMPATTACK,
		SE_KNOCKBACK,
		1,1,	// stamina
		0,0,	// magic
		{10,1},
		{4,1},
		{18,18},	// max level
	},
	{	// #14
		"Critical",
		{"If you examine your foes carefully, you will find weak",
		 "points.  With practice, you can hit them just right to",
		 "send them crying home to Momma.",
		 "",
		 "(+2% chance per level of doing 150% damage)"},
		ST_ATTACKP,
		SA_NONE,
		SE_CRITICAL,
		0,0,	// stamina
		0,0,	// magic
		{2,50},
		{2,0},
		{18,18},	// max level
	},
	{	// #15
		"Persuasion",
		{"Clonking an enemy on the head with a jump attack, you",
		 "may confuse them into thinking they are your friend.  For",
		 "a moment.  Tougher ones figure it out sooner.",
		 "(+3% chance and lasts longer per level, costs 1 Magic per",
		 "level when it succeeds)"},
		ST_ATTACK,
		SA_ONHIT,
		SE_PERSUADE,
		0,0,	// stamina
		1,1,	// magic
		{3,90},
		{3,90},
		{18,18},	// max level
	},
	{	// #16
		"Flame",
		{"In this wintry landscape, sometimes you need to warm up.",
		 "Help your foes do so with this handy skill!  Anytime you",
		 "hit someone with a jumping attack, a fire will start under",
		 "their feet.  Also works with Whirl attacks.",
		 "(Lasts 1/2 second per level, costs 1 Magic per 2 levels)"},
		ST_ATTACK,
		SA_ONHIT,
		SE_FIRE,
		0,0,	// stamina
		1,0.5f,	// magic
		{15,0},
		{15,0},
		{18,18},	// max level
	},
	{	// #17
		"Vampirism",
		{"Don't try to figure out the logic.  Hitting guys with your",
		 "axe has a chance of giving you Life.  Just roll with it.",
		 "Do I come in your house and criticize the curtains?",
		 "",
		 "(1% chance per level of gaining 1 Life)"},
		ST_ATTACK,
		SA_ONHIT,
		SE_VAMP,
		0,0,	// stamina
		0,0,	// magic
		{1,1},
		{1,0},
		{18,18},	// max level
	},
	{	// #18
		"Parry",
		{"A properly timed swing can deflect incoming attacks.",
		 "Better yet, it can reflect enemy projectiles! You can only",
		 "parry one attack with each of your attacks, though.",
		 "(Lasts 1/15 second per level, can parry once per 2",
		 "seconds minus 1/10 second per level)"},
		ST_ATTACKP,
		SA_NONE,
		SE_PARRY,
		0,0,	// stamina
		0,0,	// magic
		{2,3},
		{2,3},
		{18,18},	// max level
	},
	{	// #19
		"Stomp",
		{"Feeling surrounded?  Jump, then press Jump again to slam",
		 "into the ground, releasing a shockwave that hurts, stuns,",
		 "and knocks away all nearby enemies. Only works in Attack",
		 "mode. (Costs 5 Stamina plus 3 per level.  Damage, stun,",
		 "and radius increase with level)"},
		ST_ATTACK,
		SA_JUMPJUMP,
		SE_NOVA,
		5,3,	// stamina
		0,0,	// magic
		{1,32*2},
		{0.5f,16},
		{18,18},	// max level
	},
	{	// #20
		"Whirl",
		{"If you're tired of smacking guys in the ordinary way,",
		 "tap Jump during an attack to whirl around, hitting all",
		 "nearby enemies.  You can even glide around as you do it!",
		 "(Lasts 1/4 second per level, costs 3 Stamina per level,",
		 "hits more often per level)"},
		ST_ATTACK,
		SA_ATTACKJUMP,
		SE_WHIRL,
		3,3,	// stamina
		0,0,	// magic
		{15,7.5f},
		{-0.5f,7.5f},
		{18,18},	// max level
	},
	{	// #21
		"Throwing",
		{"Throw your axe instead of swinging it!  A novel concept.",
		 "Damage is half as much as attacking, but at least you",
		 "don't have to get all up in their grill to do it.",
		 "(+1 Speed per level, increased Range per level, -1 Stamina",
		 "cost per level)"},
		ST_THROWP,
		SA_NONE,
		SE_SPEED,
		-1,-1,	// stamina
		0,0,	// magic
		{1,0},
		{1,0},
		{18,18},	// max level
	},
	{	// #22
		"Piercing",
		{"A truly fierce throw doesn't stop at one monster!  It",
		 "keeps on going, and going, and going...",
		 "",
		 "",
		 "(Throws go through 1 enemy per level)"},
		ST_THROWP,
		SA_NONE,
		SE_PIERCE,
		0,0,	// stamina
		0,0,	// magic
		{1,0},
		{1,0},
		{18,18},	// max level
	},
	{	// #23
		"Homing",
		{"If you... okay, there's no explanation for it.  Your axes",
		 "home in on enemies when thrown.  That's just how it is.",
		 "",
		 "",
		 "(Accuracy increases with level)"},
		ST_THROWP,
		SA_NONE,
		SE_HOME,
		0,0,	// stamina
		0,0,	// magic
		{1,0},
		{1,0},
		{18,18},	// max level
	},
	{	// #24
		"Triple",
		{"A skilled craftsman can make miniature axe replicas,",
		 "suitable for collecting.  A less skilled one, like you,",
		 "can make ones suitable for throwing alongside normal axes.",
		 "They have a percentage of the normal axe's abilities.",
		 "(10% of full power per level, +1/2 Stamina cost per level)"},
		ST_THROWP,
		SA_NONE,
		SE_TRIPLE,
		0.5f,0.5f,	// stamina
		0,0,	// magic
		{0.1f,0},
		{0.1f,0},
		{10,18},	// max level
	},
	{	// #25
		"Ricochet",
		{"A properly aimed axe can actually bounce off of a wall",
		 "and keep going.  At least in this game.",
		 "",
		 "",
		 "(Bounce 1 time per level)"},
		ST_THROWP,
		SA_NONE,
		SE_RICOCHET,
		0,0,	// stamina
		0,0,	// magic
		{1,0},
		{1,0},
		{18,18},	// max level
	},
	{	// #26
		"Air Strike",
		{"Jump and throw an axe to make it slam into the ground",
		 "and explode, hurting all enemies in a radius.",
		 "",
		 "(4 Damage plus 1.5 per level, radius increases with",
		 "level, costs 3 Stamina per level)"},
		ST_THROW,
		SA_JUMPATTACK,
		SE_AIRSTRIKE,
		3,3,	// stamina
		0,0,	// magic
		{4,32},
		{1.5f,8},
		{18,18},	// max level
	},
	{	// #27
		"Static",
		{"Rub the axe on your hair briskly before throwing, and it",
		 "will shock enemies as it flies by.",
		 "Increases the Stamina cost of throwing.",
		 "",
		 "(Shocks more often per level, +1 Stamina per 2 levels)"},
		ST_THROWP,
		SA_NONE,
		SE_SHOCK,
		0.5f,0.5f,	// stamina
		0,0,	// magic
		{35,1},
		{-1,0},
		{18,18},	// max level
	},
	{	// #28
		"Poison",
		{"The proper mixture of decidedly improper ingredients",
		 "creates a deadly poison to coat your axe.",
		 "Increases the Stamina cost of throwing, since you need",
		 "to mix up a batch with each throw.",
		 "(Lasts 1 second per level, +1 Stamina per 2 levels)"},
		ST_THROWP,
		SA_NONE,
		SE_POISON,
		0.5f,0.5f,	// stamina
		0,0,	// magic
		{30,0},
		{30,0},
		{18,18},	// max level
	},
	{	// #29
		"Shatter",
		{"Sometimes quality is not the answer.  A poorly made axe",
		 "will shatter into pointy bits on impact, hurting others.",
		 "Increases the Stamina cost of throwing.",
		 "",
		 "(3 shards plus 1 per level, +1 Stamina per 2 levels)"},
		ST_THROWP,
		SA_NONE,
		SE_SHATTER,
		0.5f,0.5f,	// stamina
		0,0,	// magic
		{3,0},
		{1,0},
		{18,18},	// max level
	},
	{	// #30
		"Axe Trap",
		{"Press Jump while in the air to place an Axe Trap on the",
		 "ground. It flings axes at any enemies who come near.",
		 "It loses Life when hit, when throwing an Axe, and slowly",
		 "over time.  Only works in Throwing mode.",
		 "(10 Life per level, costs 2 Magic per level)"},
		ST_THROW,
		SA_JUMPJUMP,
		SE_TURRET,
		0,0,	// stamina
		2,2,	// magic
		{10,0},
		{10,0},
		{18,18},	// max level
	},
	//-----------------------
	{	// #31
		"Shroom",
		{"When the monsters seem just too tough to handle, turn",
		 "them into something easier!  They change back quickly,",
		 "so beat on them while you can.  You get almost nothing",
		 "at all for defeating a mushroom, understandably.",
		 "(Costs 2 Magic per level, lasts 1 second per level)"},
		ST_MAGIC,
		SA_NONE,
		SE_STOP,
		0,0,	// stamina
		2,2,	// magic
		{30,100},
		{30,50},
		{18,18},	// max level
	},
	{	// #32
		"Stone Spike",
		{"Call on the power of the earth to smite your foes.",
		 "Huge stalagmites rip from the ground in a wave, impaling",
		 "the unwary.",
		 "(Costs 5 Magic plus 2 per level, 1 Spike per level,",
		 "Spikes do 5 damage plus 1 per 2 levels)"},
		ST_MAGIC,
		SA_NONE,
		SE_SPIKE,
		0,0,	// stamina
		5,2,	// magic
		{1,5},
		{1,0.5f},
		{18,18},	// max level
	},
	{	// #33
		"Brambles",
		{"Exercise your green thumb by tossing out a huge patch",
		 "of brambly bushes.  They'll trip up and poison enemies",
		 "that step on them.",
		 "(Costs 3 Magic plus 1 per level, brambles grow over more",
		 "area per level, live 2 seconds per level)"},
		ST_MAGIC,
		SA_NONE,
		SE_BLAH,
		0,0,	// stamina
		3,1,	// magic
		{2,60},
		{0.5f,60},
		{18,18},	// max level
	},
	{	// #34
		"Lifeberries",
		{"Who doesn't love the fresh clean taste of Lifeberries?",
		 "This spell grows a Lifeberry Bush which spits out berries",
		 "that anyone can eat to gain Life and Magic.",
		 "(Plant lasts 5 seconds per level, berries heal 1 life per 2",
		 "levels, and always give 1 Magic. Costs 2 Magic per level)"},
		ST_MAGIC,
		SA_NONE,
		SE_NONE,
		0,0,	// stamina
		2,2,	// magic
		{1,5},
		{0.5f,5},
		{18,18},	// max level
	},
	{	// #35
		"Burninate",
		{"An enormous fireball that would make Kid Mystic green",
		 "with envy.  Excellent against peasant villages.",
		 "(Costs 10 Magic plus 3 per level, drops flames lasting",
		 "1 second per level, asplodes for 7 damage per level,",
		 "spewing 5 more flames per level)"},
		ST_MAGIC,
		SA_NONE,
		SE_BLAH,
		0,0,	// stamina
		10,3,	// magic
		{7,30},
		{7,30},
		{18,18},	// max level
	},
	{	// #36
		"Ignite",
		{"Fun at parties - set the nearest enemy on fire.  It'll",
		 "move double speed, but leave a trail of deadly flames",
		 "all over as it does.",
		 "(Costs 3 Magic plus 1 per level, 2 seconds per level,",
		 "lit enemy has 10% less armor per level)"},
		ST_MAGIC,
		SA_NONE,
		SE_BLAH,
		0,0,	// stamina
		3,1,	// magic
		{2*30,10},
		{2*30,10},
		{18,10},	// max level
	},
	{	// #37
		"Toasties",
		{"If it gets too cold for you, summon some Toasties!  They",
		 "will hover in a circle around you, shooting at enemies.",
		 "If you get hit, one of the Toasties will explode.",
		 "(Costs 2 Magic per level, 2 Toasties plus 1 per 2 levels,",
		 "Explosion does 4 damage plus 2 per level)"},
		ST_MAGIC,
		SA_NONE,
		SE_BLAH,
		0,0,	// stamina
		2,2,	// magic
		{2,4},
		{0.6f,2},
		{18,18},	// max level
	},
	{	// #38
		"Heat Shield",
		{"Spontaneously combust!  For a while, you'll be so hot",
		 "that you may melt enemy projectiles, and do damage to",
		 "anyone who attacks you.  Fire Magic is cheaper when",
		 "heated. (3 Seconds per level, costs 3 Magic per level,",
		 "reduces Fire spell costs by 1 Magic per level)"},
		ST_MAGIC,
		SA_NONE,
		SE_BLAH,
		0,0,	// stamina
		3,3,	// magic
		{2,30*3},
		{1,30*3},
		{18,18},	// max level
	},
	{	// #39
		"Shock Orb",
		{"Harness the power of lightning in a ball and toss it.",
		 "As it drifts away, it shocks nearby enemies.",
		 "",
		 "(Costs 2 Magic per level, does 1 damage per level,",
		 "flies for 3 seconds plus 1 per level)"},
		ST_MAGIC,
		SA_NONE,
		SE_SHOCKSHIELD,
		0,0,	// stamina
		2,2,	// magic
		{1,30*3},
		{1,30*1},
		{18,18},	// max level
	},
	{	// #40
		"Berserk",
		{"For the wizard who's really more the jock type.  Move at",
		 "double speed for a while, and maintain full Stamina the",
		 "entire time.",
		 "You lose all Stamina when Berserk ends.",
		 "(Costs 3 Magic per level, lasts 1 second per level)"},
		ST_MAGIC,
		SA_NONE,
		SE_BERSERK,
		0,0,	// stamina
		3,3,	// magic
		{30*2,0},
		{30*2,0},
		{18,18},	// max level
	},
	{	// #41
		"Gale",
		{"Blow your enemies away, literally.  The hurricane will",
		 "smash them into obstacles and each other for damage",
		 "and stunning, not to mention push them away from you",
		 "very nicely.  Also blows bullets away.",
		 "(Costs 2 Magic per level, blows harder with levels)"},
		ST_MAGIC,
		SA_NONE,
		SE_BLAH,
		0,0,	// stamina
		2,2,	// magic
		{FIXAMT/2.0f,FIXAMT/4.0f},
		{FIXAMT/4.0f,FIXAMT/4.0f},
		{18,18},	// max level
	},
	{	// #42
		"Tornado",
		{"We're not in Kansas anymore... This spell summons a",
		 "tornado that sucks in all nearby enemies.  It won't",
		 "hurt them, but bunched-up enemies make nice targets!",
		 "(Costs 2 Magic per level, lasts 1 second per level,",
		 "radius increases with level)"},
		ST_MAGIC,
		SA_NONE,
		SE_BLAH,
		0,0,	// stamina
		2,2,	// magic
		{60,1},
		{30,1},
		{18,18},	// max level
	},
	{	// #43
		"Restoration",
		{"Restore balance in the universe, in a very selfish sense.",
		 "This spell restores some of your Life, makes you briefly",
		 "invincible, and cures you of poison and cold.  It even",
		 "vaporizes nearby enemy projectiles!",
		 "(Costs 3 Magic per level, heals 2 Life per level)"},
		ST_MAGIC,
		SA_NONE,
		SE_RESTORE,
		0,0,	// stamina
		3,3,	// magic
		{2,30},
		{2,15},
		{18,18},	// max level
	},
	{	// #44
		"Cryozoid",
		{"Summon forth a dread beast from beneath the earth.",
		 "For a time, it will pelt foes with freezing bolts, then",
		 "return to the nightmarish realm from whence it squirmed.",
		 "(Costs 2 Magic per level, stays 5 seconds plus 2",
		 "per level, fire rate and Damage improve with levels)"},
		ST_MAGIC,
		SA_NONE,
		SE_CRYOZOID,
		0,0,	// stamina
		2,2,	// magic
		{5,40},
		{2,-3},
		{18,13},	// max level
	},
	{	// #45
		"Cold Wave",
		{"Fires a wave of ice that passes through enemies,",
		 "freezing and damaging them as it goes.",
		 "Has no effect on enemies that are already frozen.",
		 "(Costs 2 Magic per level, does 2 damage per level,",
		 "freezes for 1 second plus 1/2 second per level)"},
		ST_MAGIC,
		SA_NONE,
		SE_BLAH,
		0,0,	// stamina
		2,2,	// magic
		{2,1},
		{2,0.5f},
		{18,18},	// max level
	},
	{	// #46
		"Ice Cube",
		{"A celestial storm of oddly rectilinear ice crystals!",
		 "",
		 "(Lasts 5 seconds plus 1 per level, cubes do 1 damage",
		 "per level, radius increases with levels, costs 3",
		 "Magic per level)"},
		ST_MAGIC,
		SA_NONE,
		SE_BLAH,
		0,0,	// stamina
		3,3,	// magic
		{5,64},
		{1,12},
		{18,14},	// max level
	},
	{	// #47
		"Drain",
		{"Continuously sucks the Life from an enemy, giving it",
		 "to you.  The connection breaks if you get too far",
		 "away.  The damage done ignores enemy Armor.",
		 "(Costs 3 Magic plus 2 per level, lasts 2 seconds per",
		 "level, does 1/2 damage per level, always heals 1 Life)"},
		ST_MAGIC,
		SA_NONE,
		SE_BLAH,
		0,0,	// stamina
		3,2,	// magic
		{1,60},
		{0.5f,60},
		{18,18},	// max level
	},
	{	// #48
		"Bonehead",
		{"If you are feeling lonely, cast this spell to get yourself a",
		 "skinny and dimwitted friend.  You can only control a",
		 "limited number of them. They'll even collect items for you!",
		 "(Costs 5 Magic plus 2 per level, 1 Bonehead per level,",
		 "Boneheads get faster and gain Life and Armor with levels)"},
		ST_MAGIC,
		SA_NONE,
		SE_SUMMON,
		0,0,	// stamina
		5,1,	// magic
		{1,128},
		{1,12.8f},
		{18,10},	// max level
	},
	{	// #49
		"Bone Bomb",
		{"Tired of your new skeletal pals?  Blow them up!",
		 "Has no effect if you have no Boneheads summoned.",
		 "Leveling this skill up also boosts Bonehead strength.",
		 "(Costs 2 Magic plus 1 per level, 3 damage +2 per",
		 "level, radius increases with level)"},
		ST_MAGIC,
		SA_NONE,
		SE_SUMBOMB,
		0,0,	// stamina
		2,1,	// magic
		{3,32},
		{2,16},
		{18,18},	// max level
	},
	{	// #50
		"Plague",
		{"It's like a nightmare, or a plague, or a nightmare about",
		 "a plague!  Infects you with the plague, so all attacks",
		 "poison enemies, and within a radius, poison does triple",
		 "damage and slain foes unleash exploding Plague Rats.",
		 "(Costs 5 Magic +2 per level, lasts 5 seconds +2 per level)"},
		ST_MAGIC,
		SA_NONE,
		SE_BLAH,
		0,0,	// stamina
		5,2,	// magic
		{5*30,96},
		{2*30,16},
		{18,18},	// max level
	},
};

void InitialSkills(void)
{
	int i;

	for(i=0;i<PLAYER_SKILLS;i++)
	{
		player.skillHave[i]=0;	// no skill
		player.skillLvl[i]=0;	// and no points in it
		player.skillClock[i]=0;	// and empty the clock for it
	}
	if(ModifierOn(MOD_EXPERT))
	{
		for(i=0;i<PLAYER_SKILLS;i++)
		{
			if(i>0)
				player.skillHave[i]=i;	// no skill
			else
				player.skillHave[i]=SKILL_PLAGUE;
			player.skillLvl[i]=0;	// and no points in it
			player.skillClock[i]=0;	// and empty the clock for it
		}
	}
}

byte PassiveSkillIsActive(byte skill)
{
	// only affects jumping attacks
	if(skills[skill].activate==SA_JUMPATTACK && goodguy && goodguy->z==0)
		return 0;

	return ((skills[skill].type==ST_PASSIVE) || (skills[skill].type==ST_MAGICP) ||
			(skills[skill].type==ST_ATTACKP && player.axeMode==0) ||
			(skills[skill].type==ST_THROWP && player.axeMode==1));
}

byte SkillLevel(byte s)
{
	byte lvl;

	lvl=player.skillLvl[s]+EquipSkillBoost(player.skillHave[s]);
	if(player.potionType==POT_SPARKLING)
		lvl+=PotionEffect(player.potionType,player.potionPower);
	if(lvl>18)
		lvl=18;
	return lvl;
}

byte SpecificSkillLevel(byte s)
{
	int i;
	byte sNum;

	sNum=255;
	for(i=0;i<PLAYER_SKILLS;i++)
		if(player.skillHave[i]==s)
			sNum=(byte)i;

	if(sNum==255 || SkillLevel(sNum)==0)
		return 0;
	else
		return SkillLevel(sNum);
}

float SkillVal(byte w,byte s)
{
	byte lvl;

	if(SkillLevel(s)>skills[player.skillHave[s]].capLevel[w])
		lvl=skills[player.skillHave[s]].capLevel[w];
	else
		lvl=SkillLevel(s);
	return (skills[player.skillHave[s]].val[w]+skills[player.skillHave[s]].valUp[w]*(lvl-1));
}

float SpecificSkillVal(byte w,byte s)
{
	int i;
	byte sNum;

	sNum=255;
	for(i=0;i<PLAYER_SKILLS;i++)
		if(player.skillHave[i]==s)
			sNum=(byte)i;

	if(sNum==255 || SkillLevel(sNum)==0)
		return 0;
	else
		return SkillVal(w,sNum);
}

float SpecificSkillValDiminished(byte w,byte s,float f)
{
	int i;
	byte sNum;
	byte realLev,lv;

	sNum=255;
	for(i=0;i<PLAYER_SKILLS;i++)
		if(player.skillHave[i]==s)
			sNum=(byte)i;

	if(sNum==255 || SkillLevel(sNum)==0)
		return 0;
	else
	{
		realLev=SkillLevel(sNum);
		lv=(byte)((float)realLev*f);
		if(lv==0)
			return 0;
		if(lv>skills[s].capLevel[w])
			lv=skills[s].capLevel[w];
		return (skills[player.skillHave[sNum]].val[w]+skills[player.skillHave[s]].valUp[w]*(lv-1));
	}
}

float SkillStaminaCost(byte s)
{
	return (skills[player.skillHave[s]].stamina+skills[player.skillHave[s]].staminaUp*(SkillLevel(s)-1));
}

float SkillMagicCost(byte s)
{
	return (skills[player.skillHave[s]].magic+skills[player.skillHave[s]].magicUp*(SkillLevel(s)-1));
}

float SpecificSkillCost(byte w,byte s)
{
	int i;
	byte sNum;

	sNum=255;
	for(i=0;i<PLAYER_SKILLS;i++)
		if(player.skillHave[i]==s)
			sNum=(byte)i;

	if(sNum==255 || SkillLevel(sNum)==0)
		return 0;
	else
	{
		if(w==0)
		{
			float f=SkillStaminaCost(sNum);
			f=f*(100.0f-TalentBonus(TLT_VIVACITY))/100.0f;
#ifdef DIRECTORS
			if(player.axe.magic==EM_DIRECTORS)
				f/=2.0f;
#endif
			return f;
		}
		else
		{
			float f=SkillMagicCost(sNum);
			if(player.amulet.magic==EM_NATURE && s>=SKILL_SHROOM && s<=SKILL_LIFEBERRY)
			{
				f-=2.0f;
				if(f<1.0f)
					f=1.0f;
			}
			if(s>=SKILL_SHOCK && s<=SKILL_TORNADO)
			{
				f=f*(100.0f-TalentBonus(TLT_WINDBAG))/100.0f;
			}
			if(GlassEffect()==GE_SPELLCOST)
			{
				if((GlassAmount()==5 && s>=SKILL_SHROOM && s<=SKILL_PLAGUE) ||
					(GlassAmount()==0 && s>=SKILL_BURNINATE && s<=SKILL_HEATSHIELD) ||
					(GlassAmount()==1 && s>=SKILL_SHOCK && s<=SKILL_TORNADO) ||
					(GlassAmount()==2 && s>=SKILL_RESTORE && s<=SKILL_ICECUBE) ||
					(GlassAmount()==3 && s>=SKILL_SHROOM && s<=SKILL_LIFEBERRY) ||
					(GlassAmount()==4 && s>=SKILL_DRAIN && s<=SKILL_PLAGUE))
				{
					f--;
					if(f<1.0f)
						f=1.0f;
				}
			}
			if(s>=SKILL_BURNINATE && s<=SKILL_HEATSHIELD && HeatShieldOn())
			{
				f-=SpecificSkillLevel(SKILL_HEATSHIELD);
				if(f<1.0f)
					f=1.0f;
			}
#ifdef DIRECTORS
			if(player.amulet.magic==EM_OMELET)
			{
				f/=2.0f;
				if(f<1.0f)
					f=1.0f;
			}
#endif
			return f;
		}
	}
}

byte SkillDamageBonus(void)
{
	int i;
	byte bonus=0;

	for(i=0;i<PLAYER_SKILLS;i++)
	{
		if(SkillLevel(i)>0 && PassiveSkillIsActive(player.skillHave[i]) &&
			skills[player.skillHave[i]].effect==SE_STRENGTH)
		{
			bonus+=(byte)SkillVal(0,i);
		}
	}

	return bonus;
}

byte SkillSpeedBonus(void)
{
	int i;
	byte bonus=0;

	for(i=0;i<PLAYER_SKILLS;i++)
	{
		if(SkillLevel(i)>0 && PassiveSkillIsActive(player.skillHave[i]) &&
			skills[player.skillHave[i]].effect==SE_SPEED)
		{
			bonus+=(byte)SkillVal(0,i);
		}
	}

	return bonus;
}

byte SkillLifeBonus(void)
{
	int i;
	byte bonus=0;

	for(i=0;i<PLAYER_SKILLS;i++)
	{
		if(SkillLevel(i)>0 && PassiveSkillIsActive(player.skillHave[i]) &&
			skills[player.skillHave[i]].effect==SE_LIFE)
		{
			bonus+=(byte)SkillVal(0,i);
		}
	}

	return bonus;
}

byte SkillMagicBonus(void)
{
	int i;
	byte bonus=0;

	for(i=0;i<PLAYER_SKILLS;i++)
	{
		if(SkillLevel(i)>0 && PassiveSkillIsActive(player.skillHave[i]) &&
			skills[player.skillHave[i]].effect==SE_MAGIC)
		{
			bonus+=(byte)SkillVal(0,i);
		}
	}

	return bonus;
}

byte SkillArmorBonus(void)
{
	int i;
	byte bonus=0;

	for(i=0;i<PLAYER_SKILLS;i++)
	{
		if(SkillLevel(i)>0 && PassiveSkillIsActive(player.skillHave[i]) &&
			skills[player.skillHave[i]].effect==SE_ARMOR)
		{
			bonus+=(byte)SkillVal(0,i);
		}
	}

	return bonus;
}

byte SkillStaminaBonus(void)
{
	int i;
	byte bonus=0;

	for(i=0;i<PLAYER_SKILLS;i++)
	{
		if(SkillLevel(i)>0 && PassiveSkillIsActive(player.skillHave[i]) &&
			skills[player.skillHave[i]].effect==SE_STAMINA)
		{
			bonus+=(byte)SkillVal(0,i);
		}
	}

	return bonus;
}

byte SkillDodgeRate(void)
{
	int i;
	word bonus=0;

	for(i=0;i<PLAYER_SKILLS;i++)
	{
		if(SkillLevel(i)>0 && PassiveSkillIsActive(player.skillHave[i]) &&
			skills[player.skillHave[i]].effect==SE_DODGE)
		{
			bonus+=(byte)SkillVal(0,i);
		}
	}

	if(bonus>100)
		bonus=100;
	return (byte)bonus;
}

void SkillsUpdate(void)
{
	int i;

	for(i=0;i<PLAYER_SKILLS;i++)
	{
		if(player.skillHave[i] && SkillLevel(i)>0 && PassiveSkillIsActive(player.skillHave[i]))
		{
			player.skillClock[i]++;
			switch(skills[player.skillHave[i]].effect)
			{
				case SE_RECOVER:
					if(player.skillClock[i]>=(word)SkillVal(1,i))
					{
						player.skillClock[i]-=(word)SkillVal(1,i);
						HealGoodguy(RecoverAmt((word)SkillVal(0,i)));
					}
					break;
				case SE_MEDITATE:
					if(goodguy->ouch || (goodguy->seq!=ANIM_IDLE && goodguy->seq!=ANIM_A2))	// not standing still or bored or hit
						player.skillClock[i]=0;

					if(player.skillClock[i]>=(word)SkillVal(1,i))
					{
						player.skillClock[i]-=(word)SkillVal(1,i);
						if(player.magic<player.maxMagic)
						{
							player.magic+=(byte)SkillVal(0,i);
							if(player.magic>player.maxMagic)
								player.magic=player.maxMagic;
						}
					}
					break;
			}
		}
		if(player.skillHave[i]==SKILL_PLAGUE && player.skillClock[i])
		{
			player.skillClock[i]--;
			if(player.skillClock[i]==0)
			{
				ExplodePlagueRats();
			}
			if((player.skillClock[i]%12)==0)
				MakePlagueRingParticle(goodguy->x,goodguy->y,0,(int)SpecificSkillVal(1,SKILL_PLAGUE));

			goodguy->GetPoisoned(10);	// keep player poisoned for duration
		}
		if(player.skillHave[i]==SKILL_HEATSHIELD && player.skillClock[i])
		{
			player.skillClock[i]--;
			if((player.skillClock[i]%12)==0)
			{
				MakeRingParticle(goodguy->x,goodguy->y,0,32,16);
				curMap->BrightTorch(goodguy->mapx,goodguy->mapy,10,3);
			}
		}
	}
}

byte PlagueOn(void)
{
	int i;

	for(i=0;i<PLAYER_SKILLS;i++)
	{
		if(player.skillHave[i]==SKILL_PLAGUE && player.skillClock[i])
		{
			return 1;
		}
	}
	return 0;
}

byte HeatShieldOn(void)
{
	int i;

	for(i=0;i<PLAYER_SKILLS;i++)
	{
		if(player.skillHave[i]==SKILL_HEATSHIELD && player.skillClock[i])
		{
			return 1;
		}
	}
	return 0;
}

byte StaminaCost(void)
{
	int i;
	float cost=0;

	cost=player.damage;

	for(i=0;i<PLAYER_SKILLS;i++)
	{
		if(SkillLevel(i)>0 && PassiveSkillIsActive(player.skillHave[i]))
		{
			cost+=SkillStaminaCost(i);
		}
	}

	cost=cost*(100.0f-TalentBonus(TLT_VIVACITY))/100.0f;
	if(player.axe.magic==EM_HANDY)
		cost-=1;
#ifdef DIRECTORS
	if(player.axe.magic==EM_DIRECTORS)
		cost/=2;
#endif
	if(cost<1)
		cost=1;

	return (byte)cost;
}

byte MagicCost(void)
{
	int i;
	float cost=0;

	for(i=0;i<PLAYER_SKILLS;i++)
	{
		if(SkillLevel(i)>0 && PassiveSkillIsActive(player.skillHave[i]))
		{
			cost+=SkillMagicCost(i);
		}
	}

#ifdef DIRECTORS
	if(player.amulet.magic==EM_OMELET)
		cost/=2;
#endif

	if(cost<0)
		cost=0;

	return (byte)cost;
}

char *GetSkillName(byte n,byte longName)
{
	int i;
	static char huh[]="????",axeM[]="Mastery";

	if(n>MAX_SKILLS)
		return huh;
	for(i=0;i<PLAYER_SKILLS;i++)
		if(player.skillHave[i]==n)
		{
			if(n==SKILL_AXEMASTERY && !longName)
				return axeM;
			else
				return skills[n].name;
		}

	return huh;
}

skill_t *GetSkill(byte n)
{
	if(n>MAX_SKILLS)
		return NULL;
	return &skills[n];
}

byte PickSkillToBoost(byte type)
{
	byte ok[4],s;
	word cnt;

	if(type==0)
	{
		s=rand()%(MAX_SKILLS);
		s++;
		return s;
	}

	ok[0]=ok[1]=ok[2]=ok[3]=255;
	if(type==IT_MAGICAXE || type==IT_GOLDENAXE)
	{
		ok[0]=ST_ATTACKP;
		ok[1]=ST_ATTACK;
		ok[2]=ST_THROWP;
		ok[3]=ST_THROW;
	}
	if(type==IT_MAGICAMULET || type==IT_GOLDENAMULET)
	{
		ok[0]=ST_MAGICP;
		ok[1]=ST_MAGIC;
	}
	if(type==IT_MAGICPARKA || type==IT_GOLDENPARKA)
	{
		ok[0]=ST_PASSIVE;
	}

	cnt=0;
	while((cnt++)<1000)
	{
		s=rand()%(MAX_SKILLS);
		s++;
		if(skills[s].type==ok[0] || skills[s].type==ok[1] ||
		   skills[s].type==ok[2] || skills[s].type==ok[3])
		   return s;
	}
	return 1;
}

void LearnSkill(byte s)
{
	char txt[64];

	if(s==50)
		s=0;
	if(player.skillHave[s]==0)
	{
		if(s!=0)
		{
			player.skillHave[s]=s;
			player.skillLvl[s]=0;
			sprintf(txt,"Learned %s Skill!",skills[s].name);
			NewMessage(txt,0);
			EnterNewSkill(s);
			player.skillClock[s]=0;
		}
		else
		{
			player.skillHave[s]=50;
			player.skillLvl[s]=0;
			player.skillClock[s]=0;
			sprintf(txt,"Learned %s Skill!",skills[50].name);
			NewMessage(txt,0);
			EnterNewSkill(50);
		}
		return;
	}
}

word SkillTrainingAmount(void)
{
	int i;
	word bonus=0;

	for(i=0;i<PLAYER_SKILLS;i++)
	{
		if(SkillLevel(i)>0 && PassiveSkillIsActive(player.skillHave[i]) &&
			skills[player.skillHave[i]].effect==SE_XP)
		{
			bonus+=(word)SkillVal(0,i);
		}
	}

	return bonus;
}

word SkillStunAmount(void)
{
	int i;
	word bonus=0;

	for(i=0;i<PLAYER_SKILLS;i++)
	{
		if(SkillLevel(i)>0 && PassiveSkillIsActive(player.skillHave[i]) &&
			skills[player.skillHave[i]].effect==SE_STUN)
		{
			bonus+=(byte)SkillVal(0,i);
		}
	}

	return bonus;
}

byte SkillHaveASpell(void)
{
	int i;

	for(i=0;i<PLAYER_SKILLS;i++)
		if(player.skillHave[i]>=SKILL_SHROOM && player.skillHave[i]<=SKILL_PLAGUE)
			return 1;

	return 0;
}

byte SkillHave(byte s)
{
	int i;

	for(i=0;i<PLAYER_SKILLS;i++)
		if(player.skillHave[i]==s)
			return 1;

	return 0;
}

byte HaveTwoOfASchool(void)
{
	int i;
	byte c;

	c=0;
	for(i=0;i<PLAYER_SKILLS;i++)
		if(player.skillHave[i]>=SKILL_SHROOM && player.skillHave[i]<=SKILL_LIFEBERRY)
			c++;
	if(c>=2)
		return 1;
	c=0;
	for(i=0;i<PLAYER_SKILLS;i++)
		if(player.skillHave[i]>=SKILL_BURNINATE && player.skillHave[i]<=SKILL_HEATSHIELD)
			c++;
	if(c>=2)
		return 1;
	c=0;
	for(i=0;i<PLAYER_SKILLS;i++)
		if(player.skillHave[i]>=SKILL_SHOCK && player.skillHave[i]<=SKILL_TORNADO)
			c++;
	if(c>=2)
		return 1;
	c=0;
	for(i=0;i<PLAYER_SKILLS;i++)
		if(player.skillHave[i]>=SKILL_RESTORE && player.skillHave[i]<=SKILL_ICECUBE)
			c++;
	if(c>=2)
		return 1;
	c=0;
	for(i=0;i<PLAYER_SKILLS;i++)
		if(player.skillHave[i]>=SKILL_DRAIN && player.skillHave[i]<=SKILL_PLAGUE)
			c++;
	if(c>=2)
		return 1;

	return 0;
}

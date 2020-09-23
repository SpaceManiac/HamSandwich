#include "winpch.h"
#include "items.h"
#include "display.h"
#include "player.h"
#include "repair.h"
#include "hiscore.h"
#include "editor.h"
#include "shop.h"
#include "vars.h"
#include "goal.h"
#include "worldstitch.h"
#include <ctype.h>

item_t baseItems[]={
	{"None",0,0,0,0,0,0,0,0,0,0,0,0,"",0},
	{"Hammer Up",-2,0,0,0,0,0,
		0,
		IF_PICKUP,
		IT_PICKUP|IT_POWERUP,
		ITR_GET,IE_HAMMER,1,
		"Hammer Up!",SND_HAMMERUP},
	{"Pants Of Power",-3,-2,3,0,0,0,
		0,
		IF_PICKUP,
		IT_PICKUP|IT_POWERUP,
		ITR_GET,IE_PANTS,1,
		"Pants Of Power!",SND_PANTS},
	{"Reverse Hammer",-2,0,4,0,0,0,
		0,
		IF_PICKUP,
		IT_PICKUP|IT_POWERUP,
		ITR_GET,IE_POWERUP,PU_REVERSE,
		"Reverse Hammer!",SND_REVERSE},
	{"Sproingy Spring",0,-3,5,0,0,0,
		0,
		IF_PICKUP,
		IT_PICKUP|IT_POWERUP,
		ITR_GET,IE_POWERUP,PU_REFLECT,
		"Sproingy Spring!",SND_REFLECT},
	{"Missile Pack",-3,-4,6,0,0,0,
		50,
		IF_PICKUP,
		IT_PICKUP|IT_WEAPON,
		ITR_GET,IE_WEAPON,WPN_MISSILES,
		"Missile Pack!",SND_WEAPON},
	{"AK-8087",0,-4,33,0,0,0,
		50,
		IF_PICKUP,
		IT_PICKUP|IT_WEAPON,
		ITR_GET,IE_WEAPON,WPN_AK8087,
		"AK-8087!",SND_WEAPON},
	{"Chinese Take-Out",0,0,38,0,0,0,
		100,
		IF_PICKUP,
		IT_PICKUP|IT_FOOD,
		ITR_GET,IE_HEALRANDOM,0,
		"",SND_FOOD},
	{"Energy Shield",0,-3,39,0,0,0,
		10,
		IF_PICKUP|IF_GLOW,
		IT_PICKUP|IT_POWERUP,
		ITR_GET,IE_POWERUP,PU_SHIELD,
		"Energy Shield!",SND_SHIELD},
	{"Bombs",3,-5,35,0,0,0,
		50,
		IF_PICKUP,
		IT_PICKUP|IT_WEAPON,
		ITR_GET,IE_WEAPON,WPN_BOMBS,
		"Cherry Bombs!",SND_WEAPON},
	{"Toaster",0,-2,34,0,0,0,
		50,
		IF_PICKUP,
		IT_PICKUP|IT_WEAPON,
		ITR_GET,IE_WEAPON,WPN_FLAME,
		"Toaster!",SND_WEAPON},
	{"Brain",0,-2,7,0,0,0,
		0,
		IF_PICKUP,
		IT_PICKUP|IT_COLLECT,
		ITR_GET,IE_BRAIN,1,
		"",SND_GETBRAIN},
	{"Pumpkin Keychain",0,-4,24,0,0,0,
		0,
		IF_PICKUP,
		IT_PICKUP|IT_COLLECT,
		ITR_GET,IE_KEYCHAIN,0,
		"Cool! A Pumpkin Keychain!",SND_GETKEYCHAIN},
	{"Hammer Keychain",0,-4,21,0,0,0,
		0,
		IF_PICKUP,
		IT_PICKUP|IT_COLLECT,
		ITR_GET,IE_KEYCHAIN,1,
		"Cool! A Hammer Keychain!",SND_GETKEYCHAIN},
	{"Rocket Keychain",0,-4,22,0,0,0,
		0,
		IF_PICKUP,
		IT_PICKUP|IT_COLLECT,
		ITR_GET,IE_KEYCHAIN,2,
		"Cool! A Rocket Keychain!",SND_GETKEYCHAIN},
	{"Squash Keychain",0,-4,23,0,0,0,
		0,
		IF_PICKUP,
		IT_PICKUP|IT_COLLECT,
		ITR_GET,IE_KEYCHAIN,3,
		"Cool! A Squash Keychain!",SND_GETKEYCHAIN},
	{"Yellow Key",-4,-3,17,0,0,0,
		0,
		IF_PICKUP,
		IT_PICKUP|IT_KEY,
		ITR_GET,IE_KEY,1,
		"",SND_GETKEY},
	{"Red Key",-4,-3,18,0,0,0,
		0,
		IF_PICKUP,
		IT_PICKUP|IT_KEY,
		ITR_GET,IE_RKEY,1,
		"",SND_GETKEY},
	{"Green Key",-4,-3,19,0,0,0,
		0,
		IF_PICKUP,
		IT_PICKUP|IT_KEY,
		ITR_GET,IE_GKEY,1,
		"",SND_GETKEY},
	{"Blue Key",-4,-3,20,0,0,0,
		0,
		IF_PICKUP,
		IT_PICKUP|IT_KEY,
		ITR_GET,IE_BKEY,1,
		"",SND_GETKEY},
	{"Key Of Lunacy",0,-1,25,0,0,0,
		0,
		IF_PICKUP|IF_LOONYCOLOR,
		IT_PICKUP|IT_KEY|IT_COLLECT,
		ITR_GET,IE_LOONYKEY,1,
		"KEY OF LUNACY!!",SND_LOONYKEY},
	{"Big Axe",0,0,44,0,0,0,
		50,
		IF_PICKUP,
		IT_PICKUP|IT_WEAPON,
		ITR_GET,IE_WEAPON,WPN_BIGAXE,
		"BIG Axe!",SND_WEAPON},
	{"Power Armor",0,0,43,0,0,0,
		0,
		IF_PICKUP,
		IT_PICKUP|IT_WEAPON,
		ITR_GET,IE_WEAPON,WPN_PWRARMOR,
		"Power Armor!",SND_ROBOBOUAPHAON},
	{"Zap Wand",0,-5,46,0,0,0,
		50,
		IF_PICKUP,
		IT_PICKUP|IT_WEAPON,
		ITR_GET,IE_WEAPON,WPN_LIGHTNING,
		"Zap Wand!",SND_WEAPON},
	{"Spears",0,-3,45,0,0,0,
		50,
		IF_PICKUP,
		IT_PICKUP|IT_WEAPON,
		ITR_GET,IE_WEAPON,WPN_SPEAR,
		"Flimsy Spears!",SND_WEAPON},
	{"Machete",0,0,47,0,0,0,
		50,
		IF_PICKUP,
		IT_PICKUP|IT_WEAPON,
		ITR_GET,IE_WEAPON,WPN_MACHETE,
		"Machete!",SND_WEAPON},
	{"Mines",0,0,48,0,0,0,
		50,
		IF_PICKUP,
		IT_PICKUP|IT_WEAPON,
		ITR_GET,IE_WEAPON,WPN_MINES,
		"Mines!",SND_WEAPON},
	{"Garlic",0,0,49,0,0,0,
		0,
		IF_PICKUP,
		IT_PICKUP|IT_FOOD,
		ITR_GET,IE_POWERUP,PU_GARLIC,
		"Fresh Garlic!",SND_WEAPON},
	{"Orbiter",-5,-2,58,0,0,0,
		25,
		IF_PICKUP,
		IT_PICKUP|IT_POWERUP,
		ITR_GET,IE_ORBITER,1,
		"Orbiter!",0},
	{"Particle Accelerator",-3,0,59,0,0,0,
		0,
		IF_PICKUP,
		IT_PICKUP|IT_POWERUP,
		ITR_GET,IE_POWERUP,PU_SPEED,
		"Particle Accelerator!",SND_SPEEDUP},
	{"Small Grey Rocks",-2,0,32,0,0,0,
		0,
		0,
		IT_DECOR|IT_ROCK,
		ITR_NONE,IE_NONE,0,
		"",0},
	{"Hollow Tree",0,-4,26,0,0,0,
		0,
		0,
		IT_DECOR|IT_TREE|IT_ENTRANCE,
		ITR_NONE,IE_NONE,0,"",0},
	{"Igloo",0,-3,42,0,0,0,
		0,
		0,
		IT_DECOR|IT_ENTRANCE,
		ITR_NONE,IE_NONE,0,"",0},
	{"Spider Web",4,-6,54,0,0,0,
		0,
		0,
		IT_DECOR,
		ITR_NONE,IE_NONE,0,"",0},
	{"Spider Web 2",-1,-10,55,0,0,0,
		0,
		0,
		IT_DECOR,
		ITR_NONE,IE_NONE,0,"",0},
	{"Spider Web 3",-3,-8,56,0,0,0,
		0,
		0,
		IT_DECOR,
		ITR_NONE,IE_NONE,0,"",0},
	{"Spider Web 4",-1,-9,57,0,0,0,
		0,
		0,
		IT_DECOR,
		ITR_NONE,IE_NONE,0,"",0},
	{"Grass",-2,-2,61,0,0,0,
		0,
		0,
		IT_DECOR|IT_TREE,
		ITR_NONE,IE_NONE,0,"",0},
	{"Grass 2",-1,-3,62,0,0,0,
		0,
		0,
		IT_DECOR|IT_TREE,
		ITR_NONE,IE_NONE,0,"",0},
	{"Tiny Hut",-1,-5,65,0,0,0,
		0,
		0,
		IT_DECOR|IT_ENTRANCE,
		ITR_NONE,IE_NONE,0,"",0},

	{"Coin",1,-7,90,0,0,0,
		100,
		IF_PICKUP,
		IT_PICKUP|IT_COLLECT,
		ITR_GET,IE_COIN,1,"",SND_COINGET},
	{"Candle",-1,-6,91,0,0,0,
		0,
		IF_PICKUP,
		IT_PICKUP|IT_COLLECT,
		ITR_GET,IE_CANDLE,1,"",SND_CANDLEGET},
	{"Orbit Bomber",-5,-2,58,1,4,0,
		5,
		IF_PICKUP,
		IT_PICKUP|IT_POWERUP,
		ITR_GET,IE_ORBITER2,1,
		"Orbit Bomber!",0},
	{"Pizza",0,-2,93,0,0,0,
		80,
		IF_PICKUP,
		IT_PICKUP|IT_FOOD,
		ITR_GET,IE_HEAL,25,"Mmm, Pizza!",SND_FOOD},
	{"Torch",0,-7,96,0,0,0,
		0,
		IF_PICKUP,
		IT_PICKUP|IT_WEAPON,
		ITR_GET,IE_WEAPON,WPN_TORCH,
		"Torch!",SND_WEAPON},
	{"Cone",-2,-7,94,0,0,0,
		0,
		IF_SOLID|IF_SHADOW,
		IT_OBSTACLE,
		ITR_NONE,IE_NONE,0,"",0},
	{"Fallen Cone",0,-3,95,0,0,0,
		0,
		0,
		IT_DECOR,
		ITR_NONE,IE_NONE,0,"",0},
	{"Bubbles",0,-5,92,0,0,0,
		0,
		IF_GLOW|IF_BUBBLES,
		IT_POWERUP|IT_DECOR,
		ITR_PLAYERBUMP,IE_OXYGEN,1,"",0},
	{"Scuba Tank",-2,0,89,0,0,0,
		0,
		IF_PICKUP,
		IT_POWERUP|IT_PICKUP,
		ITR_GET,IE_OXYGEN,128,"Scuba Gear!",SND_SCUBA},
	{"Mailbox Open",0,0,97,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF|IF_SHADOW,
		IT_OBSTACLE|IT_BULLETPROOF,
		ITR_NONE,IE_NONE,0,"",0},
	{"Radiator Thing",0,-4,1,0,0,0,
		0,
		IF_SOLID,
		IT_OBSTACLE|IT_CRATE,
		ITR_NONE,IE_NONE,0,"",0},
	{"Stump",0,-3,27,0,0,0,
		0,
		IF_SOLID,
		IT_OBSTACLE|IT_TREE,
		ITR_NONE,IE_NONE,0,"",0},
	{"Bush",-1,0,30,0,0,0,
		0,
		IF_SOLID,
		IT_OBSTACLE|IT_TREE,
		ITR_NONE,IE_NONE,0,"",0},
	{"Grey Rocks",0,-2,31,0,0,0,
		0,
		IF_SOLID,
		IT_OBSTACLE|IT_ROCK,
		ITR_NONE,IE_NONE,0,"",0},
	{"Wooden Post",-2,0,28,0,0,0,
		0,
		IF_SOLID,
		IT_OBSTACLE|IT_CRATE,
		ITR_NONE,IE_NONE,0,"",0},
	{"Chair",-2,-2,36,0,0,0,
		0,
		IF_SOLID|IF_SHADOW,
		IT_OBSTACLE|IT_CHAIR,
		ITR_NONE,IE_NONE,0,"",0},
	{"Chair 2",-2,-2,37,0,0,0,
		0,
		IF_SOLID|IF_SHADOW,
		IT_OBSTACLE|IT_CHAIR,
		ITR_NONE,IE_NONE,0,"",0},
	{"Machete Grass",0,-3,66,0,0,0,
		0,
		IF_SOLID,
		IT_OBSTACLE|IT_TREE,
		ITR_CHOP,IE_DESTROY,1,"",0},
	{"Barrel",-1,-6,72,0,0,0,
		0,
		IF_SOLID|IF_SHADOW,
		IT_OBSTACLE|IT_CRATE,
		ITR_NONE,IE_NONE,0,"",0},
	{"Barrel 2",-1,-6,73,0,0,0,
		0,
		IF_SOLID|IF_SHADOW,
		IT_OBSTACLE|IT_CRATE,
		ITR_NONE,IE_NONE,0,"",0},
	{"Barrel 3",-1,-6,74,0,0,0,
		0,
		IF_SOLID|IF_SHADOW,
		IT_OBSTACLE|IT_CRATE,
		ITR_NONE,IE_NONE,0,"",0},
	{"Trashcan",-2,-5,75,0,0,0,
		0,
		IF_SOLID,
		IT_OBSTACLE|IT_CRATE,
		ITR_NONE,IE_NONE,0,"",0},
	{"Trashcan: Full",-2,-5,76,0,0,0,
		0,
		IF_SOLID,
		IT_OBSTACLE|IT_CRATE,
		ITR_NONE,IE_NONE,0,"",0},
	{"Crate",-4,-5,77,0,0,0,
		0,
		IF_SOLID,
		IT_OBSTACLE|IT_CRATE,
		ITR_NONE,IE_NONE,0,"",0},
	{"Crate 2",-4,-5,78,0,0,0,
		0,
		IF_SOLID,
		IT_OBSTACLE|IT_CRATE,
		ITR_NONE,IE_NONE,0,"",0},
	{"Bush 2",-1,0,79,0,0,0,
		0,
		IF_SOLID,
		IT_OBSTACLE|IT_TREE,
		ITR_NONE,IE_NONE,0,"",0},
	{"Bonsai Bush",-4,-5,80,0,0,0,
		0,
		IF_SOLID|IF_SHADOW,
		IT_OBSTACLE|IT_TREE,
		ITR_NONE,IE_NONE,0,"",0},
	{"Brown Rock",2,-6,87,0,0,0,
		0,
		IF_SOLID,
		IT_OBSTACLE|IT_ROCK,
		ITR_NONE,IE_NONE,0,"",0},
	{"Mailbox Shut",0,0,98,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF|IF_SHADOW,
		IT_OBSTACLE|IT_BULLETPROOF,
		ITR_NONE,IE_NONE,0,"",0},
	{"Palm Tree",0,0,41,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF|IF_SHADOW,
		IT_BULLETPROOF|IT_TREE,
		ITR_NONE,IE_NONE,0,"",0},
	{"Tree",-2,-4,2,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF,
		IT_BULLETPROOF|IT_TREE,
		ITR_NONE,IE_NONE,0,"",0},
	{"Yellow Door",-15,9,9,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF,
		IT_BULLETPROOF|IT_DOOR,
		ITR_PLAYERBUMP,IE_DOOR,0,"",SND_DOOROPEN},
	{"Red Door",-15,9,10,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF,
		IT_BULLETPROOF|IT_DOOR,
		ITR_PLAYERBUMP,IE_DOOR,1,"",SND_DOOROPEN},
	{"Green Door",-15,9,11,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF,
		IT_BULLETPROOF|IT_DOOR,
		ITR_PLAYERBUMP,IE_DOOR,2,"",SND_DOOROPEN},
	{"Blue Door",-15,9,12,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF,
		IT_BULLETPROOF|IT_DOOR,
		ITR_PLAYERBUMP,IE_DOOR,3,"",SND_DOOROPEN},
	{"Yellow Door 2",-15,9,13,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF,
		IT_BULLETPROOF|IT_DOOR,
		ITR_PLAYERBUMP,IE_DOOR,0,"",SND_DOOROPEN},
	{"Red Door 2",-15,9,14,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF,
		IT_BULLETPROOF|IT_DOOR,
		ITR_PLAYERBUMP,IE_DOOR,1,"",SND_DOOROPEN},
	{"Green Door 2",-15,9,15,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF,
		IT_BULLETPROOF|IT_DOOR,
		ITR_PLAYERBUMP,IE_DOOR,2,"",SND_DOOROPEN},
	{"Blue Door 2",-15,9,16,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF,
		IT_BULLETPROOF|IT_DOOR,
		ITR_PLAYERBUMP,IE_DOOR,3,"",SND_DOOROPEN},
	{"Wooden Sign",-2,0,29,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF|IF_SHADOW,
		IT_BULLETPROOF|IT_SIGN,
		ITR_NONE,IE_NONE,0,"",0},
	{"Pine Tree",-3,0,40,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF,
		IT_BULLETPROOF|IT_TREE,
		ITR_NONE,IE_NONE,0,"",0},
	{"Dead Tree",-2,-3,52,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF,
		IT_BULLETPROOF|IT_TREE,
		ITR_NONE,IE_NONE,0,"",0},
	{"Dead Tree 2",-2,-3,53,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF,
		IT_BULLETPROOF|IT_TREE,
		ITR_NONE,IE_NONE,0,"",0},
	{"Fat Palm",-2,0,60,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF|IF_SHADOW,
		IT_BULLETPROOF|IT_TREE,
		ITR_NONE,IE_NONE,0,"",0},
	{"Tree 2",-2,-4,63,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF,
		IT_BULLETPROOF|IT_TREE,
		ITR_NONE,IE_NONE,0,"",0},
	{"Minecart Block",-1,-3,64,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF,
		IT_BULLETPROOF|IT_CRATE,
		ITR_MINECART,IE_DESTROY,2,"",SND_BOMBBOOM},
	{"Sign 2",-1,-3,81,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF|IF_SHADOW,
		IT_BULLETPROOF|IT_SIGN,
		ITR_NONE,IE_NONE,0,"",0},
	{"Sign 3",-1,-3,82,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF|IF_SHADOW,
		IT_BULLETPROOF|IT_SIGN,
		ITR_NONE,IE_NONE,0,"",0},
	{"Sign 4",-1,-3,83,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF|IF_SHADOW,
		IT_BULLETPROOF|IT_SIGN,
		ITR_NONE,IE_NONE,0,"",0},
	{"Sign 5",-1,-3,84,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF|IF_SHADOW,
		IT_BULLETPROOF|IT_SIGN,
		ITR_NONE,IE_NONE,0,"",0},
	{"Sign 6",-1,-3,85,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF|IF_SHADOW,
		IT_BULLETPROOF|IT_SIGN,
		ITR_NONE,IE_NONE,0,"",0},
	{"Big Brown Rock",0,-6,86,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF|IF_SHADOW,
		IT_BULLETPROOF|IT_ROCK,
		ITR_NONE,IE_NONE,0,"",0},
	{"Turrets",-4,-5,68,0,0,0,
		0,
		IF_PICKUP,
		IT_PICKUP|IT_WEAPON,
		ITR_GET,IE_WEAPON,WPN_TURRET,"Turrets!",SND_WEAPON},
	{"Mind Control Ray",0,0,69,0,0,0,
		5,
		IF_PICKUP,
		IT_PICKUP|IT_WEAPON,
		ITR_GET,IE_WEAPON,WPN_MINDCONTROL,"Mind Control Ray!",SND_WEAPON},
	{"Reflect Shield",-3,-3,67,0,0,0,
		5,
		IF_PICKUP,
		IT_PICKUP|IT_WEAPON,
		ITR_GET,IE_WEAPON,WPN_REFLECTOR,"Reflector Shield!",SND_WEAPON},
	{"The Cloak Invisible",-1,0,88,0,0,0,
		0,
		IF_PICKUP|IF_GLOW,
		IT_PICKUP|IT_POWERUP,
		ITR_GET,IE_POWERUP,PU_INVISO,"The Cloak Invisible!",SND_SPEEDUP},
	{"Jetpack",0,0,71,0,0,0,
		0,
		IF_PICKUP,
		IT_PICKUP|IT_WEAPON,
		ITR_GET,IE_WEAPON,WPN_JETPACK,"Jetpack!",SND_WEAPON},
	{"Hammer Down",-2,0,0,4,0,0,
		0,
		IF_PICKUP,
		IT_PICKUP|IT_POWERUP,
		ITR_GET,IE_HAMMER,-1,"Hammer Down!",SND_HAMMERDN},
	{"Pants Of Futility",-3,-2,3,3,0,0,
		0,
		IF_PICKUP,
		IT_PICKUP|IT_POWERUP,
		ITR_GET,IE_PANTS,-1,"Pants Of Futility!",SND_PANTSDN},
	{"Swapgun",0,-3,70,0,0,0,
		0,
		IF_PICKUP,
		IT_PICKUP|IT_WEAPON,
		ITR_GET,IE_WEAPON,WPN_SWAPGUN,"Swapgun!",SND_WEAPON},
	{"Radioactive Chinese",0,0,38,4,1,0,
		5,
		IF_PICKUP,
		IT_PICKUP|IT_FOOD,
		ITR_GET,IE_WEIRD,0,"",SND_FOOD},
	{"Mailbox Flag Up",0,0,99,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF|IF_SHADOW,
		IT_OBSTACLE|IT_BULLETPROOF,
		ITR_NONE,IE_NONE,0,"",0},
	{"Scanner",0,0,113,0,0,0,
		30,
		IF_PICKUP|IF_SHADOW,
		IT_PICKUP|IT_WEAPON,
		ITR_GET,IE_WEAPON,WPN_SCANNER,"Monster Scanner!",SND_WEAPON},
	{"Mini-Sub",0,0,104,0,0,0,
		0,
		IF_PICKUP,
		IT_PICKUP|IT_WEAPON,
		ITR_GET,IE_WEAPON,WPN_MINISUB,"Mini-Sub!",SND_SUBSTART},
	{"Hydrant",-1,-3,100,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF|IF_SHADOW,
		IT_BULLETPROOF,
		ITR_NONE,IE_NONE,0,"",0},
	{"Traffic Light-Red",0,-2,101,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF|IF_SHADOW,
		IT_BULLETPROOF,
		ITR_ALWAYS,IE_BECOME,ITM_TRAFFICLT3,"",0},
	{"Traffic Light-Yellow",0,-2,102,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF|IF_SHADOW,
		IT_BULLETPROOF,
		ITR_ALWAYS,IE_BECOME,ITM_TRAFFICLT1,"",0},
	{"Traffic Light-Green",0,-2,103,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF|IF_SHADOW,
		IT_BULLETPROOF,
		ITR_ALWAYS,IE_BECOME,ITM_TRAFFICLT2,"",0},
	{"Ammo Crate",0,0,105,0,0,0,
		0,
		IF_PICKUP,
		IT_PICKUP|IT_WEAPON|IT_POWERUP,
		ITR_GET,IE_POWERUP,PU_AMMO,"Ammo Crate!",SND_AMMOCRATE},
	{"Ammo Pak",0,0,106,0,0,0,
		0,
		IF_PICKUP,
		IT_PICKUP|IT_WEAPON,
		ITR_GET,IE_POWERUP,PU_AMMO2,"Ammo Pack!",SND_WEAPON},
	{"Flat Rock",0,-3,107,0,0,0,
		0,
		0,
		IT_DECOR|IT_ROCK,
		ITR_NONE,IE_NONE,0,"",0},
	{"Grey Rockpile",0,-4,108,0,0,0,
		0,
		IF_SOLID,
		IT_OBSTACLE|IT_ROCK,
		ITR_NONE,IE_NONE,0,"",0},
	{"Seaweed1",0,5,109,0,0,0,
		0,
		IF_SOLID,
		IT_OBSTACLE|IT_TREE,
		ITR_NONE,IE_NONE,0,"",0},
	{"Seaweed2",0,5,111,0,0,0,
		0,
		IF_SOLID,
		IT_OBSTACLE|IT_TREE,
		ITR_NONE,IE_NONE,0,"",0},
	{"Seaweed3",0,4,110,0,0,0,
		0,
		0,
		IT_DECOR|IT_TREE,
		ITR_NONE,IE_NONE,0,"",0},
	{"Freeze Ray",0,-4,112,0,0,0,
		30,
		IF_PICKUP,
		IT_PICKUP|IT_WEAPON,
		ITR_GET,IE_WEAPON,WPN_FREEZE,"Freeze Ray!",SND_WEAPON},
	{"Sign:Danger",0,0,114,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF|IF_SHADOW,
		IT_BULLETPROOF|IT_SIGN,
		ITR_NONE,IE_NONE,0,"",0},
	{"Sign:Stop",0,0,115,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF|IF_SHADOW,
		IT_BULLETPROOF|IT_SIGN,
		ITR_NONE,IE_NONE,0,"",0},
	{"Sign:Do Not Enter",0,0,116,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF|IF_SHADOW,
		IT_BULLETPROOF|IT_SIGN,
		ITR_NONE,IE_NONE,0,"",0},
	{"Sign:Arrow Right",0,0,117,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF|IF_SHADOW,
		IT_BULLETPROOF|IT_SIGN,
		ITR_NONE,IE_NONE,0,"",0},
	{"Sign:Arrow Down",0,0,118,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF|IF_SHADOW,
		IT_BULLETPROOF|IT_SIGN,
		ITR_NONE,IE_NONE,0,"",0},
	{"Sign:Arrow Left",0,0,119,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF|IF_SHADOW,
		IT_BULLETPROOF|IT_SIGN,
		ITR_NONE,IE_NONE,0,"",0},
	{"Sign:Arrow Up",0,0,120,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF|IF_SHADOW,
		IT_BULLETPROOF|IT_SIGN,
		ITR_NONE,IE_NONE,0,"",0},
	{"Sign:Monsters Only",0,0,121,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF|IF_SHADOW,
		IT_BULLETPROOF|IT_SIGN,
		ITR_NONE,IE_NONE,0,"",0},
	{"Sign:Yield",0,0,122,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF|IF_SHADOW,
		IT_BULLETPROOF|IT_SIGN,
		ITR_NONE,IE_NONE,0,"",0},
	{"Sign:One Way",0,0,123,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF|IF_SHADOW,
		IT_BULLETPROOF|IT_SIGN,
		ITR_NONE,IE_NONE,0,"",0},
	{"Sign:Flames",0,0,124,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF|IF_SHADOW,
		IT_BULLETPROOF|IT_SIGN,
		ITR_NONE,IE_NONE,0,"",0},
	{"Sign:Critters",0,0,125,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF|IF_SHADOW,
		IT_BULLETPROOF|IT_SIGN,
		ITR_NONE,IE_NONE,0,"",0},
	{"Sign:Falling Rocks",0,0,126,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF|IF_SHADOW,
		IT_BULLETPROOF|IT_SIGN,
		ITR_NONE,IE_NONE,0,"",0},
	{"Sign:Falling Rocks2",0,0,127,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF|IF_SHADOW,
		IT_BULLETPROOF|IT_SIGN,
		ITR_NONE,IE_NONE,0,"",0},
	{"Sign:Beware Pet",0,0,128,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF|IF_SHADOW,
		IT_BULLETPROOF|IT_SIGN,
		ITR_NONE,IE_NONE,0,"",0},
	{"Stopwatch",0,-3,129,0,0,0,
		30,
		IF_PICKUP,
		IT_PICKUP|IT_WEAPON,
		ITR_GET,IE_WEAPON,WPN_STOPWATCH,"Stopwatch!",SND_WEAPON},
	{"Streetlight",0,-2,130,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF|IF_SHADOW,
		IT_BULLETPROOF,
		ITR_NONE,IE_NONE,0,"",0},
	{"Supreme Squeezy Cheese",-7,-80,131,0,0,0,
		0,
		IF_PICKUP,
		IT_PICKUP|IT_WEAPON|IT_POWERUP,
		ITR_GET,IE_POWERUP,PU_CHEESE,"Supreme Squeezy Cheese!",SND_WEAPON},
	{"White Key",-4,-3,143,0,0,0,
		0,
		IF_PICKUP,
		IT_PICKUP|IT_KEY,
		ITR_GET,IE_WKEY,1,
		"",SND_GETKEY},
	{"Black Key",-4,-3,142,0,0,0,
		0,
		IF_PICKUP,
		IT_PICKUP|IT_KEY,
		ITR_GET,IE_BLKEY,1,
		"",SND_GETKEY},
	{"White Door",-15,9,145,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF,
		IT_BULLETPROOF|IT_DOOR,
		ITR_PLAYERBUMP,IE_DOOR,4,"",SND_DOOROPEN},
	{"Black Door",-15,9,144,0,0,0,
		0,
		IF_SOLID|IF_BULLETPROOF,
		IT_BULLETPROOF|IT_DOOR,
		ITR_PLAYERBUMP,IE_DOOR,5,"",SND_DOOROPEN},
	{"Hay Bale",0,0,160,0,0,0,
		0,
		IF_SOLID,
		IT_OBSTACLE,
		ITR_NONE,IE_NONE,0,"",0},
	{"Wooden Barrel",0,0,158,0,0,0,
		0,
		IF_SOLID|IF_SHADOW,
		IT_OBSTACLE|IT_CRATE,
		ITR_NONE,IE_NONE,0,"",0},
	{"Boomerang",0,-3,148,0,0,0,
		30,
		IF_PICKUP,
		IT_PICKUP|IT_WEAPON,
		ITR_GET,IE_WEAPON,WPN_BOOMERANG,"Boomerang!",SND_WEAPON},
	{"Potted Cactus",0,-3,149,0,0,0,
		30,
		IF_PICKUP,
		IT_PICKUP|IT_WEAPON,
		ITR_GET,IE_WEAPON,WPN_CACTUS,"Potted Cactus!",SND_WEAPON},
	{"Bionic Arm",0,-3,150,0,0,0,
		30,
		IF_PICKUP,
		IT_PICKUP|IT_WEAPON,
		ITR_GET,IE_WEAPON,WPN_ROCKETS,"Rocket Launcher!",SND_WEAPON},
	{"Super Soaker",0,-3,153,0,0,0,
		30,
		IF_PICKUP,
		IT_PICKUP|IT_WEAPON,
		ITR_GET,IE_WEAPON,WPN_WATERGUN,"Super Soaker!",SND_WEAPON},
	{"Megaphone",0,-3,151,0,0,0,
		30,
		IF_PICKUP,
		IT_PICKUP|IT_WEAPON,
		ITR_GET,IE_WEAPON,WPN_MEGAPHONE,"Megaphone!",SND_WEAPON},
	{"Cucurbinator",0,-3,152,0,0,0,
		30,
		IF_PICKUP,
		IT_PICKUP|IT_WEAPON,
		ITR_GET,IE_WEAPON,WPN_PUMPKIN,"Cucurbinator!",SND_WEAPON},
	{"Doublegunner",0,-3,154,0,0,0,
		30,
		IF_PICKUP,
		IT_PICKUP|IT_WEAPON,
		ITR_GET,IE_WEAPON,WPN_DEATHRAY,"Doublegunner!",SND_WEAPON},
	{"Whoopie Cushion",0,-3,155,0,0,0,
		30,
		IF_PICKUP,
		IT_PICKUP|IT_WEAPON,
		ITR_GET,IE_WEAPON,WPN_WHOOPIE,"Whoopie Cushion!",SND_WEAPON},
	{"Abyssinator",0,-3,156,0,0,0,
		0,
		IF_PICKUP,
		IT_PICKUP|IT_WEAPON,
		ITR_GET,IE_WEAPON,WPN_BLACKHOLE,"Abyssinator!",SND_WEAPON},
	{"Medic Kit",0,-3,157,0,0,0,
		30,
		IF_PICKUP,
		IT_PICKUP|IT_WEAPON,
		ITR_GET,IE_WEAPON,WPN_MEDICKIT,"Medic Kit!",SND_WEAPON},
};

static item_t emptyItem=
	{"New Item",0,0,0,0,0,0,
		0,
		0,
		IT_CUSTOM,
		ITR_NONE,IE_NONE,0,"",0};

sprite_set_t *itmSpr;
sprite_set_t *customItmSpr;
static byte glowism;
static byte rndItem;
static word numItems;
static item_t *items;
static int totalRare;

void InitItems(void)
{
	numItems=NUM_ORIGINAL_ITEMS;
	items=new item_t[256];

	memset(items,0,256*sizeof(item_t));
	memcpy(items,baseItems,sizeof(item_t)*NUM_ORIGINAL_ITEMS);

	itmSpr=new sprite_set_t("graphics/items.jsp");
	customItmSpr=NULL;
	glowism=0;
	SetupRandomItems();
	rndItem=GetRandomItem();
}

void ExitItems(void)
{
	delete itmSpr;
	if (customItmSpr) {
		delete customItmSpr;
		customItmSpr = NULL;
	}
	delete[] items;
}

int NumItemSprites(void)
{
	return itmSpr->GetCount();
}

int NewItem(void)
{
	if(numItems==MAX_ITEMS)
		return -1;

	items[numItems]=emptyItem;
	numItems++;

	return numItems-1;
}

void DeleteItem(int itm)
{
	int i;

	if(itm<NUM_ORIGINAL_ITEMS)
		return;
	if(itm>=numItems)
		return;

	RepairItems(itm);

	for(i=itm;i<numItems-1;i++)
	{
		memcpy(&items[i],&items[i+1],sizeof(item_t));
	}
	items[numItems-1]=emptyItem;
	numItems--;
}

void SetCustomItemSprites(char* name)
{
	if (customItmSpr) delete customItmSpr;

	customItmSpr=new sprite_set_t();

	char buf[64];
	sprintf(buf, "user/%s", name);
	if (!customItmSpr->Load(buf))
	{
		// failed to load
		delete customItmSpr;
		customItmSpr = NULL;
		return;
	}
	else
	{
		// great success!
		return;
	}
}

void DetectCustomItemSprites(world_t *world)
{
	// extract filename out of first special if possible
	special_t* special = world->map[0]->special;

	for (int i = 0; i < MAX_SPECIAL; ++i) {
		for (int j = 0; j < NUM_EFFECTS; ++j) {
			if (special[i].effect[j].type == EFF_ITEMGRAPHICS) {
				SetCustomItemSprites(special[i].effect[j].text);
				return;
			}
		}
	}
}

sprite_set_t* CustomItemSprites()
{
	if (customItmSpr == NULL) {
		DetectCustomItemSprites(editing ? EditorGetWorld() : &curWorld);
	}
	return customItmSpr;
}

int NumCustomSprites(void)
{
	return CustomItemSprites() ? customItmSpr->GetCount() : 0;
}

void RenderItem(int x,int y,byte type,char bright,byte flags)
{
	sprite_t* sprite;
	byte b;

	if(!(flags&(MAP_SHOWPICKUPS|MAP_SHOWOTHERITEMS)))
		return;

	if(type==ITM_RANDOM)
		type=rndItem;

	if(type>=numItems || type==0)
		return;

	if(!(flags&MAP_SHOWPICKUPS))
		if(items[type].flags&IF_PICKUP)
			return;
	if(!(flags&MAP_SHOWOTHERITEMS))	// if not showing other items, show ONLY those with pickup
		if(!(items[type].flags&IF_PICKUP))
			return;

	if(items[type].flags&IF_TILE)
	{
		// can't shadow, glow, loonycolor, or even offcolor!
		SprDrawTile(x+items[type].xofs,y+items[type].yofs+1,items[type].sprNum,
					bright+items[type].bright,DISPLAY_DRAWME);
	}
	else
	{
		x-=3;

		if (items[type].flags & IF_USERJSP)
		{
			sprite_set_t* custom = CustomItemSprites();
			if (custom)
				sprite = custom->GetSprite(items[type].sprNum < custom->GetCount() ? items[type].sprNum : 0);
			else
				sprite = itmSpr->GetSprite(8); // red X indicating invalid custom JSP file
		}
		else
			sprite = itmSpr->GetSprite(items[type].sprNum);

		if(items[type].flags&IF_SHADOW)
		{
			SprDraw(x+items[type].xofs,y,-items[type].yofs+1,0,bright+items[type].bright,
				sprite,DISPLAY_DRAWME|DISPLAY_SHADOW);
		}
		if(items[type].flags&IF_LOONYCOLOR)
		{
			b=abs(16-(glowism&31));
			SprDraw(x+items[type].xofs,y,-items[type].yofs+1,glowism/32,bright+b+items[type].bright,
				sprite,DISPLAY_DRAWME);
		}
		else if(items[type].flags&IF_GLOW)
		{

			SprDraw(x+items[type].xofs,y,-items[type].yofs+1,0,bright+items[type].bright,
				sprite,DISPLAY_DRAWME|DISPLAY_GLOW);
		}
		else
		{
			if(items[type].fromColor==items[type].toColor)
				SprDraw(x+items[type].xofs,y,-items[type].yofs+1,255,bright+items[type].bright,
					sprite,DISPLAY_DRAWME);
			else
				SprDrawOff(x+items[type].xofs,y,-items[type].yofs+1,items[type].fromColor,items[type].toColor,
						   bright+items[type].bright,sprite,DISPLAY_DRAWME|DISPLAY_OFFCOLOR);
		}
	}
}

void InstaRenderItem(int x,int y,byte type,char bright,MGLDraw *mgl)
{
	sprite_t* sprite;
	byte b;

	if(type==ITM_RANDOM)
		type=rndItem;

	if(type>=numItems || type==0)
		return;


	if (items[type].flags & IF_USERJSP)
	{
		sprite_set_t* custom = CustomItemSprites();
		if (custom)
			sprite = custom->GetSprite(items[type].sprNum < custom->GetCount() ? items[type].sprNum : 0);
		else
			sprite = itmSpr->GetSprite(8);
	} else
			sprite = itmSpr->GetSprite(items[type].sprNum);

	if(items[type].flags&IF_TILE)
	{
		// can't shadow, glow, loonycolor, or even offcolor!
		RenderFloorTileTrans(x+items[type].xofs-TILE_WIDTH/2,y+items[type].yofs-TILE_HEIGHT/2,items[type].sprNum,
					bright+items[type].bright);
	}
	else
	{
		x-=3;

		//itmSpr->GetSprite(items[type].sprNum)->DrawC(x+items[type].xofs,y+items[type].yofs,mgl);
		if(items[type].flags&IF_SHADOW)
		{
			sprite->DrawShadow(x+items[type].xofs, y+items[type].yofs,mgl);
		}
		if(items[type].flags&IF_LOONYCOLOR)
		{
			b=abs(16-(glowism&31));
			sprite->DrawColored(x+items[type].xofs, y+items[type].yofs,mgl,glowism/32,bright+b+items[type].bright);
		}
		else if(items[type].flags&IF_GLOW)
		{
			sprite->DrawGlow(x+items[type].xofs,y+items[type].yofs, mgl,bright+items[type].bright);
		}
		else
		{
			if(items[type].fromColor==items[type].toColor)
			{
				sprite->DrawBright(x+items[type].xofs, y+items[type].yofs,mgl,bright+items[type].bright);
			}
			else
			{
				sprite->DrawOffColor(x+items[type].xofs,y+items[type].yofs,
									mgl,items[type].fromColor,items[type].toColor,
									bright+items[type].bright);
			}
		}
	}

}

void DrawRedX(int x,int y,byte candle,MGLDraw *mgl)
{
	itmSpr->GetSprite(140+candle)->Draw(x+61,y+71,mgl);
}

item_t *GetItem(int type)
{
	if(type<0 || type>=numItems)
		return NULL;
	return &items[type];
}

word NumItems(void)
{
	return numItems;
}

void UpdateItems(void)
{
	glowism++;

	if(editing==1)
	{
		if((glowism&7)==0)
			rndItem=GetRandomItem();
	}
}

void SetupRandomItems(void)
{
	int i;

	totalRare=0;
	for(i=0;i<numItems;i++)
	{
		totalRare+=items[i].rarity;
	}

	if(totalRare==0)
		totalRare=1;
}

byte GetRandomItem(void)
{
	int i,last;
	int rareNum,curRare;

	rareNum=Random(totalRare);
	curRare=0;
	last=0;
	for(i=0;i<numItems;i++)
	{
		curRare+=items[i].rarity;
		if(items[i].rarity>0 && rareNum<=curRare)
			return i;
		if(items[i].rarity)
			last=i;
	}
	// should never fall through!  But just in case...
	return last;
}

int GetTotalRarity(void)
{
	return totalRare;
}

void SaveItems(FILE *f)
{
	int i;
	word changedItems;
	byte b;

	changedItems=numItems-NUM_ORIGINAL_ITEMS;
	for(i=0;i<NUM_ORIGINAL_ITEMS;i++)
	{
		// has this item been modified in some way?
		if(memcmp(&items[i],&baseItems[i],sizeof(item_t)))
			changedItems++;
	}

	fwrite(&changedItems,1,sizeof(word),f);
	for(i=0;i<NUM_ORIGINAL_ITEMS;i++)
	{
		if(memcmp(&items[i],&baseItems[i],sizeof(item_t)))
		{
			// this item is changed, write it out
			b=(byte)i;
			fwrite(&b,1,sizeof(byte),f);	// write the item number
			fwrite(&items[i],1,sizeof(item_t),f);
		}
	}
	if(numItems>NUM_ORIGINAL_ITEMS)
	{
		b=255;	// indicating custom items are beginning here
		fwrite(&b,1,sizeof(byte),f);
		for(i=NUM_ORIGINAL_ITEMS;i<numItems;i++)
		{
			fwrite(&items[i],1,sizeof(item_t),f);
		}
	}
}

void LoadItems(FILE *f)
{
	int i;
	word changedItems;
	byte getNumber;
	word curItem;

	ExitItems();
	InitItems();

	fread(&changedItems,1,sizeof(word),f);

	getNumber=1;
	for(i=0;i<changedItems;i++)
	{
		if(getNumber)
		{
			curItem=0;
			fread(&curItem,1,sizeof(byte),f);
			if(curItem==255)
			{
				getNumber=0;
				curItem=NUM_ORIGINAL_ITEMS;
			}
		}
		fread(&items[curItem],1,sizeof(item_t),f);
		curItem++;
	}

	if(getNumber==1)	// there were no custom items
		numItems=NUM_ORIGINAL_ITEMS;
	else
		numItems=curItem;
}

byte AppendItems(FILE *f)
{
	int i;
	word changedItems;
	byte getNumber;
	byte curItem;
	item_t garbage;

	stitchItemOffset=numItems;

	fread(&changedItems,1,sizeof(word),f);

	getNumber=1;
	for(i=0;i<changedItems;i++)
	{
		if(getNumber)
		{
			fread(&curItem,1,sizeof(byte),f);
			if(curItem==255)
			{
				getNumber=0;
				curItem=numItems;
			}
		}
		if(curItem<NUM_ORIGINAL_ITEMS)
			fread(&garbage,1,sizeof(item_t),f);	// throw away any mods of regular items
		else
		{
			fread(&items[curItem],1,sizeof(item_t),f);
			curItem++;
			numItems++;
		}
		if(curItem>=MAX_ITEMS)
		{
			return 0;
		}
	}
	return 1;
}

item_t *GetBaseItem(int type)
{
	if(type<0 || type>=NUM_ORIGINAL_ITEMS)
		return NULL;
	return &baseItems[type];
}

int BrainsGiven(int type)
{
	if(type<0 || type>=numItems)
		return 0;
	if(items[type].effect!=IE_BRAIN)
		return 0;
	return items[type].effectAmt;
}

int CandlesGiven(int type)
{
	if(type<0 || type>=numItems)
		return 0;
	if(items[type].effect!=IE_CANDLE)
		return 0;
	return items[type].effectAmt;
}

void RepairItemToItem(int n)	// when item N is deleted, repair references to it and others in
								// all items
{
	int i;

	for(i=0;i<numItems;i++)
	{
		if(items[i].effect==IE_BECOME)
		{
			if(items[i].effectAmt==n)
				items[i].effect=IE_NONE;	// can't become the item, so don't become anything
			else if(items[i].effectAmt>n)
				items[i].effectAmt--;	// must shift down the reference
		}
	}
}

void RepairItemToSound(int n)	// when sound N is deleted, repair references to it and others in
								// all items
{
	int i;

	for(i=0;i<numItems;i++)
	{
		if(items[i].sound==n)
			items[i].sound=0;
		else if(items[i].sound>n)
			items[i].sound--;	// must shift down the reference
	}
}

void RepairItemToTile(void)	// when tiles are messed with, repair the references in items
{
	int i;

	for(i=0;i<numItems;i++)
	{
		if(items[i].flags&IF_TILE)
		{
			items[i].sprNum=GetSwap(items[i].sprNum);
		}
	}
}

//--------------------------------------------------------------------

void ItemGetEffect(int x,int y)
{
	// sparkles and whatnot
	ColorRing(8,(x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
				(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,FIXAMT*5,16,4);
}

byte TriggerItem(Guy *me,mapTile_t *m,int x,int y)
{
	int i;

	switch(items[m->item].effect)
	{
		case IE_NONE:
			return 1;
			break;
		case IE_DESTROY:
			ExplodeParticlesColor(items[m->item].effectAmt,(x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
				(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,FIXAMT*5,8,8);
			m->item=ITM_NONE;
			return 1;
			break;
		case IE_HEAL:
			if(items[m->item].effectAmt>=0)
			{
				if(player.life==goodguy->maxHP)
					return 0;
				PlayerHeal((byte)items[m->item].effectAmt);
			}
			else
				goodguy->GetShot(0,goodguy,0,0,-items[m->item].effectAmt,curMap,&curWorld);
			return 1;
			break;
		case IE_WEAPON:
			return PlayerGetWeapon(items[m->item].effectAmt,x,y);
			break;
		case IE_HAMMER:
			if(player.hammers+items[m->item].effectAmt>5)
				player.hammers=5;
			else if(player.hammers+items[m->item].effectAmt<0)
				player.hammers=0;
			else
				player.hammers+=items[m->item].effectAmt;
			return 1;
			break;
		case IE_PANTS:
			if(player.hamSpeed-items[m->item].effectAmt*4<0)
				player.hamSpeed=0;
			else if(player.hamSpeed-items[m->item].effectAmt*4>20)
				player.hamSpeed=20;
			else
				player.hamSpeed-=items[m->item].effectAmt*4;
			return 1;
			break;
		case IE_KEY:
			if(player.keys[0]+items[m->item].effectAmt>3)
			{
				if(player.keys[0]==3)
					return 0;	// leave it on the ground
				else
				{
					if(!editing && !player.cheated)
					{
						profile.progress.keysFound+=3-player.keys[0];
					}
					player.keys[0]=3;
					return 1;	// go ahead and pick it up if it's a 'multi-key' that would
								// push you over the limit
				}
			}
			else if(player.keys[0]+items[m->item].effectAmt<0)
			{
				if(player.keys[0]==0)
					return 0;	// leave it on the ground
				else
				{
					player.keys[0]=0;
					return 1;	// pick up multi-key-removers that push you under the limit
				}
			}
			else
			{
				if(!editing && !player.cheated)
				{
					profile.progress.keysFound+=items[m->item].effectAmt;
				}
				player.keys[0]+=items[m->item].effectAmt;
			}
			return 1;
			break;
		case IE_RKEY:
			if(items[m->item].effectAmt>0)
			{
				if(player.keys[1])
					return 0;
				else
				{
					if(!editing && !player.cheated)
					{
						profile.progress.keysFound++;
					}
					player.keys[1]=1;
					return 1;
				}
			}
			else
			{
				if(!player.keys[1])
					return 0;
				else
				{
					player.keys[1]=0;
					return 1;
				}
			}
			break;
		case IE_GKEY:
			if(items[m->item].effectAmt>0)
			{
				if(player.keys[2])
					return 0;
				else
				{
					if(!editing && !player.cheated)
					{
						profile.progress.keysFound++;
					}
					player.keys[2]=1;
					return 1;
				}
			}
			else
			{
				if(!player.keys[2])
					return 0;
				else
				{
					player.keys[2]=0;
					return 1;
				}
			}
			break;
		case IE_BKEY:
			if(items[m->item].effectAmt>0)
			{
				if(player.keys[3])
					return 0;
				else
				{
					if(!editing && !player.cheated)
					{
						profile.progress.keysFound++;
					}
					player.keys[3]=1;
					return 1;
				}
			}
			else
			{
				if(!player.keys[3])
					return 0;
				else
				{
					player.keys[3]=0;
					return 1;
				}
			}
			break;
		case IE_WKEY:
			if(items[m->item].effectAmt>0)
			{
				if(player.keys[4])
					return 0;
				else
				{
					if(!editing && !player.cheated)
					{
						profile.progress.keysFound++;
					}
					player.keys[4]=1;
					return 1;
				}
			}
			else
			{
				if(!player.keys[4])
					return 0;
				else
				{
					player.keys[4]=0;
					return 1;
				}
			}
			break;
		case IE_BLKEY:
			if(items[m->item].effectAmt>0)
			{
				if(player.keys[5])
					return 0;
				else
				{
					if(!editing && !player.cheated)
					{
						profile.progress.keysFound++;
					}
					player.keys[5]=1;
					return 1;
				}
			}
			else
			{
				if(!player.keys[5])
					return 0;
				else
				{
					player.keys[5]=0;
					return 1;
				}
			}
			break;
		case IE_LOONYKEY:
			if(items[m->item].effectAmt>0)
			{
				if(!editing && (player.worldProg->keychains&KC_LOONY)==0)
				{
					profile.progress.loonyKeys++;
				}
				player.worldProg->keychains|=KC_LOONY;
			}
			else
			{
				if(!editing && (player.worldProg->keychains&KC_LOONY))
				{
					profile.progress.loonyKeys--;
					if(profile.progress.loonyKeysUsed>profile.progress.loonyKeys)
						profile.progress.loonyKeysUsed=profile.progress.loonyKeys;
				}
				player.worldProg->keychains&=(!KC_LOONY);
			}
			return 1;
			break;
		case IE_COIN:
			PlayerGetCoin(items[m->item].effectAmt);
			return 1;
			break;
		case IE_CANDLE:
			PlayerGetCandle(items[m->item].effectAmt);
			return 1;
			break;
		case IE_OXYGEN:
			if(player.oxygen+items[m->item].effectAmt*256<0)
				player.oxygen=0;
			else if(player.oxygen+items[m->item].effectAmt*256>127*256)
				player.oxygen=127*256;
			else
				player.oxygen+=items[m->item].effectAmt*256;
			return 1;
			break;
		case IE_RAGE:
			if(player.rage+items[m->item].effectAmt*256<0)
				player.rage=0;
			else if(player.rage+items[m->item].effectAmt*256>127*256)
				player.rage=127*256;
			else
				player.rage+=items[m->item].effectAmt*256;
			return 1;
			break;
		case IE_WEIRD:
			PlayerRadioactiveFood();
			return 1;
			break;
		case IE_BECOME:
			m->item=items[m->item].effectAmt;
			return 1;
			break;
		case IE_SUMMON:
			AddGuy((x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,10*FIXAMT,items[m->item].effectAmt,2);
			m->item=ITM_NONE;
			return 1;
			break;
		case IE_POWERUP:
			return PlayerPowerup(items[m->item].effectAmt);
			break;
		case IE_HEALRANDOM:
			if(player.life==goodguy->maxHP)
				return 0;
			PlayerChineseFood();
			return 1;
			break;
		case IE_BRAIN:
			PlayerGetBrain(items[m->item].effectAmt);
			return 1;
			break;
		case IE_KEYCHAIN:
			if(items[m->item].effectAmt==0)
				player.worldProg->keychains|=KC_KEYCH1;
			else if(items[m->item].effectAmt==1)
				player.worldProg->keychains|=KC_KEYCH2;
			else if(items[m->item].effectAmt==2)
				player.worldProg->keychains|=KC_KEYCH3;
			else if(items[m->item].effectAmt==3)
				player.worldProg->keychains|=KC_KEYCH4;
			KeyChainAllCheck();
			// keychain get effect
			return 1;
			break;
		case IE_DOOR:
			if(!me || (me->aiType!=MONS_BOUAPHA&&me->aiType!=MONS_GOODBONE))
				return 0;	// won't go away for non-Bouaphas
			else if(player.keys[items[m->item].effectAmt])
			{
				//DoorOpenEffect();
				if(items[m->item].effectAmt==0)	// yellow keys get used up
					player.keys[items[m->item].effectAmt]--;
				m->item=ITM_NONE;
				ScoreEvent(SE_DOOR,1);
				if(!editing && !player.cheated)
				{
					profile.progress.doorsOpened++;
					if(profile.progress.doorsOpened>=100)
						CompleteGoal(18);
				}
				return 1;
			}
			else return 0;
			break;
		case IE_ORBITER:
			if(items[m->item].effectAmt<0)
				RemoveOrbiters(-items[m->item].effectAmt,1,BLT_ORBITER);
			else
			{
				for(i=0;i<items[m->item].effectAmt;i++)
					FireBullet(0,goodguy->x,goodguy->y,Random(8),BLT_ORBITER,1);
			}
			return 1;
			break;
		case IE_ORBITER2:
			if(items[m->item].effectAmt<0)
				RemoveOrbiters(-items[m->item].effectAmt,1,BLT_ORBITER2);
			else
			{
				for(i=0;i<items[m->item].effectAmt;i++)
					FireBullet(0,goodguy->x,goodguy->y,Random(8),BLT_ORBITER2,1);
			}
			return 1;
			break;
		case IE_ORBITER3:
			if(items[m->item].effectAmt<0)
				RemoveOrbiters(-items[m->item].effectAmt,1,BLT_ORBITER3);
			else
			{
				for(i=0;i<items[m->item].effectAmt;i++)
					FireBullet(0,goodguy->x,goodguy->y,Random(8),BLT_ORBITER3,1);
			}
			return 1;
			break;
		case IE_ORBITER4:
			if(items[m->item].effectAmt<0)
				RemoveOrbiters(-items[m->item].effectAmt,1,BLT_ORBITER4);
			else
			{
				for(i=0;i<items[m->item].effectAmt;i++)
					FireBullet(0,goodguy->x,goodguy->y,Random(8),BLT_ORBITER4,1);
			}
			return 1;
			break;
		case IE_PUSH:
			if(!me)
			{
				i=0;
				if(items[m->item].trigger&ITR_SHOOT)
				{
					int bdx,bdy;

					GetBulletDeltas(&bdx,&bdy);
					i=0;

					if(abs(bdx)>abs(bdy))
					{
						if(bdx>0)
							i=TryToPushItem(x,y,x+1,y,curMap,&curWorld);
						else
							i=TryToPushItem(x,y,x-1,y,curMap,&curWorld);
					}
					else if(bdx!=0 && bdy!=0)	// don't do anything for non-moving bullets
					{
						if(bdy>0)
							i=TryToPushItem(x,y,x,y+1,curMap,&curWorld);
						else
							i=TryToPushItem(x,y,x,y-1,curMap,&curWorld);
					}
				}
				return i;
			}
			else
			{
				if(me->mapx!=x && me->mapy!=y)
					return 0;	// you can't be diagonal from it
				if(me->dx==0 && me->dy==0)
					return 0;	// doesn't work if you're non-moving
				if(me->mapx==x && me->mapy==y)
				{
					// in the case of pushable non-obstacles, this could be an issue
					if(abs(me->dx)>abs(me->dy))
					{
						if(me->dx<0)
							i=2;
						else
							i=0;
					}
					else
					{
						if(me->dy<0)
							i=3;
						else
							i=1;
					}
				}
				else
				{
					if(me->mapx<x)
						i=0;
					else if(me->mapy<y)
						i=1;
					else if(me->mapx>x)
						i=2;
					else
						i=3;
				}
				switch(i)
				{
					case 0:
						return TryToPushItem(x,y,x+1,y,curMap,&curWorld);
						break;
					case 1:
						return TryToPushItem(x,y,x,y+1,curMap,&curWorld);
						break;
					case 2:
						return TryToPushItem(x,y,x-1,y,curMap,&curWorld);
						break;
					case 3:
						return TryToPushItem(x,y,x,y-1,curMap,&curWorld);
						break;
				}
			}
			return 1;
			break;
		case IE_INCVAR:
			SetVar(items[m->item].effectAmt,GetVar(items[m->item].effectAmt)+1);
			return 1;
			break;
		case IE_DECVAR:
			SetVar(items[m->item].effectAmt,GetVar(items[m->item].effectAmt)-1);
			return 1;
			break;
		case IE_MOVE:
			curMap->map[x+y*curMap->width].opaque=1;
			return 0;
			break;
	}
	return 0;
}

void MoveMovableItem(int x,int y,Map *map,world_t *world)
{
	byte yes;
	byte type;

	type=map->map[x+y*map->width].item;
	switch(items[map->map[x+y*map->width].item].effectAmt)
	{
		case 0:
			yes=TryToPushItem(x,y,x+1,y,curMap,&curWorld);
			break;
		case 1:
			yes=TryToPushItem(x,y,x,y+1,curMap,&curWorld);
			break;
		case 2:
			yes=TryToPushItem(x,y,x-1,y,curMap,&curWorld);
			break;
		case 3:
			yes=TryToPushItem(x,y,x,y-1,curMap,&curWorld);
			break;
		default:
			yes=0;
			break;
	}
	if(yes)
	{
		MakeCustomSound(items[type].sound,(x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,SND_CUTOFF,1000);
		if(items[type].msg[0])
			NewMessage(items[type].msg,75,0);
	}
}

byte InteractWithItem(Guy *me,mapTile_t *m,int x,int y)
{
	byte result;
	byte type;

	type=m->item;

	if(shopping && type>=NUM_ORIGINAL_ITEMS && me->aiType==MONS_BOUAPHA)
	{
		InitShopping(x,y);	// bumped a shoppable item in the mall
		return 0;
	}

	result=1;

	// solid item, can't move through it
	if((!(MonsterFlags(me->type,me->aiType)&MF_FLYING) && items[type].flags&IF_SOLID) ||
		((MonsterFlags(me->type,me->aiType)&MF_FLYING) && items[type].flags&IF_BULLETPROOF))
		result=0;
	if(MonsterFlags(me->type,me->aiType)&MF_WALLWALK)
		result=1;	// no item can contain a ghost!

	// pickup-able item
	if((me->aiType==MONS_BOUAPHA||me->aiType==MONS_GOODBONE) && (items[type].flags&IF_PICKUP) && me->hp>0)
	{
		result=1;
		if(items[type].trigger&ITR_GET)
			result=TriggerItem(me,m,x,y);	// returns 0 if you can't pick it up

		if(result==1)
		{
			MakeCustomSound(items[type].sound,(x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,SND_CUTOFF,1000);
			if(items[type].msg[0])
				NewMessage(items[type].msg,75,0);

			ItemGetEffect(x,y);
			m->item=ITM_NONE;
			EventOccur(EVT_GET,type,x,y,me);
			return 1;
		}
		else
		{
			if((!(MonsterFlags(me->type,me->aiType)&MF_FLYING) && items[type].flags&IF_SOLID) ||
				((MonsterFlags(me->type,me->aiType)&MF_FLYING) && items[type].flags&IF_BULLETPROOF))
				return 0;
			else
				return 1;
		}
	}

	if(((me->aiType==MONS_BOUAPHA||me->aiType==MONS_GOODBONE) && (items[type].trigger&(ITR_PLAYERBUMP|ITR_FRIENDBUMP))) ||
	   (me->friendly && (items[type].trigger&ITR_FRIENDBUMP)) ||
	   (!me->friendly && (items[type].trigger&ITR_ENEMYBUMP)))
	{
		if(TriggerItem(me,m,x,y))
		{
			MakeCustomSound(items[type].sound,(x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,SND_CUTOFF,1000);
			if(items[type].msg[0])
				NewMessage(items[type].msg,75,0);
		}
	}

	if((me->aiType==MONS_MINECART || me->aiType==MONS_YUGO || me->aiType==MONS_AUTOZOID) && (items[type].trigger&ITR_MINECART))
	{
		if(TriggerItem(me,m,x,y))
		{
			MakeCustomSound(items[type].sound,(x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,SND_CUTOFF,1000);
			if(items[type].msg[0])
				NewMessage(items[type].msg,75,0);
		}
		if(!(items[m->item].flags&IF_SOLID))
			result=1;	// special case: if the minecart hit smashed it away, then count it passable immediately
	}

	return result;
}

byte BulletHitItem(byte bulType,mapTile_t *m,int x,int y)
{
	byte result;
	byte type;

	type=m->item;
	result=1;

	// bulletproof, no getting past it
	if(items[type].flags&IF_BULLETPROOF)
		result=0;

	if(items[type].trigger&ITR_SHOOT)
	{
		if(TriggerItem(NULL,m,x,y))
		{
			MakeCustomSound(items[type].sound,(x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,SND_CUTOFF,1000);
			if(items[type].msg[0])
				NewMessage(items[type].msg,75,0);
		}
	}
	if(bulType==BLT_SLASH && (items[type].trigger&ITR_CHOP))
	{
		if(TriggerItem(NULL,m,x,y))
		{
			if(!editing && !player.cheated)
				profile.progress.grassChopped++;
			if(profile.progress.grassChopped>=150)
				CompleteGoal(36);

			MakeCustomSound(items[type].sound,(x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,SND_CUTOFF,1000);
			if(items[type].msg[0])
				NewMessage(items[type].msg,75,0);
		}
	}
	if((bulType==BLT_FLAME||bulType==BLT_FLAME2||bulType==BLT_FLAME3||bulType==BLT_FLAME4||bulType==BLT_FLAME5||
	bulType==BLT_SITFLAME||bulType==BLT_BADSITFLAME||bulType==BLT_FLAMEWALL) && (items[type].trigger&ITR_BURNT))
	{
		if(TriggerItem(NULL,m,x,y))
		{
			if(!editing && !player.cheated)
				profile.progress.hayBurned++;
			if(profile.progress.hayBurned>=250)
				CompleteGoal(35);

			MakeCustomSound(items[type].sound,(x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,SND_CUTOFF,1000);
			if(items[type].msg[0])
				NewMessage(items[type].msg,75,0);
		}
	}
	if((bulType==BLT_FREEZE||bulType==BLT_FREEZE2||bulType==BLT_ICESHARD) &&
	(items[type].trigger&ITR_FROZEN))
	{
		if(TriggerItem(NULL,m,x,y))
		{
			if(!editing && !player.cheated)
				profile.progress.hayBurned++;
			if(profile.progress.hayBurned>=250)
				CompleteGoal(35);

			MakeCustomSound(items[type].sound,(x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,SND_CUTOFF,1000);
			if(items[type].msg[0])
				NewMessage(items[type].msg,75,0);
		}
	}

	return result;
}

void UpdateItem(mapTile_t *m,int width,int offset)
{
	int x,y;

	if(items[m->item].trigger==ITR_ALWAYS)
	{
		x=offset%width;
		y=offset/width;
		if(TriggerItem(NULL,m,x,y))
		{
			MakeCustomSound(items[m->item].sound,(x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,SND_CUTOFF,1000);
			if(items[m->item].msg[0])
				NewMessage(items[m->item].msg,75,0);
		}
	}
	if((items[m->item].flags&IF_BUBBLES) && Random(7)==0)
	{
		x=offset%width;
		y=offset/width;
		BlowBubble((x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,0,FIXAMT/2);
	}
}

int FindItemByName(const char *name)
{
	int i;

	for(i=0;i<numItems;i++)
	{
		if(!strcmp(name,items[i].name))
			return i;
	}
	return 0;
}

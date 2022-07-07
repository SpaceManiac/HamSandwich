#include "monster.h"
#include "player.h"
#include "options.h"
#include "bullet.h"
#include "quest.h"
#include "skill.h"
#include "ch_loony.h"
#include "leveldef.h"
#include "madcap.h"
#include "achieve.h"

monsterType_t monsType[NUM_MONSTERS]=
	{
		{"Null",{255,255,0},	// these numbers are {color to replace, color to replace with,brightness modifier}
		 0,0,0,0,0,0,0,0,"",0},
		{"Loony",{255,255,0},
		 0,11,50,28,128,0,0,0,"graphics/loony.jsp",0,MF_WATERWALK,ML_WALLBUMP|ML_GOOD,NULL,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,0,255},	// move
				{12,13,14,15,16,17,18,19,255},	// attack
				{0,0,0,0,0,0,25,25,26,27,255},		// die
				{20,21,21,21,20,255},	// A1=use item
				{22,23,24,24,24,24,24,24,24,24,24,24,23,22,255},	// A2=bored
				{7,7,8,8,8,8,8,8,8,8,8,7,7,255},	// A3=jump
				{9,10,11,10,9,8,8,8,8,8,8,8,7,7,255},	//A4=air attack
			}},
		{"Teddy",{255,255,0},
		 15,11,50,27,20,2,7,3*FIXAMT,"graphics/teddy.jsp",0,MF_NONE,ML_NONE,AI_Teddy,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,255},	// attack
				{22,23,24,25,26,254,26,254,26,254,26,254,26,254,26,254,26,255},		// die
				{13,14,15,16,17,18,19,20,21,255},	// A1 = bellyflop
			}},
		{"Villager",{255,255,0},
		 0,11,50,28,128,0,0,0,"graphics/villager.jsp",0,MF_ONEFACE|MF_FACECMD|MF_INVINCIBLE|MF_NOMOVE,ML_GOOD,AI_Villager,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{0,255},		// die
			}},
		{"Axe Trap",{255,255,0},
		 1,12,20,6,10,3,1,0,"graphics/turret.jsp",0,MF_NOMOVE|MF_FREEWALK,ML_GOOD|ML_NOMIND|ML_ALWAYSACTIVE,AI_AxeTrap,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,255},	// attack
				{0,0,254,254,254,255},		// die
			}},
		{"Bonehead",{255,255,0},
		 1,11,50,23,10,0,1,3*FIXAMT,"graphics/bonehead.jsp",0,MF_FREEWALK,ML_GOOD,AI_Bonehead,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{15,16,17,18,19,20,21,22,255},	// attack
				{7,8,9,10,11,12,13,14,254,14,254,14,254,14,254,14,254,14,255},		// die
				{14,13,12,11,10,9,8,7,255},		// A1=undie
			}},
		{"Squeaker",{255,255,0},
		 1,8,20,19,2,0,3,3*FIXAMT,"graphics/mouse.jsp",0,MF_NONE,ML_WOODS,AI_Mouse,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,0,255},	// move
				{0,255},	// attack
				{8,9,10,11,12,13,14,254,15,254,16,254,17,254,18,255},		// die
			}},
		{"Punch",{1,4,2},
		 3,11,50,28,6,0,4,3*FIXAMT,"graphics/puppet.jsp",0,MF_NONE,ML_WOODS,AI_Puppet,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{7,8,9,10,11,12,255},	// attack (punch)
				{24,25,26,27,254,27,254,27,254,27,254,27,254,27,255},		// die
				{5,5,6,6,6,6,5,5,255},	// A1=spit
				{13,14,15,16,17,18,19,20,21,22,23,15,14,13,255},	// A2=hat trick
			}},
		{"Cryozoid",{255,255,0},
		 1,11,50,19,10,0,1,0,"graphics/cryozoid.jsp",0,MF_NOMOVE|MF_FREEWALK|MF_NOHIT,ML_ALWAYSACTIVE|ML_GOOD,AI_Cryozoid,
			{
				{15,255},	// idle
				{15,255},	// move
				{10,11,12,13,14,255},	// attack
				{16,17,18,19,255},		// die
				{0,1,2,3,4,5,6,7,8,9,255},	// A1 = get born
			}},
		{"Shroom",{255,255,0},
		 1,11,30,13,1,0,1,2*FIXAMT,"graphics/shroom.jsp",0,MF_NONE,ML_NONE,AI_Shroom,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{0,255},	// attack
				{7,8,9,10,11,12,254,12,254,12,254,12,254,12,254,12,254,12,254,12,254,12,254,12,255},		// die
			}},
		{"Magic Hat",{255,255,0},
		 15,11,50,28,30,1,5,2*FIXAMT,"!7",0,MF_NONE,ML_BOSS,AI_Puppet,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{7,8,9,10,11,12,255},	// attack (punch)
				{24,25,26,27,254,27,254,27,254,27,254,27,254,27,255},		// die
				{5,5,6,6,6,6,5,5,255},	// A1=spit
				{13,14,15,16,17,18,19,20,21,22,23,15,14,13,255},	// A2=hat trick
			}},
		{"Tinny Tim",{4,3,0},
		 9,11,50,32,14,1,4,2*FIXAMT,"graphics/tinguy.jsp",0,MF_NONE,ML_TUNDRA,AI_TinGuy,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{5,6,7,8,9,10,11,12,255},	// attack (shoot)
				{23,24,25,26,27,28,29,30,31,254,31,254,31,254,31,254,31,255},		// die
				{13,14,13,255},	// A1=block
				{15,16,17,18,19,20,21,22,255},	// A2=shoot into sky
			}},
		{"Rat-A-Tat",{5,2,0},
		 2,8,20,19,3,0,3,1*FIXAMT,"!6",0,MF_NONE,ML_WOODS,AI_Mouse,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,0,255},	// move
				{0,255},	// attack
				{8,9,10,11,12,13,14,254,15,254,16,254,17,254,18,255},		// die
				{8,9,8,9,8,9,8,255},	// A1=rat-a-tat
			}},
		{"Cave Bear",{2,0,3},
		 6,11,50,27,10,0,5,3*FIXAMT,"!2",0,MF_NONE,ML_CAVE,AI_Teddy,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,255},	// attack
				{22,23,24,25,26,254,26,254,26,254,26,254,26,254,26,254,26,255},		// die
				{13,14,15,16,17,18,19,20,21,255},	// A1 = bellyflop
			}},
		{"Ratatouille",{5,6,0},
		 15,8,20,19,100,10,9,8*FIXAMT,"!6",0,MF_NONE,ML_BOSS|ML_WALLBUMP,AI_Ratatouille,
			{
				{7,6,5,4,3,2,1,0,255},	// idle
				{1,2,3,4,5,6,7,0,255},	// move
				{0,255},	// attack
				{8,9,10,11,12,13,14,254,15,254,16,254,17,254,18,255},		// die
				{8,9,8,9,8,9,8,255},	// A1=rat-a-tat
			}},
		{"Sock Monkey",{255,255,0},
		 8,11,40,28,8,0,5,2*FIXAMT,"graphics/monkey.jsp",0,MF_NONE,ML_WOODS|ML_TUNDRA,AI_SockMonkey,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{5,6,7,7,7,7,8,8,255},	// attack (charge up dash)
				{22,23,24,24,24,24,25,26,27,27,254,27,254,27,254,27,254,27,255},		// die
				{8,9,10,11,10,9,8,9,10,11,10,9,8,9,10,11,10,9,8,9,10,11,10,9,8,4,3,0,255},	// A1=do the dash
				{12,13,14,15,16,17,18,19,20,21,255},	// A2=shoot
			}},
		{"Klonk",{255,255,0},
		 20,20,70,19,30,1,10,3*FIXAMT,"graphics/klonk.jsp",0,MF_NOMOVE,ML_BOSS|ML_WALLBUMP,AI_Klonk,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{5,6,7,8,9,10,10,10,10,11,11,11,12,12,255},	// attack (smash down)
				{13,14,15,16,17,18,254,18,254,18,254,18,254,18,254,18,255},		// die
			}},
		{"Toasties",{255,255,0},
		 1,5,10,6,1,0,3,6*FIXAMT,"graphics/toasty.jsp",0,MF_NOHIT|MF_GLOW|MF_ONEFACE|MF_FLYING|MF_WALLWALK|MF_FREEWALK|MF_NOSHADOW|MF_ENEMYWALK,ML_GOOD,AI_Toasty,
			{
				{0,1,2,3,4,5,255},	// idle
				{0,1,2,3,4,5,255},	// move
				{0,255},	// attack
				{0,254,1,254,2,254,3,255},		// die
			}},
		{"Klunk",{255,255,-5},
		 20,20,70,19,30,1,7,5*FIXAMT,"!16",0,MF_NOMOVE,ML_BOSS|ML_WALLBUMP,AI_Klonk,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{5,6,7,8,9,10,10,10,10,11,11,11,12,12,255},	// attack (smash down)
				{13,14,15,16,17,18,254,18,254,18,254,18,254,18,254,18,255},		// die
			}},
		{"Ice Mice",{5,7,3},
		 10,8,20,19,10,1,4,4*FIXAMT,"!6",0,MF_ICEONLY,ML_WALLBUMP,AI_Mouse,
			{
				{7,6,5,4,3,2,1,0,255},	// idle
				{1,2,3,4,5,6,7,0,255},	// move
				{0,255},	// attack
				{8,9,10,11,12,13,14,254,15,254,16,254,17,254,18,255},		// die
				{8,9,9,9,9,9,8,255},	// A1=rat-a-tat
			}},
		{"Stone Rose",{1,0,0},
		 20,24,70,15,30,8,12,0*FIXAMT,"graphics/rose.jsp",0,MF_NOMOVE|MF_ONEFACE,ML_BOSS,AI_Rose,
			{
				{0,255},	// idle
				{7,8,9,10,11,12,13,14,255},	// move (bury yourself)
				{1,2,3,4,5,6,5,4,3,2,1,255},	// attack
				{7,8,9,10,11,12,13,14,255},		// die
				{14,13,12,11,10,9,8,7,255},	// A1=unbury if buried
				{254,254,254,255},	// A2=sit around buried
			}},
		{"Ice Hound",{255,255,0},
		 15,20,50,13,15,2,7,4*FIXAMT,"graphics/hound.jsp",0,MF_NOMOVE,0,AI_Hound,
			{
				{0,255},	// idle
				{1,2,3,4,5,0,255},	// move
				{6,7,8,8,8,8,7,6,255},	// attack
				{9,10,11,12,254,12,254,12,254,12,254,12,254,12,254,12,255},		// die
				{1,2,3,4,5,0,1,2,3,4,5,0,1,2,3,4,5,0,1,2,3,4,5,0,255},	// A1=charge
				{6,7,8,8,8,255},	// A2=get ready to charge
			}},
		{"Rosie O'Bomber",{255,255,0},
		 14,24,70,15,30,2,10,0*FIXAMT,"!20",0,MF_NOMOVE|MF_ONEFACE,0,AI_Rose,
			{
				{0,255},	// idle
				{7,8,9,10,11,12,13,14,255},	// move (bury yourself)
				{1,2,3,4,5,6,5,4,3,2,1,255},	// attack
				{7,8,9,10,11,12,13,14,255},		// die
				{14,13,12,11,10,9,8,7,255},	// A1=unbury if buried
				{254,254,254,255},	// A2=sit around buried
			}},
		{"Stinky Sock",{4,1,0},
		 15,11,40,28,30,2,10,2*FIXAMT,"!15",0,MF_NONE,ML_BOSS,AI_SockMonkey,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{5,6,7,7,7,7,8,8,255},	// attack (charge up dash)
				{22,23,24,24,24,24,25,26,27,27,254,27,254,27,254,27,254,27,255},		// die
				{8,9,10,11,10,9,8,9,10,11,10,9,8,9,10,11,10,9,8,9,10,11,10,9,8,4,3,0,255},	// A1=do the dash
				{12,13,14,15,16,17,18,19,20,21,255},	// A2=shoot
			}},
		{"KO Punch",{1,6,0},
		 15,11,50,28,20,3,12,3*FIXAMT,"!7",0,MF_NONE,0,AI_Puppet,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{7,8,9,10,11,12,255},	// attack (punch)
				{24,25,26,27,254,27,254,27,254,27,254,27,254,27,255},		// die
				{5,5,6,6,6,6,5,5,255},	// A1=spit
				{13,14,15,16,17,18,19,20,21,22,23,15,14,13,255},	// A2=hat trick
			}},
		{"Berserker Bear",{2,6,0},
		 25,11,50,27,30,1,8,2*FIXAMT,"!2",0,MF_NONE,ML_CAVE|ML_BOSS,AI_Teddy,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,255},	// attack
				{22,23,24,25,26,254,26,254,26,254,26,254,26,254,26,254,26,255},		// die
				{13,14,15,16,17,18,19,20,21,255},	// A1 = bellyflop
			}},
		{"Toymotron",{255,255,0},
		 30,30,50,12,50,3,0,0*FIXAMT,"graphics/toymaker.jsp",0,MF_NOMOVE|MF_ONEFACE,ML_NOMIND,AI_Toymaker,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,7,8,9,10,11,255},	// attack (generate)
				{0,0,0,0,0,255},		// die
			}},
		{"Power Core",{255,255,0},
		 50,40,50,12,120,4,15,0*FIXAMT,"graphics/toypower.jsp",0,MF_NOMOVE|MF_ONEFACE,ML_BOSS|ML_ALWAYSACTIVE|ML_NOMIND,AI_Toypower,
			{
				{0,1,2,3,4,5,6,7,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{8,9,10,11,254,11,254,11,254,11,254,11,255},		// die
			}},
		{"Power Crystal",{255,255,0},
		 30,20,50,16,30,1,15,0*FIXAMT,"graphics/toycrystal.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_FLYING,ML_BOSS|ML_ALWAYSACTIVE|ML_NOMIND,AI_Toycrystal,
			{
				{0,1,2,3,4,5,6,7,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{8,9,10,11,12,13,14,15,255},		// die
			}},
		{"Tin Soldier",{255,255,0},
		 20,11,50,32,30,2,10,2*FIXAMT,"!11",0,MF_NONE,0,AI_TinGuy,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{5,6,7,8,9,10,11,12,255},	// attack (shoot)
				{23,24,25,26,27,28,29,30,31,254,31,254,31,254,31,254,31,255},		// die
				{13,14,13,255},	// A1=block
				{15,16,17,18,19,20,21,22,255},	// A2=shoot into sky
			}},
		{"Lifeberry Bush",{255,255,0},
		 1,11,30,8,10,0,1,0*FIXAMT,"graphics/berry.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_NOHIT,ML_GOOD|ML_ALWAYSACTIVE,AI_Lifeberry,
			{
				{5,255},	// idle
				{0,1,2,3,4,255},	// move (dig up)
				{6,7,6,7,255},	// attack (shoot berry)
				{4,3,2,1,0,255},		// die
			}},
		{"Plague Rat",{5,1,0},
		 7,8,20,19,6,0,4,2*FIXAMT,"!6",0,MF_NONE,ML_WOODS,AI_Mouse,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,0,255},	// move
				{0,255},	// attack
				{8,9,10,11,12,13,14,254,15,254,16,254,17,254,18,255},		// die
			}},
		{"Dandy Lion",{255,255,0},
		 18,14,40,11,30,2,10,3*FIXAMT,"graphics/lion.jsp",0,MF_NONE,0,AI_Lion,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,8,0,255},	// move
				{9,10,9,255},	// attack
				{9,9,10,10,254,254,254,255},		// die
			}},
		{"Bruiser",{7,4,0},
		 22,20,70,19,40,10,13,5*FIXAMT,"!16",0,MF_NOMOVE,ML_ALWAYSACTIVE,AI_Klonk,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{5,6,7,8,9,10,10,10,10,11,11,11,12,12,255},	// attack (smash down)
				{13,14,15,16,17,18,254,18,254,18,254,18,254,18,254,18,255},		// die
			}},
		{"Burning Bush",{1,4,0},
		 25,14,40,11,90,4,15,4*FIXAMT,"!32",0,MF_NONE,ML_BOSS,AI_Lion,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,8,0,255},	// move
				{9,10,9,255},	// attack
				{9,9,10,10,254,254,254,255},		// die
			}},
		{"Onion Thug",{255,255,0},
		 20,11,40,19,30,6,10,4*FIXAMT,"graphics/thug.jsp",0,MF_NONE,ML_ALWAYSACTIVE,AI_Thug,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{10,11,12,13,14,14,14,14,14,15,255},	// attack
				{16,17,18,254,18,254,18,254,18,254,18,254,18,255},		// die
				{7,8,9,8,9,8,9,8,9,8,7,255},	// A1=wave
			}},
		{"Ghost",{255,255,0},
		 1,8,20,8,30,1,1,6*FIXAMT,"graphics/ghost.jsp",0,MF_GLOW|MF_NOHIT|MF_FREEWALK|MF_ENEMYWALK|MF_WALLWALK|MF_FLYING,ML_GOOD|ML_ALWAYSACTIVE,AI_Ghost,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,2,1,255},	// attack
				{4,5,6,7,255},		// die
				{7,6,5,4,255},	//A1=fade in
			}},
		{"Trigun",{4,6,-4},
		 30,11,50,32,60,6,20,2*FIXAMT,"!11",0,MF_NONE,ML_BOSS,AI_TinGuy,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{5,6,7,8,9,10,11,12,255},	// attack (shoot)
				{23,24,25,26,27,28,29,30,31,254,31,254,31,254,31,254,31,255},		// die
				{13,14,13,255},	// A1=block
				{15,16,17,18,19,20,21,22,255},	// A2=shoot into sky
			}},
		{"Junksmith",{255,255,0},
		 30,11,50,19,60,6,20,2*FIXAMT,"graphics/junksmith.jsp",0,MF_NONE,ML_BOSS|ML_ALWAYSACTIVE,AI_Junksmith,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,15,15,15,15,15,16,17,18,255},	// attack (shoot)
				{7,8,9,10,11,12,13,14,15,254,15,254,15,254,15,254,15,254,15,255},		// die
			}},
		{"Caravan",{255,255,0},
		 1,20,50,15,20,2,30,7*FIXAMT,"graphics/caravan.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_ENEMYWALK|MF_FREEWALK|MF_WALLWALK,ML_ALWAYSACTIVE|ML_NOMIND,AI_Caravan,
			{
				{0,1,2,3,4,5,6,7,255},	// idle
				{0,1,2,3,4,5,6,7,255},	// move
				{0,255},	// attack
				{8,9,10,11,12,13,14,254,14,254,14,254,14,255},		// die
			}},
		{"Junk Trap",{5,2,-2},
		 1,12,20,6,30,0,15,0,"!4",0,MF_NOMOVE|MF_ENEMYWALK,ML_NOMIND|ML_ALWAYSACTIVE,AI_AxeTrap,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,255},	// attack
				{0,0,254,254,254,255},		// die
			}},
		{"Order Knight",{255,255,0},
		 20,11,40,19,30,6,12,4*FIXAMT,"graphics/bunnytroop.jsp",0,MF_NONE,ML_ALWAYSACTIVE,AI_BunnyTroop,
			{
				{0,255},	// idle
				{2,3,4,3,2,1,5,6,7,6,5,1,255},	// move
				{8,9,10,11,12,13,14,255},	// attack
				{15,16,17,18,254,18,254,18,254,18,254,18,254,18,254,18,255},		// die
			}},
		{"Major Fishbug",{255,255,0},
		 20,11,40,19,90,10,10,4*FIXAMT,"graphics/bunnyguy.jsp",0,0,ML_GOOD|ML_ALWAYSACTIVE,AI_BunnyGuy,
			{
				{0,255},	// idle
				{2,3,4,3,2,1,5,6,7,6,5,1,255},	// move
				{8,9,10,11,12,13,14,255},	// attack
				{15,16,17,18,254,18,254,18,254,18,254,18,254,18,254,18,255},		// die
			}},
		{"Captain Capitan",{255,255,0},
		 40,11,40,19,150,6,15,5*FIXAMT,"graphics/bunnyboss.jsp",0,MF_NONE,ML_BOSS|ML_ALWAYSACTIVE|ML_WALLBUMP,AI_BunnyBoss,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{11,12,12,12,12,12,12,12,12,12,12,12,12,12,11,11,11,0,0,255},	// attack (spin slash)
				{7,8,9,10,254,10,254,10,254,10,254,10,254,10,254,10,255},		// die
				{13,14,14,14,14,14,14,13,255},	// A1=block
				{15,16,17,18,255},	// A2=slash on blocked hit
			}},
		{"Clockwork Robot",{255,255,0},
		 22,16,40,21,50,5,16,4*FIXAMT,"graphics/clockbot.jsp",0,MF_NONE,ML_ALWAYSACTIVE,AI_ClockBotEvil,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,0,255},	// move
				{8,9,10,11,12,13,14,255},	// attack
				{15,16,17,18,19,20,254,20,254,20,254,20,254,20,255},		// die
			}},
		{"Clockwork Pal",{255,255,0},
		 0,11,40,21,50,5,13,5*FIXAMT,"!44",0,MF_FREEWALK,ML_GOOD|ML_ALWAYSACTIVE,AI_ClockBot,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,0,255},	// move
				{8,9,10,11,12,13,14,255},	// attack
				{15,16,17,18,19,20,254,20,254,20,254,20,254,20,255},		// die
			}},
		{"Shock Monkey",{4,7,3},
		 21,11,40,28,40,2,14,3*FIXAMT,"!15",0,MF_NONE,0,AI_SockMonkey,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{5,6,7,7,7,7,8,8,255},	// attack (charge up dash)
				{22,23,24,24,24,24,25,26,27,27,254,27,254,27,254,27,254,27,255},		// die
				{8,9,10,11,10,9,8,9,10,11,10,9,8,9,10,11,10,9,8,9,10,11,10,9,8,4,3,0,255},	// A1=do the dash
				{12,13,14,15,16,17,18,19,20,21,255},	// A2=shoot
			}},
		{"Ghost Dog",{255,255,0},
		 20,20,50,13,50,2,20,5*FIXAMT,"!21",0,MF_GLOW|MF_NOSHADOW|MF_ENEMYWALK|MF_WALLWALK|MF_FREEWALK,0,AI_Hound,
			{
				{0,255},	// idle
				{1,2,3,4,5,0,255},	// move
				{6,7,8,8,8,8,7,6,255},	// attack
				{9,10,11,12,254,12,254,12,254,12,254,12,254,12,254,12,255},		// die
				{1,2,3,4,5,0,1,2,3,4,5,0,1,2,3,4,5,0,1,2,3,4,5,0,255},	// A1=charge
				{6,7,8,8,8,255},	// A2=get ready to charge
			}},
		{"Ghost Monkey",{255,255,0},
		 20,11,40,28,40,2,15,4*FIXAMT,"!15",0,MF_GLOW|MF_NOSHADOW|MF_ENEMYWALK|MF_WALLWALK|MF_FREEWALK,0,AI_SockMonkey,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{5,6,7,7,7,7,8,8,255},	// attack (charge up dash)
				{22,23,24,24,24,24,25,26,27,27,254,27,254,27,254,27,254,27,255},		// die
				{8,9,10,11,10,9,8,9,10,11,10,9,8,9,10,11,10,9,8,9,10,11,10,9,8,4,3,0,255},	// A1=do the dash
				{12,13,14,15,16,17,18,19,20,21,255},	// A2=shoot
			}},
		{"Rock Crab",{255,255,0},
		 20,11,15,20,15,8,10,5*FIXAMT,"graphics/rockcrab.jsp",0,MF_NONE,0,AI_Crab,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,0,255},	// move
				{8,9,10,11,12,13,255},	// attack
				{14,15,16,17,254,18,254,19,254,18,254,17,254,18,254,19,254,18,254,17,254,18,255},		// die
			}},
		{"Bodzha",{255,255,0},
		 20,14,40,26,30,3,18,4*FIXAMT,"graphics/badger.jsp",0,MF_NONE,0,AI_Badger,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{5,6,7,8,9,255},	// attack
				{12,13,14,254,14,254,14,254,14,254,14,254,14,254,14,254,14,254,14,255},		// die
				{10,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,10,10,10,0,0,0,255},	// A1= spin slash
				{15,16,17,18,19,20,255},	// A2=burrow
				{21,22,23,24,25,255},	// A3=arise
				{254,255},	// A4=invisible, burrowing
			}},
		{"Ice Titan",{255,255,0},
		 30,20,60,23,70,4,18,3*FIXAMT,"graphics/titan.jsp",0,MF_GLOW,0,AI_Titan,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{12,13,14,15,15,15,16,255},	// attack
				{17,18,19,20,21,22,254,22,254,22,254,22,254,22,254,22,254,22,255},		// die
				{5,6,7,8,9,9,9,10,11,255},	// A1= roar
			}},
		{"Gourdzilla",{255,255,0},
		 40,80,60,13,300,12,18,0*FIXAMT,"graphics/bigplant.jsp",0,MF_NOMOVE|MF_ONEFACE,ML_BOSS|ML_ALWAYSACTIVE,AI_BigPlant,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,7,8,255},	// attack
				{9,10,11,12,254,12,254,12,254,12,254,12,254,12,254,12,255},		// die
			}},
		{"Furnace",{255,255,0},
		 30,60,60,12,90,4,18,3*FIXAMT,"graphics/furnace.jsp",0,MF_NOMOVE|MF_INVINCIBLE|MF_ONEFACE,ML_GOOD|ML_ALWAYSACTIVE|ML_NOMIND,AI_Furnace,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,8,9,10,11,11,255},	// move
				{0,255},	// attack
				{0,255},		// die
			}},
		{"Yellow Rose",{1,5,0},
		 20,24,70,15,10,3,12,0*FIXAMT,"!20",0,MF_NOMOVE|MF_ONEFACE,0,AI_Rose,
			{
				{0,255},	// idle
				{7,8,9,10,11,12,13,14,254,255},	// move (bury yourself)
				{1,2,3,4,5,6,5,4,3,2,1,255},	// attack
				{7,8,9,10,11,12,13,14,255},		// die
				{14,13,12,11,10,9,8,7,255},	// A1=unbury if buried
				{254,254,254,255},	// A2=sit around buried
			}},
		{"Digger Bodzha",{4,2,0},
		 33,14,40,26,30,5,15,4*FIXAMT,"!50",0,MF_NONE,0,AI_Badger,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{5,6,7,8,9,255},	// attack
				{12,13,14,254,14,254,14,254,14,254,14,254,14,254,14,254,14,254,14,255},		// die
				{10,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,10,10,10,0,0,0,255},	// A1= spin slash
				{15,16,17,18,19,20,20,255},	// A2=burrow
				{21,22,23,24,25,25,255},	// A3=arise
				{254,254,255},	// A4=invisible, burrowing
			}},
		{"Thornpaw Lion",{1,5,0},
		 30,14,40,11,30,8,14,4*FIXAMT,"!32",0,MF_NONE,0,AI_Lion,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,8,0,255},	// move
				{9,10,9,255},	// attack
				{9,9,10,10,254,254,254,255},		// die
			}},
		{"Stonewall",{255,255,0},
		 33,20,60,18,50,14,20,2*FIXAMT,"graphics/stonewall.jsp",0,MF_NOMOVE,0,AI_Stonewall,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{5,6,7,8,9,9,9,9,8,7,6,5,255},	// attack (stomp ground)
				{13,14,15,16,17,254,17,254,17,254,17,254,17,254,17,254,17,254,17,255},		// die
				{10,11,12,12,12,11,10,255},	// A1=face splat
			}},
		{"Bokbok",{255,255,0},
		 1,6,10,14,10,1,0,4*FIXAMT,"graphics/bokbok.jsp",0,MF_INVINCIBLE|MF_NOHIT|MF_ENEMYWALK|MF_FREEWALK,ML_GOOD|ML_ALWAYSACTIVE,AI_Bokbok,
			{
				{0,255},	// idle
				{8,9,10,11,12,13,0,255},	// move
				{1,2,3,4,5,6,7,6,5,4,3,2,1,0,255},	// attack (peck ground)
				{0,255},		// die
			}},
		{"Nuclear Bokbok",{0,4,-3},
		 1,6,10,14,1,0,0,6*FIXAMT,"!58",0,MF_ENEMYWALK|MF_FREEWALK,ML_GOOD|ML_ALWAYSACTIVE,AI_Bokbok2,
			{
				{0,255},	// idle
				{8,9,10,11,12,13,0,255},	// move
				{1,2,3,4,5,6,7,6,5,4,3,2,1,0,255},	// attack (peck ground)
				{0,254,0,254,0,254,0,255},		// die
			}},
		{"Captain Capitan",{255,255,0},
		 45,11,40,19,200,8,18,6*FIXAMT,"graphics/bunnyboss.jsp",0,MF_NONE,ML_BOSS|ML_ALWAYSACTIVE|ML_WALLBUMP,AI_BunnyBoss2,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{11,12,12,12,12,12,12,12,12,12,12,12,12,12,11,11,11,0,0,255},	// attack (spin slash)
				{7,8,9,10,254,10,254,10,254,10,254,10,254,10,254,10,255},		// die
				{13,14,14,14,14,14,14,13,255},	// A1=block
				{15,16,17,18,255},	// A2=slash on blocked hit
			}},
		{"Titanic Titan",{255,255,0},
		 70,40,90,23,300,5,25,2*FIXAMT,"graphics/titanbig.jsp",0,MF_GLOW|MF_NOMOVE,ML_BOSS,AI_TitanBig,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{12,13,14,15,15,15,16,255},	// attack
				{17,18,19,20,21,22,254,22,254,22,254,22,254,22,254,22,254,22,255},		// die
				{5,6,7,8,9,9,9,9,9,9,9,10,11,255},	// A1= roar
			}},
		{"Stinkin' Bodzha",{4,1,0},
		 40,14,40,26,60,6,20,3*FIXAMT,"!50",0,MF_NONE,0,AI_Badger,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{5,6,7,8,9,255},	// attack
				{12,13,14,254,14,254,14,254,14,254,14,254,14,254,14,254,14,254,14,255},		// die
				{10,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,10,10,10,0,0,0,0,0,0,0,0,0,255},	// A1= spin slash
				{15,16,17,18,19,20,20,255},	// A2=burrow
				{21,22,23,24,25,25,255},	// A3=arise
				{254,254,255},	// A4=invisible, burrowing
			}},
		{"Arena Crystal",{7,1,0},
		 30,20,50,16,50,1,15,0*FIXAMT,"!28",0,MF_NOMOVE|MF_ONEFACE|MF_FLYING,ML_BOSS|ML_ALWAYSACTIVE|ML_NOMIND|ML_GOOD,AI_ArenaCrystal,
			{
				{0,1,2,3,4,5,6,7,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{8,9,10,11,12,13,14,15,255},		// die
			}},
		{"Turret",{255,255,0},
		 40,20,50,16,150,4,40,0*FIXAMT,"graphics/turret_evil.jsp",0,MF_NOMOVE|MF_ONEFACE,ML_ALWAYSACTIVE|ML_NOMIND,AI_Turret,
			{
				{0,0,0,0,1,2,3,4,5,6,7,8,8,8,8,9,10,11,12,13,14,15,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{0,254,0,254,255},		// die
			}},
		{"Prototype AQ40",{2,4,0},
		 3,11,50,28,PUPPETPAL_HEALTH,20,13,6*FIXAMT,"!7",0,MF_FREEWALK,ML_GOOD,AI_PuppetPal,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{7,8,9,10,11,12,255},	// attack (punch)
				{24,25,26,27,254,27,254,27,254,27,254,27,254,27,255},		// die
				{5,5,6,6,6,6,5,5,255},	// A1=spit
				{13,14,15,16,17,18,19,20,21,22,23,15,14,13,255},	// A2=hat trick
			}},
		{"Frost Gate",{255,255,0},
		 100,25,50,28,100,0,0,0*FIXAMT,"!7",0,MF_NONE,ML_NOMIND,AI_FrostGate,
			{
				{254,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{7,8,9,10,11,12,255},	// attack (punch)
				{254,254,255},		// die
			}},
		{"Many Mouse",{5,4,0},
		 40,8,20,19,50,5,12,3*FIXAMT,"!6",0,MF_NONE,ML_WOODS,AI_ManyMouse,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,0,255},	// move
				{8,9,9,8,255},	// attack
				{8,9,10,11,12,13,14,254,15,254,16,254,17,254,18,255},		// die
			}},
		{"Tinpot General",{4,7,0},
		 50,11,50,32,100,6,20,2*FIXAMT,"!11",0,MF_NONE,0,AI_TinGuy,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{5,6,7,8,9,10,11,12,255},	// attack (shoot)
				{23,24,25,26,27,28,29,30,31,254,31,254,31,254,31,254,31,255},		// die
				{13,14,13,255},	// A1=block
				{15,16,17,18,19,20,21,22,255},	// A2=shoot into sky
			}},
		{"Grizzly",{2,4,-2},
		 42,11,50,27,80,4,16,4*FIXAMT,"!2",0,MF_NONE,ML_NONE,AI_Teddy,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,255},	// attack
				{22,23,24,25,26,254,26,254,26,254,26,254,26,254,26,254,26,255},		// die
				{13,14,15,16,17,18,19,20,21,255},	// A1 = bellyflop
			}},
		{"Sock Gorilla",{4,6,-3},
		 44,11,40,28,140,5,10,1*FIXAMT,"!15",0,MF_NOMOVE,0,AI_SockMonkey,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{5,6,7,7,7,7,8,8,255},	// attack (charge up dash)
				{22,23,24,24,24,24,25,26,27,27,254,27,254,27,254,27,254,27,255},		// die
				{8,9,10,11,10,9,8,9,10,11,10,9,8,9,10,11,10,9,8,9,10,11,10,9,8,4,3,0,255},	// A1=do the dash
				{12,13,14,15,16,17,18,19,20,21,255},	// A2=shoot
			}},
		{"Mimic",{255,255,0},
		 15,11,30,13,40,2,7,2*FIXAMT,"graphics/mimic.jsp",0,MF_NOMOVE,ML_NOMIND,AI_Mimic,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{5,6,7,6,5,255},	// attack
				{8,9,10,11,12,254,12,254,12,254,12,254,12,254,12,254,12,254,12,255},		// die
				{5,6,7,6,5,255},	// A1=fake bite for interacting with it
			}},
		{"Army Of One",{4,1,-3},
		 50,11,50,32,200,8,40,6*FIXAMT,"!11",0,MF_NONE,0,AI_TinGuy,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{5,6,7,7,7,8,9,10,11,12,255},	// attack (shoot)
				{23,24,25,26,27,28,29,30,31,254,31,254,31,254,31,254,31,255},		// die
				{13,14,13,255},	// A1=block
				{15,16,17,18,19,20,21,22,255},	// A2=shoot into sky
			}},
		{"Von Frostburn",{255,255,0},
		 200,11,50,8,1000,15,35,6*FIXAMT,"graphics/baron.jsp",0,MF_FLYING|MF_NOMOVE|MF_ONEFACE,ML_BOSS|ML_WALLBUMP,AI_Baron,
			{
				{0,1,2,3,4,5,6,7,255},	// idle
				{0,1,2,3,4,5,6,7,255},	// move
				{7,6,5,4,3,2,1,0,7,6,5,4,3,2,1,0,255},	// attack (draw 3 stones from the ground, then unleash)
				{0,255},		// die
			}},
		{"Melton",{255,255,0},
		 200,100,200,22,1000,8,40,0*FIXAMT,"graphics/titanmax.jsp",0,MF_NOMOVE|MF_ONEFACE,ML_BOSS|ML_ALWAYSACTIVE,AI_UberTitan,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,4,4,4,3,2,1,255},	// attack (roar)
				{14,15,16,17,18,19,20,21,255},		// die
				{5,6,7,8,8,8,8,9,10,11,12,12,12,12,13,255},	// A1=pound fists (3 and 10 are the actual hits)
				{21,20,19,18,17,16,15,14,255},	// A2=arise
			}},
		{"Golden Bear",{2,5,0},
		 50,11,50,27,150,12,14,2*FIXAMT,"!2",0,MF_NONE,ML_NONE,AI_Teddy,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,255},	// attack
				{22,23,24,25,26,254,26,254,26,254,26,254,26,254,26,254,26,255},		// die
				{13,14,15,16,17,18,19,20,21,255},	// A1 = bellyflop
			}},
		{"Power Plant",{4,6,3},
		 50,24,70,15,100,5,20,0*FIXAMT,"!20",0,MF_NOMOVE|MF_ONEFACE,0,AI_Rose,
			{
				{0,255},	// idle
				{7,8,9,10,11,12,13,14,254,255},	// move (bury yourself)
				{1,2,3,4,5,6,5,4,3,2,1,255},	// attack
				{7,8,9,10,11,12,13,14,255},		// die
				{14,13,12,11,10,9,8,7,255},	// A1=unbury if buried
				{254,254,254,255},	// A2=sit around buried
			}},
		{"Doom Daisy",{4,5,-7},
		 80,24,70,15,500,5,50,0*FIXAMT,"!20",0,MF_NOMOVE|MF_ONEFACE,ML_BOSS,AI_Rose,
			{
				{0,255},	// idle
				{7,8,9,10,11,12,13,14,254,255},	// move (bury yourself)
				{1,2,3,4,5,6,5,4,3,2,1,255},	// attack
				{7,8,9,10,11,12,13,14,255},		// die
				{14,13,12,11,10,9,8,7,255},	// A1=unbury if buried
				{254,254,254,255},	// A2=sit around buried
			}},
		{"Mountain Gorka",{255,255,0},
		 50,12,60,16,70,4,30,3*FIXAMT,"graphics/bigbadger.jsp",0,0,ML_ALWAYSACTIVE,AI_BigBadger,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{5,6,7,8,9,255},	// attack
				{10,11,12,254,12,254,12,254,12,254,12,254,12,254,12,254,12,254,12,255},		// die
				{13,14,15,14,13,255},	// A1=toss boulder
			}},
		{"Happy Stick Bird",{255,255,0},
		 200,10,30,4,1000,10,50,7*FIXAMT,"graphics/krobb.jsp",0,MF_FLYING|MF_ONEFACE,ML_BOSS|ML_ALWAYSACTIVE,AI_Bird,
			{
				{0,1,2,1,255},	// idle
				{0,1,2,1,255},	// move
				{0,1,2,1,255},	// attack
				{3,3,3,3,3,3,254,3,254,3,254,3,254,3,254,3,254,3,254,3,255},		// die
			}},
		{"Killbor",{255,255,0},
		 80,20,100,14,1000,14,40,7*FIXAMT,"graphics/champ.jsp",0,MF_NOMOVE,ML_BOSS,AI_Killbor,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{5,6,7,8,9,255},	// attack (#3=hit)
				{10,11,12,13,254,13,254,13,254,13,254,13,254,13,255},		// die
				{5,6,6,6,6,5,255},	// A1=summon or something, hold sword aloft
			}},
		{"Mutant Gorka",{3,6,0},
		 50,12,60,16,60,4,30,3*FIXAMT,"!78",0,0,ML_ALWAYSACTIVE,AI_BigBadger,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{5,6,7,8,9,255},	// attack
				{10,11,12,254,12,254,12,254,12,254,12,254,12,254,12,254,12,254,12,255},		// die
				{13,14,15,14,13,255},	// A1=toss boulder
			}},
		{"Toy Bodzha",{255,255,0},
		 1,6,15,10,10,0,1,8*FIXAMT,"graphics/lilbadger.jsp",0,MF_NOHIT|MF_INVINCIBLE|MF_NOMOVE|MF_FREEWALK|MF_ENEMYWALK,ML_GOOD|ML_ALWAYSACTIVE,AI_ToyBadger,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{5,6,7,8,9,255},	// attack
				{0,255},		// die
			}},
		{"Playable Thug",{255,255,0},
		 0,11,50,19,128,0,0,0,"!35",0,MF_WATERWALK,ML_WALLBUMP|ML_GOOD,NULL,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,0,255},	// move
				{10,11,12,13,14,14,14,15,255},	// attack
				{16,17,18,18,254,18,254,18,254,18,255},		// die
				{7,8,9,8,7,255},	// A1=use item
				{1,2,3,2,1,0,0,0,0,0,1,2,3,2,1,0,255},	// A2=bored
				{0,0,0,0,0,0,0,0,0,0,0,0,0,255}, // A3=jump
				{10,11,12,13,14,14,14,14,14,14,14,14,15,15,255},	// A4=air attack
			}},
		{"Playable Knight",{255,255,0},
		 0,11,50,19,128,0,0,0,"!41",0,MF_WATERWALK,ML_WALLBUMP|ML_GOOD,NULL,
			{
				{0,255},	// idle
				{2,3,4,3,2,1,5,6,7,6,5,1,1,255},	// move
				{8,9,10,11,12,12,13,14,255},	// attack
				{15,16,17,18,254,18,254,18,254,18,255},		// die
				{0,1,1,1,0,255},	// A1=use item
				{0,0,0,0,1,2,3,4,3,2,1,1,0,0,255}, // A2=bored
				{1,1,1,1,1,1,1,1,1,1,1,1,1,255}, // A3=jump
				{8,9,10,11,12,12,12,12,12,12,13,13,14,14,255},	// A4=air attack
			}},
		// the ghosts for haunted mode
		{"Ghost",{255,255,0},
		 5,8,20,8,10,1,3,4*FIXAMT,"!36",0,MF_GLOW|MF_FREEWALK|MF_ENEMYWALK|MF_WALLWALK|MF_FLYING,ML_ALWAYSACTIVE,AI_GhostEvil,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,2,1,255},	// attack
				{4,5,6,7,255},		// die
				{7,6,5,4,255},	//A1=fade in
			}},
		{"Ghost",{255,255,0},
		 15,8,20,8,20,2,5,5*FIXAMT,"!36",0,MF_GLOW|MF_FREEWALK|MF_ENEMYWALK|MF_WALLWALK|MF_FLYING,ML_ALWAYSACTIVE,AI_GhostEvil,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,2,1,255},	// attack
				{4,5,6,7,255},		// die
				{7,6,5,4,255},	//A1=fade in
			}},
		{"Ghost",{255,255,0},
		 25,8,20,8,30,3,10,6*FIXAMT,"!36",0,MF_GLOW|MF_FREEWALK|MF_ENEMYWALK|MF_WALLWALK|MF_FLYING,ML_ALWAYSACTIVE,AI_GhostEvil,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,2,1,255},	// attack
				{4,5,6,7,255},		// die
				{7,6,5,4,255},	//A1=fade in
			}},
		{"Ghost",{255,255,0},
		 35,8,20,8,40,4,20,7*FIXAMT,"!36",0,MF_GLOW|MF_FREEWALK|MF_ENEMYWALK|MF_WALLWALK|MF_FLYING,ML_ALWAYSACTIVE,AI_GhostEvil,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,2,1,255},	// attack
				{4,5,6,7,255},		// die
				{7,6,5,4,255},	//A1=fade in
			}},
		{"Ghost",{255,255,0},
		 45,8,20,8,50,5,25,8*FIXAMT,"!36",0,MF_GLOW|MF_FREEWALK|MF_ENEMYWALK|MF_WALLWALK|MF_FLYING,ML_ALWAYSACTIVE,AI_GhostEvil,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,2,1,255},	// attack
				{4,5,6,7,255},		// die
				{7,6,5,4,255},	//A1=fade in
			}},
		{"Sock Crab",{2,4,0},
		 20,11,15,20,15,8,10,5*FIXAMT,"!49",0,MF_NONE,0,AI_Crab,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,0,255},	// move
				{8,9,10,11,12,13,255},	// attack
				{14,15,16,17,254,18,254,19,254,18,254,17,254,18,254,19,254,18,254,17,254,18,255},		// die
			}},
		{"Hot Dog",{7,4,-8},
		 15,20,50,13,20,2,20,6*FIXAMT,"!21",0,0,0,AI_HotDog,
			{
				{0,255},	// idle
				{1,2,3,4,5,0,255},	// move
				{6,7,8,8,8,8,7,6,255},	// attack
				{9,10,11,12,254,12,254,12,254,12,254,12,254,12,254,12,255},		// die
				{1,2,3,4,5,0,1,2,3,4,5,0,1,2,3,4,5,0,1,2,3,4,5,0,255},	// A1=charge
				{6,7,8,8,8,255},	// A2=get ready to charge
			}},
		{"Icebox",{5,7,0},
		 40,11,30,13,80,5,20,3*FIXAMT,"!71",0,0,ML_NOMIND,AI_Icebox,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{5,6,7,6,5,255},	// attack
				{8,9,10,11,12,254,12,254,12,254,12,254,12,254,12,254,12,254,12,255},		// die
				{5,6,7,6,5,255},	// A1=fake bite for interacting with it
			}},
		{"Deathberry Bush",{1,0,-4},
		 50,11,30,8,200,2,1,0*FIXAMT,"!30",0,MF_NOMOVE|MF_ONEFACE,ML_ALWAYSACTIVE,AI_Deathberry,
			{
				{5,255},	// idle
				{0,1,2,3,4,255},	// move (dig up)
				{6,7,6,7,255},	// attack (shoot berry)
				{4,3,2,1,0,255},		// die
			}},
		{"Fruit Punch",{2,6,0},
		 20,11,50,28,30,3,25,4*FIXAMT,"!7",0,MF_NONE,ML_WOODS,AI_FruitPunch,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{7,8,9,10,11,11,11,11,12,255},	// attack (punch)
				{24,25,26,27,254,27,254,27,254,27,254,27,254,27,255},		// die
				{5,5,6,6,6,6,5,5,255},	// A1=spit
				{13,14,15,16,17,18,19,20,21,22,23,15,14,13,255},	// A2=hat trick
				{27,26,25,24,255},		// A3=get born
			}},
		{"The Red Baron",{7,4,-2},
		 100,11,50,8,200,10,35,5*FIXAMT,"!73",0,MF_FLYING|MF_NOMOVE|MF_ONEFACE,ML_BOSS|ML_WALLBUMP,AI_RedBaron,
			{
				{0,1,2,3,4,5,6,7,255},	// idle
				{0,1,2,3,4,5,6,7,255},	// move
				{7,6,5,4,3,2,1,0,7,6,5,4,3,2,1,0,255},	// attack (draw 3 stones from the ground, then unleash)
				{0,254,0,254,0,254,0,254,0,254,0,254,0,254,0,255},		// die
			}},
		{"Artifact Jack",{5,4,0},
		 50,11,50,19,150,10,50,2*FIXAMT,"!38",0,MF_NONE,0,AI_Jack,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,15,15,15,15,15,16,17,18,255},	// attack (shoot)
				{7,8,9,10,11,12,13,14,15,254,15,254,15,254,15,254,15,254,15,255},		// die
			}},

};

void InitMonsters(void)
{
	int i;

	for(i=0;i<NUM_MONSTERS;i++)
	{
		monsType[i].spr=NULL;
	}
}

void ExitMonsters(void)
{
	int i;

	for(i=1;i<NUM_MONSTERS;i++)
	{
		if(monsType[i].spr)
			delete monsType[i].spr;
		monsType[i].spr=NULL;
	}
}

void PurgeMonsterSprites(void)
{
	int i;

	for(i=1;i<NUM_MONSTERS;i++)
	{
		if(monsType[i].sprName[0]=='!')
			monsType[i].spr=NULL;	// it is sharing with someone else

		if(monsType[i].spr)
			delete monsType[i].spr;
		monsType[i].spr=NULL;
	}
}

byte MonsterSize(byte type)
{
	return monsType[type].size;
}

byte *MonsterAnim(byte type,byte anim)
{
	return monsType[type].anim[anim];
}

word MonsterFlags(byte type)
{
	return monsType[type].flags;
}

byte MonsterFrames(byte type)
{
	return monsType[type].framesPerDir;
}

byte MonsterDamage(byte type)
{
	return monsType[type].damage;
}

word MonsterHP(byte type,byte team)
{
	dword d;

	if(type==MONS_YOURBOT)
	{
		d=(player.var[VAR_CLOCKBOT-IT_LOG+IT_LOG]*5+20)*(100+player.var[VAR_MADGEM+MC_LIFE]*10)/100;
	}
	if(player.var[VAR_MADCAP])
	{
		if(team==EVIL)
		{
			d=monsType[type].hp*MADCAP_HPBOOST;
			if(ModifierOn(MOD_TICKING))
				d*=2;
		}
		else
			d=(monsType[type].hp*(100+player.var[VAR_MADGEM+MC_LIFE]*10))/100;
	}
	else
	{
		d=monsType[type].hp;
		if(team==EVIL && ModifierOn(MOD_TICKING))
			d*=2;
	}

	if(d>65535)
		d=65535;

	return (word)d;
}

int MonsterLevel(byte type)
{
	if(player.var[VAR_MADCAP] && type!=MONS_CARAVAN)
		return monsType[type].level+MADCAP_LVLBOOST;
	else
		return monsType[type].level;
}

char *MonsterName(byte type)
{
	return monsType[type].name;
}

void SetMonsterFlags(byte type,word flags)
{
	monsType[type].flags=flags;
}

void LoadMonsterSprite(byte type)
{
	dword start,end;
	int t;

	start=timeGetTime();
	// load if not loaded
	if(monsType[type].spr==NULL)
	{
		if(monsType[type].sprName[0]=='!')
		{
			// steal another monster's sprites
			t=atoi(&monsType[type].sprName[1]);
			if(monsType[t].spr==NULL)
			{
				monsType[t].spr=new sprite_set_t(monsType[t].sprName);
				if(monsType[t].spr->GetCount()==0)
					FatalError("Out of memory!");
			}
			monsType[type].spr=monsType[t].spr;
		}
		else
		{
			monsType[type].spr=new sprite_set_t(monsType[type].sprName);
			if(monsType[type].spr->GetCount()==0)
				FatalError("Out of memory!");
		}
	}
	end=timeGetTime();
	AddGarbageTime(end-start);
}

sprite_t *GetMonsterSprite(byte type,byte seq,byte frm,byte facing)
{
	int v;

	LoadMonsterSprite(type);

	v=monsType[type].anim[seq][frm];

	if(v==254)
		return NULL;	// 254 means no sprite for this frame

	if(!(monsType[type].flags&MF_ONEFACE))
		v+=facing*monsType[type].framesPerDir;

	if(monsType[type].flags&MF_FACECMD)
		v+=facing;

	return monsType[type].spr->GetSprite(v);
}

void MonsterDrawShadow(int x,int y,int z,byte type,byte seq,byte frm,byte facing)
{
	sprite_t *curSpr;
	int v;
	byte isBouapha;

	if(type==0)
		return;

	if(type==player.monsType)
	{
		isBouapha=1;
	}
	else
		isBouapha=0;

	LoadMonsterSprite(type);

	v=monsType[type].anim[seq][frm];

	if(v==254)
		return;	// don't draw this frame

	if(!(monsType[type].flags&MF_ONEFACE))
		v+=facing*monsType[type].framesPerDir;

	if(monsType[type].flags&MF_FACECMD)
		v+=facing;

	curSpr=monsType[type].spr->GetSprite(v);
	if(!curSpr)
		return;

	if(!(monsType[type].flags&MF_NOSHADOW))
		SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,0,255,0,curSpr,DISPLAY_DRAWME|DISPLAY_SHADOW);
}

void MonsterDraw(int x,int y,int z,byte type,byte seq,byte frm,byte facing,char bright,byte ouch,int ice,byte poison)
{
	sprite_t *curSpr;
	int v;
	byte isBouapha;

	if(type==0)
		return;

	if(type==player.monsType)
	{
		isBouapha=1;
	}
	else
		isBouapha=0;

	LoadMonsterSprite(type);

	v=monsType[type].anim[seq][frm];

	if(v==254)
		return;	// don't draw this frame

	if(!(monsType[type].flags&MF_ONEFACE))
		v+=facing*monsType[type].framesPerDir;

	if(isBouapha)
	{
		if(player.invinc&1)
		{
			curSpr=monsType[type].spr->GetSprite(v);
			if(!curSpr)
				return;
			SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,255,bright,curSpr,DISPLAY_DRAWME|DISPLAY_GLOW);
			return;
		}
		if(poison>0)
		{
			curSpr=monsType[type].spr->GetSprite(v);
			if(!curSpr)
				return;
			if(!(monsType[type].flags&MF_NOSHADOW))
				SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,0,255,0,curSpr,DISPLAY_DRAWME|DISPLAY_SHADOW);
			if(ouch==0)
				SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,1,bright-4,curSpr,DISPLAY_DRAWME);	// green
			else
				SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,5,bright,curSpr,DISPLAY_DRAWME); // yellow
			return;
		}
		if(player.stone)
		{
			curSpr=monsType[type].spr->GetSprite(v);
			if(!curSpr)
				return;
			if(!(monsType[type].flags&MF_NOSHADOW))
				SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,0,255,0,curSpr,DISPLAY_DRAWME|DISPLAY_SHADOW);
			if(ouch==0)
				SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,0,bright-4,curSpr,DISPLAY_DRAWME);	// grey
			else
				SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,2,bright,curSpr,DISPLAY_DRAWME); // brown
			return;
		}
	}

	if(monsType[type].flags&MF_FACECMD)
		v+=facing;

	curSpr=monsType[type].spr->GetSprite(v);
	if(!curSpr)
		return;

	if(!(monsType[type].flags&MF_NOSHADOW))
		SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,0,255,0,curSpr,DISPLAY_DRAWME|DISPLAY_SHADOW);

	if(ouch==0)
	{
		if(ice)
		{
			SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,7,bright,curSpr,DISPLAY_DRAWME);
		}
		else
		{
			if(!(monsType[type].flags&(MF_GLOW)))
			{
				if(poison>0)
					SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,1,bright-4,curSpr,DISPLAY_DRAWME);	// green
				else if(monsType[type].offc[0]==255)
				{
					if(type==player.monsType && player.parka.magic==EM_PURPLEKOO)
						SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,6,bright-8,curSpr,DISPLAY_DRAWME);	// purple parka!
					else
						SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,255,bright+monsType[type].offc[2],
							curSpr,DISPLAY_DRAWME);
				}
				else
					OffSprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,monsType[type].offc[0],monsType[type].offc[1],
							bright+monsType[type].offc[2],curSpr,DISPLAY_DRAWME|DISPLAY_OFFCOLOR);
			}
			else if(monsType[type].flags&MF_GLOW)
				SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,255,bright+monsType[type].offc[2],
						curSpr,DISPLAY_DRAWME|DISPLAY_GLOW);
		}
	}
	else
	{
		if(poison>0)
			SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,5,bright,curSpr,DISPLAY_DRAWME);	// yellow
		else if(ice)
			SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,7,bright+5,curSpr,DISPLAY_DRAWME);	// brighter aqua
		else
			SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,4,bright,curSpr,DISPLAY_DRAWME);	// red
	}
}

void InstaRenderMonster(int x,int y,byte type,char bright,MGLDraw *mgl)
{
	sprite_t *curSpr;
	int v;

	LoadMonsterSprite(type);

	v=monsType[type].anim[ANIM_IDLE][0];
	if(!(monsType[type].flags&MF_ONEFACE))
		v+=2*monsType[type].framesPerDir;

	curSpr=monsType[type].spr->GetSprite(v);
	if(!curSpr)
		return;

	curSpr->DrawBright(x,y,mgl,bright);
}

void UpdateMonsterTypes(void)
{

/*	nifty glowing monster effect if needed
	// same for mutant pumpkins
	monsType[MONS_PUMPKIN4].offc[2]++;
	if(monsType[MONS_PUMPKIN4].offc[2]>7)
		monsType[MONS_PUMPKIN4].offc[2]=-4;
		*/
}

// AI auxiliary functions to make it simple
//---------------------------------------------

void FaceGoodguy(Guy *me,Guy *goodguy)
{
	if(goodguy->x<me->x-FIXAMT*16)
	{
		if(goodguy->y<me->y-FIXAMT*16)
			me->facing=5;
		else if(goodguy->y>me->y+FIXAMT*16)
			me->facing=3;
		else
			me->facing=4;
	}
	else if(goodguy->x>me->x+FIXAMT*16)
	{
		if(goodguy->y<me->y-FIXAMT*16)
			me->facing=7;
		else if(goodguy->y>me->y+FIXAMT*16)
			me->facing=1;
		else
			me->facing=0;
	}
	else
	{
		if(goodguy->y<me->y-FIXAMT*16)
			me->facing=6;
		else if(goodguy->y>me->y+FIXAMT*16)
			me->facing=2;
	}
}

void FaceGoodguyVillager(Guy *me,Guy *goodguy)
{
	if(goodguy->x<me->x-FIXAMT*8)
	{
		if(goodguy->y<me->y-FIXAMT*8)
			me->facing=5;
		else if(goodguy->y>me->y+FIXAMT*8)
			me->facing=3;
		else
			me->facing=4;
	}
	else if(goodguy->x>me->x+FIXAMT*8)
	{
		if(goodguy->y<me->y-FIXAMT*8)
			me->facing=7;
		else if(goodguy->y>me->y+FIXAMT*8)
			me->facing=1;
		else
			me->facing=0;
	}
	else
	{
		if(goodguy->y<me->y-FIXAMT*8)
			me->facing=6;
		else if(goodguy->y>me->y+FIXAMT*8)
			me->facing=2;
	}
}

int RangeToTarget(Guy *me,Guy *goodguy)
{
	return abs(me->x-goodguy->x)+abs(me->y-goodguy->y);
}

inline int RangeToCoords(Guy *me,int x,int y)
{
	return abs(me->x-x)+abs(me->y-y);
}

// this version doesn't insta-face, it rotates toward the right facing, and it has much
// more leeway than the 16 pixels of the other (it's for bigger creatures)
inline void FaceGoodguy2(Guy *me,Guy *goodguy)
{
	int desired;
	int diff,dir;

	if(goodguy->x<me->x-FIXAMT*32)
	{
		if(goodguy->y<me->y-FIXAMT*32)
			desired=5;
		else if(goodguy->y>me->y+FIXAMT*32)
			desired=3;
		else
			desired=4;
	}
	else if(goodguy->x>me->x+FIXAMT*32)
	{
		if(goodguy->y<me->y-FIXAMT*32)
			desired=7;
		else if(goodguy->y>me->y+FIXAMT*32)
			desired=1;
		else
			desired=0;
	}
	else
	{
		if(goodguy->y<me->y-FIXAMT*32)
			desired=6;
		else
			desired=2;
	}

	if(desired==me->facing)
		return;

	if(desired>me->facing)
	{
		diff=desired-me->facing;
		if(diff>4)
		{
			dir=-1;
			diff=8-diff;
		}
		else
			dir=1;
	}
	else
	{
		diff=me->facing-desired;
		if(diff>4)
		{
			dir=1;
			diff=8-diff;
		}
		else
			dir=-1;
	}
	me->facing=(me->facing+dir)&7;
}

// for evil pumps... extra leeway and range
inline void FaceGoodguy3(Guy *me,Guy *goodguy)
{
	int xamt,yamt;

	xamt=10*FIXAMT+abs(me->x-goodguy->x)/10;
	yamt=10*FIXAMT+abs(me->y-goodguy->y)/10;

	if(goodguy->x<me->x-yamt)
	{
		if(goodguy->y<me->y-xamt)
			me->facing=5;
		else if(goodguy->y>me->y+xamt)
			me->facing=3;
		else
			me->facing=4;
	}
	else if(goodguy->x>me->x+yamt)
	{
		if(goodguy->y<me->y-xamt)
			me->facing=7;
		else if(goodguy->y>me->y+xamt)
			me->facing=1;
		else
			me->facing=0;
	}
	else
	{
		if(goodguy->y<me->y-xamt)
			me->facing=6;
		else if(goodguy->y>me->y+xamt)
			me->facing=2;
	}
}

void BasicAI(Guy *me,byte ouchSound,byte dieSound,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(ouchSound,me->x,me->y,SND_CUTOFF|SND_RANDOM,1200);
		else
			MakeSound(dieSound,me->x,me->y,SND_CUTOFF|SND_RANDOM,1200);
	}
}

void WanderAI(Guy *me,int wanderRate,int wanderTime,int unWanderRate,Map *map,world_t *world,Guy *goodguy)
{
	if(me->action==ACTION_BUSY)
		return;

	if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		FaceGoodguy(me,goodguy);

		me->dx=(Cosine(me->facing*32)*monsType[me->type].speed)/FIXAMT;
		me->dy=(Sine(me->facing*32)*monsType[me->type].speed)/FIXAMT;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
		if(Random(wanderRate)==0)
		{
			me->mind=1;		// occasionally wander
			me->mind1=1;
		}
	}
	else if(me->mind==1)	// random wandering
	{
		if(!(me->mind1--))	// time to get a new direction
		{
			if((goodguy) && Random(unWanderRate)==0)
				me->mind=0;	// get back on track
			else
				me->facing=(byte)Random(8);
			me->mind1=(byte)Random(wanderTime)+1;
		}

		me->dx=(Cosine(me->facing*32)*monsType[me->type].speed)/FIXAMT;
		me->dy=(Sine(me->facing*32)*monsType[me->type].speed)/FIXAMT;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
}

void DoMove(Guy *me,int move,int frmAdv,byte busy,int dx,int dy)
{
	me->seq=move;
	me->frm=0;
	me->frmTimer=0;
	me->frmAdvance=frmAdv;
	if(busy)
		me->action=ACTION_BUSY;
	else
		me->action=ACTION_IDLE;
	me->dx=dx;
	me->dy=dy;
}

byte AlwaysActiveMons(byte type)
{
	if(monsType[type].location&ML_ALWAYSACTIVE)
		return 1;
	else
		return 0;
}

byte WallBumpMons(byte type)
{
	if(monsType[type].location&ML_WALLBUMP)
		return 1;
	else
		return 0;
}

byte RandomSummonMons(byte type)
{
	if(monsType[type].location&(ML_RANDOMSUMMON))
		return 1;
	else
		return 0;
}

byte IsBoss(byte type)
{
	if(monsType[type].location&ML_BOSS)
		return 1;
	else
		return 0;
}

#include "monster_ai.inl"

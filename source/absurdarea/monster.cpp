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
		 15,11,50,28,30,1,5,2*FIXAMT,"!7",0,MF_NONE,ML_NONE,AI_Puppet,	// changed to non boss
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
		 15,8,20,19,100,10,9,8*FIXAMT,"!6",0,MF_NONE,ML_BOSS|ML_WALLBUMP,AI_Ratatouille,	// changed to non boss // reverted
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
		 20,24,70,15,30,8,12,0*FIXAMT,"graphics/rose.jsp",0,MF_NOMOVE|MF_ONEFACE,ML_NONE,AI_Rose,	// changed to non boss
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
		 15,11,40,28,30,2,10,2*FIXAMT,"!15",0,MF_NONE,ML_NONE,AI_SockMonkey,	// changed to non boss
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
		 25,14,40,11,90,4,15,4*FIXAMT,"!32",0,MF_NONE,ML_NONE,AI_Lion,		// changed to non boss
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
		 30,11,50,32,60,6,20,2*FIXAMT,"!11",0,MF_NONE,ML_NONE,AI_TinGuy,	// changed to non boss
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
		{"Gourdzilla",{6,4,0},
		 40,80,60,13,300,12,18,0*FIXAMT,"graphics/bigplant.jsp",0,MF_NOMOVE|MF_ONEFACE,ML_BOSS|ML_ALWAYSACTIVE,AI_BigPlant,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,7,8,255},	// attack
				{9,10,11,12,254,12,254,12,254,12,254,12,254,12,254,12,255},		// die
			}},
		{"Furnace",{255,255,0},
		 30,60,60,12,90,4,18,3*FIXAMT,"graphics/furnace.jsp",0,MF_NOMOVE|MF_INVINCIBLE|MF_ONEFACE|MF_NOHIT,ML_GOOD|ML_ALWAYSACTIVE|ML_NOMIND,AI_Furnace,	// AA added nohit
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
		 50,11,50,32,200,8,40,6*FIXAMT,"!11",0,MF_NONE,ML_BOSS,AI_TinGuy,	// changed to boss
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
		 45,8,20,8,50,5,25,8*FIXAMT,"!36",0,MF_GLOW|MF_FREEWALK|MF_ENEMYWALK|MF_WALLWALK|MF_FLYING,ML_ALWAYSACTIVE,AI_GhostEvil, // AA is boss // NEVERMIND reverted
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
		 50,11,50,19,150,10,50,2*FIXAMT,"!38",0,MF_NONE,ML_BOSS,AI_Jack, // changed to boss
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,15,15,15,15,15,16,17,18,255},	// attack (shoot)
				{7,8,9,10,11,12,13,14,15,254,15,254,15,254,15,254,15,254,15,255},		// die
			}},
		{"Firewall",{2,4,0},
		 45,20,60,18,50,20,25,2*FIXAMT,"!57",0,MF_NOMOVE,0,AI_Stonewall,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{5,6,7,8,9,9,9,9,8,7,6,5,255},	// attack (burninate)
				{13,14,15,16,17,254,17,254,17,254,17,254,17,254,17,254,17,254,17,255},		// die
				{10,11,12,12,12,11,10,255},	// A1=face splat
			}},
		{"Nice Titan",{7,3,8},
		 40,20,60,23,90,6,20,3*FIXAMT,"!51",0,MF_NONE,0,AI_Titan,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{12,13,14,15,15,15,16,255},	// attack
				{17,18,19,20,21,22,254,22,254,22,254,22,254,22,254,22,254,22,255},		// die
				{5,6,7,8,9,9,9,10,11,255},	// A1= roar
			}},
		{"Spice Mice",{5,4,8},
		 45,8,20,19,80,4,1,3*FIXAMT,"!6",0,MF_NONE,ML_WOODS,AI_Mouse,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,0,255},	// move
				{0,255},	// attack
				{8,9,10,11,12,13,14,254,15,254,16,254,17,254,18,255},		// die
				{8,9,9,9,9,9,8,255},	// A1=rat-a-tat
			}},
		{"One Punch",{1,5,8},
		 60,11,50,28,120,10,100,3*FIXAMT,"!7",0,MF_NONE,ML_WOODS,AI_Puppet,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{7,8,9,10,11,12,255},	// attack (punch)
				{24,25,26,27,254,27,254,27,254,27,254,27,254,27,255},		// die
				{5,5,6,6,6,6,5,5,255},	// A1=spit
				{13,14,15,16,17,18,19,20,21,22,23,15,14,13,255},	// A2=hat trick
			}},
		{"Rock Gorilla",{4,2,-6},
		 48,11,40,28,80,15,30,1*FIXAMT,"!15",0,MF_NOMOVE,0,AI_SockMonkey,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{5,6,7,7,7,7,8,8,255},	// attack (charge up dash)
				{22,23,24,24,24,24,25,26,27,27,254,27,254,27,254,27,254,27,255},		// die
				{8,9,10,11,10,9,8,9,10,11,10,9,8,9,10,11,10,9,8,9,10,11,10,9,8,4,3,0,255},	// A1=do the dash
				{12,13,14,15,16,17,18,19,20,21,255},	// A2=shoot
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
	PurgeMonsterSprites();
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
	else
	{	// AA bot fix
		d=monsType[type].hp;
	}
	if(player.var[VAR_MADCAP])
	{
		if(team==EVIL)
		{
			d=d*MADCAP_HPBOOST;
			if(ModifierOn(MOD_TICKING))
				d*=2;
		}
		else
			d=(d*(100+player.var[VAR_MADGEM+MC_LIFE]*10))/100;
	}
	else
	{
		//d=monsType[type].hp;
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
	//int offs = FIXAMT * 16;
	//int dx = (goodguy->x - me->x + offs / 2) / offs;
	//int dy = (goodguy->y - me->y + offs / 2) / offs;
	//if (!dx)
	//{
	//	if (dy > 0)
	//		me->facing = 2;
	//	else if (dy < 0)
	//		me->facing = 6;
	//	return;
	//}
	//else if (!dy)
	//{
	//	if (dx > 0)
	//		me->facing = 0;
	//	else
	//		me->facing = 4;
	//	return;
	//}
	//int ax = abs(dx);
	//int ay = abs(dy);
	//if (2 * ax > 5 * ay)
	//{
	//	if (dx > 0)
	//		me->facing = 0;
	//	else
	//		me->facing = 4;
	//}
	//else if (2 * ay > 5 * ax)
	//{
	//	if (dy > 0)
	//		me->facing = 2;
	//	else
	//		me->facing = 6;
	//}
	//else
	//{
	//	if (dx < 0)
	//	{
	//		if (dy < 0)
	//			me->facing = 5;
	//		else
	//			me->facing = 3;
	//	}
	//	else
	//	{
	//		if (dy < 0)
	//			me->facing = 7;
	//		else
	//			me->facing = 1;
	//	}
	//}

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

// here be the AIs for each monster type
//--------------------------------------------------------------------------------------

void AI_Villager(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	// which frame begins this villager
	byte firstFrame[] = {
		0,	// #0
		0,	// #1
		0,	// #2 Kinyova
		7,	// #3 Bobby Joe
		15,	// #4 Healiana
		19,	// #5 Woody
		27,	// #6 Magicsmith
		35,	// #7 Geoffrey
		41,	// #8 BM Bob
		53,	// #9 chained melody
		62,	// #10 melody
		70,	// #11 mickey
		76,	// #12 guru
		83,	// #13 tina
		84,	// #14 tailor
		93,	// #15 balloon
		104,	// #16 handyman
		110,	// #17 historian
		116,	// #18 magic guy
		121,	// #19 buried ropeguy
		126,	// #20 ropeguy
		134,	// #21 ropewife
		138,	// #22 CEO
		144,	// #23 curaga
		148,	// #24 onion
		0,		// #25 junksmith
		0,		// #26 frankie beans
		0,		// #27 bonk
		0,		// #28 jimmy 2-fingers
		0,		// #29 smash
		0,		// #30 captain capitan
		0,		// #31 major fishbug
		154,	// #32 clockmaker
		0,		// #33 lt. hornswaggle
		0,		// #34 pvt. public
		155,	// #35 betty
		159,	// #36 schrodinger
		163,	// #37 sharona
		171,	// #38 shemp old
		175,	// #39 shemp young
		182,	// #40 shroud
	};
	word idleOdds[] = {
		1,	// #0
		1,	// #1
		100, // #2 Kinyova
		20,	// #3 Bobby Joe
		200,	// #4 Healiana
		1000, // #5 Woody
		100, // #6 Magicsmith
		80,	// #7 Geoffrey
		80,	// #8 BM Bob
		40,	// #9 chained melody
		100, // #10 melody
		10,	// #11 mickey
		90,	// #12 guru
		10,	// #13 tina
		200, // #14 tailor
		1,	// #15 balloon
		60,	// #16 handyman
		50,	// #17 historian
		150,	// #18 magic guy
		30,	// #19 buried ropeguy
		100,	// #20 ropeguy
		100,	// #21 ropewife
		200,	// #22 CEO
		200,	// #23 curaga
		200,	// #24 onion
		1,		// #25 junksmith
		1,		// #26 frankie beans
		1,		// #27 bonk
		1,		// #28 jimmy 2-fingers
		1,		// #29 smash
		1,		// #30 captain capitan
		1,		// #31 major fishbug
		1,		// #32 clockmaker
		1,		// #33 lt. hornswaggle
		1,		// #34 pvt. public
		150,	// #35 betty
		100,	// #36 schrodinger
		100,	// #37 sharona
		200,	// #38 shemp old
		40,		// #39 shemp young
		150,	// #40 shroud
	};
	// the idle animation for each villager
	byte idle_anim[][64] = {
		{255},	// vgr #0
		{255},	// generic villager #1
		{1,2,3,3,3,4,4,4,5,5,5,6,6,6,5,4,3,3,3,4,4,4,5,5,5,6,6,6,5,4,3,3,3,4,4,4,5,5,5,6,6,6,5,4,3,2,1,255},	// #2 Kinyova
		{8,9,10,10,10,11,11,12,12,13,13,13,13,14,255},	// #3 bobby joe
		{16,16,16,16,16,17,17,17,17,17,17,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,17,17,17,17,17,17,16,16,16,16,16,255},	// #4 healiana
		{20,21,21,21,21,21,22,23,23,23,24,25,25,25,25,25,26,255},	// #5 Woody
		{28,28,29,29,30,30,31,31,32,32,33,34,34,33,32,32,32,33,34,34,34,33,32,31,31,31,30,30,30,29,29,29,28,28,255},	// #6 Magicsmith
		{36,36,36,37,37,37,38,38,38,39,39,39,40,40,40,39,39,39,38,38,39,39,40,40,39,39,38,38,39,39,40,40,40,39,39,39,38,38,37,37,36,36,255},	// #7 Geoffrey
		{42,43,44,45,46,47,48,49,50,51,52,51,50,49,48,49,50,51,52,51,50,49,48,47,46,45,44,43,42,255},	// #8 black market bob
		{54,54,55,55,56,56,56,56,56,55,56,56,57,56,56,56,56,57,57,58,58,59,59,60,60,61,61,61,61,60,61,61,61,60,61,61,61,61,61,60,60,59,59,58,255},	// #9 melody chained
		{63,63,64,64,65,65,66,66,67,67,68,68,69,69,255},	// #10 melody unchained
		{71,72,73,74,75,74,73,72,71,255},	// #11 mickey
		{77,78,79,78,77,76,77,78,79,78,77,76,77,78,79,78,77,76,77,78,79,78,77,76,77,78,79,78,77,76,255},	// #12 guru
		{82,82,82,81,81,81,80,80,80,81,81,81,82,82,82,255},	// #13 tina
		{255},	// #14 tailor
		{93,93,94,94,94,95,95,96,96,97,97,98,98,99,99,100,100,101,101,102,102,103,103,255}, // #15 balloonist
		{105,105,106,106,107,107,108,108,109,109,108,108,107,107,108,108,109,109,108,107,107,106,106,105,105,255}, // #16 handyman
		{111,111,112,112,113,113,114,114,114,114,113,112,112,112,112,112,113,113,113,114,114,113,113,112,112,113,114,114,114,115,115,255},	// #17 historian
		{117,117,117,118,118,118,119,119,119,120,120,120,119,119,118,118,117,117,255},	// #18 magic guy
		{121,121,121,122,122,123,123,124,124,125,125,124,124,123,123,122,122,255},	// #19 buried ropeguy
		{126,126,127,127,128,128,129,129,129,129,129,130,130,131,131,131,131,132,132,133,133,255},	// #20 ropeguy
		{135,135,136,136,137,137,136,136,135,135,255},	// #21 ropewife
		{139,139,139,140,140,140,141,141,142,142,143,143,142,142,141,141,142,142,143,143,142,142,141,141,142,142,143,143,142,142,141,141,140,140,139,139,255},	// #22 CEO
		{144,144,144,144,145,145,145,145,146,146,146,146,147,147,147,147,147,147,146,146,146,145,145,145,145,144,144,144,144,255},	// #23 curaga
		{148,148,149,149,150,150,151,151,152,152,153,153,255},	// #24 onion
		{0,255},	// #25 junksmith
		{0,255},	// #26 frankie beans
		{0,255},	// #27 bonk
		{0,255},	// #28 jimmy 2-fingers
		{0,255},	// #29 smash
		{0,255},	// #30 captain capitan
		{0,255},	// #31 major fishbug
		{154,255},	// #32 clockmaker
		{0,255},	// #33 lt. hornswaggle
		{0,255},	// #34 pvt. public
		{156,156,156,157,157,157,158,158,158,158,158,158,157,157,158,158,158,158,158,157,157,157,156,156,156,156,156,157,157,158,158,158,158,158,158,157,157,156,156,255},	// #35 betty
		{160,161,162,161,160,255},	// #36 schrodinger
		{164,164,165,165,166,166,167,168,169,170,170,169,168,167,166,167,168,169,170,169,168,167,166,165,165,164,164,164,255},	// #37 sharona
		{172,172,172,172,173,173,173,174,174,174,174,173,174,174,173,174,174,174,173,173,174,173,174,174,173,174,173,173,172,172,172,255},	// #38 shemp old
		{175,175,176,176,177,177,178,178,179,179,180,180,181,181,180,180,179,179,178,178,179,179,180,180,181,181,180,180,179,179,178,178,177,177,176,176,255},	// #39 shemp young
		{182,182,182,183,183,183,184,184,184,185,185,185,185,185,185,185,185,185,185,185,185,185,185,184,184,183,183,183,182,182,255},	// #40 shroud
	};

	me->mind3++;
	me->mind2++;

	if (me->mind2 == 120 && me->tag == 14)
		MakeSound(SND_VILLAGERBORED + me->tag, me->x, me->y, SND_CUTOFF | SND_RANDOM, 200);

	if (me->mind == 0)	// standing still
	{
		if (me->tag == 24)	// onion does his idle constantly, since it's the animating smoke
		{
			me->mind = 1;
			me->mind1 = 0;
			me->mind2 = 0;
			me->facing = idle_anim[me->tag][me->mind1];
			if ((rand() % (idleOdds[me->tag])) == 0)
				MakeSound(SND_VILLAGERBORED + me->tag, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
		}
		else
		{
			me->facing = firstFrame[me->tag];
			if ((rand() % (idleOdds[me->tag])) == 0 && me->tag != 14 && (me->tag != 22 || player.levelNum != LVL_GORGE))
			{
				me->mind = 1;	// idle anim
				MakeSound(SND_VILLAGERBORED + me->tag, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
				me->mind1 = 0;
				if (idle_anim[me->tag][me->mind1] == 255)
					me->mind = 0;
				else
					me->facing = idle_anim[me->tag][me->mind1];
				me->mind2 = 0;
			}
		}
	}
	else if (me->mind == 1)	// idle anim
	{
		if (me->tag == 8)	// bob animates quarter speed
			if (me->mind3 & 3)
				me->mind1--;
		if (me->tag == 17)	// historian animates half speed
			if (me->mind3 & 1)
				me->mind1--;

		if (me->tag == 12)	// guru floats up
			me->dz += FIXAMT * 2 - FIXAMT / 8;
		me->mind1++;
		if (idle_anim[me->tag][me->mind1] == 255)
		{
			me->mind = 0;
			me->facing = firstFrame[me->tag];
		}
		else
			me->facing = idle_anim[me->tag][me->mind1];
	}
	else if (me->mind == 2)	// guru floating into the sky
	{
		if (me->z == FIXAMT)
			MakeNormalSound(SND_GURUFLOAT);

		me->facing = firstFrame[me->tag];
		me->dz += FIXAMT * 2;
		map->BrightTorch(me->mapx, me->mapy, (me->z) / FIXAMT / 2, (me->z) / FIXAMT / 2);
		me->mind1++;
		if (idle_anim[me->tag][me->mind1] == 255)
			me->mind1 = 0;
		me->facing = idle_anim[me->tag][me->mind1];

		if (me->z > FIXAMT * 100)
		{
			me->type = MONS_NONE;
			ExplodeParticles2(PART_SNOW, me->x, me->y, me->z, 100, 10);
		}
	}
}

void AI_SemiVillager(Guy* me, Map* map, world_t* world, Guy* badguy)
{
	if (goodguy == NULL)
	{
		me->dx = 0;
		me->dy = 0;
		return;
	}

	if (goodguy->type == player.monsType && RangeToTarget(me, goodguy) < 32 * FIXAMT + (10 * FIXAMT * (me->type == MONS_BRUISER)) && player.chatClock == 0 && me->hp)
	{
		BeginChatting(me->tag);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frm == 3)
			me->frmAdvance = 256;
		return;
	}

	if (me->type == MONS_THUG && (rand() % 300) == 0)
		DoMove(me, ANIM_A1, 64, 1, 0, 0);	// wave at me!

	me->dx = 0;
	me->dy = 0;
	me->facing = 2;
}

void AI_Teddy(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	BasicAI(me, SND_BEAROUCH, SND_BEARDIE, map, world, goodguy);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frm == 5)
			me->frmAdvance = 256;
		if (me->seq == ANIM_ATTACK && me->frm == 3 && me->reload == 0)
		{
			x = me->x + Cosine(me->facing * 32) * 16;
			y = me->y + Sine(me->facing * 32) * 16;
			FireBullet(me->ID, x, y, FIXAMT * 20, me->facing * 32 - 12, BLT_FIREBALL, DamageAmt(monsType[me->type].damage, me->team));
			FireBullet(me->ID, x, y, FIXAMT * 20, me->facing * 32 + 12, BLT_FIREBALL, DamageAmt(monsType[me->type].damage, me->team));
			me->reload = 30;
		}
		if (me->seq == ANIM_A1 && me->frm == 6 && me->reload == 0)
		{
			x = me->x + Cosine(me->facing * 32) * 16;
			y = me->y + Sine(me->facing * 32) * 16;

			if (me->AttackCheck(16, x >> FIXSHIFT, y >> FIXSHIFT, goodguy))
			{
				meleeAttacker = me;
				if (goodguy->GetShot(Cosine(me->facing * 32) * 4, Sine(me->facing * 32) * 4, DamageAmt(monsType[me->type].damage, me->team), map, world))
				{
					if (me->type == MONS_TEDDY4)
						goodguy->GetStunned(60);
				}
			}

			me->dx = 0;
			me->dy = 0;
			me->reload = 60;
		}

		return;	// can't do nothin' right now
	}

	if ((me->type == MONS_TEDDY || me->type == MONS_TEDDY3 || me->type == MONS_TEDDY5) && RangeToTarget(me, goodguy) < (512 * FIXAMT) && Random(32) == 0 &&
		map->CheckLOS(me->mapx, me->mapy, 15, goodguy->mapx, goodguy->mapy))
	{
		// shoot at him
		MakeSound(SND_BEARSHOOT, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
		DoMove(me, ANIM_ATTACK, 128, 1, 0, 0);
		me->reload = 0;
		FaceGoodguy(me, goodguy);
		return;
	}

	// bellyflop special attacks
	if ((me->type == MONS_TEDDY2 || me->type == MONS_TEDDY3 || me->type == MONS_TEDDY4) && Random(32) == 0 && map->CheckLOS(me->mapx, me->mapy, 5, goodguy->mapx, goodguy->mapy) && me->reload == 0)
	{
		// bellyflop
		MakeSound(SND_BEARFLOP, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
		FaceGoodguy(me, goodguy);
		DoMove(me, ANIM_A1, 128, 1, Cosine(me->facing * 32) * 6, Sine(me->facing * 32) * 6);
		return;
	}

	WanderAI(me, 64, 40, 3, map, world, goodguy);
}

void AI_AxeTrap(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;
	byte ang;
	word shotCost, timeCost;

	if (me->team == EVIL)
	{
		shotCost = 5 * MADCAP_HPBOOST;
		timeCost = 1 * MADCAP_HPBOOST;
	}
	else
	{
		shotCost = (5 * (100 + player.var[VAR_MADGEM + MC_LIFE] * 10)) / 100;
		timeCost = (1 * (100 + player.var[VAR_MADGEM + MC_LIFE] * 10)) / 100;
	}

	if (SpecificSkillLevel(SKILL_TURRET) == 0 && me->hp > 0 && me->type == MONS_AXETRAP)
	{
		me->hp = 0;
		DoMove(me, ANIM_DIE, 128, 1, 0, 0);
	}

	BasicAI(me, SND_PUPPETOUCH, 0, map, world, goodguy);
	me->tag = 255;

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE)
		{
			me->frmAdvance = 256;
			if (me->frm == 1)
			{
				MakeSound(SND_BOOM, me->x, me->y, SND_CUTOFF | SND_RANDOM, 400);
				FireBullet(me->ID, me->x, me->y, 0, 0, BLT_SMALLBOOM, DamageAmt(SpecificSkillLevel(SKILL_TURRET), me->team));
			}
		}
		if (me->seq == ANIM_ATTACK && me->frm == 2 && me->reload == 0)
		{
			x = me->x + Cosine(me->facing * 32) * 20;
			y = me->y + Sine(me->facing * 32) * 20;
			if (goodguy)
				ang = AngleFrom(me->x, me->y, goodguy->x, goodguy->y);
			else
				ang = me->facing * 32;
			if (me->type == MONS_JUNKTRAP)
				FireBullet(me->ID, me->x, me->y, FIXAMT * 15, ang, BLT_FIREBALL, DamageAmt(monsType[me->type].damage, me->team));
			else
				FireBullet(me->ID, me->x, me->y, FIXAMT * 15, ang, BLT_AXE, bulDef[BLT_AXE].damage);
			if (me->hp > shotCost)
				me->hp -= shotCost;
			else
			{
				meleeAttacker = NULL;
				bulletHittingType = 0;
				bulletHittingOwner = 65535;	// AA bear fix
				me->GetShotReal(0, 0, 9999, map, world);	// die from overuse
			}

			me->reload = 20;
		}

		return;	// can't do nothin' right now
	}

	if (me->mind > 0)
	{
		if (me->mind == 1)
		{
			if (me->hp > timeCost)
				me->hp -= timeCost;
			else
			{
				meleeAttacker = NULL;
				bulletHittingType = 0;
				bulletHittingOwner = 65535;	// AA bear fix
				me->GetShotReal(0, 0, 9999, map, world);	// die from timing out
			}
		}
		me->mind--;
	}
	else
	{
		me->mind = 30;
	}

	if (me->z == 0)
	{
		me->dx = 0;	// landed from being launched
		me->dy = 0;
	}
	if (me->z == 0 && goodguy && RangeToTarget(me, goodguy) < (512 * FIXAMT) && me->reload == 0 &&
		map->CheckLOS(me->mapx, me->mapy, 15, goodguy->mapx, goodguy->mapy))
	{
		// shoot at him
		MakeSound(SND_TINSHOOT, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
		DoMove(me, ANIM_ATTACK, 128, 1, 0, 0);
		me->reload = 0;
		FaceGoodguy(me, goodguy);
		return;
	}
}

void AI_Bonehead(Guy* me, Map* map, world_t* world, Guy* badguy)
{
	int x, y, range;
	word dmg;

	if ((!goodguy || goodguy->hp == 0 || SpecificSkillLevel(SKILL_BONEHEAD) == 0) && me->hp)
	{
		me->hp = 1;
		meleeAttacker = NULL;
		me->GetShotReal(0, 0, 9999, map, world);
		return;
	}

	BasicAI(me, SND_SKELOUCH, SND_SKELDIE, map, world, badguy);

	if (player.amulet.magic == EM_BONEY || player.amulet.magic == EM_DEATH)
	{
		// regenerate
		if (me->mind3 < 30 * 5)
			me->mind3++;
		else
		{
			me->mind3 = 0;
			if (me->hp < MonsterHP(me->type, me->team))
			{
				x = me->hp + RecoverAmt(1);
				if (x > MonsterHP(me->type, me->team))
					x = MonsterHP(me->type, me->team);
				me->hp = x;
				me->frozen = 1;
			}
		}
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frm == 5)
			me->frmAdvance = 256;
		if (me->seq == ANIM_ATTACK && me->frm == 5 && me->reload == 0)
		{
			x = me->x + Cosine(me->facing * 32) * 16;
			y = me->y + Sine(me->facing * 32) * 16;
			if (badguy != NULL && me->AttackCheck(24, x >> FIXSHIFT, y >> FIXSHIFT, badguy))
			{
				meleeAttacker = me;
				dmg = (word)((float)((1 + (SpecificSkillVal(0, SKILL_BONEBOMB) / 2))) * SpellDamageBoost(SC_DEATH) / 100.0f) + DamageAmt(3 * (player.amulet.magic == EM_SUMMONERS), GOOD);
				badguy->GetShot(Cosine(me->facing * 32) * 4, Sine(me->facing * 32) * 4, dmg, map, world);
			}
			if (player.amulet.magic == EM_BONEY || player.amulet.magic == EM_DEATH)
			{
				FireBullet(me->ID, me->x, me->y, me->z + FIXAMT * 20, me->facing * 32, BLT_SKELSHOT, (word)(2.0f * SpellDamageBoost(SC_DEATH) / 100.0f) + DamageAmt(3 * (player.amulet.magic == EM_SUMMONERS), GOOD));
			}
			me->reload = 10;
		}

		return;	// can't do nothin' right now
	}

	x = Distance(me->x, me->y, goodguy->x, goodguy->y);
	if (badguy != NULL)
		y = Distance(me->x, me->y, badguy->x, badguy->y);
	else
		y = FIXAMT * 1000;

	if (player.amulet.magic == EM_BONEY || player.amulet.magic == EM_DEATH)
		range = FIXAMT * 100;
	else
		range = FIXAMT * 54;

	if (me->mind2 == 0)	// in attack mode
	{
		if (badguy != NULL)
			WanderAI(me, 64, 40, 3, map, world, badguy);
		me->frmAdvance = (word)SpecificSkillVal(1, SKILL_BONEHEAD);
		if (x > FIXAMT * 256 || badguy == NULL || badguy->type == MONS_NONE || badguy->hp == 0 ||
			y > FIXAMT * 256)
		{
			me->mind2 = 1;	// chase player
			return;
		}
		// sword attack
		if (map->CheckLOS(me->mapx, me->mapy, 5, badguy->mapx, badguy->mapy) && y < range)
		{
			MakeSound(SND_SKELSWING, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
			FaceGoodguy(me, badguy);
			me->seq = ANIM_ATTACK;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = (word)SpecificSkillVal(1, SKILL_BONEHEAD);
			me->action = ACTION_BUSY;
			me->dx = 0;
			me->dy = 0;
			return;
		}
	}
	if (me->mind2 == 1)
	{
		FaceGoodguy(me, goodguy);

		me->dx = (Cosine(me->facing * 32) * monsType[me->type].speed) / FIXAMT;
		me->dy = (Sine(me->facing * 32) * monsType[me->type].speed) / FIXAMT;
		if (me->seq != ANIM_MOVE)
		{
			me->seq = ANIM_MOVE;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = (word)SpecificSkillVal(1, SKILL_BONEHEAD);
		}
		if (x < FIXAMT * 64)	// got close
		{
			me->mind2 = 2;
			me->facing = rand() % 8;
		}
	}
	if (me->mind2 == 2)	// close to Loony, so wander randomly until either too far away or a badguy is near
	{
		me->dx = (Cosine(me->facing * 32) * monsType[me->type].speed) / FIXAMT;
		me->dy = (Sine(me->facing * 32) * monsType[me->type].speed) / FIXAMT;
		if (me->seq != ANIM_MOVE)
		{
			me->seq = ANIM_MOVE;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = (word)SpecificSkillVal(1, SKILL_BONEHEAD);
		}

		if (x > FIXAMT * 70)
			me->mind2 = 1;	// go back to following
		if (y < FIXAMT * 128 || goodguy->ouch)
			me->mind2 = 0;	// an enemy came near, so beat on him
	}
}

void AI_Mouse(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;
	byte rightAng;
	BasicAI(me, SND_MOUSEOUCH, SND_MOUSEDIE, map, world, goodguy);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frm == 7)
		{
			if (me->type == MONS_MOUSE4 && me->reload == 0)
			{
				me->reload = 10;
				for (x = 0; x < 255; x += 16)
				{
					FireBullet(me->ID, me->x, me->y, FIXAMT * 10, (byte)x, BLT_POISON, DamageAmt(monsType[me->type].damage, me->team));
				}
			}
			me->frmAdvance = 256;
		}
		if (me->type == MONS_MOUSE2)
		{
			if (me->seq == ANIM_A1 && monsType[me->type].anim[me->seq][me->frm] == 9 && me->frmTimer < 128)
			{
				me->reload = 90;
				MakeSound(SND_MOUSESHOOT, me->x, me->y, SND_CUTOFF | SND_RANDOM, 600);
				FireBullet(me->ID, me->x, me->y, FIXAMT * 20, (byte)(me->facing * 32 - 8 + Random(17)), BLT_FIREBALL, DamageAmt(monsType[me->type].damage, me->team));
			}
			if (me->seq == ANIM_A1 && me->frm == 6)
				me->reload = 90;
		}
		else if (me->type == MONS_ICEMICE)
		{
			if (me->seq == ANIM_A1 && monsType[me->type].anim[me->seq][me->frm] == 9 && me->reload == 0)
			{
				me->reload = 90;
				MakeSound(SND_MOUSESHOOT, me->x, me->y, SND_CUTOFF | SND_RANDOM, 600);
				FireBullet(me->ID, me->x, me->y, FIXAMT * 20, (byte)(me->facing * 32), BLT_CRYO, DamageAmt(monsType[me->type].damage, me->team));
			}
		}
		else if (me->type == MONS_SPICEMICE)
		{
			if (me->seq == ANIM_A1 && monsType[me->type].anim[me->seq][me->frm] == 9 && me->reload == 0)
			{
				me->reload = 90;
				MakeSound(SND_FIRESHOOT, me->x, me->y, SND_CUTOFF | SND_RANDOM, 600);
				FireBullet(me->ID, me->x, me->y, FIXAMT * 20, (byte)(me->facing * 32), BLT_BADIGNITE, 0);
			}
		}

		return;	// can't do nothin' right now
	}

	if (!goodguy)
		return;

	if (me->type == MONS_SPICEMICE && (rand() % 10) == 0)
		FireBullet(me->ID, me->x, me->y, 0, 0, BLT_FLAMEB, FlameDmg(monsType[me->type].damage, me->team));

	if (me->type == MONS_MOUSE2 || me->type == MONS_ICEMICE || me->type == MONS_SPICEMICE)
	{
		if (me->reload == 0 && map->CheckLOS(me->mapx, me->mapy, 10, goodguy->mapx, goodguy->mapy))
		{
			DoMove(me, ANIM_A1, 128, 1, 0, 0);
			return;
		}
	}

	if ((me->type == MONS_MOUSE || me->type == MONS_ICEMICE || me->type == MONS_MOUSE4) && RangeToTarget(me, goodguy) < 40 * FIXAMT && me->reload == 0)
	{
		x = me->x + Cosine(me->facing * 32) * 16;
		y = me->y + Sine(me->facing * 32) * 16;
		if (me->AttackCheck(16, x >> FIXSHIFT, y >> FIXSHIFT, goodguy))
		{
			meleeAttacker = me;
			if (goodguy->GetShot(Cosine(me->facing * 32) * 2, Sine(me->facing * 32) * 2, DamageAmt(monsType[me->type].damage, me->team), map, world))
			{
				if (me->type == MONS_MOUSE4)
					goodguy->GetPoisoned(30 * 2);
			}
			me->reload = 30;
			me->mind1 = 10;
			me->mind = 1 + (rand() % 2);
		}
	}

	if (me->mind1)
	{
		me->mind1--;
	}
	else
	{
		switch (rand() % 8)
		{
		case 0:
			me->mind = 1;
			break;
		case 1:
			me->mind = 2;
			break;
		case 2:
			me->mind = 3;
			break;
		default:
			me->mind = 0;
			break;
		}
		me->mind1 = 1 + (rand() % 20);
	}

	if (me->mind == 0)	// heading straight at guy
	{
		rightAng = AngleFrom(me->x, me->y, goodguy->x, goodguy->y);
	}
	else if (me->mind == 1)	// turning right
	{
		rightAng = me->mind3 + 10;
	}
	else if (me->mind == 2)	// turning left
	{
		rightAng = me->mind3 - 10;
	}
	else if (me->mind == 3)	// going straight
	{
		rightAng = me->mind3;
	}

	me->mind3 = TurnToward(me->mind3, rightAng, 3);

	me->facing = ((me->mind3 + 16) / 32) & 7;
	me->dx = Cosine(me->mind3) * monsType[me->type].speed / FIXAMT;
	me->dy = Sine(me->mind3) * monsType[me->type].speed / FIXAMT;

	if (me->seq != ANIM_MOVE)
		DoMove(me, ANIM_MOVE, 128, 0, me->dx, me->dy);
}

void AI_Puppet(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	BasicAI(me, SND_PUPPETOUCH, SND_PUPPETDIE, map, world, goodguy);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frm == 4)
			me->frmAdvance = 256;
		if (me->seq == ANIM_ATTACK && me->frm == 4 && me->reload == 0)
		{
			int w = (me->type != MONS_ONEPUNCH) ? 16 : 20;	// AA One Punch aoe extend
			x = me->x + Cosine(me->facing * 32) * w;
			y = me->y + Sine(me->facing * 32) * w;
			if (me->AttackCheck(w, x >> FIXSHIFT, y >> FIXSHIFT, goodguy))
			{
				meleeAttacker = me;

				if (me->type == MONS_PUPPET3)
				{
					if (goodguy->GetShot(Cosine(me->facing * 32) * 16, Sine(me->facing * 32) * 16, DamageAmt(monsType[me->type].damage, me->team), map, world))
					{
						if (goodguy->ouch == 4)
							goodguy->GetStunned(20);
					}
				}
				else if (me->type == MONS_ONEPUNCH)
				{
					if(goodguy->GetShot(Cosine(me->facing * 32) * 60, Sine(me->facing * 32) * 60, DamageAmt(monsType[me->type].damage, me->team), map, world))
						MakeSound(SND_BOOM, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);	// Make Sound only if hit
				}
				else
					goodguy->GetShot(Cosine(me->facing * 32) * 4, Sine(me->facing * 32) * 4, DamageAmt(monsType[me->type].damage, me->team), map, world);
			}
			me->reload = 30;
		}
		if (me->seq == ANIM_A2 && me->frm == 7 && me->reload == 0)
		{
			Guy* g;

			x = me->x + Cosine(me->facing * 32) * 20;
			y = me->y + Sine(me->facing * 32) * 20;
			g = AddGuy(x, y, 0, MONS_MOUSE);
			if (g && !g->CanWalkNoMons(g->x, g->y, map, world))
				g->type = MONS_NONE;
			else if (g)
				g->tag = 255;	// summoned monster - worth no XP or items
			me->reload = 3;
		}

		return;	// can't do nothin' right now
	}

	// punch when you get close
	if (Random(10) == 0 && map->CheckLOS(me->mapx, me->mapy, 2, goodguy->mapx, goodguy->mapy) && me->reload == 0)
	{
		MakeSound(SND_PUPPETPUNCH, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
		FaceGoodguy(me, goodguy);
		me->seq = ANIM_ATTACK;
		me->frm = 0;
		me->frmTimer = 0;
		if (me->type == MONS_PUPPET || me->type == MONS_ONEPUNCH)
			me->frmAdvance = 32;
		else if (me->type == MONS_PUPPET2 || me->type == MONS_PUPPET3)
			me->frmAdvance = 128;
		me->action = ACTION_BUSY;
		me->dx = 0;
		me->dy = 0;
		return;
	}
	// Magic Hat summons rats from his hat
	if (me->type == MONS_PUPPET2 && me->reload == 0 && Random(30) == 0)
	{
		FaceGoodguy(me, goodguy);
		MakeSound(SND_PUPPETHAT, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
		DoMove(me, ANIM_A2, 128, 1, 0, 0);
		return;
	}
	WanderAI(me, 64, 40, 3, map, world, goodguy);
}

void AI_Cryozoid(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;
	byte ang;

	if (SpecificSkillLevel(SKILL_CRYOZOID) == 0 && me->hp > 0)	// AA fix, so they don't get stuck!
	{
		me->hp = 0;
		DoMove(me, ANIM_DIE, 128, 1, 0, 0);	// die!
	}
	if (me->mind > 0)
	{
		if (me->mind1)
			me->mind1--;
		else
		{
			me->mind--;
			me->mind1 = 30;
		}
	}
	else if (me->hp > 0)
	{
		me->hp = 0;
		DoMove(me, ANIM_DIE, 128, 1, 0, 0);	// die!
	}

	if (me->reload)
		me->reload--;

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK && me->frm == 3 && me->reload == 0)
		{
			x = me->x + Cosine(me->facing * 32) * 16;
			y = me->y + Sine(me->facing * 32) * 16;
			if (goodguy)
				ang = AngleFrom(me->x, me->y, goodguy->x, goodguy->y);
			else
				ang = me->facing * 32;
			FireBullet(me->ID, me->x, me->y, FIXAMT * 30, ang, BLT_CRYO, (word)(((1 + SpecificSkillLevel(SKILL_CRYOZOID) / 2) * SpellDamageBoost(SC_ICE) / 100.0f)) + DamageAmt(3 * (player.amulet.magic == EM_SUMMONERS), GOOD));
			me->reload = (byte)SpecificSkillVal(1, SKILL_CRYOZOID);
		}

		return;	// can't do nothin' right now
	}

	if (goodguy && RangeToTarget(me, goodguy) < (300 * FIXAMT) &&
		map->CheckLOS(me->mapx, me->mapy, 15, goodguy->mapx, goodguy->mapy) && me->reload == 0)
	{
		MakeSound(SND_CRYOSHOOT, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
		FaceGoodguy(me, goodguy);
		DoMove(me, ANIM_ATTACK, 128, 1, 0, 0);
	}
}

void AI_Shroom(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	BasicAI(me, SND_SKELOUCH, SND_SKELDIE, map, world, goodguy);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frm == 4)
			me->frmAdvance = 256;

		return;	// can't do nothin' right now
	}

	me->dx = Cosine(me->facing * 32) * monsType[me->type].speed / FIXAMT;
	me->dy = Sine(me->facing * 32) * monsType[me->type].speed / FIXAMT;
	if (me->seq != ANIM_MOVE)
		DoMove(me, ANIM_MOVE, 64, 0, me->dx, me->dy);

	if ((rand() % 20) == 0)
		me->facing = (me->facing + 1 - (rand() % 3)) & 7;

	if (me->shroomTime)
		me->shroomTime--;
	else
	{
		me->type = me->originalType;
		DoMove(me, ANIM_IDLE, 128, 0, 0, 0);
		MakeRingParticle(me->x, me->y, 0, 32, 32);
	}
}

void AI_TinGuy(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	BasicAI(me, SND_TINOUCH, SND_TINDIE, map, world, goodguy);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frm == 8)
			me->frmAdvance = 256;
		if (me->seq == ANIM_ATTACK && me->frm == 2 && me->reload > 0 && me->type != MONS_ARMYONE)
		{
			me->frmTimer = 0;	// wait here until ready to fire
		}
		if (me->seq == ANIM_ATTACK && me->frm == 2 && me->reload == 0 && me->type != MONS_ARMYONE)
		{
			x = me->x + Cosine(me->facing * 32) * 16;
			y = me->y + Sine(me->facing * 32) * 16;
			MakeSound(SND_TINSHOOT, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
			FireBullet(me->ID, x, y, FIXAMT * 30, me->facing * 32, BLT_FIREBALL, DamageAmt(monsType[me->type].damage, me->team));
			if (me->type == MONS_TRIGUN || me->type == MONS_TINPOT)
			{
				FireBullet(me->ID, x, y, FIXAMT * 30, me->facing * 32 - 32, BLT_FIREBALL, DamageAmt(monsType[me->type].damage, me->team));
				FireBullet(me->ID, x, y, FIXAMT * 30, me->facing * 32 + 32, BLT_FIREBALL, DamageAmt(monsType[me->type].damage, me->team));
			}
			me->reload = 20;
			if (me->type == MONS_TINGUY)
				me->mind2 = 40;
			me->dx = -Cosine(me->facing * 32) * 4;
			me->dy = -Sine(me->facing * 32) * 4;
		}
		if (me->seq == ANIM_ATTACK && me->frm >= 2 && me->frm < 6 && me->type == MONS_ARMYONE)
		{
			byte f;

			FaceGoodguy(me, goodguy);
			f = AngleFrom(me->x, me->y, goodguy->x, goodguy->y) + ((me->frm - 2) * 256 + me->frmTimer) / 32 - 20;
			x = me->x + Cosine(me->facing * 32) * 16;
			y = me->y + Sine(me->facing * 32) * 16;
			MakeSound(SND_TINSHOOT, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
			FireBullet(me->ID, x, y, FIXAMT * 30, f, BLT_TURRET, DamageAmt(monsType[me->type].damage, me->team));
			me->reload = 30;
		}
		if (me->seq == ANIM_A2 && me->frm == 2 && me->reload == 0)
		{
			MakeSound(SND_TINSHOOT, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
			FireBullet(me->ID, me->x, me->y, FIXAMT * 30, me->facing * 32, BLT_GRENADE, DamageAmt(monsType[me->type].damage, me->team));
			if (me->type == MONS_ARMYONE)
			{
				FireBullet(me->ID, me->x, me->y, FIXAMT * 30, me->facing * 32 - 16, BLT_GRENADE, DamageAmt(monsType[me->type].damage, me->team));
				FireBullet(me->ID, me->x, me->y, FIXAMT * 30, me->facing * 32 + 16, BLT_GRENADE, DamageAmt(monsType[me->type].damage, me->team));
				me->reload = 10;
			}
			else
				me->reload = 60;
		}

		return;	// can't do nothin' right now
	}

	if (me->type == MONS_TINPOT)
	{
		if (me->mind3)
			me->mind3--;
		else
		{
			me->mind3 = 60;
			x = GuyList_First();
			y = 0;
			while (x != 65535)
			{
				if (GetGuy(x)->team == me->team && GetGuy(x)->type != MONS_TINPOT && GetGuy(x)->hp > 0 && map->CheckLOS(me->mapx, me->mapy, 10, GetGuy(x)->mapx, GetGuy(x)->mapy)) // AA fix persuasion
				{
					if (GetGuy(x)->type == player.monsType)
						HealGoodguy(9999);
					else
						GetGuy(x)->hp = MonsterHP(GetGuy(x)->type, GetGuy(x)->RealTeam());	// AA hp madcap fix
					if (GetGuy(x)->type == MONS_YOURBOT)
					{
						player.var[VAR_CLOCKLIFE] = GetGuy(x)->hp & 0xFF;
						player.var[VAR_CLOCKLIFE2] = GetGuy(x)->hp >> 8;
					}
					map->map[GetGuy(x)->mapx + (GetGuy(x)->mapy * map->width)].templight = 32;
					if (y == 0)
					{
						MakeSound(SND_BIGHEAL, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
						y = 1;
					}
					LightningBolt(me->x, me->y - me->z - FIXAMT * 20, GetGuy(x)->x, GetGuy(x)->y - FIXAMT * 20);
				}
				x = GuyList_Next();
			}
		}
	}

	if (me->mind2)
	{
		me->mind2--;	// just stand around recovering from having shot
		me->dx /= 2;
		me->dy /= 2;
	}
	else
	{
		// shoot when you have a shot
		if (Random(30) == 0 && map->CheckLOS(me->mapx, me->mapy, 15, goodguy->mapx, goodguy->mapy) && me->reload == 0)
		{
			FaceGoodguy(me, goodguy);
			DoMove(me, ANIM_ATTACK, 128, 1, 0, 0);
			me->reload = 20;
			return;
		}
		// lob nukes
		if ((me->type == MONS_TINGUY2 || me->type == MONS_ARMYONE) && Random(60) == 0 && me->reload == 0)
		{
			FaceGoodguy(me, goodguy);
			DoMove(me, ANIM_A2, 128, 1, 0, 0);
			return;
		}
		WanderAI(me, 64, 40, 3, map, world, goodguy);
	}
}

void AI_Ratatouille(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;
	byte rightAng;
	BasicAI(me, SND_MOUSEOUCH, SND_MOUSEDIE, map, world, goodguy);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frm == 7)
			me->frmAdvance = 256;

		return;	// can't do nothin' right now
	}

	if (me->mind == 0)	// winding up
	{
		if (!map->CheckLOS(me->mapx, me->mapy, 10, goodguy->mapx, goodguy->mapy))
		{
			if (me->mind2 > 3)
				me->mind2 -= 3;
		}
		if (me->seq != ANIM_IDLE)
			DoMove(me, ANIM_IDLE, 256, 0, 0, 0);
		me->frmAdvance = 512 - me->mind2 * 2;
		me->mind2 += 3;
		if (me->mind2 >= 250)
		{
			DoMove(me, ANIM_MOVE, 256, 0, Cosine(me->mind3) * monsType[me->type].speed / FIXAMT, Sine(me->mind3) * monsType[me->type].speed / FIXAMT);
			me->mind2 = 0;
			me->mind = 1;	// begin zooming
		}
		rightAng = AngleFrom(me->x, me->y, goodguy->x, goodguy->y);
		me->mind3 = TurnToward(me->mind3, rightAng, 6);
	}
	else	// zooming
	{
		if (me->seq != ANIM_MOVE)
		{
			DoMove(me, ANIM_MOVE, 256, 0, Cosine(me->mind3) * monsType[me->type].speed / FIXAMT, Sine(me->mind3) * monsType[me->type].speed / FIXAMT);
			x = me->x + Cosine(me->mind3) * 16;
			y = me->y + Sine(me->mind3) * 16;
			if (me->AttackCheck(20, x >> FIXSHIFT, y >> FIXSHIFT, goodguy))
			{
				meleeAttacker = me;
				goodguy->GetShot(Cosine(me->facing * 32) * 6, Sine(me->facing * 32) * 6, DamageAmt(monsType[me->type].damage, me->team), map, world);
			}
		}
		me->mind2++;
		if (me->mind2 > 40 || me->mind1 > 0)
		{
			me->mind2 = 0;
			me->mind = 0;	// wind up again
			DoMove(me, ANIM_IDLE, 256, 0, 0, 0);
		}
		if (me->wallBump > 0)	// bonked the wall
		{
			meleeAttacker = NULL;
			me->GetShotReal(Cosine(me->facing * 32) * -8, Sine(me->facing * 32) * -8, DamageAmt(10, EVIL), map, world);
			me->GetStunned(30);
			me->wallBump = 0;
			me->mind2 = 0;
			me->mind = 0;	// wind up again
		}
	}
	me->mind1 = 0;
	me->facing = me->mind3 / 32;
}

void AI_SockMonkey(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	BasicAI(me, SND_MONKEYOUCH, SND_MONKEYDIE, map, world, goodguy);

	if (me->mind3)
		me->mind3--;

	if (me->type == MONS_MONKEY3)
	{
		// electrocute nearby enemies
		if (me->mind2 == 0)
		{
			int t;
			t = BulletFindTargetClosest(me->team, me->x, me->y, 0, 90 * FIXAMT, curMap, world);
			LightningBolt(me->x, me->y - me->z - FIXAMT * 20, me->x - 10 * FIXAMT + (rand() % (20 * FIXAMT + 1)), me->y - me->z - FIXAMT * 10 - 20 * FIXAMT + (rand() % (40 * FIXAMT + 1)));
			map->BrightTorch(me->mapx, me->mapy, 10, 2);
			if (t != 0)
			{
				meleeAttacker = NULL;
				bulletHittingType = 0;
				bulletHittingOwner = 65535;	// AA bear fix
				GetGuy(t - 1)->GetShotReal(0, 0, ShockDmg(1, me->team), curMap, world);
				LightningBolt(me->x, me->y - me->z, GetGuy(t - 1)->x, GetGuy(t - 1)->y - FIXAMT * 20);
			}
			me->mind2 = 30;
		}
		else
			me->mind2--;
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frm == 11)
			me->frmAdvance = 256;
		if (me->seq == ANIM_ATTACK)
			FaceGoodguy(me, goodguy);
		if (me->seq == ANIM_ATTACK && me->frm == 7)
		{
			// begin dashing
			DoMove(me, ANIM_A1, 256, 1, Cosine(me->facing * 32) * 8 * (1 + (me->type == MONS_GHOSTMONKEY)), Sine(me->facing * 32) * 8 * (1 + (me->type == MONS_GHOSTMONKEY)));
			MakeSound(SND_MONKEYDASH, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
			me->reload = 120 - 60 * (me->type == MONS_GHOSTMONKEY || me->type == MONS_ROCKMONKEY);
		}
		else if (me->seq == ANIM_A1 && me->frm < 23 && me->mind3 == 0)
		{
			int i;

			x = me->x + Cosine(me->facing * 32) * 16;
			y = me->y + Sine(me->facing * 32) * 16;
			i = FindVictim(x, y, me->mapx, me->mapy, 20, me->ID + 1, (GOOD + EVIL) - me->team, 0, map, world);
			if (i)
			{
				byte ang;
				ang = AngleFrom(me->x, me->y, GetGuy((word)(i - 1))->x, GetGuy((word)(i - 1))->y);
				if (rand() % 2)
					ang += 48;
				else
					ang -= 48;	// knock you off at a near sideways angle
				meleeAttacker = me;
				GetGuy((word)(i - 1))->GetShot(Cosine(ang) * 20, Sine(ang) * 20, DamageAmt(monsType[me->type].damage, me->team), map, world);
				me->mind3 = 20;
			}
		}
		else if (me->seq == ANIM_A1)
		{
			me->dx /= 2;
			me->dy /= 2;
		}
		if (me->seq == ANIM_A2 && me->frm == 7 && me->reload == 0)
		{
			x = me->x + Cosine(me->facing * 32) * 16;
			y = me->y + Sine(me->facing * 32) * 16;
			MakeSound(SND_MONKEYSHOOT, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
			me->reload = 20;
			if (me->type == MONS_MONKEY4)
			{
				FireBullet(me->ID, x, y, FIXAMT * 30, me->facing * 32, BLT_HOMER, DamageAmt(monsType[me->type].damage, me->team));
				me->reload = 60;
			}
			else if (me->type == MONS_MONKEY2)
				FireBullet(me->ID, x, y, FIXAMT * 30, me->facing * 32, BLT_POISON, DamageAmt(monsType[me->type].damage, me->team));
			else if (me->type == MONS_ROCKMONKEY)
				FireBullet(me->ID, x, y, 0, me->facing * 32, BLT_EVILSPIKE, DamageAmt(monsType[me->type].damage / 2, me->team));
			else
				FireBullet(me->ID, x, y, FIXAMT * 30, me->facing * 32, BLT_FIREBALL, DamageAmt(monsType[me->type].damage, me->team));
		}

		return;	// can't do nothin' right now
	}

	// dash at medium range
	if (Random(10) == 0 && map->CheckLOS(me->mapx, me->mapy, 10, goodguy->mapx, goodguy->mapy) && me->reload == 0 && (me->type == MONS_MONKEY || me->type == MONS_GHOSTMONKEY || me->type == MONS_ROCKMONKEY))
	{
		FaceGoodguy(me, goodguy);
		DoMove(me, ANIM_ATTACK, 64 + 128 * (me->type == MONS_GHOSTMONKEY), 1, 0, 0);
		return;
	}
	// shoot if you can
	if (Random(10) == 0 && map->CheckLOS(me->mapx, me->mapy, 15, goodguy->mapx, goodguy->mapy) && me->reload == 0 && (me->type == MONS_MONKEY2 || me->type == MONS_MONKEY3 || me->type == MONS_MONKEY4 || me->type == MONS_ROCKMONKEY))
	{
		MakeSound(SND_MONKEYLOAD, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
		FaceGoodguy(me, goodguy);
		DoMove(me, ANIM_A2, 128, 1, 0, 0);
		return;
	}

	WanderAI(me, 64, 40, 3, map, world, goodguy);
}

void AI_Klonk(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;
	int i;

	if (me->team == GOOD && player.levelNum != LVL_CLEARING && me->mindControl == 0)
	{
		// be like a villager if we are friends
		if (player.levelNum != LVL_TOYFACTORY)
			AI_SemiVillager(me, map, world, goodguy);
		return;
	}

	if (goodguy == NULL)
	{
		me->dx = 0;
		me->dy = 0;
		return;
	}

	BasicAI(me, SND_KLONKOUCH, SND_KLONKDIE, map, world, goodguy);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE)
			me->facing = 2;
		if (me->seq == ANIM_DIE && me->frm == 6)
			me->frmAdvance = 256;
		if (me->seq == ANIM_ATTACK && me->frm == 5 && me->reload == 0)
		{
			me->reload = 30 * 3 - 30 * (me->type == MONS_KLUNK);
			ShakeScreen(5);

			x = me->x + Cosine(me->facing * 32) * 60;
			y = me->y + Sine(me->facing * 32) * 60;

			i = 0;
			while (1)
			{
				// smash everyone in range
				i = FindVictim(x, y, me->mapx, me->mapy, 60, 0, (GOOD + EVIL) - me->team, i, map, world);
				if (i)
				{
					byte ang;
					ang = AngleFrom(me->x, me->y, GetGuy((word)(i - 1))->x, GetGuy((word)(i - 1))->y);
					meleeAttacker = me;
					GetGuy((word)(i - 1))->GetShot(Cosine(ang) * 16, Sine(ang) * 16, DamageAmt(monsType[me->type].damage, me->team), map, world);
				}
				else break;
			}
			if (me->type == MONS_KLONK)
			{
				MakeRingParticle(x, y, 0, 60, 40);
				SmashTrees(me->ID, x, y, 60, map);
			}
			else if (me->type == MONS_KLUNK)
			{
				for (i = 0; i < 16; i++)
				{
					FireBullet(me->ID, x, y, 20 * FIXAMT, i * 16, BLT_FIREBALL, DamageAmt(monsType[me->type].damage / 2 + 1, me->team));
				}
			}
			else	// bruiser
			{
				for (i = 0; i < 6; i++)
				{
					FireBullet(me->ID, x - FIXAMT * 40 + (rand() % (FIXAMT * 80 + 1)), y - FIXAMT * 40 + (rand() % (FIXAMT * 80 + 1)), 0, 0, BLT_FLAMEB, FlameDmg(1, me->team));
				}
			}
		}

		return;	// can't do nothin' right now
	}

	// smash if goodguy is there, OR if you bump into a tree
	if (((me->wallBump && me->type == MONS_KLONK) || RangeToTarget(me, goodguy) < 100 * FIXAMT + 200 * FIXAMT * (me->type == MONS_KLUNK || me->type == MONS_BRUISER)) && me->reload == 0)
	{
		if (me->wallBump == 0)
			FaceGoodguy(me, goodguy);
		DoMove(me, ANIM_ATTACK, 80, 1, 0, 0);
		MakeSound(SND_KLONKSMASH, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
		me->wallBump = 0;
		return;
	}

	me->wallBump = 0;
	WanderAI(me, 255, 3, 1, map, world, goodguy);
}

void AI_Toasty(Guy* me, Map* map, world_t* world, Guy* badguy)
{
	byte f;

	if ((!goodguy || goodguy->hp == 0 || SpecificSkillLevel(SKILL_TOASTIES) == 0) && me->hp)
	{
		me->type = MONS_NONE;
		return;
	}

	if (me->reload == 0)
	{
		if (badguy)
		{
			f = AngleFrom(me->x, me->y, badguy->x, badguy->y);
			MakeSound(SND_FIRESHOOT, me->x, me->y, SND_CUTOFF | SND_RANDOM, 100);
			FireBullet(goodguy->ID, me->x, me->y, FIXAMT * 20, f, BLT_PFIREBALL, (word)((float)monsType[me->type].damage * SpellDamageBoost(SC_FIRE) / 100.0f) + DamageAmt(3 * (player.amulet.magic == EM_SUMMONERS), GOOD));
			me->reload = 30 * 3 - 20 + (rand() % 21);
		}
	}
	else
		me->reload--;

	if (me->x < goodguy->x)
	{
		me->dx += FIXAMT + (rand() % (FIXAMT / 4));
		me->dy -= FIXAMT / 2 + (rand() % (FIXAMT / 8));
	}
	if (me->x > goodguy->x)
	{
		me->dx -= FIXAMT + (rand() % (FIXAMT / 4));
		me->dy += FIXAMT / 2 + (rand() % (FIXAMT / 8));
	}
	if (me->y < goodguy->y)
	{
		me->dy += FIXAMT + (rand() % (FIXAMT / 4));
		me->dx += FIXAMT / 2 + (rand() % (FIXAMT / 8));
	}
	if (me->y > goodguy->y)
	{
		me->dy -= FIXAMT + (rand() % (FIXAMT / 4));
		me->dx -= FIXAMT / 2 + (rand() % (FIXAMT / 8));
	}

	Clamp(&me->dx, monsType[me->type].speed);
	Clamp(&me->dy, monsType[me->type].speed);
}

void AI_Rose(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	BasicAI(me, SND_PLANTOUCH, SND_PLANTDIE, map, world, goodguy);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK && me->frm == 5 && me->reload == 0 && goodguy)
		{
			byte f;
			bullet_t* b;

			if (me->type == MONS_ROSE)	// Stone Rose boss
			{
				me->reload = 30 + 30 / 2;

				f = AngleFrom(me->x, me->y, goodguy->x, goodguy->y);
				b = FireBullet(me->ID, me->x, me->y, FIXAMT * 50, f, BLT_GRENADE, DamageAmt(monsType[me->type].damage, me->team));
				if (b)
					b->speed = FIXAMT * 6;
				b = FireBullet(me->ID, me->x, me->y, FIXAMT * 50, f, BLT_GRENADE, DamageAmt(monsType[me->type].damage, me->team));
				if (b)
					b->speed = FIXAMT * 4;
				b = FireBullet(me->ID, me->x, me->y, FIXAMT * 50, f, BLT_GRENADE, DamageAmt(monsType[me->type].damage, me->team));
				if (b)
					b->speed = FIXAMT * 2;
			}
			else if (me->type == MONS_ROSE2)
			{
				me->reload = 30;

				f = AngleFrom(me->x, me->y, goodguy->x, goodguy->y);
				b = FireBullet(me->ID, me->x, me->y, FIXAMT * 50, f, BLT_GRENADE, DamageAmt(monsType[me->type].damage, me->team));
				if (b)
					b->speed = FIXAMT * (rand() % 10);
			}
			else if (me->type == MONS_ROSE3 || me->type == MONS_ROSE4)	// yellow rose
			{
				int i;

				me->reload = 50;

				f = AngleFrom(me->x, me->y, goodguy->x, goodguy->y);
				for (i = 0; i < 4; i++)
				{
					b = FireBullet(me->ID, me->x, me->y, FIXAMT * 30, (f - 20 + (rand() % 41)) & 255, BLT_WOODSHARD, DamageAmt(monsType[me->type].damage, me->team));
					if (b)
						b->speed = FIXAMT * (6 + (rand() % 6));
				}
			}
			else if (me->type == MONS_ROSE5)	// doom daisy
			{
				me->reload = 30;

				f = AngleFrom(me->x, me->y, goodguy->x, goodguy->y);
				b = FireBullet(me->ID, me->x, me->y, FIXAMT * 30, (f - 20 + (rand() % 41)) & 255, BLT_HOMER, DamageAmt(monsType[me->type].damage, me->team));
			}
		}
		if (me->seq == ANIM_MOVE && me->frm == 8)
		{
			int x, y, tries;

			tries = 0;
			while ((tries++) < 200)
			{
				x = goodguy->x - FIXAMT * 100 + (rand() % (FIXAMT * 200 + 1));
				y = goodguy->y - FIXAMT * 100 + (rand() % (FIXAMT * 200 + 1));
				if (me->CanWalkNoMons(x, y, map, world))
				{
					me->x = x;
					me->y = y;
					me->mapx = x / (TILE_WIDTH * FIXAMT);
					me->mapy = y / (TILE_HEIGHT * FIXAMT);
					break;
				}
			}
			DoMove(me, ANIM_A2, 128, 0, 0, 0);
		}
		return;
	}

	if (me->mind == 0)
	{
		if (me->seq != ANIM_A2)
			DoMove(me, ANIM_A2, 128, 0, 0, 0);
		else
		{
			me->frm = 0;
			me->frmTimer = 0;
		}
		if (RangeToTarget(me, goodguy) < FIXAMT * 300 || me->ouch)
		{
			DoMove(me, ANIM_A1, 128, 1, 0, 0);
			MakeSound(SND_PLANTSURFACE, me->x, me->y, SND_CUTOFF | SND_RANDOM, 500);
			me->mind = 1;	// goodguy spotted
		}
	}
	else
	{
		if (me->reload == 0 && RangeToTarget(me, goodguy) < FIXAMT * 300)
		{
			MakeSound(SND_PLANTSHOOT, me->x, me->y, SND_CUTOFF | SND_RANDOM, 500);
			DoMove(me, ANIM_ATTACK, 128, 1, 0, 0);
		}
	}
	if ((me->type == MONS_ROSE3 || me->type == MONS_ROSE4 || me->type == MONS_ROSE5) && !map->CheckLOS(me->mapx, me->mapy, 15, goodguy->mapx, goodguy->mapy))
	{
		MakeSound(SND_PLANTBURY, me->x, me->y, SND_CUTOFF | SND_RANDOM, 500);
		DoMove(me, ANIM_MOVE, 128, 1, 0, 0);
		me->mind = 0;
	}
}

void AI_Hound(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	BasicAI(me, SND_HOUNDOUCH, SND_HOUNDDIE, map, world, goodguy);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frm == 4)
			me->frmAdvance = 256;
		if (me->seq == ANIM_A2 && me->frm == 4)
		{
			// begin charging
			if (me->type == MONS_HOUND)
				DoMove(me, ANIM_A1, 256, 1, Cosine(me->facing * 32) * 7, Sine(me->facing * 32) * 7);
			else
				DoMove(me, ANIM_A1, 256, 1, Cosine(me->facing * 32) * 10, Sine(me->facing * 32) * 10);
			MakeSound(SND_HOUNDCHARGE, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
		}
		else if (me->seq == ANIM_A1)
		{
			int i;

			x = me->x + Cosine(me->facing * 32) * 40;
			y = me->y + Sine(me->facing * 32) * 40;
			i = FindVictim(x, y, me->mapx, me->mapy, 20, me->ID + 1, (GOOD + EVIL) - me->team, 0, map, world);
			if (i)
			{
				byte ang;
				ang = AngleFrom(me->x, me->y, GetGuy((word)(i - 1))->x, GetGuy((word)(i - 1))->y);
				meleeAttacker = me;
				GetGuy((word)(i - 1))->GetShot(Cosine(ang) * 16, Sine(ang) * 16, DamageAmt(monsType[me->type].damage, me->team), map, world);
			}
		}
		if (me->seq == ANIM_ATTACK && me->frm >= 2 && me->frm <= 5)
		{
			byte f;

			f = me->facing * 32 - 10 + (rand() % 21);
			x = me->x + Cosine(me->facing * 32) * 64;
			y = me->y + Sine(me->facing * 32) * 64;
			FireBullet(me->ID, x, y, FIXAMT * 30, f, BLT_CRYO, DamageAmt(monsType[me->type].damage / 2 + 1, me->team));
			me->reload = 60;
		}

		return;	// can't do nothin' right now
	}

	// charge at medium range
	if (Random(10) == 0 && map->CheckLOS(me->mapx, me->mapy, 10, goodguy->mapx, goodguy->mapy) && me->reload == 0)
	{
		FaceGoodguy(me, goodguy);
		DoMove(me, ANIM_A2, 64, 1, 0, 0);
		return;
	}
	// spit ice barrage sometimes at about the same range
	if (Random(10) == 0 && map->CheckLOS(me->mapx, me->mapy, 10, goodguy->mapx, goodguy->mapy) && me->reload == 0 && me->type == MONS_HOUND)
	{
		FaceGoodguy(me, goodguy);
		DoMove(me, ANIM_ATTACK, 64, 1, 0, 0);
		MakeSound(SND_HOUNDSPIT, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
		return;
	}

	WanderAI(me, 64, 40, 3, map, world, goodguy);
}

void AI_Toymaker(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		if (me->hp > 0)
			MakeSound(SND_MOUSEOUCH, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frm == 0)
		{
			me->frmAdvance = 256;
			if (goodguy)
			{
				FireBullet(goodguy->ID, me->x, me->y, 0, 0, BLT_KABOOM, DamageAmt(10, GOOD));
				MakeSound(SND_BOOM, me->x, me->y, SND_CUTOFF | SND_RANDOM, 800);
			}
		}
		if (me->seq == ANIM_ATTACK && me->frm == 10 && me->reload == 0)
		{
			Guy* g;
			byte t;
			byte monsChart[8] = { MONS_TEDDY,MONS_MOUSE,MONS_PUPPET,MONS_TINGUY,MONS_MOUSE2,MONS_TEDDY2,MONS_MONKEY,MONS_PUPPET3 };
			t = monsChart[(rand() % 8)];

			g = AddGuy(me->x, me->y + 48 * FIXAMT, 0, t);
			if (g)
			{
				g->bright = 31;
				map->map[g->mapx + g->mapy * map->width].templight = 31;
				FXRing(4, g->x, g->y, 0, 16, 5);
				g->tag = 255;
			}
			me->reload = 90;
		}

		return;	// can't do nothin' right now
	}

	if (me->mind == 0)
	{
		me->mind = 1;
		me->reload = rand() % 90;
	}
	if (me->reload == 0)
	{
		DoMove(me, ANIM_ATTACK, 128, 1, 0, 0);
		MakeSound(SND_PUPPETHAT, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
	}
}

void AI_Toypower(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int i;

	BasicAI(me, SND_MOUSEOUCH, SND_POWERDIE, map, world, goodguy);

	if (me->hp == 0 && me->ouch == 4)
		KillKids(me);

	me->mind1++;
	i = GuyList_First();
	while (i != 65535)
	{
		if (GetGuy(i)->type == MONS_TOYCRYSTAL)
		{
			GetGuy(i)->mind = GetGuy(i)->mind1 * (256 / 6) + me->mind1;
		}
		i = GuyList_Next();
	}
	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE)
		{
			me->frmAdvance = 256;
			if (goodguy && (rand() % 8))
			{
				FireBullet(goodguy->ID, me->x - FIXAMT * 40 + (rand() % (FIXAMT * 81)), me->y - FIXAMT * 40 + (rand() % (FIXAMT * 81)), 0, 0, BLT_KABOOM, DamageAmt(10, GOOD));
				MakeSound(SND_BOOM, me->x, me->y, SND_CUTOFF | SND_RANDOM, 800);
			}
		}
		return;	// can't do nothin' right now
	}

	if (me->mind == 0)
	{
		me->mind = 1;
		me->reload = rand() % 60;
	}
	if (me->reload == 0)
	{
		FireBullet(me->ID, me->x, me->y, FIXAMT * 100, (byte)rand() % 256, BLT_GRENADE, DamageAmt(monsType[me->type].damage, me->team));
		MakeSound(SND_POWERLOB, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
		me->reload = 30;
	}
}

void AI_Toycrystal(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->parent)
	{
		me->x = me->parent->x + Cosine(me->mind) * 200;
		me->y = me->parent->y + Sine(me->mind) * 160;
	}

	BasicAI(me, SND_CRYSTALOUCH, SND_CRYSTALBREAK, map, world, goodguy);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE)
		{
			me->frmAdvance = 128;
		}
		return;	// can't do nothin' right now
	}

	if (me->parent == NULL)
		return;

	if (me->reload == 0)
	{
		x = me->mapx - 10 + (rand() % 21);
		y = me->mapy - 10 + (rand() % 21);

		while (x < 0 || y < 0 || x >= map->width || y >= map->height || map->map[x + y * map->width].wall)
		{
			x = me->mapx - 10 + (rand() % 21);
			y = me->mapy - 10 + (rand() % 21);
		}
		x = (x * TILE_WIDTH + TILE_WIDTH / 2) * FIXAMT;
		y = (y * TILE_HEIGHT + TILE_HEIGHT / 2) * FIXAMT;
		FireBullet(me->ID, x, y, 0, 0, BLT_CRYSTALSHOCK, DamageAmt(monsType[me->type].damage, me->team));
		MakeSound(SND_CRYSTALZAP, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
		me->reload = 45;
	}
}

void AI_Lifeberry(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	if (SpecificSkillLevel(SKILL_LIFEBERRY) == 0 && me->hp > 0)
	{
		me->hp = 0;
		DoMove(me, ANIM_DIE, 128, 1, 0, 0);
	}
	if (me->reload)
		me->reload--;

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK && me->frm == 2 && me->reload == 0)
		{
			me->reload = 2;
			FireBullet(me->ID, me->x, me->y, FIXAMT * 15, (byte)rand() % 256, BLT_BERRY, (word)(SpecificSkillVal(0, SKILL_LIFEBERRY) * SpellDamageBoost(SC_NATURE) / 100.0f));
		}
		return;	// can't do nothin' right now
	}

	if (me->mind)
	{
		if (me->mind1)
			me->mind1--;
		else
		{
			me->mind1 = 30;
			me->mind--;
		}
	}
	else
	{
		me->hp = 0;
		DoMove(me, ANIM_DIE, 128, 1, 0, 0);
	}

	if (me->reload == 0 && (rand() % 60) == 0)
	{
		DoMove(me, ANIM_ATTACK, 128, 1, 0, 0);
		MakeSound(SND_BERRYSPIT, me->x, me->y, SND_CUTOFF | SND_RANDOM, 500);
	}
}

void AI_Lion(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x;

	BasicAI(me, SND_PLANTOUCH, SND_PLANTDIE, map, world, goodguy);

	if (me->type == MONS_BURNINGBUSH && (rand() % 5) == 0)
		FireBullet(me->ID, me->x, me->y, 0, 0, BLT_FLAMEB, FlameDmg(monsType[me->type].damage, me->team));

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frm == 4)
		{
			ExplodeParticles2(PART_SNOW2, me->x, me->y, me->z, 40, 6);
			me->frmAdvance = 256;
		}
		if (me->seq == ANIM_ATTACK && me->frm == 1)
		{
			if (me->type == MONS_LION2 && me->reload == 0)
			{
				for (x = 0; x < 3; x++)
					FireBullet(me->ID, me->x, me->y, FIXAMT * 30, ((me->facing * 32 - 16) + x * 16) & 255, BLT_WOODSHARD, DamageAmt(monsType[me->type].damage, me->team));
				me->reload = 40;
			}
			else
			{
				for (x = 0; x < 256; x += (16 + 16 * (me->type == MONS_BURNINGBUSH)))
				{
					if (me->type == MONS_BURNINGBUSH)
						FireBullet(me->ID, me->x, me->y, FIXAMT * 30, (byte)x, BLT_FLAMETRAIL, DamageAmt(monsType[me->type].damage, me->team));
					else if (me->type == MONS_LION)
						FireBullet(me->ID, me->x, me->y, FIXAMT * 30, (byte)x, BLT_SPORE, DamageAmt(monsType[me->type].damage, me->team));
				}
				me->reload = 50;
			}
		}

		return;	// can't do nothin' right now
	}

	// fire spores at medium range
	if (Random(20) == 0 && map->CheckLOS(me->mapx, me->mapy, 10, goodguy->mapx, goodguy->mapy) && me->reload == 0)
	{
		DoMove(me, ANIM_ATTACK, 64, 1, 0, 0);
		return;
	}

	WanderAI(me, 64, 40, 3, map, world, goodguy);
}

void AI_Ghost(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;
	byte rightAng;

	if (me->mind3 > 0)
	{
		me->mind3--;
	}
	else if (me->mind2 > 0)
	{
		me->mind2--;
		me->mind3 = 30;
	}
	else
	{
		if (me->seq != ANIM_DIE)
		{
			DoMove(me, ANIM_DIE, 128, 1, 0, 0);
			ParticleBoom(PART_SMOKESINGLE, me->x, me->y, me->z);
			MakeSound(SND_GHOSTDIE, me->x, me->y, SND_CUTOFF | SND_RANDOM, 200);
		}
	}

	if (me->reload)
		me->reload--;

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK && me->frm == 2 && me->reload == 0)
		{
			me->reload = 60;
			if (goodguy)
			{
				x = me->x + Cosine(me->facing * 32) * 12;
				y = me->y + Sine(me->facing * 32) * 12;
				if (me->AttackCheck(16, x >> FIXSHIFT, y >> FIXSHIFT, goodguy))
				{
					meleeAttacker = me;

					goodguy->GetShot(Cosine(me->facing * 32) * 1, Sine(me->facing * 32) * 1, DamageAmt(monsType[me->type].damage, me->team), map, world);
				}
			}
		}

		return;	// can't do nothin' right now
	}

	if (goodguy)
	{
		if (me->reload == 0 && map->CheckLOS(me->mapx, me->mapy, 2, goodguy->mapx, goodguy->mapy) && RangeToTarget(me, goodguy) < FIXAMT * 20)
		{
			DoMove(me, ANIM_ATTACK, 128, 1, 0, 0);
			return;
		}

		rightAng = AngleFrom(me->x, me->y, goodguy->x, goodguy->y);
		me->mind = TurnToward(me->mind, rightAng, 6);

		me->facing = ((me->mind + 16) / 32) & 7;
		me->dx = Cosine(me->mind) * monsType[me->type].speed / FIXAMT;
		me->dy = Sine(me->mind) * monsType[me->type].speed / FIXAMT;
	}
	else	// spin to a random facing
	{
		rightAng = me->mind1;
		me->mind = TurnToward(me->mind, rightAng, 6);
		if (me->mind == me->mind1)
			me->mind1 = (rand() % 256);

		me->facing = ((me->mind + 16) / 32) & 7;
		me->dx = Cosine(me->mind) * monsType[me->type].speed / FIXAMT;
		me->dy = Sine(me->mind) * monsType[me->type].speed / FIXAMT;
	}
}

void AI_Thug(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y, i;

	if (me->team == GOOD && me->mindControl == 0 && player.levelNum != LVL_CLEARING && player.levelNum != LVL_GEYSER && !(levelDef[player.levelNum].flags & LF_ARENA))
	{
		// be like a villager if we are friends
		AI_SemiVillager(me, map, world, goodguy);
		return;
	}

	if (goodguy == NULL)
	{
		me->dx = 0;
		me->dy = 0;
		return;
	}

	BasicAI(me, SND_THUGOUCH, SND_THUGDIE, map, world, goodguy);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frm == 3)
			me->frmAdvance = 256;
		if (me->seq == ANIM_ATTACK && me->frm == 4 && me->reload == 0)
		{
			x = me->x + Cosine(me->facing * 32) * 30;
			y = me->y + Sine(me->facing * 32) * 30;

			i = 0;
			while (1)
			{
				// smash everyone in range
				i = FindVictim(x, y, me->mapx, me->mapy, 30, 0, (GOOD + EVIL) - me->team, i, map, world);
				if (i)
				{
					byte ang;
					ang = AngleFrom(me->x, me->y, GetGuy((word)(i - 1))->x, GetGuy((word)(i - 1))->y);
					meleeAttacker = me;
					if (GetGuy((word)(i - 1))->GetShot(Cosine(ang) * 10, Sine(ang) * 10, DamageAmt(monsType[me->type].damage, me->team), map, world))
						GetGuy((word)(i - 1))->GetStunned(30);
				}
				else break;
			}
			me->reload = 40;
		}

		return;	// can't do nothin' right now
	}

	// smash when you get close
	if (Random(10) == 0 && map->CheckLOS(me->mapx, me->mapy, 3, goodguy->mapx, goodguy->mapy) && me->reload == 0)
	{
		MakeSound(SND_THUGSMACK, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
		FaceGoodguy(me, goodguy);
		DoMove(me, ANIM_ATTACK, 128, 1, 0, 0);
		return;
	}
	WanderAI(me, 64, 40, 3, map, world, goodguy);
}

void AI_Junksmith(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;
	Guy* g;

	if (me->team == GOOD && player.levelNum != LVL_CLEARING && me->mindControl == 0)
	{
		// be like a villager if we are friends
		AI_SemiVillager(me, map, world, goodguy);
		return;
	}

	if (goodguy == NULL)
	{
		me->dx = 0;
		me->dy = 0;
		return;
	}

	BasicAI(me, SND_THUGOUCH, SND_THUGDIE, map, world, goodguy);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frm == 9)
			me->frmAdvance = 256;
		if ((me->seq == ANIM_ATTACK || me->seq == ANIM_DIE) && me->frm == 2 && me->reload == 0)
		{
			MakeSound(SND_PLANTSHOOT, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
			x = me->x + Cosine(me->facing * 32) * 16;
			y = me->y + Sine(me->facing * 32) * 16;

			g = AddGuy(x, y, FIXAMT * 40, MONS_JUNKTRAP);
			if (g)
			{
				g->dx = Cosine(me->facing * 32) * 8;
				g->dy = Sine(me->facing * 32) * 8;
				g->dz = FIXAMT * 8;
			}
			me->reload = 90;
		}

		return;	// can't do nothin' right now
	}

	// fire supergun when reloaded
	if (Random(10) == 0 && me->reload == 0)
	{
		MakeSound(SND_JUNKCOCK, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
		FaceGoodguy(me, goodguy);
		DoMove(me, ANIM_ATTACK, 128, 1, 0, 0);
		return;
	}
	WanderAI(me, 64, 40, 3, map, world, goodguy);
}

void AI_Caravan(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y, i;

	BasicAI(me, SND_PUPPETOUCH, SND_BOOM, map, world, goodguy);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE)
		{
			if (me->frm < 7)
			{
				me->dx = -(7 - me->frm) * FIXAMT / 2;
				me->dy = -(7 - me->frm) * FIXAMT;
			}
			else
			{
				me->dx = 0;
				me->dy = 0;
			}
			if (me->frm == 6)
				me->frmAdvance = 256;
		}

		return;	// can't do nothin' right now
	}

	me->mind++;
	if (me->mind == 8)
	{
		me->mind = 0;
		Smoke(me->x - FIXAMT * 15, me->y + FIXAMT * 15, FIXAMT * 60);
	}
	// smash anyone who touches you
	x = me->x + Cosine(me->facing * 32) * 16;
	y = me->y + Sine(me->facing * 32) * 16;
	i = FindVictim(x, y, me->mapx, me->mapy, 40, me->ID + 1, 3, 0, map, world);
	while (i)
	{
		byte ang;
		ang = AngleFrom(me->x, me->y, GetGuy((word)(i - 1))->x, GetGuy((word)(i - 1))->y);
		if (GetGuy((word)(i - 1))->type != MONS_CARAVAN)
		{
			meleeAttacker = me;
			GetGuy((word)(i - 1))->GetShot(Cosine(ang) * 48, Sine(ang) * 48, DamageAmt(monsType[me->type].damage, me->team), map, world);
		}
		i = FindVictim(x, y, me->mapx, me->mapy, 40, me->ID + 1, 3, i, map, world);
	}
	me->dx = 0;
	me->dy = -monsType[me->type].speed;

	if (me->y < -FIXAMT * 60)
		me->type = MONS_NONE;	// vanish into the ether
}

void AI_BunnyTroop(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y, i;

	if (me->team == GOOD && me->mindControl == 0 && player.levelNum != LVL_CLEARING && player.levelNum != LVL_GEYSER && player.levelNum != LVL_HIGHWAY && !(levelDef[player.levelNum].flags & LF_ARENA))
	{
		// be like a villager if we are friends
		AI_SemiVillager(me, map, world, goodguy);
		return;
	}

	if (goodguy == NULL)
	{
		me->dx = 0;
		me->dy = 0;
		return;
	}

	BasicAI(me, SND_THUGOUCH, SND_THUGDIE, map, world, goodguy);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frm == 3)
			me->frmAdvance = 256;
		if (me->seq == ANIM_ATTACK && me->frm == 4 && me->reload == 0)
		{
			x = me->x + Cosine(me->facing * 32) * 30;
			y = me->y + Sine(me->facing * 32) * 30;

			i = 0;
			while (1)
			{
				// slash everyone in range
				i = FindVictim(x, y, me->mapx, me->mapy, 30, 0, (GOOD + EVIL) - me->team, i, map, world);
				if (i)
				{
					byte ang;
					ang = AngleFrom(me->x, me->y, GetGuy((word)(i - 1))->x, GetGuy((word)(i - 1))->y);
					meleeAttacker = me;
					GetGuy((word)(i - 1))->GetShot(Cosine(ang) * 40, Sine(ang) * 40, DamageAmt(monsType[me->type].damage, me->team), map, world);
				}
				else break;
			}
			me->reload = 40;
		}

		return;	// can't do nothin' right now
	}

	// slash when you get close
	if (map->CheckLOS(me->mapx, me->mapy, 3, goodguy->mapx, goodguy->mapy) && me->reload == 0)
	{
		MakeSound(SND_THUGSMACK, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
		FaceGoodguy(me, goodguy);
		DoMove(me, ANIM_ATTACK, 128, 1, 0, 0);
		return;
	}
	WanderAI(me, 64, 40, 3, map, world, goodguy);
}

void AI_BunnyGuy(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y, i;

	if (me->team == GOOD && me->mindControl == 0 && player.levelNum != LVL_CLEARING && player.var[VAR_SNUGGLY] == 1)
	{
		// be like a villager if we are friends
		AI_SemiVillager(me, map, world, goodguy);
		return;
	}
	if (player.levelNum == LVL_CAPTAIN && player.var[VAR_TEMP] == 1)
	{
		me->dx = 0;
		me->dy = 0;
		DoMove(me, ANIM_IDLE, 128, 0, 0, 0);
		me->dz += FIXAMT / 4;
		if (me->z == 0)
		{
			player.var[VAR_TEMP] = 2;
			player.chatClock = 0;
			BeginChatting(31);
		}
		return;
	}

	if (player.var[VAR_TEMP] > 100)
	{
		player.var[VAR_TEMP]--;
	}
	if (player.var[VAR_TEMP] == 100)
	{
		player.chatClock = 0;
		BeginChatting(31);
		player.var[VAR_TEMP] = 0;
	}
	if (goodguy == NULL)
	{
		me->dx = 0;
		me->dy = 0;
		return;
	}

	if (me->hp < MonsterHP(me->type, me->team))
	{
		me->hp = MonsterHP(me->type, me->team);
	}

	BasicAI(me, SND_THUGOUCH, SND_THUGDIE, map, world, goodguy);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frm == 3)
			me->frmAdvance = 256;
		if (me->seq == ANIM_ATTACK && me->frm == 4 && me->reload == 0)
		{
			x = me->x + Cosine(me->facing * 32) * 30;
			y = me->y + Sine(me->facing * 32) * 30;

			i = 0;
			while (1)
			{
				// slash everyone in range
				i = FindVictim(x, y, me->mapx, me->mapy, 30, 0, (GOOD + EVIL) - me->team, i, map, world);
				if (i)
				{
					byte ang;
					ang = AngleFrom(me->x, me->y, GetGuy((word)(i - 1))->x, GetGuy((word)(i - 1))->y);
					meleeAttacker = me;
					GetGuy((word)(i - 1))->GetShot(Cosine(ang) * 30, Sine(ang) * 30, DamageAmt(monsType[me->type].damage, me->team), map, world);
				}
				else break;
			}
			me->reload = 40;
		}

		return;	// can't do nothin' right now
	}

	// slash when you get close
	if (Random(10) == 0 && map->CheckLOS(me->mapx, me->mapy, 3, goodguy->mapx, goodguy->mapy) && me->reload == 0)
	{
		MakeSound(SND_THUGSMACK, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
		FaceGoodguy(me, goodguy);
		DoMove(me, ANIM_ATTACK, 128, 1, 0, 0);
		return;
	}
	WanderAI(me, 64, 40, 3, map, world, goodguy);
	if (me->seq == ANIM_IDLE)
		DoMove(me, ANIM_MOVE, 128, 0, me->dx, me->dy);
}

void AI_BunnyBoss(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int i;

	// quest-related nonsense............................
	if (player.levelNum == LVL_ONIONCAMP && player.var[VAR_QUESTASSIGN + QUEST_LEADER] && !player.var[VAR_QUESTASSIGN + QUEST_FURNACE])
	{
		// he does the spin on down
		if (me->z > 0)
		{
			if (me->seq != ANIM_ATTACK)
				DoMove(me, ANIM_ATTACK, 128, 1, 0, 0);
			else if (me->frm > 4)
			{
				me->frm = 4;
				me->facing = (me->facing + 1) & 7;
			}
			me->mind = 30;
		}
		if (me->z == 0)
		{
			me->mind--;
			me->facing = 2;
			if (me->mind == 29)
			{
				FireBullet(me->ID, me->x, me->y, 0, 0, BLT_KABOOM, 0);
				MakeNormalSound(SND_BOOM);
			}
			if (me->mind == 22)
			{
				word w;
				w = GuyList_First();
				while (w != 65535)
				{
					if (GetGuy(w)->type == MONS_VILLAGER && GetGuy(w)->tag == 24)
					{
						GetGuy(w)->type = MONS_NONE;
						break;
					}
					else
						w = GuyList_Next();
				}
				PlayerSetVar(VAR_QUESTDONE + QUEST_LEADER, 1);
			}
			if (me->mind == 0)
			{
				player.chatClock = 0;
				BeginChatting(30);
			}
		}
		return;
	}
	if (me->team == GOOD && me->mindControl == 0 && player.levelNum != LVL_CLEARING)
	{
		// be like a villager if we are friends
		AI_SemiVillager(me, map, world, goodguy);
		return;
	}
	if (player.var[VAR_TEMP] > 150)
	{
		ParticleBoom(PART_SMOKE, me->x, me->y, FIXAMT * 4);
		// AA doesn't give tornado
		me->type = MONS_NONE;
		return;
	}
	if (me->hp < MonsterHP(me->type, me->RealTeam()) / 2 && player.var[VAR_QUESTDONE + QUEST_LEADER] == 0)	// half dead	// AA hp madcap fix
	{
		player.chatClock = 0;
		BeginChatting(30);	// you'll never get me!
		player.var[VAR_TEMP] = 200;
	}
	if (player.var[VAR_SNUGGLY] == 0 && player.var[VAR_QUESTASSIGN + QUEST_LEADER] && !player.var[VAR_QUESTDONE + QUEST_LEADER] &&
		player.var[VAR_TEMP] == 0 && (map->CheckLOS(me->mapx, me->mapy, 6, goodguy->mapx, goodguy->mapy) || me->ouch))
	{
		Guy* g;

		player.chatClock = 0;
		BeginChatting(30);
		g = AddGuy(me->x, me->y, FIXAMT * 500, MONS_BUNNYMAJOR);
		if (g)
		{
			g->team = GOOD;
			g->tag = 31;	// tag the major for chatting
		}
	}
	// end quest-related nonsense!!!!!!!!!!!!!!

	if (goodguy == NULL)
	{
		me->dx = 0;
		me->dy = 0;
		return;
	}

	BasicAI(me, SND_THUGOUCH, SND_THUGDIE, map, world, goodguy);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frm == 3)
			me->frmAdvance = 256;
		if (me->seq == ANIM_ATTACK)
		{
			if (me->frm == 2 && me->frmTimer < 64)
				FireBullet(me->ID, me->x, me->y, 0, 0, BLT_TORNADO2, 0);
			if (me->frm >= 1 && me->frm <= 13)
			{
				me->dx += Cosine(me->mind2) / 2;
				me->dy += Sine(me->mind2) / 2;
				Clamp(&me->dx, FIXAMT * 7);
				Clamp(&me->dy, FIXAMT * 7);
				if (me->wallBump & 1)
				{
					// flip horiz
					if (me->mind2 < 128)
						me->mind2 = 128 - me->mind2;
					else
						me->mind2 = 128 + (256 - me->mind2);
					me->dx = -me->dx;
				}
				if (me->wallBump & 2)
				{
					// flip vert
					me->mind2 = 256 - me->mind2;
					me->dy = -me->dy;
				}
				me->wallBump = 0;
				me->facing = (me->facing + 1) & 7;	// spin!

				if (me->reload == 0)
				{
					i = 0;
					while (1)
					{
						// slash everyone in range
						i = FindVictim(me->x, me->y, me->mapx, me->mapy, 80, 0, (GOOD + EVIL) - me->team, i, map, world);
						if (i)
						{
							byte ang;
							ang = AngleFrom(me->x, me->y, GetGuy((word)(i - 1))->x, GetGuy((word)(i - 1))->y);
							meleeAttacker = me;
							GetGuy((word)(i - 1))->GetShot(Cosine(ang) * 40, Sine(ang) * 40, DamageAmt(monsType[me->type].damage, me->team), map, world);
						}
						else break;
					}
					me->reload = 4;
				}
			}
		}
		if (me->seq == ANIM_A2 && me->frm == 2 && me->reload == 0)
		{
			me->reload = 10;
			for (i = 0; i < 12; i++)
				FireBullet(me->ID, me->x, me->y, FIXAMT * 20, me->facing * 32 - (6 * 10) + 5 + (i * 10), BLT_FIREBALL, DamageAmt(monsType[me->type].damage, me->team));
		}

		return;	// can't do nothin' right now
	}

	// game plan: wander around for 3 seconds (if hit during this phase, 10% chance to go into blocking)
	// when done wandering, do a tornado dash
	if (me->mind == 0)	// not yet decided to do anything
	{
		me->mind = 1;
		me->mind1 = 90;	// 3 seconds
		me->facing = rand() % 8;
		DoMove(me, ANIM_MOVE, 128, 0, Cosine(me->facing * 32) * monsType[me->type].speed, Sine(me->facing * 32) * monsType[me->type].speed);
	}
	else if (me->mind == 1)	// wandering
	{
		if (me->wallBump)
		{
			me->facing *= 32;
			if (me->wallBump & 1)
			{
				// flip horiz
				if (me->facing < 128)
					me->facing = 128 - me->facing;
				else
					me->facing = 128 + (256 - me->facing);
			}
			if (me->wallBump & 2)
			{
				// flip vert
				me->facing = 256 - me->facing;
			}
			me->facing /= 32;
			DoMove(me, ANIM_MOVE, 128, 0, Cosine(me->facing * 32) * monsType[me->type].speed / FIXAMT, Sine(me->facing * 32) * monsType[me->type].speed / FIXAMT);
			me->wallBump = 0;
		}
		if ((rand() % 40) == 0)
		{
			me->facing = rand() % 8;
			DoMove(me, ANIM_MOVE, 128, 0, Cosine(me->facing * 32) * monsType[me->type].speed / FIXAMT, Sine(me->facing * 32) * monsType[me->type].speed / FIXAMT);
		}
		if (me->seq != ANIM_MOVE)
			DoMove(me, ANIM_MOVE, 128, 0, Cosine(me->facing * 32) * monsType[me->type].speed / FIXAMT, Sine(me->facing * 32) * monsType[me->type].speed / FIXAMT);
		me->mind1--;
		if (!me->mind1)
		{
			MakeSound(SND_TORNADO, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
			DoMove(me, ANIM_ATTACK, 64, 1, 0, 0);
			me->mind2 = me->facing * 32;
			me->mind1 = 90;
			return;
		}
		if (me->ouch && (rand() % 4) == 0)
		{
			// 25% chance when hit to do block
			DoMove(me, ANIM_A1, 128, 1, 0, 0);
			if (goodguy)
				FaceGoodguy(me, goodguy);
			return;
		}
	}
}

void AI_ClockBotEvil(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y, i;

	if (goodguy == NULL)
	{
		me->dx = 0;
		me->dy = 0;
		return;
	}

	BasicAI(me, SND_PUPPETOUCH, SND_PUPPETDIE, map, world, goodguy);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frm == 3)
			me->frmAdvance = 256;
		if (me->seq == ANIM_ATTACK && me->frm == 4 && me->reload == 0)
		{
			x = me->x + Cosine(me->facing * 32) * 30;
			y = me->y + Sine(me->facing * 32) * 30;

			i = 0;
			while (1)
			{
				// slash everyone in range
				i = FindVictim(x, y, me->mapx, me->mapy, 30, 0, (GOOD + EVIL) - me->team, i, map, world);
				if (i)
				{
					byte ang;
					ang = AngleFrom(me->x, me->y, GetGuy((word)(i - 1))->x, GetGuy((word)(i - 1))->y);
					meleeAttacker = me;
					GetGuy((word)(i - 1))->GetShot(Cosine(ang) * 40, Sine(ang) * 40, DamageAmt(monsType[me->type].damage, me->team), map, world);
				}
				else break;
			}
			me->reload = 30;
		}

		return;	// can't do nothin' right now
	}

	// slash when you get close
	if (map->CheckLOS(me->mapx, me->mapy, 3, goodguy->mapx, goodguy->mapy) && me->reload == 0)
	{
		MakeSound(SND_THUGSMACK, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
		FaceGoodguy(me, goodguy);
		DoMove(me, ANIM_ATTACK, 128, 1, 0, 0);
		return;
	}
	WanderAI(me, 64, 40, 3, map, world, goodguy);
}

void ClockPursue(Guy* me, int tgtx, int tgty)
{
	byte rightAng;

	if (tgtx == 0 && tgty == 0)
	{
		tgtx = me->x - 500 * FIXAMT + rand() % (1000 * FIXAMT + 1);
		tgty = me->y - 500 * FIXAMT + rand() % (1000 * FIXAMT + 1);
	}

	if (me->mind1)
	{
		me->mind1--;
	}
	else
	{
		switch (rand() % 8)
		{
		case 0:
			me->mind = 1;
			break;
		case 1:
			me->mind = 2;
			break;
		case 2:
			me->mind = 3;
			break;
		default:
			me->mind = 0;
			break;
		}
		me->mind1 = 1 + (rand() % 20);
	}
	me->mind = 0;

	if (me->mind == 0)	// heading straight at guy
	{
		rightAng = AngleFrom(me->x, me->y, tgtx, tgty);
	}
	else if (me->mind == 1)	// turning right
	{
		rightAng = me->mind3 + 10;
	}
	else if (me->mind == 2)	// turning left
	{
		rightAng = me->mind3 - 10;
	}
	else if (me->mind == 3)	// going straight
	{
		rightAng = me->mind3;
	}

	me->mind3 = TurnToward(me->mind3, rightAng, 6);

	me->facing = ((me->mind3 + 16) / 32) & 7;
	me->dx = Cosine(me->mind3) * monsType[me->type].speed / FIXAMT;
	me->dy = Sine(me->mind3) * monsType[me->type].speed / FIXAMT;

	if (me->seq != ANIM_MOVE)
		DoMove(me, ANIM_MOVE, 128, 0, me->dx, me->dy);
}

void AI_ClockBot(Guy* me, Map* map, world_t* world, Guy* badguy)
{
	int x, y, range;
	bullet_t* b;

	if ((!goodguy || goodguy->hp == 0) && me->hp)
	{
		me->dx = 0;
		me->dy = 0;
		// do nothing
		return;
	}

	BasicAI(me, SND_PUPPETOUCH, SND_PUPPETDIE, map, world, badguy);

	if (me->hp)
	{
		if (player.var[VAR_CLOCKBOT - IT_LOG + IT_SEED])
		{
			// regenerate
			if ((player.timeToFinish % (30 * 10)) == 0)
			{
				if (me->hp < MonsterHP(me->type, me->team))
				{
					x = me->hp + ((player.var[VAR_CLOCKBOT - IT_LOG + IT_SEED] * (100 + player.var[VAR_MADGEM + MC_HEALTH] * 10)) / 100);
					if (x > MonsterHP(me->type, me->team))
						x = MonsterHP(me->type, me->team);
					NumberParticle(x - me->hp, me->x, me->y + 1, 1);
					me->hp = x;
					player.var[VAR_CLOCKLIFE] = me->hp & 0xFF;
					player.var[VAR_CLOCKLIFE2] = me->hp >> 8;
				}
			}
		}
		if (player.var[VAR_CLOCKBOT - IT_LOG + IT_EMERALD])
		{
			// shock nearby guys
			// AA freq redo
			y = player.var[VAR_CLOCKBOT - IT_LOG + IT_EMERALD];
			if (y > 10)
				x = 10;
			else
			{
				y = 12 - y;
				x = 9 + (y * (y + 1) / 2);
			}
			x = CalcShockRate((byte)x);
			if ((player.timeToFinish % x) == 0)
			{
				x = BulletFindTargetClosest(me->team, me->x, me->y, 0, 90 * FIXAMT, curMap, world);
				LightningBolt(me->x, me->y - me->z - FIXAMT * 20, me->x - 10 * FIXAMT + (rand() % (20 * FIXAMT + 1)), me->y - me->z - FIXAMT * 10 - 20 * FIXAMT + (rand() % (40 * FIXAMT + 1)));
				if (x != 0)
				{
					meleeAttacker = NULL;
					bulletHittingType = 0;
					bulletHittingOwner = 65535;	// AA bear fix
					GetGuy(x - 1)->GetShotReal(0, 0, (1 + 2 * (ModifierOn(MOD_TICKING))) * ShockDmg(1, me->team), curMap, world);
					TalentPoint(TLT_ELECTROCUTION, ShockDmg(1, me->team));	// AA fix
					LightningBolt(me->x, me->y - me->z, GetGuy(x - 1)->x, GetGuy(x - 1)->y - FIXAMT * 20);
				}
			}
		}
	}
	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frm == 5)
			me->frmAdvance = 256;
		if (me->seq == ANIM_ATTACK && me->frm == 4 && me->reload == 0)
		{
			if (player.var[VAR_CLOCKBOT - IT_LOG + IT_RUBY])
			{
				int ang, i;

				ang = -(player.var[VAR_CLOCKBOT - IT_LOG + IT_RUBY] * 6);
				for (i = 0; i < player.var[VAR_CLOCKBOT - IT_LOG + IT_RUBY]; i++)
				{
					bullet_t* b;

					b = FireBullet(me->ID, me->x + Cosine(me->facing * 32) * 20, me->y + Sine(me->facing * 32) * 20, FIXAMT * 20, (me->facing * 32 + 256 + ang), BLT_CLOCKBULLET, (1 + 2 * (ModifierOn(MOD_TICKING))) * DamageAmt(monsType[me->type].damage + 3 * (player.amulet.magic == EM_SUMMONERS), GOOD));
					ang += 12;
				}
			}
			else
			{
				x = me->x + Cosine(me->facing * 32) * 30;
				y = me->y + Sine(me->facing * 32) * 30;
				if (badguy != NULL && me->AttackCheck(30, x >> FIXSHIFT, y >> FIXSHIFT, badguy))
				{
					byte ang;

					ang = AngleFrom(me->x, me->y, badguy->x, badguy->y);
					meleeAttacker = me;
					if (badguy->GetShot(Cosine(ang) * (player.var[VAR_CLOCKBOT - IT_LOG + IT_STUFFING] * 6), Sine(ang) * (player.var[VAR_CLOCKBOT - IT_LOG + IT_STUFFING] * 6), (1 + 2 * (ModifierOn(MOD_TICKING))) * DamageAmt(monsType[me->type].damage + 3 * (player.amulet.magic == EM_SUMMONERS), me->team), map, world))
					{
						if (player.var[VAR_CLOCKBOT - IT_LOG + IT_ICECUBE])
							badguy->GetFrozen(player.var[VAR_CLOCKBOT - IT_LOG + IT_ICECUBE] * 15);
						if (player.var[VAR_CLOCKBOT - IT_LOG + IT_ECTO])
							badguy->GetPoisoned(player.var[VAR_CLOCKBOT - IT_LOG + IT_ECTO] * 30);
						if (player.var[VAR_CLOCKBOT - IT_LOG + IT_SILVER])
							badguy->GetStunned(player.var[VAR_CLOCKBOT - IT_LOG + IT_SILVER] * 30 / 4);
						if (player.var[VAR_CLOCKBOT - IT_LOG + IT_DIAMOND])
						{
							MakeSound(SND_BOOM, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
							b = FireBullet(me->ID, badguy->x, badguy->y, 0, 0, BLT_CLOCKBOOM, (1 + 2 * (ModifierOn(MOD_TICKING))) * DamageAmt(monsType[me->type].damage + 3 * (player.amulet.magic == EM_SUMMONERS), GOOD));
							if (b)
								b->canthit = badguy->ID + 1;
						}
						if (player.var[VAR_CLOCKBOT - IT_LOG + IT_CLAW])
						{
							int j;

							for (j = 0; j < player.var[VAR_CLOCKBOT - IT_LOG + IT_CLAW]; j++)
							{
								b = FireBullet(me->ID, badguy->x, badguy->y, FIXAMT * 20, rand() % 256, BLT_AXESHARD, (1 + 2 * (ModifierOn(MOD_TICKING))) * bulDef[BLT_AXESHARD].damage);
								if (b)
									b->canthit = badguy->ID + 1;
							}
						}
					}
				}
			}

			x = 90 - player.var[VAR_CLOCKBOT - IT_LOG + IT_GEAR] * 9 + player.var[VAR_CLOCKBOT - IT_LOG + IT_RUBY] * 9;
			if (x < 4)
				x = 4;
			if (x > 255)
				x = 255;
			me->reload = (byte)x;
		}

		return;	// can't do nothin' right now
	}

	x = Distance(me->x, me->y, goodguy->x, goodguy->y);
	if (badguy != NULL)
		y = Distance(me->x, me->y, badguy->x, badguy->y);
	else
		y = FIXAMT * 1000;

	if (player.var[VAR_CLOCKBOT - IT_LOG + IT_RUBY])
		range = FIXAMT * 250;
	else
		range = FIXAMT * 54;

	if (me->mind2 == 0)	// in attack mode
	{
		if (badguy != NULL)
		{
			ClockPursue(me, badguy->x, badguy->y);
		}

		if (x > FIXAMT * 350 || badguy == NULL || badguy->type == MONS_NONE || badguy->hp == 0 || y > FIXAMT * 250)
		{
			me->mind2 = 1;	// chase player
			return;
		}
		// attack
		if (me->reload == 0 && map->CheckLOS(me->mapx, me->mapy, 10, badguy->mapx, badguy->mapy) && y < range)
		{
			MakeSound(SND_SKELSWING, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
			FaceGoodguy(me, badguy);
			DoMove(me, ANIM_ATTACK, 256, 1, 0, 0);
			return;
		}
	}
	if (me->mind2 == 1)
	{
		if (goodguy != NULL)
		{
			ClockPursue(me, goodguy->x, goodguy->y);
		}

		if (x < FIXAMT * 60)	// got close
		{
			me->mind2 = 2;
		}
		if (x > FIXAMT * 600)	// too far!
		{
			me->x = goodguy->x;
			me->y = goodguy->y;
		}
	}
	if (me->mind2 == 2)	// close to Loony, so wander randomly until either too far away or a badguy is near
	{
		if (goodguy != NULL)
		{
			ClockPursue(me, 0, 0);
		}

		if (x > FIXAMT * 80)
			me->mind2 = 1;	// go back to following
		if (y < FIXAMT * 250 || goodguy->ouch)
			me->mind2 = 0;	// an enemy came near, so beat on him
	}
}

void AI_Crab(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;
	byte rightAng;

	BasicAI(me, SND_CRABOUCH, SND_CRABDIE, map, world, goodguy);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frm == 4)
		{
			me->frmAdvance = 256;
		}
		if (me->seq == ANIM_ATTACK && me->frm == 3 && me->reload == 0)
		{
			x = me->x + Cosine(me->facing * 32) * 20;
			y = me->y + Sine(me->facing * 32) * 20;
			if (me->AttackCheck(20, x >> FIXSHIFT, y >> FIXSHIFT, goodguy))
			{
				meleeAttacker = me;
				goodguy->GetShot(Cosine(me->facing * 32) * 2, Sine(me->facing * 32) * 2, DamageAmt(monsType[me->type].damage, me->team), map, world);
				me->reload = 10;
				me->mind1 = 10;
				me->mind = 1 + (rand() % 2);
			}
			if (me->type == MONS_SOCKCRAB)
			{
				Guy* g;
				g = AddGuy(me->x, me->y, 0, MONS_MONKEY);
				if (g && !g->CanWalkNoMons(g->x, g->y, map, world))
					g->type = MONS_NONE;
				else if (g)
				{
					g->bright = 32;
					g->tag = 255;
					MakeSound(SND_SHROOMIFY, me->x, me->y, SND_CUTOFF, 300);
				}
				me->reload = 20;
			}
		}
		return;	// can't do nothin' right now
	}

	if (me->reload == 0 && map->CheckLOS(me->mapx, me->mapy, 2, goodguy->mapx, goodguy->mapy))
	{
		MakeSound(SND_CRABCLACK, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
		FaceGoodguy(me, goodguy);
		DoMove(me, ANIM_ATTACK, 256, 1, 0, 0);
		return;
	}
	if (me->reload == 0 && me->type == MONS_SOCKCRAB && Random(50) == 0)
	{
		FaceGoodguy(me, goodguy);
		DoMove(me, ANIM_ATTACK, 256, 1, 0, 0);
	}

	if (me->mind1)
	{
		me->mind1--;
	}
	else
	{
		switch (rand() % 8)
		{
		case 0:
			me->mind = 1;
			break;
		case 1:
			me->mind = 2;
			break;
		case 2:
			me->mind = 3;
			break;
		default:
			me->mind = 0;
			break;
		}
		me->mind1 = 1 + (rand() % 20);
	}

	if (me->mind == 0)	// heading straight at guy
	{
		rightAng = AngleFrom(me->x, me->y, goodguy->x, goodguy->y);
	}
	else if (me->mind == 1)	// turning right
	{
		rightAng = me->mind3 + 10;
	}
	else if (me->mind == 2)	// turning left
	{
		rightAng = me->mind3 - 10;
	}
	else if (me->mind == 3)	// going straight
	{
		rightAng = me->mind3;
	}

	me->mind3 = TurnToward(me->mind3, rightAng, 3);

	me->facing = ((me->mind3 + 16) / 32) & 7;
	me->dx = Cosine(me->mind3) * monsType[me->type].speed / FIXAMT;
	me->dy = Sine(me->mind3) * monsType[me->type].speed / FIXAMT;

	if (me->seq != ANIM_MOVE)
		DoMove(me, ANIM_MOVE, 128, 0, me->dx, me->dy);
}

void AI_Badger(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y, i;

	BasicAI(me, SND_BADGEROUCH, SND_BADGERDIE, map, world, goodguy);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frm == 3)
			me->frmAdvance = 256;
		if (me->seq == ANIM_ATTACK && me->frm == 2 && me->reload == 0)
		{
			x = me->x + Cosine(me->facing * 32) * 16;
			y = me->y + Sine(me->facing * 32) * 16;

			i = 0;
			while (1)
			{
				// slash everyone in range
				i = FindVictim(x, y, me->mapx, me->mapy, 30, 0, (GOOD + EVIL) - me->team, i, map, world);
				if (i)
				{
					byte ang;
					ang = AngleFrom(me->x, me->y, GetGuy((word)(i - 1))->x, GetGuy((word)(i - 1))->y);
					meleeAttacker = me;
					GetGuy((word)(i - 1))->GetShot(Cosine(ang) * 6, Sine(ang) * 6, DamageAmt(monsType[me->type].damage, me->team), map, world);
				}
				else break;
			}
			me->reload = 30;
		}
		if (me->seq == ANIM_A2)	// burrowing
		{
			if (me->frm == 6)
			{
				DoMove(me, ANIM_A4, 16, 0, 0, 0);
				me->mind2 = 1;
				return;
			}
			else if (me->frm >= 2)
				me->facing = (me->facing + 1) & 7;
		}
		if (me->seq == ANIM_A3)	// popping up
		{
			if (me->frm == 5)
			{
				DoMove(me, ANIM_MOVE, 128, 0, 0, 0);
				me->mind2 = 0;
				return;
			}
			else if (me->frm < 5)
				me->facing = (me->facing + 1) & 7;
			if (me->frm == 2 && me->reload == 0)
			{
				i = 0;
				while (1)
				{
					// slash everyone in range
					i = FindVictim(me->x, me->y, me->mapx, me->mapy, 70, 0, (GOOD + EVIL) - me->team, i, map, world);
					if (i)
					{
						byte ang;
						ang = AngleFrom(me->x, me->y, GetGuy((word)(i - 1))->x, GetGuy((word)(i - 1))->y);
						meleeAttacker = me;
						GetGuy((word)(i - 1))->GetShot(Cosine(ang) * 14, Sine(ang) * 14, DamageAmt(monsType[me->type].damage, me->team), map, world);
					}
					else break;
				}
				me->reload = 30;
			}
		}
		if (me->seq == ANIM_A1)
		{
			if (me->frm >= 1 && me->frm <= 15)
			{
				byte ang;

				ang = AngleFrom(me->x, me->y, goodguy->x, goodguy->y);
				me->facing = (me->facing + 1) & 7;
				me->dx += Cosine(ang) / 2;
				me->dy += Sine(ang) / 2;
				Clamp(&me->dx, FIXAMT * 6);
				Clamp(&me->dy, FIXAMT * 6);
				if (me->reload == 0)
				{
					i = 0;
					while (1)
					{
						// slash someone in range
						i = FindVictim(me->x, me->y, me->mapx, me->mapy, 50, 0, (GOOD + EVIL) - me->team, i, map, world);
						if (i)
						{
							byte ang;
							ang = AngleFrom(me->x, me->y, GetGuy((word)(i - 1))->x, GetGuy((word)(i - 1))->y);
							meleeAttacker = me;
							if (GetGuy((word)(i - 1))->GetShot(Cosine(ang) * 6, Sine(ang) * 6, DamageAmt(monsType[me->type].damage, me->team), map, world))
								GetGuy((word)(i - 1))->GetPoisoned(30 * 5);
							me->reload = 10;
							break;
						}
						else break;
					}
				}
			}
			if (me->frm > 15)
			{
				Dampen(&me->dx, FIXAMT / 4);
				Dampen(&me->dy, FIXAMT / 4);
				me->reload = 30;
			}
		}
		return;	// can't do nothin' right now
	}

	if (me->type == MONS_BADGER2 && me->mind2 == 1)	// underground
	{
		ExplodeParticles2(PART_DIRT, me->x, me->y, 0, 5, 6);

		WanderAI(me, 200, 10, 3, map, world, goodguy);
		DoMove(me, ANIM_A4, 16, 0, me->dx, me->dy);
		if (map->CheckLOS(me->mapx, me->mapy, 3, goodguy->mapx, goodguy->mapy) || Random(200) == 0 || me->ouch)
		{
			DoMove(me, ANIM_A3, 128, 1, 0, 0);	// come up if close or bored
			MakeSound(SND_BADGERSPIN, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
		}
	}
	else
	{
		// slash when you get close
		if (me->type == MONS_BADGER3)	// stinkin' ones do spin slash instead of regular
		{
			if (Random(10) == 0 && map->CheckLOS(me->mapx, me->mapy, 8, goodguy->mapx, goodguy->mapy) && me->reload == 0)
			{
				MakeSound(SND_BADGERSPIN, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
				FaceGoodguy(me, goodguy);
				DoMove(me, ANIM_A1, 128, 1, 0, 0);
				return;
			}
		}
		else if (Random(10) == 0 && map->CheckLOS(me->mapx, me->mapy, 3, goodguy->mapx, goodguy->mapy) && me->reload == 0)
		{
			MakeSound(SND_BADGERSLASH, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
			FaceGoodguy(me, goodguy);
			DoMove(me, ANIM_ATTACK, 128, 1, 0, 0);
			return;
		}
		if (me->type == MONS_BADGER2 && !map->CheckLOS(me->mapx, me->mapy, 7, goodguy->mapx, goodguy->mapy) && Random(20) == 0)	// dig
		{
			MakeSound(SND_BADGERDIG, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
			DoMove(me, ANIM_A2, 128, 1, 0, 0);
			return;
		}

		WanderAI(me, 64, 40, 3, map, world, goodguy);
		if (me->seq == ANIM_IDLE)
			DoMove(me, ANIM_MOVE, 128, 0, me->dx, me->dy);
	}
}

void AI_Titan(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	BasicAI(me, SND_TITANOUCH, SND_TITANDIE, map, world, goodguy);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frm == 6)
			me->frmAdvance = 256;
		if (me->seq == ANIM_ATTACK && me->frm == 3 && me->reload == 0)
		{
			if (me->type == MONS_NICETITAN)
				MakeSound(SND_BIGHEAL, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
			else
				MakeSound(SND_TITANSMASH, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);

			x = me->x + Cosine(me->facing * 32) * 32;
			y = me->y + Sine(me->facing * 32) * 32;

			if (me->type == MONS_NICETITAN)
			{
				NovaGuys(me->team, 2, x, y, FIXAMT * 100, monsType[me->type].damage, 0, 0);
				FireBullet(me->ID, x, y, 0, 0, BLT_NOVA, 0);
				MakeColdRingParticle(x, y, 0, 64);
				DestroyBulletsOther(x, y, GOOD+EVIL-me->team, 100 * FIXAMT, PART_WHITEPOP);
				me->reload = 120;
			}
			else
			{
				ShakeScreen(20);
				NovaGuys(me->team, 1, x, y, FIXAMT * 80, monsType[me->type].damage, 30, 12 * FIXAMT);
				FireBullet(me->ID, x, y, 0, 0, BLT_NOVA, 0);
				me->reload = 60;
			}
		}

		return;	// can't do nothin' right now
	}

	// smash when you get close
	if (Random(10) == 0 && map->CheckLOS(me->mapx, me->mapy, 4, goodguy->mapx, goodguy->mapy) && me->reload == 0)
	{
		FaceGoodguy(me, goodguy);
		DoMove(me, ANIM_ATTACK, 128, 1, 0, 0);
		return;
	}
	WanderAI(me, 64, 40, 3, map, world, goodguy);
	if (me->seq == ANIM_IDLE)
		DoMove(me, ANIM_MOVE, 128, 0, me->dx, me->dy);
}

void AI_BigPlant(Guy* me, Map* map, world_t* world, Guy* badguy)
{
	int r;
	bullet_t* b;

	BasicAI(me, SND_TITANOUCH, SND_TITANDIE, map, world, goodguy);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frm == 4)
			me->frmAdvance = 256;
		if (me->seq == ANIM_ATTACK && me->frm == 4 && me->reload == 0)
		{
			MakeSound(SND_PLANTSHOOT, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);

			b = FireBullet(me->ID, me->x, me->y, FIXAMT * 40, (me->mind - 30 + (rand() % 61)), BLT_ROSESEED, 0);
			if (b && goodguy)
			{
				r = Distance(me->x, me->y, goodguy->x, goodguy->y);
				b->speed = (r / 90);
			}
			me->reload = 60;
		}

		return;	// can't do nothin' right now
	}

	// fire seeds nonstop
	if (me->reload == 0)
	{
		me->mind += (256 / 6);
		DoMove(me, ANIM_ATTACK, 128, 1, 0, 0);
		return;
	}
}

void AI_Furnace(Guy* me, Map* map, world_t* world, Guy* badguy)
{
	if (player.var[VAR_FURNACE] == 1)
	{
		if (me->seq == ANIM_IDLE || (me->seq == ANIM_MOVE && me->frm == 11))
			DoMove(me, ANIM_MOVE, 128, 0, 0, 0);
	}
	else
	{
		DoMove(me, ANIM_IDLE, 128, 0, 0, 0);
	}
}

void AI_Stonewall(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y, i;

	BasicAI(me, SND_STONEOUCH, SND_STONEDIE, map, world, goodguy);

	// firewall heatshield
	if (me->type == MONS_FIREWALL)
	{
		if (me->mind3 == 0)
		{
			MakeRingParticle(me->x, me->y, 0, 60, 32);
			curMap->BrightTorch(me->mapx, me->mapy, 20, 5);
			MakeSound(SND_IGNITE, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
			me->mind3 = 5 + (rand() % 20);
			DestroyBulletsOther(me->x, me->y, (GOOD + EVIL - me->team), 100 * FIXAMT, PART_FIRE);
		}
		else
		{
			--me->mind3;
			if (rand() % 5 == 0)
				curMap->BrightTorch(me->mapx, me->mapy, 20, 5);
		}
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frm == 6)
			me->frmAdvance = 256;
		if (me->seq == ANIM_A1 && me->frm == 2 && me->reload == 0)
		{
			x = me->x + Cosine(me->facing * 32) * 32;
			y = me->y + Sine(me->facing * 32) * 32;

			i = 0;
			while (1)
			{
				// smash everyone in range
				i = FindVictim(x, y, me->mapx, me->mapy, 30, 0, (GOOD + EVIL) - me->team, i, map, world);
				if (i)
				{
					byte ang;
					ang = AngleFrom(me->x, me->y, GetGuy((word)(i - 1))->x, GetGuy((word)(i - 1))->y);
					meleeAttacker = me;
					GetGuy((word)(i - 1))->GetShot(Cosine(ang) * 6, Sine(ang) * 6, DamageAmt(monsType[me->type].damage, me->team), map, world);
				}
				else break;
			}
			me->reload = 30;
			ShakeScreen(10);
		}
		if (me->seq == ANIM_ATTACK && me->frm == 4 && me->reload == 0)
		{
			ShakeScreen(10);
			x = me->x + Cosine(me->facing * 32) * 32;
			y = me->y + Sine(me->facing * 32) * 32;
			if (me->type == MONS_STONEWALL)
			{
				MakeSound(SND_STONESTOMP, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
				FireBullet(me->ID, x, y, 0, me->facing * 32, BLT_EVILSPIKE, DamageAmt(monsType[me->type].damage / 2, me->team));
			}
			else if (me->type == MONS_FIREWALL)
			{
				MakeSound(SND_BURNINATE, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
				FireBullet(me->ID, x, y, 0, me->facing * 32, BLT_BADBURNIN, DamageAmt(monsType[me->type].damage, me->team));
			}
			me->reload = 60;
		}

		return;	// can't do nothin' right now
	}

	// smash when you get close
	if (Random(10) == 0 && map->CheckLOS(me->mapx, me->mapy, 4, goodguy->mapx, goodguy->mapy) && me->reload == 0)
	{
		FaceGoodguy(me, goodguy);
		DoMove(me, ANIM_A1, 128, 1, 0, 0);
		MakeSound(SND_STONEDIE, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
		return;
	}
	// stomp at range
	if (Random(10) == 0 && map->CheckLOS(me->mapx, me->mapy, 15, goodguy->mapx, goodguy->mapy) && me->reload == 0)
	{
		FaceGoodguy(me, goodguy);
		DoMove(me, ANIM_ATTACK, 128, 1, 0, 0);
		return;
	}
	WanderAI(me, 64, 40, 3, map, world, goodguy);
	if (me->seq == ANIM_IDLE)
		DoMove(me, ANIM_MOVE, 128, 0, me->dx, me->dy);
}

void AI_Bokbok(Guy* me, Map* map, world_t* world, Guy* badguy)
{
	byte rightAng;

	if (!goodguy)
		return;

	BasicAI(me, SND_MOUSEOUCH, SND_MOUSEDIE, map, world, goodguy);

	if (me->action == ACTION_BUSY)
	{
		return;	// can't do nothin' right now
	}

	if (me->reload == 0 && (rand() % 100) == 0)
	{
		MakeSound(SND_VILLAGERBORED + 37, me->x, me->y, SND_CUTOFF | SND_RANDOM, 400);
		DoMove(me, ANIM_ATTACK, 128 + (rand() % 129), 1, 0, 0);
		me->reload = 30;
		return;
	}

	if (me->mind2 == 0)	// not yet caught
	{
		if (map->CheckLOS(me->mapx, me->mapy, 1, goodguy->mapx, goodguy->mapy))
		{
			me->mind2 = 1;
			MakeSound(SND_BOKBOK, me->x, me->y, SND_CUTOFF, 1200);
			if (me->tag)
				PlayerSetVar(VAR_BOKFOUND + me->tag - 1, 1);
		}
	}

	if (me->mind1)
	{
		me->mind1--;
	}
	else
	{
		if (me->mind2 == 1)	// following goodguy
		{
			if (RangeToTarget(me, goodguy) < FIXAMT * 50)
			{
				me->mind = 1 + (rand() % 2);
				me->mind1 = 10 + (rand() % 20);
			}
			else if (RangeToTarget(me, goodguy) > FIXAMT * 600)	// off screen, teleport in
			{
				me->x = goodguy->x;
				me->y = goodguy->y;
				me->z = FIXAMT * 600;
				DoMove(me, ANIM_ATTACK, 128, 1, 0, 0);
			}
			else
			{
				me->mind = 0;
				me->mind1 = 10 + (rand() % 20);
			}
		}
		else	// wander if unfound or already delivered
		{
			me->mind = 1 + (rand() % 3);
			me->mind1 = 10 + (rand() % 20);
		}
	}

	if (me->mind == 0)	// heading straight at guy
	{
		rightAng = AngleFrom(me->x, me->y, goodguy->x, goodguy->y);
		me->mind3 = TurnToward(me->mind3, rightAng, 5);
	}
	else if (me->mind == 1)	// turning right
	{
		rightAng = me->mind3 + 10;
		me->mind3 = TurnToward(me->mind3, rightAng, 3);

	}
	else if (me->mind == 2)	// turning left
	{
		rightAng = me->mind3 - 10;
		me->mind3 = TurnToward(me->mind3, rightAng, 3);
	}
	else if (me->mind == 3)	// go straight
	{
		rightAng = me->mind3;
	}
	if (me->mind2 == 1 && RangeToTarget(me, goodguy) > FIXAMT * 100)
		me->mind3 = TurnToward(me->mind3, rightAng, 3);

	me->facing = ((me->mind3 + 16) / 32) & 7;
	if (me->mind2 == 1 && RangeToTarget(me, goodguy) > FIXAMT * 100)
	{
		me->dx = Cosine(me->mind3) * (monsType[me->type].speed * 3 / 2) / FIXAMT;
		me->dy = Sine(me->mind3) * (monsType[me->type].speed * 3 / 2) / FIXAMT;
	}
	else
	{
		me->dx = Cosine(me->mind3) * monsType[me->type].speed / FIXAMT;
		me->dy = Sine(me->mind3) * monsType[me->type].speed / FIXAMT;
	}

	if (me->seq != ANIM_MOVE)
		DoMove(me, ANIM_MOVE, 128, 0, me->dx, me->dy);
}

void AI_Bokbok2(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	byte rightAng;


	if (me->mind > 0)
	{
		me->mind--;
		if (!me->mind)
		{
			DoMove(me, ANIM_DIE, 128, 1, 0, 0);
			me->hp = 0;
		}
	}
	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frm == 1 && me->reload == 0)
		{
			me->reload = 1;
			MakeSound(SND_BOOM, me->x, me->y, SND_CUTOFF | SND_RANDOM, 600);
			FireBullet(me->ID, me->x, me->y, 0, 0, BLT_SMALLBOOM, DamageAmt(me->mind2, me->team));
		}
		return;	// can't do nothin' right now
	}

	me->reload = 0;
	if (goodguy && RangeToTarget(me, goodguy) < FIXAMT * 50)
	{
		DoMove(me, ANIM_DIE, 128, 1, 0, 0);
		me->hp = 0;
	}
	if (goodguy)
		rightAng = AngleFrom(me->x, me->y, goodguy->x, goodguy->y);
	else
		rightAng = rand() % 256;

	me->mind3 = TurnToward(me->mind3, rightAng, 7);

	me->facing = ((me->mind3 + 16) / 32) & 7;
	me->dx = Cosine(me->mind3) * monsType[me->type].speed / FIXAMT;
	me->dy = Sine(me->mind3) * monsType[me->type].speed / FIXAMT;

	if (me->seq != ANIM_MOVE)
		DoMove(me, ANIM_MOVE, 192, 0, me->dx, me->dy);
}

void AI_BunnyBoss2(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int i;

	// quest-related nonsense............................
	if (player.var[VAR_CAPTAIN2] == 1)
	{
		// he does the spin on down
		if (me->z > 0)
		{
			if (me->seq != ANIM_ATTACK)
				DoMove(me, ANIM_ATTACK, 128, 1, 0, 0);
			else if (me->frm > 4)
			{
				me->frm = 4;
				me->facing = (me->facing + 1) & 7;
			}
			me->mind = 30;
		}
		if (me->z == 0)
		{
			me->mind--;
			me->facing = 2;
			if (me->mind == 29)
			{
				FireBullet(me->ID, me->x, me->y, 0, 0, BLT_KABOOM, 0);
				MakeNormalSound(SND_BOOM);
			}

			if (me->mind == 0)
			{
				player.chatClock = 0;
				BeginChatting(30);
				PlayerSetVar(VAR_CAPTAIN2, 2);
			}
		}
		return;
	}
	// end quest-related nonsense!!!!!!!!!!!!!!

	if (goodguy == NULL)
	{
		me->dx = 0;
		me->dy = 0;
		return;
	}

	BasicAI(me, SND_THUGOUCH, SND_THUGDIE, map, world, goodguy);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frm == 3)
		{
			if (player.var[VAR_CAPTAIN2] == 2)
			{
				Guy* g;

				PlayerSetVar(VAR_CAPTAIN2, 3);	// captain's dead! AA use function
				player.chatClock = 0;
				BeginChatting(30);
				g = AddGuy(me->x, me->y, FIXAMT * 500, MONS_BUNNYMAJOR);
				if (g)
				{
					g->team = GOOD;
					g->tag = 31;	// AA tag major for chatting
				}
			}
			me->frmAdvance = 256;
		}
		if (me->seq == ANIM_ATTACK)
		{
			if (me->frm == 2 && me->frmTimer < 64)
				FireBullet(me->ID, me->x, me->y, 0, 0, BLT_TORNADO2, 0);
			if (me->frm >= 1 && me->frm <= 13)
			{
				me->dx += Cosine(me->mind2) / 2;
				me->dy += Sine(me->mind2) / 2;
				Clamp(&me->dx, FIXAMT * 7);
				Clamp(&me->dy, FIXAMT * 7);
				if (me->wallBump & 1)
				{
					// flip horiz
					if (me->mind2 < 128)
						me->mind2 = 128 - me->mind2;
					else
						me->mind2 = 128 + (256 - me->mind2);
					me->dx = -me->dx;
				}
				if (me->wallBump & 2)
				{
					// flip vert
					me->mind2 = 256 - me->mind2;
					me->dy = -me->dy;
				}
				me->wallBump = 0;
				me->facing = (me->facing + 1) & 7;	// spin!

				if (me->reload == 0)
				{
					i = 0;
					while (1)
					{
						// slash everyone in range
						i = FindVictim(me->x, me->y, me->mapx, me->mapy, 80, 0, (GOOD + EVIL) - me->team, i, map, world);
						if (i)
						{
							byte ang;
							ang = AngleFrom(me->x, me->y, GetGuy((word)(i - 1))->x, GetGuy((word)(i - 1))->y);
							meleeAttacker = me;
							GetGuy((word)(i - 1))->GetShot(Cosine(ang) * 40, Sine(ang) * 40, DamageAmt(monsType[me->type].damage, me->team), map, world);
						}
						else break;
					}
					me->reload = 4;
				}
			}
		}
		if (me->seq == ANIM_A2 && me->frm == 2 && me->reload == 0)
		{
			me->reload = 10;
			for (i = 0; i < 12; i++)
				FireBullet(me->ID, me->x, me->y, FIXAMT * 20, me->facing * 32 - (6 * 10) + 5 + (i * 10), BLT_FIREBALL, DamageAmt(monsType[me->type].damage, me->team));
		}

		return;	// can't do nothin' right now
	}

	// game plan: wander around for 1 seconds (if hit during this phase, 10% chance to go into blocking)
	// when done wandering, do a tornado dash
	if (me->mind == 0)	// not yet decided to do anything
	{
		me->mind = 1;
		me->mind1 = 90;	// 1 seconds
		me->facing = rand() % 8;
		DoMove(me, ANIM_MOVE, 128, 0, Cosine(me->facing * 32) * monsType[me->type].speed, Sine(me->facing * 32) * monsType[me->type].speed);
	}
	else if (me->mind == 1)	// wandering
	{
		if (me->wallBump)
		{
			me->facing *= 32;
			if (me->wallBump & 1)
			{
				// flip horiz
				if (me->facing < 128)
					me->facing = 128 - me->facing;
				else
					me->facing = 128 + (256 - me->facing);
			}
			if (me->wallBump & 2)
			{
				// flip vert
				me->facing = 256 - me->facing;
			}
			me->facing /= 32;
			DoMove(me, ANIM_MOVE, 128, 0, Cosine(me->facing * 32) * monsType[me->type].speed / FIXAMT, Sine(me->facing * 32) * monsType[me->type].speed / FIXAMT);
			me->wallBump = 0;
		}
		if ((rand() % 40) == 0)
		{
			me->facing = rand() % 8;
			DoMove(me, ANIM_MOVE, 128, 0, Cosine(me->facing * 32) * monsType[me->type].speed / FIXAMT, Sine(me->facing * 32) * monsType[me->type].speed / FIXAMT);
		}
		if (me->seq != ANIM_MOVE)
			DoMove(me, ANIM_MOVE, 128, 0, Cosine(me->facing * 32) * monsType[me->type].speed / FIXAMT, Sine(me->facing * 32) * monsType[me->type].speed / FIXAMT);
		me->mind1--;
		if (!me->mind1)
		{
			MakeSound(SND_TORNADO, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
			DoMove(me, ANIM_ATTACK, 64, 1, 0, 0);
			me->mind2 = me->facing * 32;
			me->mind1 = 90;
			return;
		}
		if (me->ouch && (rand() % 4) == 0)
		{
			// 25% chance when hit to do block
			DoMove(me, ANIM_A1, 128, 1, 0, 0);
			if (goodguy)
				FaceGoodguy(me, goodguy);
			return;
		}
	}
}

void AI_TitanBig(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y, i;

	BasicAI(me, SND_TITANOUCH, SND_TITANDIE, map, world, goodguy);

	if (me->ouch && me->mind < 40)
		me->mind++;

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frm == 6)
			me->frmAdvance = 256;
		if (me->seq == ANIM_ATTACK && me->frm == 3 && me->reload == 0)
		{
			MakeSound(SND_TITANSMASH, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
			x = me->x + Cosine(me->facing * 32) * 32;
			y = me->y + Sine(me->facing * 32) * 32;

			ShakeScreen(20);
			i = 0;
			NovaGuys(me->team, 1, x, y, FIXAMT * 80, monsType[me->type].damage, 30, 12 * FIXAMT);
			for (i = 0; i < 3; i++)
				FireBullet(me->ID, x, y, 0, me->mind3 - 32 + (rand() % 65), BLT_EVILSPIKE2, DamageAmt(17, me->team));
			me->reload = 30;
		}
		if (me->seq == ANIM_A1 && me->frm >= 4 && me->frm <= 10)
		{
			int g;

			g = GuyList_First();
			while (g != 65535)
			{
				if (GetGuy(g)->hp && GetGuy(g)->team == GOOD && !(monsType[GetGuy(g)->type].flags & MF_NOMOVE))
				{
					i = AngleFrom(me->x, me->y, GetGuy(g)->x, GetGuy(g)->y);
					i = me->mind3 + 256 - i;
					if (i > 256 - 48 && i < 256 + 48 && map->CheckLOS(me->mapx, me->mapy, 12, GetGuy(g)->mapx, GetGuy(g)->mapy))	// make sure the player is somewhere in front of you
					{
						i = AngleFrom(me->x, me->y, GetGuy(g)->x, GetGuy(g)->y);
						GetGuy(g)->kbdx += Cosine(i) * 5;
						GetGuy(g)->kbdy += Sine(i) * 5;
						GetGuy(g)->GetFrozen(30);
					}
				}
				g = GuyList_Next();
			}
		}

		return;	// can't do nothin' right now
	}

	// smash when you get a chance to
	if (Random(30) == 0 && me->reload == 0)
	{
		DoMove(me, ANIM_ATTACK, 128, 1, 0, 0);
		return;
	}

	if (me->mind >= 40)	// roar when bothered enough
	{
		MakeSound(SND_TITANROAR, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
		me->mind = 0;
		me->mind3 = AngleFrom(me->x, me->y, goodguy->x, goodguy->y);	// insta-face!
		me->facing = ((me->mind3 + 16) / 32) & 7;
		DoMove(me, ANIM_A1, 128, 1, 0, 0);
		return;
	}

	byte rightAng = AngleFrom(me->x, me->y, goodguy->x, goodguy->y);
	me->mind3 = TurnToward(me->mind3, rightAng, 1);

	me->facing = ((me->mind3 + 16) / 32) & 7;
	me->dx = Cosine(me->mind3) * monsType[me->type].speed / FIXAMT;
	me->dy = Sine(me->mind3) * monsType[me->type].speed / FIXAMT;

	if (me->seq != ANIM_MOVE)
		DoMove(me, ANIM_MOVE, 128, 0, me->dx, me->dy);
}

void AI_ArenaCrystal(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	BasicAI(me, SND_CRYSTALOUCH, SND_CRYSTALBREAK, map, world, goodguy);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE)
		{
			me->frmAdvance = 128;
		}
		return;	// can't do nothin' right now
	}
}

void AI_Turret(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int i, x, y;

	BasicAI(me, SND_MOUSEOUCH, SND_MOUSEDIE, map, world, goodguy);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frm == 0 && me->frmTimer < 128)
		{
			me->frmAdvance = 128;
			MakeSound(SND_BOOM, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
			FireBullet(me->ID, me->x, me->y + 1, me->z, 0, BLT_SMALLBOOM, 0);
		}
		return;	// can't do nothin' right now
	}
	if (me->frm == 4 && me->reload == 0)
	{
		MakeSound(SND_TINSHOOT, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
		for (i = 0; i < 4; i++)
		{
			x = me->x + Cosine(i * 64) * 32;
			y = me->y + Sine(i * 64) * 32;
			FireBullet(me->ID, x, y, FIXAMT * 5, i * 64, BLT_TURRET, DamageAmt(monsType[me->type].damage, me->team));
		}
		me->reload = 5;
	}
	if (me->frm == 15 && me->reload == 0)
	{
		MakeSound(SND_TINSHOOT, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
		for (i = 0; i < 4; i++)
		{
			x = me->x + Cosine(i * 64 + 32) * 32;
			y = me->y + Sine(i * 64 + 32) * 32;
			FireBullet(me->ID, x, y, FIXAMT * 5, i * 64 + 32, BLT_TURRET, DamageAmt(monsType[me->type].damage, me->team));
		}
		me->reload = 5;
	}
	me->frmAdvance = 128;
}

void AI_PuppetPal(Guy* me, Map* map, world_t* world, Guy* badguy)
{
	int x, y, range;

	//player.var[VAR_PUPPETLIFE + me->tag] = me->hp;
	SetPuppetLife(me->tag, (word)me->hp);

	if ((!goodguy || goodguy->hp == 0) && me->hp)
	{
		return;
	}

	BasicAI(me, SND_PUPPETOUCH, SND_PUPPETDIE, map, world, badguy);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frm == 4)
			me->frmAdvance = 256;
		if (me->seq == ANIM_ATTACK && me->frm == 4 && me->reload == 0)
		{
			x = me->x + Cosine(me->facing * 32) * 16;
			y = me->y + Sine(me->facing * 32) * 16;
			if (badguy && me->AttackCheck(20, x >> FIXSHIFT, y >> FIXSHIFT, badguy))
			{
				meleeAttacker = me;

				if (badguy->GetShot(Cosine(me->facing * 32) * 16, Sine(me->facing * 32) * 16, DamageAmt(monsType[me->type].damage, me->team), map, world))
				{
					if (badguy->ouch == 4)
						badguy->GetStunned(20);
				}
			}
			me->reload = 30;
		}
		return;	// can't do nothin' right now
	}

	x = Distance(me->x, me->y, goodguy->x, goodguy->y);
	if (badguy != NULL)
		y = Distance(me->x, me->y, badguy->x, badguy->y);
	else
		y = FIXAMT * 1000;

	range = FIXAMT * 54;

	if (me->mind2 == 0)	// in attack mode
	{
		if (badguy != NULL)
			WanderAI(me, 64, 40, 3, map, world, badguy);
		me->frmAdvance = 180;
		if (x > FIXAMT * 256 || badguy == NULL || badguy->type == MONS_NONE || badguy->hp == 0 ||
			y > FIXAMT * 256)
		{
			me->mind2 = 1;	// chase player
			return;
		}
		// punch
		if (map->CheckLOS(me->mapx, me->mapy, 5, badguy->mapx, badguy->mapy) && y < range)
		{
			MakeSound(SND_PUPPETPUNCH, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
			FaceGoodguy(me, badguy);
			me->seq = ANIM_ATTACK;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 200;
			me->action = ACTION_BUSY;
			me->dx = 0;
			me->dy = 0;
			return;
		}
	}
	if (me->mind2 == 1)
	{
		FaceGoodguy(me, goodguy);

		me->dx = (Cosine(me->facing * 32) * monsType[me->type].speed) / FIXAMT;
		me->dy = (Sine(me->facing * 32) * monsType[me->type].speed) / FIXAMT;
		if (me->seq != ANIM_MOVE)
		{
			me->seq = ANIM_MOVE;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 180;
		}
		if (x < FIXAMT * 64)	// got close
		{
			me->mind2 = 2;
			me->facing = rand() % 8;
		}
	}
	if (me->mind2 == 2)	// close to Loony, so wander randomly until either too far away or a badguy is near
	{
		me->dx = (Cosine(me->facing * 32) * monsType[me->type].speed) / FIXAMT;
		me->dy = (Sine(me->facing * 32) * monsType[me->type].speed) / FIXAMT;
		if (me->seq != ANIM_MOVE)
		{
			me->seq = ANIM_MOVE;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 180;
		}

		if (x > FIXAMT * 70)
			me->mind2 = 1;	// go back to following
		if (y < FIXAMT * 128 || goodguy->ouch)
			me->mind2 = 0;	// an enemy came near, so beat on him
	}
}

void AI_FrostGate(Guy* me, Map* map, world_t* world, Guy* badguy)
{
	BasicAI(me, SND_PUPPETOUCH, SND_PUPPETDIE, map, world, badguy);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && player.var[VAR_FROSTGATE] == 0)
		{
			MakeNormalSound(SND_BOOM);
			SmashFrostGate(map, 1);
		}
	}
}

void AI_ManyMouse(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	byte rightAng;

	BasicAI(me, SND_MOUSEOUCH, SND_MOUSEDIE, map, world, goodguy);

	if (me->mind2 == 0)
		me->mind2 = (byte)monsType[me->type].hp;

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frm == 7 && me->frmTimer < 128)
		{
			Guy* g;
			me->mind2 /= 3;
			if (me->mind2 > 0)
			{
				g = AddGuy(me->x, me->y, 0, MONS_MOUSE5);
				if (g)
				{
					g->hp = me->mind2;
					if (player.var[VAR_MADCAP])
						g->hp *= MADCAP_HPBOOST;
					g->mind2 = me->mind2;
					g->tag = 255;
				}
				g = AddGuy(me->x, me->y, 0, MONS_MOUSE5);
				if (g)
				{
					g->hp = me->mind2;
					if (player.var[VAR_MADCAP])
						g->hp *= MADCAP_HPBOOST;
					g->mind2 = me->mind2;
					g->tag = 255;
				}
			}
			me->frmAdvance = 256;
		}
		if (me->seq == ANIM_ATTACK && me->frm == 1 && me->reload == 0)
		{
			me->reload = 40;
			MakeSound(SND_MOUSESHOOT, me->x, me->y, SND_CUTOFF | SND_RANDOM, 600);
			FireBullet(me->ID, me->x, me->y, FIXAMT * 20, (byte)(me->facing * 32 - 8 + Random(17)), BLT_FIREBALL, DamageAmt(monsType[me->type].damage, me->team));
		}
		return;	// can't do nothin' right now
	}

	if (!goodguy)
		return;

	if (me->reload == 0 && map->CheckLOS(me->mapx, me->mapy, 10, goodguy->mapx, goodguy->mapy))
	{
		DoMove(me, ANIM_ATTACK, 128, 1, 0, 0);
		return;
	}

	if (me->mind1)
	{
		me->mind1--;
	}
	else
	{
		switch (rand() % 8)
		{
		case 0:
			me->mind = 1;
			break;
		case 1:
			me->mind = 2;
			break;
		case 2:
			me->mind = 3;
			break;
		default:
			me->mind = 0;
			break;
		}
		me->mind1 = 1 + (rand() % 20);
	}

	if (me->mind == 0)	// heading straight at guy
	{
		rightAng = AngleFrom(me->x, me->y, goodguy->x, goodguy->y);
	}
	else if (me->mind == 1)	// turning right
	{
		rightAng = me->mind3 + 10;
	}
	else if (me->mind == 2)	// turning left
	{
		rightAng = me->mind3 - 10;
	}
	else if (me->mind == 3)	// going straight
	{
		rightAng = me->mind3;
	}

	me->mind3 = TurnToward(me->mind3, rightAng, 3);

	me->facing = ((me->mind3 + 16) / 32) & 7;
	me->dx = Cosine(me->mind3) * monsType[me->type].speed / FIXAMT;
	me->dy = Sine(me->mind3) * monsType[me->type].speed / FIXAMT;

	if (me->seq != ANIM_MOVE)
		DoMove(me, ANIM_MOVE, 128, 0, me->dx, me->dy);
}

void AI_Mimic(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;
	byte rightAng;

	// insert here things for when not actually fighting the mimic
	if (player.levelNum == LVL_SOUTHWOOD)
	{
		if ((rand() % 100) == 0)
			DoMove(me, ANIM_MOVE, 64 + (rand() % 192), 0, 0, 0);
		if ((rand() % 400) == 0)
			DoMove(me, ANIM_A1, 64 + (rand() % 192), 0, 0, 0);
		return;
	}
	if (levelDef[player.levelNum].flags & LF_TOWN)
	{
		// can bump it for stash usage
		if ((rand() % 100) == 0)
			DoMove(me, ANIM_MOVE, 64 + (rand() % 192), 0, 0, 0);
		if ((rand() % 400) == 0)
			DoMove(me, ANIM_A1, 64 + (rand() % 192), 0, 0, 0);
		return;
	}

	if (me->ouch == 4)
	{
		if (me->mind2 > 0 && (player.var[VAR_TAMED] & 3) < 3)
		{
			// untame!
			me->mind2--;
			MakeRingParticle(me->x, me->y, 0, 30, 10);
			MakeSound(SND_BADGERSLASH, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
		}
	}

	BasicAI(me, SND_PUPPETOUCH, SND_PUPPETDIE, map, world, goodguy);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frm == 5)
		{
			me->frmAdvance = 256;
		}
		if (me->seq == ANIM_ATTACK && me->frm == 2 && me->reload == 0)
		{
			x = me->x + Cosine(me->facing * 32) * 16;
			y = me->y + Sine(me->facing * 32) * 16;
			if (goodguy && me->AttackCheck(20, x >> FIXSHIFT, y >> FIXSHIFT, goodguy))
			{
				meleeAttacker = me;
				goodguy->GetShot(Cosine(me->facing * 32) * 4, Sine(me->facing * 32) * 4, DamageAmt(monsType[me->type].damage, me->team), map, world);
			}
			me->reload = 15;
			if (me->mind2 > 0 && (player.var[VAR_TAMED] & 3) < 3)
			{
				// untame!
				me->mind2--;
				MakeRingParticle(me->x, me->y, 0, 30, 10);
				MakeSound(SND_BADGERSLASH, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
			}

		}
		return;	// can't do nothin' right now
	}

	if (EatItem(me->x, me->y))
	{
		MakeSound(SND_BERRYEAT, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
		DoMove(me, ANIM_A1, 128, 1, 0, 0);
		if ((player.var[VAR_TAMED] & 3) < 3)
		{
			me->mind2++;
			MakeColdRingParticle(me->x, me->y, 0, 30);
			if (me->mind2 == 5)
			{
				for (x = 0; x < 30; x++)
					Smoke(me->x, me->y, rand() % (FIXAMT * 30));
				me->type = MONS_NONE;
				if (!(player.var[VAR_TAMED] & 1))
				{
					player.var[VAR_TAMED] |= 1;
					ReportQuestReport(QUEST_MIMIC);
					NewMessage("The tamed Mimic poofs away to the Magicsmith!", 0);
				}
				else if (!(player.var[VAR_TAMED] & 2))
				{
					player.var[VAR_TAMED] |= 2;
					NewMessage("The tamed Mimic poofs away to town!", 0);
				}
				return;
			}
		}
		return;
	}
	if (!goodguy)
		return;

	if (RangeToTarget(me, goodguy) < 60 * FIXAMT && me->reload == 0 && (rand() % 20) == 0)
	{
		FaceGoodguy(me, goodguy);
		DoMove(me, ANIM_ATTACK, 128, 1, 0, 0);
		return;
	}

	if (me->mind1)
	{
		me->mind1--;
	}
	else
	{
		switch (rand() % 8)
		{
		case 0:
			me->mind = 1;
			break;
		case 1:
			me->mind = 2;
			break;
		case 2:
			me->mind = 3;
			break;
		default:
			me->mind = 0;
			break;
		}
		me->mind1 = 1 + (rand() % 20);
	}

	if (me->mind == 0)	// heading straight at guy
	{
		rightAng = AngleFrom(me->x, me->y, goodguy->x, goodguy->y);
	}
	else if (me->mind == 1)	// turning right
	{
		rightAng = me->mind3 + 10;
	}
	else if (me->mind == 2)	// turning left
	{
		rightAng = me->mind3 - 10;
	}
	else if (me->mind == 3)	// going straight
	{
		rightAng = me->mind3;
	}

	me->mind3 = TurnToward(me->mind3, rightAng, 3);

	me->facing = ((me->mind3 + 16) / 32) & 7;
	me->dx = Cosine(me->mind3) * monsType[me->type].speed / FIXAMT;
	me->dy = Sine(me->mind3) * monsType[me->type].speed / FIXAMT;

	if (me->seq != ANIM_MOVE)
		DoMove(me, ANIM_MOVE, 128, 0, me->dx, me->dy);
}

void AI_Baron(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	byte rightAng;

	if (player.var[VAR_TEMP + 1] == 0)
	{
		BeginChatting(41);
		player.var[VAR_TEMP + 1] = 1;
	}

	BasicAI(me, SND_BARONOUCH, SND_PUPPETDIE, map, world, goodguy);

	if (me->hp < MonsterHP(me->type, me->RealTeam()) / 2)	// AA madcap HP fix
		me->hp = MonsterHP(me->type, me->RealTeam()) / 2;	// capped at 50% HP loss

	if (me->hp <= MonsterHP(me->type, me->RealTeam()) / 2)	// 50% health, time to end it
	{
		me->bright = 20;
		player.var[VAR_TEMP + 2] = 1;
		// head straight to the end-it point
		me->mind3 = AngleFrom(me->x, me->y, (35 * TILE_WIDTH + TILE_WIDTH / 2) * FIXAMT, (42 * TILE_HEIGHT + TILE_HEIGHT / 2) * FIXAMT);
		me->facing = ((me->mind3 + 16) / 32) & 7;
		me->dx = Cosine(me->mind3) * monsType[me->type].speed / FIXAMT;
		me->dy = Sine(me->mind3) * monsType[me->type].speed / FIXAMT;

		if (me->seq != ANIM_MOVE)
			DoMove(me, ANIM_MOVE, 128, 0, me->dx, me->dy);

		if (player.var[VAR_TEMP] == 1 && player.var[VAR_TEMP + 3] == 0)
		{
			Guy* g = AddGuy((32 * TILE_WIDTH + TILE_WIDTH / 2) * FIXAMT, (37 * TILE_HEIGHT + TILE_HEIGHT / 2) * FIXAMT, 0, MONS_UBERTITAN);
			if (g)
			{
				DoMove(g, ANIM_A2, 64, 1, 0, 0);
				ExplodeStones(map);
				ShakeScreen(60);
				player.var[VAR_TEMP + 3] = 1;
			}
		}
		if ((me->mapx == 35 && me->mapy == 42) && player.var[VAR_TEMP] == 0)
		{
			me->x = (35 * TILE_WIDTH + TILE_WIDTH / 2) * FIXAMT;
			me->y = (42 * TILE_HEIGHT + TILE_HEIGHT / 2) * FIXAMT;
			player.var[VAR_TEMP] = 1;
			BeginChatting(41);
		}
		if (player.var[VAR_TEMP] == 3)	// got squished
		{
			me->type = MONS_NONE;
			MakeNormalSound(SND_BARONDIE);
			player.var[VAR_BARONDEAD] = 1;
			player.var[VAR_TEMP + 2] = 0;
			return;
		}
		return;
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK && me->reload == 0)
		{
			map->BrightTorch(me->mapx, me->mapy, 24, 4);
			DrawStones(map, me, monsType[me->type].damage);
			me->reload = 8;
		}
		return;	// can't do nothin' right now
	}

	if (!goodguy)
		return;

	if (me->mind2 == 0)
	{
		FireBullet(me->ID, me->x, me->y, FIXAMT * 20, me->facing * 32, BLT_ICEMINE, DamageAmt(monsType[me->type].damage, me->team));
		me->mind2 = 30 * 3;

	}
	else
		me->mind2--;

	if (me->mind2 == 30)
	{
		FaceGoodguy(me, goodguy);
		DoMove(me, ANIM_ATTACK, 128, 1, 0, 0);
		return;
	}

	if (me->wallBump & 1)
	{
		if (me->mind3 < 128)
		{
			me->mind3 = (128 - me->mind3);
		}
		else
		{
			me->mind3 = (byte)(384 - me->mind3);
		}
	}
	if (me->wallBump & 2)
	{
		me->mind3 = 256 - me->mind3;
	}

	me->wallBump = 0;

	if (me->mind1)
	{
		me->mind1--;
	}
	else
	{
		switch (rand() % 8)
		{
		case 0:
			me->mind = 1;
			break;
		case 1:
			me->mind = 2;
			break;
		case 2:
			me->mind = 3;
			break;
		default:
			me->mind = 0;
			break;
		}
		me->mind1 = 1 + (rand() % 20);
	}

	if (me->mind == 0)	// heading straight (doesn't care where you are anyway!)
	{
		rightAng = me->mind3;
	}
	else if (me->mind == 1)	// turning right
	{
		rightAng = me->mind3 + 10;
	}
	else if (me->mind == 2)	// turning left
	{
		rightAng = me->mind3 - 10;
	}
	else if (me->mind == 3)	// going straight
	{
		rightAng = me->mind3;
	}

	me->mind3 = TurnToward(me->mind3, rightAng, 3);

	me->facing = ((me->mind3 + 16) / 32) & 7;
	me->dx = Cosine(me->mind3) * monsType[me->type].speed / FIXAMT;
	me->dy = Sine(me->mind3) * monsType[me->type].speed / FIXAMT;

	if (me->seq != ANIM_MOVE)
		DoMove(me, ANIM_MOVE, 128, 0, me->dx, me->dy);
}

void AI_UberTitan(Guy* me, Map* map, world_t* world, Guy* badguy)
{
	int i, x, y, a;

	BasicAI(me, SND_TITANOUCH, SND_TITANDIE, map, world, goodguy);

	x = me->x + (-150 + (rand() % 301)) * FIXAMT;
	y = me->y + (-60 + (rand() % 151)) * FIXAMT;
	a = FIXAMT * 10 + (rand() % (FIXAMT * 80));
	ExplodeParticles2(PART_WATER, x, y, a, 1, 1);
	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK && (me->frm == 3 || me->frm == 6) && me->reload == 0)
		{
			a = AngleFrom(me->x, me->y, goodguy->x, goodguy->y) - 48;
			if (a > 64 && a < 128)
				a = 64;
			if (a >= 128)
				a = 208;
			for (i = 0; i < 12; i++)
			{
				x = me->x + Cosine(a) * 40;
				y = me->y + Sine(a) * 30;
				FireBullet(me->ID, x, y, FIXAMT * 30, a, BLT_CRYO2, DamageAmt(monsType[me->type].damage / 2, me->team));
				a += 8;
			}
			me->reload = 4;
		}
		if (me->seq == ANIM_A1 && me->reload == 0 && me->frm == 3)
		{
			// left stomp
			me->reload = 4;
			GroundStomp(monsType[me->type].damage, map, world);
			ShakeScreen(60);
			MakeNormalSound(SND_TITANSMASH);
		}
		if (me->seq == ANIM_A1 && me->reload == 0 && me->frm == 10)
		{
			// right stomp
			if (player.var[VAR_TEMP] == 2)
			{
				player.var[VAR_TEMP] = 3;
			}
			else
				GroundStomp(monsType[me->type].damage, map, world);
			ShakeScreen(60);
			MakeNormalSound(SND_TITANSMASH);
			me->reload = 4;
		}
		if (me->seq == ANIM_A2 && me->frm == 1)
			MakeNormalSound(SND_TITANROAR);

		if (me->seq == ANIM_DIE && player.var[VAR_TEMP] == 4)
		{
			player.var[VAR_TEMP] = 5;
			BeginChatting(42);
			for (x = 0; x < map->width * map->height; x++)
				if (map->map[x].tag == 3)
					map->map[x].item = IT_NONE;
		}
		return;	// can't do nothin' right now
	}

	if (player.var[VAR_TEMP] == 2)
	{
		DoMove(me, ANIM_A1, 64, 1, 0, 0);
		me->frm = 7;
		return;
	}
	if (player.var[VAR_TEMP] == 3)	// finished squishing the baron
	{
		player.var[VAR_TEMP] = 4;
		BeginChatting(42);
		return;
	}
	if (player.var[VAR_TEMP] == 1)	// need to pound baron
	{
		BeginChatting(42);
		player.var[VAR_TEMP] = 2;
		return;
	}

	if (!goodguy)
		return;

	switch (me->mind)
	{
	case 0:	// setup
		me->mind = 1;
		me->mind1 = 30;	// wait one second, then spit
		break;
	case 1:
		if (me->mind1)
			me->mind1--;
		else
		{
			MakeNormalSound(SND_TITANROAR);
			DoMove(me, ANIM_ATTACK, 64, 1, 0, 0);
			me->mind = 2;
			me->mind1 = 45;	//	1.5 seconds, then stomp
			return;
		}
		break;
	case 2:
		if (me->mind1)
			me->mind1--;
		else
		{
			DoMove(me, ANIM_A1, 64, 1, 0, 0);
			me->mind = 1;
			me->mind1 = 60;	//	2 seconds, then spit again
			return;
		}
		break;
	}
}

void AI_BigBadger(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	BasicAI(me, SND_BEAROUCH, SND_BEARDIE, map, world, goodguy);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frm == 3)
			me->frmAdvance = 256;
		if (me->seq == ANIM_ATTACK && me->frm == 2 && me->reload == 0)
		{
			x = me->x + Cosine(me->facing * 32) * 20;
			y = me->y + Sine(me->facing * 32) * 20;
			if (me->type == MONS_GORKA2)
			{
				FireBullet(me->ID, me->x, me->y, FIXAMT * 40, me->facing * 32, BLT_ROCK2, DamageAmt(monsType[me->type].damage, me->team));
			}
			if (me->AttackCheck(20, x >> FIXSHIFT, y >> FIXSHIFT, goodguy))
			{
				meleeAttacker = me;
				goodguy->GetShot(Cosine(me->facing * 32) * 4, Sine(me->facing * 32) * 4, DamageAmt(monsType[me->type].damage, me->team), map, world);
			}
			me->reload = 15 + 30 * (me->type == MONS_GORKA2);
		}
		if (me->seq == ANIM_A1 && me->frm == 3 && me->reload == 0)
		{
			FireBullet(me->ID, me->x, me->y, FIXAMT * 60, 64, BLT_ROCK, DamageAmt(monsType[me->type].damage, me->team));
			me->reload = 30;
		}

		return;	// can't do nothin' right now
	}

	if (me->type == MONS_GORKA2)
		me->mind2 = 1;
	if (me->mind2 == 0)	// tossing things down at player
	{
		if (goodguy && goodguy->y < (me->y + FIXAMT * 50))// FIX: added goodguynull check
		{
			// goodguy got close, get mighty
			me->mind2 = 1;
			me->mind = 0;
			me->mind1 = 10;
			return;
		}
		switch (me->mind1)
		{
		case 0:	// not yet setup
			me->mind1 = (rand() % 3);
			me->mind = 30 + (rand() % (30 * 2));
			break;
		case 1:	// standing still
			me->facing = 2;
			if (me->seq != ANIM_IDLE)
				DoMove(me, ANIM_IDLE, 128, 0, 0, 0);
			break;
		case 2:	// walk left
			me->facing = 4;
			if (me->seq != ANIM_MOVE)
				DoMove(me, ANIM_MOVE, 128, 0, (Cosine(me->facing * 32) * monsType[me->type].speed) / FIXAMT, (Sine(me->facing * 32) * monsType[me->type].speed) / FIXAMT);
			break;
		case 3:	// walk right
			me->facing = 1;
			if (me->seq != ANIM_MOVE)
				DoMove(me, ANIM_MOVE, 128, 0, (Cosine(me->facing * 32) * monsType[me->type].speed) / FIXAMT, (Sine(me->facing * 32) * monsType[me->type].speed) / FIXAMT);
			break;
		}
		if (me->mind1 >= 1 && me->mind1 <= 3)
		{
			if (me->mind)
				me->mind--;
			else
			{
				// toss a rock, then start all over again
				me->mind1 = 0;
				me->facing = 2;
				MakeSound(SND_GORKATOSS, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
				DoMove(me, ANIM_A1, 128, 1, 0, 0);
			}
		}
	}
	else	// normal fighting mode
	{
		if (goodguy == NULL)	// FIX: This is added to fix the crash bug!
		{
			me->dx = 0;
			me->dy = 0;
			return;
		}
		if (me->type == MONS_GORKA2 && Random(32) == 0 && me->reload == 0)
		{
			// slash
			MakeSound(SND_GORKASLASH, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
			FaceGoodguy(me, goodguy);
			DoMove(me, ANIM_ATTACK, 128, 1, 0, 0);
			return;
		}

		if (me->type == MONS_BIGBADGER && Random(32) == 0 && map->CheckLOS(me->mapx, me->mapy, 4, goodguy->mapx, goodguy->mapy) && me->reload == 0)
		{
			// slash
			MakeSound(SND_GORKASLASH, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
			FaceGoodguy(me, goodguy);
			DoMove(me, ANIM_ATTACK, 128, 1, Cosine(me->facing * 32) * 6, Sine(me->facing * 32) * 6);
			return;
		}

		WanderAI(me, 600, 40, 3, map, world, goodguy);
	}
}

void AI_Bird(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	byte rightAng;
	bullet_t* b;

	if (player.amulet.magic == EM_FATCROW && (player.timeToFinish % 30) == 0)
	{
		bulletHittingType = BLT_NONE;
		bulletHittingOwner = 65535;	// AA bear fix
		meleeAttacker = goodguy;
		me->GetShot(0, 0, 100, map, world);	// fat crow amulet, useless everywhere else, instawin here
	}
	if (player.var[VAR_BIRDCHAT] == 0 && (levelDef[player.levelNum].flags & LF_ARENA) == 0)
	{
		me->dx = 0;
		if (me->y < FIXAMT * (TILE_HEIGHT + 10 * TILE_HEIGHT))
			me->dy = FIXAMT * 2;
		else
		{
			me->dy = 0;
			me->mind++;
			if (me->mind == 20)
			{
				BeginChatting(43);
				player.var[VAR_BIRDCHAT] = 1;
				me->mind = 0;
				me->mind2 = rand() % 3;	// battle mode
			}
		}
		return;
	}

	BasicAI(me, SND_STICKOUCH, SND_STICKDIE, map, world, goodguy);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frm == 7 && (levelDef[player.levelNum].flags & LF_ARENA) == 0)
		{
			me->frmAdvance = 256;
			BeginChatting(43);
			me->frm = 8;
		}

		return;	// can't do nothin' right now
	}

	if (!goodguy)
		return;

	if ((rand() % 100) == 0)
	{
		rightAng = me->mind2;
		me->mind2 = rand() % 3;
		if (me->mind2 != rightAng)	// switched modes
		{
			MakeSound(SND_CANTBUY, me->x, me->y, SND_CUTOFF, 1200);
			MakeColdRingParticle(me->x, me->y, 0, 60);
		}
	}

	// constant barrage
	switch (me->mind2)
	{
	case 0:	// ice mode
		if (!(rand() % 50))
			FireBullet(me->ID, me->x, me->y, 0, 0, BLT_ICEMINE, DamageAmt(monsType[me->type].damage / 2, me->team));
		break;
	case 1:	// fire mode
		if (!(rand() % 5))
			FireBullet(me->ID, me->x, me->y, 0, 0, BLT_FLAMEB, FlameDmg(1, me->team));
		break;
	case 2:	// earth mode
		if (!(rand() % 20))
			FireBullet(me->ID, me->x, me->y, 0, 0, BLT_EVILSPIKE2, DamageAmt(monsType[me->type].damage / 2, me->team));
		break;
	}

	rightAng = AngleFrom(me->x, me->y, goodguy->x, goodguy->y);
	// aimed projectile
	switch (me->mind2)
	{
	case 0:	// ice mode
		if (!(rand() % 10))
		{
			MakeSound(SND_CRYSTALZAP, me->x, me->y, SND_CUTOFF, 1200);
			FireBullet(me->ID, me->x, me->y, FIXAMT * 30, rightAng, BLT_CRYO2, DamageAmt(monsType[me->type].damage, me->team));
		}
		break;
	case 1:	// fire mode
		if (!(rand() % 20))
		{
			MakeSound(SND_PLANTSHOOT, me->x, me->y, SND_CUTOFF, 1200);
			b = FireBullet(me->ID, me->x, me->y, FIXAMT * 30, rightAng, BLT_GRENADE, DamageAmt(monsType[me->type].damage, me->team));
			if (b)
				b->speed = FIXAMT * 2 + (rand() % (FIXAMT * 4 + 1));
		}
		break;
	case 2:	// earth mode
		// no need!  spikes cover it
		break;
	}
	if (me->mind1)
	{
		me->mind1--;
	}
	else
	{
		switch (rand() % 8)
		{
		case 0:
			me->mind = 1;
			break;
		case 1:
			me->mind = 2;
			break;
		case 2:
			me->mind = 3;
			break;
		default:
			me->mind = 0;
			break;
		}
		me->mind1 = 1 + (rand() % 20);
	}

	if (me->mind == 0)	// heading straight at guy
	{
		rightAng = AngleFrom(me->x, me->y, goodguy->x, goodguy->y);
	}
	else if (me->mind == 1)	// turning right
	{
		rightAng = me->mind3 + 10;
	}
	else if (me->mind == 2)	// turning left
	{
		rightAng = me->mind3 - 10;
	}
	else if (me->mind == 3)	// going straight
	{
		rightAng = me->mind3;
	}

	me->mind3 = TurnToward(me->mind3, rightAng, 3);

	me->facing = ((me->mind3 + 16) / 32) & 7;
	me->dx = Cosine(me->mind3) * monsType[me->type].speed / FIXAMT;
	me->dy = Sine(me->mind3) * monsType[me->type].speed / FIXAMT;

	if (me->seq != ANIM_MOVE)
		DoMove(me, ANIM_MOVE, 128, 0, me->dx, me->dy);
}

void AI_Killbor(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	BasicAI(me, SND_KILLBOROUCH, SND_KILLBORDIE, map, world, goodguy);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE)
		{
			// AA remove champion var
			me->facing = 2;
		}

		if (me->seq == ANIM_DIE && me->frm == 3)
			me->frmAdvance = 256;
		if (me->seq == ANIM_ATTACK && me->frm == 2 && me->reload == 0)
		{
			x = me->x + Cosine(me->facing * 32) * 60;
			y = me->y + Sine(me->facing * 32) * 60;
			if (me->AttackCheck(80, x >> FIXSHIFT, y >> FIXSHIFT, goodguy))
			{
				meleeAttacker = me;
				goodguy->GetShot(Cosine(me->facing * 32) * 4, Sine(me->facing * 32) * 4, DamageAmt(monsType[me->type].damage, me->team), map, world);
			}
			me->reload = 15;
		}
		if (me->seq == ANIM_A1 && me->frm >= 1 && me->frm <= 4 && me->reload == 0)
		{
			Guy* g;

			g = AddGuy(me->x, me->y, 0, MONS_BOKBOK2);
			if (g)
			{
				g->team = EVIL;
				g->mind2 = monsType[me->type].damage;
				g->tag = 255;
				g->mind3 = rand() % 256;
			}
		}

		return;	// can't do nothin' right now
	}

	if (Random(64) == 0 && me->reload == 0)
	{
		// bokboks!
		MakeSound(SND_KILLBORSPELL, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
		FaceGoodguy(me, goodguy);
		DoMove(me, ANIM_A1, 128, 1, 0, 0);
		return;
	}
	if (Random(10) == 0 && map->CheckLOS(me->mapx, me->mapy, 6, goodguy->mapx, goodguy->mapy) && me->reload == 0)
	{
		// slash
		MakeSound(SND_KILLBORSLASH, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
		FaceGoodguy(me, goodguy);
		DoMove(me, ANIM_ATTACK, 180, 1, Cosine(me->facing * 32) * 6, Sine(me->facing * 32) * 6);
		return;
	}

	WanderAI(me, 600, 40, 3, map, world, goodguy);
}

void AI_ToyBadger(Guy* me, Map* map, world_t* world, Guy* badguy)
{
	byte rightAng;
	int x, y;

	if (!goodguy)
		return;

	BasicAI(me, 0, 0, map, world, goodguy);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK && me->frm == 2 && me->reload == 0)
		{
			x = me->x + Cosine(me->facing * 32) * 10;
			y = me->y + Sine(me->facing * 32) * 10;
			if (me->AttackCheck(10, x >> FIXSHIFT, y >> FIXSHIFT, badguy))
			{
				meleeAttacker = me;
				badguy->GetShot(Cosine(me->facing * 32), Sine(me->facing * 32), DamageAmt(monsType[me->type].damage, me->team), map, world);
			}
			me->reload = 15;
		}
		return;	// can't do nothin' right now
	}

	if (badguy && me->reload == 0 && (rand() % 30) == 0 && RangeToTarget(me, badguy) < FIXAMT * 30)
	{
		MakeSound(SND_BADGERSLASH, me->x, me->y, SND_CUTOFF | SND_RANDOM, 400);
		DoMove(me, ANIM_ATTACK, 200, 1, 0, 0);
		return;
	}

	if (me->mind1)
	{
		me->mind1--;
	}
	else
	{
		if (RangeToTarget(me, goodguy) < FIXAMT * 50)
		{
			me->mind = 1 + (rand() % 2);
			me->mind1 = 10 + (rand() % 20);
		}
		else if (RangeToTarget(me, goodguy) > FIXAMT * 600)	// off screen, teleport in
		{
			me->x = goodguy->x;
			me->y = goodguy->y;
			me->z = FIXAMT * 600;
			DoMove(me, ANIM_ATTACK, 128, 1, 0, 0);
		}
		else
		{
			me->mind = 0;
			me->mind1 = 10 + (rand() % 20);
		}
	}

	if (me->mind == 0)	// heading straight at guy
	{
		rightAng = AngleFrom(me->x, me->y, goodguy->x, goodguy->y);
		me->mind3 = TurnToward(me->mind3, rightAng, 6);
	}
	else if (me->mind == 1)	// turning right
	{
		rightAng = me->mind3 + 10;
		me->mind3 = TurnToward(me->mind3, rightAng, 4);

	}
	else if (me->mind == 2)	// turning left
	{
		rightAng = me->mind3 - 10;
		me->mind3 = TurnToward(me->mind3, rightAng, 4);
	}
	else if (me->mind == 3)	// go straight
	{
		rightAng = me->mind3;
	}
	if (me->mind2 == 1 && RangeToTarget(me, goodguy) > FIXAMT * 100)
		me->mind3 = TurnToward(me->mind3, rightAng, 3);

	me->facing = ((me->mind3 + 16) / 32) & 7;
	x = RangeToTarget(me, goodguy);
	if (x > FIXAMT * 200)
		x = FIXAMT * 200;
	if (x < FIXAMT * 10)
		x = FIXAMT * 10;
	y = monsType[me->type].speed * x / (FIXAMT * 200);
	me->dx = Cosine(me->mind3) * y / FIXAMT;
	me->dy = Sine(me->mind3) * y / FIXAMT;

	y = 240 * x / (FIXAMT * 200);
	if (me->seq != ANIM_MOVE)
		DoMove(me, ANIM_MOVE, y, 0, me->dx, me->dy);
	else
		me->frmAdvance = y;
}

void AI_GhostEvil(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;
	byte rightAng;

	BasicAI(me, 0, SND_GHOSTDIE, map, world, goodguy);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->ouch)
			ParticleBoom(PART_SMOKESINGLE, me->x, me->y, me->z);

		if (me->seq == ANIM_ATTACK && me->frm == 2 && me->reload == 0)
		{
			me->reload = 30;
			if (goodguy)
			{
				x = me->x + Cosine(me->facing * 32) * 12;
				y = me->y + Sine(me->facing * 32) * 12;
				if (me->AttackCheck(16, x >> FIXSHIFT, y >> FIXSHIFT, goodguy))
				{
					meleeAttacker = me;

					goodguy->GetShot(Cosine(me->facing * 32) * 1, Sine(me->facing * 32) * 1, DamageAmt(monsType[me->type].damage, me->team), map, world);
				}
			}
		}

		return;	// can't do nothin' right now
	}

	if (goodguy && me->reload == 0 && map->CheckLOS(me->mapx, me->mapy, 2, goodguy->mapx, goodguy->mapy) && RangeToTarget(me, goodguy) < FIXAMT * 20)
	{
		DoMove(me, ANIM_ATTACK, 128, 1, 0, 0);
		return;
	}

	if (me->mind == 0)	// hunting
	{
		if (goodguy)
		{
			rightAng = AngleFrom(me->x, me->y, goodguy->x, goodguy->y);
			me->mind3 = TurnToward(me->mind3, rightAng, 6);

			me->facing = ((me->mind3 + 16) / 32) & 7;
			me->dx = Cosine(me->mind3) * monsType[me->type].speed / FIXAMT;
			me->dy = Sine(me->mind3) * monsType[me->type].speed / FIXAMT;
		}
		if (!me->mind1)
		{
			me->mind = rand() % 4;
			if (me->mind != 1)
				me->mind = 0;
			me->mind1 = 10 + (rand() % 30);
			me->mind2 = rand() % 256;
		}
		else
			me->mind1--;
	}
	else	// random heading
	{
		me->mind3 = TurnToward(me->mind3, me->mind2, 6);

		me->facing = ((me->mind3 + 16) / 32) & 7;
		me->dx = Cosine(me->mind3) * monsType[me->type].speed / FIXAMT;
		me->dy = Sine(me->mind3) * monsType[me->type].speed / FIXAMT;

		if (!me->mind1)
		{
			me->mind = rand() % 4;
			if (me->mind != 1)
				me->mind = 0;
			me->mind1 = 10 + (rand() % 30);
			me->mind2 = rand() % 256;
		}
		else
			me->mind1--;
	}
}

void AI_HotDog(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	BasicAI(me, SND_HOUNDOUCH, SND_HOUNDDIE, map, world, goodguy);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frm == 4)
		{
			me->frmAdvance = 256;
		}
		if (me->seq == ANIM_ATTACK && me->frm >= 2 && me->frm <= 5)
		{
			byte f;

			f = me->facing * 32 - 10 + (rand() % 21);
			x = me->x + Cosine(me->facing * 32) * 64;
			y = me->y + Sine(me->facing * 32) * 64;
			FireBullet(me->ID, x, y, FIXAMT * 30, f, BLT_FLAMETRAIL, DamageAmt(monsType[me->type].damage, me->team));
			me->reload = 60;
		}
		return;	// can't do nothin' right now
	}

	// fire flames at medium range
	if (Random(20) == 0 && map->CheckLOS(me->mapx, me->mapy, 10, goodguy->mapx, goodguy->mapy) && me->reload == 0)
	{
		DoMove(me, ANIM_ATTACK, 64, 1, 0, 0);
		MakeSound(SND_HOUNDSPIT, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
		return;
	}

	WanderAI(me, 64, 40, 3, map, world, goodguy);
}

void AI_Icebox(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;
	byte rightAng;

	BasicAI(me, SND_PUPPETOUCH, SND_PUPPETDIE, map, world, goodguy);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frm == 5)
		{
			me->frmAdvance = 256;
		}
		if (me->seq == ANIM_ATTACK && me->frm == 2 && me->reload == 0)
		{
			x = me->x + Cosine(me->facing * 32) * 16;
			y = me->y + Sine(me->facing * 32) * 16;
			FireBullet(me->ID, x, y, FIXAMT * 25, me->facing * 32, BLT_CRYO2, DamageAmt(monsType[me->type].damage, me->team));
			FireBullet(me->ID, x, y, FIXAMT * 25, me->facing * 32 - 16, BLT_CRYO2, DamageAmt(monsType[me->type].damage, me->team));
			FireBullet(me->ID, x, y, FIXAMT * 25, me->facing * 32 + 16, BLT_CRYO2, DamageAmt(monsType[me->type].damage, me->team));
			me->reload = 15;
		}
		return;	// can't do nothin' right now
	}

	if (!goodguy)
		return;

	if (Random(20) == 0 && map->CheckLOS(me->mapx, me->mapy, 10, goodguy->mapx, goodguy->mapy) && me->reload == 0)
	{
		FaceGoodguy(me, goodguy);
		DoMove(me, ANIM_ATTACK, 128, 1, 0, 0);
		return;
	}

	if (me->mind1)
	{
		me->mind1--;
	}
	else
	{
		switch (rand() % 8)
		{
		case 0:
			me->mind = 1;
			break;
		case 1:
			me->mind = 2;
			break;
		case 2:
			me->mind = 3;
			break;
		default:
			me->mind = 0;
			break;
		}
		me->mind1 = 1 + (rand() % 20);
	}

	if (me->mind == 0)	// heading straight at guy
	{
		rightAng = AngleFrom(me->x, me->y, goodguy->x, goodguy->y);
	}
	else if (me->mind == 1)	// turning right
	{
		rightAng = me->mind3 + 10;
	}
	else if (me->mind == 2)	// turning left
	{
		rightAng = me->mind3 - 10;
	}
	else if (me->mind == 3)	// going straight
	{
		rightAng = me->mind3;
	}

	me->mind3 = TurnToward(me->mind3, rightAng, 3);

	me->facing = ((me->mind3 + 16) / 32) & 7;
	me->dx = Cosine(me->mind3) * monsType[me->type].speed / FIXAMT;
	me->dy = Sine(me->mind3) * monsType[me->type].speed / FIXAMT;

	if (me->seq != ANIM_MOVE)
		DoMove(me, ANIM_MOVE, 128, 0, me->dx, me->dy);
}

void AI_Deathberry(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	BasicAI(me, SND_PLANTOUCH, SND_PLANTDIE, map, world, goodguy);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK && me->frm == 2 && me->reload == 0)
		{
			me->reload = 10;
			FireBullet(me->ID, me->x, me->y, FIXAMT * 15, (byte)rand() % 256, BLT_BERRY2, 0);
		}
		return;	// can't do nothin' right now
	}

	if (me->reload == 0 && (rand() % 60) == 0)
	{
		DoMove(me, ANIM_ATTACK, 128, 1, 0, 0);
		MakeSound(SND_BERRYSPIT, me->x, me->y, SND_CUTOFF | SND_RANDOM, 500);
	}
}

void AI_FruitPunch(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	BasicAI(me, SND_PUPPETOUCH, SND_PUPPETDIE, map, world, goodguy);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frm == 4)
			me->frmAdvance = 256;
		if (me->seq == ANIM_ATTACK && me->frm >= 4 && me->frm <= 7 && me->reload == 0)
		{
			x = me->x + Cosine(me->facing * 32) * 16;
			y = me->y + Sine(me->facing * 32) * 16;
			if (me->AttackCheck(16, x >> FIXSHIFT, y >> FIXSHIFT, goodguy))
			{
				meleeAttacker = me;

				goodguy->GetShot(Cosine(me->facing * 32) * 10, Sine(me->facing * 32) * 10, DamageAmt(monsType[me->type].damage, me->team), map, world);
				me->reload = 20;
			}
			if (me->frm == 7)
				me->reload = 20;
		}

		return;	// can't do nothin' right now
	}

	// punch when you get close
	if (Random(10) == 0 && map->CheckLOS(me->mapx, me->mapy, 2, goodguy->mapx, goodguy->mapy) && me->reload == 0)
	{
		MakeSound(SND_PUPPETPUNCH, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
		FaceGoodguy(me, goodguy);
		me->seq = ANIM_ATTACK;
		me->frm = 0;
		me->frmTimer = 0;
		me->frmAdvance = 128;
		me->action = ACTION_BUSY;
		me->dx = Cosine(me->facing * 32) * 8;
		me->dy = Sine(me->facing * 32) * 8;
		return;
	}
	WanderAI(me, 64, 40, 3, map, world, goodguy);
}

void AI_RedBaron(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	byte rightAng;

	BasicAI(me, SND_BARONOUCH, SND_PUPPETDIE, map, world, goodguy);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK && me->reload == 0)
		{
			Guy* g;

			map->BrightTorch(me->mapx, me->mapy, 24, 4);
			g = SummonGuy(me->x, me->y, FIXAMT * 200, MONS_BADGHOST2);
			if (g)
				g->tag = 255;

			me->reload = 6;
		}
		return;	// can't do nothin' right now
	}

	if (!goodguy)
		return;

	if (me->mind2 == 0)
	{
		FireBullet(me->ID, me->x, me->y, FIXAMT * 20, me->facing * 32, BLT_ICEMINE, DamageAmt(monsType[me->type].damage, me->team));
		me->mind2 = 30 * 3;

	}
	else
		me->mind2--;

	if (me->mind2 == 30)
	{
		FaceGoodguy(me, goodguy);
		DoMove(me, ANIM_ATTACK, 128, 1, 0, 0);
		return;
	}

	if (me->wallBump & 1)
	{
		if (me->mind3 < 128)
		{
			me->mind3 = (128 - me->mind3);
		}
		else
		{
			me->mind3 = (byte)(384 - me->mind3);
		}
	}
	if (me->wallBump & 2)
	{
		me->mind3 = 256 - me->mind3;
	}

	me->wallBump = 0;

	if (me->mind1)
	{
		me->mind1--;
	}
	else
	{
		switch (rand() % 8)
		{
		case 0:
			me->mind = 1;
			break;
		case 1:
			me->mind = 2;
			break;
		case 2:
			me->mind = 3;
			break;
		default:
			me->mind = 0;
			break;
		}
		me->mind1 = 1 + (rand() % 20);
	}

	if (me->mind == 0)	// heading straight (doesn't care where you are anyway!)
	{
		rightAng = me->mind3;
	}
	else if (me->mind == 1)	// turning right
	{
		rightAng = me->mind3 + 10;
	}
	else if (me->mind == 2)	// turning left
	{
		rightAng = me->mind3 - 10;
	}
	else if (me->mind == 3)	// going straight
	{
		rightAng = me->mind3;
	}

	me->mind3 = TurnToward(me->mind3, rightAng, 3);

	me->facing = ((me->mind3 + 16) / 32) & 7;
	me->dx = Cosine(me->mind3) * monsType[me->type].speed / FIXAMT;
	me->dy = Sine(me->mind3) * monsType[me->type].speed / FIXAMT;

	if (me->seq != ANIM_MOVE)
		DoMove(me, ANIM_MOVE, 128, 0, me->dx, me->dy);
}

void AI_Jack(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (goodguy == NULL)
	{
		me->dx = 0;
		me->dy = 0;
		return;
	}

	BasicAI(me, SND_THUGOUCH, SND_THUGDIE, map, world, goodguy);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frm == 9)
			me->frmAdvance = 256;
		if ((me->seq == ANIM_ATTACK || me->seq == ANIM_DIE) && me->frm == 2 && me->reload == 0)
		{
			MakeSound(SND_PLANTSHOOT, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
			x = me->x + Cosine(me->facing * 32) * 16;
			y = me->y + Sine(me->facing * 32) * 16;

			FireBullet(me->ID, x, y, FIXAMT * 30, me->facing * 32, BLT_BADBURNIN, DamageAmt(monsType[me->type].damage, me->team));
			me->reload = 90;
		}

		return;	// can't do nothin' right now
	}

	// fire supergun when reloaded
	if (Random(10) == 0 && me->reload == 0)
	{
		MakeSound(SND_JUNKCOCK, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
		FaceGoodguy(me, goodguy);
		DoMove(me, ANIM_ATTACK, 128, 1, 0, 0);
		return;
	}
	WanderAI(me, 64, 40, 3, map, world, goodguy);
}

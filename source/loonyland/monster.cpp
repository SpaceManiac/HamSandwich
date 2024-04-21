#include "monster.h"
#include "player.h"
#include "options.h"
#include "bullet.h"
#include "quest.h"
#include "badge.h"
#include "bossbash.h"

monsterType_t monsType[NUM_MONSTERS]=
	{
		{"Null",{255,255,0},	// these numbers are {color to replace, color to replace with,brightness modifier}
		 0,0,0,0,0,0,"",0},
		{"Loony",{255,255,0},
		 0,11,25,128,0,0,"graphics/kidloony.jsp",0,MF_WATERWALK,
			{
				{0,255},	// idle
				{1,2,3,4,3,2,1,0,5,6,7,8,7,6,5,0,255},	// move
				{9,10,11,12,13,14,15,16,255},	// attack
				{0,0,0,0,0,0,17,18,19,20,255},		// die
				{9,11,13,15,16,255},	// use item
				{21,21,22,22,23,23,24,24,24,24,24,24,5,1,255},	// bored
				{2,4,2,0,6,8,6,0,2,4,2,0,0,0,18,19,20,255},	// watery death
			}},
		{"Bonehead",{255,255,0},
		 3,8,32,10,1,4*FIXAMT,"graphics/bonehead.jsp",0,0,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{12,13,14,15,16,17,18,19,255},	// attack
				{7,8,9,10,11,11,11,11,254,11,254,11,254,11,254,11,254,11,254,11,254,11,255},		// die
				{20,21,22,23,24,25,255},	// chomp/spit projectile
				{26,27,27,27,26,255},	// point at bouapha
				{11,10,9,8,7,255},	// resurrect=A3
				{28,29,30,31,30,29,28,255},	// leap skyward
			}},
		{"Redbone",{0,4,-6},
		 5,8,32,15,2,4*FIXAMT,"!2",0,0,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{12,13,14,15,16,17,18,19,255},	// attack
				{7,8,9,10,11,11,11,11,254,11,254,11,254,11,254,11,254,11,254,11,254,11,255},		// die
				{20,21,22,23,24,25,255},	// chomp/spit projectile
				{26,27,27,27,26,255},	// point at bouapha
				{11,10,9,8,7,255},	// resurrect=A3
				{28,29,30,31,30,29,28,255},	// leap skyward
			}},
		{"Gangrene",{0,1,-4},
		 8,8,32,20,2,5*FIXAMT,"!2",0,0,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{12,13,14,15,16,17,18,19,255},	// attack
				{7,8,9,10,11,11,11,11,254,11,254,11,254,11,254,11,254,11,254,11,254,11,255},		// die
				{20,21,22,23,24,25,255},	// chomp/spit projectile
				{26,27,27,27,26,255},	// point at bouapha
				{11,10,9,8,7,255},	// resurrect=A3
				{28,29,30,31,30,29,28,255},	// leap skyward
			}},
		{"Numbskull",{0,5,-5},
		 15,8,32,50,3,3*FIXAMT,"!2",0,0,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{12,13,14,15,16,17,18,19,255},	// attack
				{7,8,9,10,11,11,11,11,254,11,254,11,254,11,254,11,254,11,254,11,254,11,255},		// die
				{20,21,22,23,24,25,255},	// chomp/spit projectile
				{26,27,27,27,26,255},	// point at bouapha
				{11,10,9,8,7,255},	// resurrect=A3
				{28,29,30,31,30,29,28,255},	// leap skyward
			}},
		{"Zombie",{255,255,0},
		 4,12,42,20,2,2*FIXAMT,"graphics/zombie.jsp",0,0,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{15,16,17,18,19,20,21,22,23,23,24,25,26,27,28,255},	// attack
				{29,30,31,32,33,34,35,36,37,38,39,40,41,255},		// die
				{7,8,9,10,11,12,13,14,255},	// A1 = stomp ground
			}},
		{"Bombie",{0,4,0},	// kamikaze exploding zombie
		 6,12,42,10,3,4*FIXAMT,"!6",0,0,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{15,16,17,18,19,20,21,22,23,23,24,25,26,27,28,255},	// attack
				{29,30,31,32,33,34,35,36,37,38,39,40,41,255},		// die
				{7,8,9,10,11,12,13,14,255},	// A1 = stomp ground
			}},
		{"Zombie Lord",{0,6,2},
		 30,12,42,100,4,1*FIXAMT,"!6",0,0,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{15,16,17,18,19,20,21,22,23,23,24,25,26,27,28,255},	// attack
				{29,30,31,32,33,34,35,36,37,38,39,40,41,255},		// die
				{7,8,9,10,11,12,13,14,255},	// A1 = stomp ground
			}},
		{"Villager",{255,255,0},
		 0,12,14,1000,1,2*FIXAMT,"graphics/villager.jsp",0,MF_INVINCIBLE,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{0,255},	// attack
				{0,255},		// die
				{7,8,9,10,11,12,13,12,11,10,11,12,13,12,11,10,9,8,7,255},	// A1 = scratch head
			}},
		{"Villager",{5,2,0},
		 0,12,14,1000,1,2*FIXAMT,"!9",0,MF_INVINCIBLE,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{0,255},	// attack
				{0,255},		// die
				{7,8,9,10,11,12,13,12,11,10,11,12,13,12,11,10,9,8,7,255},	// A1 = scratch head
			}},
		{"Villager",{4,1,0},
		 0,12,14,1000,1,2*FIXAMT,"!9",0,MF_INVINCIBLE,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{0,255},	// attack
				{0,255},		// die
				{7,8,9,10,11,12,13,12,11,10,11,12,13,12,11,10,9,8,7,255},	// A1 = scratch head
			}},
		{"Villager",{255,255,0},
		 0,12,12,1000,1,2*FIXAMT,"graphics/villager2.jsp",0,MF_INVINCIBLE,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{0,255},	// attack
				{0,255},		// die
				{7,8,9,10,11,10,9,10,11,10,9,8,7,255},	// A1 = yawn
			}},
		{"Villager",{4,3,0},
		 0,12,12,1000,1,2*FIXAMT,"!12",0,MF_INVINCIBLE,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{0,255},	// attack
				{0,255},		// die
				{7,8,9,10,11,10,9,10,11,10,9,8,7,255},	// A1 = yawn
			}},
		{"Villager",{4,5,-2},
		 0,12,12,1000,1,2*FIXAMT,"!12",0,MF_INVINCIBLE,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{0,255},	// attack
				{0,255},		// die
				{7,8,9,10,11,10,9,10,11,10,9,8,7,255},	// A1 = yawn
			}},
		{"Ribbit",{255,255,0},
		 1,6,22,1,1,3*FIXAMT,"graphics/frog.jsp",0,0,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,8,0,255},	// move
				{9,10,11,12,13,14,15,16,255},	// attack
				{17,18,19,20,21,255},		// die
			}},
		{"Croaker",{1,6,-3},
		 3,6,22,6,1,3*FIXAMT,"!15",0,0,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,8,0,255},	// move
				{9,10,10,10,10,9,255},	// attack
				{17,18,19,20,21,255},		// die
			}},
		{"Toad",{1,4,2},
		 2,6,22,1,1,4*FIXAMT,"!15",0,0,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,8,0,255},	// move
				{9,9,9,9,255},	// attack
				{17,18,19,20,21,255},		// die
			}},
		{"Rag Man",{255,255,0},
		 5,11,28,10,1,4*FIXAMT,"graphics/mummy.jsp",0,0,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,255},	// attack
				{19,20,21,22,23,24,25,26,27,255},		// die
				{15,16,17,18,18,17,16,15,255},	// A1 = gem zap
				{7,8,9,9,9,9,9,8,7,255},	// A2 = magic hands
				{27,26,25,24,23,22,21,20,19,255},	// A3 = resurrect
			}},
		{"Mummy",{6,1,0},
		 8,11,28,30,1,1*FIXAMT,"!18",0,0,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,255},	// attack
				{19,20,21,22,23,24,25,26,27,255},		// die
				{15,16,17,18,18,17,16,15,255},	// A1 = gem zap
				{7,8,9,9,9,9,9,8,7,255},	// A2 = magic hands
				{27,26,25,24,23,22,21,20,19,255},	// A3 = resurrect
			}},
		{"Pharaoh Faucet",{0,2,0},
		 15,11,28,60,2,2*FIXAMT,"!18",0,0,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,255},	// attack
				{19,20,21,22,23,24,25,26,27,255},		// die
				{15,16,17,18,18,17,16,15,255},	// A1 = gem zap
				{7,8,9,9,9,9,9,8,7,255},	// A2 = magic hands
			}},
		{"Grand Pharaoh",{0,7,3},
		 25,11,28,150,3,1*FIXAMT,"!18",0,0,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,255},	// attack
				{19,20,21,22,23,24,25,26,27,255},		// die
				{15,16,17,18,18,17,16,15,255},	// A1 = gem zap
				{7,8,9,9,9,9,9,8,7,255},	// A2 = magic hands
			}},
		{"Terror Tree",{255,255,0},
		 3,32,24,10,1,0,"graphics/eviltree.jsp",0,MF_NOMOVE|MF_ONEFACE,
			{
				{4,5,6,7,8,9,10,11,12,13,255},	// idle
				{0,255},	// move
				{0,1,2,3,1,255},	// attack
				{14,15,16,17,18,19,20,21,22,23,255},		// die
			}},
		{"Swamp Stump",{4,1,0},
		 0,32,24,5000,1,0,"!22",0,MF_NOMOVE|MF_ONEFACE|MF_INVINCIBLE,
			{
				{4,5,6,7,8,9,10,11,12,13,255},	// idle
				{0,255},	// move
				{0,1,2,3,1,255},	// attack
				{14,15,16,17,18,19,20,21,22,23,255},		// die
			}},
		{"Dingbat",{255,255,0},
		 5,8,33,5,1,8*FIXAMT,"graphics/bat.jsp",0,MF_FLYING,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{11,12,13,12,11,255},	// attack
				{23,24,25,26,27,28,29,30,31,32,255},		// die
				{14,15,16,16,16,16,15,14,255},	// diving attack =A1
				{17,18,19,20,21,22,255},	// bounce off during dive =A2
				{7,8,9,10,9,8,7,255}		// shoot = A3
			}},
		{"Gargoyle",{2,0,0},
		 8,8,33,50,1,5*FIXAMT,"!24",0,MF_FLYING,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{11,12,13,12,11,255},	// attack
				{23,24,25,26,27,28,29,30,31,32,255},		// die
				{14,15,16,16,16,16,15,14,255},	// diving attack =A1
				{17,18,19,20,21,22,255},	// bounce off during dive =A2
				{7,8,9,10,9,8,7,255}		// shoot = A3
			}},
		{"Vamp Bat",{2,4,-6},
		 15,8,33,10,2,6*FIXAMT,"!24",0,MF_FLYING,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{11,12,13,12,11,255},	// attack
				{23,24,25,26,27,28,29,30,31,32,255},		// die
				{14,15,16,16,16,16,15,14,255},	// diving attack =A1
				{17,18,19,20,21,22,255},	// bounce off during dive =A2
				{7,8,9,10,9,8,7,255}		// shoot = A3
			}},
		{"Dogboy",{255,255,0},
		 6,12,55,15,1,6*FIXAMT,"graphics/wolfman.jsp",0,0,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{30,31,32,32,32,33,34,35,36,37,38,39,40,41,42,43,255},	// attack (pounce)
				{44,45,46,47,48,49,50,51,51,51,52,53,54,254,54,254,54,254,54,255},		// die
				{7,8,9,9,8,7,255},	// spit projectile = A1
				{10,11,12,12,12,13,14,15,16,17,17,18,19,255},	// charging slash = A2
				{20,21,22,23,24,25,26,26,26,26,26,26,26,26,27,28,29,255}		// howl = A3
			}},
		{"Wolfman",{3,4,-3},
		 14,12,55,25,1,4*FIXAMT,"!27",0,0,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{30,31,32,32,32,33,34,35,36,37,38,39,40,41,42,43,255},	// attack (pounce)
				{44,45,46,47,48,49,50,51,51,51,52,53,54,254,54,254,54,254,54,255},		// die
				{7,8,9,9,8,7,255},	// spit projectile = A1
				{10,11,12,12,12,13,14,15,16,17,17,18,19,255},	// charging slash = A2
				{20,21,22,23,24,25,26,26,26,26,26,26,26,26,27,28,29,255}		// howl = A3
			}},
		{"Dire Wolf",{2,0,0},
		 20,12,55,40,2,5*FIXAMT,"!27",0,0,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{30,31,32,32,32,33,34,35,36,37,38,39,40,41,42,43,255},	// attack (pounce)
				{44,45,46,47,48,49,50,51,51,51,52,53,54,254,54,254,54,254,54,255},		// die
				{7,8,9,9,8,7,255},	// spit projectile = A1
				{10,11,12,12,12,13,13,14,14,15,15,16,16,17,17,17,17,18,19,255},	// charging slash = A2
				{20,21,22,23,24,25,26,26,26,26,26,26,26,26,27,28,29,255}		// howl = A3
			}},
		{"Larry",{255,255,0},
		 150,25,38,150,3,7*FIXAMT,"graphics/larry.jsp",0,MF_NOMOVE|MF_INVINCIBLE,
			{
				{0,255},	// idle
				{1,2,3,3,2,1,0,4,5,6,6,5,4,0,255},	// move
				{25,26,27,27,28,29,30,31,32,33,33,33,34,35,36,37,255},	// attack (pounce)
				{0,0,0,0,0,255},		// die
				{7,8,9,9,10,11,12,13,14,15,16,17,255},	// fire darts=A1
				{18,19,20,21,22,23,23,23,23,24,24,255},		// howl = A2
				{0,255},	// blank = A3
			}},
		{"Swampdog",{255,255,0},
		 6,18,43,20,1,1*FIXAMT,"graphics/swampdog.jsp",0,0,
			{
				{11,255},	// idle
				{12,13,14,13,12,11,15,16,17,16,15,255},	// move
				{20,21,22,23,23,22,21,20,255},	// attack (eye jab)
				{33,34,35,36,37,38,39,40,41,42,255},		// die
				{18,19,19,19,255},	// perk up = A1
				{1,2,3,4,5,6,7,8,9,10,0,255},	// charge = A2
				{24,25,26,27,28,29,30,31,32,255},		// eyebonk/shoot = A3
				{19,18,18,255},	// get bored = A4
				{0,0,0,0,255},	// hyper idle = A5
			}},
		{"Bog Beast",{5,1,-3},
		 10,12,43,40,2,2*FIXAMT,"!31",0,0,
			{
				{11,255},	// idle
				{12,13,14,13,12,11,15,16,17,16,15,255},	// move
				{20,21,22,23,23,22,21,20,255},	// attack (eye jab)
				{33,34,35,36,37,38,39,40,41,42,255},		// die
				{18,19,19,19,255},	// perk up = A1
				{1,2,3,4,5,6,7,8,9,10,0,255},	// charge = A2
				{24,25,26,27,28,29,30,31,32,255},		// eyebonk/shoot = A3
				{19,18,18,255},	// get bored = A4
				{0,0,0,0,255},	// hyper idle = A5
			}},
		{"Shambling Thing",{5,3,0},
		 15,12,43,70,3,1*FIXAMT,"!31",0,0,
			{
				{11,255},	// idle
				{12,13,14,13,12,11,15,16,17,16,15,255},	// move
				{20,21,22,23,23,22,21,20,255},	// attack (eye jab)
				{33,34,35,36,37,38,39,40,41,42,255},		// die
				{18,19,19,19,255},	// perk up = A1
				{1,2,3,4,5,6,7,8,9,10,0,255},	// charge = A2
				{24,25,26,27,28,29,30,31,32,255},		// eyebonk/shoot = A3
				{19,18,18,255},	// get bored = A4
				{0,0,0,0,255},	// hyper idle = A5
			}},
		{"Ghost",{255,255,0},
		 6,14,23,15,1,3*FIXAMT,"graphics/ghost.jsp",0,MF_FLYING|MF_GHOST|MF_WALLWALK|MF_FREEWALK|MF_ENEMYWALK|MF_NOSHADOW,
			{
				{0,255},	// idle
				{0,0,255},	// move
				{4,5,6,12,13,14,15,16,17,255},	// attack (quick punch)
				{18,19,20,21,22,255},		// die
				{18,19,20,21,22,21,20,19,18,255},	// teleport = A1
				{4,5,6,7,8,9,10,11,6,7,8,9,10,11,6,7,8,9,10,11,12,13,14,14,14,15,15,16,17,255},	// super punch = A2
				{1,2,3,3,2,3,2,3,3,3,2,2,1,1,255},		// scream = A3
			}},
		{"Shade",{255,255,-12},
		 13,14,23,40,1,5*FIXAMT,"!34",0,MF_FLYING|MF_GHOST|MF_WALLWALK|MF_FREEWALK|MF_ENEMYWALK|MF_NOSHADOW,
			{
				{0,255},	// idle
				{0,0,255},	// move
				{4,5,6,12,13,14,15,16,17,255},	// attack (quick punch)
				{18,19,20,21,22,255},		// die
				{18,19,20,21,22,21,20,19,18,255},	// teleport = A1
				{4,5,6,7,8,9,10,11,6,7,8,9,10,11,6,7,8,9,10,11,12,13,14,14,14,15,15,16,17,255},	// super punch = A2
				{1,2,3,3,2,3,2,3,3,3,2,2,1,1,255},		// scream = A3
			}},
		{"Banshee",{255,255,10},
		 20,14,23,80,2,4*FIXAMT,"!34",0,MF_FLYING|MF_GHOST|MF_WALLWALK|MF_FREEWALK|MF_ENEMYWALK|MF_NOSHADOW,
			{
				{0,255},	// idle
				{0,0,255},	// move
				{4,5,6,12,13,14,15,16,17,255},	// attack (quick punch)
				{18,19,20,21,22,255},		// die
				{18,19,20,21,22,21,20,19,18,255},	// teleport = A1
				{4,5,6,7,8,9,10,11,6,7,8,9,10,11,6,7,8,9,10,11,12,13,14,14,14,15,15,16,17,255},	// super punch = A2
				{1,2,3,3,2,3,2,3,3,3,2,2,1,1,255},		// scream = A3
			}},
		{"Pumpkin",{255,255,0},
		 10,10,13,55,2,2*FIXAMT,"graphics/pumpkin.jsp",0,0,
			{
				{0,255},	// idle
				{0,1,2,3,4,5,255},	// move
				{0,1,2,3,255},	// attack (unused)
				{6,7,8,9,10,11,12,255},		// die
			}},
		{"Pumpkin",{255,255,4},
		 10,10,13,45,3,6*FIXAMT,"!37",0,0,
			{
				{0,255},	// idle
				{0,1,2,3,4,5,255},	// move
				{0,1,2,3,255},	// attack (unused)
				{6,7,8,9,10,11,12,255},		// die
			}},
		{"Pumpkin",{255,255,-4},
		 10,10,13,70,2,5*FIXAMT,"!37",0,0,
			{
				{0,255},	// idle
				{0,1,2,3,4,5,255},	// move
				{0,1,2,3,255},	// attack (unused)
				{6,7,8,9,10,11,12,255},		// die
			}},
		{"Vamp Jr.",{255,255,0},
		 20,13,20,20,2,5*FIXAMT,"graphics/vampire.jsp",0,0,
			{
				{0,1,2,1,255},	// idle
				{1,2,1,0,255},	// move
				{3,4,5,6,7,8,9,255},	// attack (shove)
				{16,17,18,19,255},		// die
				{10,11,12,12,12,11,10,255},	// summon = A1
				{13,14,15,15,14,13,255},	// point = A2
			}},
		{"Bloodsucker",{6,4,3},
		 30,13,20,30,3,7*FIXAMT,"!40",0,0,
			{
				{0,1,2,1,255},	// idle
				{1,2,1,0,255},	// move
				{3,4,5,6,7,8,9,255},	// attack (shove)
				{16,17,18,19,255},		// die
				{10,11,12,12,12,11,10,255},	// summon = A1
				{13,14,15,15,14,13,255},	// point = A2
			}},
		{"Draculite",{0,1,-4},
		 50,13,20,50,3,5*FIXAMT,"!40",0,0,
			{
				{0,1,2,1,255},	// idle
				{1,2,1,0,255},	// move
				{3,4,5,6,7,8,9,255},	// attack (shove)
				{16,17,18,19,255},		// die
				{10,11,12,12,12,11,10,255},	// summon = A1
				{13,14,15,15,14,13,255},	// point = A2
			}},
		{"Vamp Lord",{0,6,-3},
		 80,13,20,150,5,4*FIXAMT,"!40",0,0,
			{
				{0,1,2,1,255},	// idle
				{1,2,1,0,255},	// move
				{3,4,5,6,7,8,9,255},	// attack (shove)
				{16,17,18,19,255},		// die
				{10,11,12,12,12,11,10,255},	// summon = A1
				{13,14,15,15,14,13,255},	// point = A2
			}},
		{"Happy Stick Witch",{255,255,0},
		 100,12,20,500,5,8*FIXAMT,"graphics/stikwtch.jsp",0,MF_ONEFACE,
			{
				{0,255},	// idle
				{1,1,0,2,2,0,255},	// move
				{4,0,0,255},	// attack (shove)
				{5,5,5,5,5,6,6,7,8,9,10,11,12,13,14,15,16,16,16,16,16,17,18,255},		// die
				{3,3,3,255},	// ouch = A1
				{1,0,2,0,1,0,2,0,1,0,2,0,1,0,2,0,4,0,3,3,3,255},	// appear=A2
			}},
		{"Loony Toad",{1,0,0},
		 0,6,22,128,0,0,"!15",0,MF_WATERWALK,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,8,0,255},	// move
				{9,10,11,12,13,14,15,16,255},	// attack
				{17,17,17,17,17,17,18,19,20,21,255},		// die
				{9,11,13,15,16,255},	// use item
				{0,0,0,0,0,0,0,0,0,0,0,0,9,10,255},	// bored
				{2,4,7,0,3,5,8,0,1,4,7,0,0,0,19,20,21,255},	// watery death
			}},
		{"Tumblerock",{255,255,0},
		 0,32,8,3000,10,3*FIXAMT,"graphics/tmblrock.jsp",0,MF_NOMOVE|MF_INVINCIBLE,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,255},	// move
				{0,255},	// attack (shove)
				{0,255},		// die
			}},
		{"Tumblerock2",{255,255,0},
		 0,32,8,3000,10,3*FIXAMT,"graphics/tmblrock.jsp",0,MF_NOMOVE|MF_INVINCIBLE,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,255},	// move
				{0,255},	// attack (shove)
				{0,255},		// die
			}},
		{"Tumblestone",{2,0,0},
		 0,32,8,3000,10,3*FIXAMT,"graphics/tmblrock.jsp",0,MF_NOMOVE|MF_INVINCIBLE,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,255},	// move
				{0,255},	// attack (shove)
				{0,255},		// die
			}},
		{"Tumblestone2",{2,0,0},
		 0,32,8,3000,10,3*FIXAMT,"graphics/tmblrock.jsp",0,MF_NOMOVE|MF_INVINCIBLE,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,255},	// move
				{0,255},	// attack (shove)
				{0,255},		// die
			}},
		{"Lightball2.5",{255,255,0},
		 2,10,8,3000,1,0,"graphics/lghtball.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_INVINCIBLE,
			{
				{0,1,2,3,4,5,6,7,6,5,4,3,2,1,255},	// idle
				{0,255},	// move
				{0,255},	// attack (shove)
				{0,255},	// die
			}},
		{"Lightball5.2",{255,255,-3},
		 2,10,8,3000,1,0,"!50",0,MF_NOMOVE|MF_ONEFACE|MF_INVINCIBLE,
			{
				{0,1,2,3,4,5,6,7,6,5,4,3,2,1,255},	// idle
				{0,255},	// move
				{0,255},	// attack (shove)
				{0,255},	// die
			}},
		{"Lightball",{5,1,2},	// can be destroyed
		 2,10,8,50,1,0,"!50",0,MF_NOMOVE|MF_ONEFACE,
			{
				{0,1,2,3,4,5,6,7,6,5,4,3,2,1,255},	// idle
				{0,255},	// move
				{0,255},	// attack (shove)
				{0,255},	// die
			}},
		{"LightballOn",{255,255,6},
		 2,10,8,3000,1,0,"!50",0,MF_NOMOVE|MF_ONEFACE|MF_INVINCIBLE,
			{
				{0,1,2,3,2,1,255},	// idle
				{0,255},	// move
				{0,255},	// attack (shove)
				{0,255},	// die
			}},
		{"LightballOff",{255,255,-3},
		 2,10,8,3000,1,0,"!50",0,MF_NOMOVE|MF_ONEFACE|MF_INVINCIBLE,
			{
				{7,255},	// idle
				{0,255},	// move
				{0,255},	// attack (shove)
				{0,255},	// die
			}},
		{"Bonehead-O-Matic",{255,255,0},
		 15,32,47,100,1,0,"graphics/bhomatic.jsp",0,MF_NOMOVE|MF_ONEFACE,
			{
				{0,1,255},	// idle
				{0,255},	// move
				{2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,255},	// attack (shove)
				{0,0,255},	// die
				{28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,255},	// A1=finish creating
			}},
		{"Frankenjulie",{255,255,0},
		 150,80,39,600,3,3*FIXAMT,"graphics/frankenj.jsp",0,MF_NOMOVE|MF_ONEFACE,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,0,0,255},	// attack (headspin)
				{34,34,34,34,34,255},	// die
				{16,16,16,255},	// A1=idle no head
				{34,34,34,255},	// A2=idle no body
				{16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,19,19,18,18,17,17,16,16,255}, // A3=arm spin
				{35,36,37,38,38,38,38,37,36,35,34,255},	// A4=missile launch
			}},
		{"Frankenhand",{255,255,0},
		 80,44,40,50,1,5*FIXAMT,"graphics/hand.jsp",0,0,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,8,9,10,11,255},	// move
				{12,13,14,15,16,17,18,19,20,21,22,255},	// attack
				{23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,255},	// die
			}},
		{"PolterGuy",{255,255,0},
		 100,30,9,300,1,1*FIXAMT,"graphics/bigghost.jsp",0,MF_ONEFACE|MF_FLYING|MF_GHOST|MF_FREEWALK|MF_ENEMYWALK|MF_NOSHADOW|MF_NOMOVE,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,2,1,255},	// attack
				{4,4,5,5,6,6,7,7,8,8,255},	// die
				{1,2,3,3,3,2,1,2,3,2,1,0,0,0,0,1,2,3,2,1,255},	// A1=sneeze
			}},
		{"Junk",{255,255,0},
		 8,11,9,10,1,12*FIXAMT,"graphics/junk.jsp",0,MF_ONEFACE|MF_FLYING|MF_FREEWALK|MF_ENEMYWALK,
			{
				{0,1,2,3,4,5,6,7,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{0,255},	// die
			}},
		{"Junk",{255,255,0},
		 8,11,9,10,1,12*FIXAMT,"!59",0,MF_ONEFACE|MF_FLYING|MF_FREEWALK|MF_ENEMYWALK,
			{
				{8,9,10,11,12,13,14,15,255},	// idle
				{8,255},	// move
				{8,255},	// attack
				{8,255},	// die
			}},
		{"Junk",{255,255,0},
		 8,11,9,10,1,12*FIXAMT,"!59",0,MF_ONEFACE|MF_FLYING|MF_FREEWALK|MF_ENEMYWALK,
			{
				{16,17,18,19,20,21,22,23,255},	// idle
				{16,255},	// move
				{16,255},	// attack
				{16,255},	// die
			}},
		{"Farley",{255,255,0},	// Farley the helper bat
		 8,8,33,15,1,5*FIXAMT,"!24",0,MF_FLYING|MF_GLOW|MF_FREEWALK|MF_ENEMYWALK|MF_WALLWALK|MF_NOHIT,
			{
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{11,12,13,12,11,255},	// attack
				{23,24,25,26,27,28,29,30,31,32,255},		// die
				{14,15,16,16,16,16,15,14,255},	// diving attack =A1
				{17,18,19,20,21,22,255},	// bounce off during dive =A2
				{7,8,9,10,9,8,7,255}		// shoot = A3
			}},
		{"Gusty The Elder",{3,7,3},	// Elder Wind Vampire
		 100,20,13,200,3,5*FIXAMT,"graphics/elder.jsp",0,0,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,4,4,4,5,6,255},	// attack (shove)
				{0,0,0,0,0,0,255},		// die
				{7,8,9,9,9,9,8,8,7,7,255},	// summon pose = A1
				{10,11,12,12,12,11,10,255},	// point = A2
			}},
		{"Stoney The Elder",{3,2,0},	// Elder Earth Vampire
		 100,20,13,400,5,5*FIXAMT,"!63",0,0,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,4,5,6,255},	// attack (shove)
				{254,254,254,254,255},		// die
				{7,8,9,9,9,9,8,8,7,7,255},	// summon pose = A1
				{10,11,12,12,12,12,12,12,12,12,11,10,255},	// point = A2
			}},
		{"Drippy The Elder",{255,255,0},	// Elder Water Vampire
		 100,20,13,400,5,5*FIXAMT,"!63",0,MF_GLOW|MF_ENEMYWALK,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,4,5,6,255},	// attack (shove)
				{0,0,0,0,0,0,255},		// die
				{7,8,9,9,9,9,8,8,7,7,255},	// summon pose = A1
				{10,11,12,12,12,11,10,255},	// point = A2
			}},
		{"Toasty The Elder",{3,4,0},	// Elder Fire Vampire
		 100,20,13,400,5,4*FIXAMT,"!63",0,0,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,4,5,6,255},	// attack (shove)
				{0,0,0,0,0,0,255},		// die
				{7,8,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,8,8,7,7,255},	// summon pose = A1
				{10,11,12,12,12,12,12,12,12,12,12,12,12,12,12,11,10,255},	// point = A2
			}},
		{"Bubble",{255,255,0},	// bubble for water elder to shoot
		 1,13,5,10,1,4*FIXAMT,"graphics/bubble.jsp",0,MF_GLOW|MF_ONEFACE|MF_ENEMYWALK|MF_INVINCIBLE,
			{
				{0,1,2,1,0,3,4,3,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{0,0,255},		// die
			}},
		{"Good Bonkula",{5,6,0},	// player-controlled Bonkula
		 1,8,28,5,1,7*FIXAMT,"graphics/bossvamp.jsp",0,MF_WATERWALK,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,255},	// attack
				{21,22,23,24,25,26,27,26,25,24,25,26,27,255},		// die
				{0,1,2,3,2,1,255},	// A1=shoot special weapon
				{0,0,0,0,0,21,22,23,24,255},	// A2=bored
			}},
		{"Good Swampdog",{5,6,-2},
		 1,12,43,20,1,6*FIXAMT,"!31",0,MF_WATERWALK,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,8,9,10,0,255},	// move
				{24,25,26,27,28,29,30,31,32,255},		// attack (shoot)
				{33,34,35,36,37,38,39,40,41,42,255},		// die
				{20,21,22,23,23,22,21,20,255},	// A1=special attack (eye jab)
				{19,18,18,11,11,11,11,11,11,11,18,19,255},	// A2=get bored
			}},
		{"Good Stick Witch",{7,1,0},
		 100,11,20,160,5,7*FIXAMT,"!44",0,MF_ONEFACE|MF_WATERWALK,
			{
				{0,255},	// idle
				{1,1,0,2,2,0,255},	// move
				{4,4,4,255},	// attack (shoot)
				{5,5,5,5,5,6,6,7,8,9,10,11,12,13,14,15,16,16,16,16,16,17,18,255},		// die
				{3,0,3,255},	// A1=change spell
				{3,3,3,3,3,3,3,3,3,3,255},	// A2 = bored
			}},
		{"Count Bonkula",{255,255,0},
		 200,8,28,100,5,7*FIXAMT,"graphics/bossvamp.jsp",0,MF_WALLWALK|MF_NOMOVE|MF_INVINCIBLE|MF_FLYING,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,255},	// attack (shoot)
				{21,22,23,24,25,26,27,26,25,24,25,26,27,255},		// die
				{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255},	// A1=spin around
			}},
		{"The Evilizer",{255,255,0},
		 200,140,32,2000,5,0,"graphics/evilizer.jsp",0,MF_NOMOVE|MF_INVINCIBLE|MF_ONEFACE|MF_FACECMD,
			{
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// idle
				{0,255},	// move
				{0,255},	// attack (shoot)
				{0,0,0,0,0,0,0,255},	// die
			}},
		{"Steam Pump",{255,255,0},
		 100,30,13,500,5,0,"graphics/evilpump.jsp",0,MF_NOMOVE,
			{
				{0,1,2,3,4,5,6,7,8,7,6,5,4,3,2,1,255},	// idle
				{0,255},	// move
				{9,10,11,12,12,12,12,11,10,9,255},	// attack (shoot)
				{9,10,11,12,11,10,9,10,12,9,12,9,12,9,255},	// die
			}},
		{"Wolf Loony",{2,4,-3},
		 6,12,55,15,1,6*FIXAMT,"graphics/wolfman.jsp",0,0,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{30,31,32,33,34,35,36,37,38,39,40,41,42,43,255},	// attack (pounce)
				{44,45,46,47,48,49,50,51,51,51,52,53,54,254,54,254,54,254,54,255},		// die
				{7,8,9,9,8,7,255},	// spit projectile = A1
				{10,11,12,12,12,13,14,15,16,17,17,18,19,255},	// charging slash = A2
				{20,21,22,23,24,25,26,26,26,26,26,26,26,26,27,28,29,255}		// howl = A3
			}},
		{"Loony",{6,4,0},
		 0,11,25,128,0,0,"graphics/kidloony.jsp",0,MF_WATERWALK,
			{
				{0,255},	// idle
				{1,2,3,4,3,2,1,0,5,6,7,8,7,6,5,0,255},	// move
				{9,10,11,12,13,14,15,16,255},	// attack
				{0,0,0,0,0,0,17,18,19,20,255},		// die
				{9,11,13,15,16,255},	// use item
				{21,21,22,22,23,23,24,24,24,24,24,24,5,1,255},	// bored
				{2,4,2,0,6,8,6,0,2,4,2,0,0,0,18,19,20,255},	// watery death
			}},
		{"Larry",{4,1,0},
		 150,25,14,150,1,5*FIXAMT+FIXAMT/2,"!9",0,0,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{0,255},	// attack
				{0,255},		// die
			}},
		{"Agent Skully",{0,6,2},
		 70,8,32,250,5,2*FIXAMT,"!2",0,0,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{12,13,14,15,16,17,18,19,255},	// attack
				{7,8,9,10,11,11,11,11,254,11,254,11,254,11,254,11,254,11,254,11,254,11,255},		// die
				{20,21,22,23,24,25,255},	// chomp/spit projectile
				{26,27,27,27,26,255},	// point at bouapha
				{11,10,9,8,7,255},	// resurrect=A3
				{28,29,30,31,30,29,28,255},	// leap skyward
			}},
		{"Summony The Elder",{3,1,-5},	// Elder Summoning Vampire
		 200,20,13,800,5,2*FIXAMT,"!63",0,0,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,4,5,6,255},	// attack (shove)
				{0,0,0,0,0,0,255},		// die
				{7,8,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,8,8,7,7,255},	// summon pose = A1
				{10,11,12,12,12,12,12,12,12,12,12,12,12,12,12,11,10,255},	// point = A2
			}},
		{"Harry",{5,2,0},
		 150,25,38,300,3,4*FIXAMT,"graphics/larry.jsp",0,MF_NOMOVE,
			{
				{0,255},	// idle
				{1,2,3,3,2,1,0,4,5,6,6,5,4,0,255},	// move
				{25,26,27,27,28,29,30,31,32,33,33,33,34,35,36,37,255},	// attack (pounce)
				{0,0,0,0,0,255},		// die
				{7,8,9,9,10,11,12,13,14,15,16,17,255},	// fire darts=A1
				{18,19,20,21,22,23,23,23,23,24,24,255},		// howl = A2
				{0,255},	// blank = A3
			}},
		{"Frogurt",{1,5,2},
		 10,6,22,1,1,4*FIXAMT,"!15",0,0,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,8,0,255},	// move
				{9,9,9,9,255},	// attack
				{17,18,19,20,21,255},		// die
			}},
		{"Torture Tree",{4,5,0},
		 60,32,24,100,1,0,"!22",0,MF_NOMOVE|MF_ONEFACE,
			{
				{4,5,6,7,8,9,10,11,12,13,255},	// idle
				{0,255},	// move
				{0,1,2,3,1,255},	// attack
				{14,15,16,17,18,19,20,21,22,23,255},		// die
			}},
		{"Gasbat",{2,1,4},
		 20,8,33,30,1,5*FIXAMT,"!24",0,MF_FLYING,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{11,12,13,12,11,255},	// attack
				{23,24,25,26,27,28,29,30,31,32,255},		// die
				{14,15,16,16,16,16,15,14,255},	// diving attack =A1
				{17,18,19,20,21,22,255},	// bounce off during dive =A2
				{7,8,9,10,9,8,7,255}		// shoot = A3
			}},
		{"Marsh Mutt",{5,6,3},
		 70,12,43,150,4,2*FIXAMT,"!31",0,0,
			{
				{11,255},	// idle
				{12,13,14,13,12,11,15,16,17,16,15,255},	// move
				{20,21,22,23,23,22,21,20,255},	// attack (eye jab)
				{33,34,35,36,37,38,39,40,41,42,255},		// die
				{18,19,19,19,255},	// perk up = A1
				{1,2,3,4,5,6,7,8,9,10,0,255},	// charge = A2
				{24,25,26,27,28,29,30,31,32,255},		// eyebonk/shoot = A3
				{19,18,18,255},	// get bored = A4
				{0,0,0,0,255},	// hyper idle = A5
			}},
		{"King Toot",{0,6,0},
		 120,11,28,300,5,2*FIXAMT,"!18",0,0,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,255},	// attack
				{19,20,21,22,23,24,25,26,27,255},		// die
				{15,16,17,18,18,17,16,15,255},	// A1 = gem zap
				{7,8,9,9,9,9,9,8,7,255},	// A2 = magic hands
			}},
		{"Ghastly Stench",{0,1,3},
		 100,30,9,300,1,1*FIXAMT,"graphics/bigghost.jsp",0,MF_ONEFACE|MF_FLYING|MF_FREEWALK|MF_ENEMYWALK|MF_NOSHADOW|MF_NOMOVE,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,2,1,255},	// attack
				{4,4,5,5,6,6,7,7,8,8,255},	// die
				{1,2,3,3,3,2,1,2,3,2,1,0,0,0,0,1,2,3,2,1,255},	// A1=sneeze
			}},
		{"Bombie Lord",{0,4,7},
		 100,12,42,100,4,2*FIXAMT,"!6",0,0,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{15,16,17,18,19,20,21,22,23,23,24,25,26,27,28,255},	// attack
				{29,30,31,32,33,34,35,36,37,38,39,40,41,255},		// die
				{7,8,9,10,11,12,13,14,255},	// A1 = stomp ground
			}},
		{"Mutant Pumpkin",{2,6,0},
		 30,10,13,100,3,3*FIXAMT,"!37",0,0,
			{
				{0,255},	// idle
				{0,1,2,3,4,5,255},	// move
				{0,1,2,3,255},	// attack (unused)
				{6,7,8,9,10,11,12,255},		// die
			}},
		{"Redbone-O-Matic",{0,4,0},
		 40,32,47,150,1,0,"!55",0,MF_NOMOVE|MF_ONEFACE,
			{
				{0,1,255},	// idle
				{0,255},	// move
				{2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,255},	// attack (shove)
				{0,0,255},	// die
				{28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,255},	// A1=finish creating
			}},
		{"Gangrene-O-Matic",{0,1,2},
		 50,32,47,200,1,0,"!55",0,MF_NOMOVE|MF_ONEFACE,
			{
				{0,1,255},	// idle
				{0,255},	// move
				{2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,255},	// attack (shove)
				{0,0,255},	// die
				{28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,255},	// A1=finish creating
			}},
		{"Numbskull-O-Matic",{0,5,2},
		 100,32,47,400,1,0,"!55",0,MF_NOMOVE|MF_ONEFACE,
			{
				{0,1,255},	// idle
				{0,255},	// move
				{2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,255},	// attack (shove)
				{0,0,255},	// die
				{28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,255},	// A1=finish creating
			}},
		{"Mini-Frankenjulie",{255,255,0},
		 70,40,39,400,3,6*FIXAMT,"graphics/franksml.jsp",0,MF_ONEFACE,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,0,0,255},	// attack (headspin)
				{34,34,34,34,34,255},	// die
				{16,16,16,255},	// A1=idle no head
				{34,34,34,255},	// A2=idle no body
				{16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,19,19,18,18,17,17,16,16,255}, // A3=arm spin
				{35,36,37,38,38,38,38,37,36,35,34,255},	// A4=missile launch
			}},
		{"Mecha-Bonkula",{5,0,4},
		 200,8,28,700,5,5*FIXAMT,"!71",0,MF_WALLWALK|MF_NOMOVE|MF_FLYING,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,255},	// attack (shoot)
				{21,22,23,24,25,26,27,26,25,24,25,26,27,255},		// die
				{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255},	// A1=spin around
			}},
		{"Good Summony",{6,1,2},
		 50,11,20,50,3,5*FIXAMT,"!40",0,MF_WATERWALK,
			{
				{0,0,1,1,2,2,1,1,255},	// idle
				{1,1,2,2,1,1,0,0,255},	// move
				{3,4,5,6,7,8,9,255},	// attack (shove)
				{16,17,18,19,255},		// die
				{10,11,12,12,12,11,10,255},	// summon = A1
				{13,14,15,15,14,13,255},	// point = A2
			}},
		{"Good Girl",{4,6,1},
		 0,11,12,100,1,2*FIXAMT,"!12",0,MF_WATERWALK,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,8,7,0,255},	// attack
				{0,0,0,0,0,255},		// die
				{1,2,1,0,255},			// A1= switch bombs
				{7,8,9,10,11,10,9,10,11,10,9,8,7,255},	// A2 = yawn
			}},
		{"Bombie",{0,5,0},	// summon: kamikaze exploding zombie
		 6,11,42,30,3,6*FIXAMT,"!6",0,MF_ENEMYWALK|MF_FREEWALK,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{15,16,17,18,19,20,21,22,23,23,24,25,26,27,28,255},	// attack
				{29,30,31,32,33,34,35,36,37,38,39,40,41,255},		// die
				{7,8,9,10,11,12,13,14,255},	// A1 = stomp ground
			}},
		{"Shocking Horror",{0,3,-6},
		 13,11,23,60,1,5*FIXAMT,"!34",0,MF_FLYING|MF_WALLWALK|MF_FREEWALK|MF_ENEMYWALK|MF_NOSHADOW,
			{
				{0,255},	// idle
				{0,0,255},	// move
				{4,5,6,12,13,14,15,16,17,255},	// attack (quick punch)
				{18,19,20,21,22,255},		// die
				{18,19,20,21,22,21,20,19,18,255},	// teleport = A1
				{4,5,6,7,8,9,10,11,6,7,8,9,10,11,6,7,8,9,10,11,12,13,14,14,14,15,15,16,17,255},	// super punch = A2
				{1,2,3,3,2,3,2,3,3,3,2,2,1,1,255},		// scream = A3
			}},
		{"Arctic Wolf",{2,7,0},
		 14,11,55,70,1,7*FIXAMT,"!27",0,MF_ENEMYWALK|MF_FREEWALK,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{30,31,32,32,32,33,34,35,36,37,38,39,40,41,42,43,255},	// attack (pounce)
				{44,45,46,47,48,49,50,51,51,51,52,53,54,254,54,254,54,254,54,255},		// die
				{7,8,9,9,8,7,255},	// spit projectile = A1
				{10,11,12,12,12,13,14,15,16,17,17,18,19,255},	// charging slash = A2
				{20,21,22,23,24,25,26,26,26,26,26,26,26,26,27,28,29,255}		// howl = A3
			}},
		{"Grabby Bat",{2,6,-3},
		 8,8,33,70,1,12*FIXAMT,"!24",0,MF_FLYING|MF_ENEMYWALK|MF_FREEWALK,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{11,12,13,12,11,255},	// attack
				{23,24,25,26,27,28,29,30,31,32,255},		// die
				{14,15,16,16,16,16,15,14,255},	// diving attack =A1
				{17,18,19,20,21,22,255},	// bounce off during dive =A2
				{7,8,9,10,9,8,7,255}		// shoot = A3
			}},
		{"Healer",{0,1,-4},
		 8,11,28,50,1,3*FIXAMT,"!18",0,MF_ENEMYWALK|MF_FREEWALK,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,255},	// attack
				{19,20,21,22,23,24,25,26,27,255},		// die
				{15,16,17,18,18,17,16,15,255},	// A1 = gem zap
				{7,8,9,9,9,9,9,8,7,255},	// A2 = magic hands
				{27,26,25,24,23,22,21,20,19,255},	// A3 = resurrect
			}},
		{"Croaker",{1,0,2},
		 3,6,22,50,1,5*FIXAMT,"!15",0,MF_ENEMYWALK|MF_FREEWALK,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,8,0,255},	// move
				{9,10,10,10,10,9,255},	// attack
				{17,18,19,20,21,255},		// die
			}},
		{"Hot Dog",{5,4,2},
		 10,11,43,80,2,3*FIXAMT,"!31",0,MF_ENEMYWALK|MF_FREEWALK,
			{
				{11,255},	// idle
				{12,13,14,13,12,11,15,16,17,16,15,255},	// move
				{20,21,22,23,23,22,21,20,255},	// attack (eye jab)
				{33,34,35,36,37,38,39,40,41,42,255},		// die
				{18,19,19,19,255},	// perk up = A1
				{1,2,3,4,5,6,7,8,9,10,0,255},	// charge = A2
				{24,25,26,27,28,29,30,31,32,255},		// eyebonk/shoot = A3
				{19,18,18,255},	// get bored = A4
				{0,0,0,0,255},	// hyper idle = A5
			}},
};

void MakeSuperLoony(void)
{
	int i,j;

	j=0;
	for(i=0;i<40;i++)
		if(opt.meritBadge[i])
			j++;
	if(j==40)
	{
		strcpy(monsType[MONS_LOONY].sprName,"graphics/loony2.jsp");
		strcpy(monsType[MONS_LOONY2].sprName,"graphics/loony2.jsp");
		monsType[MONS_LOONY2].offc[0]=3;
	}
	else
	{
		strcpy(monsType[MONS_LOONY].sprName,"graphics/kidloony.jsp");
		strcpy(monsType[MONS_LOONY2].sprName,"graphics/kidloony.jsp");
		monsType[MONS_LOONY2].offc[0]=6;
	}
}

void InitMonsters(void)
{
	int i;

	for(i=0;i<NUM_MONSTERS;i++)
	{
		monsType[i].spr=NULL;
	}

	// if you have all the merit badges, you get to look COOL
	MakeSuperLoony();
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
	monsType[MONS_EVILIZER].flags|=MF_INVINCIBLE;	// reset its invincibility

	MakeSuperLoony();
}

void TurnLoonyIntoToad(void)
{
	EarnBadge(BADGE_ANNOY);

	player.monsType=MONS_LOONYTOAD;
	goodguy->type=MONS_LOONYTOAD;

	player.cheatsOn&=(PC_PLAYBONK|PC_PLAYWITCH|PC_PLAYDOG|PC_PLAYWOLF|PC_PLAYSUMMON|PC_PLAYTHIEF);
	player.cheatsOn|=(PC_PLAYTOAD);
	opt.cheats[CH_BONKULA]=0;
	opt.cheats[CH_WITCH]=0;
	opt.cheats[CH_SWAMPDOG]=0;
	opt.cheats[CH_WEREWOLF]=0;
	opt.cheats[CH_SUMMON]=0;
	opt.cheats[CH_THIEF]=0;
	opt.cheats[CH_TOAD]=1;
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
	if(player.worldNum==WORLD_SLINGSHOT && type==player.monsType)
		return monsType[type].flags|MF_NOMOVE;
	else if(type>=MONS_WOLFMAN && type<=MONS_WOLFMAN3 && !player.var[VAR_SILVERSLING])
		return monsType[type].flags|MF_INVINCIBLE;
	else
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

word MonsterHP(byte type)
{
	return monsType[type].hp;
}

int MonsterLevel(byte type)
{
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
				if(monsType[t].spr==NULL)
					FatalError("Out of memory!");
			}
			monsType[type].spr=monsType[t].spr;
		}
		else
		{
			monsType[type].spr=new sprite_set_t(monsType[type].sprName);
			if(monsType[type].spr==NULL)
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

void MonsterDraw(int x,int y,int z,byte type,byte seq,byte frm,byte facing,char bright,byte ouch,int ice)
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
		if(PlayerIsPoisoned())
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
			if(!(monsType[type].flags&(MF_GHOST|MF_GLOW)))
			{
				if(monsType[type].offc[0]==255)
					SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,255,bright+monsType[type].offc[2],
							curSpr,DISPLAY_DRAWME);
				else
					OffSprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,monsType[type].offc[0],monsType[type].offc[1],
							bright+monsType[type].offc[2],curSpr,DISPLAY_DRAWME|DISPLAY_OFFCOLOR);
			}
			else if(monsType[type].flags&MF_GHOST)
				SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,255,bright+monsType[type].offc[2],
						curSpr,DISPLAY_DRAWME|DISPLAY_GHOST);
			else if(monsType[type].flags&MF_GLOW)
				SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,255,bright+monsType[type].offc[2],
						curSpr,DISPLAY_DRAWME|DISPLAY_GLOW);
		}
	}
	else
		SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,4,bright,curSpr,DISPLAY_DRAWME);
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

	// update bombie brightness, so they flash
	monsType[MONS_ZOMBIE2].offc[2]++;
	if(monsType[MONS_ZOMBIE2].offc[2]>7)
		monsType[MONS_ZOMBIE2].offc[2]=-4;

	// update friendly bombie brightness, so they flash
	monsType[MONS_SUMBOMBIE].offc[2]++;
	if(monsType[MONS_SUMBOMBIE].offc[2]>7)
		monsType[MONS_SUMBOMBIE].offc[2]=-4;

	// same for mutant pumpkins
	monsType[MONS_PUMPKIN4].offc[2]++;
	if(monsType[MONS_PUMPKIN4].offc[2]>7)
		monsType[MONS_PUMPKIN4].offc[2]=-4;
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

#define NUM_DESTS 5

void SelectDestination(Guy *me,Guy *goodguy,Map *map,byte repeat)
{
	int i,x,y;
	byte bestx[NUM_DESTS],besty[NUM_DESTS];

	for(i=0;i<NUM_DESTS;i++)
		bestx[i]=255;

	for(x=0;x<map->width;x++)
		for(y=0;y<map->height;y++)
		{
			if(map->map[x+y*map->width].tag==1 && (x!=me->mind2 || y!=me->mind3 || repeat==1))
			{
				// there is a tag here... which is NOT my previous target
				if(map->CheckLOS(me->mapx,me->mapy,7,x,y))
				{
					// and it's in line of sight
					for(i=0;i<NUM_DESTS;i++)
					{
						if(bestx[i]==255)
						{
							bestx[i]=x;
							besty[i]=y;
							i=NUM_DESTS+2;
						}
					}
					if(i!=NUM_DESTS+3)
					{
						// all 3 tags are presently occupied, so let's see
						// if this is closer than one of them
						for(i=0;i<NUM_DESTS;i++)
						{
							if(abs(bestx[i]-me->mapx)+abs(besty[i]-me->mapy)>
								abs(x-me->mapx)+abs(y-me->mapy))
							{
								bestx[i]=x;
								besty[i]=y;
								i=NUM_DESTS+2;
							}
						}
					}
					// now if the tag is one of the closest 3 visible ones, then
					// it is in the best list
				}
			}
		}
	// now we have a list of up to 3 tags within sight
	// which one is closest or farthest from Loony?
	if(me->type==MONS_LARRY)
	{
		// want to kill - go TOWARDS loony
		x=255;
		for(i=0;i<NUM_DESTS;i++)
		{
			if(x==255 ||
				(bestx[i]!=255 && abs(bestx[i]-goodguy->mapx)+abs(besty[i]-goodguy->mapy)<
						abs(bestx[x]-goodguy->mapx)+abs(besty[x]-goodguy->mapy)))
				x=i;
		}
		if(x==255)
		{
			// no visible valid tags, so head straight for Loony!
			me->mind2=goodguy->mapx;
			me->mind3=goodguy->mapy;
		}
		else
		{
			me->mind2=bestx[x];
			me->mind3=besty[x];
		}
	}
	else
	{
		// want to run - go AWAY from loony
		x=255;
		for(i=0;i<NUM_DESTS;i++)
		{
			if(x==255 ||
				(bestx[i]!=255 && abs(bestx[i]-goodguy->mapx)+abs(besty[i]-goodguy->mapy)>
						abs(bestx[x]-goodguy->mapx)+abs(besty[x]-goodguy->mapy)))
				x=i;
		}
		if(x==255)
		{
			// no visible valid tags, so flee randomly
			me->mind2=(byte)Random(map->width);
			me->mind3=(byte)Random(map->height);
		}
		else
		{
			me->mind2=bestx[x];
			me->mind3=besty[x];
		}
	}
}

void BasicAI(Guy *me,byte ouchSound,byte dieSound,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(ouchSound,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(dieSound,me->x,me->y,SND_CUTOFF,1200);
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

// here be the AIs for each monster type
//--------------------------------------------------------------------------------------

void AI_Bonehead(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;

	// Gangrenes glow
	if(me->type==MONS_BONEHEAD3)
		map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,(me->y/TILE_HEIGHT)>>FIXSHIFT,12,16);

	BasicAI(me,SND_SKELOUCH,SND_SKELDIE,map,world,goodguy);

	if(me->type==MONS_BONEHEAD5 && me->seq==ANIM_DIE && Random(10)==0)
	{
		y=me->y-FIXAMT*10+Random(FIXAMT*11);
		x=me->x-32*FIXAMT+Random(65*FIXAMT);
		FireBullet(x,y,0,BLT_EVILFACE);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE && me->frm==8)
			me->frmAdvance=256;
		if(me->seq==ANIM_ATTACK && me->type==MONS_BONEHEAD5)
		{
			me->dx+=Cosine(me->facing*32)*2;
			me->dy+=Sine(me->facing*32)*2;
			if(me->AttackCheck(24,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				goodguy->GetShot(Cosine(me->facing*32)*4,Sine(me->facing*32)*4,monsType[me->type].damage,map,world);
		}
		if(me->seq==ANIM_ATTACK && me->frm==3 && me->reload==0)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			if(me->AttackCheck(16,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				goodguy->GetShot(Cosine(me->facing*32)*4,Sine(me->facing*32)*4,monsType[me->type].damage,map,world);
			me->reload=5;
		}
		if(me->seq==ANIM_A1 && me->frm==3 && me->reload==0)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			if(me->type==MONS_BONEHEAD2)
				FireBullet(x,y,me->facing*32,BLT_ENERGY);
			else if(me->type==MONS_BONEHEAD3)
				FireBullet(x,y,me->facing*32,BLT_GASBLAST);
			else if(me->type==MONS_BONEHEAD5)
			{
				FireBullet(x,y,(byte)((me->facing*32-16+Random(32))&255),BLT_FLAMESHOT);
				FireBullet(x,y,(byte)((me->facing*32-16+Random(32))&255),BLT_FLAMESHOT);
				FireBullet(x,y,(byte)((me->facing*32-16+Random(32))&255),BLT_FLAMESHOT);
			}
			me->reload=5;
			me->mind1=1;
		}
		if(me->seq==ANIM_A2 && me->type==MONS_BONEHEAD4 && me->reload==0 && me->frm>=2)
		{
			i=Random(TILE_WIDTH)+TILE_WIDTH*(me->frm-1);
			x=me->x+Cosine(me->facing*32)*i;
			y=me->y+Sine(me->facing*32)*i;
			FireBullet(x,y,me->facing*32,BLT_FLAMEWALL);
			me->reload=3;
		}
		if(me->seq==ANIM_A4 && me->frm==4 && me->reload==0)
		{
			// raise the dying
			me->reload=5;
			RaiseSkellies();
		}
		return;	// can't do nothin' right now
	}

	// randomly decide to point at Bouapha to unnerve him
	// (but only if in pursuit mode, because otherwise you'd point the wrong way)
	// Numbskulls don't do this, since they have a much nastier point they use
	// neither does Agent Skully
	if((!Random(100)) && me->mind==0 && me->type!=MONS_BONEHEAD4 && me->type!=MONS_BONEHEAD5)
	{
		me->seq=ANIM_A2;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=64;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		return;
	}

	// if you aren't wandering and you are a Bonehead or Numbskull, then kick as needed
	if(me->mind==0 && (me->type==MONS_BONEHEAD || me->type==MONS_BONEHEAD4))
	{
		if(RangeToTarget(me,goodguy)<(48*FIXAMT) && Random(8)==0 &&
			map->CheckLOS(me->mapx,me->mapy,4,goodguy->mapx,goodguy->mapy))
		{
			// get him!
			MakeSound(SND_SKELKICK,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=0;
			return;
		}
	}
	if(me->mind==0 && me->type==MONS_BONEHEAD5)
	{
		if(RangeToTarget(me,goodguy)<(200*FIXAMT) && Random(8)==0 &&
			map->CheckLOS(me->mapx,me->mapy,10,goodguy->mapx,goodguy->mapy))
		{
			// get him!
			MakeSound(SND_SKELKICK,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=0;
			return;
		}
	}
	// Redbones and Gangrenes shoot only
	if(me->type==MONS_BONEHEAD2 || me->type==MONS_BONEHEAD3 || me->type==MONS_BONEHEAD5)
	{
		if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(32)==0 &&
			map->CheckLOS(me->mapx,me->mapy,15,goodguy->mapx,goodguy->mapy))
		{
			// spit at him
			MakeSound(SND_SKELSHOOT,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_A1;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=0;
			FaceGoodguy(me,goodguy);
			return;
		}
	}
	// numbskull special attacks
	if(me->type==MONS_BONEHEAD4)
	{
		// resurrect any dying skellies
		if(Random(8)==0 && DyingSkellies())
		{
			// resurrect dying skellies
			MakeSound(SND_SKELSUMMON,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_A4;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=64;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=0;
			FaceGoodguy(me,goodguy);
			return;
		}
		if(Random(32)==0 &&
			map->CheckLOS(me->mapx,me->mapy,15,goodguy->mapx,goodguy->mapy))
		{
			// point to cause flame geyser
			MakeSound(SND_SKELBLAST,me->x,me->y,SND_CUTOFF,1200);
			FaceGoodguy(me,goodguy);
			me->seq=ANIM_A2;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=32;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			return;
		}
	}
	WanderAI(me,64,40,3,map,world,goodguy);
}

void AI_Zombie(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;
	Guy *g;

	BasicAI(me,SND_ZOMBIEOUCH,SND_ZOMBIEDIE,map,world,goodguy);

	if(me->seq==ANIM_DIE && me->type==MONS_ZOMBIE4)
	{
		if(Random(5)==0)
		{
			FireBullet(me->x-32*FIXAMT+Random(65)*FIXAMT,me->y-32*FIXAMT+Random(65)*FIXAMT,0,BLT_BADBOOM);
			FireBullet(me->x-32*FIXAMT+Random(65)*FIXAMT,me->y-32*FIXAMT+Random(65)*FIXAMT,0,BLT_EVILFACE);
		}
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==5 && me->reload==0 && goodguy)
		{
			me->reload=5;
			me->dx=Cosine(me->facing*32)*8;
			me->dy=Sine(me->facing*32)*8;
			me->reload=5;
		}
		if(me->seq==ANIM_ATTACK && me->frm>5)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			if(me->AttackCheck(24,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(Cosine(me->facing*32)*12,Sine(me->facing*32)*12,monsType[me->type].damage,map,world);
			}
		}
		if(me->seq==ANIM_ATTACK && me->frm>9)
		{
			Dampen(&me->dx,FIXAMT*2);
			Dampen(&me->dy,FIXAMT*2);
			if(me->frm==11 && me->reload==0 && (me->type==MONS_ZOMBIE2 || me->type==MONS_ZOMBIE4))	// bombie
			{
				// blow self up
				FireBullet(me->x,me->y,0,BLT_MEGABOOM);
				if(me->type==MONS_ZOMBIE2)
				{
					me->seq=ANIM_DIE;
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=128;
					me->dx=0;
					me->dy=0;
					me->hp=0;
				}
				return;
			}
		}
		if(me->seq==ANIM_A1 && me->frm==3 && me->reload==0 && goodguy)
		{
			if(me->type==MONS_ZOMBIE3)
			{
				x=me->x+Cosine(me->facing*32)*16;
				y=me->y+Sine(me->facing*32)*16;
				FireBullet(x,y,me->facing*32,BLT_EARTHSPIKE);
				FireBullet(x,y,((me->facing+6)*32)&255,BLT_EARTHSPIKE);
				FireBullet(x,y,((me->facing+1)*32)&255,BLT_EARTHSPIKE);
				me->reload=5;
				me->mind1=1;
			}
			else
			{
				i=0;
				while(i<20)
				{
					x=me->x-48+Random(97)*FIXAMT;
					y=me->y-48+Random(97)*FIXAMT;
					g=AddGuy(x,y,FIXAMT*40,MONS_ZOMBIE2);
					if(g && !g->CanWalk(g->x,g->y,map,world))
					{
						g->type=MONS_NONE;
						i++;
						continue;
					}
					else
					{
						MakeRingParticle(x,y,FIXAMT*40,20,40);
						FireBullet(x,y,0,BLT_BADBOOM);
						break;
					}
				}
				me->reload=5;
			}
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)	// heading for Bouapha
	{
		x=128*FIXAMT;
		y=32;
		if(me->type==MONS_ZOMBIE2)
			y=16;

		if(RangeToTarget(me,goodguy)<(x) && Random(y)==0 &&
			map->CheckLOS(me->mapx,me->mapy,8,goodguy->mapx,goodguy->mapy))
		{
			// get him!
			MakeSound(SND_ZOMBIELEAP,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=0;
			return;
		}
	}
	else	// wandering
	{
		if(me->type==MONS_ZOMBIE2)	// bombie
		{
			me->dx=me->dx/2;
			me->dy=me->dy/2;
		}
	}

	if((me->type==MONS_ZOMBIE3 || me->type==MONS_ZOMBIE4) && RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(64)==0)
	{
		// stomp earth at him
		MakeSound(SND_ZOMBIESTOMP,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_A1;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		me->reload=0;
		FaceGoodguy(me,goodguy);
		return;
	}
	if(me->seq==ANIM_MOVE)
	{
		switch(me->type)
		{
			case MONS_ZOMBIE:
				me->frmAdvance=64;
				break;
			case MONS_ZOMBIE2:	// bombie
				me->frmAdvance=128;
				break;
			case MONS_ZOMBIE3:	// zombie lord
			case MONS_ZOMBIE4:
				me->frmAdvance=48;
				break;
		}
	}
	if(me->type!=MONS_ZOMBIE2)
		WanderAI(me,64,40,3,map,world,goodguy);
	else
		WanderAI(me,128,10,3,map,world,goodguy);
}

void AI_Bat(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	bullet_t *b;

	BasicAI(me,SND_BATOUCH,SND_BATDIE,map,world,goodguy);

	if(me->type==MONS_BAT4 && Random(3)==0)
	{
		b=FireBullet(me->x,me->y,(byte)Random(256),BLT_GASBLAST);
		if(b)
		{
			b->dx/=2;
			b->dy/=2;
		}
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==2 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			if(me->AttackCheck(16,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				goodguy->GetShot(Cosine(me->facing*32)*4,Sine(me->facing*32)*4,monsType[me->type].damage,map,world);
			me->reload=2;

		}
		if(me->seq==ANIM_A1 && me->frm>1 && goodguy && me->hp>0)
		{
			// diving attack hit check
			x=me->x;
			y=me->y;
			if(me->AttackCheck(16,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(Cosine(me->facing*32)*8,Sine(me->facing*32)*8,monsType[me->type].damage,map,world);
				me->seq=ANIM_A2;	// bounce off
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=64;
				me->dx=-me->dx/4;
				me->dy=-me->dy/4;
			}
		}
		if(me->seq==ANIM_A3 && me->frm==3 && me->reload==0)
		{
			FireBullet(me->x,me->y,me->facing*32,BLT_ENERGY);
			me->reload=5;
		}
		if(me->seq==ANIM_DIE)
			me->frmAdvance=256;

		return;	// can't do nothin' right now
	}

	if(me->type!=MONS_BAT4)
	{
		if(RangeToTarget(me,goodguy)<(48*FIXAMT) && Random(4)==0 &&
			map->CheckLOS(me->mapx,me->mapy,6,goodguy->mapx,goodguy->mapy))
		{
			// get him!
			MakeSound(SND_BATEYES,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=0;
			FaceGoodguy(me,goodguy);
			return;
		}

		if(RangeToTarget(me,goodguy)>(75*FIXAMT) && Random(32)==0 && me->type==MONS_BAT2 &&
			map->CheckLOS(me->mapx,me->mapy,15,goodguy->mapx,goodguy->mapy))
		{
			FaceGoodguy(me,goodguy);
			MakeSound(SND_BATDIVE,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_A1;	// diving attack move
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx=Cosine(me->facing*32)*12;
			me->dy=Sine(me->facing*32)*12;
			return;
		}

		if(RangeToTarget(me,goodguy)>(200*FIXAMT) && Random(32)==0 && me->type==MONS_BAT3 &&
			map->CheckLOS(me->mapx,me->mapy,15,goodguy->mapx,goodguy->mapy))
		{
			MakeSound(SND_BATSHOOT,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_A3;	// shoot
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			FaceGoodguy(me,goodguy);
			return;
		}
	}

	WanderAI(me,64,40,3,map,world,goodguy);
}

void AI_Villager(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->ouch==4 && me->hp==0)
		MakeSound(SND_VILLDIE1+(me->type>MONS_VILLAGER3),me->x,me->y,SND_CUTOFF,1200);

	if(me->action==ACTION_BUSY)
	{
		return;	// can't do nothin' right now
	}

	if(me->reload==0 && Random(64)==0 && me->mind3==0)	// scared girl doesn't yawn
	{
		if(me->type<MONS_VILLAGER4)
			MakeSound(SND_VILLAGERSCRATCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_VILLAGERYAWN,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_A1;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		me->reload=(byte)(30*4+Random(136));
		return;
	}

	if(!(me->mind2--) || (me->mind1>0 && me->mind3==0))	// time to get a new direction
	{
		me->facing=(byte)Random(8);
		me->mind2=(byte)Random(40)+20;
		me->mind=(byte)Random(4);
		if(me->mind==1)
			me->mind=2;
		if(me->mind3 && me->mind>0)
			me->mind=(byte)Random(4)+3;

	}

	if(me->mind3==0 && RangeToTarget(me,goodguy)<FIXAMT*60)
	{
		me->mind=0;
		FaceGoodguy(me,goodguy);
	}

	me->dx=(Cosine(me->facing*32)*me->mind*FIXAMT)/FIXAMT;
	me->dy=(Sine(me->facing*32)*me->mind*FIXAMT)/FIXAMT;
	if(me->mind==0)
	{
		if(me->seq!=ANIM_IDLE)
		{
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
	else
	{
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=me->mind*32;
		}
	}
	me->mind1=0;

	if(me->mind3)
	{
		// follow obnoxiously
		FaceGoodguy(me,goodguy);
		player.var[250]=1;	// a variable to keep track of her being obnoxious
	}
	// villagers have lanterns in dark places, until they give it to the player
	// only in Gloomy Cavern though
	if((map->flags&MAP_TORCHLIT) && (player.var[VAR_QUESTDONE+QUEST_RESCUE]==0) &&
		player.levelNum==24)
		map->WeakTorch(me->mapx,me->mapy);
}

void AI_Frog(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	BasicAI(me,SND_FROGOUCH,SND_FROGDIE,map,world,goodguy);

	if(me->type==MONS_FROG4 && Random(3)==0)
	{
		x=me->x-10*FIXAMT+Random(21*FIXAMT);
		y=me->y-10*FIXAMT+Random(21*FIXAMT);
		FireBullet(x,y,0,BLT_FLAMEWALL);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==4 && me->reload==0 && me->type==MONS_FROG)
		{
			x=me->x+Cosine(me->facing*32)*30;
			y=me->y+Sine(me->facing*32)*30;
			if(me->AttackCheck(10,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(Cosine(me->facing*32)*4,Sine(me->facing*32)*4,monsType[me->type].damage,map,world);
			}
			x=me->x+Cosine(me->facing*32)*60;
			y=me->y+Sine(me->facing*32)*60;
			if(me->AttackCheck(10,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(Cosine(me->facing*32)*4,Sine(me->facing*32)*4,monsType[me->type].damage,map,world);
			}
			me->reload=5;
		}
		if(me->seq==ANIM_ATTACK && me->frm==3 && me->reload==0 && me->type==MONS_FROG2)
		{
			x=me->x+Cosine(me->facing*32)*8;
			y=me->y+Sine(me->facing*32)*8;
			FireBullet(x,y,me->facing*32,BLT_GASBLAST);
			me->reload=5;
		}
		if(me->seq==ANIM_ATTACK && me->frm==2 && me->reload==0 && me->type==MONS_FROG3)
		{
			x=me->x+Cosine(me->facing*32)*8;
			y=me->y+Sine(me->facing*32)*8;
			FireBullet(x,y,me->facing*32,BLT_ENERGY);
			me->reload=30;
		}
		if(me->seq==ANIM_DIE && me->type==MONS_FROG2 && me->frm==4)
		{
			for(y=0;y<4;y++)
			{
				x=Random(256);
				FireBullet(me->x,me->y,x,BLT_GASBLAST);
			}
		}
		if(me->seq==ANIM_DIE && me->type==MONS_FROG3 && me->frm==4 && me->reload==0)
		{
			FireBullet(me->x,me->y,0,BLT_FLAMEWALL);
			me->reload=5;
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0 && me->type!=MONS_FROG4)	// heading for Bouapha
	{
		if(RangeToTarget(me,goodguy)<(128*FIXAMT) && Random(8)==0 && me->type==MONS_FROG &&
			map->CheckLOS(me->mapx,me->mapy,5,goodguy->mapx,goodguy->mapy))
		{
			// get him!
			MakeSound(SND_FROGLICK,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=0;
			return;
		}
	}

	if(me->type==MONS_FROG2 && RangeToTarget(me,goodguy)<(256*FIXAMT) && Random(64)==0 &&
		map->CheckLOS(me->mapx,me->mapy,8,goodguy->mapx,goodguy->mapy))
	{
		// spit out gas
		MakeSound(SND_FROGLICK,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		me->reload=0;
		FaceGoodguy(me,goodguy);
		return;
	}
	if(me->type==MONS_FROG3 && RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(64)==0 && me->reload==0 &&
		map->CheckLOS(me->mapx,me->mapy,15,goodguy->mapx,goodguy->mapy))
	{
		// spit out fireball
		MakeSound(SND_FROGLICK,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=256;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		me->reload=0;
		FaceGoodguy(me,goodguy);
		return;
	}

	WanderAI(me,64,40,3,map,world,goodguy);
}

void AI_Mummy(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,z;
	Guy *g;

	BasicAI(me,SND_ZOMBIEOUCH,SND_ZOMBIEDIE,map,world,goodguy);

	if(me->type==MONS_MUMMY5 && me->seq==ANIM_DIE && Random(5)==0)
	{
		y=me->y-FIXAMT*10+Random(FIXAMT*11);
		x=me->x-32*FIXAMT+Random(65*FIXAMT);
		FireBullet(x,y,0,BLT_EVILFACE);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==5 && me->reload==0)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			if(me->AttackCheck(16,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(Cosine(me->facing*32)*4,Sine(me->facing*32)*4,monsType[me->type].damage,map,world);
			}
			me->reload=5;
		}
		if(me->seq==ANIM_A1 && me->frm==3 && me->reload==0 && me->type!=MONS_MUMMY3)
		{
			x=me->x+Cosine(me->facing*32)*8;
			y=me->y+Sine(me->facing*32)*8;
			MakeSound(SND_MUMMYSHOOT,me->x,me->y,SND_CUTOFF,1200);
			if(me->type!=MONS_MUMMY5)
			{
				FireBullet(x,y,me->facing*32,BLT_ENERGY);
				if(me->type==MONS_MUMMY4)
				{
					FireBullet(x,y,(me->facing*32+240)&255,BLT_ENERGY);
					FireBullet(x,y,(me->facing*32+16)&255,BLT_ENERGY);
				}
			}
			else
				FireBullet(x,y,me->facing*32,BLT_WIND);
			me->reload=5;
		}
		if(me->seq==ANIM_A1 && me->frm>=3 && me->frm<=5 && me->reload==0 && me->type==MONS_MUMMY3)
		{
			x=me->x+Cosine(me->facing*32)*8;
			y=me->y+Sine(me->facing*32)*8;
			MakeSound(SND_MUMMYSHOOT,me->x,me->y,SND_CUTOFF,1200);
			z=me->facing*32-12+Random(25)+256;
			FireBullet(x,y,z&255,BLT_ENERGY);
		}
		if(me->seq==ANIM_A2 && me->frm==4 && me->reload==0)
		{
			g=AddGuy(me->x-FIXAMT*50,me->y+FIXAMT*10,0,MONS_MUMMY+2*(me->type==MONS_MUMMY5));
			if(g && (!g->CanWalk(g->x,g->y,map,world)))
				g->type=MONS_NONE;	// vanish if this spot is occupied
			g=AddGuy(me->x+FIXAMT*50,me->y+FIXAMT*10,0,MONS_MUMMY+2*(me->type==MONS_MUMMY5));
			if(g && (!g->CanWalk(g->x,g->y,map,world)))
				g->type=MONS_NONE;	// vanish if this spot is occupied
			me->reload=5;
		}
		return;	// can't do nothin' right now
	}

	if(me->mind1==0)
		me->mind1=70;
	else
	{
		me->mind1--;
		if(me->mind1==0)
		{
			if((me->type==MONS_MUMMY4 || me->type==MONS_MUMMY5) && Random(4)==0)
			{
				MakeSound(SND_MUMMYSUMMON,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_A2;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=0;
			}
		}
	}

	if(RangeToTarget(me,goodguy)<(72*FIXAMT) && Random(8)==0 &&
		map->CheckLOS(me->mapx,me->mapy,4,goodguy->mapx,goodguy->mapy))
	{
		// get him!
		MakeSound(SND_MUMMYSMACK,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		if(me->type==MONS_MUMMY)
			me->frmAdvance=256;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		me->reload=0;
		return;
	}

	x=64;
	if(me->type==MONS_MUMMY3)
		x=32;
	if(me->type!=MONS_MUMMY && RangeToTarget(me,goodguy)<(360*FIXAMT) && Random(x)==0 &&
		map->CheckLOS(me->mapx,me->mapy,15,goodguy->mapx,goodguy->mapy))
	{
		// shoot
		me->seq=ANIM_A1;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		me->reload=0;
		FaceGoodguy(me,goodguy);
		return;
	}

	if(me->type==MONS_MUMMY && me->seq==ANIM_MOVE)
		me->frmAdvance=256;

	WanderAI(me,1024,2,1,map,world,goodguy);
}

void AI_EvilTree(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	Guy *g;

	// initially adjust position
	if(me->mind3==0)
	{
		me->x-=FIXAMT*3;
		me->y+=FIXAMT;
		me->mind3=1;
	}

	BasicAI(me,SND_TREEOUCH,SND_TREEDIE,map,world,goodguy);

	if(me->type==MONS_EVILTREE3 && me->seq==ANIM_DIE && Random(10)==0)
	{
		y=me->y-FIXAMT*10+Random(FIXAMT*11);
		x=me->x-32*FIXAMT+Random(65*FIXAMT);
		FireBullet(x,y,0,BLT_EVILFACE);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==4 && me->reload==0)
		{
			if(me->type==MONS_EVILTREE2)
			{
				// gas attack
				MakeSound(SND_SKELGAS,me->x,me->y,SND_CUTOFF,1200);
				FireBullet(me->x,me->y,64,BLT_GASBLAST);
				FireBullet(me->x,me->y,64+16,BLT_GASBLAST);
				FireBullet(me->x,me->y,64-16,BLT_GASBLAST);
				me->reload=30*2;
			}
			else if(me->type==MONS_EVILTREE)
			{
				// summon frog
				MakeSound(SND_TREEFROG,me->x,me->y,SND_CUTOFF,1200);
				x=me->x;
				y=me->y+40*FIXAMT;
				g=AddGuy(x,y,FIXAMT*10,MONS_FROG3);
				if(g && (!g->CanWalk(g->x,g->y,map,world)))
					g->type=MONS_NONE;	// vanish if this spot is occupied
				me->reload=30*3;
			}
			else
			{
				// summon frog
				MakeSound(SND_TREEFROG,me->x,me->y,SND_CUTOFF,1200);
				x=me->x;
				y=me->y+40*FIXAMT;
				g=AddGuy(x,y,FIXAMT*10,MONS_FROG4);
				if(g && (!g->CanWalk(g->x,g->y,map,world)))
					g->type=MONS_NONE;	// vanish if this spot is occupied
				me->reload=30*3;
			}
		}
		if(me->seq==ANIM_DIE && me->frm==0 && me->frmTimer<64)
		{
			if(player.worldNum==WORLD_NORMAL || player.worldNum==WORLD_REMIX|| player.worldNum==WORLD_RANDOMIZER)
				map->map[me->mapx+me->mapy*map->width].item=ITM_TREE2;
		}
		return;	// can't do nothin' right now
	}


	if(me->type==MONS_EVILTREE)
	{
		// launch frogs when player is very near
		if(RangeToTarget(me,goodguy)<(256*FIXAMT) && me->reload==0 && Random(16)==0)
		{
			// shoot
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			return;
		}
	}
	else if(me->type==MONS_EVILTREE3)
	{
		// launch frogs when player is near
		if(RangeToTarget(me,goodguy)<(400*FIXAMT) && me->reload==0 && Random(16)==0)
		{
			// shoot
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			return;
		}
	}
	else
	{
		// launch gas at a standard rate
		if(me->reload==0)
		{
			// shoot
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			return;
		}
	}
}

void AI_Wolfman(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	Guy *g;

	if(me->type==MONS_WOLFMAN2 && me->mind2>0)
	{
		map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,
						 (me->y/TILE_HEIGHT)>>FIXSHIFT,4,2);
	}

	BasicAI(me,SND_WOLFOUCH,SND_WOLFDIE,map,world,goodguy);

	if(me->mind3 && me->mind2==0)
		me->mind3--;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==5)
		{
			me->dx=Cosine(me->facing*32)*10;
			me->dy=Sine(me->facing*32)*10;
		}
		if(me->seq==ANIM_ATTACK && me->frm>=6 && me->frm<=10)
		{
			Dampen(&me->dx,FIXAMT);
			Dampen(&me->dy,FIXAMT);
			if(me->AttackCheck(40,me->x>>FIXSHIFT,me->y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(Cosine(me->facing*32)*8,Sine(me->facing*32)*8,monsType[me->type].damage,map,world);
			}
			if(me->frm==10)
			{
				me->dx=0;
				me->dy=0;
			}
		}
		if(me->seq==ANIM_A1 && me->frm==2 && me->reload==0)
		{
			x=me->x+Cosine(me->facing*32)*8;
			y=me->y+Sine(me->facing*32)*8;
			FireBullet(x,y,me->facing*32,BLT_ENERGY);
			me->reload=5;
		}
		if(me->seq==ANIM_A2 && me->frm>=5 && me->frm<=15)
		{
			x=me->x+Cosine(me->facing*32)*32;
			y=me->y+Sine(me->facing*32)*32;
			if(me->AttackCheck(16,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(Cosine(me->facing*32)*4,Sine(me->facing*32)*4,monsType[me->type].damage,map,world);
			}
		}
		if(me->seq==ANIM_A2)
		{
			// constantly turn toward goodguy
			FaceGoodguy2(me,goodguy);
			if(me->frm<15 && me->frm>4)
			{
				x=Cosine(me->facing*32)*12;
				y=Sine(me->facing*32)*12;
			}
			else
			{
				x=0;
				y=0;
			}
			me->dx-=x;
			Dampen(&me->dx,FIXAMT+FIXAMT/2);
			me->dx+=x;
			me->dy-=y;
			Dampen(&me->dy,FIXAMT+FIXAMT/2);
			me->dy+=y;
		}
		if(me->seq==ANIM_A3 && me->frm==6 && me->reload==0)
		{
			// howl effect
			me->reload=10;
			if(me->type==MONS_WOLFMAN2)
			{
				// go fast for 3 seconds
				me->mind2=30*3;
			}
			else // dire wolf summons dogboys
			{
				// summon one randomly off each edge of the screen
				// left
				GetCamera(&x,&y);
				x=x-320-80-Random(320);
				y=y-240+Random(480);
				g=AddGuy(x*FIXAMT,y*FIXAMT,0,MONS_WOLFMAN);
				if(g && (!g->CanWalk(g->x,g->y,map,world)))
					g->type=MONS_NONE;	// vanish if this spot is occupied
				// right
				GetCamera(&x,&y);
				x=x+320+80+Random(320);
				y=y-240+Random(480);
				g=AddGuy(x*FIXAMT,y*FIXAMT,0,MONS_WOLFMAN);
				if(g && (!g->CanWalk(g->x,g->y,map,world)))
					g->type=MONS_NONE;	// vanish if this spot is occupied
				// top
				GetCamera(&x,&y);
				x=x-320+Random(640);
				y=y-240-80-Random(240);
				g=AddGuy(x*FIXAMT,y*FIXAMT,0,MONS_WOLFMAN);
				if(g && (!g->CanWalk(g->x,g->y,map,world)))
					g->type=MONS_NONE;	// vanish if this spot is occupied
				// bottom
				GetCamera(&x,&y);
				x=x-320+Random(640);
				y=y+240+80+Random(240);
				g=AddGuy(x*FIXAMT,y*FIXAMT,0,MONS_WOLFMAN);
				if(g && (!g->CanWalk(g->x,g->y,map,world)))
					g->type=MONS_NONE;	// vanish if this spot is occupied
			}
		}
		if(me->seq==ANIM_DIE)
			me->frmAdvance=190;
		return;	// can't do nothin' right now
	}

	// dire wolves don't pounce
	if(me->type!=MONS_WOLFMAN3 && RangeToTarget(me,goodguy)<(128*FIXAMT) && Random(12)==0 &&
		map->CheckLOS(me->mapx,me->mapy,15,goodguy->mapx,goodguy->mapy))
	{
		// get him!
		MakeSound(SND_WOLFPOUNCE,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=160;
		me->action=ACTION_BUSY;
		me->reload=0;
		me->dx=0;
		me->dy=0;
		return;
	}
	// dogboys don't spit
	if(me->type>=MONS_WOLFMAN2 && RangeToTarget(me,goodguy)<(360*FIXAMT) && Random(128)==0 &&
		map->CheckLOS(me->mapx,me->mapy,15,goodguy->mapx,goodguy->mapy))
	{
		// shoot
		me->seq=ANIM_A1;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		me->reload=0;
		MakeSound(SND_WOLFSPIT,me->x,me->y,SND_CUTOFF,1200);
		FaceGoodguy(me,goodguy);
		return;
	}
	// only dire wolves charge
	if(me->type==MONS_WOLFMAN3 && RangeToTarget(me,goodguy)<(256*FIXAMT) && Random(32)==0 &&
		map->CheckLOS(me->mapx,me->mapy,15,goodguy->mapx,goodguy->mapy))
	{
		// charge!!!
		MakeSound(SND_WOLFCHARGE,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_A2;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		me->reload=0;
		FaceGoodguy(me,goodguy);
		return;
	}
	// Dire Wolf and Wolfman howl
	if((me->type==MONS_WOLFMAN2 || me->type==MONS_WOLFMAN3) && me->mind3==0 && me->mind2==0 && Random(128)==0 &&
		map->CheckLOS(me->mapx,me->mapy,20,goodguy->mapx,goodguy->mapy))
	{
		// howl
		me->mind3=30*5;
		MakeSound(SND_WOLFHOWL,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_A3;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		me->reload=0;
		return;
	}
	if(me->seq==ANIM_MOVE)
		me->frmAdvance=256;

	WanderAI(me,1024,2,1,map,world,goodguy);
}

void AI_Swampdog(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,z;

	BasicAI(me,SND_SWAMPOUCH,SND_SWAMPDIE,map,world,goodguy);

	if(me->mind)
		me->mind--;

	if(me->type==MONS_SWAMPDOG4 && me->seq==ANIM_DIE && Random(10)==0)
	{
		y=me->y-FIXAMT*20+Random(FIXAMT*41);
		x=me->x-32*FIXAMT+Random(65*FIXAMT);
		FireBullet(x,y,0,BLT_EVILFACE);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==3 && me->reload==0)
		{
			x=me->x+Cosine(me->facing*32)*32;
			y=me->y+Sine(me->facing*32)*32;
			if(me->AttackCheck(32,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(Cosine(me->facing*32)*4,Sine(me->facing*32)*4,monsType[me->type].damage,map,world);
			}
			me->reload=5;
		}
		if(me->seq==ANIM_A3 && me->frm==6 && me->reload==0)
		{
			x=me->x+Cosine(me->facing*32)*8;
			y=me->y+Sine(me->facing*32)*8;

			if(me->type==MONS_SWAMPDOG3 || me->type==MONS_SWAMPDOG4)
			{
				if(me->mind3&1)
				{
					z=(me->facing+8)*32-32;
					z+=(me->mind3-1)*8;
					z=z&255;
				}
				else
				{
					z=(me->facing+8)*32+32;
					z-=(me->mind3-1)*8;
					z=z&255;
				}
				if(me->type==MONS_SWAMPDOG4)
					FireBullet(x,y,z,BLT_BIGSHOT);
				else
					FireBullet(x,y,z,BLT_ENERGY);
				if(me->mind3>1)
				{
					// repeat fire
					me->mind3-=2;
					me->frm=4;
				}
				me->reload=3;
			}
			else
			{
				FireBullet(x,y,me->facing*32,BLT_ENERGY);
				me->reload=5;
			}
		}
		return;	// can't do nothin' right now
	}

	if(me->mind2==0)	// unalert
	{
		if(RangeToTarget(me,goodguy)<128*FIXAMT || me->ouch>0)
		{
			// seen goodguy/got shot - ALERT!!
			me->mind2=1;
			me->action=ACTION_BUSY;
			me->seq=ANIM_A1;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
			me->mind=30*5;
			MakeSound(SND_SWAMPALERT,me->x,me->y,SND_CUTOFF,1200);
		}
		else
		{
			if(Random(64)==0)
				me->facing=(me->facing+7+(byte)Random(3))&7;

			if(me->mind1==0)	// standing
			{
				me->dx=0;
				me->dy=0;
				if(me->seq!=ANIM_IDLE)
				{
					me->seq=ANIM_IDLE;
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=64;
				}
				if(me->mind==0)
				{
					// plod around a bit
					me->mind=(byte)Random(30*4);
					me->mind1=1;
				}
			}
			else	// plodding
			{
				me->dx=Cosine(me->facing*32)*monsType[me->type].speed/FIXAMT;
				me->dy=Sine(me->facing*32)*monsType[me->type].speed/FIXAMT;
				if(me->seq!=ANIM_MOVE)
				{
					me->seq=ANIM_MOVE;
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=64;
				}
				if(me->mind==0)
				{
					// stand around a bit
					me->mind=(byte)Random(30*2);
					me->mind1=0;
				}
			}
		}
	}
	else // ALERT
	{
		if(me->ouch)
			me->mind=30*5;	// force alertness when injured

		if(me->mind==0)
		{
			// time to be bored again
			DoMove(me,ANIM_A4,64,1,0,0);
			me->mind2=0;
		}
		if(me->seq==ANIM_A5 && me->frm>1)
			me->frm=0;

		if(me->seq==ANIM_IDLE)
		{
			// can't stand still, must charge
			FaceGoodguy(me,goodguy);
			if(me->type!=MONS_SWAMPDOG3)
			{
				MakeSound(SND_SWAMPCHARGE,me->x,me->y,SND_CUTOFF,1200);
				DoMove(me,ANIM_A2,128,0,Cosine(me->facing*32)*5,Sine(me->facing*32)*5);
			}
			else	// Shambling Thing does stand still
				DoMove(me,ANIM_A5,128,0,0,0);
		}
		if(RangeToTarget(me,goodguy)<(72*FIXAMT) && Random(8)==0 &&
			map->CheckLOS(me->mapx,me->mapy,15,goodguy->mapx,goodguy->mapy))
		{
			// get him!
			FaceGoodguy(me,goodguy);
			MakeSound(SND_SWAMPJAB,me->x,me->y,SND_CUTOFF,1200);
			DoMove(me,ANIM_ATTACK,128,1,0,0);
			me->reload=0;
			return;
		}
		if(me->type==MONS_SWAMPDOG3)
			x=8;
		else
			x=32;
		if(RangeToTarget(me,goodguy)<(380*FIXAMT) && Random(x)==0 &&
			map->CheckLOS(me->mapx,me->mapy,15,goodguy->mapx,goodguy->mapy))
		{
			// get him!
			FaceGoodguy(me,goodguy);
			MakeSound(SND_SWAMPSHOOT,me->x,me->y,SND_CUTOFF,1200);
			DoMove(me,ANIM_A3,128,1,0,0);
			me->reload=0;
			me->mind3=4*2+(byte)Random(2);

			return;
		}
	}
	if(me->seq==ANIM_A2)
	{
		// charging
		if(me->AttackCheck(32,me->x>>FIXSHIFT,me->y>>FIXSHIFT,goodguy))
		{
			goodguy->GetShot(Cosine(me->facing*32)*4,Sine(me->facing*32)*4,monsType[me->type].damage,map,world);
		}
	}
}

void AI_Ghost(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	// banshees glow
	if(me->type==MONS_GHOST3)
		map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,
						 (me->y/TILE_HEIGHT)>>FIXSHIFT,8,9);
	// shades make darkness
	else if(me->type==MONS_GHOST2)
		map->DarkTorch((me->x/TILE_WIDTH)>>FIXSHIFT,
						 (me->y/TILE_HEIGHT)>>FIXSHIFT,-4,8);

	BasicAI(me,SND_GHOSTOUCH,SND_GHOSTDIE,map,world,goodguy);

	if(me->mind3>0)
		me->mind3--;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==5)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			if(me->AttackCheck(24,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(Cosine(me->facing*32)*2,Sine(me->facing*32)*2,monsType[me->type].damage,map,world);
			}
		}
		if(me->seq==ANIM_A1 && me->frm==4 && me->reload==0)
		{
			// banshees teleport randomly
			if(me->type==MONS_GHOST3)
			{
				me->x=(me->x-320*FIXAMT)+Random(640*FIXAMT);
				me->y=(me->y-240*FIXAMT)+Random(480*FIXAMT);
				if(me->x<32*FIXAMT)
					me->x=32*FIXAMT;
				if(me->y<32*FIXAMT)
					me->y=32*FIXAMT;
				if(me->x>map->width*TILE_WIDTH*FIXAMT-TILE_WIDTH*FIXAMT)
					me->x=map->width*TILE_WIDTH*FIXAMT-TILE_WIDTH*FIXAMT;
				if(me->y>map->height*TILE_HEIGHT*FIXAMT-TILE_HEIGHT*FIXAMT)
					me->y=map->height*TILE_HEIGHT*FIXAMT-TILE_HEIGHT*FIXAMT;
			}
			else	// shades teleport to player
			{
				me->x=Random(80*FIXAMT)+40*FIXAMT;
				if(Random(2)==0)
					me->x=-me->x;
				me->x+=goodguy->x;
				me->y=Random(60*FIXAMT)+30*FIXAMT;
				if(Random(2)==0)
					me->y=-me->y;
				me->y+=goodguy->y;
				if(me->x<32*FIXAMT)
					me->x=32*FIXAMT;
				if(me->y<32*FIXAMT)
					me->y=32*FIXAMT;
				if(me->x>map->width*TILE_WIDTH*FIXAMT-TILE_WIDTH*FIXAMT)
					me->x=map->width*TILE_WIDTH*FIXAMT-TILE_WIDTH*FIXAMT;
				if(me->y>map->height*TILE_HEIGHT*FIXAMT-TILE_HEIGHT*FIXAMT)
					me->y=map->height*TILE_HEIGHT*FIXAMT-TILE_HEIGHT*FIXAMT;
			}
			for(x=0;x<4;x++)
			{
				map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,
							 (me->y/TILE_HEIGHT)>>FIXSHIFT,16,9);
			}
			me->reload=5;
		}
		if(me->seq==ANIM_A2 && (me->frm==2 || me->frm==8 || me->frm==14))
		{
			MakeSound(SND_GHOSTSPIN,me->x,me->y,SND_CUTOFF,1200);
			if(me->frm==14)
			{
				me->dx=Cosine(me->facing*32)*6;
				me->dy=Sine(me->facing*32)*6;
			}
		}
		if(me->seq==ANIM_A2 && me->frm==18)
			MakeSound(SND_GHOSTPUNCH,me->x,me->y,SND_CUTOFF,1200);
		if(me->seq==ANIM_A2 && me->frm>=17 && me->frm<=25)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			if(me->AttackCheck(24,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(Cosine(me->facing*32)*8,Sine(me->facing*32)*8,monsType[me->type].damage*3,map,world);
			}
		}
		if(me->seq==ANIM_A2 && me->frm>21)
		{
			Dampen(&me->dx,FIXAMT/4);
			Dampen(&me->dy,FIXAMT/4);
		}
		if(me->seq==ANIM_A3 && me->frm==2 && me->reload==0)
		{
			// scream
			me->reload=10;

			MakeColdRingParticle(me->x,me->y,0,180);
			if(RangeToTarget(me,goodguy)<200*FIXAMT)
			{
				goodguy->dx=0;
				goodguy->dy=0;
				player.stone=30*2;
				//MakeSound(SND_FREEZE,me->x,me->y,SND_CUTOFF,1200);
			}
		}
		return;	// can't do nothin' right now
	}

	// punch
	// Shades only use super punches if player is stone
	if(RangeToTarget(me,goodguy)<(88*FIXAMT) && Random(6)==0 &&
		(me->type!=MONS_GHOST2 || player.stone==0))
	{
		// get him!
		MakeSound(SND_GHOSTPUNCH,me->x,me->y,SND_CUTOFF,1200);
		DoMove(me,ANIM_ATTACK,164,1,0,0);
		me->reload=0;
		FaceGoodguy(me,goodguy);
		return;
	}
	// Super punch!!
	if(me->type==MONS_GHOST2 && RangeToTarget(me,goodguy)<(160*FIXAMT) && player.stone &&
		Random(32)==0)
	{
		DoMove(me,ANIM_A2,256,1,0,0);
		me->reload=0;
		FaceGoodguy(me,goodguy);
		return;
	}
	// teleport if non-Ghost
	if(me->type!=MONS_GHOST && RangeToTarget(me,goodguy)>(320*FIXAMT) && Random(32)==0)
	{
		MakeSound(SND_GHOSTTELEPORT,me->x,me->y,SND_CUTOFF,1200);
		DoMove(me,ANIM_A1,128,1,0,0);
		me->reload=0;
		return;
	}
	// Banshees scream
	if(me->type==MONS_GHOST3 && RangeToTarget(me,goodguy)<256*FIXAMT && me->mind3==0 && Random(128)==0)
	{
		// scream
		me->mind3=30*5;
		MakeSound(SND_GHOSTSCREAM,me->x,me->y,SND_CUTOFF,1200);
		DoMove(me,ANIM_A3,128,1,0,0);
		me->reload=0;
		return;
	}
	WanderAI(me,256,20,3,map,world,goodguy);
}

void AI_Pumpkin(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(RangeToTarget(me,goodguy)>512*FIXAMT)
		return;

	BasicAI(me,SND_PUMPKINOUCH,SND_PUMPKINDIE,map,world,goodguy);

	if(me->action==ACTION_BUSY)
		return;
	if(RangeToTarget(me,goodguy)<48*FIXAMT)
	{
		// 'acidic'
		goodguy->GetShot(0,0,monsType[me->type].damage,map,world);
	}
	if(me->type==MONS_PUMPKIN4)
	{
		if(Random(10)==0)
			FireBullet(me->x,me->y,(byte)Random(256),BLT_BIGSHOT);
	}

	WanderAI(me,1024,5,3,map,world,goodguy);
}

void AI_Vampire(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	Guy *g;

	BasicAI(me,SND_VAMPOUCH,SND_VAMPDIE,map,world,goodguy);

	if(me->mind3>0)
		me->mind3--;

	if(me->mind2>0)
		me->mind2--;

	if(me->action==ACTION_BUSY)
	{
		// shove player
		if(me->seq==ANIM_ATTACK && me->frm==4)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			if(me->AttackCheck(24,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(Cosine(me->facing*32)*2,Sine(me->facing*32)*2,monsType[me->type].damage,map,world);
			}
		}
		if(me->seq==ANIM_A1 && me->frm==4 && me->reload==0)
		{
			// summon something
			if(me->type==MONS_VAMPIRE3)
			{
				// summon Gangrenes, one on either side
				x=me->x-64*FIXAMT;
				y=me->y;
				g=AddGuy(x,y,0,MONS_BONEHEAD3);
				if(g)
				{
					if(!g->CanWalk(g->x,g->y,map,world))
						g->type=MONS_NONE;	// vanish if this spot is occupied
					else if(!map->CheckLOS(me->mapx,me->mapy,5,g->mapx,g->mapy))
						g->type=MONS_NONE;
					else // do resurrect move
						DoMove(g,ANIM_A3,128,1,0,0);
				}
				x=me->x+64*FIXAMT;
				y=me->y;
				g=AddGuy(x,y,0,MONS_BONEHEAD3);
				if(g)
				{
					if(!g->CanWalk(g->x,g->y,map,world))
						g->type=MONS_NONE;	// vanish if this spot is occupied
					else if(!map->CheckLOS(me->mapx,me->mapy,5,g->mapx,g->mapy))
						g->type=MONS_NONE;
					else // do resurrect move
						DoMove(g,ANIM_A3,128,1,0,0);
				}
			}
			if(me->type==MONS_VAMPIRE4)
			{
				// summon 4 ghosts
				g=AddGuy(me->x,me->y,0,MONS_GHOST);
				if(g) // do end half of teleport move
				{
					DoMove(g,ANIM_A1,128,1,0,0);
					g->frm=4;
				}
				g=AddGuy(me->x,me->y,0,MONS_GHOST);
				if(g) // do end half of teleport move
				{
					DoMove(g,ANIM_A1,128,1,0,0);
					g->frm=4;
				}
				g=AddGuy(me->x,me->y,0,MONS_GHOST);
				if(g) // do end half of teleport move
				{
					DoMove(g,ANIM_A1,128,1,0,0);
					g->frm=4;
				}
				g=AddGuy(me->x,me->y,0,MONS_GHOST);
				if(g) // do end half of teleport move
				{
					DoMove(g,ANIM_A1,128,1,0,0);
					g->frm=4;
				}
			}
			me->reload=5;
		}
		if(me->seq==ANIM_A2 && me->frm==2 && me->reload==0)
		{
			// shoot something
			if(me->type==MONS_VAMPIRE2)
			{
				// shoot bullet
				x=me->x+Cosine(me->facing*32)*16;
				y=me->y+Sine(me->facing*32)*16;
				FireBullet(x,y,me->facing*32,BLT_ENERGY);
			}
			if(me->type==MONS_VAMPIRE3)
			{
				// shoot poison gas
				x=me->x+Cosine(me->facing*32)*16;
				y=me->y+Sine(me->facing*32)*16;
				FireBullet(x,y,me->facing*32,BLT_GASBLAST);
			}
			if(me->type==MONS_VAMPIRE4)
			{
				// shoot a bat
				x=me->x+Cosine(me->facing*32)*32;
				y=me->y+Sine(me->facing*32)*32;
				g=AddGuy(x,y,0,MONS_BAT);
				if(g && (!g->CanWalk(g->x,g->y,map,world)))
					g->type=MONS_NONE;	// vanish if this spot is occupied
				else if(g)
				{
					g->facing=me->facing;
					DoMove(g,ANIM_A1,128,1,Cosine(me->facing*32)*8,Sine(me->facing*32)*8);
				}
			}
			me->reload=5;
		}
		return;	// can't do nothin' right now
	}

	// the two lower classes of vampire use shoves
	if(me->type<MONS_VAMPIRE3)
	{
		if(RangeToTarget(me,goodguy)<64*FIXAMT && Random(4)==0 &&
			map->CheckLOS(me->mapx,me->mapy,5,goodguy->mapx,goodguy->mapy))
		{
			FaceGoodguy(me,goodguy);
			if(me->type==MONS_VAMPIRE2)
				x=256;
			else
				x=180;
			MakeSound(SND_VAMPSHOVE,me->x,me->y,SND_CUTOFF,1200);
			DoMove(me,ANIM_ATTACK,x,1,0,0);
			return;
		}
	}

	// all non-Junior vampires shoot things
	if(me->type>MONS_VAMPIRE)
	{
		if(RangeToTarget(me,goodguy)<300*FIXAMT && Random(48)==0 && me->mind2==0 &&
			map->CheckLOS(me->mapx,me->mapy,15,goodguy->mapx,goodguy->mapy))
		{
			// Vamp Lords are limited to one shot every second, to reduce the insanity
			if(me->type==MONS_VAMPIRE3)
				me->mind2=30;
			else	// 1 second for Draculites
				me->mind2=30;
			FaceGoodguy(me,goodguy);
			MakeSound(SND_VAMPSHOOT,me->x,me->y,SND_CUTOFF,1200);
			DoMove(me,ANIM_A2,128,1,0,0);
			return;
		}
	}

	// everything above Bloodsucker has a summon attack
	if(me->type>MONS_VAMPIRE2 && map->CheckLOS(me->mapx,me->mapy,15,goodguy->mapx,goodguy->mapy))
	{
		if(Random(320)==0 && me->mind3==0)
		{
			me->mind3=5*30;
			MakeSound(SND_VAMPSUMMON,me->x,me->y,SND_CUTOFF,1200);
			DoMove(me,ANIM_A1,128,1,0,0);
			return;
		}
	}

	WanderAI(me,256,20,3,map,world,goodguy);
}

void AI_StickWitch(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	bullet_t *b;

	SetBossHP(me->hp,MonsterHP(me->type));

	BasicAI(me,SND_STICKOUCH,SND_STICKDIE,map,world,goodguy);

	if(me->mind3<2)
	{
		if(me->mind3==0 && (RangeToTarget(me,goodguy)<400*FIXAMT || me->ouch))
		{
			BeginChatting(236);
			me->mind3=1;
			return;
		}
		else if(me->mind3==1 && (RangeToTarget(me,goodguy)<400*FIXAMT || me->ouch))
		{
			me->mind3=2;
			MakeSound(SND_WITCHHI,me->x,me->y,SND_CUTOFF,1200);
			DoMove(me,ANIM_A2,128,1,0,0);
		}
		return;
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->reload==0)
		{
			// shoot
			switch(me->mind2)
			{
				case 0:	// blast spell
					MakeSound(SND_CATKICK,me->x,me->y,SND_CUTOFF,1200);
					x=me->x+Cosine(me->mind*32)*16;
					y=me->y+Sine(me->mind*32)*16;
					b=FireBullet(x,y,(me->mind*32+256-16+(byte)Random(32)),BLT_BIGSHOT);
					if(b)
					{
						b->dx*=2;
						b->dy*=2;
					}
					me->reload=2;
					break;
				case 1:	// gas blast
					MakeSound(SND_SKELGAS,me->x,me->y,SND_CUTOFF,1200);
					x=me->x+Cosine(me->mind*32)*16;
					y=me->y+Sine(me->mind*32)*16;
					b=FireBullet(x,y,(me->mind*32+256-16+(byte)Random(32)),BLT_GASBLAST);
					if(b)
					{
						b->dx*=2;
						b->dy*=2;
					}
					me->reload=1;
					break;
				case 2:	// flames
					MakeSound(SND_STEAM,me->x,me->y,SND_CUTOFF,1200);
					x=me->x+Cosine(me->mind*32)*16;
					y=me->y+Sine(me->mind*32)*16;
					b=FireBullet(x,y,(me->mind*32+256-16+(byte)Random(32)),BLT_FLAMESHOT);
					me->reload=1;
					break;
				case 3:	// wind missiles
					MakeSound(SND_FRANKMISSILE,me->x,me->y,SND_CUTOFF,1200);
					x=me->x+Cosine(me->mind*32)*16;
					y=me->y+Sine(me->mind*32)*16;
					b=FireBullet(x,y,(me->mind*32+256-16+(byte)Random(32)),BLT_WIND);
					me->reload=3;
					break;
				case 4: // earth spikes
					MakeSound(SND_ZOMBIESTOMP,me->x,me->y,SND_CUTOFF,1200);
					x=me->x+Cosine(me->mind*32)*16;
					y=me->y+Sine(me->mind*32)*16;
					b=FireBullet(x,y,(me->mind*32+256-16+(byte)Random(32)),BLT_EARTHSPIKE2);
					me->reload=3;
					break;
			}
		}
		return;	// can't do nothin' right now
	}

	if(RangeToTarget(me,goodguy)<512*FIXAMT && Random(16)==0)
	{
		x=me->facing;
		FaceGoodguy3(me,goodguy);
		me->mind=me->facing;
		me->facing=x;
		DoMove(me,ANIM_ATTACK,128,1,me->dx,me->dy);
		return;
	}


	if(me->mind3>2)
	{
		me->mind3--;
		if(me->mind3<10)
		{
			MakeRingParticle(me->x,me->y,0,(15-me->mind3)*3,50);
			if(RangeToTarget(me,goodguy)<70*FIXAMT)
				goodguy->GetShot(Cosine(me->facing)*16,Sine(me->facing)*16,8,map,world);
		}
	}
	else
	{
		me->mind2=(byte)Random(5);
		me->mind3=30*2+(byte)Random(30*5);
		MakeSound(SND_STICKATTACK,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->ouch>0)
	{
		if(me->mind3>10)
			me->mind3-=5;
		DoMove(me,ANIM_A1,128,0,me->dx,me->dy);
		return;
	}

	if(!(me->mind1--))	// time to get a new direction
	{
		me->facing=(byte)Random(8);
		me->mind1=(byte)Random(30*3)+1;
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

void AI_Tumblerock(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->action==ACTION_BUSY)
	{
		// can be killed by touch o death
		return;
	}

	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=256;
		me->action=ACTION_IDLE;
	}

	if(me->AttackCheck(36,me->x>>FIXSHIFT,me->y>>FIXSHIFT,goodguy))
	{
		goodguy->GetShot(me->dx,me->dy,100,map,world);
	}

	if(me->mind1)
	{
		if(me->type==MONS_TUMBLEROCK || me->type==MONS_TUMBLEROCK3)
			me->facing=(me->facing+1)&3;
		else
			me->facing=(me->facing+3)&3;
		me->mind1=0;
		MakeSound(SND_STONEBONK,me->x,me->y,SND_CUTOFF,400);
	}

	switch(me->facing)
	{
		case 0:
			me->dx=FIXAMT*5;
			me->dy=0;
			break;
		case 1:
			me->dx=0;
			me->dy=FIXAMT*5;
			break;
		case 2:
			me->dx=-FIXAMT*5;
			me->dy=0;
			break;
		case 3:
			me->dx=0;
			me->dy=-FIXAMT*5;
			break;
	}
}

void AI_Lightball(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->ouch==4)
		MakeSound(SND_LIGHTSMACK,me->x,me->y,SND_CUTOFF,1200);

	if(me->seq==ANIM_DIE && me->frm==0 && me->reload==0)
	{
		FireBullet(me->x,me->y,0,BLT_GOODBOOM);
		me->reload=1;
	}
	me->mind++;
	me->mind1++;

	if(RangeToTarget(me,goodguy)<400*FIXAMT)
	{
		switch(me->type)
		{
			case MONS_LIGHTBALL:
				if((me->mind&32)==0)
					Shock(map,world,me->type,me->mapx,me->mapy,me->mind1);
				break;
			case MONS_LIGHTBALL2:
				if(((me->mind+16)&64)==0)
					Shock(map,world,me->type,me->mapx,me->mapy,me->mind1);
				break;
			case MONS_LIGHTBALL3:
				Shock(map,world,me->type,me->mapx,me->mapy,me->mind1);
				break;
			case MONS_LIGHTBALL4:
				Shock(map,world,me->type,me->mapx,me->mapy,me->mind1);
				break;
			case MONS_LIGHTBALL5:
				break;
		}
	}
}

void AI_Boneheadomatic(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	Guy *g;

	if(me->ouch==4)
		MakeSound(SND_LIGHTSMACK,me->x,me->y,SND_CUTOFF,1200);

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==26)
		{
			me->seq=ANIM_A1;
			me->frm=0;
			me->frmTimer=0;
		}
		if(me->seq==ANIM_A1 && me->frm==18 && me->frmTimer<128)
		{
			if(me->type==MONS_BHOMATIC)
				g=AddGuy(me->x,me->y+36*FIXAMT,0,MONS_BONEHEAD);
			else if(me->type==MONS_RBOMATIC)
				g=AddGuy(me->x,me->y+36*FIXAMT,0,MONS_BONEHEAD2);
			else if(me->type==MONS_GGOMATIC)
				g=AddGuy(me->x,me->y+36*FIXAMT,0,MONS_BONEHEAD3);
			else if(me->type==MONS_NSOMATIC)
				g=AddGuy(me->x,me->y+36*FIXAMT,0,MONS_BONEHEAD4);
			if(g && (!g->CanWalk(g->x,g->y,map,world)))
				g->type=MONS_NONE;	// vanish if this spot is occupied
		}
		if(me->seq==ANIM_DIE && me->frm==1 && me->reload==0)
		{
			me->reload=4;
			FireBullet(me->x,me->y,0,BLT_GOODBOOM2);
		}
		return;
	}

	if(RangeToTarget(me,goodguy)<1024*FIXAMT)
	{
		me->mind++;
		if(me->mind>30)
		{
			me->mind=0;
 			DoMove(me,ANIM_ATTACK,128,1,0,0);
		}
	}
}

void AI_Frankenjulie(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	Guy *g;

	SetBossHP(me->hp,MonsterHP(me->type));

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
		{
			if(me->mind<2)
				MakeSound(SND_FRANKOUCH,me->x,me->y,SND_CUTOFF,1200);
			else
				MakeSound(SND_FRANKOUCH2,me->x,me->y,SND_CUTOFF,1200);

			if(me->hp<monsType[me->type].hp/2 && me->mind==0)
			{
				me->mind=1;
				FireBullet(me->x,me->y,0,BLT_MEGABOOM);
				for(x=0;x<20;x++)
					FireBullet(me->x,me->y,ITM_TINYHEART,BLT_ITEM);
				DoMove(me,ANIM_A1,128,0,0,0);
				ShakeScreen(5);
			}
			if(me->hp<monsType[me->type].hp/4 && me->mind==1)
			{
				me->mind=2;
				FireBullet(me->x,me->y,0,BLT_MEGABOOM);
				for(x=0;x<20;x++)
					FireBullet(me->x,me->y,ITM_TINYHEART,BLT_ITEM);
				DoMove(me,ANIM_A1,128,0,0,0);
				ShakeScreen(5);
				g=AddGuy(me->x-FIXAMT*150,me->y,0,MONS_HAND);
				if(g && (!g->CanWalk(g->x,g->y,map,world)))
					g->type=MONS_NONE;	// vanish if this spot is occupied
				g=AddGuy(me->x+FIXAMT*150,me->y,0,MONS_HAND);
				if(g && (!g->CanWalk(g->x,g->y,map,world)))
					g->type=MONS_NONE;	// vanish if this spot is occupied
			}
		}
		else
		{
			MakeSound(SND_FRANKDIE,me->x,me->y,SND_CUTOFF,1200);
		}
	}

	if(me->mind2==0)
	{
		if(RangeToTarget(me,goodguy)<256*FIXAMT || me->ouch)
		{
			me->mind2=1;
			BeginChatting(251);
		}
		return;
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			ShakeScreen(120);
			FireBullet(me->x-60*FIXAMT+Random(120*FIXAMT),
						   me->y-60*FIXAMT+Random(120*FIXAMT),0,BLT_MEGABOOM);
		}
		if(me->seq==ANIM_ATTACK)
		{
			if(me->frmTimer<128)
				MakeSound(SND_FRANKHEAD,me->x,me->y,SND_CUTOFF,400);
			if(me->frm<16 && me->frmTimer<128)
			{
				x=(me->x+Cosine(me->frm*16)*80);
				y=(me->y+Sine(me->frm*16)*60);
				FireExactBullet(x,y,FIXAMT*70,Cosine(me->frm*16)*8,Sine(me->frm*16)*8,
					-FIXAMT*2,0,60,me->frm*16,BLT_ENERGY);
				x=(me->x+Cosine((me->frm+8)*16)*80);
				y=(me->y+Sine((me->frm+8)*16)*60);
				FireExactBullet(x,y,FIXAMT*70,Cosine((me->frm+8)*16)*8,Sine((me->frm+8)*16)*8,
					-FIXAMT*2,0,60,(me->frm+8)*16,BLT_ENERGY);

			}
		}
		if(me->seq==ANIM_A3)
		{
			if(me->frm>4 && me->frm<20)
			{
				me->dx+=Cosine(me->facing*32);
				me->dy+=Sine(me->facing*32);
				Clamp(&me->dx,6*FIXAMT);
				Clamp(&me->dy,6*FIXAMT);

				x=(me->x+Cosine((me->frm-4)*16)*200)/FIXAMT;
				y=(me->y+Sine((me->frm-4)*16)*160)/FIXAMT;
				if(me->AttackCheck(50,x,y,goodguy))
					goodguy->GetShot(Cosine((me->frm-4)*16)*8,Sine((me->frm-4)*16)*8,3,map,world);
				x=(me->x+Cosine((me->frm+4)*16)*200)/FIXAMT;
				y=(me->y+Sine((me->frm+4)*16)*160)/FIXAMT;
				if(me->AttackCheck(50,x,y,goodguy))
					goodguy->GetShot(Cosine((me->frm+4)*16)*8,Sine((me->frm+4)*16)*8,3,map,world);
			}
			else if(me->frm>=20)
			{
				me->dx=(me->dx*3)/4;
				me->dy=(me->dy*3)/4;
				if(me->frm==21)
					DoMove(me,ANIM_A1,128,0,0,0);
			}
		}
		if(me->seq==ANIM_A4 && me->frm==4 && me->frmTimer<128)
		{
			x=me->x-120*FIXAMT;
			y=me->y-20*FIXAMT;
			FireBullet(x,y,128,BLT_MISSILE);
			FireBullet(x,y-30*FIXAMT,148,BLT_MISSILE);
			x=me->x+120*FIXAMT;
			y=me->y-20*FIXAMT;
			FireBullet(x,y,0,BLT_MISSILE);
			FireBullet(x,y-30*FIXAMT,236,BLT_MISSILE);
			MakeSound(SND_FRANKMISSILE,me->x,me->y,SND_CUTOFF,1200);
		}
		if(me->seq==ANIM_A4 && me->frm==10)
		{
			DoMove(me,ANIM_A2,128,0,0,0);
		}
		return;
	}
	if(RangeToTarget(me,goodguy)<96*FIXAMT)
	{
		// 'acidic'
		goodguy->GetShot(0,0,monsType[me->type].damage,map,world);
	}

	FaceGoodguy2(me,goodguy);
	me->dx+=Cosine(me->facing*32)/4;
	me->dy+=Sine(me->facing*32)/4;
	Clamp(&me->dx,monsType[me->type].speed);
	Clamp(&me->dy,monsType[me->type].speed);

	if(me->mind==0)	// have head
	{
		if(me->mind1==0)
			me->mind1=30*3;
		else
		{
			me->mind1--;
			if(me->mind1==0)
			{
				DoMove(me,ANIM_ATTACK,128,1,me->dx,me->dy);
			}
		}
	}
	if(me->mind==1)	// lost head already
	{
		if(me->seq==ANIM_IDLE || me->seq==ANIM_MOVE || (me->seq==ANIM_A1 && me->frm>1))
		{
			DoMove(me,ANIM_A1,128,0,me->dx,me->dy);
		}
		if(me->mind1==0)
			me->mind1=30*3;
		else
		{
			me->mind1--;
			if(me->mind1==0)
			{
				MakeSound(SND_FRANKARMS,me->x,me->y,SND_CUTOFF,1200);
				DoMove(me,ANIM_A3,256,1,me->dx,me->dy);
				FaceGoodguy(me,goodguy);
			}
			if(me->mind1%15==0)
			{
				MakeSound(SND_GRENADE,me->x,me->y,SND_CUTOFF,1200);
				FireBullet(me->x,me->y,(byte)Random(256),BLT_ORBGRENADE);
			}
		}
	}
	if(me->mind==2)	// lost body too
	{
		if(me->seq==ANIM_IDLE || me->seq==ANIM_MOVE || (me->seq==ANIM_A2 && me->frm>1))
		{
			DoMove(me,ANIM_A2,128,0,me->dx,me->dy);
		}
		if(me->mind1==0)
			me->mind1=30*2;
		else
		{
			me->mind1--;
			if(me->mind1==0)
			{
				DoMove(me,ANIM_A4,128,1,me->dx,me->dy);
			}
		}
	}
}

void AI_Hand(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	BasicAI(me,SND_PUMPKINOUCH,SND_PUMPKINDIE,map,world,goodguy);

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==7 && me->reload==0)
		{
			x=me->x+Cosine(me->facing*32)*40;
			y=me->y+Sine(me->facing*32)*40;
			if(me->AttackCheck(32,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				goodguy->GetShot(Cosine(me->facing*32)*8,Sine(me->facing*32)*8,monsType[me->type].damage,map,world);
			me->reload=5;
		}
		return;	// can't do nothin' right now
	}

	if(RangeToTarget(me,goodguy)<(128*FIXAMT) && Random(20)==0)
	{
		// get him!
		MakeSound(SND_SKELKICK,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=256;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		me->reload=0;
		return;
	}

	WanderAI(me,512,10,10,map,world,goodguy);
}

void AI_Polterguy(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int i;
	int x,y;
	Guy *g;

	SetBossHP(me->hp,MonsterHP(me->type));

	BasicAI(me,SND_POLTEROUCH,SND_POLTERDIE,map,world,goodguy);

	if(me->mind==0)
	{
		me->mind=1;
		BeginChatting(249);
		for(i=0;i<15;i++)
			LaunchJunk(map,me);
		me->reload=60;
		me->mind3=120+15;
		me->facing=(byte)Random(256);
		me->dx=Cosine(me->facing)*2;
		me->dy=Sine(me->facing)*2;
	}

	if(me->mind1)
	{
		if(me->mind1&1)
			me->dx=-me->dx;
		if(me->mind1&2)
			me->dy=-me->dy;

		if(Random(20)==0)
		{
			me->facing=(byte)Random(256);
			me->dx=Cosine(me->facing)*2;
			me->dy=Sine(me->facing)*2;
		}
		me->mind1=0;
	}

	if(RangeToTarget(me,goodguy)<128*FIXAMT)
		PoisonPlayer(32*3);

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==4 && me->reload==0)
		{
			//LaunchJunk(map,me);
			LaunchJunk(map,me);
			LaunchJunk(map,me);
			me->reload=30*2;

			x=me->x-300*FIXAMT+Random(600)*FIXAMT;
			y=me->y-200*FIXAMT+Random(500)*FIXAMT;
			g=AddGuy(x,y,FIXAMT*50,MONS_GHOST);
			if(g && !g->CanWalk(g->x,g->y,map,world))
			{
				g->type=MONS_NONE;
			}
			else if(g)
			{
				DoMove(g,ANIM_A1,128,1,0,0);
				g->frm=4;
			}
		}
		if(me->seq==ANIM_A1 && me->frm==15 && me->frmTimer<128)
		{
			Sneeze();
		}
		return;	// can't do nothin' right now
	}

	if(me->reload==0)
	{
		MakeSound(SND_POLTERGET,me->x,me->y,SND_CUTOFF,1200);
		DoMove(me,ANIM_ATTACK,128,1,me->dx,me->dy);
	}
	if(me->mind3)
	{
		me->mind3--;
		if(me->mind3==0)
		{
			MakeSound(SND_POLTERSNEEZE,me->x,me->y,SND_CUTOFF,1200);
			DoMove(me,ANIM_A1,128,1,me->dx,me->dy);
			me->mind3=120;
		}
	}
}

void AI_Junk(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	BasicAI(me,SND_LIGHTSMACK,SND_BOOM,map,world,goodguy);

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			ExplodeParticles(PART_DIRT,me->x,me->y,me->z,16);
		}
		return;
	}
	if(me->parent==NULL || me->parent->hp==0)
	{
		me->hp=0;
		DoMove(me,ANIM_DIE,128,1,0,0);
	}

	if(RangeToTarget(me,goodguy)<20*FIXAMT)
	{
		goodguy->GetShot(me->dx,me->dy,2,map,world);
	}

	me->frmAdvance=180;
	FaceGoodguy(me,me->parent);
	me->dx+=Cosine(me->facing*32)/2;
	me->dy+=Sine(me->facing*32)/2;
	// circling Polterguy
	if(me->mind==0)
	{
		if(me->x<me->parent->x)
		{
			// on left side
			if(me->y<me->parent->y)
			{
				// upper left
				me->dy+=FIXAMT;
			}
			else
				// lower left
				me->dx+=FIXAMT;
		}
		else
		{
			// on right side
			if(me->y<me->parent->y)
			{
				// upper left
				me->dx-=FIXAMT;
			}
			else
				// lower left
				me->dy-=FIXAMT;
		}
		Dampen(&me->dx,FIXAMT/4);
		Dampen(&me->dy,FIXAMT/4);
		Clamp(&me->dx,FIXAMT*11);
		Clamp(&me->dy,FIXAMT*11);
	}
	else
	{
		// flying outward
		me->mind1--;
		if(me->mind1==0)
			me->mind=0;
	}
}

void AI_HelperBat(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	Guy *g;
	word w;

	BasicAI(me,SND_BATOUCH,SND_BATDIE,map,world,goodguy);

	if(!player.var[VAR_QUESTDONE + QUEST_FARLEY])
	{
		if(RangeToTarget(me,goodguy)<FIXAMT*72 && me->reload==0)
		{
			BeginChatting(244);
			me->reload=30;
		}
		return;
	}

	if(me->mind1)
		me->mind1--;

	w=LockOnEvil(me->x>>FIXSHIFT,me->y>>FIXSHIFT);
	if(w==65535)
		g=NULL;
	else
		g=GetGuy(w);

	if(player.batLevel>64)
		map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,(me->y/TILE_HEIGHT)>>FIXSHIFT,(char)(player.batLevel/16),(byte)(player.batLevel/8));

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_A1 && me->frm==3 && me->reload==0)
		{
			// shoot
			if(opt.cheats[CH_STINKY])
			{
				FireBullet(me->x,me->y,me->facing*32,BLT_BATGAS);
			}
			else
			{
				MakeSound(SND_BATSHOOT,me->x,me->y,SND_CUTOFF,600);
				FireBullet(me->x,me->y,me->facing*32,BLT_BATSHOT);
			}
			me->reload=(byte)(32+8-player.batLevel/4);
		}
		return;
	}
	if((player.hearts<player.maxHearts && opt.cheats[CH_SIDEKICK]==0) || opt.cheats[CH_NOFARLEY])
	{
		DoMove(me,ANIM_DIE,256,1,0,0);
		player.fireFlags&=(~FF_HELPERHERE);
		player.batLevel=0;
		return;
	}
	player.fireFlags|=FF_HELPERHERE;

	if(me->mind==0)	// standard combat mode
	{
		if(RangeToTarget(me,goodguy)>600*FIXAMT || g==NULL)
		{
			// too far from goodguy, run home
			me->mind=1;
		}
		else
		{
			if(g==NULL || RangeToTarget(me,g)>500*FIXAMT)	// too far to fight, go home
			{
				me->mind=1;
			}
			else
			{
				if(me->mind1==0 && g!=NULL)
				{
					FaceGoodguy2(me,g);
					me->mind1=2;
				}
				if(me->reload==0 && g!=NULL && RangeToTarget(me,g)<256*FIXAMT)
				{
					if(player.batLevel<32*4)
					{
						DoMove(me,ANIM_A1,32+player.batLevel*2,1,0,0);
					}
					else
					{
						// don't have to stop to shoot
						if(opt.cheats[CH_STINKY])
						{
							FireBullet(me->x,me->y,me->facing*32,BLT_BATGAS);
							me->reload=(byte)((68-player.batLevel/4)/2);
						}
						else
						{
							MakeSound(SND_BATSHOOT,me->x,me->y,SND_CUTOFF,600);
							FireBullet(me->x,me->y,me->facing*32,BLT_BATSHOT);
							if(player.batLevel==255)
							{	// triple fire when maxed out
								FireBullet(me->x,me->y,(me->facing*32+256-48)&255,BLT_BATSHOT);
								FireBullet(me->x,me->y,(me->facing*32+48)&255,BLT_BATSHOT);
							}
							me->reload=(byte)((68-player.batLevel/4)/2);
						}
					}
				}
			}
		}
	}
	else	// running home mode
	{
		if(RangeToTarget(me,goodguy)<200*FIXAMT && g!=NULL && RangeToTarget(me,g)<500*FIXAMT)
		{
			// got close enough, get violent again
			me->mind=0;
		}
		if(me->mind1==0)
		{
			FaceGoodguy2(me,goodguy);
			me->mind1=2;
		}
	}
	me->frmAdvance=256;
	me->dx+=Cosine(me->facing*32);
	me->dy+=Sine(me->facing*32);
	w=(word)(FIXAMT*4+(player.batLevel/4)*FIXAMT/8);
	Clamp(&me->dx,w);
	Clamp(&me->dy,w);
}

void AI_WindElder(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	Guy *g;

	SetBossHP(me->hp,MonsterHP(me->type));

	BasicAI(me,SND_ELDEROUCH,SND_ELDERDIE,map,world,goodguy);

	if(player.var[240]==0)
	{
		if(RangeToTarget(me,goodguy)<256*FIXAMT || me->ouch)
		{
			player.var[240]=1;
			BeginChatting(243);
		}
		return;
	}

	if(me->mind3)
		me->mind3--;
	else
		me->mind3=30*6;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm<6)
		{
			me->dx=Cosine(me->facing*32)*(6+me->frm*2);
			me->dy=Sine(me->facing*32)*(6+me->frm*2);
		}
		else if(me->seq==ANIM_ATTACK)
		{
			me->dx/=2;
			me->dy/=2;
		}
		if(me->seq==ANIM_ATTACK && me->frm>=3 && me->frm<=6)
		{
			x=me->x+Cosine(me->facing*32)*30;
			y=me->y+Sine(me->facing*32)*30;
			if(me->AttackCheck(32,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				goodguy->GetShot(Cosine(me->facing*32)*16,Sine(me->facing*32)*16,monsType[me->type].damage,map,world);
			me->reload=5;
		}
		if(me->seq==ANIM_A1 && me->frmTimer<128)
		{
			x=me->x-400*FIXAMT+Random(800*FIXAMT);
			y=me->y-300*FIXAMT+Random(600*FIXAMT);
			g=AddGuy(x,y,0,MONS_GHOST);
			if(g && !g->CanWalk(g->x,g->y,map,world))
				g->type=MONS_NONE;
		}
		if(me->seq==ANIM_A2 && me->frm==2 && me->reload==0)
		{
			FireBullet(me->x,me->y,(me->facing*32-96+256)&255,BLT_WIND);
			FireBullet(me->x,me->y,(me->facing*32-72+256)&255,BLT_WIND);
			FireBullet(me->x,me->y,(me->facing*32+96+256)&255,BLT_WIND);
			FireBullet(me->x,me->y,(me->facing*32+72+256)&255,BLT_WIND);
			me->reload=4;
		}
		if(me->seq==ANIM_DIE)
		{
			x=me->x-40*FIXAMT+Random(80*FIXAMT);
			y=me->y-30*FIXAMT+Random(60*FIXAMT);
			FireBullet(x,y,0,BLT_GOODBOOM+(byte)Random(2));
			MakeSound(SND_ELDERDIE,me->x,me->y-200*me->frm,SND_CUTOFF,2000);
		}
		return;	// can't do nothin' right now
	}

	if(me->mind3==0)
	{
		// summon hordes of ghosts
		MakeSound(SND_ELDERSUMMON,me->x,me->y,SND_CUTOFF,1200);
		DoMove(me,ANIM_A1,128,1,0,0);
		return;
	}

	if(RangeToTarget(me,goodguy)<(300*FIXAMT) && Random(20)==0)
	{
		// charge at him
		MakeSound(SND_ELDERCHARGE,me->x,me->y,SND_CUTOFF,1200);
		DoMove(me,ANIM_ATTACK,128,1,0,0);
		return;
	}
	if(Random(50)==0)
	{
		// fire wind missiles
		MakeSound(SND_ELDERSHOOT,me->x,me->y,SND_CUTOFF,1200);
		DoMove(me,ANIM_A2,128,1,0,0);
		return;
	}

	WanderAI(me,1024,2,1,map,world,goodguy);
}

void AI_EarthElder(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	SetBossHP(me->hp,MonsterHP(me->type));

	BasicAI(me,SND_ELDEROUCH,SND_ELDERDIE,map,world,goodguy);

	if(player.var[240]==0)
	{
		if(RangeToTarget(me,goodguy)<256*FIXAMT || me->ouch)
		{
			player.var[240]=1;
			BeginChatting(242);
		}
		return;
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==4 && me->reload==0)
		{
			FireBullet(me->x,me->y,(me->facing*32),BLT_EARTHSPIKE2);
			FireBullet(me->x,me->y,(me->facing*32-32+256)&255,BLT_EARTHSPIKE2);
			FireBullet(me->x,me->y,(me->facing*32+32)&255,BLT_EARTHSPIKE2);
			me->reload=5;
		}

		if(me->seq==ANIM_A1 && me->frm==3 && me->reload==0)
		{
			for(x=0;x<6;x++)
				FireBullet(me->x,me->y,(byte)Random(256),BLT_ORBGRENADE);
			me->reload=5;
		}
		if(me->seq==ANIM_A2 && me->frm<9)
		{
			x=me->x+Cosine(me->facing*32)*me->frm*48;
			y=me->y+Sine(me->facing*32)*me->frm*32;
			if(me->frm==8 && me->reload==0)
			{
				me->reload=3;
				FireBullet(x,y,0,BLT_MEGABOOM);
			}
			else if(me->frmTimer<128)
				FireBullet(x,y,0,BLT_BADBOOM);
		}
		if(me->seq==ANIM_DIE)
		{
			MakeSound(SND_ELDERDIE,me->x,me->y-200*me->frm,SND_CUTOFF,2000);
			// doesn't explode
		}
		return;	// can't do nothin' right now
	}

	if(me->y<(7*TILE_HEIGHT+TILE_HEIGHT*3/4)*FIXAMT)
	{
		BeginChatting(241);
		DoMove(me,ANIM_DIE,128,1,0,0);
		KilledBoss(BOSS_ELDER2);
		me->hp=0;
	}

	if(me->mind3)
		me->mind3-=2;
	else
		me->mind3=30*8;

	if(me->mind3==30*5 || me->mind3==30*3)
	{
		// launch grenades
		MakeSound(SND_ELDERSUMMON,me->x,me->y,SND_CUTOFF,1200);
		DoMove(me,ANIM_A1,128,1,0,0);
		return;
	}

	if(me->mind3==30*8 || me->mind3==30*6 || me->mind3==30*2)
	{
		// shoot earth spikes
		MakeSound(SND_ELDERCHARGE,me->x,me->y,SND_CUTOFF,1200);
		DoMove(me,ANIM_ATTACK,128,1,0,0);
		return;
	}

	FaceGoodguy(me,goodguy);

	me->dx=0;
	me->dy=0;
}

void AI_WaterElder(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	Guy *g;

	SetBossHP(me->hp,MonsterHP(me->type));

	BasicAI(me,SND_ELDEROUCH,SND_ELDERDIE,map,world,goodguy);

	if(player.var[240]==0)
	{
		if(RangeToTarget(me,goodguy)<256*FIXAMT || me->ouch)
		{
			player.var[240]=1;
			BeginChatting(240);
		}
		return;
	}

	if(me->mind3)
		me->mind3--;
	else
		me->mind3=30*6;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm>=3 && me->frm<=6)
		{
			FireBullet(me->x,me->y,me->facing*32+256-8+(byte)Random(16),BLT_ICESHARD);
			me->reload=5;
		}
		if(me->seq==ANIM_A2 && me->frm==2 && me->reload==0)
		{
			x=me->x+Cosine(me->facing*32)*30;
			y=me->y+Sine(me->facing*32)*30;
			g=AddGuy(x,y,FIXAMT*10,MONS_BUBBLE);
			if(g && g->CanWalk(g->x,g->y,map,world))
			{
				g->dx=Cosine(me->facing*32)*4;
				g->dy=Sine(me->facing*32)*4;
			}
			else if(g)
				g->type=MONS_NONE;
			me->reload=4;
		}
		if(me->seq==ANIM_DIE)
		{
			x=me->x-40*FIXAMT+Random(80*FIXAMT);
			y=me->y-30*FIXAMT+Random(60*FIXAMT);
			FireBullet(x,y,0,BLT_GOODBOOM+(byte)Random(2));
		}
		return;	// can't do nothin' right now
	}

	if(Random(40)==0)
	{
		// fire ice shards
		MakeSound(SND_ELDERCHARGE,me->x,me->y,SND_CUTOFF,1200);
		DoMove(me,ANIM_ATTACK,128,1,me->dx,me->dy);
		return;
	}
	if(Random(30)==0)
	{
		// fire bubbles
		MakeSound(SND_ELDERSHOOT,me->x,me->y,SND_CUTOFF,1200);
		DoMove(me,ANIM_A2,128,1,me->dx,me->dy);
		return;
	}

	FaceGoodguy2(me,goodguy);
	me->dx+=Cosine(me->facing*32)*3/2;
	me->dy+=Sine(me->facing*32)*3/2;
	Clamp(&me->dx,FIXAMT*6);
	Clamp(&me->dy,FIXAMT*6);
}

void AI_Bubble(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->mind1&1)
		me->dx=-me->dx;
	if(me->mind1&2)
		me->dy=-me->dy;

	me->mind3++;
	if(me->mind3==30)
	{
		me->mind3=0;
		me->mind++;
	}

	if(me->seq==ANIM_DIE)
	{
		ExplodeParticles(PART_WATER,me->x,me->y,me->z,4);
		return;
	}
	if(me->mind==15)	// last 15 seconds
	{
		MakeSound(SND_BUBBLEPOP,me->x,me->y,SND_CUTOFF,1200);
		me->hp=0;
		DoMove(me,ANIM_DIE,128,1,0,0);
		me->dx=0;
		me->dy=0;
	}

	me->mind1=0;

	if(RangeToTarget(me,goodguy)<40*FIXAMT)
	{
		goodguy->dx+=me->dx*2;
		goodguy->dy+=me->dy*2;
		FaceGoodguy(me,goodguy);

		me->dx=-Cosine(me->facing*32)*4;
		me->dy=-Sine(me->facing*32)*4;
	}
	if(me->z==0)
		me->dz=FIXAMT*8;
}

void AI_FireElder(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	SetBossHP(me->hp,MonsterHP(me->type));

	BasicAI(me,SND_ELDEROUCH,SND_ELDERDIE,map,world,goodguy);

	map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,(me->y/TILE_HEIGHT)>>FIXSHIFT,16,8);

	if(player.var[240]==0)
	{
		if(RangeToTarget(me,goodguy)<256*FIXAMT || me->ouch)
		{
			player.var[240]=1;
			BeginChatting(239);
		}
		return;
	}

	if(me->ouch && me->action==ACTION_BUSY && me->seq!=ANIM_DIE && me->seq!=ANIM_ATTACK && Random(10)==0)
	{
		// run away from the water
		DoMove(me,ANIM_ATTACK,128,1,0,0);
		me->facing=(byte)Random(8);
		return;
	}
	if(me->mind3)
		me->mind3--;
	else
		me->mind3=30*8;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm<6)
		{
			me->dx=Cosine(me->facing*32)*(6+me->frm*2);
			me->dy=Sine(me->facing*32)*(6+me->frm*2);
		}
		else if(me->seq==ANIM_ATTACK)
		{
			me->dx/=2;
			me->dy/=2;
		}
		if(me->seq==ANIM_ATTACK && me->frm>=3 && me->frm<=6)
		{
			x=me->x+Cosine(me->facing*32)*30;
			y=me->y+Sine(me->facing*32)*30;
			if(me->AttackCheck(32,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				goodguy->GetShot(Cosine(me->facing*32)*16,Sine(me->facing*32)*16,monsType[me->type].damage,map,world);
			me->reload=5;
		}
		if(me->seq==ANIM_A1 && me->frm==1)
			me->mind2=me->facing*32;
		if(me->seq==ANIM_A1 && me->frm>0 && me->frm<22)
		{
			me->mind2+=8;
			me->facing=me->mind2/32;
			x=me->x+20*Cosine(me->mind2);
			y=me->y+20*Sine(me->mind2);
			FireBullet(x,y,me->mind2,BLT_FLAMESHOT);
		}

		if(me->seq==ANIM_A2 && me->frm>4 && me->frm<9 && me->frmTimer<128)
		{
			x=me->x+Cosine(me->facing*32)*(me->frm-4)*48;
			y=me->y+Sine(me->facing*32)*(me->frm-4)*32;
			if(me->frm==8 && me->reload==0)
			{
				me->reload=3;
				FireBullet(x,y,0,BLT_MEGABOOM);
			}
			else if(me->frmTimer<128)
				FireBullet(x,y,0,BLT_BADBOOM);
		}

		if(me->seq==ANIM_DIE)
		{
			x=me->x-40*FIXAMT+Random(80*FIXAMT);
			y=me->y-30*FIXAMT+Random(60*FIXAMT);
			FireBullet(x,y,0,BLT_GOODBOOM+(byte)Random(2));
			MakeSound(SND_ELDERDIE,me->x,me->y-200*me->frm,SND_CUTOFF,2000);
		}
		return;	// can't do nothin' right now
	}

	x=me->x-40*FIXAMT+Random(80*FIXAMT);
	y=me->y-30*FIXAMT+Random(60*FIXAMT);
	FireBullet(x,y,0,BLT_FLAMEWALL);

	if(me->mind3==0 || me->mind3==30*4)
	{
		// unleash flame circle
		MakeSound(SND_ELDERSUMMON,me->x,me->y,SND_CUTOFF,1200);
		DoMove(me,ANIM_A1,128,1,0,0);
		return;
	}

	/*
	if(RangeToTarget(me,goodguy)<(300*FIXAMT) && Random(20)==0)
	{
		// charge at him
		MakeSound(SND_ELDERCHARGE,me->x,me->y,SND_CUTOFF,1200);
		DoMove(me,ANIM_ATTACK,128,1,0,0);
		return;
	}
	*/

	if(me->mind3==30*2 || me->mind3==30*6)
	{
		// fire explosion stream
		MakeSound(SND_ELDERSHOOT,me->x,me->y,SND_CUTOFF,1200);
		DoMove(me,ANIM_A2,128,1,0,0);
		return;
	}

	WanderAI(me,1024,2,1,map,world,goodguy);
}

void AI_SummonElder(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,a,b;
	Guy *g;

	if(RangeToTarget(me,goodguy)<400*FIXAMT)
		SetBossHP(me->hp,MonsterHP(me->type));

	BasicAI(me,SND_ELDEROUCH,SND_ELDERDIE,map,world,goodguy);

	if(me->seq==ANIM_DIE && Random(5)==0)
	{
		y=me->y-FIXAMT*10+Random(FIXAMT*11);
		x=me->x-32*FIXAMT+Random(65*FIXAMT);
		FireBullet(x,y,0,BLT_EVILFACE);
	}

	map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,(me->y/TILE_HEIGHT)>>FIXSHIFT,8,8);

	if(me->mind3)
		me->mind3--;
	else
		me->mind3=30*4;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_A1 && me->frm>0 && me->frm<22 && me->reload==0)
		{
			a=Random(256);
			b=Random(60)+20;
			x=me->x+b*Cosine(a);
			y=me->y+b*Sine(a);

			g=AddGuy(x,y,FIXAMT*40,MONS_VAMPIRE+me->mind2);
			if(g && !g->CanWalk(g->x,g->y,map,world))
				g->type=MONS_NONE;
			else
			{
				MakeRingParticle(x,y,FIXAMT*40,20,40);
				FireBullet(x,y,0,BLT_BADBOOM);
			}
			me->reload=(me->mind2+1)*4;
		}

		if(me->seq==ANIM_DIE)
		{
			x=me->x-40*FIXAMT+Random(80*FIXAMT);
			y=me->y-30*FIXAMT+Random(60*FIXAMT);
			FireBullet(x,y,0,BLT_GOODBOOM+(byte)Random(2));
			MakeSound(SND_ELDERDIE,me->x,me->y-200*me->frm,SND_CUTOFF,2000);
		}
		return;	// can't do nothin' right now
	}

	if(me->mind3==0 || me->mind3==30*4)
	{
		// summon minions
		MakeSound(SND_ELDERSUMMON,me->x,me->y,SND_CUTOFF,1200);
		DoMove(me,ANIM_A1,128,1,0,0);
		me->mind2=(byte)Random(4);
		return;
	}

	WanderAI(me,1024,2,1,map,world,goodguy);
}

void AI_Goalie(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,z;

	GetBallCoords(&x,&y,&z);

	BasicAI(me,SND_ZOMBIEOUCH,SND_ZOMBIEDIE,map,world,goodguy);

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==5 && me->reload==0)
		{
			ThrowBall();
			me->reload=5;
		}
		return;	// can't do nothin' right now
	}


	if(abs(me->x-x)<10*FIXAMT)
	{
		Dampen(&me->dx,FIXAMT);
		if(me->seq!=ANIM_IDLE)
			DoMove(me,ANIM_IDLE,128,0,me->dx,me->dy);
	}
	else
	{
		if(me->x>x)
			me->dx-=FIXAMT*2;
		else
			me->dx+=FIXAMT*2;
		Clamp(&me->dx,monsType[me->type].speed);
		if(me->seq!=ANIM_MOVE)
			DoMove(me,ANIM_MOVE,256,0,me->dx,me->dy);
	}

	if(RangeToCoords(me,x,y)<100*FIXAMT)
	{
		// close to the ball!
		if(z>20*FIXAMT && me->z==0)
			me->dz=FIXAMT*10;	// jump for it!

		if(RangeToCoords(me,x,y)<25*FIXAMT && abs(me->z-z)<FIXAMT*30 && abs(me->x-x)<FIXAMT*15)
		{
			// caught it!
			DoMove(me,ANIM_ATTACK,128,1,me->dx,me->dy);
			MakeSound(SND_MUMMYSMACK,me->x,me->y,SND_CUTOFF,1200);
			CatchBall(me->ID);
		}
	}
}

void AI_BowlingPin(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	bullet_t *b;

	if(me->hp>0)
		me->hp=1;

	BasicAI(me,SND_PUMPKINOUCH,SND_PUMPKINDIE,map,world,goodguy);

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			me->frmAdvance=256;
			Dampen(&me->dx,FIXAMT/4);
			Dampen(&me->dy,FIXAMT/4);
			if(me->frm==6 && me->reload==0)
			{
				// explode in hopes of killing other 'pins'
				b=FireBullet(me->x,me->y,0,BLT_BOWLBOOM);
				if(b)
				{
					b->dx=me->dx;
					b->dy=me->dy;
				}
				me->reload=10;
			}
		}
		return;
	}
}

void AI_Bonkula(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	Guy *g;

	SetBossHP(me->hp,MonsterHP(me->type));

	if(me->reload)
		me->reload--;

	if(me->ouch==3)
	{
		if(me->hp>0)
			MakeSound(SND_BONKOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_BONKDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->ouch)
	{
		x=me->x-40*FIXAMT+Random(80*FIXAMT);
		y=me->y-30*FIXAMT+Random(60*FIXAMT);
		FireBullet(x,y,0,BLT_FLAMEWALL);
	}

	// handle introductions
	if(player.var[240]==0)
	{
		if(me->y>goodguy->y-FIXAMT*(TILE_HEIGHT*3+TILE_HEIGHT/2))
		{
			player.var[240]=1;
			me->dy=0;
		}
		else
			me->dy=FIXAMT*16;
		return;
	}
	else if(player.var[240]<10)
	{
		player.var[240]++;
		if(player.var[240]==10)
			BeginChatting(238);
		return;
	}

	if(player.var[245]==1)
	{
		if(map->map[me->mapx+me->mapy*map->width].tag==10)
		{
			me->mind3=30;
			FaceGoodguy(me,goodguy);
			me->dx=Cosine(me->facing*32)*8;
			me->dy=Sine(me->facing*32)*8;
			me->dx+=-FIXAMT*2+Random(4*FIXAMT);
			me->dy+=-FIXAMT*2+Random(4*FIXAMT);
			monsType[me->type].flags&=(~MF_INVINCIBLE);
			me->GetShot(0,0,2,map,world);
			monsType[me->type].flags|=MF_INVINCIBLE;

			if(me->reload==0)
			{
				x=me->x-300*FIXAMT+Random(600)*FIXAMT;
				y=me->y+Random(900)*FIXAMT;
				g=AddGuy(x,y,FIXAMT*50,MONS_VAMPIRE2+(byte)Random(3));
				if(g && !g->CanWalk(g->x,g->y,map,world))
				{
					g->type=MONS_NONE;
				}
				me->reload=2;
			}
		}
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==8 && me->reload==0)
		{
			MakeSound(SND_BONKHIT,me->x,me->y,SND_CUTOFF,1200);
			x=me->x+Cosine(me->facing*32)*60;
			y=me->y+Sine(me->facing*32)*60;

			FindGoodVictim(x>>FIXSHIFT,y>>FIXSHIFT,6*15,Cosine(me->facing*32)*2,
							   Sine(me->facing*32)*2,monsType[me->type].damage,map,world);
			MakeRingParticle(x,y,0,6*15,10*15);
			me->reload=5;
		}

		if(me->seq==ANIM_DIE)
		{
			me->dx=0;
			me->dy=0;
			x=me->x-40*FIXAMT+Random(80*FIXAMT);
			y=me->y-30*FIXAMT+Random(60*FIXAMT);
			FireBullet(x,y,0,BLT_FLAMEWALL);
			MakeSound(SND_BONKDIE,me->x,me->y-200*me->frm,SND_CUTOFF,2000);
		}
		if(me->seq==ANIM_A1)
		{
			me->facing++;
			if(me->facing>7)
				me->facing=0;
			if(Random(30)==0)
			{
				x=me->x-300*FIXAMT+Random(600)*FIXAMT;
				y=me->y-300*FIXAMT+Random(600)*FIXAMT;
				g=AddGuy(x,y,FIXAMT*50,MONS_VAMPIRE+(byte)Random(4));
				if(g && !g->CanWalk(g->x,g->y,map,world))
				{
					g->type=MONS_NONE;
				}
			}
		}
		return;	// can't do nothin' right now
	}

	if(me->mind3==0)
	{
		// begin spinning
		DoMove(me,ANIM_A1,256,1,0,0);
		me->mind3=60;
	}
	else
	{
		if(me->mind3==60)
		{
			FaceGoodguy(me,goodguy);
			me->dx=Cosine(me->facing*32)*8;
			me->dy=Sine(me->facing*32)*8;
			me->dx+=-FIXAMT*2+Random(4*FIXAMT);
			me->dy+=-FIXAMT*2+Random(4*FIXAMT);
		}
		me->mind3--;
		if(RangeToTarget(me,goodguy)<200*FIXAMT)
		{
			MakeSound(SND_BONKSWING,me->x,me->y,SND_CUTOFF,2000);
			DoMove(me,ANIM_ATTACK,256,1,0,0);
			me->mind3=0;
		}
	}
}

void AI_Evilizer(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	byte summonTab[]={MONS_BONEHEAD4,MONS_ZOMBIE3,MONS_MUMMY4,MONS_WOLFMAN3,MONS_SWAMPDOG3,
		MONS_GHOST3,MONS_VAMPIRE4};

	int x,y;
	Guy *g;

	SetBossHP(me->hp,MonsterHP(me->type));

	if(me->hp && RangeToTarget(me,goodguy)<780*FIXAMT)
		SetNoSaving(true);
	if(me->hp==0)
		SetNoSaving(false);

	if(me->ouch==4)
		MakeSound(SND_METALSMACK,me->x,me->y,SND_CUTOFF,1200);

	if(!CheckForPipes(map))
	{
		monsType[me->type].flags&=(~MF_INVINCIBLE);
		me->mind=1;
		if(opt.cheats[CH_DEATH]==0 && me->hp>2000-8*50)
			me->hp=2000-8*50;
	}

	if(player.var[240]==8 && me->mind==0)
	{
		monsType[me->type].flags&=(~MF_INVINCIBLE);
		me->mind=1;
		if(opt.cheats[CH_DEATH]==0 && me->hp>2000-player.var[240]*50)
			me->hp=2000-player.var[240]*50;
	}
	else if(me->mind==0)
	{
		if(opt.cheats[CH_DEATH]==0 && me->hp>2000-player.var[240]*50)
			me->hp=2000-player.var[240]*50;
	}

	// mind=whether uncovered or not
	if(me->mind==0)
		me->facing=0;
	else
		me->facing=16;

	// mind1=time to shoot an evilface off
	if(me->mind1==0)
	{
		y=me->y-FIXAMT;
		x=me->x-140*FIXAMT+Random(281*FIXAMT);
		FireBullet(x,y,0,BLT_EVILFACE);
		me->mind1=12;
		MakeSound(SND_EVILIZER,me->x,me->y,SND_CUTOFF,300);
	}
	else
	{
		me->mind1--;
		if(!(me->mind1&3))
		{
			y=me->y-FIXAMT;
			x=me->x-140*FIXAMT+Random(281*FIXAMT);
			FireBullet(x,y,0,BLT_EVILFACE);
		}
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			y=me->y+140*FIXAMT-Random(281*FIXAMT);
			x=me->x-140*FIXAMT+Random(281*FIXAMT);
			FireBullet(x,y,0,BLT_GOODBOOM2);
		}
	}

	// mind2=grenade timer (faster as it gets hurt more)
	if(me->mind2==0)
		me->mind2=30+me->hp/10;
	else
	{
		me->mind2--;
		if(me->mind2==0)
		{
			x=180*FIXAMT;
			y=180*FIXAMT;
			FireBullet(me->x-x,me->y,64+(byte)Random(128),BLT_ORBGRENADE);
			FireBullet(me->x+x,me->y,192+(byte)Random(128),BLT_ORBGRENADE);
			FireBullet(me->x,me->y-y,128+(byte)Random(128),BLT_ORBGRENADE);
			FireBullet(me->x,me->y+y,(byte)Random(128),BLT_ORBGRENADE);
			MakeSound(SND_GRENADE,me->x,me->y,SND_CUTOFF,1200);
		}
	}

	// mind3=missile timer (only works once exposed)
	if(me->mind==1)
	{
		if(me->mind3==0)
			me->mind3=60+me->hp/20;
		else
		{
			me->mind3--;
			if(me->mind3==0)
			{
				x=100*FIXAMT;
				y=40*FIXAMT;
				FireBullet(me->x-x,me->y-y,160,BLT_MISSILE);
				FireBullet(me->x+x,me->y-y,224,BLT_MISSILE);
				FireBullet(me->x-x,me->y+y,96,BLT_MISSILE);
				FireBullet(me->x+x,me->y+y,32,BLT_MISSILE);
				MakeSound(SND_FRANKMISSILE,me->x,me->y,SND_CUTOFF,1200);
			}
		}

		// reload=summon super duper monsters, only when exposed
		if(me->reload==0)
			me->reload=30*2;
		else
		{
			me->reload--;
			if(me->reload==0)
			{
				x=((Random(map->width-6)+3)*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
				y=((Random(map->height-20)+10)*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
				g=AddGuy(x,y,FIXAMT*40,summonTab[Random(7)]);
				if(g && !g->CanWalk(g->x,g->y,map,world))
					g->type=MONS_NONE;
			}

		}
	}
}

void AI_EvilPump(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	FaceGoodguy3(me,goodguy);

	if(me->ouch==4)
		MakeSound(SND_METALSMACK,me->x,me->y,SND_CUTOFF,1200);

	if(me->seq==ANIM_IDLE && me->frm==8 && me->frmTimer<128)
	{
		map->GasSpray(me->mapx,me->mapy);
		me->reload=3;
	}
	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && (me->frm==3 || me->frm==5 || me->frm==7) && me->reload==0)
		{
			MakeSound(SND_MUMMYSHOOT,me->x,me->y,SND_CUTOFF,1200);
			x=me->x+Cosine(me->facing*32)*48;
			y=me->y+Sine(me->facing*32)*48;
			FireBullet(x,y,(byte)(me->facing*32+240+Random(33)),BLT_BIGSHOT);
			me->reload=3;
			if(me->frm==7)
				me->reload=30*3;
		}
		if(me->seq==ANIM_DIE)
		{
			if(me->frm==0)
				me->reload=0;
			x=me->x-20*FIXAMT+Random(40*FIXAMT);
			y=me->y-20*FIXAMT+Random(40*FIXAMT);
			FireBullet(x,y,0,BLT_BADBOOM);
			if(me->frm==4 && me->reload==0)
			{
				me->reload=10;
				map->GasSpray2(me->mapx,me->mapy);
			}
		}
		return;
	}

	if(me->reload==0)
	{
		DoMove(me,ANIM_ATTACK,128,1,0,0);
	}
}

void AI_Larry(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;
	Guy *g;

	if(RangeToTarget(me,goodguy)<400*FIXAMT)
		SetBossHP(me->hp,MonsterHP(me->type));

	//Burn((me->mind2*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(me->mind3*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,0);

	BasicAI(me,SND_WOLFOUCH,SND_WOLFDIE,map,world,goodguy);

	if(me->mind)
		me->mind--;

	if(me->mind<30)
	{
		Burn(me->x,me->y,Random(50*FIXAMT));
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==4)
		{
			me->dx=Cosine(me->facing*32)*10;
			me->dy=Sine(me->facing*32)*10;
		}
		if(me->seq==ANIM_ATTACK && me->frm>=6 && me->frm<=10)
		{
			Dampen(&me->dx,FIXAMT);
			Dampen(&me->dy,FIXAMT);
			if(me->AttackCheck(40,me->x>>FIXSHIFT,me->y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(Cosine(me->facing*32)*8,Sine(me->facing*32)*8,monsType[me->type].damage,map,world);
			}
			if(me->frm==8 && me->frmTimer<128)
			{
				x=me->x+Cosine(me->facing*32)*48;
				y=me->y+Sine(me->facing*32)*48;
				MakeRingParticle(x,y,0,128,64);
				if(me->AttackCheck(128,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				{
					goodguy->GetShot(Cosine(me->facing*32)*8,Sine(me->facing*32)*8,monsType[me->type].damage,map,world);
				}
				ShakeScreen(15);
			}
			if(me->frm==10)
			{
				me->dx=0;
				me->dy=0;
			}
		}
		if(me->seq==ANIM_A1 && (me->frm==6 || me->frm==7) && me->frmTimer<128)
		{
			x=me->x+Cosine((me->facing+7+2*(7-me->frm))*32)*20;
			y=me->y+Sine((me->facing+7+2*(7-me->frm))*32)*20;
			MakeSound(SND_LARRYDARTS,me->x,me->y,SND_CUTOFF,1200);
			for(i=0;i<10;i++)
			{
				FireBullet(x,y,(me->facing*32+(me->frm-6)*8+256-48+i*(96/10))&255,BLT_CLAW);
			}
			me->reload=5;
		}
		if(me->seq==ANIM_A2 && me->frm>=5 && me->frm<=15)
		{
			x=me->x+Cosine(me->facing*32)*32;
			y=me->y+Sine(me->facing*32)*32;
			if(me->AttackCheck(16,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(Cosine(me->facing*32)*4,Sine(me->facing*32)*4,monsType[me->type].damage,map,world);
			}
		}
		if(me->seq==ANIM_DIE)
		{
			if(player.worldNum==WORLD_NORMAL || player.worldNum==WORLD_REMIX|| player.worldNum==WORLD_RANDOMIZER)
			{
				g=AddGuy(me->x,me->y,me->z,MONS_VILLAGER2);
				BadgeCheck(BE_KILL,me->type,map);
				PlayerSetVar(VAR_QUESTDONE+QUEST_WOLF,1);
				me->type=MONS_NONE;
				FireBullet(me->x,me->y,0,BLT_GOODBOOM);
				if(g)
					g->tag=16;
			}
		}
		return;	// can't do nothin' right now
	}

	if(map->CheckLOS(me->mapx,me->mapy,15,goodguy->mapx,goodguy->mapy))
	{
		// if goodguy is in sight
		if(RangeToTarget(me,goodguy)<(256*FIXAMT) && Random(12)==0)
		{
			// get him!
			MakeSound(SND_LARRYPOUNCE,me->x,me->y,SND_CUTOFF,1200);
			DoMove(me,ANIM_ATTACK,128,1,0,0);
			FaceGoodguy2(me,goodguy);
			return;
		}
		if(Random(32)==0)	// fire at any range
		{
			// shoot darts
			MakeSound(SND_LARRYPOUNCE,me->x,me->y,SND_CUTOFF,1200);
			DoMove(me,ANIM_A1,200,1,0,0);
			FaceGoodguy2(me,goodguy);
			return;
		}
		if(me->mapx==me->mind2 && me->mapy==me->mind3)
		{
		//	me->mind2=goodguy->mapx;
		//	me->mind3=goodguy->mapy;
			SelectDestination(me,goodguy,map,0);
		}
	}
	else
	{
		if(me->mapx==me->mind2 && me->mapy==me->mind3)
			SelectDestination(me,goodguy,map,0);
	}

	/*
	if(me->mind1 && (abs(me->dx)>FIXAMT*3 || abs(me->dy)>FIXAMT*3))
	{
		me->mind1=0;
		me->dx=0;
		me->dy=0;
		SelectDestination(me,goodguy,map,1);
	}
	*/
	if(me->mind==0)
	{
		me->type=MONS_HUMANLARRY;
		DoMove(me,ANIM_IDLE,128,0,0,0);
		MakeSound(SND_LARRYNORMAL,me->x,me->y,SND_CUTOFF,1800);
		SelectDestination(me,goodguy,map,1);
		me->mind=30*8;
		return;
	}

	if(me->mapx>me->mind2)
		me->dx-=FIXAMT;
	else if(me->mapx<me->mind2)
		me->dx+=FIXAMT;
	if(me->mapy>me->mind3)
		me->dy-=FIXAMT;
	else if(me->mapy<me->mind3)
		me->dy+=FIXAMT;
	Dampen(&me->dx,FIXAMT/4);
	Dampen(&me->dy,FIXAMT/4);
	Clamp(&me->dx,monsType[me->type].speed);
	Clamp(&me->dy,monsType[me->type].speed);

	if(me->dx<-FIXAMT)
	{
		if(me->dy<-FIXAMT)
			me->facing=5;
		else if(me->dy>FIXAMT)
			me->facing=3;
		else
			me->facing=4;
	}
	else if(me->dx>FIXAMT)
	{
		if(me->dy<-FIXAMT)
			me->facing=7;
		else if(me->dy>FIXAMT)
			me->facing=1;
		else
			me->facing=0;
	}
	else
	{
		if(me->dy<-FIXAMT)
			me->facing=6;
		else
			me->facing=2;
	}

	if(me->seq!=ANIM_MOVE)
		DoMove(me,ANIM_MOVE,180,0,me->dx,me->dy);
}

void AI_HumanLarry(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;
	Guy *g;

	if(me->mind==30*8+2)
	{
		BeginChatting(235);
		SelectDestination(me,goodguy,map,1);
	}

	//Burn((me->mind2*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(me->mind3*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,0);

	if(RangeToTarget(me,goodguy)<400*FIXAMT)
		SetBossHP(me->hp,MonsterHP(me->type));

	if(me->ouch==4)
	{
		if(me->mind>15)
			me->mind-=15;
		else
			me->mind=0;
	}
	if(me->mind)
		me->mind--;

	if(me->mind<30)
	{
		Burn(me->x,me->y,Random(50*FIXAMT));
	}

	BasicAI(me,SND_LARRYOUCH,SND_VILLDIE1,map,world,goodguy);

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==4)
		{
			me->dx=Cosine(me->facing*32)*10;
			me->dy=Sine(me->facing*32)*10;
		}
		if(me->seq==ANIM_ATTACK && me->frm>=6 && me->frm<=10)
		{
			Dampen(&me->dx,FIXAMT);
			Dampen(&me->dy,FIXAMT);
			if(me->AttackCheck(40,me->x>>FIXSHIFT,me->y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(Cosine(me->facing*32)*8,Sine(me->facing*32)*8,monsType[me->type].damage,map,world);
			}
			if(me->frm==10)
			{
				me->dx=0;
				me->dy=0;
			}
		}
		if(me->seq==ANIM_A1 && (me->frm==6 || me->frm==7) && me->frmTimer<128)
		{
			x=me->x+Cosine((me->facing+7)*32)*20;
			y=me->y+Sine((me->facing+7)*32)*20;
			for(i=0;i<3;i++)
			{
				FireBullet(x,y,(me->facing*32+(me->frm-6)*8+256-32+i*(32/6))&255,BLT_CLAW);
			}
			x=me->x+Cosine((me->facing+1)*32)*20;
			y=me->y+Sine((me->facing+1)*32)*20;
			for(i=0;i<3;i++)
			{
				FireBullet(x,y,(me->facing*32-(me->frm-6)*8+256-16+i*(32/6))&255,BLT_CLAW);
			}
			me->reload=5;
		}
		if(me->seq==ANIM_A2 && me->frm>=5 && me->frm<=15)
		{
			x=me->x+Cosine(me->facing*32)*32;
			y=me->y+Sine(me->facing*32)*32;
			if(me->AttackCheck(16,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(Cosine(me->facing*32)*4,Sine(me->facing*32)*4,monsType[me->type].damage,map,world);
			}
		}
		if(me->seq==ANIM_DIE)
		{
			if(player.worldNum==WORLD_NORMAL || player.worldNum==WORLD_REMIX|| player.worldNum==WORLD_RANDOMIZER)
			{
				g=AddGuy(me->x,me->y,me->z,MONS_VILLAGER3);
				BadgeCheck(BE_KILL,me->type,map);
				PlayerSetVar(VAR_QUESTDONE+QUEST_WOLF,1);
				me->type=MONS_NONE;
				if(g)
					g->tag=16;
				BeginChatting(234);
			}
		}
		return;	// can't do nothin' right now
	}

	if(me->mapx==me->mind2 && me->mapy==me->mind3)
		SelectDestination(me,goodguy,map,0);

	/*
	if(me->mind1)
	{
		me->mind1=0;
		me->dx=0;
		me->dy=0;
		SelectDestination(me,goodguy,map,1);
	}
	*/

	if(me->mind==0)
	{
		me->type=MONS_LARRY;
		DoMove(me,ANIM_A2,128,1,0,0);
		MakeSound(SND_LARRYHOWL,me->x,me->y,SND_CUTOFF,1800);
		SelectDestination(me,goodguy,map,1);
		me->mind=30*8;
		return;
	}

	if(me->mapx>me->mind2)
		me->dx-=FIXAMT;
	else if(me->mapx<me->mind2)
		me->dx+=FIXAMT;
	if(me->mapy>me->mind3)
		me->dy-=FIXAMT;
	else if(me->mapy<me->mind3)
		me->dy+=FIXAMT;
	Dampen(&me->dx,FIXAMT/4);
	Dampen(&me->dy,FIXAMT/4);
	Clamp(&me->dx,monsType[me->type].speed);
	Clamp(&me->dy,monsType[me->type].speed);

	if(me->dx<-FIXAMT)
	{
		if(me->dy<-FIXAMT)
			me->facing=5;
		else if(me->dy>FIXAMT)
			me->facing=3;
		else
			me->facing=4;
	}
	else if(me->dx>FIXAMT)
	{
		if(me->dy<-FIXAMT)
			me->facing=7;
		else if(me->dy>FIXAMT)
			me->facing=1;
		else
			me->facing=0;
	}
	else
	{
		if(me->dy<-FIXAMT)
			me->facing=6;
		else
			me->facing=2;
	}

	if(me->seq!=ANIM_MOVE)
		DoMove(me,ANIM_MOVE,256,0,me->dx,me->dy);
}

void AI_Harry(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i,a,b;
	Guy *g;

	BasicAI(me,SND_WOLFOUCH,SND_WOLFDIE,map,world,goodguy);

	if(me->mind3==0)
		me->mind3=30*4;
	if(me->mind3>1)
		me->mind3--;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==4)
		{
			me->dx=Cosine(me->facing*32)*10;
			me->dy=Sine(me->facing*32)*10;
		}
		if(me->seq==ANIM_ATTACK && me->frm>=6 && me->frm<=10)
		{
			Dampen(&me->dx,FIXAMT);
			Dampen(&me->dy,FIXAMT);
			if(me->AttackCheck(40,me->x>>FIXSHIFT,me->y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(Cosine(me->facing*32)*8,Sine(me->facing*32)*8,monsType[me->type].damage,map,world);
			}
			if(me->frm==8 && me->frmTimer<128)
			{
				x=me->x+Cosine(me->facing*32)*48;
				y=me->y+Sine(me->facing*32)*48;
				MakeRingParticle(x,y,0,128,64);
				if(me->AttackCheck(128,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				{
					goodguy->GetShot(Cosine(me->facing*32)*8,Sine(me->facing*32)*8,monsType[me->type].damage,map,world);
				}
				ShakeScreen(15);
			}
			if(me->frm==10)
			{
				me->dx=0;
				me->dy=0;
			}
		}
		if(me->seq==ANIM_A1 && me->frm==6 && me->frmTimer<128)
		{
			x=me->x+Cosine((me->facing+7+2*(7-me->frm))*32)*20;
			y=me->y+Sine((me->facing+7+2*(7-me->frm))*32)*20;
			MakeSound(SND_LARRYDARTS,me->x,me->y,SND_CUTOFF,1200);
			for(i=0;i<10;i++)
			{
				FireBullet(x,y,(me->facing*32+(me->frm-6)*4+256-24+i*(48/10))&255,BLT_CLAW);
			}
			me->reload=5;
		}

		if(me->seq==ANIM_A2 && me->frm>=5 && me->frm<=15 && me->reload==0)
		{
			a=Random(256);
			b=Random(60)+20;
			x=me->x+b*Cosine(a);
			y=me->y+b*Sine(a);

			g=AddGuy(x,y,FIXAMT*40,MONS_WOLFMAN3);
			if(g && !g->CanWalk(g->x,g->y,map,world))
				g->type=MONS_NONE;
			else
			{
				MakeRingParticle(x,y,FIXAMT*40,20,40);
				FireBullet(x,y,0,BLT_BADBOOM);
			}
			me->reload=3;
		}
		if(me->seq==ANIM_DIE)
		{
			x=me->x-20*FIXAMT+Random(40*FIXAMT);
			y=me->y-20*FIXAMT+Random(40*FIXAMT);
			FireBullet(x,y,0,BLT_BADBOOM);
			if(Random(2)==0)
			{
				y=me->y-FIXAMT*10+Random(FIXAMT*11);
				x=me->x-32*FIXAMT+Random(65*FIXAMT);
				FireBullet(x,y,0,BLT_EVILFACE);
			}
		}
		return;	// can't do nothin' right now
	}

	if(RangeToTarget(me,goodguy)<64*FIXAMT && Random(4)==0 &&
		map->CheckLOS(me->mapx,me->mapy,5,goodguy->mapx,goodguy->mapy))
	{
		// get him!
		MakeSound(SND_LARRYPOUNCE,me->x,me->y,SND_CUTOFF,1200);
		DoMove(me,ANIM_ATTACK,128,1,0,0);
		FaceGoodguy2(me,goodguy);
		return;
	}
	if(Random(32)==0)	// fire at any range
	{
		// shoot darts
		MakeSound(SND_LARRYPOUNCE,me->x,me->y,SND_CUTOFF,1200);
		DoMove(me,ANIM_A1,200,1,0,0);
		FaceGoodguy2(me,goodguy);
		return;
	}

	if(me->mind3==1)
	{
		me->mind3=0;
		MakeSound(SND_WOLFHOWL,me->x,me->y,SND_CUTOFF,1000);
		DoMove(me,ANIM_A2,128,1,0,0);
		return;
	}

	WanderAI(me,256,20,3,map,world,goodguy);
}

void AI_Ghastly(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int i;
	int x,y;
	Guy *g;

	SetBossHP(me->hp,MonsterHP(me->type));

	BasicAI(me,SND_POLTEROUCH,SND_POLTERDIE,map,world,goodguy);

	if(me->mind==0 && RangeToTarget(me,goodguy)<256*FIXAMT)
	{
		me->mind=1;
		me->reload=60;
		me->mind3=120+15;
		me->facing=(byte)Random(256);
		me->dx=Cosine(me->facing)*2;
		me->dy=Sine(me->facing)*2;
	}

	if(me->mind1)
	{
		if(me->mind1&1)
			me->dx=-me->dx;
		if(me->mind1&2)
			me->dy=-me->dy;

		if(Random(20)==0)
		{
			me->facing=(byte)Random(256);
			me->dx=Cosine(me->facing)*2;
			me->dy=Sine(me->facing)*2;
		}
		me->mind1=0;
	}

	if(Random(4)==0)
		FireBullet(me->x,me->y,(byte)Random(256),BLT_GASBLAST);

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==4 && me->reload==0)
		{
			me->reload=30*4;

			for(i=0;i<3;i++)
			{
				x=me->x-30*FIXAMT+Random(61)*FIXAMT;
				y=me->y-20*FIXAMT+Random(51)*FIXAMT;
				g=AddGuy(x,y,FIXAMT*30,MONS_BAT4);
				if(g && !g->CanWalk(g->x,g->y,map,world))
				{
					g->type=MONS_NONE;
				}
			}
		}
		if(me->seq==ANIM_A1 && me->frm==15 && me->frmTimer<128)
		{
			for(i=0;i<16;i++)
				FireBullet(me->x,me->y,i*16,BLT_GASBLAST);
		}
		return;	// can't do nothin' right now
	}

	if(me->reload==0)
	{
		MakeSound(SND_POLTERGET,me->x,me->y,SND_CUTOFF,1200);
		DoMove(me,ANIM_ATTACK,128,1,me->dx,me->dy);
	}
	if(Random(50)==0)
	{
		me->facing=(byte)Random(256);
		me->dx=Cosine(me->facing)*2;
		me->dy=Sine(me->facing)*2;
	}
}

void AI_MiniFrankenjulie(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	SetBossHP(me->hp,MonsterHP(me->type));

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
		{
			MakeSound(SND_FRANKOUCH2,me->x,me->y,SND_CUTOFF,1200);

			if(me->hp<monsType[me->type].hp/2 && me->mind==0)
			{
				me->mind=1;
				FireBullet(me->x,me->y,0,BLT_BADBOOM);
				DoMove(me,ANIM_A1,256,0,0,0);
			}
			if(me->hp<monsType[me->type].hp/4 && me->mind==1)
			{
				me->mind=2;
				FireBullet(me->x,me->y,0,BLT_BADBOOM);
				DoMove(me,ANIM_A1,256,0,0,0);
			}
		}
		else
		{
			MakeSound(SND_FRANKDIE,me->x,me->y,SND_CUTOFF,1200);
		}
	}

	if(me->mind2==0)
	{
		if(RangeToTarget(me,goodguy)<256*FIXAMT || me->ouch)
		{
			me->mind2=1;
			BeginChatting(233);
		}
		return;
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			FireBullet(me->x-60*FIXAMT+Random(120*FIXAMT),
						   me->y-60*FIXAMT+Random(120*FIXAMT),0,BLT_BADBOOM);
		}
		if(me->seq==ANIM_ATTACK)
		{
			if(me->frmTimer<128)
				MakeSound(SND_FRANKHEAD,me->x,me->y,SND_CUTOFF,400);
			if(me->frm<16 && me->frmTimer<128)
			{
				x=(me->x+Cosine(me->frm*16)*40);
				y=(me->y+Sine(me->frm*16)*30);
				FireExactBullet(x,y,FIXAMT*40,Cosine(me->frm*16)*12,Sine(me->frm*16)*12,
					-FIXAMT*2,0,60,me->frm*16,BLT_ENERGY);
				x=(me->x+Cosine((me->frm+8)*16)*40);
				y=(me->y+Sine((me->frm+8)*16)*30);
				FireExactBullet(x,y,FIXAMT*40,Cosine((me->frm+8)*16)*12,Sine((me->frm+8)*16)*12,
					-FIXAMT*2,0,60,(me->frm+8)*16,BLT_ENERGY);
			}
		}
		if(me->seq==ANIM_A3)
		{
			if(me->frm>4 && me->frm<20)
			{
				me->dx+=Cosine(me->facing*32)*3;
				me->dy+=Sine(me->facing*32)*3;
				Clamp(&me->dx,9*FIXAMT);
				Clamp(&me->dy,9*FIXAMT);

				x=(me->x+Cosine((me->frm-4)*16)*100)/FIXAMT;
				y=(me->y+Sine((me->frm-4)*16)*80)/FIXAMT;
				if(me->AttackCheck(25,x,y,goodguy))
					goodguy->GetShot(Cosine((me->frm-4)*16)*8,Sine((me->frm-4)*16)*8,3,map,world);
				x=(me->x+Cosine((me->frm+4)*16)*100)/FIXAMT;
				y=(me->y+Sine((me->frm+4)*16)*80)/FIXAMT;
				if(me->AttackCheck(25,x,y,goodguy))
					goodguy->GetShot(Cosine((me->frm+4)*16)*8,Sine((me->frm+4)*16)*8,3,map,world);
			}
			else if(me->frm>=20)
			{
				me->dx=(me->dx*3)/4;
				me->dy=(me->dy*3)/4;
				if(me->frm==21)
					DoMove(me,ANIM_A1,256,0,0,0);
			}
		}
		if(me->seq==ANIM_A4 && me->frm==4 && me->frmTimer<128)
		{
			x=me->x-60*FIXAMT;
			y=me->y-10*FIXAMT;
			FireBullet(x,y,128,BLT_MISSILE);
			x=me->x+60*FIXAMT;
			y=me->y-10*FIXAMT;
			FireBullet(x,y,0,BLT_MISSILE);
			MakeSound(SND_FRANKMISSILE,me->x,me->y,SND_CUTOFF,1200);
		}
		if(me->seq==ANIM_A4 && me->frm==10)
		{
			DoMove(me,ANIM_A2,256,0,0,0);
		}
		return;
	}
	if(RangeToTarget(me,goodguy)<65*FIXAMT)
	{
		// 'acidic'
		goodguy->GetShot(0,0,monsType[me->type].damage,map,world);
	}

	FaceGoodguy2(me,goodguy);
	me->dx+=Cosine(me->facing*32)/4;
	me->dy+=Sine(me->facing*32)/4;
	Clamp(&me->dx,monsType[me->type].speed);
	Clamp(&me->dy,monsType[me->type].speed);

	if(me->mind==0)	// have head
	{
		if(me->mind1==0)
			me->mind1=30*3;
		else
		{
			me->mind1--;
			if(me->mind1==0)
			{
				DoMove(me,ANIM_ATTACK,256,1,me->dx,me->dy);
			}
		}
	}
	if(me->mind==1)	// lost head already
	{
		if(me->seq==ANIM_IDLE || me->seq==ANIM_MOVE || (me->seq==ANIM_A1 && me->frm>1))
		{
			DoMove(me,ANIM_A1,256,0,me->dx,me->dy);
		}
		if(me->mind1==0)
			me->mind1=30*3;
		else
		{
			me->mind1--;
			if(me->mind1==0)
			{
				MakeSound(SND_FRANKARMS,me->x,me->y,SND_CUTOFF,1200);
				DoMove(me,ANIM_A3,160,1,me->dx,me->dy);
				FaceGoodguy(me,goodguy);
			}
			if(me->mind1%15==0)
			{
				MakeSound(SND_GRENADE,me->x,me->y,SND_CUTOFF,1200);
				FireBullet(me->x,me->y,(byte)Random(256),BLT_ORBGRENADE);
			}
		}
	}
	if(me->mind==2)	// lost body too
	{
		if(me->seq==ANIM_IDLE || me->seq==ANIM_MOVE || (me->seq==ANIM_A2 && me->frm>1))
		{
			DoMove(me,ANIM_A2,256,0,me->dx,me->dy);
		}
		if(me->mind1==0)
			me->mind1=30*2;
		else
		{
			me->mind1--;
			if(me->mind1==0)
			{
				DoMove(me,ANIM_A4,256,1,me->dx,me->dy);
			}
		}
	}
}

void AI_MechaBonkula(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	Guy *g;

	SetBossHP(me->hp,MonsterHP(me->type));

	if(me->reload)
		me->reload--;

	if(me->ouch==3)
	{
		if(me->hp>0)
			MakeSound(SND_BONKOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_BONKDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	// handle introductions
	if(player.var[240]==0)
	{
		if(me->y>goodguy->y-FIXAMT*(TILE_HEIGHT*3+TILE_HEIGHT/2))
		{
			player.var[240]=1;
			me->dy=0;
		}
		else
			me->dy=FIXAMT*16;
		return;
	}
	else if(player.var[240]<10)
	{
		player.var[240]++;
		if(player.var[240]==10)
			BeginChatting(232);
		return;
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==8 && me->reload==0)
		{
			MakeSound(SND_BONKHIT,me->x,me->y,SND_CUTOFF,1200);
			x=me->x+Cosine(me->facing*32)*60;
			y=me->y+Sine(me->facing*32)*60;

			FindGoodVictim(x>>FIXSHIFT,y>>FIXSHIFT,6*15,Cosine(me->facing*32)*2,
							   Sine(me->facing*32)*2,monsType[me->type].damage,map,world);
			MakeRingParticle(x,y,0,6*15,10*15);
			me->reload=5;
		}

		if(me->seq==ANIM_DIE)
		{
			me->dx=0;
			me->dy=0;
			x=me->x-40*FIXAMT+Random(80*FIXAMT);
			y=me->y-30*FIXAMT+Random(60*FIXAMT);
			//FireBullet(x,y,0,BLT_FLAMEWALL);
			ExplodeParticles(PART_YELLOW,x,y,Random(10*FIXAMT),5);
			MakeSound(SND_BONKDIE,me->x,me->y-200*me->frm,SND_CUTOFF,2000);
		}
		if(me->seq==ANIM_A1)
		{
			me->facing++;
			if(me->facing>7)
				me->facing=0;
			if(Random(30)==0)
			{
				x=me->x-300*FIXAMT+Random(600)*FIXAMT;
				y=me->y-300*FIXAMT+Random(600)*FIXAMT;
				g=AddGuy(x,y,FIXAMT*50,MONS_VAMPIRE+(byte)Random(4));
				if(g && !g->CanWalk(g->x,g->y,map,world))
				{
					g->type=MONS_NONE;
				}
			}
		}
		return;	// can't do nothin' right now
	}

	if(me->mind3==0)
	{
		// begin spinning
		DoMove(me,ANIM_A1,256,1,0,0);
		me->mind3=60;
	}
	else
	{
		if(me->mind3==60)
		{
			FaceGoodguy(me,goodguy);
			me->dx=Cosine(me->facing*32)*6;
			me->dy=Sine(me->facing*32)*6;
			me->dx+=-FIXAMT*2+Random(4*FIXAMT);
			me->dy+=-FIXAMT*2+Random(4*FIXAMT);
		}
		me->mind3--;
		if(RangeToTarget(me,goodguy)<150*FIXAMT)
		{
			MakeSound(SND_BONKSWING,me->x,me->y,SND_CUTOFF,2000);
			DoMove(me,ANIM_ATTACK,256,1,0,0);
			me->mind3=0;
		}
	}
}

// summoned monsters
void SummonAI(Guy *me,Guy *target,int range,Map *map,world_t *world,Guy *goodguy)
{
	int moveFactor;

	moveFactor=128+128*player.fireRate/10;	// a modifier from 50% to 100% (128-256) modifying your movement speed

	if(me->action==ACTION_BUSY)
		return;

	if(me->mind2)	// while mind2 is on, you just head in whichever direction you're facing
		me->mind2--;
	else
	{
		if(me->mind1)	// bumped a wall
		{
			me->mind2=10+(byte)Random(40);
			me->facing=(me->facing+7+(byte)Random(3))&7;
		}
		else
		{
			if(me->mind==0)	// seeking enemies
			{
				if(RangeToTarget(me,goodguy)>range || target==NULL)	// return home to Summony
				{
					me->mind=1;
					return;
				}
				FaceGoodguy2(me,target);
				me->mind2=3+(byte)Random(5);
			}
			else	// coming home
			{
				FaceGoodguy2(me,goodguy);
				me->mind2=3+(byte)Random(5);
				if(RangeToTarget(me,goodguy)<range/2 && target!=NULL)	// go hunting again
				{
					me->mind=0;
					return;
				}
				if(RangeToTarget(me,goodguy)<=range/2 && target==NULL)	// if you are within the okay range, but just have no target, wander
				{
					if(Random(30)==0)
					{
						me->facing=(me->facing+7+(byte)Random(3))&7;
						me->mind2=10+(byte)Random(40);
					}
				}
			}
		}
	}

	me->dx=(Cosine(me->facing*32)*monsType[me->type].speed*moveFactor/256)/FIXAMT;
	me->dy=(Sine(me->facing*32)*monsType[me->type].speed*moveFactor/256)/FIXAMT;
	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=moveFactor;
	}
	if(RangeToTarget(me,goodguy)>600*FIXAMT)
	{
		me->x=goodguy->x;
		me->y=goodguy->y;
		me->bright=32;
		MakeNormalSound(SND_AUTODOOROPEN);
	}
}

void AI_Summon_Bombie(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	Guy *target;
	int dmgFactor;
	word w;

	dmgFactor=128+128*player.firePower/10;	// a modifier from 50% to 100% (128-256) modifying your damage

	w=LockOnEvil(me->x>>FIXSHIFT,me->y>>FIXSHIFT);
	if(w==65535)
		target=NULL;
	else
		target=GetGuy(w);

	BasicAI(me,SND_ZOMBIEOUCH,SND_ZOMBIEDIE,map,world,goodguy);

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==5 && me->reload==0)
		{
			me->reload=5;
			me->dx=Cosine(me->facing*32)*8;
			me->dy=Sine(me->facing*32)*8;
			me->reload=5;
		}
		if(me->seq==ANIM_ATTACK && me->frm>5)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			if(target && me->AttackCheck(24,x>>FIXSHIFT,y>>FIXSHIFT,target))
			{
				target->GetShot(Cosine(me->facing*32)*12,Sine(me->facing*32)*12,monsType[me->type].damage*dmgFactor/256,map,world);
			}
		}
		if(me->seq==ANIM_ATTACK && me->frm>9)
		{
			Dampen(&me->dx,FIXAMT*2);
			Dampen(&me->dy,FIXAMT*2);
			if(me->frm==11 && me->reload==0)
			{
				// blow self up
				FireBullet(me->x,me->y,0,BLT_SUMBOOM);
				me->seq=ANIM_DIE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->dx=0;
				me->dy=0;
				me->hp=0;
				return;
			}
		}

		return;	// can't do nothin' right now
	}

	x=160*FIXAMT;
	y=4;

	if(target && RangeToTarget(me,target)<(x) && Random(y)==0 &&
		map->CheckLOS(me->mapx,me->mapy,8,target->mapx,target->mapy))
	{
		// get him!
		FaceGoodguy(me,target);
		MakeSound(SND_ZOMBIELEAP,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		me->reload=0;
		return;
	}

	if((me->mapx>0 && map->map[me->mapx-1+me->mapy*map->width].item==ITM_BOOMROCK))
	{
		// autodestruct if a boomrock is nearby
		me->facing=4;
		MakeSound(SND_ZOMBIELEAP,me->x,me->y,SND_CUTOFF,1200);
		DoMove(me,ANIM_ATTACK,128,1,0,0);
		me->reload=0;
		return;
	}
	if(me->mapx<map->width-1 && map->map[me->mapx+1+me->mapy*map->width].item==ITM_BOOMROCK)
	{
		// autodestruct if a boomrock is nearby
		me->facing=0;
		MakeSound(SND_ZOMBIELEAP,me->x,me->y,SND_CUTOFF,1200);
		DoMove(me,ANIM_ATTACK,128,1,0,0);
		me->reload=0;
		return;
	}
	if(me->mapy>0 && map->map[me->mapx+(me->mapy-1)*map->width].item==ITM_BOOMROCK)
	{
		// autodestruct if a boomrock is nearby
		me->facing=2;
		MakeSound(SND_ZOMBIELEAP,me->x,me->y,SND_CUTOFF,1200);
		DoMove(me,ANIM_ATTACK,128,1,0,0);
		me->reload=0;
		return;
	}
	if(me->mapy<map->height-1 && map->map[me->mapx+(me->mapy+1)*map->width].item==ITM_BOOMROCK)
	{
		// autodestruct if a boomrock is nearby
		me->facing=6;
		MakeSound(SND_ZOMBIELEAP,me->x,me->y,SND_CUTOFF,1200);
		DoMove(me,ANIM_ATTACK,128,1,0,0);
		me->reload=0;
		return;
	}



	SummonAI(me,target,300*FIXAMT,map,world,goodguy);
	me->mind1=0;
}

void AI_Summon_Bat(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	bullet_t *b;
	Guy g;
	int moveFactor;

	moveFactor=128+128*player.fireRate/10;	// a modifier from 50% to 100% (128-256) modifying your movement speed

	BasicAI(me,SND_BATOUCH,SND_BATDIE,map,world,goodguy);

	if(me->mind2)
		me->mind2--;

	b=FindItemBullet(me->x,me->y);
	if(b!=NULL)
	{
		g.x=b->x;
		g.y=b->y;
	}

	if(me->action==ACTION_BUSY)
	{
		return;
	}

	if(me->mind==0)	// standard item-hunt mode
	{
		if(RangeToTarget(me,goodguy)>600*FIXAMT || b==NULL)
		{
			// too far from goodguy, run home
			me->mind=1;
		}
		else
		{
			if(b==NULL || RangeToTarget(me,&g)>500*FIXAMT)	// too far to fight, go home
			{
				me->mind=1;
			}
			else
			{
				if(me->mind2==0 && b!=NULL)
				{
					FaceGoodguy2(me,&g);
					me->mind2=2;
				}
			}
		}
	}
	else	// running home mode
	{
		if(RangeToTarget(me,goodguy)<200*FIXAMT && b!=NULL && RangeToTarget(me,&g)<500*FIXAMT)
		{
			// got close enough, get violent again
			me->mind=0;
		}
		if(me->mind2==0)
		{
			FaceGoodguy2(me,goodguy);
			me->mind2=2;
		}
	}
	me->frmAdvance=256;
	Dampen(&me->dx,FIXAMT/2);
	Dampen(&me->dy,FIXAMT/2);
	me->dx+=Cosine(me->facing*32)*2;
	me->dy+=Sine(me->facing*32)*2;
	Clamp(&me->dx,monsType[me->type].speed*moveFactor/256);
	Clamp(&me->dy,monsType[me->type].speed*moveFactor/256);
	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=moveFactor;
	}
}

void AI_Summon_Frog(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	Guy *target;
	int dmgFactor,moveFactor;
	word w;

	moveFactor=128+128*player.fireRate/10;	// a modifier from 50% to 100% (128-256) modifying your movement speed

	dmgFactor=32+224*player.firePower/10;
	w=LockOnEvil(me->x>>FIXSHIFT,me->y>>FIXSHIFT);
	if(w==65535)
		target=NULL;
	else
		target=GetGuy(w);

	BasicAI(me,SND_FROGOUCH,SND_FROGDIE,map,world,goodguy);

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==3 && me->reload==0)
		{
			me->reload=2;
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			FireBullet(x,y,me->facing*32,BLT_SUMGAS);
		}

		if(me->seq==ANIM_DIE && me->frm==4)
		{
			for(y=0;y<8*dmgFactor/256;y++)
			{
				x=Random(256);
				FireBullet(me->x,me->y,x,BLT_SUMGAS);
			}
		}
		return;	// can't do nothin' right now
	}

	if(target && RangeToTarget(me,target)<(256*FIXAMT) && Random(24)==0 &&
		map->CheckLOS(me->mapx,me->mapy,8,target->mapx,target->mapy))
	{
		// spit out gas
		MakeSound(SND_FROGLICK,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=moveFactor;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		me->reload=0;
		FaceGoodguy(me,target);
		return;
	}

	SummonAI(me,target,300*FIXAMT,map,world,goodguy);
	me->mind1=0;
}

void AI_Summon_HotDog(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;
	Guy *target;
	int dmgFactor,moveFactor;
	word w;

	moveFactor=128+128*player.fireRate/10;	// a modifier from 50% to 100% (128-256) modifying your movement speed

	dmgFactor=32+224*player.firePower/10;
	w=LockOnEvil(me->x>>FIXSHIFT,me->y>>FIXSHIFT);
	if(w==65535)
		target=NULL;
	else
		target=GetGuy(w);

	BasicAI(me,SND_SWAMPOUCH,SND_SWAMPDIE,map,world,goodguy);

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_A3 && me->frm==6 && me->reload==0)
		{
			me->reload=2;
			for(i=0;i<player.firePower/2+1;i++)
			{
				w=(player.firePower/2+1)/2;
				w=me->facing*32+256-w*(player.firePower*2)+i*(player.firePower*2);
				w&=255;
				x=me->x+Cosine(w)*24;
				y=me->y+Sine(w)*24;
				FireBullet(x,y,(byte)w,BLT_SUMFLAME);
			}
		}

		return;	// can't do nothin' right now
	}

	if(target && RangeToTarget(me,target)<((dmgFactor+16)*FIXAMT) && Random(16)==0 &&
		map->CheckLOS(me->mapx,me->mapy,8,target->mapx,target->mapy))
	{
		// spit out flames
		MakeSound(SND_SWAMPSHOOT,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_A3;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=moveFactor;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		me->reload=0;
		FaceGoodguy(me,target);
		return;
	}

	SummonAI(me,target,300*FIXAMT,map,world,goodguy);
	me->mind1=0;
}

void AI_Summon_Mummy(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;
	int dmgFactor,moveFactor;

	moveFactor=128+128*player.fireRate/10;	// a modifier from 50% to 100% (128-256) modifying your movement speed

	dmgFactor=240*player.firePower/10;

	BasicAI(me,SND_ZOMBIEOUCH,SND_ZOMBIEDIE,map,world,goodguy);

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_A1 && me->frm==3 && me->reload==0 && me->type!=MONS_MUMMY3)
		{
			x=me->x+Cosine(me->facing*32)*8;
			y=me->y+Sine(me->facing*32)*8;
			FireBullet(x,y,me->facing*32,BLT_SUMHEAL);
			me->reload=255-dmgFactor;
		}
		if(me->seq==ANIM_A2 && me->frm==4 && me->frmTimer<128)
		{
			for(i=0;i<(player.firePower/2)+1;i++)
				FireBullet(me->x,me->y,ITM_TINYHEART,BLT_ITEM);
			me->reload=255-dmgFactor;
		}
		return;	// can't do nothin' right now
	}

	if(me->reload==0 && Random(16)==0)
	{
		// launch a healing shot
		MakeSound(SND_MUMMYSHOOT,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_A1;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=moveFactor;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		return;
	}
	if(me->reload==0 && Random(128)==0)
	{
		// launch a player heart
		MakeSound(SND_MUMMYSUMMON,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_A2;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=moveFactor;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		me->reload=0;
		return;
	}

	SummonAI(me,NULL,300*FIXAMT,map,world,goodguy);
	me->mind1=0;
}

void AI_Summon_Ghost(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	Guy *target;
	int dmgFactor,moveFactor;
	word w;

	moveFactor=128+128*player.fireRate/10;	// a modifier from 50% to 100% (128-256) modifying your movement speed

	dmgFactor=32+224*player.firePower/10;
	w=LockOnEvil(me->x>>FIXSHIFT,me->y>>FIXSHIFT);
	if(w==65535)
		target=NULL;
	else
		target=GetGuy(w);

	BasicAI(me,SND_GHOSTOUCH,SND_GHOSTDIE,map,world,goodguy);

	if(me->action==ACTION_BUSY)
	{
		return;	// can't do nothin' right now
	}

	if(me->reload==0)
	{
		me->reload=20-20*dmgFactor/256;
		FireBullet(me->x,me->y,0,BLT_SUMSHOCK);
	}

	SummonAI(me,target,300*FIXAMT,map,world,goodguy);
	me->mind1=0;
}

void AI_Summon_Wolf(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;
	Guy *target;
	int dmgFactor,moveFactor;
	word w;

	moveFactor=128+128*player.fireRate/10;	// a modifier from 50% to 100% (128-256) modifying your movement speed

	dmgFactor=32+224*player.firePower/10;
	w=LockOnEvil(me->x>>FIXSHIFT,me->y>>FIXSHIFT);
	if(w==65535)
		target=NULL;
	else
		target=GetGuy(w);

	BasicAI(me,SND_WOLFOUCH,SND_WOLFDIE,map,world,goodguy);

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_A1 && me->frm==3 && me->reload==0)
		{
			me->reload=2;
			for(i=0;i<player.firePower/2+1;i++)
			{
				w=(player.firePower/2+1)/2;
				w=me->facing*32+256-w*(player.firePower*2)+i*(player.firePower*2);
				w&=255;
				x=me->x+Cosine(w)*16;
				y=me->y+Sine(w)*16;
				FireBullet(x,y,(byte)w,BLT_SUMFROST);
			}
		}

		return;	// can't do nothin' right now
	}

	if(target && RangeToTarget(me,target)<((dmgFactor+16)*FIXAMT) && Random(16)==0 &&
		map->CheckLOS(me->mapx,me->mapy,8,target->mapx,target->mapy))
	{
		// spit out ice
		MakeSound(SND_WOLFSPIT,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_A1;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=moveFactor;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		me->reload=0;
		FaceGoodguy(me,target);
		return;
	}

	SummonAI(me,target,300*FIXAMT,map,world,goodguy);
	me->mind1=0;
}

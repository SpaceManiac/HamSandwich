#include "monster.h"
#include "player.h"
#include "fairy.h"
#include "spell.h"
#include "water.h"

monsterType_t monsType[NUM_MONSTERS]=
	{
		{"Null",
		 0,0,0,0,"",0},
		{"Kid Mystic",
		 11,25,128,0,"graphics/sylabulus.jsp",0,MF_WATERWALK|MF_GOODGUY,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,10,11,11,12,12,255},	// attack
				{15,16,17,18,19,20,21,22,23,24,255},		// die
				{13,13,14,14,13,13,255},	// cast non-pointy spell
				{7,8,9,10,11,12,12,255},	// cast pointy spell
				{1,3,1,0,4,6,4,0,1,3,1,0,0,254,254,254,254,255},	// watery death
				{24,23,22,21,20,19,18,17,16,15,255},		// A4=undie
			}},
		{"Bonehead",
		 8,38,10,50,"graphics/skeleton.jsp",0,0,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{20,21,22,23,24,255},	// attack
				{7,8,9,10,11,12,13,14,15,16,255},		// die
				{25,26,27,28,29,30,255},	// chomp/spit projectile
				{31,32,32,32,31,255}	// point at bouapha
			}},
		{"Scary Bat",
		 8,27,5,25,"graphics/bat.jsp",0,MF_FLYING,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,8,7,255},	// attack
				{17,18,19,20,21,22,23,24,25,26,255},		// die
				{10,11,12,12,12,12,12,11,10,255},	// diving attack
				{13,14,15,15,16,255}	// bounce off during dive
			}},
		{"Eensy Weensy",
		 4,22,1,10,"graphics/spider.jsp",0,0,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,8,9,10,11,0,255},	// move
				{12,13,14,13,12,255},	// attack
				{15,16,17,18,19,18,19,18,19,20,20,21,21,255},	// die
			}},
		{"Spitter",
		 8,21,10,50,"graphics/bigspdr.jsp",0,0,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,12,13,13,255},	// attack
				{14,15,16,17,18,17,18,17,18,19,19,20,20,255},	// die
			}},
		{"Zombie",
		 11,32,20,150,"graphics/zombie.jsp",0,0,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,9,10,11,12,13,13,14,15,16,17,18,19,255},	// attack
				{20,21,22,23,24,25,26,27,28,29,30,31,255},	// die
			}},
		{"Egg Sac",
		 20,19,150,100,"graphics/eggsac.jsp",0,MF_ONEFACE|MF_ENEMYWALK|MF_NOMOVE,
			{
				{0,0,0,0,0,0,0,0,1,2,1,0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,0,255},	// attack
				{19,20,21,22,23,24,25,255},	// die
			}},
		{"Mama Spider",
		 30,30,200,300,"graphics/mamaspdr.jsp",0,MF_NOMOVE,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,12,13,13,255},	// spit (attack)
				{19,20,21,22,23,24,25,25,25,26,27,28,29,255},	// die
				{14,15,16,17,18,255},	// bite (A1)
			}},
		{"Pygmy",
		 10,50,15,100,"graphics/pygmy.jsp",0,0,
			{
				{0,0,0,0,0,0,0,0,0,0,1,2,3,2,1,0,0,0,4,5,6,5,4,255},	// idle
				{8,9,10,9,8,7,11,12,13,12,11,7,255},	// move
				{29,30,31,31,32,33,34,35,36,255},	// attack
				{37,38,39,40,41,42,43,44,45,46,255},	// die
				{14,15,16,17,18,19,20,16,17,18,19,15,14,255},	// (A1) bored (spin spear)
				{21,22,23,24,25,26,26,26,27,28,27,26,27,28,27,26,255}, // (A2) bored (yawn)
				{47,48,49,48,47,0,47,48,49,48,47,255},	// (A3) angry, spotted Bouapha
			}},
		{"Aquazoid",
		 15,20,15,75,"graphics/serpent.jsp",0,MF_AQUATIC|MF_WATERWALK,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,255},	// move
				{4,5,6,7,8,9,10,11,12,255},	// spit
				{13,14,15,16,17,18,19,255},	// die
			}},
		{"Friendly",	// a friendly bystander
		 9,4,100,0,"graphics/friendly.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_INVINCIBLE,
			{
				{0,255},	// idle (bald guy)
				{1,255},	// move (blond hair)
				{2,255},	// fire (chick)
				{0,255},	// die
				{3,255}		// A1   (wizard)
			}},
		{"Pterodactyl",	// pterodactyl - kid mystic's little buddy
		 10,11,15,5,"graphics/ptero.jsp",0,MF_FLYING|MF_ENEMYWALK|MF_FREEWALK|MF_GOODGUY,
			{
				{0,1,2,6,3,4,5,4,3,6,2,1,255},	// idle
				{0,1,2,6,3,4,5,4,3,6,2,1,255},	// move
				{7,8,9,10,9,8,7,255},	// fire
				{0,0,0,255},	// die
			}},
		{"Eye Guy",	// Eye Guy
		 14,17,20,75,"graphics/eyeguy.jsp",0,0,
			{
				{0,0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,255},	// attack
				{13,14,15,16,255},	// die
			}},
		{"Oculoid",
		 26,8,40,150,"graphics/peeper.jsp",0,MF_FLYING|MF_NOMOVE,
			{
				{4,4,255},	// idle
				{4,4,255},	// move
				{5,6,7,6,5,255},	// fire
				{3,2,1,1,1,0,0,0,0,255},	// die
				{3,2,1,0,1,2,3,255},	// A1 = blink
				{0,0,1,1,2,2,3,3,255},	// A2 = wake up
			}},
		{"Tower Guardian",
		 60,17,500,500,"graphics/tower.jsp",0,MF_NOMOVE,
			{
				{0,0,0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{5,5,6,6,7,7,7,8,9,9,9,9,9,10,10,10,11,11,11,255},	// attack (leap)
				{12,13,14,15,16,16,16,16,16,255},	// die
				{1,2,2,2,2,2,2,1,255},	// A1
			}},
		{"The Lookey-Loo",
		 64,7,1000,2000,"graphics/lookyloo.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_SPRITEBOX,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,6,6,6,6,6,6,6,6,6,5,4,3,2,1,255},	// eye-beam (frm 9 is shoot)
				{1,3,5,6,6,6,6,255},	// die
			}},
		{"Pinkeye",
		 11,13,20,1,"graphics/pinkeye.jsp",0,MF_NOMOVE,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,7,8,255},	// fire
				{9,10,11,12,12,11,10,9,9,255},	// die & resurrect (freezes on frm 3 until ready)
			}},
		{"Mushlord",
		 15,14,40,150,"graphics/shrmlord.jsp",0,0,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,7,8,8,8,9,10,11,12,12,13,13,255},	// spit spores
				{0,0,255},	// die
			}},
		{"Microfish",
		 1,1,1,1,"graphics/micro.jsp",0,MF_AQUATIC|MF_NOSHADOW|MF_WATERWALK,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack unused
				{0,255},	// die
			}},
		{"Grumpy Golem",
		 32,26,100,0,"graphics/golem.jsp",0,MF_GOODGUY|MF_NOMOVE|MF_FREEWALK,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,12,12,13,13,14,14,255},	// attack
				{15,16,17,18,19,20,21,21,21,254,21,254,21,254,21,255},	// die
				{22,23,24,25,255},	// form out of nothing (A1)
			}},
		{"Mucho Moss",
		 15,17,10,1,"graphics/moss.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_FREEWALK|MF_ENEMYWALK|MF_WALLWALK|MF_NOSHADOW,
			{
				{0,255},	// idle
				{1,2,3,2,1,255},	// move=grow vertically
				{4,5,6,5,4,255},	// attack=grow horizontally
				{7,8,9,10,11,12,13,14,15,16,255},	// die
			}},
		{"Moss Grande",
		 18,17,100,250,"graphics/mossgrnd.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_ENEMYWALK,
			{
				{0,1,2,3,4,5,6,255},	// idle
				{0,255},	// move=unused
				{7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,255},	// attack
				{22,23,24,25,26,27,28,255},	// die
			}},
		{"Magmazoid",
		 15,20,20,80,"graphics/magmazoid.jsp",0,MF_AQUATIC|MF_WATERWALK,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,255},	// move
				{4,5,6,7,8,9,10,10,10,10,10,10,10,11,12,255},	// attack
				{13,14,15,16,17,18,19,255},	// die
			}},
		{"Mushie",
		 15,23,10,25,"graphics/shroom.jsp",0,0,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,255},	// attack
				{15,16,17,18,19,20,21,22,255},	// die
			}},
		{"King Mushroom",
		 32,27,200,800,"graphics/bgshroom.jsp",0,MF_NOMOVE,
			{
				{0,255},	// idle
				{1,2,2,1,0,3,4,4,3,0,255},	// move
				{5,6,6,7,8,9,10,255},	// attack=cannon fire
				{21,22,23,24,25,26,26,26,26,255},	// die
				{11,12,13,12,13,12,13,12,13,12,13,12,13,12,13,12,13,14,15,16,255},	// A1=sporegun fire
				{17,18,19,20,19,18,19,20,19,18,17,255},	// A2=angry
			}},
		{"The Thing",
		 64,11,800,5000,"graphics/thething.jsp",0,MF_NOMOVE|MF_AQUATIC|MF_WATERWALK|MF_ENEMYWALK|MF_ONEFACE,
			{
				{0,255},	// idle
				{0,255},	// move
				{3,4,5,6,7,8,9,10,10,10,9,8,7,6,5,4,3,255},	// attack=tongue
				{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255},	// die
				{1,2,1,255},	// A1=blink
			}},
		{"Tentacle",
		 32,1,500,100,"graphics/thingtent.jsp",0,MF_NOMOVE|MF_FREEWALK|MF_NOGRAV|MF_ENEMYWALK,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{0,0,0,0,0,255},	// die
			}},
		{"Tentacle",	// tentacle tip
		 32,1,500,100,"graphics/thingtent2.jsp",0,MF_NOMOVE|MF_FREEWALK|MF_NOGRAV|MF_ENEMYWALK,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{0,0,0,0,0,255},	// die
			}},
		{"Die Uberbat",
		 64,7,1000,1000,"graphics/hugebat.jsp",0,MF_NOMOVE|MF_ENEMYWALK|MF_ONEFACE,
			{
				{0,0,255},	// idle
				{0,255},	// move
				{1,2,3,2,1,255},	// attack=spit bats
				{4,4,4,5,5,5,6,6,6,6,6,6,255},	// die
			}},
		{"Fairy",
		 12,6,100,0,"graphics/fairy.jsp",0,MF_FLYING|MF_GOODGUY|MF_ENEMYWALK|MF_FREEWALK|MF_NOHIT,
			{
				{0,1,2,3,4,5,4,3,2,1,255},	// idle
				{0,1,2,3,4,5,4,3,2,1,255},	// move
				{10,11,11,10,255},	// attack=unused
				{3,3,3,3,3,4,5,255},	// die=unused
			}},
		{"Scarier Bat",
		 26,15,200,500,"graphics/bigbat.jsp",0,MF_FLYING|MF_NOMOVE|MF_ENEMYWALK,
			{
				{0,1,2,1,0,3,4,5,4,3,255},	// idle
				{0,1,2,1,0,3,4,5,4,3,255},	// move
				{6,7,8,8,7,7,6,255},	// attack=spit bat
				{9,10,11,12,13,14,255},	// die
			}},
		{"Bobby Khan",
		 48,46,2000,5000,"graphics/bobby.jsp",0,MF_NOMOVE|MF_ONEFACE,
			{
				{0,0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,255},	// attack=sword slash
				{42,42,42,43,43,43,44,44,44,44,44,44,255},	// die
				{15,16,17,17,17,17,17,17,17,17,17,16,15,255},	//A1=block and reflect
				{18,19,19,19,20,21,22,23,24,25,26,27,28,28,29,29,255},			//A2=spin of doom
				{30,31,31,31,32,33,34,35,36,37,38,39,40,41,255},	// A3=stomp
				{45,45,45,17,17,255},	// A4=blocked a shot
			}},
		{"Fairy2",
		 12,6,100,0,"graphics/fairy.jsp",0,MF_FLYING|MF_GOODGUY|MF_ENEMYWALK|MF_FREEWALK|MF_WALLWALK|MF_NOHIT,
			{
				{0,1,2,3,4,5,4,3,2,1,255},	// idle
				{0,1,2,3,4,5,4,3,2,1,255},	// move
				{10,11,11,10,255},	// attack=unused
				{3,3,3,3,3,4,5,255},	// die=unused
			}},
		{"Crazy Bush",
		 10,8,1,5,"graphics/crazybush.jsp",0,0,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{0,255},	// attack
				{5,6,7,255},	// die
			}},
		{"Octon",
		 19,21,20,40,"graphics/octopus.jsp",0,MF_AQUATIC|MF_WATERWALK,
			{
				{5,6,7,6,255},	// idle
				{5,6,6,7,6,6,255},	// move
				{16,17,18,19,20,19,18,17,18,19,20,19,18,17,18,19,20,19,18,17,16,255},	// attack=spit from arisen postion
				{8,9,10,11,12,13,14,15,255},	// die
				{5,4,3,2,1,0,0,255},	// A1=arise
				{1,2,3,4,5,5,255},	// A2=descend

			}},
		{"Blockhead",
		 12,23,5,2,"graphics/incabot.jsp",0,MF_NOMOVE,
			{
				{0,255},	// idle
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,255},	// attack=unused
				{16,17,18,19,20,21,22,255},	// die
			}},
		{"Blockhead Kiln",
		 40,16,100,100,"graphics/incagen.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_ENEMYWALK,
			{
				{0,255},	// idle
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move=summon
				{0,255},	// attack=unused
				{0,0,0,255},	// die=explode
			}},
		{"Megablockhead H",
		 11,16,20,75,"graphics/incagold.jsp",0,MF_NOMOVE|MF_INVINCIBLE,
			{
				{0,255},	// idle
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,255},	// attack=unused
				{0,255},	// die=doesn't!
			}},
		{"Megablockhead V",
		 11,16,20,75,"graphics/incagold.jsp",0,MF_NOMOVE|MF_INVINCIBLE,
			{
				{0,255},	// idle
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,255},	// attack=unused
				{0,255},	// die=doesn't!
			}},
		{"Log",
		 15,10,10,50,"graphics/log.jsp",0,MF_WATERWALK|MF_AQUATIC|MF_FREEWALK|MF_NOHIT|MF_ONEFACE|MF_FACECMD|MF_ENEMYWALK,
			{
				{0,1,2,3,4,5,6,7,8,9,255},	// idle
				{0,1,2,3,4,5,6,7,8,9,255},	// move
				{0,255},	// attack=unused
				{0,1,2,255},	// die
			}},
		{"Blocko The Great",
		 65,20,150,500,"graphics/incaboss.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_ENEMYWALK|MF_INVINCIBLE,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,5,5,5,5,5,5,5,5,5,5,5,4,3,2,1,255},	// attack=open mouth flames
				{16,16,17,17,18,18,19,19,19,19,255},	// die (loop the end forever to leave the corpse)
				{1,2,3,4,5,6,7,8,9,10,11,12,8,9,10,11,12,7,6,5,4,3,2,1,255}, // A1=tongue
				{13,13,13,14,14,14,15,15,15,15,15,15,15,15,15,15,15,15,14,13,255},	// A2=eyebeam
			}},
		{"Blocko Tongue",
		 45,1,25,100,"graphics/log.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_ENEMYWALK,
			{
				{254,255},	// idle
				{254,255},	// move
				{254,255},	// attack
				{254,255},	// die
			}},
		{"Peeping Bomb",
		 13,41,20,50,"graphics/peepbomb.jsp",0,MF_FLYING|MF_ONEFACE|MF_FACECMD,
			{
				{0,0,0,0,255},	// idle
				{0,0,0,255},	// move
				{0,255},	// attack=rotate to next facing
				{32,33,34,35,36,37,38,39,40,255},	// die=explode
			}},
		{"Richie Lich",
		 50,8,1500,1000,"graphics/lich.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_GHOST|MF_NOSHADOW|MF_FLYING,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,4,4,4,4,4,4,4,4,4,4,4,4,3,2,1,255},	// attack=open mouth
				{0,5,6,7,8,255},	// die
			}},
		{"Trapazoid",
		 14,10,30,70,"graphics/trapazoid.jsp",0,MF_NOMOVE|MF_NOSHADOW,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,7,8,9,9,255},	// attack=torch in a circle
				{6,5,4,3,2,1,0,255},	// die (turn into dud trapazoid)
			}},
		{"Trapazoid2",
		 14,10,30,70,"graphics/trapazoid.jsp",0,MF_NOHIT|MF_NOMOVE|MF_NOSHADOW|MF_ENEMYWALK|MF_FREEWALK,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack=torch in a circle
				{0,255},	// die (turn into dud trapazoid)
			}},
		{"Peeping Bomb",
		 13,41,20,50,"graphics/peepbomb.jsp",0,MF_FLYING|MF_ONEFACE|MF_FACECMD,
			{
				{0,0,0,0,255},	// idle
				{0,0,0,255},	// move
				{0,255},	// attack=rotate to next facing
				{32,33,34,35,36,37,38,39,40,255},	// die=explode
			}},
		{"Whackazoid",
		 14,10,1,5,"graphics/trapazoid.jsp",0,MF_NOMOVE|MF_NOSHADOW,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,7,7,7,7,7,7,7,7,7,7,6,5,4,3,2,1,255},	// attack=torch in a circle
				{6,5,4,3,2,1,0,255},	// die (turn into dud trapazoid)
			}},
		{"Slimy Slug",
		 12,19,30,50,"graphics/slug.jsp",0,0,
			{
				{0,255},	// idle
				{0,1,2,3,4,3,2,1,255},	// move
				{1,2,3,255},	// attack=unused
				{5,6,7,8,9,10,11,12,13,14,15,16,17,18,255},	// die
			}},
		{"Sniper Snail",
		 12,19,45,60,"graphics/snail.jsp",0,0,
			{
				{0,255},	// idle
				{1,2,3,4,2,1,0,255},	// move
				{4,5,6,5,4,3,2,1,255},	// attack=spit goop
				{5,6,7,8,9,10,11,12,13,14,15,16,17,18,255},	// die
				{5,6,7,8,9,10,11,12,13,14,14,255},	// A1=go into shell
				{13,12,11,10,9,8,7,6,5,255},	// A2=get out of shell
			}},
		{"Sneaky Snail",	// in his shell
		 12,19,45,60,"graphics/snail.jsp",0,MF_INVINCIBLE,
			{
				{14,255},	// idle
				{1,2,3,4,2,1,0,255},	// move
				{4,5,6,5,4,3,2,1,255},	// attack=spit goop
				{5,6,7,8,9,10,11,12,13,14,15,16,17,18,255},	// die
				{5,6,7,8,9,10,11,12,13,14,14,255},	// A1=go into shell
				{13,12,11,10,9,8,7,6,5,255},	// A2=get out of shell
			}},
		{"Smashball",
		 14,1,40,500,"graphics/ball.jsp",0,MF_INVINCIBLE|MF_ENEMYWALK|MF_NOMOVE|MF_ONEFACE|MF_WATERWALK|MF_FREEWALK,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{0,255},	// die
			}},
		{"Optimum Octon",
		 90,15,200,300,"graphics/octoboss.jsp",0,MF_ONEFACE|MF_NOMOVE|MF_AQUATIC|MF_WATERWALK|MF_ENEMYWALK,
			{
				{0,255},	// idle
				{0,255},	// move
				{10,11,12,13,14,14,14,14,14,14,14,14,14,13,12,11,10,255},	// attack=spit
				{1,2,3,4,5,6,7,8,9,255},	// die
			}},
		{"Octentacle",
		 20,11,20,50,"graphics/octotent.jsp",0,MF_NOMOVE|MF_AQUATIC|MF_WATERWALK|MF_ENEMYWALK,
			{
				{1,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,7,8,9,10,9,8,7,6,5,4,3,2,1,255},	// attack=slap
				{6,5,4,3,2,1,0,255},	// die
			}},
		{"Octentacle",	// for the right side
		 20,11,20,50,"graphics/octotent.jsp",0,MF_NOMOVE|MF_AQUATIC|MF_WATERWALK|MF_ENEMYWALK,
			{
				{1,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,7,8,9,10,9,8,7,6,5,4,3,2,1,255},	// attack=slap
				{6,5,4,3,2,1,0,255},	// die
			}},
		{"Rammy Gruff",
		 20,52,100,300,"graphics/goat.jsp",0,MF_NOMOVE|MF_INVINCIBLE,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,8,9,10,11,12,1,255},	// move=charge
				{0,255},	// attack=none
				{32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,255},	// die
			}},
		{"Rammy Gruff",	// vulnerable
		 20,52,100,300,"!56",0,MF_NOMOVE,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,8,9,10,11,12,255},	// move=charge
				{0,255},	// attack=none
				{32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,255},	// die
				{13,14,15,16,17,18,19,20,21,22,23,15,16,17,18,19,20,21,22,23,15,14,13,255},	// A1=be stunned
			}},
		{"Surefoot Gruff",	// vulnerable, on the ground
		 20,52,200,300,"!56",0,MF_NOMOVE,
			{
				{0,255},	// idle
				{32,33,34,33,32,0,35,36,37,36,35,0,255},	// move=walk
				{24,25,26,26,255},	// attack=begin hop
				{32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,255},	// die
				{27,28,29,29,29,29,29,29,29,30,31,32,255},	// A1=squish onto the ground after a hop
			}},
		{"Surefoot Gruff",	// airborne, nohit
		 20,52,200,300,"!56",0,MF_NOMOVE|MF_NOHIT,
			{
				{26,255},	// idle
				{32,33,34,33,32,0,35,36,37,36,35,0,255},	// move=walk
				{0,255},	// attack=begin hop
				{32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,255},	// die
				{27,28,29,29,29,29,29,29,29,30,31,32,255},	// A1=squish onto the ground after a hop
			}},
		{"Disgruntled Gruff",
		 20,52,300,300,"!56",0,MF_NOMOVE,
			{
				{0,255},	// idle
				{32,33,34,33,32,0,35,36,37,36,35,0,255},	// move=walk
				{1,2,3,2,1,255},	// attack=shoot
				{32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,255},	// die
			}},
		{"Happy Stick Shroom",
		 9,6,15,50,"graphics/stkshr.jsp",0,MF_ONEFACE,
			{
				{0,1,255},	// idle
				{0,1,255},	// move=walk
				{2,2,2,2,2,2,255},	// attack=shoot
				{3,3,4,4,5,5,255},	// die
			}},
		{"Happy Stick Spider",
		 9,6,25,70,"graphics/stkspd.jsp",0,MF_ONEFACE,
			{
				{0,1,255},	// idle
				{0,1,255},	// move=walk
				{2,2,2,2,2,2,255},	// attack=shoot
				{3,4,3,4,3,4,254,3,254,4,254,3,254,254,4,254,254,3,255},	// die
			}},
		{"Happy Stick Corpse",
		 11,10,40,100,"graphics/stkskl.jsp",0,MF_ONEFACE,
			{
				{0,255},	// idle
				{0,1,0,2,255},	// move=walk
				{3,3,3,3,3,3,255},	// attack=shoot
				{4,5,6,7,8,9,255},	// die
			}},
		{"Happy Stick Bat",
		 11,6,20,60,"graphics/stkbat.jsp",0,MF_ONEFACE|MF_FLYING,
			{
				{0,1,255},	// idle
				{0,1,255},	// move=walk
				{2,2,2,2,2,2,255},	// attack=shoot
				{3,3,4,4,5,5,255},	// die
			}},
		{"Happy Stick Dancer",
		 11,10,400,5000,"graphics/stkskl.jsp",0,MF_ONEFACE|MF_FREEWALK|MF_ENEMYWALK|MF_NOMOVE,
			{
				{0,255},	// idle
				{0,1,0,2,0,1,0,2,255},	// move=walk
				{3,3,3,3,255},	// attack=shoot
				{4,5,6,7,8,9,255},	// die
			}},
	};

static byte kidSpr;

void InitMonsters(void)
{
	int i;

	for(i=0;i<NUM_MONSTERS;i++)
	{
		monsType[i].spr=NULL;
	}
	// just keep bouapha perma-loaded
	monsType[MONS_BOUAPHA].spr=new sprite_set_t(monsType[MONS_BOUAPHA].sprName);
	kidSpr=0;
}

void ExitMonsters(void)
{
	int i;

	for(i=1;i<NUM_MONSTERS;i++)
	{
		if(monsType[i].spr && monsType[i].sprName[0]!='!')
			delete monsType[i].spr;
		monsType[i].spr=NULL;
	}
}

void SetKidSprite(byte armageddon)
{
	if(kidSpr==armageddon)
		return;

	if(armageddon)
	{
		if(monsType[MONS_BOUAPHA].spr)
			delete monsType[MONS_BOUAPHA].spr;
		monsType[MONS_BOUAPHA].spr=new sprite_set_t("graphics/sylab2.jsp");
	}
	else
	{
		if(monsType[MONS_BOUAPHA].spr)
			delete monsType[MONS_BOUAPHA].spr;
		monsType[MONS_BOUAPHA].spr=new sprite_set_t("graphics/sylabulus.jsp");
	}
	kidSpr=armageddon;
}

void SetWaterwalk(word on)
{
	monsType[MONS_BOUAPHA].flags&=(~MF_WATERWALK);
	monsType[MONS_BOUAPHA].flags|=(MF_WATERWALK*on);
}

void PurgeMonsterSprites(void)
{
	int i;

	// note this starts at 2, skipping bouapha
	for(i=2;i<NUM_MONSTERS;i++)
	{
		if(monsType[i].spr && monsType[i].sprName[0]!='!')
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

word MonsterPoints(byte type)
{
	return monsType[type].points;
}

word MonsterHP(byte type)
{
	if(player.nightmare)
	{
		if(monsType[type].flags&MF_GOODGUY)
		{
			return monsType[type].hp*(100+player.spellStones*10)/100;
		}
		else
		{
			if(type==MONS_BOBBY)
				return monsType[type].hp*(NIGHTMAREHP*3/4);
			else
				return monsType[type].hp*NIGHTMAREHP;
		}
	}
	else
		return monsType[type].hp;
}

char *MonsterName(byte type)
{
	return monsType[type].name;
}

sprite_t *GetMonsterSprite2(byte type,byte frm)
{
	return monsType[type].spr->GetSprite(frm);
}

void LoadMySprite(byte type)
{
	int v;

	if(monsType[type].spr==NULL)
	{
		if(monsType[type].sprName[0]=='!')
		{
			// it's a repeat of someone else's sprite
			v=atoi(&monsType[type].sprName[1]);
			if(!monsType[v].spr)
				monsType[v].spr=new sprite_set_t(monsType[v].sprName);

			monsType[type].spr=monsType[v].spr;
		}
		else
			monsType[type].spr=new sprite_set_t(monsType[type].sprName);

		if(monsType[type].spr==NULL)
			FatalError("Out of memory!");
	}

}

sprite_t *GetMonsterSprite(byte type,byte seq,byte frm,byte facing)
{
	int v;

	// load if not loaded
	LoadMySprite(type);

	v=monsType[type].anim[seq][frm];

	if(v==254)
		return NULL;	// 254 means no sprite for this frame

	if(!(monsType[type].flags&MF_ONEFACE))
		v+=facing*monsType[type].framesPerDir;

	if(monsType[type].flags&MF_FACECMD)
		v+=facing;

	return monsType[type].spr->GetSprite(v);
}

byte GetMonsterFrameNum(byte type,byte seq,byte frm,byte facing)
{
	int v;

	// load if not loaded
	LoadMySprite(type);

	v=monsType[type].anim[seq][frm];

	if(v==254)
		return NULL;	// 254 means no sprite for this frame

	if(!(monsType[type].flags&MF_ONEFACE))
		v+=facing*monsType[type].framesPerDir;

	if(monsType[type].flags&MF_FACECMD)
		v+=facing;

	return v;
}

void MonsterDraw(int x,int y,int z,byte type,byte seq,byte frm,byte facing,char bright,byte mind1,byte ouch,word frozen)
{
	sprite_t *curSpr;
	int v;
	byte shld;

	// load if not loaded
	LoadMySprite(type);

	v=monsType[type].anim[seq][frm];

	if(v==254)
		return;	// don't draw this frame

	if(!(monsType[type].flags&MF_ONEFACE))
		v+=facing*monsType[type].framesPerDir;

	if(type==MONS_BOUAPHA)
	{
		shld=PlayerShield();
		//if((shld<16) && (shld&2))	// it blinks when there is 1/2 second left
			//shld=0;
		//curSpr=monsType[type].spr->GetSprite(464+(shld&7));
		//if(shld)
		//	SprDraw(x>>FIXSHIFT,(y>>FIXSHIFT)+1,1,255,bright,curSpr,DISPLAY_DRAWME|DISPLAY_GLOW);
		if(shld&1)
			return;	// blink on and off if the shield is up
		if(PlayerIsPoisoned())
		{
			curSpr=monsType[type].spr->GetSprite(v);
			if(!curSpr)
				return;
			SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,0,255,0,curSpr,DISPLAY_DRAWME|DISPLAY_SHADOW);
			if(ouch==0)
				SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,1,bright-4,curSpr,DISPLAY_DRAWME);	// green
			else
				SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,4,bright,curSpr,DISPLAY_DRAWME);
			return;
		}
		if(player.stoneskin)
		{
			curSpr=monsType[type].spr->GetSprite(v);
			if(!curSpr)
				return;
			SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,0,255,0,curSpr,DISPLAY_DRAWME|DISPLAY_SHADOW);
			SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,2-2*(player.spell[6]==2),bright-4+ouch*4,curSpr,DISPLAY_DRAWME);	// brown/grey
			return;
		}
		if(player.berserk)
			return;	// don't draw the actual player when berserk, to give pure ghostliness
	}

	if(monsType[type].flags&MF_FACECMD)
		v+=facing;

	if(type==MONS_FAIRY || type==MONS_FAIRY2)
		v+=(6*8*(mind1-1));

	curSpr=monsType[type].spr->GetSprite(v);
	if(!curSpr)
		return;

	if(!(monsType[type].flags&MF_NOSHADOW))
		SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,0,255,0,curSpr,DISPLAY_DRAWME|DISPLAY_SHADOW);

	if(ouch==0)
	{
		if(!(monsType[type].flags&MF_GHOST))
		{
			if(frozen>30 || (frozen>0 && (frozen&1)==0))
				SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,7,bright+4,curSpr,DISPLAY_DRAWME);	// frozen blue
			else
				SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,255,bright,curSpr,DISPLAY_DRAWME);
		}
		else
		{
			if(frozen)
				SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,7,bright-4,curSpr,DISPLAY_DRAWME);	// frozen blue
			else
				SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,255,bright,curSpr,DISPLAY_DRAWME|DISPLAY_GHOST);
		}
	}
	else
		SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,4,bright,curSpr,DISPLAY_DRAWME);

}

void InstaRenderMonster(int x,int y,byte type,char bright,MGLDraw *mgl)
{
	sprite_t *curSpr;
	int v;

	// load if not loaded
	LoadMySprite(type);

	v=monsType[type].anim[ANIM_IDLE][0];
	if(!(monsType[type].flags&MF_ONEFACE))
		v+=2*monsType[type].framesPerDir;

	curSpr=monsType[type].spr->GetSprite(v);
	if(!curSpr)
		return;

	curSpr->DrawBright(x,y,mgl,bright);
}

void InstaRenderMonster2(int x,int y,byte type,byte seq,byte frm,byte facing,byte mind1,char bright,MGLDraw *mgl)
{
	sprite_t *curSpr;
	int v;

	// load if not loaded
	LoadMySprite(type);

	v=monsType[type].anim[seq][frm];
	if(!(monsType[type].flags&MF_ONEFACE))
		v+=facing*monsType[type].framesPerDir;

	if(type==MONS_FAIRY || type==MONS_FAIRY2)
		v+=(6*8*(mind1-1));

	curSpr=monsType[type].spr->GetSprite(v);
	if(!curSpr)
		return;

	curSpr->DrawBright(x,y,mgl,bright);
}

// AI auxiliary functions to make it simple
//---------------------------------------------

void FaceGoodguy(Guy *me,Guy *goodguy)
{
	if(!goodguy)
		return;

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

int RangeToTarget(Guy *me,Guy *goodguy)
{
	if(!me || !goodguy)
		return 0;

	return abs(me->x-goodguy->x)+abs(me->y-goodguy->y);
}

// this version doesn't insta-face, it rotates toward the right facing, and it has much
// more leeway than the 16 pixels of the other (it's for bigger creatures)
inline void FaceGoodguy2(Guy *me,Guy *goodguy)
{
	int desired;
	int diff,dir;

	if(!goodguy)
		return;

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

// this is only used for The Thing's tentacles, to keep their flailing within quadrants
void FlailLock(Guy *me)
{
	byte parentangle;
	byte diff;
	char dir;

	if(!me->parent || me->parent->type==MONS_NONE)
		return;	// no good

	if(me->parent->type!=MONS_THINGTENT)
	{
		switch(me->mind)	// which quadrant of the tentacles is he?
		{
			case 0:	// lower left
				parentangle=6;
				break;
			case 1:	// lower right
				parentangle=2;
				break;
			case 2:	// upper right
				parentangle=14;
				break;
			case 3:	// upper left
				parentangle=10;
				break;
		}
	}
	else
	{
		parentangle=me->parent->facing;
	}

	if(parentangle>me->facing)
	{
		diff=parentangle-me->facing;
		if(diff>8)
		{
			dir=1;
			diff=16-diff;
		}
		else
			dir=-1;
	}
	else
	{
		diff=me->facing-parentangle;
		if(diff>8)
		{
			dir=-1;
			diff=16-diff;
		}
		else
			dir=1;
	}
	if(diff>2)
	{
		me->facing=(parentangle+dir*2)&15;
	}
}

void AI_Overworld(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int size;

	if(me->placed)	// placed enemies await bumping into player
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(48*FIXAMT))
			{
				// do whatever is required
				//void SendMessageToGame(byte msg,int content);
				return;
			}
		}
		return;
	}

	if(me->mind==0)		// when mind=0, singlemindedly lumber towards the player
	{
		if(goodguy)
		{
			if(me->type!=MONS_PEEPER && me->type!=MONS_BIGBAT)
				size=48*FIXAMT;
			else
				size=64*FIXAMT;

			if(RangeToTarget(me,goodguy)<size && map->FindGuy(me->mapx,me->mapy,5,goodguy))
			{
				// get him!
				MakeNormalSound(SND_ATTACKED);
				SendMessageToGame(MSG_BATTLE,me->type);
				return;
			}
			if(player.fairyOn==FAIRY_SCARY && RangeToTarget(me,goodguy)<250*FIXAMT)
			{
				FaceGoodguy(me,goodguy);
				me->facing=(8-me->facing)&7;
			}
			else
				FaceGoodguy(me,goodguy);

			me->dx=Cosine(me->facing*32)*3;
			me->dy=Sine(me->facing*32)*3;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
			if(MGL_random(64)==0)
			{
				me->mind=1;		// occasionally wander
				me->mind1=1;
			}
		}
		else
		{
			me->mind=1;	// if there's no goodguy, get random
			me->mind1=1;
		}
	}
	else if(me->mind==1)	// random wandering
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(48*FIXAMT) && map->FindGuy(me->mapx,me->mapy,5,goodguy))
			{
				// get him!
				MakeNormalSound(SND_ATTACKED);
				SendMessageToGame(MSG_BATTLE,me->type);
				return;
			}
		}
		if(!(me->mind1--))	// time to get a new direction
		{
			if((goodguy) && MGL_random(3)==0)
				me->mind=0;	// get back on track
			else
				me->facing=(byte)MGL_random(8);
			me->mind1=MGL_random(40)+1;
		}

		if(player.fairyOn==FAIRY_SCARY && RangeToTarget(me,goodguy)<250*FIXAMT)
		{
			FaceGoodguy(me,goodguy);
			me->facing=(8-me->facing)&7;
		}

		me->dx=Cosine(me->facing*32)*3;
		me->dy=Sine(me->facing*32)*3;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
}
// here be the AIs for each monster type
//--------------------------------------------------------------------------------------
void AI_Ptero(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	Guy *g;

	g=GetGoodguy();

	if(g==NULL)
	{
		// vanish if there is no goodguy to follow
		me->type=MONS_NONE;
		return;
	}

	if(me->reload)
		me->reload--;

	if(me->mind1)
		me->mind1--;

	if(me->ouch==4)
		MakeSound(SND_PTERODIE,me->x,me->y,SND_CUTOFF,1200);

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==3 && me->reload==0)
		{
			// shoot
			FaceGoodguy(me,goodguy);
			MakeSound(SND_PTEROSHOOT,me->x,me->y,SND_CUTOFF,600);
			FireBullet(me->x,me->y,me->facing*32,BLT_MINIFBALL);
			me->reload=25-SpellLevel()/2+2;
		}
		if(me->seq==ANIM_ATTACK)
		{
			Dampen(&me->dx,FIXAMT*2);
			Dampen(&me->dy,FIXAMT*2);
		}
		if(me->seq==ANIM_DIE)
		{
			BlowWigglySmoke(me->x,me->y,me->z,0);
		}
		return;
	}
	if(me->mind==0)	// standard combat mode
	{
		if(goodguy==NULL || goodguy->type==MONS_FRIENDLY || RangeToTarget(me,g)>200*FIXAMT)
		{
			// too far from goodguy, run home
			me->mind=1;
		}
		else
		{
			if(RangeToTarget(me,goodguy)>300*FIXAMT)	// too far to fight, go home
			{
				me->mind=1;
			}
			else
			{
				if(me->mind1==0)
				{
					FaceGoodguy2(me,goodguy);
					me->mind1=2;
				}
				if(me->reload==0 && RangeToTarget(me,goodguy)<128*FIXAMT)
				{
					// don't have to stop to shoot
					MakeSound(SND_PTEROSHOOT,me->x,me->y,SND_CUTOFF,600);
					FireBullet(me->x,me->y,me->facing*32,BLT_MINIFBALL);
					me->reload=10-SpellLevel()/5+4;
				}
			}
		}
	}
	else	// running home mode
	{
		if(RangeToTarget(me,g)<128*FIXAMT && goodguy && RangeToTarget(me,goodguy)<300*FIXAMT)
		{
			// got close enough, get violent again
			me->mind=0;
		}
		if(me->mind1==0)
		{
			FaceGoodguy2(me,g);
			me->mind1=2;
		}
	}
	me->frmAdvance=256;
	me->dx+=Cosine(me->facing*32);
	me->dy+=Sine(me->facing*32);
	Clamp(&me->dx,FIXAMT*10);
	Clamp(&me->dy,FIXAMT*10);
}

void AI_Bonehead(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SKELOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SKELDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==3 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			if(me->AttackCheck(16,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				goodguy->GetShot(Cosine(me->facing*32)*4,Sine(me->facing*32)*4,10,map,world);
			me->reload=5;
		}
		if(me->seq==ANIM_A1 && me->frm==3 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			FireBullet(x,y,me->facing*32,BLT_ENERGY);
			me->reload=5;
			me->mind1=1;
		}
		return;	// can't do nothin' right now
	}

	// randomly decide to point at Bouapha to unnerve him
	// (but only if in pursuit mode, because otherwise you'd point the wrong way)
	if((!MGL_random(100)) && me->mind==0)
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

	if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(48*FIXAMT) && MGL_random(8)==0)
			{
				// get him!
				MakeSound(SND_SKELKICK,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=64;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=0;
				return;
			}
			FaceGoodguy(me,goodguy);

			me->dx=Cosine(me->facing*32)*4;
			me->dy=Sine(me->facing*32)*4;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
			if(MGL_random(64)==0)
			{
				me->mind=1;		// occasionally wander
				me->mind1=1;
			}
		}
		else
		{
			me->mind=1;	// if there's no goodguy, get random
			me->mind1=1;
		}
	}
	else if(me->mind==1)	// random wandering
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(512*FIXAMT) && MGL_random(32)==0)
			{
				// spit at him
				MakeSound(SND_SKELSHOOT,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_A1;
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
		}
		if(!(me->mind1--))	// time to get a new direction
		{
			if((goodguy) && MGL_random(3)==0)
				me->mind=0;	// get back on track
			else
				me->facing=(byte)MGL_random(8);
			me->mind1=MGL_random(40)+1;
		}

		me->dx=Cosine(me->facing*32)*4;
		me->dy=Sine(me->facing*32)*4;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
}

void AI_Bat(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_BATOUCH,me->x,me->y,SND_CUTOFF,1200);
	}
	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==2 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			if(me->AttackCheck(16,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				goodguy->GetShot(Cosine(me->facing*32)*4,Sine(me->facing*32)*4,2,map,world);
			me->reload=2;

		}
		if(me->seq==ANIM_A1 && me->frm>1 && goodguy)
		{
			// diving attack hit check
			x=me->x;
			y=me->y;
			if(me->AttackCheck(16,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(Cosine(me->facing*32)*8,Sine(me->facing*32)*8,6,map,world);
				me->seq=ANIM_A2;	// bounce off
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=64;
				me->dx=-me->dx/4;
				me->dy=-me->dy/4;
			}
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly zip towards Bouapha
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(48*FIXAMT) && MGL_random(4)==0)
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
				return;
			}
			if(RangeToTarget(me,goodguy)>(64*FIXAMT) && MGL_random(32)==0)
			{
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

			FaceGoodguy(me,goodguy);

			me->dx=Cosine(me->facing*32)*8;
			me->dy=Sine(me->facing*32)*8;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
			if(MGL_random(16)==0)
			{
				me->mind=1;		// occasionally wander
				me->mind1=1;
			}
		}
		else
		{
			me->mind=1;	// if there's no goodguy, get random
			me->mind1=1;
		}
	}
	else if(me->mind==1)	// random wandering
	{
		if(!(me->mind1--))	// time to get a new direction
		{
			if((goodguy) && MGL_random(3)==0)
				me->mind=0;	// get back on track
			else
				me->facing=(byte)MGL_random(8);
			me->mind1=MGL_random(40)+1;
		}

		me->dx=Cosine(me->facing*32)*6;
		me->dy=Sine(me->facing*32)*6;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=110;
		}
	}
}

void AI_Spider(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_SPD1DIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==2 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*8;
			y=me->y+Sine(me->facing*32)*8;
			if(me->AttackCheck(16,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				goodguy->GetShot(Cosine(me->facing*32)*2,Sine(me->facing*32)*2,1,map,world);
			me->reload=2;

		}
		return;	// can't do nothin' right now
	}

	if(goodguy)
	{
		if(RangeToTarget(me,goodguy)<(32*FIXAMT) && MGL_random(4)==0)
		{
			FaceGoodguy(me,goodguy);
			// get him!
			MakeSound(SND_SPD1GNAW,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=255;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=0;
			return;
		}
	}

	if(me->mind1)	// if you bump a wall, go somewhere else
	{
		me->mind1=0;
		me->mind=0;
	}
	if(me->mind)
		me->mind--;
	if(!me->mind)	// time to get a new direction
	{
		me->facing=(byte)MGL_random(8);
		me->mind=MGL_random(40)+1;
	}

	me->dx=Cosine(me->facing*32)*8;
	me->dy=Sine(me->facing*32)*8;
	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}

void AI_BigSpider(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SPD2OUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SPD2DIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==6 && me->reload==0)
		{
			// spit acid glob
			MakeSound(SND_SPD2SPIT,me->x,me->y,SND_CUTOFF,1200);
			x=me->x+Cosine(me->facing*32)*8;
			y=me->y+Sine(me->facing*32)*8;
			FireBullet(x,y,me->facing*32,BLT_ACID);
			me->reload=40;
		}
		return;	// can't do nothin' right now
	}

	if(goodguy)
	{
		if(RangeToTarget(me,goodguy)<(256*FIXAMT) && MGL_random(32)==0 && me->reload==0)
		{
			FaceGoodguy(me,goodguy);
			// spit on him!
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

	if(me->mind)
		me->mind--;
	if(!me->mind)	// time to get a new direction
	{
		me->facing=(byte)MGL_random(8);
		me->mind=MGL_random(120)+1;
	}

	me->dx=Cosine(me->facing*32)*4;
	me->dy=Sine(me->facing*32)*4;
	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}

void AI_Zombie(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_ZOMBIEOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_ZOMBIEDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==5)
		{
			me->dx=Cosine(me->facing*32)*8;
			me->dy=Sine(me->facing*32)*8;
		}
		if(me->seq==ANIM_ATTACK && me->frm>6)
		{
			Dampen(&me->dx,FIXAMT/2);
			Dampen(&me->dy,FIXAMT/2);
		}
		if(me->seq==ANIM_ATTACK && me->frm>=7 && me->frm<13 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			if(me->AttackCheck(8,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(Cosine(me->facing*32)*4,Sine(me->facing*32)*4,10,map,world);
				me->reload=2;
			}
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(128*FIXAMT) && MGL_random(32)==0)
			{
				// get him!
				MakeSound(SND_ZOMBIELEAP,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=64;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=0;
				return;
			}

			FaceGoodguy(me,goodguy);

			me->dx=Cosine(me->facing*32)*1;
			me->dy=Sine(me->facing*32)*1;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=64;
			}
			if(MGL_random(64)==0)
			{
				me->mind=1;		// occasionally wander
				me->mind1=1;
			}
		}
		else
		{
			me->mind=1;	// if there's no goodguy, get random
			me->mind1=1;
		}
	}
	else if(me->mind==1)	// random wandering
	{
		if(!(me->mind1--))	// time to get a new direction
		{
			if((goodguy) && MGL_random(3)==0)
				me->mind=0;	// get back on track
			else
				me->facing=(byte)MGL_random(8);
			me->mind1=MGL_random(40)+1;
		}

		me->dx=Cosine(me->facing*32)*1;
		me->dy=Sine(me->facing*32)*1;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=64;
		}
	}
}

void AI_EggSac(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	Guy *g;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_EGGSACDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==18 && me->reload==0)
		{
			g=AddGuy(me->x,me->y+FIXAMT*2,0,MONS_SPIDER);
			if(g && (!g->CanWalk(me->x,me->y,map,world)))
				g->type=MONS_NONE;	// vanish if this spot is occupied
			me->reload=200;
		}
		return;
	}

	if((!me->reload) && MGL_random(500)==0)
	{
		MakeSound(SND_EGGSACBIRTH,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=64;
		me->action=ACTION_BUSY;
	}
}

void AI_MamaSpider(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;
	byte f;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SPD3OUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SPD3DIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			if(MGL_random(2))
				AddGuy(me->x+((-8+MGL_random(17))<<FIXSHIFT),
						me->y+((-8+MGL_random(17))<<FIXSHIFT),0,MONS_SPIDER);
		}
		if(me->seq==ANIM_ATTACK && me->frm>4 && me->frm<8 && me->reload==0)
		{
			// only actually fire if the range is reasonable to avoid creating excess bullets
			if(goodguy && RangeToTarget(me,goodguy)<700*FIXAMT)
			{
				for(i=0;i<3;i++)
				{
					// spit acid glob
					x=me->x+Cosine(me->facing*32)*20-FIXAMT*4+(MGL_random(9)<<FIXSHIFT);
					y=me->y+Sine(me->facing*32)*20-FIXAMT*4+(MGL_random(9)<<FIXSHIFT);
					f=me->facing*32-16+MGL_random(33);

					FireBullet(x,y,f,BLT_ACID);
				}
			}
			me->reload=0;
		}
		if(me->seq==ANIM_A1 && me->frm==2 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*32;
			y=me->y+Sine(me->facing*32)*32;
			if(me->AttackCheck(32,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(Cosine(me->facing*32)*12,Sine(me->facing*32)*12,20,map,world);
				me->reload=2;
			}
		}
		return;	// can't do nothin' right now
	}

	if(goodguy)
	{
		x=RangeToTarget(me,goodguy);
		if(x<(512*FIXAMT) && x>128*FIXAMT && MGL_random(32)==0 && me->reload==0)
		{
			MakeSound(SND_SPD3SPEW,me->x,me->y,SND_CUTOFF,1200);
			FaceGoodguy(me,goodguy);
			// spit on him!
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=1;
			return;
		}
		else if(x<(128*FIXAMT) && MGL_random(16)==0 && me->reload==0)
		{
			MakeSound(SND_SPD3CHOMP,me->x,me->y,SND_CUTOFF,1200);
			FaceGoodguy(me,goodguy);
			// bite him
			me->seq=ANIM_A1;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=1;
			return;
		}
	}

	if(me->mind)
		me->mind--;
	if(!me->mind)	// time to get a new direction
	{
		me->facing=(byte)MGL_random(8);
		me->mind=MGL_random(120)+1;
	}

	me->dx=Cosine(me->facing*32)*3;
	me->dy=Sine(me->facing*32)*3;
	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}

void AI_Pygmy(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_PYGMYOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_PYGMYDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==6 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			if(me->AttackCheck(16,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(Cosine(me->facing*32)*6,Sine(me->facing*32)*6,8,map,world);
				me->reload=10;
			}
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)	// not currently aware of goodguy
	{
		me->frmAdvance=32;	// overcome the auto-128 frmAdvance of guy.cpp

		if(me->seq==ANIM_MOVE)
		{
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=32;
		}
		if(me->mind1<101)
			me->mind1++;

		if(me->mind1>100 && MGL_random(200)==0)
		{
			switch(MGL_random(3))
			{
				case 0:
					MakeSound(SND_PYGMYSPIN,me->x,me->y,SND_CUTOFF,1200);
					me->seq=ANIM_A1;	// spin the spear
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=64;
					me->mind1=0;
					me->action=ACTION_BUSY;
					break;
				case 1:
					MakeSound(SND_PYGMYYAWN,me->x,me->y,SND_CUTOFF,1200);
					me->seq=ANIM_A2;	// yawn
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=64;
					me->mind1=0;
					me->action=ACTION_BUSY;
					break;
				case 2:
					me->facing=(me->facing+(-1+MGL_random(3)))&7;	//change facing
					me->mind1=0;
					break;
			}
		}
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<256*FIXAMT || me->ouch>0)
			{
				// if the goodguy is near, or he shot me
				me->mind=1;	// start hunting
				if(me->ouch==0)
					me->mind1=60;	// for 2 seconds minimum
				else
					me->mind1=120;	// 4 seconds, because they HATE getting shot
				FaceGoodguy(me,goodguy);

				// do angry animation
				me->action=ACTION_BUSY;
				me->seq=ANIM_A3;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=255;	// very fast
				MakeSound(SND_PYGMYANGRY,me->x,me->y,SND_CUTOFF,1200);
			}
		}
		me->dx=0;
		me->dy=0;
	}
	else
	{
		FaceGoodguy(me,goodguy);
		if(!goodguy)
		{
			me->mind=0;	// go back to being bored, nothing to see here
			me->mind1=0;
			return;
		}
		if(RangeToTarget(me,goodguy)<(72*FIXAMT) && MGL_random(16)==0 && me->reload==0)
		{
			// spear him!
			MakeSound(SND_PYGMYSTAB,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=255;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=1;
			return;
		}

		if(me->ouch && me->mind1<240)
			me->mind1+=10;
		if(me->mind1)
			me->mind1--;
		else
		{
			if(RangeToTarget(me,goodguy)>256*FIXAMT)
				me->mind=0;	// get bored again
			else
				me->mind1=20;	// stay on trail a little longer
			return;
		}
		me->dx=Cosine(me->facing*32)*4;
		me->dy=Sine(me->facing*32)*4;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
}

void AI_Serpent(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SERPENTOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SERPENTDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==6 && me->reload==0 && goodguy)
		{
			// spit stuff
			x=me->x+Cosine(me->facing*32)*8;
			y=me->y+Sine(me->facing*32)*8;
			FireBullet(x,y,me->facing*32,BLT_ENERGY);
			FireBullet(x,y,(me->facing*32+4)&255,BLT_ENERGY);
			FireBullet(x,y,(me->facing*32-4)&255,BLT_ENERGY);
			me->reload=15;
			me->mind=0;
		}
		return;	// can't do nothin' right now
	}

	if(goodguy && RangeToTarget(me,goodguy)<500*FIXAMT)
		WaterRipple((me->x+me->dx*2)/FIXAMT,(me->y+me->dy*2)/FIXAMT,8*MGL_random(32));

	if(goodguy)
	{
		if(RangeToTarget(me,goodguy)<(512*FIXAMT) && MGL_random(16)==0 && me->reload==0)
		{
			MakeSound(SND_SERPENTSPIT,me->x,me->y,SND_CUTOFF,1200);
			FaceGoodguy(me,goodguy);
			// spit on him!
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

	if(me->mind)
		me->mind--;
	if(!me->mind)	// time to get a new direction
	{
		me->facing=(byte)MGL_random(8);
		me->mind=MGL_random(60)+1;
	}

	me->dx=Cosine(me->facing*32)*2;
	me->dy=Sine(me->facing*32)*2;
	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
	if((world->terrain[map->map[me->mapx+me->mapy*map->width].floor].flags&(TF_WATER|TF_LAVA))==0)
		me->GetShot(0,0,20,map,world);
}

void AI_Friendly(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(goodguy==NULL)
		return;

	// sometimes tap foot
	if(goodguy->type==MONS_BOUAPHA && RangeToTarget(me,goodguy)<32*FIXAMT)
	{
		if(me->mind1==0)
		{
			// talk to him!
			switch(player.worldNum*50+player.levelNum)
			{
				case 1:
					InitSpeech(0);
					break;
				case 10:
					InitSpeech(1);
					break;
				case 11:
					InitSpeech(2);
					break;
				case 13:
					InitSpeech(3);
					break;
				case 55:
					InitSpeech(7);
					break;
				case 51:
					InitSpeech(5);
					break;
				case 111:
					InitSpeech(10);
					break;
				case 116:
					InitSpeech(12);
					break;
			}
			me->mind1=1;
		}
	}
	else if(RangeToTarget(me,goodguy)>64*FIXAMT)
	{
		me->mind1=0;	// once the player has moved away, any nearness again will trigger talking
	}
	me->seq=me->mind;
	me->frm=0;
	me->frmTimer=0;
	me->frmAdvance=1;
}

void AI_Shroomlord(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i,j;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_GINGEROUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_GINGERDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm>=6 && me->frm<=8 && me->reload==0)
		{
			i=(me->facing*32-32+me->frm)&255;
			if(i<0)
				i+=256;
			for(j=-32+me->frm;j<33;j+=me->frm*2)
			{
				x=me->x+Cosine(i)*32;
				y=me->y+Sine(i)*24;
				FireExactBullet(x,y,FIXAMT*20,Cosine(i)*8,Sine(i)*8,0,0,16,i,BLT_SPORE);
				i=(i+me->frm*2)&255;
			}
			me->reload=1;
		}
		if(me->seq==ANIM_DIE && me->frm==1 && me->frmTimer<64)
		{
			AddGuy(me->x+(32<<FIXSHIFT),me->y,0,MONS_SHROOM);
			AddGuy(me->x-(32<<FIXSHIFT),me->y,0,MONS_SHROOM);
			for(i=0;i<30;i++)
				BlowWigglySmoke(me->x-32*FIXAMT+MGL_randoml(64*FIXAMT),
								me->y-32*FIXAMT+MGL_randoml(64*FIXAMT),
								MGL_random(64)<<FIXSHIFT,MGL_randoml(FIXAMT*4));
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, charge towards Bouapha
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(128*FIXAMT) && MGL_random(8)==0)
			{
				// get him!
				MakeSound(SND_GINGERCOMBO,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				return;
			}
			FaceGoodguy2(me,goodguy);

			me->dx=Cosine(me->facing*32)*4;
			me->dy=Sine(me->facing*32)*4;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
			if(MGL_random(256)==0)
			{
				me->mind=1;		// occasionally rest
				me->mind1=(byte)MGL_random(100);
			}
		}
		else
		{
			me->mind=1;	// if there's no goodguy, sit around
			me->mind1=(byte)MGL_random(100);
		}
	}
	else if(me->mind==1)	// random wandering
	{
		if(goodguy && RangeToTarget(me,goodguy)<(128*FIXAMT) && MGL_random(8)==0)
		{
			// get him!
			MakeSound(SND_GINGERCOMBO,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			return;
		}
		FaceGoodguy2(me,goodguy);

		if(!(me->mind1--))	// time to get a new direction
		{
			me->mind=0;	// get back on track
		}

		me->dx=0;
		me->dy=0;
		if(me->seq!=ANIM_IDLE)
		{
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=64;
		}
	}
}

void AI_Microfish(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			ExplodeParticles(PART_SHORTYELLOW,me->x,me->y,me->z,2);
		}
		return;	// can't do nothin' right now
	}

	me->dx=Cosine(me->facing*32)*2;
	me->dy=Sine(me->facing*32)*2;

	if(me->mind==0)
	{
		me->mind=MGL_random(10)+1;
		me->facing=(byte)MGL_random(8);
	}
	else
		me->mind--;

}

void AI_Moss(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	Guy *baby;

	if(me->reload)
		me->reload--;

	if(me->mind)
		me->mind--;

	if(me->action==ACTION_BUSY)
	{
		if(me->frm==4)
		{
			x=((me->x>>FIXSHIFT)/TILE_WIDTH);
			y=((me->y>>FIXSHIFT)/TILE_HEIGHT);
			me->x=(x*TILE_WIDTH+TILE_WIDTH/2)<<FIXSHIFT;
			me->y=(y*TILE_HEIGHT+TILE_HEIGHT/2)<<FIXSHIFT;
			me->dx=0;
			me->dy=0;	// you are at your destination
			me->reload=MGL_random(200)+55;

			// this prevents overlapping ones, in cases of ones getting born near-simultaneously
			me->type=MONS_NONE;	// so it doesn't get counted in the moss check
			if(MossCheck(x,y))
				return;	// with type=MONS_NONE, so it is dead
			me->type=MONS_MOSS;
		}
		return;
	}

	// if goodguy steps on you, hurt him
	if(goodguy && (!me->mind) && RangeToTarget(me,goodguy)<20*FIXAMT)
	{
		goodguy->GetShot(0,0,4,map,world);
		me->mind=5;	// so as not to hurt him too often
	}
	if(me->reload==0)
	{
		me->reload=MGL_random(200)+55;
		// spawn
		x=(me->x>>FIXSHIFT)/TILE_WIDTH;
		y=(me->y>>FIXSHIFT)/TILE_HEIGHT;
		switch(MGL_random(4))
		{
			case 0:
				// left
				if(x>0 && map->map[x-1+y*map->width].wall==0 &&
					(world->terrain[map->map[x-1+y*map->width].floor].flags&(TF_WATER|TF_LAVA|TF_SOLID))==0
					&& map->map[x-1+y*map->width].item<MAX_WALKABLE_ITMS
					&& (!MossCheck(x-1,y)))
				{
					baby=AddGuy(me->x,me->y,0,MONS_MOSS);
					if(baby)
					{
						baby->action=ACTION_BUSY;
						baby->seq=ANIM_ATTACK;
						baby->frm=0;
						baby->frmTimer=0;
						baby->frmAdvance=64;
						baby->dx=-(32*FIXAMT)/20;
						baby->dy=0;
					}
				}
				break;
			case 1:
				// right
				if(x<map->width-1 && map->map[x+1+y*map->width].wall==0 &&
					(world->terrain[map->map[x+1+y*map->width].floor].flags&(TF_WATER|TF_LAVA|TF_SOLID))==0
					&& map->map[x+1+y*map->width].item<MAX_WALKABLE_ITMS
					&& (!MossCheck(x+1,y)))
				{
					baby=AddGuy(me->x,me->y,0,MONS_MOSS);
					if(baby)
					{
						baby->action=ACTION_BUSY;
						baby->seq=ANIM_ATTACK;
						baby->frm=0;
						baby->frmTimer=0;
						baby->frmAdvance=64;
						baby->dx=(32*FIXAMT)/20;
						baby->dy=0;
					}
				}
				break;
			case 2:
				// up
				if(y>0 && map->map[x+(y-1)*map->width].wall==0 &&
					(world->terrain[map->map[x+(y-1)*map->width].floor].flags&(TF_WATER|TF_LAVA|TF_SOLID))==0
					&& map->map[x+(y-1)*map->width].item<MAX_WALKABLE_ITMS
					&& (!MossCheck(x,y-1)))
				{
					baby=AddGuy(me->x,me->y,0,MONS_MOSS);
					if(baby)
					{
						baby->action=ACTION_BUSY;
						baby->seq=ANIM_MOVE;
						baby->frm=0;
						baby->frmTimer=0;
						baby->frmAdvance=64;
						baby->dx=0;
						baby->dy=-(24*FIXAMT)/20;
					}
				}
				break;
			case 3:
				// down
				if(y<map->height-1 && map->map[x+(y+1)*map->width].wall==0 &&
					(world->terrain[map->map[x+(y+1)*map->width].floor].flags&(TF_WATER|TF_LAVA|TF_SOLID))==0
					&& map->map[x+(y+1)*map->width].item<MAX_WALKABLE_ITMS
					&& (!MossCheck(x,y+1)))
				{
					baby=AddGuy(me->x,me->y,0,MONS_MOSS);
					if(baby)
					{
						baby->action=ACTION_BUSY;
						baby->seq=ANIM_MOVE;
						baby->frm=0;
						baby->frmTimer=0;
						baby->frmAdvance=64;
						baby->dx=0;
						baby->dy=(24*FIXAMT)/20;
					}
				}
				break;
		}
	}
}

void AI_MossGrande(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	Guy *baby;

	if(me->reload)
		me->reload--;

	if(me->mind1)
		me->mind1--;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==11 && me->mind1==0)
		{
			for(x=0;x<16;x++)
				FireBullet(me->x,me->y,x*16,BLT_ENERGY);	// ring of fire
			me->mind1=60;
		}
		return;
	}

	if(me->seq==ANIM_IDLE)
		me->frmAdvance=MGL_random(200)+1;	// animate idle at random speeds

	if(!me->mind1 && MGL_random(100)==0)
	{
		me->mind1=0;
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
	}
	if((me->reload==0) && MGL_random(200)==0)
	{
		me->reload=255;
		// spawn in all four directions at once
		x=(me->x>>FIXSHIFT)/TILE_WIDTH;
		y=(me->y>>FIXSHIFT)/TILE_HEIGHT;

		// left
		if(x>0 && map->map[x-1+y*map->width].wall==0 &&
			(world->terrain[map->map[x-1+y*map->width].floor].flags&(TF_WATER|TF_LAVA|TF_SOLID))==0
			&& map->map[x-1+y*map->width].item<MAX_WALKABLE_ITMS
			&& (!MossCheck(x-1,y)))
		{
			baby=AddGuy(me->x,me->y,0,MONS_MOSS);
			if(baby)
			{
				baby->action=ACTION_BUSY;
				baby->seq=ANIM_ATTACK;
				baby->frm=0;
				baby->frmTimer=0;
				baby->frmAdvance=64;
				baby->dx=-(32*FIXAMT)/20;
				baby->dy=0;
			}
		}
		// right
		if(x<map->width-1 && map->map[x+1+y*map->width].wall==0 &&
			(world->terrain[map->map[x+1+y*map->width].floor].flags&(TF_WATER|TF_LAVA|TF_SOLID))==0
			&& map->map[x+1+y*map->width].item<MAX_WALKABLE_ITMS
			&& (!MossCheck(x+1,y)))
		{
			baby=AddGuy(me->x,me->y,0,MONS_MOSS);
			if(baby)
			{
				baby->action=ACTION_BUSY;
				baby->seq=ANIM_ATTACK;
				baby->frm=0;
				baby->frmTimer=0;
				baby->frmAdvance=64;
				baby->dx=(32*FIXAMT)/20;
				baby->dy=0;
			}
		}
		// up
		if(y>0 && map->map[x+(y-1)*map->width].wall==0 &&
			(world->terrain[map->map[x+(y-1)*map->width].floor].flags&(TF_WATER|TF_LAVA|TF_SOLID))==0
			&& map->map[x+(y-1)*map->width].item<MAX_WALKABLE_ITMS
			&& (!MossCheck(x,y-1)))
		{
			baby=AddGuy(me->x,me->y,0,MONS_MOSS);
			if(baby)
			{
				baby->action=ACTION_BUSY;
				baby->seq=ANIM_MOVE;
				baby->frm=0;
				baby->frmTimer=0;
				baby->frmAdvance=64;
				baby->dx=0;
				baby->dy=-(24*FIXAMT)/20;
			}
		}
		// down
		if(y<map->height-1 && map->map[x+(y+1)*map->width].wall==0 &&
			(world->terrain[map->map[x+(y+1)*map->width].floor].flags&(TF_WATER|TF_LAVA|TF_SOLID))==0
			&& map->map[x+(y+1)*map->width].item<MAX_WALKABLE_ITMS
			&& (!MossCheck(x,y+1)))
		{
			baby=AddGuy(me->x,me->y,0,MONS_MOSS);
			if(baby)
			{
				baby->action=ACTION_BUSY;
				baby->seq=ANIM_MOVE;
				baby->frm=0;
				baby->frmTimer=0;
				baby->frmAdvance=64;
				baby->dx=0;
				baby->dy=(24*FIXAMT)/20;
			}
		}
	}
}

void AI_Magmazoid(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SERPENTOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SERPENTDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(goodguy && RangeToTarget(me,goodguy)<500*FIXAMT)
		WaterRipple((me->x+me->dx*2)/FIXAMT,(me->y+me->dy*2)/FIXAMT,8*MGL_random(32));

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm>=6 && me->frm<=12 && me->reload==0 && goodguy)
		{
			// spit stuff
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			FireBullet(x,y,me->facing*32,BLT_FLAME2);
			me->reload=2;
			me->mind=0;
		}
		if(me->seq==ANIM_ATTACK && me->frm==13 && me->reload==0)
			me->reload=20;
		return;	// can't do nothin' right now
	}

	if(goodguy)
	{
		if(RangeToTarget(me,goodguy)<(512*FIXAMT) && MGL_random(16)==0 && me->reload==0)
		{
			FaceGoodguy(me,goodguy);
			// roast him!
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

	if(me->mind)
		me->mind--;
	if(!me->mind)	// time to get a new direction
	{
		me->facing=(byte)MGL_random(8);
		me->mind=MGL_random(60)+1;
	}

	me->dx=Cosine(me->facing*32)*2;
	me->dy=Sine(me->facing*32)*2;
	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}

void AI_Shroom(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int i;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		/*
		if(me->hp>0)
			MakeSound(SND_SKELOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SKELDIE,me->x,me->y,SND_CUTOFF,1200);
			*/
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==5 && me->reload==0 && goodguy)
		{
			for(i=0;i<256;i+=16)
				FireBullet(me->x,me->y,i,BLT_SPORE);
			me->reload=5;
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(72*FIXAMT) && MGL_random(8)==0)
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
			FaceGoodguy(me,goodguy);

			me->dx=Cosine(me->facing*32)*1;
			me->dy=Sine(me->facing*32)*1;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
			if(MGL_random(64)==0)
			{
				me->mind=1;		// occasionally wander
				me->mind1=1;
			}
		}
		else
		{
			me->mind=1;	// if there's no goodguy, get random
			me->mind1=1;
		}
	}
	else if(me->mind==1)	// random wandering
	{
		if(!(me->mind1--))	// time to get a new direction
		{
			if((goodguy) && MGL_random(3)==0)
				me->mind=0;	// get back on track
			else
				me->facing=(byte)MGL_random(8);
			me->mind1=MGL_random(40)+1;
		}

		me->dx=Cosine(me->facing*32)*1;
		me->dy=Sine(me->facing*32)*1;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
}

void AI_Mush(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int i,x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_MUSHOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_MUSHDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_A1 && me->frm>=1 && me->frm<=15 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*72;
			y=me->y+Sine(me->facing*32)*64;
			i=(me->facing*32-64)&255;
			x+=Cosine(i)*48;
			y+=Sine(i)*32;
			i=(me->facing*32-16+MGL_random(33))&255;
			FireExactBullet(x,y,FIXAMT*64,Cosine(i)*12,Sine(i)*12,0,0,16,i,BLT_SPORE);
			MakeSound(SND_MUSHSPORES,me->x,me->y,SND_CUTOFF,600);
			me->reload=1;
		}
		if(me->seq==ANIM_A1 && me->frm==16)
			me->frmAdvance=64;	// slow the animation for the recovery part
		if(me->seq==ANIM_ATTACK && me->frm==1 && me->reload==0)
		{
			x=me->x+Cosine(me->facing*32)*72;
			y=me->y+Sine(me->facing*32)*64;
			i=(me->facing*32+64)&255;
			x+=Cosine(i)*48;
			y+=Sine(i)*32;
			FireBullet(x,y,me->facing,BLT_SHROOM);
			MakeSound(SND_MUSHMISSILE,me->x,me->y,SND_CUTOFF,1000);
			me->reload=5;
		}
		if(me->seq==ANIM_DIE)
		{
			me->facing=2;	// die only works from forward (memory saver)
			if(me->frm>=3)
			{
				x=me->x>>FIXSHIFT;
				y=me->y>>FIXSHIFT;
				BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
				BlowSmoke((x+me->rectx+MGL_random(me->rectx2-me->rectx))<<FIXSHIFT,
						  (y+me->recty+MGL_random(me->recty2-me->recty))<<FIXSHIFT,
						  me->z,FIXAMT);
			}
		}

		return;	// can't do nothin' right now
	}

	if(me->mind==0)	// this mode is only used prior to him seeing Bouapha the first time
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<256*FIXAMT || me->ouch>0)
			{
				// get mad!
				MakeSound(SND_MUSHMAD,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_A2;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=0;
				me->mind=2;		// destroy bouapha
				me->facing=2;	// angry animation ONLY works from forward (memory saver)
			}
		}
	}
	else if(me->mind==2)		// when mind=2, hold still and go off on Bouapha
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(256*FIXAMT) && MGL_random(32)==0)
			{
				// get him! (fire shroom cannon)
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
			if(RangeToTarget(me,goodguy)<(256*FIXAMT) && MGL_random(28)==0)
			{
				// get him! (fire sporegun)
				me->seq=ANIM_A1;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=256;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=0;
				return;
			}
			FaceGoodguy2(me,goodguy);

			me->dx=0;
			me->dy=0;
			if(me->seq!=ANIM_IDLE)
			{
				me->seq=ANIM_IDLE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
			if(RangeToTarget(me,goodguy)>(256*FIXAMT))
			{
				// chase him down!
				me->mind=1;
			}
		}
		else
		{
			// just sit there
		}
	}
	else if(me->mind==1)	// chase down Bouapha
	{
		if(goodguy)
		{
			FaceGoodguy2(me,goodguy);

			me->dx=Cosine(me->facing*32)*8;
			me->dy=Sine(me->facing*32)*8;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
			if(RangeToTarget(me,goodguy)<200*FIXAMT)
				me->mind=2;	// in range, start killin'
		}
		else
			me->mind=0;
	}
}

void AI_TheThing(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_MUSHOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_MUSHDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			if(me->frm==0)
				KillKids(me);	// destroy the tentacles
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,2);
			BlowSmoke((x+me->rectx+MGL_random(me->rectx2-me->rectx))<<FIXSHIFT,
					  (y+me->recty+MGL_random(me->recty2-me->recty))<<FIXSHIFT,
					  me->z,FIXAMT);
			me->x+=((1-MGL_random(3))<<FIXSHIFT);
			me->y+=((1-MGL_random(3))<<FIXSHIFT);
			if(me->frm==14 && me->frmTimer<32)
			{
				ShowVictoryAnim(0);	// you killed the final boss
				SendMessageToGame(MSG_WINGAME,0);
			}
		}
		if(me->seq==ANIM_ATTACK && me->frm==3 && me->frmTimer<32)
		{
			//g=AddGuy(me->x,me->y,0,MONS_BABYTHING);
		}
		return;	// can't do nothin' right now
	}
	if(MGL_random(128)==0)	// blink
	{
		me->action=ACTION_BUSY;
		me->seq=ANIM_A1;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
	if(me->reload==0 && MGL_random(16)==0)
	{
		MakeSound(SND_MUSHMISSILE,me->x,me->y,SND_CUTOFF,1500);
		FireBullet(me->x-48*FIXAMT,me->y-10*FIXAMT,MGL_random(88)+20,BLT_GRENADE);
		FireBullet(me->x+48*FIXAMT,me->y-10*FIXAMT,MGL_random(88)+20,BLT_GRENADE);
		me->reload=30;
	}
	if(MGL_random(128)==0)	// tongue
	{
		MakeSound(SND_THINGVOMIT,me->x,me->y,SND_CUTOFF,1500);
		me->action=ACTION_BUSY;
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}

void AI_ThingTentacle(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	Guy *g;

	if(me->reload)
		me->reload--;

	if(me->mind1<128)
		me->mind1++;
	if(me->mind1>128)
		me->mind1--;

	if((!me->parent) || me->parent->type==MONS_NONE)
	{
		me->type=MONS_NONE;
		return;
	}

	if(me->ouch==4)
	{
		/*
		if(me->hp>0)
			MakeSound(SND_SKELOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SKELDIE,me->x,me->y,SND_CUTOFF,1200);
			*/
	}

	if(me->parent->type==MONS_THINGTENT)
	{
		// send damage up and down the list
		if(me->hp>me->parent->hp)
		{
			me->hp=me->parent->hp;
			me->ouch=4;
		}
		else if(me->hp<me->parent->hp)
		{
			me->parent->hp=me->hp;
			me->parent->ouch=4;
		}
		if(me->parent->hp==0 && me->seq!=ANIM_DIE)
		{
			me->seq=ANIM_DIE;
			me->action=ACTION_BUSY;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=32;
		}
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
			BlowSmoke((x+me->rectx+MGL_random(me->rectx2-me->rectx))<<FIXSHIFT,
					  (y+me->recty+MGL_random(me->recty2-me->recty))<<FIXSHIFT,
					  me->z,FIXAMT);
			me->x+=((1-MGL_random(3))<<FIXSHIFT);
			me->y+=((1-MGL_random(3))<<FIXSHIFT);
			me->facing=(me->facing+1-MGL_random(3))&15;
			FlailLock(me);
			if(me->frm==4 && me->parent->type==MONS_THINGTENT)	// kill parent (unless it's The Thing)
			{
				me->parent->hp=0;
				me->parent->seq=ANIM_DIE;
				me->parent->frm=0;
				me->parent->frmTimer=0;
				me->parent->frmAdvance=64;
				me->parent->action=ACTION_BUSY;
			}
			g=me->parent;
			while(g && g->type==MONS_THINGTENT)
			{
				// make the whole thing flail
				g->facing=(g->facing+1-MGL_random(3))&15;
				FlailLock(g);
				g=g->parent;
			}
			if(me->parent->type==MONS_THINGTENT)
			{
				me->x=me->parent->x+Cosine(me->parent->facing*16)*64;
				me->y=me->parent->y+Sine(me->parent->facing*16)*44;
			}
		}

		return;	// can't do nothin' right now
	}

	if(goodguy && me->AttackCheck(32,me->x>>FIXSHIFT,me->y>>FIXSHIFT,goodguy))
	{
		goodguy->GetShot(0,0,2,map,world);
	}
	if(me->parent->type==MONS_THINGTENT)
		me->mind1=me->parent->mind1;
	else
	{
		if(me->mind1==128)
		{
			me->mind1=128-20+MGL_random(41);
		}
	}
	if(MGL_random(64)==0)
	{
		me->mind1=128-20+MGL_random(41);
	}
	else
	{
		if(me->mind1<128)
			me->facing=(me->facing+1)&15;
		else if(me->mind1>128)
			me->facing=(me->facing-1)&15;
	}

	// keep tentacles within allowable angles
	FlailLock(me);

	if(me->parent->type==MONS_THINGTENT)
	{
		me->x=me->parent->x+Cosine(me->parent->facing*16)*64;
		me->y=me->parent->y+Sine(me->parent->facing*16)*44;
	}
}

void AI_StickMan(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int i;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_HAPPYOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_HAPPYDIE,me->x,me->y,SND_CUTOFF,1200);
		if(me->hp>0)
		{
			me->seq=ANIM_A2;	//sad
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=32;
		}
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==1 && me->reload==0 && goodguy)
		{
			if(RangeToTarget(me,goodguy)<60*FIXAMT)
				goodguy->GetShot(Cosine(me->facing*32)*16,Sine(me->facing*32)*16,10,map,world);
			me->reload=4;
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)	// waiting for Bouapha
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<300*FIXAMT || me->ouch)
			{
				MakeSound(SND_HAPPYFLEX,me->x,me->y,SND_CUTOFF,1500);
				me->action=ACTION_BUSY;
				me->seq=ANIM_A1;	// muscles
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=64;
				me->mind=1;
			}
		}
	}
	else if(me->mind==1)	// chase down Bouapha
	{
		if(goodguy)
		{
			i=RangeToTarget(me,goodguy);
			if(i<(60*FIXAMT) && MGL_random(4)==0 && me->reload==0)
			{
				// get him! (chomp)
				MakeSound(SND_HAPPYCHOMP,me->x,me->y,SND_CUTOFF,1500);
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
				me->dx=Cosine(me->facing*32)*2;
				me->dy=Sine(me->facing*32)*2;
				return;
			}
			FaceGoodguy2(me,goodguy);

			if(me->seq!=ANIM_A2)
			{
				me->dx=Cosine(me->facing*32)*8;
				me->dy=Sine(me->facing*32)*8;
			}
			else
			{
				me->dx=Cosine(me->facing*32)*2;
				me->dy=Sine(me->facing*32)*2;
			}
			if(me->seq!=ANIM_MOVE && me->seq!=ANIM_A2)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
			if(MGL_random(256)==0)
			{
				// show off
				MakeSound(SND_HAPPYFLEX,me->x,me->y,SND_CUTOFF,1500);
				me->seq=ANIM_A1;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=64;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=0;
			}
		}
		else
			me->mind=0;
	}
}

void AI_Penguin(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_PENGDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK)
		{
			if(me->frm<4)
			{
				me->dx=Cosine(me->facing*32)*7;
				me->dy=Sine(me->facing*32)*7;
				x=me->x+Cosine(me->facing*32)*10;
				y=me->y+Sine(me->facing*32)*10;
				if(me->AttackCheck(10,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				{
					goodguy->GetShot(Cosine(me->facing*32)*4,Sine(me->facing*32)*4,4,map,world);
					me->frm=4;	// bounce back
					me->frmTimer=0;
				}
			}
			else
			{
				me->dx=-Cosine(me->facing*32)*3;
				me->dy=-Sine(me->facing*32)*3;
			}
			me->reload=10;
		}

		if(me->seq==ANIM_DIE)
		{
			me->frmAdvance=128;
			me->facing=(me->facing+1)&7;
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==2)		// when mind=2, pursue relentlessly
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(128*FIXAMT))
			{
				me->mind=1;	// start circling
			}
			FaceGoodguy2(me,goodguy);

			me->dx=Cosine(me->facing*32)*4;
			me->dy=Sine(me->facing*32)*4;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=256;
			}
		}
		else
		{
			me->mind=0;	// if there's no goodguy, get random
			me->mind1=1;
		}
	}
	else if(me->mind==0)	// random wandering
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(128*FIXAMT))
			{
				me->mind=1;	// circling mode
			}
		}
		if(!(me->mind1--))	// time to get a new direction
		{
			me->facing=(byte)MGL_random(8);
			me->mind1=MGL_random(40)+1;
		}

		me->dx=Cosine(me->facing*32)*4;
		me->dy=Sine(me->facing*32)*4;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
		}
	}
	else if(me->mind==1)	// circling
	{
		FaceGoodguy2(me,goodguy);

		// move sideways
		me->dx=Cosine((me->facing*32+64)&255)*3;
		me->dy=Sine((me->facing*32+64)&255)*3;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
		}

		if(goodguy && RangeToTarget(me,goodguy)>128*FIXAMT)
		{
			me->mind=2;	// chase him down
		}

		if((!me->reload) && MGL_random(32)==0)
		{
			MakeSound(SND_PENGPECK,me->x,me->y,SND_CUTOFF,1000);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
		}
	}
}

void AI_Geozoid(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SERPENTOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SERPENTDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==6 && me->reload==0 && goodguy)
		{
			// spit rock
			x=me->x+Cosine(me->facing*32)*8;
			y=me->y+Sine(me->facing*32)*8;
			FireBullet(x,y,me->facing,BLT_ROCK);
			me->reload=20;
		}
		return;	// can't do nothin' right now
	}

	if(goodguy)
	{
		if(RangeToTarget(me,goodguy)<(512*FIXAMT) && MGL_random(8)==0 && me->reload==0)
		{
			MakeSound(SND_SERPENTSPIT,me->x,me->y,SND_CUTOFF,1200);
			// spit on him!
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

	FaceGoodguy2(me,goodguy);

	if(me->mind1)
	{
		if(!(--me->mind1))
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=64;
			// VERY slow inching along
			me->dx=Cosine(me->facing*32)/8;
			me->dy=Sine(me->facing*32)/8;
		}
		else
		{
			me->dx=0;
			me->dy=0;
		}
	}
	else
	{
		if(me->seq==ANIM_IDLE)
		{
			me->dx=0;
			me->dy=0;
			me->mind1=10+MGL_random(50);
		}
	}
}

void AI_Mumble(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->mind1)
		me->mind1--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_ZOMBIEOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_ZOMBIEDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==5 && me->reload==0 && goodguy)
		{
			// crush skull
			x=me->x+Cosine(me->facing*32)*20;
			y=me->y+Sine(me->facing*32)*20;
			if(me->AttackCheck(24,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(Cosine(me->facing*32)*10,Sine(me->facing*32)*10,12,map,world);
			}
			me->reload=5;
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)	// not yet aware
	{
		if(goodguy && RangeToTarget(me,goodguy)<200*FIXAMT)
		{
			me->mind=1;	// begin chasing.  Slowly.
		}
		if(me->seq==ANIM_IDLE)
		{
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
	else
	{
		if(RangeToTarget(me,goodguy)<(60*FIXAMT) && MGL_random(8)==0 && me->reload==0)
		{
			// crush his skull
			MakeSound(SND_MUMBLECRUSH,me->x,me->y,SND_CUTOFF,1000);
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
		if(!me->mind1)
		{
			// turns only once a second
			FaceGoodguy2(me,goodguy);
			me->mind1=30;
		}
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=64;
		}
		else
		{
			if(!((me->frm>=3 && me->frm<=5) || (me->frm>=9 && me->frm<=11)))
			{
				me->dx=Cosine(me->facing*32);
				me->dy=Sine(me->facing*32);
			}
			else
			{
				me->dx=0;
				me->dy=0;
			}
		}
	}
}

void AI_Cactus(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int i,x,y;
	byte b;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_CACTONDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE && me->frm>6)
		{
			me->frmAdvance=256;
		}
		if(me->seq==ANIM_ATTACK && me->frm==3)
		{
			// fire lots of spines
			MakeSound(SND_CACTONSHOOT,me->x,me->y,SND_CUTOFF,600);
			// but only fire if within a few hundred miles
			if(goodguy && RangeToTarget(me,goodguy)<1024*FIXAMT)
			{
				b=(me->facing*32-60);
				if(me->frmTimer>64)
					b+=5;

				for(i=0;i<12;i++)
				{
					x=me->x+Cosine(b)*32;
					y=me->y+Sine(b)*32;
					FireBullet(me->x,me->y,b,BLT_SPINE);
					b+=10;
				}
			}

		}
		if(me->seq==ANIM_A1)
		{
			// accelerate
			if(abs(me->dx)<abs(Cosine(me->facing*32)*6))
				me->dx+=Cosine(me->facing*32)/2;
			if(abs(me->dy)<abs(Sine(me->facing*32)*6))
				me->dy+=Sine(me->facing*32)/2;
		}
		if(me->seq==ANIM_A2)
		{
			// decelerate
			if(abs(me->dx)>0)
				me->dx-=Cosine(me->facing*32);
			if(abs(me->dy)>0)
				me->dy-=Sine(me->facing*32);
		}
		return;
	}

	if(me->mind==0)	//idling
	{
		if(me->mind1)
			me->mind1--;
		else
		{
			me->mind=1;	// start rolling somewhere
			me->facing=(byte)MGL_random(8);
			me->mind1=MGL_random(30)+30;
			me->action=ACTION_BUSY;
			me->seq=ANIM_A1;	// get into rolling animation
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
			return;
		}
		if(me->reload==0)
		{
			me->reload=120;
			if(goodguy)
				FaceGoodguy(me,goodguy);
			me->action=ACTION_BUSY;
			me->seq=ANIM_ATTACK;	// shoot some spines
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			return;
		}
		me->dx=0;
		me->dy=0;
		if(me->seq!=ANIM_IDLE)
		{
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
		}
		me->frmAdvance=32;

	}
	else if(me->mind==1)
	{
		if(me->mind1)
			me->mind1--;
		else
		{
			me->mind=0;	// got to the destination
			me->mind1=MGL_random(70)+30;
			me->action=ACTION_BUSY;
			me->seq=ANIM_A2;	// get out of rolling animation
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
			// drift to a stop
			me->dx=Cosine(me->facing*32);
			me->dy=Sine(me->facing*32);
			return;
		}
		if(abs(me->dx)<abs(Cosine(me->facing*32)*6))
			me->dx+=Cosine(me->facing*32)/2;
		if(abs(me->dy)<abs(Sine(me->facing*32)*6))
			me->dy+=Sine(me->facing*32)/2;

		if((me->seq!=ANIM_MOVE) || (me->seq==ANIM_MOVE && me->frm==4 && me->frmTimer>128))
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
		}
		me->frmAdvance=128;
	}
}

void AI_Roller(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	byte f;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		// make noise?
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			FireBullet(me->x+((32-MGL_random(65))<<FIXSHIFT),me->y+((32-MGL_random(65))<<FIXSHIFT),
				(byte)MGL_random(8),BLT_ROCK);
			FireBullet(me->x+((32-MGL_random(65))<<FIXSHIFT),me->y+((32-MGL_random(65))<<FIXSHIFT),
				(byte)MGL_random(8),BLT_ROCK);
			FireBullet(me->x+((32-MGL_random(65))<<FIXSHIFT),me->y+((32-MGL_random(65))<<FIXSHIFT),
				(byte)MGL_random(8),BLT_ROCK);
		}
		return;
	}

	if(goodguy && (me->AttackCheck(40,me->x>>FIXSHIFT,me->y>>FIXSHIFT,goodguy)))
	{
		// kill him
		goodguy->GetShot(0,0,128,map,world);
		MakeSound(SND_ROLYPOLYWALL,me->x,me->y,SND_CUTOFF,1000);
	}
	if(me->mind1)	// bonked a wall
	{
		me->mind1=0;
		me->facing=(me->facing+2)&7;
		MakeSound(SND_ROLYPOLYWALL,me->x,me->y,SND_CUTOFF,800);
	}

	switch(me->facing)
	{
		case 0:
			f=ANIM_A1;	// roll to the right
			break;
		case 2:
			f=ANIM_MOVE; // roll down
			break;
		case 4:
			f=ANIM_A2;	// roll to the left
			break;
		case 6:
			f=ANIM_ATTACK;	// roll up
			break;
	}
	if(me->seq!=f)
	{
		me->seq=f;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->dx=Cosine(me->facing*32)*4;
		me->dy=Sine(me->facing*32)*4;
	}
}

void AI_Lich(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	byte f;
	int x,y;

	if(!goodguy)
		return;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		// make noise?
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK)
		{
			// still chase, but slowly
			x=goodguy->x;
			y=goodguy->y-96*FIXAMT;

			FaceGoodguy2(me,goodguy);
			if(me->x<x)
				me->dx+=FIXAMT/8;
			if(me->y<y)
				me->dy+=FIXAMT/8;
			if(me->x>x)
				me->dx-=FIXAMT/8;
			if(me->y>y)
				me->dy-=FIXAMT/8;

			Clamp(&me->dx,FIXAMT*1);
			Clamp(&me->dy,FIXAMT*1);

			if(me->frm==1 && me->frmTimer<64)
			{
				// shoot laser
				x=me->x;
				y=me->y+FIXAMT*8;
				LaunchMegabeam(x,y,me->ID);
			}
		}

		if(me->seq==ANIM_DIE)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
			BlowSmoke((x+me->rectx+MGL_random(me->rectx2-me->rectx))<<FIXSHIFT,
					  (y+me->recty+MGL_random(me->recty2-me->recty))<<FIXSHIFT,
					  me->z,FIXAMT);
		}
		return;
	}

	if(me->mind==0)
	{
		if(goodguy && (RangeToTarget(me,goodguy)<400*FIXAMT || me->ouch>0))
		{
			me->mind=1;
		}
	}
	if(me->mind==1)	// active
	{
		if(goodguy)
		{
			if((me->AttackCheck(55,me->x>>FIXSHIFT,me->y>>FIXSHIFT,goodguy)) && (me->reload==0))
			{
				// ouch, Richie is poisonous!
				// this is to make up for the way you can stand in a position in which he can't hit you
				goodguy->GetShot(0,0,10,map,world);
				if(goodguy->type==MONS_BOUAPHA)
					PoisonPlayer(60);
			}

			// calculate desired location (want to be above Bouapha)
			x=goodguy->x;
			y=goodguy->y-96*FIXAMT;

			FaceGoodguy2(me,goodguy);
			if(me->x<x)
				me->dx+=FIXAMT/8;
			if(me->y<y)
				me->dy+=FIXAMT/8;
			if(me->x>x)
				me->dx-=FIXAMT/8;
			if(me->y>y)
				me->dy-=FIXAMT/8;

			Clamp(&me->dx,FIXAMT*3);
			Clamp(&me->dy,FIXAMT*4);
		}

		if(me->reload==8)
		{
			// left eye fires
			f=(me->facing*32-32+me->mind1)&255;
			// can't fire backwards
			if(f>128)
			{
				if(f<128+64)
					f=128;
				else
					f=0;
			}
			FireExactBullet(me->x-32*FIXAMT,me->y+32*FIXAMT,me->z+80*FIXAMT,
						Cosine(f)*8,Sine(f)*8,-3*FIXAMT,0,30,me->facing,BLT_ENERGY);
		}
		if(!me->reload)
		{
			// right eye fires
			f=(me->facing*32+32-me->mind1)&255;
			// can't fire backwards
			if(f>128)
			{
				if(f<128+64)
					f=128;
				else
					f=0;
			}
			FireExactBullet(me->x+32*FIXAMT,me->y+32*FIXAMT,me->z+80*FIXAMT,
						Cosine(f)*8,Sine(f)*8,-3*FIXAMT,0,30,me->facing,BLT_ENERGY);
			me->reload=16;
			me->mind1+=8;
			if(me->mind1>=64)
				me->mind1=0;
		}

		if(MGL_random(128)==0)
		{
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=64;
			me->action=ACTION_BUSY;
			return;
		}
	}

	if(me->seq!=ANIM_IDLE)
	{
		me->seq=ANIM_IDLE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}

void AI_DustDevil(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		// make noise?
	}

	if(me->mind1<255)
		me->mind1++;

	me->frmAdvance=128;	// always frame speed=128
	if(goodguy)
	{
		if(me->mind==0)
		{
			if(goodguy->x<me->x)
			{
				goodguy->dx+=(me->mind1*FIXAMT/128);
				goodguy->dy+=(me->mind1*FIXAMT/1024);
			}
			else
			{
				goodguy->dx-=(me->mind1*FIXAMT/128);
				goodguy->dy-=(me->mind1*FIXAMT/1024);
			}
			if(goodguy->y<me->y)
			{
				goodguy->dy+=(me->mind1*FIXAMT/128);
				goodguy->dx-=(me->mind1*FIXAMT/1024);
			}
			else
			{
				goodguy->dy-=(me->mind1*FIXAMT/128);
				goodguy->dx+=(me->mind1*FIXAMT/1024);
			}
		}
		else
		{
			// immediately after hitting him, shove him away
			if(goodguy->x<me->x)
				goodguy->dx-=FIXAMT*2;
			else
				goodguy->dx+=FIXAMT*2;
			if(goodguy->y<me->y)
				goodguy->dy-=FIXAMT*2;
			else
				goodguy->dy+=FIXAMT*2;
			if(me->mind1>10)
				me->mind=0;	// go back to sucking
		}
		Clamp(&goodguy->dx,FIXAMT*6);
		Clamp(&goodguy->dy,FIXAMT*6);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==5 && me->frmTimer<64 && (goodguy))
		{
			if(me->AttackCheck(80,me->x>>FIXSHIFT,me->y>>FIXSHIFT,goodguy))
			{
				FaceGoodguy(me,goodguy);
				goodguy->GetShot(Cosine(me->facing*32)*12,
								Sine(me->facing*32)*12,128/5,map,world);
				me->mind1=0;
				me->mind=1;	// push him away
			}
		}

		if(me->seq==ANIM_DIE)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
			BlowSmoke((x+me->rectx+MGL_random(me->rectx2-me->rectx))<<FIXSHIFT,
					  (y+me->recty+MGL_random(me->recty2-me->recty))<<FIXSHIFT,
					  me->z,FIXAMT);
		}
		return;
	}

	if(goodguy)
	{
		if(RangeToTarget(me,goodguy)<80*FIXAMT)
		{
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->action=ACTION_BUSY;
			return;
		}
	}
	if(me->seq!=ANIM_IDLE)
	{
		me->seq=ANIM_IDLE;
		me->frm=0;
		me->frmTimer=0;
	}
}

void AI_MechaBouapha(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->mind1)
		me->mind1--;

	if(me->ouch==4)
	{
		// make noise?
		if(me->mind1<64 && me->mind==1)
			me->mind1+=16;	// ouch
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==4 && me->frmTimer<64)
		{
			// fire a hammer
			FireBullet(me->x,me->y,me->facing,BLT_EVILHAMMER);
			me->reload=20+MGL_random(50);
		}
		if(me->seq==ANIM_ATTACK && me->frm==7 && me->frmTimer<64 && MGL_random(4)==0)
			me->facing=(byte)MGL_random(8);

		if(me->seq==ANIM_DIE)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
			BlowSmoke((x+me->rectx+MGL_random(me->rectx2-me->rectx))<<FIXSHIFT,
					  (y+me->recty+MGL_random(me->recty2-me->recty))<<FIXSHIFT,
					  me->z,FIXAMT);
		}
		if(me->seq==ANIM_A1 && me->frm==7)
		{
			me->facing=(byte)MGL_random(8);
		}
		return;
	}

	if(me->mind==0)	// unaware of Bouapha
	{
		if(goodguy && (RangeToTarget(me,goodguy)<256*FIXAMT || me->ouch>0))
		{
			me->seq=ANIM_A3;	// unmelt
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
			me->action=ACTION_BUSY;
			me->reload=5+MGL_random(30);
			me->mind=1;	// activate
		}
		else
		{
			me->seq=ANIM_A2;	// sit as a ball
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=32;
		}
	}
	else if(me->mind==1)	// normal mode
	{
		if(me->reload==0)	// time to fire
		{
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			if(goodguy)
				FaceGoodguy(me,goodguy);
			me->dx=0;
			me->dy=0;
			return;
		}
		if(me->mind1>32)	// time to run!
		{
			me->mind=2;	// running mode
			me->seq=ANIM_A1;	// melt
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			return;
		}
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->dx=Cosine(me->facing*32)*6;
			me->dy=Sine(me->facing*32)*6;
		}
	}
	else // running away mode
	{
		if(MGL_random(32)==0)
			me->facing=(byte)MGL_random(8);
		me->seq=ANIM_A2;	// stay melted
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=16;
		if(me->mind1==0)
		{
			me->seq=ANIM_A3;	// unmelt
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
			me->action=ACTION_BUSY;
			me->reload=5+MGL_random(30);
			me->mind=1;	// activate
			return;
		}
		// accelerate
		me->dx+=Cosine(me->facing*32)*2;
		me->dy+=Sine(me->facing*32)*2;
		if(me->z<16*FIXAMT)
			me->dz+=FIXAMT*3;
		Clamp(&me->dx,FIXAMT*12);
		Clamp(&me->dy,FIXAMT*12);
	}
}

void AI_Wacko(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_WACKOOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_WACKODIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frmTimer<196 && goodguy && (me->frm==5 || me->frm==11 || me->frm==17))
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			if(me->AttackCheck(32,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				goodguy->GetShot(Cosine(me->facing*32)*4,Sine(me->facing*32)*4,8,map,world);
			me->reload=10;
		}

		if(me->seq==ANIM_DIE && me->frm==5)
			me->frmAdvance=128;
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(48*FIXAMT) && MGL_random(8)==0)
			{
				// get him!
				MakeSound(SND_WACKOSTAB,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=196;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=0;
				return;
			}
			FaceGoodguy(me,goodguy);

			me->dx=Cosine(me->facing*32)*4;
			me->dy=Sine(me->facing*32)*4;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
			if(MGL_random(64)==0)
			{
				me->mind=1;		// occasionally wander
				me->mind1=1;
			}
		}
		else
		{
			me->mind=1;	// if there's no goodguy, get random
			me->mind1=1;
		}
	}
	else if(me->mind==1)	// random wandering
	{
		if(!(me->mind1--))	// time to get a new direction
		{
			if((goodguy) && MGL_random(3)==0)
				me->mind=0;	// get back on track
			else
				me->facing=(byte)MGL_random(8);
			me->mind1=MGL_random(40)+1;
		}

		me->dx=Cosine(me->facing*32)*3;
		me->dy=Sine(me->facing*32)*3;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
}

void AI_Boiler(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_BOILERHIT,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm>=3 && me->frm<=10)
		{
			FireExactBullet(me->x+FIXAMT*36,me->y,FIXAMT*50,
							-FIXAMT*2,FIXAMT*12,-FIXAMT*3,
							0,24,2*32,BLT_FLAME2);
			FireExactBullet(me->x+FIXAMT*52,me->y,FIXAMT*50,
							0,FIXAMT*12,-FIXAMT*3,
							0,24,2*32,BLT_FLAME2);
			FireExactBullet(me->x+FIXAMT*68,me->y,FIXAMT*50,
							FIXAMT*2,FIXAMT*12,-FIXAMT*3,
							0,24,2*32,BLT_FLAME2);

			MakeSound(SND_FLAMEGO,me->x,me->y,SND_CUTOFF,1100);
		}

		if(me->seq==ANIM_DIE)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,2);
			BlowSmoke((x+me->rectx+MGL_random(me->rectx2-me->rectx))<<FIXSHIFT,
					  (y+me->recty+MGL_random(me->recty2-me->recty))<<FIXSHIFT,
					  me->z,FIXAMT);
			ShakeScreen(10);
		}
		return;	// can't do nothin' right now
	}

	// lob grenades occasionally
	if(me->reload==0 && MGL_random(16)==0)
	{
		me->mind1=25;
		me->reload=80;
	}
	if(me->mind1>0)
	{
		me->mind1--;
		if((me->mind1&7)==0)
		{
			MakeSound(SND_MUSHMISSILE,me->x,me->y,SND_CUTOFF,1500);
			FireBullet(me->x-48*FIXAMT,me->y+40*FIXAMT,(byte)MGL_random(129),BLT_GRENADE);
		}
	}
	// shoot flame occasionally, but only when not grenading
	if(MGL_random(64)==0 && me->mind1==0)
	{
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=64;
		me->action=ACTION_BUSY;
	}
}

void AI_Eyeguy(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_EYEGUYOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_EYEGUYDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==4 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*20;
			y=me->y+Sine(me->facing*32)*20;
			// fire a shot
			FireBullet(me->x,me->y,me->facing,BLT_YELWAVE);
			me->reload=10;
		}
		return;	// can't do nothin' right now
	}

	if(goodguy)
	{
		if(RangeToTarget(me,goodguy)<(256*FIXAMT) && MGL_random(32)==0 && me->reload==0)
		{
			// spit at him
			MakeSound(SND_EYEGUYSHT,me->x,me->y,SND_CUTOFF,1200);
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
	}

	if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if(goodguy)
		{
			FaceGoodguy(me,goodguy);

			me->dx=Cosine(me->facing*32)*3;
			me->dy=Sine(me->facing*32)*3;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
			if(MGL_random(64)==0)
			{
				me->mind=1;		// occasionally wander
				me->mind1=1;
			}
		}
		else
		{
			me->mind=1;	// if there's no goodguy, get random
			me->mind1=1;
		}
	}
	else if(me->mind==1)	// random wandering
	{
		if(!(me->mind1--))	// time to get a new direction
		{
			if((goodguy) && MGL_random(3)==0)
				me->mind=0;	// get back on track
			else
				me->facing=(byte)MGL_random(8);
			me->mind1=MGL_random(40)+1;
		}

		me->dx=Cosine(me->facing*32)*3;
		me->dy=Sine(me->facing*32)*3;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
}

void AI_Peeper(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_PEEPOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_PEEPDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==3 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*32;
			y=me->y+Sine(me->facing*32)*32;
			// fire a shot
			FireBullet(me->x,me->y,me->facing*32,BLT_BIGYELLOW);
			MakeSound(SND_PEEPSHOOT,me->x,me->y,SND_CUTOFF,1200);
			me->reload=30*4;
		}
		if(me->seq==ANIM_DIE)
		{
			BlowWigglySmoke(me->x-FIXAMT*16+MGL_randoml(FIXAMT*32),me->y-FIXAMT*16+MGL_randoml(FIXAMT*32),
				me->z,MGL_randoml(FIXAMT*2));
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// not awake yet
	{
		me->seq=ANIM_A2;	// sleep
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=64;

		if(goodguy && (RangeToTarget(me,goodguy)<256*FIXAMT || me->ouch>0))
		{
			me->mind=1;	// wake up
			me->action=ACTION_BUSY;	// but don't do anything while waking up
			me->mind1=4;
		}
	}
	else	// awake and pissed
	{
		if(me->mind1)
			me->mind1--;
		else if(goodguy)
		{
			FaceGoodguy2(me,goodguy);
			me->mind1=4;
		}

		me->dx=Cosine(me->facing*32)*2;
		me->dy=Sine(me->facing*32)*2;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
		if(MGL_random(64)==0 && me->reload==0)
		{
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
		}
		if(MGL_random(256)==0)
		{
			me->seq=ANIM_A1;	// blink
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
			me->action=ACTION_BUSY;
		}
	}
}

void AI_Tower(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_TOWEROUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_TOWERDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK)	// leaping attack
		{
			if(me->frm<4)
			{
				me->dx=0;
				me->dy=0;
				me->dz=0;
			}
			else if(me->frm==4)
			{
				me->dx=Cosine(me->facing*32)*12;
				me->dy=Sine(me->facing*32)*12;
				me->dz=10*FIXAMT;
			}
			else if(me->frm<8)
			{
				// stay the course
				Dampen(&me->dx,FIXAMT/8);
				Dampen(&me->dy,FIXAMT/8);
			}
			else if(me->frm==12 && me->frmTimer<32)
			{
				FireBullet(me->x,me->y-FIXAMT,0,BLT_SHOCKWAVE);
				MakeSound(SND_BOMBBOOM,me->x,me->y,SND_CUTOFF,1500);
				ShakeScreen(10);
			}
			else
			{
				Dampen(&me->dx,FIXAMT/2);
				Dampen(&me->dy,FIXAMT/2);
			}
		}
		if(me->seq==ANIM_DIE)
		{
			if(me->frm>=4)
			{
				x=me->x>>FIXSHIFT;
				y=me->y>>FIXSHIFT;
				BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
				BlowSmoke((x+me->rectx+MGL_random(me->rectx2-me->rectx))<<FIXSHIFT,
						  (y+me->recty+MGL_random(me->recty2-me->recty))<<FIXSHIFT,
						  me->z,FIXAMT);
			}
		}
		if(me->seq==ANIM_A1)
		{
			if(me->frmTimer<32)
			{
				MakeSound(SND_MUSHMISSILE,me->x,me->y,SND_CUTOFF,1500);
				me->facing=(me->facing+1)&7;
				FireBullet(me->x,me->y,me->facing*32,BLT_GRENADE);
			}
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<300*FIXAMT)
				me->mind=1;
		}
		if(me->ouch)
			me->mind=1;
	}
	else	// chase down Kid Mystic
	{
		if(goodguy)
		{
			if(me->placed)
				me->placed--;
			else
			{
				if(me->mind1)
				{
					me->placed=8;
					me->facing=(me->facing+3+MGL_random(3))&7;
					me->mind1=0;
				}
				else
				{
					FaceGoodguy2(me,goodguy);
					me->placed=4;
				}
			}

			me->dx=Cosine(me->facing*32)*4;
			me->dy=Sine(me->facing*32)*4;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
			if(RangeToTarget(me,goodguy)>200*FIXAMT && me->reload==0)
			{
				me->seq=ANIM_A1;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=120;
			}
			if(MGL_random(32)==0 || RangeToTarget(me,goodguy)<100*FIXAMT)
			{
				//leap!
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
			}
		}
		else
			me->mind=0;
	}
}

void AI_Pinkeye(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_PEEPOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
		{
			MakeSound(SND_PEEPDIE,me->x,me->y,SND_CUTOFF,1200);
			me->reload=255;	// set the resurrection timer
		}
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			if(me->frm==4 && me->reload>0)
			{
				me->frm=3;	// stay still until you're done reloading!
			}
			if(me->frm==8)
			{
				// resurrect!
				me->hp=20;
				me->frm=0;
				me->seq=ANIM_IDLE;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_IDLE;
			}
		}
		if(me->seq==ANIM_ATTACK)
		{
			if(me->frm==5 && me->frmTimer<32)
			{
				FireBullet(me->x,me->y,(byte)((me->facing*32)-16+MGL_random(32)),BLT_ENERGY);
				//MakeSound(SND_PEEPSHOOT,me->x,me->y,SND_CUTOFF,1500);
			}
		}
		return;	// can't do nothin' right now
	}

	if(goodguy)
		FaceGoodguy(me,goodguy);
	if(me->reload==0)
	{
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
		me->reload=30+MGL_random(90);
	}
}


void AI_Lookeyloo(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_LOOKEYOUCH,me->x,me->y,SND_CUTOFF,1800);
		else
		{
			MakeSound(SND_LOOKEYDIE,me->x,me->y,SND_CUTOFF,2000);
		}
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			FireBullet(me->x-128*FIXAMT+MGL_randoml(256*FIXAMT),me->y-64*FIXAMT+MGL_randoml(128*FIXAMT),
				(byte)MGL_random(256),BLT_BOOM);
			ShakeScreen(20);
			if(me->frm==4)
				KillPinkeyes();
		}
		if(me->seq==ANIM_ATTACK)
		{
			if(me->frm==9 && me->frmTimer<32)
			{
				LaunchMegabeam(me->x,me->y+FIXAMT*40,me->ID);
			}
			if(me->frm==18)
				me->reload=150;
		}
		return;	// can't do nothin' right now
	}

	if(me->reload==0)
	{
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=32;
		me->action=ACTION_BUSY;
	}
}

void AI_BigBat(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	Guy *g;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SCARIEROUCH,me->x,me->y,SND_CUTOFF,1200);
		else
		{
			MakeSound(SND_SCARIERDIE,me->x,me->y,SND_CUTOFF,1200);
			me->reload=255;	// set the resurrection timer
		}
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			BlowWigglySmoke(me->x,me->y,me->z,FIXAMT*2);
		}
		if(me->seq==ANIM_ATTACK)
		{
			if(me->frm==2 && me->frmTimer<32)
			{
				g=AddGuy(me->x,me->y,me->z,MONS_BAT);
				if(g)
				{
					if(!g->CanWalk(g->x,g->y,map,world))
						g->type=MONS_NONE;
					g->z=me->z;
					g->facing=me->facing;
					g->action=ACTION_BUSY;
					g->seq=ANIM_A1;	// diving attack move
					g->frm=0;
					g->frmTimer=0;
					g->frmAdvance=128;
					g->dx=Cosine(g->facing*32)*12;
					g->dy=Sine(g->facing*32)*12;
				}
				me->reload=30;
			}
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly fly towards the player
	{
		if(goodguy)
		{
			if(me->mind1>0)
				me->mind1--;
			else
			{
				FaceGoodguy2(me,goodguy);
				me->mind1=8;
				if(MGL_random(16)==0)
				{
					me->mind=1;		// occasionally wander
					me->mind1=60;
				}
			}

			me->dx=Cosine(me->facing*32)*2;
			me->dy=Sine(me->facing*32)*2;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
		}
		else
		{
			me->mind=1;	// if there's no goodguy, get random
			me->mind1=60;
		}
	}
	else if(me->mind==1)	// random wandering
	{
		if(me->mind1)
			me->mind1--;
		else
		{
			if((goodguy) && MGL_random(16)==0)
				me->mind=0;	// get back on track
			else
				me->facing=(me->facing-1+MGL_random(3))&7;
			me->mind1=8;
		}

		me->dx=Cosine(me->facing*32)*2;
		me->dy=Sine(me->facing*32)*2;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
	if(me->reload==0 && MGL_random(32)==0 && goodguy && RangeToTarget(me,goodguy)<512*FIXAMT)
	{
		me->action=ACTION_BUSY;
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		MakeSound(SND_SCARIERSPIT,me->x,me->y,SND_CUTOFF,1200);
	}
}

void AI_Golem(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	Guy *g;
	int x,y;

	g=GetGoodguy();

	if(g==NULL)
	{
		// vanish if there is no goodguy to follow
		me->type=MONS_NONE;
		return;
	}

	if(me->reload)
		me->reload--;

	if(me->mind1)
		me->mind1--;

	if(me->placed)
		me->placed--;
	else if(me->hp>0)
	{
		me->placed=15;
		me->hp--;
		if(me->hp==0)
		{
			me->seq=ANIM_DIE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=64;
			me->action=ACTION_BUSY;
			me->ouch=4;
			me->dx=0;
			me->dy=0;
		}
		if(!MonsterShown())
		{
			x=me->hp*128/MonsterHP(me->type);
			ShowEnemyLife(MonsterName(me->type),x,x,me->hp>0);
		}
	}

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_GOLEMOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_GOLEMDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==5 && me->reload==0)
		{
			// smash
			MakeSound(SND_BOMBBOOM,me->x,me->y,SND_CUTOFF,600);
			ShakeScreen(5);
			x=me->x+Cosine(me->facing*32)*32;
			y=me->y+Sine(me->facing*32)*32;
			FireBullet(x,y,me->facing,BLT_GOODSHOCK);
			/*
			if(goodguy)
			{
				if(me->AttackCheck(48,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
					goodguy->GetShot(Cosine(me->facing*32)*6,Sine(me->facing*32)*6,15,map,world);
			}
			*/
			me->reload=60;
		}
		if(me->seq==ANIM_A1)	// forming
		{
			if(!me->CanWalk(me->x,me->y,map,world))
				me->type=MONS_NONE;
		}
		if(me->seq==ANIM_DIE)
		{
			if(me->frm>3)
			{
				x=me->x-32*FIXAMT+MGL_randoml(64*FIXAMT);
				y=me->y-32*FIXAMT+MGL_randoml(64*FIXAMT);
				BlowWigglySmoke(x,y,me->z,0);
				if(me->frmTimer<32 && MGL_random(2)==0)
					FireExactBullet(x,y,0,0,0,0,0,3,0,BLT_BOMB);
			}
		}
		return;
	}
	if(me->mind==0)	// standard combat mode
	{
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frmAdvance=128;
		}
		me->dx=Cosine(me->facing*32)*2;
		me->dy=Sine(me->facing*32)*2;
		if(goodguy==NULL || goodguy->type==MONS_FRIENDLY || RangeToTarget(me,g)>512*FIXAMT)
		{
			// too far from goodguy, run home
			me->mind=1;
		}
		else
		{
			if(RangeToTarget(me,goodguy)>300*FIXAMT)	// too far to fight, go home
			{
				me->mind=1;
			}
			else
			{
				if(me->mind1==0)
				{
					FaceGoodguy2(me,goodguy);
					me->mind1=4;
				}
				if(me->reload==0 && RangeToTarget(me,goodguy)<96*FIXAMT)
				{
					me->action=ACTION_BUSY;
					me->seq=ANIM_ATTACK;
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=128;
					me->dx=0;
					me->dy=0;
					MakeSound(SND_GOLEMPOUND,me->x,me->y,SND_CUTOFF,1200);
				}
			}
		}
	}
	else if(me->mind==1)	// running home mode
	{
		if(RangeToTarget(me,g)<128*FIXAMT)
		{
			if(goodguy && RangeToTarget(me,goodguy)<300*FIXAMT)
			{
				// got close enough, get violent again
				me->mind=0;
			}
			else
			{
				me->mind=2;	// close enough to the Kid, sit still
			}
		}
		if(me->mind1==0)
		{
			FaceGoodguy2(me,g);
			me->mind1=4;
		}
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frmAdvance=128;
		}
		me->dx=Cosine(me->facing*32)*2;
		me->dy=Sine(me->facing*32)*2;
	}
	else	// waiting patiently mode
	{
		if(me->mind1==0)
			me->mind1=4;
		if(goodguy && RangeToTarget(me,goodguy)<300*FIXAMT)
		{
			// enemy is near, get violent again
			me->mind=0;
		}
		else if(RangeToTarget(me,g)>256*FIXAMT)
		{
			me->mind=1;	// got too far from home!
		}
		if(me->seq!=ANIM_IDLE)
		{
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
		me->dx=0;
		me->dy=0;
	}
}

void AI_HugeBat(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	Guy *g;

	if(me->reload)
		me->reload--;

	if(me->mind1)
		me->mind1--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_UBEROUCH,me->x,me->y,SND_CUTOFF,1800);
		else
			MakeSound(SND_UBERDIE,me->x,me->y,SND_CUTOFF,2000);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==2 && me->reload==0)
		{
			x=me->x-FIXAMT*32;
			y=me->y+FIXAMT*20;
			g=AddGuy(x,y,FIXAMT*40,MONS_BAT);
			if(g)
			{
				g->facing=3;
				g->action=ACTION_BUSY;
				g->seq=ANIM_A1;	// diving attack move
				g->frm=0;
				g->frmTimer=0;
				g->frmAdvance=128;
				g->dx=Cosine(g->facing*32)*12;
				g->dy=Sine(g->facing*32)*12;
			}
			x=me->x;
			g=AddGuy(x,y,FIXAMT*40,MONS_BAT);
			if(g)
			{
				g->facing=2;
				g->action=ACTION_BUSY;
				g->seq=ANIM_A1;	// diving attack move
				g->frm=0;
				g->frmTimer=0;
				g->frmAdvance=128;
				g->dx=Cosine(g->facing*32)*12;
				g->dy=Sine(g->facing*32)*12;
			}
			x=me->x+FIXAMT*32;
			g=AddGuy(x,y,FIXAMT*40,MONS_BAT);
			if(g)
			{
				g->facing=1;
				g->action=ACTION_BUSY;
				g->seq=ANIM_A1;	// diving attack move
				g->frm=0;
				g->frmTimer=0;
				g->frmAdvance=128;
				g->dx=Cosine(g->facing*32)*12;
				g->dy=Sine(g->facing*32)*12;
			}
			me->reload=60;
		}
		if(me->seq==ANIM_DIE)
		{
			if(me->frm>=8)
			{
				x=me->x>>FIXSHIFT;
				y=me->y>>FIXSHIFT;
				BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
				BlowSmoke((x+me->rectx+MGL_random(me->rectx2-me->rectx))<<FIXSHIFT,
						  (y+me->recty+MGL_random(me->recty2-me->recty))<<FIXSHIFT,
						  me->z,FIXAMT);
			}
		}

		return;	// can't do nothin' right now
	}

	if(goodguy && me->reload==0 && RangeToTarget(me,goodguy)<512*FIXAMT)
	{
		// spit some bats
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		MakeSound(SND_UBERSPIT,me->x,me->y,SND_CUTOFF,1200);
	}
}

void AI_BobbyKhan(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_BOBBYOUCH,me->x,me->y,SND_CUTOFF,1800);
		else
			MakeSound(SND_BOBBYDIE,me->x,me->y,SND_CUTOFF,2000);
	}

	if(!goodguy)
		return;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_A2 && me->frm>=3)
		{
			// spin attack, begin moving
			if(me->frm<10)
			{
				me->dx+=Cosine(me->facing*32);
				me->dy+=Sine(me->facing*32);
				Clamp(&me->dx,FIXAMT*12);
				Clamp(&me->dy,FIXAMT*12);
			}
			else
			{
				Dampen(&me->dx,FIXAMT);
				Dampen(&me->dy,FIXAMT);
			}
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;

			if(!me->mind1)
				switch(me->frm)
				{
					case 4:
						if(me->AttackCheck2(x-180,y-100,x,y+80,goodguy))
						{
							goodguy->GetShot(-FIXAMT*8,FIXAMT*8,32,map,world);
							goodguy->dx=-FIXAMT*8;
							goodguy->dy=FIXAMT*8;
							me->mind1=1;
						}
						break;
					case 5:
						if(me->AttackCheck2(x-200,y-180,x,y,goodguy))
						{
							goodguy->GetShot(-FIXAMT*12,0,32,map,world);
							goodguy->dx=-FIXAMT*12;
							goodguy->dy=0;
							me->mind1=1;
						}
						break;
					case 6:
						if(me->AttackCheck2(x-150,y-120,x+40,y,goodguy))
						{
							goodguy->GetShot(-FIXAMT*8,-FIXAMT*8,32,map,world);
							goodguy->dx=-FIXAMT*8;
							goodguy->dy=-FIXAMT*8;
							me->mind1=1;
						}
						break;
					case 7:
						if(me->AttackCheck2(x,y-150,x+150,y-20,goodguy))
						{
							goodguy->GetShot(0,-FIXAMT*12,32,map,world);
							goodguy->dx=0;
							goodguy->dy=-FIXAMT*12;
							me->mind1=1;
						}
						break;
					case 8:
						if(me->AttackCheck2(x+20,y-100,x+230,y,goodguy))
						{
							goodguy->GetShot(FIXAMT*8,-FIXAMT*8,32,map,world);
							goodguy->dx=FIXAMT*8;
							goodguy->dy=-FIXAMT*8;
							me->mind1=1;
						}
						break;
					case 9:
						if(me->AttackCheck2(x,y,x+230,y+70,goodguy))
						{
							goodguy->GetShot(FIXAMT*12,0,32,map,world);
							goodguy->dx=FIXAMT*12;
							goodguy->dy=0;
							me->mind1=1;
						}
						break;
					case 3:
					case 10:
						if(me->AttackCheck2(x-120,y,x+45,y+160,goodguy))
						{
							goodguy->GetShot(FIXAMT*8,FIXAMT*8,32,map,world);
							goodguy->dx=FIXAMT*8;
							goodguy->dy=FIXAMT*8;
							me->mind1=1;
						}
						break;
					case 11:
						if(me->AttackCheck2(x-220,y,x-10,y+120,goodguy))
						{
							goodguy->GetShot(FIXAMT*12,0,32,map,world);
							goodguy->dx=0;
							goodguy->dy=FIXAMT*12;
							me->mind1=1;
						}
						break;
				}
		}

		if(me->seq==ANIM_A3 && (me->frm==9 || me->frm==10))
		{
			ShakeScreen(30);
			if(me->frm==9 && me->frmTimer<32)
				MakeSound(SND_SDZLPOUND,me->x,me->y,SND_CUTOFF,1200);

			for(i=0;i<15;i++)
			{
				x=me->x+((400-MGL_random(801))<<FIXSHIFT);
				y=me->y+((400-MGL_random(801))<<FIXSHIFT);
				FireExactBullet(x,y,FIXAMT*160+(MGL_random(40)<<FIXSHIFT),
								0,0,0,0,30,2,BLT_ROCK);
			}
		}
		if(me->seq==ANIM_DIE)
		{
			if(me->frm>=8)
			{
				x=me->x>>FIXSHIFT;
				y=me->y>>FIXSHIFT;
				BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
				BlowSmoke((x+me->rectx+MGL_random(me->rectx2-me->rectx))<<FIXSHIFT,
						  (y+me->recty+MGL_random(me->recty2-me->recty))<<FIXSHIFT,
						  me->z,FIXAMT);
			}
		}
		if(me->seq==ANIM_A4 && me->frm==0 && me->frmTimer<32)
		{
			if(player.nightmare)
			{
				for(x=0;x<3;x++)
				{
					i=MGL_random(256);
					FireExactBullet(me->x,me->y+FIXAMT*30,FIXAMT*30,Cosine(i)*4,Sine(i)*4,0,0,255,4*32,BLT_BIGYELLOW);
				}
			}
			else
			{
				for(x=0;x<6;x++)
				{
					i=MGL_random(256);
					FireExactBullet(me->x,me->y+FIXAMT*30,FIXAMT*30,Cosine(i)*4,Sine(i)*4,0,0,255,4*32,BLT_BIGYELLOW);
				}
			}
		}
		if(me->seq==ANIM_ATTACK && me->frm==5 && me->frmTimer<32)
		{
			// slash for pain
			x=(me->x>>FIXSHIFT)-64;
			y=(me->y>>FIXSHIFT);
			if(me->AttackCheck2(x,y,x+72,y+225,goodguy))
			{
				goodguy->GetShot(0,FIXAMT*12,64,map,world);
				goodguy->dx=0;
				goodguy->dy=FIXAMT*12;
			}
		}

		return;	// can't do nothin' right now
	}

	if(me->reload==0)
	{
		x=RangeToTarget(me,goodguy);
		y=0;	// flags to decide which attack to do

		// spin ANY time he gets close
		if(x<180*FIXAMT)
		{
			y|=8;
			if(goodguy->x>me->x-64*FIXAMT && goodguy->x<me->x+64*FIXAMT &&
				goodguy->y>me->y)
			{
				y|=1;	// 1=slash attack
			}
		}
		else if(x<300*FIXAMT)	// medium range
		{
			// do the slash if he is conveniently in front of you
			if(goodguy->x>me->x-64*FIXAMT && goodguy->x<me->x+64*FIXAMT &&
				goodguy->y>me->y)
			{
				y|=1;	// 1=slash attack
			}
			y|=2;	// 2=rock stomp

			// only do block if player is below me
			if(goodguy->y>me->y)
				y|=4;	// 4=shield block
		}
		else	// long range
		{
			y|=2;	// rock stomp
			y|=8;	// spin attack from afar sometimes to close the gap
		}

		if(y!=0)
		{
			if((y&1) && (MGL_random(10)==0))	// slash attack is highly likely
			{
				MakeSound(SND_BOBBYSLASH,me->x,me->y,SND_CUTOFF,1200);
				me->reload=30;
				me->action=ACTION_BUSY;
				me->seq=ANIM_ATTACK;	// slash
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=64;
				me->dx=0;
				me->dy=0;
				me->mind=0;
				return;
			}
			if((y&2) && (MGL_random(40)==0))	// rock stomp is rare
			{
				MakeSound(SND_BOBBYSTOMP,me->x,me->y,SND_CUTOFF,1200);
				me->reload=60;
				me->action=ACTION_BUSY;
				me->seq=ANIM_A3;	// stomp
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->dx=0;
				me->dy=0;
				me->mind=0;
				return;
			}
			if((y&4) && (MGL_random(20)==0))	// shield block is less rare, but is superceded by rock stomp
			{
				me->reload=30;
				me->action=ACTION_BUSY;
				me->seq=ANIM_A1;	// block
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->dx=0;
				me->dy=0;
				return;
			}
			if((y&8) && (MGL_random(20)==0))	// spin attack is relatively likely
			{
				MakeSound(SND_BOBBYSPIN,me->x,me->y,SND_CUTOFF,1200);
				me->reload=60;
				me->action=ACTION_BUSY;
				me->seq=ANIM_A2;	// spin attack
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				FaceGoodguy(me,goodguy);
				me->dx=0;
				me->dy=0;
				me->mind=0;
				return;
			}
		}
	}
	if(me->ouch>2 && me->action==ACTION_IDLE && me->reload==0 && MGL_random(4+(player.nightmare!=0)*4)==0 && me->mind==0)
	{
		me->reload=60;
		me->action=ACTION_BUSY;
		me->seq=ANIM_A1;	// block
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->dx=0;
		me->dy=0;
		me->mind=1;
		return;
	}
	if(me->action==ACTION_IDLE)
	{
		FaceGoodguy2(me,goodguy);
		me->dx=Cosine(me->facing*32)*4;
		me->dy=Sine(me->facing*32)*4;
		me->mind1=0;
	}
	if(me->seq!=ANIM_MOVE && me->action==ACTION_IDLE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}

void AI_Fairy(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int neighbor[8];
	char dxTab[8]={1,1,0,-1,-1,-1,0,1};
	char dyTab[8]={0,1,1,1,0,-1,-1,-1};
	int i,dir;

	if(!goodguy)
		return;	// nothing

	ExplodeParticles(PART_YELLOW,me->x,me->y,me->z,1);

	if(RangeToTarget(me,goodguy)>(260*FIXAMT))
	{
		me->dx=0;
		me->dy=0;
		me->reload=0;
		if(MGL_random(40)==0)
			me->facing=(me->facing-1+MGL_random(3))&7;
		return;
	}

	if(me->reload==0)	// time to pick a new square to go to
	{
		for(i=0;i<8;i++)
		{
			if(me->mapx+dxTab[i]<0 || me->mapx+dxTab[i]>map->width-1 ||
			   me->mapy+dyTab[i]<0 || me->mapy+dyTab[i]>map->height-1 ||
			   map->map[me->mapx+dxTab[i]+(me->mapy+dyTab[i])*map->width].item>=MAX_SHOOTABLE_ITMS ||
			   map->map[me->mapx+dxTab[i]+(me->mapy+dyTab[i])*map->width].wall)
				neighbor[i]=-1;
			else
			{
				neighbor[i]=abs((me->mapx+dxTab[i])-goodguy->mapx)*TILE_WIDTH+abs((me->mapy+dyTab[i])-goodguy->mapy)*TILE_HEIGHT;
			}
		}
		dir=MGL_random(8);
		for(i=0;i<8;i++)
			if(neighbor[dir]==-1 || neighbor[i]>neighbor[dir])
				dir=i;

		me->facing=dir;
		me->mind=dir*32-16+MGL_random(33);
		Dampen(&me->dx,FIXAMT);
		Dampen(&me->dy,FIXAMT);
		me->dx+=Cosine(me->mind);
		me->dy+=Sine(me->mind);
		me->reload=((24+MGL_random(9))*FIXAMT/player.maxSpeed);
	}
	else
	{
		Dampen(&me->dx,FIXAMT/4);
		Dampen(&me->dy,FIXAMT/4);
		me->dx+=Cosine(me->mind);
		me->dy+=Sine(me->mind);
		if(player.worldNum==1)	// chapter 2
		{
			Clamp(&me->dx,player.maxSpeed/2);
			Clamp(&me->dy,player.maxSpeed/2);
		}
		else
		{
			Clamp(&me->dx,player.maxSpeed);
			Clamp(&me->dy,player.maxSpeed);
		}
		me->reload--;
	}
	if(RangeToTarget(me,goodguy)<(48*FIXAMT))
	{
		// get him!
		MakeNormalSound(SND_FAIRYGET);
		brainX=255;
		if(player.haveFairy==0)
			InitSpeech(18);
		FairyCaughtText(me->mind1);

		player.haveFairy|=(1<<(me->mind1-1));
		for(i=0;i<4;i++)
			if(player.chaseFairy[i]==me->mind1)
				player.chaseFairy[i]=0;

		CheckForAllSecrets();
		me->type=MONS_NONE;
		return;
	}
	me->frmAdvance=200;
}

void AI_Fairy2(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	Guy *g;
	int i;

	g=GetGoodguy();
	ExplodeParticles(PART_YELLOW,me->x,me->y,me->z,1);

	if(g==NULL)
	{
		// vanish if there is no goodguy to follow
		me->type=MONS_NONE;
		return;
	}

	if(me->reload)
		me->reload--;

	if(me->mind)
		me->mind--;

	if(me->mind==0)
	{
		FaceGoodguy2(me,g);
		me->mind=2;
	}

	me->frmAdvance=200;
	if(RangeToTarget(me,g)>100*FIXAMT)
	{
		me->dx+=Cosine(me->facing*32)*4;
		me->dy+=Sine(me->facing*32)*4;

		Clamp(&me->dx,FIXAMT*16);
		Clamp(&me->dy,FIXAMT*16);
	}
	else
	{
		me->dx+=Cosine(me->facing*32)*2;
		me->dy+=Sine(me->facing*32)*2;

		Clamp(&me->dx,FIXAMT*7);
		Clamp(&me->dy,FIXAMT*7);
	}
	for(i=0;i<16;i++)
	{
		if((1<<i)==player.fairyOn)
			me->mind1=(byte)(i+1);
	}
}

void AI_CrazyBush(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_BUSHOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_BUSHDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		return;	// can't do nothin' right now
	}

	if(goodguy)
	{
		if(RangeToTarget(me,goodguy)<(50*FIXAMT) && MGL_random(8)==0 && map->FindGuy(me->mapx,me->mapy,5,goodguy))
		{
			// get him!
			goodguy->GetShot(me->dx/2,me->dy/2,1,map,world);
		}
		FaceGoodguy2(me,goodguy);

		me->dx=Cosine(me->facing*32)*5;
		me->dy=Sine(me->facing*32)*5;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
}

void AI_Octopus(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(!goodguy)
		return;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_OCTONOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_OCTONDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK)
		{
			WaterRipple(me->x/FIXAMT,me->y/FIXAMT,10*32);
			if((me->frm==4 || me->frm==10 || me->frm==16) && me->reload==0 && goodguy)
			{
				// spit stuff
				byte f;

				FaceGoodguy2(me,goodguy);
				x=me->x+Cosine(me->facing*32)*14;
				y=me->y+Sine(me->facing*32)*14;
				f=me->facing*32-12+MGL_random(25);
				FireBullet(x,y,f,BLT_ENERGY);
				me->reload=2;
				MakeSound(SND_OCTONSHOOT,me->x,me->y,SND_CUTOFF,1200);
			}
			if(me->frm==20)
			{
				WaterRipple(me->x/FIXAMT,me->y/FIXAMT,32*32);
				me->seq=ANIM_A2;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
		}
		if(me->seq==ANIM_A2)
		{
			if(me->frm==5)
				me->mind=0;
		}
		return;	// can't do nothin' right now
	}

	WaterRipple((me->x+me->dx*2)/FIXAMT,(me->y+me->dy*2)/FIXAMT,8*MGL_random(32));

	if(me->mind==0)	// lurking
	{
		if(RangeToTarget(me,goodguy)<(300*FIXAMT) && MGL_random(48)==0 && me->reload==0)
		{
			ExplodeParticles(PART_WATER,me->x,me->y,0,10);
			WaterRipple(me->x/FIXAMT,me->y/FIXAMT,64*32);
			FaceGoodguy2(me,goodguy);
			// arise
			me->seq=ANIM_A1;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->dx=0;
			me->dy=0;
			me->reload=0;
			me->mind=1;
			me->mind1=0;
			MakeSound(SND_OCTONSPLASH,me->x,me->y,SND_CUTOFF,1200);
			return;
		}

		if(me->mind1)
			me->mind1--;
		if(!me->mind1)	// time to get a new direction
		{
			me->facing=(byte)MGL_random(8);
			me->mind1=MGL_random(60)+1;
		}
		me->dx=Cosine(me->facing*32)*2;
		me->dy=Sine(me->facing*32)*2;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
		}
		me->frmAdvance=MGL_random(128)+2;
	}
	else if(me->mind==1)	// arising
	{
		if(me->frm>=5)
		{
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=200;
			me->mind=1;
			me->action=ACTION_BUSY;
		}
	}
	if((world->terrain[map->map[me->mapx+me->mapy*map->width].floor].flags&(TF_WATER|TF_LAVA))==0)
		me->GetShot(0,0,20,map,world);
}

void AI_Incabot(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_INCAHIT,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_INCADIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		return;	// can't do nothin' right now
	}

	if(goodguy)
	{
		me->x+=Cosine(me->facing*32)*15;
		me->y+=Sine(me->facing*32)*15;
		if(RangeToTarget(me,goodguy)<(35*FIXAMT) && me->frmTimer<128 && me->reload==0 &&
			map->FindGuy(me->mapx,me->mapy,5,goodguy))
		{
			// get him!
			MakeSound(SND_GINGERCOMBO,me->x,me->y,SND_CUTOFF,1200);
			goodguy->GetShot(me->dx/2,me->dy/2,1,map,world);
			me->reload=4;
		}
		me->x-=Cosine(me->facing*32)*15;
		me->y-=Sine(me->facing*32)*15;
		FaceGoodguy(me,goodguy);

		me->dx=Cosine(me->facing*32)*4;
		me->dy=Sine(me->facing*32)*4;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
}

void AI_Incagen(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	Guy *g;
	int rng;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		MakeSound(SND_INCAHIT,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_MOVE && me->frm==8 && me->reload==0)
		{
			g=AddGuy(me->x,me->y+FIXAMT*2,0,MONS_INCABOT);
			if(g && (!g->CanWalk(me->x,me->y,map,world)))
				g->type=MONS_NONE;	// vanish if this spot is occupied
			if(player.levelNum==18)
				me->reload=10;
			else
				me->reload=80;
		}
		if(me->seq==ANIM_DIE)
		{
			FireBullet(me->x-48*FIXAMT+MGL_randoml(97*FIXAMT),me->y-40*FIXAMT+MGL_randoml(81*FIXAMT),0,BLT_BOOM);
		}
		return;
	}
	if(player.levelNum==18)
		rng=800*FIXAMT;
	else
		rng=400*FIXAMT;

	if(!me->reload && me->mind==1 && goodguy && RangeToTarget(me,goodguy)<rng)
	{
		MakeSound(SND_INCAGEN,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
		map->map[me->mapx+me->mapy*map->width].templight=8;
		if(me->mapx>0)
			map->map[me->mapx-1+me->mapy*map->width].templight=0;
		if(me->mapx<map->width-1)
			map->map[me->mapx+1+me->mapy*map->width].templight=0;
		if(me->mapy>0)
			map->map[me->mapx+(me->mapy-1)*map->width].templight=0;
		if(me->mapy<map->height-1)
			map->map[me->mapx+(me->mapy+1)*map->width].templight=0;
	}
	if(goodguy && RangeToTarget(me,goodguy)<rng)
		me->mind=1;
}


void AI_Incagold(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->action==ACTION_BUSY)
	{
		return;
	}

	if(me->type==MONS_INCAGOLD && me->mind==0)
	{
		me->mind=1;
		if(me->mapx>0 && map->map[me->mapx-1+me->mapy*map->width].wall>0)
		{
			me->facing=0;	// head right
			me->dx=FIXAMT*3/2;
			me->dy=0;
		}
		else
		{
			me->facing=4;	// head left
			me->dx=-FIXAMT*3/2;
			me->dy=0;
		}
	}
	else if(me->type==MONS_INCAGOLD2 && me->mind==0)
	{
		me->mind=1;
		if(me->mapy>0 && map->map[me->mapx+(me->mapy-1)*map->width].wall>0)
		{
			me->facing=2;	// head down
			me->dx=0;
			me->dy=FIXAMT*3/2;
		}
		else
		{
			me->facing=6;	// head up
			me->dx=0;
			me->dy=-FIXAMT*3/2;
		}
	}

	if(me->mind1)	// bumped a wall
	{
		me->mind1=0;

		if(me->type==MONS_INCAGOLD)
		{
			me->facing=4-me->facing;
			me->dx=-me->dx;
		}
		else
		{
			me->facing=8-me->facing;
			me->dy=-me->dy;
		}
	}

	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}

	if(goodguy)
	{
		char sway[]={0,1,2,3,4,3,2,1,0,-1,-2,-3,-4,-3,-2,-1};

		if(me->facing==0 && goodguy->x>me->x && abs(goodguy->y-me->y)<32*FIXAMT)
		{
			FireBullet(me->x+FIXAMT*32,me->y-4*FIXAMT-sway[me->frm]*FIXAMT,0*32,BLT_DEATHBEAM);
			FireBullet(me->x+FIXAMT*32,me->y+4*FIXAMT-sway[me->frm]*FIXAMT,0*32,BLT_DEATHBEAM);
			if(me->reload==0)
			{
				MakeSound(SND_INCABEAM,me->x,me->y,SND_CUTOFF,100);
				me->reload=10;
			}
		}
		if(me->facing==4 && goodguy->x<me->x && abs(goodguy->y-me->y)<32*FIXAMT)
		{
			FireBullet(me->x-FIXAMT*32,me->y-4*FIXAMT+sway[me->frm]*FIXAMT,4*32,BLT_DEATHBEAM);
			FireBullet(me->x-FIXAMT*32,me->y+4*FIXAMT+sway[me->frm]*FIXAMT,4*32,BLT_DEATHBEAM);
			if(me->reload==0)
			{
				MakeSound(SND_INCABEAM,me->x,me->y,SND_CUTOFF,100);
				me->reload=10;
			}
		}
		if(me->facing==2 && goodguy->y>me->y && abs(goodguy->x-me->x)<48*FIXAMT)
		{
			FireBullet(me->x-8*FIXAMT+sway[me->frm]*FIXAMT,me->y+FIXAMT*16,2*32,BLT_DEATHBEAM);
			FireBullet(me->x+8*FIXAMT+sway[me->frm]*FIXAMT,me->y+FIXAMT*16,2*32,BLT_DEATHBEAM);
			if(me->reload==0)
			{
				MakeSound(SND_INCABEAM,me->x,me->y,SND_CUTOFF,100);
				me->reload=10;
			}
		}
		if(me->facing==6 && goodguy->y<me->y && abs(goodguy->x-me->x)<48*FIXAMT)
		{
			FireBullet(me->x-8*FIXAMT-sway[me->frm]*FIXAMT,me->y-FIXAMT*16,6*32,BLT_DEATHBEAM);
			FireBullet(me->x+8*FIXAMT-sway[me->frm]*FIXAMT,me->y-FIXAMT*16,6*32,BLT_DEATHBEAM);
			if(me->reload==0)
			{
				MakeSound(SND_INCABEAM,me->x,me->y,SND_CUTOFF,100);
				me->reload=10;
			}
		}
	}
}

void AI_Log(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(player.worldNum==3)
		me->facing=11;
	else
		me->facing=0;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			x=me->x/FIXAMT;
			y=me->y/FIXAMT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
		}
		return;
	}

	if(me->mind==0)	// waiting to be boarded)
	{
		if(goodguy && RangeToTarget(me,goodguy)<70*FIXAMT)
		{
			// boarded!
			me->mind=1;
			goodguy->parent=me;
		}
	}
	else	// drifting
	{
		if(me->dy<FIXAMT*3)
			me->dy+=FIXAMT/8;
		if(me->mind1&2)
		{
			if(map->map[me->mapx+(me->mapy+1)*map->width].floor==53)
			{
				goodguy->dy=FIXAMT*20;
			}
			else
			{
				goodguy->hp=1;
				player.life=1;
			}
			me->seq=ANIM_DIE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			goodguy->parent=NULL;
		}
		if(me->mind1)
			me->dx=0;
		me->mind1=0;
	}
	WaterRipple(me->x/FIXAMT,me->y/FIXAMT,MGL_random(32*20));
	me->frmAdvance=64;
}


void AI_Incaboss(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int i,j;

	if(!goodguy)
		return;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_INCAHIT,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_INCADIE2,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE && me->frm==7)
			me->frm=6;	// stay dying forever

		return;
	}

	if(goodguy->y<me->y+FIXAMT*64 && abs(goodguy->x-me->x)<FIXAMT*90 && me->mind>0)
		goodguy->GetShot(0,0,10,map,world);

	if(me->mind==0)	// not set up yet
	{
		if(goodguy && map->FindGuy(me->mapx,me->mapy,12,goodguy) && (player.levelNum==18 || goodguy->y<=50*TILE_HEIGHT*FIXAMT+(FIXAMT*8)))
		{
			me->mind=1;	// mode 1 = doing nothing
			me->dx=FIXAMT*2;
			me->facing=60;	// used as a timer
			for(i=32;i<=54;i++)
				for(j=38;j<=50;j++)
					map->map[i+j*map->width].light=0;	// light the room
			map->map[40+51*map->width].floor=157;
			map->map[41+51*map->width].floor=157;
			map->map[40+51*map->width].wall=42;
			map->map[41+51*map->width].wall=42;
			map->map[40+51*map->width].item=0;
			map->map[41+51*map->width].item=0;
		}
		else
			return;	// wait til he is close
	}

	if(me->mind1)	// bumped a wall
	{
		me->mind1=0;

		me->dx=-me->dx;
	}

	me->facing--;
	if(me->facing==0)
	{
		me->mind++;
		if(me->mind==6)
			me->mind=1;
		me->facing=60;
		switch(me->mind)	// setup each mode
		{
			case 1:
				me->frmAdvance=128;

				break;
			case 2:	// fire breath
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				break;
			case 3:
				break;
			case 4:	// laser eyes
				me->seq=ANIM_A2;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				break;
			case 5:	// tongue
				me->seq=ANIM_A1;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=80;
				break;
		}
	}
	switch(me->mind)
	{
		case 1:	// doing nothing
			break;
		case 2:	// fire breath
			// shoot flames
			if(me->frm>=4 && me->frm<=15)
			{
				for(i=0;i<4;i++)
					FireBullet(me->x-FIXAMT*48+MGL_randoml(FIXAMT*96+1),me->y+FIXAMT*32,(byte)(2*32-8+MGL_random(17)),BLT_FLAME2);
			}

			if(me->seq==ANIM_IDLE)
				me->facing=1;
			break;
		case 3:	// waiting after fire
			break;
		case 4:	// laser eyes
			// fire lasers
			if(me->frm>=6 && me->frm<=16)
			{
				if(me->reload==0)
				{
					MakeSound(SND_INCABEAM,me->x,me->y,SND_CUTOFF,100);
					me->reload=10;
				}
				if(me->dx>0)
				{
					FireBullet(me->x-FIXAMT*48,me->y-FIXAMT,1*32,BLT_DEATHBEAM);
					FireBullet(me->x+FIXAMT*42,me->y-FIXAMT,1*32,BLT_DEATHBEAM);
				}
				else
				{
					FireBullet(me->x-FIXAMT*48,me->y-FIXAMT,3*32,BLT_DEATHBEAM);
					FireBullet(me->x+FIXAMT*42,me->y-FIXAMT,3*32,BLT_DEATHBEAM);
				}
			}
			if(me->seq==ANIM_IDLE)
				me->facing=1;
			break;
		case 5:	// tongue
			if(me->frm==7 && me->frmTimer<128)
				MakeSound(SND_INCATONGUE,me->x,me->y,SND_CUTOFF,100);
			break;

	}
}

void AI_Incatongue(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->parent->seq==ANIM_DIE)
		me->type=MONS_NONE;	// vanish!

	if(me->action==ACTION_BUSY)
	{
		return;
	}

	me->x=me->parent->x;
	if(me->parent->seq==ANIM_A1 && me->parent->frm>5)
	{
		me->y=me->parent->y+FIXAMT*48;
		monsType[me->type].size=50;
		monsType[me->type].flags&=(~MF_INVINCIBLE);

	}
	else
	{
		me->y=me->parent->y-FIXAMT*32;
		monsType[me->type].flags|=MF_INVINCIBLE;
		monsType[me->type].size=5;
	}
}

void AI_PeepBomb(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(!goodguy)
		return;

	if(me->reload)
		me->reload--;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			me->frmAdvance=256;
			me->facing=0;
		}
		return;
	}

	if(me->mind==0)	// spinning, looking for goodguy
	{
		if(me->type==MONS_PEEPBOMB)
		{
			me->mind1++;
		}
		else
		{
			me->mind1--;

		}
		me->facing=me->mind1/8;	// pick the frame

		if(PeepAtKid(me->x,me->y,map,me->mind1))
		{
			MakeSound(SND_PEEPALARM,me->x,me->y,SND_CUTOFF|SND_ONE,3000);
			me->mind=1;
			me->dx=Cosine(me->mind1)*7;
			me->dy=Sine(me->mind1)*7;
			me->mind1=30;
		}
	}
	if(me->mind==1)	// chasing!
	{
		me->mind1--;
		if(!me->mind1)
		{
			me->mind=0;
			me->dx=0;
			me->dy=0;
			me->mind1=me->facing*8;
		}
		else
		{
			FaceGoodguy(me,goodguy);
			me->facing*=4;
			me->dx=Cosine(me->facing*8 )*7;
			me->dy=Sine(me->facing*8)*7;
			if(RangeToTarget(me,goodguy)<FIXAMT*64)
			{
				me->hp=0;
				me->facing=0;
				me->seq=ANIM_DIE;
				me->frmAdvance=256;
				me->frm=0;
				me->frmTimer=0;
				me->dx=0;
				me->dy=0;
				MakeSound(SND_BOMBBOOM,me->x,me->y,SND_CUTOFF,1200);
				goodguy->GetShot(-Cosine(me->facing*8)*20,-Sine(me->facing*8)*20,30,map,world);
				me->action=ACTION_BUSY;
			}
		}
	}
}

void AI_Trapazoid(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->type==MONS_TRAPAZOID2)
	{
		if(me->hp==0)
			return;	// don't sweat it if you dead

		if(goodguy && RangeToTarget(me,goodguy)<150*FIXAMT && me->reload==0)
		{
			FaceGoodguy(me,goodguy);
			me->type=MONS_TRAPAZOID;
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
			me->action=ACTION_BUSY;
			me->mind1=0;
			return;
		}
	}
	else
	{
		if(me->ouch==4)
		{
			if(me->hp>0)
				MakeSound(SND_SERPENTOUCH,me->x,me->y,SND_CUTOFF,1200);
			else
				MakeSound(SND_SERPENTDIE,me->x,me->y,SND_CUTOFF,1200);
		}

		if(me->action==ACTION_BUSY)
		{
			if(me->seq==ANIM_DIE && me->frm==6)
			{
				me->type=MONS_TRAPAZOID2;
				me->seq=ANIM_IDLE;
				me->frm=0;
				me->frmTimer=0;
			}
			if(me->seq==ANIM_ATTACK)
			{
				if(me->frm>=6 && me->frm<=8)
				{
					FireBullet(me->x,me->y,me->facing*32+(me->frm-6)*10,BLT_FLAME2);
				}
				if(me->frm==9)
				{
					me->frm=6;
					me->facing=(me->facing+1)&7;
					me->mind1++;
					if(me->mind1==8)
					{
						me->seq=ANIM_DIE;
						me->frm=0;
						me->frmTimer=0;
						me->frmAdvance=128;
					}
				}
			}

			return;
		}
	}
}

void AI_Whackazoid(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->type==MONS_TRAPAZOID2)
	{
		if(me->hp==0)
			return;	// don't sweat it if you dead
	}
	else
	{
		if(me->ouch==4)
		{
			if(me->hp>0)
				MakeSound(SND_SERPENTOUCH,me->x,me->y,SND_CUTOFF,1200);
			else
			{
				MakeSound(SND_SERPENTDIE,me->x,me->y,SND_CUTOFF,1200);
				WhackedAZoid(map);
			}
		}

		if(me->action==ACTION_BUSY)
		{
			if((me->seq==ANIM_DIE && me->frm==6) ||
				(me->seq==ANIM_ATTACK && me->frm==21))
			{
				me->type=MONS_TRAPAZOID2;
				me->seq=ANIM_IDLE;
				me->frm=0;
				me->frmTimer=0;
			}

			return;
		}
	}
}

void AI_Slug(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_SNAILDIE,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SNAILHIT,me->x,me->y,SND_CUTOFF,1200);
		me->facing=(me->facing+1-MGL_random(3))&7;
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE && MGL_random(7)==0)
		{
			FireBullet(me->x-FIXAMT*40+MGL_randoml(FIXAMT*81),me->y-FIXAMT*30+MGL_randoml(FIXAMT*61),0,BLT_SLIME);
		}
		return;	// can't do nothin' right now
	}

	if(me->mind1)	// bumped a wall
	{
		me->facing=(byte)MGL_random(8);
		me->mind1=0;
	}

	if(me->reload==0 && goodguy && RangeToTarget(me,goodguy)<800*FIXAMT)
	{
		me->reload=10;
		FireBullet(me->x,me->y,0,BLT_SLIME);
	}

	me->dx=Cosine(me->facing*32)*2;
	me->dy=Sine(me->facing*32)*2;

	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}

void AI_Snail(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int i,x,y;
	byte b;

	if(me->type==MONS_SNAIL2)
	{
		if(goodguy && RangeToTarget(me,goodguy)>(260*FIXAMT))
		{
			MakeSound(SND_SNAILOUTSHELL,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_A2;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
			me->action=ACTION_BUSY;
			me->type=MONS_SNAIL;
		}

		return;
	}

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_SNAILDIE,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SNAILHIT,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_A1 && me->frm==10)
		{
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_IDLE;
			me->type=MONS_SNAIL2;
		}

		if(me->seq==ANIM_ATTACK && me->frm>=3 && me->frm<=5)
		{
			// fire lots of acid
			for(i=0;i<2;i++)
			{
				x=me->x+Cosine(me->facing*32)*20-FIXAMT*4+MGL_randoml(FIXAMT*4+1);
				y=me->y+Sine(me->facing*32)*20-FIXAMT*4+MGL_randoml(FIXAMT*4+1);
				b=me->facing*32-4+MGL_random(9);
				FireExactBullet(x,y,FIXAMT*15,Cosine(b)*12,Sine(b)*12,FIXAMT*6,0,60,((b+16)&255)/32,BLT_ACID);
			}
		}
		if(me->seq==ANIM_DIE)
			me->frmAdvance=256;
		return;
	}

	if(goodguy && RangeToTarget(me,goodguy)<(240*FIXAMT))
	{
		// hide in your shell!
		MakeSound(SND_SNAILINSHELL,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_A1;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=256;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		me->reload=0;
		return;
	}

	if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if(goodguy)
		{
			FaceGoodguy2(me,goodguy);

			me->dx=Cosine(me->facing*32)*2;
			me->dy=Sine(me->facing*32)*2;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
			if(MGL_random(64)==0)
			{
				me->mind=1;		// occasionally wander
				me->mind1=1;
			}
		}
		else
		{
			me->mind=1;	// if there's no goodguy, get random
			me->mind1=1;
		}
	}
	else if(me->mind==1)	// random wandering
	{
		if(!(me->mind1--))	// time to get a new direction
		{
			if((goodguy) && MGL_random(3)==0)
				me->mind=0;	// get back on track
			else
				me->facing=(byte)MGL_random(8);
			me->mind1=MGL_random(40)+1;
		}

		me->dx=Cosine(me->facing*32)*2;
		me->dy=Sine(me->facing*32)*2;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}

	// spit if close enough and you want to
	if(goodguy && RangeToTarget(me,goodguy)<500*FIXAMT && me->reload==0)
	{
		FaceGoodguy(me,goodguy);
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		me->reload=60;
	}
}

void AI_Ball(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(goodguy && RangeToTarget(me,goodguy)<40*FIXAMT && me->dy>=0)
	{
		MakeSound(SND_BALLBOUNCE,me->x,me->y,SND_CUTOFF,200);
		if(me->dy==0)
			me->dy=-FIXAMT*4;
		else
			me->dy=-me->dy;
		me->dx=(goodguy->dx/4)+Cosine(goodguy->facing*32)*2;
	}

	if(me->mind1)
	{
		MakeSound(SND_BALLBOUNCE,me->x,me->y,SND_CUTOFF,200);
		if(me->mind1&1)	// horiz hit
		{
			me->dx=-me->dx;
			me->dy+=-FIXAMT/16+MGL_random(FIXAMT/8+1);
		}
		if(me->mind1&2)	// vert hit
		{
			me->dy=-me->dy;
			me->dx+=-FIXAMT/16+MGL_random(FIXAMT/8+1);
		}
		me->mind1=0;
	}

	if(world->terrain[map->map[me->mapx+me->mapy*map->width].floor].flags&(TF_WATER|TF_LAVA))
	{
		FireBullet(me->x,me->y,0,BLT_BOOM);
		me->type=MONS_NONE;
		goodguy->GetShot(0,0,99999,map,world);
	}

	Clamp(&me->dx,FIXAMT*5);
	Clamp(&me->dy,FIXAMT*5);
}

void AI_OctoBoss(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int i,x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_INCAHIT,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_INCADIE,me->x,me->y,SND_CUTOFF,1200);
	}

	for(i=0;i<8;i++)
	{
		x=me->x/FIXAMT-60+MGL_random(121);
		y=me->y/FIXAMT-60+MGL_random(121);
		WaterRipple(x,y,MGL_random(32*20));
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm>=4 && me->frm<=11)
		{
			x=me->x-FIXAMT*30+MGL_randoml(60*FIXAMT+1);
			y=me->y+FIXAMT*70;
			FireBullet(x,y,2*32-14+MGL_random(29),BLT_ENERGY);
			me->reload=60;
		}
		if(me->seq==ANIM_DIE && me->frm==5 && me->frmTimer<128)
			OctoBossDeath(map);

		return;	// can't do nothin' right now
	}

	if(goodguy)
	{
		x=goodguy->x;
		y=goodguy->y-100*FIXAMT;

		FaceGoodguy2(me,goodguy);
		if(me->x<x)
			me->dx+=FIXAMT/8;
		if(me->y<y)
			me->dy+=FIXAMT/8;
		if(me->x>x)
			me->dx-=FIXAMT/8;
		if(me->y>y)
			me->dy-=FIXAMT/8;

		if(me->mind1)
		{
			me->mind1=0;
			me->dy-=FIXAMT/4;	// move up if bumping an obstacle
		}
		Clamp(&me->dx,FIXAMT*3);
		Clamp(&me->dy,FIXAMT*3);

		if(me->reload==0 && (abs(me->x-x)<150*FIXAMT))	// within a reasonable horiz range
		{
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
		}
	}
}

void AI_OctoTentacle(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(!me->parent || me->parent->hp==0)
	{
		me->GetShot(0,0,255,map,world);
	}

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_INCAHIT,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_INCADIE,me->x,me->y,SND_CUTOFF,1200);
	}

	WaterRipple(me->x/FIXAMT,me->y/FIXAMT,MGL_random(32*20));

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==9 && me->reload==0)
		{
			if(!FindGoodVictim((me->x+Cosine(me->facing*32)*160)/FIXAMT,(me->y+Sine(me->facing*32)*100)/FIXAMT,60,
				Cosine(me->facing*32)*10,Sine(me->facing*32)*10,8,map,world))
			{
				FindGoodVictim((me->x+Cosine(me->facing*32)*40)/FIXAMT,(me->y+Sine(me->facing*32)*30)/FIXAMT,80,
					Cosine(me->facing*32)*10,Sine(me->facing*32)*10,8,map,world);
			}
			me->reload=4;
		}

		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(200*FIXAMT) && MGL_random(8)==0)
			{
				// get him!
				FaceGoodguy(me,goodguy);
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
			FaceGoodguy2(me,goodguy);

			me->dx=Cosine(me->facing*32)*4;
			me->dy=Sine(me->facing*32)*4;
			if(MGL_random(20)==0)
			{
				me->mind=1;		// occasionally wander
				me->mind1=10;
			}
		}
		else
		{
			me->mind=1;	// if there's no goodguy, get random
			me->mind1=10;
		}
	}
	else if(me->mind==1)	// random wandering
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(200*FIXAMT) && MGL_random(8)==0)
			{
				// get him!
				FaceGoodguy(me,goodguy);
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
		if(!(me->mind1--))	// time to get a new direction
		{
			if((goodguy) && MGL_random(7)==0)
				me->mind=0;	// get back on track
			else
				me->facing=(byte)MGL_random(8);
			me->mind1=MGL_random(40)+1;
			me->dx=Cosine(me->facing*32)*4;
			me->dy=Sine(me->facing*32)*4;
		}
	}

	if(me->y<me->parent->y-FIXAMT*140)
	{
		me->dy=FIXAMT*4;
		me->mind=1;
		me->mind1=10;
	}
	if(me->y>me->parent->y+FIXAMT*140)
	{
		me->dy=-FIXAMT*4;
		me->mind=1;
		me->mind1=10;
	}
	if(me->type==MONS_OCTOTENT)	// left-side tentacle
	{
		if(me->x<me->parent->x-FIXAMT*220)
		{
			me->dx=FIXAMT*4;
			me->mind=1;
			me->mind1=10;
		}
		if(me->x>me->parent->x-FIXAMT*30)
		{
			me->dx=-FIXAMT*4;
			me->mind=1;
			me->mind1=10;
		}
	}
	else
	{
		if(me->x>me->parent->x+FIXAMT*220)
		{
			me->dx=-FIXAMT*4;
			me->mind=1;
			me->mind1=10;
		}
		if(me->x<me->parent->x+FIXAMT*30)
		{
			me->dx=FIXAMT*4;
			me->mind=1;
			me->mind1=10;
		}
	}

	if(RangeToTarget(me,me->parent)<FIXAMT*100)
	{
		if(me->x<me->parent->x)
			me->dx-=FIXAMT*6;
		if(me->x>me->parent->x)
			me->dx+=FIXAMT*6;
		if(me->y<me->parent->y)
			me->dy-=FIXAMT*6;
		if(me->y>me->parent->y)
			me->dy+=FIXAMT*6;
		me->mind=1;
		me->mind1=10;
	}
}

void AI_Rammy(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_GOATHIT,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_GOATDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(!goodguy)
		return;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_MOVE)
		{
			if(me->mind<20)
			{
				me->mind++;
				me->mind1=0;	// don't let him bump stuff when slow
			}
			else
				if(FindGoodVictim((me->x+Cosine(me->facing*32)*40)/FIXAMT,(me->y+Sine(me->facing*32)*30)/FIXAMT,40,
						Cosine(me->facing*32)*10,Sine(me->facing*32)*10,12,map,world) || me->mind1)
				{
					// make THUD noise
					MakeSound(SND_GOATDIZZY,me->x,me->y,SND_CUTOFF,1200);
					me->type=MONS_GOAT1B;	// become vulnerable
					me->seq=ANIM_A1;	// get stunned
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=128;
					me->action=ACTION_BUSY;
					me->dx=0;
					me->dy=0;
					ShakeScreen(10);
					me->mind1=0;
					return;
				}

			me->dx+=Cosine(me->facing*32)/2;
			me->dy+=Sine(me->facing*32)/2;
			Clamp(&me->dx,FIXAMT*8);
			Clamp(&me->dy,FIXAMT*8);
			if(me->reload==0)
			{
				me->reload=10;
				FaceGoodguy2(me,goodguy);
			}
			if(me->frm==12)
			{
				me->frm=0;
				MakeSound(SND_GOATRUN,me->x,me->y,SND_CUTOFF,1200);
			}
			if(MGL_random(3)==0)
				BlowWigglySmoke(me->x-me->dx-FIXAMT*12+MGL_randoml(FIXAMT*24+1),me->y-me->dy-FIXAMT*12+MGL_randoml(FIXAMT*24+1),0,FIXAMT);
		}
		if(me->seq==ANIM_A1 && me->frm==22)
		{
			me->type=MONS_GOAT1;	// invulnerable again
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_IDLE;
		}
		return;	// can't do nothin' right now
	}

	FaceGoodguy(me,goodguy);

	if(MGL_random(48)==0)
	{
		MakeSound(SND_GOATRUN,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		me->reload=10;
		me->mind=0;
		return;
	}
}

void AI_Hoppy(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(!goodguy)
		return;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_GOATHIT,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_GOATDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->type==MONS_GOAT2B)
		me->action=ACTION_BUSY;

	if(me->action==ACTION_BUSY)
	{
		if(me->z>0 )
			me->dz+=FIXAMT/4;
		if(me->seq==ANIM_ATTACK && me->frm==3)
		{
			me->type=MONS_GOAT2B;
			me->dz=FIXAMT*20;
			me->dx=Cosine(me->facing*32)*5;
			me->dy=Sine(me->facing*32)*5;
			me->z=FIXAMT;
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}

		if(me->z==0 && me->type==MONS_GOAT2B && me->seq==ANIM_IDLE)
		{
			// thud sound again
			MakeSound(SND_BOMBBOOM,me->x,me->y,SND_CUTOFF,1200);
			ShakeScreen(10);
			me->type=MONS_GOAT2;
			me->seq=ANIM_A1;	// squish onto the ground
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->dx=0;
			me->dy=0;
			me->mind1=0;
			FireBullet(me->x,me->y,0,BLT_SHOCKWAVE);
		}

		return;	// can't do nothin' right now
	}

	if(RangeToTarget(me,goodguy)<(300*FIXAMT) && MGL_random(16)==0)
	{
		// get him!
		FaceGoodguy(me,goodguy);
		MakeSound(SND_GOATSHOOT,me->x,me->y,SND_CUTOFF,1200);
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

	if(!me->mind1)	// time to get a new direction
	{
		me->facing=(byte)MGL_random(8);
		me->mind1=MGL_random(40)+1;
		me->dx=Cosine(me->facing*32)*4;
		me->dy=Sine(me->facing*32)*4;
	}
	else
		me->mind1--;

	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}

void AI_Gruffy(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_GOATHIT,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_GOATDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==2 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*24;
			y=me->y+Sine(me->facing*32)*24;
			FireBullet(x,y,(byte)(me->facing*32-8+MGL_random(17)),BLT_BIGYELLOW);
			me->reload=5;
			me->mind1=1;
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(512*FIXAMT) && MGL_random(64)==0)
			{
				// spit at him
				MakeSound(SND_GOATSHOOT,me->x,me->y,SND_CUTOFF,1200);
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

			FaceGoodguy2(me,goodguy);

			me->dx=Cosine(me->facing*32)*4;
			me->dy=Sine(me->facing*32)*4;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
			if(MGL_random(64)==0)
			{
				me->mind=1;		// occasionally wander
				me->mind1=1;
			}
		}
		else
		{
			me->mind=1;	// if there's no goodguy, get random
			me->mind1=1;
		}
	}
	else if(me->mind==1)	// random wandering
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(512*FIXAMT) && MGL_random(64)==0)
			{
				// spit at him
				MakeSound(SND_GOATSHOOT,me->x,me->y,SND_CUTOFF,1200);
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
		}
		if(!(me->mind1--))	// time to get a new direction
		{
			if((goodguy) && MGL_random(10)==0)
				me->mind=0;	// get back on track
			else
				me->facing=(byte)MGL_random(8);
			me->mind1=MGL_random(40)+1;
		}

		me->dx=Cosine(me->facing*32)*4;
		me->dy=Sine(me->facing*32)*4;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
}

void AI_StickShroom(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_GINGEROUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_GINGERDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK)
		{
			i=(me->facing*32-8+MGL_random(17))&255;
			if(i<0)
				i+=256;
			x=me->x+Cosine(i)*4;
			y=me->y+Sine(i)*4;
			FireExactBullet(x,y,FIXAMT*20,Cosine(i)*8,Sine(i)*8,0,0,16,i,BLT_SPORE);
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, charge towards Bouapha
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(128*FIXAMT) && MGL_random(8)==0)
			{
				// get him!
				MakeSound(SND_GINGERCOMBO,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				return;
			}
			FaceGoodguy2(me,goodguy);

			me->dx=Cosine(me->facing*32)*3;
			me->dy=Sine(me->facing*32)*3;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
			if(MGL_random(128)==0)
			{
				me->mind=1;		// occasionally wander
				me->mind1=(byte)MGL_random(100);
			}
		}
		else
		{
			me->mind=1;	// if there's no goodguy, sit around
			me->mind1=(byte)MGL_random(100);
		}
	}
	else if(me->mind==1)	// random wandering
	{
		if(goodguy && RangeToTarget(me,goodguy)<(128*FIXAMT) && MGL_random(8)==0)
		{
			// get him!
			MakeSound(SND_GINGERCOMBO,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			return;
		}
		FaceGoodguy2(me,goodguy);

		if(!(me->mind1--))	// time to get a new direction
		{
			if((goodguy) && MGL_random(10)==0)
				me->mind=0;	// get back on track
			else
				me->facing=(byte)MGL_random(8);
			me->mind1=MGL_random(40)+1;
		}

		me->dx=Cosine(me->facing*32)*3;
		me->dy=Sine(me->facing*32)*3;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
}

void AI_StickSpider(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SPD2OUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SPD2DIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->reload==0)
		{
			i=(me->facing*32-8+MGL_random(17))&255;
			if(i<0)
				i+=256;
			x=me->x+Cosine(i)*4;
			y=me->y+Sine(i)*4;
			FireBullet(x,y,i,BLT_ACID);
			me->reload=2;
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, charge towards Bouapha
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(128*FIXAMT) && MGL_random(8)==0)
			{
				// get him!
				MakeSound(SND_GINGERCOMBO,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				return;
			}
			FaceGoodguy2(me,goodguy);

			me->dx=Cosine(me->facing*32)*3;
			me->dy=Sine(me->facing*32)*3;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
			if(MGL_random(128)==0)
			{
				me->mind=1;		// occasionally wander
				me->mind1=(byte)MGL_random(100);
			}
		}
		else
		{
			me->mind=1;	// if there's no goodguy, sit around
			me->mind1=(byte)MGL_random(100);
		}
	}
	else if(me->mind==1)	// random wandering
	{
		if(goodguy && RangeToTarget(me,goodguy)<(128*FIXAMT) && MGL_random(8)==0)
		{
			// get him!
			MakeSound(SND_GINGERCOMBO,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			return;
		}
		FaceGoodguy2(me,goodguy);

		if(!(me->mind1--))	// time to get a new direction
		{
			if((goodguy) && MGL_random(10)==0)
				me->mind=0;	// get back on track
			else
				me->facing=(byte)MGL_random(8);
			me->mind1=MGL_random(40)+1;
		}

		me->dx=Cosine(me->facing*32)*3;
		me->dy=Sine(me->facing*32)*3;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
}

void AI_StickCorpse(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SKELOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SKELDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK)
		{
			i=(me->facing*32-8+MGL_random(17))&255;
			if(i<0)
				i+=256;
			x=me->x+Cosine(i)*4;
			y=me->y+Sine(i)*4;
			FireExactBullet(x,y,FIXAMT*50,Cosine(i)*8,Sine(i)*8,-FIXAMT,0,50,i,BLT_ENERGY);
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, charge towards Bouapha
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(256*FIXAMT) && MGL_random(16)==0)
			{
				// get him!
				MakeSound(SND_SKELSHOOT,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				return;
			}
			FaceGoodguy2(me,goodguy);

			me->dx=Cosine(me->facing*32)*4;
			me->dy=Sine(me->facing*32)*4;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
			if(MGL_random(10)==0)
			{
				me->mind=1;		// occasionally wander
				me->mind1=(byte)MGL_random(100);
			}
		}
		else
		{
			me->mind=1;	// if there's no goodguy, sit around
			me->mind1=(byte)MGL_random(100);
		}
	}
	else if(me->mind==1)	// random wandering
	{
		if(goodguy && RangeToTarget(me,goodguy)<(256*FIXAMT) && MGL_random(16)==0)
		{
			// get him!
			FaceGoodguy2(me,goodguy);
			MakeSound(SND_SKELSHOOT,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			return;
		}

		if(!(me->mind1--))	// time to get a new direction
		{
			if((goodguy) && MGL_random(40)==0)
				me->mind=0;	// get back on track
			else
				me->facing=(byte)MGL_random(8);
			me->mind1=MGL_random(40)+1;
		}

		me->dx=Cosine(me->facing*32)*4;
		me->dy=Sine(me->facing*32)*4;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
}

void AI_StickBat(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	byte f;
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_BATOUCH,me->x,me->y,SND_CUTOFF,1200);
	}
	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK)
		{
			x=me->x+Cosine(me->facing*32)*4;
			y=me->y+Sine(me->facing*32)*4;
			f=me->facing*32-8+MGL_random(17);
			FireExactBullet(x,y,me->z,Cosine(f)*8,Sine(f)*8,0,0,24-MGL_random(4),f,BLT_FLAME2);
			MakeSound(SND_FLAMEGO,me->x,me->y,SND_CUTOFF,200);
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly zip towards Bouapha
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(128*FIXAMT) && MGL_random(4)==0)
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
				return;
			}

			FaceGoodguy(me,goodguy);

			me->dx=Cosine(me->facing*32)*6;
			me->dy=Sine(me->facing*32)*6;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
			if(MGL_random(16)==0)
			{
				me->mind=1;		// occasionally wander
				me->mind1=1;
			}
		}
		else
		{
			me->mind=1;	// if there's no goodguy, get random
			me->mind1=1;
		}
	}
	else if(me->mind==1)	// random wandering
	{
		if(!(me->mind1--))	// time to get a new direction
		{
			if((goodguy) && MGL_random(3)==0)
				me->mind=0;	// get back on track
			else
				me->facing=(byte)MGL_random(8);
			me->mind1=MGL_random(40)+1;
		}

		me->dx=Cosine(me->facing*32)*6;
		me->dy=Sine(me->facing*32)*6;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=110;
		}
	}
}

void DoMove(Guy *me,byte move,int dx,int dy)
{
	me->seq=move;
	me->frm=0;
	me->frmTimer=0;
	me->frmAdvance=128;
	me->action=ACTION_BUSY;
	me->dx=dx;
	me->dy=dy;
}

void AI_Dancer(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SKELOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SKELDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		return;	// can't do nothin' right now
	}
	switch(me->mind)
	{
		case 0:
		case 12:
			DoMove(me,ANIM_MOVE,-FIXAMT,0);
			break;
		case 1:
		case 2:
		case 4:
		case 8:
		case 10:
			DoMove(me,ANIM_ATTACK,0,0);
			break;
		case 3:
		case 11:
			DoMove(me,ANIM_MOVE,FIXAMT,0);
			break;
		case 5:
		case 9:
			DoMove(me,ANIM_MOVE,0,-FIXAMT);
			break;
		case 6:
		case 7:
			DoMove(me,ANIM_MOVE,0,FIXAMT);
			break;
	}
	me->mind++;
	if(me->mind==13)
		me->mind=0;
}

#include "monster.h"
#include "player.h"

monsterType_t monsType[NUM_MONSTERS]=
	{
		{"Null",
		 0,0,0,0,"",0},
		{"Bouapha",
		 11,29,128,0,"graphics/bouapha.jsp",0,MF_WATERWALK,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,15,16,255},	// attack
				{0,0,0,0,0,0,0,26,27,28,255},		// die
				{23,24,25,24,23,255},	// use item
				{17,18,19,20,21,22,21,20,21,22,21,20,19,18,17,255},	// bored
				{1,3,1,0,4,6,4,0,1,3,1,0,0,0,26,27,28,255},	// watery death
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
		 20,19,150,1000,"graphics/eggsac.jsp",0,MF_ONEFACE|MF_ENEMYWALK|MF_NOMOVE,
			{
				{0,0,0,0,0,0,0,0,1,2,1,0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,0,255},	// attack
				{19,20,21,22,23,24,25,255},	// die
			}},
		{"Mama Spider",
		 30,30,200,1500,"graphics/mamaspdr.jsp",0,MF_NOMOVE,
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
		{"Matilda",	// the head of matilda
		 40,11,400,1000,"graphics/mathead.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_FACECMD|MF_NOGRAV|MF_ENEMYWALK|MF_FREEWALK,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,4,4,4,4,4,4,3,2,1,255},	// fire
				{1,2,0,3,4,3,2,0,3,4,3,0,1,4,2,4,3,1,2,255},	// die
			}},
		{"Matilda's Skull",	// the skull of matilda (head becomes this when killed)
		 40,11,200,1000,"graphics/matskull.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_FACECMD|MF_NOGRAV|MF_ENEMYWALK|MF_FREEWALK,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,4,4,4,4,4,4,3,2,1,255},	// fire
				{1,2,0,3,4,3,2,0,3,4,3,0,1,4,2,4,3,1,2,255},	// die
			}},
		{"Matilda's Brain",	// the brain of matilda (skull becomes this when killed)
		 40,7,100,5000,"graphics/matbrain.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_FACECMD|MF_NOGRAV|MF_ENEMYWALK|MF_FREEWALK,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// no form of attack
				{1,2,3,2,1,0,4,5,6,5,4,0,1,2,3,2,1,0,4,5,6,5,255},	// die
			}},
		{"Matilda's Body",	// the body of matilda
		 50,7,1,1,"graphics/matbody.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_INVINCIBLE|MF_NOGRAV|MF_ENEMYWALK,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{0,255},	// fire
				{1,2,0,3,4,3,2,0,3,4,3,0,1,4,2,4,3,1,2,255},	// die
			}},
		{"Matilda's Claw",	// the left claw (her right)
		 20,16,1000,1000,"graphics/matclaw1.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_SPRITEBOX|MF_NOGRAV|MF_ENEMYWALK|MF_FREEWALK,
			{
				{0,1,2,3,2,1,255},	// idle
				{0,255},	// move
				{7,8,9,10,11,12,13,14,15,255},	// claw slash
				{1,2,0,3,4,3,2,0,3,4,3,0,1,4,2,4,3,1,2,255},	// die
				{4,5,6,6,6,6,6,6,6,5,4,255},	// block (A1)
			}},
		{"Matilda's Claw",	// the right claw (her left)
		 20,16,1000,1000,"graphics/matclaw2.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_SPRITEBOX|MF_NOGRAV|MF_ENEMYWALK|MF_FREEWALK,
			{
				{0,1,2,3,2,1,255},	// idle
				{0,255},	// move
				{7,8,9,10,11,12,13,14,15,255},	// claw slash
				{1,2,0,3,4,3,2,0,3,4,3,0,1,4,2,4,3,1,2,255},	// die
				{4,5,6,6,6,6,6,6,6,5,4,255},	// block (A1)
			}},
		{"Matilda's Tail",	// her tail
		 80,16,200,1000,"graphics/mattail.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_INVINCIBLE|MF_FACECMD|MF_NOGRAV|MF_ENEMYWALK|MF_FREEWALK,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,1,2,3,2,1,255},	// fire
				{1,2,0,3,2,0,3,3,0,1,2,3,1,2,255},	// die
			}},
		{"Ninjabread Man",
		 12,33,50,300,"graphics/ginger.jsp",0,0,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,255},	// move
				{7,8,9,10,11,12,13,14,15,16,17,18,19,20,19,20,19,20,21,22,23,255},	// triple punch
				{27,28,29,30,31,32,32,32,32,255},	// die
				{24,25,26,26,26,26,26,255},	// jumpkick (A1)
				{25,24,255}	// unjumpkick (A2)
			}},
		{"PUMPKIN!",
		 11,6,5,500,"graphics/pumpkin.jsp",0,0,
			{
				{0,255},	// idle
				{1,2,3,4,5,0,255},	// move
				{0,255},	// attack unused
				{0,0,255},	// die (just long enough to spew shrapnel)
			}},
		{"Thingie",
		 12,44,10,50,"graphics/babything.jsp",0,0,
			{
				{12,255},	// idle
				{1,2,3,4,5,6,7,8,9,10,11,255},	// move
				{20,21,22,23,24,25,26,27,255},	// attack
				{34,35,36,37,38,39,40,41,42,43,255},	// die
				{13,14,15,16,17,16,15,16,17,16,15,16,17,16,15,255},	// fall asleep (A1)
				{18,19,18,255},	// blink (A2)
				{28,29,30,29,28,255}, // look right (A3)
				{31,32,33,32,31,255}, // look left (A4)
			}},
		{"Mucho Moss",
		 15,17,10,25,"graphics/moss.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_FREEWALK|MF_ENEMYWALK|MF_WALLWALK|MF_NOSHADOW,
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
		{"Shroom",
		 15,23,10,50,"graphics/shroom.jsp",0,0,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,255},	// attack
				{15,16,17,18,19,20,21,22,255},	// die
			}},
		{"Mush the Shroom",
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
		{"Super Zombie",
		 36,38,290,2500,"graphics/suprzmbe.jsp",0,MF_NOMOVE,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{17,18,19,20,21,22,23,24,25,255},	// attack=punch
				{9,10,11,12,13,14,15,15,15,15,15,15,255},	// die
				{7,8,9,10,11,11,11,12,12,13,13,13,14,14,14,15,16,16,255},	// A1=leap
				{26,27,28,29,30,255},	// A2=attempted grab
				{27,31,32,33,34,35,34,36,37,36,34,35,34,36,37,36,34,35,34,33,31,27,255}	//A3= grab n' pound
			}},
		{"Happy Stick Man",
		 16,12,100,1000,"graphics/stickman.jsp",0,MF_ONEFACE,
			{
				{0,255},	// idle
				{1,0,2,0,255},	// move
				{10,11,11,10,255},	// attack=chomp
				{3,3,3,3,3,4,5,255},	// die
				{6,7,7,7,6,0,8,9,9,9,8,255},	// A1=show off muscles
				{3,255},	// A2=sadness
			}},
		{"Baby SEAL",
		 22,20,20,125,"graphics/babyseal.jsp",0,0,
			{
				{0,255},	// idle
				{1,2,3,4,5,0,255},	// move
				{6,6,7,8,7,8,9,10,9,10,11,12,11,12,13,13,13,14,14,14,255},	// attack=full auto
				{15,16,17,18,19,19,19,255},	// die
			}},
		{"Cryozoid",
		 15,23,20,75,"graphics/isozoid.jsp",0,MF_NOMOVE,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,255},	// move
				{10,11,12,13,14,15,16,255},	// attack=spit snow
				{17,18,19,20,21,22,255},	// die
				{4,5,6,7,8,9,255},	//A1=rise out of the snow
				{254,255},			//A2=idle under the ground
			}},
		{"Snowguy",
		 26,36,40,200,"graphics/snowguy.jsp",0,0,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,0,255},	// move
				{24,25,26,27,28,29,255},	// attack=slash
				{30,31,32,33,34,35,254,35,254,35,254,35,254,254,35,255},	// die
				{8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,255},	//A1=throw snowball
			}},
		{"Pengulon",
		 10,8,5,50,"graphics/penguin.jsp",0,MF_FLYING,
			{
				{0,1,2,3,4,5,4,3,2,1,255},	// idle
				{0,1,2,3,4,5,4,3,2,1,255},	// move
				{6,7,7,7,6,6,3,3,3,3,3,255},	// attack=dive
				{0,254,3,254,5,254,1,254,4,254,2,254,3,255},	// die
			}},
		{"Zomboni",
		 48,9,100,500,"graphics/zomboni.jsp",0,MF_NOMOVE,
			{
				{0,255},	// idle
				{0,1,2,0,1,2,255},	// move
				{0,255},	// attack=unused
				{3,4,5,5,5,6,7,8,8,255},	// die
			}},
		{"Sven",
		 48,40,500,5000,"graphics/yeti.jsp",0,MF_NOMOVE,
			{
				{0,255},	// idle
				{0,1,2,1,0,3,4,3,255},	// move
				{14,15,16,17,18,19,20,21,22,23,255},	// attack=snowball
				{35,36,37,38,39,39,39,39,39,39,39,39,39,39,39,39,39,39,255},	// die
				{28,29,30,31,32,33,33,33,34,34,255},	// A1=ground pound
				{5,6,7,8,9,10,11,12,13,255},	//A2=death yodel
				{24,25,26,27,26,25,26,27,26,25,26,27,26,25,24,255},	// A3=wave hello
			}},
		{"Bjorn",
		 48,40,500,5000,"graphics/yeti.jsp",0,MF_NOMOVE,
			{
				{0,255},	// idle
				{0,1,2,1,0,3,4,3,255},	// move
				{14,15,16,17,18,19,20,21,22,23,255},	// attack=snowball
				{35,36,37,38,39,39,39,39,39,39,39,39,39,39,39,39,39,39,255},	// die
				{28,29,30,31,32,33,33,33,34,34,255},	// A1=ground pound
				{5,6,7,8,9,10,11,12,13,255},	//A2=death yodel
				{24,25,26,27,26,25,26,27,26,25,26,27,26,25,24,255},	// A3=wave hello
			}},
		{"Geozoid",
		 15,20,20,75,"graphics/geozoid.jsp",0,MF_NOMOVE,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,255},	// move
				{4,5,6,7,8,9,10,11,12,255},	// attack=spit rock
				{13,14,15,16,17,18,19,255},	// die
			}},
		{"Mumble",
		 15,28,30,100,"graphics/mumble.jsp",0,MF_NOMOVE,
			{
				{0,255},	// idle
				{1,2,3,4,5,0,6,7,8,9,10,0,255},	// move
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{19,20,21,22,23,24,25,26,27,255},	// die
			}},
		{"Djinni",
		 12,24,10,50,"graphics/djinni.jsp",0,MF_WALLWALK|MF_NOSHADOW|MF_FLYING|MF_GHOST|MF_ENEMYWALK|MF_FREEWALK,
			{
				{0,1,2,3,2,1,255},	// idle
				{0,1,2,3,2,1,255},	// move
				{12,13,14,15,16,17,18,19,255},	// attack=punch
				{20,21,22,23,255},	// die
				{4,5,6,7,8,9,8,7,6,7,8,9,8,7,6,7,8,9,10,11,255},	// A1=scare
				{23,22,21,20,255},	// A2=materialize
			}},
		{"Magic Lamp",
		 15,10,150,500,"graphics/lamp.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_ENEMYWALK,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,7,8,0,1,2,3,4,5,6,7,8,0,1,255},	// attack=summon Djinni
				{1,5,8,3,7,2,0,8,254,1,254,7,254,5,254,8,254,2,255},	// die
			}},
		{"Cacton",
		 20,25,25,100,"graphics/cactus.jsp",0,0,
			{
				{0,0,0,13,14,15,15,14,13,0,0,0,16,17,18,18,17,16,0,255},	// idle
				{3,4,5,4,3,255},	// move
				{6,7,8,8,9,10,11,12,255},	// attack=shoot spines
				{19,20,21,21,22,23,24,24,254,24,254,24,254,24,254,24,255},	// die
				{1,2,3,255},	// A1=begin moving
				{3,2,1,255},	// A2=stop moving
			}},
		{"Roly Poly",
		 35,11,2000,1000,"graphics/roller.jsp",0,MF_NOMOVE|MF_ONEFACE,
			{
				{0,255},	// idle
				{1,2,3,4,5,0,255},	// move (roll down the screen)
				{5,4,3,2,1,0,255},	// attack=roll up the screen
				{0,0,255},	// die
				{6,7,8,9,10,0,255},	// A1=roll to the right
				{10,9,8,7,6,0,255},	// A2=roll to the left
			}},
		{"Richie Lich",
		 50,8,500,1500,"graphics/lich.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_GHOST|MF_NOSHADOW|MF_FLYING,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,4,4,4,4,4,4,4,4,4,4,4,4,3,2,1,255},	// attack=open mouth
				{0,5,6,7,8,255},	// die
			}},
		{"Dust Devil",
		 50,23,600,2000,"graphics/dustdevil.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_GHOST|MF_NOSHADOW,
			{
				{0,1,2,3,4,5,255},	// idle
				{0,1,2,3,4,5,255},	// move
				{6,7,8,9,10,11,12,13,14,15,16,255},	// attack=swipe pitchfork
				{17,18,19,20,21,22,255},	// die
			}},
		{"MechaBouapha",
		 20,24,500,1000,"graphics/mechabouapha.jsp",0,0,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,255},	// attack=hammer toss
				{15,16,17,18,19,20,21,22,23,255},	// die
				{15,16,17,18,19,20,21,22,255},	// A1=melt
				{23,255},	// A2=ball form
				{22,21,20,19,18,17,16,15,255},	// A3=unmelt
			}},
		{"Sphinx Arm",	// arm1
		 30,10,1000,2000,"graphics/sphinxarm1.jsp",0,MF_ONEFACE|MF_NOMOVE|MF_SPRITEBOX,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,7,7,8,9,255},	// attack=swipe
				{0,1,2,3,2,3,4,3,2,3,4,3,254,4,254,3,254,4,255},	// die
			}},
		{"Sphinx Arm",	// arm2
		 30,10,1000,2000,"graphics/sphinxarm2.jsp",0,MF_ONEFACE|MF_NOMOVE|MF_SPRITEBOX,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,7,7,8,9,255},	// attack=swipe
				{0,1,2,3,2,3,4,3,2,3,4,3,254,4,254,3,254,4,255},	// die
			}},
		{"Sphinxter",
		 80,11,2000,5000,"graphics/sphinx.jsp",0,MF_ONEFACE|MF_NOMOVE,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,255},	// attack=summon mummy
				{10,10,10,10,10,10,10,10,10,255},	// die
				{4,5,6,7,7,6,7,7,7,6,7,8,9,10,255}, // A1=sneeze
			}},
		{"Freakazoid",
		 15,18,15,75,"graphics/freakazoid.jsp",0,MF_AQUATIC|MF_WATERWALK,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,255},	// move
				{4,5,6,7,8,9,9,9,9,10,255},	// attack=spit spines
				{11,12,13,14,15,16,17,255},	// die
			}},
		{"Centipumpkin",	// body
		 14,5,20,200,"graphics/cpbody.jsp",0,MF_ENEMYWALK,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{0,255},	// attack
				{0,0,0,255},	// die
			}},
		{"Centipumpkin",	// head
		 14,1,40,500,"graphics/cphead.jsp",0,MF_ENEMYWALK,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{0,0,0,255},	// die
			}},
		{"Wacko",
		 12,21,20,100,"graphics/wacko.jsp",0,0,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,11,10,9,10,11,12,11,10,9,10,11,12,13,14,255},	// attack=stab
				{15,16,17,18,19,20,254,20,254,20,254,20,254,20,255},	// die
			}},
		{"The Boiler",
		 100,22,1400,2000,"graphics/boiler.jsp",0,MF_NOMOVE|MF_ONEFACE,
			{
				{0,1,2,3,4,5,6,7,255},	// idle
				{0,255},	// move
				{8,9,10,11,12,13,14,15,14,13,12,11,10,9,8,255},	// attack=flames
				{16,17,18,19,20,21,255},	// die
			}},
		{"Great Pumpkin",
		 120,13,2000,4000,"graphics/greatpk.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_ENEMYWALK,
			{
				{0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,8,255},	// idle
				{0,255},	// move
				{1,2,3,3,4,5,6,6,7,8,255},	// attack=vomit babies
				{9,9,10,10,11,11,12,12,255},	// die
			}},
		{"The Ultrazoid",
		 40,17,750,2500,"graphics/ultrazoid.jsp",0,MF_NOMOVE|MF_ONEFACE,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,7,8,9,255},	// attack=breathe
				{10,11,12,13,14,15,16,255},	// die
			}},
		{"Dr. Lunatic",
		 40,21,50,10000,"graphics/dr-l.jsp",0,0,
			{
				{0,255},	// idle
				{7,8,9,8,7,0,10,11,12,11,10,0,255},	// move
				{1,2,3,4,5,6,5,4,5,6,5,6,5,4,3,4,3,4,5,4,3,2,1,255},	// attack=laugh
				{13,14,15,16,17,17,17,17,17,17,17,18,19,20,20,20,255},	// die
			}},
		{"Super Duper Zombie",
		 40,48,1500,25000,"graphics/sdzl.jsp",0,MF_NOMOVE|MF_ONEFACE,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move (to the right)
				{28,29,30,31,31,31,31,31,32,33,34,35,36,37,38,39,255},	// attack=slash
				{40,41,42,43,43,43,43,43,44,45,46,47,47,47,47,47,47,255},	// die
				{5,6,5,0,7,8,7,0,255},	// A1=move left
				{9,10,11,12,12,12,12,12,12,13,14,15,16,16,17,18,255},	// A2=breathe
				{19,20,21,21,22,23,24,25,25,25,25,25,25,26,27,255},	// A3=ground pound
			}},
		{"Santa Claus",
		 16,25,100,500,"graphics/santa.jsp",0,0,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,15,16,17,18,19,255},	// attack=smack
				{20,21,22,23,24,24,24,254,24,254,24,254,24,254,24,255},	// die
			}},

	};

void InitMonsters(void)
{
	int i;

	for(i=0;i<NUM_MONSTERS;i++)
	{
		monsType[i].spr=NULL;
	}
	// just keep bouapha perma-loaded
	monsType[MONS_BOUAPHA].spr=new sprite_set_t(monsType[MONS_BOUAPHA].sprName);
}

void ExitMonsters(void)
{
	int i;

	for(i=1;i<NUM_MONSTERS;i++)
	{
		if(i==MONS_BJORN)
			monsType[i].spr=NULL;	// he uses the same sprites as MONS_SVEN
		if(monsType[i].spr)
			delete monsType[i].spr;
		monsType[i].spr=NULL;
	}
}

void PurgeMonsterSprites(void)
{
	int i;

	// note this starts at 2, skipping bouapha
	for(i=2;i<NUM_MONSTERS;i++)
	{
		if(i==MONS_BJORN)
			monsType[i].spr=NULL;	// he uses the same sprites as MONS_SVEN
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

word MonsterPoints(byte type)
{
	return monsType[type].points;
}

word MonsterHP(byte type)
{
	return monsType[type].hp;
}

char *MonsterName(byte type)
{
	return monsType[type].name;
}

sprite_t *GetMonsterSprite(byte type,byte seq,byte frm,byte facing)
{
	int v;

	// load if not loaded
	if(monsType[type].spr==NULL)
	{
		if(type==MONS_BJORN)
		{
			if(!monsType[MONS_SVEN].spr)
				monsType[MONS_SVEN].spr=new sprite_set_t(monsType[MONS_SVEN].sprName);
			monsType[type].spr=monsType[MONS_SVEN].spr;	// he uses the same sprites as MONS_SVEN
		}
		else
			monsType[type].spr=new sprite_set_t(monsType[type].sprName);
		if(monsType[type].spr==NULL)
			FatalError("Out of memory!");
	}

	v=monsType[type].anim[seq][frm];

	if(v==254)
		return NULL;	// 254 means no sprite for this frame

	if(!(monsType[type].flags&MF_ONEFACE))
		v+=facing*monsType[type].framesPerDir;

	if(type==MONS_BOUAPHA)
	{
		if(PlayerHasHammer())
			v+=8*monsType[type].framesPerDir;
	}
	if(monsType[type].flags&MF_FACECMD)
		v+=facing;

	return monsType[type].spr->GetSprite(v);
}

void MonsterDraw(int x,int y,int z,byte type,byte seq,byte frm,byte facing,char bright,byte ouch)
{
	sprite_t *curSpr;
	int v;
	byte shld;

	// load if not loaded
	if(monsType[type].spr==NULL)
	{
		if(type==MONS_BJORN)
		{
			if(!monsType[MONS_SVEN].spr)
				monsType[MONS_SVEN].spr=new sprite_set_t(monsType[MONS_SVEN].sprName);
			monsType[type].spr=monsType[MONS_SVEN].spr;	// he uses the same sprites as MONS_SVEN
		}
		else
			monsType[type].spr=new sprite_set_t(monsType[type].sprName);
		if(monsType[type].spr==NULL)
			FatalError("Out of memory!");
	}

	v=monsType[type].anim[seq][frm];

	if(v==254)
		return;	// don't draw this frame

	if(!(monsType[type].flags&MF_ONEFACE))
		v+=facing*monsType[type].framesPerDir;

	if(type==MONS_BOUAPHA)
	{
		if(PlayerHasHammer())
			v+=8*monsType[type].framesPerDir;
		shld=PlayerShield();
		if((shld<16) && (shld&2))	// it blinks when there is 1/2 second left
			shld=0;
		curSpr=monsType[type].spr->GetSprite(464+(shld&7));
		if(shld)
			SprDraw(x>>FIXSHIFT,(y>>FIXSHIFT)+1,1,255,bright,curSpr,DISPLAY_DRAWME|DISPLAY_GLOW);
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
				SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,4,bright,curSpr,DISPLAY_DRAWME);
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
		if(!(monsType[type].flags&MF_GHOST))
			SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,255,bright,curSpr,DISPLAY_DRAWME);
		else
			SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,255,bright,curSpr,DISPLAY_DRAWME|DISPLAY_GHOST);
	}
	else
		SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,4,bright,curSpr,DISPLAY_DRAWME);

}

void InstaRenderMonster(int x,int y,byte type,char bright,MGLDraw *mgl)
{
	sprite_t *curSpr;
	int v;

	// load if not loaded
	if(monsType[type].spr==NULL)
	{
		if(type==MONS_BJORN)
		{
			if(!monsType[MONS_SVEN].spr)
				monsType[MONS_SVEN].spr=new sprite_set_t(monsType[MONS_SVEN].sprName);
			monsType[type].spr=monsType[MONS_SVEN].spr;	// he uses the same sprites as MONS_SVEN
		}
		else
			monsType[type].spr=new sprite_set_t(monsType[type].sprName);
		if(monsType[type].spr==NULL)
			FatalError("Out of memory!");
	}

	v=monsType[type].anim[ANIM_IDLE][0];
	if(!(monsType[type].flags&MF_ONEFACE))
		v+=2*monsType[type].framesPerDir;

	curSpr=monsType[type].spr->GetSprite(v);
	if(!curSpr)
		return;

	curSpr->DrawBright(x,y,mgl,bright);
}

// AI auxiliary functions to make it simple
//---------------------------------------------

inline void FaceGoodguy(Guy *me,Guy *goodguy)
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

inline int RangeToTarget(Guy *me,Guy *goodguy)
{
	return abs(me->x-goodguy->x)+abs(me->y-goodguy->y);
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

// here be the AIs for each monster type
//--------------------------------------------------------------------------------------

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
				goodguy->GetShot(Cosine(me->facing*32)*4,Sine(me->facing*32)*4,4,map,world);
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
				goodguy->GetShot(Cosine(me->facing*32)*4,Sine(me->facing*32)*4,1,map,world);
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
}

void AI_MattieBrain(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	byte tempface;
	byte faceTable[8]={5,4,0,1,2,3,0,6};
	int x,y;

	if(me->ouch==4 && me->type==MONS_MATBRAIN)	// skull and head have their own ouch noises
	{
		if(me->hp>0)
			MakeSound(SND_MATTIEOUCH,me->x,me->y,SND_CUTOFF,1200);
	}

	// use facing variable to determine which way for eyes to look
	if(me->action!=ACTION_BUSY)
	{
		tempface=me->facing;
		FaceGoodguy(me,goodguy);

		if(me->facing==6)	// can't look straight back, so deal with it
		{
			if(tempface<2)
				me->facing=0;
			else
				me->facing=4;
		}
		me->facing=faceTable[me->facing];
		if(me->type!=MONS_MATBRAIN && me->facing!=0)
			me->facing+=4;
	}
	else
		me->facing=0;	// always look ahead when doing things

	if(me->seq==ANIM_DIE)
	{
		x=me->x>>FIXSHIFT;
		y=me->y>>FIXSHIFT;
		if(me->type==MONS_MATBRAIN)
		{
			BlowUpGuy(x+me->rectx-40,y+me->recty-200,x+me->rectx2+40,y+me->recty2+10,me->z,4);
			BlowSmoke((x+me->rectx-40+MGL_random(me->rectx2-me->rectx+80))<<FIXSHIFT,
					  (y+me->recty-200+MGL_random(me->recty2-me->recty+210))<<FIXSHIFT,
					  me->z,FIXAMT);
			BlowSmoke((x+me->rectx-40+MGL_random(me->rectx2-me->rectx+80))<<FIXSHIFT,
					  (y+me->recty-200+MGL_random(me->recty2-me->recty+210))<<FIXSHIFT,
					  me->z,FIXAMT);
			ShakeScreen(2);
		}
	}
	// use the claws to cover your face when scared
	if(me->ouch && me->mind1<64)
		me->mind1+=8;	// mind1 is fear, when it gets high, the claws cover her face

	if(me->mind1)
		me->mind1--;
}

void AI_MattieSkullOrHead(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_MATTIEOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_MATTIEDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE && me->frm==5)
		{
			if(me->type==MONS_MATHEAD)
			{
				me->type=MONS_MATSKULL;
				me->hp=monsType[MONS_MATSKULL].hp;
			}
			else
			{
				me->type=MONS_MATBRAIN;
				me->hp=monsType[MONS_MATBRAIN].hp;
			}
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_IDLE;
			me->reload=100;
			me->facing=0;
			return;
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
		if(me->seq==ANIM_ATTACK && me->frm==3 && me->reload==0)
		{
			// shoot laser
			x=me->x;
			y=me->y+FIXAMT*8;
			LaunchMegabeam(x,y,me->ID);
			me->reload=0;
			me->mind=0;
			me->reload=100;
		}
		me->facing=0;
		return;
	}

	if((!me->reload) && MGL_random(100)==0)
	{
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=32;	// very very slow animation
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
	}

	// do everything the brain does
	AI_MattieBrain(me,map,world,goodguy);
}

void AI_MattieClaw(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload>0)
		me->reload--;

	if(!me->parent || me->parent->type==MONS_NONE)
	{
		me->type=MONS_NONE;	// can't exist alone
		return;
	}

	if(me->type==MONS_MATCLAW1)
	{
		me->x=me->parent->x-45*FIXAMT;
		me->y=me->parent->y+12*FIXAMT;
		me->z=me->parent->z-12*FIXAMT;
	}
	else	// MONS_MATCLAW2
	{
		me->x=me->parent->x+45*FIXAMT;
		me->y=me->parent->y+12*FIXAMT;
		me->z=me->parent->z-12*FIXAMT;
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
		}
		if(me->seq==ANIM_A1 && me->frm==2 && me->parent->mind1>32)
			me->frmTimer=0;	// hold them up until she calms down
		if(me->seq==ANIM_ATTACK && me->frm>1)
		{
			if(me->frm<5)
			{
				if(me->type==MONS_MATCLAW1)
					x=me->x-FIXAMT*118;
				else
					x=me->x+FIXAMT*118;
				y=me->y+FIXAMT*89;
			}
			else
			{
				if(me->type==MONS_MATCLAW1)
					x=me->x-FIXAMT*69;
				else
					x=me->x+FIXAMT*69;
				y=me->y+FIXAMT*141;
			}

			if(me->AttackCheck(50,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(0,FIXAMT*8,8,map,world);
			}
		}
		return;
	}

	if(me->reload==0)
	{
		MakeSound(SND_MATTIECLAW,me->x,me->y,SND_CUTOFF,1200);
		me->reload=MGL_random(180)+20;
		me->action=ACTION_BUSY;
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		return;
	}

	// make their idle weird and wiggly
	me->frmAdvance=MGL_random(128)+1;

	if(me->parent->mind1>32)	// she is scared, block!!
	{
		me->action=ACTION_BUSY;
		me->seq=ANIM_A1;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=48;
	}
}

void AI_MattieTail(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	byte b;
	int x,y;
	short shootXTable[9]={-134,-124,-101,-57,0,67,113,134,143};
	short shootYTable[9]={-51,-43,-24,-7,-11,-15,-29,-41,-46};

	if(!me->parent || me->parent->type==MONS_NONE)
		me->type=MONS_NONE;	// can't exist alone

	me->x=me->parent->x;
	me->y=me->parent->y-104*FIXAMT;
	me->z=me->parent->z;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK)
		{
			b=(32-me->facing)*4;
			// shoot some bullets
			x=me->x+(shootXTable[(me->facing>>2)]<<FIXSHIFT);
			y=me->y+(shootYTable[(me->facing>>2)]<<FIXSHIFT);

			b+=16-MGL_random(33);
			FireExactBullet(x,y,me->z+32*FIXAMT,Cosine(b)*6,Sine(b)*6,-FIXAMT/2,
				0,60,b,BLT_ENERGY);
			if(me->parent->type==MONS_MATBRAIN)
			{
				b+=16-MGL_random(33);
				FireExactBullet(x,y,me->z+32*FIXAMT,Cosine(b)*6,Sine(b)*6,-FIXAMT/2,
					0,120,b,BLT_ENERGY);
			}
		}
		return;
	}

	if(me->mind)
		me->mind--;

	if(me->mind<10)
	{
		MakeSound(SND_MATTIESHOOT,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=255;
		me->action=ACTION_BUSY;
		if(me->mind==0)
		{
			if(me->parent->type!=MONS_MATBRAIN)
				me->mind=80;
			else
				me->mind=30;
		}
	}

	if(goodguy)
	{
		// point at Bouapha
		if(goodguy->x<me->x-FIXAMT*128)
		{
			if(goodguy->y<me->y+FIXAMT*64)
				b=0;
			else
				b=8;
		}
		else if(goodguy->x>me->x+FIXAMT*128)
		{
			if(goodguy->y<me->y+FIXAMT*64)
				b=32;
			else
				b=24;
		}
		else if(goodguy->y>me->y+FIXAMT*64)
		{
			b=16;
		}
		else
		{
			// can't point at him when he is behind, but you can try
			if(goodguy->x<me->x)
				b=0;
			else
				b=32;
		}
	}
	else
		b=16;

	// b contains the desired facing
	if(me->mind1<b)
		me->mind1++;
	else if(me->mind1>b)
		me->mind1--;
	me->facing=me->mind1&(~3);	// facing is locked in multiples of 4
}

void AI_MattieBody(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	byte b;

	if(!me->parent || me->parent->type==MONS_NONE)
		me->type=MONS_NONE;	// can't exist alone

	// unlike other components, this one tells the head itself where to be
	me->parent->x=me->x;
	me->parent->y=me->y+40*FIXAMT;
	me->parent->z=me->z+32*FIXAMT;

	if(goodguy)
	{
		if(goodguy->x<me->x)
			me->dx-=FIXAMT/16;	// slow acceleration
		if(goodguy->x>me->x)
			me->dx+=FIXAMT/16;
		if(goodguy->y<me->y)	//CRUSH him if he gets behind you
			me->dy-=FIXAMT;
		else
		{
			if(me->mind>128)
			{
				me->mind--;
				me->dy+=FIXAMT/16;
			}
			else if(me->mind<128)
			{
				me->mind++;
				me->dy-=FIXAMT/16;
			}
		}
	}
	b=(byte)MGL_random(128);
	if(b<10)
		me->mind=128-MGL_random(64);
	else if(b<40)
		me->mind=128+MGL_random(64);
	else if(b<50)
		me->mind=128;

	Dampen(&me->dx,FIXAMT/32);
	Dampen(&me->dy,FIXAMT/32);
	Clamp(&me->dx,FIXAMT*1);
	Clamp(&me->dy,FIXAMT*1);

	if(me->seq!=ANIM_MOVE)
	{
		MakeSound(SND_MATTIESTOMP,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=64;
	}
}

void AI_Ginger(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

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
		if(me->seq==ANIM_ATTACK && (me->frm==2 || me->frm==4 || me->frm==11) && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*32;
			y=me->y+Sine(me->facing*32)*32;
			if(me->AttackCheck(32,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				goodguy->GetShot(Cosine(me->facing*32)*4,Sine(me->facing*32)*4,6,map,world);
			if(me->frm==11)
			{
				me->frmAdvance=64;
				me->dx=0;
				me->dy=0;
			}
			me->reload=3;
		}
		if(me->seq==ANIM_A1 && goodguy)
		{
			if(me->frm<2)
			{
				me->dx=0;
				me->dy=0;
			}
			else
			{
				me->dx=Cosine(me->facing*32)*10;
				me->dy=Sine(me->facing*32)*10;
				x=me->x+Cosine(me->facing*32)*16;
				y=me->y+Sine(me->facing*32)*16;
				if(me->AttackCheck(16,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				{
					goodguy->GetShot(Cosine(me->facing*32)*12,Sine(me->facing*32)*12,12,map,world);
					me->seq=ANIM_A2;
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=32;
					me->dx=-Cosine(me->facing*32)*4;
					me->dy=-Sine(me->facing*32)*4;
				}
			}
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly stalk Bouapha
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(64*FIXAMT) && MGL_random(8)==0)
			{
				// get him!
				MakeSound(SND_GINGERCOMBO,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
				me->dx=Cosine(me->facing*32);
				me->dy=Sine(me->facing*32);
				return;
			}
			FaceGoodguy(me,goodguy);

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
		if(RangeToTarget(me,goodguy)<(512*FIXAMT) && MGL_random(128)==0)
		{
			// jumpkick him
			MakeSound(SND_GINGERJKICK,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_A1;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			FaceGoodguy(me,goodguy);
			me->dz=FIXAMT*6;
			return;
		}

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
			me->frmAdvance=64;
		}
	}
}

void AI_Pumpkin(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_PUMPKINDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE && me->reload==0)
		{
			ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,8);
			me->reload=2;
		}
		return;	// can't do nothin' right now
	}

	if(me->seq==ANIM_MOVE && me->frm==2 && goodguy)	// hits on this frame
	{
		if(me->AttackCheck(24,me->x>>FIXSHIFT,me->y>>FIXSHIFT,goodguy))
			goodguy->GetShot(Cosine(me->facing*32)*4,Sine(me->facing*32)*4,2,map,world);
	}

	if(me->mind==0)	// not currently aware of goodguy
	{
		if(me->seq==ANIM_MOVE)
		{
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=32;
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
}

void AI_BabyThing(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_PUMPKINDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->parent)
	{
		// sitting on The Thing's tongue
		switch(me->parent->frm)
		{
			case 4:
			case 5:
				me->y+=10*FIXAMT;
				break;
			case 6:
			case 7:
				me->y+=15*FIXAMT;
				break;
			case 8:
				me->parent=NULL;	// I am free!
				if(me->hp>0)
				{
					me->seq=ANIM_MOVE;
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=128;
					me->mind1=200;
					me->mind=1;	// pop out in hunt mode
				}
				if(!me->CanWalk(me->x,me->y,map,world))
					me->type=MONS_NONE;	// vanish if this spot is occupied
				break;
		}
		return;
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->reload==0 && me->frm==5)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			FireBullet(x,y,me->facing*32,BLT_ENERGY);
			me->reload=5;
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)	// not currently aware of goodguy
	{
		if(me->seq==ANIM_MOVE)
		{
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=32;
		}
		if(me->mind1<31)
			me->mind1++;

		if(me->mind1>30 && MGL_random(50)==0)
		{
			switch(MGL_random(16))
			{
				case 0:
					MakeSound(SND_BABYSNORE,me->x,me->y,SND_CUTOFF,800);
					me->seq=ANIM_A1;	// take a nap
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=32;
					me->mind1=0;
					me->action=ACTION_BUSY;
					break;
				case 1:
				case 2:
				case 3:

					me->seq=ANIM_A3;	// look right
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=64;
					me->mind1=0;
					me->action=ACTION_BUSY;
					break;
				case 4:
				case 5:
				case 6:
					me->seq=ANIM_A4;	// look left
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=64;
					me->mind1=0;
					me->action=ACTION_BUSY;
					break;
				case 7:
				case 8:
					me->mind=2;	// go into wander mode
					me->mind1=MGL_random(40)+1;
					break;
				default:	// blink is very common
					me->seq=ANIM_A2;
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=128;
					me->mind1=0;
					me->action=ACTION_BUSY;
					break;
			}
		}
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<128*FIXAMT || me->ouch>0)
			{
				// if the goodguy is near, or he shot me
				me->mind=1;	// start hunting
				me->mind1=255;

				FaceGoodguy(me,goodguy);
			}
		}
		me->dx=0;
		me->dy=0;
	}
	else if(me->mind==1)	// hunting mode
	{
		FaceGoodguy(me,goodguy);
		if(!goodguy)
		{
			me->mind=0;	// go back to being bored, nothing to see here
			me->mind1=0;
			return;
		}
		if(RangeToTarget(me,goodguy)<(512*FIXAMT) && MGL_random(32)==0 && me->reload==0)
		{
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
		if(MGL_random(80)==0)
		{
			me->mind1=20;
			me->mind=2;	// wander
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
	else	// random wander mode
	{
		if(!(me->mind1--))	// time to get a new direction
		{
			switch(MGL_random(3))
			{
				case 0:
					if(goodguy)
						me->mind=1;	// hunt him
					break;
				case 1:
					me->mind=0;	// stop here and rest
					me->mind1=0;
					break;
				case 2:	// just wander a different way
					me->facing=(byte)MGL_random(8);
					me->mind1=MGL_random(20)+1;
					break;
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
	// this is needed because his idle frame doesn't fit into his walking animation, so this skips it
	if(me->seq==ANIM_MOVE && me->frm==10 && me->frmTimer>127)
	{
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
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

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm>=6 && me->frm<=12 && me->reload==0 && goodguy)
		{
			// spit stuff
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			FireBullet(x,y,me->facing,BLT_FLAME2);
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
	Guy *g;

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
				coro::launch(std::bind(ShowVictoryAnim, 0));	// you killed the final boss
				SendMessageToGame(MSG_WINGAME,0);
				EndClock();
				StartClock();
			}
		}
		if(me->seq==ANIM_ATTACK && me->frm==3 && me->frmTimer<32)
		{
			g=AddGuy(me->x,me->y,0,MONS_BABYTHING);
			if(g)
			{
				g->parent=me;
				g->seq=ANIM_A1;	// take a nap
				g->frm=0;
				g->frmTimer=0;
				g->frmAdvance=32;
				g->action=ACTION_BUSY;
			}
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

void AI_SuperZombie(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int i,x,y;

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
		if(me->seq==ANIM_A1)	// leaping attack
		{
			if(me->frm<3)
			{
				me->dx=0;
				me->dy=0;
				me->dz=0;
			}
			else if(me->frm==3)
			{
				me->dx=Cosine(me->facing*32)*12;
				me->dy=Sine(me->facing*32)*12;
				me->dz=10*FIXAMT;
			}
			else if(me->frm<7)
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
		if(me->seq==ANIM_A2 && me->frm==1 && me->reload==0 && goodguy)
		{
			if(RangeToTarget(me,goodguy)<70*FIXAMT && goodguy->hp>0 && PlayerShield()==0)
			{
				// got him
				MakeSound(SND_SZGRAB,me->x,me->y,SND_CUTOFF,1500);
				me->seq=ANIM_A3;	// begin lifting
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=64;
				me->action=ACTION_BUSY;
				me->reload=100;	// to prevent multigrabs
				goodguy->parent=me;
				goodguy->facing=(me->facing+4)&7;
				goodguy->x=me->x+Cosine(me->facing*32)*60;
				goodguy->y=me->y+Sine(me->facing*32)*60;
				goodguy->z=0;
				goodguy->dx=0;
				goodguy->dy=0;
				goodguy->dz=0;
				goodguy->action=ACTION_BUSY;
				goodguy->seq=ANIM_MOVE;	// wiggle his feet
				goodguy->frm=0;
				goodguy->frmTimer=0;
				goodguy->frmAdvance=512;
				return;
			}
			me->reload=4;
		}
		if(me->seq==ANIM_A3 && goodguy)
		{
			if((me->frm==5 || me->frm==8 || me->frm==11 || me->frm==14 || me->frm==17) && me->frmTimer<32)
			{
				MakeSound(SND_MISSILEBOOM,me->x,me->y,SND_CUTOFF,1500);
				goodguy->GetShot(0,0,8,map,world);
			}
		}
		if(me->seq==ANIM_ATTACK && me->frm==5 && me->reload==0)
		{
			x=(me->x+Cosine(me->facing*32)*40)>>FIXSHIFT;
			y=(me->y+Sine(me->facing*32)*40)>>FIXSHIFT;
			if(me->AttackCheck(30,x,y,goodguy))
				goodguy->GetShot(Cosine(me->facing*32)*16,Sine(me->facing*32)*16,15,map,world);
			me->reload=4;
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
	else if(me->mind==2)		// when mind=2, hold still and go off on Bouapha
	{
		if(goodguy)
		{
			i=RangeToTarget(me,goodguy);
			if(i<(96*FIXAMT) && MGL_random(10)==0 && me->reload==0)
			{
				// get him! (punch)
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				return;
			}
			if(i<(96*FIXAMT) && MGL_random(16)==0 && me->reload==0)
			{
				// get him! (grab)
				me->seq=ANIM_A2;
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

			me->dx=0;
			me->dy=0;
			if(me->seq!=ANIM_IDLE)
			{
				me->seq=ANIM_IDLE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
			if(i>(80*FIXAMT) && me->mind1==0)
			{
				// chase him down!
				me->mind=1;
				me->mind1=8;
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

			me->dx=Cosine(me->facing*32)*4;
			me->dy=Sine(me->facing*32)*4;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
			if(RangeToTarget(me,goodguy)<128*FIXAMT && me->mind1==0)
			{
				me->mind=2;	// in range, start killin'
				me->mind1=8;
			}
			else if(MGL_random(64)==0)
			{
				//leap!
				me->seq=ANIM_A1;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
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

void AI_BabySeal(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	byte f;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SEALOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SEALDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frmTimer<32)
		{
			f=me->facing*32;
			switch(me->frm)
			{
				case 3:
					f-=20;
					break;
				case 5:
					f-=10;
					break;
				case 7:
					f-=4;
					break;
				case 9:
					f+=4;
					break;
				case 11:
					f+=10;
					break;
				case 13:
					f+=20;
					break;
				default:
					return;	// don't fire on this frame
			}
			f=f-8+MGL_random(17);
			x=me->x+Cosine(f)*16;
			y=me->y+Sine(f)*16;
			FireBullet(x,y,f,BLT_ENERGY);
			MakeSound(SND_BULLETFIRE,x,y,SND_CUTOFF,1050);
			me->reload=20;
		}
		if(me->seq==ANIM_DIE && me->frm>=4)
		{
			for(f=0;f<8;f++)
				SpecialSnow(me->x+((-32+MGL_random(65))<<FIXSHIFT),
							me->y+((-32+MGL_random(65))<<FIXSHIFT));
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if(goodguy)
		{
			if(MGL_random(64)==0 && me->reload==0)
			{
				// get him!
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
			FaceGoodguy(me,goodguy);

			me->dx=Cosine(me->facing*32)*2;
			me->dy=Sine(me->facing*32)*2;
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
}

void AI_Isozoid(Guy *me,Map *map,world_t *world,Guy *goodguy)
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

	if(me->mind==0 && me->hp>0)	// hasn't spotted Bouapha yet
	{
		if((goodguy && RangeToTarget(me,goodguy)<200*FIXAMT) || (me->ouch>0))
		{
			me->mind=1;
			me->action=ACTION_BUSY;
			me->seq=ANIM_A1;	// rise out of snow
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=72;
			ExplodeParticles2(PART_SNOW2,me->x,me->y,0,12,6);
		}
		else if(me->seq!=ANIM_A2)
		{
			me->seq=ANIM_A2;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=1;
		}
		else if(me->frmTimer>20)
			me->frmTimer=0;	// keep him on that frame
		FaceGoodguy2(me,goodguy);
		return;
	}
	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==5 && me->reload==0 && goodguy)
		{
			// spit snowball
			x=me->x+Cosine(me->facing*32)*8;
			y=me->y+Sine(me->facing*32)*8;
			FireBullet(x,y,(me->facing*32-16+MGL_random(33))&255,BLT_SNOWBALL);
			me->reload=10;
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

	// Isozoids don't move
	me->dx=0;
	me->dy=0;
	if(me->seq!=ANIM_IDLE)
	{
		me->seq=ANIM_IDLE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}

void AI_Snowguy(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SNOWOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SNOWDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==3 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*20;
			y=me->y+Sine(me->facing*32)*20;
			if(me->AttackCheck(20,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				goodguy->GetShot(Cosine(me->facing*32)*4,Sine(me->facing*32)*4,8,map,world);
			me->reload=5;
		}
		if(me->seq==ANIM_A1 && me->frm==11 && me->reload==0 && goodguy)
		{
			// only fire if the range is good
			if(RangeToTarget(me,goodguy)<800*FIXAMT)
			{
				x=me->x+Cosine(me->facing*32)*32;
				y=me->y+Sine(me->facing*32)*32;
				FireBullet(x,y,me->facing*32,BLT_BIGSNOW);
			}
			me->reload=5;
			me->mind1=1;
		}
		if(me->seq==ANIM_DIE)
			me->frmAdvance=128;
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(64*FIXAMT) && MGL_random(8)==0)
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
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(512*FIXAMT) && MGL_random(32)==0)
			{
				// throw a snowball
				MakeSound(SND_SNOWTHROW,me->x,me->y,SND_CUTOFF,1100);
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

void AI_Zomboni(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			if(me->frm>=4)
			{
				x=me->x>>FIXSHIFT;
				y=me->y>>FIXSHIFT;
				BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,2);
				BlowSmoke((x+me->rectx+MGL_random(me->rectx2-me->rectx))<<FIXSHIFT,
						  (y+me->recty+MGL_random(me->recty2-me->recty))<<FIXSHIFT,
						  me->z,FIXAMT);
			}
			if(me->frm==8)
			{
				// become a zombie
				me->type=MONS_ZOMBIE;
				me->mind=0;
				me->mind1=0;
				me->hp=20;
				me->seq=ANIM_IDLE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=32;
			}
		}
		return;	// can't do nothin' right now
	}

	if(me->seq==ANIM_MOVE && me->frm==1 && me->frmTimer<32)
		MakeSound(SND_ZOMBONIZOOM,me->x,me->y,SND_CUTOFF,1500);

	if(me->mind==0)
	{
		if((goodguy && RangeToTarget(me,goodguy)<(400*FIXAMT)) || (me->ouch))
		{
			me->mind=1;	// start chasing him down
		}
	}
	else
	{
		if(!me->reload)
		{
			FaceGoodguy2(me,goodguy);
			me->reload=8;
		}
		me->dx=Cosine(me->facing*32)*6;
		me->dy=Sine(me->facing*32)*6;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
		}
		if(me->frm==0)
		{
			BlowSmoke(me->x-Cosine(me->facing*32)*32,me->y-Sine(me->facing*32)*32,me->z+FIXAMT*40,FIXAMT);
			if(goodguy && RangeToTarget(me,goodguy)<90*FIXAMT)
			{
				// smash into him
				goodguy->GetShot(Cosine(me->facing*32)*12,Sine(me->facing*32)*12,10,map,world);
			}
		}
	}
}

void AI_Yeti(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_YETIOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_YETIDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			me->facing=2;	// can only die forward
			if(me->frm>=4)
			{
				x=me->x>>FIXSHIFT;
				y=me->y>>FIXSHIFT;
				BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,2);
				BlowSmoke((x+me->rectx+MGL_random(me->rectx2-me->rectx))<<FIXSHIFT,
						  (y+me->recty+MGL_random(me->recty2-me->recty))<<FIXSHIFT,
						  me->z,FIXAMT);
			}
		}
		if(me->seq==ANIM_ATTACK && me->frm==7 && me->frmTimer<32)
		{
			x=me->x+Cosine(me->facing*32)*64;
			y=me->y+Sine(me->facing*32)*64;
			x+=Cosine(((me->facing+2)*32)&255)*32;
			y+=Sine(((me->facing+2)*32)&255)*32;
			FireExactBullet(x,y,40*FIXAMT,Cosine(me->facing*32)*8,Sine(me->facing*32)*8,4*FIXAMT,0,100,
							me->facing*32,BLT_BIGSNOW);
			me->reload=10;
		}
		if(me->seq==ANIM_A2 && me->frm>3)
		{
			ShakeScreen(3);
			// snowballs fall all over
			x=me->x+((320-MGL_random(641))<<FIXSHIFT);
			y=me->y+((240-MGL_random(481))<<FIXSHIFT);
			if(MGL_random(3)==0)
				FireExactBullet(x,y,400*FIXAMT,0,0,0,0,100,me->facing*32,BLT_BIGSNOW);
			else
				FireExactBullet(x,y,400*FIXAMT,0,0,0,0,100,me->facing*32,BLT_SNOWBALL);
		}
		if(me->seq==ANIM_A1 && (me->frm==5 || me->frm==6))
		{
			x=me->x+Cosine(me->facing*32)*64;
			y=me->y+Sine(me->facing*32)*64;
			FireBullet(x,y,me->facing,BLT_ICESPIKE);
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)
	{
		if((goodguy && RangeToTarget(me,goodguy)<(300*FIXAMT)) || (me->ouch))
		{
			me->mind=1;	// start into action
			me->mind1=0;

			me->seq=ANIM_A3;	// wave hello first
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=64;
			me->facing=2;
			me->action=ACTION_BUSY;

			if(me->type==MONS_SVEN) // say "Hello, I am Sven"
				MakeSound(SND_SVENHI,me->x,me->y,SND_CUTOFF,1200);
			else	// say "Hello, I am Bjorn"
				MakeSound(SND_BJORNHI,me->x,me->y,SND_CUTOFF,1200);
		}
	}
	else
	{
		if(me->mind1)
			me->mind1--;
		else
		{
			// rotate
			me->facing=(me->facing-1+MGL_random(3))&7;
			me->mind1=MGL_random(30)+1;
		}

		if((!me->reload) && goodguy && MGL_random(20)==0)
		{
			// select an attack method
			x=MGL_random(16);

			if(x<12) // 75% chance of primary weapon
			{
				// Sven throws snowballs
				if(me->type==MONS_SVEN)
				{
					me->seq=ANIM_ATTACK;
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=128;
					FaceGoodguy(me,goodguy);
					me->action=ACTION_BUSY;
				}
				else
				{
					// Bjorn causes stalag-quakes
					me->seq=ANIM_A1;
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=128;
					FaceGoodguy(me,goodguy);
					me->action=ACTION_BUSY;
				}
			}
			else	// 25% chance of yodel
			{
				MakeSound(SND_YETIYODEL,me->x,me->y,SND_CUTOFF,1500);
				me->seq=ANIM_A2;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=64;
				me->facing=2;
				me->action=ACTION_BUSY;
			}
			me->dx=0;
			me->dy=0;
			return;
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

void AI_Djinni(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->mind1)
		me->mind1--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_DJINNIOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_DJINNIDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==5 && me->reload==0 && goodguy)
		{
			// punch him
			x=me->x+Cosine(me->facing*32)*20;
			y=me->y+Sine(me->facing*32)*20;
			if(me->AttackCheck(20,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(Cosine(me->facing*32)*10,Sine(me->facing*32)*10,6,map,world);
			}
			me->reload=10;
		}
		return;	// can't do nothin' right now
	}

	if(RangeToTarget(me,goodguy)<(54*FIXAMT) && MGL_random(8)==0 && me->reload==0)
	{
		// punch him
		MakeSound(SND_DJINNIPUNCH,me->x,me->y,SND_CUTOFF,600);
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
	if(MGL_random(256)==0)
	{
		// scare him
		MakeSound(SND_DJINNIBOO,me->x,me->y,SND_CUTOFF,1300);
		me->seq=ANIM_A1;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		return;
	}

	if(!me->mind1)
	{
		// turns only every so often
		FaceGoodguy2(me,goodguy);
		me->mind1=10;
	}
	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->dx=Cosine(me->facing*32)*2;
		me->dy=Sine(me->facing*32)*2;
	}
}

void AI_MagicLamp(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	Guy *g;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_LAMPDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==18 && me->reload==0)
		{
			g=AddGuy(me->x+FIXAMT*20,me->y+FIXAMT*12,10*FIXAMT,MONS_DJINNI);
			if(g && (!g->CanWalk(me->x,me->y,map,world)))
				g->type=MONS_NONE;	// vanish if this spot is occupied
			if(g)
			{
				g->seq=ANIM_A2;	// materialize
				g->frm=0;
				g->frmTimer=0;
				g->frmAdvance=64;
				g->action=ACTION_BUSY;
			}
			me->reload=200;
		}
		else if(me->seq==ANIM_ATTACK && me->frm>4)
		{
			BlowSmoke(me->x+FIXAMT*20,me->y+FIXAMT*12,FIXAMT*10,3*FIXAMT);
		}
		if(me->seq==ANIM_DIE)
			me->frmAdvance=256;
		return;
	}

	if((!me->reload) && MGL_random(200)==0)
	{
		MakeSound(SND_LAMPPOOF,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
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

void AI_SphinxArm(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		// make noise?
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==6 && me->frmTimer<64 && (goodguy))
		{
			// hurt guy
			MakeSound(SND_SPHINXSTOMP,me->x,me->y,SND_CUTOFF,600);
			x=me->x>>FIXSHIFT;
			y=(me->y>>FIXSHIFT)+115;
			if(me->type==MONS_SPHXARM1)
				x-=20;
			else
				x+=20;
			if(me->AttackCheck(60,x,y,goodguy))
			{
				FaceGoodguy(me,goodguy);
				goodguy->GetShot(Cosine(me->facing*32)*12,
								Sine(me->facing*32)*12,30,map,world);
				goodguy->dx=Cosine(me->facing*32)*6;
				goodguy->dx=Sine(me->facing*32)*6;
			}
			ShakeScreen(6);
		}

		if(me->seq==ANIM_DIE)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2+1,y+me->recty2+1,me->z,1);
			BlowSmoke((x+me->rectx+MGL_random(me->rectx2-me->rectx+1))<<FIXSHIFT,
					  (y+me->recty+MGL_random(me->recty2-me->recty+1))<<FIXSHIFT,
					  me->z,FIXAMT);
		}
		return;
	}

	if(me->reload==0)
	{
		// swipe at him
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
		me->reload=30+MGL_random(60);
		return;
	}
	// just sit there when not swiping
	me->seq=ANIM_IDLE;
	me->frm=0;
	me->frmTimer=0;
	me->frmAdvance=128;
}

void AI_Sphinx(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	Guy *g;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_ZOMBIEOUCH,me->x,me->y,SND_CUTOFF,600);
		else
			MakeSound(SND_MUSHDIE,me->x,me->y,SND_CUTOFF,600);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==2 && me->frmTimer<32)
		{
			g=AddGuy(me->x,me->y+FIXAMT*90,0,MONS_MUMBLE);
			if(g && (!g->CanWalk(g->x,g->y,map,world)))
				g->type=MONS_NONE;	// vanish if this spot is occupied
			if(g)
			{
				g->mind=1;
			}
		}
		if(me->seq==ANIM_A1 && me->frm==12 && me->frmTimer<32)
		{
			// sneeze some rocks out
			for(x=0;x<8;x++)
			{
				FireExactBullet(me->x,me->y+FIXAMT*40,60*FIXAMT,(8*FIXAMT-MGL_randoml(17*FIXAMT)),
								MGL_randoml(8*FIXAMT),0,0,60,2,BLT_ROCK);
			}
			// special rocks to deal with people trying to hide in "safe" spots
			FireExactBullet(me->x,me->y+FIXAMT*40,60*FIXAMT,8*FIXAMT,0,0,0,60,2,BLT_ROCK);
			FireExactBullet(me->x,me->y+FIXAMT*40,60*FIXAMT,-8*FIXAMT,0,0,0,60,2,BLT_ROCK);
		}

		if(me->seq==ANIM_DIE)
		{
			if(me->frm==0 && me->frmTimer<32)
				KillKids(me);
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,2);
			BlowSmoke((x+me->rectx+MGL_random(me->rectx2-me->rectx))<<FIXSHIFT,
					  (y+me->recty+MGL_random(me->recty2-me->recty))<<FIXSHIFT,
					  me->z,FIXAMT);
			ShakeScreen(60);
		}
		return;
	}

	if(me->reload==0)
	{
		// call upon a mummy
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=64;
		me->action=ACTION_BUSY;
		me->reload=30+MGL_random(120);
		return;
	}
	if(MGL_random(128)==0)	// sneeze
	{
		MakeSound(SND_SPHINXSNEEZE,me->x,me->y,SND_CUTOFF,600);
		me->seq=ANIM_A1;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
		return;
	}
	// just sit there when not making mummies
	me->seq=ANIM_IDLE;
	me->frm=0;
	me->frmTimer=0;
	me->frmAdvance=128;
}

void AI_Freakazoid(Guy *me,Map *map,world_t *world,Guy *goodguy)
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
		if(me->seq==ANIM_ATTACK && (me->frm>=5 && me->frm<=8) && goodguy)
		{
			// spit stuff
			x=me->x+Cosine(me->facing*32)*8;
			y=me->y+Sine(me->facing*32)*8;
			FireBullet(x,y,me->facing*32-16+MGL_random(33),BLT_SPINE);
			me->reload=15;
			me->mind=0;
		}
		return;	// can't do nothin' right now
	}

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
}

void AI_CentiBody(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_PUMPKINDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(!me->parent || (me->parent->type!=MONS_CENTIBODY && me->parent->type!=MONS_CENTIHEAD))
	{
		me->type=MONS_CENTIHEAD;	// if you have no parent, time to get mean
		me->hp=40;	// with full HP
		return;
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
			ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,3);

		return;	// can't do nothin' right now
	}

	if(goodguy && RangeToTarget(me,goodguy)<44*FIXAMT && me->reload==0)
	{
		goodguy->GetShot(me->dy,-me->dx,4,map,world);
		me->reload=5;
	}

	// head towards parent
	if(me->parent->x>me->x)
		me->dx+=FIXAMT*2;
	else
		me->dx-=FIXAMT*2;
	if(me->parent->y>me->y)
		me->dy+=FIXAMT*2;
	else
		me->dy-=FIXAMT*2;

	if(RangeToTarget(me,me->parent)<FIXAMT*15)
	{
		me->dx=0;
		me->dy=0;
	}

	Clamp(&me->dx,FIXAMT*6);
	Clamp(&me->dy,FIXAMT*6);

	// decide on a facing
	if(me->dx>FIXAMT)
	{
		if(me->dy<-FIXAMT)
			me->facing=7;
		else if(me->dy>FIXAMT)
			me->facing=1;
		else
			me->facing=0;
	}
	else if(me->dx<-FIXAMT)
	{
		if(me->dy<-FIXAMT)
			me->facing=5;
		else if(me->dy>FIXAMT)
			me->facing=3;
		else
			me->facing=4;
	}
	else
	{
		if(me->dy<0)
			me->facing=6;
		else
			me->facing=2;
	}

	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}

void AI_CentiHead(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_ZOMBIEOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_PUMPKINDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
			ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,3);

		return;	// can't do nothin' right now
	}

	if(goodguy && RangeToTarget(me,goodguy)<44*FIXAMT && me->reload==0)
	{
		goodguy->GetShot(me->dx,me->dy,6,map,world);
		me->reload=5;
	}

	if(me->mind1)	// bonked a wall or guy
	{
		me->facing=(byte)MGL_random(8);
		me->mind1=0;
		me->dx=0;
		me->dy=0;
	}

	if(MGL_random(8)==0)
	{
		if(goodguy)
			FaceGoodguy2(me,goodguy);
	}
	if(me->facing==0 || me->facing==1 || me->facing==7)
		me->dx+=FIXAMT;
	else if(me->facing==3 || me->facing==4 || me->facing==5)
		me->dx-=FIXAMT;
	if(me->facing==1 || me->facing==2 || me->facing==3)
		me->dy+=FIXAMT;
	else if(me->facing==5 || me->facing==6 || me->facing==7)
		me->dy-=FIXAMT;

	Dampen(&me->dx,FIXAMT/2);
	Dampen(&me->dy,FIXAMT/2);
	Clamp(&me->dx,FIXAMT*5);
	Clamp(&me->dy,FIXAMT*5);

	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
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
							0,24,2,BLT_FLAME2);
			FireExactBullet(me->x+FIXAMT*52,me->y,FIXAMT*50,
							0,FIXAMT*12,-FIXAMT*3,
							0,24,2,BLT_FLAME2);
			FireExactBullet(me->x+FIXAMT*68,me->y,FIXAMT*50,
							FIXAMT*2,FIXAMT*12,-FIXAMT*3,
							0,24,2,BLT_FLAME2);

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

void AI_GreatPumpkin(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	Guy *g;
	byte f;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_GREATPKOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_GREATPKDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==6 && me->frmTimer<32)
		{
			g=AddGuy(me->x-FIXAMT*32,me->y+FIXAMT*40,0,MONS_PUMPKIN);
			if(g && (!g->CanWalk(me->x,me->y,map,world)))
				g->type=MONS_NONE;	// vanish if this spot is occupied
			if(g)
			{
				g->mind=1;
				g->mind1=120;
			}
			g=AddGuy(me->x,me->y+FIXAMT*40,0,MONS_PUMPKIN);
			if(g && (!g->CanWalk(me->x,me->y,map,world)))
				g->type=MONS_NONE;	// vanish if this spot is occupied
			if(g)
			{
				g->mind=1;
				g->mind1=120;
			}
			g=AddGuy(me->x+FIXAMT*32,me->y+FIXAMT*40,0,MONS_PUMPKIN);
			if(g && (!g->CanWalk(me->x,me->y,map,world)))
				g->type=MONS_NONE;	// vanish if this spot is occupied
			if(g)
			{
				g->mind=1;
				g->mind1=120;
			}
			g=AddGuy(me->x+FIXAMT*64,me->y+FIXAMT*40,0,MONS_PUMPKIN);
			if(g && (!g->CanWalk(me->x,me->y,map,world)))
				g->type=MONS_NONE;	// vanish if this spot is occupied
			if(g)
			{
				g->mind=1;
				g->mind1=120;
			}
			g=AddGuy(me->x-FIXAMT*64,me->y+FIXAMT*40,0,MONS_PUMPKIN);
			if(g && (!g->CanWalk(me->x,me->y,map,world)))
				g->type=MONS_NONE;	// vanish if this spot is occupied
			if(g)
			{
				g->mind=1;
				g->mind1=120;
			}
			me->reload=60;
			MakeSound(SND_GREATPKVOMIT,me->x,me->y,SND_CUTOFF,1100);
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

	if(me->mind>0)
		me->mind--;

	if(goodguy)
		FaceGoodguy(me,goodguy);

	if(me->mind==8)
	{
		// left eye fires
		f=(me->facing*32-32+me->mind1)&255;

		FireExactBullet(me->x-64*FIXAMT,me->y+32*FIXAMT,me->z+80*FIXAMT,
					Cosine(f)*8,Sine(f)*8,-3*FIXAMT,0,30,me->facing,BLT_ENERGY);
	}
	if(!me->mind)
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
		FireExactBullet(me->x+64*FIXAMT,me->y+32*FIXAMT,me->z+80*FIXAMT,
					Cosine(f)*8,Sine(f)*8,-3*FIXAMT,0,30,me->facing,BLT_ENERGY);
		me->mind=16;
		me->mind1+=8;
		if(me->mind1>=64)
			me->mind1=0;
	}

	if(me->reload==0 && MGL_random(20)==0)
	{
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=64;
		me->action=ACTION_BUSY;
	}
}

void AI_Ultrazoid(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int i;

	if(me->reload)
		me->reload--;

	if(me->mind1)
		me->mind1--;
	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SERPENTOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SERPENTDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && (me->frm==6 || me->frm==7) && me->frmTimer<64)
		{
			// dump a wad of zoid projectiles
				// first a pile of energy bullets
			FireExactBullet(me->x,me->y+FIXAMT*30,FIXAMT*60,-FIXAMT*4,FIXAMT*6,-FIXAMT*2,0,60,2,BLT_ENERGY);
			FireExactBullet(me->x,me->y+FIXAMT*30,FIXAMT*60,-FIXAMT*2,FIXAMT*8,-FIXAMT*2,0,60,2,BLT_ENERGY);
			FireExactBullet(me->x,me->y+FIXAMT*30,FIXAMT*60,0        ,FIXAMT*9,-FIXAMT*2,0,60,2,BLT_ENERGY);
			FireExactBullet(me->x,me->y+FIXAMT*30,FIXAMT*60, FIXAMT*2,FIXAMT*8,-FIXAMT*2,0,60,2,BLT_ENERGY);
			FireExactBullet(me->x,me->y+FIXAMT*30,FIXAMT*60, FIXAMT*4,FIXAMT*6,-FIXAMT*2,0,60,2,BLT_ENERGY);
				// now some fire
			FireExactBullet(me->x,me->y+FIXAMT*30,FIXAMT*60,-FIXAMT*4,FIXAMT*16,-FIXAMT*10,0,24,2,BLT_FLAME2);
			FireExactBullet(me->x,me->y+FIXAMT*30,FIXAMT*60,0        ,FIXAMT*18,-FIXAMT*10,0,24,2,BLT_FLAME2);
			FireExactBullet(me->x,me->y+FIXAMT*30,FIXAMT*60, FIXAMT*4,FIXAMT*16,-FIXAMT*10,0,24,2,BLT_FLAME2);
				// and some rocks
			FireExactBullet(me->x,me->y+FIXAMT*30,FIXAMT*60,-FIXAMT*8,FIXAMT*1,-FIXAMT*1,0,60,2,BLT_ROCK);
			FireExactBullet(me->x,me->y+FIXAMT*30,FIXAMT*60,-FIXAMT*4,FIXAMT*4,-FIXAMT*1,0,60,2,BLT_ROCK);
			FireExactBullet(me->x,me->y+FIXAMT*30,FIXAMT*60, FIXAMT*4,FIXAMT*4,-FIXAMT*1,0,60,2,BLT_ROCK);
			FireExactBullet(me->x,me->y+FIXAMT*30,FIXAMT*60, FIXAMT*8,FIXAMT*1,-FIXAMT*1,0,60,2,BLT_ROCK);
				// and big snowballs
			FireExactBullet(me->x,me->y+FIXAMT*30,FIXAMT*60,-FIXAMT*5,FIXAMT*1,-FIXAMT*2,0,40,2,BLT_BIGSNOW);
			FireExactBullet(me->x,me->y+FIXAMT*30,FIXAMT*60,0        ,FIXAMT*6,-FIXAMT*2,0,40,2,BLT_BIGSNOW);
			FireExactBullet(me->x,me->y+FIXAMT*30,FIXAMT*60, FIXAMT*5,FIXAMT*1,-FIXAMT*2,0,40,2,BLT_BIGSNOW);
				// and cactus spines
			for(i=108;i>19;i-=10)
				FireExactBullet(me->x,me->y+FIXAMT*30,FIXAMT*60,Cosine(i)*8,Sine(i)*8,-FIXAMT*2,0,40,i/16,BLT_SPINE);

			MakeSound(SND_SERPENTSPIT,me->x,me->y,SND_CUTOFF,1100);
			me->mind1=2;
		}

		if(me->seq==ANIM_DIE)
		{
			ShakeScreen(10);
		}
		return;	// can't do nothin' right now
	}

	if(me->reload==0)
	{
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=72;
		me->action=ACTION_BUSY;
		me->reload=90;
	}
}

void AI_DrLunatic(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_DRLOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_DRLDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE && me->frm==15 && me->frmTimer<32)
		{
			coro::launch(std::bind(ShowVictoryAnim, 10));	// show him transforming
			me->type=MONS_SDZL;	// become the Super Duper Zombie Lunatic
			me->hp=1500;
			me->mind=0;
			me->mind1=0;
			me->facing=2;
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_IDLE;
		}

		return;	// can't do nothin' right now
	}

	if(me->mind==0)	// if Bouapha gets near, laugh and then run
	{
		if(goodguy && RangeToTarget(me,goodguy)<160*FIXAMT)
		{
			MakeNormalSound(SND_DRLLAUGH);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=64;
			me->action=ACTION_BUSY;
			me->mind=1;
			me->reload=96;
			return;
		}
	}
	if(me->mind==1)	// running away
	{
		if(me->reload)
			me->reload--;
		else
		{
			me->mind=2;	// stop running
		}

		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
			me->facing=6;
			me->dx=0;
			me->dy=-FIXAMT*8;
		}
	}
	if(me->mind==2)	// waiting for Bouapha again
	{
		me->facing=2;
		me->dx=0;
		me->dy=0;
		if(goodguy && (RangeToTarget(me,goodguy)<128*FIXAMT || me->ouch>0))
		{
			MakeNormalSound(SND_DRLFEAR);
			me->mind=3;	// start running in fear
			me->reload=1;
		}
	}
	if(me->mind==3)
	{
		if(me->reload)
			me->reload--;
		else
		{
			me->facing=(me->facing+1-(byte)MGL_random(3))&7;
			me->reload=MGL_random(16)+1;
		}

		if(me->mind1)
		{
			me->facing=(byte)MGL_random(8);
			me->mind1=0;
			me->reload=32;
		}

		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
		}
		me->dx=Cosine(me->facing*32)*8;
		me->dy=Sine(me->facing*32)*8;
	}
}

void AI_SDZL(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int i,x,y;

	if(!goodguy)
		return;	// no point in doing anything without an opponent

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SDZLOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SDZLDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm>=9 && me->frm<=13 && me->mind1==0)
		{
			// slash for pain
			x=(me->x>>FIXSHIFT)+50;
			y=(me->y>>FIXSHIFT)+10;
			if(me->AttackCheck(100,x,y,goodguy))
			{
				goodguy->GetShot(-FIXAMT*12,FIXAMT*4,25,map,world);
				goodguy->dx=-FIXAMT*12;
				goodguy->dy=FIXAMT*2;
				me->mind1=1;	// don't hit him again
			}
		}
		if(me->seq==ANIM_A2 && me->frm==12 && me->frmTimer<64)
		{
			// spew projectiles
			FireExactBullet(me->x,me->y+FIXAMT*20,FIXAMT*60,
							-FIXAMT*6,FIXAMT*2,FIXAMT,0,60,2,BLT_GRENADE);
			FireExactBullet(me->x,me->y+FIXAMT*20,FIXAMT*60,
							-FIXAMT*4,FIXAMT*4,FIXAMT,0,60,2,BLT_GRENADE);
			FireExactBullet(me->x,me->y+FIXAMT*20,FIXAMT*60,
							0,FIXAMT*5,FIXAMT,0,60,2,BLT_GRENADE);
			FireExactBullet(me->x,me->y+FIXAMT*20,FIXAMT*60,
							FIXAMT*4,FIXAMT*4,FIXAMT,0,60,2,BLT_GRENADE);
			FireExactBullet(me->x,me->y+FIXAMT*20,FIXAMT*60,
							FIXAMT*6,FIXAMT*2,FIXAMT,0,60,2,BLT_GRENADE);
		}
		if(me->seq==ANIM_A3 && (me->frm==7 || me->frm==8))
		{
			ShakeScreen(30);
			MakeSound(SND_SDZLPOUND,me->x,me->y,SND_CUTOFF,1200);
			for(i=0;i<10;i++)
			{
				x=me->x+((256-MGL_random(513))<<FIXSHIFT);
				y=me->y+((256-MGL_random(513))<<FIXSHIFT);
				FireExactBullet(x,y,FIXAMT*80+(MGL_random(40)<<FIXSHIFT),
								0,0,0,0,30,2,BLT_ROCK);
			}
		}

		if(me->seq==ANIM_DIE && me->frm==14 && me->frmTimer<32)
		{
			if(GetPlayerWorld()==4)	// this is the real asylum, not some knock-off
			{
				coro::launch(std::bind(ShowVictoryAnim, 4));	// you killed him.
				SendMessageToGame(MSG_WINGAME,0);
			}
		}
		return;	// can't do nothin' right now
	}

	me->mind1=0;

	FaceGoodguy2(me,goodguy);

	if(me->reload==0 && RangeToTarget(me,goodguy)<256*FIXAMT)
	{
		i=MGL_random(1024);
		if(i<40)
		{
			MakeSound(SND_SDZLSLASH,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_A3;	// ground pound!
			me->frmTimer=0;
			me->frm=0;
			me->frmAdvance=128;
			me->dx=0;
			me->dy=0;
			me->action=ACTION_BUSY;
			me->reload=60;
			return;
		}
		else if(i<100)
		{
			MakeSound(SND_SDZLSPIT,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_A2;	// breathe stuff
			me->frmTimer=0;
			me->frm=0;
			me->frmAdvance=128;
			me->dx=0;
			me->dy=0;
			me->action=ACTION_BUSY;
			me->reload=60;
			return;
		}
		else if(i<180)
		{
			MakeSound(SND_SDZLSLASH,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_ATTACK;	// slash
			me->frmTimer=0;
			me->frm=0;
			me->frmAdvance=128;
			me->dx=0;
			me->dy=0;
			me->action=ACTION_BUSY;
			me->reload=30;
			return;
		}
	}

	if(me->facing==7 || me->facing<3)
		i=ANIM_MOVE;	// move to the right
	else
		i=ANIM_A1;	// move to the left

	if(me->seq!=i)
	{
		me->seq=i;
		me->frmTimer=0;
		me->frm=0;
		me->frmAdvance=128;
		me->dx=Cosine(me->facing*32)*4;
		me->dy=Sine(me->facing*32)*3;
	}
}

void AI_Santa(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SANTAOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SANTADIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==8 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			if(me->AttackCheck(32,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(Cosine(me->facing*32)*6,Sine(me->facing*32)*6,16,map,world);
			}
			if(me->mind1)
			{
				me->frm=3;
				me->mind1--;
			}
			else
				me->reload=10;
		}
		if(me->seq==ANIM_DIE && me->frm>5)
			me->frmAdvance=128;

		return;	// can't do nothin' right now
	}

	if(goodguy)
	{
		FaceGoodguy2(me,goodguy);
		if(RangeToTarget(me,goodguy)<(96*FIXAMT) && MGL_random(16)==0 && me->reload==0)
		{
			// smack him!
			MakeSound(SND_SANTAHIT,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=255;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=1;
			me->mind1=2;	// tell him to repeat twice
			return;
		}

		me->dx=Cosine(me->facing*32)*6;
		me->dy=Sine(me->facing*32)*6;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
}

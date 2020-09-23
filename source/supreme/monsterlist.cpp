//jeff
#define MT_WEIRD MT_BITS
#define MT_SPOOKY MT_MANSION
#define MT_PYGMY MT_ISLAND
#define MT_HIGHTECH MT_SPACE
#define MT_THINGY MT_ALIEN
monsterType_t monsType[NUM_MONSTERS]=
	{
		{"Null",255,255,0,NULL,
		 0,0,0,0,"",0,0,0},
		{"Bouapha",255,255,0,NULL,
		 11,29,128,0,"graphics/bouapha.jsp",0,MF_WATERWALK,
		 MT_GOOD|MT_HUMAN,MG_DRL,MC_GREEN,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,15,16,255},	// attack
				{0,0,0,0,0,0,0,26,27,28,255},		// die
				{23,24,25,24,23,255},	// use item
				{17,18,19,20,21,22,21,20,21,22,21,20,19,18,17,255},	// bored
				{1,3,1,0,4,6,4,0,1,3,1,0,0,0,26,27,28,255},	// watery death
				{7,8,9,10,11,12,13,14,15,16,255},	// attack
				{0,255},
			}},
		{"Bonehead",255,255,0,AI_Bonehead,
		 8,38,10,50,"graphics/skeleton.jsp",0,0,
		 MT_SPOOKY|MT_FOREST|MT_CAVES|MT_SKELETON|MT_EVIL,MG_DRL,MC_GREY,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{20,21,22,23,24,255},	// attack
				{7,8,9,10,11,12,13,14,15,16,255},		// die
				{25,26,27,28,29,30,255},	// chomp/spit projectile
				{31,32,32,32,31,255},	// point at bouapha
				{16,15,14,13,12,11,10,9,8,7,255},	// resurrect=A3
				{0,255},
				{0,255},
			}},
		{"Scary Bat",255,255,0,AI_Bat,
		 8,27,5,25,"graphics/bat.jsp",0,MF_FLYING,
		 MT_SPOOKY|MT_CAVES|MT_FLYING|MT_EVIL|MT_ANIMAL,MG_DRL,MC_BROWN,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,8,7,255},	// attack
				{17,18,19,20,21,22,23,24,25,26,255},		// die
				{10,11,12,12,12,12,12,11,10,255},	// diving attack
				{13,14,15,15,16,255},	// bounce off during dive
				{7,8,9,8,7,255},	// attack
				{0,255},
				{0,255},
			}},
		{"Eensy Weensy",255,255,0,AI_Spider,
		 4,22,1,10,"graphics/spider.jsp",0,0,
		 MT_EVIL|MT_SPIDER|MT_ANIMAL|MT_FOREST|MT_CAVES,MG_DRL,MC_BLUE,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,8,9,10,11,0,255},	// move
				{12,13,14,13,12,255},	// attack
				{15,16,17,18,19,18,19,18,19,20,20,21,21,255},	// die
				{12,13,14,13,12,255},	// attack
				{12,13,14,13,12,255},	// attack
				{12,13,14,13,12,255},	// attack
				{0,255},
				{0,255},
			}},
		{"Spitter",255,255,0,AI_BigSpider,
		 8,21,10,50,"graphics/bigspdr.jsp",0,0,
		 MT_EVIL|MT_SPIDER|MT_ANIMAL|MT_FOREST|MT_CAVES,MG_DRL,MC_GREEN,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,12,13,13,255},	// attack
				{14,15,16,17,18,17,18,17,18,19,19,20,20,255},	// die
				{7,8,9,10,11,12,12,13,13,255},	// attack
				{7,8,9,10,11,12,12,13,13,255},	// attack
				{7,8,9,10,11,12,12,13,13,255},	// attack
				{0,255},
				{0,255},
			}},
		{"Zombie",255,255,0,AI_Zombie,
		 11,32,20,150,"graphics/zombie.jsp",0,0,
		 MT_SPOOKY|MT_ZOMBIE|MT_EVIL|MT_ASYLUM|MT_FOREST,MG_DRL,MC_GREY,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,9,10,11,12,13,13,14,15,16,17,18,19,255},	// attack
				{20,21,22,23,24,25,26,27,28,29,30,31,255},	// die
				{7,8,9,9,10,11,12,13,13,14,15,16,17,18,19,255},	// attack
				{7,8,9,9,10,11,12,13,13,14,15,16,17,18,19,255},	// attack
				{7,8,9,9,10,11,12,13,13,14,15,16,17,18,19,255},	// attack
				{0,255},
				{0,255},
			}},
		{"Egg Sac",255,255,0,AI_EggSac,
		 20,19,150,1000,"graphics/eggsac.jsp",0,MF_ONEFACE|MF_ENEMYWALK|MF_NOMOVE,
		 MT_EVIL|MT_GENERATE|MT_SPIDER|MT_FOREST|MT_CAVES,MG_DRL,MC_GREY,
			{
				{0,0,0,0,0,0,0,0,1,2,1,0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,0,255},	// attack
				{19,20,21,22,23,24,25,255},	// die
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,0,255},	// attack
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,0,255},	// attack
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,0,255},	// attack
				{0,255},
				{0,255},
			}},
		{"Mama Spider",255,255,0,AI_MamaSpider,
		 30,30,200,1500,"graphics/mamaspdr.jsp",0,MF_NOMOVE,
		 MT_EVIL|MT_MINIBOSS|MT_SPIDER|MT_FOREST|MT_CAVES|MT_ANIMAL,MG_DRL,MC_VIOLET,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,12,13,13,255},	// spit (attack)
				{19,20,21,22,23,24,25,25,25,26,27,28,29,255},	// die
				{14,15,16,17,18,255},	// bite (A1)
				{7,8,9,10,11,12,12,13,13,255},	// spit (attack)
				{7,8,9,10,11,12,12,13,13,255},	// spit (attack)
				{0,255},
				{0,255},
			}},
		{"Pygmy",255,255,0,AI_Pygmy,
		 10,50,15,100,"graphics/pygmy.jsp",0,0,
		 MT_EVIL|MT_PYGMY|MT_HUMAN,MG_DRL,MC_VIOLET,
			{
				{0,0,0,0,0,0,0,0,0,0,1,2,3,2,1,0,0,0,4,5,6,5,4,255},	// idle
				{8,9,10,9,8,7,11,12,13,12,11,7,255},	// move
				{29,30,31,31,32,33,34,35,36,255},	// attack
				{37,38,39,40,41,42,43,44,45,46,255},	// die
				{14,15,16,17,18,19,20,16,17,18,19,15,14,255},	// (A1) bored (spin spear)
				{21,22,23,24,25,26,26,26,27,28,27,26,27,28,27,26,255}, // (A2) bored (yawn)
				{47,48,49,48,47,0,47,48,49,48,47,255},	// (A3) angry, spotted Bouapha
				{0,255},
				{0,255},
			}},
		{"Aquazoid",255,255,0,AI_Serpent,
		 15,20,15,75,"graphics/serpent.jsp",0,MF_AQUATIC|MF_WATERWALK,
		 MT_AQUATIC|MT_EVIL|MT_ZOID|MT_FOREST,MG_DRL,MC_VIOLET,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,255},	// move
				{4,5,6,7,8,9,10,11,12,255},	// spit
				{13,14,15,16,17,18,19,255},	// die
				{4,5,6,7,8,9,10,11,12,255},	// spit
				{4,5,6,7,8,9,10,11,12,255},	// spit
				{4,5,6,7,8,9,10,11,12,255},	// spit
				{0,255},
				{0,255},
			}},
		{"Matilda",255,255,0,AI_MattieSkullOrHead,	// the head of matilda
		 40,11,300,1000,"graphics/mathead.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_FACECMD|MF_NOGRAV|MF_ENEMYWALK|MF_FREEWALK,
		 MT_EVIL|MT_SPIDER|MT_BOSS|MT_FOREST,MG_DRL,MC_RED,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,4,4,4,4,4,4,3,2,1,255},	// fire
				{1,2,0,3,4,3,2,0,3,4,3,0,1,4,2,4,3,1,2,255},	// die
				{1,2,3,4,4,4,4,4,4,4,3,2,1,255},	// fire
				{1,2,3,4,4,4,4,4,4,4,3,2,1,255},	// fire
				{1,2,3,4,4,4,4,4,4,4,3,2,1,255},	// fire
				{0,255},
				{0,255},
			}},
		{"Matilda's Skull",255,255,0,AI_MattieSkullOrHead,	// the skull of matilda (head becomes this when killed)
		 40,11,200,1000,"graphics/matskull.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_FACECMD|MF_NOGRAV|MF_ENEMYWALK|MF_FREEWALK,
		 MT_BITS,MG_DRL,MC_RED,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,4,4,4,4,4,4,3,2,1,255},	// fire
				{1,2,0,3,4,3,2,0,3,4,3,0,1,4,2,4,3,1,2,255},	// die
				{1,2,3,4,4,4,4,4,4,4,3,2,1,255},	// fire
				{1,2,3,4,4,4,4,4,4,4,3,2,1,255},	// fire
				{1,2,3,4,4,4,4,4,4,4,3,2,1,255},	// fire
				{0,255},
				{0,255},
			}},
		{"Matilda's Brain",255,255,0,AI_MattieBrain,	// the brain of matilda (skull becomes this when killed)
		 40,7,100,5000,"graphics/matbrain.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_FACECMD|MF_NOGRAV|MF_ENEMYWALK|MF_FREEWALK,
		 MT_BITS,MG_DRL,MC_GREY,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// no form of attack
				{1,2,3,2,1,0,4,5,6,5,4,0,1,2,3,2,1,0,4,5,6,5,255},	// die
				{0,255},
				{0,255},
				{0,255},
				{0,255},
				{0,255},
			}},
		{"Matilda's Body",255,255,0,AI_MattieBody,	// the body of matilda
		 50,7,1,1,"graphics/matbody.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_INVINCIBLE|MF_NOGRAV|MF_ENEMYWALK,
		 MT_BITS,MG_DRL,MC_RED,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{0,255},	// fire
				{1,2,0,3,4,3,2,0,3,4,3,0,1,4,2,4,3,1,2,255},	// die
				{0,255},
				{0,255},
				{0,255},
				{0,255},
				{0,255},
			}},
		{"Matilda's Claw",255,255,0,AI_MattieClaw,	// the left claw (her right)
		 20,16,1000,1000,"graphics/matclaw1.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_SPRITEBOX|MF_NOGRAV|MF_ENEMYWALK|MF_FREEWALK,
		 MT_BITS,MG_DRL,MC_GREY,
			{
				{0,1,2,3,2,1,255},	// idle
				{0,255},	// move
				{7,8,9,10,11,12,13,14,15,255},	// claw slash
				{1,2,0,3,4,3,2,0,3,4,3,0,1,4,2,4,3,1,2,255},	// die
				{4,5,6,6,6,6,6,6,6,5,4,255},	// block (A1)
				{7,8,9,10,11,12,13,14,15,255},	// claw slash
				{7,8,9,10,11,12,13,14,15,255},	// claw slash
				{0,255},
				{0,255},
			}},
		{"Matilda's Claw",255,255,0,AI_MattieClaw,	// the right claw (her left)
		 20,16,1000,1000,"graphics/matclaw2.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_SPRITEBOX|MF_NOGRAV|MF_ENEMYWALK|MF_FREEWALK,
		 MT_BITS,MG_DRL,MC_GREY,
			{
				{0,1,2,3,2,1,255},	// idle
				{0,255},	// move
				{7,8,9,10,11,12,13,14,15,255},	// claw slash
				{1,2,0,3,4,3,2,0,3,4,3,0,1,4,2,4,3,1,2,255},	// die
				{4,5,6,6,6,6,6,6,6,5,4,255},	// block (A1)
				{7,8,9,10,11,12,13,14,15,255},	// claw slash
				{7,8,9,10,11,12,13,14,15,255},	// claw slash
				{0,255},
				{0,255},
			}},
		{"Matilda's Tail",255,255,0,AI_MattieTail,	// her tail
		 80,16,200,1000,"graphics/mattail.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_INVINCIBLE|MF_FACECMD|MF_NOGRAV|MF_ENEMYWALK|MF_FREEWALK,
		 MT_BITS,MG_DRL,MC_RED,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,1,2,3,2,1,255},	// fire
				{1,2,0,3,2,0,3,3,0,1,2,3,1,2,255},	// die
				{0,1,2,3,2,1,255},	// fire
				{0,1,2,3,2,1,255},	// fire
				{0,255},
				{0,255},
				{0,255},
			}},
		{"Ninjabread Man",255,255,0,AI_Ginger,
		 12,33,50,300,"graphics/ginger.jsp",0,0,
		 MT_EVIL|MT_WACKY|MT_FOREST,MG_DRL,MC_BROWN,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,255},	// move
				{7,8,9,10,11,12,13,14,15,16,17,18,19,20,19,20,19,20,21,22,23,255},	// triple punch
				{27,28,29,30,31,32,32,32,32,255},	// die
				{24,25,26,26,26,26,26,255},	// jumpkick (A1)
				{25,24,255},	// unjumpkick (A2)
				{7,8,9,10,11,12,13,14,15,16,17,18,19,20,19,20,19,20,21,22,23,255},	// triple punch
				{0,255},
				{0,255},
			}},
		{"PUMPKIN!",255,255,0,AI_Pumpkin,
		 11,6,5,500,"graphics/pumpkin.jsp",0,0,
		 MT_EVIL|MT_PUMPKIN|MT_VEGGIE|MT_FOREST,MG_DRL,MC_BROWN,
			{
				{0,255},	// idle
				{1,2,3,4,5,0,255},	// move
				{1,2,3,4,5,0,255},	// move
				{0,0,255},	// die (just long enough to spew shrapnel)
				{1,2,3,4,5,0,255},	// move
				{1,2,3,4,5,0,255},	// move
				{1,2,3,4,5,0,255},	// move
				{1,2,3,4,5,0,255},	// move
				{1,2,3,4,5,0,255},	// move
			}},
		{"Thingie",255,255,0,AI_BabyThing,
		 12,44,10,50,"graphics/babything.jsp",0,0,
		 MT_EVIL|MT_THINGY|MT_CAVES,MG_DRL,MC_GREEN,
			{
				{12,255},	// idle
				{1,2,3,4,5,6,7,8,9,10,11,255},	// move
				{20,21,22,23,24,25,26,27,255},	// attack
				{34,35,36,37,38,39,40,41,42,43,255},	// die
				{13,14,15,16,17,16,15,16,17,16,15,16,17,16,15,255},	// fall asleep (A1)
				{18,19,18,255},	// blink (A2)
				{28,29,30,29,28,255}, // look right (A3)
				{31,32,33,32,31,255}, // look left (A4)
				{0,255},
			}},
		{"Mucho Moss",255,255,0,AI_Moss,
		 15,17,10,25,"graphics/moss.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_FREEWALK|MF_ENEMYWALK|MF_WALLWALK|MF_NOSHADOW,
		 MT_EVIL|MT_VEGGIE|MT_CAVES,MG_DRL,MC_GREEN,
			{
				{0,255},	// idle
				{1,2,3,2,1,255},	// move=grow vertically
				{4,5,6,5,4,255},	// attack=grow horizontally
				{7,8,9,10,11,12,13,14,15,16,255},	// die
				{4,5,6,5,4,255},	// attack=grow horizontally
				{4,5,6,5,4,255},	// attack=grow horizontally
				{4,5,6,5,4,255},	// attack=grow horizontally
				{0,255},
				{0,255},
			}},
		{"Moss Grande",255,255,0,AI_MossGrande,
		 18,17,100,250,"graphics/mossgrnd.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_ENEMYWALK,
		 MT_EVIL|MT_VEGGIE|MT_CAVES,MG_DRL,MC_GREEN,
			{
				{0,1,2,3,4,5,6,255},	// idle
				{0,255},	// move=unused
				{7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,255},	// attack
				{22,23,24,25,26,27,28,255},	// die
				{7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,255},	// attack
				{7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,255},	// attack
				{7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,255},	// attack
				{0,255},
				{0,255},
			}},
		{"Magmazoid",255,255,0,AI_Magmazoid,
		 15,20,20,80,"graphics/magmazoid.jsp",0,MF_AQUATIC|MF_WATERWALK,
		 MT_EVIL|MT_ZOID|MT_AQUATIC|MT_CAVES,MG_DRL,MC_RED,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,255},	// move
				{4,5,6,7,8,9,10,10,10,10,10,10,10,11,12,255},	// attack
				{13,14,15,16,17,18,19,255},	// die
				{4,5,6,7,8,9,10,10,10,10,10,10,10,11,12,255},	// attack
				{4,5,6,7,8,9,10,10,10,10,10,10,10,11,12,255},	// attack
				{4,5,6,7,8,9,10,10,10,10,10,10,10,11,12,255},	// attack
				{0,255},
				{0,255},
			}},
		{"Shroom",255,255,0,AI_Shroom,
		 15,23,10,50,"graphics/shroom.jsp",0,0,
		 MT_EVIL|MT_VEGGIE|MT_CAVES|MT_FOREST,MG_DRL,MC_GREEN,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,255},	// attack
				{15,16,17,18,19,20,21,22,255},	// die
				{7,8,9,10,11,12,13,14,255},	// a1
				{7,8,9,10,11,12,13,14,255},	// attack
				{7,8,9,10,11,12,13,14,255},	// attack
				{0,255},
				{0,255},
			}},
		{"Mush the Shroom",255,255,0,AI_Mush,
		 32,27,200,800,"graphics/bgshroom.jsp",0,MF_NOMOVE,
		 MT_EVIL|MT_MINIBOSS|MT_VEGGIE|MT_CAVES,MG_DRL,MC_GREEN,
			{
				{0,255},	// idle
				{1,2,2,1,0,3,4,4,3,0,255},	// move
				{5,6,6,7,8,9,10,255},	// attack=cannon fire
				{21,22,23,24,25,26,26,26,26,255},	// die
				{11,12,13,12,13,12,13,12,13,12,13,12,13,12,13,12,13,14,15,16,255},	// A1=sporegun fire
				{17,18,19,20,19,18,19,20,19,18,17,255},	// A2=angry
				{5,6,6,7,8,9,10,255},	// attack=cannon fire
				{0,255},
				{0,255},
			}},
		{"The Thing",255,255,0,AI_TheThing,
		 64,11,800,5000,"graphics/thething.jsp",0,MF_NOMOVE|MF_AQUATIC|MF_WATERWALK|MF_ENEMYWALK|MF_ONEFACE,
		 MT_EVIL|MT_THINGY|MT_BOSS|MT_CAVES,MG_DRL,MC_GREEN,
			{
				{0,255},	// idle
				{0,255},	// move
				{3,4,5,6,7,8,9,10,10,10,9,8,7,6,5,4,3,255},	// attack=tongue
				{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255},	// die
				{1,2,1,255},	// A1=blink
				{3,4,5,6,7,8,9,10,10,10,9,8,7,6,5,4,3,255},	// attack=tongue
				{3,4,5,6,7,8,9,10,10,10,9,8,7,6,5,4,3,255},	// attack=tongue
				{0,255},
				{0,255},
			}},
		{"Tentacle",255,255,0,AI_ThingTentacle,
		 32,1,500,100,"graphics/thingtent.jsp",0,MF_NOMOVE|MF_FREEWALK|MF_NOGRAV|MF_ENEMYWALK,
		 MT_BITS,MG_DRL,MC_GREEN,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{0,0,0,0,0,255},	// die
				{0,255},
				{0,255},
				{0,255},
				{0,255},
				{0,255},
			}},
		{"Tentacle",255,255,0,AI_ThingTentacle,	// tentacle tip
		 32,1,500,100,"graphics/thingtent2.jsp",0,MF_NOMOVE|MF_FREEWALK|MF_NOGRAV|MF_ENEMYWALK,
		 MT_BITS,MG_DRL,MC_GREEN,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{0,0,0,0,0,255},	// die
				{0,255},
				{0,255},
				{0,255},
				{0,255},
				{0,255},
			}},
		{"Super Zombie",255,255,0,AI_SuperZombie,
		 36,38,300,2500,"graphics/suprzmbe.jsp",0,MF_NOMOVE,
		 MT_EVIL|MT_MINIBOSS|MT_SPOOKY|MT_ZOMBIE|MT_CAVES|MT_ASYLUM,MG_DRL,MC_GREY,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{17,18,19,20,21,22,23,24,25,255},	// attack=punch
				{9,10,11,12,13,14,15,15,15,15,15,15,255},	// die
				{7,8,9,10,11,11,11,12,12,13,13,13,14,14,14,15,16,16,255},	// A1=leap
				{26,27,28,29,30,255},	// A2=attempted grab
				{27,31,32,33,34,35,34,36,37,36,34,35,34,36,37,36,34,35,34,33,31,27,255},	//A3= grab n' pound
				{0,255},
				{0,255},
			}},
		{"Happy Stick Man",255,255,0,AI_StickMan,
		 16,12,100,1000,"graphics/stickman.jsp",0,MF_ONEFACE,
		 MT_EVIL|MT_WACKY|MT_MINIBOSS|MT_HUMAN|MT_CAVES,MG_DRL,MC_AQUA,
			{
				{0,255},	// idle
				{1,0,2,0,1,0,2,0,1,0,2,0,255},	// move
				{10,11,11,10,10,10,10,10,10,10,255},	// attack=chomp
				{3,3,3,3,3,3,4,4,5,5,255},	// die
				{6,7,7,7,6,0,8,9,9,9,8,255},	// A1=show off muscles
				{3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,255},	// A2=sadness
				{1,0,2,0,1,0,2,0,1,0,2,0,0,0,3,4,5,255},	// watery death
				{0,255},
				{0,255},
			}},
		{"Baby SEAL",255,255,0,AI_BabySeal,
		 22,20,20,125,"graphics/babyseal.jsp",0,0,
		 MT_EVIL|MT_ANIMAL|MT_ARCTIC,MG_DRL,MC_GREY,
			{
				{0,255},	// idle
				{1,2,3,4,5,0,255},	// move
				{6,6,7,8,7,8,9,10,9,10,11,12,11,12,13,13,13,14,14,14,255},	// attack=full auto
				{15,16,17,18,19,19,19,255},	// die
				{6,6,7,8,7,8,9,10,9,10,11,12,11,12,13,13,13,14,14,14,255},	// attack=full auto
				{6,6,7,8,7,8,9,10,9,10,11,12,11,12,13,13,13,14,14,14,255},	// attack=full auto
				{0,255},
				{0,255},
				{0,255},
			}},
		{"Cryozoid",255,255,0,AI_Isozoid,
		 15,23,20,75,"graphics/isozoid.jsp",0,MF_NOMOVE,
		 MT_EVIL|MT_ZOID|MT_ARCTIC|MT_TRAP,MG_DRL,MC_BROWN,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,255},	// move
				{10,11,12,13,14,15,16,255},	// attack=spit snow
				{17,18,19,20,21,22,255},	// die
				{4,5,6,7,8,9,255},	//A1=rise out of the snow
				{254,255},			//A2=idle under the ground
				{10,11,12,13,14,15,16,255},	// attack=spit snow
				{0,255},
				{0,255},
			}},
		{"Snowguy",255,255,0,AI_Snowguy,
		 26,36,40,200,"graphics/snowguy.jsp",0,0,
		 MT_EVIL|MT_ARCTIC,MG_DRL,MC_GREY,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,0,255},	// move
				{24,25,26,27,28,29,255},	// attack=slash
				{30,31,32,33,34,35,254,35,254,35,254,35,254,254,35,255},	// die
				{8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,255},	//A1=throw snowball
				{24,25,26,27,28,29,255},	// attack=slash
				{24,25,26,27,28,29,255},	// attack=slash
				{0,255},
				{0,255},
			}},
		{"Pengulon",255,255,0,AI_Penguin,
		 10,8,5,50,"graphics/penguin.jsp",0,MF_FLYING,
		 MT_EVIL|MT_FLYING|MT_ARCTIC|MT_ANIMAL,MG_DRL,MC_GREY,
			{
				{0,1,2,3,4,5,4,3,2,1,255},	// idle
				{0,1,2,3,4,5,4,3,2,1,255},	// move
				{6,7,7,7,6,6,3,3,3,3,3,255},	// attack=dive
				{0,254,3,254,5,254,1,254,4,254,2,254,3,255},	// die
				{6,7,7,7,6,6,3,3,3,3,3,255},	// attack=dive
				{6,7,7,7,6,6,3,3,3,3,3,255},	// attack=dive
				{6,7,7,7,6,6,3,3,3,3,3,255},	// attack=dive
				{0,255},
				{0,255},
			}},
		{"Zomboni",255,255,0,AI_Zomboni,
		 48,9,100,500,"graphics/zomboni.jsp",0,MF_NOMOVE,
		 MT_EVIL|MT_VEHICLE|MT_ARCTIC|MT_ZOMBIE|MT_SPOOKY,MG_DRL,MC_RED,
			{
				{0,255},	// idle
				{0,1,2,0,1,2,255},	// move
				{0,255},	// attack=unused
				{3,4,5,5,5,6,7,8,8,255},	// die
				{0,255},
				{0,255},
				{0,255},
				{0,255},
				{0,255},
			}},
		{"Sven",255,255,0,AI_Yeti,
		 48,40,500,5000,"graphics/yeti.jsp",0,MF_NOMOVE,
		 MT_EVIL|MT_BOSS|MT_ARCTIC|MT_ANIMAL,MG_DRL,MC_GREY,
			{
				{0,255},	// idle
				{0,1,2,1,0,3,4,3,255},	// move
				{14,15,16,17,18,19,20,21,22,23,255},	// attack=snowball
				{35,36,37,38,39,39,39,39,39,39,39,39,39,39,39,39,39,39,255},	// die
				{28,29,30,31,32,33,33,33,34,34,255},	// A1=ground pound
				{5,6,7,8,9,10,11,12,13,255},	//A2=death yodel
				{24,25,26,27,26,25,26,27,26,25,26,27,26,25,24,255},	// A3=wave hello
				{0,255},
				{0,255},
			}},
		{"Bjorn",1,4,2,AI_Yeti,
		 48,40,500,5000,"!36",0,MF_NOMOVE,
		 MT_EVIL|MT_BOSS|MT_ARCTIC|MT_ANIMAL,MG_DRL,MC_GREY,
			{
				{0,255},	// idle
				{0,1,2,1,0,3,4,3,255},	// move
				{14,15,16,17,18,19,20,21,22,23,255},	// attack=snowball
				{35,36,37,38,39,39,39,39,39,39,39,39,39,39,39,39,39,39,255},	// die
				{28,29,30,31,32,33,33,33,34,34,255},	// A1=ground pound
				{5,6,7,8,9,10,11,12,13,255},	//A2=death yodel
				{24,25,26,27,26,25,26,27,26,25,26,27,26,25,24,255},	// A3=wave hello
				{0,255},
				{0,255},
			}},
		{"Geozoid",255,255,0,AI_Geozoid,
		 15,20,20,75,"graphics/geozoid.jsp",0,MF_NOMOVE,
		 MT_EVIL|MT_ZOID|MT_DESERT,MG_DRL,MC_BROWN,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,255},	// move
				{4,5,6,7,8,9,10,11,12,255},	// attack=spit rock
				{13,14,15,16,17,18,19,255},	// die
				{4,5,6,7,8,9,10,11,12,255},	// attack=spit rock
				{4,5,6,7,8,9,10,11,12,255},	// attack=spit rock
				{4,5,6,7,8,9,10,11,12,255},	// attack=spit rock
				{0,255},
				{0,255},
			}},
		{"Mumble",255,255,0,AI_Mumble,
		 15,28,30,100,"graphics/mumble.jsp",0,MF_NOMOVE,
		 MT_EVIL|MT_DESERT|MT_SPOOKY,MG_DRL,MC_GREY,
			{
				{0,255},	// idle
				{1,2,3,4,5,0,6,7,8,9,10,0,255},	// move
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{19,20,21,22,23,24,25,26,27,255},	// die
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{0,255},
				{0,255},
			}},
		{"Djinni",255,255,0,AI_Djinni,
		 12,24,10,50,"graphics/djinni.jsp",0,MF_WALLWALK|MF_NOSHADOW|MF_FLYING|MF_GHOST|MF_ENEMYWALK|MF_FREEWALK,
		 MT_EVIL|MT_SPOOKY|MT_DESERT|MT_FLYING|MT_HUMAN,MG_DRL,MC_GLOW,
			{
				{0,1,2,3,2,1,255},	// idle
				{0,1,2,3,2,1,255},	// move
				{12,13,14,15,16,17,18,19,255},	// attack=punch
				{20,21,22,23,255},	// die
				{4,5,6,7,8,9,8,7,6,7,8,9,8,7,6,7,8,9,10,11,255},	// A1=scare
				{23,22,21,20,255},	// A2=materialize
				{12,13,14,15,16,17,18,19,255},	// attack=punch
				{0,255},
				{0,255},
			}},
		{"Magic Lamp",255,255,0,AI_MagicLamp,
		 15,10,150,500,"graphics/lamp.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_ENEMYWALK,
		 MT_EVIL|MT_GENERATE|MT_DESERT|MT_TRAP,MG_DRL,MC_BROWN,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,7,8,0,1,2,3,4,5,6,7,8,0,1,255},	// attack=summon Djinni
				{1,5,8,3,7,2,0,8,254,1,254,7,254,5,254,8,254,2,255},	// die
				{1,2,3,4,5,6,7,8,0,1,2,3,4,5,6,7,8,0,1,255},	// attack=summon Djinni
				{1,2,3,4,5,6,7,8,0,1,2,3,4,5,6,7,8,0,1,255},	// attack=summon Djinni
				{1,2,3,4,5,6,7,8,0,1,2,3,4,5,6,7,8,0,1,255},	// attack=summon Djinni
				{0,255},
				{0,255},
			}},
		{"Cacton",255,255,0,AI_Cactus,
		 20,25,25,100,"graphics/cactus.jsp",0,0,
		 MT_EVIL|MT_VEGGIE|MT_DESERT,MG_DRL,MC_GREEN,
			{
				{0,0,0,13,14,15,15,14,13,0,0,0,16,17,18,18,17,16,0,255},	// idle
				{3,4,5,4,3,255},	// move
				{6,7,8,8,9,10,11,12,255},	// attack=shoot spines
				{19,20,21,21,22,23,24,24,254,24,254,24,254,24,254,24,255},	// die
				{1,2,3,255},	// A1=begin moving
				{3,2,1,255},	// A2=stop moving
				{6,7,8,8,9,10,11,12,255},	// attack=shoot spines
				{0,255},
				{0,255},
			}},
		{"Roly Poly",255,255,0,AI_Roller,
		 35,11,2000,1000,"graphics/roller.jsp",0,MF_NOMOVE|MF_ONEFACE,
		 MT_EVIL|MT_DESERT|MT_TRAP,MG_DRL,MC_BROWN,
			{
				{0,255},	// idle
				{1,2,3,4,5,0,255},	// move (roll down the screen)
				{5,4,3,2,1,0,255},	// attack=roll up the screen
				{0,0,255},	// die
				{6,7,8,9,10,0,255},	// A1=roll to the right
				{10,9,8,7,6,0,255},	// A2=roll to the left
				{5,4,3,2,1,0,255},	// attack=roll up the screen
				{0,255},
				{0,255},
			}},
		{"Richie Lich",255,255,0,AI_Lich,
		 50,8,500,1500,"graphics/lich.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_GHOST|MF_NOSHADOW|MF_FLYING,
		 MT_EVIL|MT_SPOOKY|MT_MINIBOSS|MT_FLYING|MT_SKELETON,MG_DRL,MC_GLOW,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,4,4,4,4,4,4,4,4,4,4,4,4,3,2,1,255},	// attack=open mouth
				{0,5,6,7,8,255},	// die
				{1,2,3,4,4,4,4,4,4,4,4,4,4,4,4,4,3,2,1,255},	// attack=open mouth
				{1,2,3,4,4,4,4,4,4,4,4,4,4,4,4,4,3,2,1,255},	// attack=open mouth
				{1,2,3,4,4,4,4,4,4,4,4,4,4,4,4,4,3,2,1,255},	// attack=open mouth
				{0,255},
				{0,255},
			}},
		{"Dust Devil",255,255,0,AI_DustDevil,
		 50,23,450,2000,"graphics/dustdevil.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_GHOST|MF_NOSHADOW,
		 MT_EVIL|MT_WACKY|MT_DESERT|MT_MINIBOSS,MG_DRL,MC_GLOW,
			{
				{0,1,2,3,4,5,255},	// idle
				{0,1,2,3,4,5,255},	// move
				{6,7,8,9,10,11,12,13,14,15,16,255},	// attack=swipe pitchfork
				{17,18,19,20,21,22,255},	// die
				{6,7,8,9,10,11,12,13,14,15,16,255},	// attack=swipe pitchfork
				{6,7,8,9,10,11,12,13,14,15,16,255},	// attack=swipe pitchfork
				{6,7,8,9,10,11,12,13,14,15,16,255},	// attack=swipe pitchfork
				{0,255},
				{0,255},
			}},
		{"MechaBouapha",255,255,0,AI_MechaBouapha,
		 20,24,250,1000,"graphics/mechabouapha.jsp",0,0,
		 MT_EVIL|MT_HIGHTECH|MT_HUMAN|MT_ASYLUM,MG_DRL,MC_GREY,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,255},	// attack=hammer toss
				{15,16,17,18,19,20,21,22,23,255},	// die
				{15,16,17,18,19,20,21,22,255},	// A1=melt
				{23,255},	// A2=ball form
				{22,21,20,19,18,17,16,15,255},	// A3=unmelt
				{0,255},
				{0,255},
			}},
		{"Sphinx Arm",255,255,0,AI_SphinxArm,	// arm1
		 30,10,600,2000,"graphics/sphinxarm1.jsp",0,MF_ONEFACE|MF_NOMOVE|MF_SPRITEBOX,
		 MT_BITS,MG_DRL,MC_BROWN,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,7,7,8,9,255},	// attack=swipe
				{0,1,2,3,2,3,4,3,2,3,4,3,254,4,254,3,254,4,255},	// die
				{1,2,3,4,5,6,7,7,8,9,255},	// attack=swipe
				{1,2,3,4,5,6,7,7,8,9,255},	// attack=swipe
				{1,2,3,4,5,6,7,7,8,9,255},	// attack=swipe
				{1,2,3,4,5,6,7,7,8,9,255},	// attack=swipe
			}},
		{"Sphinx Arm",255,255,0,AI_SphinxArm,	// arm2
		 30,10,600,2000,"graphics/sphinxarm2.jsp",0,MF_ONEFACE|MF_NOMOVE|MF_SPRITEBOX,
		 MT_BITS,MG_DRL,MC_BROWN,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,7,7,8,9,255},	// attack=swipe
				{0,1,2,3,2,3,4,3,2,3,4,3,254,4,254,3,254,4,255},	// die
				{1,2,3,4,5,6,7,7,8,9,255},	// attack=swipe
				{1,2,3,4,5,6,7,7,8,9,255},	// attack=swipe
				{1,2,3,4,5,6,7,7,8,9,255},	// attack=swipe
				{1,2,3,4,5,6,7,7,8,9,255},	// attack=swipe
			}},
		{"Sphinxter",255,255,0,AI_Sphinx,
		 80,11,1000,5000,"graphics/sphinx.jsp",0,MF_ONEFACE|MF_NOMOVE,
		 MT_EVIL|MT_BOSS|MT_DESERT,MG_DRL,MC_BROWN,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,255},	// attack=summon mummy
				{10,10,10,10,10,10,10,10,10,255},	// die
				{4,5,6,7,7,6,7,7,7,6,7,8,9,10,255}, // A1=sneeze
				{1,2,3,255},	// attack=summon mummy
				{1,2,3,255},	// attack=summon mummy
				{1,2,3,255},	// attack=summon mummy
			}},
		{"Freakazoid",255,255,0,AI_Freakazoid,
		 15,18,15,75,"graphics/freakazoid.jsp",0,MF_AQUATIC|MF_WATERWALK,
		 MT_EVIL|MT_ZOID|MT_WACKY|MT_AQUATIC|MT_ASYLUM,MG_DRL,MC_BLUE,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,255},	// move
				{4,5,6,7,8,9,9,9,9,10,255},	// attack=spit spines
				{11,12,13,14,15,16,17,255},	// die
				{4,5,6,7,8,9,9,9,9,10,255},	// attack=spit spines
				{4,5,6,7,8,9,9,9,9,10,255},	// attack=spit spines
				{4,5,6,7,8,9,9,9,9,10,255},	// attack=spit spines
				{4,5,6,7,8,9,9,9,9,10,255},	// attack=spit spines
			}},
		{"Centipumpkin",255,255,0,AI_CentiBody,	// body
		 14,5,20,200,"graphics/cpbody.jsp",0,MF_ENEMYWALK,
		 MT_BITS,MG_DRL,MC_BROWN,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{0,255},	// attack
				{0,0,0,255},	// die
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
			}},
		{"Centipumpkin",255,255,0,AI_CentiHead,	// head
		 14,1,40,500,"graphics/cphead.jsp",0,MF_ENEMYWALK,
		 MT_EVIL|MT_VEGGIE|MT_PUMPKIN|MT_MINIBOSS|MT_DESERT,MG_DRL,MC_BROWN,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{0,0,0,255},	// die
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
			}},
		{"Wacko",255,255,0,AI_Wacko,
		 12,21,20,100,"graphics/wacko.jsp",0,0,
		 MT_EVIL|MT_HUMAN|MT_ASYLUM,MG_DRL,MC_YELLOW,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,11,10,9,10,11,12,11,10,9,10,11,12,13,14,255},	// attack=stab
				{15,16,17,18,19,20,254,20,254,20,254,20,254,20,255},	// die
				{7,8,9,10,11,12,11,10,9,10,11,12,11,10,9,10,11,12,13,14,255},	// attack=stab
				{7,8,9,10,11,12,11,10,9,10,11,12,11,10,9,10,11,12,13,14,255},	// attack=stab
				{7,8,9,10,11,12,11,10,9,10,11,12,11,10,9,10,11,12,13,14,255},	// attack=stab
				{7,8,9,10,11,12,11,10,9,10,11,12,11,10,9,10,11,12,13,14,255},	// attack=stab
			}},
		{"The Boiler",255,255,0,AI_Boiler,
		 100,22,1400,2000,"graphics/boiler.jsp",0,MF_NOMOVE|MF_ONEFACE,
		 MT_EVIL|MT_TRAP|MT_MINIBOSS|MT_URBAN|MT_ASYLUM,MG_DRL,MC_BROWN,
			{
				{0,1,2,3,4,5,6,7,255},	// idle
				{0,255},	// move
				{8,9,10,11,12,13,14,15,14,13,12,11,10,9,8,255},	// attack=flames
				{16,17,18,19,20,21,255},	// die
				{8,9,10,11,12,13,14,15,14,13,12,11,10,9,8,255},	// attack=flames
				{8,9,10,11,12,13,14,15,14,13,12,11,10,9,8,255},	// attack=flames
				{8,9,10,11,12,13,14,15,14,13,12,11,10,9,8,255},	// attack=flames
				{8,9,10,11,12,13,14,15,14,13,12,11,10,9,8,255},	// attack=flames
			}},
		{"Great Pumpkin",255,255,0,AI_GreatPumpkin,
		 120,13,1000,4000,"graphics/greatpk.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_ENEMYWALK,
		 MT_EVIL|MT_PUMPKIN|MT_VEGGIE|MT_MINIBOSS|MT_ASYLUM,MG_DRL,MC_BROWN,
			{
				{0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,8,255},	// idle
				{0,255},	// move
				{1,2,3,3,4,5,6,6,7,8,255},	// attack=vomit babies
				{9,9,10,10,11,11,12,12,255},	// die
				{1,2,3,3,4,5,6,6,7,8,255},	// attack=vomit babies
				{1,2,3,3,4,5,6,6,7,8,255},	// attack=vomit babies
				{1,2,3,3,4,5,6,6,7,8,255},	// attack=vomit babies
				{1,2,3,3,4,5,6,6,7,8,255},	// attack=vomit babies
			}},
		{"The Ultrazoid",255,255,0,AI_Ultrazoid,
		 40,17,750,2500,"graphics/ultrazoid.jsp",0,MF_NOMOVE|MF_ONEFACE,
		 MT_EVIL|MT_ZOID|MT_MINIBOSS|MT_ASYLUM,MG_DRL,MC_VIOLET,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,7,8,9,255},	// attack=breathe
				{10,11,12,13,14,15,16,255},	// die
				{1,2,3,4,5,6,7,8,9,255},	// attack=breathe
				{1,2,3,4,5,6,7,8,9,255},	// attack=breathe
				{1,2,3,4,5,6,7,8,9,255},	// attack=breathe
				{1,2,3,4,5,6,7,8,9,255},	// attack=breathe
			}},
		{"Dr. Lunatic",255,255,0,AI_DrLunatic,
		 40,21,50,10000,"graphics/dr-l.jsp",0,0,
		 MT_EVIL|MT_HUMAN|MT_BOSS|MT_ASYLUM,MG_DRL,MC_VIOLET,
			{
				{0,255},	// idle
				{7,8,9,8,7,0,10,11,12,11,10,0,255},	// move
				{1,2,3,4,5,6,5,4,5,6,5,6,5,4,3,4,3,4,5,4,3,2,1,255},	// attack=laugh
				{13,14,15,16,17,17,17,17,17,17,17,18,19,20,20,20,20,255},	// die
				// these following are only used when you play as Dr. L
				{1,2,3,2,1,255},	// use item
				{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255},	// bored
				{7,9,7,0,10,12,10,0,7,9,7,0,0,0,0,0,0,255},	// watery death
				{1,2,3,4,5,6,5,4,5,6,5,6,5,4,3,4,3,4,5,4,3,2,1,255},	// attack=laugh
			}},
		{"Super Duper Zombie",255,255,0,AI_SDZL,
		 40,48,1000,25000,"graphics/sdzl.jsp",0,MF_NOMOVE|MF_ONEFACE,
		 MT_EVIL|MT_BOSS|MT_ZOMBIE|MT_ASYLUM,MG_DRL,MC_GREY,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move (to the right)
				{28,29,30,31,31,31,31,31,32,33,34,35,36,37,38,39,255},	// attack=slash
				{40,41,42,43,43,43,43,43,44,45,46,47,47,47,47,47,47,255},	// die
				{5,6,5,0,7,8,7,0,255},	// A1=move left
				{9,10,11,12,12,12,12,12,12,13,14,15,16,16,17,18,255},	// A2=breathe
				{19,20,21,21,22,23,24,25,25,25,25,25,25,26,27,255},	// A3=ground pound
				{28,29,30,31,31,31,31,31,32,33,34,35,36,37,38,39,255},	// attack=slash
				{28,29,30,31,31,31,31,31,32,33,34,35,36,37,38,39,255},	// attack=slash
			}},
		{"Santa Claus",255,255,0,AI_Santa,
		 18,25,100,500,"graphics/santa.jsp",0,0,
		 MT_EVIL|MT_HUMAN|MT_WACKY|MT_ARCTIC,MG_DRL,MC_RED,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,15,16,17,18,19,255},	// attack=smack
				{20,21,22,23,24,24,24,254,24,254,24,254,24,254,24,255},	// die
				{7,8,9,10,11,12,13,14,15,16,17,18,19,255},	// attack=smack
				{7,8,9,10,11,12,13,14,15,16,17,18,19,255},	// attack=smack
				{7,8,9,10,11,12,13,14,15,16,17,18,19,255},	// attack=smack
				{7,8,9,10,11,12,13,14,15,16,17,18,19,255},	// attack=smack
			}},
		// EXPANSION MONSTERS
		{"Mine Cart",255,255,0,AI_MineCart,
		 11,8,100,500,"graphics/minecar.jsp",0,MF_NOMOVE|MF_INVINCIBLE|MF_FREEWALK|MF_NOHIT,
		 MT_VEHICLE|MT_GOOD,MG_EXPANDO,MC_BROWN,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,0,255},	// move
				{1,2,3,4,5,6,7,0,255},	// move
				{1,2,3,4,5,6,7,0,255},	// move
				{1,2,3,4,5,6,7,0,255},	// move
				{1,2,3,4,5,6,7,0,255},	// move
				{1,2,3,4,5,6,7,0,255},	// move
				{1,2,3,4,5,6,7,0,255},	// move
				{1,2,3,4,5,6,7,0,255},	// move
			}},
		{"Raft",255,255,0,AI_Raft,
		 8,8,100,500,"graphics/raft.jsp",0,MF_NOMOVE|MF_INVINCIBLE|MF_FREEWALK|MF_NOHIT|MF_ONEFACE|MF_AQUATIC|MF_WATERWALK,
		 MT_VEHICLE|MT_GOOD|MT_AQUATIC,MG_EXPANDO,MC_BROWN,
			{
				{1,2,3,4,5,6,0,255},	// idle
				{1,2,3,4,5,6,0,255},	// move
				{1,2,3,4,5,6,0,255},	// move
				{1,2,3,4,5,6,0,255},	// move
				{1,2,3,4,5,6,0,255},	// move
				{1,2,3,4,5,6,0,255},	// move
				{1,2,3,4,5,6,0,255},	// move
				{1,2,3,4,5,6,0,255},	// move
				{1,2,3,4,5,6,0,255},	// move
			}},
		{"Power Armor",255,255,0,NULL,	// in power armor
		 20,14,128,0,"graphics/pwrarmor.jsp",0,0,
		 MT_WEIRD,MG_EXPANDO,MC_RED,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,9,9,9,8,7,255},	// attack
				{10,11,12,13,255},		// die
				{7,7,7,255},		// fire missiles
				{13,12,11,10,255},	// activate
				{7,8,9,9,9,9,8,7,255},	// attack
				{7,8,9,9,9,9,8,7,255},	// attack
				{7,8,9,9,9,9,8,7,255},	// attack
			}},
		{"Vampire",255,255,0,AI_Vampire,
		 12,13,60,500,"graphics/vampire.jsp",0,0,
		 MT_EVIL|MT_VAMPIRE|MT_SPOOKY,MG_EXPANDO,MC_VIOLET,
			{
				{1,2,1,0,255},	// idle
				{1,2,1,0,255},	// move
				{3,4,5,6,7,8,9,255},	// attack
				{12,11,10,255},		// die
				{3,4,5,6,7,8,9,255},	// attack
				{3,4,5,6,7,8,9,255},	// attack
				{3,4,5,6,7,8,9,255},	// attack
				{3,4,5,6,7,8,9,255},	// attack
				{3,4,5,6,7,8,9,255},	// attack
			}},
		{"Coffin",255,255,0,AI_Coffin,
		 25,20,2000,500,"graphics/coffin.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_NOSHADOW,
		 MT_EVIL|MT_TRAP|MT_VAMPIRE|MT_SPOOKY|MT_GENERATE,MG_EXPANDO,MC_BROWN,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,255},	// attack
				{17,18,19,255},		// die
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,255},	// attack
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,255},	// attack
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,255},	// attack
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,255},	// attack
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,255},	// attack
			}},
		{"Spooky Ghost",255,255,0,AI_Ghost,
		 18,7,10,300,"graphics/ghost.jsp",0,MF_WALLWALK|MF_NOSHADOW|MF_FLYING|MF_GHOST|MF_ENEMYWALK|MF_FREEWALK,
		 MT_EVIL|MT_SPOOKY|MT_FLYING,MG_EXPANDO,MC_GLOW,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,3,3,2,1,255},	// attack (scream)
				{4,4,5,5,6,6,255},		// die
				{4,5,6,6,5,4,255},		// A1=teleport
				{254,255},				// A2=idle invisible
				{6,5,4,255},			// A3=materialize
				{1,2,3,3,3,2,1,255},	// attack (scream)
				{1,2,3,3,3,2,1,255},	// attack (scream)
			}},
		{"Burner",255,255,0,AI_Burner,
		 18,2,30,300,"graphics/burner.jsp",0,MF_NOSHADOW|MF_ONEFACE|MF_NOMOVE|MF_INVINCIBLE,
		 MT_EVIL|MT_TRAP|MT_SPOOKY,MG_EXPANDO,MC_GREY,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{0,255},		// die
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
			}},
		{"Lefty",255,255,0,AI_Lefty,
		 9,28,10,100,"graphics/lefty.jsp",0,0,
		 MT_EVIL|MT_SPOOKY|MT_WACKY,MG_EXPANDO,MC_BROWN,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{14,15,16,17,18,19,20,21,255},	// attack (punch)
				{22,23,24,25,26,27,26,25,24,26,254,25,254,27,254,26,255},		// die
				{7,8,9,10,11,12,13,12,11,10,11,12,13,12,11,10,9,8,7,255}, // A1=wave
				{14,15,16,17,18,19,20,21,255},	// attack (punch)
				{14,15,16,17,18,19,20,21,255},	// attack (punch)
				{14,15,16,17,18,19,20,21,255},	// attack (punch)
				{14,15,16,17,18,19,20,21,255},	// attack (punch)
			}},
		{"Pygmy Hunter",255,255,0,AI_Pygmy2,
		 10,41,15,100,"graphics/pygmy2.jsp",0,0,
		 MT_EVIL|MT_PYGMY|MT_HUMAN,MG_EXPANDO,MC_YELLOW,
			{
				{0,0,0,0,0,0,0,0,0,0,1,2,3,2,1,0,0,0,4,5,6,5,4,255},	// idle
				{8,9,10,9,8,7,11,12,13,12,11,7,255},	// move
				{20,21,22,23,24,25,26,27,28,255},	// attack
				{32,33,34,35,36,37,38,39,40,255},	// die
				{14,15,16,15,14,0,17,18,19,18,17,0,255},	// (A1) bored (unga dance)
				{29,30,31,30,29,0,29,30,31,30,29,255},	// (A2) angry, spotted Bouapha
				{20,21,22,23,24,25,26,27,28,255},	// attack
				{20,21,22,23,24,25,26,27,28,255},	// attack
				{20,21,22,23,24,25,26,27,28,255},	// attack
			}},
		{"Pygmy Shaman",255,255,0,AI_Pygmy3,
		 10,35,15,100,"graphics/pygmy3.jsp",0,0,
		 MT_EVIL|MT_PYGMY|MT_HUMAN,MG_EXPANDO,MC_RED,
			{
				{0,0,0,0,0,0,0,0,0,0,1,2,3,2,1,0,0,0,4,5,6,5,4,255},	// idle
				{8,9,10,9,8,7,11,12,13,12,11,7,255},	// move
				{20,21,22,22,22,21,20,255},	// attack
				{26,27,28,29,30,31,32,33,34,255},	// die
				{14,15,16,15,14,0,17,18,19,18,17,0,255},	// (A1) bored (unga dance)
				{23,24,25,24,23,0,23,24,25,24,23,255},	// (A2) angry, spotted Bouapha
				{20,21,22,22,22,21,20,255},	// attack
				{20,21,22,22,22,21,20,255},	// attack
				{20,21,22,22,22,21,20,255},	// attack
			}},
		{"Pumpkinstein",255,255,0,AI_Pumpkinstein,
		 18,13,80,1000,"graphics/pkstein.jsp",0,MF_NOMOVE,
		 MT_EVIL|MT_SPOOKY|MT_MINIBOSS|MT_PUMPKIN|MT_VEGGIE,MG_EXPANDO,MC_AQUA,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{0,255},	// attack
				{7,8,9,10,11,12,12,12,12,12,12,12,255},	// die
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
			}},
		{"Stan Glass",255,255,0,AI_Knight,
		 25,47,150,1000,"graphics/knight.jsp",0,MF_GLOW|MF_ONEFACE,
		 MT_EVIL|MT_MINIBOSS|MT_SPOOKY,MG_EXPANDO,MC_GLOW,
			{
				{14,255},	// idle
				{15,16,17,18,19,20,21,22,14,255},	// move
				{26,27,28,29,30,31,32,33,255},	// attack
				{34,35,36,37,38,39,40,41,42,43,44,45,46,255},	// die
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,255}, // A1 = arrive
				{23,24,25,25,25,25,25,25,25,24,23,255}, // A2 = block
				{26,27,28,29,30,31,32,33,255},	// attack
				{26,27,28,29,30,31,32,33,255},	// attack
				{26,27,28,29,30,31,32,33,255},	// attack
			}},
		{"Triceroid",255,255,0,AI_Triceroid,
		 30,18,60,250,"graphics/triceroid.jsp",0,MF_NOMOVE,
		 MT_EVIL|MT_ANIMAL|MT_PYGMY,MG_EXPANDO,MC_GREY,
			{
				{6,255},	// idle
				{5,4,3,2,1,0,1,2,3,4,5,6,255},	// move
				{7,8,9,9,9,8,7,255},	// attack
				{10,11,12,13,14,15,16,17,255},	// die
				{7,8,9,9,9,8,7,255},	// attack
				{7,8,9,9,9,8,7,255},	// attack
				{7,8,9,9,9,8,7,255},	// attack
				{7,8,9,9,9,8,7,255},	// attack
				{7,8,9,9,9,8,7,255},	// attack
			}},
		{"Countess",255,255,0,AI_Countess,
		 25,28,220,1000,"graphics/countess.jsp",0,MF_ONEFACE,
		 MT_EVIL|MT_BOSS|MT_SPOOKY|MT_VAMPIRE,MG_EXPANDO,MC_VIOLET,
			{
				{0,1,2,3,4,5,6,7,8,9,255},	// idle
				{0,1,2,3,4,5,6,7,8,9,255},	// move
				{13,14,15,16,17,18,18,18,18,18,19,255},	// attack
				{20,21,22,22,22,22,23,24,25,26,27,255},	// die
				{10,11,12,12,12,12,11,10,255},	// A1=summon boneheads
				{3,3,3,255},	// A2=charge up to rush
				{13,14,15,16,17,18,18,18,18,18,19,255},	// attack
				{13,14,15,16,17,18,18,18,18,18,19,255},	// attack
				{13,14,15,16,17,18,18,18,18,18,19,255},	// attack
			}},
		{"Egg",255,255,0,AI_AlienEgg,
		 12,14,100,200,"graphics/alienegg.jsp",0,MF_ONEFACE|MF_ENEMYWALK|MF_NOMOVE,
		 MT_EVIL|MT_GENERATE|MT_ALIEN|MT_SPACE,MG_EXPANDO,MC_GREY,
			{
				{0,1,2,3,2,1,255},	// idle
				{0,255},	// move
				{4,5,6,7,8,9,10,11,12,13,12,11,10,9,8,7,6,5,4,255},	// attack
				{4,5,6,7,8,9,10,11,12,13,255},	// die
				{4,5,6,7,8,9,10,11,12,13,12,11,10,9,8,7,6,5,4,255},	// attack
				{4,5,6,7,8,9,10,11,12,13,12,11,10,9,8,7,6,5,4,255},	// attack
				{4,5,6,7,8,9,10,11,12,13,12,11,10,9,8,7,6,5,4,255},	// attack
				{4,5,6,7,8,9,10,11,12,13,12,11,10,9,8,7,6,5,4,255},	// attack
				{4,5,6,7,8,9,10,11,12,13,12,11,10,9,8,7,6,5,4,255},	// attack
			}},
		{"Xenoid",255,255,0,AI_BabyAlien,
		 8,10,1,20,"graphics/babyalien.jsp",0,0,
		 MT_EVIL|MT_SPIDER|MT_ALIEN|MT_SPACE,MG_EXPANDO,MC_GREY,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,255},	// move
				{7,8,9,9,9,8,7,255},	// attack
				{0,255},	// die
				{7,8,9,9,9,8,7,255},	// attack
				{7,8,9,9,9,8,7,255},	// attack
				{7,8,9,9,9,8,7,255},	// attack
				{7,8,9,9,9,8,7,255},	// attack
				{7,8,9,9,9,8,7,255},	// attack
			}},
		{"Xeno Hunter",255,255,0,AI_Alien,
		 13,29,30,350,"graphics/alien.jsp",0,0,
		 MT_EVIL|MT_ALIEN|MT_SPACE,MG_EXPANDO,MC_GREY,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{15,16,17,18,19,20,21,22,255},	// attack
				{23,24,25,26,27,28,255},	// die
				{7,8,9,10,11,12,13,14,255},	// A1=spit
				{15,16,17,18,19,20,21,22,255},	// attack
				{15,16,17,18,19,20,21,22,255},	// attack
				{15,16,17,18,19,20,21,22,255},	// attack
				{15,16,17,18,19,20,21,22,255},	// attack
			}},
		{"Robopumpkin",255,255,0,AI_Robopumpkin,
		 26,7,200,1500,"graphics/robopk.jsp",0,MF_NOMOVE,
		 MT_EVIL|MT_HIGHTECH|MT_PUMPKIN|MT_VEGGIE,MG_EXPANDO,MC_GREY,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,2,1,255},	// attack
				{0,0,0,0,255},	// die
				{4,5,6,0,4,5,6,255},	// A1=chaingun
				{1,2,3,2,1,255},	// attack
				{1,2,3,2,1,255},	// attack
				{1,2,3,2,1,255},	// attack
				{1,2,3,2,1,255},	// attack
			}},
		{"Shock Trooper",255,255,0,AI_ShockTrooper,
		 40,23,200,1500,"graphics/shocktr.jsp",0,MF_NOMOVE,
		 MT_EVIL|MT_HIGHTECH,MG_EXPANDO,MC_VIOLET,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{10,11,11,11,12,13,14,14,15,255},	// attack
				{16,17,18,19,20,21,22,22,22,255},	// die
				{5,6,7,7,8,9,255},	// A1=lob grenade
				{10,11,11,11,12,13,14,14,15,255},	// attack
				{10,11,11,11,12,13,14,14,15,255},	// attack
				{10,11,11,11,12,13,14,14,15,255},	// attack
				{10,11,11,11,12,13,14,14,15,255},	// attack
			}},
		{"MiniBot",255,255,0,AI_Minibot,
		 10,6,5,50,"graphics/robot1.jsp",0,0,
		 MT_EVIL|MT_HIGHTECH|MT_SPACE,MG_EXPANDO,MC_GREY,
			{
				{0,255},	// idle
				{1,2,1,0,255},	// move
				{0,0,255},	// attack
				{3,4,5,5,5,255},	// die
				{0,0,255},	// attack
				{0,0,255},	// attack
				{0,0,255},	// attack
				{0,0,255},	// attack
				{0,0,255},	// attack
			}},
		{"MeanieBot",255,255,0,AI_Meaniebot,
		 13,10,30,100,"graphics/robot2.jsp",0,0,
		 MT_EVIL|MT_HIGHTECH|MT_SPACE,MG_EXPANDO,MC_GREY,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{5,6,6,6,6,6,5,255},	// attack
				{7,8,9,9,9,9,255},	// die
				{5,6,6,6,6,6,5,255},	// attack
				{5,6,6,6,6,6,5,255},	// attack
				{5,6,6,6,6,6,5,255},	// attack
				{5,6,6,6,6,6,5,255},	// attack
				{5,6,6,6,6,6,5,255},	// attack
			}},
		{"RoboFactory",255,255,0,AI_Robofactory,
		 20,28,150,500,"graphics/robofcty.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_ENEMYWALK,
		 MT_EVIL|MT_HIGHTECH|MT_GENERATE|MT_SPACE,MG_EXPANDO,MC_GREY,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,255},	// attack (summon)
				{0,0,0,255},	// die
				{17,18,19,20,21,22,23,24,25,26,27,0,255},	// A1=finish the summon
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,255},	// attack (summon)
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,255},	// attack (summon)
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,255},	// attack (summon)
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,255},	// attack (summon)
			}},
		{"Turret",255,255,0,AI_Turret,
		 10,1,80,200,"graphics/turret.jsp",0,MF_NOMOVE,
		 MT_EVIL|MT_TRAP|MT_HIGHTECH|MT_SPACE,MG_EXPANDO,MC_GREY,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{0,0,0,255},	// die
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
			}},
		{"Doom Bunny",255,255,0,AI_Bunny,
		 10,8,30,1000,"graphics/doombnny.jsp",0,0,
		 MT_EVIL|MT_WACKY|MT_ANIMAL,MG_EXPANDO,MC_BROWN,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,0,255},	// move
				{0,255},	// attack
				{0,0,0,0,255},	// die
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
			}},
		{"KONGOR!!",255,255,0,AI_Kongor,
		 80,32,400,2000,"graphics/kongor.jsp",0,MF_NOMOVE|MF_ONEFACE,
		 MT_EVIL|MT_ANIMAL|MT_BOSS|MT_PYGMY,MG_EXPANDO,MC_BROWN,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,255},	// move (left)
				{7,8,9,10,10,9,8,7,255},	// attack (left pound)
				{15,16,17,24,25,26,27,28,29,30,31,255},	// die
				{4,5,6,5,4,0,255}, // A1=move (right)
				{11,12,13,14,14,13,12,11,255}, // A2=right pound
				{15,16,17,18,19,20,19,18,21,22,23,22,21,18,19,20,19,18,255}, // A3=chest pound
				{21,22,23,22,21,17,16,15,255}, // A4=finish chest pound
				{7,8,9,10,10,9,8,7,255},	// attack (left pound)
			}},
		{"Squash",255,255,0,AI_Squash,
		 8,14,10,100,"graphics/squash.jsp",0,0,
		 MT_EVIL|MT_VEGGIE|MT_PUMPKIN|MT_PYGMY,MG_EXPANDO,MC_GREEN,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,9,8,7,255},	// attack
				{11,12,13,255},	// die
				{7,8,9,10,9,8,7,255},	// attack
				{7,8,9,10,9,8,7,255},	// attack
				{7,8,9,10,9,8,7,255},	// attack
				{7,8,9,10,9,8,7,255},	// attack
				{7,8,9,10,9,8,7,255},	// attack
			}},
		{"UltraPygmy",255,255,0,AI_UltraPygmy,
		 5,50,5,100,"graphics/pygmy4.jsp",0,0,
		 MT_EVIL|MT_PYGMY|MT_HUMAN|MT_WACKY,MG_EXPANDO,MC_VIOLET,
			{
				{0,0,0,0,0,0,0,0,0,0,1,2,3,2,1,0,0,0,4,5,6,5,4,255},	// idle
				{8,9,10,9,8,7,11,12,13,12,11,7,255},	// move
				{29,30,31,31,32,33,34,35,36,255},	// attack
				{37,38,39,40,41,42,43,44,45,46,255},	// die
				{14,15,16,17,18,19,20,16,17,18,19,15,14,255},	// (A1) bored (spin spear)
				{21,22,23,24,25,26,26,26,27,28,27,26,27,28,27,26,255}, // (A2) bored (yawn)
				{47,48,49,48,47,0,47,48,49,48,47,255},	// (A3) angry, spotted Bouapha
				{29,30,31,31,32,33,34,35,36,255},	// attack
				{29,30,31,31,32,33,34,35,36,255},	// attack
			}},
		{"LoonyBot 5000",255,255,0,AI_LoonyBot,
		 120,15,500,3000,"graphics/loonybot.jsp",0,MF_ONEFACE|MF_NOMOVE|MF_INVINCIBLE,
		 MT_EVIL|MT_BOSS|MT_HIGHTECH|MT_VEHICLE,MG_EXPANDO,MC_GREY,
			{
				{0,0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,8,7,255},	// attack
				{10,11,12,13,14,14,14,14,14,255},	// die
				{7,8,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,8,7,255},	// attack
				{7,8,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,8,7,255},	// attack
				{7,8,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,8,7,255},	// attack
				{7,8,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,8,7,255},	// attack
				{7,8,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,8,7,255},	// attack
			}},
		{"LoonyBot 5000",255,255,0,AI_LoonyCore,	// the core object, used to allow it to get hit when it's open
		 40,16,500,3000,"graphics/loonygun.jsp",0,MF_ONEFACE|MF_NOMOVE|MF_NOSHADOW|MF_FREEWALK|MF_ENEMYWALK,
		 MT_BITS,MG_EXPANDO,MC_GREY,
			{
				{254,254,255},	// idle
				{254,254,255},	// move
				{254,254,255},	// attack
				{254,254,255},	// die
				{254,254,255},	// attack
				{254,254,255},	// attack
				{254,254,255},	// attack
				{254,254,255},	// attack
				{254,254,255},	// attack
			}},
		{"LoonyGun",255,255,0,AI_LoonyGun,
		 40,16,500,3000,"graphics/loonygun.jsp",0,MF_ONEFACE|MF_NOMOVE|MF_FACECMD|MF_NOSHADOW|MF_ENEMYWALK|MF_FREEWALK,
		 MT_BITS,MG_EXPANDO,MC_GREY,
			{
				{0,0,255},	// idle
				{0,0,255},	// move
				{0,0,255},	// attack
				{0,0,255},	// die
				{0,0,255},	// attack
				{0,0,255},	// attack
				{0,0,255},	// attack
				{0,0,255},	// attack
				{0,0,255},	// attack
			}},
		{"Loony Zoomer",255,255,0,AI_LoonyShip,
		 30,1,100,3000,"graphics/loonyshp.jsp",0,MF_FLYING,
		 MT_EVIL|MT_MINIBOSS|MT_HIGHTECH|MT_FLYING|MT_VEHICLE,MG_EXPANDO,MC_GREY,
			{
				{0,0,255},	// idle
				{0,0,255},	// move
				{0,0,255},	// attack
				{0,0,0,0,255},	// die
				{0,0,255},	// attack
				{0,0,255},	// attack
				{0,0,255},	// attack
				{0,0,255},	// attack
				{0,0,255},	// attack
			}},

		// ------------------------------------------
		// FUN PACK MONSTERS!

		{"Buddy Bunny",2,0,3,AI_BuddyBunny,
		 10,8,30,1000,"!83",0,0,
		 MT_GOOD|MT_ANIMAL,MG_FUN,MC_GREY,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,0,255},	// move
				{0,255},	// attack
				{0,0,0,0,255},	// die
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
			}},
		{"Good Turret",4,1,5,AI_GoodTurret,
		 10,1,30,200,"!82",0,MF_NOMOVE,
		 MT_GOOD|MT_TRAP|MT_HIGHTECH,MG_FUN,MC_GREY,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{0,0,0,255},	// die
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
			}},
		{"Roly Poly",255,255,0,AI_Roller,
		 35,11,2000,1000,"!43",0,MF_NOMOVE|MF_ONEFACE,
		 MT_EVIL|MT_TRAP|MT_DESERT,MG_DRL,MC_BROWN,
			{
				{0,255},	// idle
				{1,2,3,4,5,0,255},	// move (roll down the screen)
				{5,4,3,2,1,0,255},	// attack=roll up the screen
				{0,0,255},	// die
				{6,7,8,9,10,0,255},	// A1=roll to the right
				{10,9,8,7,6,0,255},	// A2=roll to the left
				{5,4,3,2,1,0,255},	// attack=roll up the screen
				{5,4,3,2,1,0,255},	// attack=roll up the screen
				{5,4,3,2,1,0,255},	// attack=roll up the screen
			}},
		{"Crazy Egg",0,1,3,AI_AlienEgg,
		 12,14,100,200,"!74",0,MF_ONEFACE|MF_ENEMYWALK|MF_NOMOVE,
		 MT_EVIL|MT_ALIEN|MT_GENERATE|MT_WACKY,MG_FUN,MC_GREEN,
			{
				{0,1,2,3,2,1,255},	// idle
				{0,255},	// move
				{4,5,6,7,8,9,10,11,12,13,12,11,10,9,8,7,6,5,4,255},	// attack
				{4,5,6,7,8,9,10,11,12,13,255},	// die
				{4,5,6,7,8,9,10,11,12,13,12,11,10,9,8,7,6,5,4,255},	// attack
				{4,5,6,7,8,9,10,11,12,13,12,11,10,9,8,7,6,5,4,255},	// attack
				{4,5,6,7,8,9,10,11,12,13,12,11,10,9,8,7,6,5,4,255},	// attack
				{4,5,6,7,8,9,10,11,12,13,12,11,10,9,8,7,6,5,4,255},	// attack
				{4,5,6,7,8,9,10,11,12,13,12,11,10,9,8,7,6,5,4,255},	// attack
			}},
		{"Matilda-X",4,1,0,AI_Mattie2SkullOrHead, // the head of matilda
		 40,11,500,1000,"!11",0,MF_NOMOVE|MF_ONEFACE|MF_FACECMD|MF_NOGRAV|MF_ENEMYWALK|MF_FREEWALK,
		 MT_EVIL|MT_BOSS|MT_SPIDER|MT_FOREST,MG_FUN,MC_GREEN,
			{
				{0,0,255},	// idle
				{0,0,255},	// move
				{1,2,3,4,4,4,4,4,4,4,3,2,1,255},	// fire
				{1,2,0,3,4,3,2,0,3,4,3,0,1,4,2,4,3,1,2,255},	// die
				{1,2,3,4,4,4,4,4,4,4,3,2,1,255},	// fire
				{1,2,3,4,4,4,4,4,4,4,3,2,1,255},	// fire
				{1,2,3,4,4,4,4,4,4,4,3,2,1,255},	// fire
				{1,2,3,4,4,4,4,4,4,4,3,2,1,255},	// fire
				{1,2,3,4,4,4,4,4,4,4,3,2,1,255},	// fire
			}},
		{"Matilda-X Skull",4,1,0,AI_Mattie2SkullOrHead,	// the skull of matilda (head becomes this when killed)
		 40,11,400,1000,"!12",0,MF_NOMOVE|MF_ONEFACE|MF_FACECMD|MF_NOGRAV|MF_ENEMYWALK|MF_FREEWALK,
		 MT_BITS,MG_FUN,MC_GREEN,
			{
				{0,0,255},	// idle
				{0,0,255},	// move
				{1,2,3,4,4,4,4,4,4,4,3,2,1,255},	// fire
				{1,2,0,3,4,3,2,0,3,4,3,0,1,4,2,4,3,1,2,255},	// die
				{1,2,3,4,4,4,4,4,4,4,3,2,1,255},	// fire
				{1,2,3,4,4,4,4,4,4,4,3,2,1,255},	// fire
				{1,2,3,4,4,4,4,4,4,4,3,2,1,255},	// fire
				{1,2,3,4,4,4,4,4,4,4,3,2,1,255},	// fire
				{1,2,3,4,4,4,4,4,4,4,3,2,1,255},	// fire
			}},
		{"Matilda-X Brain",4,1,0,AI_Mattie2Brain,	// the brain of matilda (skull becomes this when killed)
		 40,7,100,5000,"!13",0,MF_NOMOVE|MF_ONEFACE|MF_FACECMD|MF_NOGRAV|MF_ENEMYWALK|MF_FREEWALK,
		 MT_BITS,MG_FUN,MC_GREY,
			{
				{0,0,255},	// idle
				{0,0,255},	// move
				{0,0,255},	// no form of attack
				{1,2,3,2,1,0,4,5,6,5,4,0,1,2,3,2,1,0,4,5,6,5,255},	// die
				{0,0,255},	// no form of attack
				{0,0,255},	// no form of attack
				{0,0,255},	// no form of attack
				{0,0,255},	// no form of attack
				{0,0,255},	// no form of attack
			}},
		{"Matilda-X Body",4,1,0,AI_Mattie2Body,	// the body of matilda
		 50,7,1,1,"!14",0,MF_NOMOVE|MF_ONEFACE|MF_INVINCIBLE|MF_NOGRAV|MF_ENEMYWALK,
		 MT_BITS,MG_FUN,MC_GREEN,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{0,255},	// fire
				{1,2,0,3,4,3,2,0,3,4,3,0,1,4,2,4,3,1,2,255},	// die
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
			}},
		{"Matilda-X Tail",4,1,0,AI_Mattie2Tail,	// her tail
		 80,16,200,1000,"!17",0,MF_NOMOVE|MF_ONEFACE|MF_INVINCIBLE|MF_FACECMD|MF_NOGRAV|MF_ENEMYWALK|MF_FREEWALK,
		 MT_BITS,MG_FUN,MC_GREEN,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,1,2,3,2,1,255},	// fire
				{1,2,0,3,2,0,3,3,0,1,2,3,1,2,255},	// die
				{0,1,2,3,2,1,255},	// fire
				{0,1,2,3,2,1,255},	// fire
				{0,1,2,3,2,1,255},	// fire
				{0,1,2,3,2,1,255},	// fire
				{0,1,2,3,2,1,255},	// fire
			}},

		{"Pygmy Queen",4,7,3,AI_PygmyQueen,
		 10,35,50,200,"!69",0,0,
		 MT_EVIL|MT_PYGMY|MT_GENERATE|MT_HUMAN,MG_FUN,MC_AQUA,
			{
				{0,0,0,0,0,0,0,0,0,0,1,2,3,2,1,0,0,0,4,5,6,5,4,255},	// idle
				{8,9,10,9,8,7,11,12,13,12,11,7,255},	// move
				{20,21,22,22,22,21,20,255},	// attack
				{26,27,28,29,30,31,32,33,34,255},	// die
				{14,15,16,15,14,0,17,18,19,18,17,0,255},	// (A1) bored (unga dance)
				{23,24,25,24,23,0,23,24,25,24,23,255},	// (A2) angry, spotted Bouapha
				{20,21,22,22,22,21,20,255},	// attack
				{20,21,22,22,22,21,20,255},	// attack
				{20,21,22,22,22,21,20,255},	// attack
			}},
		{"Jalapeno",1,4,4,AI_Jalapeno,
		 8,14,5,100,"!85",0,0,
		 MT_EVIL|MT_VEGGIE|MT_PUMPKIN|MT_PYGMY,MG_FUN,MC_RED,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,9,8,7,255},	// attack
				{11,12,13,255},	// die
				{7,8,9,10,9,8,7,255},	// attack
				{7,8,9,10,9,8,7,255},	// attack
				{7,8,9,10,9,8,7,255},	// attack
				{7,8,9,10,9,8,7,255},	// attack
				{7,8,9,10,9,8,7,255},	// attack
			}},
		{"Generator",255,255,0,AI_Generator,		// once every second
		 30,10,100,1000,"graphics/genrtr.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_FACECMD|MF_ENEMYWALK,
		 MT_EVIL|MT_GENERATE,MG_FUN,MC_GREY,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{0,0,0,0,0,255}, //die
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
			}},
		{"Generator",1,6,0,AI_Generator,		// once every 5 seconds
		 30,10,200,1000,"graphics/genrtr.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_FACECMD|MF_ENEMYWALK,
		 MT_EVIL|MT_GENERATE,MG_FUN,MC_GREY,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{0,0,0,0,0,255}, //die
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
			}},
		{"Generator",1,3,0,AI_Generator,		// once every 15 seconds
		 30,10,400,1000,"graphics/genrtr.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_FACECMD|MF_ENEMYWALK,
		 MT_EVIL|MT_GENERATE,MG_FUN,MC_GREY,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{0,0,0,0,0,255}, //die
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
			}},
		{"Generator",1,5,0,AI_Generator,		// once every 30 seconds
		 30,10,800,1000,"graphics/genrtr.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_FACECMD|MF_ENEMYWALK,
		 MT_EVIL|MT_GENERATE,MG_FUN,MC_GREY,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{0,0,0,0,0,255}, //die
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
			}},
		{"Sneaky Shark",255,255,0,AI_Shark,
		 15,13,50,100,"graphics/shark.jsp",0,MF_ONEFACE|MF_AQUATIC|MF_WATERWALK,
		 MT_EVIL|MT_WACKY|MT_AQUATIC|MT_TRAP|MT_ANIMAL,MG_FUN,MC_AQUA,
			{
				{0,1,2,3,4,5,6,255},	// idle
				{7,255},				// move
				{7,8,9,9,9,9,9,9,8,7,255},		// attack
				{10,11,12,255},			// die
				{7,8,9,9,8,7,255},		// A1=surprise!!!
				{7,8,9,9,9,9,9,9,8,7,255},		// attack
				{7,8,9,9,9,9,9,9,8,7,255},		// attack
				{7,8,9,9,9,9,9,9,8,7,255},		// attack
				{7,8,9,9,9,9,9,9,8,7,255},		// attack
			}},
		{"Mad Millennium Bug",3,4,8,AI_MadBug,
		 4,22,100,500,"!4",0,0,
		 MT_EVIL|MT_ANIMAL|MT_SPIDER|MT_MINIBOSS|MT_WACKY|MT_FOREST,MG_FUN,MC_RED,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,8,9,10,11,0,255},	// move
				{12,13,14,13,12,255},	// attack
				{15,16,17,18,19,18,19,18,19,20,20,21,21,255},	// die
				{12,13,14,13,12,255},	// A1=shoot
				{12,13,14,13,12,255},	// attack
				{12,13,14,13,12,255},	// attack
				{12,13,14,13,12,255},	// attack
				{12,13,14,13,12,255},	// attack
			}},
		{"Wacky Wizard",4,3,0,AI_Wizard,
		 18,25,200,500,"!59",0,0,
		 MT_WACKY|MT_GOOD|MT_HUMAN,MG_FUN,MC_BLUE,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,15,16,17,18,19,255},	// attack=smack
				{20,21,22,23,24,24,24,254,24,254,24,254,24,254,24,255},	// die
				{7,9,11,12,12,12,12,12,12,11,9,7,255},	// A1=cast spell
				{7,8,9,10,11,12,13,14,15,16,17,18,19,255},	// attack=smack
				{7,8,9,10,11,12,13,14,15,16,17,18,19,255},	// attack=smack
				{7,8,9,10,11,12,13,14,15,16,17,18,19,255},	// attack=smack
				{7,8,9,10,11,12,13,14,15,16,17,18,19,255},	// attack=smack
			}},
		{"Evil Clone",1,3,0,AI_EvilClone,
		 11,29,128,0,"!1",0,0,
		 MT_EVIL|MT_HUMAN,MG_FUN,MC_BLUE,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,15,16,255},	// attack
				{0,0,0,0,0,0,0,26,27,28,255},		// die
				{23,24,25,24,23,255},	// use item
				{17,18,19,20,21,22,21,20,21,22,21,20,19,18,17,255},	// bored
				{1,3,1,0,4,6,4,0,1,3,1,0,0,0,26,27,28,255},	// watery death
				{7,8,9,10,11,12,13,14,15,16,255},	// attack
				{7,8,9,10,11,12,13,14,15,16,255},	// attack
			}},
		{"Bob The Biscuit",255,255,-7,AI_Bob,
		 12,33,70,300,"!18",0,0,
		 MT_EVIL|MT_WACKY|MT_FOREST,MG_FUN,MC_BROWN,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,255},	// move
				{7,8,9,10,11,12,13,14,15,16,17,18,19,20,19,20,19,20,21,22,23,255},	// triple punch
				{27,28,29,30,31,32,32,32,32,255},	// die
				{24,25,26,26,26,26,26,255},	// jumpkick (A1)
				{25,24,255},	// unjumpkick (A2)
				{23,22,21,20,19,20,19,20,21,22,23,255}, // summon help (A3)
				{7,8,9,10,11,12,13,14,15,16,17,18,19,20,19,20,19,20,21,22,23,255},	// triple punch
				{7,8,9,10,11,12,13,14,15,16,17,18,19,20,19,20,19,20,21,22,23,255},	// triple punch
			}},
		{"MultiMoss",1,6,0,AI_MultiMoss,
		 18,17,30,250,"!22",0,MF_ONEFACE|MF_ENEMYWALK|MF_FREEWALK|MF_NOSHADOW,
		 MT_EVIL|MT_TRAP|MT_VEGGIE|MT_CAVES,MG_FUN,MC_VIOLET,
			{
				{0,1,2,3,4,5,6,255},	// idle
				{0,1,2,3,4,5,6,255},	// move
				{7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,255},	// attack
				{22,23,24,25,26,27,28,255},	// die
				{28,27,26,25,24,23,22,255},	// A1=reborn!
				{7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,255},	// attack
				{7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,255},	// attack
				{7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,255},	// attack
				{7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,255},	// attack
			}},
		{"Moss Rapido",1,5,0,AI_MossRapido,
		 15,17,5,25,"!21",0,MF_NOMOVE|MF_ONEFACE|MF_FREEWALK|MF_ENEMYWALK|MF_WALLWALK|MF_NOSHADOW,
		 MT_EVIL|MT_VEGGIE|MT_CAVES,MG_FUN,MC_YELLOW,
			{
				{0,255},	// idle
				{1,2,3,2,1,255},	// move=grow vertically
				{4,5,6,5,4,255},	// attack=grow horizontally
				{7,8,9,10,11,12,13,14,15,16,255},	// die
				{4,5,6,5,4,255},	// attack=grow horizontally
				{4,5,6,5,4,255},	// attack=grow horizontally
				{4,5,6,5,4,255},	// attack=grow horizontally
				{4,5,6,5,4,255},	// attack=grow horizontally
				{4,5,6,5,4,255},	// attack=grow horizontally
			}},
		{"Snowball",2,0,10,AI_Snowball,
		 35,11,2000,1000,"!43",0,MF_NOMOVE|MF_ONEFACE|MF_WATERWALK,
		 MT_EVIL|MT_TRAP|MT_ARCTIC,MG_FUN,MC_GREY,
			{
				{0,255},	// idle
				{1,2,3,4,5,0,255},	// move (roll down the screen)
				{5,4,3,2,1,0,255},	// attack=roll up the screen
				{0,0,255},	// die
				{6,7,8,9,10,0,255},	// A1=roll to the right
				{10,9,8,7,6,0,255},	// A2=roll to the left
				{5,4,3,2,1,0,255},	// attack=roll up the screen
				{5,4,3,2,1,0,255},	// attack=roll up the screen
				{5,4,3,2,1,0,255},	// attack=roll up the screen
			}},
		{"Snowball",2,0,10,AI_Snowball,
		 35,11,2000,1000,"!43",0,MF_NOMOVE|MF_ONEFACE|MF_WATERWALK,
		 MT_EVIL|MT_TRAP|MT_ARCTIC,MG_FUN,MC_GREY,
			{
				{0,255},	// idle
				{1,2,3,4,5,0,255},	// move (roll down the screen)
				{5,4,3,2,1,0,255},	// attack=roll up the screen
				{0,0,255},	// die
				{6,7,8,9,10,0,255},	// A1=roll to the right
				{10,9,8,7,6,0,255},	// A2=roll to the left
				{5,4,3,2,1,0,255},	// attack=roll up the screen
				{5,4,3,2,1,0,255},	// attack=roll up the screen
				{5,4,3,2,1,0,255},	// attack=roll up the screen
			}},
		{"Snow Blower",255,255,10,AI_Snowblower,
		 18,2,30,300,"!66",0,MF_NOSHADOW|MF_ONEFACE|MF_NOMOVE|MF_INVINCIBLE,
		 MT_EVIL|MT_TRAP|MT_ARCTIC,MG_FUN,MC_GREY,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{0,255},	// die
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
			}},
		{"Boomkin",2,4,5,AI_Pumpkin,
		 11,6,1,500,"!19",0,0,
		 MT_EVIL|MT_VEGGIE|MT_PUMPKIN|MT_CAVES,MG_FUN,MC_RED,
			{
				{0,255},	// idle
				{1,2,3,4,5,0,255},	// move
				{0,255},	// attack unused
				{0,0,255},	// die (just long enough to spew shrapnel)
				{1,2,3,4,5,0,255},	// move
				{1,2,3,4,5,0,255},	// move
				{1,2,3,4,5,0,255},	// move
				{1,2,3,4,5,0,255},	// move
				{1,2,3,4,5,0,255},	// move
			}},
		{"Manic Mumble",0,7,4,AI_Mumble,
		 15,28,20,100,"!39",0,MF_NOMOVE,
		 MT_EVIL|MT_SPOOKY|MT_DESERT|MT_ASYLUM,MG_FUN,MC_AQUA,
			{
				{0,255},	// idle
				{1,2,3,4,5,0,6,7,8,9,10,0,255},	// move
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{19,20,21,22,23,24,25,26,27,255},	// die
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
			}},
		{"BuddyBot",0,7,-2,AI_Minibot,
		 10,6,5,50,"!79",0,0,
		 MT_GOOD|MT_HIGHTECH,MG_FUN,MC_AQUA,
			{
				{0,255},	// idle
				{1,2,1,0,255},	// move
				{0,0,255},	// attack
				{3,4,5,5,5,255},	// die
				{0,0,255},	// attack
				{0,0,255},	// attack
				{0,0,255},	// attack
				{0,0,255},	// attack
				{0,0,255},	// attack
			}},
		{"HelpyBot",0,7,-2,AI_Meaniebot,
		 13,10,30,100,"!80",0,0,
		 MT_GOOD|MT_HIGHTECH,MG_FUN,MC_AQUA,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{5,6,6,6,6,6,5,255},	// attack
				{7,8,9,9,9,9,255},	// die
				{5,6,6,6,6,6,5,255},	// attack
				{5,6,6,6,6,6,5,255},	// attack
				{5,6,6,6,6,6,5,255},	// attack
				{5,6,6,6,6,6,5,255},	// attack
				{5,6,6,6,6,6,5,255},	// attack
			}},
		{"Xeno Queen",0,6,0,AI_XenoMama,
		 13,29,200,350,"!76",0,MF_ENEMYWALK,
		 MT_EVIL|MT_GENERATE|MT_MINIBOSS|MT_ALIEN|MT_SPACE,MG_FUN,MC_VIOLET,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{15,16,17,18,19,20,21,22,255},	// attack
				{23,24,25,26,27,28,255},	// die
				{7,8,9,10,11,12,13,14,255},	// A1=spit
				{15,16,17,18,19,20,21,22,255},	// attack
				{15,16,17,18,19,20,21,22,255},	// attack
				{15,16,17,18,19,20,21,22,255},	// attack
				{15,16,17,18,19,20,21,22,255},	// attack
			}},
		{"Rumbly Tumbly",255,255,4,AI_Roller,
		 35,11,50,100,"!43",0,MF_NOMOVE|MF_ONEFACE|MF_WATERWALK,
		 MT_EVIL|MT_TRAP|MT_DESERT,MG_FUN,MC_BROWN,
			{
				{0,255},	// idle
				{1,2,3,4,5,0,255},	// move (roll down the screen)
				{5,4,3,2,1,0,255},	// attack=roll up the screen
				{0,0,255},	// die
				{6,7,8,9,10,0,255},	// A1=roll to the right
				{10,9,8,7,6,0,255},	// A2=roll to the left
				{5,4,3,2,1,0,255},	// attack=roll up the screen
				{5,4,3,2,1,0,255},	// attack=roll up the screen
				{5,4,3,2,1,0,255},	// attack=roll up the screen
			}},
		{"Rumbly Tumbly",255,255,4,AI_Roller,
		 35,11,50,100,"!43",0,MF_NOMOVE|MF_ONEFACE|MF_WATERWALK,
		 MT_EVIL|MT_TRAP|MT_DESERT,MG_FUN,MC_BROWN,
			{
				{0,255},	// idle
				{1,2,3,4,5,0,255},	// move (roll down the screen)
				{5,4,3,2,1,0,255},	// attack=roll up the screen
				{0,0,255},	// die
				{6,7,8,9,10,0,255},	// A1=roll to the right
				{10,9,8,7,6,0,255},	// A2=roll to the left
				{5,4,3,2,1,0,255},	// attack=roll up the screen
				{5,4,3,2,1,0,255},	// attack=roll up the screen
				{5,4,3,2,1,0,255},	// attack=roll up the screen
			}},
		{"Dark Vampire",6,0,-6,AI_Vampire,
		 12,13,150,500,"!63",0,MF_INVINCIBLE,
		 MT_EVIL|MT_VAMPIRE|MT_SPOOKY,MG_FUN,MC_GREY,
			{
				{1,2,1,0,255},	// idle
				{1,2,1,0,255},	// move
				{3,4,5,6,7,8,9,255},	// attack
				{12,11,10,255},		// die
				{3,4,5,6,7,8,9,255},	// attack
				{3,4,5,6,7,8,9,255},	// attack
				{3,4,5,6,7,8,9,255},	// attack
				{3,4,5,6,7,8,9,255},	// attack
				{3,4,5,6,7,8,9,255},	// attack
			}},
		{"Grabby Gnome",2,1,0,AI_Gnome,
		 5,50,15,100,"!86",0,0,
		 MT_EVIL|MT_WACKY|MT_PYGMY|MT_HUMAN,MG_FUN,MC_GREEN,
			{
				{0,0,0,0,0,0,0,0,0,0,1,2,3,2,1,0,0,0,4,5,6,5,4,255},	// idle
				{8,9,10,9,8,7,11,12,13,12,11,7,255},	// move
				{29,30,31,31,32,33,34,35,36,255},	// attack
				{37,38,39,40,41,42,43,44,45,46,255},	// die
				{14,15,16,17,18,19,20,16,17,18,19,15,14,255},	// (A1) bored (spin spear)
				{21,22,23,24,25,26,26,26,27,28,27,26,27,28,27,26,255}, // (A2) bored (yawn)
				{47,48,49,48,47,0,47,48,49,48,47,255},	// (A3) angry, spotted Bouapha
				{29,30,31,31,32,33,34,35,36,255},	// attack
				{29,30,31,31,32,33,34,35,36,255},	// attack
			}},
		{"Nobody",255,255,0,AI_Nobody,
		 1,1,1,1,"!1",0,MF_INVINCIBLE|MF_NOHIT|MF_FREEWALK|MF_ENEMYWALK|MF_WALLWALK,
		 MT_NONE,MG_FUN,MC_GREY,
			{
				{254,255},	// idle
				{254,255},	// move
				{254,255},	// attack
				{254,255},	// die
				{254,255},	// (A1) bored (spin spear)
				{254,255}, // (A2) bored (yawn)
				{254,255},	// (A3) angry, spotted Bouapha
				{254,255},	// attack
				{254,255},	// attack
			}},
		{"Buddy Bunny",2,0,3,AI_BuddyBunny,
		 10,8,30,1000,"!83",0,MF_NOMOVE,
		 MT_GOOD|MT_ANIMAL,MG_FUN,MC_GREY,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,0,255},	// move
				{0,255},	// attack
				{0,0,0,0,255},	// die
				{1,2,3,4,5,6,7,0,255},	// move
				{1,2,3,4,5,6,7,0,255},	// move
				{1,2,3,4,5,6,7,0,255},	// move
				{1,2,3,4,5,6,7,0,255},	// move
				{1,2,3,4,5,6,7,0,255},	// move
			}},
		{"Rock Trooper",6,2,0,AI_RockTrooper,
		 40,23,100,1000,"!78",0,MF_NOMOVE,
		 MT_EVIL|MT_HIGHTECH,MG_FUN,MC_BROWN,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{10,11,11,11,12,13,14,14,15,255},	// attack
				{16,17,18,19,20,21,22,22,22,255},	// die
				{5,6,7,7,8,9,255},	// A1=lob grenade
				{10,11,11,11,12,13,14,14,15,255},	// attack
				{10,11,11,11,12,13,14,14,15,255},	// attack
				{10,11,11,11,12,13,14,14,15,255},	// attack
				{10,11,11,11,12,13,14,14,15,255},	// attack
			}},
		{"Jacko",255,255,-2,AI_BigPumpkin,	// big pumpkin
		 14,1,30,500,"!52",0,0,
		 MT_EVIL|MT_PUMPKIN|MT_VEGGIE|MT_FOREST,MG_FUN,MC_BROWN,
			{
				{0,255},	// idle
				{0,0,0,255},	// move
				{0,255},	// attack
				{0,0,0,255},	// die
				{0,0,0,255},	// move
				{0,0,0,255},	// move
				{0,0,0,255},	// move
				{0,0,0,255},	// move
				{0,0,0,255},	// move
			}},
		{"Crazybone",0,3,0,AI_CrazyBone,
		 8,38,10,100,"!2",0,0,
		 MT_EVIL|MT_SPOOKY|MT_SKELETON,MG_FUN,MC_BLUE,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{20,21,22,23,24,255},	// attack
				{7,8,9,10,11,12,13,14,15,16,255},		// die
				{25,26,27,28,29,30,255},	// chomp/spit projectile
				{31,32,32,32,31,255},	// point at bouapha
				{16,15,14,13,12,11,10,9,8,7,255},	// resurrect=A3
				{20,21,22,23,24,255},	// attack
				{20,21,22,23,24,255},	// attack
			}},
		{"Creepazoid",255,255,10,AI_Creepazoid,
		 15,20,15,75,"!10",0,MF_WATERWALK|MF_WALLWALK|MF_FREEWALK|MF_ENEMYWALK|MF_NOSHADOW|MF_GLOW|MF_FLYING,
		 MT_EVIL|MT_ZOID|MT_SPOOKY|MT_FLYING,MG_FUN,MC_GLOW,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,255},	// move
				{4,5,6,7,8,9,10,11,12,255},	// spit
				{13,14,15,16,17,18,19,255},	// die
				{4,5,6,7,8,9,10,11,12,255},	// spit
				{4,5,6,7,8,9,10,11,12,255},	// spit
				{4,5,6,7,8,9,10,11,12,255},	// spit
				{4,5,6,7,8,9,10,11,12,255},	// spit
				{4,5,6,7,8,9,10,11,12,255},	// spit
			}},
		{"LazyBones",0,2,-4,AI_Lazybones,
		 8,38,30,50,"!2",0,MF_NOMOVE,
		 MT_SPOOKY|MT_EVIL|MT_SKELETON,MG_SUPREME,MC_BROWN,
			{
				{13,13,13,13,13,13,13,13,13,13,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{25,26,27,28,28,28,28,28,28,28,28,28,28,28,28,28,29,30,255},	// attack
				{7,8,9,10,11,12,13,14,15,16,255},		// die
				{13,12,11,10,9,8,7,255},	// wake up=A1
				{7,8,9,10,11,12,13,255},		// go back to sleep=A2
				{25,26,27,28,28,28,28,28,28,28,28,28,28,28,28,28,29,30,255},	// attack
				{25,26,27,28,28,28,28,28,28,28,28,28,28,28,28,28,29,30,255},	// attack
				{25,26,27,28,28,28,28,28,28,28,28,28,28,28,28,28,29,30,255},	// attack
			}},
		{"Starey Bat",2,1,-1,AI_StareyBat,
		 8,27,15,25,"!3",0,MF_FLYING|MF_NOMOVE,
		 MT_SPOOKY|MT_FLYING|MT_EVIL|MT_ANIMAL|MT_TRAP,MG_SUPREME,MC_GREEN,
			{
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// idle
				{0,255},	// move
				{7,8,9,9,9,9,9,8,7,255},	// attack
				{17,18,19,20,21,22,23,24,25,26,255},		// die
				{7,8,9,9,9,9,9,8,7,255},	// attack
				{7,8,9,9,9,9,9,8,7,255},	// attack
				{7,8,9,9,9,9,9,8,7,255},	// attack
				{7,8,9,9,9,9,9,8,7,255},	// attack
				{7,8,9,9,9,9,9,8,7,255},	// attack
			}},
		{"Madman",5,2,1,AI_Madman,
		 12,21,10,100,"!53",0,0,
		 MT_EVIL|MT_HUMAN|MT_ASYLUM,MG_SUPREME,MC_BROWN,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,255},	// attack=stab
				{15,16,17,18,19,20,254,20,254,20,254,20,254,20,255},	// die
				{7,8,9,10,11,12,13,14,255},	// attack=stab
				{7,8,9,10,11,12,13,14,255},	// attack=stab
				{7,8,9,10,11,12,13,14,255},	// attack=stab
				{7,8,9,10,11,12,13,14,255},	// attack=stab
				{7,8,9,10,11,12,13,14,255},	// attack=stab
			}},
		{"Pygmy Accountant",255,255,0,AI_PygmyAcct,
		 10,41,25,100,"graphics/pygacct.jsp",0,0,
		 MT_EVIL|MT_PYGMY|MT_HUMAN|MT_URBAN,MG_SUPREME,MC_BLUE,
			{
				{0,0,0,0,0,0,0,0,0,0,1,2,3,2,1,0,0,0,4,5,6,5,4,255},	// idle
				{7,8,9,8,7,0,10,11,12,11,10,0,255},	// move
				{24,25,26,27,28,29,30,31,255},	// attack
				{32,33,34,35,36,37,38,39,40,255},	// die
				{13,14,15,16,17,18,18,18,18,18,18,17,16,15,14,13,255},	// (A1) bored (read paper)
				{19,20,21,22,23,22,21,22,23,22,21,20,19,255},	// (A2) angry, spotted Bouapha
				{24,25,26,27,28,29,30,31,255},	// attack
				{24,25,26,27,28,29,30,31,255},	// attack
				{24,25,26,27,28,29,30,31,255},	// attack
			}},
		{"Mr. Crazypants",255,255,0,AI_Crazypants,
		 12,35,80,100,"graphics/crazypants.jsp",0,0,
		 MT_EVIL|MT_HUMAN|MT_MINIBOSS|MT_ASYLUM,MG_SUPREME,MC_YELLOW,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,15,16,17,18,19,8,7,255},	// attack=dash
				{29,30,31,31,32,32,33,33,34,255},	// die
				{20,21,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,21,20,255},	// A1=spin attack
				{23,24,25,26,27,28,27,26,25,26,27,28,27,26,25,24,23,255},	// A2=angry
				{7,8,9,10,11,12,13,14,15,16,17,18,19,8,7,255},	// attack=dash
				{7,8,9,10,11,12,13,14,15,16,17,18,19,8,7,255},	// attack=dash
				{7,8,9,10,11,12,13,14,15,16,17,18,19,8,7,255},	// attack=dash
			}},
		{"Yerfdog",0,0,0,AI_Yerfdog,
		 10,3,30,1000,"graphics/yerfdog.jsp",0,MF_NOHIT|MF_FREEWALK|MF_ENEMYWALK|MF_WATERWALK|MF_ONEFACE|MF_FACECMD,
		 MT_GOOD|MT_ANIMAL,MG_SUPREME,MC_GREY,
			{
				{0,255},	// idle
				{1,2,0,255},	// move
				{0,255},	// attack
				{0,0,0,0,255},	// die
				{1,2,0,255},	// move
				{1,2,0,255},	// move
				{1,2,0,255},	// move
				{1,2,0,255},	// move
				{1,2,0,255},	// move
			}},
		{"Elite SEAL",1,4,0,AI_EliteSeal,
		 22,20,50,125,"!31",0,0,
		 MT_EVIL|MT_ANIMAL|MT_ARCTIC,MG_SUPREME,MC_GREY,
			{
				{0,255},	// idle
				{1,2,3,4,5,0,255},	// move
				{6,6,9,10,13,14,255},	// attack=full auto
				{15,16,17,18,19,19,19,255},	// die
				{6,6,9,10,13,14,255},	// attack=full auto
				{6,6,9,10,13,14,255},	// attack=full auto
				{6,6,9,10,13,14,255},	// attack=full auto
				{6,6,9,10,13,14,255},	// attack=full auto
				{6,6,9,10,13,14,255},	// attack=full auto
			}},
		{"Creeping Cone",255,255,0,AI_Cone,
		 8,21,30,25,"graphics/cone.jsp",0,0,
		 MT_EVIL|MT_URBAN|MT_WACKY|MT_TRAP,MG_SUPREME,MC_RED,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,0,255},	// move
				{8,9,10,9,8,255},	// attack=poke
				{17,18,19,20,255},	// die
				{11,12,13,14,14,15,16,15,14,13,12,11,255}, // A1=shoot
				{8,9,10,9,8,255},	// attack=poke
				{8,9,10,9,8,255},	// attack=poke
				{8,9,10,9,8,255},	// attack=poke
				{8,9,10,9,8,255},	// attack=poke
			}},
		{"Pygmy Diver",255,255,0,AI_PygmyDiver,
		 10,54,15,25,"graphics/pygmydiver.jsp",0,MF_WATERWALK,
		 MT_EVIL|MT_UNDERSEA|MT_PYGMY|MT_HUMAN,MG_SUPREME,MC_AQUA,
			{
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,255},	// idle
				{17,18,19,20,21,22,23,24,25,26,27,28,17,255},	// move
				{29,30,31,32,33,34,35,36,37,255},	// attack=throw spear
				{38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,255},	// die
				{15,16,16,255}, // A1=wake up
				{16,15,255},	// A2=go back to waiting
				{29,30,31,32,33,34,35,36,37,255},	// attack=throw spear
				{29,30,31,32,33,34,35,36,37,255},	// attack=throw spear
				{29,30,31,32,33,34,35,36,37,255},	// attack=throw spear
			}},
		{"Puffyfish",255,255,0,AI_Puffyfish,
		 8,23,300,25,"graphics/puffyfish.jsp",0,MF_FLYING|MF_WATERWALK,
		 MT_UNDERSEA|MT_ANIMAL|MT_EVIL,MG_SUPREME,MC_YELLOW,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,1,255},	// move
				{0,255},	// attack=unused
				{20,21,22,22,22,255},	// die=POP!
				{6,7,6,5,8,9,8,5,6,255}, // A1=move, fatter
				{11,12,11,10,13,14,13,10,11,255},	// A2=move, fatter yet
				{16,17,16,15,18,19,18,15,16,255},	// A3=move, fattest of all
				{20,21,22,22,22,255},	// die=POP!
				{20,21,22,22,22,255},	// die=POP!
			}},
		{"Puffyfish",255,255,0,AI_Puffyfish,
		 8,23,300,25,"!140",0,MF_FLYING|MF_WATERWALK,
		 MT_UNDERSEA|MT_ANIMAL|MT_EVIL,MG_SUPREME,MC_YELLOW,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,1,255},	// move
				{0,255},	// attack=unused
				{20,21,22,22,22,255},	// die=POP!
				{6,7,6,5,8,9,8,5,6,255}, // A1=move, fatter
				{11,12,11,10,13,14,13,10,11,255},	// A2=move, fatter yet
				{16,17,16,15,18,19,18,15,16,255},	// A3=move, fattest of all
				{20,21,22,22,22,255},	// die=POP!
				{20,21,22,22,22,255},	// die=POP!
			}},
		{"Olaf",1,6,-2,AI_Yeti2,
		 48,40,250,5000,"!36",0,MF_NOMOVE|MF_ENEMYWALK,
		 MT_EVIL|MT_BOSS|MT_ARCTIC|MT_ANIMAL,MG_SUPREME,MC_VIOLET,
			{
				{0,255},	// idle
				{0,1,2,1,0,3,4,3,255},	// move
				{14,15,16,17,18,19,20,21,22,23,255},	// attack=snowball
				{35,36,37,38,39,39,39,39,39,39,39,39,39,39,39,39,39,39,255},	// die
				{28,29,30,31,32,33,33,33,34,34,255},	// A1=ground pound
				{5,6,7,8,9,10,11,12,13,255},	//A2=death yodel
				{24,25,26,27,26,25,26,27,26,25,26,27,26,25,24,255},	// A3=wave hello
				{14,15,16,17,18,19,20,21,22,23,255},	// attack=snowball
				{14,15,16,17,18,19,20,21,22,23,255},	// attack=snowball
			}},
		{"Spikey",6,4,5,AI_Spikey,
		 12,13,200,500,"!63",0,0,
		 MT_EVIL|MT_VAMPIRE|MT_SPOOKY,MG_SUPREME,MC_RED,
			{
				{1,2,1,0,255},	// idle
				{1,2,1,0,255},	// move
				{3,4,5,5,6,6,7,7,8,9,255},	// attack
				{12,11,10,255},		// die
				{3,4,5,5,6,6,7,7,8,9,255},	// attack
				{3,4,5,5,6,6,7,7,8,9,255},	// attack
				{3,4,5,5,6,6,7,7,8,9,255},	// attack
				{3,4,5,5,6,6,7,7,8,9,255},	// attack
				{3,4,5,5,6,6,7,7,8,9,255},	// attack
			}},
		{"BomBot",0,4,-4,AI_Bombot,
		 13,10,50,100,"!80",0,0,
		 MT_EVIL|MT_HIGHTECH|MT_TRAP,MG_SUPREME,MC_RED,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{5,6,6,6,6,6,5,255},	// attack
				{7,8,9,9,9,9,255},	// die
				{5,6,6,6,6,6,5,255},	// attack
				{5,6,6,6,6,6,5,255},	// attack
				{5,6,6,6,6,6,5,255},	// attack
				{5,6,6,6,6,6,5,255},	// attack
				{5,6,6,6,6,6,5,255},	// attack
			}},
		{"Buddy Bunny",2,0,3,AI_FollowBunny,
		 10,8,30,1000,"!83",0,0,
		 MT_GOOD|MT_ANIMAL,MG_SUPREME,MC_GREY,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,0,255},	// move
				{0,255},	// attack
				{0,0,0,0,255},	// die
				{1,2,3,4,5,6,7,0,255},	// move
				{1,2,3,4,5,6,7,0,255},	// move
				{1,2,3,4,5,6,7,0,255},	// move
				{1,2,3,4,5,6,7,0,255},	// move
				{1,2,3,4,5,6,7,0,255},	// move
			}},
		{"Autozoid",255,255,0,AI_Autozoid,
		 15,16,150,1000,"graphics/autozoid.jsp",0,0,
		 MT_EVIL|MT_ZOID|MT_URBAN|MT_VEHICLE,MG_SUPREME,MC_YELLOW,
			{
				{0,255},	// idle
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,255},	// attack
				{0,0,0,255},	// die
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
			}},
		{"Treasure Chest",255,255,0,AI_Chest,
		 16,5,50,1000,"graphics/chest.jsp",0,MF_NOMOVE|MF_ONEFACE,
		 MT_TRAP|MT_UNDERSEA,MG_SUPREME,MC_BROWN,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{1,2,3,4,255},	// die
				{1,2,3,4,255},	// die
				{1,2,3,4,255},	// die
				{1,2,3,4,255},	// die
				{1,2,3,4,255},	// die
				{1,2,3,4,255},	// die
			}},
		{"Jellofish",255,255,0,AI_Jellofish,
		 8,22,10,1000,"graphics/jello.jsp",0,MF_FLYING,
		 MT_EVIL|MT_UNDERSEA|MT_ANIMAL,MG_SUPREME,MC_GREEN,
			{
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// idle
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,255},	// attack
				{16,17,18,19,20,21,255},	// die
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
			}},
		{"King Cone",255,255,0,AI_KingCone,
		 64,27,500,1000,"graphics/kingcone.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_FACECMD|MF_ENEMYWALK,
		 MT_EVIL|MT_WACKY|MT_URBAN|MT_MINIBOSS,MG_SUPREME,MC_RED,
			{
				{0,255},	// idle
				{5,6,7,7,8,8,9,0,255},	// move (hop to summon cones)
				{10,11,12,13,14,15,16,17,18,255},	// attack (crown spin)
				{19,19,20,20,21,21,22,22,23,23,24,24,25,25,26,255},	// die
				{10,11,12,13,14,15,16,17,18,255},	// attack (crown spin)
				{10,11,12,13,14,15,16,17,18,255},	// attack (crown spin)
				{10,11,12,13,14,15,16,17,18,255},	// attack (crown spin)
				{10,11,12,13,14,15,16,17,18,255},	// attack (crown spin)
				{10,11,12,13,14,15,16,17,18,255},	// attack (crown spin)
			}},
		{"Mailboxer",255,255,0,AI_Mailboxer,
		 12,32,40,1000,"graphics/mboxer.jsp",0,0,
		 MT_EVIL|MT_WACKY|MT_URBAN,MG_SUPREME,MC_BLUE,
			{
				{0,1,2,3,2,1,0,4,5,6,5,4,255},	// idle
				{21,22,21,0,23,24,23,0,255},	// move
				{7,8,9,10,11,12,13,255},	// attack
				{25,26,27,28,29,30,31,254,31,254,31,254,31,254,31,254,31,255},	// die
				{14,14,15,15,16,16,17,17,18,19,20,255},	// A1=punch2
				{7,8,9,10,11,12,13,255},	// attack
				{7,8,9,10,11,12,13,255},	// attack
				{7,8,9,10,11,12,13,255},	// attack
				{7,8,9,10,11,12,13,255},	// attack
			}},
		{"Piranha",255,255,0,AI_Piranha,
		 7,14,5,1000,"graphics/piranha.jsp",0,MF_FLYING,
		 MT_EVIL|MT_UNDERSEA|MT_ANIMAL,MG_SUPREME,MC_VIOLET,
			{
				{0,1,2,3,2,1,0,4,5,6,5,4,255},	// idle
				{0,1,2,3,2,1,0,4,5,6,5,4,255},	// move
				{0,1,2,3,2,1,0,4,5,6,5,4,255},	// attack
				{7,8,9,10,11,12,13,255},	// die
				{0,1,2,3,2,1,0,4,5,6,5,4,255},	// attack
				{0,1,2,3,2,1,0,4,5,6,5,4,255},	// attack
				{0,1,2,3,2,1,0,4,5,6,5,4,255},	// attack
				{0,1,2,3,2,1,0,4,5,6,5,4,255},	// attack
				{0,1,2,3,2,1,0,4,5,6,5,4,255},	// attack
			}},
		{"You-Go",255,255,0,AI_Yugo,
		 14,16,5,1000,"graphics/yugo.jsp",0,MF_NOMOVE|MF_NOHIT|MF_INVINCIBLE|MF_FREEWALK|MF_WATERWALK,
		 MT_VEHICLE|MT_GOOD,MG_SUPREME,MC_RED,
			{
				{0,255},	// idle
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,255},	// attack
				{0,0,0,255},	// die
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
			}},
		{"Mini-Sub",255,255,0,NULL,	// in mini-sub
		 11,1,128,0,"graphics/minisub.jsp",0,MF_FLYING,
		 MT_WEIRD,MG_SUPREME,MC_RED,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{0,0,0,255}, // die
				{0,255},	// fire missiles
				{0,255},	// activate
				{0,0,0,255}, // die
				{0,0,0,255}, // die
				{0,0,0,255}, // die
			}},
		{"Punkin",255,255,0,AI_Punkin,
		 9,10,10,1000,"graphics/punkin.jsp",0,0,
		 MT_EVIL|MT_PUMPKIN|MT_VEGGIE|MT_URBAN,MG_SUPREME,MC_VIOLET,
			{
				{0,255},	// idle
				{0,1,2,3,4,5,6,7,8,9,255},	// move
				{0,255},	// attack
				{0,0,0,0,255},	// die
				{0,1,2,3,4,5,6,7,8,9,255},	// move
				{0,1,2,3,4,5,6,7,8,9,255},	// move
				{0,1,2,3,4,5,6,7,8,9,255},	// move
				{0,1,2,3,4,5,6,7,8,9,255},	// move
				{0,1,2,3,4,5,6,7,8,9,255},	// move
			}},
		{"Sea Cucumber",255,255,0,AI_SeaCuke,
		 11,32,100,1000,"graphics/seacuke.jsp",0,0,
		 MT_EVIL|MT_VEGGIE|MT_UNDERSEA|MT_MINIBOSS,MG_SUPREME,MC_GREEN,
			{
				{0,255},	// idle
				{9,10,11,12,11,10,9,13,14,15,14,13,9,255},	// move
				{1,2,3,4,5,6,6,7,7,8,255},	// attack
				{25,26,27,28,29,30,31,254,31,254,31,254,31,254,31,255},	// die
				{16,17,18,19,20,21,22,23,24,255}, // A1=dance
				{1,2,3,4,5,6,6,7,7,8,255},	// attack
				{1,2,3,4,5,6,6,7,7,8,255},	// attack
				{1,2,3,4,5,6,6,7,7,8,255},	// attack
				{1,2,3,4,5,6,6,7,7,8,255},	// attack
			}},
		{"Sneaky Shark",255,255,0,AI_Sneaky2,
		 11,32,50,1000,"graphics/snkyshrk2.jsp",0,MF_NOMOVE|MF_ONEFACE,
		 MT_EVIL|MT_UNDERSEA|MT_TRAP,MG_SUPREME,MC_AQUA,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,6,5,4,3,2,1,255},	// attack
				{0,0,0,255},	// die
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,6,5,4,3,2,1,255},	// attack
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,6,5,4,3,2,1,255},	// attack
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,6,5,4,3,2,1,255},	// attack
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,6,5,4,3,2,1,255},	// attack
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,6,5,4,3,2,1,255},	// attack
			}},
		{"Starfish",255,255,0,AI_Starfish,
		 11,31,15,1000,"graphics/starfish.jsp",0,0,
		 MT_EVIL|MT_UNDERSEA|MT_ANIMAL,MG_SUPREME,MC_YELLOW,
			{
				{0,255},	// idle
				{1,2,3,10,9,8,7,6,5,4,2,1,0,255},	// move
				{11,12,13,14,15,16,17,18,19,12,11,255},	// attack
				{20,21,22,23,24,25,26,27,28,29,30,255},	// die
				{12,11,255},	// A1=stop buzzsawing
				{2,1,0,255},	// A2=stop walking
				{11,12,13,14,15,16,17,18,19,12,11,255},	// attack
				{11,12,13,14,15,16,17,18,19,12,11,255},	// attack
				{11,12,13,14,15,16,17,18,19,12,11,255},	// attack
			}},
		{"Aquazoid",255,255,0,AI_Underzoid,
		 17,29,15,1000,"graphics/underzoid.jsp",0,MF_FLYING,
		 MT_EVIL|MT_UNDERSEA|MT_ZOID,MG_SUPREME,MC_GREEN,
			{
				{0,255},	// idle
				{0,1,2,3,4,5,6,7,8,9,10,11,255},	// move
				{12,13,14,15,16,17,18,19,20,255},	// attack
				{21,22,23,24,25,26,27,28,254,28,254,28,254,28,254,255},	// die
				{12,13,14,15,16,17,18,19,20,255},	// attack
				{12,13,14,15,16,17,18,19,20,255},	// attack
				{12,13,14,15,16,17,18,19,20,255},	// attack
				{12,13,14,15,16,17,18,19,20,255},	// attack
				{12,13,14,15,16,17,18,19,20,255},	// attack
			}},
		{"Wet Seal",255,255,0,AI_WetSeal,
		 22,22,20,1000,"graphics/wetseal.jsp",0,MF_FLYING,
		 MT_EVIL|MT_UNDERSEA|MT_ANIMAL|MT_ARCTIC,MG_SUPREME,MC_GREY,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,7,8,9,8,9,10,11,10,11,12,13,12,13,14,14,14,15,15,16,255},	// attack=full auto
				{17,18,19,20,21,255},	// die
				{7,7,8,9,8,9,10,11,10,11,12,13,12,13,14,14,14,15,15,16,255},	// attack=full auto
				{7,7,8,9,8,9,10,11,10,11,12,13,12,13,14,14,14,15,15,16,255},	// attack=full auto
				{7,7,8,9,8,9,10,11,10,11,12,13,12,13,14,14,14,15,15,16,255},	// attack=full auto
				{7,7,8,9,8,9,10,11,10,11,12,13,12,13,14,14,14,15,15,16,255},	// attack=full auto
				{7,7,8,9,8,9,10,11,10,11,12,13,12,13,14,14,14,15,15,16,255},	// attack=full auto
			}},
		{"Wet Willy",255,255,0,AI_WetWilly,
		 12,20,30,1000,"graphics/willy.jsp",0,0,
		 MT_EVIL|MT_URBAN|MT_WACKY,MG_SUPREME,MC_YELLOW,
			{
				{0,1,2,3,4,5,6,7,8,9,10,11,255},	// idle
				{0,1,2,3,4,5,6,7,8,9,10,11,255},	// move
				{12,13,14,15,15,15,15,15,14,13,12,255},	// attack
				{16,17,18,19,17,18,19,18,19,255},	// die
				{12,13,14,15,15,15,15,15,14,13,12,255},	// attack
				{12,13,14,15,15,15,15,15,14,13,12,255},	// attack
				{12,13,14,15,15,15,15,15,14,13,12,255},	// attack
				{12,13,14,15,15,15,15,15,14,13,12,255},	// attack
				{12,13,14,15,15,15,15,15,14,13,12,255},	// attack
			}},
		{"Crab Puff",255,255,0,AI_CrabPuff,
		 7,5,1,1000,"graphics/puff.jsp",0,0,
		 MT_EVIL|MT_UNDERSEA|MT_ANIMAL,MG_SUPREME,MC_AQUA,
			{
				{0,255},	// idle
				{0,1,2,3,4,3,2,1,255},	// move
				{0,255},	// attack
				{0,0,255},	// die
				{0,1,2,3,4,3,2,1,255},	// move
				{0,1,2,3,4,3,2,1,255},	// move
				{0,1,2,3,4,3,2,1,255},	// move
				{0,1,2,3,4,3,2,1,255},	// move
				{0,1,2,3,4,3,2,1,255},	// move
			}},
		{"Pumpkin Patch",255,255,0,AI_Patch,
		 16,17,150,1000,"graphics/patch.jsp",0,MF_NOMOVE|MF_ENEMYWALK|MF_ONEFACE|MF_NOSHADOW,
		 MT_EVIL|MT_VEGGIE|MT_PUMPKIN|MT_GENERATE,MG_SUPREME,MC_GREEN,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,0,2,0,1,2,1,0,1,0,2,1,2,1,0,2,1,2,255},	// attack
				{11,12,13,14,15,16,255},	// die
				{3,4,5,6,7,8,9,10,255},	// A1=summon
				{1,2,0,2,0,1,2,1,0,1,0,2,1,2,1,0,2,1,2,255},	// attack
				{1,2,0,2,0,1,2,1,0,1,0,2,1,2,1,0,2,1,2,255},	// attack
				{1,2,0,2,0,1,2,1,0,1,0,2,1,2,1,0,2,1,2,255},	// attack
				{1,2,0,2,0,1,2,1,0,1,0,2,1,2,1,0,2,1,2,255},	// attack
			}},
		{"Pumpkin Patch",255,255,2,AI_Patch,
		 16,17,150,1000,"!162",0,MF_NOMOVE|MF_ENEMYWALK|MF_ONEFACE|MF_NOSHADOW,
		 MT_EVIL|MT_VEGGIE|MT_PUMPKIN|MT_GENERATE,MG_SUPREME,MC_GREEN,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,0,2,0,1,2,1,0,1,0,2,1,2,1,0,2,1,2,255},	// attack
				{11,12,13,14,15,16,255},	// die
				{3,4,5,6,7,8,9,10,255},	// A1=summon
				{3,4,5,6,7,8,9,10,255},	// A1=summon
				{3,4,5,6,7,8,9,10,255},	// A1=summon
				{3,4,5,6,7,8,9,10,255},	// A1=summon
				{3,4,5,6,7,8,9,10,255},	// A1=summon
			}},
		{"Boomkin Patch",1,4,0,AI_Patch,
		 16,17,150,1000,"!162",0,MF_NOMOVE|MF_ENEMYWALK|MF_ONEFACE|MF_NOSHADOW,
		 MT_EVIL|MT_VEGGIE|MT_PUMPKIN|MT_GENERATE,MG_SUPREME,MC_RED,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,0,2,0,1,2,1,0,1,0,2,1,2,1,0,2,1,2,255},	// attack
				{11,12,13,14,15,16,255},	// die
				{3,4,5,6,7,8,9,10,255},	// A1=summon
				{3,4,5,6,7,8,9,10,255},	// A1=summon
				{3,4,5,6,7,8,9,10,255},	// A1=summon
				{3,4,5,6,7,8,9,10,255},	// A1=summon
				{3,4,5,6,7,8,9,10,255},	// A1=summon
			}},
		{"Squash Patch",255,255,4,AI_Patch,
		 16,17,150,1000,"!162",0,MF_NOMOVE|MF_ENEMYWALK|MF_ONEFACE|MF_NOSHADOW,
		 MT_EVIL|MT_VEGGIE|MT_PUMPKIN|MT_GENERATE,MG_SUPREME,MC_GREEN,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,0,2,0,1,2,1,0,1,0,2,1,2,1,0,2,1,2,255},	// attack
				{11,12,13,14,15,16,255},	// die
				{3,4,5,6,7,8,9,10,255},	// A1=summon
				{3,4,5,6,7,8,9,10,255},	// A1=summon
				{3,4,5,6,7,8,9,10,255},	// A1=summon
				{3,4,5,6,7,8,9,10,255},	// A1=summon
				{3,4,5,6,7,8,9,10,255},	// A1=summon
			}},
		{"Crabby Patty",255,255,0,AI_Patty,
		 34,17,300,1000,"graphics/patty.jsp",0,MF_NOMOVE,
		 MT_EVIL|MT_UNDERSEA|MT_BOSS|MT_ANIMAL,MG_SUPREME,MC_VIOLET,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{8,9,10,10,10,10,10,9,8,255},	// attack
				{11,12,13,14,15,16,255},	// die
				{5,6,6,6,6,6,6,6,7,255},	// A1=summon
				{8,9,10,10,10,10,10,9,8,255},	// attack
				{8,9,10,10,10,10,10,9,8,255},	// attack
				{8,9,10,10,10,10,10,9,8,255},	// attack
				{8,9,10,10,10,10,10,9,8,255},	// attack
			}},
		{"Dozer The Dozerian",255,255,0,AI_Dozer,
		 140,17,1000,1000,"graphics/dozer.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_FACECMD,
		 MT_EVIL|MT_VEHICLE|MT_BOSS|MT_URBAN|MT_FOREST,MG_SUPREME,MC_YELLOW,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,4,5,6,4,5,6,4,5,6,3,2,1,255},	// attack (left spin)
				{29,29,30,30,31,31,255},	// die
				{7,8,9,10,11,12,10,11,12,10,11,12,10,11,12,9,8,7,255},	// A1=right spin
				{13,14,15,16,17,18,18,255},	// A2=lose arms
				{0,0,0,0,0,0,255},	// A3=grill explode
				{1,2,3,4,5,6,4,5,6,4,5,6,4,5,6,3,2,1,255},	// attack (left spin)
				{1,2,3,4,5,6,4,5,6,4,5,6,4,5,6,3,2,1,255},	// attack (left spin)
			}},
		{"Dark Coffin",6,0,-6,AI_Coffin,
		 25,20,2000,500,"!64",0,MF_NOMOVE|MF_ONEFACE|MF_NOSHADOW,
		 MT_EVIL|MT_TRAP|MT_VAMPIRE|MT_SPOOKY,MG_SUPREME,MC_BROWN,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,255},	// attack
				{17,18,19,255},		// die
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,255},	// attack
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,255},	// attack
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,255},	// attack
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,255},	// attack
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,255},	// attack
			}},
		{"Mutant Zombie",0,1,0,AI_MutantZombie,
		 11,32,30,150,"!6",0,0,
		 MT_SPOOKY|MT_ZOMBIE|MT_EVIL,MG_SUPREME,MC_GREEN,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,9,10,11,12,13,13,14,15,16,17,18,19,255},	// attack
				{20,21,22,23,24,25,26,27,28,29,30,31,255},	// die
				{7,8,9,9,10,11,12,13,13,14,15,16,17,18,19,255},	// attack
				{7,8,9,9,10,11,12,13,13,14,15,16,17,18,19,255},	// attack
				{7,8,9,9,10,11,12,13,13,14,15,16,17,18,19,255},	// attack
				{7,8,9,9,10,11,12,13,13,14,15,16,17,18,19,255},	// attack
				{7,8,9,9,10,11,12,13,13,14,15,16,17,18,19,255},	// attack
			}},
		{"Whatsit",1,4,-4,AI_Whatsit,
		 12,44,40,50,"!20",0,0,
		 MT_THINGY|MT_EVIL,MG_SUPREME,MC_RED,
			{
				{12,255},	// idle
				{1,2,3,4,5,6,7,8,9,10,11,255},	// move
				{20,21,22,23,24,25,26,27,255},	// attack
				{34,35,36,37,38,39,40,41,42,43,255},	// die
				{13,14,15,16,17,16,15,16,17,16,15,16,17,16,15,255},	// fall asleep (A1)
				{18,19,18,255},	// blink (A2)
				{28,29,30,29,28,255}, // look right (A3)
				{31,32,33,32,31,255}, // look left (A4)
				{20,21,22,23,24,25,26,27,255},	// attack
		}},
		{"Shocking Horror",255,255,3,AI_ShockGhost,
		 18,7,15,300,"graphics/ghost.jsp",0,MF_WALLWALK|MF_NOSHADOW|MF_FLYING|MF_GHOST|MF_ENEMYWALK|MF_FREEWALK,
		 MT_EVIL|MT_SPOOKY|MT_FLYING|MT_TRAP,MG_SUPREME,MC_GLOW,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,3,3,2,1,255},	// attack (scream)
				{4,4,5,5,6,6,255},		// die
				{4,5,6,6,5,4,255},		// A1=teleport
				{254,255},				// A2=idle invisible
				{6,5,4,255},			// A3=materialize
				{1,2,3,3,3,2,1,255},	// attack (scream)
				{1,2,3,3,3,2,1,255},	// attack (scream)
			}},
		{"Mechazoid",1,0,5,AI_Mechazoid,
		 17,29,20,1000,"!158",0,MF_FLYING,
		 MT_EVIL|MT_HIGHTECH|MT_ZOID|MT_FLYING,MG_SUPREME,MC_GREY,
			{
				{0,255},	// idle
				{0,1,2,3,4,5,6,7,8,9,10,11,255},	// move
				{12,13,14,15,16,17,18,18,18,18,18,19,20,255},	// attack
				{21,22,23,24,25,26,27,28,254,28,254,28,254,28,254,255},	// die
				{12,13,14,15,16,17,18,18,18,18,18,19,20,255},	// attack
				{12,13,14,15,16,17,18,18,18,18,18,19,20,255},	// attack
				{12,13,14,15,16,17,18,18,18,18,18,19,20,255},	// attack
				{12,13,14,15,16,17,18,18,18,18,18,19,20,255},	// attack
				{12,13,14,15,16,17,18,18,18,18,18,19,20,255},	// attack
			}},
		{"Sucker",0,1,-5,AI_Sucker,
		 18,2,30,300,"!66",0,MF_NOSHADOW|MF_ONEFACE|MF_NOMOVE|MF_NOHIT|MF_INVINCIBLE|MF_ENEMYWALK|MF_FREEWALK,
		 MT_EVIL|MT_TRAP,MG_SUPREME,MC_GREEN,
			{
				{254,254,254,255},	// idle
				{254,254,254,255},	// move
				{254,254,254,255},	// attack
				{254,255},		// die
				{254,254,254,255},	// move
				{254,254,254,255},	// move
				{254,254,254,255},	// move
				{254,254,254,255},	// move
				{254,254,254,255},	// move
			}},
		{"Sucker: Strong",0,1,0,AI_Sucker,
		 18,2,30,300,"!66",0,MF_NOSHADOW|MF_ONEFACE|MF_NOMOVE|MF_NOHIT|MF_INVINCIBLE|MF_ENEMYWALK|MF_FREEWALK,
		 MT_EVIL|MT_TRAP,MG_SUPREME,MC_GREEN,
			{
				{254,254,254,255},	// idle
				{254,254,254,255},	// move
				{254,254,254,255},	// attack
				{254,255},		// die
				{254,254,254,255},	// move
				{254,254,254,255},	// move
				{254,254,254,255},	// move
				{254,254,254,255},	// move
				{254,254,254,255},	// move
			}},
		{"Sucker: Mini",0,1,5,AI_Sucker,
		 18,2,30,300,"!66",0,MF_NOSHADOW|MF_ONEFACE|MF_NOMOVE|MF_NOHIT|MF_INVINCIBLE|MF_ENEMYWALK|MF_FREEWALK,
		 MT_EVIL|MT_TRAP,MG_SUPREME,MC_GREEN,
			{
				{254,254,254,255},	// idle
				{254,254,254,255},	// move
				{254,254,254,255},	// attack
				{254,255},		// die
				{254,254,254,255},	// move
				{254,254,254,255},	// move
				{254,254,254,255},	// move
				{254,254,254,255},	// move
				{254,254,254,255},	// move
			}},
		{"Sucker: Strong Mini",0,1,10,AI_Sucker,
		 18,2,30,300,"!66",0,MF_NOSHADOW|MF_ONEFACE|MF_NOMOVE|MF_NOHIT|MF_INVINCIBLE|MF_ENEMYWALK|MF_FREEWALK,
		 MT_EVIL|MT_TRAP,MG_SUPREME,MC_GREEN,
			{
				{254,254,254,255},	// idle
				{254,254,254,255},	// move
				{254,254,254,255},	// attack
				{254,255},		// die
				{254,254,254,255},	// move
				{254,254,254,255},	// move
				{254,254,254,255},	// move
				{254,254,254,255},	// move
				{254,254,254,255},	// move
			}},
		{"Blower",0,4,-5,AI_Blower,
		 18,2,30,300,"!66",0,MF_NOSHADOW|MF_ONEFACE|MF_NOMOVE|MF_NOHIT|MF_INVINCIBLE|MF_ENEMYWALK|MF_FREEWALK,
		 MT_EVIL|MT_TRAP,MG_SUPREME,MC_RED,
			{
				{254,254,254,255},	// idle
				{254,254,254,255},	// move
				{254,254,254,255},	// attack
				{254,255},		// die
				{254,254,254,255},	// move
				{254,254,254,255},	// move
				{254,254,254,255},	// move
				{254,254,254,255},	// move
				{254,254,254,255},	// move
			}},
		{"Blower: Strong",0,4,0,AI_Blower,
		 18,2,30,300,"!66",0,MF_NOSHADOW|MF_ONEFACE|MF_NOMOVE|MF_NOHIT|MF_INVINCIBLE|MF_ENEMYWALK|MF_FREEWALK,
		 MT_EVIL|MT_TRAP,MG_SUPREME,MC_RED,
			{
				{254,254,254,255},	// idle
				{254,254,254,255},	// move
				{254,254,254,255},	// attack
				{254,255},		// die
				{254,254,254,255},	// move
				{254,254,254,255},	// move
				{254,254,254,255},	// move
				{254,254,254,255},	// move
				{254,254,254,255},	// move
			}},
		{"Blower: Mini",0,4,5,AI_Blower,
		 18,2,30,300,"!66",0,MF_NOSHADOW|MF_ONEFACE|MF_NOMOVE|MF_NOHIT|MF_INVINCIBLE|MF_ENEMYWALK|MF_FREEWALK,
		 MT_EVIL|MT_TRAP,MG_SUPREME,MC_RED,
			{
				{254,254,254,255},	// idle
				{254,254,254,255},	// move
				{254,254,254,255},	// attack
				{254,255},		// die
				{254,254,254,255},	// move
				{254,254,254,255},	// move
				{254,254,254,255},	// move
				{254,254,254,255},	// move
				{254,254,254,255},	// move
			}},
		{"Blower: Strong Mini",0,4,10,AI_Blower,
		 18,2,30,300,"!66",0,MF_NOSHADOW|MF_ONEFACE|MF_NOMOVE|MF_NOHIT|MF_INVINCIBLE|MF_ENEMYWALK|MF_FREEWALK,
		 MT_EVIL|MT_TRAP,MG_SUPREME,MC_RED,
			{
				{254,254,254,255},	// idle
				{254,254,254,255},	// move
				{254,254,254,255},	// attack
				{254,255},		// die
				{254,254,254,255},	// move
				{254,254,254,255},	// move
				{254,254,254,255},	// move
				{254,254,254,255},	// move
				{254,254,254,255},	// move
			}},
		{"The Beetles",7,0,-8,AI_Beetle,
		 7,5,1,1000,"!161",0,0,
		 MT_EVIL|MT_DESERT|MT_ANIMAL,MG_SUPREME,MC_GREY,
			{
				{0,255},	// idle
				{0,1,2,3,4,3,2,1,255},	// move
				{0,255},	// attack
				{0,0,255},	// die
				{0,1,2,3,4,3,2,1,255},	// move
				{0,1,2,3,4,3,2,1,255},	// move
				{0,1,2,3,4,3,2,1,255},	// move
				{0,1,2,3,4,3,2,1,255},	// move
				{0,1,2,3,4,3,2,1,255},	// move
			}},
		{"Scary Scarab",6,4,-3,AI_Scarab,
		 34,17,150,1000,"!166",0,MF_NOMOVE,
		 MT_EVIL|MT_DESERT|MT_MINIBOSS|MT_ANIMAL,MG_SUPREME,MC_RED,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{8,9,10,10,10,10,10,10,10,10,9,8,255},	// attack
				{11,12,13,14,15,16,255},	// die
				{5,6,6,6,6,6,6,6,7,255},	// A1=summon
				{8,9,10,10,10,10,10,10,10,10,9,8,255},	// attack
				{8,9,10,10,10,10,10,10,10,10,9,8,255},	// attack
				{8,9,10,10,10,10,10,10,10,10,9,8,255},	// attack
				{8,9,10,10,10,10,10,10,10,10,9,8,255},	// attack
			}},
		{"Frostbiter",1,7,3,AI_Frostbiter,
		 8,21,10,50,"!5",0,0,
		 MT_EVIL|MT_ARCTIC|MT_SPIDER|MT_ANIMAL,MG_SUPREME,MC_AQUA,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,12,13,13,255},	// attack
				{14,15,16,17,18,17,18,17,18,19,19,20,20,255},	// die
				{7,8,9,10,11,12,12,13,13,255},	// attack
				{7,8,9,10,11,12,12,13,13,255},	// attack
				{7,8,9,10,11,12,12,13,13,255},	// attack
				{7,8,9,10,11,12,12,13,13,255},	// attack
				{7,8,9,10,11,12,12,13,13,255},	// attack
			}},
		{"Missile Turret",0,3,4,AI_Turret,
		 10,1,80,200,"!82",0,MF_NOMOVE,
		 MT_EVIL|MT_TRAP|MT_HIGHTECH,MG_SUPREME,MC_BLUE,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{0,0,0,255},	// die
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
			}},
		{"Death Turret",0,7,-4,AI_Turret,
		 10,1,80,200,"!82",0,MF_NOMOVE,
		 MT_EVIL|MT_TRAP|MT_HIGHTECH,MG_SUPREME,MC_AQUA,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{0,0,0,255},	// die
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
			}},
		{"Magmazoid",1,4,0,AI_UnderMagmazoid,
		 17,29,20,1000,"!158",0,MF_FLYING,
		 MT_EVIL|MT_ZOID|MT_UNDERSEA,MG_SUPREME,MC_RED,
			{
				{0,255},	// idle
				{0,1,2,3,4,5,6,7,8,9,10,11,255},	// move
				{12,13,14,15,16,17,18,18,18,18,18,19,20,255},	// attack
				{21,22,23,24,25,26,27,28,254,28,254,28,254,28,254,255},	// die
				{12,13,14,15,16,17,18,18,18,18,18,19,20,255},	// attack
				{12,13,14,15,16,17,18,18,18,18,18,19,20,255},	// attack
				{12,13,14,15,16,17,18,18,18,18,18,19,20,255},	// attack
				{12,13,14,15,16,17,18,18,18,18,18,19,20,255},	// attack
				{12,13,14,15,16,17,18,18,18,18,18,19,20,255},	// attack
			}},
		{"Parked Car",4,3,0,AI_ParkedCar,
		 15,16,15,1000,"!152",0,MF_NOMOVE,
		 MT_VEHICLE|MT_TRAP|MT_URBAN,MG_SUPREME,MC_BLUE,
			{
				{0,255},	// idle
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,255},	// attack
				{0,0,0,255},	// die
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
			}},
		{"Parked Car",4,5,0,AI_ParkedCar,
		 15,16,15,1000,"!152",0,MF_NOMOVE,
		 MT_VEHICLE|MT_TRAP|MT_URBAN,MG_SUPREME,MC_YELLOW,
			{
				{0,255},	// idle
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,255},	// attack
				{0,0,0,255},	// die
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
			}},
		{"Traffic",4,6,0,AI_Traffic,
		 11,16,200,1000,"!152",0,MF_NOMOVE|MF_WALLWALK,
		 MT_VEHICLE|MT_TRAP|MT_EVIL|MT_URBAN,MG_SUPREME,MC_VIOLET,
			{
				{0,255},	// idle
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,255},	// attack
				{0,0,0,255},	// die
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
			}},
		{"Traffic",4,1,0,AI_Traffic,
		 11,16,200,1000,"!152",0,MF_NOMOVE|MF_WALLWALK,
		 MT_VEHICLE|MT_TRAP|MT_EVIL|MT_URBAN,MG_SUPREME,MC_GREEN,
			{
				{0,255},	// idle
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,255},	// attack
				{0,0,0,255},	// die
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
			}},
		{"MechaBouapha",0,4,0,NULL,	// playable version
		 11,24,250,1000,"graphics/mechabouapha.jsp",0,MF_WATERWALK,
		 MT_WEIRD,MG_SUPREME,MC_RED,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,0,0,255},	// attack=hammer toss
				{15,16,17,18,19,20,21,22,23,255},	// die
				{9,11,13,14,13,255},	// A1=use weapon
				{15,16,17,18,19,20,21,22,23,21,19,17,15,255},	// A2=bored
				{1,3,1,0,4,6,4,0,1,3,1,0,0,0,17,19,22,255},	// A3=watery death
				{7,8,9,10,11,12,13,14,0,0,255},	// attack=hammer toss
				{7,8,9,10,11,12,13,14,0,0,255},	// attack=hammer toss
			}},
		{"Shroomlord",255,255,0,AI_ShroomLord,
		 15,14,40,300,"graphics/shrmlord.jsp",0,0,
		 MT_EVIL|MT_VEGGIE|MT_FOREST,MG_SUPREME,MC_GREEN,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,7,8,8,8,9,10,11,12,12,13,13,255},	// spit spores
				{0,0,255},	// die
				{7,7,8,8,8,9,10,11,12,12,13,13,255},	// spit spores
				{7,7,8,8,8,9,10,11,12,12,13,13,255},	// spit spores
				{7,7,8,8,8,9,10,11,12,12,13,13,255},	// spit spores
				{7,7,8,8,8,9,10,11,12,12,13,13,255},	// spit spores
				{7,7,8,8,8,9,10,11,12,12,13,13,255},	// spit spores
			}},
		{"Scarier Bat",255,255,0,AI_BigBat,
		 26,15,200,500,"graphics/bigbat.jsp",0,MF_FLYING|MF_NOMOVE|MF_ENEMYWALK,
		 MT_EVIL|MT_ANIMAL|MT_SPOOKY|MT_FLYING,MG_SUPREME,MC_RED,
			{
				{0,1,2,1,0,3,4,5,4,3,255},	// idle
				{0,1,2,1,0,3,4,5,4,3,255},	// move
				{6,7,8,8,7,7,6,255},	// attack=spit bat
				{9,10,11,12,13,14,255},	// die
			}},
		{"Die Uberbat",255,255,0,AI_HugeBat,
		 64,7,1000,5000,"graphics/hugebat.jsp",0,MF_NOMOVE|MF_ENEMYWALK|MF_ONEFACE,
		 MT_EVIL|MT_ANIMAL|MT_SPOOKY|MT_MINIBOSS,MG_SUPREME,MC_VIOLET,
			{
				{0,0,255},	// idle
				{0,255},	// move
				{1,2,3,2,1,255},	// attack=spit bats
				{4,4,4,5,5,5,6,6,6,6,6,6,255},	// die
				{1,2,3,2,1,255},	// attack=spit bats
				{1,2,3,2,1,255},	// attack=spit bats
				{1,2,3,2,1,255},	// attack=spit bats
				{1,2,3,2,1,255},	// attack=spit bats
				{1,2,3,2,1,255},	// attack=spit bats
			}},
		{"Werepuppy",255,255,0,AI_Werewolf,
		 12,55,15,100,"graphics/wolfman.jsp",0,0,
		 MT_EVIL|MT_SPOOKY|MT_ANIMAL,MG_SUPREME,MC_BROWN,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{30,31,32,32,32,33,34,35,36,37,38,39,40,41,42,43,255},	// attack (pounce)
				{44,45,46,47,48,49,50,51,51,51,52,53,54,254,54,254,54,254,54,255},		// die
				{7,8,9,9,8,7,255},	// spit projectile = A1
				{10,11,12,12,12,13,14,15,16,17,17,18,19,255},	// charging slash = A2
				{20,21,22,23,24,25,26,26,26,26,26,26,26,26,27,28,29,255},		// howl = A3
				{30,31,32,32,32,33,34,35,36,37,38,39,40,41,42,43,255},	// attack (pounce)
				{30,31,32,32,32,33,34,35,36,37,38,39,40,41,42,43,255},	// attack (pounce)
			}},
		{"Weremutt",2,4,0,AI_Werewolf,
		 12,55,25,100,"!195",0,0,
		 MT_EVIL|MT_SPOOKY|MT_ANIMAL,MG_SUPREME,MC_RED,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{30,31,32,32,32,33,34,35,36,37,38,39,40,41,42,43,255},	// attack (pounce)
				{44,45,46,47,48,49,50,51,51,51,52,53,54,254,54,254,54,254,54,255},		// die
				{7,8,9,9,8,7,255},	// spit projectile = A1
				{10,11,12,12,12,13,14,15,16,17,17,18,19,255},	// charging slash = A2
				{20,21,22,23,24,25,26,26,26,26,26,26,26,26,27,28,29,255},		// howl = A3
				{30,31,32,32,32,33,34,35,36,37,38,39,40,41,42,43,255},	// attack (pounce)
				{30,31,32,32,32,33,34,35,36,37,38,39,40,41,42,43,255},	// attack (pounce)
			}},
		{"Patrol Mumble",0,2,0,AI_Patrol,
		 15,28,100,100,"!39",0,MF_NOMOVE,
		 MT_EVIL|MT_SPOOKY|MT_DESERT|MT_TRAP,MG_SUPREME,MC_BROWN,
			{
				{0,255},	// idle
				{1,2,3,4,5,0,6,7,8,9,10,0,255},	// move
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{19,20,21,22,23,24,25,26,27,255},	// die
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
			}},
		{"Patrol Mumble",0,2,0,AI_Patrol,
		 15,28,100,100,"!39",0,MF_NOMOVE,
		 MT_EVIL|MT_SPOOKY|MT_DESERT|MT_TRAP,MG_SUPREME,MC_BROWN,
			{
				{0,255},	// idle
				{1,2,3,4,5,0,6,7,8,9,10,0,255},	// move
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{19,20,21,22,23,24,25,26,27,255},	// die
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
			}},
		{"Death Patrol",0,4,0,AI_Patrol,
		 15,28,200,100,"!39",0,MF_NOMOVE,
		 MT_EVIL|MT_SPOOKY|MT_DESERT|MT_TRAP,MG_SUPREME,MC_RED,
			{
				{0,255},	// idle
				{1,2,3,4,5,0,6,7,8,9,10,0,255},	// move
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{19,20,21,22,23,24,25,26,27,255},	// die
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
			}},
		{"Death Patrol",0,4,0,AI_Patrol,
		 15,28,200,100,"!39",0,MF_NOMOVE,
		 MT_EVIL|MT_SPOOKY|MT_DESERT|MT_TRAP,MG_SUPREME,MC_RED,
			{
				{0,255},	// idle
				{1,2,3,4,5,0,6,7,8,9,10,0,255},	// move
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{19,20,21,22,23,24,25,26,27,255},	// die
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
			}},
		{"Pygmy Weatherman",6,3,5,AI_Weatherman,
		 10,50,100,100,"!9",0,0,
		 MT_EVIL|MT_PYGMY|MT_HUMAN|MT_MINIBOSS,MG_SUPREME,MC_BLUE,
			{
				{0,0,0,0,0,0,0,0,0,0,1,2,3,2,1,0,0,0,4,5,6,5,4,255},	// idle
				{8,9,10,9,8,7,11,12,13,12,11,7,255},	// move
				{29,30,31,31,32,33,33,34,34,35,35,36,36,255},	// attack
				{37,38,39,40,41,42,43,44,45,46,255},	// die
				{14,15,16,17,18,19,20,16,17,18,19,15,14,255},	// (A1) bored (spin spear)
				{47,48,49,48,47,0,47,48,49,48,47,255},	// (A2) angry, spotted Bouapha
				{29,30,31,31,32,33,33,34,34,35,35,36,36,255},	// attack
				{29,30,31,31,32,33,33,34,34,35,35,36,36,255},	// attack
				{29,30,31,31,32,33,33,34,34,35,35,36,36,255},	// attack
			}},
		{"Hunter Egg",0,6,0,AI_HunterEgg,
		 12,14,1000,200,"!74",0,MF_ONEFACE|MF_ENEMYWALK|MF_NOMOVE,
		 MT_EVIL|MT_ALIEN|MT_GENERATE,MG_SUPREME,MC_VIOLET,
			{
				{0,1,2,3,2,1,255},	// idle
				{0,255},	// move
				{4,5,6,7,8,9,10,11,12,13,12,11,10,9,8,7,6,5,4,255},	// attack
				{4,5,6,7,8,9,10,11,12,13,255},	// die
				{4,5,6,7,8,9,10,11,12,13,12,11,10,9,8,7,6,5,4,255},	// attack
				{4,5,6,7,8,9,10,11,12,13,12,11,10,9,8,7,6,5,4,255},	// attack
				{4,5,6,7,8,9,10,11,12,13,12,11,10,9,8,7,6,5,4,255},	// attack
				{4,5,6,7,8,9,10,11,12,13,12,11,10,9,8,7,6,5,4,255},	// attack
				{4,5,6,7,8,9,10,11,12,13,12,11,10,9,8,7,6,5,4,255},	// attack
			}},
		{"Stupid Shroom",1,4,0,NULL,
		 11,14,40,300,"!192",0,MF_WATERWALK,
		 MT_WEIRD,MG_SUPREME,MC_RED,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,7,8,8,9,10,11,12,12,13,255},	// attack
				{0,0,0,0,0,0,0,0,0,0,255},	// die
				{7,8,10,11,13,255},	// use item
				{7,8,9,9,9,9,9,9,8,7,0,0,0,0,0,255},	// bored
				{1,3,1,0,4,6,4,0,1,3,1,0,0,0,0,0,0,255},	// watery death
				{7,7,8,8,9,10,11,12,12,13,255},	// attack
				{7,7,8,8,9,10,11,12,12,13,255},	// attack
			}},
		{"Punk Bunny",255,255,0,AI_PunkBunny,
		 8,16,40,100,"graphics/punkbunny.jsp",0,0,
		 MT_ANIMAL|MT_URBAN|MT_GOOD,MG_SUPREME,MC_GREY,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// walk
				{5,6,7,8,9,7,8,9,7,8,9,10,7,6,5,255},	// attack (machine gun)
				{0,0,254,254,255},	// die
				{11,12,13,14,15,255},	// A1 = bomb throw
				{5,6,7,8,9,7,8,9,7,8,9,10,7,6,5,255},	// attack (machine gun)
				{5,6,7,8,9,7,8,9,7,8,9,10,7,6,5,255},	// attack (machine gun)
				{5,6,7,8,9,7,8,9,7,8,9,10,7,6,5,255},	// attack (machine gun)
				{5,6,7,8,9,7,8,9,7,8,9,10,7,6,5,255},	// attack (machine gun)
			}},
		{"LunaChick",255,255,0,NULL,	// playable char
		 11,27,250,1000,"graphics/lunachick.jsp",0,MF_WATERWALK,
		 MT_WEIRD,MG_SUPREME,MC_AQUA,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,13,0,0,255},	// attack=fire gun
				{14,15,16,17,18,19,20,21,22,23,24,25,26,255},	// die
				{7,9,10,12,13,255},	// A1=use weapon
				{14,15,16,17,18,17,16,15,14,15,17,15,14,0,0,255},	// A2=bored
				{1,3,1,0,4,6,4,0,1,3,1,0,0,0,24,25,26,255},	// A3=watery death
				{7,8,9,10,11,12,13,13,0,0,255},	// attack=fire gun
				{7,8,9,10,11,12,13,13,0,0,255},	// attack=fire gun
			}},
		{"Skele-TON",4,1,-8,AI_Skeleton,
		 8,38,70,50,"!2",0,MF_NOMOVE,
		 MT_SPOOKY|MT_EVIL|MT_SKELETON,MG_SUPREME,MC_GREY,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,9,9,9,9,9,9,8,8,8,7,255},	// attack
				{7,8,9,10,11,12,13,14,15,16,255},		// die
				{25,26,27,28,29,30,255},	// chomp/spit projectile
				{31,32,32,32,31,255},	// point at bouapha
				{16,15,14,13,12,11,10,9,8,7,255},	// resurrect=A3
				{7,8,9,9,9,9,9,9,9,8,8,8,7,255},	// attack
				{7,8,9,9,9,9,9,9,9,8,8,8,7,255},	// attack
			}},
		{"Hot N' Sour",1,4,0,AI_HotShroom,
		 15,23,10,50,"!24",0,0,
		 MT_EVIL|MT_VEGGIE,MG_SUPREME,MC_RED,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,255},	// attack
				{15,16,17,18,19,20,21,22,255},	// die
				{7,8,9,10,11,12,13,14,255},	// attack
				{7,8,9,10,11,12,13,14,255},	// attack
				{7,8,9,10,11,12,13,14,255},	// attack
				{7,8,9,10,11,12,13,14,255},	// attack
				{7,8,9,10,11,12,13,14,255},	// attack
			}},
		{"Microzoid",2,5,0,AI_Microzoid,
		 6,20,1,75,"graphics/minizoid.jsp",0,MF_WATERWALK|MF_AQUATIC,
		 MT_EVIL|MT_ZOID|MT_AQUATIC|MT_WACKY,MG_SUPREME,MC_YELLOW,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,255},	// move
				{4,5,6,7,8,9,10,11,12,255},	// attack=spit something small
				{13,14,15,16,17,18,19,255},	// die
				{4,5,6,7,8,9,10,11,12,255},	// attack=spit something small
				{4,5,6,7,8,9,10,11,12,255},	// attack=spit something small
				{4,5,6,7,8,9,10,11,12,255},	// attack=spit something small
				{4,5,6,7,8,9,10,11,12,255},	// attack=spit something small
				{4,5,6,7,8,9,10,11,12,255},	// attack=spit something small
			}},
		{"A Little Crazy",5,1,0,AI_MiniWacko,
		 6,21,5,100,"graphics/miniwacko.jsp",0,0,
		 MT_EVIL|MT_HUMAN|MT_WACKY|MT_ASYLUM,MG_SUPREME,MC_GREEN,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,11,10,9,10,11,12,11,10,9,10,11,12,13,14,255},	// attack=stab
				{15,16,17,18,19,20,254,20,254,20,254,20,254,20,255},	// die
				{7,8,9,10,11,12,11,10,9,10,11,12,11,10,9,10,11,12,13,14,255},	// attack=stab
				{7,8,9,10,11,12,11,10,9,10,11,12,11,10,9,10,11,12,13,14,255},	// attack=stab
				{7,8,9,10,11,12,11,10,9,10,11,12,11,10,9,10,11,12,13,14,255},	// attack=stab
				{7,8,9,10,11,12,11,10,9,10,11,12,11,10,9,10,11,12,13,14,255},	// attack=stab
				{7,8,9,10,11,12,11,10,9,10,11,12,11,10,9,10,11,12,13,14,255},	// attack=stab
			}},
		{"Jack Frost",4,7,2,AI_JackFrost,
		 50,23,400,2000,"!45",0,MF_NOMOVE|MF_ONEFACE|MF_NOSHADOW,
		 MT_EVIL|MT_ARCTIC|MT_MINIBOSS,MG_SUPREME,MC_AQUA,
			{
				{0,1,2,3,4,5,255},	// idle
				{0,1,2,3,4,5,255},	// move
				{6,7,8,9,10,11,12,13,14,15,16,255},	// attack=swipe pitchfork
				{17,18,19,20,21,22,255},	// die
				{6,7,8,9,10,11,12,13,14,15,16,255},	// attack=swipe pitchfork
				{6,7,8,9,10,11,12,13,14,15,16,255},	// attack=swipe pitchfork
				{6,7,8,9,10,11,12,13,14,15,16,255},	// attack=swipe pitchfork
				{6,7,8,9,10,11,12,13,14,15,16,255},	// attack=swipe pitchfork
				{6,7,8,9,10,11,12,13,14,15,16,255},	// attack=swipe pitchfork
			}},
		{"Kid Mystic",255,255,0,NULL,
		 11,25,128,0,"kmgraphics/sylabulus.jsp",0,MF_WATERWALK,
		 MT_BITS,MG_MYSTIC,MC_BLUE,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,10,11,11,12,12,255},	// attack
				{15,16,17,18,19,20,21,22,23,24,255},		// die
				{13,13,14,14,13,13,255},	// cast non-pointy spell
				{7,8,9,10,11,12,12,255},	// cast pointy spell
				{1,3,1,0,4,6,4,0,1,3,1,0,0,254,254,254,254,255},	// watery death
				{24,23,22,21,20,19,18,17,16,15,255},		// A4=undie
				{0,255},
			}},
		//KID MYSTIC
		{"Eye Guy",255,255,0,AI_Eyeguy, //good to go - just add particles upon yelwave despawn
		 14,17,20,75,"kmgraphics/eyeguy.jsp",0,0,
		 MT_EVIL|MT_THINGY,MG_MYSTIC,MC_YELLOW,
			{
				{0,0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,255},	// attack
				{13,14,15,16,255},	// die
				{7,8,9,10,11,12,255},	// a1
				{7,8,9,10,11,12,255},	// a2
				{7,8,9,10,11,12,255},	// a3
				{7,8,9,10,11,12,255},	// a4
				{7,8,9,10,11,12,255},	// a5
			}},
		{"Oculoid",255,255,0,AI_Peeper, //good to go!
		 26,8,40,150,"kmgraphics/peeper.jsp",0,MF_FLYING|MF_NOMOVE,
		 MT_EVIL|MT_THINGY|MT_FLYING,MG_MYSTIC,MC_RED,
			{
				{4,4,255},	// idle
				{4,4,255},	// move
				{5,6,7,6,5,255},	// fire
				{3,2,1,1,1,0,0,0,0,255},	// die
				{3,2,1,0,1,2,3,255},	// A1 = blink
				{0,0,1,1,2,2,3,3,255},	// A2 = wake up
				{5,6,7,6,5,255},	// a3
				{5,6,7,6,5,255},	// a4
				{5,6,7,6,5,255},	// a5
			}},
		{"The Rook",255,255,0,AI_Tower, //good to go
		 60,17,500,500,"kmgraphics/tower.jsp",0,MF_NOMOVE,
		 MT_EVIL|MT_MINIBOSS,MG_MYSTIC,MC_GREY,
			{
				{0,0,0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{5,5,6,6,7,7,7,8,9,9,9,9,9,10,10,10,11,11,11,255},	// attack (leap)
				{12,13,14,15,16,16,16,16,16,255},	// die
				{1,2,2,2,2,2,2,1,255},	// A1
				{1,2,2,2,2,2,2,1,255},	// A2
				{1,2,2,2,2,2,2,1,255},	// A3
				{1,2,2,2,2,2,2,1,255},	// A4
				{1,2,2,2,2,2,2,1,255},	// A5
			}},
		{"The Lookey-Loo",255,255,0,AI_Lookeyloo, //mind4
		 64,7,1000,2000,"kmgraphics/lookyloo.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_SPRITEBOX,
		 MT_EVIL|MT_BOSS|MT_THINGY,MG_MYSTIC,MC_GREEN,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,6,6,6,6,6,6,6,6,6,5,4,3,2,1,255},	// eye-beam (frm 9 is shoot)
				{1,3,5,6,6,6,6,255},	// die
				{0,255}, //unused
				{0,255}, //unused
				{0,255}, //unused
				{0,255}, //unused
				{0,255}, //unused
			}},
		{"Pinkeye",255,255,0,AI_Pinkeye, //good to go
		 11,13,20,1,"kmgraphics/pinkeye.jsp",0,MF_NOMOVE,
		 MT_EVIL|MT_THINGY,MG_MYSTIC,MC_BROWN,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,7,8,255},	// fire
				{9,10,11,12,12,11,10,9,9,255},	// die & resurrect (freezes on frm 3 until ready)
				{0,255},
				{1,2,3,4,5,6,7,8,255},	// unused
				{1,2,3,4,5,6,7,8,255},	// unused
				{1,2,3,4,5,6,7,8,255},	// unused
				{1,2,3,4,5,6,7,8,255},	// unused
			}},
		{"Microfish",255,255,0,AI_Microfish, //good to go
		 1,1,1,1,"kmgraphics/micro.jsp",0,MF_AQUATIC|MF_NOSHADOW|MF_WATERWALK,
		 MT_EVIL|MT_ANIMAL|MT_AQUATIC|MT_WACKY,MG_MYSTIC,MC_YELLOW,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack unused
				{0,255},	// die
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
			}},
		{"Grumpy Golem",255,255,0,AI_Golem, //good to go
		 32,26,100,0,"kmgraphics/golem.jsp",0,MF_NOMOVE|MF_FREEWALK,
		 MT_GOOD,MG_MYSTIC,MC_GREY,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,12,12,13,13,14,14,255},	// attack
				{15,16,17,18,19,20,21,21,21,254,21,254,21,254,21,255},	// die
				{22,23,24,25,255},	// form out of nothing (A1)
				{7,8,9,10,11,12,12,12,13,13,14,14,255},	// unused
				{7,8,9,10,11,12,12,12,13,13,14,14,255},	// unused
				{7,8,9,10,11,12,12,12,13,13,14,14,255},	// unused
				{7,8,9,10,11,12,12,12,13,13,14,14,255},	// unused
			}},
		{"Bobby Khan",255,255,0,AI_BobbyKhan, //good to go
		 48,46,1500,5000,"kmgraphics/bobby.jsp",0,MF_NOMOVE|MF_ONEFACE,
		 MT_EVIL|MT_BOSS,MG_MYSTIC,MC_VIOLET,
			{
				{0,0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,255},	// attack=sword slash
				{42,42,42,43,43,43,44,44,44,44,44,44,255},	// die
				{15,16,17,17,17,17,17,17,17,17,17,16,15,255},	//A1=block and reflect
				{18,19,19,19,20,21,22,23,24,25,26,27,28,28,29,29,255},			//A2=spin of doom
				{30,31,31,31,32,33,34,35,36,37,38,39,40,41,255},	// A3=stomp
				{45,45,45,17,17,255},	// A4=blocked a shot
				{7,8,9,10,11,12,13,14,255},	// unused attack
			}},
		{"Crazy Bush",255,255,0,AI_Crazybush, //good to go
		 10,8,20,5,"kmgraphics/crazybush.jsp",0,0,
		 MT_EVIL|MT_WACKY|MT_VEGGIE|MT_FOREST,MG_MYSTIC,MC_GREEN,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{0,255},	// attack
				{5,6,7,255},	// die
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
			}},
		{"Octon",255,255,0,AI_Octopus, //good to go
		 19,21,20,40,"kmgraphics/octopus.jsp",0,MF_AQUATIC|MF_WATERWALK,
		 MT_EVIL|MT_ZOID|MT_AQUATIC|MT_FOREST,MG_MYSTIC,MC_VIOLET,
			{
				{5,6,7,6,255},	// idle
				{5,6,6,7,6,6,255},	// move
				{16,17,18,19,20,19,18,17,18,19,20,19,18,17,18,19,20,19,18,17,16,255},	// attack=spit from arisen postion
				{8,9,10,11,12,13,14,15,255},	// die
				{5,4,3,2,1,0,0,255},	// A1=arise
				{1,2,3,4,5,5,255},	// A2=descend
				{16,17,18,19,20,19,18,17,16,255},	// unused
				{16,17,18,19,20,19,18,17,16,255},	// unused
				{16,17,18,19,20,19,18,17,16,255},	// unused
			}},
		{"Blockhead",255,255,0,AI_Incabot, //good to go
		 12,23,5,2,"kmgraphics/incabot.jsp",0,MF_NOMOVE,
		 MT_EVIL|MT_FOREST,MG_MYSTIC,MC_GREY,
			{
				{0,255},	// idle
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,255},	// attack=unused
				{16,17,18,19,20,21,22,255},	// die
				{0,255},
				{0,255},
				{0,255},
				{0,255},
				{0,255},
			}},
		{"Blockhead Kiln",255,255,0,AI_Incagen, //good to go
		 40,16,100,100,"kmgraphics/incagen.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_ENEMYWALK,
		 MT_EVIL|MT_FOREST|MT_GENERATE|MT_TRAP,MG_MYSTIC,MC_GREY,
			{
				{0,255},	// idle
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move=summon
				{0,255},	// attack=unused
				{0,0,0,255},	// die=explode
				{0,255},	// idle
				{0,255},	// idle
				{0,255},	// idle
				{0,255},	// idle
				{0,255},	// idle
			}},
		{"Megablockhead H",255,255,0,AI_Incagold, //good to go
		 11,16,2000,75,"kmgraphics/incagold.jsp",0,MF_NOMOVE,
		 MT_EVIL|MT_FOREST|MT_TRAP,MG_MYSTIC,MC_BROWN,
			{
				{0,255},	// idle
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,255},	// attack=unused
				{0,255},	// die=doesn't!
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
			}},
		{"Megablockhead V",255,255,0,AI_Incagold, //good to go
		 11,16,2000,75,"kmgraphics/incagold.jsp",0,MF_NOMOVE,
		 MT_EVIL|MT_FOREST,MG_MYSTIC,MC_BROWN,
			{
				{0,255},	// idle
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{0,255},	// attack=unused
				{0,255},	// die=doesn't!
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
			}},
		{"Log",255,255,0,AI_Log, //good to go
		 15,10,1,50,"kmgraphics/log.jsp",0,MF_WATERWALK|MF_AQUATIC|MF_FREEWALK|MF_NOHIT|MF_ONEFACE|MF_FACECMD|MF_ENEMYWALK,
		 MT_GOOD|MT_AQUATIC|MT_FOREST,MG_MYSTIC,MC_BROWN,
			{
				{0,1,2,3,4,5,6,7,8,9,255},	// idle
				{0,1,2,3,4,5,6,7,8,9,255},	// move
				{0,255},	// attack=unused
				{0,1,2,255},	// die
				{0,255},	// still
				{0,1,2,3,4,5,6,7,8,9,255},	// idle
				{0,1,2,3,4,5,6,7,8,9,255},	// idle
				{0,1,2,3,4,5,6,7,8,9,255},	// idle
				{0,1,2,3,4,5,6,7,8,9,255},	// idle
			}},
		{"Blocko the Great",255,255,0,AI_Incaboss, //fix lasers - DONE!
		 65,20,1,500,"kmgraphics/incaboss.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_ENEMYWALK|MF_INVINCIBLE,
		 MT_EVIL|MT_MINIBOSS|MT_FOREST,MG_MYSTIC,MC_GREY,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,5,5,5,5,5,5,5,5,5,5,5,4,3,2,1,255},	// attack=open mouth flames
				{16,16,17,17,18,18,19,19,19,19,255},	// die (loop the end forever to leave the corpse)
				{1,2,3,4,5,6,7,8,9,10,11,12,8,9,10,11,12,7,6,5,4,3,2,1,255}, // A1=tongue
				{13,13,13,14,14,14,15,15,15,15,15,15,15,15,15,15,15,15,14,13,255},	// A2=eyebeam
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
			}},
		{"Blocko the Great",255,255,0,AI_Incatongue, //make invisible? - DONE!
		 45,1,400,100,"kmgraphics/log.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_ENEMYWALK,
		 0,MG_MYSTIC,MC_GREY,
			{
				{254,255},	// idle
				{254,255},	// move
				{254,255},	// attack=unused
				{254,255},	// die
				{254,255},	// unused
				{254,255},	// unused
				{254,255},	// unused
				{254,255},	// unused
				{254,255},	// unused
			}},
		{"Peeping Bomb",255,255,0,AI_PeepBomb, //good to go
		 13,41,20,50,"kmgraphics/peepbomb.jsp",0,MF_FLYING|MF_ONEFACE|MF_FACECMD,
		 MT_EVIL|MT_THINGY|MT_TRAP,MG_MYSTIC,MC_GREY,
			{
				{0,0,0,0,255},	// idle
				{0,0,0,255},	// move
				{0,255},	// attack=rotate to next facing
				{32,33,34,35,36,37,38,39,40,255},	// die=explode
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
			}},
		{"Peeping Bomb",255,255,0,AI_PeepBomb, //good to go
		 13,41,20,50,"kmgraphics/peepbomb.jsp",0,MF_FLYING|MF_ONEFACE|MF_FACECMD,
		 MT_EVIL|MT_THINGY|MT_TRAP,MG_MYSTIC,MC_GREEN,
			{
				{0,0,0,0,255},	// idle
				{0,0,0,255},	// move
				{0,255},	// attack=rotate to next facing
				{32,33,34,35,36,37,38,39,40,255},	// die=explode
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
			}},
		{"Trapazoid",255,255,0,AI_Trapazoid, //good to go
		 14,10,30,70,"kmgraphics/trapazoid.jsp",0,MF_NOMOVE|MF_NOSHADOW,
		 MT_EVIL|MT_ZOID|MT_TRAP,MG_MYSTIC,MC_AQUA,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,7,8,9,9,255},	// attack=torch in a circle
				{6,5,4,3,2,1,0,255},	// die (turn into dud trapazoid)
				{0,255},	// hiding time!
				{1,2,3,4,5,6,7,8,9,9,255},	// unused
				{1,2,3,4,5,6,7,8,9,9,255},	// unused
				{1,2,3,4,5,6,7,8,9,9,255},	// unused
				{1,2,3,4,5,6,7,8,9,9,255},	// unused
			}},
		{"Trapazoid II",7,2,-2,AI_Trapazoid, //try something with the bullets
		 14,10,30,70,"kmgraphics/trapazoid.jsp",0,MF_NOMOVE|MF_NOSHADOW,
		 MT_EVIL|MT_ZOID|MT_TRAP,MG_MYSTIC,MC_BROWN,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,7,8,9,9,255},	// attack=torch in a circle
				{6,5,4,3,2,1,0,255},	// die (turn into dud trapazoid)
				{0,255},	// hiding time!
				{1,2,3,4,5,6,7,8,9,9,255},	// unused
				{1,2,3,4,5,6,7,8,9,9,255},	// unused
				{1,2,3,4,5,6,7,8,9,9,255},	// unused
				{1,2,3,4,5,6,7,8,9,9,255},	// unused
			}},
		{"Whackazoid",7,6,0,AI_Whackazoid, //check original
		 14,10,1,5,"kmgraphics/trapazoid.jsp",0,MF_NOMOVE|MF_NOSHADOW,
		 MT_EVIL|MT_TRAP,MG_MYSTIC,MC_VIOLET,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,7,7,7,7,7,7,7,7,7,7,6,5,4,3,2,1,255},	// attack=torch in a circle
				{6,5,4,3,2,1,0,255},	// die (turn into dud trapazoid)
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
			}},
		{"Slimy Slug",255,255,0,AI_Slug, //good to go
		 12,19,30,50,"kmgraphics/slug.jsp",0,0,
		 MT_EVIL|MT_ANIMAL|MT_CAVES,MG_MYSTIC,MC_AQUA,
			{
				{0,255},	// idle
				{0,1,2,3,4,3,2,1,255},	// move
				{1,2,3,255},	// attack=unused
				{5,6,7,8,9,10,11,12,13,14,15,16,17,18,255},	// die
				{4,2,3,255},	// unused
				{1,2,3,255},	// unused
				{1,2,3,255},	// unused
				{1,2,3,255},	// unused
				{1,2,3,255},	// unused
			}},
		{"Sniper Snail",255,255,0,AI_Snail, //just fix the animation
		 12,19,45,60,"kmgraphics/snail.jsp",0,0,
		 MT_EVIL|MT_ANIMAL|MT_CAVES,MG_MYSTIC,MC_GREEN,
			{
				{0,255},	// idle
				{0,1,2,3,4,3,2,1,255},	// move
				{4,5,6,5,4,3,2,1,255},	// attack=spit goop
				{5,6,7,8,9,10,11,12,13,14,15,16,17,18,255},	// die
				{5,6,7,8,9,10,11,12,13,14,14,14,255},	// A1=go into shell
				{13,12,11,10,9,8,7,6,5,5,255},	// A2=get out of shell
				{14,255},	// hiding in shell
				{4,5,6,5,4,3,2,1,255},	// unused
				{4,5,6,5,4,3,2,1,255},	// unused
			}},
		{"Smashball",255,255,0,AI_Ball, //good to go
		 14,1,40,500,"kmgraphics/ball.jsp",0,MF_INVINCIBLE|MF_ENEMYWALK|MF_NOMOVE|MF_ONEFACE|MF_WATERWALK|MF_FREEWALK,
		 MT_GOOD|MT_TRAP|MT_CAVES,MG_MYSTIC,MC_GREY,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,0,0,0,0,0,0,0,0,0,255},	// attack
				{0,255},	// die
				{0,0,0,0,0,0,0,0,0,0,255},	// attack
				{0,0,0,0,0,0,0,0,0,0,255},	// attack
				{0,0,0,0,0,0,0,0,0,0,255},	// attack
				{0,0,0,0,0,0,0,0,0,0,255},	// attack
				{0,0,0,0,0,0,0,0,0,0,255},	// attack
			}},
		{"Optimum Octon",255,255,0,AI_OctoBoss, //good to go
		 90,15,450,300,"kmgraphics/octoboss.jsp",0,MF_ONEFACE|MF_NOMOVE|MF_AQUATIC|MF_WATERWALK|MF_ENEMYWALK,
		 MT_EVIL|MT_ZOID|MT_BOSS|MT_AQUATIC|MT_FOREST,MG_MYSTIC,MC_VIOLET,
			{
				{0,255},	// idle
				{0,255},	// move
				{10,11,12,13,14,14,14,14,14,14,14,14,14,13,12,11,10,255},	// attack=spit
				{1,2,3,4,5,6,7,8,9,255},	// die
				{10,11,12,13,14,14,14,14,14,14,14,14,14,13,12,11,10,255},	// unused
				{10,11,12,13,14,14,14,14,14,14,14,14,14,13,12,11,10,255},	// unused
				{10,11,12,13,14,14,14,14,14,14,14,14,14,13,12,11,10,255},	// unused
				{10,11,12,13,14,14,14,14,14,14,14,14,14,13,12,11,10,255},	// unused
				{10,11,12,13,14,14,14,14,14,14,14,14,14,13,12,11,10,255},	// unused
			}},
		{"Octentacle",255,255,0,AI_OctoTentacle, //good to go
		 20,11,80,50,"kmgraphics/octotent.jsp",0,MF_NOMOVE|MF_AQUATIC|MF_WATERWALK|MF_ENEMYWALK,
		 MT_BITS,MG_MYSTIC,MC_VIOLET,
			{
				{1,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,7,8,9,10,9,8,7,6,5,4,3,2,1,255},	// attack=slap
				{6,5,4,3,2,1,0,255},	// die
				{1,2,3,4,5,6,7,8,9,10,9,8,7,6,5,4,3,2,1,255},	// unused
				{1,2,3,4,5,6,7,8,9,10,9,8,7,6,5,4,3,2,1,255},	// unused
				{1,2,3,4,5,6,7,8,9,10,9,8,7,6,5,4,3,2,1,255},	// unused
				{1,2,3,4,5,6,7,8,9,10,9,8,7,6,5,4,3,2,1,255},	// unused
				{1,2,3,4,5,6,7,8,9,10,9,8,7,6,5,4,3,2,1,255},	// unused
			}},
		{"Octentacle",255,255,0,AI_OctoTentacle, //good to go
		 20,11,80,50,"kmgraphics/octotent.jsp",0,MF_NOMOVE|MF_AQUATIC|MF_WATERWALK|MF_ENEMYWALK,
		 MT_BITS,MG_MYSTIC,MC_VIOLET,
			{
				{1,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,7,8,9,10,9,8,7,6,5,4,3,2,1,255},	// attack=slap
				{6,5,4,3,2,1,0,255},	// die
				{1,2,3,4,5,6,7,8,9,10,9,8,7,6,5,4,3,2,1,255},	// unused
				{1,2,3,4,5,6,7,8,9,10,9,8,7,6,5,4,3,2,1,255},	// unused
				{1,2,3,4,5,6,7,8,9,10,9,8,7,6,5,4,3,2,1,255},	// unused
				{1,2,3,4,5,6,7,8,9,10,9,8,7,6,5,4,3,2,1,255},	// unused
				{1,2,3,4,5,6,7,8,9,10,9,8,7,6,5,4,3,2,1,255},	// unused
			}},
		{"Rammy Gruff",255,255,0,AI_Rammy, //check ramming?
		 20,52,275,300,"kmgraphics/goat.jsp",0,MF_NOMOVE,
		 MT_EVIL|MT_BOSS|MT_ANIMAL|MT_FOREST,MG_MYSTIC,MC_GREY,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,8,9,10,11,12,255},	// move=charge
				{1,2,3,2,1,255},	// attack=shoot
				{32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,255},	// die
				{13,14,15,16,17,18,19,20,21,22,23,15,16,17,18,19,20,21,22,23,15,14,13,255},	// stunned
				{24,25,26,26,255},	//begin hop
				{27,28,29,29,29,29,29,29,29,30,31,32,255},	//squish onto the ground after a hop
				{0,255},	// none
				{0,255},	// unused
			}},
		{"Hoppy Gruff",255,255,-3,AI_Hoppy, //good to go
		 20,52,325,300,"kmgraphics/goat.jsp",0,MF_NOMOVE,
		 MT_EVIL|MT_BOSS|MT_ANIMAL|MT_FOREST,MG_MYSTIC,MC_GREY,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,8,9,10,11,12,255},	// move=charge
				{1,2,3,2,1,255},	// attack=shoot
				{32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,255},	// die
				{13,14,15,16,17,18,19,20,21,22,23,15,16,17,18,19,20,21,22,23,15,14,13,255},	// stunned
				{24,25,26,26,255},	//begin hop
				{27,28,29,29,29,29,29,29,29,30,31,32,255},	//squish onto the ground after a hop
				{0,255},	// none
				{0,255},	// unused
			}},
		{"Gruffy Gruff",255,255,3,AI_Gruffy, //good to go
		 20,52,400,300,"kmgraphics/goat.jsp",0,MF_NOMOVE,
		 MT_EVIL|MT_BOSS|MT_ANIMAL|MT_FOREST,MG_MYSTIC,MC_GREY,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,8,9,10,11,12,255},	// move=charge
				{1,2,3,2,1,255},	// attack=shoot
				{32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,255},	// die
				{13,14,15,16,17,18,19,20,21,22,23,15,16,17,18,19,20,21,22,23,15,14,13,255},	// stunned
				{24,25,26,26,255},	//begin hop
				{27,28,29,29,29,29,29,29,29,30,31,32,255},	//squish onto the ground after a hop
				{0,255},	// none
				{0,255},	// unused
			}},
		{"Happy Stick Shroom",255,255,0,AI_StickShroom, //good to go
		 9,6,15,50,"kmgraphics/stkshr.jsp",0,MF_ONEFACE,
		 MT_EVIL|MT_WACKY|MT_VEGGIE|MT_FOREST,MG_MYSTIC,MC_AQUA,
			{
				{0,1,255},	// idle
				{0,1,255},	// move=walk
				{2,2,2,2,2,2,255},	// attack=shoot
				{3,3,4,4,5,5,255},	// die
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
			}},
		{"Happy Stick Spider",255,255,0,AI_StickSpider, //good to go
		 9,6,25,70,"kmgraphics/stkspd.jsp",0,MF_ONEFACE,
		 MT_EVIL|MT_WACKY|MT_SPIDER|MT_ANIMAL|MT_FOREST,MG_MYSTIC,MC_AQUA,
			{
				{0,1,255},	// idle
				{0,1,255},	// move=walk
				{2,2,2,2,2,2,255},	// attack=shoot
				{3,4,3,4,3,4,254,3,254,4,254,3,254,254,4,254,254,3,255},	// die
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
			}},
		{"Happy Stick Corpse",255,255,0,AI_StickCorpse, //good to go
		 11,10,40,100,"kmgraphics/stkskl.jsp",0,MF_ONEFACE,
		 MT_EVIL|MT_WACKY|MT_SPOOKY|MT_CAVES,MG_MYSTIC,MC_AQUA,
			{
				{0,255},	// idle
				{0,1,0,2,255},	// move=walk
				{3,3,3,3,3,3,255},	// attack=shoot
				{4,5,6,7,8,9,255},	// die
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
			}},
		{"Happy Stick Bat",255,255,0,AI_StickBat, //good to go
		 11,6,20,60,"kmgraphics/stkbat.jsp",0,MF_ONEFACE|MF_FLYING,
		 MT_EVIL|MT_WACKY|MT_FLYING|MT_SPOOKY|MT_ANIMAL|MT_CAVES,MG_MYSTIC,MC_AQUA,
			{
				{0,1,255},	// idle
				{0,1,255},	// move=walk
				{2,2,2,2,2,2,255},	// attack=shoot
				{3,3,4,4,5,5,255},	// die
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
			}},
		{"Happy Stick Dancer",7,6,8,AI_Dancer, //good to go
		 11,10,400,5000,"kmgraphics/stkskl.jsp",0,MF_ONEFACE|MF_FREEWALK|MF_ENEMYWALK|MF_NOMOVE,
		 MT_EVIL|MT_WACKY|MT_MINIBOSS|MT_CAVES,MG_MYSTIC,MC_AQUA,
			{
				{0,255},	// idle
				{0,1,0,2,0,1,0,2,255},	// move=walk
				{3,3,3,3,255},	// attack=shoot
				{4,5,6,7,8,9,255},	// die
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
			}},
		//SLEEPLESS HOLLOW
		{"Creepy Crawly",1,0,-3,AI_BigSpider, //good to go
		 8,21,20,50,"graphics/bigspdr.jsp",0,0,
		 MT_EVIL|MT_SPIDER|MT_ANIMAL|MT_FOREST,MG_HOLLOW,MC_GREY,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,12,13,13,255},	// attack
				{14,15,16,17,18,17,18,17,18,19,19,20,20,255},	// die
				{7,8,9,10,11,12,12,13,13,255},	// attack
				{7,8,9,10,11,12,12,13,13,255},	// attack
				{7,8,9,10,11,12,12,13,13,255},	// attack
				{0,255},
				{0,255},
			}},
		{"Froggy",255,255,0,AI_Frog, //good to go
		 6,22,4,100,"graphics/frog.jsp",0,0,
		 MT_EVIL|MT_ANIMAL|MT_FOREST,MG_HOLLOW,MC_GREEN,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,8,0,255},	// move
				{9,10,11,12,13,14,15,16,255},	// attack
				{17,18,19,20,21,255},		// die
				{9,10,11,12,13,14,15,16,255},	// unused
				{9,10,11,12,13,14,15,16,255},	// unused
				{9,10,11,12,13,14,15,16,255},	// unused
				{9,10,11,12,13,14,15,16,255},	// unused
				{9,10,11,12,13,14,15,16,255},	// unused
			}},
		{"Flying Pumpkin",2,4,0,AI_PumpkinFly, //good to go
		 11,6,5,500,"!19",0,MF_FLYING,
		 MT_EVIL|MT_PUMPKIN|MT_FLYING|MT_FOREST,MG_HOLLOW,MC_RED,
			{
				{0,255},	// idle
				{1,2,3,4,5,0,255},	// move
				{1,2,3,4,5,0,255},	// move
				{0,0,255},	// die (just long enough to spew shrapnel)
				{1,2,3,4,5,0,255},	// move
				{1,2,3,4,5,0,255},	// move
				{1,2,3,4,5,0,255},	// move
				{1,2,3,4,5,0,255},	// move
				{1,2,3,4,5,0,255},	// move
			}},
		{"Dumble",1,6,0,AI_Whatsit, //good to go
		 12,44,20,50,"graphics/babything.jsp",0,0,
		 MT_EVIL|MT_THINGY,MG_HOLLOW,MC_VIOLET,
			{
				{12,255},	// idle
				{1,2,3,4,5,6,7,8,9,10,11,255},	// move
				{20,21,22,23,24,25,26,27,255},	// attack
				{34,35,36,37,38,39,40,41,42,43,255},	// die
				{13,14,15,16,17,16,15,16,17,16,15,16,17,16,15,255},	// fall asleep (A1)
				{18,19,18,255},	// blink (A2)
				{28,29,30,29,28,255}, // look right (A3)
				{31,32,33,32,31,255}, // look left (A4)
				{0,255},
			}},
		{"Glass Jaw",0,7,0,AI_Bonehead, //good to go - check glass effect
		 8,38,10,50,"!2",0,0,
		 MT_EVIL|MT_SPOOKY|MT_ARCTIC,MG_HOLLOW,MC_AQUA,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{20,21,22,23,24,255},	// attack
				{7,8,9,10,11,12,13,14,15,16,255},		// die
				{25,26,27,28,29,30,255},	// chomp/spit projectile
				{31,32,32,32,31,255},	// point at bouapha
				{16,15,14,13,12,11,10,9,8,7,255},	// resurrect=A3
				{0,255},
				{0,255},
			}},
		{"Toadie",1,2,0,AI_Frog, //good to go
		 6,22,8,100,"!249",0,0,
		 MT_EVIL|MT_ANIMAL|MT_FOREST,MG_HOLLOW,MC_BROWN,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,8,0,255},	// move
				{9,10,11,12,13,14,15,16,255},	// attack
				{17,18,19,20,21,255},		// die
			}},
		{"Firebug",1,4,0,AI_BigSpider, //good to go
		 8,21,10,50,"!5",0,0,
		 MT_EVIL|MT_SPIDER|MT_ANIMAL|MT_CAVES,MG_HOLLOW,MC_RED,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,12,13,13,255},	// attack
				{14,15,16,17,18,17,18,17,18,19,19,20,20,255},	// die
				{7,8,9,10,11,12,12,13,13,255},	// attack
				{7,8,9,10,11,12,12,13,13,255},	// attack
				{7,8,9,10,11,12,12,13,13,255},	// attack
				{0,255},
				{0,255},
			}},
		{"Firebat",2,4,0,AI_Bat, //good to go
		 8,27,12,25,"!3",0,MF_FLYING,
		 MT_SPOOKY|MT_FLYING|MT_EVIL|MT_ANIMAL|MT_CAVES,MG_HOLLOW,MC_RED,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,8,7,255},	// attack
				{17,18,19,20,21,22,23,24,25,26,255},		// die
				{10,11,12,12,12,12,12,11,10,255},	// diving attack
				{13,14,15,15,16,255},	// bounce off during dive
				{7,8,9,8,7,255},	// attack
				{0,255},
				{0,255},
			}},
		{"Happy Stick Tree",255,255,0,AI_Tree, //good to go
		 20,5,128,5000,"graphics/tree.jsp",0,MF_NOMOVE|MF_ONEFACE,
		 MT_EVIL|MT_WACKY|MT_MINIBOSS|MT_FOREST,MG_HOLLOW,MC_AQUA,
			{
				{0,255},	// idle
				{0,0,0,0,255},	// move
				{0,0,0,0,255},	// attack=snowball
				{1,2,3,4,254,4,254,4,254,4,255},	// die
				{1,1,255},	// A1=sad
				{0,0,0,0,255},	// unused
				{0,0,0,0,255},	// unused
				{0,0,0,0,255},	// unused
				{0,0,0,0,255},	// unused
			}},
		{"Funnybone",4,1,6,AI_Goodbone, //good to go
		 8,38,10,50,"!2",0,0,
		 MT_GOOD,MG_HOLLOW,MC_GREY,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{12,13,14,15,16,17,18,19,255},	// attack
				{7,8,9,10,11,11,11,11,254,11,254,11,254,11,254,11,254,11,254,11,254,11,255},		// die
				{20,21,22,23,24,25,255},	// chomp/spit projectile
				{26,27,27,27,26,255},	// point at bouapha
				{7,8,9,10,11,11,11,11,254,11,254,11,254,11,254,11,254,11,254,11,254,11,255},		// drown=A3
				{11,10,9,8,7,255},	// resurrect=A4
		}},
		{"Headless Horseradish",255,255,0,AI_Horseradish, //good to go - removed 10000 hp bs
		 11,10,1000,10000,"graphics/radish.jsp",0,MF_NOMOVE|MF_ENEMYWALK|MF_FREEWALK,
		 MT_EVIL|MT_VEGGIE|MT_BOSS|MT_PUMPKIN,MG_HOLLOW,MC_GREY,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{5,6,7,8,9,255},	// attack=shoot spines
				{0,0,0,0,255},	// die
				{5,6,7,8,9,255},	// unused
				{5,6,7,8,9,255},	// unused
				{5,6,7,8,9,255},	// unused
				{5,6,7,8,9,255},	// unused
				{5,6,7,8,9,255},	// unused
			}},
		{"Big Head Todd",4,5,0,AI_BigHead, //good to go
		 50,8,300,1500,"graphics/lich.jsp",0,MF_NOMOVE|MF_ONEFACE,
		 MT_EVIL|MT_SPOOKY|MT_MINIBOSS|MT_SKELETON,MG_HOLLOW,MC_GREY,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,4,3,2,1,255},	// attack=open mouth
				{0,5,6,7,8,255},	// die
				{1,2,3,4,4,3,2,1,255},	// attack=open mouth
				{0,255},
				{0,255},
				{0,255},
				{0,255},
			}},
		{"Big Head Rod",255,255,0,AI_BigHead2, //good to go
		 50,8,300,1500,"graphics/lich.jsp",0,MF_NOMOVE|MF_ONEFACE,
		 MT_EVIL|MT_SPOOKY|MT_MINIBOSS|MT_SKELETON,MG_HOLLOW,MC_GREY,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,4,3,2,1,255},	// attack=open mouth
				{0,5,6,7,8,255},	// die
				{1,2,3,4,4,3,2,1,255},	// attack=open mouth
				{0,255},
				{0,255},
				{0,255},
				{0,255},
			}},
		{"Big Head Maude",4,1,0,AI_BigHead, //good to go
		 50,8,300,1500,"graphics/lich.jsp",0,MF_NOMOVE|MF_ONEFACE,
		 MT_EVIL|MT_SPOOKY|MT_MINIBOSS|MT_SKELETON,MG_HOLLOW,MC_GREY,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,4,3,2,1,255},	// attack=open mouth
				{0,5,6,7,8,255},	// die
				{1,2,3,4,4,3,2,1,255},	// attack=open mouth
				{0,255},
				{0,255},
				{0,255},
				{0,255},
			}},
		{"Big Head Bod",255,255,0,AI_BigHeadBod, //good to go
		 8,32,10,50,"llgraphics/bonehead.jsp",0,MF_NOHIT|MF_ENEMYWALK|MF_FREEWALK|MF_NOMOVE,
		 MT_BITS,MG_HOLLOW,MC_GREY,
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
		{"Sir Spookley Hauntmore",255,255,0,AI_Spook, //good to go, just fix boss hp issue
		 30,9,300,1,"llgraphics/bigghost.jsp",0,MF_ONEFACE|MF_FLYING|MF_FREEWALK|MF_ENEMYWALK,
		 MT_EVIL|MT_BOSS|MT_SPOOKY|MT_FLYING,MG_HOLLOW,MC_GREY,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,2,1,255},	// attack
				{4,4,5,5,6,6,7,7,8,8,255},	// die
				{1,2,3,3,3,2,1,2,3,2,1,0,0,0,0,1,2,3,2,1,255},	// A1=sneeze
				{1,2,3,2,1,255},	// unused
				{1,2,3,2,1,255},	// unused
				{1,2,3,2,1,255},	// unused
				{1,2,3,2,1,255},	// unused
			}},
		{"Spookley:Inv",255,255,0,AI_Spook, //good to go, just fix boss hp issue
		 30,9,300,1,"llgraphics/bigghost.jsp",0,MF_ONEFACE|MF_FLYING|MF_GHOST|MF_FREEWALK|MF_ENEMYWALK|MF_NOSHADOW|MF_NOHIT,
		 MT_BITS,MG_HOLLOW,MC_GLOW,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,2,1,255},	// attack
				{4,4,5,5,6,6,7,7,8,8,255},	// die
				{1,2,3,3,3,2,1,2,3,2,1,0,0,0,0,1,2,3,2,1,255},	// A1=sneeze
				{1,2,3,2,1,255},	// unused
				{1,2,3,2,1,255},	// unused
				{1,2,3,2,1,255},	// unused
				{1,2,3,2,1,255},	// unused
			}},
		{"Spark",0,7,7,AI_Spark, //good to go
		 11,2,6,300,"!66",0,MF_GHOST|MF_ONEFACE|MF_NOHIT|MF_FREEWALK|MF_ENEMYWALK|MF_INVINCIBLE|MF_NOMOVE|MF_WALLWALK|MF_FLYING|MF_WATERWALK,
		 MT_EVIL|MT_TRAP,MG_HOLLOW,MC_AQUA,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{0,255},	// die
			}},
		{"Sparky",3,7,2,AI_Sparky, //good to go
		 12,55,100,100,"graphics/wolfman.jsp",0,MF_FREEWALK|MF_ENEMYWALK|MF_NOMOVE,
		 MT_EVIL|MT_MINIBOSS|MT_ANIMAL,MG_HOLLOW,MC_BROWN,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{30,31,32,32,32,33,34,35,36,37,38,39,40,41,42,43,255},	// attack (pounce)
				{44,45,46,47,48,49,50,51,51,51,52,53,54,254,54,254,54,254,54,255},		// die
				{7,8,9,9,8,7,255},	// spit projectile = A1
				{10,11,12,12,12,13,14,15,16,17,17,18,19,255},	// charging slash = A2
				{20,21,22,23,24,25,26,26,26,26,26,26,26,26,27,28,29,255},		// howl = A3
				{30,31,32,32,32,33,34,35,36,37,38,39,40,41,42,43,255},	// attack (pounce)
				{30,31,32,32,32,33,34,35,36,37,38,39,40,41,42,43,255},	// attack (pounce)
			}},
		{"Megaburner",255,255,0,AI_Burner, //good to go
		 18,2,30,300,"!66",0,MF_NOSHADOW|MF_ONEFACE|MF_NOMOVE|MF_INVINCIBLE,
		 MT_EVIL|MT_TRAP,MG_HOLLOW,MC_GREY,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{0,255},		// die
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
			}},
		{"Iron Skull",4,0,-8,AI_IronSkull, //fix attacks?
		 50,8,900,1500,"!44",0,MF_NOMOVE|MF_ONEFACE|MF_FLYING|MF_ENEMYWALK,
		 MT_EVIL|MT_SPOOKY|MT_BOSS|MT_SKELETON,MG_HOLLOW,MC_GREY,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,4,3,2,1,255},	// attack=open mouth
				{0,5,6,7,8,255},	// die
				{1,2,3,4,4,3,2,1,255},	// attack=open mouth
				{0,255},
				{0,255},
				{0,255},
				{0,255},
			}},
		{"Lightswitch",255,255,0,AI_LightSwitch, //what does it do?
		 10,8,3000,1,"llgraphics/lghtball.jsp",0,MF_NOMOVE|MF_ONEFACE,
		 MT_EVIL|MT_TRAP,MG_HOLLOW,MC_GREY,
			{
				{7,255},	// idle
				{7,255},	// move
				{7,6,5,4,3,2,1,0,0,0,0,255},	// attack (shove)
				{0,255},	// die
			}},
		{"Unbreakable Glass",255,255,0,AI_Knight, //make it invincible
		 50,47,5000,1200,"graphics/knight.jsp",0,MF_GLOW|MF_ONEFACE|MF_NOMOVE,
		 MT_EVIL|MT_MINIBOSS,MG_HOLLOW,MC_GLOW,
			{
				{14,255},	// idle
				{15,16,17,18,19,20,21,22,14,255},	// move
				{26,27,28,29,30,31,32,33,255},	// attack
				{34,35,36,37,38,39,40,41,42,43,44,45,46,255},	// die
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,255}, // A1 = arrive
				{23,24,25,25,25,25,25,25,25,24,23,255}, // A2 = block
				{26,27,28,29,30,31,32,33,255},	// attack
				{26,27,28,29,30,31,32,33,255},	// attack
				{26,27,28,29,30,31,32,33,255},	// attack
			}},
		{"Mad Cow",255,255,0,AI_MadCow, //good to go
		 22,15,20,250,"graphics/cow.jsp",0,MF_NOMOVE,
		 MT_EVIL|MT_ANIMAL|MT_FOREST,MG_HOLLOW,MC_BROWN,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,255},	// move
				{5,5,6,6,6,5,5,255},	// attack
				{7,8,9,10,11,12,13,14,255},	// die
				{5,5,6,6,6,5,5,255},	// attack
			}},
		{"Extremely Mad Cow",2,4,2,AI_MadCow2, //good to go
		 32,15,1000,250,"graphics/cow.jsp",0,MF_NOMOVE,
		 MT_EVIL|MT_ANIMAL|MT_FOREST,MG_HOLLOW,MC_RED,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,255},	// move
				{5,5,6,6,6,5,5,255},	// attack
				{7,8,9,10,11,12,13,14,255},	// die
				{5,5,6,6,6,5,5,255},	// attack
			}},
		{"Lady Von Spatula",6,3,-3,AI_Spatula, //good to go
		 25,28,300,1000,"!73",0,MF_ONEFACE|MF_NOMOVE,
		 MT_EVIL|MT_BOSS|MT_SPOOKY,MG_HOLLOW,MC_VIOLET,
			{
				{0,1,2,3,4,5,6,7,8,9,255},	// idle
				{0,1,2,3,4,5,6,7,8,9,255},	// move
				{13,14,15,16,17,18,18,18,18,18,19,255},	// attack
				{20,21,22,22,22,22,23,24,25,26,27,255},	// die
				{10,11,12,12,12,12,11,10,255},	// A1=summon boneheads
				{3,3,3,255},	// A2=charge up to rush
				{13,14,15,16,17,18,18,18,18,18,19,255},	// attack
				{13,14,15,16,17,18,18,18,18,18,19,255},	// attack
				{13,14,15,16,17,18,18,18,18,18,19,255},	// attack
			}},
		{"Royal Guard",2,6,-4,AI_BatGuard, //good to go
		 8,27,50,25,"!3",0,MF_FLYING|MF_NOMOVE|MF_ENEMYWALK|MF_FREEWALK|MF_WALLWALK,
		 MT_BITS,MG_HOLLOW,MC_VIOLET,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,8,7,255},	// attack
				{17,18,19,20,21,22,23,24,25,26,255},		// die
				{10,11,12,12,12,12,12,11,10,255},	// diving attack
				{13,14,15,15,16,255},	// bounce off during dive
				{7,8,9,8,7,255},	// attack
				{0,255},
				{0,255},
			}},
		{"Laser",0,4,-2,AI_Turret, //good to go
		 10,1,80,200,"graphics/turret.jsp",0,MF_NOMOVE,
		 MT_EVIL|MT_TRAP|MT_HIGHTECH,MG_HOLLOW,MC_RED,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{0,0,0,255},	// die
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
			}},
		{"Laser",0,4,-2,AI_Turret, //good to go
		 10,1,80,200,"graphics/turret.jsp",0,MF_NOMOVE,
		 MT_EVIL|MT_TRAP|MT_HIGHTECH,MG_HOLLOW,MC_RED,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{0,0,0,255},	// die
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
			}},
		{"Laser",0,4,-2,AI_Turret, //good to go
		 10,1,80,200,"graphics/turret.jsp",0,MF_NOMOVE,
		 MT_EVIL|MT_TRAP|MT_HIGHTECH,MG_HOLLOW,MC_RED,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{0,0,0,255},	// die
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
			}},
		{"Laser",0,4,-2,AI_Turret, //good to go
		 10,1,80,200,"graphics/turret.jsp",0,MF_NOMOVE,
		 MT_EVIL|MT_TRAP|MT_HIGHTECH,MG_HOLLOW,MC_RED,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{0,0,0,255},	// die
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
			}},
		{"Reflection",255,255,0,AI_Reflection, //good to go
		 4,29,128,0,"!1",0,MF_NOMOVE|MF_INVINCIBLE|MF_WATERWALK,
		 MT_GOOD|MT_TRAP,MG_HOLLOW,MC_GREEN,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,15,16,255},	// attack
				{0,0,0,0,0,0,0,26,27,28,255},		// die
				{23,24,25,24,23,255},	// use item
				{17,18,19,20,21,22,21,20,21,22,21,20,19,18,17,255},	// bored
				{1,3,1,0,4,6,4,0,1,3,1,0,0,0,26,27,28,255},	// watery death
				{7,8,9,10,11,12,13,14,15,16,255},	// attack
				{7,8,9,10,11,12,13,14,15,16,255},	// attack
			}},
		{"The Darkness",255,255,-7,AI_Darkness, //good to go
		 11,29,32,300,"!1",0,MF_WATERWALK|MF_FREEWALK,
		 MT_EVIL|MT_TRAP|MT_SPOOKY,MG_HOLLOW,MC_GLOW,
			{
				{254,255},	// idle
				{254,255},	// move
				{254,255},	// attack
				{254,255},	// die
				{254,255},	// unused
				{254,255},	// unused
				{254,255},	// unused
				{254,255},	// unused
				{254,255},	// unused
			}},
		{"Scaredy Bat",2,5,0,AI_ScaredyBat, //good to go
		 8,27,20,25,"graphics/bat.jsp",0,MF_FLYING,
		 MT_FLYING|MT_ANIMAL|MT_GOOD|MT_WACKY|MT_SPOOKY,MG_HOLLOW,MC_YELLOW,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,8,7,255},	// attack
				{17,18,19,20,21,22,23,24,25,26,255},		// die
				{10,11,12,12,12,12,12,11,10,255},	// diving attack
				{13,14,15,15,16,255},	// bounce off during dive
				{7,8,9,8,7,255},	// attack
				{0,255},
				{0,255},
			}},
		{"Blaster",5,4,0,AI_LightBlaster, //good to go
		 10,8,100,1,"llgraphics/lghtball.jsp",0,MF_NOMOVE|MF_ONEFACE,
		 MT_EVIL|MT_TRAP,MG_HOLLOW,MC_RED,
			{
				{7,255},	// idle
				{7,255},	// move
				{7,6,5,4,3,2,1,0,255},	// attack
				{0,255},	// die
			}},
		{"Speedy Hernandez",3,5,0,AI_Speedy, //good to go
		 12,55,350,100,"graphics/wolfman.jsp",0,MF_FREEWALK|MF_ENEMYWALK|MF_NOMOVE,
		 MT_EVIL|MT_BOSS|MT_ANIMAL,MG_HOLLOW,MC_BROWN,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{30,31,32,32,32,33,34,35,36,37,38,39,40,41,42,43,255},	// attack (pounce)
				{44,45,46,47,48,49,50,51,51,51,52,53,54,254,54,254,54,254,54,255},		// die
				{7,8,9,9,8,7,255},	// spit projectile = A1
				{10,11,12,12,12,13,14,15,16,17,17,18,19,255},	// charging slash = A2
				{20,21,22,23,24,25,26,26,26,26,26,26,26,26,27,28,29,255},		// howl = A3
				{30,31,32,32,32,33,34,35,36,37,38,39,40,41,42,43,255},	// attack (pounce)
				{30,31,32,32,32,33,34,35,36,37,38,39,40,41,42,43,255},	// attack (pounce)
			}},
		{"The Grue",6,0,-13,AI_Grue, //good to go
		 30,30,500,1500,"graphics/mamaspdr.jsp",0,MF_NOMOVE|MF_ENEMYWALK,
		 MT_EVIL|MT_BOSS|MT_SPIDER,MG_HOLLOW,MC_GREY,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,12,13,13,255},	// spit (attack)
				{19,20,21,22,23,24,25,25,25,26,27,28,29,255},	// die
				{14,15,16,17,18,255},	// bite (A1)
				{7,8,9,10,11,12,12,13,13,255},	// spit (attack)
				{7,8,9,10,11,12,12,13,13,255},	// spit (attack)
				{0,255},
				{0,255},
			}},
		{"Marsh Marrow",0,1,-4,AI_Bonehead, //good to go
		 8,38,40,50,"!2",0,0,
		 MT_SPOOKY|MT_EVIL|MT_SKELETON,MG_HOLLOW,MC_GREEN,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{20,21,22,23,24,255},	// attack
				{7,8,9,10,11,12,13,14,15,16,255},		// die
				{25,26,27,28,29,30,255},	// chomp/spit projectile
				{31,32,32,32,31,255},	// point at bouapha
				{16,15,14,13,12,11,10,9,8,7,255},	// resurrect=A3
				{0,255},
				{0,255},
			}},
		{"Toad Hole",1,2,6,AI_Patch, //good to go
		 16,17,50,1000,"graphics/patch.jsp",0,MF_NOMOVE|MF_ENEMYWALK|MF_ONEFACE|MF_NOSHADOW,
		 MT_EVIL|MT_TRAP|MT_GENERATE|MT_FOREST,MG_HOLLOW,MC_BROWN,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,0,2,0,1,2,1,0,1,0,2,1,2,1,0,2,1,2,255},	// attack
				{11,12,13,14,15,16,255},	// die
				{3,4,5,6,7,8,9,10,255},	// A1=summon
				{1,2,0,2,0,1,2,1,0,1,0,2,1,2,1,0,2,1,2,255},	// attack
				{1,2,0,2,0,1,2,1,0,1,0,2,1,2,1,0,2,1,2,255},	// attack
				{1,2,0,2,0,1,2,1,0,1,0,2,1,2,1,0,2,1,2,255},	// attack
				{1,2,0,2,0,1,2,1,0,1,0,2,1,2,1,0,2,1,2,255},	// attack
			}},
		{"Beetle Nest",1,5,6,AI_Patch, //good to go
		 16,17,100,1000,"graphics/patch.jsp",0,MF_NOMOVE|MF_ENEMYWALK|MF_ONEFACE|MF_NOSHADOW,
		 MT_EVIL|MT_TRAP|MT_GENERATE|MT_FOREST,MG_HOLLOW,MC_YELLOW,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,0,2,0,1,2,1,0,1,0,2,1,2,1,0,2,1,2,255},	// attack
				{11,12,13,14,15,16,255},	// die
				{3,4,5,6,7,8,9,10,255},	// A1=summon
				{1,2,0,2,0,1,2,1,0,1,0,2,1,2,1,0,2,1,2,255},	// attack
				{1,2,0,2,0,1,2,1,0,1,0,2,1,2,1,0,2,1,2,255},	// attack
				{1,2,0,2,0,1,2,1,0,1,0,2,1,2,1,0,2,1,2,255},	// attack
				{1,2,0,2,0,1,2,1,0,1,0,2,1,2,1,0,2,1,2,255},	// attack
			}},
		{"Simulated Head",255,255,0,AI_PumpkinFly, //good to go
		 11,6,25,250,"!19",0,MF_FLYING,MG_HOLLOW,MC_BROWN,
		 MT_BITS,
			{
				{0,255},	// idle
				{1,2,3,4,5,0,255},	// move
				{1,2,3,4,5,0,255},	// move
				{0,0,255},	// die (just long enough to spew shrapnel)
				{1,2,3,4,5,0,255},	// move
				{1,2,3,4,5,0,255},	// move
				{1,2,3,4,5,0,255},	// move
				{1,2,3,4,5,0,255},	// move
				{1,2,3,4,5,0,255},	// move
			}},
		//LOONYLAND
		{"Redbone",0,4,-6,AI_Bonehead, //give a funny
		 8,38,10,50,"!2",0,0,
		 MT_SPOOKY|MT_EVIL|MT_SKELETON,MG_LOONY,MC_RED,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{20,21,22,23,24,255},	// attack
				{7,8,9,10,11,12,13,14,15,16,255},		// die
				{25,26,27,28,29,30,255},	// chomp/spit projectile
				{31,32,32,32,31,255},	// point at bouapha
				{16,15,14,13,12,11,10,9,8,7,255},	// resurrect=A3
				{0,255},
				{0,255},
			}},
		{"Gangrene",0,1,-4,AI_Bonehead, //good to go
		 8,38,10,50,"!2",0,0,
		 MT_SPOOKY|MT_EVIL|MT_SKELETON,MG_LOONY,MC_GREEN,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{20,21,22,23,24,255},	// attack
				{7,8,9,10,11,12,13,14,15,16,255},		// die
				{25,26,27,28,29,30,255},	// chomp/spit projectile
				{31,32,32,32,31,255},	// point at bouapha
				{16,15,14,13,12,11,10,9,8,7,255},	// resurrect=A3
				{0,255},
				{0,255},
			}},
		{"Numbskull",0,5,-5,AI_Bonehead, //good to go
		 8,38,10,50,"!2",0,0,
		 MT_SPOOKY|MT_EVIL|MT_SKELETON,MG_LOONY,MC_YELLOW,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{20,21,22,23,24,255},	// attack
				{7,8,9,10,11,12,13,14,15,16,255},		// die
				{25,26,27,28,29,30,255},	// chomp/spit projectile
				{31,32,32,32,31,255},	// point at bouapha
				{16,15,14,13,12,11,10,9,8,7,255},	// resurrect=A3
				{31,32,32,32,31,255},	// point at bouapha
				{0,255},
			}},
		{"Bombie",0,4,0,AI_Zombie, //good to go
		 11,32,10,150,"graphics/zombie.jsp",0,0,
		 MT_SPOOKY|MT_ZOMBIE|MT_EVIL,MG_LOONY,MC_RED,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,9,10,11,12,13,13,14,15,16,17,18,19,255},	// attack
				{20,21,22,23,24,25,26,27,28,29,30,31,255},	// die
				{7,8,9,9,10,11,12,13,13,14,15,16,17,18,19,255},	// attack
				{7,8,9,9,10,11,12,13,13,14,15,16,17,18,19,255},	// attack
				{7,8,9,9,10,11,12,13,13,14,15,16,17,18,19,255},	// attack
				{0,255},
				{0,255},
			}},
		{"Zombie Lord",0,6,2,AI_Zombie, //fix speed!!
		 11,32,80,150,"graphics/zombie.jsp",0,0,
		 MT_SPOOKY|MT_ZOMBIE|MT_EVIL|MT_MINIBOSS,MG_LOONY,MC_VIOLET,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,9,10,11,12,13,13,14,15,16,17,18,19,255},	// attack
				{20,21,22,23,24,25,26,27,28,29,30,31,255},	// die
				{7,8,9,9,10,11,12,13,13,14,15,16,17,18,19,255},	// attack
				{7,8,9,9,10,11,12,13,13,14,15,16,17,18,19,255},	// attack
				{7,8,9,9,10,11,12,13,13,14,15,16,17,18,19,255},	// attack
				{0,255},
				{0,255},
			}},
		{"Villager",255,255,0,AI_Villager,
		 11,14,100,0,"llgraphics/villager.jsp",0,0,
		 MT_GOOD|MT_HUMAN,MG_LOONY,MC_RED,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{0,255},	// attack
				{0,255},		// die
				{7,8,9,10,11,12,13,12,11,10,11,12,13,12,11,10,9,8,7,255},	// A1 = scratch head
				{7,8,9,10,11,12,13,12,11,10,11,12,13,12,11,10,9,8,7,255},	// unused
				{7,8,9,10,11,12,13,12,11,10,11,12,13,12,11,10,9,8,7,255},	// unused
				{7,8,9,10,11,12,13,12,11,10,11,12,13,12,11,10,9,8,7,255},	// unused
				{7,8,9,10,11,12,13,12,11,10,11,12,13,12,11,10,9,8,7,255},	// unused
			}},
		{"Villager",255,255,0,AI_Villager,
		 11,12,100,0,"llgraphics/villager2.jsp",0,0,
		 MT_GOOD|MT_HUMAN,MG_LOONY,MC_RED,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{0,255},	// attack
				{0,255},		// die
				{7,8,9,10,11,12,13,12,11,10,11,12,13,12,11,10,9,8,7,255},	// A1 = scratch head
				{7,8,9,10,11,12,13,12,11,10,11,12,13,12,11,10,9,8,7,255},	// unused
				{7,8,9,10,11,12,13,12,11,10,11,12,13,12,11,10,9,8,7,255},	// unused
				{7,8,9,10,11,12,13,12,11,10,11,12,13,12,11,10,9,8,7,255},	// unused
				{7,8,9,10,11,12,13,12,11,10,11,12,13,12,11,10,9,8,7,255},	// unused
			}},
		{"Croaker",1,6,-3,AI_Frog, //fix attack
		 6,22,4,100,"graphics/frog.jsp",0,0,
		 MT_EVIL|MT_ANIMAL|MT_FOREST,MG_LOONY,MC_VIOLET,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,8,0,255},	// move
				{9,10,11,12,13,14,15,16,255},	// attack
				{17,18,19,20,21,255},		// die
				{9,10,11,12,13,14,15,16,255},	// unused
				{9,10,11,12,13,14,15,16,255},	// unused
				{9,10,11,12,13,14,15,16,255},	// unused
				{9,10,11,12,13,14,15,16,255},	// unused
				{9,10,11,12,13,14,15,16,255},	// unused
			}},
		{"Rag Man",0,0,0,AI_Mummy, //good to go
		 15,28,60,300,"llgraphics/mummy.jsp",0,MF_NOMOVE,
		 MT_EVIL|MT_SPOOKY|MT_DESERT,MG_LOONY,MC_GREY,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,255},	// attack
				{19,20,21,22,23,24,25,26,27,255},		// die
				{15,16,17,18,18,17,16,15,255},	// A1 = gem zap
				{7,8,9,9,9,9,9,8,7,255},	// A2 = magic hands
				{27,26,25,24,23,22,21,20,19,255},	// A3 = resurrect
				{7,8,9,10,11,12,13,14,255},	// attack 4
				{7,8,9,10,11,12,13,14,255},	// attack 5
			}},
		{"Pharaoh Faucet",6,5,2,AI_Mummy, //good to go
		 15,28,60,300,"llgraphics/mummy.jsp",0,MF_NOMOVE,
		 MT_EVIL|MT_SPOOKY|MT_DESERT,MG_LOONY,MC_GREY,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,255},	// attack
				{19,20,21,22,23,24,25,26,27,255},		// die
				{15,16,17,18,18,17,16,15,255},	// A1 = gem zap
				{7,8,9,9,9,9,9,8,7,255},	// A2 = magic hands
				{27,26,25,24,23,22,21,20,19,255},	// A3 = resurrect
				{7,8,9,10,11,12,13,14,255},	// attack 4
				{7,8,9,10,11,12,13,14,255},	// attack 5
			}},
		{"Grand Pharaoh",0,5,2,AI_Mummy, //good to go
		 15,28,150,500,"llgraphics/mummy.jsp",0,MF_NOMOVE,
		 MT_EVIL|MT_SPOOKY|MT_DESERT|MT_MINIBOSS,MG_LOONY,MC_YELLOW,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,255},	// attack
				{19,20,21,22,23,24,25,26,27,255},		// die
				{15,16,17,18,18,17,16,15,255},	// A1 = gem zap
				{7,8,9,9,9,9,9,8,7,255},	// A2 = magic hands
				{27,26,25,24,23,22,21,20,19,255},	// A3 = resurrect
				{7,8,9,10,11,12,13,14,255},	// attack 4
				{7,8,9,10,11,12,13,14,255},	// attack 5
			}},
		{"Terror Tree",0,0,0,AI_EvilTree, //fix steam
		 32,24,80,1,"llgraphics/eviltree.jsp",0,MF_NOMOVE|MF_ONEFACE,
		 MT_EVIL|MT_SPOOKY|MT_VEGGIE|MT_FOREST,MG_LOONY,MC_BROWN,
			{
				{4,5,6,7,8,9,10,11,12,13,255},	// idle
				{0,255},	// move
				{0,1,2,3,1,255},	// attack
				{14,15,16,17,18,19,20,21,22,23,255},		// die
				{0,1,2,3,1,255},	// unused
				{0,1,2,3,1,255},	// unused
				{0,1,2,3,1,255},	// unused
				{0,1,2,3,1,255},	// unused
				{0,1,2,3,1,255},	// unused
			}},
		{"Swamp Stump",4,1,0,AI_EvilTree, //fix bullet attack
		 32,24,80,1,"llgraphics/eviltree.jsp",0,MF_NOMOVE|MF_ONEFACE,
		 MT_EVIL|MT_SPOOKY|MT_VEGGIE|MT_FOREST,MG_LOONY,MC_BROWN,
			{
				{4,5,6,7,8,9,10,11,12,13,255},	// idle
				{0,255},	// move
				{0,1,2,3,1,255},	// attack
				{14,15,16,17,18,19,20,21,22,23,255},		// die
				{0,1,2,3,1,255},	// unused
				{0,1,2,3,1,255},	// unused
				{0,1,2,3,1,255},	// unused
				{0,1,2,3,1,255},	// unused
				{0,1,2,3,1,255},	// unused
			}},
		{"Dingbat",255,255,0,AI_Bat2, //compare attacks to source
		 8,33,25,25,"llgraphics/bat.jsp",0,MF_FLYING,
		 MT_SPOOKY|MT_FLYING|MT_EVIL|MT_ANIMAL,MG_LOONY,MC_BROWN, //work on!
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{11,12,13,12,11,255},	// attack
				{23,24,25,26,27,28,29,30,31,32,255},		// die
				{14,15,16,16,16,16,15,14,255},	// diving attack =A1
				{17,18,19,20,21,22,255},	// bounce off during dive =A2
				{7,8,9,10,9,8,7,255},		// shoot = A3
				{0,255},
				{0,255},
			}},
		{"Gargoyle",2,0,0,AI_Bat2, //compare attacks to source
		 8,33,50,25,"llgraphics/bat.jsp",0,MF_FLYING,
		 MT_SPOOKY|MT_FLYING|MT_EVIL|MT_ANIMAL,MG_LOONY,MC_GREY,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,8,7,255},	// attack
				{17,18,19,20,21,22,23,24,25,26,255},		// die
				{10,11,12,12,12,12,12,11,10,255},	// diving attack
				{13,14,15,15,16,255},	// bounce off during dive
				{7,8,9,8,7,255},	// attack
				{0,255},
				{0,255},
			}},
		{"Vamp Bat",2,4,-6,AI_Bat2, //compare attacks to source
		 8,33,40,25,"llgraphics/bat.jsp",0,MF_FLYING,
		 MT_SPOOKY|MT_FLYING|MT_EVIL|MT_ANIMAL,MG_LOONY,MC_RED,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,8,7,255},	// attack
				{17,18,19,20,21,22,23,24,25,26,255},		// die
				{10,11,12,12,12,12,12,11,10,255},	// diving attack
				{13,14,15,15,16,255},	// bounce off during dive
				{7,8,9,8,7,255},	// attack
				{0,255},
				{0,255},
			}},
		{"Dire Wolf",2,0,0,AI_Werewolf, //good to go
		 12,55,70,100,"graphics/wolfman.jsp",0,0,
		 MT_EVIL|MT_SPOOKY|MT_ANIMAL,MG_LOONY,MC_GREY,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{30,31,32,32,32,33,34,35,36,37,38,39,40,41,42,43,255},	// attack (pounce)
				{44,45,46,47,48,49,50,51,51,51,52,53,54,254,54,254,54,254,54,255},		// die
				{7,8,9,9,8,7,255},	// spit projectile = A1
				{10,11,12,12,12,13,14,15,16,17,17,18,19,255},	// charging slash = A2
				{20,21,22,23,24,25,26,26,26,26,26,26,26,26,27,28,29,255},		// howl = A3
				{30,31,32,32,32,33,34,35,36,37,38,39,40,41,42,43,255},	// attack (pounce)
				{30,31,32,32,32,33,34,35,36,37,38,39,40,41,42,43,255},	// attack (pounce)
			}},
		{"Larry",0,0,0,AI_Larry, //good to go
		 25,38,300,1000,"llgraphics/larry.jsp",0,0,
		 MT_EVIL|MT_SPOOKY|MT_ANIMAL|MT_BOSS,MG_LOONY,MC_YELLOW,
			{
				{0,255},	// idle
				{1,2,3,3,2,1,0,4,5,6,6,5,4,0,255},	// move
				{25,26,27,27,28,29,30,31,32,33,33,33,34,35,36,37,255},	// attack (pounce)
				{0,0,0,0,0,255},		// die
				{7,8,9,9,10,11,12,13,14,15,16,17,255},	// fire darts=A1
				{18,19,20,21,22,23,23,23,23,24,24,255},		// howl = A2
				{0,255},	// blank = A3
				{0,255},	// blank = A4
				{0,255},	// blank = A5
			}},
		{"Swampdog",0,0,0,AI_Swampdog, //good to go
		 18,43,20,75,"llgraphics/swampdog.jsp",0,0,
		 MT_EVIL|MT_SPOOKY|MT_THINGY,MG_LOONY,MC_YELLOW,
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
		{"Bog Beast",5,1,-3,AI_Swampdog, //good to go
		 18,43,40,125,"llgraphics/swampdog.jsp",0,0,
		 MT_EVIL|MT_SPOOKY|MT_THINGY,MG_LOONY,MC_GREEN,
			{
				{11,255},	// idle
				{12,13,14,13,12,11,15,16,17,16,15,255},	// move
				{20,21,22,23,23,22,22,21,20,255},	// attack (eye jab)
				{33,34,35,36,37,38,39,40,41,42,255},		// die
				{18,19,19,19,255},	// perk up = A1
				{1,2,3,4,5,6,7,8,9,10,0,255},	// charge = A2
				{24,25,26,27,28,29,30,31,32,255},		// eyebonk/shoot = A3
				{19,18,18,255},	// get bored = A4
				{0,0,0,0,255},	// hyper idle = A5
			}},
		{"Shambling Thing",5,3,-3,AI_Swampdog, //good to go
		 18,43,70,175,"llgraphics/swampdog.jsp",0,0,
		 MT_EVIL|MT_SPOOKY|MT_THINGY,MG_LOONY,MC_BLUE,
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
		{"Shade",255,255,-12,AI_LLGhost, //good to go
		 14,23,10,50,"llgraphics/ghost.jsp",0,MF_WALLWALK|MF_NOSHADOW|MF_FLYING|MF_GLOW|MF_ENEMYWALK|MF_FREEWALK,
		 MT_EVIL|MT_SPOOKY|MT_FLYING,MG_LOONY,MC_GLOW,
			{
				{0,255},	// idle
				{0,0,255},	// move
				{4,5,6,12,13,14,15,16,17,255},	// attack (quick punch)
				{18,19,20,21,22,255},		// die
				{18,19,20,21,22,21,20,19,18,255},	// teleport = A1
				{4,5,6,7,8,9,10,11,6,7,8,9,10,11,6,7,8,9,10,11,12,13,14,14,14,15,15,16,17,255},	// super punch = A2
				{1,2,3,3,2,3,2,3,3,3,2,2,1,1,255},		// scream = A3
				{4,5,6,7,8,9,10,11,6,7,8,9,10,11,6,7,8,9,10,11,12,13,14,14,14,15,15,16,17,255},	//unused
				{4,5,6,7,8,9,10,11,6,7,8,9,10,11,6,7,8,9,10,11,12,13,14,14,14,15,15,16,17,255},	//unused
			}},
		{"Banshee",255,255,10,AI_LLGhost, //good to go
		 14,23,10,50,"llgraphics/ghost.jsp",0,MF_WALLWALK|MF_NOSHADOW|MF_FLYING|MF_GLOW|MF_ENEMYWALK|MF_FREEWALK,
		 MT_EVIL|MT_SPOOKY|MT_FLYING,MG_LOONY,MC_GLOW,
			{
				{0,255},	// idle
				{0,0,255},	// move
				{4,5,6,12,13,14,15,16,17,255},	// attack (quick punch)
				{18,19,20,21,22,255},		// die
				{18,19,20,21,22,21,20,19,18,255},	// teleport = A1
				{4,5,6,7,8,9,10,11,6,7,8,9,10,11,6,7,8,9,10,11,12,13,14,14,14,15,15,16,17,255},	// super punch = A2
				{1,2,3,3,2,3,2,3,3,3,2,2,1,1,255},		// scream = A3
				{4,5,6,7,8,9,10,11,6,7,8,9,10,11,6,7,8,9,10,11,12,13,14,14,14,15,15,16,17,255},	//unused
				{4,5,6,7,8,9,10,11,6,7,8,9,10,11,6,7,8,9,10,11,12,13,14,14,14,15,15,16,17,255},	//unused
			}},
		{"Draculite",0,1,-4,AI_Spikey, //good to go
		 12,13,80,500,"graphics/vampire.jsp",0,0,
		 MT_EVIL|MT_VAMPIRE|MT_SPOOKY,MG_LOONY,MC_GREEN,
			{
				{1,2,1,0,255},	// idle
				{1,2,1,0,255},	// move
				{3,4,5,6,7,8,9,255},	// attack
				{12,11,10,255},		// die
				{3,4,5,6,7,8,9,255},	// attack
				{3,4,5,6,7,8,9,255},	// attack
				{3,4,5,6,7,8,9,255},	// attack
				{3,4,5,6,7,8,9,255},	// attack
				{3,4,5,6,7,8,9,255},	// attack
			}},
		{"Vamp Lord",0,6,-2,AI_Spikey, //spice em' up!
		 12,13,80,500,"graphics/vampire.jsp",0,0,
		 MT_EVIL|MT_VAMPIRE|MT_SPOOKY|MT_MINIBOSS,MG_LOONY,MC_VIOLET,
			{
				{1,2,1,0,255},	// idle
				{1,2,1,0,255},	// move
				{3,4,5,6,7,8,9,255},	// attack
				{12,11,10,255},		// die
				{3,4,5,6,7,8,9,255},	// attack
				{3,4,5,6,7,8,9,255},	// attack
				{3,4,5,6,7,8,9,255},	// attack
				{3,4,5,6,7,8,9,255},	// attack
				{3,4,5,6,7,8,9,255},	// attack
			}},
		{"Frankenjulie",0,0,0,AI_Frankenjulie, //good to go
		 80,39,600,3500,"llgraphics/frankenj.jsp",0,MF_NOMOVE|MF_ONEFACE,
		 MT_EVIL|MT_BOSS|MT_SPOOKY|MT_WACKY,MG_LOONY,MC_AQUA,
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
		{"Frankenhand",0,0,0,AI_Hand, //increase knockback!
		 30,40,100,3500,"llgraphics/hand.jsp",0,MF_NOMOVE,
		 MT_EVIL|MT_WACKY|MT_SPOOKY,MG_LOONY,MC_AQUA,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,8,9,10,11,255},	// move
				{12,13,14,15,16,17,18,19,20,21,22,255},	// attack
				{23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,255},	// die
			}},
		{"Polterguy",255,255,2,AI_Polterguy, //fix direction!
		 30,9,300,1,"llgraphics/bigghost.jsp",0,MF_ONEFACE|MF_FLYING|MF_FREEWALK|MF_ENEMYWALK,
		 MT_EVIL|MT_BOSS|MT_SPOOKY|MT_FLYING,MG_LOONY,MC_GREY,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,2,1,255},	// attack
				{4,4,5,5,6,6,7,7,8,8,255},	// die
				{1,2,3,3,3,2,1,2,3,2,1,0,0,0,0,1,2,3,2,1,255},	// A1=sneeze
				{1,2,3,2,1,255},	// unused
				{1,2,3,2,1,255},	// unused
				{1,2,3,2,1,255},	// unused
				{1,2,3,2,1,255},	// unused
			}},
		{"Gusty the Elder",3,7,3,AI_WindElder, //include new attack?
		 20,13,300,800,"llgraphics/elder.jsp",0,0,
		 MT_EVIL|MT_MINIBOSS|MT_SPOOKY|MT_VAMPIRE,MG_LOONY,MC_AQUA,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,4,4,4,5,6,255},	// attack (shove)
				{0,0,0,0,0,0,255},		// die
				{7,8,9,9,9,9,8,8,7,7,255},	// summon pose = A1
				{10,11,12,12,12,11,10,255},	// point = A2
			}},
		{"Stoney the Elder",3,2,0,AI_EarthElder, //good to go
		 20,13,300,800,"llgraphics/elder.jsp",0,0,
		 MT_EVIL|MT_MINIBOSS|MT_SPOOKY|MT_VAMPIRE,MG_LOONY,MC_BROWN,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,4,5,6,255},	// attack (shove)
				{254,254,254,254,255},		// die
				{7,8,9,9,9,9,8,8,7,7,255},	// summon pose = A1
				{10,11,12,12,12,12,12,12,12,12,11,10,255},	// point = A2
			}},
		{"Drippy the Elder",255,255,0,AI_WaterElder, //make harder! include big bubble attack please
		 20,13,300,800,"llgraphics/elder.jsp",0,MF_FLYING,
		 MT_EVIL|MT_MINIBOSS|MT_SPOOKY|MT_VAMPIRE,MG_LOONY,MC_BLUE,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,4,5,6,255},	// attack (shove)
				{254,254,254,254,255},		// die
				{7,8,9,9,9,9,8,8,7,7,255},	// summon pose = A1
				{10,11,12,12,12,12,12,12,12,12,11,10,255},	// point = A2
			}},
		{"Toasty the Elder",3,4,0,AI_FireElder, //good to go
		 20,13,300,800,"llgraphics/elder.jsp",0,0,
		 MT_EVIL|MT_MINIBOSS|MT_SPOOKY|MT_VAMPIRE,MG_LOONY,MC_RED,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,4,5,6,255},	// attack (shove)
				{254,254,254,254,255},		// die
				{7,8,9,9,9,9,8,8,7,7,255},	// summon pose = A1
				{10,11,12,12,12,12,12,12,12,12,11,10,255},	// point = A2
			}},
		{"Count Bonkula",255,255,0,AI_Bonkula, //fix bullet attack, compare source material
		 8,28,200,1000,"llgraphics/bossvamp.jsp",0,MF_WALLWALK|MF_NOMOVE|MF_INVINCIBLE|MF_FLYING,
		 MT_EVIL|MT_BOSS|MT_SPOOKY|MT_VAMPIRE,MG_LOONY,MC_VIOLET,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,255},	// attack (shoot)
				{21,22,23,24,25,26,27,26,25,24,25,26,27,255},		// die
				{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255},	// A1=spin around
			}},
		{"The Evilizer",255,255,0,AI_Evilizer, //include new enemies & invincible
		 140,32,2000,35000,"llgraphics/evilizer.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_FACECMD,
		 MT_EVIL|MT_BOSS|MT_SPOOKY|MT_SKELETON,MG_LOONY,MC_VIOLET,
			{
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// idle
				{0,255},	// move
				{0,255},	// attack (shoot)
				{0,0,0,0,0,0,0,255},	// die
			}},
		{"Steam Pump",255,255,0,AI_EvilPump, //nerf!!!
		 30,13,500,100,"llgraphics/evilpump.jsp",0,MF_NOMOVE,
		 MT_EVIL|MT_SPOOKY|MT_SKELETON,MG_LOONY,MC_VIOLET,
			{
				{0,1,2,3,4,5,6,7,8,7,6,5,4,3,2,1,255},	// idle
				{0,255},	// move
				{9,10,11,12,12,12,12,11,10,9,255},	// attack (shoot)
				{9,10,11,12,11,10,9,10,12,9,12,9,12,9,255},	// die
			}},
		{"Agent Skully",0,6,2,AI_Bonehead, //compare source material
		 8,38,10,50,"graphics/skeleton.jsp",0,0,
		 MT_SPOOKY|MT_EVIL|MT_SKELETON,MG_LOONY,MC_VIOLET,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{20,21,22,23,24,255},	// attack
				{7,8,9,10,11,12,13,14,15,16,255},		// die
				{25,26,27,28,29,30,255},	// chomp/spit projectile
				{31,32,32,32,31,255},	// point at bouapha
				{16,15,14,13,12,11,10,9,8,7,255},	// resurrect=A3
				{0,255},
				{0,255},
			}},
		{"Summony the Elder",3,1,-5,AI_SummonElder, //good to go
		 20,13,500,5000,"llgraphics/elder.jsp",0,0,
		 MT_EVIL|MT_MINIBOSS|MT_SPOOKY|MT_VAMPIRE,MG_LOONY,MC_GREEN,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,4,5,6,255},	// attack (shove)
				{254,254,254,254,255},		// die
				{7,8,9,9,9,9,8,8,7,7,255},	// summon pose = A1
				{10,11,12,12,12,12,12,12,12,12,11,10,255},	// point = A2
			}},
		{"Harry",5,2,0,AI_Harry, //check source
		 25,38,300,1500,"llgraphics/larry.jsp",0,0,
		 MT_EVIL|MT_SPOOKY|MT_ANIMAL|MT_BOSS,MG_LOONY,MC_BROWN, //work on!
			{
				{0,255},	// idle
				{1,2,3,3,2,1,0,4,5,6,6,5,4,0,255},	// move
				{25,26,27,27,28,29,30,31,32,33,33,33,34,35,36,37,255},	// attack (pounce)
				{0,0,0,0,0,255},		// die
				{7,8,9,9,10,11,12,13,14,15,16,17,255},	// fire darts=A1
				{18,19,20,21,22,23,23,23,23,24,24,255},		// howl = A2
				{0,255},	// blank = A3
				{0,255},	// blank = A4
				{0,255},	// blank = A5
			}},
		{"Frogurt",1,5,2,AI_Frog, //cjheck source
		 6,22,4,100,"graphics/frog.jsp",0,0,
		 MT_EVIL|MT_ANIMAL|MT_FOREST,MG_LOONY,MC_GREEN,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,8,0,255},	// move
				{9,10,11,12,13,14,15,16,255},	// attack
				{17,18,19,20,21,255},		// die
				{9,10,11,12,13,14,15,16,255},	// unused
				{9,10,11,12,13,14,15,16,255},	// unused
				{9,10,11,12,13,14,15,16,255},	// unused
				{9,10,11,12,13,14,15,16,255},	// unused
				{9,10,11,12,13,14,15,16,255},	// unused
			}},
		{"Torture Tree",4,5,0,AI_EvilTree, //check source?
		 32,24,150,1,"llgraphics/eviltree.jsp",0,MF_NOMOVE|MF_ONEFACE, //work on!
		 MT_EVIL|MT_SPOOKY|MT_VEGGIE|MT_FOREST,MG_LOONY,MC_BROWN,
			{
				{4,5,6,7,8,9,10,11,12,13,255},	// idle
				{0,255},	// move
				{0,1,2,3,1,255},	// attack
				{14,15,16,17,18,19,20,21,22,23,255},		// die
				{0,1,2,3,1,255},	// unused
				{0,1,2,3,1,255},	// unused
				{0,1,2,3,1,255},	// unused
				{0,1,2,3,1,255},	// unused
				{0,1,2,3,1,255},	// unused
			}},
		{"Gasbat",2,1,-3,AI_Bat2, //check source
		 8,33,15,25,"llgraphics/bat.jsp",0,MF_FLYING,
		 MT_SPOOKY|MT_FLYING|MT_EVIL|MT_ANIMAL,MG_LOONY,MC_GREEN,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,8,7,255},	// attack
				{17,18,19,20,21,22,23,24,25,26,255},		// die
				{10,11,12,12,12,12,12,11,10,255},	// diving attack
				{13,14,15,15,16,255},	// bounce off during dive
				{7,8,9,8,7,255},	// attack
				{0,255},
				{0,255},
			}},
		{"Marsh Mutt",5,6,3,AI_Swampdog, //good to go
		 18,43,150,350,"llgraphics/swampdog.jsp",0,0,
		 MT_EVIL|MT_SPOOKY|MT_THINGY,MG_LOONY,MC_VIOLET,
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
		{"King Toot",0,6,0,AI_Mummy, //good to go
		 15,28,300,800,"llgraphics/mummy.jsp",0,MF_NOMOVE,
		 MT_EVIL|MT_SPOOKY|MT_DESERT|MT_MINIBOSS,MG_LOONY,MC_VIOLET,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,255},	// attack
				{19,20,21,22,23,24,25,26,27,255},		// die
				{15,16,17,18,18,17,16,15,255},	// A1 = gem zap
				{7,8,9,9,9,9,9,8,7,255},	// A2 = magic hands
				{27,26,25,24,23,22,21,20,19,255},	// A3 = resurrect
				{7,8,9,10,11,12,13,14,255},	// attack 4
				{7,8,9,10,11,12,13,14,255},	// attack 5
			}},
		{"Ghastly Stench",0,1,3,AI_Ghastly,
		 30,9,250,1,"llgraphics/bigghost.jsp",0,MF_ONEFACE|MF_FLYING|MF_FREEWALK|MF_ENEMYWALK, //work on!
		 MT_EVIL|MT_BOSS|MT_SPOOKY|MT_FLYING,MG_LOONY,MC_GREEN,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,2,1,255},	// attack
				{4,4,5,5,6,6,7,7,8,8,255},	// die
				{1,2,3,3,3,2,1,2,3,2,1,0,0,0,0,1,2,3,2,1,255},	// A1=sneeze
				{1,2,3,2,1,255},	// unused
				{1,2,3,2,1,255},	// unused
				{1,2,3,2,1,255},	// unused
				{1,2,3,2,1,255},	// unused
			}},
		{"Bombie Lord",0,4,7,AI_Zombie, //give a different attack OR stop brains from spawning!
		 11,32,70,150,"graphics/zombie.jsp",0,0,
		 MT_SPOOKY|MT_ZOMBIE|MT_EVIL|MT_MINIBOSS|MT_WACKY,MG_LOONY,MC_RED,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,9,10,11,12,13,13,14,15,16,17,18,19,255},	// attack
				{20,21,22,23,24,25,26,27,28,29,30,31,255},	// die
				{7,8,9,9,10,11,12,13,13,14,15,16,17,18,19,255},	// attack
				{7,8,9,9,10,11,12,13,13,14,15,16,17,18,19,255},	// attack
				{7,8,9,9,10,11,12,13,13,14,15,16,17,18,19,255},	// attack
				{0,255},
				{0,255},
			}},
		{"MUTANT PUMPKIN!",2,6,0,AI_Pumpkin,
		 10,13,50,750,"llgraphics/pumpkin.jsp",0,0,
		 MT_EVIL|MT_PUMPKIN|MT_VEGGIE|MT_WACKY,MG_LOONY,MC_VIOLET,
			{
				{0,255},	// idle
				{0,1,2,3,4,5,255},	// move
				{0,1,2,3,0,1,2,3,0,1,2,3,255},	// attack (unused)
				{6,7,8,9,10,11,12,255},		// die
				{1,2,3,4,5,0,255},	// move
				{1,2,3,4,5,0,255},	// move
				{1,2,3,4,5,0,255},	// move
				{1,2,3,4,5,0,255},	// move
				{1,2,3,4,5,0,255},	// move
			}},
		{"Mini-Frankenjulie",0,0,0,AI_MiniFrankenjulie, //nerf a bit??
		 40,39,400,1500,"llgraphics/franksml.jsp",0,MF_NOMOVE|MF_ONEFACE,
		 MT_EVIL|MT_MINIBOSS|MT_SPOOKY|MT_WACKY,MG_LOONY,MC_AQUA,
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
		{"MechaBonkula",5,255,4,AI_MechaBonkula,  //check source material, add hammer shake ground
		 8,28,700,5000,"llgraphics/bossvamp.jsp",0,MF_WALLWALK|MF_NOMOVE|MF_FLYING,
		 MT_EVIL|MT_BOSS|MT_SPOOKY|MT_VAMPIRE|MT_HIGHTECH|MT_WACKY,MG_LOONY,MC_GREY,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,255},	// attack (shoot)
				{21,22,23,24,25,26,27,26,25,24,25,26,27,255},		// die
				{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255},	// A1=spin around
			}},
		{"Arctic Wolf",2,7,2,AI_Werewolf,  //good to go
		 12,55,60,100,"graphics/wolfman.jsp",0,0,
		 MT_EVIL|MT_SPOOKY|MT_ANIMAL|MT_ARCTIC,MG_LOONY,MC_AQUA,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{30,31,32,32,32,33,34,35,36,37,38,39,40,41,42,43,255},	// attack (pounce)
				{44,45,46,47,48,49,50,51,51,51,52,53,54,254,54,254,54,254,54,255},		// die
				{7,8,9,9,8,7,255},	// spit projectile = A1
				{10,11,12,12,12,13,14,15,16,17,17,18,19,255},	// charging slash = A2
				{20,21,22,23,24,25,26,26,26,26,26,26,26,26,27,28,29,255},		// howl = A3
				{20,21,22,23,24,25,26,26,26,26,26,26,26,26,27,28,29,255},	// howl 2
				{30,31,32,32,32,33,34,35,36,37,38,39,40,41,42,43,255},	// attack (pounce)
			}},
		{"Grabby Bat",2,6,-3,AI_Bat2,  //good to go
		 8,33,15,25,"llgraphics/bat.jsp",0,MF_FLYING|MF_ENEMYWALK|MF_FREEWALK,
		 MT_SPOOKY|MT_FLYING|MT_EVIL|MT_ANIMAL,MG_LOONY,MC_VIOLET,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{11,12,13,12,11,255},	// attack
				{23,24,25,26,27,28,29,30,31,32,255},		// die
				{14,15,16,16,16,16,15,14,255},	// diving attack =A1
				{17,18,19,20,21,22,255},	// bounce off during dive =A2
				{7,8,9,10,9,8,7,255},		// shoot = A3
				{0,255},
				{0,255},
			}},
		{"Frankenfurter",5,4,-1,AI_Swampdog,  //good to go
		 18,43,80,150,"llgraphics/swampdog.jsp",0,0,
		 MT_EVIL|MT_SPOOKY|MT_THINGY,MG_LOONY,MC_RED,
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
		{"Happy Stick Witch",255,255,0,AI_StickWitch,  //check wall hugging and projecitle issues
		 12,20,500,5000,"llgraphics/stikwtch.jsp",0,MF_ONEFACE,
		 MT_EVIL|MT_SPOOKY|MT_THINGY|MT_MINIBOSS|MT_WACKY,MG_LOONY,MC_AQUA,
			{
				{0,255},	// idle
				{1,1,0,2,2,0,255},	// move
				{4,0,0,255},	// attack (shove)
				{5,5,5,5,5,6,6,7,8,9,10,11,12,13,14,15,16,16,16,16,16,17,18,255},		// die
				{3,3,3,255},	// ouch = A1
				{1,0,2,0,1,0,2,0,1,0,2,0,1,0,2,0,4,0,3,3,3,255},	// appear=A2
			}},
		{"Wolfman",2,0,2,NULL,
		 12,55,128,0,"graphics/wolfman.jsp",0,MF_WATERWALK,
		 MT_WEIRD,MG_NEW,MC_GREY,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{30,31,32,32,32,33,34,35,36,37,38,39,40,41,42,43,255},	// attack (pounce)
				{44,45,46,47,48,49,50,51,51,51,52,53,54,254,54,254,54,254,54,255},		// die
				{7,8,9,9,8,7,255},	// spit projectile = A1
				{10,11,12,12,12,13,14,15,16,17,17,18,19,255},	// charging slash = A2
				{44,45,46,47,48,49,50,51,51,51,52,53,54,254,54,254,54,254,54,255},		// drown = A3
				{20,21,22,23,24,25,26,26,26,26,26,26,26,26,27,28,29,255},		// howl = A4
				{30,31,32,32,32,33,34,35,36,37,38,39,40,41,42,43,255},	// attack (pounce)
			}},
		{"Wild Wizard",4,5,4,NULL,
		 18,25,128,0,"!59",0,MF_WATERWALK,
		 MT_WEIRD,MG_NEW,MC_YELLOW,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,15,16,17,18,19,255},	// attack=smack
				{20,21,22,23,24,24,24,254,24,254,24,254,24,254,24,255},	// die
				{7,9,11,12,12,12,12,12,12,11,9,7,255},	// A1=cast spell
				{7,8,9,10,11,12,13,14,15,16,17,18,19,255},	// attack=smack
				{20,21,22,23,24,24,24,254,24,254,24,254,24,254,24,255},	// die
				{7,8,9,10,11,12,13,14,15,16,17,18,19,255},	// attack=smack
				{7,8,9,10,11,12,13,14,15,16,17,18,19,255},	// attack=smack
			}},
		{"Ice Pumpkin",2,7,2,AI_Pumpkin,
		 11,6,5,500,"graphics/pumpkin.jsp",0,0,
		 MT_EVIL|MT_PUMPKIN|MT_ARCTIC|MT_VEGGIE|MT_FOREST,MG_NEW,MC_AQUA,
			{
				{0,255},	// idle
				{1,2,3,4,5,0,255},	// move
				{1,2,3,4,5,0,255},	// move
				{0,0,255},	// die (just long enough to spew shrapnel)
				{1,2,3,4,5,0,255},	// move
				{1,2,3,4,5,0,255},	// move
				{1,2,3,4,5,0,255},	// move
				{1,2,3,4,5,0,255},	// move
				{1,2,3,4,5,0,255},	// move
			}},
		{"Night Terror",6,2,-4,AI_NightTerror, //good to go
		 12,13,60,500,"graphics/vampire.jsp",0,0,
		 MT_EVIL|MT_VAMPIRE|MT_SPOOKY,MG_NEW,MC_BROWN,
			{
				{1,2,1,0,255},	// idle
				{1,2,1,0,255},	// move
				{3,4,5,6,7,8,9,255},	// attack
				{12,11,10,255},		// die
				{3,4,5,6,7,8,9,255},	// attack
				{3,4,5,6,7,8,9,255},	// attack
				{3,4,5,6,7,8,9,255},	// attack
				{3,4,5,6,7,8,9,255},	// attack
				{3,4,5,6,7,8,9,255},	// attack
			}},
		{"Revenge Armor",4,3,-3,AI_CloneArmor, //turn into evil clone upon destruction
		 20,14,128,0,"graphics/pwrarmor.jsp",0,0,
		 MT_SPACE|MT_EVIL|MT_HUMAN,MG_NEW,MC_BLUE,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,9,9,9,8,7,255},	// attack
				{10,11,12,13,255},		// die
				{7,7,7,255},		// fire missiles
				{13,12,11,10,255},	// activate
				{7,8,9,9,9,9,8,7,255},	// attack
				{7,8,9,9,9,9,8,7,255},	// attack
				{7,8,9,9,9,9,8,7,255},	// attack
			}},
		{"Ice Scream",1,7,3,AI_ScreamQueen, //good to go
		 12,44,20,50,"graphics/babything.jsp",0,0,
		 MT_EVIL|MT_THINGY|MT_ARCTIC,MG_NEW,MC_AQUA,
			{
				{12,255},	// idle
				{1,2,3,4,5,6,7,8,9,10,11,255},	// move
				{20,21,22,23,24,25,26,27,255},	// attack
				{34,35,36,37,38,39,40,41,42,43,255},	// die
				{13,14,15,16,17,16,15,16,17,16,15,16,17,16,15,255},	// fall asleep (A1)
				{18,19,18,255},	// blink (A2)
				{28,29,30,29,28,255}, // look right (A3)
				{31,32,33,32,31,255}, // look left (A4)
				{0,255},
			}},
		{"Grue Jr.",1,0,-13,AI_GrueJr, //fix particle glitch
		 8,21,10,50,"graphics/bigspdr.jsp",0,0,
		 MT_EVIL|MT_SPIDER|MT_ANIMAL,MG_NEW,MC_GREY,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,12,13,13,255},	// attack
				{14,15,16,17,18,17,18,17,18,19,19,20,20,255},	// die
				{7,8,9,10,11,12,12,13,13,255},	// attack
				{7,8,9,10,11,12,12,13,13,255},	// attack
				{7,8,9,10,11,12,12,13,13,255},	// attack
				{0,255},
				{0,255},
			}},
		{"Pygmy Boomeranger",5,2,0,AI_PygmyBoomer, //make less floaty?
		 10,41,15,100,"graphics/pygmy2.jsp",0,0,
		 MT_EVIL|MT_PYGMY|MT_HUMAN,MG_NEW,MC_BROWN,
			{
				{0,0,0,0,0,0,0,0,0,0,1,2,3,2,1,0,0,0,4,5,6,5,4,255},	// idle
				{8,9,10,9,8,7,11,12,13,12,11,7,255},	// move
				{20,21,22,23,24,25,26,27,28,255},	// attack
				{32,33,34,35,36,37,38,39,40,255},	// die
				{14,15,16,15,14,0,17,18,19,18,17,0,255},	// (A1) bored (unga dance)
				{29,30,31,30,29,0,29,30,31,30,29,255},	// (A2) angry, spotted Bouapha
				{20,21,22,23,24,25,26,27,28,255},	// attack
				{20,21,22,23,24,25,26,27,28,255},	// attack
				{20,21,22,23,24,25,26,27,28,255},	// attack
			}},
		{"Deathcap",255,255,0,AI_HotShroom, //good to go
		 15,23,30,50,"expgraphics/shroomgrn.jsp",0,0,
		 MT_EVIL|MT_VEGGIE|MT_TRAP,MG_NEW,MC_AQUA,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,255},	// attack
				{15,16,17,18,19,20,21,22,255},	// die
				{7,8,9,10,11,12,13,14,255},	// a1
				{7,8,9,10,11,12,13,14,255},	// attack
				{7,8,9,10,11,12,13,14,255},	// attack
				{0,255},
				{0,255},
			}},
		{"Wild Bush",1,4,3,AI_Crazybush, //actually program
		 10,8,20,5,"kmgraphics/crazybush.jsp",0,0,
		 MT_EVIL|MT_WACKY|MT_VEGGIE|MT_FOREST,MG_NEW,MC_RED,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{0,255},	// attack
				{5,6,7,255},	// die
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
			}},
		{"Atom Splitter",0,6,3,AI_Bombot, //actually program
		 13,10,50,100,"!80",0,0,
		 MT_EVIL|MT_HIGHTECH|MT_TRAP,MG_NEW,MC_VIOLET,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{5,6,6,6,6,6,5,255},	// attack
				{7,8,9,9,9,9,255},	// die
				{5,6,6,6,6,6,5,255},	// attack
				{5,6,6,6,6,6,5,255},	// attack
				{5,6,6,6,6,6,5,255},	// attack
				{5,6,6,6,6,6,5,255},	// attack
				{5,6,6,6,6,6,5,255},	// attack
			}},
		{"Repairbot",255,255,0,AI_Noodlenode, //good to go?
		 10,8,40,50,"expgraphics/dancebot.jsp",0,MF_ONEFACE|MF_NOMOVE|MF_WATERWALK|MF_FLYING,
		 MT_EVIL|MT_WACKY|MT_HIGHTECH,MG_NEW,MC_GREY,
			{
				{0,255},// idle
				{0,1,2,3,4,5,6,7,6,5,4,3,2,1,255},// move
				{0,2,4,6,7,6,4,2,1,255},// attack
				{0,2,4,6,6,6,4,2,1,255},// die
				{0,2,4,6,7,6,4,2,1,255},// attack
				{0,2,4,6,7,6,4,2,1,255},// attack
				{0,2,4,6,7,6,4,2,1,255},// attack
				{0,2,4,6,7,6,4,2,1,255},// attack
				{0,2,4,6,7,6,4,2,1,255},// attack
			}},
		{"Sandman",0,2,-1,AI_Snowguy, //actually program
		 26,36,40,200,"graphics/snowguy.jsp",0,0,
		 MT_EVIL|MT_DESERT|MT_HUMAN,MG_NEW,MC_BROWN,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,0,255},	// move
				{24,25,26,27,28,29,255},	// attack=slash
				{30,31,32,33,34,35,254,35,254,35,254,35,254,254,35,255},	// die
				{8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,255},	//A1=throw snowball
				{24,25,26,27,28,29,255},	// attack=slash
				{24,25,26,27,28,29,255},	// attack=slash
				{0,255},
				{0,255},
			}},
		{"Nasty Tree",255,255,0,AI_Decoy, //good to go
		 12,6,80,500,"expgraphics/treedecoy.jsp",0,MF_ONEFACE|MF_NOMOVE|MF_NOSHADOW,
		 MT_EVIL|MT_WACKY|MT_VEGGIE|MT_TRAP|MT_FOREST,MG_NEW,MC_GREEN,
			{
				{0,255},	// idle
				{0,0,0,255},	// move
				{0,0,0,0,0,0,255},	// attack=slash
				{1,1,2,2,3,3,4,4,5,255},	// die
				{0,0,0,0,0,0,255},	// attack=slash
				{0,0,0,0,0,0,255},	// attack=slash
				{0,0,0,0,0,0,255},	// attack=slash
				{0,0,0,0,0,0,255},	// attack=slash
				{0,0,0,0,0,0,255},	// attack=slash
			}},
		{"Deadeye",5,4,3,AI_Eyeguy, //actually program
		 14,17,20,75,"kmgraphics/eyeguy.jsp",0,0,
		 MT_EVIL|MT_THINGY|MT_TRAP,MG_NEW,MC_RED,
			{
				{0,0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,255},	// attack
				{13,14,15,16,255},	// die
				{7,8,9,10,11,12,255},	// a1
				{7,8,9,10,11,12,255},	// a2
				{7,8,9,10,11,12,255},	// a3
				{7,8,9,10,11,12,255},	// a4
				{7,8,9,10,11,12,255},	// a5
			}},
		{"Peabrain",255,255,0,AI_Zombie, //properly animate
		 5,42,10,50,"expgraphics/zombie2.jsp",0,MF_ONEFACE,
		 MT_EVIL|MT_SPOOKY|MT_ZOMBIE|MT_WACKY,MG_NEW,MC_GREY,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{15,16,17,18,19,20,21,22,23,23,24,25,26,27,28,255},	// attack
				{29,30,31,32,33,34,35,36,37,38,39,40,41,255},		// die
				{7,8,9,10,11,12,13,14,255},	// A1 = stomp ground
				{15,16,17,18,19,20,21,22,23,23,24,25,26,27,28,255},	// attack
				{7,8,9,10,11,12,13,14,255},	// A1 = stomp ground
				{15,16,17,18,19,20,21,22,23,23,24,25,26,27,28,255},	// attack
				{7,8,9,10,11,12,13,14,255},	// A1 = stomp ground
			}},
		{"Anubis",2,0,-4,AI_Werewolf, //actually program
		 12,55,60,100,"graphics/wolfman.jsp",0,0,
		 MT_EVIL|MT_ANIMAL|MT_DESERT,MG_NEW,MC_GREY,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{30,31,32,32,32,33,34,35,36,37,38,39,40,41,42,43,255},	// attack (pounce)
				{44,45,46,47,48,49,50,51,51,51,52,53,54,254,54,254,54,254,54,255},		// die
				{7,8,9,9,8,7,255},	// spit projectile = A1
				{10,11,12,12,12,13,14,15,16,17,17,18,19,255},	// charging slash = A2
				{20,21,22,23,24,25,26,26,26,26,26,26,26,26,27,28,29,255},		// howl = A3
				{20,21,22,23,24,25,26,26,26,26,26,26,26,26,27,28,29,255},	// howl 2
				{30,31,32,32,32,33,34,35,36,37,38,39,40,41,42,43,255},	// attack (pounce)
			}},
		{"Odd Hologram",255,255,0,AI_Hologram, //good to go
		 40,21,256,100,"expgraphics/drhologram.jsp",0,MF_GLOW,
		 MT_EVIL|MT_WACKY|MT_HIGHTECH|MT_ASYLUM,MG_NEW,MC_RED,
			{
				{0,255},	// idle
				{7,8,9,8,7,0,10,11,12,11,10,0,255},	// move
				{1,2,3,4,5,6,5,4,5,6,5,6,5,4,3,4,3,4,5,4,3,2,1,255},	// attack=laugh
				{13,14,15,16,17,17,17,17,17,17,17,18,19,20,20,20,20,255},	// die
				// these following are only used when you play as Dr. L
				{1,2,3,2,1,255},	// use item
				{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255},	// bored
				{7,9,7,0,10,12,10,0,7,9,7,0,0,0,0,0,0,255},	// watery death
				{1,2,3,4,5,6,5,4,5,6,5,6,5,4,3,4,3,4,5,4,3,2,1,255},	// attack=laugh
				{1,2,3,4,5,6,5,4,5,6,5,6,5,4,3,4,3,4,5,4,3,2,1,255},	// attack=laugh
			}},
		{"Spiderbot",255,255,0,AI_Crabshell, //not even programmed yet?
		 12,7,120,50,"expgraphics/crabbot.jsp",0,MF_ONEFACE,
		 MT_EVIL|MT_HIGHTECH,MG_NEW,MC_GREY,
			{
				{0,255},// idle
				{0,1,2,3,4,5,6,5,4,3,2,1,0,255},// move
				{0,1,2,3,4,5,6,5,4,3,2,1,0,255},// attack
				{0,2,4,6,6,4,2,1,2,255},// die
				{0,2,4,6,7,6,4,2,1,255},// attack
				{0,2,4,6,7,6,4,2,1,255},// attack
				{0,2,4,6,7,6,4,2,1,255},// attack
				{0,2,4,6,7,6,4,2,1,255},// attack
				{0,2,4,6,7,6,4,2,1,255},// attack
			}},
		{"Patrol Zoid",2,4,-3,AI_PatrolZoid, //good to go
		 15,20,80,75,"graphics/geozoid.jsp",0,MF_NOMOVE|MF_AQUATIC|MF_WATERWALK,
		 MT_EVIL|MT_ZOID|MT_ASYLUM|MT_TRAP,MG_NEW,MC_BROWN,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,255},	// move
				{4,5,6,7,8,9,10,11,12,255},	// attack=spit rock
				{13,14,15,16,17,18,19,255},	// die
				{4,5,6,7,8,9,10,11,12,255},	// attack=spit rock
				{4,5,6,7,8,9,10,11,12,255},	// attack=spit rock
				{4,5,6,7,8,9,10,11,12,255},	// attack=spit rock
				{0,255},
				{0,255},
			}},
		{"Galvigourd",2,0,-1,AI_Galvigourd, //good to go
		 10,13,100,500,"llgraphics/pumpkin.jsp",0,0,
		 MT_EVIL|MT_PUMPKIN|MT_VEGGIE|MT_HIGHTECH,MG_NEW,MC_GREY,
			{
				{0,255},	// idle
				{0,1,2,3,4,5,255},	// move
				{0,1,2,3,0,1,2,3,0,1,2,3,255},	// attack (unused)
				{6,7,8,9,10,11,12,255},		// die
				{6,7,8,9,10,11,12,255},	// attack (unused)
				{12,255},	// ball
				{12,11,10,9,8,7,6,255},	// attack part 2
				{1,2,3,4,5,0,255},	// move
				{1,2,3,4,5,0,255},	// move
			}},
		{"Kaboomkin",2,4,2,AI_BigPumpkin, //good to go
		 14,1,20,500,"!52",0,0,
		 MT_EVIL|MT_PUMPKIN|MT_VEGGIE,MG_NEW,MC_RED,
			{
				{0,255},	// idle
				{0,0,0,255},	// move
				{0,255},	// attack
				{0,0,0,255},	// die
				{0,0,0,255},	// move
				{0,0,0,255},	// move
				{0,0,0,255},	// move
				{0,0,0,255},	// move
				{0,0,0,255},	// move
			}},
		{"Chef Liem",255,255,0,AI_ChefLiem, //PLEASE PROGRAM THIS
		 13,3,600,100,"expgraphics/chefliem.jsp",0,MF_ONEFACE,
		 MT_EVIL|MT_HUMAN|MT_BOSS|MT_WACKY,MG_NEW,MC_GREY,
			{
				{0,255},	// idle
				{0,1,0,2,255},	// move
				{0,0,0,0,0,0,0,0,0,0,255},	// attack
				{0,0,0,0,0,0,0,0,0,0,255},	// die
				{0,0,0,255},	// move
				{0,0,0,255},	// move
				{0,0,0,255},	// move
				{0,0,0,255},	// move
				{0,0,0,255},	// move
			}},
		{"Kelvozoid",3,7,2,AI_Serpent, //good to go
		 15,20,25,75,"graphics/magmazoid.jsp",0,MF_AQUATIC|MF_WATERWALK,
		 MT_AQUATIC|MT_EVIL|MT_ZOID|MT_ARCTIC,MG_NEW,MC_AQUA,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,255},	// move
				{4,5,6,7,8,9,9,9,9,10,255},	// attack=spit spines
				{11,12,13,14,15,16,17,255},	// die
				{4,5,6,7,8,9,9,9,9,10,255},	// attack=spit spines
				{4,5,6,7,8,9,9,9,9,10,255},	// attack=spit spines
				{4,5,6,7,8,9,9,9,9,10,255},	// attack=spit spines
				{4,5,6,7,8,9,9,9,9,10,255},	// attack=spit spines
			}},
		{"Missozoid",3,0,-3,AI_Serpent, //good to go
		 15,18,30,75,"graphics/freakazoid.jsp",0,MF_AQUATIC|MF_WATERWALK,
		 MT_EVIL|MT_ZOID|MT_HIGHTECH|MT_AQUATIC,MG_NEW,MC_GREY,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,255},	// move
				{4,5,6,7,8,9,9,9,9,10,255},	// attack=spit spines
				{11,12,13,14,15,16,17,255},	// die
				{4,5,6,7,8,9,9,9,9,10,255},	// attack=spit spines
				{4,5,6,7,8,9,9,9,9,10,255},	// attack=spit spines
				{4,5,6,7,8,9,9,9,9,10,255},	// attack=spit spines
				{4,5,6,7,8,9,9,9,9,10,255},	// attack=spit spines
			}},
		{"FroZombie",0,7,8,AI_Zombie, //good to go
		 11,32,20,150,"graphics/zombie.jsp",0,0,
		 MT_SPOOKY|MT_ZOMBIE|MT_EVIL|MT_ARCTIC,MG_NEW,MC_AQUA,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,9,10,11,12,13,13,14,15,16,17,18,19,255},	// attack
				{20,21,22,23,24,25,26,27,28,29,30,31,255},	// die
				{7,8,9,9,10,11,12,13,13,14,15,16,17,18,19,255},	// attack
				{7,8,9,9,10,11,12,13,13,14,15,16,17,18,19,255},	// attack
				{7,8,9,9,10,11,12,13,13,14,15,16,17,18,19,255},	// attack
				{0,255},
				{0,255},
			}},
		{"Super Mutant Zombie",0,1,2,AI_SuperMutantZombie, //re-examine
		 36,38,200,2500,"graphics/suprzmbe.jsp",0,MF_NOMOVE,
		 MT_EVIL|MT_MINIBOSS|MT_SPOOKY|MT_ZOMBIE,MG_NEW,MC_GREEN,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{17,18,19,20,21,22,23,24,25,255},	// attack=punch
				{9,10,11,12,13,14,15,15,15,15,15,15,255},	// die
				{7,8,9,10,11,11,11,12,12,13,13,13,14,14,14,15,16,16,255},	// A1=leap
				{26,27,28,29,30,255},	// A2=attempted grab
				{27,31,32,33,34,35,34,36,37,36,34,35,34,36,37,36,34,35,34,33,31,27,255},	//A3= grab n' pound
				{0,255},
				{0,255},
			}},
		{"Hotfoot",255,255,0,AI_Hotfoot, //add attacks and make it move right
		 4,3,20,2500,"expgraphics/burny.jsp",0,MF_NOMOVE|MF_ONEFACE,
		 MT_EVIL|MT_WACKY|MT_SPOOKY,MG_NEW,MC_YELLOW,
			{
				{0,255},	// idle
				{0,1,2,1,255},	// move
				{0,255},	// attack
				{0,0,0,255},	// die
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
			}},
		{"Xeno Empress",255,255,0,AI_XenoEmpressHead, //add attacks and make it move right
		 36,5,500,1000,"expgraphics/queenskull.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_FREEWALK|MF_ENEMYWALK,
		 MT_BITS,MG_NEW,MC_GREY,
			{
				{0,255},	// idle
				{0,255},	// idle
				{0,1,2,3,4,4,4,3,2,1,255},	// idle
				{0,0,0,0,0,0,0,0,0,0,0,0,0,255}, //dead!
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
			}},
		{"Xeno Empress",255,255,0,AI_XenoEmpressBody,
		 50,4,500,1000,"expgraphics/queenbody.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_ENEMYWALK,
		 MT_EVIL|MT_BOSS|MT_ALIEN,MG_NEW,MC_GREY,
			{
				{0,255},	// idle
				{0,255},	// idle
				{0,1,2,3,2,1,255},	// idle
				{0,0,0,0,0,0,0,0,0,0,255},	// idle
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
			}},
		{"Xeno Empress Tail",255,255,0,AI_XenoEmpressTail,
		 36,13,1,1000,"expgraphics/queentail.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_INVINCIBLE|MF_ENEMYWALK,
		 MT_BITS,MG_NEW,MC_GREY,
			{
				{0,255},	// idle
				{0,255},	// idle
				{0,1,2,3,4,5,6,7,8,9,10,11,12,255},	// idle
				{0,0,0,0,0,0,0,0,0,0,255},	// idle
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
			}},
		{"LunaChick",7,6,-2,AI_Lunachick,	// the boss!!
		 11,27,500,1000,"graphics/lunachick.jsp",0,0,
		 MT_EVIL|MT_BOSS|MT_HUMAN|MT_SPACE,MG_NEW,MC_VIOLET,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,13,0,0,255},	// attack=fire gun
				{14,15,16,17,18,19,20,21,22,23,24,25,26,255},	// die
				{7,9,10,12,13,255},	// A1=use weapon
				{14,15,16,17,18,17,16,15,14,15,17,15,14,0,0,255},	// A2=bored
				{1,3,1,0,4,6,4,0,1,3,1,0,0,0,24,25,26,255},	// A3=watery death
				{14,15,16,17,18,19,20,21,21,21,21,21,20,19,18,17,16,15,14,1,2,3,2,1,255},	// attack=funny teleport
				{7,7,7,8,8,8,9,9,9,9,9,9,9,9,9,9,9,9,10,9,8,7,255},	// attack=slow fire
			}},
		
		{"Flying Zombie",3,4,-1,AI_Zombie, //make it cooler
		 11,32,40,250,"graphics/zombie.jsp",0,MF_FLYING|MF_WATERWALK,
		 MT_SPOOKY|MT_ZOMBIE|MT_EVIL|MT_FLYING,MG_NEW,MC_GREY,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,9,10,11,12,13,13,14,15,16,17,18,19,255},	// attack
				{20,21,22,23,24,25,26,27,28,29,30,31,255},	// die
				{7,8,9,9,10,11,12,13,13,14,15,16,17,18,19,255},	// attack
				{7,8,9,9,10,11,12,13,13,14,15,16,17,18,19,255},	// attack
				{7,8,9,9,10,11,12,13,13,14,15,16,17,18,19,255},	// attack
				{0,255},
				{0,255},
			}},
		{"Gingersnap",2,4,1,AI_Gingersnap, //replace AI
		 12,33,90,600,"graphics/ginger.jsp",0,0,
		 MT_EVIL|MT_WACKY|MT_TRAP,MG_NEW,MC_RED,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,255},	// move
				{7,8,9,10,11,12,13,14,15,16,17,18,19,20,19,20,19,20,21,22,23,255},	// triple punch
				{27,28,29,30,31,32,32,32,32,255},	// die
				{24,25,26,26,26,26,26,255},	// jumpkick (A1)
				{25,24,255},	// unjumpkick (A2)
				{7,8,9,10,11,12,13,14,15,16,17,18,19,20,19,20,19,20,21,22,23,255},	// triple punch
				{0,255},
				{0,255},
			}},
		{"Atomic SEAL",1,6,-1,AI_EliteSeal, //good to go
		 22,20,75,125,"!31",0,0,
		 MT_EVIL|MT_ANIMAL|MT_ARCTIC,MG_NEW,MC_GREY,
			{
				{0,255},	// idle
				{1,2,3,4,5,0,255},	// move
				{6,6,9,10,13,14,255},	// attack=full auto
				{15,16,17,18,19,19,19,255},	// die
				{6,6,9,10,13,14,255},	// attack=full auto
				{6,6,9,10,13,14,255},	// attack=full auto
				{6,6,9,10,13,14,255},	// attack=full auto
				{6,6,9,10,13,14,255},	// attack=full auto
				{6,6,9,10,13,14,255},	// attack=full auto
			}},
		{"Wet Blanket",255,255,0,AI_Ghost, //good to go
		 14,23,40,300,"expgraphics/ghostblu.jsp",0,MF_WALLWALK|MF_NOSHADOW|MF_FLYING|MF_GLOW|MF_ENEMYWALK|MF_FREEWALK,
		 MT_EVIL|MT_SPOOKY|MT_FLYING,MG_NEW,MC_GLOW,
			{
				{0,255},	// idle
				{0,0,255},	// move
				{1,2,3,3,2,3,2,3,3,3,2,2,1,1,255},		// scream = attack
				{18,19,20,21,22,255},		// die
				{18,19,20,21,22,21,20,19,18,255},	// teleport = A1
				{254,255},//invis = A2
				{22,21,20,19,18,255,255},	// a3
				{4,5,6,7,8,9,10,11,6,7,8,9,10,11,6,7,8,9,10,11,12,13,14,14,14,15,15,16,17,255},	//unused
				{4,5,6,7,8,9,10,11,6,7,8,9,10,11,6,7,8,9,10,11,12,13,14,14,14,15,15,16,17,255},	//unused
			}},
		{"Minitilda",4,3,0,AI_Minitilda, //good to go
		 40,11,500,1000,"expgraphics/minitilda.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_FACECMD|MF_NOGRAV|MF_ENEMYWALK|MF_FREEWALK,
		 MT_EVIL|MT_SPIDER|MT_MINIBOSS,MG_NEW,MC_BLUE,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,4,4,4,4,4,4,3,2,1,255},	// fire
				{1,2,0,3,4,3,2,0,3,4,3,0,1,4,2,4,3,1,2,255},	// die
				{1,2,3,4,4,4,4,4,4,4,3,2,1,255},	// fire
				{1,2,3,4,4,4,4,4,4,4,3,2,1,255},	// fire
				{1,2,3,4,4,4,4,4,4,4,3,2,1,255},	// fire
				{0,255},
				{0,255},
			}},
		{"Minitilda's Body",4,3,0,AI_MattieBody,	// the body of matilda
		 50,7,1,1,"expgraphics/matminibody.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_INVINCIBLE|MF_NOGRAV,
		 MT_BITS,MG_NEW,MC_BLUE,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{0,255},	// fire
				{1,2,0,3,4,3,2,0,3,4,3,0,1,4,2,4,3,1,2,255},	// die
				{0,255},
				{0,255},
				{0,255},
				{0,255},
				{0,255},
			}},
		{"Minitilda's Claw",255,255,0,AI_MattieClaw,	// the left claw (her right)
		 20,16,500,1000,"expgraphics/miniclawl.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_SPRITEBOX|MF_NOGRAV|MF_ENEMYWALK|MF_FREEWALK,
		 MT_BITS,MG_NEW,MC_GREY,
			{
				{0,1,2,3,2,1,255},	// idle
				{0,255},	// move
				{7,8,9,10,11,12,13,14,15,255},	// claw slash
				{1,2,0,3,4,3,2,0,3,4,3,0,1,4,2,4,3,1,2,255},	// die
				{4,5,6,6,6,6,6,6,6,5,4,255},	// block (A1)
				{7,8,9,10,11,12,13,14,15,255},	// claw slash
				{7,8,9,10,11,12,13,14,15,255},	// claw slash
				{0,255},
				{0,255},
			}},
		{"Minitilda's Claw",255,255,0,AI_MattieClaw,	// the right claw (her left)
		 20,16,500,1000,"expgraphics/miniclawr.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_SPRITEBOX|MF_NOGRAV|MF_ENEMYWALK|MF_FREEWALK,
		 MT_BITS,MG_NEW,MC_GREY,
			{
				{0,1,2,3,2,1,255},	// idle
				{0,255},	// move
				{7,8,9,10,11,12,13,14,15,255},	// claw slash
				{1,2,0,3,4,3,2,0,3,4,3,0,1,4,2,4,3,1,2,255},	// die
				{4,5,6,6,6,6,6,6,6,5,4,255},	// block (A1)
				{7,8,9,10,11,12,13,14,15,255},	// claw slash
				{7,8,9,10,11,12,13,14,15,255},	// claw slash
				{0,255},
				{0,255},
			}},
		{"Minitilda's Tail",4,3,0,AI_MattieTail,	// her tail
		 80,16,1000,1000,"expgraphics/matminitail.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_FACECMD|MF_NOGRAV|MF_ENEMYWALK|MF_FREEWALK,
		 MT_BITS,MG_NEW,MC_BLUE,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,1,2,3,2,1,255},	// fire
				{1,2,0,3,2,0,3,3,0,1,2,3,1,2,255},	// die
				{0,1,2,3,2,1,255},	// fire
				{0,1,2,3,2,1,255},	// fire
				{0,255},
				{0,255},
				{0,255},
			}},
		{"Marshmallow Man",255,255,0,AI_MarshmallowMan,	//thank you jamul!!
		 10,11,80,1000,"expgraphics/mallowman.jsp",0,0,
		 MT_EVIL|MT_WACKY,MG_NEW,MC_GREY,
			{
				{0,255},	// idle
				{0,1,2,1,0,3,4,3,255},	// move
				{0,5,6,6,5,255}, //shoot
				{7,7,8,8,9,9,10,10,255}, //dead!
				{0,5,6,6,5,255}, //shoot
				{0,5,6,6,5,255}, //shoot
				{0,5,6,6,5,255}, //shoot
				{0,5,6,6,5,255}, //shoot
				{0,5,6,6,5,255}, //shoot
			}},
		{"Toasted Marsh",0,4,2,AI_MarshmallowMan,	//thank you jamul!!
		 10,11,90,1000,"expgraphics/mallowman.jsp",0,0,
		 MT_EVIL|MT_WACKY,MG_NEW,MC_RED,
			{
				{0,255},	// idle
				{0,1,2,1,0,3,4,3,255},	// move
				{0,5,6,6,5,255}, //shoot
				{7,7,8,8,9,9,10,10,255}, //dead!
				{0,5,6,6,5,255}, //shoot
				{0,5,6,6,5,255}, //shoot
				{0,5,6,6,5,255}, //shoot
				{0,5,6,6,5,255}, //shoot
				{0,5,6,6,5,255}, //shoot
			}},
		{"Charge Coil",5,6,3,NULL,
		 10,8,3000,1,"llgraphics/lghtball.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_INVINCIBLE,
		 MT_EVIL|MT_TRAP,MG_NEW,MC_GREY,
			{
				{5,255},	// idle
				{5,255},	// move
				{5,4,3,2,1,0,0,0,0,0,0,0,255},	// attack (charge up)
				{0,255},	// die
			}},
		{"Junk",255,255,0,AI_Junk,
		 11,9,10,1,"llgraphics/junk.jsp",0,MF_ONEFACE|MF_FLYING|MF_FREEWALK|MF_ENEMYWALK,
		 0,MG_NEW,MC_BROWN,
			{
				{0,1,2,3,4,5,6,7,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{0,255},	// die
			}},
		{"Pygmy Firedancer",6,4,-2,AI_Firedancer, //good to go
		 10,50,45,100,"graphics/pygmy.jsp",0,0,
		 MT_EVIL|MT_PYGMY|MT_HUMAN,MG_NEW,MC_RED,
			{
				{0,0,0,0,0,0,0,0,0,0,1,2,3,2,1,0,0,0,4,5,6,5,4,255},	// idle
				{8,9,10,9,8,7,11,12,13,12,11,7,255},	// move
				{29,30,31,31,32,33,34,35,36,255},	// attack
				{37,38,39,40,41,42,43,44,45,46,255},	// die
				{14,15,16,17,18,19,20,16,17,18,19,15,14,255},	// (A1) bored (spin spear)
				{21,22,23,24,25,26,26,26,27,28,27,26,27,28,27,26,255}, // (A2) bored (yawn)
				{47,48,49,48,47,0,47,48,49,48,47,255},	// (A3) angry, spotted Bouapha
				{0,47,48,49,49,49,48,47,47,0,255}, // (A4) jump!!
				{0,255},
			}},
		{"Atomic Zombie",0,6,6,AI_SDZL2, //good to go
		 40,48,1200,35000,"graphics/sdzl.jsp",0,MF_NOMOVE|MF_ONEFACE,
		 MT_EVIL|MT_ZOMBIE|MT_BOSS,MG_NEW,MC_VIOLET,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move (to the right)
				{28,29,30,31,31,31,31,31,32,33,34,35,36,37,38,39,255},	// attack=slash
				{40,41,42,43,43,43,43,43,44,45,46,47,47,47,47,47,47,255},	// die
				{5,6,5,0,7,8,7,0,255},	// A1=move left
				{9,10,11,12,12,12,12,12,12,13,14,15,16,16,17,18,255},	// A2=breathe
				{19,20,21,21,22,23,24,25,25,25,25,25,25,26,27,255},	// A3=ground pound
				{39, 38, 37, 36, 35, 35, 35, 36, 36, 36, 37, 37, 37, 38, 39, 255}, // A4=atomic dodge
				{27, 26, 25, 25, 24, 23, 22, 22, 21, 21, 20, 19, 255}, // A5=atomic throw
			}},
		{"Deadly Burner",0,4,-2,AI_Burner, //good to go
		 18,2,30,300,"graphics/burner.jsp",0,MF_NOSHADOW|MF_ONEFACE|MF_NOMOVE|MF_INVINCIBLE,
		 MT_EVIL|MT_TRAP,MG_NEW,MC_RED,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{0,255},		// die
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
			}},
		{"LunaChick Prime",6,2,0,AI_RockTrooper,
		 40,23,800,1000,"!78",0,MF_NOMOVE,
		 MT_EVIL|MT_HIGHTECH,MG_NEW,MC_RED,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{10,11,11,11,12,13,14,14,15,255},	// attack
				{16,17,18,19,20,21,22,22,22,255},	// die
				{5,6,7,7,8,9,255},	// A1=lob grenade
				{10,11,11,11,12,13,14,14,15,255},	// attack
				{10,11,11,11,12,13,14,14,15,255},	// attack
				{10,11,11,11,12,13,14,14,15,255},	// attack
				{10,11,11,11,12,13,14,14,15,255},	// attack
			}},
		{"Young Loony",255,255,0,NULL,
		 11,25,128,0,"llgraphics/kidloony.jsp",0,MF_WATERWALK,
		 MT_WEIRD,MG_NEW,MC_VIOLET,
			{
				{0,255},	// idle
				{1,2,3,4,3,2,1,0,5,6,7,8,7,6,5,0,255},	// move
				{9,10,11,12,13,13,14,15,16,255},	// attack
				{0,0,0,0,0,0,17,18,19,20,255},		// die
				{9,11,13,15,16,255},	// use item
				{21,21,22,22,23,23,24,24,24,24,24,24,5,1,255},	// bored
				{2,4,2,0,6,8,6,0,2,4,2,0,0,0,18,19,20,255},	// watery death
				{21,21,22,22,23,23,24,24,24,24,24,24,5,1,255},	// a4
				{0,255},
			}},
		{"Flying Pumpkin Patch",1,4,-2,AI_FlyingPatch,
		 16,17,150,1000,"graphics/patch.jsp",0,MF_NOMOVE|MF_ENEMYWALK|MF_ONEFACE|MF_FLYING,
		 MT_EVIL|MT_VEGGIE|MT_PUMPKIN|MT_GENERATE|MT_FLYING,MG_NEW,MC_RED,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,0,2,0,1,2,1,0,1,0,2,1,2,1,0,2,1,2,255},	// attack
				{11,12,13,14,15,16,255},	// die
				{3,4,5,6,7,8,9,10,255},	// A1=summon
				{1,2,0,2,0,1,2,1,0,1,0,2,1,2,1,0,2,1,2,255},	// attack
				{1,2,0,2,0,1,2,1,0,1,0,2,1,2,1,0,2,1,2,255},	// attack
				{1,2,0,2,0,1,2,1,0,1,0,2,1,2,1,0,2,1,2,255},	// attack
				{1,2,0,2,0,1,2,1,0,1,0,2,1,2,1,0,2,1,2,255},	// attack
			}},
		{"Winter Gourd",1,3,4,AI_WinterGourd, //good to go
		 8,14,10,100,"graphics/squash.jsp",0,0,
		 MT_EVIL|MT_VEGGIE|MT_PUMPKIN|MT_ARCTIC,MG_NEW,MC_BLUE,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,9,8,7,255},	// attack
				{11,12,13,255},	// die
				{7,8,9,10,9,8,7,255},	// attack
				{7,8,9,10,9,8,7,255},	// attack
				{7,8,9,10,9,8,7,255},	// attack
				{7,8,9,10,9,8,7,255},	// attack
				{7,8,9,10,9,8,7,255},	// attack
			}},
		{"Count Eshkah",3,6,-3,AI_CountEshkah, //good to go
		 20,13,500,800,"llgraphics/elder.jsp",0,MF_ENEMYWALK,
		 MT_EVIL|MT_BOSS|MT_VAMPIRE,MG_NEW,MC_VIOLET,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,4,4,4,5,6,255},	// attack (shove)
				{0,0,0,0,0,0,255},		// die
				{7,8,9,9,9,9,8,8,7,7,255},	// summon pose = A1
				{10,11,12,12,12,11,10,255},	// point = A2
				{10,11,12,12,12,11,10,255},	// point = A3
				{1,2,3,4,4,4,4,5,6,255},	// shove = A5
				{1,2,3,4,4,4,4,4,4,4,4,5,6,255},	// shove = A4
			}},
		{"Deathstone",2,7,-4,AI_RollerKiller, //good to go
		 35,11,500,1000,"graphics/roller.jsp",0,MF_NOMOVE|MF_ONEFACE,
		 MT_EVIL|MT_HIGHTECH|MT_TRAP,MG_NEW,MC_AQUA,
			{
				{0,255},	// idle
				{1,2,3,4,5,0,255},	// move (roll down the screen)
				{5,4,3,2,1,0,255},	// attack=roll up the screen
				{0,0,255},	// die
				{6,7,8,9,10,0,255},	// A1=roll to the right
				{10,9,8,7,6,0,255},	// A2=roll to the left
				{5,4,3,2,1,0,255},	// attack=roll up the screen
				{0,255},
				{0,255},
			}},
		{"Happy Stick Snowman",255,255,0,AI_StickSnowman, //fix sprite
		 11,6,40,60,"expgraphics/stksnow.jsp",0,MF_ONEFACE,
		 MT_EVIL|MT_WACKY|MT_ARCTIC,MG_NEW,MC_AQUA,
			{
				{0,255},	// idle
				{0,1,0,1,255},	// move=walk
				{2,2,2,2,2,2,255},	// attack=shoot
				{3,4,5,6,255},	// die
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
			}},
		{"Happy Stick Fish",255,255,0,AI_StickFish, //actually program
		 11,6,40,60,"expgraphics/stkfish.jsp",0,MF_ONEFACE|MF_FLYING,
		 MT_EVIL|MT_WACKY|MT_UNDERSEA|MT_ANIMAL,MG_NEW,MC_AQUA,
			{
				{0,1,255},	// idle
				{0,1,255},	// move=walk
				{2,2,2,2,2,2,255},	// attack=shoot
				{3,3,4,4,5,5,255},	// die
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
				{0,255},	// unused
			}},
		{"Yerfdog",0,0,0,AI_Yerfboss, //good to go
		 28,31,500,1000,"expgraphics/yerfboss.jsp",0,MF_FREEWALK|MF_ENEMYWALK|MF_WATERWALK|MF_ONEFACE,
		 MT_EVIL|MT_ANIMAL|MT_BOSS,MG_NEW,MC_GREY,
			{
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// idle
				{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255},	// move
				{17,18,19,20,21,22,23,24,25,26,255},	// attack
				{0,0,27,27,28,28,29,29,30,30,255},	// die
				{17,18,19,20,21,22,23,24,25,26,255},	// unused
				{17,18,19,20,21,22,23,24,25,26,255},	// unused
				{17,18,19,20,21,22,23,24,25,26,255},	// unused
				{17,18,19,20,21,22,23,24,25,26,255},	// unused
				{17,18,19,20,21,22,23,24,25,26,255},	// unused
			}},
		{"Happy Stick Supreme",1,0,0,AI_Shapeshifter, //good to go
		 80,29,1500,10000,"expgraphics/stickboss.jsp",0,MF_WATERWALK|MF_ENEMYWALK|MF_ONEFACE,
		 MT_EVIL|MT_BOSS,MG_NEW,MC_GREY,
			{
				{0,255},	// idle
				{0,255},	// move
				{3,4,5,6,7,8,9,255},	// attack=chomp
				{3,3,4,4,5,5,6,6,7,7,8,254,8,254,9,254,9,255},	// die
				{6,7,7,7,6,0,8,9,9,9,8,255},	// A1=show off muscles
				{3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,255},	// A2=sadness
				{1,0,2,0,1,0,2,0,1,0,2,0,0,0,3,4,5,255},	// watery death
				{0,255},
				{0,255},
			}},
		{"The Eldritch",2,0,-12,AI_Sphinx2, //tentacles to arms
		 80,11,1200,5000,"graphics/sphinx.jsp",0,MF_ONEFACE|MF_NOMOVE|MF_WATERWALK,
		 MT_EVIL|MT_BOSS|MT_DESERT,MG_NEW,MC_GREY,
			{
				{0,255},	// idle
				{0,255},	// move
				{10,9,8,7,6,5,5,5,5,5,5,5,5,5,5,5,4,4,255},	// attack=summon mummy
				{10,10,10,10,10,10,10,10,10,255},	// die
				{4,5,6,7,7,6,7,7,7,6,7,8,9,10,255}, // A1=sneeze
				{1,2,3,255},	// attack=summon mummy
				{1,2,3,255},	// attack=summon mummy
				{1,2,3,255},	// attack=summon mummy
			}},
		{"Thing Crimson",1,4,-4,AI_Thing2, //good to go
		 64,11,1100,5000,"graphics/thething.jsp",0,MF_NOMOVE|MF_AQUATIC|MF_WATERWALK|MF_ENEMYWALK|MF_ONEFACE,
		 MT_EVIL|MT_THINGY|MT_BOSS,MG_NEW,MC_RED,
			{
				{2,255},	// idle
				{2,255},	// move
				{3,4,5,6,7,8,9,10,10,10,9,8,7,6,5,4,3,255},	// attack=tongue
				{2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,255},	// die
				{1,0,1,255},	// A1=blink
				{2,2,1,1,0,0,0,0,0,0,0,0,1,1,255},	// a2 = open eyes
				{3,4,5,6,7,8,9,10,10,10,9,8,7,6,5,4,3,255},	// attack=tongue
				{0,255},
				{0,255},
			}},
		{"Hans Solo",1,5,4,AI_Yeti3, //make better moves
		 48,40,500,5000,"graphics/yeti.jsp",0,MF_NOMOVE,
		 MT_EVIL|MT_BOSS|MT_ARCTIC|MT_ANIMAL,MG_NEW,MC_GREY,
			{
				{0,255},	// idle
				{0,1,2,1,0,3,4,3,255},	// move
				{14,15,16,17,18,19,20,21,21,21,21,21,21,21,21,22,23,255},	// attack=snowball
				{35,36,37,38,39,39,39,39,39,39,39,39,39,39,39,39,39,39,255},	// die
				{28,29,30,31,32,33,33,33,34,34,255},	// A1=ground pound
				{5,6,7,8,9,10,11,12,13,255},	//A2=death yodel
				{24,25,26,27,26,25,26,27,26,25,26,27,26,25,24,255},	// A3=wave hello
				{0,255},
				{0,255},
			}},
		{"Eldritch Arm",2,0,-12,AI_SphinxArm,	// arm1
		 30,10,600,2000,"graphics/sphinxarm1.jsp",0,MF_ONEFACE|MF_NOMOVE|MF_SPRITEBOX|MF_ENEMYWALK,
		 MT_BITS,MG_NEW,MC_BROWN,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,7,7,8,9,255},	// attack=swipe
				{0,1,2,3,2,3,4,3,2,3,4,3,254,4,254,3,254,4,255},	// die
				{1,2,3,4,5,6,7,7,8,9,255},	// attack=swipe
				{1,2,3,4,5,6,7,7,8,9,255},	// attack=swipe
				{1,2,3,4,5,6,7,7,8,9,255},	// attack=swipe
				{1,2,3,4,5,6,7,7,8,9,255},	// attack=swipe
			}},
		{"Eldritch Arm",2,0,-12,AI_SphinxArm,	// arm2
		 30,10,600,2000,"graphics/sphinxarm2.jsp",0,MF_ONEFACE|MF_NOMOVE|MF_SPRITEBOX|MF_ENEMYWALK,
		 MT_BITS,MG_NEW,MC_GREY,
			{
				{0,255},	// idle
				{0,255},	// move
				{1,2,3,4,5,6,7,7,8,9,255},	// attack=swipe
				{0,1,2,3,2,3,4,3,2,3,4,3,254,4,254,3,254,4,255},	// die
				{1,2,3,4,5,6,7,7,8,9,255},	// attack=swipe
				{1,2,3,4,5,6,7,7,8,9,255},	// attack=swipe
				{1,2,3,4,5,6,7,7,8,9,255},	// attack=swipe
				{1,2,3,4,5,6,7,7,8,9,255},	// attack=swipe
			}},
		{"Crimson Tentacle",1,4,-4,AI_ThingTentacle,
		 32,1,600,100,"graphics/thingtent.jsp",0,MF_NOMOVE|MF_FREEWALK|MF_NOGRAV|MF_ENEMYWALK,
		 MT_BITS,MG_NEW,MC_RED,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{0,0,0,0,0,255},	// die
				{0,255},
				{0,255},
				{0,255},
				{0,255},
				{0,255},
			}},
		{"Crimson Tentacle",1,4,-4,AI_ThingTentacle,	// tentacle tip
		 32,1,600,100,"graphics/thingtent2.jsp",0,MF_NOMOVE|MF_FREEWALK|MF_NOGRAV|MF_ENEMYWALK,
		 MT_BITS,MG_NEW,MC_RED,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{0,0,0,0,0,255},	// die
				{0,255},
				{0,255},
				{0,255},
				{0,255},
				{0,255},
			}},
		{"The Ascended",255,255,4,AI_Kongor2, //re-examine
		 80,32,750,2000,"expgraphics/kongorghost.jsp",0,MF_NOMOVE|MF_NOSHADOW|MF_ONEFACE|MF_GHOST|MF_ENEMYWALK|MF_FREEWALK|MF_WALLWALK,
		 MT_EVIL|MT_ANIMAL|MT_BOSS|MT_PYGMY,MG_NEW,MC_GLOW,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,255},	// move (left)
				{7,8,9,10,10,9,8,7,255},	// attack (left pound)
				{15,16,17,24,25,26,27,28,29,30,31,255},	// die
				{4,5,6,5,4,0,255}, // A1=move (right)
				{11,12,13,14,14,13,12,11,255}, // A2=right pound
				{15,16,17,18,19,20,19,18,21,22,23,22,21,18,19,20,19,18,255}, // A3=chest pound
				{21,22,23,22,21,17,16,15,255}, // A4=finish chest pound
				{7,8,9,10,10,9,8,7,255},	// attack (left pound)
			}},
		{"Vamp Jr.",6,3,2,AI_Vampire, //good to go
		 12,13,30,500,"graphics/vampire.jsp",0,0,
		 MT_EVIL|MT_VAMPIRE|MT_SPOOKY,MG_NEW,MC_BLUE,
			{
				{1,2,1,0,255},	// idle
				{1,2,1,0,255},	// move
				{3,4,5,6,7,8,9,255},	// attack
				{12,11,10,255},		// die
				{3,4,5,6,7,8,9,255},	// attack
				{3,4,5,6,7,8,9,255},	// attack
				{3,4,5,6,7,8,9,255},	// attack
				{3,4,5,6,7,8,9,255},	// attack
				{3,4,5,6,7,8,9,255},	// attack
			}},
		{"Centiboomkin",2,4,5,AI_CentiBody,	// body
		 14,5,20,200,"graphics/cpbody.jsp",0,MF_ENEMYWALK,
		 MT_BITS,MG_NEW,MC_RED,
			{
				{0,255},	// idle
				{1,2,1,0,3,4,3,0,255},	// move
				{0,255},	// attack
				{0,0,0,255},	// die
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
			}},
		{"Centiboomkin",2,4,5,AI_CentiHead,	// head
		 14,1,40,500,"graphics/cphead.jsp",0,MF_ENEMYWALK,
		 MT_EVIL|MT_VEGGIE|MT_PUMPKIN|MT_MINIBOSS|MT_DESERT,MG_NEW,MC_RED,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{0,0,0,255},	// die
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
			}},
		{"Freezer",0,7,10,AI_Snowblower,  //good to go
		 18,2,30,300,"!66",0,MF_NOSHADOW|MF_ONEFACE|MF_NOMOVE|MF_INVINCIBLE,
		 MT_EVIL|MT_TRAP|MT_ARCTIC,MG_NEW,MC_AQUA,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{0,255},	// die
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
				{0,255},	// attack
			}},
		{"MultiShroom",1,6,4,AI_MultiShroom,  //good to go
		 15,23,30,50,"graphics/shroom.jsp",0,0,
		 MT_EVIL|MT_VEGGIE|MT_CAVES|MT_TRAP|MT_FOREST,MG_NEW,MC_VIOLET,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,255},	// attack
				{15,16,17,18,19,20,21,22,255},	// die
				{22,21,20,19,18,17,16,15,255},	// a1 (revive)
				{2,8,9,10,11,12,13,14,255},	// attack
				{7,8,9,10,11,12,13,14,255},	// attack
				{0,255},
				{0,255},
			}},
		{"Wacky Wrapper",0,3,2,AI_Mumble, //fix speed
		 15,28,30,100,"!39",0,MF_NOMOVE,
		 MT_EVIL|MT_SPOOKY|MT_DESERT|MT_ASYLUM,MG_NEW,MC_BLUE,
			{
				{0,255},	// idle
				{1,2,3,4,5,0,6,7,8,9,10,0,255},	// move
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{19,20,21,22,23,24,25,26,27,255},	// die
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
				{11,12,13,14,15,16,17,18,255},	// attack=head crush
			}},
		{"Pusher:Horizontal",0,3,1,AI_Pusher,
		 18,2,30,300,"!66",0,MF_NOSHADOW|MF_ONEFACE|MF_NOMOVE|MF_NOHIT|MF_INVINCIBLE|MF_ENEMYWALK|MF_FREEWALK,
		 MT_EVIL|MT_TRAP,MG_NEW,MC_BLUE,
			{
				{254,254,254,255},	// idle
				{254,254,254,255},	// move
				{254,254,254,255},	// attack
				{254,255},		// die
				{254,254,254,255},	// move
				{254,254,254,255},	// move
				{254,254,254,255},	// move
				{254,254,254,255},	// move
				{254,254,254,255},	// move
			}},
		{"Pusher:Vertical",0,3,1,AI_Pusher,
		 18,2,30,300,"!66",0,MF_NOSHADOW|MF_ONEFACE|MF_NOMOVE|MF_NOHIT|MF_INVINCIBLE|MF_ENEMYWALK|MF_FREEWALK,
		 MT_EVIL|MT_TRAP,MG_NEW,MC_BLUE,
			{
				{254,254,254,255},	// idle
				{254,254,254,255},	// move
				{254,254,254,255},	// attack
				{254,255},		// die
				{254,254,254,255},	// move
				{254,254,254,255},	// move
				{254,254,254,255},	// move
				{254,254,254,255},	// move
				{254,254,254,255},	// move
			}},
		{"Medic Cap",5,1,5,AI_ShroomLord,
		 15,14,40,300,"graphics/shrmlord.jsp",0,0,
		 MT_EVIL|MT_VEGGIE|MT_FOREST,MG_NEW,MC_GREEN,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,7,8,8,8,9,10,11,12,12,13,13,255},	// spit spores
				{0,0,255},	// die
				{7,7,8,8,8,9,10,11,12,12,13,13,255},	// spit spores
				{7,7,8,8,8,9,10,11,12,12,13,13,255},	// spit spores
				{7,7,8,8,8,9,10,11,12,12,13,13,255},	// spit spores
				{7,7,8,8,8,9,10,11,12,12,13,13,255},	// spit spores
				{7,7,8,8,8,9,10,11,12,12,13,13,255},	// spit spores
			}},
		{"BUDDY PUMPKIN!",255,255,4,AI_Pumpkin,
		 11,6,5,500,"graphics/pumpkin.jsp",0,0,
		 MT_GOOD|MT_PUMPKIN|MT_VEGGIE|MT_FOREST,MG_NEW,MC_BROWN,
			{
				{0,255},	// idle
				{1,2,3,4,5,0,255},	// move
				{1,2,3,4,5,0,255},	// move
				{0,0,255},	// die (just long enough to spew shrapnel)
				{1,2,3,4,5,0,255},	// move
				{1,2,3,4,5,0,255},	// move
				{1,2,3,4,5,0,255},	// move
				{1,2,3,4,5,0,255},	// move
				{1,2,3,4,5,0,255},	// move
			}},
		{"Death Cart",2,4,2,AI_MinecartBad,
		 11,8,100,500,"graphics/minecar.jsp",0,MF_NOMOVE|MF_FREEWALK,
		 MT_VEHICLE|MT_TRAP,MG_NEW,MC_RED,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,0,255},	// move
				{1,2,3,4,3,2,1,0,255},	// attack
				{1,2,3,4,5,6,7,6,5,4,3,2,1,0,255},	// die
				{1,2,3,4,3,2,1,0,255},	// attack
				{1,2,3,4,3,2,1,0,255},	// attack
				{1,2,3,4,3,2,1,0,255},	// attack
				{1,2,3,4,3,2,1,0,255},	// attack
				{1,2,3,4,3,2,1,0,255},	// attack
			}},
		{"Bone Rider",4,3,-2,AI_Bonehead,
		 8,38,25,50,"graphics/skeleton.jsp",0,MF_ENEMYWALK|MF_FREEWALK,
		 MT_SPOOKY|MT_FOREST|MT_CAVES|MT_SKELETON|MT_EVIL,MG_NEW,MC_GREY,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{20,21,22,23,24,255},	// attack
				{7,8,9,10,11,12,13,14,15,16,255},		// die
				{25,26,27,28,29,30,255},	// chomp/spit projectile
				{31,32,32,32,31,255},	// point at bouapha
				{16,15,14,13,12,11,10,9,8,7,255},	// resurrect=A3
				{0,255},
				{0,255},
			}},
		{"Cart Jockey",6,5,-2,AI_Pygmy,
		 10,50,35,100,"graphics/pygmy.jsp",0,MF_ENEMYWALK|MF_FREEWALK,
		 MT_EVIL|MT_PYGMY|MT_HUMAN,MG_DRL,MC_VIOLET,
			{
				{0,0,0,0,0,0,0,0,0,0,1,2,3,2,1,0,0,0,4,5,6,5,4,255},	// idle
				{8,9,10,9,8,7,11,12,13,12,11,7,255},	// move
				{29,30,31,31,32,33,34,35,36,255},	// attack
				{37,38,39,40,41,42,43,44,45,46,255},	// die
				{14,15,16,17,18,19,20,16,17,18,19,15,14,255},	// (A1) bored (spin spear)
				{21,22,23,24,25,26,26,26,27,28,27,26,27,28,27,26,255}, // (A2) bored (yawn)
				{47,48,49,48,47,0,47,48,49,48,47,255},	// (A3) angry, spotted Bouapha
				{0,255},
				{0,255},
			}},
		{"Minecart Chest",2,4,-4,AI_Chest,
		 16,5,100,1000,"graphics/chest.jsp",0,MF_NOMOVE|MF_ONEFACE|MF_ENEMYWALK|MF_FREEWALK,
		 MT_TRAP|MT_UNDERSEA,MG_SUPREME,MC_BROWN,
			{
				{0,255},	// idle
				{0,255},	// move
				{0,255},	// attack
				{1,2,3,4,255},	// die
				{1,2,3,4,255},	// die
				{1,2,3,4,255},	// die
				{1,2,3,4,255},	// die
				{1,2,3,4,255},	// die
				{1,2,3,4,255},	// die
			}},
		{"Super Bombie",0,4,5,AI_SuperBombie, //re-examine
		 36,38,128,2500,"graphics/suprzmbe.jsp",0,MF_NOMOVE,
		 MT_EVIL|MT_MINIBOSS|MT_SPOOKY|MT_ZOMBIE,MG_NEW,MC_RED,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{17,18,19,20,21,22,23,24,25,255},	// attack=punch
				{9,10,11,12,13,14,15,15,15,15,15,15,255},	// die
				{7,8,9,10,11,11,11,12,12,13,13,13,14,14,14,15,16,16,255},	// A1=leap
				{26,27,28,29,30,255},	// A2=attempted grab
				{27,31,32,33,34,35,34,36,37,36,34,35,34,36,37,36,34,35,34,33,31,27,255},	//A3= grab n' pound
				{0,255},
				{0,255},
			}},
	};

#ifndef MONSTERAI_H
#define MONSTERAI_H

class Guy;
class Map;
struct world_t;

// ai functions for each monster type
void AI_Bonehead(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Bat(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Spider(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_BigSpider(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Zombie(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_EggSac(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MamaSpider(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Pygmy(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Serpent(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Ginger(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Pumpkin(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_BabyThing(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Moss(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MossGrande(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Magmazoid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Shroom(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Mush(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_SuperZombie(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_StickMan(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_BabySeal(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Isozoid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Snowguy(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Penguin(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Zomboni(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Geozoid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Mumble(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Djinni(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MagicLamp(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Cactus(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Roller(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Lich(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_DustDevil(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MechaBouapha(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Freakazoid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_CentiBody(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_CentiHead(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Wacko(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Boiler(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_GreatPumpkin(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Ultrazoid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Santa(Guy *me,Map *map,world_t *world,Guy *goodguy);

	//Matilda's AIs
void AI_MattieBrain(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MattieSkullOrHead(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MattieClaw(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MattieTail(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MattieBody(Guy *me,Map *map,world_t *world,Guy *goodguy);
	// The Thing's AIs
void AI_TheThing(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_ThingTentacle(Guy *me,Map *map,world_t *world,Guy *goodguy);
	// Sven & Bjorn's AI
void AI_Yeti(Guy *me,Map *map,world_t *world,Guy *goodguy);
	// Sphinxter's AIs
void AI_SphinxArm(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Sphinx(Guy *me,Map *map,world_t *world,Guy *goodguy);
	// Asylum bosses' AIs
void AI_DrLunatic(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_SDZL(Guy *me,Map *map,world_t *world,Guy *goodguy);

// expansion monsters
void AI_MineCart(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Raft(Guy *me,Map *map,world_t *world,Guy *goodguy);

void AI_Vampire(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Coffin(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Ghost(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Burner(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Lefty(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Pygmy2(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Pygmy3(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Pumpkinstein(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Knight(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Triceroid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Countess(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_AlienEgg(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_BabyAlien(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Alien(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Robopumpkin(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_ShockTrooper(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Minibot(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Meaniebot(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Robofactory(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Turret(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Bunny(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Kongor(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Squash(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_UltraPygmy(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_LoonyShip(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_LoonyGun(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_LoonyBot(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_LoonyCore(Guy *me,Map *map,world_t *world,Guy *goodguy);

// fun pack monsters
void AI_GoodTurret(Guy *me,Map *map,world_t *world,Guy *goodguy);

void AI_Mattie2Brain(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Mattie2SkullOrHead(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Mattie2Tail(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Mattie2Body(Guy *me,Map *map,world_t *world,Guy *goodguy);

void AI_BuddyBunny(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_PygmyQueen(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Jalapeno(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Generator(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Shark(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MadBug(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Wizard(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_EvilClone(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Bob(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MultiMoss(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MossRapido(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Snowball(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Snowblower(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_XenoMama(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Gnome(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Nobody(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_BigPumpkin(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_RockTrooper(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_CrazyBone(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Creepazoid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Lazybones(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_StareyBat(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Madman(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_PygmyAcct(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Crazypants(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Yerfdog(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_EliteSeal(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Cone(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_PygmyDiver(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Puffyfish(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Yeti2(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Spikey(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Bombot(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_FollowBunny(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Autozoid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Chest(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Jellofish(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_KingCone(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Mailboxer(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Piranha(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Yugo(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Punkin(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_SeaCuke(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Sneaky2(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Starfish(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Underzoid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_WetSeal(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_WetWilly(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_CrabPuff(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Patch(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Patty(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Dozer(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MutantZombie(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Whatsit(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_ShockGhost(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Mechazoid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Sucker(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Blower(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Beetle(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Scarab(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Frostbiter(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_UnderMagmazoid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_ParkedCar(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Traffic(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_ShroomLord(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_BigBat(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_HugeBat(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Werewolf(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Patrol(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Weatherman(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_HunterEgg(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_PunkBunny(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Skeleton(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_Microzoid(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_HotShroom(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_MiniWacko(Guy *me,Map *map,world_t *world,Guy *goodguy);
void AI_JackFrost(Guy *me,Map *map,world_t *world,Guy *goodguy);

#endif

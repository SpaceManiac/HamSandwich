#include "cards.h"
#include "mgldraw.h"
#include "control.h"
#include "display.h"
#include "player.h"
#include "music.h"
#include "dialogbits.h"
#include "textdialog.h"
#include "playlist.h"
#include "nameentry.h"
#include "yesnodialog.h"
#include "recordbook.h"
#include "scanner.h"
#include "shop.h"
#include "lsdir.h"

#if __linux__
#include <unistd.h>
#endif

static byte *backgd;
sprite_set_t *plSpr;
sprite_set_t *backSpr;
sprite_set_t *frontSpr;
static char msBright,msDBright;
static int msx,msy;
int cardPage;
dword monsSel;
int time;
int time2;
int time3;
int numObtained;

#define CARD_WIDTH	58
#define CARD_HEIGHT	74

#define PBTN_HEIGHT	19
#define BTN_NEXTPG	1
#define BTN_PREVPG	2
#define BTN_EXITPG	3

int MAX_PAGES = floor(NUM_CARDS/16)+1;

typedef struct cardButton_t
{
	int x,y;
	int wid;
	char txt[32];
	byte id;
} cardButton_t;

static cardButton_t btn[]={
	{20,56,122,"Next Page",BTN_NEXTPG},
	{20,86,122,"Previous Page",BTN_PREVPG},
	{20,116,122,"Exit",BTN_EXITPG},
};
#define NUM_CARD_BTNS	3

cardsType_t cardsType[NUM_CARDS]=
{
	{MONS_BOUAPHA,2}, 		//Bouapha
	{MONS_BONEHEAD,0},	 	//Bonehead
	{MONS_BAT,0}, 			//Scary Bat
	{MONS_SPIDER,0},		//Eensy Weensy
	{MONS_BIGSPDR,0},		//Spitter
	{MONS_ZOMBIE,1},		//Zombie
	{MONS_EGGSAC,0},		//Egg Sac
	{MONS_MAMASPDR,1},		//Mama Spider
	{MONS_PYGMY,0},			//Pygmy
	{MONS_SERPENT,0},		//Aquazoid
	{MONS_MATHEAD,2},		//Matilda - head counts as kill (Must start at Stage 1)
	{MONS_GINGER,1},		//Ninjabread Man
	{MONS_PUMPKIN,1},		//PUMPKIN!
	{MONS_BABYTHING,0},		//Thingie
	{MONS_MOSS,0},			//Mucho Moss
	{MONS_MOSSGRANDE,1},	//Moss Grande
	{MONS_MAGMAZOID,1},		//Magmazoid
	{MONS_SHROOM,0},		//Shroom
	{MONS_MUSH,1},			//Mush the Shroom
	{MONS_THING,2},			//The Thing
	{MONS_SUPERZOMBIE,1},	//Super Zombie
	{MONS_STICKMAN,3},		//Happy Stick Man
	{MONS_BABYSEAL,0},		//Baby SEAL
	{MONS_ISOZOID,0},		//Cryozoid
	{MONS_SNOWGUY,0},		//Snowguy
	{MONS_PENGUIN,0},		//Pengulon
	{MONS_ZOMBONI,0},		//Zomboni
	{MONS_SVEN,2},			//Sven
	{MONS_BJORN,2},			//Bjorn
	{MONS_GEOZOID,0},		//Geozoid
	{MONS_MUMBLE,0},		//Mumble
	{MONS_DJINNI,0},		//Djinni
	{MONS_LAMP,0},			//Magic Lamp
	{MONS_CACTUS,0},		//Cacton
	{MONS_ROLLER,1},		//Roly Poly - 100% chance of gaining card
	{MONS_LICH,2},			//Richie Lich
	{MONS_DUSTDEVIL,1},		//Dust Devil
	{MONS_MECHABOUAPHA,1},	//MechaBouapha
	{MONS_SPHINX,2},		//Sphinxter
	{MONS_FREAKAZOID,0},	//Freakazoid
	{MONS_CENTIHEAD,2},		//Centipumpkin
	{MONS_WACKO,0},			//Wacko
	{MONS_BOILER,2},		//The Boiler
	{MONS_GREATPK,2},		//Great Pumpkin
	{MONS_ULTRAZOID,2},		//The Ultrazoid
	{MONS_DRL,2},			//Dr. Lunatic
	{MONS_SDZL,4},			//Super Duper Zombie
	{MONS_SANTA,2},			//Santa Claus
	{MONS_VAMPIRE,0},		//Vampire
	{MONS_GHOST,0},			//Spooky Ghost
	{MONS_LEFTY,0},			//Lefty
	{MONS_PYGMY2,0},		//Pygmy Hunter
	{MONS_PYGMY3,0},		//Pygmy Shaman
	{MONS_PKSTEIN,1},		//Pumpkinstein
	{MONS_KNIGHT,0},		//Stan Glass
	{MONS_TRICEROID,0},		//Triceroid
	{MONS_COUNTESS,2},		//Countess
	{MONS_ALIENEGG,0},		//Egg
	{MONS_BABYALIEN,0},		//Xenoid
	{MONS_ALIEN,0},			//Xeno Hunter
	{MONS_ROBOPK,1},		//Robopumpkin
	{MONS_SHOCKTR,1},		//Shock Trooper
	{MONS_ROBOT1,0},		//MiniBot
	{MONS_ROBOT2,0},		//Meaniebot
	{MONS_ROBOFACTORY,0},	//RoboFactory
	{MONS_TURRET,0},		//Turret
	{MONS_BUNNY,3},			//Doom Bunny
	{MONS_KONGOR,2},		//KONGOR!!
	{MONS_SQUASH,1},		//Squash
	{MONS_MINIPYGMY,1},		//UltraPygmy
	{MONS_LOONYBOT,2},		//LoonyBot 5000 (core)
	{MONS_LOONYSHIP,1},		//Loony Zoomer
	{MONS_FRIENDLY,0},		//Buddy Bunny
	{MONS_GOODTURRET,0},	//Good Turret
	{MONS_ALIENEGG2,0},		//Crazy Egg
	{MONS_MAT2HEAD,3},		//Matilda-X
	{MONS_SHAMAN2,2},		//Pygmy Queen
	{MONS_JALAPENO,1},		//Jalapeno
	{MONS_GENERATOR1,0},	//Generator
	{MONS_SHARK,1},			//Sneaky Shark
	{MONS_MADBUG,1},		//Mad Millenium Bug
	{MONS_WIZARD,1},		//Wacky Wizard
	{MONS_EVILCLONE,1},		//Evil Clone
	{MONS_BOB,2},			//Bob the Biscuit
	{MONS_MULTIMOSS,0},		//MultiMoss
	{MONS_MOSS2,0},			//Moss Rapido
	{MONS_SNOWBALL,1},		//Snowball
	{MONS_BOOMKIN,1},		//Boomkin
	{MONS_MUMBLE2,0},		//Manic Mumble
	{MONS_GOODROBOT,0},		//BuddyBot
	{MONS_GOODROBOT2,0},	//HelpyBot
	{MONS_XENOMAMA,0},		//Xeno Queen
	{MONS_ROLLER3,0},		//Rumbly Tumbly
	{MONS_DARKVAMP,0},		//Dark Vampire
	{MONS_GNOME,1},			//Grabby Gnome
	{MONS_TROOPER2,1},		//Rock Trooper
	{MONS_PUMPKIN2,0},		//Jacko
	{MONS_CRAZYBONE,1},		//Crazybone
	{MONS_CREEPAZOID,0},	//Creepazoid
	{MONS_LAZYBONE,1},		//LazyBones
	{MONS_STAREYBAT,0},		//Starey Bat
	{MONS_MADMAN,0},		//Madman
	{MONS_PYGMYACCT,1},		//Pygmy Accountant
	{MONS_CRAZYPANTS,2},	//Mr. Crazypants
	{MONS_ELITESEAL,0},		//Elite Seal
	{MONS_CONE,0},			//Creeping Cone
	{MONS_PYGMYDIVER,0},	//Pygmy Diver
	{MONS_PUFFYFISH,0},		//Puffyfish (+Rvs)
	{MONS_OLAF,2},			//Olaf
	{MONS_SPIKEY,1},		//Spikey
	{MONS_BOMBOT,1},		//BomBot
	{MONS_AUTOZOID,1},		//Autozoid
	{MONS_CHEST,3},			//Treasure Chest
	{MONS_JELLOFISH,0},		//Jellofish
	{MONS_KINGCONE,2},		//King Cone
	{MONS_MAILBOXER,0},		//Mailboxer
	{MONS_PIRANHA,0},		//Piranha
	{MONS_PUNKIN,0},		//Punkin
	{MONS_SEACUKE,1},		//Sea Cucumber
	{MONS_SNKYSHRK2,3},		//Sneaky Shark (Chest)
	{MONS_STARFISH,0},		//Starfish
	{MONS_UNDERZOID,0},		//Aquazoid: Underwater
	{MONS_WETSEAL,0},		//Wet Seal
	{MONS_WETWILLY,0},		//Wet Willy
	{MONS_CRABPUFF,0},		//Crab Puff
	{MONS_PATCH,1},			//Pumpkin Patch (+Fast)
	{MONS_PATCH3,0},		//Boomkin Patch
	{MONS_PATCH4,0},		//Squash Patch
	{MONS_PATTY,2},			//Crabby Patty
	{MONS_DOZER,2},			//Dozer the Dozerian
	{MONS_MUTANT,1},		//Mutant Zombie
	{MONS_WHATSIT,1},		//Whatsit
	{MONS_SHOCKGHOST,1},	//Shocking Horror
	{MONS_MECHAZOID,0},		//Mechazoid
	{MONS_BEETLE,0},		//The Beetles
	{MONS_SCARAB,2},		//Scary Scarab
	{MONS_FROSTBITER,0},	//Frostbiter
	{MONS_MISLTURRET,1},	//Missile Turret
	{MONS_DEATHTURRET,3},	//Death Turret
	{MONS_UNDERMAGMA,0},	//Magmazoid: Underwater
	{MONS_PARKEDCAR,0},		//Parked Car (both)
	{MONS_TRAFFIC,0},		//Traffic (both)
	{MONS_PLAYMECHA,2},		//Playable MechaBouapha
	{MONS_SHROOMLORD,1},	//Shroomlord
	{MONS_BIGBAT,1},		//Scarier Bat
	{MONS_HUGEBAT,2},		//Die Uberbat
	{MONS_WOLF,0},			//Werepuppy
	{MONS_WOLF2,1},			//Weremutt
	{MONS_PATROLLR,0},		//Patrol Mumble
	{MONS_DPATROLLR,1},	//Death Patrol
	{MONS_WEATHERMAN,1},	//Pygmy Weatherman
	{MONS_XENOEGG,1},		//Hunter Egg
	{MONS_PLAYSHROOM,2},	//Playable Shtupid Shroom
	{MONS_PUNKBUNNY,1},		//Punk Bunny
	{MONS_LUNACHICK,2},		//Playable LunaChick
	{MONS_SKELETON,1},		//Skele-TON
	{MONS_HOTSHROOM,1},		//Hot n Sour
	{MONS_MINIZOID,2},		//Microzoid
	{MONS_MINIWACKO,1},		//A Little Crazy
	{MONS_JACKFROST,2},		//Jack Frost
	{MONS_MYSTIC,2},		//Kid Mystic - playable character (earned by dying)
	{MONS_EYEGUY,0},		//Eye Guy
	{MONS_PEEPER,0},		//Peeper
	{MONS_TOWER,1},			//The Tower
	{MONS_LOOKEYLOO,2},		//The Lookey-loo
	{MONS_PINKEYE,1},		//Pinkeye
	{MONS_MICRO,3},			//Microfish
	{MONS_GOLEM,0},			//Grumpy Golem
	{MONS_BOBBY,4},			//Bobby Khan
	{MONS_CRAZYBUSH,0},		//Crazy Bush
	{MONS_OCTOPUS,0},		//Octon
	{MONS_INCABOT,0},		//Blockhead
	{MONS_INCAGEN,0},		//Blockhead Kiln
	{MONS_INCAGOLD,0},		//Megablockhead (killed by reflection)
	{MONS_INCABOSS,2},		//Blocko The Great
	{MONS_PEEPBOMB,1},		//Peeping Bomb
	{MONS_TRAPAZOID,0},		//Trapazoid
	{MONS_SLUG,0},			//Slimy Slug
	{MONS_SNAIL,0},			//Sniper Snail
	{MONS_OCTOBOSS,0},		//Optimum Octon
	{MONS_GOAT1,2},			//Rammy Gruff
	{MONS_GOAT2,2},			//Hoppy Gruff
	{MONS_GOAT3,2},			//Gruffy Gruff
	{MONS_STKSHROOM,3},		//Happy Stick Shroom
	{MONS_STKSPIDER,3},		//Happy Stick Spider
	{MONS_STKCORPSE,3},		//Happy Stick Corpse
	{MONS_STKBAT,3},		//Happy Stick Bat
	{MONS_DANCER,4},		//Happy Stick Dancer
	{MONS_BALL,4},			//Smashball (letting it fall into lava)
	{MONS_SPIDER2,0},		//Creepy Crawler
	{MONS_FROG,0},			//Froggy
	{MONS_PUMPKINFLY,0},	//Null
	{MONS_DUMBLE,0},		//Null
	{MONS_GLASSJAW,0},		//Null
	{MONS_FROG2,0},			//Null
	{MONS_FIREBUG,1},		//Null
	{MONS_FIREBAT,1},		//Null
	{MONS_STICKTREE,3},		//Null
	{MONS_GOODBONE,1},		//Null
	{MONS_RADISH,4},		//Null
	{MONS_BIGHEAD1,1},		//Null
	{MONS_BIGHEAD2,1},		//Null
	{MONS_BIGHEAD3,1},		//Null
	{MONS_SPOOKLEY,2},		//Null
	{MONS_SPARKY,1},		//Null
	{MONS_SKULL,2},			//Null
	{MONS_KNIGHT2,1},		//Null
	{MONS_MADCOW,0},		//Null
	{MONS_MADCOW,1},		//Null
	{MONS_SPATULA,2},		//Null
	{MONS_BATGUARD,0},		//Null
	{MONS_TURRETR,0},		//Null
	{MONS_DARKNESS,0},		//Null
	{MONS_SCAREDYBAT,0},		//Null
	{MONS_BLASTER,0},		//Null
	{MONS_SPEEDY,2},		//Null
	{MONS_GRUE,2},			//Null
	{MONS_MARSHBONE,0},		//Null
	{MONS_PATCH5,0},		//Null
	{MONS_PATCH6,0},		//Null
	{MONS_YOUNGLOONY,2},		//Null
	{MONS_REDBONE,0},		//Null
	{MONS_GANGRENE,0},		//Null
	{MONS_NUMBSKULL,1},		//Null
	{MONS_BOMBIE,0},		//Null
	{MONS_ZOMBIELORD,1},		//Null
	{MONS_FROG3,0},		//Null
	{MONS_RAGMAN,0},		//Null
	{MONS_PHARAOHF,0},		//Null
	{MONS_PHARAOHG,1},		//Null
	{MONS_TERRORTREE,0},		//Null
	{MONS_SWAMPSTUMP,0},		//Null
	{MONS_DINGBAT,0},		//Null
	{MONS_GARGOYLE,0},		//Null
	{MONS_VAMPBAT,0},		//Null
	{MONS_DIREWOLF,1},		//Null
	{MONS_LARRY,2},		//Null
	{MONS_SWAMPDOG,0},		//Null
	{MONS_SWAMPDOG2,0},		//Null
	{MONS_SWAMPDOG3,1},		//Null
	{MONS_GHOST2,0},		//Null
	{MONS_GHOST3,0},		//Null
	{MONS_DRACULITE,0},		//Null
	{MONS_VAMPLORD,1},		//Null
	{MONS_FRANK,2},		//Null
	{MONS_HAND,0},		//Null
	{MONS_BIGGHOST,2},		//Null
	{MONS_ELDER,2},		//Null
	{MONS_ELDER2,2},		//Null
	{MONS_ELDER3,2},		//Null
	{MONS_ELDER4,2},		//Null
	{MONS_BONKULA,2},		//Null
	{MONS_EVILIZER,4},		//Null
	{MONS_EVILPUMP,2},		//Null
	{MONS_SKULLY,1},		//Null
	{MONS_ELDER5,2},		//Null
	{MONS_HARRY,2},		//Null
	{MONS_FROGURT,1},		//Null
	{MONS_TORTREE,1},		//Null
	{MONS_GASBAT,1},		//Null
	{MONS_SWAMPDOG4,1},		//Null
	{MONS_KINGTOOT,1},		//Null
	{MONS_GHASTLY,2},		//Null
	{MONS_BOMBIELORD,2},		//Null
	{MONS_MUTANTPUMKIN,1},		//Null
	{MONS_MINIFRANK,2},		//Null
	{MONS_MECHABONK,2},		//Null
	{MONS_ARCTICWOLF,1},		//Null
	{MONS_GRABBYBAT,1},		//Null
	{MONS_HOTDOG,1},		//Null
	{MONS_STICKWITCH,3},		//Null
	{MONS_PLAYWOLF,2},		//Null
	{MONS_PLAYWIZARD,2},		//Null
	{MONS_NIGHTVAMP,0},		//Null
	{MONS_REVENGECLONE,2},		//Null
	{MONS_THINGY4,0},		//Null
	{MONS_GRUESML,0},		//Null
	{MONS_PYGMYBOOMER,0},		//Null
	{MONS_DEATHCAP,0},		//Null
	{MONS_BURNBUSH,0},		//Null
	{MONS_ATOMBOT,0},		//Null
	{MONS_NOODLENODE,0},		//Null
	{MONS_SANDMAN,0},		//Null
	{MONS_DECOY,0},		//Null
	{MONS_DEADEYE,1},		//Null
	{MONS_PEABRAIN,2},		//Null
	{MONS_ANUBIS,0},		//Null
	{MONS_HOLOGRAM,2},		//Null
	{MONS_CRABSHELL,0},		//Null
	{MONS_PATROLZOID,1},		//Null
	{MONS_METALPUMPKIN,1},		//Null
	{MONS_KABOOMKIN,0},		//Null
	{MONS_CHEF,2},		//Null
	{MONS_KELVOZOID,0},		//Null
	{MONS_MISSOZOID,0},		//Null
	{MONS_FROZOMBIE,0},		//Null
	{MONS_SUMUZOMBIE,1},		//Null
	{MONS_HOTFOOT,0},		//Null
	{MONS_EMPRESS,4},		//Null
	{MONS_LUNABOSS,2},		//Null
	{MONS_FLYINGZOMBIE,0},		//Null
	{MONS_GINGERSNAP,0},		//Null
	{MONS_ATOMICSEAL,0},		//Null
	{MONS_WETGHOST,0},		//Null
	{MONS_MINIMATTIE,3},		//Null
	{MONS_MALLOWMAN,0},		//Null
	{MONS_MALLOWMAN2,0},		//Null
	{MONS_PYGMYFIRE,0},		//Null
	{MONS_ATOMICZOMB,3},		//Null
	{MONS_LUNAMECHA,3},		//Null
	{MONS_PATCH7,0},		//Null
	{MONS_GOURD,1},		//Null
	{MONS_ESHKAH,2},		//Null
	{MONS_ROCKGUARD,1},		//Null
	{MONS_STKSNOW,3},		//Null
	{MONS_STKFISH,3},		//Null
	{MONS_YERFBOSS,2},		//Null
	{MONS_SPHINX2,3},		//Null
	{MONS_THING2,3},		//Null
	{MONS_YETIX,3},		//Null
	{MONS_KONGOR2,0},		//Null
	{MONS_VAMPJR,0},		//Null
	{MONS_CENTIBHEAD,2},		//Null
	{MONS_MULTISHROOM,1},		//Null
	{MONS_MUMBLE3,1},		//Null
	{MONS_HEALER,1},		//Null
	{MONS_BUDDYPUMPKIN,1},		//Null
	{MONS_MINECARTEVIL,1},		//Null
	{MONS_BONERIDER,1},		//Null
	{MONS_PYGMYRIDER,1},		//Null
	{MONS_CHESTRIDER,3},		//Null
	{MONS_IMITATER,4},		//Null
};

cardsType_t *GetCardID(dword type)
{
	return &cardsType[type];
}

int GetMonsterCard(dword g)
{
	int card,i;
	card = 0;
	for(i=0;i<NUM_CARDS;i++)
	{
		if(cardsType[i].id == g)
		card = i;
	}
	return card;
}

int GetMonsterCardsObtained(void)
{
	int h,i;
	h = 0;
	for(i=0;i<NUM_CARDS;i++)
	{
		if(profile.progress.carded[GetCardID(GetMonsterCard(i))->id]>0)
		h++;
	}
	return h;
}

int GetMonsterCardChance(dword g)
{
	int card,i,chance;
	chance = 0;
	for(i=0;i<NUM_CARDS;i++)
	{
		if(cardsType[i].id == g)
		{
			chance = ceil(monsType[g].hp/pow(2,cardsType[i].rarity));
			if (chance>1000)
				chance = 1000;
			else if (chance<1)
				chance = 1;
		}
	}
	return chance;
}

int GetMonsterCardRarity(dword g)
{
	int rare,i;
	rare = 0;
	for(i=0;i<NUM_CARDS;i++)
	{
		if(cardsType[i].id == g)
		{
			rare = cardsType[i].rarity;
		}
	}
	return rare;
}

cardsType_t *GrabCardID(dword type)
{
	monsSel = cardsType[type].id;
}

//init
void InitCardMenu(MGLDraw *mgl)
{
	int i;
	msBright=0;
	msDBright=1;
	monsSel=0;
	time = time2 = 0;

	mgl->LoadBMP("graphics/profmenu.bmp");
	backgd=(byte *)malloc(640*480);
	plSpr=new sprite_set_t("graphics/pause.jsp");
	backSpr=new sprite_set_t("expgraphics/monsbacks.jsp");
	frontSpr=new sprite_set_t("expgraphics/monstercards.jsp");
	cardPage=1;
	numObtained = GetMonsterCardsObtained();

	for(i=0;i<480;i++)
		memcpy(&backgd[i*640],&mgl->GetScreen()[i*mgl->GetWidth()],640);
}

//update
byte UpdateCardMenu(int *lastTime,MGLDraw *mgl)
{
	int i,j;
	char k;
	
	if(time2==4)
	{
		time2 = 0;
		time++;
	}
	time2++;

	mgl->GetMouse(&msx,&msy);

	if(*lastTime>TIME_PER_FRAME*5)
		*lastTime=TIME_PER_FRAME*5;

	k=mgl->LastKeyPressed();

	while(*lastTime>=TIME_PER_FRAME)
	{
		msBright+=msDBright;
		if(msBright>10)
			msDBright=-1;
		if(msBright<-2)
			msDBright=1;

		mgl->Process();
		*lastTime-=TIME_PER_FRAME;
	}
	
	if(mgl->MouseTap())
	{
		for(i=0;i<16;i++)
		{
			if(PointInRect(msx,msy,306+(i%4)*(58+12),88+floor(i/4)*(74+12),(306+(i%4)*(58+12))+CARD_WIDTH,(88+floor(i/4)*(74+12))+CARD_HEIGHT))
			{
				if(profile.progress.carded[GetCardID((cardPage-1)*16+i)->id]>0)
				{
					MakeNormalSound(SND_MENUSELECT);
					GrabCardID((cardPage-1)*16+i);
					time = time2 = 0;
				}
			}
			
		}
		for(i=0;i<NUM_CARD_BTNS;i++)
		{
			if(PointInRect(msx,msy,btn[i].x,btn[i].y,btn[i].x+btn[i].wid,btn[i].y+PBTN_HEIGHT))
			{
				MakeNormalSound(SND_MENUSELECT);
				switch(btn[i].id)
				{
					case BTN_NONE:
						break;
					case BTN_NEXTPG:
						if(cardPage<MAX_PAGES)
						cardPage++;
						break;
					case BTN_PREVPG:
						if(cardPage>1)
						cardPage--;
						else
						cardPage=1;
						break;
					case BTN_EXITPG:
						return 1;
						break;
				}
			}
		}
	}
	
	if(k==27)//pressing ESC exits to menu
	return 1;

	return 0;
}

//render buttons
void RenderCardButton(int x,int y,int wid,char *txt,MGLDraw *mgl)
{
	if(PointInRect(msx,msy,x,y,x+wid,y+PBTN_HEIGHT))
	{
		mgl->Box(x,y,x+wid,y+PBTN_HEIGHT,32+31);
		mgl->FillBox(x+1,y+1,x+wid-1,y+PBTN_HEIGHT-1,32+8);
	}
	else
		mgl->Box(x,y,x+wid,y+PBTN_HEIGHT,32+16);

	PrintGlow(x+2,y+3,txt,0,2);
}

//render the menu!
void RenderCardMenu(MGLDraw *mgl)
{
	int i,j,k;
	int msx2,msy2;
	char s[64];
	static char yesno[][5]={"Off","On"};
	
	for(i=0;i<480;i++)
		memcpy(&mgl->GetScreen()[i*mgl->GetWidth()],&backgd[i*640],640);

	for(i=0;i<NUM_CARD_BTNS;i++)
	{
			RenderCardButton(btn[i].x,btn[i].y,btn[i].wid,btn[i].txt,mgl);
	}

	PrintGlow(20,16,"Monster Cards!",0,4);
	//what page?
	sprintf(s,"Page %d/%d",cardPage,MAX_PAGES);
	PrintGlow(290,44,s,0,2);
	
	SetSpriteConstraints(13,13,627,467);
	DrawBox(290,72,590,448,1*32+16);
	DrawBox(20,168,260,448,1*32+16);
	msx2=msx;
	msy2=msy;
	if(msx2<13)
		msx2=13;
	if(msy2<13)
		msy2=13;
	if(msx2>622)
		msx2=622;
	if(msy2>462)
		msy2=462;
	
	if (cardPage>0)
		{
		for (i=0;i<16;i++)
		{
			if(i+(cardPage-1)*16 < NUM_CARDS)
			{
				j = i+(cardPage-1)*16;
				if(profile.progress.carded[GetCardID(j)->id]>0)
				{
					frontSpr->GetSprite(j)->Draw(306+(i%4)*(58+12),88+floor(i/4)*(74+12),mgl);
					backSpr->GetSprite(GetCardID(j)->rarity+1)->Draw(306+(i%4)*(58+12),88+floor(i/4)*(74+12),mgl);
				}
				else
					backSpr->GetSprite(0)->Draw(306+(i%4)*(58+12),88+floor(i/4)*(74+12),mgl);
				//sprintf(s,"ID %d, R%d",GetCardID(j)->id,GetCardID(j)->rarity);
				//sprintf(s,"C: %i,",profile.progress.carded[GetCardID(j)->id]);
				//PrintGlow(306+(i%4)*(58+12),88+floor(i/4)*(74+12),s,0,2);
			}
		}
	}
	if (monsSel>0)
	{
		SetSpriteConstraints(156,169,260,448);
		InstaRenderMonsterAnimated2(208,420,monsSel,0,time,mgl);
		sprintf(s,"#%i",GetMonsterCard(monsSel)+1);
		PrintGlow(22,150,s,0,2);
		sprintf(s,"NAME: %s",monsType[monsSel].name);
		PrintGlow(22,170,s,0,2);
		sprintf(s,"RANK %i CHAR",GetCardID(GetMonsterCard(monsSel))->rarity+1);
		PrintGlow(22,190,s,0,2);
		sprintf(s,"CARDS: %i",profile.progress.carded[GetCardID(GetMonsterCard(monsSel))->id]);
		PrintGlow(22,210,s,0,2);
		sprintf(s,"%i DEFEATED",profile.progress.kills[GetCardID(GetMonsterCard(monsSel))->id]);
		PrintGlow(22,230,s,0,2);
	}
	else
	{
		PrintGlow(20,150,"Awaiting selection...",0,2);
	}
	SetSpriteConstraints(0,0,639,479);
	DrawBox(156,168,260,448,1*32+16);
	plSpr->GetSprite(0)->DrawBright(msx2,msy2,mgl,msBright/2);
	sprintf(s,"%i / %i CARDS OBTAINED",numObtained,NUM_CARDS);
	PrintGlow(392,24,s,0,2);
		
}

//exit
void ExitCardMenu(void)
{
	free(backgd);
	delete plSpr;
	delete frontSpr;
	delete backSpr;
	time = time2 = 0;
}

void CardMenu(MGLDraw *mgl)
{
	byte done=0;
	int lastTime=1;

	InitCardMenu(mgl);
	PlaySongForce("SWC_21_secretlevel.ogg");
	
	while(!done)
	{
		lastTime+=TimeLength();
		StartClock();
		done=UpdateCardMenu(&lastTime,mgl);
		RenderCardMenu(mgl);
		
		mgl->Flip();
	
		if(!mgl->Process())
		done=1;
		EndClock();
	}
	ExitCardMenu();
}
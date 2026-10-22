#include "leveldef.h"
#include "monster.h"
#include "sound.h"
#include "music.h"
#include "appdata.h"
#include "ioext.h"

levelDef_t levelDef[NUM_LEVELS];

levelDef_t baseLevelDef[NUM_LEVELS]={	// AA leveldefs
	{LF_TOWN,{0,0,0,0,0},0,{0,2,0,0,0,0,0},SONG_NATCAVES}, //0, Einsburg
	{0,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_TITLE}, //1, Select an Upgrade
	{LF_TOWN,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_TITLE}, //2, Halls of Mastery
	{0,{MONS_TEDDY2,MONS_MOUSE,MONS_TEDDY2,MONS_TEDDY2,MONS_MOUSE4},40,{0,0,2,4,0,0,7},SONG_LIGHTFACT}, //3, Vulgar Vault
	{0,{MONS_PUPPET,MONS_MOUSE2,MONS_PUPPET,MONS_MOUSE,MONS_MONKEY},60,{0,3,4,0,1,10,0},SONG_STORAGE}, //4 Frigid Forest
	{LF_TOWN,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_NATCAVES}, //5, Woody's Cabin
	{0,{MONS_HOUND,MONS_HOUND,MONS_HOUND,MONS_HOUND,MONS_TITAN},30,{25,0,5,0,2,2,0},SONG_CONTROL}, //6, Treacherous Taiga
	{LF_TOWN,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_STORY}, //7, Rooms of Research
	{0,{MONS_ICEMICE,0,0,0,0},20,{2,0,5,0,2,3,0},SONG_STORAGE}, //8, Polar Pond
	{LF_TOWN,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_NATCAVES}, //9, Inside
	{0,{MONS_ROSE2,MONS_ROSE2,MONS_ROSE2,MONS_LION,MONS_ROSE2},15,{1,0,1,0,10,0,0},SONG_STORAGE}, //10, Perilous Pass
	{0,{MONS_PUPPET3,MONS_TEDDY,MONS_TINGUY2,0,MONS_LION},25,{0,8,2,0,0,5,0},SONG_NEWCAVES}, //11, Rowdy River
	{0,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_BOSSFIGHT}, //12, Pirate Attack!
	{0,{MONS_MOUSE2,MONS_MONKEY,MONS_TINGUY,MONS_MOUSE,MONS_MOUSE4},60,{10,10,3,0,0,5,0},SONG_STORAGE}, //13, Brisk Bog
	{0,{MONS_ICEMICE,MONS_HOUND,MONS_ICEMICE,MONS_TITAN,MONS_HOUND},30,{0,0,1,0,0,7,0},SONG_WARFACT}, //14, Drafty Den
	{LF_TOWN,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_NATCAVES}, //15, The Arena
	{LF_ARENA,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_BOSSFIGHT}, //16, Arena Small
	{LF_ARENA,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_BOSSFIGHT}, //17, Arena Maze
	{0,{MONS_MOUSE4,MONS_LION,MONS_MONKEY2,MONS_BADGER3,MONS_MOUSE4},40,{0,0,0,0,0,0,0},SONG_GUARDIAN}, //18, Poisonous Pit
	{0,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_STORY}, //19, Claim Your Reward
	{0,{0,0,0,0,0},0,{2,0,0,0,0,0,0},SONG_NEWCAVES}, //20, Pitiless Peak
	{0,{MONS_BADGHOST2,MONS_BADGHOST3,MONS_GHOSTMONKEY,MONS_BADGHOST2,MONS_DEATHBERRY},40,{0,1,3,5,0,0,0},SONG_WARFACT}, //21, Creepy Copse
	{0,{MONS_CRAB,MONS_BADGER,MONS_CRAB,MONS_TITAN,MONS_STONEWALL},15,{3,2,0,0,0,0,0},SONG_CORE}, //22, Sankyo
	{0,{MONS_CRAB,0,0,0,0},10,{0,0,2,5,0,0,10},SONG_GUARDIAN}, //23, Homicidal Hillside
	{0,{MONS_BADGER2,MONS_TITAN,MONS_STONEWALL,MONS_GHOSTDOG,MONS_GORKA2},30,{7,3,5,0,10,0,0},SONG_CORE}, //24, Sinister Slopes
	{0,{MONS_MOUSE,MONS_MOUSE2,MONS_MOUSE,MONS_MOUSE4,MONS_MOUSE},60,{0,0,1,0,0,3,0},SONG_LIGHTFACT}, //25, Infested Woods
	{0,{MONS_BADGER,MONS_GHOSTDOG,MONS_TITAN,MONS_STONEWALL,MONS_GORKA2},25,{0,0,1,7,0,0,15},SONG_CORE}, //26, Caliginous Cavities
	{0,{MONS_PUPPET,MONS_PUPPET,MONS_PUPPET,MONS_PUPPET,MONS_PUPPET3},60,{0,4,1,0,0,2,0},SONG_LIGHTFACT}, //27, Island of the Marionettes
	{0,{MONS_PUPPET3,MONS_LION,MONS_MONKEY3,MONS_PUPPET2,MONS_FRUITPUNCH},30,{0,10,3,0,5,5,0},SONG_NEWCAVES}, //28, Gelid Grove
	{0,{0,0,0,0,0},0,{3,0,1,0,5,0,0},SONG_NEWCAVES}, //29, Rickety Ridge
	{0,{MONS_TEDDY2,MONS_TEDDY,MONS_TEDDY2,0,0},40,{0,0,1,4,0,0,10},SONG_CONTROL}, //30, Bear Lair
	{0,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_ANNIHILATOR}, //31, Toy Factory
	{LF_TOWN,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_STORY}, //32, Guru's Home
	{0,{MONS_MONKEY,MONS_MONKEY2,MONS_SOCKCRAB,MONS_MONKEY3,MONS_GHOSTMONKEY},30,{0,0,3,10,0,0,15},SONG_GUARDIAN}, //33, Monkey Grotto
	{0,{MONS_TINGUY,MONS_TINGUY2,MONS_TINGUY,0,0},60,{0,0,0,0,0,0,5},SONG_CONTROL}, //34, Army Camp
	{0,{MONS_CRAB,MONS_MONKEY3,MONS_LION,MONS_MOUSE5,MONS_BADGER2},60,{0,0,5,20,0,0,25},SONG_WARFACT}, //35, Hiemal Holes
	{0,{MONS_BADGER,MONS_BADGER2,MONS_BADGER3,MONS_GORKA2,0},40,{0,0,1,5,0,0,10},SONG_GUARDIAN}, //36, Bodzha Nest
	{0,{MONS_ROSE2,MONS_LION,MONS_LION,MONS_ROSE3,MONS_LION2},30,{0,50,50,0,50,0,0},SONG_WARFACT}, //37, Overgrown Farm
	{LF_ARENA,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_BOSSFIGHT}, //38, Arena Icy
	{LF_ARENA,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_BOSSFIGHT}, //39, Arena Wet
	{LF_ARENA,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_BOSSFIGHT}, //40, Arena Capital
	{0,{MONS_ROSE2,MONS_TEDDY,MONS_PUPPET3,MONS_MONKEY2,MONS_ROSE},30,{10,2,5,0,20,3,0},SONG_NEWCAVES}, //41, Precipitous Precipices
	{0,{MONS_THUG,MONS_THUG,0,0,MONS_BRUISER},30,{0,0,2,5,0,0,10},SONG_WARFACT}, //42, Onion Camp
	{LF_TOWN,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_LIGHTFACT}, //43, Tweestad
	{LF_TOWN,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_NATCAVES}, //44, Clockmaker's Workshop
	{0,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_CORE}, //45, Battlegrounds
	{0,{MONS_BUNNYTROOP,MONS_BUNNYTROOP,0,0,MONS_EVILBOT},30,{0,2,5,0,0,0,10},SONG_NEWCAVES}, //46, Bunny Camp
	{0,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_NEWCAVES}, //47, Capitan's Chamber
	{0,{MONS_CRAB,MONS_HOUND,MONS_STONEWALL,MONS_HOTDOG,MONS_TITAN},50,{0,0,0,0,0,0,18},SONG_OLDBASE}, //48, Cavernous Caldera
	{LF_TOWN,{0,0,0,0,0},0,{0,0,0,0,0,0,1},SONG_NATCAVES}, //49, underground town
	{0,{MONS_CRAB,MONS_STONEWALL,MONS_BADGER2,MONS_NICETITAN,MONS_TITAN},50,{0,0,0,0,0,0,25},SONG_OLDBASE}, //50, underground crystal
	{0,{MONS_LION2,MONS_ROSE3,MONS_BURNINGBUSH,MONS_ROSE4,MONS_TITAN},60,{0,0,0,0,0,0,25},SONG_OLDBASE}, //51, underground garden
	{0,{MONS_LION2,MONS_STONEWALL,MONS_HOTDOG,MONS_FIREWALL,MONS_TITAN},40,{0,0,0,0,0,0,36},SONG_OLDBASE}, //52, underground bodzhas
	{0,{MONS_HOTDOG,0,MONS_BURNINGBUSH,MONS_FIREWALL,0},40,{0,0,0,0,0,0,16},SONG_OLDBASE}, //53, underground pits
	{0,{0,0,0,0,0},0,{0,0,0,0,0,0,3},SONG_OLDBASE}, //54, underground furnace
	{0,{0,MONS_HOUND,MONS_ICEBOX,MONS_NICETITAN,MONS_TITAN},50,{0,0,0,0,0,0,28},SONG_OLDBASE}, //55, underground titan
	{0,{MONS_BADGER3,0,0,0,0},0,{10,5,0,0,0,0,0},SONG_CORE}, //56, Sankyo Castle
	{LF_TOWN,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_COREALT}, //57, Trial Entrance
	{0,{MONS_TEDDY4,MONS_MONKEY4,MONS_ONEPUNCH,MONS_MOUSE5,MONS_TINPOT},40,{0,0,0,0,0,0,0},SONG_ANNIHILATOR}, //58, castle 1
	{0,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_BOSSFIGHT}, //59, castle boss
	{0,{MONS_ROCKMONKEY,MONS_SPICEMICE,MONS_TURRET,MONS_TINPOT,MONS_TEDDY5},50,{0,0,0,0,0,0,0},SONG_ANNIHILATOR}, //50, castle 2
	{0,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_BOSSALT}, //61, The Trial
	{0,{MONS_BIGBADGER,0,0,MONS_GORKA2,0},40,{0,0,0,0,0,0,0},SONG_ANNIHILATOR}, //62, The Clouds
	{0,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_TETRON}, //63, Cloud Castle
	//{LF_TOWN,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_TOWN}, // eisburg
	//{0,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_TOWN}, // cozy cabin
	//{LF_TOWN,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_TOWN}, // mickey's cabin
	//{0,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_CAVES}, // mickey's cellar
	//{0,{MONS_PUPPET,MONS_PUPPET,MONS_MOUSE2,MONS_MOUSE,MONS_PUPPET},60,{0,0,4,0,0,12,0},SONG_WINTER}, // winter woods
	//{LF_TOWN,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_TOWN}, // woody's cabin
	//{0,{MONS_TINGUY,MONS_MONKEY,0,0,0},60,{15,5,3,0,0,0,0},SONG_WINTER}, // tundra
	//{0,{MONS_TEDDY2,MONS_MOUSE4,0,0,0},50,{0,0,2,6,0,0,15},SONG_CAVES}, // chilly cavern
	//{LF_TOWN,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_TOWN}, // axes R us
	//{LF_TOWN,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_TOWN}, // guru cabin
	//{0,{MONS_TEDDY2,MONS_MOUSE,0,0,0},15,{0,0,1,1,0,0,3},SONG_CAVES},	// cozy cave
	//{LF_TOWN,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_TOWN}, // storage cabin
	//{LF_TOWN,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_TOWN}, // black market
	//{0,{MONS_PUPPET,MONS_MOUSE2,MONS_MONKEY,0,0},40,{0,4,3,0,0,6,0},SONG_WINTER}, // southwood
	//{LF_TOWN,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_TOWN}, // klonk's cabin
	//{LF_TOWN,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_TOWN}, // tina's arena
	//{LF_ARENA,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_BOSS}, // arena1 - mini chamber
	//{LF_ARENA,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_BOSS}, // arena2 - dark maze
	//{0,{MONS_ICEMICE,0,0,0,0},3,{3,0,2,0,0,0,0},SONG_BOSS}, // icy lake
	//{0,{MONS_MONKEY,0,0,0,0},5,{0,0,1,2,0,0,2},SONG_CAVES}, // rocky ridge
	//{0,{MONS_MONKEY,MONS_MOUSE4,MONS_ICEMICE,MONS_HOUND,0},30,{4,4,3,0,0,6,0},SONG_FACTORY}, // baskerville
	//{LF_TOWN,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_TOWN}, // baskerville cabin
	//{0,{MONS_ICEMICE,0,0,0,0},10,{0,0,2,0,0,2,0},SONG_WINTER}, // frozen fjord
	//{0,{MONS_GHOSTDOG,MONS_GHOSTMONKEY,0,0,0},40,{0,0,3,0,0,8,0},SONG_CAVES}, // eastwood
	//{LF_TOWN,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_TOWN}, // baskerville cabin2
	//{LF_TOWN,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_TOWN}, // baskerville cabin3
	//{0,{MONS_ROSE2,MONS_TINGUY,0,0,0},15,{0,0,2,6,0,0,0},SONG_CAVES}, // under baskerville
	//{LF_TOWN,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_TOWN}, // halls of history
	//{0,{MONS_TINGUY,MONS_PUPPET3,0,0,0},13,{2,4,1,0,0,0,0},SONG_WINTER}, // limpid lake
	//{0,{MONS_TEDDY,MONS_ROSE2,MONS_PUPPET3,0,0},25,{5,2,3,0,10,0,0},SONG_WINTER}, // frosty cliffs
	//{LF_TOWN,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_TOWN}, // rope cabin
	//{0,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_FACTORY}, // toy factory
	//{0,{MONS_TINGUY2,0,0,0,0},20,{0,0,0,0,0,0,0},SONG_FACTORY}, // toy story 2
	//{LF_TOWN,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_TOWN}, // toy office
	//{0,{MONS_TINGUY2,0,0,0,0},5,{0,0,1,2,0,0,3},SONG_CAVES}, // toy basement
	//{0,{MONS_MONKEY3,MONS_ROSE2,MONS_CRAB,0,0},6,{0,0,1,3,0,0,3},SONG_CAVES}, // arms dealer cave
	//{0,{MONS_MONKEY3,MONS_LION,MONS_CRAB,0,0},15,{0,0,1,5,0,0,8},SONG_CAVES}, // cliff cave
	//{0,{0,0,0,0,0},0,{0,0,0,1,0,0,1},SONG_CAVES}, // cramped cave (supply depot)
	//{LF_ARENA,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_BOSS}, // arena3 - icy chamber
	//{LF_ARENA,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_BOSS}, // arena4 - wet chamber
	//{LF_ARENA,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_BOSS}, // arena5 - capital plaza
	//{0,{MONS_PUPPET3,MONS_TINGUY2,MONS_LION,0,0},40,{0,4,3,0,0,6,0},SONG_WINTER}, // westwood
	//{0,{MONS_THUG,MONS_THUG,0,0,MONS_BRUISER},20,{0,0,1,2,0,0,2},SONG_CAVES}, // onion camp
	//{0,{MONS_GHOSTMONKEY,MONS_GHOSTDOG,MONS_BADGER,0,0},30,{0,0,2,0,0,6,5},SONG_WINTER}, // highway
	//{0,{MONS_CRAB,MONS_MONKEY3,MONS_ROSE2,MONS_LION,0},40,{6,1,4,0,8,0,0},SONG_WINTER}, // northwood
	//{0,{MONS_PUPPET3,MONS_TINGUY2,MONS_BADGER,0,0},10,{3,0,2,0,0,2,0},SONG_WINTER}, // clearing
	//{0,{MONS_BUNNYTROOP,MONS_BUNNYTROOP,0,0,MONS_EVILBOT},12,{0,0,1,2,0,0,3},SONG_CAVES}, // snuggly HQ
	//{0,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_CAVES}, // captain's quarters
	//{0,{MONS_STONEWALL,MONS_CRAB,0,0,0},20,{0,0,0,0,0,0,14},SONG_UNDERWORLD}, // geyser grotto
	//{LF_TOWN,{0,0,0,0,0},0,{0,0,0,0,0,0,1},SONG_TOWN}, //  shroom camp
	//{0,{MONS_STONEWALL,MONS_BADGER2,0,0,0},30,{0,0,0,0,0,0,24},SONG_UNDERWORLD}, // crystal caves
	//{0,{MONS_LION2,MONS_ROSE3,0,0,0},50,{0,0,0,0,0,0,24},SONG_UNDERWORLD}, // gardens
	//{0,{MONS_LION2,MONS_STONEWALL,0,0,0},60,{0,0,0,0,0,0,30},SONG_UNDERWORLD}, // shroom forest
	//{0,{MONS_BADGER2,MONS_STONEWALL,MONS_LION2,0,0},40,{0,0,0,0,0,0,20},SONG_UNDERWORLD}, // pits of despair
	//{LF_TOWN,{0,0,0,0,0},0,{0,0,0,0,0,0,2},SONG_TOWN}, // furnace
	//{0,{MONS_TITAN,MONS_HOUND,0,0,0},40,{0,0,0,0,0,0,17},SONG_UNDERWORLD}, // titan tunnels
	//{0,{MONS_BADGER3,0,0,0,0},60,{0,0,4,8,0,0,10},SONG_WINTER}, // glacial gorge
	//{LF_TOWN,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_TOWN}, // mtn cabin
	//{0,{MONS_TEDDY4,0,MONS_MONKEY4,MONS_MOUSE5,MONS_TINPOT},20,{0,0,0,0,0,0,0},SONG_FACTORY}, // castle 1
	//{0,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_BOSS}, // throne room
	//{0,{MONS_TEDDY4,0,MONS_MONKEY4,MONS_MOUSE5,MONS_TINPOT},30,{0,0,0,0,0,0,0},SONG_FACTORY}, // castle 2
	//{0,{MONS_BIGBADGER,0,0,0,0},20,{0,0,2,0,8,0,0},SONG_WINTER}, // ascent
	//{LF_TOWN,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_TOWN}, // peak cave
	//{0,{0,0,0,0,0},0,{0,0,0,0,0,0,0},SONG_BOSS}, // don't look down
};

char songList[16][20]={
	"SONG_GUARDIAN",
	"SONG_STORAGE",
	"SONG_COREALT",
	"SONG_BOSSALT",
	"SONG_STORY",
	"SONG_TITLE",
	"SONG_BOSSFIGHT",
	"SONG_TETRON",
	"SONG_NATCAVES",
	"SONG_CORE",
	"SONG_CONTROL",
	"SONG_LIGHTFACT",
	"SONG_NEWCAVES",
	"SONG_WARFACT",
	"SONG_OLDBASE",
	"SONG_ANNIHILATOR",
};

char badguyList[NUM_MONSTERS][20]={
	"0",
	"MONS_LOONY",
	"MONS_TEDDY",
	"MONS_VILLAGER",
	"MONS_AXETRAP",
	"MONS_BONEHEAD",
	"MONS_MOUSE",
	"MONS_PUPPET",
	"MONS_CRYOZOID",
	"MONS_SHROOM",
	"MONS_PUPPET2",
	"MONS_TINGUY",
	"MONS_MOUSE2",
	"MONS_TEDDY2",
	"MONS_MOUSE3",
	"MONS_MONKEY",
	"MONS_KLONK",
	"MONS_TOASTY",
	"MONS_KLUNK",
	"MONS_ICEMICE",
	"MONS_ROSE",
	"MONS_HOUND",
	"MONS_ROSE2",
	"MONS_MONKEY2",
	"MONS_PUPPET3",
	"MONS_TEDDY3",
	"MONS_TOYMAKER",
	"MONS_TOYPOWER",
	"MONS_TOYCRYSTAL",
	"MONS_TINGUY2",
	"MONS_LIFEBERRY",
	"MONS_MOUSE4",
	"MONS_LION",
	"MONS_BRUISER",
	"MONS_BURNINGBUSH",
	"MONS_THUG",
	"MONS_GHOST",
	"MONS_TRIGUN",
	"MONS_JUNKSMITH",
	"MONS_CARAVAN",
	"MONS_JUNKTRAP",
	"MONS_BUNNYTROOP",
	"MONS_BUNNYMAJOR",
	"MONS_BUNNYBOSS",
	"MONS_EVILBOT",
	"MONS_YOURBOT",
	"MONS_MONKEY3",
	"MONS_GHOSTDOG",
	"MONS_GHOSTMONKEY",
	"MONS_CRAB",
	"MONS_BADGER",
	"MONS_TITAN",
	"MONS_BIGPLANT",
	"MONS_FURNACE",
	"MONS_ROSE3",
	"MONS_BADGER2",
	"MONS_LION2",
	"MONS_STONEWALL",
	"MONS_BOKBOK",
	"MONS_BOKBOK2",
	"MONS_BUNNYBOSS2",
	"MONS_TITAN2",
	"MONS_BADGER3",
	"MONS_ARENACRYSTAL",
	"MONS_TURRET",
	"MONS_PROTOTYPE",
	"MONS_FROSTGATE",
	"MONS_MOUSE5",
	"MONS_TINPOT",
	"MONS_TEDDY4",
	"MONS_MONKEY4",
	"MONS_MIMIC",
	"MONS_ARMYONE",
	"MONS_BARON",
	"MONS_UBERTITAN",
	"MONS_TEDDY5",
	"MONS_ROSE4",
	"MONS_ROSE5",
	"MONS_BIGBADGER",
	"MONS_BIRD",
	"MONS_KILLBOR",
	"MONS_GORKA2",
	"MONS_LILBADGER",
	"MONS_PLAYTHUG",
	"MONS_PLAYBUNNY",
	"MONS_BADGHOST",
	"MONS_BADGHOST2",
	"MONS_BADGHOST3",
	"MONS_BADGHOST4",
	"MONS_BADGHOST5",
	"MONS_SOCKCRAB",
	"MONS_HOTDOG",
	"MONS_ICEBOX",
	"MONS_DEATHBERRY",
	"MONS_FRUITPUNCH",
	"MONS_REDBARON",
	"MONS_JACK",
	"MONS_FIREWALL",
	"MONS_NICETITAN",
	"MONS_SPICEMICE",
	"MONS_ONEPUNCH",
	"MONS_ROCKMONKEY",
};

char err[64];

void ResetLevelDefs(void)
{
	memcpy(levelDef,baseLevelDef,sizeof(levelDef_t)*NUM_LEVELS);
}

void LDError(const char *txt,int lv)
{
	sprintf(err,"aal_*.txt line %d: %s",lv+3,txt);
}

int ParseLevelFlags(byte lv,char *line)
{
	char txt[256];
	int end;

	end=strcspn(line,",");
	if(end>=(int)strlen(line))
	{
		LDError("No , after level type.",lv);
		return -1;
	}
	strncpy(txt,&line[1],end-1);
	txt[end-1]='\0';
	if(!strcmp(txt,"LF_TOWN"))
	{
		levelDef[lv].flags=LF_TOWN;
	}
	else if(!strcmp(txt,"LF_ARENA"))
		levelDef[lv].flags=LF_ARENA;
	else if(!strcmp(txt,"0"))
		levelDef[lv].flags=0;
	else
	{
		LDError("Invalid level type.",lv);
		return -1;
	}
	return end;
}

int ParseSong(char *line,int start,byte lv)
{
	char txt[256];
	int end,i;

	end=strcspn(&line[start],"}");
	if(end>=(int)strlen(line))
	{
		LDError("No } after song.",lv);
		return -1;
	}
	strcpy(txt,&line[start]);
	txt[end]='\0';

	i=atoi(txt);	// you can just put a number here instead of a song name - in this way, more songs can be added
	if(i!=0)
	{
		levelDef[lv].song=(byte)(240+i);
		return end+start;
	}
	for(i=0;i<16;i++)
		if(!strcmp(txt,songList[i]))
		{
			levelDef[lv].song=(byte)(240+i);
			return end+start;
		}
	LDError("Invalid song.",lv);
	return -1;
}

int ParseBadguys(char *line,int start,byte lv)
{
	int end,i,j;
	char txt[256],*tok;

	if(line[start]!='{')
	{
		LDError("Badguy list doesn't start with {.",lv);
		return -1;
	}
	start++;

	strcpy(txt,&line[start]);
	end=strcspn(txt,"}");
	if(end>=(int)strlen(txt))
	{
		LDError("Badguy list doesn't end with }.",lv);
		return -1;
	}
	txt[end]='\0';
	i=0;
	tok=strtok(txt,",}");
	while(i<5 && tok!=NULL)
	{
		for(j=0;j<NUM_MONSTERS;j++)
		{
			if(!strcmp(tok,badguyList[j]))
			{
				levelDef[lv].badguyType[i]=(byte)j;
				i++;
				j=-1;
				break;
			}
		}
		if(j!=-1)
		{
			LDError("Illegal badguy name.",lv);
			return -1;
		}
		tok=strtok(NULL,",}");
	}
	if(i<5)
	{
		LDError("Less than 5 badguy types.",lv);
		return -1;
	}
	return end+start+1;
}

int ParseHerbs(char *line,int start,byte lv)
{
	int end,i;
	char txt[256],*tok;

	if(line[start]!='{')
	{
		LDError("Herb list doesn't start with {.",lv);
		return -1;
	}
	start++;

	strcpy(txt,&line[start]);
	end=strcspn(txt,"}");
	if(end>=(int)strlen(txt))
	{
		LDError("Herb list doesn't end with }.",lv);
		return -1;
	}
	txt[end]='\0';
	i=0;
	tok=strtok(txt,",}");
	while(i<7 && tok!=NULL)
	{
		levelDef[lv].herbCount[i]=(byte)atoi(tok);
		tok=strtok(NULL,",}");
		i++;
	}
	if(i<7)
	{
		LDError("Less than 7 herb/mine entries.",lv);
		return -1;
	}
	return end+start+1;
}

int ParseBadguyCount(char *line,int start,byte lv)
{
	int end;
	char txt[256];

	strcpy(txt,&line[start]);
	end=strcspn(txt,",");
	if(end>=(int)strlen(txt))
	{
		LDError("Lacking , after badguy count.",lv);
		return -1;
	}
	txt[end]='\0';
	levelDef[lv].badguyCount=atoi(txt);
	return end+start;
}

void LoadLevelDefs(char *name)
{
	int lv;
	int spot;
	char line[256];

	err[0]='\0';
	ResetLevelDefs();
	sprintf(line,"addons/aal_%s.txt",name);
	auto f = AppdataOpen(line);
	if(!f)
		return;

	SdlRwStream stream(f.get());

	if(!stream.getline(line, std::size(line)))
	{
		LDError("Lacking any content!",-2);
		return;
	}
	else
	{
		// this first line is the campaign name
	}

	for(lv=0;lv<NUM_LEVELS;lv++)
	{
		if(!stream.getline(line, std::size(line)))
		{
			break;
		}
		if(line[0]!='{')
		{
			LDError("Missing { at start.",lv);
			break;
		}
		spot=ParseLevelFlags(lv,line);
		if(spot==-1)
			break;
		spot++;	// move spot past the ,
		if(spot>=(int)strlen(line))
		{
			LDError("Ends too soon!",lv);
			break;
		}
		spot=ParseBadguys(line,spot,lv);
		if(spot==-1)
			break;
		spot++;	// move spot past the ,
		if(spot>=(int)strlen(line))
		{
			LDError("Ends too soon!",lv);
			break;
		}
		spot=ParseBadguyCount(line,spot,lv);
		if(spot==-1)
			break;
		spot++;	// move spot past the ,
		if(spot>=(int)strlen(line))
		{
			LDError("Ends too soon!",lv);
			break;
		}
		spot=ParseHerbs(line,spot,lv);
		if(spot==-1)
			break;
		spot++;	// move spot past the ,
		if(spot>=(int)strlen(line))
		{
			LDError("Ends too soon!",lv);
			break;
		}
		spot=ParseSong(line,spot,lv);
		if(spot==-1)
			break;
		spot++;	// move spot past the ,
		if(spot>=(int)strlen(line))
		{
			LDError("Ends too soon!",lv);
			break;
		}
		if(err[0]!='\0')
			break;
	}
}

char *LevelError(void)
{
#ifdef DIRECTORS
	return err;
#else
	return "";
#endif
}

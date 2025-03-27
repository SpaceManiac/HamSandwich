#ifndef MAP_H
#define MAP_H

#include "winpch.h"
#include "tile.h"
#include "items.h"

#define MAX_LIGHT 16
#define MIN_LIGHT -32

// the maximum # of monsters you can put on the map
#define MAX_MAPMONS 128
#define MAX_SPECIAL 32

// flags for calling map render
#define MAP_SHOWLIGHTS 1
#define MAP_SHOWWALLS  2
#define MAP_SHOWITEMS  4
#define MAP_SHOWBADGUYS 8
#define MAP_SHOWSPECIALS 16

//special trigger flags
#define TRG_STEP		1
#define TRG_ENEMYSTEP	2
#define TRG_NEAR		4
#define TRG_PASSLEVELS	8
#define TRG_KEYCHAINS	16
#define	TRG_KILLALL		32
#define TRG_HAVEBRAINS	64
#define TRG_SHOOT		128
#define TRG_REPEATABLE	256		// works more than once
#define TRG_MESSAGE		512		// displays a message
#define TRG_CHAIN		1024	// goes off if any special in an adjacent square goes off
#define TRG_KILLONE		2048	// kill any one of the type
#define TRG_GETHURT		4096	// just take any damage

//special effect choices
#define SPC_NONE		0
#define SPC_SUMMON		1
#define SPC_ZAPWALL		2
#define SPC_RAISEWALL	3
#define SPC_TOGGLEWALL	4
#define SPC_TELEPORT	5
#define SPC_LIGHT		6
#define SPC_GOTOMAP		7
#define SPC_EXIT		8
#define SPC_PICTURE		9
#define SPC_CHGITEM		10
#define SPC_CHGFLOOR	11
#define SPC_KILLSPCL	12
#define SPC_CHGMONS		13
#define SPC_MAXEFFECTS  14

// Map flags
#define MAP_SNOWING		1
#define MAP_MANYITEMS	2
#define MAP_SECRET		4
#define MAP_TORCHLIT	8

// map updating modes
#define UPDATE_GAME		0
#define UPDATE_EDIT		1
#define UPDATE_FADE		2
#define UPDATE_FADEIN	3

// horkbox effects
#define HORK_NONE	(0)
#define HORK_LIFEDRAIN (1)
#define HORK_NOFIREBALLS (2)
#define HORK_MANADRAIN (3)
#define HORK_TRAP (4)
#define HORK_HEAL (5)
#define HORK_BOMBS (6)
#define HORK_FLAMES (7)
#define HORK_MAX (8)

typedef struct special_t
{
	word trigger;
	byte trigValue;
	byte effect;
	byte x,y;
	byte effectX,effectY;
	int value;
	char msg[32];
} special_t;

typedef struct mapTile_t
{
	byte floor;
	byte wall;
	byte item;
	char light;
	char templight;
	byte opaque;
} mapTile_t;

typedef struct mapBadguy_t
{
	byte x,y;
	byte type;
} mapBadguy_t;

struct world_t;

class Guy;

class Map
{
	public:
		Map(byte size,const char *name);
		Map(Map *m);
		Map(SDL_RWops *f);
		~Map(void);

		void ScanForContent(void);
		byte Save(SDL_RWops *f);
		void Init(world_t *wrld);
		void Render(world_t *world,int camX,int camY,byte flags);
		byte DropItem(int x,int y,byte itm);
		void PermaTorch(int x,int y,char brt);
		void TempTorch(int x,int y,char brt);
		void BrightTorch(int x,int y,char brt,byte size);
		void LightSpecial(int x,int y,char brt,byte size);
		void Update(byte mode,world_t *world);
		void FillRect(int x, int y, int x2, int y2, byte flr,byte wall);
		byte LOS(int x,int y,int radius,int value,byte (*DoIt)(int,int,int,int,int,Map *));
		byte TightLOS(int x,int y,int radius,int value,byte (*DoIt)(int,int,int,int,int,Map *));
		byte FindGuy(int x,int y,byte size,Guy *g);
		byte FindGuy2(int x,int y,byte size,Guy *g);
		void SewerTrick(int x,int y);
		mapTile_t* GetTile(int x, int y) { if (x >= 0 && y >= 0 && x < width && y < height) return &map[x + y * width]; else return nullptr; };
		void SmoothLight(int x,int y);
		void SmoothLights(void);

		void FindNearBrain(int myx,int myy);

		int width,height;
		mapTile_t *map;
		char name[32];
		byte song;
		byte flags;
		byte contentFlags;
		mapBadguy_t badguy[MAX_MAPMONS];
		special_t   special[MAX_SPECIAL];

	private:
		void LOSPoints(int x,int y,int curx,int cury,int *p1x,int *p1y,int *p2x,int *p2y);
};

extern byte brainX,brainY;

byte PlaceItemCallback(int x,int y,int cx,int cy,int value,Map *map);
byte TorchCallback(int x,int y,int cx,int cy,int value,Map *map);
byte TempTorchCallback(int x,int y,int cx,int cy,int value,Map *map);
byte TempBrightCallback(int x,int y,int cx,int cy,int value,Map *map);

class Guy;
void SpecialStepCheck(Map *map,int x,int y,Guy *me);
void SpecialTakeEffect(Map *map,special_t *spcl,Guy *victim);
void SpecialKillCheck(Map* map, byte type);
void SpecialOuchCheck(Map* map, byte type);

void ChangeFloor(Map* map, int x, int y, byte newFloor);
void ZapWall(Map *map,int x,int y,byte newFloor);
void SpecialShootCheck(Map *map,int x,int y,bool justThisTile);
void SpecialAnytimeCheck(Map *map);
void RenderSpecialXes(MGLDraw *mgl,Map *map,byte world);
int  TotalBrains(void);
void OpenVaultDoor(Map *map);
void OpenVaultDoor2(Map* map);
byte VaultPuzzleCheck(Map* map);
byte VaultPuzzle2Check(Map *map);
void Cavernize(Map *map);
void GetRidOfGoodStuff(Map *map);
void MushAidPuzzleUpdate(Map* map);
void MushAidPuzzleReset(Map* map);
void AbandonedVillagePuzzleReset(Map* map);
void AbandonedVillagePuzzle(Map* map);
void LibraryPuzzle(Map* map);
byte TemplePuzzle(Map *map);
void ResetTemplePuzzle(Map *map);
void CreateMaze(Map *map,byte wid,byte hei,byte mazewid,byte mazehei);
void CreateLockedMaze(Map *map,byte wid,byte hei,byte mazewid,byte mazehei);
void LockedMazePuzzle(Map *map);
void LockedMazeSpikes(Map* map, int x, int y);
byte SmashballCheck(Map *map);
void LightsOutPuzzle(Map *map,int x,int y);
void OctoBossDeath(Map *map);
byte MySpecialType(byte type,Map *map,int mapx,int mapy);
void GuestChamberPuzzleStep(Map* map, int mapx, int mapy);
void MinesPuzzle(Map* map,int x,int y);
void BatsPuzzle(Map* map,int x,int y);
void SwampStewSetup(Map* map);
void RenderSwampRecipe(void);
void SwampUpdate(Map* map);
bool SwampPotIsFull(void);
void SwampEnableCooking(void);
void GetSwampStats(byte* state, byte* temp, word* time,byte *score);
const char* GetSwampTempName(byte temp);
void GetSwampTemps(byte* boil, byte* simmer);
void SwampDestroyCauldron(byte how);
void AmongHedgesPuzzleInit(Map* map);
void AmongHedgesPuzzleUpdate(Map* map);
void AmongHedgesShootCheck(Map* map, int x, int y);
void LostInWoodsPuzzleStep(Map* map, int x, int y);
void HauntedWoodsPuzzleInit(Map* map);
void HauntedPuzzleGhostControl(Map* map, Guy* me);
bool HauntedWoodsCastSpell(Map *map);
void HauntedWoodsCatchGhost(Guy* me);
byte HauntedWoodsGhosts(void);
void DeepEndPuzzleInit(void);
void DeepEndPuzzleKill(Map* map, byte type);
void HorkBoxInit(Map* map);
void HorkBoxBones(Map* map, int x, int y);
void OrderUpSetup(Map* map);
void OrderUpUpdate(Map* map);
void GetOrderUpStats(byte *state, byte *stews, byte *temp);
void OrderUpSetState(byte s);

#endif

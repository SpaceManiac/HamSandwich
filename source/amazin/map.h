#ifndef MAP_H
#define MAP_H

#include "jamultypes.h"
#include "game.h"

enum class Direction
{
	East,
	South,
	West,
	North,
};

namespace TileFlags
{
	enum : word
	{
		WallV = 0x1,
		WallH = 0x2,
		Candle = 0x4,
		HammerUp = 0x8,
		PK = 0x10,
		PU = 0x20,
		P1 = 0x40,
		P2 = 0x80,
		P = 0x100,
		TP = 0x200,
	};
}

enum class ItemType : byte
{
	None,
	Candle,
	HammerUp,
	Points100,
	Points250,
	Points500,
	Points1000,
	SpeedyBoots,
	Ghost,
	IceCream,
	HammerUpRandom,
	Zzz,
	Shield,
	_13,
	OneUp,
	FirePower,
	IcePower,
	EvilEye,
	BigCandle,
	FireTrail,
	IceTrail,
};

struct MapTile
{
	word flags;
	byte distanceFromPumpkinSpawn;
	byte tileAnim;
	byte tile;
	ItemType item;
	byte itemAnim;
	byte _7;
};
static_assert(sizeof(MapTile) == 8);

class sprite_set_t;

extern MapTile g_Map[14][19];
extern sprite_set_t* g_ItemsJsp;

void MapInit();
void MapExit();

void MapLoad(const char* fname);
void MapSave(const char* fname);
void MapClear(GameMode theme);
void MapRenderToBackground(GameMode theme);
void TileRender(byte tile, int x, int y);
void MapSetupStart();
void MapRandomize(int w, int h);
void MapFilterForRivalry();
void GetPlayerStart(byte player, int *x, int *y);

void MapRedrawTile(byte tx, byte ty);
void MapSpawnItems();

void ErasePowerup();
void ShootEvilEye(int x, unsigned int y, Direction direction);
bool MapUpdate(byte);
void CheatYippee();

void MapRender(bool editor);

#endif  // MAP_H

#include "repair.h"
#include "sound.h"
#include "tile.h"
#include "world.h"
#include "map.h"
#include "items.h"
#include "editor.h"

void RepairCustomSounds(int n)
{
	// fix references inside other items
	RepairItemToSound(n);

	for (Map *map : EditorGetWorld()->Maps())
	{
		// first, fix specials
		RepairSpecialToSound(map->special, n);
	}
}

void RepairItems(int n)
{
	// fix references inside other items
	RepairItemToItem(n);

	for (Map *map : EditorGetWorld()->Maps())
	{
		// on each level, we need to fix item references

		// first, fix specials
		RepairSpecialToItem(map->special,n);

		// now fix the items in the map themselves
		for (mapTile_t &target : map->Tiles())
		{
			if (target.item == n)
				target.item = 0;
			else if (target.item > n)
				target.item--;
		}

		// then fix any reference of a guy holding them
		for (mapBadguy_t &badguy : map->badguy)
		{
			if (badguy.item == n)
				badguy.item = 0;
			else if (badguy.item > n && badguy.item != ITM_RANDOM)
				badguy.item--;
		}
	}
}

void RepairLevels(const SwapTable &table)
{
	for (Map *map : EditorGetWorld()->Maps())
	{
		// on each level, we need to fix level references, ironically

		// fix specials
		RepairSpecialToLevel(map->special, table);
	}
}

void RepairTiles(const SwapTable &table)
{
	world_t *w = EditorGetWorld();

	// fix references inside the tiles
	RepairTileToTile(w, table);

	for (Map *map : w->Maps())
	{
		// on each level, we need to fix tile references

		// first, fix specials
		RepairSpecialToTile(map->special, table);

		// now fix the tiles in the map themselves
		for (mapTile_t &tile : map->Tiles())
		{
			if(tile.wall!=0)
				tile.wall=table.GetSwap(tile.wall);
			tile.floor=table.GetSwap(tile.floor);
		}
	}

	RepairItemToTile(table);
}

SwapTable::SwapTable(int size)
	: swapTable(size)
{
	for (int i = 0; i < size; ++i)
	{
		swapTable[i]=i;	// initialize each spot to itself
	}
}

void SwapTable::Swap(int me,int you)
{
	std::swap(swapTable[me], swapTable[you]);
}

void SwapTable::Delete(int me)
{
	int i;

	for(i=swapTable.size()-1;i>me;i--)
	{
		swapTable[i]=swapTable[i-1];
	}
	swapTable[me]=0;
}

void SwapTable::DeleteBlock(int me,int you)
{
	int i;

	for(i=swapTable.size()-1;i>you;i--)
	{
		swapTable[i]=swapTable[i-(you-me+1)];
	}
	for(i=me;i<=you;i++)
		swapTable[i]=0;
}

void SwapTable::SwapBlock(int me,int me2,int you)
{
	int i;

	for(i=me;i<=me2;i++)
	{
		Swap(i,you+i-me);
	}
}

int SwapTable::GetSwap(int me) const
{
	if(me<0 || me>=(int)swapTable.size())
		return 0;

	return swapTable[me];
}

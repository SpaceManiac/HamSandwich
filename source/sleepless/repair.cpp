#include "repair.h"
#include "sound.h"
#include "tile.h"
#include "world.h"
#include "map.h"
#include "items.h"
#include "editor.h"

int swapSize;
int *swapTable;

void RepairCustomSounds(int n)
{
	world_t *w;
	int lvl;

	w=EditorGetWorld();

	// fix references inside other items
	RepairItemToSound(n);

	for(lvl=0;lvl<w->numMaps;lvl++)
	{
		// first, fix specials
		RepairSpecialToSound(w->map[lvl]->special,n);
	}
}

void RepairItems(int n)
{
	world_t *w;
	int lvl,i;

	w=EditorGetWorld();

	// fix references inside other items
	RepairItemToItem(n);

	for(lvl=0;lvl<w->numMaps;lvl++)
	{
		// on each level, we need to fix item references

		// first, fix specials
		RepairSpecialToItem(w->map[lvl]->special,n);

		// now fix the items in the map themselves
		for(i=0;i<w->map[lvl]->width*w->map[lvl]->height;i++)
		{
			if(w->map[lvl]->map[i].item==n)
				w->map[lvl]->map[i].item=0;
			else if(w->map[lvl]->map[i].item>n)
				w->map[lvl]->map[i].item--;
		}
		// then fix any reference of a guy holding them
		for(i=0;i<MAX_MAPMONS;i++)
		{
			if(w->map[lvl]->badguy[i].item==n)
				w->map[lvl]->badguy[i].item=0;
			else if(w->map[lvl]->badguy[i].item>n && w->map[lvl]->badguy[i].item!=ITM_RANDOM)
				w->map[lvl]->badguy[i].item--;
		}
	}
}

void RepairLevels(void)
{
	world_t *w;
	int lvl;

	w=EditorGetWorld();

	for(lvl=0;lvl<w->numMaps;lvl++)
	{
		// on each level, we need to fix level references, ironically

		// fix specials
		RepairSpecialToLevel(w->map[lvl]->special);
	}
}

void RepairTiles(void)
{
	world_t *w;
	int lvl,i;

	w=EditorGetWorld();

	// fix references inside the tiles
	RepairTileToTile(w);

	for(lvl=0;lvl<w->numMaps;lvl++)
	{
		// on each level, we need to fix tile references

		// first, fix specials
		RepairSpecialToTile(w->map[lvl]->special);

		// now fix the tiles in the map themselves
		for(i=0;i<w->map[lvl]->width*w->map[lvl]->height;i++)
		{
			if(w->map[lvl]->map[i].wall!=0)
				w->map[lvl]->map[i].wall=GetSwap(w->map[lvl]->map[i].wall);
			w->map[lvl]->map[i].floor=GetSwap(w->map[lvl]->map[i].floor);
		}
	}

	RepairItemToTile();
}

void InitSwapTable(int size)
{
	int i;

	swapSize=size;
	swapTable=new int[size];

	for(i=0;i<size;i++)
		swapTable[i]=i;	// initialize each spot to itself
}

void ExitSwapTable(void)
{
	delete[] swapTable;
	swapTable=NULL;
}

void SwapInSwapTable(int me,int you)
{
	int i;

	i=swapTable[me];
	swapTable[me]=swapTable[you];
	swapTable[you]=i;
}

void DeleteFromSwapTable(int me)
{
	int i;

	for(i=swapSize-1;i>me;i--)
	{
		swapTable[i]=swapTable[i-1];
	}
	swapTable[me]=0;
}

void DeleteBlockFromSwapTable(int me,int you)
{
	int i;

	for(i=swapSize-1;i>you;i--)
	{
		swapTable[i]=swapTable[i-(you-me+1)];
	}
	for(i=me;i<=you;i++)
		swapTable[i]=0;
}

void SwapBlockInSwapTable(int me,int me2,int you)
{
	int i;

	for(i=me;i<=me2;i++)
	{
		SwapInSwapTable(i,you+i-me);
	}
}

int GetSwap(int me)
{
	if(me<0 || me>=swapSize)
		return 0;

	return swapTable[me];
}

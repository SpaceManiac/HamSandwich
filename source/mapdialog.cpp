#include "mapdialog.h"
#include "editor.h"
#include <ctype.h>

#define MAX_MAPNAMES MAX_MAPS

char mapnames[MAX_MAPNAMES][40];
char newmapname[34] = "";
static byte mapNum;
world_t *world;

void InitMapDialog(world_t *wrld, byte currentMap)
{
	int i;

	world = wrld;
	mapNum = currentMap;

	for (i = 0; i < MAX_MAPNAMES; i++)
		mapnames[i][0] = '\0';
	i = 0;

	while (i < MAX_MAPNAMES)
	{
		sprintf(mapnames[i], "%02d: %s", i, world->map[i]->name);
		i++;
		if (i >= world->numMaps)
			break;
	}
}

void ExitMapDialog(void)
{
}

void RenderMapDialog(int msx, int msy, MGLDraw *mgl)
{
	int i;

	// box for the whole dialog
	mgl->FillBox(60, 10, 480, 370, 8);
	mgl->Box(60, 10, 480, 370, 16);
	// the box that contains the map list
	mgl->Box(62, 12, 362, 352, 16);
	mgl->FillBox(63, 13, 361, 351, 0);
	for (i = 0; i < MAX_MAPNAMES; i++)
	{
		Print(67, 16 + i * 14, mapnames[i], 0, 1);
		if (msx > 64 && msx < 362 && msy > 13 + i * 14 && msy < 14 + (i + 1)*14)
			mgl->Box(64, 14 + i * 14, 360, 14 + (i + 1)*14, 16); // hilite if the cursor is on it
		if (i == mapNum)
			mgl->Box(64, 14 + i * 14, 360, 14 + (i + 1)*14, 31); // megahilite if this is the selected one
	}
	// the box to enter a new name
	mgl->Box(62, 354, 362, 368, 16);
	mgl->FillBox(63, 355, 361, 367, 0);
	Print(64, 356, newmapname, 0, 1);

	// now the buttons
	mgl->Box(380, 12, 470, 12 + 14, 16);
	Print(382, 14, "New Small", 0, 1);
	mgl->Box(380, 28, 470, 28 + 14, 16);
	Print(382, 30, "New Medium", 0, 1);
	mgl->Box(380, 44, 470, 44 + 14, 16);
	Print(382, 46, "New Large", 0, 1);
	mgl->Box(380, 60, 470, 60 + 14, 16);
	Print(382, 62, "Copy", 0, 1);

	mgl->Box(380, 120, 470, 120 + 14, 16);
	Print(382, 122, "Edit", 0, 1);
	mgl->Box(380, 200, 470, 200 + 14, 16);
	Print(382, 202, "Rename", 0, 1);
	mgl->Box(380, 280, 470, 280 + 14, 16);
	Print(382, 282, "Delete", 0, 1);
}

byte MapDialogKey(char key)
{
	int len;

	if (key == 27) // esc
		return 0;

	if (key == 8) // backspace
	{
		if (strlen(newmapname) > 0)
			newmapname[strlen(newmapname) - 1] = '\0';
		return 1;
	}

	if (key == 10) // enter
	{
		return 1; // ignore it- what does enter do?  Load or save?
	}

	if (!isprint(key))
	{
		// non-printables keep ending up in the PixelToaster keyboard
		return 1;
	}

	if (strlen(newmapname) < 31)
	{
		len = strlen(newmapname);
		newmapname[len] = key;
		newmapname[len + 1] = '\0';
	}
	return 1;
}

byte MapDialogClick(int msx, int msy)
{
	int i;

	// if click on a mapname, that's the current map
	for (i = 0; i < MAX_MAPNAMES; i++)
		if (msx > 64 && msx < 362 && msy > 13 + i * 14 && msy < 14 + (i + 1)*14)
		{
			if (i < world->numMaps)
			{
				EditorSelectMap(i);
				mapNum = i;
			}
		}

	// now the buttons
	if (msx > 379 && msy > 11 && msx < 471 && msy < 11 + 15)
	{
		// new small
		if (world->numMaps < MAX_MAPS)
		{
			world->numMaps++;
			world->map[world->numMaps - 1] = new Map(0, newmapname);
		}
	}
	if (msx > 379 && msy > 28 && msx < 471 && msy < 28 + 15)
	{
		// new medium
		if (world->numMaps < MAX_MAPS)
		{
			world->numMaps++;
			world->map[world->numMaps - 1] = new Map(1, newmapname);
		}
	}
	if (msx > 379 && msy > 44 && msx < 471 && msy < 44 + 15)
	{
		// new large
		if (world->numMaps < MAX_MAPS)
		{
			world->numMaps++;
			world->map[world->numMaps - 1] = new Map(2, newmapname);
		}
	}
	if (msx > 379 && msy > 60 && msx < 471 && msy < 60 + 15)
	{
		// copy
		if (world->numMaps < MAX_MAPS)
		{
			world->numMaps++;
			world->map[world->numMaps - 1] = new Map(world->map[mapNum]);
		}
	}
	if (msx > 379 && msy > 120 && msx < 471 && msy < 120 + 15)
	{
		return 0; // edit
	}
	if (msx > 379 && msy > 200 && msx < 471 && msy < 200 + 15)
	{
		// rename
		strncpy(world->map[mapNum]->name, newmapname, 32);
	}
	if (msx > 379 && msy > 280 && msx < 471 && msy < 280 + 15)
	{
		// delete
		if (mapNum != 0) // can't delete the original one
		{
			delete world->map[mapNum];
			for (i = mapNum + 1; i <= world->numMaps - 1; i++)
			{
				world->map[i - 1] = world->map[i];
			}
			world->map[world->numMaps - 1] = NULL;
			world->numMaps--;
			EditorSelectMap(0);
			mapNum = 0;
		}
	}

	// reset the mapname stuff in case it was tweaked around, like a map was
	// renamed or deleted
	for (i = 0; i < MAX_MAPNAMES; i++)
		mapnames[i][0] = '\0';
	i = 0;

	while (i < MAX_MAPNAMES)
	{
		sprintf(mapnames[i], "%02d: %s", i, world->map[i]->name);
		i++;
		if (i >= world->numMaps)
			break;
	}

	return 1;
}

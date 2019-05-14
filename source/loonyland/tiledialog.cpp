#include "tiledialog.h"
#include "editor.h"
#include "lsdir.h"
#include <memory>

#define MAX_FILES 18

static char fnames[MAX_FILES][32];
static char newfname[32]="";
static byte numFiles;
static std::unique_ptr<filterdir> filter;

void InitTileDialog(void)
{
	int i;

	for(i=0;i<MAX_FILES;i++)
		fnames[i][0]='\0';

	numFiles=0;
	filter = std::make_unique<filterdir>("graphics", ".bmp", 32);
	for (const char* name : *filter)
	{
		strncpy(fnames[numFiles++], name, 32);
		if (numFiles >= MAX_FILES)
			break;
	}
}

void ExitTileDialog(void)
{
	filter.reset();
}

void RenderTileDialog(int msx,int msy,MGLDraw *mgl)
{
	int i;

	// box for the whole dialog
	mgl->FillBox(100,80,430,400,8);
	mgl->Box(100,80,430,400,16);
	// the box that contains the file list
	mgl->Box(102,82,362,340,16);
	mgl->FillBox(103,83,361,339,0);
	for(i=0;i<MAX_FILES;i++)
	{
		Print(107,86+i*14,fnames[i],0,1);
		if(msx>104 && msx<362 && msy>85+i*14 && msy<85+(i+1)*14)
			mgl->Box(104,84+i*14,360,84+(i+1)*14,16);	// hilite if the cursor is on it
	}
	// the box to enter a new filename
	mgl->Box(102,342,362,356,16);
	mgl->FillBox(103,343,361,355,0);
	Print(104,344,newfname,0,1);

	// now the buttons
	mgl->Box(102,358,182,372,16);
	Print(104,360,"More Files",0,1);
	mgl->Box(370,180,420,180+14,16);
	Print(372,182,"Load 1",0,1);
	mgl->Box(370,210,420,210+14,16);
	Print(372,212,"Load 2",0,1);
	mgl->Box(370,240,420,240+14,16);
	Print(372,242,"Load 3",0,1);
	mgl->Box(370,370,420,370+14,16);
	Print(372,372,"Quit",0,1);
}

byte TileDialogKey(char key)
{
	int len;

	if(key==27)	// esc
		return 0;

	if(key==8)	// backspace
	{
		if(strlen(newfname)>0)
			newfname[strlen(newfname)-1]='\0';
		return 1;
	}

	if(key==13)	// enter
	{
		// load the named one
	}

	if(strlen(newfname)<30)
	{
		len=strlen(newfname);
		newfname[len]=key;
		newfname[len+1]='\0';
	}
	return 1;
}

void TileDialogMoreFiles(void)
{
	int i;

	for(i=0;i<MAX_FILES;i++)
		fnames[i][0]='\0';

	numFiles=0;
	for (const char* name : *filter)
	{
		strncpy(fnames[numFiles++], name, 32);
		if (numFiles >= MAX_FILES)
			break;
	}
	if(numFiles==0)	// there aren't any more to list at all!
	{
		ExitTileDialog();
		InitTileDialog();	// reget the first page of them
	}
}

byte TileDialogClick(int msx,int msy)
{
	int i;
	char fn[64];

	sprintf(fn,"graphics/%s",newfname);
	// if click on a filename, that's the current filename
	for(i=0;i<MAX_FILES;i++)
		if(msx>104 && msx<362 && msy>85+i*14 && msy<85+(i+1)*14)
		{
			strcpy(newfname,fnames[i]);
			return 1;
		}


	if(msx>102 && msx<182 && msy>358 && msy<372)	// More Files
	{
		TileDialogMoreFiles();
		return 1;
	}
	if(msx>370 && msy>180 && msx<420 && msy<180+14)	// Load 1
	{
		EditorLoadTiles(fn,0);
		return 0;
	}
	if(msx>370 && msy>210 && msx<420 && msy<210+14)	// Load 2
	{
		EditorLoadTiles(fn,1);
		return 0;
	}
	if(msx>370 && msy>240 && msx<420 && msy<240+14)	// Load 3
	{
		EditorLoadTiles(fn,2);
		return 0;
	}
	if(msx>370 && msy>370 && msx<420 && msy<370+14)	// Quit
		return 0;

	return 1;
}

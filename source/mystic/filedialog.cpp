#include "filedialog.h"
#include "editor.h"
#include "appdata.h"
#include <memory>

#define MAX_FILES 18

static char fnames[MAX_FILES][32];
static char newfname[32]="";
static byte numFiles;

static std::vector<std::string> files_all;
static std::vector<std::string>::iterator files_current;

void InitFileDialog(void)
{
	int i;

	for(i=0;i<MAX_FILES;i++)
		fnames[i][0]='\0';

	numFiles=0;
	files_all = ListDirectory(".", ".dlw", 32);
	for (files_current = files_all.begin(); files_current != files_all.end(); ++files_current)
	{
		const char* name = files_current->c_str();
		strncpy(fnames[numFiles++], name, 32);
		if (numFiles >= MAX_FILES)
			break;
	}
}

void ExitFileDialog(void)
{
	files_all.clear();
}

void RenderFileDialog(int msx,int msy,MGLDraw *mgl)
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
	mgl->Box(370,92,420,92+14,16);
	Print(372,94,"New",0,1);
	mgl->Box(370,180,420,180+14,16);
	Print(372,182,"Load",0,1);
	mgl->Box(370,270,420,270+14,16);
	Print(372,272,"Save",0,1);
	mgl->Box(370,370,420,370+14,16);
	Print(372,372,"Quit",0,1);
}

byte FileDialogKey(char key)
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
		return 1;	// ignore it- what does enter do?  Load or save?
	}

	if(strlen(newfname)<30)
	{
		len=strlen(newfname);
		newfname[len]=key;
		newfname[len+1]='\0';
	}
	return 1;
}

void FileDialogMoreFiles(void)
{
	int i;

	for(i=0;i<MAX_FILES;i++)
		fnames[i][0]='\0';

	numFiles=0;
	for (; files_current != files_all.end(); ++files_current)
	{
		const char* name = files_current->c_str();
		strncpy(fnames[numFiles++], name, 32);
		if (numFiles >= MAX_FILES)
			break;
	}
	if(numFiles==0)	// there aren't any more to list at all!
	{
		ExitFileDialog();
		InitFileDialog();	// reget the first page of them
	}
}

byte FileDialogClick(int msx,int msy)
{
	int i;
	char fn[64];

	sprintf(fn,"%s",newfname);
	// if click on a filename, that's the current filename
	for(i=0;i<MAX_FILES;i++)
		if(msx>104 && msx<362 && msy>85+i*14 && msy<85+(i+1)*14)
		{
			strcpy(newfname,fnames[i]);
			return 1;
		}

		// shareware version doesn't let you do this
	if(msx>102 && msx<182 && msy>358 && msy<372)	// More Files
	{
		FileDialogMoreFiles();
		return 1;
	}

	if(msx>370 && msy>92 && msx<420 && msy<92+14)	// New
	{
		EditorNewWorld();
		return 0;
	}
	if(msx>370 && msy>180 && msx<420 && msy<180+14)	// Load
	{
		EditorLoadWorld(fn);
		return 0;
	}
	if(msx>370 && msy>270 && msx<420 && msy<270+14)	// Save
	{
		EditorSaveWorld(fn);
		return 0;
	}
	if(msx>370 && msy>370 && msx<420 && msy<370+14)	// Quit
		return 0;

	return 1;
}

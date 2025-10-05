#include "filedialog.h"
#include <string.h>
#include <algorithm>
#include <array>
#include "editor.h"
#include "dialogbits.h"
#include "viewdialog.h"
#include "appdata.h"
#include "string_extras.h"

static constexpr int MAX_FILES = 18;

static char newfname[FNAMELEN]="";
static FileDialogFlags menuItems;
static int filePos;
static byte asking,yesNo;
static char question[64];
static word exitCode;
static bool hamSandwich;

std::vector<std::string> fnames;

static void ObtainFilenames(const char *dir, const char *ext)
{
	fnames.clear();
	fnames.reserve(64);

	auto files = ListDirectory(dir, ext, FNAMELEN);
	for (const auto& str : files)
	{
		const char* extension = "";
		size_t dot = str.rfind('.');
		if (dot != std::string::npos)
		{
			extension = &str[dot];
		}

		if ((menuItems & FM_PICMOVIE) && !(!strcasecmp(extension, ".txt") || !strcasecmp(extension, ".bmp") || !strcasecmp(extension, ".flc")))
			continue;
		if ((menuItems & FM_SOUNDS) && !(!strcasecmp(extension, ".wav") || !strcasecmp(extension, ".ogg")))
			continue;

		fnames.push_back(str);
	}
}

static void SortFilenames(void)
{
	bool flip = true;
	while(flip)
	{
		flip = false;
		for(size_t i=0;i<fnames.size()-1;i++)
		{
			if(strcasecmp(fnames[i].c_str(),fnames[i+1].c_str())>0)
			{
				std::swap(fnames[i], fnames[i+1]);
				flip = true;
			}
		}
	}
}

static void FindFilename(const char *str);

void InitFileDialog(const char *dir, const char *ext, FileDialogFlags menuItemsToShow,const char *defaultName)
{
	menuItems=menuItemsToShow;
	asking=0;
	exitCode=0;
	filePos=0;

	ObtainFilenames(dir, ext);
	SortFilenames();
	strcpy(newfname,defaultName);
	FindFilename(defaultName);

	if (menuItems & (FM_SAVE | FM_SAVEPACK))
	{
		hamSandwich = MustBeHamSandwichWorld(EditorGetWorld());
	}
}

void ExitFileDialog(void)
{
	fnames.clear();
	fnames.shrink_to_fit();

	if(menuItems&FM_PLAYSONGS)
		ReturnToSong();
}

void FileDialogScroll(int msz)
{
	if (msz > 0)
	{
		filePos = std::max(filePos - msz, 0);
	}
	else if (msz < 0)
	{
		filePos = std::min(filePos - msz, (int)(fnames.size() - 1));
	}
}

void RenderFileDialog(int msx,int msy,MGLDraw *mgl)
{
	int i;

	// box for the whole dialog
	mgl->FillBox(100,80,430,400,8);
	mgl->Box(100,80,430,400,31);
	// the box that contains the file list
	mgl->Box(102,82,362,340,16);
	mgl->FillBox(103,83,361,339,0);
	for(i=0;i<MAX_FILES;i++)
	{
		if(i+filePos < (int)fnames.size())
		{
			Print(107,86+i*14,fnames[i+filePos],0,1);
			if(msx>104 && msx<362 && msy>85+i*14 && msy<85+(i+1)*14)
				mgl->Box(104,84+i*14,360,84+(i+1)*14,31);	// hilite if the cursor is on it
		}
	}
	// the box to enter a new filename
	mgl->Box(102,342,362,356,16);
	mgl->FillBox(103,343,361,355,0);
	Print(104,344,newfname,0,1);

	// now the buttons
	if(msx>=102 && msx<=182 && msy>=358 && msy<=372)
		mgl->FillBox(102,358,182,372,8+32*1);
	mgl->Box(102,358,182,372,31);
	Print(104,360,"More Files",0,1);
	if(menuItems&FM_NEW)
	{
		if(msx>=370 && msx<=420 && msy>=92 && msy<=92+14)
			mgl->FillBox(370,92,420,92+14,8+32*1);
		mgl->Box(370,92,420,92+14,31);
		Print(372,94,"New",0,1);
	}
	if(menuItems&FM_LOAD)
	{
		if(msx>=370 && msx<=420 && msy>=180 && msy<=180+14)
			mgl->FillBox(370,180,420,180+14,8+32*1);
		mgl->Box(370,180,420,180+14,31);
		if(menuItems&FM_MERGE)
			Print(372,182,"Merge",0,1);
		else
			Print(372,182,"Load",0,1);
	}
	if(menuItems&FM_SAVE)
	{
		if(msx>=370 && msx<=420 && msy>=270 && msy<=270+14)
			mgl->FillBox(370,270,420,270+14,8+32*1);
		mgl->Box(370,270,420,270+14,31);
		Print(372,272,"Save",0,1);

		if (hamSandwich)
		{
			Print(366,286,"HamSwch",0,1);
			Print(366,298,"format",0,1);
		}
	}
	if ((menuItems&FM_SAVEPACK) && !hamSandwich)  // For now, no exporting worlds in new format.
	{
		if(msx>=370 && msx<=420 && msy>=290 && msy<=290+27)
			mgl->FillBox(370,290,420,290+27,8+32*1);
		mgl->Box(370,290,420,290+27,31);
		Print(372,292,"Save&",0,1);
		Print(372,292+13,"Export",0,1);
	}

	if(msx>=370 && msx<=420 && msy>=370 && msy<=370+14)
		mgl->FillBox(370,370,420,370+14,8+32*1);
	mgl->Box(370,370,420,370+14,31);
	Print(372,372,"Quit",0,1);

	if(asking)
	{
		mgl->FillBox(320-140,240-10,320+140,240+60,8);
		mgl->Box(320-140,240-10,320+140,240+60,31);
		CenterPrint(320,244,question,0,1);
		RenderButtonImage(msx,msy,320-50-15,240+40,30,15,"Yes");
		RenderButtonImage(msx,msy,320+50-15,240+40,30,15,"No");
	}
}

void FileDialogYes(void)
{
	exitCode=asking;
	asking=0;
}

void FindFilename(const char *str)
{
	int match,matchAmt;
	int m;
	char a,b;

	if(str[0]=='\0')	// blank, go to front of the list
	{
		filePos=0;
		return;
	}

	match=0;
	matchAmt=0;
	for(size_t i=0;i<fnames.size();i++)
	{
		m=0;
		for(int j=0;j<(int)strlen(str);j++)
		{
			a=str[j];
			if(a>='A' && a<='Z')
				a=a+'a'-'A';
			b=fnames[i][j];
			if(b>='A' && b<='Z')
				b=b+'a'-'A';
			if(a==b)
				m++;
			else
				break;
		}
		if(m>matchAmt)
		{
			matchAmt=m;
			match=i;
		}
	}
	filePos=match;
}

byte FileDialogKey(char key)
{
	int len;

	if(asking)
	{
		if(key==27 || key=='n' || key=='N')
		{
			yesNo=0;
			asking=0;
		}
		if(key=='y' || key=='Y')
		{
			FileDialogYes();
		}
		return 1;
	}
	else
	{
		if(key==27)	// esc
			return 0;

		if(key==8)	// backspace
		{
			if(strlen(newfname)>0)
				newfname[strlen(newfname)-1]='\0';
			FindFilename(newfname);
			return 1;
		}

		if(key==13)	// enter
		{
			return 1;	// ignore it- what does enter do?  Load or save?
		}

		if(strlen(newfname)<30 && key!=0)
		{
			len=strlen(newfname);
			newfname[len]=key;
			newfname[len+1]='\0';
			FindFilename(newfname);
		}
	}
	return 1;
}

void FileDialogMoreFiles(void)
{
	filePos += MAX_FILES;

	if(filePos >= (int)fnames.size())
		filePos = 0;
}

word FileDialogClick(int msx,int msy)
{
	int i;

	if(exitCode)
		return exitCode;

	if(asking)
	{
		if(PointInRect(msx,msy,320-50-15,240+40,320-50+15,240+40+15))
		{
			FileDialogYes();
			if(exitCode)
				return exitCode;
		}
		else if(PointInRect(msx,msy,320+50-15,240+40,320+50+15,240+40+15))
		{
			asking=0;
			yesNo=0;
		}
	}
	else
	{
		// if click on a filename, that's the current filename
		for(i=0;i<MAX_FILES;i++)
			if(msx>104 && msx<362 && msy>85+i*14 && msy<85+(i+1)*14 && i+filePos<(int)fnames.size())
			{
				ham_strcpy(newfname, fnames[i+filePos]);
				if(menuItems&FM_PLAYSONGS)
					PlaySong(newfname);
				return 0;
			}


		if(msx>102 && msx<182 && msy>358 && msy<372)	// More Files
		{
			FileDialogMoreFiles();
			return 0;
		}
		if(msx>370 && msy>92 && msx<420 && msy<92+14)	// New
		{
			if(menuItems&FM_NEW)
			{
				strcpy(question,"Really start over from scratch?");
				asking=FM_NEW;
			}
		}
		if(msx>370 && msy>180 && msx<420 && msy<180+14)	// Load
		{
			if(newfname[0])
			{
				if(menuItems&FM_LOAD)
				{
					if(menuItems&FM_ASKLOAD)
					{
						if(menuItems&FM_MERGE)
							sprintf(question,"Merge in %s?",newfname);
						else
							sprintf(question,"Load %s?",newfname);
						asking=FM_LOAD;
					}
					else
					{
						exitCode=FM_LOAD;
						return FM_LOAD;
					}
				}
			}
		}
		if(msx>370 && msy>270 && msx<420 && msy<270+14)	// Save
		{
			if(newfname[0])
			{
				if(menuItems&FM_SAVE)
				{
					exitCode=FM_SAVE;
					return FM_SAVE;
				}
			}
		}
		if(msx>=370 && msx<=420 && msy>=290 && msy<=290+27)  // Save&Export
		{
			if ((menuItems & FM_SAVEPACK) && !hamSandwich && newfname[0])
			{
				exitCode = FM_SAVEPACK;
				return FM_SAVEPACK;
			}
		}
		if(msx>370 && msy>370 && msx<420 && msy<370+14)	// Quit
		{
			exitCode=FM_EXIT;
			return FM_EXIT;
		}
	}

	return 0;
}

const char *GetFilename(const char *header)
{
	static char result[64];

	sprintf(result,"%s%s",header,newfname);

	return result;
}

void AddDLWToFilename(void)
{
	char result[64];

	sprintf(result,"%s.dlw",newfname);
	strcpy(newfname,result);
}

word FileDialogCommand(void)
{
	if((menuItems&FM_MERGE) && exitCode==FM_LOAD)
		return FM_MERGE;
	else
		return exitCode;
}

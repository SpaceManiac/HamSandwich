#include "soundedit.h"
#include "dialogbits.h"
#include "filedialog.h"
#include "textdialog.h"
#include "yesnodialog.h"
#include "sound.h"
#include "tool.h"
#include "edithelp.h"

#define SNDMODE_NORMAL	0	// doing nothing
#define SNDMODE_SELECT	1	// selecting a sound (for a special or whatever)
#define SNDMODE_DELETE	2	// asking yes/no on sound deletion
#define SNDMODE_LOAD	3	// picking a wav file (file menu)
#define SNDMODE_NAME	4	// entering a name
#define SNDMODE_RELOAD	5	// file menu again, but to replace
#define SNDMODE_HELP	6

#define ID_EXIT		1
#define ID_DELETE	2
#define ID_NEW		3
#define ID_RENAME	4
#define ID_RELOAD	5
#define ID_MORESNDS 6
#define ID_PICKTHEME	50
#define ID_SOUNDS	100

static byte mode,curTheme,rememberMode;
static world_t *world;
static word curSound;
static byte helpRemember;

static dword themes[]={ST_INTFACE,ST_PLAYER,ST_MONSTER,ST_EFFECT,ST_VOCAL,ST_CUSTOM};

static word sndList[600];
static word sndsInList,sndStart,sndsShown;

static void MakeSoundList(void);

static void DeleteClick(int id)
{
	char s[128];

	MakeNormalSound(SND_MENUCLICK);
	if(curSound>=CUSTOM_SND_START)
	{
		mode=SNDMODE_DELETE;
		sprintf(s,"Delete '%s'?",GetSoundInfo(curSound)->name);
		InitYesNoDialog(s,"Yes!","No!");
		MakeNormalSound(SND_MENUSELECT);
	}
}

static void NewClick(int id)
{
	MakeNormalSound(SND_MENUCLICK);
	if(GetNumCustomSounds()==MAX_CUSTOM_SOUNDS)
		return;

	mode=SNDMODE_LOAD;
	InitFileDialog("user/*.wav",FM_LOAD|FM_EXIT,"");
	MakeNormalSound(SND_MENUSELECT);
}

static void RenameClick(int id)
{
	MakeNormalSound(SND_MENUCLICK);
	if(curSound>=CUSTOM_SND_START)
	{
		mode=SNDMODE_NAME;
		InitTextDialog("Enter a name for this sound",GetSoundInfo(curSound)->name,31);
		MakeNormalSound(SND_MENUSELECT);
	}
}

static void ReloadClick(int id)
{
	MakeNormalSound(SND_MENUCLICK);
	if(curSound>=CUSTOM_SND_START)
	{
		mode=SNDMODE_RELOAD;
		InitFileDialog("user/*.wav",FM_LOAD|FM_EXIT,"");
		MakeNormalSound(SND_MENUSELECT);
	}
}

static void ExitClick(int id)
{
	PickedTile(-1);
	SetEditMode(rememberMode);
	MakeNormalSound(SND_MENUCLICK);
}

static void PickSoundClick(int id)
{
	RadioOn(id,ID_SOUNDS,ID_SOUNDS+200);
	curSound=sndList[sndStart+(id-ID_SOUNDS)];
	if(mode==SNDMODE_SELECT)
	{
		PickedTile(curSound);
		SetEditMode(rememberMode);
	}
}

static void SoundRightClick(int id)
{
	int s;
	s=sndList[sndStart+(id-ID_SOUNDS)];
	JamulSoundPurge();
	MakeNormalCustomSound(s);
}

static void MoreSoundsClick(int id)
{
	int i;
	int x,y;

	sndStart+=sndsShown;
	if(sndStart>=sndsInList)
		sndStart=0;

	ClearButtons(ID_SOUNDS,ID_SOUNDS+200);
	ClearButtons(ID_MORESNDS,ID_MORESNDS);

	// now make buttons for the first N, which is however many fit on the screen
	x=163;
	y=6;
	sndsShown=0;
	for(i=0;i<sndsInList-sndStart;i++)
	{
		MakeButton(BTN_RADIO,ID_SOUNDS+i,0,x,y,156,16,GetSoundInfo(sndList[i+sndStart])->name,PickSoundClick);
		sndsShown++;
		y+=18;
		if(x>640-160 && y>480-30)
			break;
		else if(y>480-16)
		{
			x+=159;
			y=6;
		}
	}
	MakeButton(BTN_NORMAL,ID_MORESNDS,0,x,y,156,16,"More Sounds...",MoreSoundsClick);
}

static void MakeSoundList(void)
{
	int i,pos,x,y;
	int b,swapped;

	ClearButtons(ID_SOUNDS,ID_SOUNDS+200);
	ClearButtons(ID_MORESNDS,ID_MORESNDS);

	sndStart=0;
	pos=0;
	sndsInList=0;
	// get all sounds which match this theme
	for(i=0;i<GetNumSounds();i++)
	{
		if(GetSoundInfo(i)->theme&themes[curTheme])
		{
			sndList[pos]=i;
			sndsInList=pos+1;
			pos++;
		}
	}

	// now sort them alphabetically, simple bubble sort
	swapped=1;
	while(swapped)
	{
		swapped=0;
		for(i=0;i<sndsInList-1;i++)
		{
			if(strcmp(GetSoundInfo(sndList[i])->name,GetSoundInfo(sndList[i+1])->name)>0)
			{
				b=sndList[i+1];
				sndList[i+1]=sndList[i];
				sndList[i]=b;
				swapped=1;
			}
		}
	}

	// now make buttons for the first N, which is however many fit on the screen
	x=163;
	y=6;
	sndsShown=0;
	for(i=0;i<sndsInList-sndStart;i++)
	{
		MakeButton(BTN_RADIO,ID_SOUNDS+i,(sndList[i+sndStart]==curSound)*CHECK_ON,x,y,156,16,GetSoundInfo(sndList[i+sndStart])->name,PickSoundClick);
		sndsShown++;
		y+=18;
		if(x>640-160 && y>480-30)
		{
			MakeButton(BTN_NORMAL,ID_MORESNDS,0,x,y,156,16,"More Sounds...",MoreSoundsClick);
			break;
		}
		else if(y>480-16)
		{
			x+=159;
			y=6;
		}
	}
}

static void PickThemeClick(int id)
{
	RadioOn(id,ID_PICKTHEME,ID_PICKTHEME+20);
	curTheme=id-ID_PICKTHEME;
	MakeSoundList();
}

static void SoundEditSetupButtons(void)
{
	ClearButtons();

	// theme selection
	MakeButton(BTN_RADIO,ID_PICKTHEME+0,0,2,2+18*0,156,16,"Interface Sounds",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+1,0,2,2+18*1,156,16,"Player Sounds",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+2,0,2,2+18*2,156,16,"Monster Sounds",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+3,0,2,2+18*3,156,16,"Sound Effects",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+4,0,2,2+18*4,156,16,"Vocals",PickThemeClick);
	MakeButton(BTN_RADIO,ID_PICKTHEME+5,0,2,2+18*5,156,16,"Custom Sounds",PickThemeClick);

	// delete, move, and exit
	if(mode!=SNDMODE_SELECT)
	{
		MakeButton(BTN_NORMAL,ID_NEW,0,2,460-120,156,14,"Add New Sound",NewClick);
		MakeButton(BTN_NORMAL,ID_DELETE,0,2,460-100,156,14,"Delete Sound",DeleteClick);
		MakeButton(BTN_NORMAL,ID_RENAME,0,2,460-80,156,14,"Rename Sound",RenameClick);
		MakeButton(BTN_NORMAL,ID_RELOAD,0,2,460-60,156,14,"Reload Sound",ReloadClick);
	}

	MakeButton(BTN_NORMAL,ID_EXIT,0,2,460,156,14,"Exit Sound Editor",ExitClick);
}

void SoundEdit_Init(world_t *wrld)
{
	GetDisplayMGL()->MouseTap();
	world=wrld;
	mode=SNDMODE_NORMAL;
	curSound=0;
	RadioOn(ID_PICKTHEME+0,ID_PICKTHEME,ID_PICKTHEME+20);
	curTheme=0;
	MakeSoundList();
	rememberMode=EDITMODE_EDIT;
	SoundEditSetupButtons();
}

void SoundEdit_InitPicker(byte modeFrom,world_t *wrld)
{
	rememberMode=modeFrom;
	GetDisplayMGL()->MouseTap();
	world=wrld;
	mode=SNDMODE_SELECT;
	SoundEditSetupButtons();
	curSound=0;
	RadioOn(ID_PICKTHEME+0,ID_PICKTHEME,ID_PICKTHEME+20);
	curTheme=0;
	MakeSoundList();
}

void SoundEdit_Exit(void)
{
}

void SoundEdit_Update(int mouseX,int mouseY,MGLDraw *mgl)
{
	int i;


	switch(mode)
	{
		case SNDMODE_NORMAL:
			if(mgl->MouseTap())
			{
				CheckButtons(mouseX,mouseY);
			}

			if(mgl->RMouseTap())
			{
				for(i=0;i<sndsInList;i++)
					if(CheckButtonCallback(mouseX,mouseY,ID_SOUNDS+i,SoundRightClick))
						break;
			}
			break;
		case SNDMODE_SELECT:
			if(mgl->MouseTap())
			{
				CheckButtons(mouseX,mouseY);
			}

			if(mgl->RMouseTap())
			{
				for(i=0;i<sndsInList;i++)
					if(CheckButtonCallback(mouseX,mouseY,ID_SOUNDS+i,SoundRightClick))
						break;
			}
			break;
		case SNDMODE_LOAD:
		case SNDMODE_RELOAD:
			if(mgl->MouseTap())
			{
				i=FileDialogClick(mouseX,mouseY);
				if(i==FM_LOAD)
				{
					if(mode==SNDMODE_LOAD)
					{
						if(AddCustomSound(GetFilename("user/")))
						{
							curTheme=5;
							curSound=GetNumCustomSounds()+CUSTOM_SND_START-1;
							RadioOn(curTheme+ID_PICKTHEME,ID_PICKTHEME,ID_PICKTHEME+20);
							MakeSoundList();
						}
					}
					else if(mode==SNDMODE_RELOAD)
					{
						ReplaceCustomSound(curSound-CUSTOM_SND_START,GetFilename("user/"));
					}
					ExitFileDialog();
					mode=SNDMODE_NORMAL;
				}
				if(i==FM_EXIT)
				{
					ExitFileDialog();
					mode=SNDMODE_NORMAL;
				}
			}
			break;
		case SNDMODE_NAME:
			if(TextDialogCommand()==TM_OKAY)
			{
				strcpy(GetSoundInfo(curSound)->name,GetText());
				mode=SNDMODE_NORMAL;
				ExitTextDialog();
				MakeSoundList();
			}
			else if(TextDialogCommand()==TM_CANCEL)
			{
				mode=SNDMODE_NORMAL;
				ExitTextDialog();
			}
			if(mgl->MouseTap())
			{
				TextDialogClick(mouseX,mouseY);
			}
			break;
		case SNDMODE_DELETE:
			if(mgl->MouseTap())
			{
				YesNoDialogClick(mouseX,mouseY);
			}

			if(YesNoDialogCommand()==YNM_YES)
			{
				ExitYesNoDialog();
				DeleteCustomSound(curSound-CUSTOM_SND_START);
				MakeNormalSound(SND_MENUSELECT);
				curSound=0;
				MakeSoundList();
				mode=SNDMODE_NORMAL;
			}
			else if(YesNoDialogCommand()==YNM_NO)
			{
				ExitYesNoDialog();
				MakeNormalSound(SND_MENUSELECT);
				mode=SNDMODE_NORMAL;
			}
			break;
		case SNDMODE_HELP:
			if(mgl->MouseTap())
			{
				if(EditHelpClick(mouseX,mouseY))
					mode=helpRemember;
			}
			break;
	}
}

void SoundEdit_Key(char k)
{
	switch(mode)
	{
		case SNDMODE_NORMAL:
			if(k==27)
			{
				SetEditMode(rememberMode);
				return;
			}
			break;
		case SNDMODE_SELECT:
			if(k==27)
			{
				PickedTile(-1);
				SetEditMode(rememberMode);
				return;
			}
			break;
		case SNDMODE_LOAD:
		case SNDMODE_RELOAD:
			if(!FileDialogKey(k))
			{
				ExitFileDialog();
				mode=SNDMODE_NORMAL;
			}
			break;
		case SNDMODE_NAME:
			if(!TextDialogKey(k))
			{
				ExitTextDialog();
				mode=SNDMODE_NORMAL;
			}
			break;
		case SNDMODE_DELETE:
			YesNoDialogKey(k);
			break;
		case SNDMODE_HELP:
			if(!EditHelpKey(k))
				mode=helpRemember;
			break;

	}
}

void SoundEdit_Render(int mouseX,int mouseY,MGLDraw *mgl)
{
	mgl->ClearScreen();

	// misc. lines and the background behind the buttons
	mgl->FillBox(160,0,160,479,31);
	mgl->FillBox(0,0,159,479,32*1+4);

	switch(mode)
	{
		case SNDMODE_NORMAL:
		case SNDMODE_SELECT:
			RenderButtons(mouseX,mouseY,mgl);
			break;
		case SNDMODE_LOAD:
		case SNDMODE_RELOAD:
			RenderButtons(-1,-1,mgl);	// prevent buttons from lighting up!
			RenderFileDialog(mouseX,mouseY,mgl);
			break;
		case SNDMODE_NAME:
			RenderButtons(-1,-1,mgl);	// prevent buttons from lighting up!
			RenderTextDialog(mouseX,mouseY,mgl);
			break;
		case SNDMODE_DELETE:
			RenderButtons(-1,-1,mgl);	// prevent buttons from lighting up!
			RenderYesNoDialog(mouseX,mouseY,mgl);
			break;
		case SNDMODE_HELP:
			RenderButtons(-1,-1,mgl);	// prevent buttons from lighting up!
			RenderEditHelp(mouseX,mouseY,mgl);
			break;
	}
}

void SoundEdit_Help(void)
{
	if(mode==SNDMODE_HELP)
		return;

	helpRemember=mode;
	if(mode!=SNDMODE_SELECT)
		InitEditHelp(HELP_SOUNDEDIT);
	else
		InitEditHelp(HELP_SOUNDPICK);
	mode=SNDMODE_HELP;
}

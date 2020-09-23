#include "mapdialog.h"
#include "editor.h"
#include "dialogbits.h"
#include "textdialog.h"
#include "repair.h"

#define WD_NORMAL	0
#define WD_NAME		1

#define MAX_MAPSHOW		18
static int mapPos;
static byte asking,yesNo;
static char question[64];

char mapnames[MAX_MAPSHOW][40];
char newmapname[34]="";
static byte mapNum;
world_t *world;
static byte mode;

void InitMapDialog(world_t *wrld,byte currentMap)
{
	int i;

	mode=WD_NORMAL;

	world=wrld;
	mapNum=currentMap;
	if(mapNum>=world->numMaps)
		mapNum=0;

	mapPos=(mapNum/MAX_MAPSHOW)*18;
	for(i=0;i<MAX_MAPSHOW;i++)
		mapnames[i][0]='\0';
	i=0;

	while(i<MAX_MAPSHOW)
	{
		sprintf(mapnames[i],"%02d: %s",i+mapPos,world->map[i+mapPos]->name);
		i++;
		if(i+mapPos>=world->numMaps)
			break;
	}

	asking=0;
}

void ExitMapDialog(void)
{
}

void RenderMapDialog(int msx,int msy,MGLDraw *mgl)
{
	int i;

	// box for the whole dialog
	mgl->FillBox(70,80,460,400,8);
	mgl->Box(70,80,460,400,31);
	// the box that contains the map list
	mgl->Box(72,82,392,340,16);
	mgl->FillBox(73,83,391,339,0);
	for(i=0;i<MAX_MAPSHOW;i++)
	{
		if(i+mapPos<world->numMaps)
		{
			if(i+mapPos==mapNum)
				mgl->FillBox(75,85+i*14,389,83+(i+1)*14,32+6);
			Print(77,86+i*14,mapnames[i],0,1);
			if(msx>74 && msx<392 && msy>85+i*14 && msy<85+(i+1)*14)
				mgl->Box(74,84+i*14,390,84+(i+1)*14,31);	// hilite if the cursor is on it
		}
	}
	// the box to enter a new map name
	mgl->Box(72,342,392,356,16);
	mgl->FillBox(73,343,391,355,0);
	Print(74,344,newmapname,0,1);

	// now the buttons
	if(msx>=72 && msx<=187 && msy>=358 && msy<=372)
		mgl->FillBox(72,358,187,372,8+32*1);
	mgl->Box(72,358,187,372,31);
	Print(74,360,"More Levels...",0,1);

	// new, delete, copy, move up, move down
	RenderButtonImage(msx,msy,400,92,56,15,"New");
	RenderButtonImage(msx,msy,400,92+17,56,15,"Copy");
	RenderButtonImage(msx,msy,400,92+17*2,56,15,"Rename");
	RenderButtonImage(msx,msy,400,92+17*3,56,15,"Delete");
	RenderButtonImage(msx,msy,400,92+17*4,56,15,"Move Up");
	RenderButtonImage(msx,msy,400,92+17*5,56,15,"Move Dn");
	RenderButtonImage(msx,msy,400,380,56,15,"Exit");

	RenderButtonImage(msx,msy,72,380,100,15,"Author Name");
	PrintLimited(176,382,368,world->author,0,1);

	if(asking)
	{
		mgl->FillBox(320-140,240-10,320+140,240+60,8);
		mgl->Box(320-140,240-10,320+140,240+60,31);
		CenterPrint(320,244,question,0,1);
		RenderButtonImage(msx,msy,320-50-15,240+40,30,15,"Yes");
		RenderButtonImage(msx,msy,320+50-15,240+40,30,15,"No");
	}
	if(mode==WD_NAME)
	{
		RenderTextDialog(msx,msy,mgl);
	}
}

void MapDialogYes(void)
{
	int i;

	switch(asking)
	{
		case 1:
			// delete the map
			if(mapNum!=0)	// can't delete the original one
			{
				InitSwapTable(world->numMaps);
				DeleteFromSwapTable(mapNum);
				RepairLevels();
				ExitSwapTable();
				delete world->map[mapNum];
				for(i=mapNum+1;i<=world->numMaps-1;i++)
				{
					world->map[i-1]=world->map[i];
				}
				world->map[world->numMaps-1]=NULL;
				world->numMaps--;
				mapNum--;
				mapPos=(mapNum/MAX_MAPSHOW)*MAX_MAPSHOW;
				EditorSelectMap(mapNum);
			}
			asking=0;

			for(i=0;i<MAX_MAPSHOW;i++)
				mapnames[i][0]='\0';
			i=0;

			while(i<MAX_MAPSHOW)
			{
				sprintf(mapnames[i],"%02d: %s",i+mapPos,world->map[i+mapPos]->name);
				i++;
				if(i+mapPos>=world->numMaps)
					break;
			}
			break;
	}
}

byte MapDialogKey(char key)
{
	int len;

	if(mode==WD_NAME)
	{
		TextDialogKey(key);
		if(TextDialogCommand()==TM_OKAY)
		{
			strcpy(world->author,GetText());
			mode=WD_NORMAL;
		}
		else if(TextDialogCommand()==TM_CANCEL)
		{
			mode=WD_NORMAL;
		}
		return 1;
	}
	if(asking)
	{
		if(key==27 || key=='n' || key=='N')
		{
			yesNo=0;
			asking=0;
		}
		if(key=='y' || key=='Y')
		{
			MapDialogYes();
		}
		return 1;
	}
	else
	{
		if(key==27)	// esc
			return 0;

		if(key==8)	// backspace
		{
			if(strlen(newmapname)>0)
				newmapname[strlen(newmapname)-1]='\0';
			return 1;
		}

		if(key==13)	// enter
		{
			return 0;	// exit, why not?
		}

		if(key=='|')
			key='/';	// don't allow vertical pipes, needed for the level list

		if(strlen(newmapname)<30)
		{
			len=strlen(newmapname);
			newmapname[len]=key;
			newmapname[len+1]='\0';
		}
	}
	return 1;
}

void MapDialogMoreMaps(void)
{
	int i;

	mapPos+=MAX_MAPSHOW;

	if(mapPos>=world->numMaps)
		mapPos=0;

	for(i=0;i<MAX_MAPSHOW;i++)
		mapnames[i][0]='\0';
	i=0;

	while(i<MAX_MAPSHOW)
	{
		sprintf(mapnames[i],"%02d: %s",i+mapPos,world->map[i+mapPos]->name);
		i++;
		if(i+mapPos>=world->numMaps)
			break;
	}
}

byte MapDialogClick(int msx,int msy)
{
	int i;

	if(mode==WD_NAME)
	{
		TextDialogClick(msx,msy);
		if(TextDialogCommand()==TM_OKAY)
		{
			strcpy(world->author,GetText());
			mode=WD_NORMAL;
		}
		else if(TextDialogCommand()==TM_CANCEL)
		{
			mode=WD_NORMAL;
		}
		return 1;
	}

	if(asking)
	{
		if(PointInRect(msx,msy,320-50-15,240+40,320-50+15,240+40+15))
		{
			MapDialogYes();
		}
		else if(PointInRect(msx,msy,320+50-15,240+40,320+50+15,240+40+15))
		{
			asking=0;
			yesNo=0;
		}
	}
	else
	{
		// if click on a map name, select that map and get its name
		for(i=0;i<MAX_MAPSHOW;i++)
			if(msx>74 && msx<392 && msy>85+i*14 && msy<85+(i+1)*14 && i+mapPos<world->numMaps)
			{
				mapNum=i+mapPos;
				strcpy(newmapname,world->map[mapNum]->name);
				EditorSelectMap(mapNum);
				return 1;
			}

		if(msx>72 && msx<187 && msy>358 && msy<372)	// More Files
		{
			MapDialogMoreMaps();
			return 1;
		}

		if(msx>400 && msy>92 && msx<400+56 && msy<92+15)
		{
			// new level
			if(world->numMaps<MAX_MAPS)
			{
				world->numMaps++;
				world->map[world->numMaps-1]=new Map(0,newmapname);
				mapNum=world->numMaps-1;
				EditorSelectMap(mapNum);
				mapPos=(mapNum/MAX_MAPSHOW)*MAX_MAPSHOW;
			}
		}
		if(msx>400 && msy>92+17*1 && msx<400+56 && msy<92+17*1+15)
		{
			// copy level
			if(world->numMaps<MAX_MAPS)
			{
				world->numMaps++;
				world->map[world->numMaps-1]=new Map(world->map[mapNum]);
				mapNum=world->numMaps-1;
				EditorSelectMap(mapNum);
				mapPos=(mapNum/MAX_MAPSHOW)*MAX_MAPSHOW;
			}
		}
		if(msx>400 && msy>92+17*2 && msx<400+56 && msy<92+17*2+15)
		{
			// rename level
			strncpy(world->map[mapNum]->name,newmapname,32);
		}
		if(msx>400 && msy>92+17*3 && msx<400+56 && msy<92+17*3+15)
		{
			// delete level
			asking=1;
			sprintf(question,"Really delete '%s'?",world->map[mapNum]->name);
			return 1;
		}
		if(msx>400 && msy>92+17*4 && msx<400+56 && msy<92+17*4+15)
		{
			// move up
			if(world->numMaps>1)
			{
				Map *m;

				InitSwapTable(world->numMaps);
				if(mapNum==0)
				{
					m=world->map[world->numMaps-1];
					world->map[world->numMaps-1]=world->map[mapNum];
					world->map[mapNum]=m;
					SwapInSwapTable(mapNum,world->numMaps-1);
					mapNum=world->numMaps-1;
				}
				else
				{
					m=world->map[mapNum-1];
					world->map[mapNum-1]=world->map[mapNum];
					world->map[mapNum]=m;
					SwapInSwapTable(mapNum,mapNum-1);
					mapNum--;
				}
				mapPos=(mapNum/MAX_MAPSHOW)*MAX_MAPSHOW;
				EditorSelectMap(mapNum);
				RepairLevels();
				ExitSwapTable();
			}
		}
		if(msx>400 && msy>92+17*5 && msx<400+56 && msy<92+17*5+15)
		{
			// move down
			if(world->numMaps>1)
			{
				Map *m;

				InitSwapTable(world->numMaps);
				if(mapNum==world->numMaps-1)
				{
					m=world->map[0];
					world->map[0]=world->map[mapNum];
					world->map[mapNum]=m;
					SwapInSwapTable(mapNum,0);
					mapNum=0;
				}
				else
				{
					m=world->map[mapNum+1];
					world->map[mapNum+1]=world->map[mapNum];
					world->map[mapNum]=m;
					SwapInSwapTable(mapNum,mapNum+1);
					mapNum++;
				}
				mapPos=(mapNum/MAX_MAPSHOW)*MAX_MAPSHOW;
				EditorSelectMap(mapNum);
				RepairLevels();
				ExitSwapTable();
			}
		}

		if(msx>72 && msy>380 && msx<172 && msy<380+15)
		{
			mode=WD_NAME;
			InitTextDialog("Enter World Author:","",31);
		}

		if(msx>400 && msy>380 && msx<400+56 && msy<380+15)
		{
			// quit
			return 0;
		}

		if(mapPos>=world->numMaps)
			mapPos=0;

		for(i=0;i<MAX_MAPSHOW;i++)
			mapnames[i][0]='\0';
		i=0;

		while(i<MAX_MAPSHOW)
		{
			sprintf(mapnames[i],"%02d: %s",i+mapPos,world->map[i+mapPos]->name);
			i++;
			if(i+mapPos>=world->numMaps)
				break;
		}

	}

	return 1;
}
#include "editmenu.h"
#include "goal.h"
#include "floor.h"
#include "profile.h"
#include "highscore.h"
#include "appdata.h"

EditMenu::EditMenu(Map *map,YesNoMenu *yesNoMenu,ToolMenu *toolMenu,MGLDraw *mymgl)
{
	x=300;
	y=100;
	mgl=mymgl;
	minmax=1;
	winminmax=1;
	menu=MENU_FILE;

	msClock=5;

	yesno=yesNoMenu;
	toolmenu=toolMenu;

	numFiles=0;
	maxFiles=32;
	curFile=-1;
	filePos=0;
	curLvl=0;
	lvlPos=0;
	curFlash=0;

	curFname[0]='\0';

	filename=NULL;

	extraMode=0;
	scoreMode=0;
	showGuys=1;
	showItems=1;
	delMode=0;

	SetScoreTxt();
	ScanFiles();
}

EditMenu::~EditMenu(void)
{
	if(filename)
		free(filename);
}

byte EditMenu::CanEditFile(const char *fname)
{
// in debug mode you can load all levels
#ifndef NDEBUG

	int i;
	char s[32];

	// training levels
	for(i=0;i<10;i++)
	{
		sprintf(s,"pyr%d.sbl",i+1);
		if(!strcmp(fname,s))
			return 0;
	}
	// stockroom levels
	for(i=0;i<13;i++)
	{
		sprintf(s,"sr%02d.sbl",i);
		if(!strcmp(fname,s))
			return 0;
		sprintf(s,"pu%02d.sbl",i);
		if(!strcmp(fname,s))
			return 0;
	}
	if(!strcmp(fname,"pestcontrol.sbl"))
		return 0;
	if(!strcmp(fname,"clearance.sbl"))
		return 0;
	if(!strcmp(fname,"blowout.sbl"))
		return 0;
#endif
	return 1;
}

void EditMenu::ScanFiles(void)
{
	int i;
	char tmp[FNAMELEN];
	byte flip;

	numFiles=0;
	maxFiles=32;

	if(filename)
	{
		if(curFile!=-1)
			strcpy(tmp,&filename[FNAMELEN*curFile]);
		free(filename);
	}
	else
		tmp[0]='\0';

	filename=(char *)calloc(FNAMELEN*maxFiles,1);

	for (const auto& file : ListDirectory("levels", ".sbl", FNAMELEN))
	{
		// completely ignore ones with filenames that are too long!
		// and ones that are not allowed to be edited
		if(CanEditFile(file.c_str()))
		{
			strncpy(&filename[numFiles*FNAMELEN],file.c_str(),FNAMELEN);
			numFiles++;
			filename[FNAMELEN*numFiles-1]='\0';
			if(numFiles==maxFiles)
			{
				maxFiles+=32;
				filename=(char *)realloc(filename,FNAMELEN*maxFiles);
				// clear the future space
				memset(&filename[numFiles*FNAMELEN],0,FNAMELEN*maxFiles-FNAMELEN*numFiles);
			}
		}
	}

	if(curFile>=numFiles)
		curFile=-1;
	if(filePos>=numFiles)
		filePos=0;

	if(tmp[0]!='\0')
	{
		for(i=0;i<numFiles-1;i++)
			if(!strcmp(tmp,&filename[i*FNAMELEN]))
				curFile=i;
	}

	// sort them
	flip=1;
	while(flip)
	{
		flip=0;
		for(i=0;i<numFiles-1;i++)
		{
			if(strcmp(&filename[i*FNAMELEN],&filename[(i+1)*FNAMELEN])>0)
			{
				flip=1;
				strcpy(tmp,&filename[i*FNAMELEN]);
				strcpy(&filename[i*FNAMELEN],&filename[(i+1)*FNAMELEN]);
				strcpy(&filename[(i+1)*FNAMELEN],tmp);
				if(i==curFile)
					curFile++;
				else if((i+1)==curFile)
					curFile--;
			}
		}
	}
}

void EditMenu::Render(int msx,int msy)
{
	char btnName[8][16]={"File","Set Opt","Lvl Opt","Floor","Items","Critters","Extra","Scores"};
	int i;

	curFlash++;

	msx-=x;
	msy-=y;

	if(minmax==0)	// minimized
	{
		editspr->GetSprite(1)->Draw(x,y,mgl);
		// min/max
		if(PointInRect(msx,msy,0,0,18,10))
		{
			editspr->GetSprite(12)->Draw(x,y,mgl);
		}
		// mover
		if(PointInRect(msx,msy,23,-1,39,15))
		{
			editspr->GetSprite(11)->Draw(x,y,mgl);
		}
	}
	else
	{
		// draw the menu
		editspr->GetSprite(3)->Draw(x,y,mgl);

		// all the buttons
		for(i=7;i>=0;i--)
		{
			editspr->GetSprite(4+(i==7))->Draw(x,y+15+23*i,mgl);
			if(PointInRect(msx,msy,0,15+23*i,92,15+23*i+22) || menu==i)
			{
				editspr->GetSprite(6)->Draw(x,y+15+23*i,mgl);
				PrintGlow(x+4,y+15+23*i+3,btnName[i],6,2);
			}
			else
				PrintGlow(x+4,y+15+23*i+3,btnName[i],-4,2);
		}

		if(winminmax==0)
		{
			editspr->GetSprite(8)->Draw(x,y,mgl);
			// win-min/max
			if(PointInRect(msx,msy,92,0,97,11))
			{
				editspr->GetSprite(17)->Draw(x,y,mgl);
			}
		}
		else
		{
			editspr->GetSprite(7)->Draw(x,y,mgl);
			// win-min/max
			if(PointInRect(msx,msy,100,0,105,11))
			{
				editspr->GetSprite(18)->Draw(x,y,mgl);
			}
			switch(menu)
			{
				case MENU_FILE:
					RenderFileWindow(msx,msy);
					break;
				case MENU_SETOPT:
					RenderSetWindow(msx,msy);
					break;
				case MENU_LVLOPT:
					RenderLevelWindow(msx,msy);
					break;
				case MENU_FLOOR:
					RenderFloorWindow(msx,msy);
					break;
				case MENU_ITEMS:
					RenderItemWindow(msx,msy);
					break;
				case MENU_CRITTERS:
					RenderCritterWindow(msx,msy);
					break;
				case MENU_EXTRA:
					RenderExtraWindow(msx,msy);
					break;
				case MENU_SCORE:
					RenderScoreWindow(msx,msy);
					break;
			}
		}

		// min/max
		if(PointInRect(msx,msy,0,0,18,10))
		{
			editspr->GetSprite(10)->Draw(x,y,mgl);
		}
		// mover
		if(PointInRect(msx,msy,23,-1,39,15))
		{
			editspr->GetSprite(11)->Draw(x,y,mgl);
		}
	}
}

// returns whether or not the click occurred in this menu, and hence needs no more processing
TASK(byte) EditMenu::Click(int msx,int msy,byte btn)
{
	int i;

	msClock=5;
	msRep=0;

	msx-=x;
	msy-=y;

	// min/max
	if(PointInRect(msx,msy,0,0,18,10))
	{
		minmax=1-minmax;
		CO_RETURN 1;
	}
	// mover
	if(PointInRect(msx,msy,23,-1,39,15))
	{
		// set editmode to 'moving edit menu' until button is released
		editMode=EDITMODE_DRAGMENU;
		relmsx=-msx;
		relmsy=-msy;
		CO_RETURN 1;
	}

	if(minmax==1)	// these buttons require it to be maxed
	{
		// the main buttons
		for(i=0;i<8;i++)
			if(PointInRect(msx,msy,0,15+23*i,92,15+23*i+22))
			{
				menu=i;
				switch(menu)
				{
					case MENU_FLOOR:
						toolmenu->SwitchTool(TOOL_FLOOR);
						break;
					case MENU_ITEMS:
						toolmenu->SwitchTool(TOOL_ITEM);
						break;
					case MENU_CRITTERS:
						toolmenu->SwitchTool(TOOL_CRITTER);
						break;
				}
				CO_RETURN 1;
			}

		if(!winminmax)	// window is not maxed
		{
			// win-min/max
			if(PointInRect(msx,msy,92,0,97,11))
			{
				winminmax=1-winminmax;
			}
		}
		else	// if the window is maxed
		{
			// win-min/max
			if(PointInRect(msx,msy,100,0,105,11))
			{
				winminmax=1-winminmax;
			}

			switch(menu)
			{
				case MENU_FILE:
					AWAIT FileClick(msx,msy,btn);
					break;
				case MENU_SETOPT:
					SetOptClick(msx,msy,btn);
					break;
				case MENU_LVLOPT:
					LevelOptClick(msx,msy,btn);
					break;
				case MENU_FLOOR:
					FloorClick(msx,msy,btn);
					break;
				case MENU_EXTRA:
					ExtraClick(msx,msy,btn);
					break;
				case MENU_SCORE:
					ScoreClick(msx,msy,btn);
					break;
			}
			// anywhere in the window space means the click is absorbed
			if(PointInRect(msx,msy,88,13,289,143))
				CO_RETURN 1;
		}
	}

	CO_RETURN 0;
}

void EditMenu::MouseHold(int msx,int msy,byte btn)
{
	int winx,winy;
	int dx;

	msx-=x;
	msy-=y;

	winx=x+88;
	winy=y+13;

	if(msClock)
		msClock--;
	else
	{
		if(msRep<10)
			msClock=5;
		else if(msRep<20)
			msClock=2;
		else
			msClock=0;

		if(winminmax)
		{
			if(menu==MENU_LVLOPT)
			{
				// goal number
				dx=GetStrLength(EditGoalName(EditorGetMap()->goal),1);
				if(PointInRect(msx,msy,winx-x+58+dx,winy-y+32,winx-x+dx+86,winy-y+46))
				{
					if(btn==2)
						EditorGetMap()->goal.number--;
					else
						EditorGetMap()->goal.number++;
					if(msRep<30)
						msRep++;
				}
				// antigoal number
				dx=GetStrLength(EditGoalName(EditorGetMap()->antigoal),1);
				if(PointInRect(msx,msy,winx-x+58+dx,winy-y+62,winx-x+dx+86,winy-y+76))
				{
					if(btn==2)
						EditorGetMap()->antigoal.number--;
					else
						EditorGetMap()->antigoal.number++;
					if(msRep<30)
						msRep++;
				}
				// timer
				if(PointInRect(msx,msy,winx-x+50,winy-y+92,winx-x+78,winy-y+106))
				{
					if(btn==2)
					{
						EditorGetMap()->timer--;
						if(EditorGetMap()->timer>999)
							EditorGetMap()->timer=999;
					}
					else
					{
						EditorGetMap()->timer++;
						if(EditorGetMap()->timer>999)
							EditorGetMap()->timer=0;
					}
					if(msRep<30)
						msRep++;
				}
				// genTime
				if(PointInRect(msx,msy,winx-x+156,winy-y+92,winx-x+190,winy-y+106))
				{
					if(btn==2)
					{
						if(msRep<30)
							EditorGetMap()->genTime-=3;
						else
							EditorGetMap()->genTime-=30;
						if(EditorGetMap()->genTime>30*99)
							EditorGetMap()->genTime=30*99;
					}
					else
					{
						if(msRep<30)
							EditorGetMap()->genTime+=3;
						else
							EditorGetMap()->genTime+=30;
						if(EditorGetMap()->genTime>30*99)
							EditorGetMap()->genTime=0;
					}
					if(msRep<30)
						msRep++;
				}
			}
		}
	}
}

void EditMenu::LevelOptClick(int msx,int msy,byte btn)
{
	int winx,winy;
	int dx;

	winx=x+88;
	winy=y+13;

	// goal type
	dx=GetStrLength(EditGoalName(EditorGetMap()->goal),1);
	if(PointInRect(msx,msy,winx-x+50,winy-y+32,winx-x+50+4+dx,winy-y+46))
	{
		ChangeGoalType(&EditorGetMap()->goal,0);
	}

	// goal number
	if(GoalHasNumber(EditorGetMap()->goal))
	{
		if(PointInRect(msx,msy,winx-x+58+dx,winy-y+32,winx-x+dx+86,winy-y+46))
		{
			if(btn==2)
				EditorGetMap()->goal.number--;
			else
				EditorGetMap()->goal.number++;
		}
		dx+=86+4;
	}
	// goal modifier
	if(GoalModType(EditorGetMap()->goal)!=GMT_NONE)
	{
		if(PointInRect(msx,msy,winx-x+dx,winy-y+32,winx-x+dx+4+GetStrLength(GoalColorName(EditorGetMap()->goal.modifier),1),winy-y+46))
		{
			ChangeGoalMod(&EditorGetMap()->goal);
		}
	}

	// anti-goal type
	dx=GetStrLength(EditGoalName(EditorGetMap()->antigoal),1);
	if(PointInRect(msx,msy,winx-x+50,winy-y+62,winx-x+50+4+dx,winy-y+76))
	{
		ChangeGoalType(&EditorGetMap()->antigoal,1);
	}

	// antigoal number
	if(GoalHasNumber(EditorGetMap()->antigoal))
	{
		if(PointInRect(msx,msy,winx-x+58+dx,winy-y+62,winx-x+dx+86,winy-y+76))
		{
			if(btn==2)
				EditorGetMap()->antigoal.number--;
			else
				EditorGetMap()->antigoal.number++;
		}
		dx+=86+4;
	}
	// antigoal modifier
	if(GoalModType(EditorGetMap()->antigoal)!=GMT_NONE)
	{
		if(PointInRect(msx,msy,winx-x+dx,winy-y+62,winx-x+dx+4+GetStrLength(GoalColorName(EditorGetMap()->goal.modifier),1),winy-y+76))
		{
			ChangeGoalMod(&EditorGetMap()->antigoal);
		}
	}

	// timer
	if(PointInRect(msx,msy,winx-x+50,winy-y+92,winx-x+78,winy-y+106))
	{
		if(btn==2)
		{
			EditorGetMap()->timer--;
			if(EditorGetMap()->timer>999)
				EditorGetMap()->timer=999;
		}
		else
		{
			EditorGetMap()->timer++;
			if(EditorGetMap()->timer>999)
				EditorGetMap()->timer=0;
		}
	}

	// genTime
	if(PointInRect(msx,msy,winx-x+156,winy-y+92,winx-x+190,winy-y+106))
	{
		if(btn==2)
		{
			EditorGetMap()->genTime-=3;
			if(EditorGetMap()->genTime>30*99)
				EditorGetMap()->genTime=30*99;
		}
		else
		{
			EditorGetMap()->genTime+=3;
			if(EditorGetMap()->genTime>30*99)
				EditorGetMap()->genTime=0;
		}
	}
}

TASK(void) EditMenu::FileClick(int msx,int msy,byte btn)
{
	int i;
	int winx,winy;
	char s[64];

	if(btn==2)
		CO_RETURN;

	winx=x+88;
	winy=y+13;

	for(i=0;i<5;i++)
	{
		if(PointInRect(msx,msy,winx-x+150,winy-y+8+22*i,winx-x+200,winy-y+29+22*i))
		{
			switch(i)
			{
				case 0:	// new
					sprintf(s,"Clear Everything?");
					yesno->Activate(s,"","Yay","Nay");
					editMode=EDITMODE_YESNO;
					askingFor=4;
					break;
				case 1:	// load
					if(curFname[0]=='\0')
					{
						MakeNormalSound(SND_MENUCANCEL);
						break;
					}
					if(SDL_GetModState() & KMOD_SHIFT)	// shift-click to merge levels
					{
						sprintf(s,"%s?",curFname);
						yesno->Activate("Merge in",s,"Yup","Nope");
						editMode=EDITMODE_YESNO;
						askingFor=8;
					}
					else
					{
						sprintf(s,"%s?",curFname);
						yesno->Activate("Load file",s,"Yup","Nope");
						editMode=EDITMODE_YESNO;
						askingFor=1;
					}
					break;
				case 2:	// save
					if(curFname[0]=='\0')
					{
						MakeNormalSound(SND_MENUCANCEL);
						break;
					}
					sprintf(s,"%s?",curFname);
					yesno->Activate("Save",s,"Yes","No");
					editMode=EDITMODE_YESNO;
					askingFor=2;
					break;
				case 3:	// test
					ExitGuys();
					PurgeMonsterSprites();
					AWAIT TestLevel(EditorGetMapNum());

					InitGuys(MAX_MAPMONS);
					AddMapGuys(EditorGetMap());
					player.invinc=0;
					break;
				case 4:	// exit
					sprintf(s,"Exit editor?");
					yesno->Activate(s,"","Yes","No");
					editMode=EDITMODE_YESNO;
					askingFor=3;
					break;
			}
		}
	}

	if(PointInRect(msx,msy,winx-x+7,winy-y+8,winx-x+134,winy-y+110))
	{
		i=(msy-(winy-y+8))/13;
		if(i+filePos<numFiles)
		{
			curFile=i+filePos;
			strcpy(curFname,&filename[curFile*FNAMELEN]);
		}
	}

	if(PointInRect(msx,msy,winx-x+137,winy-y+8,winx-x+145,winy-y+59))
	{
		// top half of scroll bar
		if(filePos>0)
		{
			filePos-=8;
		}
	}
	else if(PointInRect(msx,msy,winx-x+137,winy-y+60,winx-x+145,winy-y+110))
	{
		// bottom half of scroll bar
		if(filePos<numFiles-8)
			filePos+=8;
	}
}

void EditMenu::SetOptClick(int msx,int msy,byte btn)
{
	int i;
	int winx,winy;
	char s[64];

	if(btn==2)
		return;

	winx=x+88;
	winy=y+13;

	for(i=0;i<5;i++)
	{
		if(PointInRect(msx,msy,winx-x+150,winy-y+8+22*i,winx-x+200,winy-y+29+22*i))
		{
			switch(i)
			{
				case 0:	// new level
					WorldNewLevel(EditorGetWorld(),NULL);
					break;
				case 1:	// Delete level
					sprintf(s,"\"%s\"?",EditorGetMap()->name);
					yesno->Activate("Delete level",s,"Yup","Nope");
					editMode=EDITMODE_YESNO;
					askingFor=5;
					break;
				case 2:	// copy level
					sprintf(s,"\"%s\"?",EditorGetMap()->name);
					yesno->Activate("Copy level",s,"Yes","No");
					editMode=EDITMODE_YESNO;
					askingFor=6;
					break;
				case 3:	// move up
					MoveLevel(EditorGetWorld(),EditorGetMapNum(),-1);
					curLvl=EditorGetMapNum();
					break;
				case 4:	// move down
					MoveLevel(EditorGetWorld(),EditorGetMapNum(),1);
					curLvl=EditorGetMapNum();
					break;
			}
		}
	}

	if(PointInRect(msx,msy,winx-x+7,winy-y+21,winx-x+134,winy-y+123))
	{
		i=(msy-(winy-y+21))/13;
		if(i+lvlPos<EditorGetWorld()->numMaps)
		{
			EditorSelectMap(i+lvlPos);
			curLvl=i+lvlPos;
			SetScoreTxt();
		}
	}

	if(PointInRect(msx,msy,winx-x+137,winy-y+8,winx-x+145,winy-y+59))
	{
		// top half of scroll bar
		if(lvlPos>0)
		{
			lvlPos-=8;
		}
	}
	else if(PointInRect(msx,msy,winx-x+137,winy-y+60,winx-x+145,winy-y+110))
	{
		// bottom half of scroll bar
		if(lvlPos<EditorGetWorld()->numMaps-8)
			lvlPos+=8;
	}
}

void EditMenu::FloorClick(int msx,int msy,byte btn)
{
	int winx,winy;

	if(btn==2)
		return;

	winx=x+88;
	winy=y+13;

	if(PointInRect(msx,msy,winx-x+140,winy-y+105,winx-x+190,winy-y+125))
	{
		yesno->Activate("Fill map","with tile?","Yes","No");
		editMode=EDITMODE_YESNO;
		askingFor=7;
	}
}

void EditMenu::ExtraClick(int msx,int msy,byte btn)
{
	int i;
	int winx,winy;

	if(btn==2)
		return;

	winx=x+88;
	winy=y+13;

	for(i=0;i<2;i++)
	{
		if(PointInRect(msx,msy,winx-x+140,winy-y+8+22*i,winx-x+200,winy-y+29+22*i))
		{
			switch(i)
			{
				case 0:	// author
					extraMode=0;
					break;
				case 1:	// tileset
					extraMode=1;
					if(!GetWorldTiles(EditorGetWorld(),mgl))
						MakeNormalSound(SND_MENUCANCEL);
					break;
			}
		}
	}

	if(PointInRect(msx,msy,winx-x+6,winy-y+50,winx-x+16,winy-y+60))
	{
		showItems=1-showItems;
	}
	if(PointInRect(msx,msy,winx-x+6,winy-y+65,winx-x+16,winy-y+75))
	{
		showGuys=1-showGuys;
	}
	if(PointInRect(msx,msy,winx-x+6,winy-y+80,winx-x+16,winy-y+90))
	{
		delMode=1-delMode;
	}
	if(PointInRect(msx,msy,winx-x+6,winy-y+105,winx-x+180,winy-y+115))
	{
		yesno->Activate("!","","","");
		editMode=EDITMODE_YESNO;
		askingFor=9;
	}
}

void EditMenu::ScoreClick(int msx,int msy,byte btn)
{
	int i;
	int winx,winy;

	if(btn==2)
		return;

	winx=x+88;
	winy=y+13;

	for(i=0;i<3;i++)
	{
		if(PointInRect(msx,msy,winx-x+140,winy-y+30+22*i,winx-x+200,winy-y+51+22*i))
		{
			scoreMode=i;
		}
	}
}

void EditMenu::DraggedByMouse(int msx,int msy)
{
	x=msx+relmsx;
	y=msy+relmsy;
}

void EditMenu::RenderFileWindow(int msx,int msy)
{
	char btnName[5][8]={"New","Load","Save","Test","Exit"};
	int winx,winy;
	int i;

	winx=x+88;
	winy=y+13;

	if(SDL_GetModState() & KMOD_SHIFT)
	{
		strcpy(btnName[1],"Merge");
	}
	else
		strcpy(btnName[1],"Load");

	for(i=0;i<5;i++)
	{
		if(PointInRect(msx,msy,winx-x+150,winy-y+8+22*i,winx-x+200,winy-y+29+22*i))
			DrawFillBox(winx+150,winy+8+22*i,winx+200-2,winy+29+22*i,236);
		RightPrintGlow(winx+200-4,winy+9+22*i,btnName[i],0,2);
	}

	// render file list
	DrawBox(winx+6,winy+7,winx+135,winy+111,255-32+8);
	DrawBox(winx+6,winy+111,winx+135,winy+126,255-32+8);
	DrawFillBox(winx+7,winy+8,winx+134,winy+110,255-32+4);
	DrawFillBox(winx+7,winy+112,winx+134,winy+125,255-32+4);
	for(i=0;i<8;i++)
	{
		if(curFile==filePos+i)
			DrawFillBox(winx+7,winy+8+i*13,winx+134,winy+8+i*13+12,255-32+8);
		if(filePos+i<numFiles)
			PrintLimited(winx+8,winy+8+i*13,winx+135,&filename[(filePos+i)*FNAMELEN],0,1);
	}
	// scroll bar
	DrawFillBox(winx+136,winy+7,winx+146,winy+111,255-32+8);
	if(numFiles<9)	// all files fit on one page
		DrawFillBox(winx+137,winy+8,winx+145,winy+110,255-32+16);
	else	// just showing a subset of files
	{
		if(filePos==0)	// if you're at the top, have the top half of the bar highlighted
			DrawFillBox(winx+137,winy+8,winx+145,winy+59,255-32+16);
		else if(filePos+7>=numFiles)	// at the bottom, highlight the bottom half
			DrawFillBox(winx+137,winy+59,winx+145,winy+110,255-32+16);
		else	// you're somewhere between, so fill the middle of the bar
			DrawFillBox(winx+137,winy+33,winx+145,winy+86,255-32+16);
	}
	if(curFlash&3)
	{
		curFname[strlen(curFname)+1]='\0';
		curFname[strlen(curFname)]='_';
	}
	PrintLimited(winx+8,winy+8+8*13,winx+135,curFname,0,1);
	if(curFlash&3)
	{
		curFname[strlen(curFname)-1]='\0';
	}

	// show the text in case it's too long
	//if(GetStrLength(curFname,1)+10>134-7)
	PrintOutline(2,450,curFname,0,0);
}

void EditMenu::RenderSetWindow(int msx,int msy)
{
	char btnName[5][8]={"New","Del","Copy","Mv Up","Mv Dn"};
	int winx,winy;
	int i;

	winx=x+88;
	winy=y+13;

	for(i=0;i<5;i++)
	{
		if(PointInRect(msx,msy,winx-x+150,winy-y+8+22*i,winx-x+200,winy-y+29+22*i))
			DrawFillBox(winx+150,winy+8+22*i,winx+200-2,winy+29+22*i,236);
		RightPrintGlow(winx+200-4,winy+9+22*i,btnName[i],0,2);
	}

	// render level list
	DrawBox(winx+6,winy+7,winx+135,winy+20,255-32+8);
	DrawBox(winx+6,winy+20,winx+135,winy+126,255-32+8);

	DrawFillBox(winx+7,winy+8,winx+134,winy+19,255-32+4);
	DrawFillBox(winx+7,winy+21,winx+134,winy+125,255-32+4);
	for(i=0;i<8;i++)
	{
		if(curLvl==lvlPos+i)
			DrawFillBox(winx+7,winy+21+i*13,winx+134,winy+21+i*13+12,255-32+8);
		if(lvlPos+i<EditorGetWorld()->numMaps)
			PrintLimited(winx+8,winy+21+i*13,winx+135,
				EditorGetWorld()->map[lvlPos+i]->name,0,1);
	}

	// scroll bar
	DrawFillBox(winx+136,winy+7,winx+146,winy+111,255-32+8);
	if(EditorGetWorld()->numMaps<9)	// all maps fit on one page
		DrawFillBox(winx+137,winy+8,winx+145,winy+110,255-32+16);
	else	// just showing a subset of maps
	{
		if(lvlPos==0)	// if you're at the top, have the top half of the bar highlighted
			DrawFillBox(winx+137,winy+8,winx+145,winy+59,255-32+16);
		else if(lvlPos+7>=EditorGetWorld()->numMaps)	// at the bottom, highlight the bottom half
			DrawFillBox(winx+137,winy+59,winx+145,winy+110,255-32+16);
		else	// you're somewhere between, so fill the middle of the bar
			DrawFillBox(winx+137,winy+33,winx+145,winy+86,255-32+16);
	}

	// set name
	if(curFlash&3)
	{
		EditorGetWorld()->setname[strlen(EditorGetWorld()->setname)+1]='\0';
		EditorGetWorld()->setname[strlen(EditorGetWorld()->setname)]='_';
	}
	PrintLimited(winx+8,winy+8,winx+135,EditorGetWorld()->setname,0,1);
	if(curFlash&3)
	{
		EditorGetWorld()->setname[strlen(EditorGetWorld()->setname)-1]='\0';
	}

	// show the text in case it's too long
	//if(GetStrLength(EditorGetWorld()->setname,1)+10>134-7)
	PrintOutline(2,450,EditorGetWorld()->setname,0,0);
}

void EditMenu::RenderLevelWindow(int msx,int msy)
{
	int winx,winy;
	int dx;
	char s[8];

	winx=x+88;
	winy=y+13;

	// level name
	PrintGlow(winx+4,winy+4,"Name",0,2);

	DrawBox(winx+50,winy+6,winx+165,winy+21,255-32+8);
	DrawFillBox(winx+51,winy+7,winx+164,winy+20,255-32+4);

	if(curFlash&3)
	{
		EditorGetMap()->name[strlen(EditorGetMap()->name)+1]='\0';
		EditorGetMap()->name[strlen(EditorGetMap()->name)]='_';
	}
	PrintLimited(winx+52,winy+8,winx+165,EditorGetMap()->name,0,1);
	if(curFlash&3)
	{
		EditorGetMap()->name[strlen(EditorGetMap()->name)-1]='\0';
	}

	// goal
	PrintGlow(winx+4,winy+28,"Goal",0,2);
	dx=GetStrLength(EditGoalName(EditorGetMap()->goal),1);
	DrawBox(winx+50,winy+32,winx+50+4+dx,winy+46,31);
	Print(winx+52,winy+34,EditGoalName(EditorGetMap()->goal),0,1);

	// goal number
	if(GoalHasNumber(EditorGetMap()->goal))
	{
		DrawBox(winx+58+dx,winy+32,winx+dx+86,winy+46,31);
		sprintf(s,"%03d",EditorGetMap()->goal.number);
		Print(winx+dx+60,winy+34,s,0,1);
		dx+=86+4;
	}
	// goal modifier
	if(GoalModType(EditorGetMap()->goal)!=GMT_NONE)
	{
		if(EditorGetMap()->goal.modifier>=GM_COLOR0 && EditorGetMap()->goal.modifier<=GM_COLOR7)
			DrawFillBox(winx+dx,winy+32,winx+dx+4+GetStrLength(GoalColorName(EditorGetMap()->goal.modifier),1),winy+46,(EditorGetMap()->goal.modifier-GM_COLOR0)*32+12);
		DrawBox(winx+dx,winy+32,winx+dx+4+GetStrLength(GoalColorName(EditorGetMap()->goal.modifier),1),winy+46,31);
		Print(winx+dx+2,winy+34,GoalColorName(EditorGetMap()->goal.modifier),0,1);
	}
	RightPrint(winx+163,winy+48,EditGoalEnding(EditorGetMap()->goal),0,1);

	// anti-goal
	PrintGlow(winx+4,winy+58,"Fail",0,2);
	dx=GetStrLength(EditGoalName(EditorGetMap()->antigoal),1);
	DrawBox(winx+50,winy+62,winx+50+4+dx,winy+76,31);
	Print(winx+52,winy+64,EditGoalName(EditorGetMap()->antigoal),0,1);

	// antigoal number
	if(GoalHasNumber(EditorGetMap()->antigoal))
	{
		DrawBox(winx+58+dx,winy+62,winx+dx+86,winy+76,31);
		sprintf(s,"%03d",EditorGetMap()->antigoal.number);
		Print(winx+dx+60,winy+64,s,0,1);
		dx+=86+4;
	}
	// antigoal modifier
	if(GoalModType(EditorGetMap()->antigoal)!=GMT_NONE)
	{
		if(EditorGetMap()->antigoal.modifier>=GM_COLOR0 && EditorGetMap()->antigoal.modifier<=GM_COLOR7)
			DrawFillBox(winx+dx,winy+62,winx+dx+4+GetStrLength(GoalColorName(EditorGetMap()->antigoal.modifier),1),winy+76,(EditorGetMap()->antigoal.modifier-GM_COLOR0)*32+12);
		DrawBox(winx+dx,winy+62,winx+dx+4+GetStrLength(GoalColorName(EditorGetMap()->antigoal.modifier),1),winy+76,31);
		Print(winx+dx+2,winy+64,GoalColorName(EditorGetMap()->antigoal.modifier),0,1);
	}
	RightPrint(winx+163,winy+78,EditGoalEnding(EditorGetMap()->antigoal),0,1);

	// timer
	PrintGlow(winx+4,winy+88,"Time",0,2);
	DrawBox(winx+50,winy+92,winx+78,winy+106,31);
	sprintf(s,"%03d",EditorGetMap()->timer);
	Print(winx+52,winy+94,s,0,1);

	// GenTime
	PrintGlow(winx+85,winy+88,"GenTime",0,2);
	DrawBox(winx+156,winy+92,winx+190,winy+106,31);
	sprintf(s,"%2.1f",(float)(EditorGetMap()->genTime/30.0f));
	Print(winx+158,winy+94,s,0,1);

	// show the text in case it's too long
	//if(GetStrLength(EditorGetMap()->name,1)+10>164-51)
	PrintOutline(2,450,EditorGetMap()->name,0,0);
}

void EditMenu::RenderFloorWindow(int msx,int msy)
{
	int winx,winy;
	word t;

	winx=x+88;
	winy=y+13;

	if(PointInRect(msx,msy,winx-x+140,winy-y+105,winx-x+190,winy-y+125))
		mgl->FillBox(winx+139,winy+104,winx+195,winy+126,236);
	PrintGlow(winx+140,winy+105,"Fill Map",0,2);

	t=GetTileNumber(toolmenu->curTool,toolmenu->color,toolmenu->facing);
	// show the current floor tile
	RenderFloorTile(winx+6,winy+6,t,0);

	if(t==0 || t==20 ||
	   t==40 || t==60)
	{
		// floor
		PrintGlow(winx+40,winy+8,"Floor",0,2);
		Print(winx+4,winy+32,"Free movement for all.",0,1);
	}
	if(t==1 || t==21 ||
	   t==41 || t==61)
	{
		// wall
		PrintGlow(winx+40,winy+8,"Wall",0,2);
		Print(winx+4,winy+32,"Impassable to all objects.",0,1);
	}
	if(t==2 || t==22 ||
	   t==42 || t==62)
	{
		// arrow
		PrintGlow(winx+40,winy+8,"Arrow Tile",0,2);
		Print(winx+4,winy+32,"Player: No effect.",0,1);
		Print(winx+4,winy+52,"Barrel: Limits pushing.",0,1);
		Print(winx+4,winy+72,"Ball: Rolls that way.",0,1);
		Print(winx+4,winy+92,"Critter: Walks that way.",0,1);
	}
	if(t==3 || t==23 ||
	   t==43 || t==63)
	{
		// spikes
		PrintGlow(winx+40,winy+8,"Spikes",0,2);
		Print(winx+4,winy+32,"Player: Deadly.",0,1);
		Print(winx+4,winy+52,"Objects: No effect.",0,1);
		Print(winx+4,winy+72,"Critter: Deadly.",0,1);
	}
	if(t==4 || t==24 ||
	   t==44 || t==64)
	{
		// color tiles (grey version)
		PrintGlow(winx+40,winy+8,"Grey Color Tile",0,2);
		Print(winx+4,winy+32,"Player: No effect.",0,1);
		Print(winx+4,winy+52,"Impassable to all objects",0,1);
		Print(winx+4,winy+72,"and critters.",0,1);
	}
	if((t>=5 && t<=11) ||
	   (t>=25 && t<=31) ||
	   (t>=45 && t<=51) ||
	   (t>=65 && t<=71))
	{
		// color tiles
		PrintGlow(winx+40,winy+8,"Color Tile",0,2);
		Print(winx+4,winy+32,"Player: No effect.",0,1);
		Print(winx+4,winy+52,"Passable only to objects",0,1);
		Print(winx+4,winy+72,"and critters of the same",0,1);
		Print(winx+4,winy+92,"color as the tile.",0,1);
	}
	if((t>=12 && t<=19) ||
	   (t>=32 && t<=39) ||
	   (t>=52 && t<=59) ||
	   (t>=72 && t<=79))
	{
		// pressure pads
		PrintGlow(winx+40,winy+8,"Pressure Pad",0,2);
		Print(winx+4,winy+32,"Any object sitting on it",0,1);
		Print(winx+4,winy+52,"holds all doors of this",0,1);
		Print(winx+4,winy+72,"color open or shut, until",0,1);
		Print(winx+4,winy+92,"the object moves off.",0,1);
	}
	if(t==80 ||
	   t==120)
	{
		// holes
		PrintGlow(winx+40,winy+8,"Hole",0,2);
		Print(winx+4,winy+32,"Player: Impassable.",0,1);
		Print(winx+4,winy+52,"If a critter or object",0,1);
		Print(winx+4,winy+72,"falls in, this becomes a",0,1);
		Print(winx+4,winy+92,"filled hole.",0,1);
	}
	if(t==100 ||
	   t==140)
	{
		// filled holes
		PrintGlow(winx+40,winy+8,"Filled Hole",0,2);
		Print(winx+4,winy+32,"Same as normal floor.",0,1);
	}
	if(t==81 || t==101 ||
	   t==121 || t==141)
	{
		// pit
		PrintGlow(winx+40,winy+8,"Pit",0,2);
		Print(winx+4,winy+32,"Player: Impassable.",0,1);
		Print(winx+4,winy+52,"If a critter or object",0,1);
		Print(winx+4,winy+72,"falls in, it is gone",0,1);
		Print(winx+4,winy+92,"forever.",0,1);
	}
	if((t>=82 && t<=89) ||
	   (t>=102 && t<=109) ||
	   (t>=122 && t<=129) ||
	   (t>=142 && t<=149))
	{
		// door tiles
		PrintGlow(winx+40,winy+8,"Door Tile",0,2);
		Print(winx+4,winy+32,"Same as normal floor.",0,1);
		Print(winx+4,winy+52,"Just marks where Doors",0,1);
		Print(winx+4,winy+72,"are. Placed automatically",0,1);
		Print(winx+4,winy+92,"when you place a Door.",0,1);
	}
	if((t>=90 && t<=97) ||
	   (t>=110 && t<=117) ||
	   (t>=130 && t<=137) ||
	   (t>=150 && t<=157))
	{
		// teleporters
		PrintGlow(winx+40,winy+8,"Teleporter",0,2);
		Print(winx+4,winy+32,"Any object moved onto",0,1);
		Print(winx+4,winy+52,"this teleports to any",0,1);
		Print(winx+4,winy+72,"other Teleporter of the",0,1);
		Print(winx+4,winy+92,"same color.",0,1);
	}
	if((t>=160 && t<=167) ||
	   (t>=180 && t<=187) ||
	   (t>=200 && t<=207) ||
	   (t>=220 && t<=227))
	{
		// blobby generator
		PrintGlow(winx+40,winy+8,"Blobby Generator",0,2);
		Print(winx+4,winy+32,"Creates Blobbies at the",0,1);
		Print(winx+4,winy+52,"rate specified in Level",0,1);
		Print(winx+4,winy+72,"Options.",0,1);
	}
	if((t>=168 && t<=175) ||
	   (t>=188 && t<=195) ||
	   (t>=208 && t<=215) ||
	   (t>=228 && t<=235))
	{
		// fluffy generator
		PrintGlow(winx+40,winy+8,"Fluffy Generator",0,2);
		Print(winx+4,winy+32,"Creates Fluffies at the",0,1);
		Print(winx+4,winy+52,"rate specified in Level",0,1);
		Print(winx+4,winy+72,"Options.",0,1);
	}
	if((t>=240 && t<=247) ||
	   (t>=260 && t<=267) ||
	   (t>=280 && t<=287) ||
	   (t>=300 && t<=307))
	{
		// inchy generator
		PrintGlow(winx+40,winy+8,"Inchy Generator",0,2);
		Print(winx+4,winy+32,"Creates Inchies at the",0,1);
		Print(winx+4,winy+52,"rate specified in Level",0,1);
		Print(winx+4,winy+72,"Options.",0,1);
	}
	if((t>=248 && t<=255) ||
	   (t>=268 && t<=275) ||
	   (t>=288 && t<=295) ||
	   (t>=308 && t<=315))
	{
		// piggy generator
		PrintGlow(winx+40,winy+8,"Piggy Generator",0,2);
		Print(winx+4,winy+32,"Creates Piggies at the",0,1);
		Print(winx+4,winy+52,"rate specified in Level",0,1);
		Print(winx+4,winy+72,"Options.",0,1);
	}
	if((t>=160 && t<=175) || (t>=240 && t<=255))
		Print(winx+4,winy+92,"They appear facing right.",0,1);
	else if((t>=180 && t<=195) || (t>=260 && t<=275))
		Print(winx+4,winy+92,"They appear facing down.",0,1);
	else if((t>=200 && t<=215) || (t>=280 && t<=295))
		Print(winx+4,winy+92,"They appear facing left.",0,1);
	else if((t>=220 && t<=235) || (t>=300 && t<=315))
		Print(winx+4,winy+92,"They appear facing up.",0,1);
	if(t==98 ||
	   t==118 ||
	   t==138 ||
	   t==158)
	{
		// exit
		PrintGlow(winx+40,winy+8,"Exit",0,2);
		Print(winx+4,winy+32,"Can be a goal of the",0,1);
		Print(winx+4,winy+52,"level to escape here, or",0,1);
		Print(winx+4,winy+72,"rescue critters here.",0,1);
		Print(winx+4,winy+92,"Does not affect objects.",0,1);
	}
}

void EditMenu::RenderItemWindow(int msx,int msy)
{
	int winx,winy;

	winx=x+88;
	winy=y+13;

	toolmenu->RenderItem(winx+22,winy+24);

	switch(toolmenu->GetItemTool())
	{
		case ITM_BARREL:
			PrintGlow(winx+52,winy+8,"Barrel",0,2);
			Print(winx+4,winy+40,"Can be pushed and",0,1);
			Print(winx+4,winy+55,"blown up. Grey ones",0,1);
			Print(winx+4,winy+70,"can't be destroyed.",0,1);
			break;
		case ITM_BALL:
			PrintGlow(winx+52,winy+8,"Ball",0,2);
			Print(winx+4,winy+40,"Can be pushed and",0,1);
			Print(winx+4,winy+55,"blown up. They roll",0,1);
			Print(winx+4,winy+70,"when pushed. Grey",0,1);
			Print(winx+4,winy+85,"ones can't be destroyed.",0,1);
			break;
		case ITM_CRATE:
			PrintGlow(winx+52,winy+8,"Crate",0,2);
			Print(winx+4,winy+40,"Can't be pushed, but",0,1);
			Print(winx+4,winy+55,"can be blown up. Grey",0,1);
			Print(winx+4,winy+70,"ones can't be destroyed.",0,1);
			break;
		case ITM_SWITCH:
		case ITM_SWITCH2:
			PrintGlow(winx+52,winy+8,"Switch",0,2);
			Print(winx+4,winy+40,"If anything bumps into",0,1);
			Print(winx+4,winy+55,"it, all doors of the same",0,1);
			Print(winx+4,winy+70,"color as the switch will",0,1);
			Print(winx+4,winy+85,"toggle if not blocked.",0,1);
			break;
		case ITM_CRANK:
			PrintGlow(winx+52,winy+8,"Crank",0,2);
			Print(winx+4,winy+40,"If anything bumps into",0,1);
			Print(winx+4,winy+55,"it, all arrows will",0,1);
			Print(winx+4,winy+70,"rotate clockwise. The",0,1);
			Print(winx+4,winy+85,"color is just for looks.",0,1);
			break;
		case ITM_DOOR:
			PrintGlow(winx+52,winy+8,"Door",0,2);
			Print(winx+4,winy+40,"Completely impassable.",0,1);
			Print(winx+4,winy+55,"Can be opened with keys,",0,1);
			Print(winx+4,winy+70,"switches, or pressure pads",0,1);
			Print(winx+4,winy+85,"of the same color.",0,1);
			break;
		case ITM_OPENDOOR:
			PrintGlow(winx+52,winy+8,"Open Door",0,2);
			Print(winx+4,winy+40,"The same as empty floor,",0,1);
			Print(winx+4,winy+55,"but it can be shut with",0,1);
			Print(winx+4,winy+70,"a switch or pressure pad.",0,1);
			break;
		case ITM_BOMB:
			PrintGlow(winx+52,winy+8,"Bomb",0,2);
			Print(winx+4,winy+40,"The player picks it",0,1);
			Print(winx+4,winy+55,"up and can plant it to",0,1);
			Print(winx+4,winy+70,"destroy objects. Grey",0,1);
			Print(winx+4,winy+85,"bombs blow up any color.",0,1);
			break;
		case ITM_DETONATE:
			PrintGlow(winx+52,winy+8,"Detonator",0,2);
			Print(winx+4,winy+40,"The player picks it up",0,1);
			Print(winx+4,winy+55,"and uses it to blow up all",0,1);
			Print(winx+4,winy+70,"bombs of the same color.",0,1);
			Print(winx+4,winy+85,"Grey ones destroy all",0,1);
			Print(winx+4,winy+100,"colors at once!",0,1);
			break;
		case ITM_SHIELD:
			PrintGlow(winx+52,winy+8,"Shield",0,2);
			Print(winx+4,winy+40,"The player picks it",0,1);
			Print(winx+4,winy+55,"up and it activates",0,1);
			Print(winx+4,winy+70,"instantly, making you",0,1);
			Print(winx+4,winy+85,"invincible for 10 seconds.",0,1);
			Print(winx+4,winy+100,"Color is just for looks.",0,1);
			break;
		case ITM_ACID:
			PrintGlow(winx+52,winy+8,"Acid",0,2);
			Print(winx+4,winy+40,"The player picks it",0,1);
			Print(winx+4,winy+55,"up and can use it to",0,1);
			Print(winx+4,winy+70,"erase one object of the",0,1);
			Print(winx+4,winy+85,"same color. Grey ones",0,1);
			Print(winx+4,winy+100,"erase any color.",0,1);
			break;
		case ITM_LASER:
			PrintGlow(winx+52,winy+8,"Laser",0,2);
			Print(winx+4,winy+40,"The player picks it",0,1);
			Print(winx+4,winy+55,"up and can fire it to",0,1);
			Print(winx+4,winy+70,"destroy objects in a",0,1);
			Print(winx+4,winy+85,"straight line. Follows",0,1);
			Print(winx+4,winy+100,"arrows.",0,1);
			break;
		case ITM_KEY:
			PrintGlow(winx+52,winy+8,"Key",0,2);
			Print(winx+4,winy+40,"The player picks it",0,1);
			Print(winx+4,winy+55,"up and it is used when",0,1);
			Print(winx+4,winy+70,"they bump a door of the",0,1);
			Print(winx+4,winy+85,"same color to open it.",0,1);
			Print(winx+4,winy+100,"Grey opens all doors.",0,1);
			break;
		case ITM_ICEBERG:
			PrintGlow(winx+52,winy+8,"Iceberg",0,2);
			Print(winx+4,winy+40,"Can't be pushed, but can",0,1);
			Print(winx+4,winy+55,"be blown up. Grey ones",0,1);
			Print(winx+4,winy+70,"are indestructible.",0,1);
			break;
		case ITM_GOODIE:
			PrintGlow(winx+52,winy+8,"Goodie",0,2);
			Print(winx+4,winy+40,"Invisible to the player.",0,1);
			Print(winx+4,winy+55,"Maximum of one per level.",0,1);
			Print(winx+4,winy+70,"It's just a special prize",0,1);
			Print(winx+4,winy+85,"to find by:",0,1);
			switch(toolmenu->facing)
			{
				case 0:
					Print(winx+4,winy+100,"blowing it up.",0,1);
					break;
				case 1:
					Print(winx+4,winy+100,"stepping on it.",0,1);
					break;
				case 2:
					Print(winx+4,winy+100,"pushing an item.",0,1);
					break;
				case 3:
					Print(winx+4,winy+100,"a critter's step.",0,1);
					break;
			}
			break;
		case ITM_PENCIL:
			PrintGlow(winx+52,winy+8,"Pencil",0,2);
			Print(winx+4,winy+40,"Player: Earn 15 points.",0,1);
			Print(winx+4,winy+55,"Objects: Impassable.",0,1);
			Print(winx+4,winy+70,"Critter: Impassable.",0,1);
			break;
	}
}

void EditMenu::RenderCritterWindow(int msx,int msy)
{
	int winx,winy;

	winx=x+88;
	winy=y+13;

	switch(toolmenu->curTool)
	{
		case 0:
			InstaRenderMonster(winx+44,winy+20,MONS_LOONY,1,profile.color,profile.bright,mgl);
			PrintGlow(winx+140,winy+8,"Player",0,2);
			Print(winx+4,winy+40,"Put this where you want",0,1);
			Print(winx+4,winy+55,"the player to start.",0,1);
			break;
		case 1:
			InstaRenderMonster(winx+44,winy+20,MONS_BLOBBY,toolmenu->facing,toolmenu->color,0,mgl);
			PrintGlow(winx+140,winy+8,"Blobby",0,2);
			Print(winx+4,winy+40,"Moves totally randomly.",0,1);
			break;
		case 2:
			InstaRenderMonster(winx+44,winy+20,MONS_BUNNY,toolmenu->facing,toolmenu->color,0,mgl);
			PrintGlow(winx+140,winy+8,"Fluffy",0,2);
			Print(winx+4,winy+40,"Turns clockwise when it",0,1);
			Print(winx+4,winy+55,"bumps a wall.",0,1);
			break;
		case 3:
			InstaRenderMonster(winx+44,winy+20,MONS_INCHY,toolmenu->facing,toolmenu->color,0,mgl);
			PrintGlow(winx+140,winy+8,"Inchy",0,2);
			Print(winx+4,winy+40,"Reverses direction when",0,1);
			Print(winx+4,winy+55,"it bumps a wall.",0,1);
			break;
		case 4:
			InstaRenderMonster(winx+44,winy+20,MONS_PIGGY,toolmenu->facing,toolmenu->color,0,mgl);
			PrintGlow(winx+140,winy+8,"Piggy",0,2);
			Print(winx+4,winy+40,"Always moves towards",0,1);
			Print(winx+4,winy+55,"the player.",0,1);
			break;
	}

	if(toolmenu->curTool!=0)
	{
		switch(toolmenu->color)
		{
			case 0:
				Print(winx+4,winy+80,"Grey critters are",0,1);
				Print(winx+4,winy+95,"invincible and",0,1);
				Print(winx+4,winy+110,"harmless.",0,1);
				break;
			case 1:
				Print(winx+4,winy+95,"Green critters push",0,1);
				Print(winx+4,winy+110,"objects they bump into.",0,1);
				break;
			case 2:
				Print(winx+4,winy+95,"Brown critters are",0,1);
				Print(winx+4,winy+110,"harmless.",0,1);
				break;
			case 3:
				Print(winx+4,winy+95,"Blue critters freeze",0,1);
				Print(winx+4,winy+110,"objects they bump into.",0,1);
				break;
			case 4:
				Print(winx+4,winy+95,"Red critters light",0,1);
				Print(winx+4,winy+110,"bombs they bump into.",0,1);
				break;
			case 5:
				Print(winx+4,winy+80,"Yellow critters eat",0,1);
				Print(winx+4,winy+95,"objects they bump into.",0,1);
				break;
			case 6:
				Print(winx+4,winy+95,"Purple critters are",0,1);
				Print(winx+4,winy+110,"deadly to the player.",0,1);
				break;
			case 7:
				Print(winx+4,winy+95,"Aqua critters shoot",0,1);
				Print(winx+4,winy+110,"at the player.",0,1);
				break;
		}
	}
}

void EditMenu::RenderExtraWindow(int msx,int msy)
{
	char btnName[2][8]={"Author","Tiles"};
	int winx,winy;
	int i;

	winx=x+88;
	winy=y+13;

	for(i=0;i<2;i++)
	{
		if(PointInRect(msx,msy,winx-x+140,winy-y+8+22*i,winx-x+200,winy-y+29+22*i))
			DrawFillBox(winx+140,winy+8+22*i,winx+200-2,winy+29+22*i,236);
		RightPrintGlow(winx+200-4,winy+9+22*i,btnName[i],0,2);
	}

	// author
	DrawBox(winx+6,winy+7,winx+135,winy+20,255-32+8+12*(extraMode==0));
	DrawFillBox(winx+7,winy+8,winx+134,winy+19,255-32+4);
	if((extraMode==0) && (curFlash&3))
	{
		EditorGetWorld()->desc[strlen(EditorGetWorld()->desc)+1]='\0';
		EditorGetWorld()->desc[strlen(EditorGetWorld()->desc)]='_';
	}
	PrintLimited(winx+8,winy+8,winx+135,EditorGetWorld()->desc,0,1);
	if((extraMode==0) && (curFlash&3))
	{
		EditorGetWorld()->desc[strlen(EditorGetWorld()->desc)-1]='\0';
	}

	// tile set
	DrawBox(winx+6,winy+29,winx+135,winy+42,255-32+8+12*(extraMode==1));
	DrawFillBox(winx+7,winy+30,winx+134,winy+41,255-32+4);
	if((extraMode==1) && (curFlash&3))
	{
		EditorGetWorld()->tileName[strlen(EditorGetWorld()->tileName)+1]='\0';
		EditorGetWorld()->tileName[strlen(EditorGetWorld()->tileName)]='_';
	}
	PrintLimited(winx+8,winy+30,winx+135,EditorGetWorld()->tileName,0,1);
	if((extraMode==1) && (curFlash&3))
	{
		EditorGetWorld()->tileName[strlen(EditorGetWorld()->tileName)-1]='\0';
	}

	// buttons
	DrawFillBox(winx+6,winy+50,winx+16,winy+60,255-32+4);
	if(!showItems)
		DrawFillBox(winx+7,winy+51,winx+15,winy+59,255);
	Print(winx+20,winy+50,"Hide Items",0,1);

	DrawFillBox(winx+6,winy+65,winx+16,winy+75,255-32+4);
	if(!showGuys)
		DrawFillBox(winx+7,winy+66,winx+15,winy+74,255);
	Print(winx+20,winy+65,"Hide Critters",0,1);

	DrawFillBox(winx+6,winy+80,winx+16,winy+90,255-32+4);
	if(!delMode)
		DrawFillBox(winx+7,winy+81,winx+15,winy+89,255);
	Print(winx+20,winy+80,"Rt-click zaps floor",0,1);

	Print(winx+6,winy+105,"See keyboard shortcuts",0,1);

	// show the text in case it's too long
	//if(GetStrLength(EditorGetWorld()->desc,1)+10>134-7)
	PrintOutline(2,420,EditorGetWorld()->desc,0,0);
	//if(GetStrLength(EditorGetWorld()->desc,1)+10>134-7)
	PrintOutline(2,450,EditorGetWorld()->tileName,0,0);
}

void EditMenu::RenderScoreWindow(int msx,int msy)
{
	char btnName[3][8]={"Gold","Silver","Bronze"};
	int winx,winy;
	int i;

	winx=x+88;
	winy=y+13;

	PrintGlow(winx+5,winy+5,"High Scores",0,0);

	for(i=0;i<3;i++)
	{
		if(PointInRect(msx,msy,winx-x+140,winy-y+30+22*i,winx-x+200,winy-y+51+22*i))
			DrawFillBox(winx+140,winy+30+22*i,winx+200-2,winy+51+22*i,236);
		RightPrintGlow(winx+200-4,winy+31+22*i,btnName[i],0,2);

		DrawBox(winx+6,winy+34+i*22,winx+115,winy+47+i*22,255-32+8+12*(scoreMode==i));
		DrawFillBox(winx+7,winy+35+i*22,winx+114,winy+46+i*22,255-32+4);
		if((scoreMode==i) && (curFlash&3))
		{
			scoreTxt[i][strlen(scoreTxt[i])+1]='\0';
			scoreTxt[i][strlen(scoreTxt[i])]='_';
		}
		PrintLimited(winx+8,winy+35+i*22,winx+115,scoreTxt[i],0,1);
		if((scoreMode==i) && (curFlash&3))
		{
			scoreTxt[i][strlen(scoreTxt[i])-1]='\0';
		}
	}
}

byte EditMenu::KeyPress(char c)
{
	if(c==27)	// ESC is not checked in this part
		return 0;
	if(c==13)	// Enter... would it load or save?  Let's ignore it.
		return 0;
	if(c==9)
		return 0;	// tab has no effect
	if(c==0)
		return 0;

	if(winminmax==0 || minmax==0)
		return 0;
	if(menu==MENU_FILE)
	{
		if(c==8)	// backspace
		{
			if(strlen(curFname)>0)
				curFname[strlen(curFname)-1]='\0';
			return 1;
		}
		else
		{
			if(strlen(curFname)<FNAMELEN-2)
			{
				curFname[strlen(curFname)+1]='\0';
				curFname[strlen(curFname)]=c;
			}
			return 1;
		}
	}
	if(menu==MENU_EXTRA)
	{
		if(extraMode==0)
		{
			if(c==8)	// backspace
			{
				if(strlen(EditorGetWorld()->desc)>0)
					EditorGetWorld()->desc[strlen(EditorGetWorld()->desc)-1]='\0';
				return 1;
			}
			else
			{
				if(strlen(EditorGetWorld()->desc)<FNAMELEN-2)
				{
					EditorGetWorld()->desc[strlen(EditorGetWorld()->desc)+1]='\0';
					EditorGetWorld()->desc[strlen(EditorGetWorld()->desc)]=c;
				}
				return 1;
			}
		}
		else
		{
			if(c==8)	// backspace
			{
				if(strlen(EditorGetWorld()->tileName)>0)
					EditorGetWorld()->tileName[strlen(EditorGetWorld()->tileName)-1]='\0';
				return 1;
			}
			else
			{
				if(strlen(EditorGetWorld()->tileName)<FNAMELEN-2)
				{
					EditorGetWorld()->tileName[strlen(EditorGetWorld()->tileName)+1]='\0';
					EditorGetWorld()->tileName[strlen(EditorGetWorld()->tileName)]=c;
				}
				return 1;
			}
		}
	}
	if(menu==MENU_SCORE)
	{
		if(c==8)	// backspace
		{
			if(strlen(scoreTxt[scoreMode])>0)
				scoreTxt[scoreMode][strlen(scoreTxt[scoreMode])-1]='\0';
			SetScores();
			return 1;
		}
		else if(c>='0' && c<='9')	// numbers only
		{
			if(strlen(scoreTxt[scoreMode])<12)
			{
				scoreTxt[scoreMode][strlen(scoreTxt[scoreMode])+1]='\0';
				scoreTxt[scoreMode][strlen(scoreTxt[scoreMode])]=c;
			}
			SetScores();
			return 1;
		}
	}
	if(menu==MENU_SETOPT)
	{
		if(c==8)	// backspace
		{
			if(strlen(EditorGetWorld()->setname)>0)
				EditorGetWorld()->setname[strlen(EditorGetWorld()->setname)-1]='\0';
			return 1;
		}
		else
		{
			if(strlen(EditorGetWorld()->setname)<32-2)
			{
				EditorGetWorld()->setname[strlen(EditorGetWorld()->setname)+1]='\0';
				EditorGetWorld()->setname[strlen(EditorGetWorld()->setname)]=c;
			}
			return 1;
		}
	}
	if(menu==MENU_LVLOPT)
	{
		if(c==8)	// backspace
		{
			if(strlen(EditorGetMap()->name)>0)
				EditorGetMap()->name[strlen(EditorGetMap()->name)-1]='\0';
			return 1;
		}
		else
		{
			if(strlen(EditorGetMap()->name)<32-2)
			{
				EditorGetMap()->name[strlen(EditorGetMap()->name)+1]='\0';
				EditorGetMap()->name[strlen(EditorGetMap()->name)]=c;
			}
			return 1;
		}
	}

	return 0;
}

byte EditMenu::AlwaysKeyPress(char c,byte x,byte y)
{
	if(c=='g' || c=='G')
	{
		toolmenu->Suck(&EditorGetMap()->map[x+y*EditorGetMap()->width]);
		return 1;
	}
	if(c=='t' || c=='T')
	{
		toolmenu->NextTool();
		menu=toolmenu->toolType+MENU_FLOOR;
		return 1;
	}
	if(c==27)	// ESC exits
	{
		yesno->Activate("Exit editor?","","Yes","No");
		editMode=EDITMODE_YESNO;
		askingFor=3;
		if(MenusHidden())	// must show menus to get yesno visible
			HideMenus();
		return 1;
	}
	return 0;
}

void EditMenu::SetScoreTxt(void)
{
	int i;

	for(i=0;i<3;i++)
		sprintf(scoreTxt[i],"%d",GoldScore(curLvl,i));
}

void EditMenu::SetScores(void)
{
	int i;

	for(i=0;i<3;i++)
		SetGoldScore(curLvl,i,atoi(scoreTxt[i]));
}

void EditMenu::GetAnswer(void)
{
	char name[48];

	switch(askingFor)
	{
		case 1:	// load
			if(yesno->answer==ANSWER_YES)
			{
				if(CanEditFile(curFname))
				{
					sprintf(name,"levels/%s",curFname);
					if(strlen(name)>4 && strcmp(&name[strlen(name)-4],".sbl"))
					{
						strcat(name,".sbl");	// concatenate the ending on
					}
					EditorLoadWorld(name);
					LoadAddOnScores(name);
					SetScoreTxt();
					EditorSelectMap(0);
					curLvl=0;
					lvlPos=0;
				}
				else
				{
					yesno->Activate("You may not","load that!","Drat","Darn");
					editMode=EDITMODE_YESNO;
					askingFor=9;
				}
			}
			break;
		case 2:	// save
			if(yesno->answer==ANSWER_YES)
			{
				if(CanEditFile(curFname))
				{
					sprintf(name,"levels/%s",curFname);
					if(strlen(name)>4 && strcmp(&name[strlen(name)-4],".sbl"))
					{
						strcat(name,".sbl");	// concatenate the ending on
					}
					EditorSaveWorld(name);
					SetAddOnScoreName(name);
					ResetHighScores(GAME_ADDON);
					SaveAddOnScores();
					ScanFiles();
				}
				else
				{
					yesno->Activate("That file is","read-only!","Drat","Darn");
					editMode=EDITMODE_YESNO;
					askingFor=9;
				}
			}
			break;
		case 3:	// exit
			if(yesno->answer==ANSWER_YES)
			{
				editMode=EDITMODE_EXIT;
			}
			break;
		case 4:	// new
			if(yesno->answer==ANSWER_YES)
			{
				EditorNewWorld();
				EditorSelectMap(0);
				curLvl=0;
				lvlPos=0;
			}
			break;
		case 5:	// delete level
			if(yesno->answer==ANSWER_YES)
			{
				curLvl=EditorGetMapNum();
				WorldDeleteLevel(EditorGetWorld(),curLvl);
				if(curLvl>=EditorGetWorld()->numMaps)
					curLvl--;
				EditorSelectMap(curLvl);
			}
			break;
		case 6: // copy level
			if(yesno->answer==ANSWER_YES)
				WorldNewLevel(EditorGetWorld(),EditorGetMap());
			break;
		case 7:	// fill map
			if(yesno->answer==ANSWER_YES)
			{
				int i;

				for(i=0;i<EditorGetMap()->width*EditorGetMap()->height;i++)
					EditorGetMap()->map[i].floor=GetTileNumber(toolmenu->curTool,toolmenu->color,toolmenu->facing);
			}
			break;
		case 8:	// merge levels
			if(yesno->answer==ANSWER_YES)
			{
				if(CanEditFile(curFname))
				{
					sprintf(name,"levels/%s",curFname);
					if(strlen(name)>4 && strcmp(&name[strlen(name)-4],".sbl"))
					{
						strcat(name,".sbl");	// concatenate the ending on
					}
					EditorMergeWorld(name);
				}
				else
				{
					yesno->Activate("You may not","load that!","Drat","Darn");
					editMode=EDITMODE_YESNO;
					askingFor=9;
				}
			}
			break;
		case 9:	// just a general warning
			// doesn't matter what you answer
			break;
	}
}

byte EditMenu::ShowGuys(void)
{
	return showGuys;
}

byte EditMenu::ShowItems(void)
{
	return showItems;
}

byte EditMenu::DelMode(void)
{
	return delMode;
}

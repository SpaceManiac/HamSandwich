#include "winpch.h"
#include "editor.h"
#include "terrainedit.h"
#include "itemedit.h"
#include "soundedit.h"
#include "monsteredit.h"
#include "specialedit.h"
#include "tool.h"
#include "edithelp.h"
#include "yesnodialog.h"
#include "leveldialog.h"
#include "viewdialog.h"
#include "worldstitch.h"
#include "levelscan.h"

#define PLOPRATE	5

static char lastKey=0;

static MGLDraw *editmgl;

static world_t	world;
static Map		*editorMap;
static byte		curMapNum;
static int	   mouseX,mouseY;
static int	   tileX,tileY;
static int	rectX1,rectX2,rectY1,rectY2;
static int  pickerWid,pickerHei;

static byte  showStats=0;
static dword gameStartTime,visFrameCount,updFrameCount;
static word  numRunsToMakeUp;

static byte plopCounter=0;
static byte msButton=0;
static byte monsPage=0;

static byte viewMenu,editMenu;

static byte musicPlaying;

static char worldFilename[FNAMELEN]="";

static byte displayFlags;
byte editing=0;

static int lastPick;

byte editMode=EDITMODE_EDIT;

byte InitEditor(void)
{
	int i;

	NewWorld(&world,editmgl);
	editorMap=world.map[0];
	curMapNum=0;

	ToolInit();

	mouseX=320;
	mouseY=240;
	PutCamera(0,0);
	gameStartTime=timeGetTime();
	InitGuys(256);
	musicPlaying=0;
	lastKey=0;
	InitPlayer(0,"TEST");
	InitBullets();
	// modify monsters for the editor
	//strcpy(MonsterName(MONS_ROLLER2),"Roly Poly Rvs");
	strcpy(MonsterName(MONS_ROLLER4),"Rumbly Tumbly Rvs");
	strcpy(MonsterName(MONS_SNOWBALL2),"Snowball Rvs");
	strcpy(MonsterName(MONS_PUFFYFISH2),"Puffyfish Rvs");
	strcpy(MonsterName(MONS_GENERATOR1),"Generator-1s");
	strcpy(MonsterName(MONS_GENERATOR2),"Generator-5s");
	strcpy(MonsterName(MONS_GENERATOR3),"Generator-15s");
	strcpy(MonsterName(MONS_GENERATOR4),"Generator-30s");
	strcpy(MonsterName(MONS_FRIENDLY2),"Buddy Bunny:Determined");
	strcpy(MonsterName(MONS_FOLLOWBUNNY),"Buddy Bunny:Follow");
	strcpy(MonsterName(MONS_SNKYSHRK2),"Sneaky Shark: Chest");
	strcpy(MonsterName(MONS_UNDERZOID),"Aquazoid Underwater");
	//strcpy(MonsterName(MONS_PATCH2),"Pumpkin Patch Fast");
	strcpy(MonsterName(MONS_UNDERMAGMA),"Magmazoid Underwater");
	strcpy(MonsterName(MONS_PARKED2),"Parked Car2");
	strcpy(MonsterName(MONS_TRAFFIC),"Traffic: Clockwise");
	strcpy(MonsterName(MONS_TRAFFIC2),"Traffic: Counterclockwise");
	strcpy(MonsterName(MONS_PATROLLR),"Patrol Mumble Horiz.");
	strcpy(MonsterName(MONS_PATROLUD),"Patrol Mumble Vert.");
	strcpy(MonsterName(MONS_DPATROLLR),"Death Patrol Horiz.");
	strcpy(MonsterName(MONS_DPATROLUD),"Death Patrol Vert.");
	strcpy(MonsterName(MONS_TURRETR),"Laser Right");
	strcpy(MonsterName(MONS_TURRETD),"Laser Down");
	strcpy(MonsterName(MONS_TURRETL),"Laser Left");
	strcpy(MonsterName(MONS_TURRETU),"Laser Up");

	for(i=MONS_SUCKER1;i<=MONS_BLOWER4;i++)
	{
		MonsterAnim(i,0)[0]=0;
	}
	MonsterAnim(MONS_DARKNESS,0)[0]=1;

	ChangeOffColor(MONS_SHARK,2,4);
	ChangeOffColor(MONS_SNKYSHRK2,2,4);

	editmgl->MouseTap();
	editmgl->RMouseTap();
	editmgl->LastKeyPressed();

	viewMenu=0;
	editMenu=1;
	editing=1;
	editMode=EDITMODE_HELP;
	InitEditHelp(HELP_BASIC);

	displayFlags=MAP_SHOWWALLS|MAP_SHOWLIGHTS|MAP_SHOWBADGUYS|
			MAP_SHOWSPECIALS|MAP_SHOWPICKUPS|MAP_SHOWOTHERITEMS;

	InitSpecials(world.map[0]->special);
	StopSong();
	SetPlayerStart(-1,-1);
	return 1;
}

void ExitEditor(void)
{
	int i;

	ExitFileDialog();
	ToolExit();
	ExitBullets();
	EditorSaveWorld("worlds/backup_exit.shw");

	// change monsters back to normal
	ChangeOffColor(MONS_SHARK,255,255);
	ChangeOffColor(MONS_SNKYSHRK2,255,255);
	//strcpy(MonsterName(MONS_ROLLER2),"Roly Poly");
	strcpy(MonsterName(MONS_ROLLER4),"Rumbly Tumbly");
	strcpy(MonsterName(MONS_SNOWBALL2),"Snowball");
	strcpy(MonsterName(MONS_GENERATOR1),"Generator");
	strcpy(MonsterName(MONS_GENERATOR2),"Generator");
	strcpy(MonsterName(MONS_GENERATOR3),"Generator");
	strcpy(MonsterName(MONS_GENERATOR4),"Generator");
	strcpy(MonsterName(MONS_FRIENDLY2),"Buddy Bunny");
	strcpy(MonsterName(MONS_FOLLOWBUNNY),"Buddy Bunny");
	strcpy(MonsterName(MONS_SNKYSHRK2),"Sneaky Shark");
	strcpy(MonsterName(MONS_UNDERZOID),"Aquazoid");
	//strcpy(MonsterName(MONS_PATCH2),"Pumpkin Patch");
	strcpy(MonsterName(MONS_UNDERMAGMA),"Magmazoid");
	strcpy(MonsterName(MONS_PARKED2),"Parked Car");
	strcpy(MonsterName(MONS_TRAFFIC),"Traffic");
	strcpy(MonsterName(MONS_TRAFFIC2),"Traffic");
	strcpy(MonsterName(MONS_PATROLLR),"Patrol Mumble");
	strcpy(MonsterName(MONS_PATROLUD),"Patrol Mumble");
	strcpy(MonsterName(MONS_DPATROLLR),"Death Patrol");
	strcpy(MonsterName(MONS_DPATROLUD),"Death Patrol");
	strcpy(MonsterName(MONS_TURRETR),"Laser");
	strcpy(MonsterName(MONS_TURRETD),"Laser");
	strcpy(MonsterName(MONS_TURRETL),"Laser");
	strcpy(MonsterName(MONS_TURRETU),"Laser");

	for(i=MONS_SUCKER1;i<=MONS_BLOWER4;i++)
	{
		MonsterAnim(i,0)[0]=254;
	}
	MonsterAnim(MONS_DARKNESS,0)[0]=254;

	StopSong();
	ExitGuys();
	FreeWorld(&world);
	PurgeMonsterSprites();
	editing=0;
}

void Delete(int x,int y)
{
	int i;

	if(x<0 || x>editorMap->width-1 || y<0 || y>editorMap->height-1)
		return;

	for(i=0;i<MAX_MAPMONS;i++)
		if((editorMap->badguy[i].type) && (editorMap->badguy[i].x==x) && (editorMap->badguy[i].y==y))
		{
			DeleteGuy((x*TILE_WIDTH+(TILE_WIDTH/2))<<FIXSHIFT,
				   (y*TILE_HEIGHT+(TILE_HEIGHT/2))<<FIXSHIFT,
				   editorMap->badguy[i].type);
			editorMap->badguy[i].type=0;
		}
	editorMap->GetTile(x,y)->item=0;

	for(i=0;i<MAX_SPECIAL;i++)
		if(editorMap->special[i].trigger && editorMap->special[i].x==x && editorMap->special[i].y==y)
		{
			memset(&editorMap->special[i],0,sizeof(special_t));
			editorMap->special[i].x=255;
		}
}

void BackupWorld(const char *name)
{
	char inName[128],outName[128];
	FILE *inF,*outF;
	int bytes;
	byte *data;

	sprintf(inName,"worlds/%s",name);
	sprintf(outName,"worlds/backup_save.shw");

	inF=fopen(inName,"rb");
	if(!inF)
		return;	// the source didn't exist, so nothing to back up
	outF=fopen(outName,"wb");
	if(!outF)
	{
		fclose(inF);
		return;	// was unable to open the file for writing for some reason
	}
	data=(byte *)malloc(sizeof(byte)*1024);
	if(!data)
	{
		fclose(inF);
		fclose(outF);
		return;	// couldn't allocate that lousy bit of memory!
	}
	while(1)
	{
		bytes=fread(data,sizeof(byte),1024,inF);
		if(bytes>0)
			fwrite(data,sizeof(byte),bytes,outF);
		if(bytes<1024)
			break;
	}
	free(data);
	fclose(inF);
	fclose(outF);
}

void UpdateMouse(void)
{
	int cx,cy;

	editmgl->GetMouse(&mouseX,&mouseY);

	if(mouseX<0)
		mouseX=0;
	if(mouseY<0)
		mouseY=0;
	if(mouseX>639)
		mouseX=639;
	if(mouseY>479)
		mouseY=479;

	GetCamera(&cx,&cy);

	tileX=(mouseX+cx-320);
	tileY=(mouseY+cy-240);

	if(tileX<0)
		tileX=tileX/TILE_WIDTH-1;
	else
		tileX=tileX/TILE_WIDTH;
	if(tileY<0)
		tileY=tileY/TILE_HEIGHT-1;
	else
		tileY=tileY/TILE_HEIGHT;

	if(mouseX==0)
	{
		cx-=8;
		if(cx<0)
			cx=0;
		PutCamera(cx<<FIXSHIFT,cy<<FIXSHIFT);
	}
	if(mouseX==639)
	{
		cx+=8;
		if(cx>editorMap->width*TILE_WIDTH)
			cx=editorMap->width*TILE_WIDTH;
		PutCamera(cx<<FIXSHIFT,cy<<FIXSHIFT);
	}
	if(mouseY==0)
	{
		cy-=8;
		if(cy<0)
			cy=0;
		PutCamera(cx<<FIXSHIFT,cy<<FIXSHIFT);
	}
	if(mouseY==479)
	{
		cy+=8;
		if(cy>editorMap->height*TILE_HEIGHT)
			cy=editorMap->height*TILE_HEIGHT;
		PutCamera(cx<<FIXSHIFT,cy<<FIXSHIFT);
	}

	switch(editMode)
	{
		case EDITMODE_TERRAIN:
			TerrainEdit_Update(mouseX,mouseY,editmgl);
			break;
		case EDITMODE_SPECIAL:
			SpecialEdit_Update(mouseX,mouseY,editmgl);
			break;
		case EDITMODE_ITEM:
			ItemEdit_Update(mouseX,mouseY,editmgl);
			break;
		case EDITMODE_SOUND:
			SoundEdit_Update(mouseX,mouseY,editmgl);
			break;
		case EDITMODE_PICKENEMY:
			MonsterEdit_Update(mouseX,mouseY,editmgl);
			break;
		case EDITMODE_EDIT:
			if(!viewMenu || !editMenu || (ToolDoing()!=TD_USING) || (viewMenu && ViewDialogClick(mouseX,mouseY)))
				ToolUpdate(mouseX,mouseY,editMenu,editmgl);
			break;
		case EDITMODE_EXIT:
			if(editmgl->MouseTap())
				YesNoDialogClick(mouseX,mouseY);
			if(YesNoDialogCommand()==YNM_YES)
			{
				editMode=EDITMODE_EXITYES;
				ExitYesNoDialog();
			}
			else if(YesNoDialogCommand()==YNM_NO)
			{
				editMode=EDITMODE_EDIT;
				ExitYesNoDialog();
			}
			break;
		case EDITMODE_FILE:
			if(editmgl->MouseTap())
				FileDialogClick(mouseX,mouseY);
			switch(FileDialogCommand())
			{
				case FM_NEW:
					EditorSaveWorld("worlds/backup_load.shw");
					FreeWorld(&world);
					NewWorld(&world,editmgl);
					EditorSelectMap(0);
					editMode=EDITMODE_EDIT;
					break;
				case FM_MERGE:
					if(GetFilename("")[0])	// don't do any of this if the filename is blank!
					{
						AddWorldIn(&world,GetFilename("worlds/"));
						InitEditHelp(HELP_WORLDSTITCH);
						editMode=EDITMODE_HELP;
					}
					else
						editMode=EDITMODE_EDIT;
					break;
				case FM_LOAD:
					if(GetFilename("")[0])	// don't do any of this if the filename is blank!
					{
						EditorSaveWorld("worlds/backup_load.shw");
						ToolSetFilename();
						FreeWorld(&world);
						if(!LoadWorld(&world,GetFilename("worlds/")))
							NewWorld(&world,editmgl);	// if you can't load it, start a new one instead
						EditorSelectMap(0);
						editMode=EDITMODE_EDIT;
					}
					else
						editMode=EDITMODE_EDIT;
					break;
				case FM_SAVE:
					if(GetFilename("")[0])	// don't do any of this if the filename is blank!
					{
						if(strcmp(&GetFilename("")[strlen(GetFilename(""))-4],".shw"))
						{
							AddDLWToFilename();
						}
						ToolSetFilename();
						GetCamera(&cx,&cy);
						BackupWorld(GetFilename(""));
						SaveWorld(&world,GetFilename("worlds/"));
						EditorSelectMap(curMapNum);
						PutCamera(cx*FIXAMT,cy*FIXAMT);
					}
					else
						MakeNormalSound(SND_TURRETBZZT);
					editMode=EDITMODE_EDIT;
					break;
				case FM_EXIT:
					editMode=EDITMODE_EDIT;
					break;
				case FM_FOILEDAGAIN:
					SetStitchError("Unable to save over hollow.shw.");
					InitEditHelp(HELP_WORLDSTITCH);
					editMode=EDITMODE_HELP;
					break;
			}
			break;
		case EDITMODE_MAPMENU:
			if(editmgl->MouseTap())
			{
				if(!MapDialogClick(mouseX,mouseY))
				{
					editMode=EDITMODE_EDIT;
				}
			}
			break;
		case EDITMODE_LEVELMENU:
			if(editmgl->MouseTap())
			{
				if(!LevelDialogClick(mouseX,mouseY))
				{
					editMode=EDITMODE_EDIT;
				}
			}
			if(editmgl->RMouseTap())
			{
				if(!LevelDialogRClick(mouseX,mouseY))
				{
					editMode=EDITMODE_EDIT;
				}
			}
			break;
		case EDITMODE_HELP:
			if(editmgl->MouseTap())
				if(EditHelpClick(mouseX,mouseY))
				{
					editMode=EDITMODE_EDIT;
					ExitEditHelp();
				}
			break;
		case EDITMODE_PICKSPOT:
		case EDITMODE_PICKSPOT2:
			if(editmgl->MouseTap())
			{
				editMode=EDITMODE_SPECIAL;
				cx=tileX;
				cy=tileY;
				if(cx<0)
					cx=0;
				if(cy<0)
					cy=0;
				if(cx>=editorMap->width)
					cx=editorMap->width-1;
				if(cy>=editorMap->height)
					cy=editorMap->height-1;
				tileX=cx;
				tileY=cy;
				SetSpecialCoords(tileX,tileY);
			}
			break;
		case EDITMODE_PICKRSPOT:
			if(editmgl->MouseTap())
			{
				editMode=EDITMODE_PICKRSPOT2;
				cx=tileX;
				cy=tileY;
				if(cx<0)
					cx=0;
				if(cy<0)
					cy=0;
				if(cx>=editorMap->width)
					cx=editorMap->width-1;
				if(cy>=editorMap->height)
					cy=editorMap->height-1;
				tileX=cx;
				tileY=cy;
				rectX1=tileX;
				rectY1=tileY;
				rectX2=tileX;
				rectY2=tileY;
			}
			break;
		case EDITMODE_PICKRSPOT2:
			cx=tileX;
			cy=tileY;
			if(cx<0)
				cx=0;
			if(cy<0)
				cy=0;
			if(cx>=editorMap->width)
				cx=editorMap->width-1;
			if(cy>=editorMap->height)
				cy=editorMap->height-1;
			rectX2=cx;
			rectY2=cy;

			if(editmgl->MouseTap())
			{
				editMode=EDITMODE_SPECIAL;
				if(rectX2<rectX1)
				{
					cx=rectX2;
					rectX2=rectX1;
					rectX1=cx;
				}
				if(rectY2<rectY1)
				{
					cy=rectY2;
					rectY2=rectY1;
					rectY1=cy;
				}
				SetSpecialRect(rectX1,rectY1,rectX2,rectY2);
			}
			break;
	}
}

byte EditorRun(int *lastTime)
{
	numRunsToMakeUp=0;
	if(*lastTime>TIME_PER_FRAME*5)
		*lastTime=TIME_PER_FRAME*5;

	while(*lastTime>=TIME_PER_FRAME)
	{
		if(!editmgl->Process())
			return QUITGAME;

		// update everything here
		UpdateItems();
		EditorUpdateGuys(editorMap);
		if(displayFlags&MAP_TEMPTORCH)
		{
			editorMap->Update(UPDATE_FADE,&world);
			editorMap->GlowCursor(tileX,tileY,10,40);
		}
		else
			editorMap->Update(UPDATE_EDIT,&world);

		UpdateMouse();

		*lastTime-=TIME_PER_FRAME;
		numRunsToMakeUp++;
		updFrameCount++;
	}

	if(curMapNum==0)
		editorMap->flags|=MAP_HUB;

	return CONTINUE;
}

void ShowSpecials(void)
{
	int i;
	int sx,sy;
	char s[8];

	if(!(displayFlags&MAP_SHOWSPECIALS))
		return;

	GetCamera(&sx,&sy);
	for(i=0;i<MAX_SPECIAL;i++)
		if(editorMap->special[i].x!=255)
		{
			Print(editorMap->special[i].x*TILE_WIDTH+2-sx+320-1,
				  editorMap->special[i].y*TILE_HEIGHT-sy+240-1,
				  "Spcl",-32,1);
			Print(editorMap->special[i].x*TILE_WIDTH+2-sx+320+1,
				  editorMap->special[i].y*TILE_HEIGHT-sy+240+1,
				  "Spcl",-32,1);
			Print(editorMap->special[i].x*TILE_WIDTH+2-sx+320,
				  editorMap->special[i].y*TILE_HEIGHT-sy+240,
				  "Spcl",0,1);
			sprintf(s,"%03d",i);
			Print(editorMap->special[i].x*TILE_WIDTH+2-sx+320-1,
				  editorMap->special[i].y*TILE_HEIGHT+12-sy+240-1,
				  s,-32,1);
			Print(editorMap->special[i].x*TILE_WIDTH+2-sx+320+1,
				  editorMap->special[i].y*TILE_HEIGHT+12-sy+240+1,
				  s,-32,1);
			Print(editorMap->special[i].x*TILE_WIDTH+2-sx+320,
				  editorMap->special[i].y*TILE_HEIGHT+12-sy+240,
				  s,0,1);
		}
}

void ShowSpecials2(void)
{
	int i;
	int sx,sy;

	if(!(displayFlags&MAP_SHOWSPECIALS))
		return;

	GetCamera(&sx,&sy);
	for(i=0;i<MAX_SPECIAL;i++)
		if(editorMap->special[i].x!=255)
		{
			Print(editorMap->special[i].x*TILE_WIDTH+2-sx+320-1,
				  editorMap->special[i].y*TILE_HEIGHT+6-sy+240-1,
				  "Spcl",-32,1);
			Print(editorMap->special[i].x*TILE_WIDTH+2-sx+320+1,
				  editorMap->special[i].y*TILE_HEIGHT+6-sy+240+1,
				  "Spcl",-32,1);
			Print(editorMap->special[i].x*TILE_WIDTH+2-sx+320,
				  editorMap->special[i].y*TILE_HEIGHT+6-sy+240,
				  "Spcl",0,1);
		}
}
void EditorShowRect(void)
{
	int x1,x2,y1,y2,cx,cy;
	static byte col=0;

	col=255-col;

	x1=rectX1;
	x2=rectX2;
	y1=rectY1;
	y2=rectY2;
	if(x1>x2)
	{
		cx=x1;
		x1=x2;
		x2=cx;
	}
	if(y1>y2)
	{
		cy=y1;
		y1=y2;
		y2=cy;
	}
	GetCamera(&cx,&cy);

	x1=x1*TILE_WIDTH-(cx-320);
	y1=y1*TILE_HEIGHT-(cy-240);

	x2=x2*TILE_WIDTH-(cx-320)+TILE_WIDTH-1;
	y2=y2*TILE_HEIGHT-(cy-240)+TILE_HEIGHT-1;

	DrawBox(x1,y1,x2,y2,col);
}

void EditorDraw(void)
{
	char s[16];

	switch(editMode)
	{
		case EDITMODE_EDIT:
			if(displayFlags&MAP_SHOWBADGUYS)
				RenderGuys(displayFlags&MAP_SHOWLIGHTS);
			RenderItAll(&world,editorMap,displayFlags);
			ShowSpecials();
			ToolShowTarget();
			if(editMenu)
				ToolRender(mouseX,mouseY,editmgl);
			if(viewMenu && editMenu)
				RenderViewDialog(mouseX,mouseY,editmgl);
			break;
		case EDITMODE_PICKSPOT:
			if(displayFlags&MAP_SHOWBADGUYS)
				RenderGuys(displayFlags&MAP_SHOWLIGHTS);
			RenderItAll(&world,editorMap,displayFlags);
			ShowSpecials();
			ToolShowTarget();
			Print(3,466,"Click on a location!",-32,1);
			Print(1,464,"Click on a location!",-32,1);
			Print(2,465,"Click on a location!",0,1);
			sprintf(s,"(%03d,%03d)",tileX,tileY);
			Print(580,466,s,-32,1);
			Print(578,464,s,-32,1);
			Print(579,465,s,0,1);
			break;
		case EDITMODE_PICKSPOT2:
			if(displayFlags&MAP_SHOWBADGUYS)
				RenderGuys(displayFlags&MAP_SHOWLIGHTS);
			RenderItAll(&world,editorMap,displayFlags);
			ShowSpecials();
			rectX1=tileX;
			rectY1=tileY;
			rectX2=tileX+pickerWid;
			rectY2=tileY+pickerHei;
			EditorShowRect();
			Print(3,466,"Click on a location!",-32,1);
			Print(1,464,"Click on a location!",-32,1);
			Print(2,465,"Click on a location!",0,1);
			sprintf(s,"(%03d,%03d)",tileX,tileY);
			Print(580,466,s,-32,1);
			Print(578,464,s,-32,1);
			Print(579,465,s,0,1);
			break;
		case EDITMODE_PICKRSPOT:
			if(displayFlags&MAP_SHOWBADGUYS)
				RenderGuys(displayFlags&MAP_SHOWLIGHTS);
			RenderItAll(&world,editorMap,displayFlags);
			ShowSpecials();
			ToolShowTarget();
			Print(3,466,"Click a corner of your rectangle!",-32,1);
			Print(1,464,"Click a corner of your rectangle!",-32,1);
			Print(2,465,"Click a corner of your rectangle!",0,1);
			sprintf(s,"(%03d,%03d)",tileX,tileY);
			Print(580,466,s,-32,1);
			Print(578,464,s,-32,1);
			Print(579,465,s,0,1);
			break;
		case EDITMODE_PICKRSPOT2:
			if(displayFlags&MAP_SHOWBADGUYS)
				RenderGuys(displayFlags&MAP_SHOWLIGHTS);
			RenderItAll(&world,editorMap,displayFlags);
			ShowSpecials();
			EditorShowRect();
			Print(3,466,"Click the other corner of your rectangle!",-32,1);
			Print(1,464,"Click the other corner of your rectangle!",-32,1);
			Print(2,465,"Click the other corner of your rectangle!",0,1);
			sprintf(s,"(%03d,%03d)",tileX,tileY);
			Print(580,466,s,-32,1);
			Print(578,464,s,-32,1);
			Print(579,465,s,0,1);
			break;
		case EDITMODE_HELP:
			if(displayFlags&MAP_SHOWBADGUYS)
				RenderGuys(displayFlags&MAP_SHOWLIGHTS);
			RenderItAll(&world,editorMap,displayFlags);
			ShowSpecials();
			ToolShowTarget();
			if(editMenu)
				ToolRender(mouseX,mouseY,editmgl);
			if(viewMenu && editMenu)
				RenderViewDialog(mouseX,mouseY,editmgl);
			RenderEditHelp(mouseX,mouseY,editmgl);
			break;
		case EDITMODE_TERRAIN:
			TerrainEdit_Render(mouseX,mouseY,editmgl);
			break;
		case EDITMODE_ITEM:
			ItemEdit_Render(mouseX,mouseY,editmgl);
			break;
		case EDITMODE_SPECIAL:
			SpecialEdit_Render(mouseX,mouseY,editmgl);
			break;
		case EDITMODE_SOUND:
			SoundEdit_Render(mouseX,mouseY,editmgl);
			break;
		case EDITMODE_PICKENEMY:
			MonsterEdit_Render(mouseX,mouseY,editmgl);
			break;
		case EDITMODE_FILE:
			if(displayFlags&MAP_SHOWBADGUYS)
				RenderGuys(displayFlags&MAP_SHOWLIGHTS);
			RenderItAll(&world,editorMap,displayFlags);
			ShowSpecials();
			RenderFileDialog(mouseX,mouseY,editmgl);
			break;
		case EDITMODE_EXIT:
			if(displayFlags&MAP_SHOWBADGUYS)
				RenderGuys(displayFlags&MAP_SHOWLIGHTS);
			RenderItAll(&world,editorMap,displayFlags);
			ShowSpecials();
			RenderYesNoDialog(mouseX,mouseY,editmgl);
			break;
		case EDITMODE_MAPMENU:
			if(displayFlags&MAP_SHOWBADGUYS)
				RenderGuys(displayFlags&MAP_SHOWLIGHTS);
			RenderItAll(&world,editorMap,displayFlags);
			ShowSpecials();
			RenderMapDialog(mouseX,mouseY,editmgl);
			break;
		case EDITMODE_LEVELMENU:
			if(displayFlags&MAP_SHOWBADGUYS)
				RenderGuys(displayFlags&MAP_SHOWLIGHTS);
			RenderItAll(&world,editorMap,displayFlags);
			ShowSpecials();
			RenderLevelDialog(mouseX,mouseY,editmgl);
			break;
	}

	// draw the mouse cursor
	DrawMouseCursor(mouseX,mouseY);

	editmgl->Flip();
}

static void HandleKeyPresses(void)
{
	char k;
	byte s;
	int x,y;

	s=LastScanCode();
	k=editmgl->LastKeyPressed();
	if(k)
		lastKey=k;

	switch(s)
	{
		case SDL_SCANCODE_LEFT: // left arrow
		case SDL_SCANCODE_KP_4:
			GetCamera(&x,&y);
			if(GetControls()&CONTROL_B1)
				x-=TILE_WIDTH*3;
			x-=TILE_WIDTH;
			if(x<0)
				x=0;
			PutCamera(x<<FIXSHIFT,y<<FIXSHIFT);
			break;
		case SDL_SCANCODE_UP:	// up arrow
		case SDL_SCANCODE_KP_8:
			GetCamera(&x,&y);
			if(GetControls()&CONTROL_B1)
				y-=TILE_HEIGHT*3;
			y-=TILE_HEIGHT;
			if(y<0)
				y=0;
			PutCamera(x<<FIXSHIFT,y<<FIXSHIFT);
			break;
		case SDL_SCANCODE_RIGHT:	// right arrow
		case SDL_SCANCODE_KP_6:
			GetCamera(&x,&y);
			if(GetControls()&CONTROL_B1)
				x+=TILE_WIDTH*3;
			x+=TILE_WIDTH;
			if(x>editorMap->width*TILE_WIDTH)
				x=editorMap->width*TILE_WIDTH;
			PutCamera(x<<FIXSHIFT,y<<FIXSHIFT);
			break;
		case SDL_SCANCODE_DOWN:	// down arrow
		case SDL_SCANCODE_KP_2:
			GetCamera(&x,&y);
			if(GetControls()&CONTROL_B1)
				y+=TILE_HEIGHT*3;
			y+=TILE_HEIGHT;
			if(y>editorMap->height*TILE_HEIGHT)
				y=editorMap->height*TILE_HEIGHT;
			PutCamera(x<<FIXSHIFT,y<<FIXSHIFT);
			break;
		case SDL_SCANCODE_F1:	// F1
			if(editMode==EDITMODE_EDIT)
				ToolGetHelp();
			if(editMode==EDITMODE_ITEM)
				ItemEditHelp();
			if(editMode==EDITMODE_PICKENEMY)
				MonsterEdit_Help();
			if(editMode==EDITMODE_SOUND)
				SoundEdit_Help();
			if(editMode==EDITMODE_SPECIAL)
				SpecialEdit_Help();
			if(editMode==EDITMODE_TERRAIN)
				TerrainEdit_Help();
			break;
		case SDL_SCANCODE_TAB:
			editMenu=1-editMenu;
			break;
	}

	if(editMode==EDITMODE_EDIT)
	{
		switch(k)
		{
			case 'f':
			case 'F':
				editMode=EDITMODE_FILE;
				InitFileDialog("worlds/*.shw",FM_NEW|FM_LOAD|FM_SAVE|FM_ASKLOAD,ToolGetFilename());
				break;
			case 'M':
				editMode=EDITMODE_FILE;
				InitFileDialog("worlds/*.shw",FM_LOAD|FM_ASKLOAD|FM_MERGE,ToolGetFilename());
				break;
			case 'l':
			case 'L':
				editMode=EDITMODE_LEVELMENU;
				InitLevelDialog(&world,curMapNum);
				break;
			case 'w':
			case 'W':
				editMode=EDITMODE_MAPMENU;
				InitMapDialog(&world,curMapNum);
				break;
			case 't':
			case 'T':
				int cx,cy;
				GetCamera(&cx,&cy);
				TestLevel(EditorGetWorld(),EditorGetMapNum());
				StopSong();
				SetPlayerStart(-1,-1);
				ExitPlayer();
				AddMapGuys(EditorGetMap());
				PutCamera(cx<<FIXSHIFT,cy<<FIXSHIFT);
				break;
			case 'b':
			case 'B':
				ToolBrushUp();
				break;
			case 'v':
			case 'V':
				ViewMenuOn();
				break;
			case 'C':
				for(x=0;x<editorMap->width*editorMap->height;x++)
					editorMap->map[x].light=editorMap->GetTile(tileX,tileY)->light;
				break;
			case 8:
				Delete(tileX,tileY);
				break;
			case 'g':
			case 'G':
				// absorb under the cursor
				ToolSuckUp(tileX,tileY);
				break;
			case 'Q':
				LogRequirements(&world);
				SetStitchError("Saved world requirements list to req_files.txt.");
				InitEditHelp(HELP_WORLDSTITCH);
				editMode=EDITMODE_HELP;
				break;
			case 'I':
				if(!Scan_Level(&world,editorMap) || !Scan_Vars(&world))
					SetStitchError("Error saving information!");
				else
					SetStitchError("Saved information to level_scan.txt and var_scan.txt.");
				InitEditHelp(HELP_WORLDSTITCH);
				editMode=EDITMODE_HELP;
				break;
			case 27:
				if(viewMenu)
				{
					ViewMenuOff();
				}
				else
				{
					InitYesNoDialog("Exit the editor?","Yes","No");
					editMode=EDITMODE_EXIT;
				}
				break;
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
				PickTool(k-'1');
				break;
		}
	}
	else if(editMode==EDITMODE_PICKSPOT || editMode==EDITMODE_PICKRSPOT || editMode==EDITMODE_PICKRSPOT2 ||
		editMode==EDITMODE_PICKSPOT2)
	{
		if(k==27)
		{
			if(WhichTool()==TOOL_SPECIAL)
			{
				if(editMode==EDITMODE_PICKSPOT || editMode==EDITMODE_PICKSPOT2)
					SetSpecialCoords(-1,-1);
				else
					SetSpecialRect(-1,-1,-1,-1);
				editMode=EDITMODE_SPECIAL;
			}
			else
				SetEditMode(EDITMODE_EDIT);
			lastKey=0;
		}
	}
	else if(editMode==EDITMODE_FILE)
	{
		if(k)
		{
			if(!FileDialogKey(lastKey))
			{
				ExitFileDialog();
				SetEditMode(EDITMODE_EDIT);
			}
			lastKey=0;
		}
	}
	else if(editMode==EDITMODE_EXIT)
	{
		if(k)
		{
			YesNoDialogKey(lastKey);
			lastKey=0;
		}
	}
	else if(editMode==EDITMODE_SPECIAL)
	{
		SpecialEdit_Key(lastKey);
		lastKey=0;
	}
	else if(editMode==EDITMODE_MAPMENU)
	{
		if(k)
		{
			if(!MapDialogKey(lastKey))
			{
				ExitMapDialog();
				SetEditMode(EDITMODE_EDIT);
			}
			lastKey=0;
		}
	}
	else if(editMode==EDITMODE_LEVELMENU)
	{
		if(k)
		{
			if(!LevelDialogKey(lastKey))
			{
				ExitLevelDialog();
				SetEditMode(EDITMODE_EDIT);
			}
			lastKey=0;
		}
	}
	else if(editMode==EDITMODE_TERRAIN)
	{
		TerrainEdit_Key(lastKey);
		lastKey=0;
	}
	else if(editMode==EDITMODE_ITEM)
	{
		ItemEdit_Key(lastKey);
		lastKey=0;
	}
	else if(editMode==EDITMODE_SOUND)
	{
		SoundEdit_Key(lastKey);
		lastKey=0;
	}
	else if(editMode==EDITMODE_PICKENEMY)
	{
		MonsterEdit_Key(lastKey);
		lastKey=0;
	}
	else if(editMode==EDITMODE_HELP)
	{
		if(!EditHelpKey(lastKey))
		{
			ExitEditHelp();
			SetEditMode(EDITMODE_EDIT);
		}
		lastKey=0;
	}
	else
	{
		switch(k)
		{
			case 27:
				SetEditMode(EDITMODE_EDIT);
				lastKey=0;
				break;
		}
	}
}

void SetEditMode(byte m)
{
	editMode=m;
	if(editMode==EDITMODE_EDIT && viewMenu)
		InitViewDialog();
}

byte LunaticEditor(MGLDraw *mgl)
{
	int lastTime=1;
	byte exitcode=0;

	editmgl=mgl;

	if(!InitEditor())
		return QUITGAME;

	exitcode=CONTINUE;
	while(exitcode==CONTINUE)
	{
		lastTime+=TimeLength();
		StartClock();
		HandleKeyPresses();
		exitcode=EditorRun(&lastTime);
		EditorDraw();

		if(editMode==EDITMODE_EXITYES)
			exitcode=QUITGAME;
		if(!editmgl->Process())
			exitcode=QUITGAME;
		EndClock();
	}

	ExitEditor();
	return exitcode;
}

void EditorSaveWorld(const char *fname)
{
	SaveWorld(&world,fname);
}

void EditorSelectMap(byte w)
{
	editorMap=world.map[w];
	curMapNum=w;
	AddMapGuys(editorMap);
	if(goodguy)
		PutCamera(goodguy->x,goodguy->y);
	PutCamera(320<<FIXSHIFT,240<<FIXSHIFT);
	GetSpecialsFromMap(editorMap->special);
}

void PickedTile(int t)
{
	lastPick=t;
}

world_t *EditorGetWorld(void)
{
	return &world;
}

int EditorGetLastPick(void)
{
	return lastPick;
}

void EditorGetTileXY(int *x,int *y)
{
	*x=tileX;
	*y=tileY;
}

Map *EditorGetMap(void)
{
	return editorMap;
}

byte EditorGetMapNum(void)
{
	return curMapNum;
}

byte EditorShowMonsItems(void)
{
	return 1;
}

byte EditorGetSelectMode(void)
{
	if(displayFlags&MAP_SHOWSELECT)
		return 1;
	else return 0;
}

void EditorSetSelectMode(byte mode)
{
	if(mode)
		displayFlags|=MAP_SHOWSELECT;
	else
		displayFlags&=(~MAP_SHOWSELECT);
}

void SetEditPickerRect(int wid,int hei)
{
	pickerWid=wid;
	pickerHei=hei;
}

byte GetDisplayFlags(void)
{
	return displayFlags;
}

void ToggleDisplayFlag(byte f)
{
	displayFlags^=f;
}

void ViewMenuOff(void)
{
	ExitViewDialog();
	viewMenu=0;
}

void ViewMenuOn(void)
{
	InitViewDialog();
	viewMenu=1;
}

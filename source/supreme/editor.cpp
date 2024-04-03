#include "winpch.h"
#include "editor.h"
#include "terrainedit.h"
#include "itemedit.h"
#include "soundedit.h"
#include "monsteredit.h"
#include "bulletedit.h"
#include "specialedit.h"
#include "tool.h"
#include "edithelp.h"
#include "yesnodialog.h"
#include "leveldialog.h"
#include "viewdialog.h"
#include "worldstitch.h"
#include "levelscan.h"
#include "appdata.h"
#include "exportdialog.h"
#include "steam.h"

byte editing = 0;

namespace
{
	char lastKey = 0;

	MGLDraw *editmgl;

	world_t world;
	Map *editorMap;
	byte curMapNum;
	int mouseX,mouseY,mouseZ;
	int tileX,tileY;
	int rectX1,rectX2,rectY1,rectY2;
	int pickerWid,pickerHei;

	dword gameStartTime,updFrameCount;
	word numRunsToMakeUp;

	byte viewMenu, editMenu;

	byte musicPlaying;
	word displayFlags;

	byte editMode = EDITMODE_EDIT;

	int lastPick;

	byte zoom = 1;

	dword oldGamepad = ~0;
}

byte InitEditor(void)
{
	int i;

	NewWorld(&world,editmgl);
	editorMap=world.map[0];
	curMapNum=0;

	ToolInit();

	mouseX=320;
	mouseY=240;
	mouseZ = editmgl->mouse_z;
	PutCamera(0,0);
	gameStartTime=timeGetTime();
	InitGuys(256);
	musicPlaying=0;
	lastKey=0;
	InitPlayer(0,"TEST");
	// modify monsters for the editor
	strcpy(MonsterName(MONS_ROLLER2),"Roly Poly Rvs");
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
	strcpy(MonsterName(MONS_PATCH2),"Pumpkin Patch Fast");
	strcpy(MonsterName(MONS_UNDERMAGMA),"Magmazoid Underwater");
	strcpy(MonsterName(MONS_PARKED2),"Parked Car2");
	strcpy(MonsterName(MONS_TRAFFIC),"Traffic: Clockwise");
	strcpy(MonsterName(MONS_TRAFFIC2),"Traffic: Counterclockwise");
	strcpy(MonsterName(MONS_PATROLLR),"Patrol Mumble Horiz.");
	strcpy(MonsterName(MONS_PATROLUD),"Patrol Mumble Vert.");
	strcpy(MonsterName(MONS_DPATROLLR),"Death Patrol Horiz.");
	strcpy(MonsterName(MONS_DPATROLUD),"Death Patrol Vert.");

	for(i=MONS_SUCKER1;i<=MONS_BLOWER4;i++)
	{
		MonsterAnim(i,0)[0]=0;
	}

	ChangeOffColor(MONS_SHARK,2,4);
	ChangeOffColor(MONS_SNKYSHRK2,2,4);

	editmgl->MouseTap();
	editmgl->RMouseTap();
	editmgl->LastKeyPressed();
	oldGamepad = ~0;

	viewMenu=0;
	editMenu=1;
	editing=1;
	editMode=EDITMODE_HELP;
	InitEditHelp(HELP_BASIC);

	displayFlags =
		MAP_SHOWWALLS |
		MAP_SHOWLIGHTS |
		MAP_SHOWBADGUYS |
		MAP_SHOWSPECIALS |
		MAP_SHOWPICKUPS |
		MAP_SHOWOTHERITEMS |
		(zoom == 1 ? 0 : MAP_ZOOMOUT);

	InitSpecials(world.map[0]->special);
	StopSong();
	SetPlayerStart(-1,-1);
	InitStars();

	SteamManager::Get()->SetPresenceEditor();
	return 1;
}

void ExitEditor(void)
{
	int i;

	ExitFileDialog();
	ToolExit();

	EditorSaveWorld("worlds/backup_exit.dlw");

	// change monsters back to normal
	ChangeOffColor(MONS_SHARK,255,255);
	ChangeOffColor(MONS_SNKYSHRK2,255,255);
	strcpy(MonsterName(MONS_ROLLER2),"Roly Poly");
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
	strcpy(MonsterName(MONS_PATCH2),"Pumpkin Patch");
	strcpy(MonsterName(MONS_UNDERMAGMA),"Magmazoid");
	strcpy(MonsterName(MONS_PARKED2),"Parked Car");
	strcpy(MonsterName(MONS_TRAFFIC),"Traffic");
	strcpy(MonsterName(MONS_TRAFFIC2),"Traffic");
	strcpy(MonsterName(MONS_PATROLLR),"Patrol Mumble");
	strcpy(MonsterName(MONS_PATROLUD),"Patrol Mumble");
	strcpy(MonsterName(MONS_DPATROLLR),"Death Patrol");
	strcpy(MonsterName(MONS_DPATROLUD),"Death Patrol");

	for(i=MONS_SUCKER1;i<=MONS_BLOWER4;i++)
	{
		MonsterAnim(i,0)[0]=254;
	}
	StopSong();
	ExitGuys();
	FreeWorld(&world);
	PurgeMonsterSprites();
	editing=0;

	editmgl->ResizeBuffer(SCRWID, SCRHEI);

	SteamManager::Get()->SetPresenceNone();
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
		if(editorMap->special[i].x==x && editorMap->special[i].y==y)
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
	sprintf(outName,"worlds/backup_save.dlw");

	inF=AssetOpen(inName);
	if(!inF)
		return;	// the source didn't exist, so nothing to back up
	outF=AssetOpen_Write(outName);
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
	AppdataSync();
}

TASK(void) UpdateMouse(void)
{
	int cx,cy;

	editmgl->GetMouse(&mouseX,&mouseY);

	if(mouseX<0)
		mouseX=0;
	if(mouseY<0)
		mouseY=0;
	if(mouseX>=editmgl->GetWidth())
		mouseX=editmgl->GetWidth()-1;
	if(mouseY>=editmgl->GetHeight())
		mouseY=editmgl->GetHeight()-1;

	GetCamera(&cx,&cy);

	tileX=(mouseX+cx - editmgl->GetWidth()/2);
	tileY=(mouseY+cy - editmgl->GetHeight()/2);

	if(tileX<0)
		tileX=tileX/TILE_WIDTH-1;
	else
		tileX=tileX/TILE_WIDTH;
	if(tileY<0)
		tileY=tileY/TILE_HEIGHT-1;
	else
		tileY=tileY/TILE_HEIGHT;

	int scroll = editmgl->mouse_z - mouseZ;
	mouseZ = editmgl->mouse_z;

	// Mouse scrolling, but exclude fullscreen modes.
	switch (editMode)
	{
		case EDITMODE_TERRAIN:
		case EDITMODE_SPECIAL:
		case EDITMODE_ITEM:
		case EDITMODE_SOUND:
		case EDITMODE_PICKENEMY:
		case EDITMODE_PICKBULLET:
			break;
		default:
			if(mouseX==0)
			{
				cx -= 8 * zoom;
				if(cx<0)
					cx=0;
				PutCamera(cx<<FIXSHIFT,cy<<FIXSHIFT);
			}
			if(mouseX==editmgl->GetWidth()-1)
			{
				cx += 8 * zoom;
				if(cx>editorMap->width*TILE_WIDTH)
					cx=editorMap->width*TILE_WIDTH;
				PutCamera(cx<<FIXSHIFT,cy<<FIXSHIFT);
			}
			if(mouseY==0)
			{
				cy -= 8 * zoom;
				if(cy<0)
					cy=0;
				PutCamera(cx<<FIXSHIFT,cy<<FIXSHIFT);
			}
			if(mouseY==editmgl->GetHeight()-1)
			{
				cy += 8 * zoom;
				if(cy>editorMap->height*TILE_HEIGHT)
					cy=editorMap->height*TILE_HEIGHT;
				PutCamera(cx<<FIXSHIFT,cy<<FIXSHIFT);
			}
			break;
	}

	// The mode itself.
	switch(editMode)
	{
		case EDITMODE_TERRAIN:
			TerrainEdit_Update(mouseX,mouseY,scroll,editmgl);
			break;
		case EDITMODE_SPECIAL:
			SpecialEdit_Update(mouseX,mouseY,scroll,editmgl);
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
		case EDITMODE_PICKBULLET:
			BulletEdit_Update(mouseX, mouseY, editmgl);
			break;
		case EDITMODE_EDIT:
			if(!viewMenu || !editMenu || (ToolDoing()!=TD_USING) || (viewMenu && ViewDialogClick(mouseX,mouseY)))
				AWAIT ToolUpdate(mouseX,mouseY,editMenu,editmgl);
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
			if(scroll)
				FileDialogScroll(scroll);
			switch(FileDialogCommand())
			{
				case FM_NEW:
					EditorSaveWorld("worlds/backup_load.dlw");
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
						EditorSaveWorld("worlds/backup_load.dlw");
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
				case FM_SAVEPACK:
					editMode = EDITMODE_EDIT;
					if(GetFilename("")[0])	// don't do any of this if the filename is blank!
					{
						if(strlen(GetFilename(""))<4 || strcmp(&GetFilename("")[strlen(GetFilename(""))-4],".dlw"))
						{
							AddDLWToFilename();
						}
						ToolSetFilename();
						GetCamera(&cx,&cy);
						BackupWorld(GetFilename(""));
						SaveWorld(&world,GetFilename("worlds/"));
						EditorSelectMap(curMapNum);
						PutCamera(cx*FIXAMT,cy*FIXAMT);

						if (FileDialogCommand() == FM_SAVEPACK)
						{
							InitExportDialog(&world, GetFilename("worlds/"));
							editMode = EDITMODE_EXPORT;
						}
					}
					else
						MakeNormalSound(SND_TURRETBZZT);
					break;
				case FM_EXIT:
					editMode=EDITMODE_EDIT;
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
			if (scroll)
				MapDialogScroll(scroll);
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
		case EDITMODE_EXPORT:
			if(editmgl->MouseTap())
				if (!ExportDialogClick(mouseX,mouseY))
					SetEditMode(EDITMODE_EDIT);
			if(scroll)
				ExportDialogScroll(scroll);
			break;
	}
}

TASK(byte) EditorRun(int *lastTime)
{
	numRunsToMakeUp=0;
	if(*lastTime>TIME_PER_FRAME*5)
		*lastTime=TIME_PER_FRAME*5;

	while(*lastTime>=TIME_PER_FRAME)
	{
		if(!editmgl->Process())
			CO_RETURN QUITGAME;

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

		AWAIT UpdateMouse();

		*lastTime-=TIME_PER_FRAME;
		numRunsToMakeUp++;
		updFrameCount++;
	}

	if(curMapNum==0)
		editorMap->flags|=MAP_HUB;

	CO_RETURN CONTINUE;
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
			Print(editorMap->special[i].x*TILE_WIDTH+2-sx+editmgl->GetWidth()/2-1,
				  editorMap->special[i].y*TILE_HEIGHT-sy+editmgl->GetHeight()/2-1,
				  "Spcl",-32,1);
			Print(editorMap->special[i].x*TILE_WIDTH+2-sx+editmgl->GetWidth()/2+1,
				  editorMap->special[i].y*TILE_HEIGHT-sy+editmgl->GetHeight()/2+1,
				  "Spcl",-32,1);
			Print(editorMap->special[i].x*TILE_WIDTH+2-sx+editmgl->GetWidth()/2,
				  editorMap->special[i].y*TILE_HEIGHT-sy+editmgl->GetHeight()/2,
				  "Spcl",0,1);
			sprintf(s,"%03d",i);
			Print(editorMap->special[i].x*TILE_WIDTH+2-sx+editmgl->GetWidth()/2-1,
				  editorMap->special[i].y*TILE_HEIGHT+12-sy+editmgl->GetHeight()/2-1,
				  s,-32,1);
			Print(editorMap->special[i].x*TILE_WIDTH+2-sx+editmgl->GetWidth()/2+1,
				  editorMap->special[i].y*TILE_HEIGHT+12-sy+editmgl->GetHeight()/2+1,
				  s,-32,1);
			Print(editorMap->special[i].x*TILE_WIDTH+2-sx+editmgl->GetWidth()/2,
				  editorMap->special[i].y*TILE_HEIGHT+12-sy+editmgl->GetHeight()/2,
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
			Print(editorMap->special[i].x*TILE_WIDTH+2-sx+editmgl->GetWidth()/2-1,
				  editorMap->special[i].y*TILE_HEIGHT+6-sy+editmgl->GetHeight()/2-1,
				  "Spcl",-32,1);
			Print(editorMap->special[i].x*TILE_WIDTH+2-sx+editmgl->GetWidth()/2+1,
				  editorMap->special[i].y*TILE_HEIGHT+6-sy+editmgl->GetHeight()/2+1,
				  "Spcl",-32,1);
			Print(editorMap->special[i].x*TILE_WIDTH+2-sx+editmgl->GetWidth()/2,
				  editorMap->special[i].y*TILE_HEIGHT+6-sy+editmgl->GetHeight()/2,
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

	x1=x1*TILE_WIDTH-(cx-editmgl->GetWidth()/2);
	y1=y1*TILE_HEIGHT-(cy-editmgl->GetHeight()/2);

	x2=x2*TILE_WIDTH-(cx-editmgl->GetWidth()/2)+TILE_WIDTH-1;
	y2=y2*TILE_HEIGHT-(cy-editmgl->GetHeight()/2)+TILE_HEIGHT-1;

	DrawBox(x1,y1,x2,y2,col);
}

static void ApplyZoom()
{
	if (zoom == 1)
	{
		editmgl->ResizeBuffer(SCRWID, SCRHEI);
	}
	else
	{
		editmgl->ResizeBuffer(SCRWID * zoom * 4 / 3, SCRHEI * zoom, /*clamp*/ true);
	}
}

void EditorDraw(void)
{
	char s[16];

	switch(editMode)
	{
		case EDITMODE_EDIT:
			ApplyZoom();
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
			ApplyZoom();
			if(displayFlags&MAP_SHOWBADGUYS)
				RenderGuys(displayFlags&MAP_SHOWLIGHTS);
			RenderItAll(&world,editorMap,displayFlags);
			ShowSpecials();
			ToolShowTarget();
			Print(3,editmgl->GetHeight()-480+466,"Click on a location!",-32,1);
			Print(1,editmgl->GetHeight()-480+464,"Click on a location!",-32,1);
			Print(2,editmgl->GetHeight()-480+465,"Click on a location!",0,1);
			sprintf(s,"(%03d,%03d)",tileX,tileY);
			Print(editmgl->GetWidth()-640+580,editmgl->GetHeight()-480+466,s,-32,1);
			Print(editmgl->GetWidth()-640+578,editmgl->GetHeight()-480+464,s,-32,1);
			Print(editmgl->GetWidth()-640+579,editmgl->GetHeight()-480+465,s,0,1);
			break;
		case EDITMODE_PICKSPOT2:
			ApplyZoom();
			if(displayFlags&MAP_SHOWBADGUYS)
				RenderGuys(displayFlags&MAP_SHOWLIGHTS);
			RenderItAll(&world,editorMap,displayFlags);
			ShowSpecials();
			rectX1=tileX;
			rectY1=tileY;
			rectX2=tileX+pickerWid;
			rectY2=tileY+pickerHei;
			EditorShowRect();
			Print(3,editmgl->GetHeight()-480+466,"Click on a location!",-32,1);
			Print(1,editmgl->GetHeight()-480+464,"Click on a location!",-32,1);
			Print(2,editmgl->GetHeight()-480+465,"Click on a location!",0,1);
			sprintf(s,"(%03d,%03d)",tileX,tileY);
			Print(editmgl->GetWidth()-640+580,editmgl->GetHeight()-480+466,s,-32,1);
			Print(editmgl->GetWidth()-640+578,editmgl->GetHeight()-480+464,s,-32,1);
			Print(editmgl->GetWidth()-640+579,editmgl->GetHeight()-480+465,s,0,1);
			break;
		case EDITMODE_PICKRSPOT:
			ApplyZoom();
			if(displayFlags&MAP_SHOWBADGUYS)
				RenderGuys(displayFlags&MAP_SHOWLIGHTS);
			RenderItAll(&world,editorMap,displayFlags);
			ShowSpecials();
			ToolShowTarget();
			Print(3,editmgl->GetHeight()-480+466,"Click a corner of your rectangle!",-32,1);
			Print(1,editmgl->GetHeight()-480+464,"Click a corner of your rectangle!",-32,1);
			Print(2,editmgl->GetHeight()-480+465,"Click a corner of your rectangle!",0,1);
			sprintf(s,"(%03d,%03d)",tileX,tileY);
			Print(editmgl->GetWidth()-640+580,editmgl->GetHeight()-480+466,s,-32,1);
			Print(editmgl->GetWidth()-640+578,editmgl->GetHeight()-480+464,s,-32,1);
			Print(editmgl->GetWidth()-640+579,editmgl->GetHeight()-480+465,s,0,1);
			break;
		case EDITMODE_PICKRSPOT2:
			ApplyZoom();
			if(displayFlags&MAP_SHOWBADGUYS)
				RenderGuys(displayFlags&MAP_SHOWLIGHTS);
			RenderItAll(&world,editorMap,displayFlags);
			ShowSpecials();
			EditorShowRect();
			Print(3,editmgl->GetHeight()-480+466,"Click the other corner of your rectangle!",-32,1);
			Print(1,editmgl->GetHeight()-480+464,"Click the other corner of your rectangle!",-32,1);
			Print(2,editmgl->GetHeight()-480+465,"Click the other corner of your rectangle!",0,1);
			sprintf(s,"(%03d,%03d)",tileX,tileY);
			Print(editmgl->GetWidth()-640+580,editmgl->GetHeight()-480+466,s,-32,1);
			Print(editmgl->GetWidth()-640+578,editmgl->GetHeight()-480+464,s,-32,1);
			Print(editmgl->GetWidth()-640+579,editmgl->GetHeight()-480+465,s,0,1);
			break;
		case EDITMODE_HELP:
			editmgl->ResizeBuffer(SCRWID, SCRHEI);
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
			editmgl->ResizeBuffer(SCRWID, SCRHEI);
			TerrainEdit_Render(mouseX,mouseY,editmgl);
			break;
		case EDITMODE_ITEM:
			editmgl->ResizeBuffer(SCRWID, SCRHEI);
			ItemEdit_Render(mouseX,mouseY,editmgl);
			break;
		case EDITMODE_SPECIAL:
			editmgl->ResizeBuffer(SCRWID, SCRHEI);
			SpecialEdit_Render(mouseX,mouseY,editmgl);
			break;
		case EDITMODE_SOUND:
			editmgl->ResizeBuffer(SCRWID, SCRHEI);
			SoundEdit_Render(mouseX,mouseY,editmgl);
			break;
		case EDITMODE_PICKENEMY:
			editmgl->ResizeBuffer(SCRWID, SCRHEI);
			MonsterEdit_Render(mouseX,mouseY,editmgl);
			break;
		case EDITMODE_PICKBULLET:
			BulletEdit_Render(mouseX, mouseY, editmgl);
			break;
		case EDITMODE_FILE:
			editmgl->ResizeBuffer(SCRWID, SCRHEI);
			if(displayFlags&MAP_SHOWBADGUYS)
				RenderGuys(displayFlags&MAP_SHOWLIGHTS);
			RenderItAll(&world,editorMap,displayFlags);
			ShowSpecials();
			RenderFileDialog(mouseX,mouseY,editmgl);
			break;
		case EDITMODE_EXIT:
			editmgl->ResizeBuffer(SCRWID, SCRHEI);
			if(displayFlags&MAP_SHOWBADGUYS)
				RenderGuys(displayFlags&MAP_SHOWLIGHTS);
			RenderItAll(&world,editorMap,displayFlags);
			ShowSpecials();
			RenderYesNoDialog(mouseX,mouseY,editmgl);
			break;
		case EDITMODE_MAPMENU:
			editmgl->ResizeBuffer(SCRWID, SCRHEI);
			if(displayFlags&MAP_SHOWBADGUYS)
				RenderGuys(displayFlags&MAP_SHOWLIGHTS);
			RenderItAll(&world,editorMap,displayFlags);
			ShowSpecials();
			RenderMapDialog(mouseX,mouseY,editmgl);
			break;
		case EDITMODE_LEVELMENU:
			editmgl->ResizeBuffer(SCRWID, SCRHEI);
			if(displayFlags&MAP_SHOWBADGUYS)
				RenderGuys(displayFlags&MAP_SHOWLIGHTS);
			RenderItAll(&world,editorMap,displayFlags);
			ShowSpecials();
			RenderLevelDialog(mouseX,mouseY,editmgl);
			break;
		case EDITMODE_EXPORT:
			editmgl->ResizeBuffer(SCRWID, SCRHEI);
			if(displayFlags&MAP_SHOWBADGUYS)
				RenderGuys(displayFlags&MAP_SHOWLIGHTS);
			RenderItAll(&world,editorMap,displayFlags);
			ShowSpecials();
			RenderExportDialog(editmgl, mouseX, mouseY);
			break;
	}

	// draw the mouse cursor
	DrawMouseCursor(mouseX,mouseY);
}

static TASK(void) HandleKeyPresses(void)
{
	int x, y;

	byte s = LastScanCode();
	char k = editmgl->LastKeyPressed();
	if(k)
		lastKey = k;
	dword gamepad = GetGamepadButtons();

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
		case SDL_SCANCODE_F2:
			ToggleDisplayFlag(MAP_ZOOMOUT);
			break;
	}

	if(editMode==EDITMODE_EDIT)
	{
		switch(k)
		{
			case 'f':
			case 'F':
				editMode=EDITMODE_FILE;
				InitFileDialog("worlds",".dlw",FM_NEW|FM_LOAD|FM_SAVE|FM_SAVEPACK|FM_ASKLOAD,ToolGetFilename());
				break;
			case 'M':
				editMode=EDITMODE_FILE;
				InitFileDialog("worlds",".dlw",FM_LOAD|FM_ASKLOAD|FM_MERGE,ToolGetFilename());
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
				AWAIT TestLevel(EditorGetWorld(),EditorGetMapNum());
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
				InitExportDialog(&world, nullptr);
				editMode = EDITMODE_EXPORT;
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

		if ((gamepad & ~oldGamepad) & (1 << SDL_CONTROLLER_BUTTON_BACK))
		{
			InitYesNoDialog("Exit the editor?", "Yes", "No");
			editMode = EDITMODE_EXIT;
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
		if((gamepad & ~oldGamepad & (1 << SDL_CONTROLLER_BUTTON_BACK)))
		{
			YesNoDialogKey('y');
		}

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
	else if (editMode == EDITMODE_PICKBULLET)
	{
		BulletEdit_Key(lastKey);
		lastKey = 0;
	}
	else if(editMode==EDITMODE_HELP)
	{
		if(!EditHelpKey(lastKey) || (gamepad & ~oldGamepad & (1 << SDL_CONTROLLER_BUTTON_BACK)))
		{
			ExitEditHelp();
			SetEditMode(EDITMODE_EDIT);
		}

		lastKey=0;
	}
	else if (editMode == EDITMODE_EXPORT)
	{
		if (!ExportDialogKey(lastKey))
		{
			SetEditMode(EDITMODE_EDIT);
		}
		lastKey = 0;
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

	oldGamepad = gamepad;
}

void SetEditMode(byte m)
{
	editMode=m;
	if(editMode==EDITMODE_EDIT && viewMenu)
		InitViewDialog();
}

TASK(byte) LunaticEditor(MGLDraw *mgl)
{
	int lastTime=1;
	byte exitcode=0;

	editmgl=mgl;

	if(!InitEditor())
		CO_RETURN QUITGAME;

	exitcode=CONTINUE;
	while(exitcode==CONTINUE)
	{
		lastTime+=TimeLength();
		StartClock();
		AWAIT HandleKeyPresses();
		exitcode=AWAIT EditorRun(&lastTime);
		EditorDraw();
		AWAIT editmgl->Flip();

		if(editMode==EDITMODE_EXITYES)
			exitcode=QUITGAME;
		if(!editmgl->Process())
			exitcode=QUITGAME;
		EndClock();
	}

	ExitEditor();
	CO_RETURN exitcode;
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
	else
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

word GetDisplayFlags(void)
{
	return displayFlags;
}

void ToggleDisplayFlag(word f)
{
	displayFlags ^= f;
	zoom = (displayFlags & MAP_ZOOMOUT) ? 2 : 1;
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

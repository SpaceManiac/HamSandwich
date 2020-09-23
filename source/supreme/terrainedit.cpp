#include "terrainedit.h"
#include "dialogbits.h"
#include "filedialog.h"
#include "tool.h"
#include "repair.h"
#include "edithelp.h"

#define TMODE_NORMAL	0	// doing nothing
#define TMODE_SELECT	1	// selecting tiles
#define TMODE_PICKNEXT	2	// picking the next tile
#define TMODE_DELETE	3	// asking yes/no on tile deletion
#define TMODE_PICKDEST	4	// picking where to move tiles to
#define TMODE_LOADBMP	5	// BMP file menu
#define TMODE_PICKBMP   6	// picking tiles on a BMP to import
#define TMODE_PICKBMPSEL 7	// actually selecting said tiles
#define TMODE_PICKTILE	8	// not editing terrain at all, just picking which tile to use in the
							// editor
#define TMODE_SAVEBMP	9	// saving the tiles as a BMP
#define TMODE_HELP		10

#define ID_NEXTPAGE		22
#define ID_FWDLOOP		23
#define ID_RVSLOOP		24
#define ID_DELETE		25
#define ID_MOVE			26
#define ID_EXIT			27
#define ID_LOADBMP		28
#define ID_DELETEUNUSED 30
#define ID_AUTOREPAIR	31
#define ID_SAVEBMP		32

static int selStart,selMin,selMax;
static byte mode;
static int tileStart;
static byte dottedLineOfs;
static byte *bmpScr;
static word nextTileShow;
static word showNextFrom;
static byte nextUpdate;
static byte autoRepair=1;
static world_t *world;
static byte noCanPickMsgTime,rememberMode;
static byte helpRemember;
static int tileNum;

static int saveTileStart=0;

static char bmpFilename[FNAMELEN]="";

void TerrainSetupButtons(void);

void FlagClick(int id)
{
	dword flags[]={TF_SOLID,TF_ICE,TF_MUD,TF_WATER,TF_LAVA,TF_PUSHY,TF_PUSHON,TF_ANIM,TF_STEP,
				  TF_DESTRUCT,TF_TRANS,TF_MINECART,TF_BUNNY,TF_NOGHOST,TF_NOENEMY,TF_RUBBER,TF_NOPLAYER,TF_SPACE,TF_QUICKS,TF_PROPULSE};
	int i;
	dword b;

	b=GetButtonState(id);

	if(b==CHECK_ON || b==CHECK_MIXED)	// if it's on, or mixed
	{
		// shut them off
		for (i=selMin;i<=selMax;i++)
		{
			world->terrain[i].flags&=(~flags[id-1]);
		}
		SetButtonState(id,CHECK_OFF);
	}
	else
	{
		// turn them on
		for (i=selMin;i<=selMax;i++)
		{
			world->terrain[i].flags|=flags[id-1];
		}
		SetButtonState(id,CHECK_ON);
	}
	MakeNormalSound(SND_MENUCLICK);
}

void NextTileClick(int id)
{
	MakeNormalSound(SND_MENUCLICK);
	mode=TMODE_PICKNEXT;
}

void NextPageClick(int id)
{
	tileStart+=200;
	if(tileStart>NUMTILES-200)
		tileStart=0;
	if(tileStart>=world->numTiles)
		tileStart=0;
	MakeNormalSound(SND_MENUCLICK);
}

void MakeLoopClick(int id)
{
	int i;

	MakeNormalSound(SND_MENUCLICK);

	if(id==ID_FWDLOOP)
	{
		for(i=selMin;i<selMax;i++)
		{
			world->terrain[i].next=i+1;
		}
		world->terrain[selMax].next=selMin;
	}
	else
	{
		for(i=selMin+1;i<=selMax;i++)
		{
			world->terrain[i].next=i-1;
		}
		world->terrain[selMin].next=selMax;
	}
}

void DeleteClick(int id)
{
	int i;

	if(selMin==0 && world->numTiles==(selMax-selMin+1))
	{
		if(world->numTiles>1)
			selMin=1;
		else
			return;	// can't delete the last tile, dammit!
	}
	MakeNormalSound(SND_MENUCLICK);

	// move everything past them over on top of them
	for(i=selMin;i<world->numTiles-(selMax-selMin+1);i++)
	{
		world->terrain[i]=world->terrain[i+(selMax-selMin+1)];
		memcpy(GetTileData(i),GetTileData(i+(selMax-selMin+1)),TILE_WIDTH*TILE_HEIGHT);
	}

	if(autoRepair)
	{
		InitSwapTable(NUMTILES);
		DeleteBlockFromSwapTable(selMin,selMax);
		RepairTiles();
		ExitSwapTable();
	}

	world->numTiles-=(selMax-selMin+1);
	SetNumTiles(world->numTiles);

	selMin=0;
	selMax=0;

	MakeNormalSound(SND_MENUSELECT);
}

void MoveClick(int id)
{
	mode=TMODE_PICKDEST;
	MakeNormalSound(SND_MENUCLICK);
}

void LoadClick(int id)
{
	InitFileDialog("tilegfx/*.bmp",FM_LOAD|FM_EXIT,bmpFilename);
	mode=TMODE_LOADBMP;
	MakeNormalSound(SND_MENUCLICK);
}

void SaveClick(int id)
{
	InitFileDialog("tilegfx/*.bmp",FM_SAVE|FM_EXIT,bmpFilename);
	mode=TMODE_SAVEBMP;
	MakeNormalSound(SND_MENUCLICK);
}

void ExitClick(int id)
{
	if(mode==TMODE_PICKTILE)
		PickedTile(-1);
	SetEditMode(rememberMode);
	MakeNormalSound(SND_MENUCLICK);
	saveTileStart=tileStart;
}

void DeleteUnusedTiles(int id)
{
	byte tileUsed[NUMTILES];
	int i,j,k;

	MakeNormalSound(SND_MENUCLICK);

	for(i=0;i<world->numTiles;i++)
		tileUsed[i]=0;

	// find all tiles used by items
	for(i=0;i<NumItems();i++)
	{
		if(GetItem(i)->flags&IF_TILE)
			tileUsed[GetItem(i)->sprNum]=1;
	}
	for(i=0;i<world->numMaps;i++)
	{
		for(j=0;j<MAX_SPECIAL;j++)
		{
			if(world->map[i]->special[j].x!=255)
			{
				for(k=0;k<NUM_TRIGGERS;k++)
				{
					if(world->map[i]->special[j].trigger[k].type==TRG_FLOOR ||
						world->map[i]->special[j].trigger[k].type==TRG_FLOORRECT)
					{
						tileUsed[world->map[i]->special[j].trigger[k].value]=1;
					}
					if(world->map[i]->special[j].trigger[k].type==TRG_STEPTILE)
						tileUsed[world->map[i]->special[j].trigger[k].value2]=1;
				}
				for(k=0;k<NUM_EFFECTS;k++)
				{
					if(world->map[i]->special[j].effect[k].type==EFF_CHANGETILE ||
						world->map[i]->special[j].effect[k].type==EFF_OLDTOGGLE)
					{
						tileUsed[world->map[i]->special[j].effect[k].value]=1;
						tileUsed[world->map[i]->special[j].effect[k].value2]=1;
					}
				}
			}
		}
		for(j=0;j<world->map[i]->width*world->map[i]->height;j++)
		{
			tileUsed[world->map[i]->map[j].floor]=1;
			tileUsed[world->map[i]->map[j].wall]=1;
		}
	}
	for(i=0;i<world->numTiles;i++)
	{
		tileUsed[world->terrain[i].next]=1;
	}

	i=0;
	while(i<world->numTiles)
	{
		if(!tileUsed[i])
		{
			for(j=i;j<world->numTiles-1;j++)
			{
				world->terrain[j]=world->terrain[j+1];
				tileUsed[j]=tileUsed[j+1];
				memcpy(GetTileData(j),GetTileData(j+1),TILE_WIDTH*TILE_HEIGHT);
			}

			if(autoRepair)
			{
				InitSwapTable(NUMTILES);
				DeleteBlockFromSwapTable(i,i);
				RepairTiles();
				ExitSwapTable();
			}

			world->numTiles--;
			SetNumTiles(world->numTiles);
		}
		else
			i++;
	}
}

void AutoRepairClick(int id)
{
	MakeNormalSound(SND_MENUCLICK);

	autoRepair=1-autoRepair;
	TerrainSetupButtons();
}

void TerrainSetupButtons(void)
{
	ClearButtons();

	// terrain flags
	MakeButton(BTN_CHECK,1, 0,4,243+0    ,128,15,"Impassable",FlagClick);
	MakeButton(BTN_CHECK,2, 0,4,243+15*1 ,128,15,"Icy",FlagClick);
	MakeButton(BTN_CHECK,3, 0,4,243+15*2 ,128,15,"Muddy",FlagClick);
	MakeButton(BTN_CHECK,4, 0,4,243+15*3 ,128,15,"Water",FlagClick);
	MakeButton(BTN_CHECK,5, 0,4,243+15*4 ,128,15,"Lava",FlagClick);
	MakeButton(BTN_CHECK,6, 0,4,243+15*5 ,128,15,"Pushable",FlagClick);
	MakeButton(BTN_CHECK,7, 0,4,243+15*6 ,128,15,"Can Push On",FlagClick);
	MakeButton(BTN_CHECK,8, 0,4,243+15*7 ,128,15,"Animates To Next",FlagClick);
	MakeButton(BTN_CHECK,9, 0,4,243+15*8 ,128,15,"Animates On Step",FlagClick);
	MakeButton(BTN_CHECK,10,0,4,243+15*9 ,128,15,"Animates When Hit",FlagClick);
	MakeButton(BTN_CHECK,11,0,4,243+15*10,128,15,"Trans./Shadowless",FlagClick);
	MakeButton(BTN_CHECK,12,0,4,243+15*11,128,15,"Drive Path",FlagClick);
	MakeButton(BTN_CHECK,13,0,4,243+15*12,128,15,"Bunny Path",FlagClick);
	MakeButton(BTN_CHECK,14,0,4,243+15*13,128,15,"Ghost Proof",FlagClick);
	MakeButton(BTN_CHECK,15,0,4,243+15*14,128,15,"Enemy Proof",FlagClick);
	MakeButton(BTN_CHECK,16,0,4,243+15*15,128,15,"Bouncy",FlagClick);
	MakeButton(BTN_CHECK,17,0,204,243+15*5,128,15,"Player Proof",FlagClick);
	MakeButton(BTN_CHECK,18,0,204,243+15*6,128,15,"Zero Grav",FlagClick);
	MakeButton(BTN_CHECK,19,0,204,243+15*7,128,15,"Quicksand",FlagClick);
	MakeButton(BTN_CHECK,20,0,204,243+15*8,128,15,"Propulsive",FlagClick);

	// next tile button
	MakeButton(BTN_NORMAL,20,0,200,245,TILE_WIDTH+3,TILE_HEIGHT+3,"",NextTileClick);
	MakeButton(BTN_STATIC,21,0,240,245,2,2,"Next Tile",NULL);

	if(mode!=TMODE_PICKTILE)
	{
		MakeButton(BTN_NORMAL,ID_FWDLOOP,0,200,245+30,128,14,"Make Tile Loop",MakeLoopClick);
		MakeButton(BTN_NORMAL,ID_RVSLOOP,0,200,245+50,128,14,"Make Rvrs Loop",MakeLoopClick);
	}
	// next page button
	MakeButton(BTN_NORMAL,ID_NEXTPAGE,0,560,245,80,14,"Next Page",NextPageClick);

	// delete, move, and exit
	if(mode!=TMODE_PICKTILE)
	{
		MakeButton(BTN_NORMAL,ID_DELETE,0,480,460-120,160,14,"Delete Tiles",DeleteClick);
		MakeButton(BTN_NORMAL,ID_MOVE,0,480,460-100,160,14,"Move Tiles",MoveClick);
		MakeButton(BTN_NORMAL,ID_LOADBMP,0,480,460-80,160,14,"Load Tiles From BMP",LoadClick);
		MakeButton(BTN_NORMAL,ID_SAVEBMP,0,480,460-60,160,14,"Save Tiles To BMPs",SaveClick);
		MakeButton(BTN_NORMAL,ID_DELETEUNUSED,0,480,460-40,160,14,"Delete Unused Tiles",DeleteUnusedTiles);
	}

	if(mode==TMODE_PICKTILE)
		MakeButton(BTN_NORMAL,ID_EXIT,0,480,460,160,14,"Exit Tile Picker",ExitClick);
	else
		MakeButton(BTN_NORMAL,ID_EXIT,0,480,460,160,14,"Exit Tile Editor",ExitClick);

	// auto-repair button
	MakeButton(BTN_CHECK,ID_AUTOREPAIR,(autoRepair)*CHECK_ON,250,460,190,14,"Auto-Repair On Move/Delete",AutoRepairClick);
}

void SetNextTile(word n)
{
	int i;

	for(i=selMin;i<=selMax;i++)
	{
		world->terrain[i].next=n;
	}
}

void TerrainSetFlags(void)
{
	int i,j;
	dword flags[]={TF_SOLID,TF_ICE,TF_MUD,TF_WATER,TF_LAVA,TF_PUSHY,TF_PUSHON,TF_ANIM,TF_STEP,
				  TF_DESTRUCT,TF_TRANS,TF_MINECART,TF_BUNNY,TF_NOGHOST,TF_NOENEMY,TF_RUBBER,TF_NOPLAYER,TF_SPACE,TF_QUICKS,TF_PROPULSE};

	dword flagCount[20];	// count how many tiles have each flag set

	for(i=0;i<20;i++)
		flagCount[i]=0;

	for(i=selMin;i<=selMax;i++)
	{
		for(j=0;j<20;j++)
		{
			if(world->terrain[i].flags&flags[j])
			{
				flagCount[j]++;
			}
		}
	}

	i=(selMax-selMin)+1;

	for(j=0;j<20;j++)
	{
		if(flagCount[j]==0)
			SetButtonState(j+1,CHECK_OFF);
		else if(flagCount[j]<i)
			SetButtonState(j+1,CHECK_MIXED);
		else
			SetButtonState(j+1,CHECK_ON);
	}
}

void TerrainEdit_Init(world_t *wrld)
{
	mode=TMODE_NORMAL;
	GetDisplayMGL()->MouseTap();
	world=wrld;
	TerrainSetupButtons();
	selMin=0;
	selMax=0;
	selStart=0;
	tileStart=saveTileStart;
	if(tileStart>=world->numTiles)
		tileStart=0;
	noCanPickMsgTime=0;
	TerrainSetFlags();
	rememberMode=EDITMODE_EDIT;
	nextUpdate=0;
	nextTileShow=0;
}

void TerrainEdit_InitPicker(byte fromMode,world_t *wrld)
{
	rememberMode=fromMode;
	GetDisplayMGL()->MouseTap();
	world=wrld;
	mode=TMODE_PICKTILE;
	TerrainSetupButtons();
	selMin=0;
	selMax=0;
	selStart=0;
	tileStart=saveTileStart;
	if(tileStart>=world->numTiles)
		tileStart=0;
	noCanPickMsgTime=0;
	TerrainSetFlags();
	nextUpdate=0;
	nextTileShow=0;
}

void TerrainEdit_Exit(void)
{
}

byte MoveTilesTo(int dest)
{
	terrain_t *tempTerrain;
	byte	  *tempImg;
	int i;

	if(dest<=selMax && dest>selMin-(selMax-selMin+1))
	{
		MakeNormalSound(SND_ENERGYBONK);
		return 0;	// invalid
	}
	if(dest>world->numTiles-(selMax-selMin+1))
	{
		MakeNormalSound(SND_ENERGYBONK);
		return 0;	// invalid
	}

	tempTerrain=new terrain_t[selMax-selMin+1];
	tempImg=new byte[(selMax-selMin+1)*TILE_WIDTH*TILE_HEIGHT];

	if(tempTerrain==NULL || tempImg==NULL)
		FatalError("Out of memory!");

	// record the ones to be moved
	for(i=selMin;i<=selMax;i++)
	{
		tempTerrain[i-selMin]=world->terrain[i];
		memcpy(&tempImg[(i-selMin)*TILE_WIDTH*TILE_HEIGHT],GetTileData(i),TILE_WIDTH*TILE_HEIGHT);
	}

	// now swap the other ones over them
	for(i=dest;i<dest+(selMax-selMin+1);i++)
	{
		world->terrain[i-dest+selMin]=world->terrain[i];
		memcpy(GetTileData(i-dest+selMin),GetTileData(i),TILE_WIDTH*TILE_HEIGHT);
	}

	// now paste the moved ones over those!
	for(i=dest;i<dest+(selMax-selMin+1);i++)
	{
		world->terrain[i]=tempTerrain[i-dest];
		memcpy(GetTileData(i),&tempImg[(i-dest)*TILE_WIDTH*TILE_HEIGHT],TILE_WIDTH*TILE_HEIGHT);
	}

	if(autoRepair)
	{
		InitSwapTable(NUMTILES);
		SwapBlockInSwapTable(selMin,selMax,dest);
		RepairTiles();
		ExitSwapTable();
	}

	i=(selMax-selMin+1);
	selMin=dest;
	selMax=dest+i-1;

	delete tempTerrain;
	delete tempImg;

	MakeNormalSound(SND_MENUSELECT);
	return 1;
}

void InitBMPPick(void)
{
	int i;
	byte *scr;
	int pitch;
	char tmpName[64];

	strcpy(bmpFilename,GetFilename(""));
	strcpy(tmpName,GetFilename("tilegfx/"));
	mode=TMODE_PICKBMP;
	bmpScr=new byte[640*480];
	if(!bmpScr)
		FatalError("Out of memory!");
	if(!GetDisplayMGL()->LoadBMP(tmpName, NULL))
	{
		mode=TMODE_NORMAL;
		MakeNormalSound(SND_WALLDOWN);
		return;
	}

	// copy that screen to the storage screen
	scr=GetDisplayMGL()->GetScreen();
	pitch=GetDisplayMGL()->GetWidth();
	for(i=0;i<480;i++)
		memcpy(&bmpScr[i*640],&scr[i*pitch],640);

	selMin=-1;
	selMax=-1;
}

void ExitBMPPick(void)
{
	mode=TMODE_NORMAL;
	delete[] bmpScr;
	selMin=0;
	selMax=0;
}

void ImportTiles(void)
{
	int dst;
	int i;

	if(selMin==-1 || selMax==-1)
		return;

	noCanPickMsgTime=0;

	if((selMax-selMin+1)>NUMTILES-world->numTiles)
	{
		noCanPickMsgTime=120;
		MakeNormalSound(SND_BOMBBOOM);
		selMax=selMin-1+(NUMTILES-world->numTiles);
		if(selMax<selMin)
			return;
	}
	dst=world->numTiles;
	world->numTiles+=(selMax-selMin+1);
	SetNumTiles(world->numTiles);

	for(i=selMin;i<=selMax;i++)
	{
		world->terrain[dst].flags=0;
		world->terrain[dst].next=0;
		SetTile(dst,(i%20)*TILE_WIDTH,(i/20)*TILE_HEIGHT,bmpScr);
		dst++;
	}
	selMin=-1;
	selMax=-1;
}

void TerrainEdit_Update(int mouseX,int mouseY,MGLDraw *mgl)
{
	int n;

	if(nextUpdate)
		nextUpdate--;
	else
	{
		showNextFrom++;
		if(showNextFrom<selMin)
			showNextFrom=selMin;
		if(showNextFrom>selMax)
			showNextFrom=selMin;
		nextUpdate=4;

		nextTileShow=world->terrain[showNextFrom].next;
	}


	n=(mouseX/TILE_WIDTH)+(mouseY/TILE_HEIGHT)*20;

	tileNum=n;
	if(tileNum>199)
		tileNum=199;
	tileNum+=tileStart;

	switch(mode)
	{
		case TMODE_SELECT:
			if(mgl->MouseDown())
			{
				// still holding
				if(n>199)
					n=199;	// can't go beyond the end

				n=tileStart+n;

				if(n<selStart)
				{
					selMax=selStart;
					selMin=n;
				}
				else
				{
					selMin=selStart;
					selMax=n;
				}
				if(selMin>=world->numTiles)
					selMin=world->numTiles-1;
				if(selMax>=world->numTiles)
					selMax=world->numTiles-1;
			}
			else
			{
				// done selecting
				mode=TMODE_NORMAL;
			}
			TerrainSetFlags();
			break;
		case TMODE_NORMAL:
			if(mgl->MouseTap())
			{
				if(n>199)	// clicked outside the display
				{
					CheckButtons(mouseX,mouseY);
				}
				else
				{
					// begin selecting
					MakeNormalSound(SND_MENUCLICK);
					selStart=n+tileStart;
					selMin=selStart;
					selMax=selStart;
					mode=TMODE_SELECT;
				}
			}
			break;
		case TMODE_PICKNEXT:
			if(mgl->MouseTap())
			{
				if(n>199)
					CheckButton(mouseX,mouseY,ID_NEXTPAGE);
				else
				{
					mode=TMODE_NORMAL;
					MakeNormalSound(SND_MENUSELECT);
					SetNextTile(n+tileStart);
					TerrainSetFlags();
				}
			}
			break;
		case TMODE_PICKDEST:
			if(mgl->MouseTap())
			{
				if(n>199)
					CheckButton(mouseX,mouseY,ID_NEXTPAGE);
				else
				{
					if(MoveTilesTo(n+tileStart))
						mode=TMODE_NORMAL;
				}
			}
			break;
		case TMODE_LOADBMP:
			if(mgl->MouseTap())
			{
				n=FileDialogClick(mouseX,mouseY);
				if(n==FM_LOAD)
				{
					ExitFileDialog();
					InitBMPPick();
				}
				if(n==FM_EXIT)
				{
					mode=TMODE_NORMAL;
				}
			}
			break;
		case TMODE_SAVEBMP:
			if(mgl->MouseTap())
			{
				n=FileDialogClick(mouseX,mouseY);
				if(n==FM_SAVE)
				{
					ExitFileDialog();
					SaveTilesToBMP(GetFilename("tilegfx/"));
					mode=TMODE_NORMAL;
				}
				if(n==FM_EXIT)
				{
					mode=TMODE_NORMAL;
				}
			}
			break;
		case TMODE_PICKBMPSEL:
			if(noCanPickMsgTime)
				noCanPickMsgTime--;
			if(mgl->MouseDown())
			{
				if(n<selStart)
				{
					selMax=selStart;
					selMin=n;
				}
				else
				{
					selMin=selStart;
					selMax=n;
				}
			}
			else
			{
				// done selecting
				mode=TMODE_PICKBMP;
				ImportTiles();
				if(noCanPickMsgTime==0)	// no problems occurred on import
					MakeNormalSound(SND_TELEPORT);
			}
			break;
		case TMODE_PICKBMP:
			if(noCanPickMsgTime)
				noCanPickMsgTime--;
			if(mgl->MouseTap())
			{
				// begin selecting
				MakeNormalSound(SND_MENUCLICK);
				selStart=n;
				selMin=selStart;
				selMax=selStart;
				mode=TMODE_PICKBMPSEL;
			}
			break;
		case TMODE_PICKTILE:
			if(n<200)
			{
				selMin=n+tileStart;
				selMax=n+tileStart;
			}
			TerrainSetFlags();
			if(mgl->MouseTap())
			{
				if(n>199)
				{
					CheckButton(mouseX,mouseY,ID_EXIT);
					CheckButton(mouseX,mouseY,ID_NEXTPAGE);
				}
				else if(n+tileStart<world->numTiles)
				{
					PickedTile(n+tileStart);
					MakeNormalSound(SND_MENUSELECT);
					SetEditMode(rememberMode);
					saveTileStart=tileStart;
				}
			}
			break;
		case TMODE_HELP:
			if(mgl->MouseTap())
			{
				if(EditHelpClick(mouseX,mouseY))
					mode=helpRemember;
			}
			break;
	}

	dottedLineOfs++;
	if(dottedLineOfs>7)
		dottedLineOfs=0;
}

void TerrainEdit_Key(char k)
{
	switch(mode)
	{
		case TMODE_SELECT:
		case TMODE_NORMAL:
		case TMODE_PICKTILE:
			if(k==27)
			{
				if(mode==TMODE_PICKTILE)
					PickedTile(-1);
				SetEditMode(EDITMODE_EDIT);
				saveTileStart=tileStart;
				return;
			}
			break;
		case TMODE_PICKNEXT:
			if(k==27)
			{
				mode=TMODE_NORMAL;
			}
			break;
		case TMODE_PICKDEST:
			if(k==27)
				mode=TMODE_NORMAL;
			break;
		case TMODE_LOADBMP:
		case TMODE_SAVEBMP:
			if(!FileDialogKey(k))
			{
				ExitFileDialog();
				mode=TMODE_NORMAL;
			}
			break;
		case TMODE_PICKBMP:
		case TMODE_PICKBMPSEL:
			if(k==27)
			{
				ExitBMPPick();
				mode=TMODE_NORMAL;
			}
			break;
		case TMODE_HELP:
			if(!EditHelpKey(k))
				mode=helpRemember;
			break;

	}
}

// --------------------- RENDER FUNCTIONS

void TerrainEdit_RenderPick(int mouseX,int mouseY,MGLDraw *mgl)
{
	int i,j;
	int pitch;
	byte *scr;

	scr=mgl->GetScreen();
	pitch=mgl->GetWidth();

	for(i=0;i<480;i++)
		memcpy(&scr[i*pitch],&bmpScr[i*640],640);

	for(i=0;i<20;i++)
	{
		for(j=0;j<20;j++)
		{
			if(i+j*20>=selMin && i+j*20<=selMax)
			{
				// make dotted lines around the selection
				if(j==0 || (i+(j-1)*20<selMin || i+(j-1)*20>selMax))
					mgl->SelectLineH(i*TILE_WIDTH,i*TILE_WIDTH+TILE_WIDTH,j*TILE_HEIGHT,dottedLineOfs);
				if(j==19 || (i+(j+1)*20<selMin || i+(j+1)*20>selMax))
					mgl->SelectLineH(i*TILE_WIDTH,i*TILE_WIDTH+TILE_WIDTH,j*TILE_HEIGHT+TILE_HEIGHT-1,dottedLineOfs);
				if(i+j*20==selMin || i==0)
					mgl->SelectLineV(i*TILE_WIDTH,j*TILE_HEIGHT,j*TILE_HEIGHT+TILE_HEIGHT,dottedLineOfs);
				if(i+j*20==selMax || i==19)
					mgl->SelectLineV(i*TILE_WIDTH+TILE_WIDTH-1,j*TILE_HEIGHT,j*TILE_HEIGHT+TILE_HEIGHT,dottedLineOfs);
			}
		}
	}

	if(mouseY<400)
	{
		if(!noCanPickMsgTime)
		{
			Print(2+1,460+1,"Drag to select a group of tiles to import.  Press ESC when done!",-32,1);
			Print(2,460,"Drag to select a group of tiles to import.  Press ESC when done!",0,1);
		}
		else
		{
			Print(2+1,460+1,"Maximum tile limit exceeded!!  No more tiles can be imported.",-32,1);
			Print(2,460,"Maximum tile limit exceeded!!  No more tiles can be imported.",0,1);
		}
	}
	else
	{
		if(!noCanPickMsgTime)
		{
			Print(2+1,2+1,"Drag to select a group of tiles to import.  Press ESC when done!",-32,1);
			Print(2,2,"Drag to select a group of tiles to import.  Press ESC when done!",0,1);
		}
		else
		{
			Print(2+1,2+1,"Maximum tile limit exceeded!!  No more tiles can be imported.",-32,1);
			Print(2,2,"Maximum tile limit exceeded!!  No more tiles can be imported.",0,1);
		}
	}
}

void TerrainEdit_Render(int mouseX,int mouseY,MGLDraw *mgl)
{
	int i,j;
	char txt[16];

	if(mode==TMODE_PICKBMP || mode==TMODE_PICKBMPSEL)
	{
		TerrainEdit_RenderPick(mouseX,mouseY,mgl);
		return;
	}

	mgl->FillBox(0,240,639,479,0);

	sprintf(txt,"Tile #%03d",tileNum);
	Print(570,262,txt,0,1);

	for(i=0;i<20;i++)
		for(j=0;j<10;j++)
		{
			if(PointInRect(mouseX,mouseY,i*TILE_WIDTH,j*TILE_HEIGHT,i*TILE_WIDTH+TILE_WIDTH-1,j*TILE_HEIGHT+TILE_HEIGHT-1))
				RenderFloorTile(i*TILE_WIDTH,j*TILE_HEIGHT,i+j*20+tileStart,16);
			else
				RenderFloorTileUnlit(i*TILE_WIDTH,j*TILE_HEIGHT,i+j*20+tileStart);

			if(i+j*20+tileStart>=selMin && i+j*20+tileStart<=selMax)
			{
				// make dotted lines around the selection
				if(j==0 || (i+(j-1)*20+tileStart<selMin || i+(j-1)*20+tileStart>selMax))
					mgl->SelectLineH(i*TILE_WIDTH,i*TILE_WIDTH+TILE_WIDTH,j*TILE_HEIGHT,dottedLineOfs);
				if(j==9 || (i+(j+1)*20+tileStart<selMin || i+(j+1)*20+tileStart>selMax))
					mgl->SelectLineH(i*TILE_WIDTH,i*TILE_WIDTH+TILE_WIDTH,j*TILE_HEIGHT+TILE_HEIGHT-1,dottedLineOfs);
				if(i+j*20+tileStart==selMin || i==0)
					mgl->SelectLineV(i*TILE_WIDTH,j*TILE_HEIGHT,j*TILE_HEIGHT+TILE_HEIGHT,dottedLineOfs);
				if(i+j*20+tileStart==selMax || i==19)
					mgl->SelectLineV(i*TILE_WIDTH+TILE_WIDTH-1,j*TILE_HEIGHT,j*TILE_HEIGHT+TILE_HEIGHT,dottedLineOfs);
			}
		}

	switch(mode)
	{
		case TMODE_NORMAL:
		case TMODE_SELECT:
		case TMODE_PICKTILE:
			RenderButtons(mouseX,mouseY,mgl);
			RenderFloorTileUnlit(202,247,nextTileShow);
			break;
		case TMODE_PICKNEXT:
			RenderButton(mouseX,mouseY,ID_NEXTPAGE,mgl);
			Print(4,245,"Select a tile to assign as Next Tile.",0,1);
			break;
		case TMODE_PICKDEST:
			RenderButton(mouseX,mouseY,ID_NEXTPAGE,mgl);
			Print(4,245,"Select a destination to move tiles to.",0,1);
			break;
		case TMODE_LOADBMP:
		case TMODE_SAVEBMP:
			RenderFileDialog(mouseX,mouseY,mgl);
			break;
		case TMODE_HELP:
			RenderButtons(-1,-1,mgl);
			RenderEditHelp(mouseX,mouseY,mgl);
			break;
	}
}

void TerrainEdit_Help(void)
{
	if(mode==TMODE_HELP)
		return;

	helpRemember=mode;
	if(mode==TMODE_PICKTILE)
		InitEditHelp(HELP_TERRAINPICK);
	else
		InitEditHelp(HELP_TERRAINEDIT);
	mode=TMODE_HELP;
}

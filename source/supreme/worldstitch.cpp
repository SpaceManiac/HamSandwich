#include "worldstitch.h"
#include "editor.h"

world_t *world2;
int stitchTileOffset,stitchSoundOffset,stitchItemOffset;
char stitchError[64];

void SetStitchError(const char *txt)
{
	strcpy(stitchError,txt);
}

const char *GetStitchError(void)
{
	return stitchError;
}

void UpdateTiles(world_t *world)
{
	int lvl,i,j;

	for(i=0;i<world->numTiles;i++)
	{
		world->terrain[i].next+=stitchTileOffset;
	}

	for(lvl=0;lvl<world->numMaps;lvl++)
	{
		for(i=0;i<world->map[lvl]->width*world->map[lvl]->height;i++)
		{
			world->map[lvl]->map[i].floor+=stitchTileOffset;
			if(world->map[lvl]->map[i].wall)
				world->map[lvl]->map[i].wall+=stitchTileOffset;
		}

		for(i=0;i<MAX_SPECIAL;i++)
		{
			if(world->map[lvl]->special[i].x!=255)
			{
				for(j=0;j<NUM_TRIGGERS;j++)
				{
					if(world->map[lvl]->special[i].trigger[j].type==TRG_FLOOR)
					{
						world->map[lvl]->special[i].trigger[j].value+=stitchTileOffset;
					}
					else if(world->map[lvl]->special[i].trigger[j].type==TRG_FLOORRECT)
					{
						world->map[lvl]->special[i].trigger[j].value+=stitchTileOffset;
					}
					else if(world->map[lvl]->special[i].trigger[j].type==TRG_STEPTILE)
					{
						world->map[lvl]->special[i].trigger[j].value2+=stitchTileOffset;
					}
				}
				for(j=0;j<NUM_EFFECTS;j++)
				{
					if(world->map[lvl]->special[i].effect[j].type==EFF_CHANGETILE)
					{
						world->map[lvl]->special[i].effect[j].value+=stitchTileOffset;
						if(world->map[lvl]->special[i].effect[j].value2!=0)
							world->map[lvl]->special[i].effect[j].value2+=stitchTileOffset;
					}
					else if(world->map[lvl]->special[i].effect[j].type==EFF_OLDTOGGLE)
					{
						world->map[lvl]->special[i].effect[j].value+=stitchTileOffset;
						if(world->map[lvl]->special[i].effect[j].value2!=0)
							world->map[lvl]->special[i].effect[j].value2+=stitchTileOffset;
					}
					else if(world->map[lvl]->special[i].trigger[j].type==EFF_TILEVAR)
					{
						SetStitchError("Warning: TileVar used.");
					}
				}
			}
		}
	}
}

void UpdateItems(world_t *world)
{
	int lvl,i,j;

	for(i=stitchItemOffset;i<NumItems();i++)
	{
		if(GetItem(i)->effect==IE_BECOME)
		{
			if(GetItem(i)->effectAmt>=NUM_ORIGINAL_ITEMS)
				GetItem(i)->effectAmt+=stitchItemOffset-NUM_ORIGINAL_ITEMS;
		}
		if(GetItem(i)->sound>=CUSTOM_SND_START)
			GetItem(i)->sound+=stitchSoundOffset;
		if(GetItem(i)->flags&IF_TILE)
			GetItem(i)->sprNum+=stitchTileOffset;
	}

	for(lvl=0;lvl<world->numMaps;lvl++)
	{
		for(i=0;i<world->map[lvl]->width*world->map[lvl]->height;i++)
		{
			if(world->map[lvl]->map[i].item>=NUM_ORIGINAL_ITEMS)
				world->map[lvl]->map[i].item+=stitchItemOffset-NUM_ORIGINAL_ITEMS;
		}
		for(i=0;i<MAX_MAPMONS;i++)
		{
			if(world->map[lvl]->badguy[i].item>=NUM_ORIGINAL_ITEMS && world->map[lvl]->badguy[i].item!=ITM_RANDOM)
			{
				world->map[lvl]->badguy[i].item+=stitchItemOffset-NUM_ORIGINAL_ITEMS;
				GetItem(world->map[lvl]->badguy[i].item);
			}
		}
		for(i=0;i<MAX_SPECIAL;i++)
		{
			if(world->map[lvl]->special[i].x!=255)
			{
				for(j=0;j<NUM_TRIGGERS;j++)
				{
					if(world->map[lvl]->special[i].trigger[j].type==TRG_HAVEITEM)
					{
						if(world->map[lvl]->special[i].trigger[j].value>=NUM_ORIGINAL_ITEMS)
							world->map[lvl]->special[i].trigger[j].value+=stitchItemOffset-NUM_ORIGINAL_ITEMS;
					}
					else if(world->map[lvl]->special[i].trigger[j].type==TRG_ITEM)
					{
						if(world->map[lvl]->special[i].trigger[j].value>=NUM_ORIGINAL_ITEMS)
							world->map[lvl]->special[i].trigger[j].value+=stitchItemOffset-NUM_ORIGINAL_ITEMS;
					}
					else if(world->map[lvl]->special[i].trigger[j].type==TRG_ITEMS)
					{
						if(world->map[lvl]->special[i].trigger[j].value>=NUM_ORIGINAL_ITEMS)
							world->map[lvl]->special[i].trigger[j].value+=stitchItemOffset-NUM_ORIGINAL_ITEMS;
					}
					else if(world->map[lvl]->special[i].trigger[j].type==TRG_ITEMRECT)
					{
						if(world->map[lvl]->special[i].trigger[j].value>=NUM_ORIGINAL_ITEMS)
							world->map[lvl]->special[i].trigger[j].value+=stitchItemOffset-NUM_ORIGINAL_ITEMS;
					}
				}
				for(j=0;j<NUM_EFFECTS;j++)
				{
					if(world->map[lvl]->special[i].effect[j].type==EFF_ITEM)
					{
						if(world->map[lvl]->special[i].effect[j].value!=ITM_RANDOM &&
							world->map[lvl]->special[i].effect[j].value>=NUM_ORIGINAL_ITEMS)
							world->map[lvl]->special[i].effect[j].value+=stitchItemOffset-NUM_ORIGINAL_ITEMS;
					}
					else if(world->map[lvl]->special[i].effect[j].type==EFF_SUMMON)
					{
						if(world->map[lvl]->special[i].effect[j].value2!=ITM_RANDOM &&
							world->map[lvl]->special[i].effect[j].value2>=NUM_ORIGINAL_ITEMS)
							world->map[lvl]->special[i].effect[j].value2+=stitchItemOffset-NUM_ORIGINAL_ITEMS;
					}
					else if(world->map[lvl]->special[i].effect[j].type==EFF_MONSITEM)
					{
						if(world->map[lvl]->special[i].effect[j].value2!=ITM_RANDOM &&
							world->map[lvl]->special[i].effect[j].value2>=NUM_ORIGINAL_ITEMS)
							world->map[lvl]->special[i].effect[j].value2+=stitchItemOffset-NUM_ORIGINAL_ITEMS;
					}
				}
			}
		}
	}
}

void UpdateSpecials(world_t *world,int levelOffset)
{
	int lvl,i,j;

	for(lvl=0;lvl<world->numMaps;lvl++)
	{
		for(i=0;i<MAX_SPECIAL;i++)
		{
			if(world->map[lvl]->special[i].x!=255)
			{
				for(j=0;j<NUM_TRIGGERS;j++)
				{
					if(world->map[lvl]->special[i].trigger[j].type==TRG_PASSLEVEL)
					{
						world->map[lvl]->special[i].trigger[j].value+=levelOffset;
					}
				}
				for(j=0;j<NUM_EFFECTS;j++)
				{
					if(world->map[lvl]->special[i].effect[j].type==EFF_WINLEVEL)
					{
						world->map[lvl]->special[i].effect[j].value+=levelOffset;
					}
					else if(world->map[lvl]->special[i].effect[j].type==EFF_GOTOMAP)
					{
						world->map[lvl]->special[i].effect[j].value+=levelOffset;
					}
					else if(world->map[lvl]->special[i].effect[j].type==EFF_SOUND)
					{
						if(world->map[lvl]->special[i].effect[j].value>=CUSTOM_SND_START)
							world->map[lvl]->special[i].effect[j].value+=stitchSoundOffset;
					}
				}
			}
		}
	}
}

byte AddWorldIn(world_t *world1,const char *fname)
{
	int i;

	EditorSaveWorld("worlds/backup_load.hsw");
	stitchTileOffset=0;
	stitchSoundOffset=0;
	stitchItemOffset=0;

	SetStitchError("No problems!  Append OK!");
	world2=(world_t *)malloc(sizeof(world_t));

	if(!BeginAppendWorld(world2,fname))
	{
		FreeWorld(world1);
		LoadWorld(world1,"worlds/backup_load.hsw");
		EditorSelectMap(0);
		free(world2);
		return 0;
	}
	if(world2->numMaps+world1->numMaps>MAX_MAPS)	// would be too many maps
	{
		FreeWorld(world1);
		LoadWorld(world1,"worlds/backup_load.hsw");
		EditorSelectMap(0);
		free(world2);
		SetStitchError("Too many levels!");
		return 0;
	}

	UpdateTiles(world2);
	UpdateItems(world2);
	UpdateSpecials(world2,world1->numMaps);

	for(i=0;i<world2->numMaps;i++)	// now copy the maps over
		world1->map[i+world1->numMaps]=world2->map[i];

	world1->numMaps+=world2->numMaps;
	for(i=0;i<world2->numTiles;i++)
		world1->terrain[stitchTileOffset+i]=world2->terrain[i];
	world1->numTiles+=world2->numTiles;

	free(world2);
	EditorSelectMap(0);
	return 1;
}

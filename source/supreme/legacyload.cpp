#include "winpch.h"
#include "legacyload.h"
#include "map.h"
#include "world.h"
#include "monster.h"
#include "sound.h"
#include "appdata.h"

constexpr int OLD_MAX_MAPMONS = 128;
constexpr int OLD_MAX_SPECIAL = 32;
constexpr int OLD_MAX_MAPS = 24;

struct old_special_t
{
	word trigger;
	byte trigValue;
	byte effect;
	byte x,y;
	byte effectX,effectY;
	int value;
	char msg[32];
};

struct old_mapTile_t
{
	byte floor;
	byte wall;
	byte item;
	char light;
	char templight;
	byte opaque;
};

struct old_mapBadguy_t
{
	byte x,y;
	byte type;
};

struct old_map_t
{
	int width,height;
	old_mapTile_t *map;
	char name[32];
	byte song;
	byte flags;
	old_mapBadguy_t badguy[OLD_MAX_MAPMONS];
	old_special_t   special[OLD_MAX_SPECIAL];
};

struct old_terrain_t
{
	word flags;
	byte next;
};

struct old_world_t
{
	byte numMaps;
	int  totalPoints;
	old_map_t *map[OLD_MAX_MAPS];
	old_terrain_t terrain[200];
};

static old_world_t *oldWorld;

static const char songTab[23][32]={
	"",	// 1=no song
	"002title.ogg",
	"003worldpicker.ogg",
	"004CavernHub.ogg",
	"005CavernLevel.ogg",
	"006cavesboss.ogg",
	"007ForestHub.ogg",
	"008forestlevel.ogg",
	"009forestboss.ogg",
	"010IcyHub.ogg",
	"011IcyLevel.ogg",
	"012icyboss.ogg",
	"013DesertHub.ogg",
	"014DesertLevel.ogg",
	"015desertboss.ogg",
	"016AsylumHub.ogg",
	"017asylumlevel.ogg",
	"018AsylumBossLevel.ogg",
	"019victory.ogg",
	"020Pumpkin.ogg",
	"021secretlevel.ogg",
	"022EndingCreditsTheme.ogg",
	"023asylumbigboss.ogg"
};


const char *GetOriginalSongName(byte song)
{
	return songTab[song];
}

void ConvertToNewWorld(world_t *world)
{
	int i;

	world->numMaps=oldWorld->numMaps;
	strcpy(world->author,"Author Unknown!");
	world->totalPoints=0;
	world->numTiles=400;
	memset(world->terrain,0,sizeof(terrain_t)*400);
	for(i=0;i<200;i++)
	{
		world->terrain[i].flags=oldWorld->terrain[i].flags;
		world->terrain[i].next=(word)oldWorld->terrain[i].next;
	}

	for(i=0;i<MAX_MAPS;i++)
		world->map[i]=NULL;

	for(i=0;i<world->numMaps;i++)
	{
		world->map[i]=new Map(oldWorld->map[i]);
	}
	world->map[0]->flags|=MAP_HUB;
}

void LoadOldMap(old_map_t *map,FILE *f)
{
	fread(&map->width,1,sizeof(int),f);
	fread(&map->height,1,sizeof(int),f);

	fread(map->name,32,sizeof(char),f);
	fread(map->badguy,OLD_MAX_MAPMONS,sizeof(old_mapBadguy_t),f);
	fread(map->special,OLD_MAX_SPECIAL,sizeof(old_special_t),f);
	fread(&map->song,1,1,f);
	fread(&map->flags,1,1,f);

	map->map=(old_mapTile_t *)calloc(sizeof(old_mapTile_t)*map->width*map->height,1);

	fread(map->map,map->width*map->height,sizeof(old_mapTile_t),f);
}

byte Legacy_LoadWorld(world_t *world,const char *fname)
{
	FILE *f;
	int i;

	oldWorld=new old_world_t;

	f=AssetOpen(fname);
	if(!f)
		return 0;

	fread(&oldWorld->numMaps,1,1,f);
	fread(&oldWorld->totalPoints,1,4,f);

	LoadOldTiles(f);

	fread(oldWorld->terrain,200,sizeof(old_terrain_t),f);

	for(i=0;i<24;i++)
		oldWorld->map[i]=NULL;

	for(i=0;i<oldWorld->numMaps;i++)
	{
		oldWorld->map[i]=new old_map_t;
		if(!oldWorld->map[i])
			return 0;
		else
			LoadOldMap(oldWorld->map[i],f);
	}

	fclose(f);

	ConvertToNewWorld(world);
	for(i=0;i<oldWorld->numMaps;i++)
	{
		if(oldWorld->map[i]->map)
			free(oldWorld->map[i]->map);
		delete oldWorld->map[i];
	}

	delete oldWorld;
	ExitItems();
	InitItems();	// reset to base item list
	return 1;
}

void AddTrigger(word flag,byte trigValue,special_t *me,char *msg,byte orIt)
{
	int i;

	for(i=0;i<NUM_TRIGGERS;i++)
	{
		if(!me->trigger[i].type)
		{
			me->trigger[i].x=me->x;
			me->trigger[i].y=me->y;
			me->trigger[i].flags=0;
			me->trigger[i].value=0;
			me->trigger[i].value2=0;
			if(i>0)
			{
				if(!orIt)
					me->trigger[i-1].flags|=TF_AND;
			}
			switch(flag)
			{
				case 1:	// TRG_STEP
					me->trigger[i].type=TRG_STEP;
					me->trigger[i].value=MONS_BOUAPHA;
					me->trigger[i].value2=0;
					break;
				case 2:	// TRG_ENEMYSTEP
					me->trigger[i].type=TRG_STEP;
					if(trigValue!=0)
						me->trigger[i].value=trigValue;
					else
						me->trigger[i].value=MONS_NONPLAYER;
					me->trigger[i].value2=0;
					break;
				case 4:	// TRG_NEAR
					me->trigger[i].type=TRG_STEP;
					me->trigger[i].value=MONS_BOUAPHA;
					me->trigger[i].value2=trigValue;
					break;
				case 8:	// TRG_PASSLEVELS
					me->trigger[i].type=TRG_PASSLEVELS;
					me->trigger[i].value=trigValue;
					me->trigger[i].flags|=TF_MORE;
					break;
				case 16:	// TRG_KEYCHAINS
					me->trigger[i].type=TRG_HAVEITEM;
					me->trigger[i].value=ITM_KEYCH1;
					me->trigger[i].value2=4;
					break;
				case 32:	// TRG_KILLALL
					me->trigger[i].type=TRG_MONSTER;
					me->trigger[i].value=trigValue;
					me->trigger[i].value2=0;
					break;
				case 64:	// TRG_HAVEBRAINS
					me->trigger[i].type=TRG_HAVEITEM;
					me->trigger[i].value=ITM_BRAIN;
					me->trigger[i].value2=trigValue;
					me->trigger[i].flags|=TF_MORE;
					break;
				case 128:	// TRG_SHOOT
					me->trigger[i].type=TRG_SHOOT;
					break;
				case 256:	// repeatable
					me->uses=0;
					break;
				case 512:	// message
					me->effect[1].type=EFF_MESSAGE;
					me->effect[1].flags=0;
					strcpy(me->effect[1].text,msg);
					break;
				case 1024:	// TRG_CHAIN
					me->trigger[i].type=TRG_CHAIN;
					me->trigger[i].x=255;
					me->trigger[i].y=255;	// special code to indicate chain off of any neighbor
					break;
				case 2048:	// TRG_TIMED
					me->trigger[i].type=TRG_TIMED;
					me->trigger[i].value=trigValue*30;
					if(msg[0]=='@' && i<NUM_TRIGGERS-1)	// must delay it
					{
						me->trigger[i].value2=atoi(&msg[1])*30;
					}
					else
						me->trigger[i].value2=0;
					break;
				case 4096:	// TRG_RANDOM
					me->trigger[i].type=TRG_RANDOM;
					me->trigger[i].value=trigValue*FIXAMT;
					if(i<NUM_TRIGGERS-1)	// must delay it
					{
						i++;
						me->trigger[i].type=TRG_TIMED;
						me->trigger[i].value=30;	// once per second only
						me->trigger[i-1].flags=TF_AND;
					}
					break;
				case 8192:	// TRG_HAVELOONY
					me->trigger[i].type=TRG_HAVEITEM;
					me->trigger[i].value=ITM_LOONYKEY;
					me->trigger[i].value2=1;
					break;
				case 16384:	// TRG_KILLONE
					me->trigger[i].type=TRG_KILL;
					me->trigger[i].value=trigValue;
					break;
				case 32768:	// TRG_FLOORHERE
					me->trigger[i].type=TRG_FLOOR;
					me->trigger[i].value=trigValue;
					break;
			}
			break;
		}
	}

}

void ParseCoords(char *txt,byte *x,byte *y,byte *x2,byte *y2)
{
	byte fail;
	char *tok;

	fail=0;
	tok=strtok(txt,", ");
	if(tok)
	{
		*x=atoi(tok);
		tok=strtok(NULL,"- ");
		if(tok)
		{
			*y=atoi(tok);
			tok=strtok(NULL,", ");
			if(tok)
			{
				*x2=atoi(tok);
				tok=strtok(NULL," ");
				if(tok)
					*y2=atoi(tok);
				else fail=1;
			}
			else fail=1;
		}
		else fail=1;
	}
	else fail=1;

	if(fail)
	{
		*x=0;
		*y=0;
		*x2=0;
		*y2=0;
	}
}

int HandleLegacyCustomSound(int value)
{
	char name[64],shrtname[32];
	int n;

	if(value<222 || (value>255 && value<274))
		return value;	// not a custom sound, a built in one

	sprintf(name,"user/snd%03d.wav",value);
	sprintf(shrtname,"custom%03d",value);

	n=GetCustomSoundByName(shrtname);
	if(n==-1)	// this custom sound doesn't exist yet
	{
		if(!AddCustomSound(name))
		{
			return 0;	// and doesn't exist as a file either, so just substitute the click noise
		}
		else
		{
			strcpy(GetSoundInfo(GetNumCustomSounds()+CUSTOM_SND_START-1)->name,shrtname);
			return (GetNumCustomSounds()+CUSTOM_SND_START-1);
		}
	}
	else	// it does exist, and N is it's number
		return n;
}

void SpecialConvert(old_special_t *old,special_t *me,Map *map)
{
	int i;
	byte b,c;
	word trigList[]={
		// first the instant triggers, which will be OR'd
		1,2,4,128,1024,16384,
		// then the general triggers, which will be AND'd
		8,16,32,64,2048,4096,8192,32768,
		// then the extraneous stuff (message and repeat)
		512,256};

	for(i=0;i<NUM_TRIGGERS;i++)
	{
		me->trigger[i].x=0;
		me->trigger[i].y=0;
		me->trigger[i].type=TRG_NONE;
		me->trigger[i].flags=0;
		me->trigger[i].value=0;
		me->trigger[i].value2=0;
	}
	for(i=0;i<NUM_EFFECTS;i++)
	{
		me->effect[i].x=0;
		me->effect[i].y=0;
		me->effect[i].type=EFF_NONE;
		me->effect[i].flags=EFF_NONE;
		me->effect[i].value=EFF_NONE;
		me->effect[i].value2=EFF_NONE;
	}

	me->uses=1;
	me->x=old->x;
	me->y=old->y;
	if(old->trigger==0)
	{
		me->x=255;
		return;
	}

	for(i=0;i<16;i++)
	{
		if(old->trigger&trigList[i])
			AddTrigger(trigList[i],old->trigValue,me,old->msg,(i<6));
	}
	if (!me->trigger[0].type)
	{
		// In Dr. L, triggers are negative logic such that a special with NO
		// triggers is one that ALWAYS triggers. We need to preserve that.
		// Example: Jubilee.dlw #05
		me->trigger[0].type = TRG_DELAY;
	}

	me->effect[0].x=old->effectX;
	me->effect[0].y=old->effectY;
	me->effect[0].flags=0;
	me->effect[0].type=EFF_NONE;
	me->effect[0].value=0;
	me->effect[0].value2=0;
	switch(old->effect)
	{
		case 1:	// summon
			me->effect[0].type=EFF_SUMMON;
			me->effect[0].value=old->value;
			me->effect[0].value2=ITM_RANDOM;
			break;
		case 2:	// zapwall
			me->effect[0].type=EFF_CHANGETILE;
			me->effect[0].flags=EF_CONTIGUOUS;
			me->effect[0].value=old->value;
			me->effect[0].value2=0;
			break;
		case 3:	//SPC_RAISEWALL	3
			me->effect[0].type=EFF_CHANGETILE;
			me->effect[0].flags=EF_CONTIGUOUS;
			me->effect[0].value=map->map[old->effectX+old->effectY*map->width].floor;
			me->effect[0].value2=old->value+199;
			break;
		case 4: // SPC_TOGGLEWALL	4
			me->effect[0].type=EFF_OLDTOGGLE;
			if(map->map[old->effectX+old->effectY*map->width].wall)
			{
				me->effect[0].value=old->value;
				me->effect[0].value2=map->map[old->effectX+old->effectY*map->width].wall;
			}
			else
			{
				me->effect[0].value=map->map[old->effectX+old->effectY*map->width].floor;
				me->effect[0].value2=old->value+199;
			}
			break;
		case 5: //SPC_TELEPORT	5
			me->effect[0].type=EFF_TELEPORT;
			break;
		case 6: //SPC_LIGHT		6
			me->effect[0].type=EFF_LIGHT;
			me->effect[0].value=old->value;
			me->effect[0].value2=abs(old->value/2);
			me->effect[0].flags|=EF_PERMLIGHT;
			break;
		case 7: //SPC_GOTOMAP		7
			me->effect[0].type=EFF_GOTOMAP;
			me->effect[0].x=255;	// auto-position
			me->effect[0].value=old->value;
			if(oldWorld->numMaps<=old->value)
				me->effect[0].value=0;
			break;
		case 8:	//SPC_EXIT		8
			me->effect[0].type=EFF_WINLEVEL;
			me->effect[0].x=255;	// auto-position
			me->effect[0].value=0;
			break;
		case 9:	//SPC_PICTURE		9
			me->effect[0].type=EFF_PICTURE;
			strcpy(me->effect[0].text,old->msg);
			me->effect[0].value=0;
			break;
		case 10:	//SPC_PLAYSONG	10
			me->effect[0].type=EFF_SONG;
			me->effect[0].value=old->value;
			break;
		case 11:	//SPC_PLAYSOUND	11
			me->effect[0].type=EFF_SOUND;
			me->effect[0].value=HandleLegacyCustomSound(old->value);
			break;
		case 12:	//SPC_DROPITEM	12
			me->effect[0].type=EFF_ITEM;
			me->effect[0].value=old->value;
			break;
		case 13:	//SPC_TEMPLIGHT	13
			me->effect[0].type=EFF_LIGHT;
			me->effect[0].value=old->value;
			me->effect[0].value2=abs(old->value/2);
			break;
		case 14:	//SPC_SWAPMAP		14
			me->effect[0].type=EFF_SWAPMAP;
			me->effect[0].value2=old->effectX+old->effectY*256;
			ParseCoords(old->msg,&me->effect[0].x,&me->effect[0].y,&b,&c);
			me->effect[0].value=b+c*256;
			me->effect[0].flags|=EF_TOGGLE;
			break;
		case 15:	//SPC_CHGTILE		15
			me->effect[0].type=EFF_CHANGETILE;
			me->effect[0].flags|=EF_CONTIGUOUS;
			me->effect[0].value=old->value;
			me->effect[0].value2=map->map[old->effectX+old->effectY*map->width].wall;
			break;
		case 16:	//SPC_PLAYSOUND2	16
			me->effect[0].type=EFF_SOUND;
			me->effect[0].value=HandleLegacyCustomSound(old->value+256);
			break;
		case 17:	//SPC_WINANDGO	17
			me->effect[0].type=EFF_WINLEVEL;
			me->effect[0].value=old->value;
			me->effect[0].x=255;	// auto-position
			break;
		case 18:	//SPC_COPYMAP		18
			me->effect[0].type=EFF_SWAPMAP;
			me->effect[0].value2=old->effectX+old->effectY*256;
			ParseCoords(old->msg,&me->effect[0].x,&me->effect[0].y,&b,&c);
			me->effect[0].value=b+c*256;
			break;
		case 19:	//SPC_KILLMONS	19
			me->effect[0].type=EFF_KILLMONS;
			if(old->effectX==0 && old->effectY==0)	// non-specific location
			{
				me->effect[0].value=old->value;
				me->effect[0].x=255;	// anywhere
			}
			else
			{
				me->effect[0].value=MONS_ANYBODY;	// kill anybody at that spot
			}
			break;
		case 20:	//SPC_CHGMONS		20
			me->effect[0].type=EFF_CHANGEMONS;
			if(old->effectX==0 && old->effectY==0)	// non-specific location
			{
				me->effect[0].value=atoi(old->msg);
				me->effect[0].value2=old->value;
				me->effect[0].x=255;	// anywhere
			}
			else
			{
				me->effect[0].value=MONS_ANYBODY;	// turn anybody at that spot
				me->effect[0].value2=old->value;	// into that
			}
			break;
		case 21:	//SPC_RMVSPCL		21
			me->effect[0].type=EFF_DELETESPCL;
			break;
		case 22:	//SPC_TOGGLEITEM	22
			me->effect[0].type=EFF_ITEM;
			me->effect[0].x=old->effectX;
			me->effect[0].y=old->effectY;
			me->effect[0].value=old->value;
			me->effect[0].flags|=EF_TOGGLE;
			break;
	}

	if(me->effect[0].type==EFF_NONE)
	{
		me->effect[0]=me->effect[1];
		me->effect[1].type=EFF_NONE;
	}
}

Map::Map(old_map_t *old)
{
	int i;

	width=old->width;
	height=old->height;
	strcpy(name,old->name);
	if(old->song>=2 && old->song<=23)
		strcpy(song,songTab[old->song-1]);
	else
		strcpy(song,"");

	flags=0;
	if(old->flags&1)	// snowing
		flags|=MAP_SNOWING;
	if(old->flags&2)	// many items
	{
		itemDrops=FIXAMT*25;	// 25%
	}
	else
		itemDrops=FIXAMT*5;		// 5% normally

	if(old->flags&4)	// secret level
		flags|=MAP_SECRET;
	if(old->flags&8)	// torch lit
		flags|=MAP_TORCHLIT;
	if(old->flags&16)	// starry background
		flags|=MAP_STARS;

	numBrains=0;
	numCandles=0;

	for(i=0;i<MAX_MAPMONS;i++)
		badguy[i].type=MONS_NONE;
	for(i=0;i<OLD_MAX_MAPMONS;i++)
	{
		badguy[i].type=old->badguy[i].type;
		if(badguy[i].type==MONS_BOUAPHA)
			badguy[i].item=ITM_NONE;
		else
			badguy[i].item=ITM_RANDOM;
		badguy[i].x=old->badguy[i].x;
		badguy[i].y=old->badguy[i].y;
		if(badguy[i].type==MONS_ZOMBIE || badguy[i].type==MONS_ZOMBONI)
			numBrains++;
		if(badguy[i].type==MONS_SUPERZOMBIE)
			numBrains+=2;
	}

	for(i=0;i<MAX_SPECIAL;i++)
		special[i].x=255;

	map=(mapTile_t *)malloc(sizeof(mapTile_t)*width*height);

	for(i=0;i<width*height;i++)
	{
		map[i].floor=old->map[i].floor;
		if(old->map[i].wall)
			map[i].wall=old->map[i].wall+199;
		else
			map[i].wall=0;
		map[i].item=old->map[i].item;
		map[i].light=old->map[i].light;
		map[i].templight=old->map[i].templight;
		map[i].opaque=0;
		map[i].select=1;

		if(BrainsGiven(map[i].item)>0)
			numBrains+=BrainsGiven(map[i].item);
	}

	for(i=0;i<OLD_MAX_SPECIAL;i++)
	{
		SpecialConvert(&old->special[i],&special[i],this);
	}
}

bool Legacy_GetWorldName(const char *fname,char *buf)
{
	if(fname[0]=='\0')
		return false;

	owned::SDL_RWops f = AssetOpen_SDL_Owned(fname);
	if(!f)
		return false;

	// this fseeks past:
	//   the int totalpoints, the 400 32x24 tiles,
	//   the 200 terrain types, the width&height of map 0, and bam there it is at the name
	//   of map 0.

	SDL_RWseek(f,1+sizeof(int)+400*32*24+200*sizeof(old_terrain_t)+2*sizeof(int),SEEK_SET);
	// read the name
	SDL_RWread(f,buf,1,32);
	return true;
}

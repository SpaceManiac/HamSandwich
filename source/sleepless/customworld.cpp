#include "customworld.h"
#include "shop.h"
#include "md5.h"
#include <stdlib.h>

#define ENTRY_NONE		0
#define ENTRY_TITLE		1
#define ENTRY_GOALS		2
#define ENTRY_ITEMS		3
#define ENTRY_FISHING	4
#define ENTRY_OPTIONS	5
#define ENTRY_PAGE		50

#define JOURNAL_SIZE	1024
#define NUM_INVENTORY	32
#define NUM_OPTIONS		128

// Basics
static byte isCustomWorld;
static char worldTitle[32];
static char worldTitleTemp[32];

// Journal pages
static char journalPages[20][JOURNAL_SIZE];

// Goals
static char goalNames[25][32];
static char goalDescriptions[25][64];

// Inventory
static invItem_t inventory[NUM_INVENTORY];
static byte inventorySize;

// Fishing
static char fishNames[10][20];
static float fishSizes[10];
static byte fishEffects[10];
static byte fishWhich[64];
static byte fishDifficulty[64];

// Options
static char optionNames[NUM_OPTIONS][32];
static byte optionValues[NUM_OPTIONS];
static byte numOptions;

// Helper stuff

byte IsCustomWorld(void)
{
	return isCustomWorld;
}

void AppendInvItem(byte pos, byte item, byte type, byte subtype, int max, char* name, char* desc)
{
	invItem_t* i = &inventory[inventorySize++];
	i->position = pos;
	i->item = item;
	i->type = type;
	i->subtype = subtype;
	i->max = max;
	strncpy(i->name, name, 31);
	i->name[31] = '\0';
	strncpy(i->desc, desc, 63);
	i->desc[63] = '\0';
}

#define C(VALUE) if (!strcasecmp(data, #VALUE)) return VALUE;
int ConstAtoi(const char* data)
{
	if (!strcasecmp(data, "NULL")) return 0;
	C(INVT_PANTS);
	C(INVT_HAMMERS);
	C(INVT_MYSTIC);
	C(INVT_BRAINS);
	C(INVT_CANDLES);
	C(INVT_JOURNAL);
	C(INVT_HFLAGS);
	C(INVT_ABILITY);
	C(INVT_KEYS);
	C(INVT_LVAR);
	C(INVT_GVAR);
	C(HMR_BIONIC);
	C(HMR_BLAST);
	C(HMR_REVERSE);
	C(HMR_REFLECT);
	C(ABIL_SHIELD);
	C(ABIL_TRAINING);
	C(ABIL_SOLAR);
	C(ABIL_FIRSTAID);
	C(ABIL_BRAIN);
	C(ABIL_FISH)
	return atoi(data);
}

// The meat of the processing

void InitCustomWorld(void)
{
	isCustomWorld = !!strcmp(profile.lastWorld, "hollow.shw");
	if (!IsCustomWorld())
		return;

	// Clear stuff
	strncpy(worldTitle,CustomWorldFname(),31);
	worldTitle[31]='\0';
	for(int i=0; i<20; ++i)
		journalPages[i][0]='\0';
	for(int i=0; i<25; ++i)
	{
		goalNames[i][0]='\0';
		goalDescriptions[i][0]='\0';
	}
	inventory[0].position=255;
	inventorySize=0;
	for (int i=0; i<10; ++i)
	{
		fishNames[i][0]='\0';
		fishSizes[i] = -1;
		fishEffects[i] = -1;
	}
	for (int i=0; i<64; ++i)
	{
		fishWhich[i] = 0;
		fishDifficulty[i] = 0;
	}
	numOptions=0;
	for (int i=0; i<NUM_OPTIONS; ++i)
	{
		optionNames[i][0]='\0';
		optionValues[i] = 0;
	}

	// Load stuff
	char buf[256];
	sprintf(buf, "worlds/%s", CustomWorldFname());
	strcpy(buf+strlen(buf)-4, ".txt");

	FILE* f = fopen(buf,"rt");
	buf[0] = '\0';

	if (!f)
		return;

	byte entrymode = ENTRY_TITLE;
	while (fgets(buf, 256, f) != NULL)
	{
		buf[strlen(buf)-1]='\0'; // ditch the LF

		if (buf[0] == '[')
		{
			// command line
			if(!strcmp(buf,"[title]"))
			{
				entrymode = ENTRY_TITLE;
			}
			else if(!strncmp(buf,"[page",5))
			{
				int page = 10*(buf[5]-'0')+buf[6]-'0';
				if (page >= 0 && page < 20)
					entrymode = ENTRY_PAGE + page;
			}
			else if (!strcmp(buf,"[goals]"))
			{
				entrymode = ENTRY_GOALS;
			}
			else if (!strcmp(buf,"[items]"))
			{
				entrymode = ENTRY_ITEMS;
			}
			else if (!strcmp(buf,"[fishing]"))
			{
				entrymode = ENTRY_FISHING;
			}
			else if (!strcmp(buf,"[options]"))
			{
				entrymode = ENTRY_OPTIONS;
			}
			else
			{
				entrymode = ENTRY_NONE;
			}
		}
		else
		{
			if(!strlen(buf)) continue;

			// text line
			if(entrymode==ENTRY_TITLE)
			{
				strncpy(worldTitle,buf,31);
				worldTitle[31] = '\0';
				entrymode=ENTRY_NONE;
			}
			else if (entrymode==ENTRY_GOALS)
			{
				char* number = strtok(buf,"=");
				char* name = strtok(NULL,"|");
				char* desc = strtok(NULL,"");

				if (!number || !name || !desc)
					continue;

				int goal = atoi(number);
				if (goal < 1 || goal > 25)
					continue;

				strncpy(goalNames[goal-1], name, 31);
				goalNames[goal-1][31]='\0';
				strncpy(goalDescriptions[goal-1], desc, 63);
				goalDescriptions[goal-1][63]='\0';
			}
			else if (entrymode==ENTRY_ITEMS)
			{
				char* pos = strtok(buf,"=");
				char* item = strtok(NULL,"|");
				char* type = strtok(NULL,"|");
				char* subtype = strtok(NULL,"|");
				char* max = strtok(NULL,"|");
				char* name = strtok(NULL,"|");
				char* desc = strtok(NULL,"");

				if (!pos || !item || !type || !subtype || !max || !name || !desc)
					continue;
				if (inventorySize >= NUM_INVENTORY)
					continue;

				AppendInvItem(atoi(pos),atoi(item),ConstAtoi(type),ConstAtoi(subtype),atoi(max),name,desc);
			}
			else if (entrymode==ENTRY_FISHING)
			{
				// name/effect/size, difficulty/fish
				char* what = strtok(buf,"|");
				char* idtext = strtok(NULL,"=");
				char* value = strtok(NULL,"");

				if (!what || !idtext || !value)
					continue;

				int id = atoi(idtext);
				if (!strcasecmp(what, "name"))
				{
					if (id < 0 || id > 9) continue;
					strncpy(fishNames[id], value, 19);
					fishNames[id][19]='\0';
				}
				else if (!strcasecmp(what, "effect"))
				{
					if (id < 0 || id > 9) continue;
					fishEffects[id]=atoi(value);
				}
				else if (!strcasecmp(what, "size"))
				{
					if (id < 0 || id > 9) continue;
					fishSizes[id]=(float) atof(value);
				}
				else if (!strcasecmp(what,"difficulty"))
				{
					if (id < 0 || id > 64) continue;
					fishDifficulty[id]=atoi(value);
				}
				else if (!strcasecmp(what,"fish"))
				{
					int fish = atoi(value);
					if (id < 0 || id > 64 || fish < 0 || (fish > 9 && fish != 255))
						continue;
					fishWhich[id]=fish;
				}
			}
			else if (entrymode==ENTRY_OPTIONS)
			{
				char* what = strtok(buf,"=");
				char* value = strtok(NULL,"|");

				if (!what || !value)
					continue;
				if (numOptions >= NUM_OPTIONS)
					continue;

				strcpy(optionNames[numOptions], what);
				optionValues[numOptions] = atoi(value);
				++numOptions;
			}
			else if (entrymode>=ENTRY_PAGE && entrymode<ENTRY_PAGE+20)
			{
				strncat(journalPages[entrymode-ENTRY_PAGE], buf, JOURNAL_SIZE-1-strlen(journalPages[entrymode-ENTRY_PAGE]));
			}
		}
	}
	fclose(f);
}

// Verification

byte VerifyHollowShw()
{
	FILE* f = fopen("worlds/hollow.shw", "rb");
	byte md5buf[17];

	// 18044403873A5A3DA052C871D4F76B6D
	// scrambled a little so it isn't so ctrl-f'able
	byte md5ok[17] = "\x5\xf1\x31\xf0\x74\x27\x47\x2a\x8d\x3f\xb5\x5e\xc1\xe4\x58\x5a";

	md5_stream(f, md5buf);
	fclose(f);
	md5buf[16] = '\0';

	for (int i = 0; i < 16; ++i)
		if (md5buf[i] != (md5ok[i] + 19) % 256)
			return 0;

	return 1;
}

// Filename and title

const char* CustomWorldFname()
{
	return profile.lastWorld;
}

const char* CustomWorldTitle(const char* fname)
{
	if (fname == NULL)
		return worldTitle;
	else if (!strcmp(fname, "hollow.shw"))
		return "Sleepless Hollow";
	else
	{
		char buf[256];
		sprintf(buf, "worlds/%s", fname);
		strcpy(buf+strlen(buf)-4, ".txt");

		FILE* f = fopen(buf,"rt");
		buf[0] = '\0';

		if (f && fgets(buf, 32, f) != NULL)
		{
			strncpy(worldTitleTemp,buf,31);
		}
		else
		{
			strncpy(worldTitleTemp,fname,31);
		}
		worldTitleTemp[31]='\0';
		if (f) fclose(f);

		return worldTitleTemp;
	}
}

// Getters

char* CustomJournalPage(int p)
{
	return journalPages[p];
}

char* CustomGoalInfo(byte goal, byte info)
{
	if (info == 1)
		return goalDescriptions[goal];
	else
		return goalNames[goal];
}

invItem_t* CustomInventory()
{
	if (inventorySize == 0)
		return NULL;
	else
	{
		inventory[inventorySize].position = 255;
		return inventory;
	}
}

char* CustomFishName(byte type)
{
	return fishNames[type];
}

float CustomFishSize(byte type)
{
	return fishSizes[type];
}

byte CustomFishEffect(byte type)
{
	return fishEffects[type];
}

byte CustomFishWhich(byte map)
{
	return fishWhich[map];
}

byte CustomFishDiff(byte map)
{
	return fishDifficulty[map];
}

byte CustomOption(const char* optname, byte def)
{
	// Shortcut so callers don't have to check this themselves
	if (!IsCustomWorld()) return def;

	for (int i=0; i<NUM_OPTIONS;++i)
	{
		if (!strcasecmp(optname, optionNames[i]))
			return optionValues[i];
	}
	return def;
}

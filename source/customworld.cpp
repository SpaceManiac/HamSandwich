#include "customworld.h"
#include "shop.h"
#include "md5.h"

#define ENTRY_NONE		0
#define ENTRY_TITLE		1
#define ENTRY_GOALS		2
#define ENTRY_ITEMS		3
#define ENTRY_PAGE		50

#define JOURNAL_SIZE	1024

static byte isCustomWorld;
static char worldTitle[32];
static char worldTitleTemp[32];
static char journalPages[20][JOURNAL_SIZE];
static char goalNames[25][32];
static char goalDescriptions[25][64];

static invItem_t inventory[32];
static byte inventorySize;

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

#define C(VALUE) if (!stricmp(data, #VALUE)) return VALUE;
int ConstAtoi(const char* data)
{
	if (!stricmp(data, "NULL")) return 0;
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

void InitCustomWorld(void)
{
	isCustomWorld = !!strcmp(profile.lastWorld, "hollow.shw");
	if (!IsCustomWorld())
		return;

	// Clear stuff
	strcpy(worldTitle,CustomWorldFname());
	for(int i=0; i<20; ++i)
		journalPages[i][0]='\0';
	for(int i=0; i<25; ++i)
	{
		goalNames[i][0]='\0';
		goalDescriptions[i][0]='\0';
	}
	inventory[0].position=255;
	inventorySize=0;

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
				char* number = strtok(buf,"=\n");
				char* name = strtok(NULL,"|\n");
				char* desc = strtok(NULL,"\n");

				if (!number || !name || !desc)
					continue;

				int goal = atoi(number);
				if (!goal)
					continue;

				strcpy(goalNames[goal-1], name);
				strcpy(goalDescriptions[goal-1], desc);
			}
			else if (entrymode==ENTRY_ITEMS)
			{
				char* pos = strtok(buf,"=\n");
				char* item = strtok(NULL,"|\n");
				char* type = strtok(NULL,"|\n");
				char* subtype = strtok(NULL,"|\n");
				char* max = strtok(NULL,"|\n");
				char* name = strtok(NULL,"|\n");
				char* desc = strtok(NULL,"\n");
				AppendInvItem(atoi(pos),atoi(item),ConstAtoi(type),ConstAtoi(subtype),atoi(max),name,desc);
			}
			else if (entrymode>=ENTRY_PAGE && entrymode<ENTRY_PAGE+20)
			{
				strncat(journalPages[entrymode-ENTRY_PAGE], buf, JOURNAL_SIZE-1-strlen(journalPages[entrymode-ENTRY_PAGE]));
			}
		}
	}
	fclose(f);
}

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
			strcpy(worldTitleTemp,buf);
		}
		else
		{
			strcpy(worldTitleTemp,fname);
		}
		if (f) fclose(f);

		return worldTitleTemp;
	}
}

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

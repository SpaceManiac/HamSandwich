#include "customworld.h"

#define ENTRY_NONE		0
#define ENTRY_TITLE		1
#define ENTRY_GOALS		2
#define ENTRY_PAGE		50

#define JOURNAL_SIZE	1024

static byte isCustomWorld;
static char worldTitle[32];
static char worldTitleTemp[32];
static char journalPages[20][JOURNAL_SIZE];
static char goalNames[25][32];
static char goalDescriptions[25][64];

byte IsCustomWorld(void)
{
	return isCustomWorld;
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

	// Load stuff
	char buf[256];
	sprintf(buf, "worlds/%s", CustomWorldFname());
	strcpy(buf+strlen(buf)-4, ".txt");

	FILE* f = fopen(buf,"r");
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
				char* desc = strtok(NULL,"\n");

				if (!number || !name || !desc)
					continue;

				int goal = atoi(number);
				if (!goal)
					continue;

				strcpy(goalNames[goal-1], name);
				strcpy(goalDescriptions[goal-1], desc);
			}
			else if (entrymode>=ENTRY_PAGE && entrymode<ENTRY_PAGE+20)
			{
				strncat(journalPages[entrymode-ENTRY_PAGE], buf, JOURNAL_SIZE-1-strlen(journalPages[entrymode-ENTRY_PAGE]));
			}
		}
	}
	fclose(f);
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

		FILE* f = fopen(buf,"r");
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

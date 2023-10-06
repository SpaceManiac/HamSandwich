#include "highscore.h"
#include "display.h"
#include "game.h"
#include "loop.h"
#include "options.h"
#include "player.h"
#include "timeattack.h"
#include "title.h"

byte g_RecordPlayer;
byte g_GotRecordIndex;
byte g_RecordFlip;
byte g_GotRecordNameLength;
byte g_RecordDone;

bool HighScoreUpdate();
void HighScoreRender();
bool HighTimeUpdate();
void HighTimeRender();

void HighScoreInit(void)

{
	memcpy(g_Background, mgl->GetScreen(), 0x4b000);
	if (g_IsTimeAttack == false)
	{
		SetUpdate(HighScoreUpdate);
		SetRender(HighScoreRender);
	}
	else
	{
		SetUpdate(HighTimeUpdate);
		SetRender(HighTimeRender);
	}
	g_RecordPlayer = 0;
	g_RecordFlip = 0;
	mgl->LastKeyPressed();
	return;
}

void HighScoreExit(void)
{
}

void HighScoreKey(void)
{
	char k;

	k = mgl->LastKeyPressed();
	if (k != '\0')
	{
		if (k == '\x1b')
		{
			strcpy((char *)g_Config.scoreNames[g_GotRecordIndex], "??????");
			g_RecordDone = true;
		}
		else if (k == '\r')
		{
			g_RecordDone = true;
		}
		else if (k == '\b')
		{
			if (g_GotRecordNameLength != 0)
			{
				g_GotRecordNameLength = g_GotRecordNameLength - 1;
				g_Config.scoreNames[g_GotRecordIndex][g_GotRecordNameLength] = 0;
			}
		}
		else if (g_GotRecordNameLength < 10)
		{
			g_Config.scoreNames[g_GotRecordIndex][g_GotRecordNameLength] = k;
			g_Config.scoreNames[g_GotRecordIndex][g_GotRecordNameLength + 1] = 0;
			g_GotRecordNameLength = g_GotRecordNameLength + 1;
		}
	}
	return;
}

bool HighScoreUpdate(void)
{
	int i;
	int recordIndex;

	g_RecordFlip = 1 - g_RecordFlip;
	if (g_RecordPlayer == 0)
	{
		recordIndex = 0;
	LAB_0040e439:
		if (recordIndex < 5)
		{
			if (g_Player[0].score < (unsigned int)g_Config.scores[recordIndex] ||
			    g_Player[0].score == g_Config.scores[recordIndex])
				goto LAB_0040e430;
			g_RecordPlayer = 1;
			g_GotRecordIndex = (byte)recordIndex;
			for (i = 4; recordIndex < i; i = i + -1)
			{
				g_Config.scores[i] = g_Config.scores[i - 1];
				strcpy(g_Config.scoreNames[i], g_Config.scoreNames[i - 1]);
			}
			g_Config.scores[g_GotRecordIndex] = g_Player[0].score;
			g_Config.scoreNames[g_GotRecordIndex][0] = '\0';
			g_RecordDone = false;
			g_GotRecordNameLength = 0;
		}
		if (g_RecordPlayer == 0)
		{
			g_RecordPlayer = 1;
			g_RecordDone = true;
		}
	}
	if (g_RecordPlayer == 1)
	{
		if (g_RecordDone == true)
		{
			if (g_NumPlayers == 2)
			{
				recordIndex = 0;
			LAB_0040e555:
				if (recordIndex < 5)
				{
					if (g_Player[1].score < (unsigned int)g_Config.scores[recordIndex] ||
					    g_Player[1].score == g_Config.scores[recordIndex])
						goto LAB_0040e54c;
					g_RecordPlayer = 2;
					g_GotRecordIndex = (byte)recordIndex;
					for (i = 4; recordIndex < i; i = i + -1)
					{
						g_Config.scores[i] = g_Config.scores[i - 1];
						strcpy(g_Config.scoreNames[i], g_Config.scoreNames[i - 1]);
					}
					g_Config.scores[g_GotRecordIndex] = g_Player[1].score;
					g_Config.scoreNames[g_GotRecordIndex][0] = '\0';
					g_RecordDone = false;
					g_GotRecordNameLength = 0;
				}
			}
			if (g_RecordPlayer == 1)
			{
				HighScoreExit();
				TitleInit();
				return true;
			}
		}
		else
		{
			HighScoreKey();
		}
	}
	if (g_RecordPlayer == 2)
	{
		if (g_RecordDone == true)
		{
			HighScoreExit();
			TitleInit();
		}
		else
		{
			HighScoreKey();
		}
	}
	return true;
LAB_0040e430:
	recordIndex = recordIndex + 1;
	goto LAB_0040e439;
LAB_0040e54c:
	recordIndex = recordIndex + 1;
	goto LAB_0040e555;
}

void HighScoreRender(void)
{
	char buf[60];

	memcpy(mgl->GetScreen(), g_Background, 0x4b000);
	if (g_RecordPlayer == 1)
	{
		CenterPrintBright(0x142, 0x110, "Player 1 got a high score!!", -0x20);
		CenterPrintMultiline(0x140, 0x10e, "Player 1 got a high score!!");
		sprintf(buf, "Enter your name: %s", g_Config.scoreNames[g_GotRecordIndex]);
		if (g_RecordFlip != 0)
		{
			strcat(buf, "_");
		}
		PrintBright(0xca, 0x12e, buf, -0x20);
		PrintMultiline(200, 300, buf);
	}
	if (g_RecordPlayer == 2)
	{
		CenterPrintBright(0x142, 0x110, "Player 2 got a high score!!", -0x20);
		CenterPrintMultiline(0x140, 0x10e, "Player 2 got a high score!!");
		sprintf(buf, "Enter your name: %s", g_Config.scoreNames[g_GotRecordIndex]);
		if (g_RecordFlip != 0)
		{
			strcat(buf, "_");
		}
		PrintBright(0xca, 0x12e, buf, -0x20);
		PrintMultiline(200, 300, buf);
	}
	return;
}

void HighTimeKey(void)
{
	char k;

	k = mgl->LastKeyPressed();
	if (k != '\0')
	{
		if (k == '\x1b')
		{
			strcpy(g_Config.times[g_GameMode == GameMode::DankDungeons][g_MapNum - 1].name, "??????");
			g_RecordDone = true;
		}
		else if (k == '\r')
		{
			g_RecordDone = true;
		}
		else if (k == '\b')
		{
			if (g_GotRecordNameLength != 0)
			{
				g_GotRecordNameLength = g_GotRecordNameLength - 1;
				g_Config.times[g_GameMode == GameMode::DankDungeons][g_MapNum - 1].name[g_GotRecordNameLength] = 0;
			}
		}
		else if (g_GotRecordNameLength < 7)
		{
			g_Config.times[g_GameMode == GameMode::DankDungeons][g_MapNum - 1].name[g_GotRecordNameLength] = k;
			g_Config.times[g_GameMode == GameMode::DankDungeons][g_MapNum - 1].name[g_GotRecordNameLength + 1] = 0;
			g_GotRecordNameLength = g_GotRecordNameLength + 1;
		}
	}
	return;
}

bool HighTimeUpdate(void)
{
	g_RecordFlip = 1 - g_RecordFlip;
	if (g_RecordPlayer == 0)
	{
		if (g_RecordTimeFrames <
		    g_Config.times[g_GameMode == GameMode::DankDungeons][g_MapNum - 1].frames)
		{
			g_RecordPlayer = 1;
			g_Config.times[g_GameMode == GameMode::DankDungeons][g_MapNum - 1].frames = g_RecordTimeFrames;
			g_Config.times[g_GameMode == GameMode::DankDungeons][g_MapNum - 1].minutes = (unsigned long long)g_RecordTimeFrames / 0x708;
			g_Config.times[g_GameMode == GameMode::DankDungeons][g_MapNum - 1].seconds =
				(char)((unsigned long long)g_RecordTimeFrames / 0x1e) +
				(char)((unsigned long long)g_RecordTimeFrames / 0x708) * -0x3c;
			g_Config.times[g_GameMode == GameMode::DankDungeons][g_MapNum - 1].name[0] = 0;
			g_RecordDone = false;
			g_GotRecordNameLength = 0;
		}
		if (g_RecordPlayer == 0)
		{
			g_RecordPlayer = 1;
			g_RecordDone = true;
		}
	}
	if (g_RecordPlayer == 1)
	{
		if (g_RecordDone == true)
		{
			HighScoreExit();
			TimeAttackInit();
		}
		else
		{
			HighTimeKey();
		}
	}
	return true;
}

void HighTimeRender(void)
{
	char buf[60];

	memcpy(mgl->GetScreen(), g_Background, 0x4b000);
	if (g_RecordPlayer == 1)
	{
		CenterPrintBright(0x142, 0x110, "You got a record time!!", -0x20);
		CenterPrintMultiline(0x140, 0x10e, "You got a record time!!");
		sprintf(buf, "Enter your name: %s", g_Config.times[g_GameMode == GameMode::DankDungeons][g_MapNum - 1].name);
		if (g_RecordFlip != 0)
		{
			strcat(buf, "_");
		}
		PrintBright(0xca, 0x12e, buf, -0x20);
		PrintMultiline(200, 300, buf);
	}
	return;
}

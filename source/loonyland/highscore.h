#ifndef HIGHSCORE_H
#define HIGHSCORE_H

#include "game.h"

typedef struct highScore_t
{
	byte mode;
	char name[4];
	int  score;

	// stats
	dword monsterPoints;
	dword time;
	dword bestCombo;
	dword shotsFired;
	dword specialShotsFired;
	dword guysKilled;
	dword gemsGotten;
	dword numSaves;
	dword hitsTaken;
	int	  spclValue;	// values needed for various modes
} highScore_t;

void NameEntry(MGLDraw *mgl);
void HighScore(MGLDraw *mgl);

void ResetHighScores(void);
void CheckForHighScore(highScore_t myScore);
void MakeHighScore(highScore_t *me,int score);

#endif

#ifndef OPTIONS_H
#define OPTIONS_H

#include "jamultypes.h"

#define DIFF_EASY	0
#define DIFF_NORMAL 1
#define DIFF_HARD	2

typedef struct options_t
{
	byte lives;			// starting # of lives, 1-9
	byte shroomReset;	// do shrooms reset between levels?
	byte difficulty;	// easy, medium, hard
	byte sound;
	byte music;
	byte cheating;
	// keep high scores handy
	char hiScoreName[5][9];
	int  hiScore[5];
} options_t;

void InitOptions(void);
void ExitOptions(void);

char *GetHiScoreName(byte w);
int GetHiScore(byte w);
void SetHiScore(byte w,int score);
byte GetDifficulty(void);
byte GetStartLives(void);
byte SoundIsOn(void);
byte MusicIsOn(void);
byte GetShroomReset(void);
void SetCheat(byte on);
byte Cheating(void);
void ToggleMusic(void);
void ToggleSound(void);

void OptionsScreen(void);

#endif
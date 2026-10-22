#ifndef HIGHSCORE_H
#define HIGHSCORE_H

#include "game.h"
#include "profile.h"

typedef struct highScore_t
{
	char name[PRF_NAME_LEN];
	byte pic;
	byte color;
	char bright;
	int  score;
} highScore_t;

void ResetHighScore(byte mode,byte level);
void ResetHighScores(byte mode);
void RenderHighScore(byte mode, byte level,byte score,int x,int y,MGLDraw *mgl);
byte CheckHighScore(byte mode,byte level,int score);
void RenderStar(byte color,byte spr,int x,int y,MGLDraw *mgl);
void RenderGlowStar(byte spr,int x,int y,MGLDraw *mgl);
void RenderGhostStar(byte spr,int x,int y,char bright,MGLDraw *mgl);

void FreeHiScoreSprites(void);

byte LoadAddOnScores(char *fname);
byte SaveAddOnScores(void);
byte CheckAddOnScore(byte level,int score);
int GoldScore(int lvl,byte score);
void SwapGoldScores(int a,int b);
void SetAddOnScoreName(char *fname);
void SetGoldScore(int lvl,byte which,int score);

#endif
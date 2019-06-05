#ifndef HISCORE_H
#define HISCORE_H

#include "mgldraw.h"
#include "map.h"

// when playing, different score events
#define SE_INIT		0
#define SE_KILL		1
#define SE_OUCH		2
#define SE_HURT		3
#define SE_SHOOT	4
#define SE_SECOND	5	// a second of time has passed
#define SE_PICKUP	6	// for picking up a weapon
#define SE_HEAL		7
#define SE_RAGE		8
#define SE_DOOR		9
#define SE_BRAIN	10
#define SE_CANDLE	11

#define RECORD_SCORE	1	// bit flags for CheckHiScore to return
#define RECORD_SCORE2	2
#define RECORD_SCORE3	3
#define RECORD_TIME		4
#define RECORD_TIME2	8
#define RECORD_TIME3	12
#define RECORD_BASE		16
#define RECORD_COMBO	32
#define RECORD_DESTROY	64

// these defines are used during the new score initiation process
#define SCORE_ABSENT 0	// indicator that a score doesn't exist
#define SCORE_SAVED	1	// indicate it came from the stored list
#define SCORE_NEW	2	// indicate it's a newly created score

typedef struct score_t
{
	char name[16];
	dword score;
	byte playAs;
	byte difficulty;
	byte day,month,year;
	dword levelChecksum;
	dword scoreChecksum;
	byte beenUploaded;
} score_t;

void ScoreEvent(byte event,int amt);

void InitHiScores(void);
void ExitHiScores(void);
void SaveHiScores(void);
void LoadHiScores(void);

byte TryHighScore(void);

byte GetTopScores(score_t *winners,Map *map);
byte GetTopTimes(score_t *winners,Map *map);

void CryptoTest(void);

void EraseHighScores(world_t *world);
dword ChecksumMap(Map *map);
char *GetScoresToUpload(int *num,int *scoreSize);
char *GetTimesToUpload(int *num,int *scoreSize);
int CountScoresToUpload(void);
void UnUpload(byte time,char *encrypted);
void ChangeHighScores(const char *oldName,const char *newName);

#endif

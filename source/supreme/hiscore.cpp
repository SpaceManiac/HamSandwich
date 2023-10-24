#include "hiscore.h"
#include "map.h"
#include "player.h"
#include <time.h>
#include <assert.h>
#include "editor.h"
#include "game.h"
#include "config.h"
#include "appdata.h"
#include "steam.h"

#if __linux__ || __EMSCRIPTEN__
#include <unistd.h>
#endif

// ---------------- Encrypting scores into strings!

static const char ctab[]=
	"0123456789"	// 0-9
	"abcdefghij"	// 10-19
	"klmnopqrst"	// 20-29
	"uvwxyzABCD"	// 30-39
	"EFGHIJKLMN"	// 40-49
	"OPQRSTUVWX"	// 50-59
	"YZ.-";			// 60-63

static char *enc_score;
static byte *cryptoTab;
static byte scoreLen;

// these weird ass things are for the random number generator
static dword s1=390451501, s2=613566701, s3=858993401;  /* The seeds */
static dword mask1, mask2, mask3;
static int shft1, shft2, shft3, k1=31, k2=29, k3=28;
static int q1=13, q2=2, q3=3, p1=12, p2=4, p3=17;

dword CryptoRand(void)
{
   dword b;

   b  = ((s1 << q1)^s1) >> shft1;
   s1 = ((s1 & mask1) << p1) ^ b;
   b  = ((s2 << q2) ^ s2) >> shft2;
   s2 = ((s2 & mask2) << p2) ^ b;
   b  = ((s3 << q3) ^ s3) >> shft3;
   s3 = ((s3 & mask3) << p3) ^ b;

   return (s1 ^ s2 ^ s3);
}

void CryptoSrand(int a,int b,int c)
{
   static dword x = 4294967295U;

   shft1 = k1-p1;
   shft2 = k2-p2;
   shft3 = k3-p3;
   mask1 = x << (32-k1);
   mask2 = x << (32-k2);
   mask3 = x << (32-k3);
   if (a > (1<<shft1)) s1 = a;
   if (b > (1<<shft2)) s2 = b;
   if (c > (1<<shft3)) s3 = c;
   CryptoRand();
}

void SetupCrypto(int key1,int key2,int key3)
{
	int i;
	byte a,b,c;

	s1=390451501;
	s2=613566701;
	s3=858993401;
	k1=31;
	k2=29;
	k3=28;
	q1=13;
	q2=2;
	q3=3;
	p1=12;
	p2=4;
	p3=17;

	// it will take 4/3 as many bytes to hold the score when it's in
	// 6 bit form as in 8 bit form: 8/6=4/3
	// the extra 2 bytes are for safety margin
	scoreLen=sizeof(score_t)*4/3+2;
	enc_score=(char *)malloc(scoreLen+1);
	cryptoTab=(byte *)malloc(scoreLen);

	for(i=0;i<scoreLen;i++)
		cryptoTab[i]=i;

	CryptoSrand(key1,key2,key3);
	// one thousand "random" swaps
	for(i=0;i<1000;i++)
	{
		b=(CryptoRand()%scoreLen);
		c=(CryptoRand()%scoreLen);
		a=cryptoTab[b];
		cryptoTab[b]=cryptoTab[c];
		cryptoTab[c]=a;
	}
	/*
	FILE *f;

	f=AppdataOpen("cryptotable.txt","wt");
	fprintf(f,"<?php\ncryptoTable=array(\n");
	for(i=0;i<scoreLen;i++)
	{
		fprintf(f,"\t%d,\n",cryptoTab[i]);
	}
	fprintf(f,"); ?>");
	fclose(f);
	*/
}

void ExitCrypto(void)
{
	free(enc_score);
	free(cryptoTab);
}

void EncryptScore(score_t *s)
{
	int pos,dst;
	byte *ptr;
	byte mode;
	byte *data;

	data=(byte *)malloc(scoreLen);
	for(pos=0;pos<scoreLen;pos++)
		data[pos]=0;

	mode=0;
	pos=0;
	dst=0;
	ptr=(byte *)s;
	while(pos<(int)sizeof(score_t))
	{
		switch(mode)
		{
			case 0:	// write bottom 6 bits out
				data[dst++]=(*ptr)&63;
				break;
			case 1:	// write top 2 bits out, bottom 4 bits of next
				data[dst]=((*ptr)>>6)&3;
				ptr++;
				pos++;
				if(pos==sizeof(score_t))
					break;
				data[dst++]|=(((*ptr)&15)<<2);	// bottom 4 bits, placed 2 spots up
				break;
			case 2:	// write top 4 bits of this, bottom 2 bits of next
				data[dst]=((*ptr)>>4)&15;
				ptr++;
				pos++;
				if(pos==sizeof(score_t))
					break;
				data[dst++]|=(((*ptr)&3)<<4);	// bottom 2 bits, placed 4 spots up
				break;
			case 3: // write top 6 bits out
				data[dst++]=((*ptr)>>2)&63;
				ptr++;
				pos++;
				break;
		}
		mode++;
		if(mode==4)
			mode=0;
	}

	mode=0;
	for(pos=0;pos<scoreLen;pos++)	// convert into char code
	{
		mode=(pos&63);
		data[pos]=(data[pos]^mode);
		enc_score[cryptoTab[pos]]=ctab[data[pos]];
	}

	enc_score[scoreLen]='\0';

	free(data);
}

void DecryptScore(score_t *s)
{
	int pos,src,k;
	byte *ptr;
	byte mode;
	byte *data;

	data=(byte *)malloc(scoreLen);
	for(pos=0;pos<scoreLen;pos++)
	{
		for(src=0;src<64;src++)
			if(enc_score[pos]==ctab[src])
			{
				for(k=0;k<scoreLen;k++)
					if(cryptoTab[k]==pos)
					{
						data[k]=(src^(k&63));
					}
			}
	}

	mode=0;
	pos=0;
	src=0;
	ptr=(byte *)s;
	while(pos<(int)sizeof(score_t))
	{
		switch(mode)
		{
			case 0:	// read bottom 6 bits
				*ptr=data[src++];
				break;
			case 1:	// read top 2 bits out, bottom 4 bits of next
				(*ptr)|=((data[src]&3)<<6);
				ptr++;
				pos++;
				if(pos==sizeof(score_t))
					break;
				*ptr=(data[src++]>>2);
				break;
			case 2:	// read top 4 bits of this, bottom 2 bits of next
				(*ptr)|=((data[src]&15)<<4);
				ptr++;
				pos++;
				if(pos==sizeof(score_t))
					break;
				*ptr=(data[src++]>>4);
				break;
			case 3: // read top 6 bits
				(*ptr)|=(data[src++]<<2);
				ptr++;
				pos++;
				break;
		}
		mode++;
		if(mode==4)
			mode=0;
	}
	free(data);
}

void CryptoTest(void)
{
	FILE *f;

	score_t test={
		"Bobby2",100,0,1,12,10,74,511718,124958,0};
	score_t test2;

	memcpy(&test2,&test,sizeof(score_t));

	SetupCrypto(3574,858734,298437);
	f=AppdataOpen_Write("test.txt");
	EncryptScore(&test);
	fprintf(f,"Encrypted:\n%s\n\n",enc_score);
	DecryptScore(&test);
	fprintf(f,"Decrypted:\n");
	fprintf(f,"name: %s\nchecksum: %u\n\n",test.name,test.scoreChecksum);
	fprintf(f,"memcmp: %d\n",memcmp(&test,&test2,sizeof(score_t)));
	fclose(f);
	ExitCrypto();
}

// --------------- CHECKSUMMING FUNCTIONS

static word r,c1,c2;
static dword sum;

void ResetChecksum(void)
{
	sum=0;
	r=55665;
	c1=52845;
	c2=22719;
}

void AddToSum(byte b)
{
	byte cipher;

	cipher = (b ^ (r >> 8));
	r = (word)((unsigned int)(cipher + r) * (unsigned int)c1 + (unsigned int)c2);
	sum += cipher;
}

void AddToSum(word w)
{
	AddToSum((byte)(w>>8));
	AddToSum((byte)(w&255));
}

void AddToSum(int32_t i)
{
	AddToSum((byte)((i>>24)&255));
	AddToSum((byte)((i>>16)&255));
	AddToSum((byte)((i>>8)&255));
	AddToSum((byte)(i&255));
}

void AddToSum(dword i)
{
	AddToSum((byte)((i>>24)&255));
	AddToSum((byte)((i>>16)&255));
	AddToSum((byte)((i>>8)&255));
	AddToSum((byte)(i&255));
}

void AddToSum(char *s)
{
	int i;

	for(i=0;i<(int)strlen(s);i++)
		AddToSum((byte)s[i]);
}

dword ChecksumMap(Map *map)
{
	int i,j;

	ResetChecksum();

	AddToSum(map->name);

	AddToSum(map->numBrains);
	AddToSum(map->numCandles);
	AddToSum(map->flags);
	AddToSum(map->width);
	AddToSum(map->height);
	AddToSum(map->itemDrops);

	for(i=0;i<map->width*map->height;i++)
	{
		AddToSum(map->map[i].floor);
		AddToSum(map->map[i].item);
		AddToSum(map->map[i].wall);
		AddToSum(map->map[i].light);
	}

	for(i=0;i<MAX_MAPMONS;i++)
	{
		if(map->badguy[i].type)
		{
			if (map->badguy[i].type > UINT8_MAX)
				AddToSum(map->badguy[i].type);
			else
				AddToSum((byte) map->badguy[i].type);
			AddToSum(map->badguy[i].x);
			AddToSum(map->badguy[i].y);
			AddToSum(map->badguy[i].item);
		}
	}

	for(i=0;i<MAX_SPECIAL;i++)
	{
		if(map->special[i].x!=255)
		{
			AddToSum(map->special[i].x);
			AddToSum(map->special[i].y);

			for(j=0;j<NUM_TRIGGERS;j++)
			{
				if(map->special[i].trigger[j].type)
				{
					AddToSum(map->special[i].trigger[j].type);
					AddToSum(map->special[i].trigger[j].flags);
					AddToSum(map->special[i].trigger[j].x);
					AddToSum(map->special[i].trigger[j].y);
					AddToSum(map->special[i].trigger[j].value);
					AddToSum(map->special[i].trigger[j].value2);
				}
			}

			for(j=0;j<NUM_EFFECTS;j++)
			{
				if(map->special[i].effect[j].type)
				{
					AddToSum(map->special[i].effect[j].type);
					AddToSum(map->special[i].effect[j].flags);
					AddToSum(map->special[i].effect[j].x);
					AddToSum(map->special[i].effect[j].y);
					AddToSum(map->special[i].effect[j].value);
					AddToSum(map->special[i].effect[j].value2);
				}
			}
		}
	}

	return sum;
}

void ScoreChecksum(score_t *score,Map *map)
{
	score->levelChecksum=ChecksumMap(map);

	ResetChecksum();

	AddToSum(score->day);
	AddToSum(score->month);
	AddToSum(score->year);
	AddToSum(score->levelChecksum);
	AddToSum(score->name);
	AddToSum(score->playAs);
	AddToSum(score->difficulty);
	AddToSum(score->score);

	score->scoreChecksum=sum;
}

void ScoreChecksumNoMap(score_t *score)
{
	ResetChecksum();

	AddToSum(score->day);
	AddToSum(score->month);
	AddToSum(score->year);
	AddToSum(score->levelChecksum);
	AddToSum(score->name);
	AddToSum(score->playAs);
	AddToSum(score->difficulty);
	AddToSum(score->score);

	score->scoreChecksum=sum;
}

// ----------- Gameplay score twiddling!

void ChangeScore(int amt)
{
	if(amt<0 && player.score<(dword)(-amt))
		player.score=0;
	else
		player.score+=amt;
}

void ScoreEvent(byte event,int amt)
{
	switch(event)
	{
		case SE_INIT:
			// amt=width*height of map
			player.score=1000;
			break;
		case SE_OUCH:
			// amt=damage taken (by you or a friendly monster)
			ChangeScore(-amt*2);
			break;
		case SE_HURT:
			// amt=damage done to an enemy
			ChangeScore(amt);
			break;
		case SE_KILL:
			// amt is ignored
			if((player.combo/5)+1<10)
				ChangeScore((player.combo/5)+1);
			else
				ChangeScore(10);
			break;
		case SE_SHOOT:
			// amt=number of shots being fired at once
			ChangeScore(-amt);
			break;
		case SE_SECOND:
			// amt is ignored
			ChangeScore(-1);
			break;
		case SE_PICKUP:
			ChangeScore(-1);
			break;
		case SE_HEAL:
			// amt=life healed
			ChangeScore(-amt);
			break;
		case SE_RAGE:
			ChangeScore(-100);
			break;
		case SE_DOOR:
			ChangeScore(10);
			break;
		case SE_BRAIN:
			ChangeScore(25*amt);
			break;
		case SE_CANDLE:
			ChangeScore(5*amt);
			break;
	}
}

// ---------------------- hi score management

static score_t these[4];	// a set of score_t's to work with on each level
static byte saved[4];		// an indicator of whether each of those 4 came from the list, or was created new

static score_t *hiScore=NULL,*hiTime=NULL;
static word numScores=0,numTimes=0;

void InitHiScores(void)
{
	if(config.hiscores)
		LoadHiScores();
}

void ExitHiScores(void)
{
	if(hiScore)
		free(hiScore);
	if(hiTime)
		free(hiTime);
}

void SaveHiScoreFile(score_t *list,word num,const char *fname)
{
	FILE *f;
	word i;

	if(!config.hiscores)
		return;

	if(num==0)
	{
		f=AppdataOpen_Write(fname);
		if(!f)
			return;
		fwrite(&num,sizeof(word),1,f);
		fclose(f);
		AppdataSync();
		return;
	}
	f=AppdataOpen_Write(fname);
	if(!f)
		return;
	fwrite(&num,sizeof(word),1,f);	// write out the number of scores

	SetupCrypto(3574,858734,298437);
	for(i=0;i<num;i++)			// write out each score, encrypted
	{
		EncryptScore(&list[i]);
		fwrite(enc_score,sizeof(char),scoreLen,f);
	}
	ExitCrypto();
	fclose(f);
	AppdataSync();
}

void SaveHiScores(void)
{
	SaveHiScoreFile(hiScore,numScores,"hiscore.dat");
	SaveHiScoreFile(hiTime,numTimes,"hitime.dat");
}

void LoadHiScoresFile(void)
{
	FILE *f;
	word i;

	if(!config.hiscores)
		return;

	f=AppdataOpen("hiscore.dat");
	if(!f)
	{
		numScores=0;
		if(hiScore)
			free(hiScore);
		hiScore=NULL;
		return;
	}
	if(hiScore)
		free(hiScore);

	fread(&numScores,sizeof(word),1,f);	// read in the number of scores

	if(numScores>0)
	{
		hiScore=(score_t *)malloc(sizeof(score_t)*numScores);
		SetupCrypto(3574,858734,298437);
		for(i=0;i<numScores;i++)			// read in each score, encrypted
		{
			fread(enc_score,sizeof(char),scoreLen,f);
			DecryptScore(&hiScore[i]);		// and decrypt!
		}
		ExitCrypto();
	}
	fclose(f);
}

void LoadHiTimesFile(void)
{
	FILE *f;
	word i;

	if(!config.hiscores)
		return;

	f=AppdataOpen("hitime.dat");
	if(!f)
	{
		numTimes=0;
		if(hiTime)
			free(hiTime);
		hiTime=NULL;
		return;
	}
	if(hiTime)
		free(hiTime);

	fread(&numTimes,sizeof(word),1,f);	// read in the number of scores

	if(numTimes>0)
	{
		hiTime=(score_t *)malloc(sizeof(score_t)*numTimes);
		SetupCrypto(3574,858734,298437);
		for(i=0;i<numTimes;i++)			// read in each score, encrypted
		{
			fread(enc_score,sizeof(char),scoreLen,f);
			DecryptScore(&hiTime[i]);		// and decrypt!
		}
		ExitCrypto();
	}
	fclose(f);
}

void LoadHiScores(void)
{
	LoadHiScoresFile();
	LoadHiTimesFile();
}

void AppendScore(score_t *me)
{
	if(numScores==65535)
		return;	// can't add any more, sorry!
	numScores++;
	hiScore=(score_t *)realloc(hiScore,sizeof(score_t)*numScores);
	if(!hiScore)
	{
		LoadHiScoresFile();	// failed to allocate enough memory!  Give up!
		return;
	}
	hiScore[numScores-1]=*me;
}

void AppendTime(score_t *me)
{
	if(numTimes==65535)
		return;	// can't add any more, sorry!
	numTimes++;
	hiTime=(score_t *)realloc(hiTime,sizeof(score_t)*numTimes);
	if(!hiTime)
	{
		LoadHiTimesFile();	// failed to allocate enough memory!  Give up!
		return;
	}
	hiTime[numTimes-1]=*me;
}

void CreateScore(dword score,Map *map)
{
	time_t ltime;
	struct tm *today;

	if(map->flags&MAP_HUB)
		return;

	// TODO tzset();

	time(&ltime);
	today=localtime(&ltime);

	these[3].day=today->tm_mday;
	these[3].month=today->tm_mon;
	these[3].year=today->tm_year;

	these[3].score=score;
	these[3].playAs=profile.playAs;
	these[3].beenUploaded=0;

	these[3].difficulty=profile.difficulty;
	strcpy(these[3].name,profile.name);

	ScoreChecksum(&these[3],map);
	saved[3]=SCORE_NEW;
}

void GetHiScores(byte time,Map *map)
{
	int i,me,num;
	score_t *list;
	dword lvlCheckSum;

	for(i=0;i<4;i++)
	{
		saved[i]=SCORE_ABSENT;			// indicate they don't exist
	}

	if(!config.hiscores)
		return;

	if(!map)
		return;

	lvlCheckSum=ChecksumMap(map);
	me=0;
	if(time)
	{
		num=numTimes;
		list=hiTime;
	}
	else
	{
		num=numScores;
		list=hiScore;
	}
	for(i=0;i<num;i++)
	{
		if(list[i].levelChecksum==lvlCheckSum)
		{
			saved[me]=SCORE_SAVED;
			these[me++]=list[i];
			if(me==4)
				break;	// got em all... if there are more, I just don't even want to know
		}
	}
}

void SortUsScores(byte time)
{
	int i;
	byte t;
	score_t tmp;
	byte flip;

	flip=1;
	while(flip)
	{
		flip=0;
		for(i=0;i<3;i++)
		{
			// if this score should be lower in the list, or this is a non-score and the one below it IS a score, then swap
			if((!time && these[i].score<these[i+1].score && saved[i]!=SCORE_ABSENT && saved[i+1]!=SCORE_ABSENT) ||
			   (time && these[i].score>these[i+1].score && saved[i]!=SCORE_ABSENT && saved[i+1]!=SCORE_ABSENT) ||
			   (saved[i]==SCORE_ABSENT && saved[i+1]!=SCORE_ABSENT))
			{
				// swap them
				tmp=these[i];
				these[i]=these[i+1];
				these[i+1]=tmp;
				t=saved[i];
				saved[i]=saved[i+1];
				saved[i+1]=t;
				flip=1;
			}
		}
	}
	// now the list is sorted, best first
}

byte CheckHiScore(dword score,Map *map)
{
	int i,me;

	GetHiScores(0,map);
	CreateScore(score,map);
	SortUsScores(0);
	if(saved[3]==SCORE_NEW)	// if this score came in 4th place
		return 0;	// no high score
	else
	{
		// came in somewhere 1-3!
		for(i=0;i<3;i++)
			if(saved[i]==SCORE_NEW)
				me=i;

		if(saved[3]!=SCORE_ABSENT)
		{
			// must replace an existing score
			for(i=0;i<numScores;i++)
			{
				if(hiScore[i].levelChecksum==these[3].levelChecksum && hiScore[i].score==these[3].score)
				{
					hiScore[i]=these[me];	// copy the new score over it!
					break;
				}
			}
		}
		else
		{
			// must append to the overall list, I replace nobody
			AppendScore(&these[me]);
		}
		if(me==0)
			return RECORD_SCORE;
		else if(me==1)
			return RECORD_SCORE2;
		else
			return RECORD_SCORE3;
	}
}

byte CheckHiTime(dword time,Map *map)
{
	int i,me;

	GetHiScores(1,map);
	CreateScore(time,map);
	SortUsScores(1);
	if(saved[3]==SCORE_NEW)	// if this time came in 4th place
		return 0;	// no high time
	else
	{
		// came in somewhere 1-3!
		for(i=0;i<3;i++)
			if(saved[i]==SCORE_NEW)
				me=i;

		if(saved[3]!=SCORE_ABSENT)
		{
			// must replace an existing score
			for(i=0;i<numTimes;i++)
			{
				if(hiTime[i].levelChecksum==these[3].levelChecksum && hiTime[i].score==these[3].score)
				{
					hiTime[i]=these[me];	// copy the new time over it!
					break;
				}
			}
		}
		else
		{
			// must append to the overall list, I replace nobody
			AppendTime(&these[me]);
		}
		if(me==0)
			return RECORD_TIME;
		else if(me==1)
			return RECORD_TIME2;
		else
			return RECORD_TIME3;
	}
}

byte CheckRecords(dword score,dword time,Map *map)
{
	if(!config.hiscores)
		return 0;

	if(map->flags&MAP_HUB)
		return 0;

	return CheckHiScore(score,map)|CheckHiTime(time,map);
}

byte TryHighScore(void)
{
	byte gotRecords;
	dword trueScore;
	float destructBonus,diffBonus;

	if(!config.hiscores)
		return 0;

	if(player.totalEnemies==0)
	{
		destructBonus=1.5f;
	}
	else
		destructBonus=0.5f+((float)player.enemiesSlain/(float)player.totalEnemies);

	if (profile.difficulty == DIFFICULTY_NORMAL)
		diffBonus = 0.75f;
	else if (profile.difficulty == DIFFICULTY_HARD)
		diffBonus = 1.0f;
	else if (profile.difficulty == DIFFICULTY_LUNATIC)
		diffBonus = 1.25f;
	static_assert(MAX_DIFFICULTY == 3, "Must handle new difficulty here");

	trueScore=player.score;
	trueScore+=player.bestCombo*10;
	if(trueScore>(dword)(player.clock/10))
		trueScore-=(player.clock/10);
	else
		trueScore=0;

	trueScore=(int)((float)trueScore*diffBonus);
	trueScore=(int)((float)trueScore*destructBonus);
	if(player.perfect)
		trueScore=(int)((float)trueScore*1.5f);

	if(!editing && verified)
		gotRecords=CheckRecords(trueScore,player.clock,curWorld.map[player.levelNum]);
	else
		gotRecords=0;

	if(gotRecords)
		SaveHiScores();

	return gotRecords;
}

byte GetTopScores(score_t *winners,Map *map)
{
	int i,cnt;

	GetHiScores(0,map);
	SortUsScores(0);
	cnt=0;
	for(i=0;i<3;i++)
		if(saved[i]!=SCORE_ABSENT)
			cnt++;

	winners[0]=these[0];
	winners[1]=these[1];
	winners[2]=these[2];

	return cnt;
}

byte GetTopTimes(score_t *winners,Map *map)
{
	int i,cnt;

	GetHiScores(1,map);
	SortUsScores(1);
	cnt=0;
	for(i=0;i<3;i++)
		if(saved[i]!=SCORE_ABSENT)
			cnt++;

	winners[0]=these[0];
	winners[1]=these[1];
	winners[2]=these[2];

	return cnt;
}


void DeleteScores(Map *map)
{
	int i,j,done;
	dword chksum;

	if(!config.hiscores)
		return;

	if(map->flags&MAP_HUB)
		return;	// there are no scores

	chksum=ChecksumMap(map);

	done=0;
	while(!done)
	{
		done=1;
		for(i=0;i<numScores;i++)
		{
			if(hiScore[i].levelChecksum==chksum)	// this belongs to this map!
			{
				done=0;
				for(j=numScores-1;j>i;j--)
					hiScore[j]=hiScore[j-1];
				numScores--;
				hiScore=(score_t *)realloc(hiScore,sizeof(score_t)*numScores);
				break;
			}
		}
	}

	done=0;
	while(!done)
	{
		done=1;
		for(i=0;i<numTimes;i++)
		{
			if(hiTime[i].levelChecksum==chksum)	// this belongs to this map!
			{
				done=0;
				for(j=numTimes-1;j>i;j--)
					hiTime[j]=hiTime[j-1];
				numTimes--;
				hiTime=(score_t *)realloc(hiTime,sizeof(score_t)*numTimes);
				break;
			}
		}
	}
}

void EraseHighScores(world_t *world)
{
	int i;

	if(!config.hiscores)
		return;

	if(!world)
		return;

	for(i=0;i<world->numMaps;i++)
	{
		DeleteScores(world->map[i]);
	}
	SaveHiScores();
}

char *GetScoresToUpload(int *num,int *scoreSize)
{
	char *data=0;
	int i,pos;

	SetupCrypto(3574,858734,298437);

	*num=0;
	for(i=0;i<numScores;i++)
	{
		if(hiScore[i].beenUploaded==0 && !strcmp(hiScore[i].name,profile.name))
		{
			(*num)++;
		}
	}

	*scoreSize=scoreLen+1;

	data=(char *)malloc((*scoreSize)*((*num)));

	pos=0;
	for(i=0;i<numScores;i++)
	{
		if(hiScore[i].beenUploaded==0 && !strcmp(hiScore[i].name,profile.name))
		{
			hiScore[i].beenUploaded=1;
			EncryptScore(&hiScore[i]);
			strcpy(&data[pos*(*scoreSize)],enc_score);
			pos++;
		}
	}

	ExitCrypto();

	return data;
}

char *GetTimesToUpload(int *num,int *scoreSize)
{
	char *data=0;
	int i,pos;

	SetupCrypto(3574,858734,298437);

	*num=0;
	for(i=0;i<numTimes;i++)
	{
		if(hiTime[i].beenUploaded==0 && !strcmp(hiTime[i].name,profile.name))
		{
			(*num)++;
		}
	}

	*scoreSize=scoreLen+1;

	data=(char *)malloc((*scoreSize)*((*num)));
	pos=0;
	for(i=0;i<numTimes;i++)
	{
		if(hiTime[i].beenUploaded==0 && !strcmp(hiTime[i].name,profile.name))
		{
			EncryptScore(&hiTime[i]);
			hiTime[i].beenUploaded=1;
			strcpy(&data[pos*(*scoreSize)],enc_score);
			pos++;
		}
	}
	ExitCrypto();

	return data;
}

int CountScoresToUpload(void)
{
	int total=0;
	int i;

	for(i=0;i<numTimes;i++)
	{
		if(hiTime[i].beenUploaded==0 && !strcmp(hiTime[i].name,profile.name))
			total++;
	}
	for(i=0;i<numScores;i++)
	{
		if(hiScore[i].beenUploaded==0 && !strcmp(hiScore[i].name,profile.name))
			total++;
	}
	return total;
}

void UnUpload(byte time,char *encrypted)
{
	score_t me;
	int i,num;
	score_t *list;

	SetupCrypto(3574,858734,298437);

	strcpy(enc_score,encrypted);
	DecryptScore(&me);

	if(time)
	{
		list=hiTime;
		num=numTimes;
	}
	else
	{
		list=hiScore;
		num=numScores;
	}

	for(i=0;i<num;i++)
	{
		if(list[i].scoreChecksum==me.scoreChecksum)
		{
			list[i].beenUploaded=0;	// unupload!
		}
	}
}

void ChangeHighScores(const char *oldName,const char *newName)
{
	int i;
	char s[48];

	for(i=0;i<numScores;i++)
	{
		if(!strcmp(hiScore[i].name,oldName))
			strcpy(hiScore[i].name,newName);
		ScoreChecksumNoMap(&hiScore[i]);
	}
	for(i=0;i<numTimes;i++)
	{
		if(!strcmp(hiTime[i].name,oldName))
			strcpy(hiTime[i].name,newName);
		ScoreChecksumNoMap(&hiTime[i]);
	}
	SaveHiScores();
	sprintf(s,"profiles/%s.prf",oldName);
	AppdataDelete(s);	// delete the old profile file
}

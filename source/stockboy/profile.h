#ifndef PROFILE_H
#define PROFILE_H

#include "mgldraw.h"
#include "game.h"

// the months
#define JANUARY		0
#define FEBRUARY	1
#define MARCH		2
#define APRIL		3
#define MAY			4
#define JUNE		5
#define JULY		6
#define AUGUST		7
#define SEPTEMBER	8
#define OCTOBER		9
#define NOVEMBER	10
#define DECEMBER	11
#define SMARCH		12

#define CHAR_SURF	0
#define CHAR_GOTH	1
#define CHAR_ROBOT	2
#define CHAR_HAPPY	3
#define CHAR_SANTA  4
#define CHAR_CHERUB 5
#define CHAR_SHROOM 6
#define CHAR_PIG	7
#define CHAR_FREAK	8
#define CHAR_HIPPIE 9
#define NUM_CHARS	10

#define MAX_PROFILES 8

#define PRF_NAME_LEN 16

#define MAX_SCORES	13*4

// different song choices
#define SONG_TITLE	0
#define SONG_MENU	1	// profile menu, stockroom menu, etc
#define SONG_PLAY	2	// randomly picks from these play songs during gameplay
#define SONG_PLAY2	3
#define SONG_PLAY3	4
#define SONG_PLAY4	5
#define SONG_PLAY5	6	// room for five of them, blank ones are ignored
#define NUM_SONGS	7

typedef struct profile_t
{
	char name[PRF_NAME_LEN];	// name of profile
	byte character;	// which character you're playing as
	byte color;		// color modifier
	char bright;	// brightness modifier
	byte training[10];	// have completed each

	byte sound,music,colorblind;

	byte keys[5];	// your key configuration

	byte bestBounce;	// biggest bounce combo
	byte empMonth[13];	// employee of the month awards for each month
	byte empMonth2[13];	// emp month for parallel
	byte empYear;		// employee of the year!
	byte empYear2;		// empYear for parallel
	byte zodiac[13];	// did the hidden goal in each week
	byte zodiac2[13];	// parallel universe hidden goals
	word stars;			// # of star points
	word starsLeft;		// # of star points unspent
	byte giftShop[16];	// purchased each giftshop item?
	byte starGot[NUM_GAMES][MAX_SCORES];	// what color star you have for every possible place
	char songChoice[NUM_SONGS][32];	// which song is assigned to each thing
} profile_t;

extern profile_t profile;

void InitProfile(void);
void ExitProfile(void);

byte LoadProfile(byte n);
byte SaveProfile(byte n);

byte FindOpenProfile(void);
void DefaultProfile(profile_t *p);

#endif
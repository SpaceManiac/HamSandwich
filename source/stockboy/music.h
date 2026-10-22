#ifndef MUSIC_H
#define MUSIC_H

#include "mgldraw.h"

#define SONG_LENGTH		1024
#define NUM_INSTRUMENTS	11
#define NUM_PITCHES		15
#define NUM_SEQS		32
#define SEQ_LENGTH		1024

#define SEQ_BLOCK_SIZE	32	// how many notes = 1 block of the sequence

// song play events
#define SP_NONE		0
#define SP_PLAY		1
#define SP_STOP		2

typedef struct songSeq_t
{
	char name[32];
	word length;		// the length of the sequence itself
	word playLength;	// the length of the play commands
	byte instrument;
	short volume;
	byte insFlags;
	byte note[NUM_PITCHES][SEQ_LENGTH];
	byte play[SONG_LENGTH];	// on/offs for this sequence
} songSeq_t;

typedef struct song_t
{
	word tempo;
	char name[32];
	char author[32];
	word length;
	byte numSeqs;
	songSeq_t *seq;
} song_t;

void Music_Init(void);
void Music_Exit(void);

byte Music_Load(char *fname);
byte Music_Save(char *fname);
void Music_Update(void);
void Music_Play(void);
void Music_Stop(void);
void Music_Scan(dword position,int seq);
void Music_Pause(void);
void Music_PlayNotes(dword when,byte seq);
void Music_MakeSong(void);
void Music_PlayNote(byte note,byte inst,int vol,byte sustain);
void Music_SetPlaySeq(int seq);	// to play one specific sequence rather than the song

byte *Music_GetSeq(byte seq);
byte Music_GetInsFlags(byte seq);
byte Music_GetInstrument(byte seq);
word Music_GetLength(int seq);
word Music_GetPos(int seq);
char *Music_GetTitle(void);
char *Music_GetAuthor(void);
song_t *Music_GetSong(void);

byte Music_GetSongName(char *fname,char *title,char *author);

void Music_Shift(int seq,dword start,int shift);
void Music_SetLength(int seq,word l);
void Music_SetInstrument(byte seq,byte type);
void Music_SetInsFlags(byte seq,byte flags);
void Music_New(void);

byte Music_AddSeq(void);
byte Music_CloneSeq(byte src);
byte Music_DelSeq(byte seq);
byte Music_MoveSeq(byte seq,char mv);
void Music_SetLengths(void);

void Music_Mute(byte seq,byte on);
byte Music_IsMuted(byte seq);
void Music_SetLoopSpot(word start,word spot);

#endif
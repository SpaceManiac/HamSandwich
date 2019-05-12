#ifndef MUSIC_H
#define MUSIC_H

#include "mgldraw.h"

// songs
#define SONG_UNDERWORLD	(240)
#define SONG_WINTER		(241)
#define SONG_CAVES		(242)
#define SONG_BOSS		(243)
#define SONG_TOWN		(244)
#define SONG_FACTORY	(245)

// bottom 3 bits of profile.musicMode say which list to use
#define MUSIC_OFFICIAL	0
#define MUSIC_PLAYLIST1 1
#define MUSIC_PLAYLIST2 2
#define MUSIC_PLAYLIST3 3
#define MUSIC_PLAYLIST4 4
#define MUSIC_LISTBITS	(1+2+4)

// the next 2 bits cover different play modes
#define MUSIC_PLAYMODE	(8+16)
#define MUSIC_RANDOM	(8)
#define MUSIC_SEQUENTIAL (16)
#define MUSIC_REPEAT	(0)

void PlaySong(char *fname);
void PlaySongForce(char *fname);
void StopSong(void);

void SetMusicVolume(int vol);

char *CurSongTitle(void);
void Song(byte w);

void UpdateMusic(void);

#endif

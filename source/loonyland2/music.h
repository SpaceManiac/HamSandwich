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

void PlaySong(char *fname);
void StopSong(void);

void SetMusicVolume(int vol);

char *CurSongTitle(void);
void Song(byte w);

#endif
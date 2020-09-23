#ifndef MUSIC_H
#define MUSIC_H

#include "mgldraw.h"
#include "hammusic.h"

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

void PlaySong(const char *fname);
void PlaySongForce(const char *fname);
void StopSong(void);

void SetMusicVolume(int vol);

const char *CurSongTitle(void);

void PickSongToPlay(void);
void PlayNextSong(void);
void PlayPrevSong(void);

void UpdateMusic(void);
bool CompareSongs(const char *fname); // if the same song is playing as the song they wish to load in...

#endif

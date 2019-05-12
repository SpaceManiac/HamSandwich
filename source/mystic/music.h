#ifndef MUSIC_H
#define MUSIC_H

#include "mgldraw.h"

void PlaySong(char *fname);
void PlaySongForce(char *fname);
void StopSong2(void);

void SetMusicVolume(int vol);

char *CurSongTitle(void);
void Song(byte w);

void UpdateMusic(void);

#endif

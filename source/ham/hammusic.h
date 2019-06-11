#ifndef HAMMUSIC_H
#define HAMMUSIC_H

#include "jamultypes.h"

void UpdateMusic();

void SetMusicVolume(int vol);

// Accepts full path, including leading "sound/" or "music/".
void PlaySongFile(const char* path);
void StopSong();
bool IsSongPlaying();

// Game-provided. Should call PlaySongFile or StopSong.
// If it doesn't, the current song will loop.
extern void ChooseNextSong();

extern bool ConfigMusicEnabled();

#endif  // HAMMUSIC_H

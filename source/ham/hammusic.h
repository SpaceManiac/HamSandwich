#ifndef HAMMUSIC_H
#define HAMMUSIC_H

#include "jamultypes.h"

void SetHamMusicEnabled(bool enabled);

void UpdateMusic();

void SetMusicVolume(int vol);

// Accepts full path, including leading "sound/" or "music/".
void PlaySongFile(const char* path);
void StopSong();
bool IsSongPlaying();

#endif  // HAMMUSIC_H

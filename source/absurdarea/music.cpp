#include "music.h"
#include "mgldraw.h"
#include "config.h"
#include "editor.h"
#include "hammusic.h"

byte lastSong=255;

void Song(byte w)
{
	char s[32];

	if(w==lastSong && IsSongPlaying())
		return;	// no need, it's already playing our song

#ifdef DIRECTORS
	if(player.var[VAR_COMMENTARY])
	{
		sprintf(s,"music/aacomm%02d.ogg",player.levelNum);
	}
	else
#endif
	sprintf(s,"music/aasong%03d.ogg",w-SONG_GUARDIAN);

	PlaySongFile(s);
#ifdef DIRECTORS
	if(player.var[VAR_COMMENTARY])
		lastSong=100+player.levelNum;
	else
#endif
	lastSong=w;
}

void ChooseNextSong()
{
#ifdef DIRECTORS
	if(player.var[VAR_COMMENTARY])
		StopSong();
#endif
}

bool ConfigMusicEnabled()
{
	return config.music;
}

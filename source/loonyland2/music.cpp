#include "music.h"
#include "mgldraw.h"
#include "config.h"
#include "editor.h"
#include "hammusic.h"

byte lastSong=255;

void Song(byte w)
{
	char s[32];

	if(w==lastSong)
		return;	// no need, it's already playing our song

#ifdef DIRECTORS
	if(player.var[VAR_COMMENTARY])
	{
		sprintf(s,"music/comm%02d.ogg",player.levelNum);
	}
	else
#endif
	sprintf(s,"music/song%03d.ogg",w-SONG_UNDERWORLD);

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

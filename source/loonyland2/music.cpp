#include "music.h"
#include "mgldraw.h"
#include <fmod.h>
#include "editor.h"
#include "player.h"

FSOUND_STREAM *curStream=NULL;
char curSongName[64];
int musVolume=255;
byte lastSong=255;
byte dontcallback=0;

signed char SongIsDone(FSOUND_STREAM *stream,void *buff,int len,int param)
{
#ifdef DIRECTORS
	if(player.var[VAR_COMMENTARY])
		return 0;
#endif
	if(!dontcallback)
		PlaySong(curSongName);

	return 0;	// ignores the return value
}

void Song(byte w)
{
	char s[32];

	if(w==lastSong)
		return;	// no need, it's already playing our song

#ifdef DIRECTORS
	if(player.var[VAR_COMMENTARY])
	{
		sprintf(s,"comm%02d.ogg",player.levelNum);
	}
	else
#endif
	sprintf(s,"song%03d.ogg",w-SONG_UNDERWORLD);

	PlaySong(s);
#ifdef DIRECTORS
	if(player.var[VAR_COMMENTARY])
		lastSong=100+player.levelNum;
	else
#endif
	lastSong=w;
}

void PlaySong(char *fname)
{
	char fullname[64];

	strcpy(curSongName,fname);
	sprintf(fullname,"music\\%s",fname);
	StopSong();
	curStream=FSOUND_Stream_OpenFile(fullname,FSOUND_NORMAL,0);
	if(curStream)
	{
		FSOUND_Stream_Play(config.numSounds,curStream);
		FSOUND_Stream_SetEndCallback(curStream,SongIsDone,0);
		FSOUND_SetVolume(config.numSounds,musVolume);
	}
}

void StopSong(void)
{
	dontcallback=1;
	lastSong=255;
	if(curStream)
	{
		FSOUND_Stream_Close(curStream);
		curStream=NULL;
	}
	dontcallback=0;
}

void SetMusicVolume(int vol)
{
	musVolume=vol;
	if(curStream)
	{
		FSOUND_SetVolume(config.numSounds,musVolume);
	}
}

char *CurSongTitle(void)
{
	return curSongName;
}

#include "music.h"
#include "mgldraw.h"
#include <fmod.h>
#include "progress.h"
#include "config.h"
#include "editor.h"

FSOUND_STREAM *curStream=NULL;
char curSongName[64];
int musVolume=255;
byte lastSong=255;
byte dontcallback=0;

void PickSongToPlay(void)
{
	if(!config.music)
		return;

	if(curStream==NULL)
	{
		PlaySongForce(curSongName);
	}
	else
	{
		if(!strcmp(curSongName,"hicksong.ogg"))
			PlaySongForce("unspeakable.ogg");
		else
		{
			FSOUND_Stream_Play(config.numSounds,curStream);	// just go ahead and repeat this song
			FSOUND_SetVolume(config.numSounds,musVolume);
			lastSong=255;
		}
	}
}

signed char SongIsDone(FSOUND_STREAM *stream,void *buff,int len,int param)
{
	if(!config.music)
		return 0;

	if(!dontcallback)
		PickSongToPlay();

	return 0;	// ignores the return value
}

void PlaySong(char *fname)
{
	char fullname[64];

	if(!config.music)
		return;

	if(!strcmp(curSongName,fname) && fname[0]!='\0')
		return;	// no need!

	strcpy(curSongName,fname);

	if(fname[0]=='\0')
	{
		StopSong();
		return;
	}
	sprintf(fullname,"music\\%s",fname);
	StopSong();
	strcpy(curSongName,fname);

	curStream=FSOUND_Stream_OpenFile(fullname,FSOUND_NORMAL,0);
	if(curStream)
	{
		FSOUND_Stream_Play(config.numSounds,curStream);
		FSOUND_Stream_SetEndCallback(curStream,SongIsDone,0);
		FSOUND_SetVolume(config.numSounds,musVolume);
	}
}

void PlaySongForce(char *fname)
{
	char fullname[64];

	if(!config.music)
		return;

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
	if(!config.music)
		return;

	dontcallback=1;
	if(curStream)
	{
		FSOUND_Stream_Close(curStream);
		curStream=NULL;
	}
	dontcallback=0;
	curSongName[0]='\0';
}

void SetMusicVolume(int vol)
{
	if(!config.music)
		return;

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

void PlayNextSong(void)
{
	byte pl;

	if(!config.music)
		return;

	if((profile.musicMode&MUSIC_LISTBITS)==MUSIC_OFFICIAL)
		return;	// can't

	pl=(profile.musicMode&MUSIC_LISTBITS)-1;

	lastSong++;
	if(lastSong>=profile.playList[pl].numSongs)
	{
		lastSong=0;
		if(profile.playList[pl].numSongs==0)
		{
			StopSong();
			return;
		}
	}
	PlaySongForce(&profile.playList[pl].song[lastSong*SONGNAME_LEN]);
}

void PlayPrevSong(void)
{
	byte pl;

	if(!config.music)
		return;

	if((profile.musicMode&MUSIC_LISTBITS)==MUSIC_OFFICIAL)
		return;	// can't

	pl=(profile.musicMode&MUSIC_LISTBITS)-1;

	lastSong--;
	if(lastSong>250)
	{
		lastSong=profile.playList[pl].numSongs-1;
		if(profile.playList[pl].numSongs==0)
		{
			StopSong();
			return;
		}
	}
	PlaySongForce(&profile.playList[pl].song[lastSong*SONGNAME_LEN]);
}

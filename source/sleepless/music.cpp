#include "music.h"
#include "mgldraw.h"
#include <allegro.h>
#include <logg.h>
#include "progress.h"
#include "config.h"
#include "editor.h"

LOGG_Stream *curStream=NULL;
char curSongName[64];
int musVolume=255;
byte lastSong=255;
byte dontcallback=0;
long musStart=0;

void PickSongToPlay(void)
{
	if(!config.music)
		return;

	if(!strcmp(curSongName,"hicksong.ogg"))
		PlaySongForce("unspeakable.ogg");
	else
		PlaySongForce(curSongName);
}

void UpdateMusic(void)
{
	if (config.music && curStream &&            // music is playing
			!logg_update_stream(curStream) &&   // we're on the last buffer
			(timeGetTime() - musStart)/1000.0f >= (float)curStream->len / (float)curStream->freq) // timewise, the whole song has played
		if (!dontcallback)
			PickSongToPlay();
}

void PlaySong(char *fname)
{
	char fullname[64];

	if(!config.music)
		return;

	if(!strcmp(curSongName,fname) && fname[0]!='\0')
		return;	// no need!

	if(fname[0]=='\0')
	{
		StopSong();
		return;
	}

	PlaySongForce(fname);
}

void PlaySongForce(char *fname)
{
	char fullname[64];

	if(!config.music)
		return;

	strcpy(curSongName,fname);
	sprintf(fullname,"music/%s",fname);
	StopSong();
	curStream=logg_get_stream(fullname, musVolume, 128, 0);
	if(curStream)
	{
		musStart = timeGetTime();
		UpdateMusic();
	}
}

void StopSong(void)
{
	if(!config.music)
		return;

	dontcallback=1;
	if(curStream)
	{
		logg_destroy_stream(curStream);
		curStream=NULL;
	}
	dontcallback=0;
}

void SetMusicVolume(int vol)
{
	if(!config.music)
		return;

	musVolume=vol;
	if(curStream)
	{
		curStream->volume = musVolume;
		voice_set_volume(curStream->audio_stream->voice, musVolume);
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

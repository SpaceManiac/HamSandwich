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

void PickSongToPlay(void)
{
	byte pl,sng;

	if(!config.music)
		return;

	if((profile.musicMode&MUSIC_LISTBITS)==MUSIC_OFFICIAL ||
	   (profile.musicMode&MUSIC_PLAYMODE)==MUSIC_REPEAT)
    {
        if((profile.musicMode&MUSIC_LISTBITS)==MUSIC_OFFICIAL)
            PlaySongForce(curSongName);
        else
        {
            if(profile.playList[(profile.musicMode&MUSIC_LISTBITS)-1].numSongs==0)
            {
                StopSong();
                return;
            }
            lastSong=0;
            PlaySongForce(&profile.playList[(profile.musicMode&MUSIC_LISTBITS)-1].song[0]);
        }
	}
	else
	{
		pl=(profile.musicMode&MUSIC_LISTBITS)-1;
		// not playing the official songs, and not on repeat
		if((profile.musicMode&MUSIC_PLAYMODE)==MUSIC_SEQUENTIAL)
		{
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
		else	// on random
		{
			if(profile.playList[pl].numSongs==0)
			{
				StopSong();
				return;
			}
			if(profile.playList[pl].numSongs>1)
			{
				sng=lastSong;
				while(sng==lastSong)
					sng=Random(profile.playList[pl].numSongs);
				lastSong=sng;
			}
			else
				lastSong=0;
			PlaySongForce(&profile.playList[pl].song[lastSong*SONGNAME_LEN]);
		}
	}
}

void UpdateMusic(void)
{
	if (config.music && curStream && !logg_update_stream(curStream))
		if (!dontcallback)
			PickSongToPlay();
}

void PlaySong(char *fname)
{
	if(!config.music)
		return;

	if((profile.musicMode&MUSIC_LISTBITS)!=MUSIC_OFFICIAL && !editing)
	{
		// if not playing the official music, we ignore the song asked for
		if(curStream==NULL)
		{
			// but we need to play something, since there are no songs playing at all
			PickSongToPlay();
			return;
		}
		else
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
	sprintf(fullname,"music\\%s",fname);
	StopSong();
	curStream=logg_get_stream(fullname, musVolume, 128, 0);
	if(curStream)
		UpdateMusic();
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

#include "music.h"
#include "mgldraw.h"
#include "progress.h"
#include "config.h"
#include "editor.h"
#include "hammusic.h"

char curSongName[64];
byte lastSong=255;

void ChooseNextSong(void)
{
	if(!config.music)
		return;

	if((profile.musicMode&MUSIC_LISTBITS)==MUSIC_OFFICIAL ||
	   (profile.musicMode&MUSIC_PLAYMODE)==MUSIC_REPEAT)
	{
		// official or repeat mode: just replay this song again

		if((profile.musicMode&MUSIC_LISTBITS)==MUSIC_OFFICIAL)
			PlaySongForce(curSongName);
		else
		{
			// in repeat mode we just loop the first song of the playlist
			// it's not very useful honestly
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
		byte pl=(profile.musicMode&MUSIC_LISTBITS)-1;
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
				byte sng=lastSong;
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

void PlaySong(const char *fname)
{
	if(!config.music)
		return;

	if((profile.musicMode&MUSIC_LISTBITS)!=MUSIC_OFFICIAL && !editing)
	{
		// if not playing the official music, we ignore the song asked for
		if(!IsSongPlaying())
		{
			// but we need to play something, since there are no songs playing at all
			ChooseNextSong();
		}
		return;
	}

	PlaySongForce(fname);
}

void PlaySongForce(const char *fname)
{
	char fullname[64];

	if(!config.music)
		return;

	strcpy(curSongName,fname);
	sprintf(fullname,"music/%s",fname);
	PlaySongFile(fullname);
}

const char *CurSongTitle(void)
{
	return curSongName;
}

bool ConfigMusicEnabled()
{
	return config.music;
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

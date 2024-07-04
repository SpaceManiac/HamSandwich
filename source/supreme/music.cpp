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

	if(curSongName != fname)
		strcpy(curSongName,fname);

	if (!strcasecmp(fname, "006cavernboss.ogg"))
	{
		/*
			The following levels are victims of an old bug in legacyload that
			assigned the wrong song filename, or of selecting the wrong song
			after working around that bug by creating the missing file.

			Dead.dlw | "Dead". . .or "Alive"? | 5 | A Walk In The Park
			HalloweenPOP.dlw | Halloween Horror - TG | 12 | The 4 Zombies of the Apocalypse
			HalloweenPOP.dlw | Halloween Horror - TG | 13 | Zombies of the Apocalypse II
			HauntedH.dlw | Haunted House | 1 | Darkness
			House.dlw | Weird House | 6 | SSSHHRRRREEAAAKKKKKKK!!!!!!!!!!
			House.dlw | Weird House | 9 | SSSHHRRRREEAAAKKK!!!! 2
			Looniesgooniesbaboonies.dlw | Loonies, Goonies, & Baboonies | 6 | Dr. Loony's Goonies
			Spaced.dlw | Spaced Station | 3 | The Robo Factory
			caverns.dlw | Cavernous Caves | 7 | The Bad Place
			excellent.dlw | Bouapha's Excellent Expedition | 4 | Metamorphosis Madness
			excellent.dlw | Bouapha's Excellent Expedition | 13 | Alienated!
			hh14sm.dlw | Halloween Horror 14 - SM | 3 | Son of a Lich
			interluna.dlw | Intergalactic Lunacy | 2 | Alien Robotics
			junglequest.dlw | Jungle Quest | 7 | Fugumogo Village
			justdeserts.dlw | Just Deserts | 14 | BRAAIINNSS!!
			lavacave.dlw | Lava Cave | 3 | Zoid Pit
			mansion.dlw | The Haunted Mansion | 14 | Countess Hausaufgaben
			splitlevellunacy.dlw | Split Level Lunacy | 6 | Let's Split
			splitlevellunacy.dlw | Split Level Lunacy | 19 | Split Decision

			Actual legacy worlds were fixed by legacyload being fixed.

			Jubilee.dlw | Queen's Jubilee | 5 | The Death Squads
			Jubilee.dlw | Queen's Jubilee | 6 | Robot Wars!
			Queenrevenge.dlw | The Queen's Revenge | 0 | The Queen's Revenge
			landofoz.dlw | The Land of Oz | 7 | The Tin Man
			loony.dlw | Lair of the Lunatic | 3 | The big,green,lava Octapus
			planetJ.dlw | Planet J | 4 | Football!!!!!
			spookycastle2.dlw | Return to Spooky Castle | 9 | The Ancient Evil
			spookycastle2.dlw | Return to Spooky Castle | 14 | The Lair
		*/
		PlaySongFile("music/006cavesboss.ogg");
	}
	else if (fname[0])
	{
		sprintf(fullname,"music/%s",fname);
		PlaySongFile(fullname);
	}
	else
		StopSong();
}

const char *CurSongTitle(void)
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

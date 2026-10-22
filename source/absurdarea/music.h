#ifndef MUSIC_H
#define MUSIC_H

#include "jamulsound.h"

// songs
#define SONG_GUARDIAN		(240)
#define SONG_STORAGE		(241)
#define SONG_COREALT		(242)
#define SONG_BOSSALT		(243)
#define SONG_STORY			(244)
#define SONG_TITLE			(245)
#define SONG_BOSSFIGHT		(246)
#define SONG_TETRON			(247)
#define SONG_NATCAVES		(248)
#define SONG_CORE			(249)
#define SONG_CONTROL		(250)
#define SONG_LIGHTFACT		(251)
#define SONG_NEWCAVES		(252)
#define SONG_WARFACT		(253)
#define SONG_OLDBASE		(254)
#define SONG_ANNIHILATOR	(255)
//#define SONG_UNDERWORLD	(240)
//#define SONG_WINTER		(241)
//#define SONG_CAVES		(242)
//#define SONG_BOSS		(243)
//#define SONG_TOWN		(244)
//#define SONG_FACTORY	(245)

void Song(byte w);
void ChooseNextSong();

#endif

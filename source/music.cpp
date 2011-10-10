#include "music.h"
#include "mgldraw.h"
#include <logg.h>

#define	CDAUDIO_CDPLAYER_STRING			( "CD Player" )
#define	CDAUDIO_CDPLAYER_LENGTH			( 10 )

byte currentMode;
int currentVoice;
SAMPLE* currentMusic;
int trackNum;

byte MusicInit(void)
{
    currentMode = CD_OFF;
    currentVoice = -1;
    currentMusic = NULL;
	return 1;
}

void MusicExit(void)
{
}


// ----------------------------------------------------------------------------------
// LOGG AUDIO STUFF

void CDPlay(int track) {
    if (trackNum == track && voice_check(currentVoice) == currentMusic) {
        return; // Already playing that track
    }

    char buf[32];
    sprintf(buf, "sound/mus%03d.ogg", track);
    trackNum = track;
    if (currentMusic) destroy_sample(currentMusic);
    currentMusic = logg_load(buf);
    currentVoice = play_sample(currentMusic, 128, 128, 1000, 0);
}

void CDNeedsUpdating(void) {}

#define CD_OFF		  0
#define CD_LOOPTRACK  1	// continuously loop the current track
#define CD_INTROLOOP  2	// plays the chosen track, then loops the next one
#define CD_RANDOM	  3 // after current track, jump to any other at random
#define CD_NORMAL	  4 // just keep playing the tracks in order, loops at end of CD to beginning

void CDPlayerUpdate(byte mode) {
    bool isPlaying = currentMusic != NULL && voice_check(currentVoice) == currentMusic;
    bool modeChanged = currentMode != mode;
    currentMode = mode;

    if (!isPlaying || modeChanged) {
        switch(mode) {
            case CD_OFF:
            default:
                if (isPlaying) {
                    CDStop();
                }
                break;
            case CD_LOOPTRACK:
                CDPlay(trackNum);
                break;
            case CD_INTROLOOP:
                CDPlay(trackNum + 1);
                currentMode = CD_LOOPTRACK;
                break;
            case CD_RANDOM:
                CDPlay(3 + MGL_random(15));
                break;
            case CD_NORMAL:
                if (!isPlaying) {
                    int newTrack = trackNum + 1;
                    if (newTrack > 18) newTrack = 3;
                    CDPlay(newTrack);
                }
                break;
        }
    }
}

void CDStop(void) {
    if (currentMusic) {
        stop_sample(currentMusic);
        destroy_sample(currentMusic);
        currentMusic = NULL;
        trackNum = 0;
    }
}

byte CDLoaded(void) { return 1; }

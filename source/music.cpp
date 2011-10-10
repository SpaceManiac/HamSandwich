#include "music.h"
#include "mgldraw.h"
#include <logg.h>

#define	CDAUDIO_CDPLAYER_STRING			( "CD Player" )
#define	CDAUDIO_CDPLAYER_LENGTH			( 10 )

byte currentMode;
LOGG_Stream* stream;
int trackNum;

byte MusicInit(void)
{
    currentMode = CD_OFF;
    stream = NULL;
	return 1;
}

void MusicExit(void)
{
}


// ----------------------------------------------------------------------------------
// LOGG AUDIO STUFF

void CDPlay(int track) {
    if (trackNum == track && !logg_update_stream(stream)) {
        return; // Already playing that track
    }

    char buf[32];
    sprintf(buf, "sound/mus%03d.ogg", track);
    trackNum = track;
    if (stream != NULL) logg_destroy_stream(stream);
    stream = logg_get_stream(buf, 128, 128, 0);
}

void CDNeedsUpdating(void) {}

void CDPlayerUpdate(byte mode) {
    bool isPlaying = logg_update_stream(stream);
    bool modeChanged = currentMode != mode;
    currentMode = mode;

    if (!isPlaying || modeChanged) {
        switch(currentMode) {
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
    printf("CDStop()\n");
    if (stream) {
        logg_destroy_stream(stream);
        stream = NULL;
        trackNum = 0;
    }
}

byte CDLoaded(void) { return 1; }

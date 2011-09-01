#include "music.h"
#include "mgldraw.h"

#define	CDAUDIO_CDPLAYER_STRING			( "CD Player" )
#define	CDAUDIO_CDPLAYER_LENGTH			( 10 )

dword CDError=0;
dword CDDevice=0;
byte  CDStarted=FALSE;
word  chosenTrack;
byte  introMode;	// for INTROLOOP mode, it needs to know if it is in the intro song or the loop song
static byte needsUpdating;

byte MusicInit(void)
{
	needsUpdating=0;
	if(!InitCDPlayer())
		return 0;

	return 1;
}

void MusicExit(void)
{
	ExitCDPlayer();
}


// ----------------------------------------------------------------------------------
// CD AUDIO STUFF
// currently the only thing music does, but maybe midis will be added later

byte InitCDPlayer(void)
{
	MCI_OPEN_PARMS	openParms;
	MCI_SET_PARMS	setParms;
	static char deviceType[]="cdaudio";

	ShutoffWindowsCD();

	memset(&openParms,0,sizeof(MCI_OPEN_PARMS));
	openParms.lpstrDeviceType=deviceType;

	CDError=mciSendCommand(0,MCI_OPEN,MCI_OPEN_TYPE,(dword)&openParms);

	if(CDError)
		return 0;

	CDDevice=openParms.wDeviceID;

	memset(&setParms,0,sizeof(MCI_SET_PARMS));
	setParms.dwTimeFormat=MCI_FORMAT_TMSF;

	CDError=mciSendCommand(CDDevice,MCI_SET,MCI_SET_TIME_FORMAT,(dword)&setParms);

	if(CDError)
	{
		mciSendCommand(CDDevice,MCI_CLOSE,0,0);
		return 0;
	}

	CDStarted=1;
	chosenTrack=0;
	introMode=0;

	return 1;
}

void ExitCDPlayer(void)
{
	if(!CDStarted)
		return;

	CDStop();

	mciSendCommand(CDDevice,MCI_CLOSE,0,0);
	CDStarted=0;
}

byte CDLoaded(void)
{
	MCI_STATUS_PARMS status;

	if(!CDStarted)
	{
		CDStarted=InitCDPlayer();
		if(!CDStarted)
			return 0;
	}

	memset(&status,0,sizeof(MCI_STATUS_PARMS));
	status.dwItem=MCI_STATUS_MEDIA_PRESENT;

	CDError=mciSendCommand(CDDevice,MCI_STATUS,MCI_STATUS_ITEM,(dword)&status);

	if(CDError)
	{
		mciSendCommand(CDDevice,MCI_CLOSE,0,0);
		CDStarted=0;
		return 0;
	}
	return (byte)status.dwReturn;
}

dword CDCurrentTrack(void)
{
	MCI_STATUS_PARMS status;

	if(!CDStarted)
		return 0;

	memset(&status,0,sizeof(MCI_STATUS_PARMS));
	status.dwItem=MCI_STATUS_CURRENT_TRACK;

	CDError=mciSendCommand(CDDevice,MCI_STATUS,MCI_STATUS_ITEM,(dword)&status);

	if(CDError)
	{
		mciSendCommand(CDDevice,MCI_CLOSE,0,0);
		CDStarted=0;
		return 0;
	}
	return status.dwReturn;
}

dword CDTrackCount(void)
{
	MCI_STATUS_PARMS status;

	if(!CDStarted)
		return 0;

	memset(&status,0,sizeof(MCI_STATUS_PARMS));
	status.dwItem=MCI_STATUS_NUMBER_OF_TRACKS;

	CDError=mciSendCommand(CDDevice,MCI_STATUS,MCI_STATUS_ITEM,(dword)&status);

	if(CDError)
	{
		mciSendCommand(CDDevice,MCI_CLOSE,0,0);
		CDStarted=0;
		return 0;
	}
	return status.dwReturn;
}

byte CDIsPlaying(void)
{
	MCI_STATUS_PARMS status;

	if(!CDStarted)
		return 0;

	memset(&status,0,sizeof(MCI_STATUS_PARMS));
	status.dwItem=MCI_STATUS_MODE;

	CDError=mciSendCommand(CDDevice,MCI_STATUS,MCI_STATUS_ITEM,(dword)&status);

	if(CDError)
	{
		mciSendCommand(CDDevice,MCI_CLOSE,0,0);
		CDStarted=0;
		return 0;
	}
	return (byte)(status.dwReturn==MCI_MODE_PLAY);
}

void CDPlay(int track)
{
	MCI_PLAY_PARMS play;
	int count;

	if(!CDLoaded())
		return;

	if(track==0)
	{
		chosenTrack=0;
		CDStop();
		return;
	}

	count=(int)CDTrackCount();

	if(count==1)	// only 1 track is a bad sign that it's probably data, so don't play it
		return;

	if(track>count)
	{
		// illegal tracks get replaced by random tracks
		track=MGL_random((word)count)+1;
	}

	memset(&play,0,sizeof(MCI_PLAY_PARMS));
	play.dwFrom=MCI_MAKE_TMSF(track,0,0,0);
	play.dwTo=MCI_MAKE_TMSF(track+1,0,0,0);	// play until the next track
	play.dwCallback=(dword)MGLGetHWnd();
	if(track==count)	// don't go "to" if it's the last, just end at the end
		CDError=mciSendCommand(CDDevice,MCI_PLAY,MCI_FROM|MCI_NOTIFY,(dword)&play);
	else
		CDError=mciSendCommand(CDDevice,MCI_PLAY,MCI_FROM|MCI_TO|MCI_NOTIFY,(dword)&play);

	chosenTrack=(word)track;
	introMode=0;
	if(CDError)
	{
		mciSendCommand(CDDevice,MCI_CLOSE,0,0);
		CDStarted=0;
		chosenTrack=0;
	}
}

void CDStop(void)
{
	MCI_GENERIC_PARMS stop;

	if(!CDStarted)
		return;

	memset(&stop,0,sizeof(MCI_GENERIC_PARMS));

	CDError=mciSendCommand(CDDevice,MCI_STOP,0,(dword)&stop);

	if(CDError)
	{
		mciSendCommand(CDDevice,MCI_CLOSE,0,0);
		CDStarted=0;
	}
}

void CDNeedsUpdating(void)
{
	needsUpdating=1;
}

void CDPlayerUpdate(byte mode)
{
	int track;
	int count;

	if(!needsUpdating)
		return;
	needsUpdating=0;

	if(!CDLoaded())
		return;

	if(CDIsPlaying())
	{
		if(mode==CD_OFF)
			CDStop();
		return;	// nothing to do here
	}
	else switch(mode)
	{
		case CD_OFF:
			// the CD isn't playing, that's what you want
			break;
		case CD_LOOPTRACK:
			if(chosenTrack>0)
				CDPlay(chosenTrack);	// play it again, sam
			break;
		case CD_INTROLOOP:
			if(introMode==0)	// in the intro part
				CDPlay(chosenTrack+1);
			else
				CDPlay(chosenTrack);	// keep looping the non-intro part
			introMode=1;	// either way, you're on the non-intro part
			break;
		case CD_RANDOM:
			count=CDTrackCount();
			if(count)
			{
				track=(MGL_random((word)count))+1;
				if(track==chosenTrack)
					track++;	// don't repeat a song, PLEASE
				CDPlay(track);
			}
			break;
		case CD_NORMAL:
			CDPlay(chosenTrack+1);	// next song
			break;
	}
}

// Windows CD shutter-downer... doesn't actually work right
BOOL CALLBACK FindCDPlayer( HWND hwnd, LPARAM lParam )
{
	char	buffer[CDAUDIO_CDPLAYER_LENGTH];
	int		result;

	result = GetWindowText( hwnd, buffer, CDAUDIO_CDPLAYER_LENGTH );

	if( result == 0 )
		return TRUE;

	if( strcmp( buffer, CDAUDIO_CDPLAYER_STRING ) )
		return TRUE;

	// window is CDplayer, so tell it to quit
	PostMessage( hwnd, WM_QUIT, 1, 0 );

	return FALSE;
}

void ShutoffWindowsCD(void)
{
	EnumWindows((WNDENUMPROC)FindCDPlayer,0);
}

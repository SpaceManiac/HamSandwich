#include "jamulsound.h"
#include <mmsystem.h>
#include <dsound.h>
#include "mgldraw.h"

typedef struct soundbuf_t
{
	IDirectSoundBuffer *buf;
} soundbuf_t;

static IDirectSound *dsound=NULL;
static int dsoundBufferCount=0,nextOpenBuffer=0;
static soundbuf_t *soundbuf=NULL;
HWND dsoundHwnd;

sound_t playBuffer[MAX_SOUNDS_AT_ONCE];
int *soundHandle;

bool JamulSoundInit(HINSTANCE hInst,char *wndName, int numBuffers)
{
	WNDCLASS wc;
	char className[]="dsoundWindow";
	int i;

	// first thing needed is to make a window, so directsound can have it
	wc.style=0;
	wc.lpfnWndProc=MGLDraw_EventHandler;//DsoundWindowProc;
	wc.cbClsExtra=0;
	wc.cbWndExtra=0;
	wc.hInstance=hInst;
	wc.hIcon=LoadIcon(hInst,IDI_APPLICATION);
	wc.hCursor=LoadCursor(NULL,IDC_ARROW);
	wc.hbrBackground=NULL;
	wc.lpszMenuName=NULL;
	wc.lpszClassName=className;
	if(!RegisterClass(&wc))
		return FALSE;
	
	dsoundHwnd=CreateWindowEx(
		0,
		className,
		wndName,
		WS_POPUPWINDOW,
		0,
		0,
		0,
		0,
		NULL,
		NULL,
		hInst,
		NULL);

	if(!dsoundHwnd)
		return FALSE;

	if(DirectSoundCreate(NULL,&dsound,NULL)!=DS_OK)
		return FALSE;
	if(dsound->SetCooperativeLevel(dsoundHwnd, DSSCL_NORMAL)!=DS_OK)
	{
		dsound->Release();
		return FALSE;
	}
	dsoundBufferCount=numBuffers;
	soundbuf=(soundbuf_t*)calloc(sizeof(soundbuf_t)*(numBuffers+MAX_SOUNDS_AT_ONCE),1);
	if(soundbuf==NULL)
	{
		dsound->Release();
		return FALSE;
	}
	soundHandle=(int *)malloc(sizeof(int)*numBuffers);
	if(soundHandle==NULL)
	{
		dsound->Release();
		free(soundbuf);
		return FALSE;
	}
	for(i=0;i<numBuffers;i++)
		soundHandle[i]=-1;	// start them all empty

	nextOpenBuffer=0;

	GM_registerMainWindow(dsoundHwnd);

	for(i=0;i<MAX_SOUNDS_AT_ONCE;i++)
	{
		playBuffer[i].soundNum=-1;
		playBuffer[i].dsHandle=-1;
		playBuffer[i].flags=0;
	}
	return TRUE;
}

void JamulSoundDestroyBuffer(int which)
{
	if(soundbuf[which].buf)
	{
		if(JamulSoundIsPlaying(which))
			JamulSoundStop(which);
		soundbuf[which].buf->Release();
		soundbuf[which].buf=NULL;
		if(which<nextOpenBuffer)
			nextOpenBuffer=which;
	}
}

void JamulSoundExit(void)
{
	int i;
	
	if(soundbuf)
	{
		for(i=0;i<dsoundBufferCount;i++)
			JamulSoundDestroyBuffer(i);
		free(soundbuf);
	}
	if(dsound)
		dsound->Release();	
	dsound=NULL;
	DestroyWindow(dsoundHwnd);
}

int JamulSoundCreateBuffer(int bufferLen, WAVEFORMATEX wfmtx)
{
	DSBUFFERDESC dsbdesc;
	IDirectSoundBuffer* buffer=NULL;
	int bufnum;

	if(nextOpenBuffer>=dsoundBufferCount)
		return -1; // no more room

	// Set up buffer description.
	memset(&dsbdesc, 0, sizeof(DSBUFFERDESC));
	dsbdesc.dwSize = sizeof(DSBUFFERDESC);
	dsbdesc.dwFlags = DSBCAPS_STATIC; 
	dsbdesc.dwBufferBytes = bufferLen; 
	dsbdesc.lpwfxFormat = &wfmtx;

	// Create buffer.
	if(dsound->CreateSoundBuffer(&dsbdesc, &buffer, 0)!=DS_OK)
		return -1;

	bufnum=nextOpenBuffer;
	soundbuf[nextOpenBuffer].buf=buffer;
	while(soundbuf[nextOpenBuffer].buf && nextOpenBuffer<dsoundBufferCount)
		nextOpenBuffer++;
	
	return bufnum;
}

// just a part of JamulSoundLoad yanked out for no real reason
bool JamulSoundWriteData(HMMIO file, int size, int bufnum)
{
	void* write1 = 0; 
	unsigned long length1 = 0;
	void* write2 = 0; 
	unsigned long length2 = 0;

	// Lock the buffer.
	if(soundbuf[bufnum].buf->Lock(0, size, &write1, &length1, &write2, &length2, 0)!=DS_OK)
		return FALSE;
	
	if (write1 > 0) 
	{
		// Copy the first bit of memory.
		if (mmioRead(file, (char*)write1, length1) != (long)length1) 
		{
			return FALSE;
		}
	}

	if (write2 > 0) 
	{
		// Copy the second bit of memory.
		if (mmioRead(file, (char*)write2, length2) != (long)length2) 
		{
			return FALSE;
		}
	}

	// Unlock the buffer.
	if(soundbuf[bufnum].buf->Unlock(write1, length1, write2, length2)!=DS_OK)
		return FALSE;
	
	// data pumped in!
	return TRUE;
}

// returns the number of the position in which it stored the sound (later use JamulSoundPlay(n) to play
// sound #n.
// -1 means it failed.
int JamulSoundLoad(char *fname)
{
	HMMIO file;
	MMCKINFO parent,child;
	WAVEFORMATEX wfmtx;
	int bufnum;

	file=mmioOpen(fname, 0, MMIO_READ | MMIO_ALLOCBUF);
	if(!file)
	{ 
		return -1;
	}

	// Set up the specifer to find the wave data.
	
	memset(&parent, 0, sizeof(MMCKINFO));
	parent.fccType = mmioFOURCC('W', 'A', 'V', 'E');

	if (mmioDescend(file, &parent, 0, MMIO_FINDRIFF)) 
	{
		mmioClose(file, 0);
		return -1;
	}

	// Set up the specifer to find the fmt data.
	memset(&child, 0, sizeof(MMCKINFO));
	child.fccType = mmioFOURCC('f', 'm', 't', ' ');

	if (mmioDescend(file, &child, &parent, 0)) 
	{
		mmioClose(file, 0);
		return -1;
	}

	// Read the format.
	if (mmioRead(file, (char*)&wfmtx, sizeof(wfmtx)) != sizeof(wfmtx)) 
	{
		mmioClose(file, 0);
		return -1;
	}

	// Make sure the wave data is the right format.
	if (wfmtx.wFormatTag != WAVE_FORMAT_PCM) 
	{
		mmioClose(file, 0);
		return -1;
	}

	// Pop back up a level.
	if (mmioAscend(file, &child, 0)) 
	{
		mmioClose(file, 0);
		return -1;
	}

	child.ckid = mmioFOURCC('d', 'a', 't', 'a');

	// And down to the data chunk.
	if (mmioDescend(file, &child, &parent, MMIO_FINDCHUNK)) 
	{
		mmioClose(file, 0);
		return -1;
	}

	// Create the buffer.
	bufnum=JamulSoundCreateBuffer(child.cksize, wfmtx);

	if (bufnum==-1) 
	{
		mmioClose(file, 0);
		return -1;
	}

	// Write the data.
	if (!JamulSoundWriteData(file, child.cksize,bufnum)) 
	{
		soundbuf[bufnum].buf->Release();
		soundbuf[bufnum].buf=NULL;
		nextOpenBuffer=bufnum;
		mmioClose(file, 0);
		return -1;
	}

	// yay it worked
	mmioClose(file, 0);
	return bufnum;
}

// loads in a WAV file.  If successful, it takes the WAV file and stores it in memory in
// the format JamulSoundGetFromMemory requires.
byte *JamulSoundLoadToMemory(char *fname)
{
	HMMIO file;
	byte *buf;
	MMCKINFO parent,child;
	WAVEFORMATEX wfmtx;

	file=mmioOpen(fname, 0, MMIO_READ | MMIO_ALLOCBUF);
	if(!file)
	{ 
		return NULL;
	}

	// Set up the specifer to find the wave data.
	
	memset(&parent, 0, sizeof(MMCKINFO));
	parent.fccType = mmioFOURCC('W', 'A', 'V', 'E');

	if (mmioDescend(file, &parent, 0, MMIO_FINDRIFF)) 
	{
		mmioClose(file, 0);
		return NULL;
	}

	// Set up the specifer to find the fmt data.
	memset(&child, 0, sizeof(MMCKINFO));
	child.fccType = mmioFOURCC('f', 'm', 't', ' ');

	if (mmioDescend(file, &child, &parent, 0)) 
	{
		mmioClose(file, 0);
		return NULL;
	}

	// Read the format.
	if (mmioRead(file, (char*)&wfmtx, sizeof(wfmtx)) != sizeof(wfmtx)) 
	{
		mmioClose(file, 0);
		return NULL;
	}

	// Make sure the wave data is the right format.
	if (wfmtx.wFormatTag != WAVE_FORMAT_PCM) 
	{
		mmioClose(file, 0);
		return NULL;
	}

	// Pop back up a level.
	if (mmioAscend(file, &child, 0)) 
	{
		mmioClose(file, 0);
		return NULL;
	}

	child.ckid = mmioFOURCC('d', 'a', 't', 'a');

	// And down to the data chunk.
	if (mmioDescend(file, &child, &parent, MMIO_FINDCHUNK)) 
	{
		mmioClose(file, 0);
		return NULL;
	}

	// allocate space for: a dword for the size of the data, a waveformat record, and the data
	buf=(byte *)malloc(sizeof(DWORD)+sizeof(WAVEFORMATEX)+child.cksize);
	if(buf==NULL)
		return NULL;
	// store the size
	memcpy(buf,&(child.cksize),sizeof(DWORD));
	// store the waveformat record
	memcpy(&(buf[sizeof(DWORD)]),&wfmtx,sizeof(WAVEFORMATEX));

	// and load and store the data
	mmioRead(file, (char*)&(buf[sizeof(DWORD)+sizeof(WAVEFORMATEX)]),child.cksize);

	mmioClose(file, 0);
	return buf;
}

// just a part of JamulSoundGetFromMemory yanked out for convenience
bool JamulSoundWriteDataFromMemory(byte *buf, int size, int bufnum)
{
	void* write1 = 0; 
	unsigned long length1 = 0;
	void* write2 = 0; 
	unsigned long length2 = 0;

	// Lock the buffer.
	if(soundbuf[bufnum].buf->Lock(0, size, &write1, &length1, &write2, &length2, 0)!=DS_OK)
		return FALSE;
	
	if (write1>0)
	{
		// Copy the first bit of memory.
		memcpy(write1,buf,length1);		
	}

	if (write2>0) 
	{
		// Copy the second bit of memory.
		memcpy(write2,&(buf[length1]),length2);		
	}

	// Unlock the buffer.
	if(soundbuf[bufnum].buf->Unlock(write1, length1, write2, length2)!=DS_OK)
		return FALSE;
	
	// data pumped in!
	return TRUE;
}

// returns the number of the position in which it stored the sound (later use JamulSoundPlay(n) to play
// sound #n.
// -1 means it failed.
int JamulSoundGetFromMemory(byte *buf)
{
	WAVEFORMATEX wfmtx;	
	DWORD *sizeptr;
	dword size;
	int bufnum;
	void* write1=0; 
	unsigned long length1=0;
	void* write2=0; 
	unsigned long length2=0;

	if(buf==NULL)
		return -1;

	sizeptr=(DWORD *)buf;
	size=*sizeptr;

	if(size==0)
		return -1;

	memcpy(&wfmtx,&(buf[sizeof(DWORD)]),sizeof(WAVEFORMATEX));

	// Make sure the wave data is the right format.
	if (wfmtx.wFormatTag != WAVE_FORMAT_PCM) 
	{		
		return -1;
	}

	// Create the buffer.
	bufnum=JamulSoundCreateBuffer(size, wfmtx);

	if (bufnum==-1) 
	{		
		return -1;
	}

	
	// Write the data.
	if (!JamulSoundWriteDataFromMemory(&buf[sizeof(DWORD)+sizeof(WAVEFORMATEX)], size,bufnum)) 
	{
		soundbuf[bufnum].buf->Release();
		soundbuf[bufnum].buf=NULL;
		nextOpenBuffer=bufnum;		
		return -1;
	}

	// yay it worked	
	return bufnum;
}

bool JamulSoundPlay(int which,long pan,long vol,byte playFlags)
{
	int flags;
	// Play the sound.
	if(playFlags&SOUND_LOOP) 
		flags=DSBPLAY_LOOPING;
	else
		flags=0;

	// if this copy is in use, can't play it
	if(JamulSoundIsPlaying(which))
	{
		if(playFlags&SOUND_CUTOFF)
		{
			if(soundbuf[which].buf->SetCurrentPosition(0)!=DS_OK)
				return FALSE;	// try to rewind, if it fails, can't play
			soundbuf[which].buf->SetPan(pan);
			soundbuf[which].buf->SetVolume(vol);
			return TRUE;	// it's already playing so ok
		}
		else
			return FALSE;	// can't play if it's playing
	}
	if(soundbuf[which].buf)
	{
		soundbuf[which].buf->SetPan(pan);
		soundbuf[which].buf->SetVolume(vol);
		if(soundbuf[which].buf->Play(0, 0, flags)!=DS_OK)
			return FALSE;	// couldn't play, sigh
	}

	/* possible loss of buffers... do I care?
	// See if the buffer was lost.
	if (hr == DSERR_BUFFERLOST) { hr = buffer->Restore(); }
	if (SUCCEEDED(hr)) { hr = buffer->Play(0, 0, flags); }
	*/
	return TRUE;
}

bool JamulSoundStop(int which)
{	
	if(!soundbuf[which].buf)
		return FALSE;
	if(soundbuf[which].buf->Stop()!=DS_OK)
		return FALSE;

	// Rewind the buffer.
	if(soundbuf[which].buf->SetCurrentPosition(0)!=DS_OK)
		return FALSE;
	return TRUE;
}

bool JamulSoundRewind(int which)
{	
	if(!soundbuf[which].buf)
		return FALSE;
	// Rewind the buffer.
	if(soundbuf[which].buf->SetCurrentPosition(0)!=DS_OK)
		return FALSE;
	return TRUE;
}

bool JamulSoundIsPlaying(int which)
{	
	unsigned long status;
	if(!soundbuf[which].buf)
		return FALSE; // ain't playin if it ain't there
	
	if(soundbuf[which].buf->GetStatus(&status)!=DS_OK)
		return FALSE; // call failed, guess it ain't playing

	if(status&DSBSTATUS_PLAYING)
		return TRUE;
	else
		return FALSE;
}

int JamulSoundCopy(int src)
{
	int w;
	if(nextOpenBuffer>=dsoundBufferCount)
		return -1;
	if(soundbuf[src].buf==NULL)
		return -1;
	if(dsound->DuplicateSoundBuffer(soundbuf[src].buf, &soundbuf[nextOpenBuffer].buf)!=DS_OK)
		return -1;

	w=nextOpenBuffer;
	while(soundbuf[nextOpenBuffer].buf && nextOpenBuffer<dsoundBufferCount)
		nextOpenBuffer++;
	return w;
}

// now here is all the big sound manager stuff, that allows multiple sounds at once

void JamulSoundUpdate(void)
{
	int i;

	for(i=1;i<MAX_SOUNDS_AT_ONCE;i++)
	{
		if(playBuffer[i].dsHandle!=-1 && playBuffer[i].flags&SND_PLAYING)
		{
			if(!JamulSoundIsPlaying(playBuffer[i].dsHandle))
			{
				playBuffer[i].flags&=(~SND_PLAYING);
			}
		}
	}
	if(playBuffer[0].dsHandle!=-1)
	{
		if(!JamulSoundIsPlaying(playBuffer[0].dsHandle))
		{
			JamulSoundRewind(playBuffer[0].dsHandle);
			JamulSoundPlay(playBuffer[0].dsHandle,playBuffer[0].pan,playBuffer[0].vol,0);
		}
	}
}

void JamulSoundStopSong(void)
{
	if(playBuffer[0].dsHandle!=-1)
	{
		JamulSoundStop(playBuffer[0].dsHandle);
		JamulSoundDestroyBuffer(playBuffer[0].dsHandle);	// slash & burn
		playBuffer[0].dsHandle=-1;
	}
}

void JamulSoundSong(int num,long vol)
{
	char txt[32];

	if(soundHandle[num]==-1)
	{
		sprintf(txt,"sound\\snd%03d.wav",num);
		soundHandle[num]=JamulSoundLoad(txt);
		if(soundHandle[num]==-1)
			return;	// can't play the sound, it won't load for some reason
	}

	if(playBuffer[0].soundNum!=num)	// if it was already playing that sound, don't waste time
	{
		playBuffer[0].soundNum=num;
		if(playBuffer[0].dsHandle!=-1)
		{
			JamulSoundDestroyBuffer(playBuffer[0].dsHandle);	// slash & burn
		}
		playBuffer[0].dsHandle=JamulSoundCopy(soundHandle[num]);
	}
	else
	{
		if(playBuffer[0].dsHandle==-1)
			playBuffer[0].dsHandle=JamulSoundCopy(soundHandle[num]);
		//JamulSoundRewind(playBuffer[0].dsHandle);
	}

	if(playBuffer[0].dsHandle==-1)
		return;	// can't play it
	playBuffer[0].priority=SND_MAXPRIORITY;
	playBuffer[0].pan=0;
	playBuffer[0].vol=vol;
	playBuffer[0].flags=SND_PLAYING;

	JamulSoundPlay(playBuffer[0].dsHandle,playBuffer[0].pan,playBuffer[0].vol,0);
}

void SetSongVolume(long vol)
{
	if(playBuffer[0].dsHandle!=-1)
		soundbuf[playBuffer[0].dsHandle].buf->SetVolume(vol);
}

void JamulSoundPurge(void)
{
	int i;

	for(i=0;i<dsoundBufferCount;i++)
	{
		if(soundHandle[i]!=-1)
		{
			JamulSoundDestroyBuffer(soundHandle[i]);
			soundHandle[i]=-1;
		}
	}
}

void GoPlaySound(int num,long pan,long vol,byte flags,int priority)
{
	char txt[32];
	int i,best;

	if(soundHandle[num]==-1)
	{
		sprintf(txt,"sound\\snd%03d.wav",num);
		soundHandle[num]=JamulSoundLoad(txt);
		if(soundHandle[num]==-1)
			return;	// can't play the sound, it won't load for some reason
	}

	priority+=vol;	// the quieter a sound, the lower the priority
	if(flags&SND_MAXPRIORITY)
		priority=MAX_SNDPRIORITY;

	if(flags&SND_ONE)
	{
		for(i=1;i<MAX_SOUNDS_AT_ONCE;i++)
			if(playBuffer[i].soundNum==num)
			{
				// if you want to cut it off, or it isn't playing, then start anew
				if((flags&SND_CUTOFF) || (!(playBuffer[i].flags&SND_PLAYING)))
				{
					playBuffer[i].pan=pan;
					playBuffer[i].vol=vol;
					playBuffer[i].flags=flags|SND_PLAYING;
					playBuffer[i].priority=priority;
					JamulSoundPlay(playBuffer[i].dsHandle,playBuffer[i].pan,playBuffer[i].vol,SOUND_CUTOFF);
					return;	// good job
				}
				else
					return;	// can't be played because can't cut it off
			}
		// if you fell through to here, it isn't playing, so go ahead as normal
	}
	best=-1;
	for(i=1;i<MAX_SOUNDS_AT_ONCE;i++)
	{
		if(playBuffer[i].soundNum==-1 || (!(playBuffer[i].flags&SND_PLAYING)))
		{
			best=i;
			break;	// can't beat that
		}
		if((playBuffer[i].priority<priority) || (playBuffer[i].soundNum==num && (flags&SND_CUTOFF)))
		{
			if(best==-1 || playBuffer[i].priority<playBuffer[best].priority)
				best=i;
		}
	}
	if(best==-1)
		return;	// sound is not worthy to be played

	if(playBuffer[best].soundNum!=num)	// if it was already playing that sound, don't waste time
	{
		playBuffer[best].soundNum=num;
		if(playBuffer[best].dsHandle!=-1)
		{
			JamulSoundDestroyBuffer(playBuffer[best].dsHandle);	// slash & burn
		}
		playBuffer[best].dsHandle=JamulSoundCopy(soundHandle[num]);
	}
	else
	{
		JamulSoundRewind(playBuffer[best].dsHandle);
	}

	if(playBuffer[best].dsHandle==-1)
		return;	// can't play it
	playBuffer[best].priority=priority;
	playBuffer[best].pan=pan;
	playBuffer[best].vol=vol;
	playBuffer[best].flags=flags|SND_PLAYING;

	JamulSoundPlay(playBuffer[best].dsHandle,playBuffer[best].pan,playBuffer[best].vol,0);
}

#include "music.h"
#include "sound.h"
#include "player.h"
#include "appdata.h"

static song_t curSong;
static dword musicClock;
static byte paused,playing;
static dword seqClock[NUM_SEQS];
static byte seqPlay[NUM_SEQS];
static byte seqMute[NUM_SEQS];
static int playSeq;
static byte songLoaded;
static dword lastUpdate;
static dword elapsed;
static word loopSpot,loopTo;

void FindSeqPos(int seq,dword position);

void Music_Init(void)
{
	int i;

	loopSpot=0;
	loopTo=0;
	playing=0;
	paused=0;
	musicClock=0;
	playSeq=-1;
	songLoaded=0;
	lastUpdate=0;
	elapsed=0;

	for(i=0;i<NUM_SEQS;i++)
	{
		seqClock[i]=0;
		seqPlay[i]=0;
		seqMute[i]=0;
	}
	curSong.seq=NULL;
}

void Music_Exit(void)
{
	if(curSong.seq)
	{
		free(curSong.seq);
		curSong.seq=NULL;
	}
}

byte Music_Load(char *fname)
{
	FILE *f;
	char fn[64];
	int i,j;

	Music_Stop();
	songLoaded=0;
	if(fname[0]=='\0')
		return 0;

	sprintf(fn,"music/%s",fname);
	f=AssetOpen(fn);
	if(!f)
		return 0;

	if(curSong.seq)
		free(curSong.seq);

	// read in the basic data
	fread(&curSong,76,1,f);

	curSong.seq=(songSeq_t *)malloc(sizeof(songSeq_t)*curSong.numSeqs);
	if(!curSong.seq)
	{
		fclose(f);
		return 0;
	}

	// load each sequence
	for(i=0;i<curSong.numSeqs;i++)
	{
		fread(curSong.seq[i].name,sizeof(char),32,f);
		fread(&curSong.seq[i].length,sizeof(word),1,f);
		fread(&curSong.seq[i].playLength,sizeof(word),1,f);
		fread(&curSong.seq[i].instrument,sizeof(byte),1,f);
		fread(&curSong.seq[i].volume,sizeof(short),1,f);
		fread(&curSong.seq[i].insFlags,sizeof(byte),1,f);
		memset(curSong.seq[i].note,0,SEQ_LENGTH*NUM_PITCHES);
		memset(curSong.seq[i].play,0,SONG_LENGTH);
		for(j=0;j<NUM_PITCHES;j++)
			fread(curSong.seq[i].note[j],sizeof(byte),curSong.seq[i].length*SEQ_BLOCK_SIZE,f);
		fread(curSong.seq[i].play,sizeof(byte),curSong.seq[i].playLength,f);
	}

	fclose(f);
	playSeq=-1;
	Music_Scan(0,-1);
	songLoaded=1;
	strcpy(player.songName,curSong.name);
	return 1;
}

byte Music_Save(char *fname)
{
	FILE *f;
	char fn[64];
	int i,j;

	Music_SetLengths();

	sprintf(fn,"music/%s",fname);
	f=AssetOpen_Write(fn);
	if(!f)
		return 0;

	// write out the basic data
	fwrite(&curSong,76,1,f);

	// write each sequence
	for(i=0;i<curSong.numSeqs;i++)
	{
		fwrite(curSong.seq[i].name,sizeof(char),32,f);
		fwrite(&curSong.seq[i].length,sizeof(word),1,f);
		fwrite(&curSong.seq[i].playLength,sizeof(word),1,f);
		fwrite(&curSong.seq[i].instrument,sizeof(byte),1,f);
		fwrite(&curSong.seq[i].volume,sizeof(short),1,f);
		fwrite(&curSong.seq[i].insFlags,sizeof(byte),1,f);
		for(j=0;j<NUM_PITCHES;j++)
			fwrite(curSong.seq[i].note[j],sizeof(byte),curSong.seq[i].length*SEQ_BLOCK_SIZE,f);
		fwrite(curSong.seq[i].play,sizeof(byte),curSong.seq[i].playLength,f);
	}

	fclose(f);
	return 1;
}

byte Music_GetSongName(char *fname,char *title,char *author)
{
	FILE *f;
	word w;
	char s[64];

	sprintf(s,"music/%s",fname);

	f=AssetOpen(s);
	if(!f)
		return 0;

	fread(&w,sizeof(word),1,f);	// skip tempo

	fread(title,sizeof(char),32,f);
	fread(author,sizeof(char),32,f);

	fclose(f);
	return 1;
}

void Music_Update(void)
{
	int i,pos;
	dword now;


	if(!playing)
		return;
	if(paused)
		return;
	if(!songLoaded)
		return;

	now=timeGetTime();
	elapsed+=(now-lastUpdate);
	lastUpdate=now;

	if(elapsed<curSong.tempo)
	{
		// wait until next time to make noise
	}
	else
	{
		while(elapsed>=curSong.tempo && elapsed>0)
		{
			elapsed-=curSong.tempo;

			if(playSeq!=-1)	// just playing one sequence
			{
				if(seqClock[playSeq]>=(dword)loopSpot || seqClock[playSeq]<loopTo)
					seqClock[playSeq]=loopTo;

				Music_PlayNotes(seqClock[playSeq],playSeq);

				seqClock[playSeq]++;
				if(seqClock[playSeq]>=(dword)loopSpot || seqClock[playSeq]<loopTo)
					seqClock[playSeq]=loopTo;
			}
			else	// playing the whole song
			{
				musicClock++;
				if(musicClock>=loopSpot || musicClock<loopTo)
				{
					musicClock=loopTo;

					for(i=0;i<curSong.numSeqs;i++)
						FindSeqPos(i,musicClock);
				}
				// reached a new block
				if((musicClock%SEQ_BLOCK_SIZE)==0)
				{
					pos=musicClock/SEQ_BLOCK_SIZE;
					for(i=0;i<curSong.numSeqs;i++)
					{
						// stop command ends it playing if it was
						if(curSong.seq[i].play[pos]==SP_STOP)
						{
							seqPlay[i]=0;
							seqClock[i]=0;
						}
						// play command starts it fresh no matter what
						else if(curSong.seq[i].play[pos]==SP_PLAY)
						{
							seqPlay[i]=1;
							seqClock[i]=0;
						}
					}
				}
				for(i=0;i<curSong.numSeqs;i++)
				{
					if(seqPlay[i])
					{
						Music_PlayNotes(seqClock[i],i);

						// advance the clock
						seqClock[i]++;
						// if it goes off the end, then stop playing
						if(seqClock[i]>=(dword)curSong.seq[i].length*SEQ_BLOCK_SIZE)
						{
							seqPlay[i]=0;
							seqClock[i]=0;
						}
					}
				}
			}
		}
	}
}

void Music_Play(void)
{
	int i;

	if(!songLoaded)
		return;

	if(!playing)
	{
		playing=1;
		paused=0;
		musicClock=0;
		for(i=0;i<curSong.numSeqs;i++)
			FindSeqPos(i,musicClock);

		if(playSeq!=-1)
			loopSpot=curSong.seq[playSeq].length*SEQ_BLOCK_SIZE;
		else
			loopSpot=curSong.length*SEQ_BLOCK_SIZE;
		loopTo=0;
	}
	else
	{
		if(paused)
			paused=0;
		for(i=0;i<curSong.numSeqs;i++)
			FindSeqPos(i,musicClock);
		if(playSeq!=-1)
			loopSpot=curSong.seq[playSeq].length*SEQ_BLOCK_SIZE;
		else
			loopSpot=curSong.length*SEQ_BLOCK_SIZE;
		loopTo=0;
	}
	lastUpdate=timeGetTime();
	elapsed=0;
}

void Music_Stop(void)
{
	playing=0;
	paused=0;
	musicClock=0;
}

void FindSeqPos(int seq,dword position)
{
	int i;
	int pos;

	pos=position/SEQ_BLOCK_SIZE;

	for(i=pos;i>=0;i--)
	{
		// a stop command means there's nothing to play here
		if(curSong.seq[seq].play[i]==SP_STOP)
		{
			seqPlay[seq]=0;
			seqClock[seq]=0;
			return;
		}
		else if(curSong.seq[seq].play[i]==SP_PLAY)
		{
			// we've found a play command
			seqPlay[seq]=1;
			seqClock[seq]=position-i*SEQ_BLOCK_SIZE;
			return;
		}
		// if we're far enough back that a sequence couldn't still be playing, then give up
		if(i==pos-curSong.seq[seq].length)
		{
			seqPlay[seq]=0;
			seqClock[seq]=0;
			return;
		}
	}

	// no play commands
	seqPlay[seq]=0;
	seqClock[seq]=0;
}

void Music_Scan(dword position,int seq)
{
	int i;

	if(!playing)
	{
		playing=1;
		paused=1;
		if(seq==-1)
		{
			musicClock=position;
			for(i=0;i<curSong.numSeqs;i++)
				FindSeqPos(i,musicClock);
		}
		else
			seqClock[seq]=position;
	}
	else
	{
		if(seq==-1)
		{
			musicClock=position;
			for(i=0;i<curSong.numSeqs;i++)
				FindSeqPos(i,musicClock);
		}
		else
			seqClock[seq]=position;
	}
}

void Music_Pause(void)
{
	paused=1;
}

void Music_PlayNotes(dword when,byte seq)
{
	int i;
	byte played;

	if(seqMute[seq])
		return;	// no play!

	played=0;
	for(i=0;i<NUM_PITCHES;i++)
	{
		if(curSong.seq[seq].note[i][when])
		{
			// this checking if it's been played is a feature so that if you play multiple notes
			// at once (a chord), with sustain off, they won't cut each other off and result in
			// only one being played.  The first one is played with sustain off, cutting off previous
			// notes, but the remaining ones are all played with sustain on, so they don't cut it off.
			if(!played)
			{
				PlayInstrument(curSong.seq[seq].instrument,i,curSong.seq[seq].volume,curSong.seq[seq].insFlags,seq);
				played=1;
			}
			else
			{
				PlayInstrument(curSong.seq[seq].instrument,i,curSong.seq[seq].volume,curSong.seq[seq].insFlags|SND_SUSTAIN,seq);
			}

		}
	}
}

void Music_PlayNote(byte note,byte inst,int vol,byte sustain)
{
	// play it in its own sequence, so it doesn't interact with anything else
	PlayInstrument(inst,note,vol,(sustain ? SND_SUSTAIN : 0),NUM_SEQS+1);
}

void Music_New(void)
{
	if(curSong.seq)
	{
		free(curSong.seq);
		curSong.seq=NULL;
	}

	curSong.tempo=60;
	curSong.length=64;
	strcpy(curSong.name,"Nosong");
	strcpy(curSong.author,"Nobody");

	curSong.numSeqs=1;
	curSong.seq=(songSeq_t *)malloc(sizeof(songSeq_t));
	strcpy(curSong.seq[0].name,"Seq#01");
	curSong.seq[0].insFlags=0;
	curSong.seq[0].instrument=0;
	curSong.seq[0].volume=0;
	curSong.seq[0].length=1;
	curSong.seq[0].playLength=0;
	memset(curSong.seq[0].note,0,NUM_PITCHES*SEQ_LENGTH);
	memset(curSong.seq[0].play,0,SONG_LENGTH);
}

byte Music_AddSeq(void)
{
	songSeq_t *s;
	int i;

	if(curSong.numSeqs==NUM_SEQS)
		return 0;
	curSong.numSeqs++;
	s=(songSeq_t *)realloc(curSong.seq,sizeof(songSeq_t)*curSong.numSeqs);
	if(!s)
		return 0;
	curSong.seq=s;
	i=curSong.numSeqs-1;

	sprintf(curSong.seq[i].name,"Seq#%02d",i+1);
	curSong.seq[i].insFlags=0;
	curSong.seq[i].instrument=0;
	curSong.seq[i].length=1;
	curSong.seq[i].playLength=0;
	curSong.seq[i].volume=0;
	memset(curSong.seq[i].note,0,NUM_PITCHES*SEQ_LENGTH);
	memset(curSong.seq[i].play,0,SONG_LENGTH);
	return 1;
}

byte Music_CloneSeq(byte src)
{
	songSeq_t *s;
	int i;

	if(curSong.numSeqs==NUM_SEQS)
		return 0;
	curSong.numSeqs++;
	s=(songSeq_t *)realloc(curSong.seq,sizeof(songSeq_t)*curSong.numSeqs);
	if(!s)
		return 0;
	curSong.seq=s;
	i=curSong.numSeqs-1;

	curSong.seq[i]=curSong.seq[src];

	return 1;
}

byte Music_MoveSeq(byte seq,char mv)
{
	songSeq_t s;

	if(seq==0 && mv<0)
		return 0;
	if(seq==curSong.numSeqs-1 && mv>0)
		return 0;

	s=curSong.seq[seq];
	curSong.seq[seq]=curSong.seq[seq+mv];
	curSong.seq[seq+mv]=s;

	return 1;
}

byte Music_DelSeq(byte seq)
{
	int i;
	songSeq_t *s;

	if(seq>=curSong.numSeqs)
		return 0;

	if(curSong.numSeqs==1)
		return 0;

	for(i=seq;i<curSong.numSeqs-1;i++)
		curSong.seq[i]=curSong.seq[i+1];
	curSong.numSeqs--;
	s=(songSeq_t *)realloc(curSong.seq,sizeof(songSeq_t)*curSong.numSeqs);
	if(!s)
		return 0;
	curSong.seq=s;

	return 1;
}

void Music_MakeSong(void)
{
}

byte *Music_GetSeq(byte seq)
{
	return (byte *)curSong.seq[seq].note;
}

byte Music_GetInstrument(byte seq)
{
	return curSong.seq[seq].instrument;
}

byte Music_GetInsFlags(byte seq)
{
	return curSong.seq[seq].insFlags;
}

song_t *Music_GetSong(void)
{
	return &curSong;
}

void Music_Shift(int seq,dword start,int shift)
{
	int i,j;

	if(seq==-1)
	{
		if(shift<0)
		{
			for(j=0;j<curSong.numSeqs;j++)
			{
				for(i=start;i<SONG_LENGTH;i++)
				{
					curSong.seq[j].play[i+shift]=curSong.seq[j].play[i];
				}
				// clear the excess space at the end
				for(i=SONG_LENGTH+shift;i<SONG_LENGTH;i++)
					curSong.seq[j].play[i]=0;
			}
		}
		else
		{
			for(j=0;j<curSong.numSeqs;j++)
			{
				for(i=SONG_LENGTH-1;i>=(int)start;i--)
				{
					if(i+shift<SONG_LENGTH)
						curSong.seq[j].play[i+shift]=curSong.seq[j].play[i];
				}
			}
		}
	}
	else
	{
		if(shift<0)
		{
			for(j=0;j<NUM_PITCHES;j++)
			{
				for(i=start;i<SEQ_LENGTH;i++)
				{
					curSong.seq[seq].note[j][i+shift]=curSong.seq[seq].note[j][i];
				}
				// clear the excess space at the end
				for(i=SEQ_LENGTH+shift;i<SEQ_LENGTH;i++)
					curSong.seq[seq].note[j][i]=0;
			}
		}
		else
		{
			for(j=0;j<NUM_PITCHES;j++)
			{
				for(i=SEQ_LENGTH-1;i>=(int)start;i--)
				{
					if(i+shift<SEQ_LENGTH)
						curSong.seq[seq].note[j][i+shift]=curSong.seq[seq].note[j][i];
				}
			}
		}
	}
}

word Music_GetLength(int seq)
{
	if(seq==-1)
		return curSong.length;
	else
		return curSong.seq[seq].length;
}

void Music_SetLength(int seq,word l)
{
	if(seq==-1)
		curSong.length=l;
	else
		curSong.seq[seq].length=l;
}

word Music_GetPos(int seq)
{
	if(seq==-1)
		return (word)musicClock;
	else
		return (word)seqClock[seq];
}

void Music_SetInstrument(byte seq,byte type)
{
	curSong.seq[seq].instrument=type;
}

void Music_SetInsFlags(byte seq,byte flags)
{
	curSong.seq[seq].insFlags=flags;
}

char *Music_GetTitle(void)
{
	return curSong.name;
}

char *Music_GetAuthor(void)
{
	return curSong.author;
}

void Music_SetPlaySeq(int seq)
{
	playSeq=seq;
}

void Music_SetLengths(void)
{
	int i,j,k;

	curSong.length=0;
	for(i=0;i<curSong.numSeqs;i++)
	{
		curSong.seq[i].length=1;
		for(j=0;j<SEQ_LENGTH;j++)
		{
			for(k=0;k<NUM_PITCHES;k++)
				if(curSong.seq[i].note[k][j])
				{
					curSong.seq[i].length=(j/SEQ_BLOCK_SIZE)+1;
				}
		}
		curSong.seq[i].playLength=0;
		for(j=0;j<SONG_LENGTH;j++)
		{
			if(curSong.seq[i].play[j]==SP_PLAY)
			{
				curSong.seq[i].playLength=j+curSong.seq[i].length;
				if(curSong.seq[i].playLength>SONG_LENGTH)
					curSong.seq[i].playLength=SONG_LENGTH;
			}
			else if(curSong.seq[i].play[j]==SP_STOP)
			{
				curSong.seq[i].playLength=j+1;
				if(curSong.seq[i].playLength>SONG_LENGTH)
					curSong.seq[i].playLength=SONG_LENGTH;
			}
		}
		if(curSong.seq[i].playLength>curSong.length)
			curSong.length=curSong.seq[i].playLength;
	}
}

byte Music_IsMuted(byte seq)
{
	return seqMute[seq];
}

void Music_Mute(byte seq,byte on)
{
	seqMute[seq]=on;
}

void Music_SetLoopSpot(word start,word spot)
{
	if(spot==0)
	{
		if(playSeq!=-1)
			loopSpot=curSong.seq[playSeq].length*SEQ_BLOCK_SIZE;
		else
			loopSpot=curSong.length*SEQ_BLOCK_SIZE;
		loopTo=0;
	}
	else
	{
		loopSpot=spot;
		loopTo=start;
	}
}

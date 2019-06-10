#include "internet.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <conio.h>
#include <winsock.h>
#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#define SOCKET int
#define SOCKET_ERROR (-1)
#define INVALID_SOCKET (-1)
#define closesocket close
#endif

#define NUM_SOCKETS	4

#define DATA_GRAB_SIZE 512
#define MEMALLOC_SIZE	2048
#define HEADER_MEM_SIZE 4096

#define SS_UNUSED				0
#define SS_WAITINGTOCONNECT		1
#define SS_CONNECTED			2
#define SS_REQUESTSENT			3
#define SS_GOTHEADER			4
#define SS_DONERECEIVING		5
#define SS_FAILED				6

// if I'm not mistaken, this number is NOT used by windows as a window event
#define INTERNET_EVENT	(WM_USER+1)

typedef struct mySock_t
{
	byte state;
	SOCKET s;
	byte *header;
	byte *buffer;
	byte downloadDone;
	int downloadTries;
	int dataSize;
	int realDataGot;
	int dataGot;
	int reqSent;
	char request[1024];
	byte error;
} mySock_t;

mySock_t sock[NUM_SOCKETS];
char errorDisp[128];

#ifdef LOG
static FILE *logF;
#endif

void Log_Init(void)
{
#ifdef LOG
	logF=fopen("netlog.txt","wt");
#endif
}

void Log_Exit(void)
{
#ifdef LOG
	fclose(logF);
#endif
}

void Log_Print(const char *txt)
{
#ifdef LOG
	fprintf(logF,"%s\n",txt);
#endif
}

void Log_Printnum(const char *txt,int num)
{
#ifdef LOG
	fprintf(logF,"%s %d\n",txt,num);
#endif
}

void Log_PrintRaw(const char *txt,int len)
{
#ifdef LOG
	char outtxt[1024];

	strncpy(outtxt,txt,len);

	fprintf(logF,"%s\n",outtxt);
#endif
}

#ifdef _WIN32
#define ERR_WOULDBLOCK WSAEWOULDBLOCK
#define ERR_NOTCONN WSAENOTCONN
#else
#define ERR_WOULDBLOCK EAGAIN
#define ERR_NOTCONN EPIPE
#endif

byte Web_Init(void)
{
	int i;

	Log_Init();

#ifdef _WIN32
	WSADATA wsaData;
	// initialize WinSock
	if(WSAStartup(MAKEWORD(1,1),&wsaData)!=0)
	{
		return IE_NOWINSOCK;
	}
#endif

	for(i=0;i<NUM_SOCKETS;i++)
	{
		sock[i].state=SS_UNUSED;
		sock[i].buffer=NULL;
		sock[i].header=NULL;
	}

	strcpy(errorDisp,"No Error!");
	Log_Print("web_init ok!");
	return IE_OK;
}

inline int SocketLastError()
{
#ifdef _WIN32
	return WSAGetLastError();
#else
	return errno;
#endif
}

byte Web_Exit(void)
{
	int i;

	for(i=0;i<NUM_SOCKETS;i++)
		if(sock[i].state!=SS_UNUSED)
		{
			sock[i].state=SS_UNUSED;
			Web_KillSocket(i);
		}

#ifdef _WIN32
	WSACleanup();
#endif
	Log_Print("web_exit ok!");
	Log_Exit();
	return IE_OK;
}

void Web_KillSocket(int sockNum)
{
	Log_Printnum("web_killsocket #",sockNum);
	closesocket(sock[sockNum].s);
	sock[sockNum].state=SS_UNUSED;

	if(sock[sockNum].buffer)
		free(sock[sockNum].buffer);
	if(sock[sockNum].header)
		free(sock[sockNum].header);
	sock[sockNum].buffer=NULL;
	sock[sockNum].header=NULL;
}

byte Web_TryReadingHeader(byte n)
{
	char line[256];
	int i,pos;
	byte lineOn;

	pos=0;

	Log_Print("web_tryreadingheader");
	i=0;
	lineOn=0;
	while(pos<sock[n].dataGot)
	{
		line[i]=(char)sock[n].header[pos++];
		if(line[i]=='\r')
		{
			pos++;	// skip over the coming \n
			line[i]='\0';
			Log_PrintRaw((char *)sock[n].header,sock[n].dataGot);
			switch(lineOn)
			{
				case 0:	// should be "HTTP/1.1 200 OK"
					if(strstr(line,"200 OK"))
					{
						Log_Print(line);
						// yay, it's here!
						lineOn++;
						i=0;
					}
					else
					{
						Log_Print(line);
						// an error occurred
						strcpy(errorDisp,line);
						return IE_FILENOTFOUND;
					}
					break;
				default:	// any greater line #
					if(!strncmp(line,"Content-Length:",15))
					{
						// this is the # of bytes in the content
						sock[n].dataSize=atoi(&line[16]);
						Log_Printnum("dataSize: ",sock[n].dataSize);
						if(sock[n].buffer!=NULL)
							free(sock[n].buffer);
						sock[n].buffer=(byte *)malloc(sock[n].dataSize+256);
						if(!sock[n].buffer)
							return IE_OUTOFMEMORY;
					}
					if(!strcmp(line,""))
					{
						// this is the content type line, signaling the begin of real data
						sock[n].realDataGot=sock[n].dataGot-pos;
						if(sock[n].realDataGot>0)
							memcpy(sock[n].buffer,&sock[n].header[pos],sock[n].realDataGot);
						Log_Printnum("dataGot: ",sock[n].dataGot);
						Log_Printnum("realDataGot: ",sock[n].realDataGot);
						return IE_DATASTART;
					}
					i=0;
					break;
			}
		}
		else
		{
			i++;
			if(i==256)	// overextended the line length limits, we don't care, so loop around
			{
				i=0;	// any line that's this long is not important to us
			}
		}
	}
	return IE_OK;	// header's not done downloading yet
}

byte Web_RequestData(MGLDraw *mgl,const char *site,const char *file,int *socketNumber)
{
	struct hostent *host;
	struct sockaddr_in addr;
	int i,sockNum;

	Log_Print("web_requestdata");
	Log_Print(site);
	Log_Print(file);

	if(!(host=gethostbyname(site))) // resolve host name
	{
		return IE_SITENOTFOUND;
	}

	sockNum=-1;
	for(i=0;i<NUM_SOCKETS;i++)
	{
		if(sock[i].state==SS_UNUSED || sock[i].state==SS_FAILED)
		{
			sockNum=i;
			sock[i].state=SS_WAITINGTOCONNECT;
			if(sock[i].buffer)
			{
				free(sock[i].buffer);
				sock[i].buffer=NULL;
			}
			if(sock[i].header)
			{
				free(sock[i].header);
				sock[i].header=NULL;
			}
			sock[i].header=(byte *)calloc(sizeof(byte)*HEADER_MEM_SIZE,sizeof(byte));
			if(!sock[i].header)
			{
				sock[i].state=SS_FAILED;
				return IE_OUTOFMEMORY;
			}
			sock[i].buffer=(byte *)calloc(sizeof(byte)*65536,sizeof(byte));

			sock[i].error=IE_OK;
			sock[i].dataGot=0;
			sock[i].realDataGot=0;
			sock[i].dataSize=-1;
			sock[i].downloadDone=0;
			sock[i].downloadTries=0;
			sock[i].reqSent=0;
			*socketNumber=i;
			break;
		}
	}

	Log_Printnum("Using socket #",sockNum);
	if(sockNum==-1)
		return IE_NOSOCKETSFREE;

	sock[sockNum].s=socket(AF_INET,SOCK_STREAM,0);

	sprintf(sock[sockNum].request,"GET %s HTTP/1.0\r\nHOST: %s:80\r\n\r\n",file,site);

	if(sock[sockNum].s==INVALID_SOCKET)
	{
		Web_KillSocket(sockNum);
		sock[sockNum].error=IE_BADSOCKET;
		return IE_BADSOCKET;
	}

	// set it to asynchronous - send windows messages whenever new things happen!
	if((i=
	#ifdef _WIN32
		WSAAsyncSelect(sock[sockNum].s,mgl->GetHWnd(),(unsigned int)INTERNET_EVENT,FD_READ|FD_WRITE|FD_CONNECT)
	#else
		fcntl(sock[sockNum].s, F_SETFL, O_NONBLOCK)
	#endif
		)!=0)
	{
		// couldn't set it to asynchronous mode
		Web_KillSocket(sockNum);
		sock[sockNum].error=IE_BADSOCKET;
		return IE_BADSOCKET;
	}
	Log_Print("async ok!");

	memset(&addr,0,sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_port=htons(80);
	memcpy(&addr.sin_addr,host->h_addr,host->h_length);


	i=connect(sock[sockNum].s,(struct sockaddr *)&addr,sizeof(addr));

	if(i==-1)
		i=SocketLastError();

	if(i==0 || i==ERR_WOULDBLOCK)
	{
		// everything's fine, we're done, it's trying to connect!
		Log_Printnum("web_requestdata ok!",i);
		return IE_OK;
	}
	else
	{
		// could not connect!
		Web_KillSocket(sockNum);
		sock[sockNum].error=IE_NOCONNECT;
		return IE_NOCONNECT;
	}
}

char *Web_ErrorString(void)
{
	return errorDisp;
}

char *Web_ErrorStringShort(void)
{
	static char buf[100];

	strncpy(buf,errorDisp,100);
	buf[99]='\0';
	return buf;
}

byte Web_DownloadPercent(int sockNum)
{
	byte b;

	if(sock[sockNum].state==SS_DONERECEIVING)
		return 100;
	else if(sock[sockNum].state==SS_UNUSED || sock[sockNum].state==SS_FAILED)
		return 0;
	else
	{
		if(sock[sockNum].dataSize<=0)
			b=0;
		else
			b=(sock[sockNum].realDataGot*100/sock[sockNum].dataSize);
		if(b==100)
			b=99;
		return b;
	}
}

byte *Web_GetData(int sockNum)
{
	return sock[sockNum].buffer;
}

int Web_GetDataSize(int sockNum)
{
	return sock[sockNum].dataSize;
}

byte Web_GetState(int sockNum)
{
	return sock[sockNum].state;
}

byte Web_GetError(int sockNum)
{
	return sock[sockNum].error;
}

void Web_Poll(int sockNum)
{
	int i;

	if(sock[sockNum].state==SS_CONNECTED || sock[sockNum].state==SS_WAITINGTOCONNECT)
	{
		// must send our request
		i=send(sock[sockNum].s,&sock[sockNum].request[sock[sockNum].reqSent],strlen(sock[sockNum].request)-sock[sockNum].reqSent,0);
		if(i==SOCKET_ERROR)
		{
			i=SocketLastError();
			if(i==ERR_NOTCONN)
			{
				return;	// not connected yet, keep trying
			}
			if(i==ERR_WOULDBLOCK)
			{
				return;	// worked, I guess
			}
			Web_KillSocket(sockNum);
			sock[sockNum].error=IE_SENDFAIL;
			sock[sockNum].state=SS_FAILED;
			return;
		}
		// if no error, then i=the number of bytes sent
		sock[sockNum].reqSent+=i;
		if(sock[sockNum].reqSent==(int)strlen(sock[sockNum].request))
		{
			// the whole request has been sent, hooray!
			sock[sockNum].state=SS_REQUESTSENT;
			Log_Printnum("request sent! #",sockNum);
		}
	}
	else if(sock[sockNum].state==SS_REQUESTSENT)
	{
		i=recv(sock[sockNum].s,(char *)&sock[sockNum].header[sock[sockNum].dataGot],DATA_GRAB_SIZE,0);
		if(i==SOCKET_ERROR)
		{
			i=SocketLastError();
			if(i==ERR_WOULDBLOCK)
			{
				return;	// seems okay, waiting for more data
			}
			Web_KillSocket(sockNum);
			sock[sockNum].state=SS_FAILED;
			sock[sockNum].error=IE_RECVFAIL;
			Log_Printnum("RECV error #",sockNum);
			Log_Printnum("error code: ",SocketLastError());
			return;
		}
		Log_Printnum("Received ",i);
		sock[sockNum].dataGot+=i;	// add the amount claimed

		// if we haven't yet got the header, examine to see if we have it now
		i=Web_TryReadingHeader(sockNum);

		if(i==IE_DATASTART)
		{
			// we have it now
			sock[sockNum].state=SS_GOTHEADER;
		}
		else if(i==IE_FILENOTFOUND || i==IE_OUTOFMEMORY)
		{
			Web_KillSocket(sockNum);
			sock[sockNum].state=SS_FAILED;
			sock[sockNum].error=i;
		}

		if(sock[sockNum].realDataGot==sock[sockNum].dataSize)
		{
			Log_Printnum("download complete #",sockNum);
			sock[sockNum].state=SS_DONERECEIVING;
		}

		Log_Printnum("dataSize: ",sock[sockNum].dataSize);
		Log_Printnum("dataGot: ",sock[sockNum].dataGot);
		Log_Printnum("realDataGot: ",sock[sockNum].realDataGot);
	}
	else if(sock[sockNum].state==SS_GOTHEADER)
	{
		// we have sent our request already, so the data is arriving!
		i=recv(sock[sockNum].s,(char *)&sock[sockNum].buffer[sock[sockNum].realDataGot],DATA_GRAB_SIZE,0);
		if(i==SOCKET_ERROR)
		{
			i=SocketLastError();
			if(i==ERR_WOULDBLOCK)
				return;	// seems okay by me, it means that it is waiting for more data
			Log_Printnum("RECV error #",sockNum);
			Log_Printnum("error code: ",SocketLastError());
			Web_KillSocket(sockNum);
			sock[sockNum].state=SS_FAILED;
			sock[sockNum].error=IE_RECVFAIL;
			return;
		}
		if(i==0)
		{	// received it all!
			sock[sockNum].dataSize=sock[sockNum].realDataGot;
			Log_Printnum("download complete #",sockNum);
			sock[sockNum].state=SS_DONERECEIVING;
			return;
		}
		Log_Printnum("Received ",i);

		sock[sockNum].realDataGot+=i;

		if(sock[sockNum].realDataGot==sock[sockNum].dataSize)
		{
			Log_Printnum("download complete #",sockNum);
			sock[sockNum].state=SS_DONERECEIVING;
		}
		Log_Printnum("dataSize: ",sock[sockNum].dataSize);
		Log_Printnum("dataGot: ",sock[sockNum].dataGot);
		Log_Printnum("realDataGot: ",sock[sockNum].realDataGot);
	}
}

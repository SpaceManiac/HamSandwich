#include "internet.h"
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <winsock.h>
#pragma comment( lib, "wsock32" )
#include "log.h"

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
	char request[256];
	byte error;
} mySock_t;

mySock_t sock[NUM_SOCKETS];
char errorDisp[128];

byte Web_Init(void)
{
    WSADATA wsaData;
	int i;

    word version = MAKEWORD(1,1);

	// initialize WinSock
    if(WSAStartup(version,&wsaData)!=0)
    {
        return IE_NOWINSOCK;
    }

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

byte Web_Exit(void)
{
	int i;

	for(i=0;i<NUM_SOCKETS;i++)
		if(sock[i].state!=SS_UNUSED)
			Web_KillSocket(i);

    WSACleanup();
	Log_Print("web_exit ok!");
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
						if(sock[n].buffer==NULL)
						{
							sock[n].buffer=(byte *)malloc(sock[n].dataSize+256);
							if(!sock[n].buffer)
								return IE_OUTOFMEMORY;
						}
					}
					if(!strncmp(line,"Content-Type:",13))
					{
						// this is the content type line, signaling the begin of real data
						sock[n].realDataGot=sock[n].dataGot-(pos+2);
						if(sock[n].realDataGot>0)
							memcpy(sock[n].buffer,&sock[n].header[pos+2],sock[n].realDataGot);
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

byte Web_RequestData(MGLDraw *mgl,char *site,char *file,int *socketNumber)
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
			sock[i].header=(byte *)malloc(sizeof(byte)*HEADER_MEM_SIZE);
			if(!sock[i].header)
			{
				sock[i].state=SS_FAILED;
				return IE_OUTOFMEMORY;
			}
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

	sprintf(sock[sockNum].request,"GET %s HTTP/1.1\r\nHOST: %s:80\r\n\r\n",file,site);

    if(sock[sockNum].s==INVALID_SOCKET)
    {
		Web_KillSocket(sockNum);
		sock[sockNum].error=IE_BADSOCKET;
        return IE_BADSOCKET;
    }

	// set it to asynchronous - send windows messages whenever new things happen!
	if((i=WSAAsyncSelect(sock[sockNum].s,mgl->GetHWnd(),INTERNET_EVENT,FD_READ|FD_WRITE|FD_CONNECT))!=0)
	{
		// couldn't set it to asynchronous mode
		Web_KillSocket(sockNum);
		sock[sockNum].error=IE_BADSOCKET;
		return IE_BADSOCKET;
	}

    memset(&addr,0,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_port=htons(80);
    memcpy(&addr.sin_addr,host->h_addr,host->h_length);

    i=connect(sock[sockNum].s,(struct sockaddr *)&addr,sizeof(addr));
	
	if(i==-1)
		i=WSAGetLastError();

	if(i==0 || i==WSAEWOULDBLOCK)
    {
		// everything's fine, we're done, it's trying to connect!
		Log_Print("web_requestdata ok!");
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

void Web_HandleEvent(SOCKET sockie,int event,int error)
{
	int i;
	int sockNum;
	
	Log_Print("web_event occurred");
	for(i=0;i<NUM_SOCKETS;i++)
	{
		if(sock[i].s==sockie && sock[i].state!=SS_UNUSED && sock[i].state!=SS_FAILED &&
			sock[i].state!=SS_DONERECEIVING)
		{
			sockNum=i;
			break;
		}
	}
	if(i==NUM_SOCKETS)
		return;	// doesn't apply to any of these sockets, weird

	if(sock[sockNum].state==SS_UNUSED)
		return;	// ignore messages for you

	switch(event)
	{
		case FD_CONNECT:	// the socket has connected
			Log_Printnum("FD_CONNECT #",sockNum);
			Log_Printnum("error ",error);
			if(sock[sockNum].state==SS_WAITINGTOCONNECT)
			{
				if(error!=0)
				{
					// unable to connect, sorry!
					Web_KillSocket(sockNum);
					sock[sockNum].state=SS_FAILED;
					return;
				}
				Log_Printnum("we're connected! #",sockNum);
				sock[sockNum].state=SS_CONNECTED;
			}
			else
			{
				// what's going on!?
			}
			break;
		case FD_WRITE:	// the socket is open for writing
			Log_Printnum("FD_WRITE #",sockNum);
			Log_Printnum("error ",error);
			if(error!=0)
				return;	// don't even know what to DO with an error here, ignore it
			if(sock[sockNum].state==SS_CONNECTED)
			{
				// must send our request
				i=send(sock[sockNum].s,&sock[sockNum].request[sock[sockNum].reqSent],
					strlen(sock[sockNum].request)-sock[sockNum].reqSent,0);
				if(i==SOCKET_ERROR)
				{
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
			break;
		case FD_READ:	// the socket is open for reading
			Log_Printnum("FD_READ #",sockNum);
			Log_Printnum("error ",error);
			if(error!=0)
				return; // don't even know what to DO with an error here, ignore it
			if(sock[sockNum].state==SS_REQUESTSENT)
			{
				i=recv(sock[sockNum].s,(char *)&sock[sockNum].header[sock[sockNum].dataGot],DATA_GRAB_SIZE,0);
				if(i==SOCKET_ERROR)
				{
					Web_KillSocket(sockNum);
					sock[sockNum].state=SS_FAILED;
					sock[sockNum].error=IE_RECVFAIL;
					Log_Printnum("RECV error #",sockNum);
					Log_Printnum("error code: ",WSAGetLastError());
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
					closesocket(sock[sockNum].s);
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
					Log_Printnum("RECV error #",sockNum);
					Log_Printnum("error code: ",WSAGetLastError());
					Web_KillSocket(sockNum);
					sock[sockNum].state=SS_FAILED;
					sock[sockNum].error=IE_RECVFAIL;
					return;
				}
				
				Log_Printnum("Received ",i);
				
				sock[sockNum].realDataGot+=i;

				if(sock[sockNum].realDataGot==sock[sockNum].dataSize)
				{
					Log_Printnum("download complete #",sockNum);
					sock[sockNum].state=SS_DONERECEIVING;
					closesocket(sock[sockNum].s);
				}
				Log_Printnum("dataSize: ",sock[sockNum].dataSize);
				Log_Printnum("dataGot: ",sock[sockNum].dataGot);
				Log_Printnum("realDataGot: ",sock[sockNum].realDataGot);
			}
			break;
		case FD_CLOSE:	// the socket has been closed
			//if(sock[sockNum].state!=SS_UNUSED && sock[sockNum].state!=SS_FAILED)
			//	Web_KillSocket(sockNum);
			break;
	}
}

byte Web_GetState(int sockNum)
{
	return sock[sockNum].state;
}

byte Web_GetError(int sockNum)
{
	return sock[sockNum].error;
}
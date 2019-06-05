#ifndef INTERNET_H
#define INTERNET_H

#include "mgldraw.h"

#define IE_OK			0
#define IE_NOCONNECT	1
#define IE_NODATA		2
#define IE_NOWINSOCK	3
#define IE_SITENOTFOUND 4
#define IE_NOSOCKETSFREE 5
#define IE_BADSOCKET	6
#define IE_SENDFAIL		7
#define IE_OUTOFMEMORY	8
#define IE_RECVFAIL		9
#define IE_FILENOTFOUND 10
#define IE_DATASTART	11

// socket states
#define SS_UNUSED				0
#define SS_WAITINGTOCONNECT		1
#define SS_CONNECTED			2
#define SS_REQUESTSENT			3
#define SS_GOTHEADER			4
#define SS_DONERECEIVING		5
#define SS_FAILED				6

byte Web_Init(void);
byte Web_Exit(void);
// returns error codes, fills in socketNumber which you need in order to track the
// download and get your data later
byte Web_RequestData(MGLDraw *mgl,const char *site,const char *file,int *socketNumber);
byte Web_DownloadPercent(int sockNum);
char *Web_ErrorString(void);
char *Web_ErrorStringShort(void);
byte *Web_GetData(int sockNum);
int Web_GetDataSize(int sockNum);
void Web_KillSocket(int sockNum);

byte Web_GetState(int sockNum);
byte Web_GetError(int sockNum);

void Web_Poll(int sockNum);

#endif

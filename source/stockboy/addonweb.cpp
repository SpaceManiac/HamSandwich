#include "addonweb.h"
#include "display.h"
#include "internet.h"
#include <io.h>
#include <time.h>
#include "control.h"
#include "game.h"
#include "display.h"
#include "log.h"

#define MODE_GETLIST	0
#define MODE_ERROR		1
#define MODE_GETFILE	2

typedef struct addOn_t
{
	char site[64];
	char url[128];
	char name[32];
	char destFileName[128];
	byte month,day,year;
} addOn_t;

static char txt[128]="This is the text!";
static char btnTxt[16]="Cancel";
static byte pct,mode;
static int pctPixels;
static int listSocket,fileSocket;
static char *addOnList;
static int listPos;
static addOn_t addOn;
static byte webInited;
static byte errIsFatal;
static byte addOnsGot;

void CreateErrorString(byte err);

byte ShouldDownloadAddOn(void)
{
	struct _finddata_t filedata;
	struct tm *fileTime;
	long hFile;

	if(!strcmp(&addOn.name[strlen(addOn.name)-3],"sbl") ||
		!strcmp(&addOn.name[strlen(addOn.name)-3],"hig"))
		sprintf(addOn.destFileName,"levels\\%s",addOn.name);
	else if(!strcmp(&addOn.name[strlen(addOn.name)-3],"jsp") ||
		!strcmp(&addOn.name[strlen(addOn.name)-3],"bmp") ||
		!strcmp(&addOn.name[strlen(addOn.name)-3],"jft"))
		sprintf(addOn.destFileName,"graphics\\%s",addOn.name);
	else if(!strcmp(&addOn.name[strlen(addOn.name)-3],"sng"))
		sprintf(addOn.destFileName,"music\\%s",addOn.name);
	else
		strcpy(addOn.destFileName,addOn.name);	// if it's none of those, put it in the main directory


	hFile=_findfirst(addOn.destFileName,&filedata);

	if(hFile==-1)	// the file does not exist
	{
		_findclose(hFile);
		return 1;	// so download it
	}
	else
	{
		// the file does exist, so only download it if the one online is newer
		fileTime=gmtime(&filedata.time_write);
		_findclose(hFile);
		fileTime->tm_year-=100;	// compensate for that oddity (years measured from 1900, so subtract a hundred
								// to get years measured from 2000)
		fileTime->tm_mon++;		// tm structs use months 0-11 instead of 1-12

		if(addOn.year>fileTime->tm_year)
			return 1;	// yes, it's more recent in years
		else if(addOn.year==fileTime->tm_year)
		{
			// same year...
			if(addOn.month>fileTime->tm_mon)
				return 1;	// it's from a more recent month
			else if(addOn.month==fileTime->tm_mon)
			{
				// same month
				if(addOn.day>=fileTime->tm_mday)
					return 1;
			}
		}

		return 0;	// your copy is more recent
	}
}

byte CheckForDownloads(MGLDraw *mgl)
{
	int i;
	int dataSize;

	Log_Print("checkfordownloads");
	dataSize=Web_GetDataSize(listSocket);

	while(1)
	{
		if(listPos>=dataSize)
		{
			// all done
			return 0;	// no more downloads
		}
		
		// read month
		i=listPos;
		while(addOnList[i]!='/')
			i++;
		addOnList[i]='\0';

		addOn.month=atoi(&addOnList[listPos]);
		listPos=i+1;
		// read day
		i=listPos;
		while(addOnList[i]!='/')
			i++;
		addOnList[i]='\0';

		addOn.day=atoi(&addOnList[listPos]);
		listPos=i+1;
		// read year
		i=listPos;
		while(addOnList[i]!=':')
			i++;
		addOnList[i]='\0';

		addOn.year=atoi(&addOnList[listPos]);
		listPos=i+1;
		// read website
		i=listPos;
		while(addOnList[i]!='/')
			i++;
		addOnList[i]='\0';

		strcpy(addOn.site,&addOnList[listPos]);
		addOnList[i]='/';
		listPos=i;
		// read url
		i=listPos;
		while(addOnList[i]!='\n' && addOnList[i]!='\0' && i<dataSize)
			i++;
		addOnList[i]='\0';

		strcpy(addOn.url,&addOnList[listPos]);
		listPos=i+1;

		for(i=strlen(addOn.url)-1;i>=0;i--)
		{
			if(addOn.url[i]=='/')
			{
				strcpy(addOn.name,&addOn.url[i+1]);
				break;
			}
		}

		// now we have an add-on info
		if(ShouldDownloadAddOn())
		{
			Log_Print("shoulddownload");
			Log_Print(addOn.name);
			mode=MODE_GETFILE;
			i=Web_RequestData(mgl,addOn.site,addOn.url,&fileSocket);
			if(i!=IE_OK)
			{
				mode=MODE_ERROR;
				CreateErrorString(i);
				strcpy(btnTxt,"Okay");
				errIsFatal=0;
			}
			else
			{
				sprintf(txt,"Downloading %s...",addOn.name);
				strcpy(btnTxt,"Skip");
			}
			return 1;
		}
	}
	return 0;
}

void CreateErrorString(byte err)
{
	switch(err)
	{
		case IE_SITENOTFOUND:
		case IE_NOCONNECT:
			strcpy(txt,"Unable to connect to website!");
			break;
		case IE_NOWINSOCK:
			strcpy(txt,"Unable to initialize sockets!");
			break;
		case IE_NOSOCKETSFREE:
			strcpy(txt,"Unable to find an empty socket!");
			break;
		case IE_BADSOCKET:
			strcpy(txt,"Unable to create socket!");
			break;
		case IE_SENDFAIL:
			strcpy(txt,"Unable to send request!");
			break;
		case IE_OUTOFMEMORY:
			strcpy(txt,"Out of memory!");
			break;
		case IE_RECVFAIL:
			strcpy(txt,"Unable to receive data!");
			break;
		case IE_FILENOTFOUND:
			// use the web error string, except for the preceding "HTTP/1.1 "
			sprintf(txt,"Download failed: %s",&Web_ErrorStringShort()[9]);
			break;
		default:
			strcpy(txt,"Unknown error!");
			break;
	}
}

void Download_Addons(void)
{
	byte err;

	Log_Begin("downloadlog.txt");

	addOnsGot=0;
	if(Web_Init()!=IE_OK)
	{
		CreateErrorString(IE_NOWINSOCK);
		mode=MODE_ERROR;
		webInited=0;
		strcpy(btnTxt,"Okay");
		errIsFatal=1;
		return;
	}
	webInited=1;

	strcpy(txt,"Getting Add-On List...");
	pct=0;
	pctPixels=0;

	strcpy(btnTxt,"Cancel");
	err=Web_RequestData(GetDisplayMGL(),"www.hamumu.com","/goods/stock_addon.txt",&listSocket);
	if(err==IE_OK)
	{
		mode=MODE_GETLIST;
	}
	else
	{
		CreateErrorString(err);
		mode=MODE_ERROR;
		errIsFatal=1;
		strcpy(btnTxt,"Okay");
	}
}

void SaveAddOn(void)
{
	FILE *f;
	int dataSize;
	byte *data;

	dataSize=Web_GetDataSize(fileSocket);
	data=Web_GetData(fileSocket);

	Log_Print("save addon");
	Log_Print(addOn.destFileName);
	Log_Printnum("size: ",dataSize);

	f=fopen(addOn.destFileName,"wb");
	fwrite(data,sizeof(byte),dataSize,f);
	fclose(f);
	
	Web_KillSocket(fileSocket);
	addOnsGot++;
}

byte Update_Download(MGLDraw *mgl,char key,byte tap,int msx,int msy,byte btn)
{
	byte state;

	if((key==27) || (btn && PointInRect(msx,msy,270,195,369,225)) ||
		(tap&CONTROL_B1))
	{
		if(mode==MODE_ERROR && !errIsFatal && key!=27)
		{
			// if you hit ESC you really want to just abort completely
			// anything else, you just want to clear the error and get the next file
			if(!CheckForDownloads(mgl))
			{
				if(addOnsGot==0)
					strcpy(txt,"No new add-ons!");
				else
					sprintf(txt,"Successfully got %d add-ons.",addOnsGot);
				mode=MODE_ERROR;
				strcpy(btnTxt,"Okay");
				errIsFatal=1;
			}
		}
		else if(mode==MODE_GETFILE && key!=27)
		{
			// if you hit esc, you want to abort completely, otherwise
			// just skip this one file and do the next
			Web_KillSocket(fileSocket);
			if(!CheckForDownloads(mgl))
			{
				if(addOnsGot==0)
					strcpy(txt,"No add-ons downloaded!");
				else
					sprintf(txt,"Successfully got %d add-ons.",addOnsGot);
				mode=MODE_ERROR;
				strcpy(btnTxt,"Okay");
				errIsFatal=1;
			}
		}
		else
		{
			if(webInited)
				Web_Exit();
			Log_End();
			return 0;
		}
	}

	switch(mode)
	{
		case MODE_GETLIST:
			state=Web_GetState(listSocket);
			if(state==SS_FAILED || state==SS_UNUSED)
			{
				CreateErrorString(Web_GetError(listSocket));
				mode=MODE_ERROR;
				strcpy(btnTxt,"Okay");
				errIsFatal=1;
				return 1;
			}
			else if(state==SS_DONERECEIVING)
			{
				addOnList=(char *)Web_GetData(listSocket);
				listPos=0;
				if(!CheckForDownloads(mgl))
				{
					strcpy(txt,"No new add-ons!");
					mode=MODE_ERROR;
					strcpy(btnTxt,"Okay");
					errIsFatal=1;
				}
			}
			pct=Web_DownloadPercent(listSocket);
			pctPixels=pct*575/100;
			break;
		case MODE_ERROR:
			break;
		case MODE_GETFILE:
			state=Web_GetState(fileSocket);
			if(state==SS_FAILED || state==SS_UNUSED)
			{
				CreateErrorString(Web_GetError(fileSocket));
				mode=MODE_ERROR;
				strcpy(btnTxt,"Okay");
				errIsFatal=0;
				return 1;
			}
			else if(state==SS_DONERECEIVING)
			{
				SaveAddOn();
				if(!CheckForDownloads(mgl))
				{
					sprintf(txt,"Successfully downloaded %d files!",addOnsGot);
					mode=MODE_ERROR;
					errIsFatal=1;
					strcpy(btnTxt,"Okay");
					return 1;
				}
			}
			pct=Web_DownloadPercent(fileSocket);
			pctPixels=pct*575/100;
			break;
	}
	return 1;
}

void Render_Download(MGLDraw *mgl,int msx,int msy)
{
	char s[12];

	DrawFillBox(10,100,629,250,16);
	DrawBox(12,102,627,248,31);
	DrawBox(10,100,629,250,0);
	DrawBox(9,99,630,251,0);

	if(!GM_doDraw)
		return;

	if(PointInRect(msx,msy,270,195,369,225))
	{
		DrawFillBox(270,195,369,225,24);
		DrawBox(271,196,368,224,16);
		CenterPrintUnGlow(320,201,btnTxt,0);
	}
	else
	{
		DrawFillBox(270,195,369,225,18);
		DrawBox(271,196,368,224,20);
		CenterPrintGlow(320,201,btnTxt,0,0);
	}

	switch(mode)
	{
		case MODE_GETLIST:
		case MODE_GETFILE:
			DrawBox(30,150,609,170,10);
			DrawFillBox(31,151,608,169,6);
			DrawFillBox(32,152,32+pctPixels,168,32*3+20);
			sprintf(s,"%d%%",pct);
			CenterPrintGlow(320,151,s,0,0);
			CenterPrintGlow(320,115,txt,0,0);
			break;
		case MODE_ERROR:
			CenterPrintGlow(320,115,txt,0,0);
			break;
	}

	PrintUnGlow(5,460,"Press ESC to cancel download at any time",0);
	Print(5-2,460-2,"Press ESC to cancel download at any time",0,0);
			
}
/* Eddie Galaxy: Intergalactic Exterminator
   
   A HamumuSoft Production.

   v 1.2

   Copyright 1998, Mike Hommel
*/

#include "mgldraw.h"
#include "jamulfont.h"
#include "jamulsound.h"

#include "game.h"

bool windowedGame=FALSE;
MGLDraw *mainmgl;

void parseCmdLine(char *cmdLine)
{
	char *token;

	token=strtok(cmdLine," ");
	while(token!=NULL)
	{
		if(!strcmp(token,"window"))
			windowedGame=TRUE;
		token=strtok(NULL," ");
	}
}

int PASCAL WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR cmdLine,int nCmdShow)
{	
	byte buy;

	parseCmdLine(cmdLine);
	mainmgl=new MGLDraw("Eddie Galaxy",640,480,8,windowedGame,hInstance);
	if(!mainmgl)
		return 0;

	CentipedeInit(mainmgl);
	buy=CentipedeGame();
	CentipedeExit();
	delete mainmgl;
#ifdef DEMO
	if(buy)
		ShellExecute(NULL,"open","http://hamumu.com/store.php?game=EDDIE&src=demoexit",NULL,NULL,SW_SHOWNORMAL);
#endif
	return 0;
}

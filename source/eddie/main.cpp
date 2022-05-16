/* Eddie Galaxy: Intergalactic Exterminator

   A HamumuSoft Production.

   v 1.2

   Copyright 1998, Mike Hommel
*/

#include "mgldraw.h"
#include "jamulfont.h"
#include "jamulsound.h"
#include "game.h"
#include "extern.h"

MGLDraw *mainmgl;

TASK(int) main(int argc, char* argv[])
{
	//HAM_EXTERN_FULFILL

	bool windowedGame=false;

	for (int i = 1; i < argc; ++i)
	{
		if (!strcmp(argv[i], "window"))
			windowedGame=true;
	}

	byte buy;

	mainmgl=new MGLDraw("Eddie Galaxy",640,480,windowedGame);
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

#include "intro.h"
#include "backgd.h"
#include "display.h"
#include "status.h"
#include "random.h"
#include "game.h"
#include "sound.h"

static byte wrld;
static byte curLine;
static byte curChar;
static byte countDown;
static byte bright;
static byte blink;
static byte done;

static byte oldKeys,keys;

char txt[15][64];

byte UpdateIntro(int *lastTime)
{
	while(*lastTime>=TIME_PER_FRAME)
	{
		//update everything
		UpdateMouse();
		JamulSoundUpdate();
		UpdateStatusDisplay();
		RetractLogo();
		if(!done)
		{
			if(!countDown)
			{
				curChar++;
				MakeSound(SND_TYPING,200);
				if(curChar==strlen(txt[curLine])-1)
				{
					MakeSound(SND_BLIP,200);
					if(curLine<5)
						countDown=20;
					else
						countDown=5;
				}
			}
			else
			{
				countDown--;
				blink=((countDown&3)>0);
				if(bright>0)
					bright-=2;
				if(countDown==0)
				{
					curLine++;
					curChar=0;
					blink=1;
					bright=32;
					if(curLine==12)
						done=1;
				}
			}
		}

		*lastTime-=TIME_PER_FRAME;
	}

	oldKeys=keys;
	keys=GetControls();

	if(mainmgl->LastKeyPressed() || mainmgl->MouseTap() || ((keys&CONTROL_B1)&&(!(oldKeys&CONTROL_B1))))
		return 0;
	else
		return 1;
}

byte RenderIntro(void)
{
	int i;

	DrawBackgd();

	if(curLine>0)
		Print(5,5,txt[0],2);
	else
		PrintCursor(5,5,txt[0],curChar,blink,bright,2);

	for(i=1;i<5;i++)
	{
		if(curLine>i)
			Print(5,75+i*20,txt[i],1);
		else if(curLine==i)
			PrintCursor(5,75+i*20,txt[i],curChar,blink,bright,1);
	}

	for(i=5;i<12;i++)
	{
		if(curLine>i)
			Print(5,95+i*20,txt[i],1);
		else if(curLine==i)
			PrintCursor(5,95+i*20,txt[i],curChar,blink,bright,1);
	}

	RenderLogo();
	RenderStatusDisplay();
	return FlipScreen();
}

void InitIntro(void)
{
	curLine=0;
	curChar=0;
	countDown=0;
	blink=1;
	bright=32;
	done=0;

	keys=0xFF;

	switch(wrld)
	{
		case 0:	//rigel
			LoadBackgd("graphics\\rigel.bmp");

			strcpy(txt[0],"Rigel IV");
			strcpy(txt[1],"Daytime Surface Temperature: -73*F");
			strcpy(txt[2],"Orbital Inclination: 11.3*");
			strcpy(txt[3],"Rotational Period: 31 hr");
			strcpy(txt[4],"Population: 1.3 million");
			strcpy(txt[5],"  This icy planet has been overrun by");
			strcpy(txt[6],"giant critters that have adapted");
			strcpy(txt[7],"to the cold by putting on");
			strcpy(txt[8],"snowshoes and santa hats.");
			strcpy(txt[9],"  Residents are afraid to");
			strcpy(txt[10],"leave their igloos!  You are");
			strcpy(txt[11],"our only hope, Mr. Galaxy!");
			break;
		case 1: // Agua
			LoadBackgd("graphics\\agua.bmp");

			strcpy(txt[0],"Aguaworld");
			strcpy(txt[1],"Daytime Surface Temperature: 82*F");
			strcpy(txt[2],"Orbital Inclination: 9.2*");
			strcpy(txt[3],"Rotational Period: 23.2 hr");
			strcpy(txt[4],"Population: 3 billion");
			strcpy(txt[5],"  A horrible sea serpent has begun");
			strcpy(txt[6],"to terrorize this peaceful ocean");
			strcpy(txt[7],"world.");
			strcpy(txt[8],"  We couldn't find an");
			strcpy(txt[9],"intergalactic marine biologist,");
			strcpy(txt[10],"so we hoped you'd handle this");
			strcpy(txt[11],"for us.");
			break;
		case 2: // Prometheus
			LoadBackgd("graphics\\prom.bmp");

			strcpy(txt[0],"Prometheus IX");
			strcpy(txt[1],"Daytime Surface Temperature: 56*F");
			strcpy(txt[2],"Orbital Inclination: 3.1*");
			strcpy(txt[3],"Rotational Period: 19 hr");
			strcpy(txt[4],"Population: 783 quintillion (counting nanobots)");
			strcpy(txt[5],"  A deadly computer virus has been");
			strcpy(txt[6],"unleashed, turning once peaceful trash");
			strcpy(txt[7],"collecting robots into engines of");
			strcpy(txt[8],"destruction.");
			strcpy(txt[9],"  Lucky thing you happened to be");
			strcpy(txt[10],"in the area, huh?");
			strcpy(txt[11],"  Please help!");
			break;
		case 3: // Tartarus
			LoadBackgd("graphics\\tart.bmp");

			strcpy(txt[0],"Tartarus II");
			strcpy(txt[1],"Daytime Surface Temperature: 732*F");
			strcpy(txt[2],"Orbital Inclination: 19.2*");
			strcpy(txt[3],"Rotational Period: 51 hr");
			strcpy(txt[4],"Population: 11");
			strcpy(txt[5],"  The fence at Bob's Dragonpede");
			strcpy(txt[6],"Ranch burned up, and the dragonpedes");
			strcpy(txt[7],"have escaped! They're trashing every-");
			strcpy(txt[8],"thing!");
			strcpy(txt[9],"  Do you possess the power to defeat");
			strcpy(txt[10],"them??");
			strcpy(txt[11],"  Sure would be nice if you did.");
			break;
		case 4: // Asteroids
			LoadBackgd("graphics\\astro.bmp");

			strcpy(txt[0],"Asteroid Belt");
			strcpy(txt[1],"Mass Density: 758");
			strcpy(txt[2],"Primary Composition: Fe, Mg, Pb");
			strcpy(txt[3],"Collision Probability: 92.3%");
			strcpy(txt[4],"Population: 4 evil Robot Space Lobsters");
			strcpy(txt[5],"  Unfortunately, the next job is");
			strcpy(txt[6],"right on the other side of this");
			strcpy(txt[7],"dangerous asteroid belt.");
			strcpy(txt[8],"  Even more unfortunately, this is");
			strcpy(txt[9],"the turf of the dreaded Robot Space");
			strcpy(txt[10],"Lobsters.");
			strcpy(txt[11],"  They don't take kindly to strangers.");
			break;
		case 5: // Varakkis
			LoadBackgd("graphics\\varak.bmp");

			strcpy(txt[0],"Varakkis");
			strcpy(txt[1],"Daytime Surface Temperature: 132*F");
			strcpy(txt[2],"Orbital Inclination: 2*");
			strcpy(txt[3],"Rotational Period: 31 hr");
			strcpy(txt[4],"Population: 2.3 billion");
			strcpy(txt[5],"  Strange prickly monsters are");
			strcpy(txt[6],"terrorizing our planet, stampeding");
			strcpy(txt[7],"the cattle and generally distressing");
			strcpy(txt[8],"the damsels.");
			strcpy(txt[9],"  We reckon you might be able to");
			strcpy(txt[10],"help us rustle 'em up.");
			strcpy(txt[11],"  ");
			break;
		case 6: // Jurassika
			LoadBackgd("graphics\\juras.bmp");

			strcpy(txt[0],"Jurassika");
			strcpy(txt[1],"Daytime Surface Temperature: 96*F");
			strcpy(txt[2],"Orbital Inclination: 3.6*");
			strcpy(txt[3],"Rotational Period: 21 hr");
			strcpy(txt[4],"Population: 7.4 million");
			strcpy(txt[5],"  Early interplanetary smoke signals");
			strcpy(txt[6],"from Jurassika seem to indicate an");
			strcpy(txt[7],"assault by giant monsters of some");
			strcpy(txt[8],"kind. Coming from a planet inhabited");
			strcpy(txt[9],"by tyrannosaurs, this is more than");
			strcpy(txt[10],"a little disconcerting.");
			strcpy(txt[11],"  You better investigate.");
			break;
		case 7: // Globulos
			LoadBackgd("graphics\\glob.bmp");

			strcpy(txt[0],"Globulos");
			strcpy(txt[1],"Daytime Surface Temperature: 42*F");
			strcpy(txt[2],"Orbital Inclination: 82*");
			strcpy(txt[3],"Rotational Period: 172 hr");
			strcpy(txt[4],"Population: ???");
			strcpy(txt[5],"  This place is not normal.  But");
			strcpy(txt[6],"it seems some sort of eyeball");
			strcpy(txt[7],"monster is harassing the whatever-");
			strcpy(txt[8],"they-ares that live here.");
			strcpy(txt[9],"  This seems to be related to the");
			strcpy(txt[10],"sudden rash of giant monster");
			strcpy(txt[11],"attacks around the galaxy...");
			break;
		case 8: // Final
			LoadBackgd("graphics\\final.bmp");

			strcpy(txt[0],"Xykulix");
			strcpy(txt[1],"Daytime Surface Temperature: 12*F");
			strcpy(txt[2],"Orbital Inclination: 4*");
			strcpy(txt[3],"Rotational Period: 23.3 hr");
			strcpy(txt[4],"Population: 3 million scary monsters");
			strcpy(txt[5],"  This appears to be the source of all");
			strcpy(txt[6],"the recent trouble.  Some sort of");
			strcpy(txt[7],"hideous, possibly acid-blooded, alien");
			strcpy(txt[8],"queen that we tracked to this world");
			strcpy(txt[9],"was the one deploying the giant");
			strcpy(txt[10],"multilegged creatures all over the");
			strcpy(txt[11],"galaxy.  Squash her!!");
			break;
	}
}

void DoIntro(byte world)
{
	int lastTime;
	dword clock,endclock;

	if(world==1)
		return;

	wrld=world-2;
	clock=0;
	endclock=0;
	lastTime=TIME_PER_FRAME;

	InitIntro();
	mainmgl->LastKeyPressed();
	while(1)
	{
		lastTime+=endclock-clock;
		clock=GetTime();

		if(!UpdateIntro(&lastTime))
			return;

		if(!RenderIntro())
			return;

		endclock=GetTime();
	}
}

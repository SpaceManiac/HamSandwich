#include "theater.h"
#include "sound.h"
#include "display.h"
#include "game.h"
#include "dialogbits.h"
#include "goal.h"

typedef struct movie_t
{
	char title[32];
	char filename[16];
	word sound;
} movie_t;

movie_t movie[32]={
	{"Asylum Welcome Mat","asylumys.flc",SND_TZONE},
	{"Asylum Unwelcome Mat","asylumno.flc",SND_TZONE},
	{"Super Duper Zombie Defeated","asylum.flc",SND_VICTORY},
	{"The Thing Defeated","caverns.flc",SND_VICTORY},
	{"Sphinxter Defeated","desert.flc",SND_VICTORY},
	{"Matilda Defeated","forest.flc",SND_VICTORY},
	{"Yetis Defeated","icymount.flc",SND_VICTORY},
	{"Kongor Defeated","island.flc",SND_VICTORY},
	{"Countess Defeated","mansion.flc",SND_VICTORY},
	{"Loonybot Defeated","station.flc",SND_VICTORY},
	{"A Change For The Worse","transfrm.flc",SND_TZONE},
	{"Bunny The Vampire Slayer Title","buntitl2.flc",SND_BUNNYTHEME},
	{"Bunny The Vampire Slayer End","bunnyend.flc",SND_BUNNYTHEME},
	{"Happy Halloween!","hallow.flc",SND_HAPPYWEEN},
	{"Fall Of The House Of Mushroom","mushbomb.flc",SND_MARKMOVIE},
	{"Marshmallow Head","marshmallow.flc",SND_HAMUMU},
	{"Hungry No More!","statue.flc",SND_VICTORY},
	{"Exit Theater","!",0},
};

static byte cursor,numMovies;
static sprite_set_t *bestSpr;
static int msx,msy,oldmsx,oldmsy;
static int msBright,msDBright;
static byte *backgd;

void InitTheater(MGLDraw *mgl)
{
	int i;

	msBright=0;
	msDBright=1;

	mgl->LoadBMP("graphics/profmenu.bmp");
	backgd=(byte *)malloc(640*480);
	for(i=0;i<480;i++)
		memcpy(&backgd[i*640],&mgl->GetScreen()[i*mgl->GetWidth()],640);

	cursor=0;

	for(i=0;i<32;i++)
		if(movie[i].filename[0]=='!')
		{
			numMovies=i;
			break;
		}

	bestSpr=new sprite_set_t("graphics/pause.jsp");
	GetTaps();
	GetArrowTaps();
	mgl->LastKeyPressed();
	mgl->GetMouse(&oldmsx,&oldmsy);
	mgl->MouseTap();
	if(profile.progress.movie[0])
		profile.progress.movie[1]=1;
	SaveProfile();
	GoalPurchase();
}

void ExitTheater(void)
{
	free(backgd);
	delete bestSpr;
}

byte AllMoviesSeen(void)
{
	int i;

	for(i=0;i<32;i++)
	{
		if(movie[i].filename[0]=='!')
			return 1;
		if(!profile.progress.movie[i])
			return 0;
	}

	return 1;
}

TASK(byte) UpdateTheater(int *lastTime,MGLDraw *mgl)
{
	byte c;

	if(*lastTime>TIME_PER_FRAME*5)
		*lastTime=TIME_PER_FRAME*5;

	while(*lastTime>=TIME_PER_FRAME)
	{
		mgl->Process();
		msBright+=msDBright;
		if(msBright>10)
			msDBright=-1;
		if(msBright<-2)
			msDBright=1;

		*lastTime-=TIME_PER_FRAME;
	}

	mgl->GetMouse(&msx,&msy);
	if(mgl->LastKeyPressed()==27)
		CO_RETURN 1;

	c=GetTaps()|GetArrowTaps();

	if(c&CONTROL_UP)
	{
		cursor--;
		if(cursor>numMovies)
			cursor=numMovies;
	}
	if(c&CONTROL_DN)
	{
		cursor++;
		if(cursor>numMovies)
			cursor=0;
	}

	if(mgl->MouseTap() || (c&CONTROL_B1))
	{
		if(cursor==numMovies)
			CO_RETURN 1;
		else if(profile.progress.movie[cursor])
		{
			MakeNormalSound(movie[cursor].sound);
			AWAIT ShowImageOrFlic(movie[cursor].filename,1,0);
			JamulSoundPurge();
		}
		else
			MakeNormalSound(SND_TURRETBZZT);
	}

	CO_RETURN 0;
}

void RenderTheater(MGLDraw *mgl)
{
	int i;
	int x,y;
	char txt[32];
	int msx2,msy2;

	for(i=0;i<480;i++)
		memcpy(&GetDisplayMGL()->GetScreen()[i*GetDisplayMGL()->GetWidth()],&backgd[i*640],640);

	x=21;
	y=21;

	PrintGlow(288,21,"Welcome to SpisMall Unoplex Theaters!",0,2);
	PrintGlow(288,51,"Select a film, get some popcorn, and enjoy!",0,2);
	for(i=0;i<numMovies+1;i++)
	{
		if(oldmsx!=msx || oldmsy!=msy)
		{
			if(PointInRect(msx,msy,x,y,x+260,y+18))
				cursor=i;
		}

		if(cursor==i)
		{
			mgl->FillBox(x,y,x+260,y+18,32*1+8);
			mgl->Box(x,y,x+260,y+18,31);
			mgl->Box(x-1,y-1,x+261,y+19,31);
		}
		else
			mgl->Box(x,y,x+260,y+18,32*1+16);

		if(profile.progress.movie[i] || i==numMovies)
			strcpy(txt,movie[i].title);
		else
			strcpy(txt,"? ? ? ? ? ?");
		PrintGlowLimited(x+2,y+2,x+258,txt,0,2);
		y+=21;
		if(movie[i].filename[0]=='!')
			break;
	}

	SetSpriteConstraints(13,13,627,467);
	msx2=msx;
	msy2=msy;
	if(msx2<13)
		msx2=13;
	if(msy2<13)
		msy2=13;
	if(msx2>622)
		msx2=622;
	if(msy2>462)
		msy2=462;
	bestSpr->GetSprite(0)->DrawBright(msx2,msy2,mgl,msBright/2);
	ClearSpriteConstraints();

	oldmsx=msx;
	oldmsy=msy;
}

TASK(void) Theater(MGLDraw *mgl)
{
	byte done=0;
	int lastTime=1;

	InitTheater(mgl);
	GetTaps();
	while(!done)
	{
		lastTime+=TimeLength();
		StartClock();

		done=AWAIT UpdateTheater(&lastTime,mgl);
		RenderTheater(mgl);
		AWAIT mgl->Flip();

		if(!mgl->Process())
			done=1;
		EndClock();
	}

	ExitTheater();
}

void SeeMovie(char *fname)
{
	int i;
	char tmp[32];

	strcpy(tmp,fname);
	//_strlwr(tmp);

	for(i=0;i<32;i++)
	{
		if(!strcasecmp(movie[i].filename,tmp))
			profile.progress.movie[i]=1;
	}
}

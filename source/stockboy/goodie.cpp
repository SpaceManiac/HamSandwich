#include "goodie.h"
#include "highscore.h"
#include "stars.h"
#include "options.h"

static int curStars;
static byte goodieOn=0,goodieMode,goodieWait;
static int goodieX,goodieY,goodieSize;
static byte noStars;

void InitGoodie(int x,int y)
{
	if((GameType()!=GAME_STOCKROOM || TestingLevel() || profile.zodiac[player.worldNum]) &&
	   (GameType()!=GAME_PARALLEL || profile.zodiac2[player.worldNum]))
	{
		noStars=1;
	}
	else
		noStars=0;

	goodieOn=1;
	curStars=profile.starsLeft;
	if(!noStars)
	{
		profile.stars+=GOODIE_STARS;
		profile.starsLeft+=GOODIE_STARS;
		if(GameType()==GAME_PARALLEL)
			profile.zodiac2[player.worldNum]=1;
		else if(GameType()==GAME_STOCKROOM)
			profile.zodiac[player.worldNum]=1;
		SaveProfile(opt.curProfile);
	}

	goodieX=(x*TILE_WIDTH+TILE_WIDTH/2);
	goodieY=(y*TILE_HEIGHT+TILE_HEIGHT/2);
	goodieSize=0;
	MakeNormalSound(SND_GETGOODIE);
	InitStars(4,curStars);
	goodieMode=0;
	if(GameType()==GAME_PARALLEL)
		NewBigMessage("!etelpmoc laog neddiH",60);
	else
		NewBigMessage("Hidden goal complete!",60);
}

void ShutoffGoodie(void)
{
	goodieOn=0;
	InitStars(4,curStars);
}

void UpdateGoodie(void)
{
	int i;

	if(!goodieOn)
		return;

	goodieX+=(320-goodieX)/16;
	goodieY+=(240-goodieY)/16;

	switch(goodieMode)
	{
		case 0:	// growing
			if(goodieSize<9*2-1)
				goodieSize++;
			else
				goodieMode=1;
			break;
		case 1: // release spores
			if(!noStars)
				for(i=0;i<GOODIE_STARS;i++)
					LaunchStarAngle(goodieX*FIXAMT,goodieY*FIXAMT,(byte)(i*256/GOODIE_STARS));

			goodieWait=0;
			goodieMode=2;
			break;
		case 2:	// sitting
			if(goodieWait<100)
				goodieWait++;
			else
				goodieMode=3;
			break;
		case 3: // shrinking
			if(goodieSize>0)
				goodieSize--;
			else if(!StarsLeft())
				goodieOn=0;
			break;
	}
	UpdateStars();
}

void RenderGoodie(MGLDraw *mgl)
{
	if(!goodieOn)
		return;

	if(goodieSize>0)
		//RenderStar(0,20+goodieSize/2,goodieX*FIXAMT,goodieY*FIXAMT,mgl);
		RenderGlowStar(21+goodieSize/2,goodieX*FIXAMT,goodieY*FIXAMT,mgl);

	RenderEndStar(mgl);
	RenderStars(mgl);
}
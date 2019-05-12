#include "bowling.h"
#include "options.h"

byte level;
byte bonus;
byte ball;
byte gamedone;

dword ticktock;

void InitBowling(Map *map)
{
	level=0;
	bonus=0;
	ball=0;
	ticktock=0;
	gamedone=0;
	player.gemsGotten=0;
	SetupLane(map);
}

void SetupLane(Map *map)
{
	int i,tries,pos,amt;
	Guy *g;

	if(gamedone)
		return;

	player.combo=0;
	player.comboClock=0;

	// place graves
	for(i=0;i<map->width*map->height;i++)
		if(map->map[i].tag==5)
			map->map[i].item=0;

	amt=(level+1)*2;
	// on level 2 or 3, there is a 75% chance that a grave will be SPECIFICALLY placed
	// in the center column
	if((level==2 || level==3) && Random(4)!=0)
	{
		amt--;
		tries=0;
		while(tries++<1000)
		{
			pos=Random(map->height)+map->width/2;
			if(map->map[pos].tag==5 && map->map[pos].item==0)
			{
				map->map[pos].item=ITM_GRAVE;
				tries=1000;
			}
		}
	}
	for(i=0;i<(level+1)*2;i++)
	{
		tries=0;
		while(tries++<1000)
		{
			pos=Random(map->width*map->height);
			if(map->map[pos].tag==5 && map->map[pos].item==0)
			{
				map->map[pos].item=ITM_GRAVE;
				tries=1000;
			}
		}
	}

	// place punkins
	DeleteMonsters();
	pos=0;
	for(i=0;i<10;i++)
	{
		while(map->map[pos].tag!=1)
			pos++;
		g=AddGuy(((pos%map->width)*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
			   ((pos/map->width)*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,
			   FIXAMT*10,MONS_PUMPKIN);
		if(g)
			g->tag=1;

		pos++;
	}

	// place balls
	for(i=18;i>13;i--)
	{
		if(opt.cheats[CH_KICKCAT])
			map->map[15+(i*map->width)].item=ITM_CAT;
		else
			map->map[15+(i*map->width)].item=ITM_MYSORB;
	}
	ball=0;
}

void RollBalls(void)
{
	int i;

	for(i=18;i>13;i--)
	{
		// move the balls down one
		curMap->map[15+(i*curMap->width)].item=
			curMap->map[15+((i-1)*curMap->width)].item;
	}
}

void BowlingBallIsDone(void)
{
	ball++;
	ticktock=40;
}

void GutterBall(void)
{
	NewBigMessage("Gutterball!",60);
	MakeNormalSound(SND_PENALTYOPEN);
	player.specialShotsFired++;
	ball++;
	ticktock=20;
}

void ThrowBowlingBall(void)
{
	if(player.money)
	{
		player.shotsFired++;
		player.money=0;
		FireBullet(goodguy->x,goodguy->y,goodguy->facing*32,BLT_BOWLINGBALL);
		if(opt.cheats[CH_KICKCAT])
			MakeNormalSound(SND_CATKICK);
		else
			MakeNormalSound(SND_BALLKICK);
	}
}

void RenderBowlingIntface(MGLDraw *mgl)
{
	char s[32];

	if(player.money)
		InstaRenderItem(620,22,ITM_MYSORB,0,mgl);

	if(bonus==0)
		sprintf(s,"Round %d",level+1);
	else
		sprintf(s,"Round %d-B",level+1);
	Print(2,455,s,0,0);
	sprintf(s,"Score: %d",player.monsterPoints);
	RightPrint(638,455,s,0,0);
}

void UpdateBowling(void)
{
	if(gamedone)
		return;

	if(ticktock)
	{
		ticktock--;
		if(ticktock==0)
		{
			if(!TaggedMonsterExists(1))
			{
				// got a strike!
				if(ball==1)
				{
					NewBigMessage("STRIKE!!!",60);
					MakeNormalSound(SND_WIN);
					player.gemsGotten++;	// # of strikes is stored here

					if(bonus==0)
						bonus=1;
					else
					{
						bonus=0;
						level++;
						if(level==5)
						{
							SendMessageToGame(MSG_RESET,0);
							gamedone=1;
							level=4;
							return;
						}
					}
					SetupLane(curMap);
					return;
				}
				else
				{
					// got a spare
					bonus=0;
					level++;
					if(level==5)
					{
						SendMessageToGame(MSG_RESET,0);
						gamedone=1;
						level=4;
						return;
					}
					SetupLane(curMap);
					return;
				}
			}
			if(ball==5)
			{
				bonus=0;
				level++;
				if(level==5)
				{
					SendMessageToGame(MSG_RESET,0);
					gamedone=1;
					level=4;
					return;
				}
				SetupLane(curMap);
			}
			else
				RollBalls();
		}
	}
}

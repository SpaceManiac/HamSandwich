#include "loonyball.h"
#include "badge.h"

int penaltyClock[2];

void InitLoonyball(Map *map)
{
	player.maxHearts=5+player.levelNum;
	player.hearts=5+player.levelNum;
	player.startHearts=5+player.levelNum;
	player.firePower=player.levelNum;
	player.fireRange=player.levelNum;
	player.fireRate=player.levelNum;

	player.money=0;
	player.maxMoney=3;
	if(player.levelNum==0)
	{
		player.oob=0;
		player.gemsGotten=5000;
	}

	player.var[VAR_TORCH]=1;
	player.timeLimit=2*60*30;
	penaltyClock[0]=0;
	penaltyClock[1]=0;
	FireBullet(((map->width/2)*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
		((map->height/2)*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,0,BLT_LOONYBALL);
	NewBigMessage(map->name,90);
	if(player.levelNum==4)
	{
		NewMessage("Score all you can in 2 minutes!",90);
		player.maxMoney=255;
	}
	else
		NewMessage("Score 3 goals in 2 minutes!",90);
}

void ScoreGoal(bullet_t *me,Map *map)
{
	if(me->y<FIXAMT*TILE_HEIGHT*6 && ((player.levelNum>0 && me->x>=FIXAMT*TILE_WIDTH*13 && me->x<FIXAMT*TILE_WIDTH*18) ||
		(player.levelNum==0 && me->x>=FIXAMT*TILE_WIDTH*12 && me->x<FIXAMT*TILE_WIDTH*19)))
	{
		// scored a goal!!!!!!
		MakeNormalSound(SND_GOAL);
		NewBigMessage("GGGGOOOOOOOAAALLLL!!!!!",60);
		player.money++;
		player.gemsGotten++;
		// make an explosion of joy
		GlassShatter(me->x-8*FIXAMT,me->y-8*FIXAMT,me->x+8*FIXAMT,me->y+8*FIXAMT,me->z,30);
		GlassShatter(me->x-100*FIXAMT,me->y-100*FIXAMT,me->x+100*FIXAMT,me->y+100*FIXAMT,me->z,20);
		// restart the ball
		me->type=BLT_NONE;
		FireBullet(((map->width/2)*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
			((map->height/2)*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,0,BLT_LOONYBALL);
		if(player.money>=player.maxMoney)
		{
			MakeNormalSound(SND_WIN);
			SendMessageToGame(MSG_WINLEVEL,0);
		}
	}
	if(me->y>(map->height*FIXAMT*TILE_HEIGHT)-4*FIXAMT*TILE_HEIGHT)
	{
		// scored a goal on yourself!
		MakeNormalSound(SND_GOAL);
		NewBigMessage("OOOH!! Score for the home team!",60);
		player.money--;
		player.gemsGotten--;
		// make an explosion of joy
		GlassShatter(me->x-8*FIXAMT,me->y-8*FIXAMT,me->x+8*FIXAMT,me->y+8*FIXAMT,me->z,30);
		GlassShatter(me->x-100*FIXAMT,me->y-100*FIXAMT,me->x+100*FIXAMT,me->y+100*FIXAMT,me->z,20);
		// restart the ball
		me->type=BLT_NONE;
		FireBullet(((map->width/2)*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
			((map->height/2)*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,0,BLT_LOONYBALL);
	}
}

void UpdateLoonyball(void)
{
	if(player.timeLimit>0)
		player.timeLimit--;
	else
	{
		if(player.levelNum<4)
			SendMessageToGame(MSG_RESET,0);
		else
			SendMessageToGame(MSG_WINLEVEL,0);
	}

	if(penaltyClock[0]>0)
	{
		penaltyClock[0]--;
		if(!penaltyClock[0])
		{
			MakeNormalSound(SND_PENALTYOPEN);
			DropItemTag(curMap,0,1,1);
		}
	}
	if(penaltyClock[1]>0)
	{
		penaltyClock[1]--;
		if(!penaltyClock[1])
		{
			MakeNormalSound(SND_PENALTYOPEN);
			DropItemTag(curMap,0,2,1);
		}
	}
}

void GoToPenaltyBox(Guy *me,Map *map)
{
	int i,x,y;
	byte dest;

	MakeNormalSound(SND_PENALTY);
	if(me->type==player.monsType)
	{
		dest=4;
		penaltyClock[1]=1*30;
		NewBigMessage("10 Second Penalty!",90);
		DropItemTag(map,68,2,0);
		player.numSaves++;
		if(player.timeLimit>9*30)
		{
			player.timeLimit-=9*30;
			player.timeToFinish+=9*30;
		}
		else
		{
			player.timeToFinish+=player.timeLimit;
			player.timeLimit=0;
		}
	}
	else
	{
		dest=3;
		penaltyClock[0]=10*30;
		DropItemTag(map,68,1,0);
	}

	x=(TILE_WIDTH/2)*FIXAMT;
	y=(TILE_HEIGHT/2)*FIXAMT;
	for(i=0;i<map->width*map->height;i++)
	{
		if(map->map[i].tag==dest)
		{
			if(me->CanWalk(x,y,map,&curWorld))
			{
				me->x=x;
				me->y=y;
				return;
			}
		}
		x+=TILE_WIDTH*FIXAMT;
		if((i%map->width)==0)
		{
			x=(TILE_WIDTH/2)*FIXAMT;
			y+=TILE_HEIGHT*FIXAMT;
		}
	}
	penaltyClock[0]=0;
	penaltyClock[1]=0;
	NewBigMessage("Riot!!",90);
	MakeNormalSound(SND_WIN);
	EarnBadge(BADGE_RIOT);
	if(dest==3)
		DropItemTag(map,0,1,1);
	else
		DropItemTag(map,0,2,1);

}

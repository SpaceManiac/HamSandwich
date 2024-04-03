#include "goal.h"
#include "map.h"
#include "floor.h"
#include "player.h"
#include "display.h"
#include "score.h"

byte goalCheck[8];

const char *GoalColorName(byte mod)
{
	static char colName[8][8]={"Grey","Green","Brown","Blue","Red","Yellow","Purple","Aqua"};
	static char monsName[4][8]={"Blobby","Fluffy","Inchy","Piggy"};
	static char pointsName[4][8]={"x1","x10","x100","x1,000"};
	static char noColor[16]="ILLEGAL";

	if(mod==GM_ALL)
		return "Any";
	else if(mod>=GM_COLOR0 && mod<=GM_COLOR7)
		return colName[mod-GM_COLOR0];
	else if(mod>=GM_MONS1 && mod<=GM_MONS4)
		return monsName[mod-GM_MONS1];
	else if(mod>=GM_X1 && mod<=GM_X1000)
		return pointsName[mod-GM_X1];
	else
		return noColor;
}

const char *CreateGoalString(goal_t goal)
{
	static char txt[64];

	switch(goal.type)
	{
		case GOAL_NONE:
			strcpy(txt,"None.");
			break;
		case GOAL_LIVE:
			strcpy(txt,"Survive!");
			break;
		case GOAL_PUSH:
			if(goal.modifier==GM_ALL)
				sprintf(txt,"Deliver %d objects.",goal.number);
			else
				sprintf(txt,"Deliver %d %s objects.",goal.number,GoalColorName(goal.modifier));
			if(goal.number==1)
			{
				txt[strlen(txt)-2]='.';
				txt[strlen(txt)-1]='\0';
			}
			break;
		case GOAL_EXPLODE:
			if(goal.modifier==GM_ALL)
				sprintf(txt,"Destroy %d objects.",goal.number);
			else
				sprintf(txt,"Destroy %d %s objects.",goal.number,GoalColorName(goal.modifier));
			if(goal.number==1)
			{
				txt[strlen(txt)-2]='.';
				txt[strlen(txt)-1]='\0';
			}
			break;
		case GOAL_KILL:
			if(goal.modifier==GM_ALL)
				sprintf(txt,"Defeat %d critters.",goal.number);
			else
				sprintf(txt,"Defeat %d %s critters.",goal.number,GoalColorName(goal.modifier));
			if(goal.number==1)
			{
				txt[strlen(txt)-2]='.';
				txt[strlen(txt)-1]='\0';
			}
			break;
		case GOAL_EXIT:
			strcpy(txt,"Get to the exit!");
			break;
		case GOAL_SAVE:
			if(goal.modifier==GM_ALL)
				sprintf(txt,"Rescue %d critters.",goal.number);
			else
				sprintf(txt,"Rescue %d %s critters.",goal.number,GoalColorName(goal.modifier));
			if(goal.number==1)
			{
				txt[strlen(txt)-2]='.';
				txt[strlen(txt)-1]='\0';
			}
			break;
		case GOAL_SCORE:
			if(goal.modifier==GM_X1)
			{
				sprintf(txt,"Earn %d points.",goal.number);
				if(goal.number==1)
				{
					txt[strlen(txt)-2]='.';
					txt[strlen(txt)-1]='\0';
				}
			}
			else if(goal.modifier==GM_X10)
				sprintf(txt,"Earn %d points.",goal.number*10);
			else if(goal.modifier==GM_X100)
				sprintf(txt,"Earn %d points.",goal.number*100);
			else if(goal.modifier==GM_X1000)
				sprintf(txt,"Earn %d points.",goal.number*1000);
			break;
		case GOAL_DUMP:
			if(goal.modifier==GM_ALL)
				sprintf(txt,"Dump %d objects.",goal.number);
			else
				sprintf(txt,"Dump %d %s objects.",goal.number,GoalColorName(goal.modifier));
			if(goal.number==1)
			{
				txt[strlen(txt)-2]='.';
				txt[strlen(txt)-1]='\0';
			}
			break;
		case GOAL_DUMPMONS:
			if(goal.modifier==GM_ALL)
				sprintf(txt,"Dump %d critters.",goal.number);
			else
				sprintf(txt,"Dump %d %s critters.",goal.number,GoalColorName(goal.modifier));
			if(goal.number==1)
			{
				txt[strlen(txt)-2]='.';
				txt[strlen(txt)-1]='\0';
			}
			break;
		case GOAL_PENCIL:
			if(goal.modifier==GM_ALL)
				sprintf(txt,"Collect %d pencils.",goal.number);
			else
				sprintf(txt,"Collect %d %s pencils.",goal.number,GoalColorName(goal.modifier));
			if(goal.number==1)
			{
				txt[strlen(txt)-2]='.';
				txt[strlen(txt)-1]='\0';
			}
			break;
		case GOAL_NOPENCIL:
			if(goal.modifier==GM_ALL)
				sprintf(txt,"Destroy %d pencils.",goal.number);
			else
				sprintf(txt,"Destroy %d %s pencils.",goal.number,GoalColorName(goal.modifier));
			if(goal.number==1)
			{
				txt[strlen(txt)-2]='.';
				txt[strlen(txt)-1]='\0';
			}
			break;
	}
	return txt;
}

void PrintGoal(goal_t goal,goal_t antigoal,int timer)
{
	char txt[256];
	byte c;
	sprintf(txt,"GOAL: %s",CreateGoalString(goal));

	if((goal.modifier>=GM_ALL && goal.modifier<=GM_MONS4) || (goal.modifier>=GM_X1))
		c=8;
	else
		c=goal.modifier-GM_COLOR0;

	if(c!=8)
	{
		CenterPrint(319,419,txt,-32,2);
		CenterPrint(321,421,txt,-32,2);
		CenterPrintColor(320,420,txt,c,-12,2);
	}
	else
		CenterPrintOutline(320,420,txt,0,2);

	if(antigoal.type!=GOAL_NONE)
	{
		sprintf(txt,"DO NOT: %s",CreateGoalString(antigoal));
		if((antigoal.modifier>=GM_ALL && antigoal.modifier<=GM_MONS4) || (antigoal.modifier>=GM_X1))
			c=8;
		else
			c=antigoal.modifier-GM_COLOR0;

		if(c!=8)
		{
			CenterPrint(319,449,txt,-32,2);
			CenterPrint(321,451,txt,-32,2);
			CenterPrintColor(320,450,txt,c,-12,2);
		}
		else
			CenterPrintOutline(320,450,txt,0,2);
	}

	if(timer>0)
	{
		//sprintf(txt,"TIME LIMIT: %d:%2d",timer/60,timer%60);
		//CenterPrintOutline(320,2,txt,0,2);
	}
}

const char *EditGoalName(goal_t goal)
{
	switch(goal.type)
	{
		case GOAL_NONE:
			return "None.";
			break;
		case GOAL_LIVE:
			return "Survive.";
			break;
		case GOAL_PUSH:
			return "Deliver";
			break;
		case GOAL_EXPLODE:
			return "Destroy";
			break;
		case GOAL_KILL:
			return "Defeat";
			break;
		case GOAL_EXIT:
			return "Escape!";
			break;
		case GOAL_SAVE:
			return "Rescue";
			break;
		case GOAL_SCORE:
			return "Earn";
			break;
		case GOAL_DUMP:
			return "Dump";
			break;
		case GOAL_DUMPMONS:
			return "Dump";
			break;
		case GOAL_PENCIL:
			return "Collect";
			break;
		case GOAL_NOPENCIL:
			return "Destroy";
			break;
	}
	return "ILLEGAL!";
}

const char *EditGoalEnding(goal_t goal)
{
	switch(goal.type)
	{
		case GOAL_NONE:
			return "";
			break;
		case GOAL_LIVE:
			return "";
			break;
		case GOAL_PUSH:
			return "objects.";
			break;
		case GOAL_EXPLODE:
			return "objects.";
			break;
		case GOAL_KILL:
			return "critters.";
			break;
		case GOAL_EXIT:
			return "";
			break;
		case GOAL_SAVE:
			return "critters.";
			break;
		case GOAL_SCORE:
			return "points.";
			break;
		case GOAL_DUMP:
			return "objects.";
			break;
		case GOAL_DUMPMONS:
			return "critters.";
			break;
		case GOAL_PENCIL:
		case GOAL_NOPENCIL:
			return "pencils.";
			break;
	}
	return "ILLEGAL!";
}

// returns 1 if the goal is one that uses numbers
byte GoalHasNumber(goal_t goal)
{
	return ((goal.type>=GOAL_PUSH && goal.type<=GOAL_KILL) || (goal.type>=GOAL_SAVE &&
		goal.type<=GOAL_NOPENCIL));
}

byte GoalModType(goal_t goal)
{
	if((goal.type>=GOAL_PUSH && goal.type<=GOAL_EXPLODE) || goal.type==GOAL_DUMP || goal.type==GOAL_PENCIL || goal.type==GOAL_NOPENCIL)
		return GMT_OBJECT;
	if(goal.type==GOAL_KILL || goal.type==GOAL_SAVE || goal.type==GOAL_DUMPMONS)
		return GMT_MONSTER;
	if(goal.type==GOAL_SCORE)
		return GMT_POINTS;

	return GMT_NONE;
}

void ChangeGoalType(goal_t *goal,byte noneok)
{
	goal->type++;
	if(goal->type>=NUM_GOALS)
	{
		if(noneok)
			goal->type=GOAL_NONE;
		else
			goal->type=GOAL_LIVE;
	}
	goal->number=1;
	if(goal->type!=GOAL_SCORE)
		goal->modifier=GM_ALL;
	else
		goal->modifier=GM_X1;
}

void ChangeGoalMod(goal_t *goal)
{
	switch(goal->type)
	{
		case GOAL_PUSH:
		case GOAL_EXPLODE:
		case GOAL_DUMP:
		case GOAL_PENCIL:
		case GOAL_NOPENCIL:
			goal->modifier++;
			if(goal->modifier>GM_COLOR7)
				goal->modifier=GM_ALL;
			if(goal->modifier==GM_MONS1)
				goal->modifier=GM_COLOR0;
			break;
		case GOAL_KILL:
		case GOAL_SAVE:
		case GOAL_DUMPMONS:
			goal->modifier++;
			if(goal->modifier>GM_COLOR7)
				goal->modifier=GM_ALL;
			break;
		case GOAL_SCORE:
			goal->modifier++;
			if(goal->modifier>GM_X1000)
				goal->modifier=GM_X1;
			break;
	}
}

void CheckForDeliveries(Map *map)
{
	int i;
	mapTile_t *m;

	for(i=0;i<8;i++)
		goalCheck[i]=0;

	for(i=0;i<map->width*map->height;i++)
	{
		m=&map->map[i];

		if(FloorOnlyColor(m->floor)!=ANY_COLOR &&
			FloorOnlyColor(m->floor)==m->itemInfo%8 &&
			(m->item==ITM_BARREL || m->item==ITM_BALL || m->item==ITM_CRATE || m->item==ITM_MOVINGBALL ||
			m->item==ITM_ICEBERG))
		{
			goalCheck[FloorOnlyColor(m->floor)]++;
		}
	}
}

byte GoalProgress(goal_t *goal,Map *map)
{
	int value,target,i,j;

	target=0;
	value=0;
	switch(goal->type)
	{
		case GOAL_PUSH:
			CheckForDeliveries(map);
			if(goal->modifier==GM_ALL)
			{
				value=0;
				for(i=0;i<8;i++)
					value+=goalCheck[i];
			}
			else
				value=goalCheck[goal->modifier-GM_COLOR0];
			target=goal->number;
			break;
		case GOAL_EXPLODE:
			if(goal->modifier==GM_ALL)
			{
				value=0;
				for(i=0;i<8;i++)
					value+=player.blownUp[i];
			}
			else
				value=player.blownUp[goal->modifier-GM_COLOR0];
			target=goal->number;
			break;
		case GOAL_KILL:
			if(goal->modifier==GM_ALL)
			{
				value=0;
				for(i=0;i<8;i++)
					for(j=0;j<4;j++)
						value+=player.blownUpMons[i][j];
			}
			else if(goal->modifier>=GM_COLOR0 && goal->modifier<=GM_COLOR7)
			{
				value=0;
				for(j=0;j<4;j++)
					value+=player.blownUpMons[goal->modifier-GM_COLOR0][j];
			}
			else
			{
				// a specific monster type
				value=0;
				for(i=0;i<8;i++)
					value+=player.blownUpMons[i][goal->modifier-GM_MONS1];
			}
			target=goal->number;
			break;
		case GOAL_SAVE:
			if(goal->modifier==GM_ALL)
			{
				value=0;
				for(i=0;i<8;i++)
					for(j=0;j<4;j++)
						value+=player.rescueMons[i][j];
			}
			else if(goal->modifier>=GM_COLOR0 && goal->modifier<=GM_COLOR7)
			{
				value=0;
				for(j=0;j<4;j++)
					value+=player.rescueMons[goal->modifier-GM_COLOR0][j];
			}
			else
			{
				// a specific monster type
				value=0;
				for(i=0;i<8;i++)
					value+=player.rescueMons[i][goal->modifier-GM_MONS1];
			}
			target=goal->number;
			break;
		case GOAL_SCORE:
			target=goal->number;
			if(goal->modifier==GM_X10)
				target*=10;
			else if(goal->modifier==GM_X100)
				target*=100;
			else if(goal->modifier==GM_X1000)
				target*=1000;
			value=player.score;
			break;
		case GOAL_EXIT:
			// no progress shown
			break;
		case GOAL_DUMP:
			if(goal->modifier==GM_ALL)
			{
				value=0;
				for(i=0;i<8;i++)
					value+=player.dumpedObj[i];
			}
			else
				value=player.dumpedObj[goal->modifier-GM_COLOR0];
			target=goal->number;
			break;
		case GOAL_DUMPMONS:
			if(goal->modifier==GM_ALL)
			{
				value=0;
				for(i=0;i<8;i++)
					for(j=0;j<4;j++)
						value+=player.dumpedMons[i][j];
			}
			else if(goal->modifier>=GM_COLOR0 && goal->modifier<=GM_COLOR7)
			{
				value=0;
				for(j=0;j<4;j++)
					value+=player.dumpedMons[goal->modifier-GM_COLOR0][j];
			}
			else
			{
				// a specific monster type
				value=0;
				for(i=0;i<8;i++)
					value+=player.dumpedMons[i][goal->modifier-GM_MONS1];
			}
			target=goal->number;
			break;
		case GOAL_PENCIL:
			if(goal->modifier==GM_ALL)
			{
				value=0;
				for(i=0;i<8;i++)
					value+=player.pencils[i];
			}
			else if(goal->modifier>=GM_COLOR0 && goal->modifier<=GM_COLOR7)
			{
				value=0;
				value+=player.pencils[goal->modifier-GM_COLOR0];
			}
			target=goal->number;
			break;
		case GOAL_NOPENCIL:
			if(goal->modifier==GM_ALL)
			{
				value=0;
				for(i=0;i<8;i++)
					value+=player.deadPencils[i];
			}
			else if(goal->modifier>=GM_COLOR0 && goal->modifier<=GM_COLOR7)
			{
				value=0;
				value+=player.deadPencils[goal->modifier-GM_COLOR0];
			}
			target=goal->number;
			break;
	}

	if(target==0)
		return 0;
	else if(value>=target)
		return 255;
	else
	{
		target=(value*256/target);
		if(target>255)
			target=255;
		return (byte)target;
	}
}

byte GoalIsMet(goal_t *goal,Map *map)
{
	int i,j;
	int n;

	switch(goal->type)
	{
		case GOAL_PUSH:
			CheckForDeliveries(map);
			if(goal->modifier==GM_ALL)
			{
				n=0;
				for(i=0;i<8;i++)
					n+=goalCheck[i];
			}
			else
				n=goalCheck[goal->modifier-GM_COLOR0];
			return (n>=goal->number);
			break;
		case GOAL_EXPLODE:
			if(goal->modifier==GM_ALL)
			{
				n=0;
				for(i=0;i<8;i++)
					n+=player.blownUp[i];
			}
			else
				n=player.blownUp[goal->modifier-GM_COLOR0];
			return (n>=goal->number);
			break;
		case GOAL_KILL:
			if(goal->modifier==GM_ALL)
			{
				n=0;
				for(i=0;i<8;i++)
					for(j=0;j<4;j++)
						n+=player.blownUpMons[i][j];
			}
			else if(goal->modifier>=GM_COLOR0 && goal->modifier<=GM_COLOR7)
			{
				n=0;
				for(j=0;j<4;j++)
					n+=player.blownUpMons[goal->modifier-GM_COLOR0][j];
			}
			else
			{
				// a specific monster type
				n=0;
				for(i=0;i<8;i++)
					n+=player.blownUpMons[i][goal->modifier-GM_MONS1];
			}
			return (n>=goal->number);
			break;
		case GOAL_SAVE:
			if(goal->modifier==GM_ALL)
			{
				n=0;
				for(i=0;i<8;i++)
					for(j=0;j<4;j++)
						n+=player.rescueMons[i][j];
			}
			else if(goal->modifier>=GM_COLOR0 && goal->modifier<=GM_COLOR7)
			{
				n=0;
				for(j=0;j<4;j++)
					n+=player.rescueMons[goal->modifier-GM_COLOR0][j];
			}
			else
			{
				// a specific monster type
				n=0;
				for(i=0;i<8;i++)
					n+=player.rescueMons[i][goal->modifier-GM_MONS1];
			}
			return (n>=goal->number);
			break;
		case GOAL_SCORE:
			n=goal->number;
			if(goal->modifier==GM_X10)
				n*=10;
			else if(goal->modifier==GM_X100)
				n*=100;
			else if(goal->modifier==GM_X1000)
				n*=1000;
			if(!Score_ComboGoing())
				return (player.score>=n);
			else
				return 0;	// can't finish until combo is done
			break;
		case GOAL_EXIT:
			if(FloorIsExit(map->map[goodguy->mapx+goodguy->mapy*map->width].floor))
			{
				player.escapeBonus=1;
				return 1;
			}
			break;
		case GOAL_DUMP:
			if(goal->modifier==GM_ALL)
			{
				n=0;
				for(i=0;i<8;i++)
					n+=player.dumpedObj[i];
			}
			else
				n=player.dumpedObj[goal->modifier-GM_COLOR0];
			return (n>=goal->number);
			break;
		case GOAL_DUMPMONS:
			if(goal->modifier==GM_ALL)
			{
				n=0;
				for(i=0;i<8;i++)
					for(j=0;j<4;j++)
						n+=player.dumpedMons[i][j];
			}
			else if(goal->modifier>=GM_COLOR0 && goal->modifier<=GM_COLOR7)
			{
				n=0;
				for(j=0;j<4;j++)
					n+=player.dumpedMons[goal->modifier-GM_COLOR0][j];
			}
			else
			{
				// a specific monster type
				n=0;
				for(i=0;i<8;i++)
					n+=player.dumpedMons[i][goal->modifier-GM_MONS1];
			}
			return (n>=goal->number);
			break;
		case GOAL_PENCIL:
			if(goal->modifier==GM_ALL)
			{
				n=0;
				for(i=0;i<8;i++)
					n+=player.pencils[i];
			}
			else if(goal->modifier>=GM_COLOR0 && goal->modifier<=GM_COLOR7)
			{
				n=0;
				n+=player.pencils[goal->modifier-GM_COLOR0];
			}
			return (n>=goal->number);
			break;
		case GOAL_NOPENCIL:
			if(goal->modifier==GM_ALL)
			{
				n=0;
				for(i=0;i<8;i++)
					n+=player.deadPencils[i];
			}
			else if(goal->modifier>=GM_COLOR0 && goal->modifier<=GM_COLOR7)
			{
				n=0;
				n+=player.deadPencils[goal->modifier-GM_COLOR0];
			}
			return (n>=goal->number);
			break;
	}
	return 0;
}

#include "intface.h"
#include "player.h"
#include "options.h"

sprite_set_t *intfaceSpr;

static byte flip=0;

static int curScore;
static int goalProg;
static int realGoalProg;

void InitInterface(void)
{
	intfaceSpr=new sprite_set_t("graphics/intface.jsp");
}

void ExitInterface(void)
{
	delete intfaceSpr;
}

void ResetInterface(void)
{
	curScore=0;
	goalProg=0;
	realGoalProg=0;
}

void RenderIntfaceSprite(int x,int y,int spr,char bright,MGLDraw *mgl)
{
	intfaceSpr->GetSprite(spr)->DrawBright(x,y,mgl,bright);
}

sprite_t *GetIntfaceSprite(int spr)
{
	return intfaceSpr->GetSprite(spr);
}

void UpdateInterface(byte life)
{
	int i;

	flip++;

	if(curScore>player.score)
	{
		if(curScore>player.score+100)
			curScore-=100;
		else if(curScore>player.score+10)
			curScore-=10;
		else
			curScore--;
	}
	else if(curScore<player.score)
	{
		if(curScore<player.score-100)
			curScore+=100;
		else if(curScore<player.score-10)
			curScore+=10;
		else
			curScore++;
	}
	if((flip&3)==0)
	{
		i=GoalProgress(&curMap->goal,curMap)/2;
		if(i>realGoalProg && curMap->goal.type==GOAL_PUSH)
		{
			MakeNormalSound(SND_GOALUP);
		}
		realGoalProg=i;
	}

	if(goalProg<realGoalProg)
		goalProg+=(realGoalProg-goalProg)/16+1;
	else if(goalProg>realGoalProg)
		goalProg-=(goalProg-realGoalProg)/16+1;
}

void RenderInterface(MGLDraw *mgl)
{
	char s[32];
	int i;

	sprintf(s,"%07d",curScore);
	RightPrintOutline(638,2,s,0,0);

	for(i=8;i>=0;i--)
		RenderItmSprite(639,30+i*30,15,mgl);

	for(i=8;i>=0;i--)
	{
		if(player.item[i])
		{
			switch(player.item[i]/8)
			{
				case PE_BOMB:
					InstaRenderItem(635,45+i*30,ITM_BOMB,player.item[i]%8,8,mgl);
					break;
				case PE_KEY:
					InstaRenderItem(635,45+i*30,ITM_KEY,player.item[i]%8,8,mgl);
					break;
				case PE_DETONATE:
					InstaRenderItem(635,45+i*30,ITM_DETONATE,player.item[i]%8,8,mgl);
					break;
				case PE_ACID:
					InstaRenderItem(635,45+i*30,ITM_ACID,player.item[i]%8,8,mgl);
					break;
				case PE_LASER:
					InstaRenderItem(635,45+i*30,ITM_LASER,player.item[i]%8,8,mgl);
					break;
			}

			if(profile.colorblind)
				{
					switch(player.item[i]%8)
					{
						case 1:
							PrintOutline(630,35+i*30,"G",0,1);
							break;
						case 2:
							PrintOutline(630,35+i*30,"O",0,1);
							break;
						case 3:
							PrintOutline(630,35+i*30,"B",0,1);
							break;
						case 4:
							PrintOutline(630,35+i*30,"R",0,1);
							break;
						case 5:
							PrintOutline(630,35+i*30,"Y",0,1);
							break;
						case 6:
							PrintOutline(630,35+i*30,"P",0,1);
							break;
						case 7:
							PrintOutline(630,35+i*30,"A",0,1);
							break;
					}
				}

		}
	}

	// progress toward goal
	DrawBox(632,477-127,639,478,31);
	if(goalProg>0)
		DrawFillBox(633,478-goalProg,638,477,31*1+15);

	// timer
	if(curMap->timer>0)
	{
		i=(curMap->timer)-(player.clock);
		if(i<0)
			i=0;
		sprintf(s,"Time: %2d:%02d",i/60,i%60);
		Print(289,1,s,-32,0);
		Print(291,3,s,-32,0);
		Print(289,3,s,-32,0);
		Print(291,1,s,-32,0);
		if(i>10)
			Print(290,2,s,0,0);
		else
			PrintColor(290,2,s,4,-16,0);
	}
}

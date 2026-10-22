#include "radar.h"
#include "guy.h"
#pragma warning (disable: 4091)
#include <math.h>
#include "skill.h"
#include "map.h"
#include "game.h"

radar_t radar[MAX_RADAR];
static dword radarTick;

void Radar_Clear(void)
{
	int i;

	for(i=0;i<MAX_RADAR;i++)
		radar[i].color=0;
}

void Radar_Init(void)
{
	Radar_Clear();
	radarTick=0;
}

void Radar_Exit(void)
{
}

void Radar_Add(byte team,int x,int y,float dist)
{
	int camX,camY,cx,cy;
	int ang;
	int i,size;

	GetCamera(&camX,&camY);
	cx=camX*FIXAMT;
	cy=camY*FIXAMT;
	camX-=320;
	camY-=240;
	camX*=FIXAMT;
	camY*=FIXAMT;

	if(x>=camX && x<camX+SCRWID*FIXAMT && y>=camY && y<camY+SCRHEI*FIXAMT)
		return;	// this guy is onscreen, no need for radar

	for(i=0;i<MAX_RADAR;i++)
	{
		if(radar[i].color==0)
		{
			ang=AngleFrom(cx,cy,x,y);
			radar[i].x=cx;
			radar[i].y=cy;

			while(radar[i].x>=camX && radar[i].x<camX+SCRWID*FIXAMT && radar[i].y>=camY && radar[i].y<camY+SCRHEI*FIXAMT)
			{
				radar[i].x+=Cosine((int)ang)*4;
				radar[i].y+=Sine((int)ang)*4;
			}
			radar[i].x-=camX;
			radar[i].y-=camY;
			radar[i].x/=FIXAMT;
			radar[i].y/=FIXAMT;

			if(radar[i].x<4)
				radar[i].x=4;
			if(radar[i].y<4)
				radar[i].y=4;
			if(radar[i].x>SCRWID-4)
				radar[i].x=SCRWID-4;
			if(radar[i].y>SCRHEI-4)
				radar[i].y=SCRHEI-4;

			size=20-(int)(dist/200.0f);
			if(size<2)
				size=2;
			if(size>20)
				size=20;
			radar[i].size=size;
			if(team==GOOD)	// good=blue
				radar[i].color=32*3+16;
			else if(team==EVIL)	// evil=red
				radar[i].color=32*4+16;
			else if(team==4)	// crack=brown
				radar[i].color=32*2+16;
			else if(team==5)	// herb=green
				radar[i].color=32*1+16;
			else if(team==6)	// item=yellow
				radar[i].color=32*5+16;
			else if(team==7)	// exit=purple
				radar[i].color=32*6+16;
			return;
		}
	}
}

void Radar_Update(void)
{
	dword time;
	word gNum;
	Guy *g;
	int range,i,cx,cy;
	float dist,dist2;

	radarTick++;

	time=(dword)SpecificSkillVal(1,SKILL_TRACKING);
	if(time==0)
	{
		Radar_Clear();
		return;
	}

	range=(int)SpecificSkillVal(0,SKILL_TRACKING);
	GetCamera(&cx,&cy);
	cx*=FIXAMT;
	cy*=FIXAMT;
	if(radarTick>=time)
	{
		radarTick=0;

		Radar_Clear();
		gNum=GuyList_First();
		while(gNum!=65535)
		{
			g=GetGuy(gNum);
			if(g->type!=MONS_LOONY && (!(monsType[g->type].flags&MF_INVINCIBLE) || g->type==MONS_VILLAGER || g->type==MONS_BOKBOK) && g->type!=MONS_FROSTGATE)	// anyone but the player!
			{
				if(SpecificSkillLevel(SKILL_TRACKING)<3 && g->team==GOOD)
				{
					// don't show friends if lower than level 3
				}
				else
				{
					dist=(float)(g->x-cx)/(float)FIXAMT;
					dist*=dist;
					dist2=(float)(g->y-cy)/(float)FIXAMT;
					dist2*=dist2;
					dist=(float)sqrt(dist+dist2);
					if((int)(dist/(TILE_WIDTH))<range)
						Radar_Add(g->team,g->x,g->y,dist);
				}
			}
			gNum=GuyList_Next();
		}

		cx=(cx/(TILE_WIDTH*FIXAMT));
		cy=(cy/(TILE_HEIGHT*FIXAMT));
		if(SpecificSkillLevel(SKILL_TRACKING)>=4)
		{
			// show items (only ones you can pick up!)
			for(i=0;i<curMap->width*curMap->height;i++)
			{
				if((curMap->map[i].item>=IT_ROPE && curMap->map[i].item<IT_ENERGYBARRIER) || curMap->map[i].item==IT_SCROLL ||
					curMap->map[i].item==IT_CRACKUNDER || curMap->map[i].item==IT_CRACK || curMap->map[i].item==IT_BURIEDASB ||
					curMap->map[i].item==IT_SUPCRACK || curMap->map[i].item==IT_SUPPLIES || curMap->map[i].item==IT_SUPPLIES2 ||
					curMap->map[i].item==IT_GEYSER || curMap->map[i].item==IT_UPGEYSER ||
					(curMap->map[i].item>=IT_CHEST1 && curMap->map[i].item<=IT_CHEST5 && curMap->map[i].itemInfo==0) || curMap->map[i].item==IT_EXIT)
				{
					dist=(float)((i%curMap->width)-cx);
					dist*=dist;
					dist2=(float)((i/curMap->width)-cy);
					dist2*=dist2;
					dist=(float)sqrt(dist+dist2);
					if((int)(dist)<range)
					{
						if((curMap->map[i].item==IT_CRACKUNDER || curMap->map[i].item==IT_SUPCRACK || curMap->map[i].item==IT_CRACK) && SpecificSkillLevel(SKILL_TRACKING)>=5)
							Radar_Add(4,((i%curMap->width)*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,((i/curMap->width)*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,dist*TILE_WIDTH);
						else if(curMap->map[i].item==IT_BURIEDASB || (curMap->map[i].item>=IT_HERBA && curMap->map[i].item<=IT_HERBF))
							Radar_Add(5,((i%curMap->width)*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,((i/curMap->width)*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,dist*TILE_WIDTH);
						else if(SpecificSkillLevel(SKILL_TRACKING)>=7 && (curMap->map[i].item==IT_EXIT || curMap->map[i].item==IT_GEYSER || curMap->map[i].item==IT_UPGEYSER))
							Radar_Add(7,((i%curMap->width)*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,((i/curMap->width)*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,dist*TILE_WIDTH);
						else if(SpecificSkillLevel(SKILL_TRACKING)>=6 && (curMap->map[i].item!=IT_EXIT && curMap->map[i].item!=IT_GEYSER && curMap->map[i].item!=IT_UPGEYSER))
							Radar_Add(6,((i%curMap->width)*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,((i/curMap->width)*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,dist*TILE_WIDTH);
					}
				}
			}
		}
	}
}

void Radar_Render(MGLDraw *mgl)
{
	int i;

	for(i=0;i<MAX_RADAR;i++)
	{
		if(radar[i].color)
		{
			RenderCircleParticle(radar[i].x,radar[i].y,radar[i].size,radar[i].color,mgl->GetScreen());
		}
	}
}

#include "pushing.h"
#include "floor.h"
#include "guy.h"
#include "player.h"
#include "score.h"

char GetDX(int facing)
{
	char dx[]={1,0,-1,0};

	return dx[facing];
}

char GetDY(int facing)
{
	char dy[]={0,1,0,-1};

	return dy[facing];
}

byte CanBePushedTo(int fromX,int fromY,int toX,int toY,Map *map)
{
	char dx[]={1,0,-1,0};
	char dy[]={0,1,0,-1};

	mapTile_t *m,*m2;
	byte b;

	// can't push out of bounds
	if(toX<0 || toY<0 || toX>=map->width || toY>=map->height)
		return 0;

	m=&map->map[fromX+fromY*map->width];
	m2=&map->map[toX+toY*map->width];

	if(PushSwitch(map,m2))
		return 0;
	if(TurnCrank(map,m2))
		return 0;

	// can only push barrels or balls
	if(m->item!=ITM_BARREL && m->item!=ITM_BALL)
		return 0;

	// can't push onto another item ever
	if(m2->item)
		return 0;

	// can't push on solid floor
	if(FloorIsSolid(m2->floor))
		return 0;

	// first make sure you can LEAVE this tile, if it's an arrow
	b=FloorArrowFacing(m->floor);
	if(b!=NOT_AN_ARROW)
	{
		b/=2;	// it returns 0-7 facing format
			
		if(toX-fromX!=dx[b] || toY-fromY!=dy[b])
		{
			ShowArrow(fromX,fromY,b*2);
			return 0;	// can't leave in any direction but the right one
		}
	}

	b=FloorArrowFacing(m2->floor);
	if(b!=NOT_AN_ARROW)
	{
		b/=2;	// it returns 0-7 facing format

		if(fromX-toX==dx[b] && fromY-toY==dy[b])
		{
			ShowArrow(toX,toY,b*2);
			return 0;	// can't move against the arrow
		}
	}

	// can't push on a color-limited floor, unless it's the right color
	// can never push on grey color-limited floor, by the way
	b=FloorOnlyColor(m2->floor);
	if((b!=ANY_COLOR && b!=(m->itemInfo%8)) || b==0)
		return 0;

	return 1;
}

void BlowUpNeighbors(Map *map,int x,int y,byte color)
{
	char dxt[]={1,0,-1,0};
	char dyt[]={0,1,0,-1};

	int tx,ty;
	int i;
	byte c;
	Guy *g;
	mapTile_t *m;

	for(i=0;i<4;i++)
	{
		tx=x+dxt[i];
		ty=y+dyt[i];
		if(tx<0 || ty<0 || tx>=map->width || ty>=map->height)
			continue;	// can't blow up offscreen
		m=&map->map[tx+ty*map->width];

		if(m->item==ITM_BALL || m->item==ITM_CRATE || m->item==ITM_BARREL || m->item==ITM_ICEBERG)
		{
			if((m->itemInfo%8)==0)
				continue;	// grey objects are indestructible

			// things that can be blown up!
			if((m->itemInfo%8)==color || color==0)
			{
				player.blownUp[m->itemInfo%8]++;
				// proper color, let's blow it up!
				g=AddGuy((tx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
					     (ty*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,
						 0,MONS_BOOM);
				// the explosion adopts the item's color
				g->color=m->itemInfo%8;
				Score_BlowUp(SCORE_OBJECT,g->color);
				m->item=ITM_ACTIVEBOMB;
			}
		}
		else if(m->item==ITM_BOMB)
		{
			// notice bombs don't have to be the right color to be blown up!
			g=AddGuy((tx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
					 (ty*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,
					 0,MONS_BOOM);
			// the explosion adopts the item's color
			g->color=m->itemInfo%8;
			Score_BlowUp(SCORE_BOMB,g->color);
			m->item=ITM_ACTIVEBOMB;
		}
		else if(m->item==ITM_PLAYER)
		{
			// uh oh, time to die
			PlayerExplode(map);
		}
		else if(m->item==ITM_MOVINGBALL)
		{
			// find which ball is on this space
			g=FindBall(tx,ty);
			if(g && g->color!=0 && (g->color==color || color==0))
			{
				// and remove it!
				c=g->color;
				player.blownUp[c]++;
				g->type=MONS_NONE;
				g=AddGuy((tx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
					 (ty*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,
					 0,MONS_BOOM);
				Score_BlowUp(SCORE_OBJECT,c);
				g->color=c;
				m->item=ITM_ACTIVEBOMB;
			}
		}
		else if(m->item==ITM_ACTIVEBOMB)
		{
			// find which bomb is on this space
			g=FindBomb(tx,ty);
			// it will return null if there's an explosion here instead, which is good
			if(g)
			{
				// set the bomb off!
				c=g->color;
				g->type=MONS_NONE;
				g=AddGuy((tx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
					 (ty*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,
					 0,MONS_BOOM);
				Score_BlowUp(SCORE_BOMB,c);
				g->color=c;
				m->item=ITM_ACTIVEBOMB;
			}
		}
		else if(m->item>=ITM_MONS1 && m->item<=ITM_MONS4)
		{
			// find which monster is on this space
			if(c=MonsterExplode(tx,ty,map,0))
			{
				g=AddGuy((tx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
					 (ty*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,
					 0,MONS_BOOM);
				g->color=c;
				m->item=ITM_ACTIVEBOMB;
			}
		}
		else if(m->item==ITM_ACID)
		{
			if((m->itemInfo%8)==0)
				continue;	// grey objects are indestructible
			BlowSmoke(tx,ty,m->itemInfo%8);
			AcidSplash(map,m->itemInfo%8,tx-1,ty-1);
			AcidSplash(map,m->itemInfo%8,tx,ty-1);
			AcidSplash(map,m->itemInfo%8,tx+1,ty-1);
			AcidSplash(map,m->itemInfo%8,tx-1,ty+1);
			AcidSplash(map,m->itemInfo%8,tx,ty+1);
			AcidSplash(map,m->itemInfo%8,tx+1,ty+1);
			AcidSplash(map,m->itemInfo%8,tx-1,ty);
			AcidSplash(map,m->itemInfo%8,tx+1,ty);
			MakeNormalSound(SND_ACID);
			m->item=ITM_NONE;
		}
		else if(m->item==ITM_LASER)
		{
			if((m->itemInfo%8)==0)
				continue;	// grey objects are indestructible
			g=AddGuy((tx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
					 (ty*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,
					 0,MONS_BOOM);
				// the explosion adopts the item's color
			g->color=m->itemInfo%8;
			Score_BlowUp(SCORE_BOMB,g->color);
			m->item=ITM_ACTIVEBOMB;
		}
		else if(m->item==ITM_DETONATE)
		{
			if((m->itemInfo%8)==0)
				continue;	// grey objects are indestructible
			// set it off
			DetonateAllBombs(m->itemInfo%8,map);
			MakeNormalSound(SND_DETONATOR);
			g=AddGuy((tx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
					 (ty*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,
					 0,MONS_BOOM);
			// the explosion adopts the item's color
			g->color=m->itemInfo%8;
			Score_BlowUp(SCORE_BOMB,g->color);
			m->item=ITM_ACTIVEBOMB;
		}
		else if(m->item==ITM_PENCIL)
		{
			if((m->itemInfo%8)==0)
				continue;	// grey objects are indestructible

			// things that can be blown up!
			if((m->itemInfo%8)==color || color==0)
			{
				player.deadPencils[m->itemInfo%8]++;
				BlowSmoke(tx,ty,m->itemInfo%8);
				m->item=ITM_NONE;
			}
		}
		else if(m->item==ITM_KEY)
		{
			if((m->itemInfo%8)==0)
				continue;	// grey objects are indestructible

			// things that can be blown up!
			if((m->itemInfo%8)==color || color==0)
			{
				BlowSmoke(tx,ty,m->itemInfo%8);
				m->item=ITM_NONE;
			}
		}
	}
}

byte PushSwitch(Map *map,mapTile_t *me)
{
	if(me->item==ITM_SWITCH || me->item==ITM_SWITCH2)
	{
		if((me->itemInfo/8)==0)
		{
			if(map->ToggleDoors(me->itemInfo%8))
			{
				// only flip it if the doors are unblocked
				me->item=ITM_SWITCH+ITM_SWITCH2-me->item;
				me->itemInfo+=8*8;
				MakeNormalSound(SND_SWITCHFLIP);
			}
		}
		return 1;	// can't walk on the switch!
	}
	else
		return 0;
}

byte TurnCrank(Map *map,mapTile_t *me)
{
	if((me->item==ITM_CRANK))
	{
		if((me->itemInfo/8)==0)
		{
			MakeNormalSound(SND_CRANKTURN);
			if(map->TurnArrows())
			{
				// only flip it if the doors are unblocked
				me->itemInfo+=8*8;
			}
		}
		return 1;	// can't walk on the crank
	}
	else
		return 0;
}

byte PushObject(int x,int y,Guy *me,mapTile_t *m,Map *curMap)
{
	int destx,desty;
	mapTile_t *m2;

	destx=x+GetDX(me->facing/2);
	desty=y+GetDY(me->facing/2);

	if(CanBePushedTo(x,y,destx,desty,curMap))
	{
		curMap->GoodieCheck(destx,desty,GOODIE_PUSH);
		if(m->item==ITM_BALL)
			LaunchBall(x,y,GetDX(me->facing/2),GetDY(me->facing/2));
		else
		{
			m2=&curMap->map[destx+desty*curMap->width];
			m2->item=m->item;
			m2->itemInfo=m->itemInfo;
			m->item=ITM_NONE;
			if(FloorIsHole(m2->floor))
			{
				player.dumpedObj[m2->itemInfo%8]++;
				m2->item=ITM_NONE;
				m2->floor+=20;
				SmokePuff((destx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
						  (desty*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,2);

				// ITEM FALL IN HOLE!!!
				MakeSound(SND_FALLINHOLE,(destx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
						  (desty*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,SND_CUTOFF,100);
			}
			if(FloorIsPit(m2->floor))
			{
				player.dumpedObj[m2->itemInfo%8]++;
				m2->item=ITM_NONE;
				SmokePuff((destx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
						  (desty*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,2);
				// ITEM FALL IN PIT!!!
				MakeSound(SND_FALLINPIT,(destx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
						  (desty*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,SND_CUTOFF,100);
				
			}
			if(FloorIsTeleporter(m2->floor))
			{
				int telex,teley;

				if(curMap->SelectTeleporter(destx,desty,&telex,&teley))
				{
					LightningBolt(me->x,me->y,(telex*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
						(teley*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT);
					LightningBolt(me->x,me->y,(telex*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
						(teley*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT);
					LightningBolt(me->x,me->y,(telex*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
						(teley*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT);
					curMap->map[telex+teley*curMap->width].item=m2->item;
					curMap->map[telex+teley*curMap->width].itemInfo=m2->itemInfo;
					m2->item=ITM_NONE;
					// add teleport FX
					MakeSound(SND_TELEPORT,(telex*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
						  (teley*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,SND_CUTOFF,100);
					curMap->GoodieCheck(telex,teley,GOODIE_PUSH);
				}
				else
				{
					// failed tport FX
					MakeSound(SND_NOTELEPORT,(destx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
						  (desty*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,SND_CUTOFF,100);
				}
			}
		}
		
		return 1;
	}
	else
		return 0;
}

void LaunchBall(int x,int y,int dx,int dy)
{
	mapTile_t *m,*m2;
	Guy *g;

	m=&curMap->map[x+y*curMap->width];
	m2=&curMap->map[(x+dx)+(y+dy)*curMap->width];

	g=AddGuy((x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,0,MONS_BALL);
	if(g)
	{
		g->dx=dx*FIXAMT*8;
		g->dy=dy*FIXAMT*6;
		if(dx>0)
		{
			g->facing=0;
			g->seq=ANIM_IDLE;
		}
		else if(dx<0)
		{
			g->facing=4;
			g->seq=ANIM_ATTACK;
		}
		else if(dy<0)
		{
			g->facing=6;
			g->seq=ANIM_A1;
		}
		else
		{
			g->facing=2;
			g->seq=ANIM_MOVE;
		}
		g->color=m->itemInfo%8;
		g->tx=((g->mapx+dx)*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
		g->ty=((g->mapy+dy)*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
	}
	m->item=ITM_NONE;
	m2->item=ITM_MOVINGBALL;
}

void DetonateBomb(int x,int y,Map *map)
{
	Guy *g;

	g=FindBomb(x,y);
	if(g && g->type==MONS_BOOM)
		return;	//can't detonate an explosion

	if(g)
	{
		g->type=MONS_BOOM;
		g->seq=ANIM_IDLE;
		g->frm=0;
		g->frmAdvance=256;
		g->frmTimer=0;
		g->mind=0;
	}
	else
	{
		g=AddGuy((x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,0,MONS_BOOM);
	}
	map->map[x+y*map->width].item=ITM_ACTIVEBOMB;
	Score_BlowUp(SCORE_BOMB,map->map[x+y*map->width].itemInfo%8);
}

byte DetonateAllBombs(byte color,Map *map)
{
	Guy *g;
	int i,x,y;
	byte areBombs;

	x=0;
	y=0;
	areBombs=0;
	for(i=0;i<map->width*map->height;i++)
	{
		if((map->map[i].item==ITM_BOMB || map->map[i].item==ITM_ACTIVEBOMB) && 
			((map->map[i].itemInfo%8)==color || color==0))
		{
			g=FindBomb(x,y);
			if(!g && map->map[i].item==ITM_ACTIVEBOMB)
				continue;	// if FindBomb couldn't find an active bomb, that means it's a boom
			areBombs=1;
			if(g)
			{
				g->type=MONS_BOOM;
				g->seq=ANIM_IDLE;
				g->frm=0;
				g->frmAdvance=256;
				g->frmTimer=0;
				g->mind=0;
			}
			else
			{
				g=AddGuy((x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,0,MONS_BOOM);
				g->color=map->map[i].itemInfo%8;
			}
			map->map[x+y*map->width].item=ITM_NONE;	// remove the item
			Score_BlowUp(SCORE_BOMB,map->map[x+y*map->width].itemInfo%8);
		}
		x++;
		if(x==map->width)
		{
			x=0;
			y++;
		}
	}
	return areBombs;
}

void FireLaser(int x,int y,byte facing,byte color,Map *map)
{
	char dxTab[]={1,0,-1,0};
	char dyTab[]={0,1,0,-1};
	char dx,dy;
	mapTile_t *m;
	byte b,preFace;
	byte length;
	Guy *g;

	dx=dxTab[facing/2];
	dy=dyTab[facing/2];

	length=0;

	MakeNormalSound(SND_FIRELASER);
	while(1)
	{
		if(x>=0 && x<map->width && y>=0 && y<map->width)
		{
			preFace=facing;

			m=&map->map[x+y*map->width];
			if(FloorIsSolid(m->floor))
				break;	// laser is done
			if(m->item==ITM_DOOR)
			{
				break;	// doors stop lasers
			}
			if(m->item==ITM_PLAYER)
			{
				PlayerExplode(map);
				if(player.invinc)	// being invincible blocks the beam
					break;
			}
			if(m->item==ITM_MONS1 || m->item==ITM_MONS2 || m->item==ITM_MONS3 || m->item==ITM_MONS4)
			{
				if(!MonsterExplode(x,y,map,0))
					break;	// grey monsters stop the laser
				else
					BlowSmoke(x,y,color);
			}
			if(m->item==ITM_ACTIVEBOMB || m->item==ITM_BOMB)
			{
				DetonateBomb(x,y,map);
			}
			if(m->item==ITM_LASER)
			{
				BlowSmoke(x,y,color);
				if((m->itemInfo%8)!=0)
					m->item=ITM_NONE;	// don't destroy grey ones
				color=m->itemInfo%8;
			}
			if(m->item==ITM_BALL || m->item==ITM_BARREL || m->item==ITM_CRATE || m->item==ITM_ICEBERG)
			{
				if(((m->itemInfo%8)==color || color==0) && (m->itemInfo%8)!=0)
				{
					player.blownUp[m->itemInfo%8]++;
					Score_BlowUp(SCORE_OBJECT,m->itemInfo%8);
					m->item=ITM_NONE;
					BlowSmoke(x,y,color);
				}
				else
					break;
			}
			if(m->item==ITM_PENCIL)
			{
				if(((m->itemInfo%8)==color || color==0) && (m->itemInfo%8)!=0)
				{
					player.deadPencils[m->itemInfo%8]++;
					m->item=ITM_NONE;
					BlowSmoke(x,y,color);
				}
				else
					break;
			}
			if(m->item==ITM_KEY)
			{
				if(((m->itemInfo%8)==color || color==0) && (m->itemInfo%8)!=0)
				{
					m->item=ITM_NONE;
					BlowSmoke(x,y,color);
				}
				else
					break;
			}
			if(m->item==ITM_MOVINGBALL)
			{
				// find which ball is on this space
				g=FindBall(x,y);
				if(g && g->color!=0 && (g->color==color || color==0))
				{
					// and remove it!
					b=g->color;
					player.blownUp[b]++;
					g->type=MONS_NONE;
					Score_BlowUp(SCORE_OBJECT,b);
					m->item=ITM_NONE;
					BlowSmoke(x,y,color);
				}
				else if(g && g->color==0)
				{
					break;	// stop the laser if it's grey
				}
			}
			if(m->item==ITM_ACID)
			{
				if((m->itemInfo%8)==0)
					break;	// stop the laser if grey
				BlowSmoke(x,y,color);
				AcidSplash(map,m->itemInfo%8,x-1,y-1);
				AcidSplash(map,m->itemInfo%8,x,y-1);
				AcidSplash(map,m->itemInfo%8,x+1,y-1);
				AcidSplash(map,m->itemInfo%8,x-1,y+1);
				AcidSplash(map,m->itemInfo%8,x,y+1);
				AcidSplash(map,m->itemInfo%8,x+1,y+1);
				AcidSplash(map,m->itemInfo%8,x-1,y);
				AcidSplash(map,m->itemInfo%8,x+1,y);
				MakeNormalSound(SND_ACID);
				m->item=ITM_NONE;
			}
			if(m->item==ITM_DETONATE)
			{
				if((m->itemInfo%8)==0)
					break;	// stop the laser if grey
				BlowSmoke(x,y,color);
				m->item=ITM_NONE;
			}
			if(m->item==ITM_CRANK)
			{
				TurnCrank(map,m);
				break;	// the laser stops at the crank
			}
			if(m->item==ITM_SWITCH || m->item==ITM_SWITCH2)
			{
				PushSwitch(map,m);
				break;	// the laser stops at the switch
			}
			b=FloorArrowFacing(m->floor);
			if(b!=NOT_AN_ARROW)
			{
				facing=b;
				dx=dxTab[facing/2];
				dy=dyTab[facing/2];
			}
			Laser(x,y,preFace,facing,color);
			x+=dx;
			y+=dy;
		}
		else
			break;
		length++;
		if(length>100)
			break;
	}
}
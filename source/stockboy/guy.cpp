#include "guy.h"
#include "player.h"
#include "intface.h"
#include "editor.h"
#include "options.h"
#include "floor.h"
#include "pushing.h"
#include "score.h"
#include "profile.h"

Guy **guys;
Guy *goodguy;
int maxGuys;
static byte checkActive=0;

//------------------------------------------------------------------------
// CLASS GUY

Guy::Guy(void)
{
	type=MONS_NONE;
}

Guy::~Guy(void)
{
}

// checks to see if a rect of size, located at (xx,yy) hits HIM
byte Guy::AttackCheck(byte size,int xx,int yy,Guy *him)
{
	return 0;
}

void Guy::SeqFinished(void)
{
	byte t;

	if(type==MONS_BALL)
	{
		// just loop the animation
		frm=0;
		frmAdvance=128;
		return;
	}
	if(seq==ANIM_DIE)
	{
		if(type==player.monsType)
		{
			// restart current level
			SendMessageToGame(MSG_RESET,0);
		}

		t=type;
		type=MONS_NONE;	// all gone
	}
	seq=ANIM_IDLE;
	frm=0;
	frmAdvance=128;
	action=ACTION_IDLE;
}

void Guy::AttackThem(void)
{
}

void Guy::NextFrame(void)
{
	frm++;

	if(MonsterAnim(type,seq)[frm]==255)
		// this sequence is done
		SeqFinished();

	if(type==MONS_NONE)
		return;	// seqfinished may have killed the guy
}

byte Guy::IsActive(Map *map)
{
	return 1;
}

void Guy::Update(Map *map,world_t *world)
{
	int oldmapx,oldmapy;
	byte b;
	static byte burnFlip=0;
	int sx,sy;

	oldmapx=mapx;
	oldmapy=mapy;

	if(type==player.monsType)	// special case, player controls Loony
	{
		PlayerControlMe(this,map,&map->map[mapx+mapy*map->width],world);
	}
	else
	{
		MonsterControl(map,world);
	}

	if(ouch>0)
		ouch--;

	oldx=x;
	oldy=y;

	x+=dx;
	y+=dy;

	if((MonsterFlags(type)&MF_FLYING) && seq!=ANIM_DIE)
	{
		if(z<10*FIXAMT)	// go up if you need to
			dz+=FIXAMT;
		else
			dz-=FIXAMT;

		Clamp(&dz,FIXAMT*2);
		z+=dz;

		if(z<0)
			z=0;
	}
	else
	{
		z+=dz;
		if(z<0)
		{
			z=0;
			dz=0;
		}
		else
		{
			if(!(MonsterFlags(type)&MF_NOGRAV))
				dz-=FIXAMT*2;
		}
	}

	frmTimer+=frmAdvance;
	while(frmTimer>255)
	{
		frmTimer-=255;
		NextFrame();
	}
	if(type==MONS_NONE)
		return;	// NextFrame may have killed you

	mapx=(x>>FIXSHIFT)/TILE_WIDTH;
	mapy=(y>>FIXSHIFT)/TILE_HEIGHT;

	if(type==player.monsType)	// special case, Loony is the player, follow him
	{
		UpdateCamera(x>>FIXSHIFT,y>>FIXSHIFT,facing*32,map);
		b=GetPlayerGlow();
		if(b>0)
		{
			map->BrightTorch(mapx,mapy,b,b/4);
			if(b>4)
				b-=4;
			else
				b--;
			SetPlayerGlow(b);
		}
	}

	if(type>=MONS_BLOBBY && type<=MONS_PIGGY)
	{
		if(mind3)
			mind3--;

		if(mind2==1 && !mind3 && color==7)	// aqua guys shoot
		{
			bullet_t *bul;

			mind2=0;
			mind3=30;
			if(abs(x-goodguy->x)>abs(y-goodguy->y))
			{
				if(x>goodguy->x)
					b=4;
				else
					b=0;
			}
			else
			{
				if(y>goodguy->y)
					b=6;
				else
					b=2;
			}
			sx=x+Cosine(b*32)*TILE_WIDTH;
			sy=y+Sine(b*32)*TILE_HEIGHT;
			bul=FireBullet(sx,sy,b,BLT_AQUA);
			if(bul)
			{
				MakeSound(SND_MONS_SHOOT,x,y,SND_CUTOFF,30);
				bul->owner=this;
			}
		}
	}
}

// this is a very half-assed version of update to be called by the editor instead of the game
void Guy::EditorUpdate(Map *map)
{
	int mapx,mapy;

	mapx=(x>>FIXSHIFT)/TILE_WIDTH;
	mapy=(y>>FIXSHIFT)/TILE_HEIGHT;
	bright=map->map[mapx+mapy*map->width].templight;
}

void Guy::Render(byte light)
{
	// table of how bright to make the creature at each color, for nicer looks
	byte brtTable[]={3,0,2,1,0,1,0,1,	// blobby
					 6,0,3,1,0,1,0,2,	// bunny
					 3,0,3,1,0,1,0,1,	// inchy
					 4,0,5,3,1,0,0,1,	// piggy
	};
	byte b;

	if(type>=MONS_BLOBBY && type<=MONS_PIGGY)
	{
		b=brtTable[(type-MONS_BLOBBY)*8+color];
		MonsterDraw(x,y,z,type,seq,frm,facing,color,bright*(light>0)+b,ouch,ice);
	}
	else if(type==MONS_LOONY)
	{
		MonsterDraw(x,y,z,type,seq,frm,facing,color,bright*(light>0)+profile.bright,ouch,ice);
	}
	else
		MonsterDraw(x,y,z,type,seq,frm,facing,color,bright*(light>0),ouch,ice);
}

void Guy::MonsterControl(Map *map,world_t *world)
{
	if(!goodguy)
		return;	// badguys don't move if you're gone, saves a lot of crash possibilities

	switch(type)
	{
		case MONS_BALL:
			AI_Ball(this,map,world);
			break;
		case MONS_BOMB:
			AI_Bomb(this,map,world);
			break;
		case MONS_BOOM:
			AI_Boom(this,map,world);
			break;
		case MONS_BLOBBY:
			AI_Blobby(this,map,world);
			break;
		case MONS_BUNNY:
			AI_Bunny(this,map,world);
			break;
		case MONS_INCHY:
			AI_Inchy(this,map,world);
			break;
		case MONS_PIGGY:
			AI_Piggy(this,map,world);
			break;
	}
}

void Guy::GetShot(int dx,int dy,byte damage,Map *map,world_t *world)
{
	int formerHP;
	dword combo;

	if(hp==0)
		return;	// can't shoot a dead guy

	if(type==0)
		return;	// shouldn't have a type 0 guy at all

	if((MonsterFlags(type)&MF_INVINCIBLE))
	{
		return;	// invincible
	}

	formerHP=hp;
	if(!ice)
		ouch=4;
	hp-=damage;

	if(ice)
	{
		ice/=2;
		if(hp>0)
		{
			ExplodeParticles2(PART_SNOW2,x,y,z,16,10);
		}
		else
			ice=0;
	}

	if(hp<=0)
	{
		if(type!=player.monsType)
		{
			if(player.comboClock>0)
			{
				player.combo++;
				if(player.combo>player.bestCombo)
					player.bestCombo=player.combo;
			}
			else
			{
				player.combo=1;
			}
			player.comboClock=COMBO_TIME;
		}

		hp=0;
		seq=ANIM_DIE;
		this->dx=0;
		this->dy=0;
		frm=0;
		frmAdvance=64;
		frmTimer=0;
		action=ACTION_BUSY;

		if(type!=player.monsType)
		{
			combo=player.combo;
			if(combo>20)
				combo=20;
		}
	}
	if(type!=player.monsType)
	{
	}
	else
	{
		player.invinc=30;
	}
}

//-----------------------------------------------------------------------

void InitGuys(int max)
{
	int i;

	maxGuys=512;

	guys=(Guy **)malloc(sizeof(Guy *)*maxGuys);
	for(i=0;i<maxGuys;i++)
		guys[i]=new Guy();
	goodguy=NULL;
}

void ExitGuys(void)
{
	int i;

	if(guys)
	{
		for(i=0;i<maxGuys;i++)
		{
			if(guys[i])
				delete guys[i];
			guys[i]=NULL;
		}
		free(guys);
		guys=NULL;
	}
}

void UpdateGuys(Map *map,world_t *world)
{
	int i;
	static byte filmflip;

	UpdateMonsterTypes();

	if(player.comboClock)
		player.comboClock--;

	checkActive++;
	if(checkActive>5)
	{
		checkActive=0;
	}

	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type!=MONS_NONE && guys[i]->IsActive(map))
		{
			guys[i]->bright=map->map[guys[i]->mapx+guys[i]->mapy*map->width].templight;
			if(guys[i]->ice)
			{
				guys[i]->ice--;
				if(guys[i]->ice>160)
					continue;	// no moving when totally frozen

				if(guys[i]->iceClock)
				{
					guys[i]->iceClock--;
					continue;
				}
				else
				{
					guys[i]->iceClock=guys[i]->ice/16;
				}

			}

			guys[i]->Update(map,world);
		}
	}
}

void EditorUpdateGuys(Map *map)
{
	int i;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type!=MONS_NONE)
			guys[i]->EditorUpdate(map);
}

void PrepGuys(Map *map)
{
	int i;

	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type==player.monsType)
			UpdateCamera(guys[i]->x>>FIXSHIFT,guys[i]->y>>FIXSHIFT,guys[i]->facing*32,map);
		if(guys[i]->type!=MONS_NONE)
		{
			guys[i]->mapx=(guys[i]->x>>FIXSHIFT)/TILE_WIDTH;
			guys[i]->mapy=(guys[i]->y>>FIXSHIFT)/TILE_HEIGHT;
			guys[i]->bright=map->map[guys[i]->mapx+guys[i]->mapy*map->width].templight;
		}
	}
}

void RenderGuys(byte light)
{
	int i;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type!=MONS_NONE)
			guys[i]->Render(light);
}

Guy *AddGuy(int x,int y,int z,byte type)
{
	int i;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type==MONS_NONE)
		{
			guys[i]->type=type;
			guys[i]->x=x;
			guys[i]->y=y;
			guys[i]->z=z;
			guys[i]->tx=x;
			guys[i]->ty=y;
			guys[i]->seq=ANIM_IDLE;
			guys[i]->frm=0;
			guys[i]->frmTimer=0;
			guys[i]->frmAdvance=128;
			guys[i]->hp=1;
			guys[i]->facing=2;
			guys[i]->ouch=0;
			guys[i]->dx=0;
			guys[i]->dy=0;
			guys[i]->dz=0;
			guys[i]->bright=0;
			guys[i]->mind=0;
			guys[i]->mind1=0;
			guys[i]->mind2=0;
			guys[i]->mind3=30;
			guys[i]->reload=0;
			guys[i]->parent=NULL;
			guys[i]->ID=i;
			guys[i]->mapx=(guys[i]->x>>FIXSHIFT)/TILE_WIDTH;
			guys[i]->mapy=(guys[i]->y>>FIXSHIFT)/TILE_HEIGHT;
			guys[i]->tgtx=guys[i]->mapx;
			guys[i]->tgty=guys[i]->mapy;
			guys[i]->oldmapx=guys[i]->mapx;
			guys[i]->oldmapy=guys[i]->mapy;
			guys[i]->ice=0;
			guys[i]->iceClock=0;
			guys[i]->active=1;
			guys[i]->tag=0;

			if(guys[i]->type==MONS_BOOM)
				guys[i]->frm=(byte)Random(4);
			guys[i]->color=MonsterColor(type);
			return guys[i];
		}
	return NULL;
}

void CameraOnPlayer(void)
{
	int i;
	Guy *g;

	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type==player.monsType)	// special case: loony is the goodguy
		{
			goodguy=guys[i];
			return;
		}
	}
	// if you got here, there was no loony
	g=AddGuy((0*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(0*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,
		0,player.monsType);
	curMap->map[0].item=ITM_START;
	goodguy=g;
}

void DeleteGuy2(Guy *g)
{
	int i;

	g->type=MONS_NONE;
	for(i=0;i<maxGuys;i++)
		if(guys[i] && guys[i]->parent==g)
		{
			DeleteGuy2(guys[i]);
		}
}

void DeleteGuy(int x,int y,byte type)
{
	int i,j;

	for(i=0;i<maxGuys;i++)
		if((guys[i]->type==type) && (guys[i]->x==x) && (guys[i]->y==y))
		{
			guys[i]->type=MONS_NONE;
			for(j=0;j<maxGuys;j++)
				if(guys[j]->parent==guys[i])	// kill all the kids too
					DeleteGuy2(guys[j]);
		}
}

void AddMapGuys(Map *map)
{
	int i,j;
	mapTile_t *m;
	Guy *g;

	for(i=0;i<map->width;i++)
		for(j=0;j<map->height;j++)
		{
			m=&map->map[i+j*map->width];

			if(m->item==ITM_START || m->item==ITM_PLAYER)
			{
				g=AddGuy((i*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
					   (j*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,0,MONS_LOONY);
				if(g)
				{
					g->color=profile.color;
					g->bright=profile.bright;
				}

				m->item=ITM_PLAYER;

			}
			if(m->item==ITM_MONS1)
			{
				g=AddGuy((i*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
					   (j*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,0,MONS_BLOBBY);
				if(g)
				{
					g->facing=(m->itemInfo/8)*2;
					g->color=m->itemInfo%8;
				}
			}
			if(m->item==ITM_MONS2)
			{
				g=AddGuy((i*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
					   (j*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,0,MONS_BUNNY);
				if(g)
				{
					g->facing=(m->itemInfo/8)*2;
					g->color=m->itemInfo%8;
				}
			}
			if(m->item==ITM_MONS3)
			{
				g=AddGuy((i*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
					   (j*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,0,MONS_INCHY);
				if(g)
				{
					g->facing=(m->itemInfo/8)*2;
					g->color=m->itemInfo%8;
				}
			}
			if(m->item==ITM_MONS4)
			{
				g=AddGuy((i*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
					   (j*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,0,MONS_PIGGY);
				if(g)
				{
					g->facing=(m->itemInfo/8)*2;
					g->color=m->itemInfo%8;
				}
			}
		}
}

byte CheckHit(byte size,int xx,int yy,Guy *him)
{
	return 0;
}

byte FindVictim(int x,int y,byte size,int dx,int dy,byte damage,Map *map,world_t *world)
{
	int i;


	for(i=0;i<maxGuys;i++)
		if(guys[i]->type && guys[i]->hp && (guys[i]->type!=player.monsType))
		{
			if(CheckHit(size,x,y,guys[i]))
			{
				guys[i]->GetShot(dx,dy,damage,map,world);
				return 1;
			}
		}

	return 0;
}

int FreezeVictim(int x,int y,byte size,int dx,int dy,int target,byte damage,Map *map,world_t *world)
{
	int i;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type && guys[i]->hp && (guys[i]->type!=player.monsType) && guys[i]->ID!=target-1)
		{
			if(CheckHit(size,x,y,guys[i]))
			{
				return guys[i]->ID+1;
			}
		}

	return 0;
}

byte FindNewVictim(int x,int y,int *target,byte size,int dx,int dy,byte damage,Map *map,world_t *world)
{
	int i;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type && guys[i]->hp && (guys[i]->type!=player.monsType) && i!=*target)
		{
			if(CheckHit(size,x,y,guys[i]))
			{
				guys[i]->GetShot(dx,dy,damage,map,world);
				*target=i;
				return 1;
			}
		}

	return 0;
}

byte FindGoodVictim(int x,int y,byte size,int dx,int dy,byte damage,Map *map,world_t *world)
{
	if(!goodguy || (!goodguy->hp))
		return 0;
	if(player.invinc)
		return 0;

	if(CheckHit(size,x,y,goodguy))
	{
		goodguy->GetShot(dx,dy,damage,map,world);
		return 1;
	}

	return 0;
}

// this doesn't quit when it finds one victim, it keeps going
byte FindVictims(int x,int y,byte size,int dx,int dy,byte damage,Map *map,world_t *world)
{
	int i;
	byte result=0;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type && guys[i]->hp && (guys[i]->type!=player.monsType))
		{
			if(CheckHit(size,x,y,guys[i]))
			{
				guys[i]->GetShot(dx,dy,damage,map,world);
				result=1;
			}
		}

	return result;
}

// Same as above, but won't hit someone who is currently in ouch mode (to avoid rapid rehits)
byte FindVictims2(int x,int y,byte size,int dx,int dy,byte damage,Map *map,world_t *world)
{
	int i;
	byte result=0;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type && guys[i]->hp && (guys[i]->type!=player.monsType) && guys[i]->ouch==0)
		{
			if(CheckHit(size,x,y,guys[i]))
			{
				guys[i]->GetShot(dx,dy,damage,map,world);
				result=1;
			}
		}

	return result;
}

word LockOnEvil(int x,int y)
{
	int i;
	int bestRange,range;
	word bestguy;
	int maxRange;

	maxRange=(320+240)-100;
	bestguy=65535;
	bestRange=maxRange;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type && guys[i]->hp && guys[i]->active && (guys[i]->type!=player.monsType) &&
			(!(MonsterFlags(guys[i]->type)&(MF_NOHIT|MF_INVINCIBLE))))
		{
			range=abs(x-(guys[i]->x>>FIXSHIFT))+abs(y-(guys[i]->y>>FIXSHIFT));
			if((range<bestRange) || (range<maxRange && Random(32)==0))
			{
				bestguy=i;
				bestRange=range;
			}
		}

	return bestguy;
}

word LockOnEvil2(int x,int y)
{
	int i,j;
	int range;

	for(j=0;j<128;j++)
	{
		i=Random(maxGuys);
		if(guys[i]->type && guys[i]->hp && (guys[i]->type!=player.monsType) && guys[i]->active &&
			(!(MonsterFlags(guys[i]->type)&MF_NOHIT)))
		{
			range=abs(x-(guys[i]->x>>FIXSHIFT))+abs(y-(guys[i]->y>>FIXSHIFT));
			if(range<160+120)
			{
				return i;
			}
		}
	}
	return 65535;
}

word LockOnEvil3(int x,int y,int maxRange)
{
	int i;
	int bestRange,range;
	word bestguy;

	bestguy=65535;
	bestRange=maxRange;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type && guys[i]->hp && guys[i]->active && (guys[i]->type!=player.monsType) &&
			(!(MonsterFlags(guys[i]->type)&(MF_NOHIT|MF_INVINCIBLE))))
		{
			range=abs(x-(guys[i]->x>>FIXSHIFT))+abs(y-(guys[i]->y>>FIXSHIFT));
			if((range<bestRange) || (range<maxRange && Random(32)==0))
			{
				bestguy=i;
				bestRange=range;
			}
		}

	return bestguy;
}

byte GetGuyPos(word guy,int *x,int *y)
{
	if(guy==65535)
		return 0;
	if(guys[guy]==NULL)
		return 0;

	*x=guys[guy]->x;
	*y=guys[guy]->y;

	return 1;
}

byte MonsterExists(byte type)
{
	int i;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type==type)
			return 1;

	return 0;
}

byte AnyMonsterExists(void)
{
	int i;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type!=player.monsType)
			return 1;

	return 0;
}

byte TaggedMonsterExists(byte tag)
{
	int i;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type!=MONS_NONE && guys[i]->tag==tag)
			return 1;

	return 0;
}

Guy *GetGuy(word w)
{
	return guys[w];
}

void KillKids(Guy *g)
{
	int i;

	for(i=0;i<maxGuys;i++)
		if(guys[i] && guys[i]->parent==g)
		{
			KillKids(guys[i]);	// kill my children too
			if(guys[i]->hp>0)
			{
				guys[i]->hp=0;
				guys[i]->seq=ANIM_DIE;
				guys[i]->action=ACTION_BUSY;
				guys[i]->frm=0;
				guys[i]->frmTimer=0;
				guys[i]->frmAdvance=64;
			}
		}
}

void GuySwap(int sx,int sy,int width,int height,int dx,int dy)
{
	int i;

	sx*=(TILE_WIDTH*FIXAMT);
	sy*=(TILE_HEIGHT*FIXAMT);
	dx*=(TILE_WIDTH*FIXAMT);
	dy*=(TILE_HEIGHT*FIXAMT);
	width*=(TILE_WIDTH*FIXAMT);
	height*=(TILE_HEIGHT*FIXAMT);

	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->x>=sx && guys[i]->y>=sy && guys[i]->x<=(sx+width) && guys[i]->y<=(sy+height))
		{
			// in target area, swap
			guys[i]->x+=(-sx+dx);
			guys[i]->y+=(-sy+dy);
		}
		else if(guys[i]->x>=dx && guys[i]->y>=dy && guys[i]->x<=(dx+width) && guys[i]->y<=(dy+height))
		{
			// in other target area, swap
			guys[i]->x+=(-dx+sx);
			guys[i]->y+=(-dy+sy);
		}
	}
}

void KillAllMonsters(byte type)
{
	int i;

	for(i=0;i<maxGuys;i++)
		if(guys[i] && guys[i]->type==type && guys[i]->hp>0 && guys[i]->seq!=ANIM_DIE)
		{
			guys[i]->hp=0;
			guys[i]->seq=ANIM_DIE;
			guys[i]->dx=0;
			guys[i]->dy=0;
			guys[i]->frm=0;
			guys[i]->frmAdvance=64;
			guys[i]->frmTimer=0;
			guys[i]->ouch=4;
			guys[i]->action=ACTION_BUSY;
		}
}

void DeleteMonsters(void)
{
	int i;

	for(i=0;i<maxGuys;i++)
		if(guys[i])
		{
			guys[i]->type=MONS_NONE;
		}
}

byte NotFriendly(byte type)
{
	return (type!=player.monsType);
}

Guy *FindBall(int x,int y)
{
	int i;
	int tx,ty;

	for(i=0;i<maxGuys;i++)
	{
		if(guys[i] && guys[i]->type==MONS_BALL)
		{
			tx=guys[i]->tx/(TILE_WIDTH*FIXAMT);
			ty=guys[i]->ty/(TILE_HEIGHT*FIXAMT);
			if(tx==x && ty==y)
				return guys[i];
		}
	}

	return NULL;
}

Guy *FindBomb(int x,int y)
{
	int i;

	for(i=0;i<maxGuys;i++)
		if(guys[i] && guys[i]->type==MONS_BOMB && guys[i]->mapx==x && guys[i]->mapy==y)
		{
			return guys[i];
		}

	return NULL;
}

Guy *FindMonster(int x,int y)
{
	int i;

	for(i=0;i<maxGuys;i++)
		if(guys[i] &&
		   guys[i]->type>=MONS_BLOBBY && guys[i]->type<=MONS_PIGGY &&
		   guys[i]->tgtx==x && guys[i]->tgty==y)
		{
			return guys[i];
		}

	return NULL;
}

byte MonsterBomb(int x,int y,Map *map)
{
	mapTile_t *m;
	Guy *g;

	// can't drop out of bounds
	if(x<0 || y<0 || x>=map->width || y>=map->height)
		return 0;

	m=&map->map[x+y*map->width];

	if(FloorIsSolid(m->floor))
		return 0;	// not on solid floor

	if(m->item)
		return 0;

	m->item=ITM_ACTIVEBOMB;
	g=AddGuy((x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,0,MONS_BOMB);
	g->color=m->itemInfo%8;

	return 1;
}

byte MonsterAtDestination(Guy *me,Map *map)
{
	byte f;
	mapTile_t *m;
	Guy *g;
	int telex,teley;

	map->GoodieCheck(me->mapx,me->mapy,GOODIE_MONSSTEP);
	m=&map->map[me->mapx+me->mapy*curMap->width];

	f=FloorArrowFacing(m->floor);
	if(f!=NOT_AN_ARROW)
	{
		me->facing=f;
	}

	if(FloorIsSpikes(m->floor))
	{
		if(MonsterExplode(me->mapx,me->mapy,map,0))
		{
			f=me->color;
			m->item=ITM_ACTIVEBOMB;
			g=AddGuy((me->mapx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
					(me->mapy*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,
					0,MONS_BOOM);
			g->color=f;
			return 0;
		}
	}

	if(FloorIsExit(m->floor))
	{
		ExitFX(me->x,me->y,me->color);
		player.rescueMons[me->color][me->type-MONS_BLOBBY]++;
		me->type=MONS_NONE;
		m->item=ITM_NONE;
		MakeSound(SND_RESCUE,me->x,me->y,SND_CUTOFF,100);
		return 0;
	}

	if(FloorIsHole(m->floor))
	{
		player.dumpedMons[me->color][me->type-MONS_BLOBBY]++;
		// fill the hole
		if(m->floor==80)
			m->floor+=20;
		else
			m->floor-=20;
		me->type=MONS_NONE;
		m->item=ITM_NONE;
		SmokePuff(me->x,me->y,2);
		MakeSound(SND_FALLINHOLE,me->x,me->y,SND_CUTOFF,100);
		player.dumpBonus++;
		return 0;
	}
	if(FloorIsPit(m->floor))
	{
		player.dumpedMons[me->color][me->type-MONS_BLOBBY]++;
		me->type=MONS_NONE;
		m->item=ITM_NONE;
		SmokePuff(me->x,me->y,2);
		MakeSound(SND_FALLINPIT,me->x,me->y,SND_CUTOFF,100);
		player.dumpBonus++;
		return 0;
	}

	if((me->mapx!=me->oldmapx || me->mapy!=me->oldmapy) && FloorIsTeleporter(map->map[me->mapx+me->mapy*map->width].floor))
	{
		if(map->SelectTeleporter(me->mapx,me->mapy,&telex,&teley))
		{
			LightningBolt(me->x,me->y,(telex*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
				(teley*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT);
			LightningBolt(me->x,me->y,(telex*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
				(teley*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT);
			LightningBolt(me->x,me->y,(telex*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
				(teley*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT);

			map->map[me->mapx+me->mapy*map->width].item=ITM_NONE;
			me->x=(telex*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
			me->y=(teley*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
			me->mapx=telex;
			me->mapy=teley;
			map->GoodieCheck(me->mapx,me->mapy,GOODIE_MONSSTEP);
			me->tx=me->x;
			me->ty=me->y;
			map->map[me->mapx+me->mapy*map->width].item=ITM_MONS1+me->type-MONS_BLOBBY;
			// add teleport FX
			MakeSound(SND_TELEPORT,me->x,me->y,SND_CUTOFF,100);
		}
		else
		{
			// fail teleport effect
			MakeSound(SND_NOTELEPORT,me->x,me->y,SND_CUTOFF,100);
		}
	}

	me->oldmapx=me->mapx;
	me->oldmapy=me->mapy;
	return 1;
}

byte MonsterMoveTo(Guy *me,int x,int y)
{
	mapTile_t *m;
	byte f;

	if(x<0 || y<0 || x>=curMap->width || y>=curMap->height)
		return 0;

	m=&curMap->map[x+y*curMap->width];

	if(FloorIsSolid(m->floor))
		return 0;	// can't walk on solid floor

	if(PushSwitch(curMap,m))
		return 0;

	if(TurnCrank(curMap,m))
		return 0;

	f=FloorOnlyColor(m->floor);
	if(f==0 || (f!=ANY_COLOR && f!=me->color))
		return 0;

	if(m->item==0)
	{
		return 1;
	}
	else
	{
		// depends on the type of monster
		switch(me->color)
		{
			case 0:	// grey
			case 2:	// brown
				// have no effect on objects of any kind
				return 0;
				break;
			case 1:	// green
				// push any object if it's pushable (but always act like you bumped)
				if(PushObject(x,y,me,m,curMap))
					MakeSound(SND_MONS_PUSH,me->x,me->y,SND_CUTOFF,30);
				return 0;
				break;
			case 3:	// blue
				// freeze objects
				if(m->item==ITM_BALL || m->item==ITM_BARREL || m->item==ITM_CRATE ||
					m->item==ITM_BOMB || m->item==ITM_ACTIVEBOMB || m->item==ITM_PENCIL ||
					m->item==ITM_LASER || m->item==ITM_ACID || m->item==ITM_SHIELD || m->item==ITM_KEY ||
					m->item==ITM_DETONATE)
				{
					if(m->item==ITM_PENCIL)
						player.deadPencils[m->itemInfo%8]++;
					// shut down active bomb
					if(m->item==ITM_ACTIVEBOMB)
					{
						Guy *g;
						g=FindBomb(x,y);
						if(g)
						{
							if(g->type!=MONS_BOOM)
								g->type=MONS_NONE;
							else
								return 0;	// can't freeze an explosion
						}
						else
							return 0;	// it's an explosion if it couldn't be found
					}
					BlowSmoke(x,y,0);
					m->item=ITM_ICEBERG;
					MakeSound(SND_FREEZE,me->x,me->y,SND_CUTOFF,30);
				}
				return 0;
				break;
			case 4:	// red
				// light bombs
				if(m->item==ITM_BOMB)
				{
					m->item=ITM_NONE;
					MonsterBomb(x,y,curMap);
					MakeSound(SND_MONS_BOMB,me->x,me->y,SND_CUTOFF,30);
				}
				// fire lasers
				if(m->item==ITM_LASER)
				{
					FireLaser(x+GetDX(me->facing/2),y+GetDY(me->facing/2),me->facing,m->itemInfo%8,curMap);
				}
				if(m->item==ITM_DETONATE)
				{
					if(DetonateAllBombs(m->itemInfo%8,curMap))
						MakeNormalSound(SND_DETONATOR);
				}
				return 0;
				break;
			case 5:	// yellow
				// vaporize stuff (same effect as acid item)
				// doesn't work on grey stuff
				if((m->item==ITM_BALL || m->item==ITM_BARREL || m->item==ITM_CRATE ||
					m->item==ITM_BOMB || m->item==ITM_ICEBERG || m->item==ITM_PENCIL ||
					m->item==ITM_LASER || m->item==ITM_ACID || m->item==ITM_KEY || m->item==ITM_DETONATE)
					&& (m->itemInfo%8)!=0)
				{
					BlowSmoke(x,y,m->itemInfo%8);
					MakeSound(SND_MONS_EAT,me->x,me->y,SND_CUTOFF,30);
					if(m->item!=ITM_PENCIL && m->item!=ITM_BOMB && m->item!=ITM_LASER && m->item!=ITM_ACID && m->item!=ITM_KEY)
					{
						player.blownUp[m->itemInfo%8]++;
						Score_BlowUp(SCORE_OBJECT,m->itemInfo%8);
					}
					else if(m->item==ITM_PENCIL)
						player.deadPencils[m->itemInfo%8]++;
					else if(m->item==ITM_ACID)
					{
						// melt if you eat acid!
						BlowSmoke(x,y,5);
						MonsterExplode(me->mapx,me->mapy,curMap,0);
						MakeNormalSound(SND_ACID);
					}
					m->item=ITM_NONE;

					return 1;
				}
				return 0;
				break;
			case 6: // purple
				// kill player
				if(m->item==ITM_PLAYER)
				{
					PlayerExplode(curMap);
				}
				return 0;
				break;
			case 7: // aqua
				// shoot sometimes, not related to bumping
				return 0;
				break;
		}
	}
	return 0;
}

byte MonsterExplode(int x,int y,Map *map,byte greyOK)
{
	Guy *g;

	g=FindMonster(x,y);
	if(g && (g->color!=0 || greyOK))
	{
		Score_BlowUp(SCORE_MONSTER,g->color);
		player.blownUpMons[g->color][g->type-MONS_BLOBBY]++;
		g->type=MONS_NONE;
		map->map[x+y*map->width].item=ITM_NONE;
		return g->color;
	}
	return 0;
}

void SummonGuy(mapTile_t *m,int x,int y)
{
	byte face;
	byte color;
	byte type;
	Guy *g;

	if(m->item)
		return;	// can't summon

	type=(m->floor%20);
	face=(m->floor/20)-8;
	if(type>7)
	{
		color=type-8;
		type=MONS_BUNNY+2*(face>=4);
		face=face%4;
	}
	else
	{
		color=type;
		type=MONS_BLOBBY+2*(face>=4);
		face=face%4;
	}

	g=AddGuy((x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,0,type);
	if(g)
	{
		g->color=color;
		g->facing=face*2;
		m->item=ITM_MONS1+g->type-MONS_BLOBBY;
		m->templight=32;
		MakeNormalSound(SND_GENERATE);
	}
}

#include "guy.h"
#include "player.h"
#include "intface.h"
#include "quest.h"
#include "editor.h"
#include "survival.h"
#include "options.h"
#include "badge.h"
#include "loonyball.h"
#include "ch_witch.h"
#include "bossbash.h"
#include "ch_summon.h"

Guy **guys;
Guy *goodguy;
int maxGuys;
static byte checkActive=0;
Guy *lifeGuy;
Guy fakeGuy;

//------------------------------------------------------------------------
// CLASS GUY

Guy::Guy(void)
{
	type=MONS_NONE;
}

Guy::~Guy(void)
{
}

byte Guy::CoconutBonk(int xx,int yy,Guy *him)
{
	int x2,y2;

	if((MonsterFlags(him->type)&MF_ENEMYWALK) && (type!=player.monsType))
		return 0;
	if((MonsterFlags(him->type)&MF_FREEWALK) && (type==player.monsType))
		return 0;

	x2=him->x>>FIXSHIFT;
	y2=him->y>>FIXSHIFT;

	if((x2+him->rectx2)>=(xx+rectx) && (y2+him->recty2)>=(yy+recty) &&
		(x2+him->rectx)<=(xx+rectx2) && (y2+him->recty)<=(yy+recty2))
	{
		return 1;
	}
	return 0;
}

// checks to see if a rect of size, located at (xx,yy) hits HIM
byte Guy::AttackCheck(byte size,int xx,int yy,Guy *him)
{
	int x2,y2;

	if(!him)
		return 0;

	xx-=size;
	yy-=size;

	x2=him->x>>FIXSHIFT;
	y2=him->y>>FIXSHIFT;

	//DrawDebugBox(xx,yy,xx+size*2,yy+size*2);
	if((x2+him->rectx2)>=xx && (y2+him->recty2)>=yy && (x2+him->rectx)<=(xx+size*2) &&
		(y2+him->recty)<=(yy+size*2))
		return 1;

	return 0;
}

// check to see if the chosen tile intersects HIMs rectangle
byte TileBonkCheck(int x,int y,Guy *him)
{
	int himX,himY;

	if(!him)
		return 0;

	himX=him->x>>FIXSHIFT;
	himY=him->y>>FIXSHIFT;

	if((himX+him->rectx2)>=x && (himY+him->recty2)>=y && (himX+him->rectx)<(x+TILE_WIDTH) &&
		(himY+him->recty)<(y+TILE_HEIGHT))
		return 1;

	return 0;
}

void TryToPush(Guy *me,int x,int y,Map *map,world_t *world)
{
	int destx,desty,i;
	int xx,yy;

	if(!PlayerPushMore())
		return;	// not pushing hard enough yet

	if(abs(me->dx)>abs(me->dy))	// horizontal push
	{
		desty=y;
		if(me->dx>0)	// right push
		{
			destx=x+1;
			if(me->mapx!=x-1 || me->mapy!=y)
				return;	// no push
		}
		else
		{
			destx=x-1;	// left push
			if(me->mapx!=x+1 || me->mapy!=y)
				return;	// no push
		}
	}
	else	// vertical push
	{
		destx=x;
		if(me->dy>0)	// down push
		{
			desty=y+1;
			if(me->mapx!=x || me->mapy!=y-1)
				return;	// no push
		}
		else
		{
			desty=y-1;	// up push
			if(me->mapx!=x || me->mapy!=y+1)
				return;	// no push
		}
	}

	if(destx<0 || destx>=map->width || desty<0 || desty>=map->height)
		return;	// can't push off edge of map
	if(map->map[destx+desty*map->width].wall)
		return;	// can't push into a wall
	if(map->map[destx+desty*map->width].item)
		return;	// can't push into any item
	if(world->terrain[map->map[destx+desty*map->width].floor].flags&TF_PUSHON)
	{
		// it passed every single test. the only thing left is to see if it would overlap
		// any badguys, which would not be good
		xx=destx*TILE_WIDTH;
		yy=desty*TILE_HEIGHT;
		for(i=0;i<maxGuys;i++)
			if((guys[i]) && (guys[i]->type) && (guys[i]->hp>0) && (guys[i]->active) &&
				(abs(guys[i]->mapx-destx)<8) && (abs(guys[i]->mapy-desty)<8))
				if(TileBonkCheck(xx,yy,guys[i]))
					return;	// the pushy block would hit the guy

		// the floor is pushonable, let's do it
		MakeSound(SND_BLOCKPUSH,me->x,me->y,SND_CUTOFF,1000);
		if(map->map[x+y*map->width].item)
		{
			// pushing an item
			map->map[destx+desty*map->width].item=map->map[x+y*map->width].item;
			map->map[x+y*map->width].item=ITM_NONE;
			SpecialItemGetCheck(map,destx,desty);
			SpecialPushCheck(map,destx,desty);
			SpecialPushCheck(map,x,y);
		}
		else
		{
			// pushing wall/floor
			map->map[destx+desty*map->width].wall=map->map[x+y*map->width].wall;
			map->map[destx+desty*map->width].floor=map->map[x+y*map->width].floor;
			map->map[x+y*map->width].floor=world->terrain[map->map[x+y*map->width].floor].next;
			map->map[x+y*map->width].wall=0;
		}
	}
}

byte Walkable(Guy *me,int x,int y,Map *map,world_t *world)
{
	byte result;
	mapTile_t *m=&map->map[x+y*map->width];

	result=1;

	if(me->type==player.monsType && (!PlayerGetItem(m->item,x,y)))
	{
		SpecialItemGetCheck(map,x,y);
		m->item=ITM_NONE;
	}

	if((world->terrain[m->floor].flags&(TF_WATER|TF_LAVA)) && (!(MonsterFlags(me->type)&(MF_WATERWALK|MF_FLYING))))
	{
		if(me->type==player.monsType)
		{
			// ouch, can't walk there!
			me->GetShot(-me->dx,-me->dy,1,map,world);
		}
		result=0;
	}

	if((world->terrain[m->floor].flags&TF_SOLID) && (!(MonsterFlags(me->type)&(MF_FLYING|MF_WALLWALK))) &&
		(m->item!=ITM_WALKPLANT))
		result=0; // impassable

	if((world->terrain[m->floor].flags&TF_NOVILLAGER) &&
		me->type>=MONS_VILLAGER && me->type<=MONS_VILLAGER6)
		result=0; // impassable

	if((m->tag==255) && (!(MonsterFlags(me->type)&(MF_WALLWALK))) && (me->type!=player.monsType) && (me->type<MONS_SUMBOMBIE))
		result=0; // impassable to non-Loony and non-ghost
	if((m->tag==254) && (me->type!=player.monsType) && (me->type>MONS_VILLAGER6 || me->type<MONS_VILLAGER) &&
		(me->type<MONS_SUMBOMBIE))
		result=0; // impassable to non-villagers
	if((m->tag==253) && (me->type>=MONS_MUMMY) && (me->type<=MONS_MUMMY4))
		result=0; // impassable to mummies.  Why?  For Loonyball mode where they are goalies

	if((world->terrain[m->floor].flags&TF_PUSHY) && me->type==player.monsType)
	{
		TryToPush(me,x,y,map,world);
	}
	if((ItemFlags(m->item)&IF_PUSHABLE) && me->type==player.monsType)
	{
		TryToPush(me,x,y,map,world);
	}

	if(m->item>=ITM_DOOR && m->item<ITM_DOOR2 && me->type==player.monsType)
	{
		if(m->itemInfo==0 && player.keys[m->item-ITM_DOOR]>0)
		{
			m->itemInfo=1;
			if(m->item==ITM_DOOR && !(player.cheatsOn&PC_ALLACCESS))
				player.keys[0]--;	// plain keys get used up
			MakeNormalSound(SND_DOOROPEN);
		}
		result=0;
	}
	if(MonsterFlags(me->type)&MF_FLYING)
	{
		if((m->wall || (ItemFlags(m->item)&IF_TALL)) && (!(MonsterFlags(me->type)&MF_WALLWALK)))
			result=0;
	}
	else if(MonsterFlags(me->type)&MF_AQUATIC)
	{
		if(!(world->terrain[m->floor].flags&(TF_WATER|TF_LAVA)))
			result=0;
		if(m->wall || m->item>=(ItemFlags(m->item)&IF_OBSTACLE))
			result=0;
	}
	else
	{
		if((m->wall || (ItemFlags(m->item)&IF_OBSTACLE)) && (!(MonsterFlags(me->type)&MF_WALLWALK)))
		{
			if(m->item==ITM_SAVEGEM && me->type==player.monsType)
			{
				if(player.saveClock==0)
					BumpSaveGem();
			}
			result=0;
		}
	}

	if(opt.cheats[CH_WALLWALK] && me->type==player.monsType && player.saveClock==0)
		result=1;

	if(!result)	// bumped a wall, see if that triggers a special
	{
		SpecialStepCheck(map,x,y,me);
	}
	return result;
}

byte Guy::CanWalk(int xx,int yy,Map *map,world_t *world)
{
	byte result;
	int mapx1,mapx2,mapy1,mapy2;
	int i,j;

	xx>>=FIXSHIFT;
	yy>>=FIXSHIFT;

	mapx1=(xx+rectx)/TILE_WIDTH;
	mapy1=(yy+recty)/TILE_HEIGHT;
	mapx2=(xx+rectx2)/TILE_WIDTH;
	mapy2=(yy+recty2)/TILE_HEIGHT;

	// boundary conditions
	if(mapx1<0 || mapy1<0 || mapx2>=map->width || mapy2>=map->height || xx<0 || yy<0)
		return 0;

	result=1;
	for(i=mapx1;i<=mapx2;i++)
	{
		for(j=mapy1;j<=mapy2;j++)
		{
			if(!Walkable(this,i,j,map,world))
			{
				result=0;
				break;
			}
		}
		if(result==0)
			break;
	}

	if(MonsterFlags(type)&MF_FREEWALK)
		return result;	// can't have a guy collision

	if(result)	// no wall collision, look for guy collision
		for(i=0;i<maxGuys;i++)
			if((guys[i]) && (guys[i]!=this) && (guys[i]->type) && (guys[i]->hp>0) &&
				(player.levelNum!=0 || (player.worldNum!=WORLD_NORMAL && player.worldNum!=WORLD_REMIX && player.worldNum!=WORLD_RANDOMIZER) || guys[i]->active))
			{
				if(CoconutBonk(xx,yy,guys[i]))
				{
					if(type==player.monsType && player.chatClock==0 && (dx!=0 || dy!=0) &&
						(guys[i]->type>=MONS_VILLAGER && guys[i]->type<=MONS_VILLAGER6) &&
						(guys[i]->mind3==0))
					{
						FaceGoodguy(guys[i],this);
						guys[i]->action=ACTION_IDLE;
						guys[i]->seq=ANIM_IDLE;
						guys[i]->frm=0;
						guys[i]->frmTimer=0;
						guys[i]->frmAdvance=0;
						if(guys[i]->type<MONS_VILLAGER3)
							MakeSound(SND_VILLAGERHELLO,guys[i]->x,guys[i]->y,SND_CUTOFF,1200);
						else if(guys[i]->type==MONS_VILLAGER3)
							MakeSound(SND_VILLAGERHELLO3,guys[i]->x,guys[i]->y,SND_CUTOFF,1200);
						else
							MakeSound(SND_VILLAGERHELLO2,guys[i]->x,guys[i]->y,SND_CUTOFF,1200);
						BeginChatting(guys[i]->tag);
					}
					return 0;	// hit heads with somebody
				}
			}

	return result;
}

void Guy::SeqFinished(void)
{
	byte t;

	if(seq==ANIM_DIE)
	{
		if(player.worldNum==WORLD_LOONYBALL && type!=MONS_HELPERBAT)
		{
			GoToPenaltyBox(this,curMap);
			DoMove(this,ANIM_IDLE,128,0,0,0);
			hp=MonsterHP(type);
			if(type==player.monsType)
			{
				player.hearts=player.maxHearts;
				hp=player.hearts;
				player.poison=0;
			}
			return;
		}
		if(type==player.monsType)
		{
			// restart current level
			SendMessageToGame(MSG_RESET,player.lastSave);
		}

		t=type;
		type=MONS_NONE;	// all gone
		BadgeCheck(BE_KILL,t,curMap);
		if(opt.cheats[CH_CORPSE])
		{
			FireBullet(x,y,0,BLT_GOODBOOM2);
		}
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

void Guy::CalculateRect(void)
{
	sprite_t *spr;
	byte s;

	if(MonsterFlags(type)&MF_SPRITEBOX)
	{
		spr=GetMonsterSprite(type,seq,frm,facing);
		if(spr)
		{
			rectx=-spr->ofsx;
			rectx2=spr->width-spr->ofsx;
			recty=-spr->ofsy;
			recty2=spr->height-spr->ofsy;
		}
		else
		{
			rectx=0;
			recty=0;
			rectx2=0;
			recty2=0;
		}
	}
	else	// normal method
	{
		s=MonsterSize(type);
		rectx=-s;
		rectx2=s;
		recty=-s*3/4;
		recty2=s*3/4;
	}
}

byte Guy::IsActive(Map *map)
{
	if(checkActive==0)
	{
		if((type<MONS_TUMBLEROCK || type>MONS_TUMBLEROCK4) && (type!=MONS_EVILTREE2) &&
		   (type<MONS_LIGHTBALL || type>MONS_LIGHTBALL5) && (type!=MONS_HELPERBAT) &&
		   (type!=MONS_BONKULA) && (type!=MONS_MECHABONK) && (type!=MONS_EVILIZER) && (type!=MONS_EVILPUMP) &&
		   type<MONS_SUMBOMBIE &&	// summons never deactivate
		   (type!=player.monsType) && (hp>0) &&
			(abs(x-goodguy->x)>FIXAMT*640 || abs(y-goodguy->y)>FIXAMT*480))
		{
			active=0;
			// monsters are inactive when far away, except 'trap' type enemies, to keep them synced
			// also, threw in that dead enemies remain active, so that they can finish their
			// death sequence rather than freezing and finishing dying when you return
		}
		else
			active=1;

		if(opt.cheats[CH_RADAR])
		{
			// Matches AnyMonsterExists so "kill all guys" checks only requires you to get red off the radar.
			if(type==player.monsType)
				AddRadarBlip(x,y,32*1+16,map);  // player: green
			else if(type==MONS_HELPERBAT || (type >= MONS_VILLAGER && type <= MONS_VILLAGER6) || (type >= MONS_SUMBOMBIE && type <= MONS_SUMDOG))
				AddRadarBlip(x,y,31,map);  // allies: gray
			else if(type==MONS_EVILTREE2 || type == MONS_LIGHTBALL || type == MONS_LIGHTBALL2 || type == MONS_LIGHTBALL4 || type == MONS_LIGHTBALL5 || type == MONS_BUBBLE)
				AddRadarBlip(x,y,224+20,map);  // invincible guys: lightish aqua
			else
				AddRadarBlip(x,y,128+16,map);  // normal guys: red
			// evilizer, bonkula, wolfmen etc. are red despite sometimes being invincible
		}
	}
	return active;
}

void Guy::Update(Map *map,world_t *world)
{
	int oldmapx,oldmapy;
	byte b;
	static byte burnFlip=0;
	int ddx,ddy;

	oldmapx=mapx;
	oldmapy=mapy;

	CalculateRect();

	if(hp==0 && seq!=ANIM_DIE)
	{
		seq=ANIM_DIE;
		frm=0;
		frmTimer=0;
		frmAdvance=64;
		action=ACTION_BUSY;
	}

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

	ddx=dx;
	ddy=dy;

	x+=ddx;

	if(!CanWalk(x,y,map,world))
	{
		x-=ddx;
		if(type==player.monsType || (type>=MONS_VILLAGER && type<=MONS_VILLAGER6) ||
			(type>=MONS_TUMBLEROCK && type<=MONS_TUMBLEROCK4) || type==MONS_BUBBLE ||
			type==MONS_BIGGHOST || type==MONS_LARRY || type==MONS_HUMANLARRY || type>=MONS_SUMBOMBIE)
			mind1=1;	// tell it that it hit a wall
	}

	y+=ddy;

	if(!CanWalk(x,y,map,world))
	{
		y-=ddy;
		if(type==player.monsType || (type>=MONS_VILLAGER && type<=MONS_VILLAGER6) ||
			(type>=MONS_TUMBLEROCK && type<=MONS_TUMBLEROCK4) || type==MONS_BUBBLE ||
			type==MONS_BIGGHOST || type==MONS_LARRY || type==MONS_HUMANLARRY || type>=MONS_SUMBOMBIE)
			mind1+=2;	// tell it that it hit a wall
	}

	if(MonsterFlags(type)&MF_FLYING)
	{
		if(z<20*FIXAMT)	// go up if you need to
			dz+=FIXAMT;
		else
			dz-=FIXAMT;

		Clamp(&dz,FIXAMT*4);
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
		if((map->flags&MAP_TORCHLIT) && (player.var[VAR_TORCH]==1 || player.var[VAR_LANTERN]==1) && (!WindingDown()))
		{
			b=GetPlayerGlow();
			if(b>32)
				map->BrightTorch(mapx,mapy,b,b/4);
			else
			{
				if((player.var[VAR_LANTERN]==0) || (player.levelNum==42 && (player.worldNum==WORLD_NORMAL || player.worldNum==WORLD_REMIX|| player.worldNum==WORLD_RANDOMIZER)))
					map->TempTorch(mapx,mapy,b);
				else
					map->WeakTorch(mapx,mapy);	// with the lantern, you have way more light
												// except in Bonkula's lair which is DARK!
			}
			if(b>32)
				b-=4;
			else if(b<32)
				b++;
			SetPlayerGlow(b);
		}
		else if((map->flags&MAP_LITDUNGEON) && (!WindingDown()))
		{
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
			map->WeakTorch(mapx,mapy);
		}
		else
		{
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

		// standing on water!!!!!!!  DROWN!!!!
		if((hp>0) && (world->terrain[map->map[mapx+mapy*map->width].floor].flags&TF_WATER) &&
			map->map[mapx+mapy*map->width].item!=ITM_WALKPLANT)
		{
			if(player.invinc)
			{
				x-=dx*2;
				if(dx!=0)
				{
					if(!CanWalk(x,y,map,world))
						x+=dx;
					if(!CanWalk(x,y,map,world))
						x+=dx;
				}
				y-=dy*2;
				if(dy!=0)
				{
					if(!CanWalk(x,y,map,world))
						y+=dy;
					if(!CanWalk(x,y,map,world))
						y+=dy;
				}
			}
			else
				GetShot(-dx*2,-dy*2,1,map,world);
		}
		// standing on lava, OW!
		if((hp>0) && (world->terrain[map->map[mapx+mapy*map->width].floor].flags&TF_LAVA) &&
			map->map[mapx+mapy*map->width].item!=ITM_WALKPLANT)
		{
			if(burnFlip)
			{
				GetShot(0,0,3,map,world);
			}
			burnFlip=1-burnFlip;
		}
		if((oldmapx!=mapx || oldmapy!=mapy) &&
			(world->terrain[map->map[mapx+mapy*map->width].floor].flags&TF_STEP))
		{
			map->map[mapx+mapy*map->width].floor=world->terrain[map->map[mapx+mapy*map->width].floor].next;
			if(player.worldNum==WORLD_REMIX && player.levelNum==49)
				CheckHiPuzzle(map);
			// spiked floor
			if(map->map[mapx+mapy*map->width].floor<200)
				MakeNormalSound(SND_SWITCH);
		}
	}
	if(oldmapx!=mapx || oldmapy!=mapy)
		SpecialStepCheck(map,mapx,mapy,this);
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
	MonsterDraw(x,y,z,type,seq,frm,facing,bright*(light>0),ouch,ice);
}

void Guy::MonsterControl(Map *map,world_t *world)
{
	if(!goodguy)
		return;	// badguys don't move if you're gone, saves a lot of crash possibilities

	switch(type)
	{
		case MONS_BONEHEAD:
		case MONS_BONEHEAD2:
		case MONS_BONEHEAD3:
		case MONS_BONEHEAD4:
		case MONS_BONEHEAD5:
			AI_Bonehead(this,map,world,goodguy);
			break;
		case MONS_ZOMBIE:
		case MONS_ZOMBIE2:
		case MONS_ZOMBIE3:
		case MONS_ZOMBIE4:
			AI_Zombie(this,map,world,goodguy);
			break;
		case MONS_BAT:
		case MONS_BAT2:
		case MONS_BAT3:
		case MONS_BAT4:
			AI_Bat(this,map,world,goodguy);
			break;
		case MONS_VILLAGER:
		case MONS_VILLAGER2:
		case MONS_VILLAGER3:
		case MONS_VILLAGER4:
		case MONS_VILLAGER5:
		case MONS_VILLAGER6:
			AI_Villager(this,map,world,goodguy);
			break;
		case MONS_FROG:
		case MONS_FROG2:
		case MONS_FROG3:
		case MONS_FROG4:
			AI_Frog(this,map,world,goodguy);
			break;
		case MONS_MUMMY:
		case MONS_MUMMY2:
		case MONS_MUMMY3:
		case MONS_MUMMY4:
		case MONS_MUMMY5:
			if(player.worldNum==WORLD_LOONYBALL)
				AI_Goalie(this,map,world,goodguy);
			else
				AI_Mummy(this,map,world,goodguy);
			break;
		case MONS_EVILTREE:
		case MONS_EVILTREE2:
		case MONS_EVILTREE3:
			AI_EvilTree(this,map,world,goodguy);
			break;
		case MONS_WOLFMAN:
		case MONS_WOLFMAN2:
		case MONS_WOLFMAN3:
			AI_Wolfman(this,map,world,goodguy);
			break;
		case MONS_LARRY:
			AI_Larry(this,map,world,goodguy);
			break;
		case MONS_HUMANLARRY:
			AI_HumanLarry(this,map,world,goodguy);
			break;
		case MONS_SWAMPDOG:
		case MONS_SWAMPDOG2:
		case MONS_SWAMPDOG3:
		case MONS_SWAMPDOG4:
			AI_Swampdog(this,map,world,goodguy);
			break;
		case MONS_GHOST:
		case MONS_GHOST2:
		case MONS_GHOST3:
			AI_Ghost(this,map,world,goodguy);
			break;
		case MONS_PUMPKIN:
		case MONS_PUMPKIN2:
		case MONS_PUMPKIN3:
		case MONS_PUMPKIN4:
			if(player.worldNum==WORLD_BOWLING)
				AI_BowlingPin(this,map,world,goodguy);
			else
				AI_Pumpkin(this,map,world,goodguy);
			break;
		case MONS_VAMPIRE:
		case MONS_VAMPIRE2:
		case MONS_VAMPIRE3:
		case MONS_VAMPIRE4:
			AI_Vampire(this,map,world,goodguy);
			break;
		case MONS_STICKWITCH:
			AI_StickWitch(this,map,world,goodguy);
			break;
		case MONS_TUMBLEROCK:
		case MONS_TUMBLEROCK2:
		case MONS_TUMBLEROCK3:
		case MONS_TUMBLEROCK4:
			AI_Tumblerock(this,map,world,goodguy);
			break;
		case MONS_LIGHTBALL:
		case MONS_LIGHTBALL2:
		case MONS_LIGHTBALL3:
		case MONS_LIGHTBALL4:
		case MONS_LIGHTBALL5:
			AI_Lightball(this,map,world,goodguy);
			break;
		case MONS_BHOMATIC:
		case MONS_RBOMATIC:
		case MONS_GGOMATIC:
		case MONS_NSOMATIC:
			AI_Boneheadomatic(this,map,world,goodguy);
			break;
		case MONS_FRANKENJ:
			AI_Frankenjulie(this,map,world,goodguy);
			break;
		case MONS_HAND:
			AI_Hand(this,map,world,goodguy);
			break;
		case MONS_BIGGHOST:
			AI_Polterguy(this,map,world,goodguy);
			break;
		case MONS_JUNK:
		case MONS_JUNK2:
		case MONS_JUNK3:
			AI_Junk(this,map,world,goodguy);
			break;
		case MONS_HELPERBAT:
			AI_HelperBat(this,map,world,goodguy);
			break;
		case MONS_ELDER:
			AI_WindElder(this,map,world,goodguy);
			break;
		case MONS_ELDER2:
			AI_EarthElder(this,map,world,goodguy);
			break;
		case MONS_ELDER3:
			AI_WaterElder(this,map,world,goodguy);
			break;
		case MONS_ELDER4:
			AI_FireElder(this,map,world,goodguy);
			break;
		case MONS_ELDER5:
			AI_SummonElder(this,map,world,goodguy);
			break;
		case MONS_BUBBLE:
			AI_Bubble(this,map,world,goodguy);
			break;
		case MONS_BONKULA:
			AI_Bonkula(this,map,world,goodguy);
			break;
		case MONS_EVILIZER:
			AI_Evilizer(this,map,world,goodguy);
			break;
		case MONS_EVILPUMP:
			AI_EvilPump(this,map,world,goodguy);
			break;
		case MONS_HARRY:
			AI_Harry(this,map,world,goodguy);
			break;
		case MONS_GHASTLY:
			AI_Ghastly(this,map,world,goodguy);
			break;
		case MONS_MINIFRANK:
			AI_MiniFrankenjulie(this,map,world,goodguy);
			break;
		case MONS_MECHABONK:
			AI_MechaBonkula(this,map,world,goodguy);
			break;
		case MONS_SUMBOMBIE:
			AI_Summon_Bombie(this,map,world,goodguy);
			break;
		case MONS_SUMBAT:
			AI_Summon_Bat(this,map,world,goodguy);
			break;
		case MONS_SUMFROG:
			AI_Summon_Frog(this,map,world,goodguy);
			break;
		case MONS_SUMDOG:
			AI_Summon_HotDog(this,map,world,goodguy);
			break;
		case MONS_SUMMUMMY:
			AI_Summon_Mummy(this,map,world,goodguy);
			break;
		case MONS_SUMGHOST:
			AI_Summon_Ghost(this,map,world,goodguy);
			break;
		case MONS_SUMWOLF:
			AI_Summon_Wolf(this,map,world,goodguy);
			break;
	}
}

void Guy::GetShot(int dx,int dy,byte damage,Map *map,world_t *world)
{
	int i,j,formerHP;
	dword combo;
	byte v;

	if(hp==0)
		return;	// can't shoot a dead guy

	if(type==0)
		return;	// shouldn't have a type 0 guy at all

	if(player.worldNum==WORLD_LOONYBALL && type>=MONS_MUMMY && type<=MONS_MUMMY4)
		return;	// invincible mummies in loonyball

	if(opt.cheats[CH_DEATH] && type!=player.monsType)
	{
		// death cheat makes them DIE.  ANYBODY.  Even the invincible!!
		hp=1;
		damage=1;
	}
	if(type==player.monsType && (PlayerShield() || player.invinc || opt.cheats[CH_SUPERSURV]))
		return; // invincible when shielded

	/*if(player.monsType=MONS_LOONYTOAD){
		// toad knocksback
		
		dx=((dx>0)-(dx<0))*FIXAMT*2 * player.fireRange;
		dy=((dy>0)-(dy<0))*FIXAMT*2 * player.fireRange;
		x+=dx;
		if(dx!=0)
			if(!CanWalk(x,y,map,world))
				x-=dx;
		y+=dy;
		if(dy!=0)
			if(!CanWalk(x,y,map,world))
				y-=dy;
	}*/

	if((MonsterFlags(type)&MF_INVINCIBLE) && !opt.cheats[CH_DEATH])
	{
		if(type>=MONS_WOLFMAN && type<=MONS_WOLFMAN3 && player.var[VAR_KNOWWOLVES]==0)
		{
			player.var[VAR_KNOWWOLVES]=1;
			BeginChatting(246);
		}
		return;	// invincible
	}

	if(type==MONS_ELDER && (bulletHittingType==BLT_BALLLIGHTNING || bulletHittingType==BLT_PSDSHOT || bulletHittingType==BLT_WITCHBLAST)
		&& !(opt.cheats[CH_DEATH]))
	{
		// immune to slingshots
		return;
	}

	if(type==MONS_ELDER2 && !opt.cheats[CH_DEATH])
	{
		// can't be harmed, just shoved around... very slightly
		dx=((dx>0)-(dx<0))*FIXAMT*2;
		dy=((dy>0)-(dy<0))*FIXAMT*2;
		x+=dx;
		if(dx!=0)
			if(!CanWalk(x,y,map,world))
				x-=dx;
		y+=dy;
		if(dy!=0)
			if(!CanWalk(x,y,map,world))
				y-=dy;
		if(player.var[240]==0)
		{
			player.var[240]=1;
			BeginChatting(242);
		}
		return;
	}

	if(type==MONS_ELDER3 && bulletHittingType!=255 && !opt.cheats[CH_DEATH])
	{
		// immune to everything but shock, though still gets moved around
		x+=dx;
		if(dx!=0)
			if(!CanWalk(x,y,map,world))
				x-=dx;
		y+=dy;
		if(dy!=0)
			if(!CanWalk(x,y,map,world))
				y-=dy;
		this->dx+=dx;
		this->dy+=dy;	// get slid around

		if(player.var[240]==0)
		{
			player.var[240]=1;
			BeginChatting(240);
		}

		return;
	}

	if(type==MONS_ELDER4 && bulletHittingType!=BLT_WATER && !opt.cheats[CH_DEATH])
	{
		// gets moved by bullets, but otherwise unaffected
		x+=dx;
		if(dx!=0)
			if(!CanWalk(x,y,map,world))
				x-=dx;
		y+=dy;
		if(dy!=0)
			if(!CanWalk(x,y,map,world))
				y-=dy;

		if(player.var[240]==0)
		{
			player.var[240]=1;
			BeginChatting(239);
		}
		return;
	}

	if(type==player.monsType)
	{
		player.stone=0;

		if(player.cheatsOn&PC_TERROR)
		{
			damage*=2;
		}
		// player takes 1/2 damage in easy mode, double in hard mode
		if(player.difficulty==DIFF_BEGINNER)
		{
			if(damage>1)
				damage=1;
		}
		else if(player.difficulty==DIFF_NORMAL)
		{
			if(damage>1)
				damage/=2;
		}
		else if(player.difficulty==DIFF_LOONY)
		{
			damage*=2;
		}

		if(player.monsType==MONS_PLYRWOLF)
		{
			if(damage>1)
				damage/=2;	// wolf takes half damage
		}
	}
	else
	{
		if(player.difficulty==DIFF_BEGINNER)
		{
			damage*=2;
		}
		else if(player.difficulty==DIFF_NORMAL || player.difficulty==DIFF_HARD)
		{
			damage=damage*3/2;
		}
		else if(player.difficulty==DIFF_MAD || player.difficulty==DIFF_LOONY)
		{
			if(damage>1)
				damage/=4;
			if(damage==0)
				damage=1;
		}

		// cut it in half again in terror mode to simulate double life
		if(player.cheatsOn&PC_TERROR)
		{
			if(damage>1)
				damage=damage/2;
		}
	}

	formerHP=hp;
	if(!ice)
		ouch=4;
	hp-=damage;

	if(opt.cheats[CH_SIDEKICK] && type==player.monsType)
	{
		if(player.batLevel>=(dword)(damage*32))
			player.batLevel-=damage*32;
		else
			player.batLevel=0;
	}

	if(ice)
	{
		ice/=2;
		if(hp>0)
		{
			if(type!=player.monsType)
			{
				lifeGuy=this;
				ShowEnemyLife(MonsterName(type),formerHP*128/MonsterHP(type),hp*128/MonsterHP(type),(byte)(hp>0));
			}
			ExplodeParticles2(PART_SNOW2,x,y,z,16,10);
		}
		else
			ice=0;
	}
	if(!(MonsterFlags(type)&MF_NOMOVE))
	{
		x+=dx;
		if(dx!=0)
			if(!CanWalk(x,y,map,world))
				x-=dx;
		y+=dy;
		if(dy!=0)
			if(!CanWalk(x,y,map,world))
				y-=dy;
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
				BadgeCheck(BE_COMBO,player.combo,map);
			}
			else
			{
				player.combo=1;
			}
			player.comboClock=COMBO_TIME;
		}

		switch(type)
		{
			case MONS_FRANKENJ:
				KilledBoss(BOSS_FRANK);
				break;
			case MONS_LARRY:
			case MONS_HUMANLARRY:
				KilledBoss(BOSS_LARRY);
				break;
			case MONS_BIGGHOST:
				KilledBoss(BOSS_GHOST);
				break;
			case MONS_ELDER:
			case MONS_ELDER2:
			case MONS_ELDER3:
			case MONS_ELDER4:
				KilledBoss(BOSS_ELDER1+type-MONS_ELDER);
				break;
			case MONS_BONKULA:
				KilledBoss(BOSS_BONKULA);
				break;
			case MONS_EVILIZER:
				KilledBoss(BOSS_EVILIZER);
				break;
			case MONS_STICKWITCH:
				KilledBoss(BOSS_WITCH);
				break;
		}

		hp=0;
		seq=ANIM_DIE;
		if(player.worldNum!=WORLD_BOWLING)
		{
			this->dx=0;
			this->dy=0;
		}
		else
		{
			this->dx=dx;
			this->dy=dy;
		}
		frm=0;
		frmAdvance=64;
		frmTimer=0;
		action=ACTION_BUSY;

		if(type!=player.monsType)
		{
			player.guysKilled++;
			combo=player.combo;
			if(combo>20)
				combo=20;

			if(tag!=255)	// no MP for resurrected guys
			{
				if(player.worldNum!=WORLD_BOWLING)
					PlayerGetMonsterPoints(MonsterLevel(type)+combo-1);
				else
					PlayerGetMonsterPoints(combo);
			}

			if((player.fireFlags&FF_HELPER) && player.hearts==player.maxHearts)
			{
				player.batLevel+=combo*2;
				if(player.batLevel>255)
					player.batLevel=255;
			}
		}

		// possible item drop (resurrected guys drop nothing)
		if(player.worldNum!=WORLD_SLINGSHOT && player.worldNum!=WORLD_LOONYBALL &&
			player.worldNum!=WORLD_BOWLING && tag!=255)
		{
			if(type!=player.monsType)
			{
				if(Random(70)==0 && (player.worldNum==WORLD_NORMAL || player.worldNum==WORLD_REMIX || player.worldNum==WORLD_RANDOMIZER))
				{
					//todo hooks for randomizer maybe
					if(type>=MONS_BAT && type<=MONS_BAT3 && player.var[VAR_DOLL]==0)
					{
						FireBullet(x,y,ITM_BATDOLL,BLT_ITEM);
					}
					else if(type>=MONS_BONEHEAD && type<=MONS_BONEHEAD4 && player.var[VAR_DOLL+1]==0)
					{
						FireBullet(x,y,ITM_SKELDOLL,BLT_ITEM);
					}
					else if(type>=MONS_FROG && type<=MONS_FROG3 && player.var[VAR_DOLL+2]==0)
					{
						FireBullet(x,y,ITM_FROGDOLL,BLT_ITEM);
					}
					else if(type>=MONS_GHOST && type<=MONS_GHOST3 && player.var[VAR_DOLL+3]==0)
					{
						FireBullet(x,y,ITM_GHOSTDOLL,BLT_ITEM);
					}
					else if(type>=MONS_MUMMY && type<=MONS_MUMMY4 && player.var[VAR_DOLL+4]==0)
					{
						FireBullet(x,y,ITM_MUMMYDOLL,BLT_ITEM);
					}
					else if(type>=MONS_SWAMPDOG && type<=MONS_SWAMPDOG3 && player.var[VAR_DOLL+5]==0)
					{
						FireBullet(x,y,ITM_SWAMPDOLL,BLT_ITEM);
					}
					else if(type>=MONS_VAMPIRE && type<=MONS_VAMPIRE4 && player.var[VAR_DOLL+6]==0)
					{
						FireBullet(x,y,ITM_VAMPDOLL,BLT_ITEM);
					}
					else if(type>=MONS_WOLFMAN && type<=MONS_WOLFMAN3 && player.var[VAR_DOLL+7]==0)
					{
						FireBullet(x,y,ITM_WOLFDOLL,BLT_ITEM);
					}
				}


				if(player.difficulty==DIFF_BEGINNER)
					v=(byte)(combo+4);
				else if(player.difficulty==DIFF_NORMAL)
					v=(byte)(combo/2+3);
				else if(player.difficulty==DIFF_CHALLENGE || player.difficulty==DIFF_MAD)
					v=(byte)(combo/4+1);
				else if(player.difficulty==DIFF_LOONY)
					v=(byte)(combo/4);
				else if(player.difficulty==DIFF_HARD)
					v=(byte)(combo/2+1);

				for(combo=0;combo<v;combo++)
				{
					// drop random item
					i=Random(100);

					if(i<45)		// 45/100
					{
						// no item
					}
					else if(i<55)	// 10/100
					{
						FireBullet(x,y,ITM_TINYHEART,BLT_ITEM);
					}
					else if(i<90)	// 35/100
						FireBullet(x,y,ITM_LITTLEGEM,BLT_ITEM);
					else if(i<91)	// 1/100
					{
						FireBullet(x,y,ITM_BIGHEART,BLT_ITEM);
					}
					else if(i<93)	// 2/100
						FireBullet(x,y,ITM_BIGGEM,BLT_ITEM);
					else			// 7/100
					{
						// drop weapon
						j=100;
						while(j-->0)
						{
							i=Random(7);
							if(player.var[i+VAR_WEAPON])
							{
								FireBullet(x,y,ITM_WBOMB+i,BLT_ITEM);
								break;
							}
						}
					}
				}
			}
		}
	}
	if(type!=player.monsType && type<MONS_SUMBOMBIE)
	{
		lifeGuy=this;
		if(type!=MONS_FRANKENJ && type!=MONS_BIGGHOST && type!=MONS_ELDER &&
			type!=MONS_ELDER2 && type!=MONS_ELDER3 && type!=MONS_ELDER4 &&
			type!=MONS_LARRY && type!=MONS_BONKULA && type!=MONS_EVILIZER &&
			type!=MONS_STICKWITCH && type!=MONS_HUMANLARRY && type!=MONS_ELDER5 &&
			type!=MONS_GHASTLY && type!=MONS_MINIFRANK && type!=MONS_MECHABONK)
			ShowEnemyLife(MonsterName(type),formerHP*128/MonsterHP(type),hp*128/MonsterHP(type),(byte)(hp>0));

		if(player.worldNum==WORLD_SLINGSHOT && hp==0)
		{
			j=1;
			if(RangeToTarget(this,goodguy)<FIXAMT*72)
			{
				j=3;
				NewMessage("Close Call!",60);
			}
			j*=player.combo;
			for(i=0;i<j;i++)
			{
				if(player.money<player.maxMoney)
				{
					player.money++;
					player.gemsGotten++;
					BadgeCheck(BE_GEMSUP,0,curMap);
				}
			}
		}
		if((player.cheatsOn&PC_PLAYBONK) && hp==0)
		{
			// reset blood clock
			player.xtraVar=30*5;
			HealGoodguy(1);
		}
		if(tag!=255 && (player.monsType==MONS_PLYRWITCH)) // resurrected guys do not improve spells
		{
			if(bulletHittingType==BLT_WITCHBLAST)
			{
				PowerupSpell(SPL_BLAST,4);
			}
			if(bulletHittingType==BLT_WITCHBUZZSAW)
			{
				PowerupSpell(SPL_BUZZSAW,1);
			}
			if(bulletHittingType==BLT_WITCHSHOCK)
			{
				PowerupSpell(SPL_SHOCK,1);
			}
			if(bulletHittingType==BLT_WITCHICE)
			{
				PowerupSpell(SPL_ICE,1);
			}
			if(bulletHittingType==BLT_WITCHGAS)
			{
				PowerupSpell(SPL_PLAGUE,1);
			}
			if(bulletHittingType==BLT_GOODBOOM || bulletHittingType==BLT_GOODBOOM2)
			{
				PowerupSpell(SPL_KABOOM,3);
			}
		}
	}
	else if(type==player.monsType)
	{
		byte invinc[]={30*3,30*2,30,15,0,45};
		SetPlayerHP(hp);
		player.invinc=invinc[player.difficulty];

		if(player.cheatsOn&PC_TERROR)
			player.invinc/=2;	// half as much invincibility

		if(opt.cheats[CH_VINTAGE])
			NewMessage("Ouch!",60);

		if(opt.cheats[CH_THORNS])
		{
			for(i=0;i<8;i++)
				FireBullet(x,y,(byte)Random(256),BLT_CACTUS2);
		}
	}
}

//-----------------------------------------------------------------------

void InitGuys(int max)
{
	int i;

	maxGuys=max;

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
	int i,j,preLife;
	static byte filmflip;
	byte numRuns,numRenders;

	UpdateMonsterTypes();

	if(player.cheatsOn&PC_PLAYSUMMON)
		UpdateSummons();
	if(opt.cheats[CH_RESPAWN])
		RespawnGuy(map);
	if(opt.cheats[CH_FROGWPN])
		Frogs(map,world);

	if(player.comboClock)
		player.comboClock--;
	player.timeToFinish++;

	checkActive++;
	if(checkActive>5)
	{
		if(opt.cheats[CH_RADAR])
			CleanRadar();
		checkActive=0;
	}

	if(opt.cheats[CH_VINTAGE])
	{
		filmflip++;
		if(filmflip==2)
		{
			filmflip=0;
			MakeNormalSound(SND_FILM);
			if(Random(60)==0)
			{
				filmflip=1;
				FilmWiggle((byte)Random(20));
			}
		}
	}

	for(i=0;i<maxGuys;i++)
	{
		if(checkActive==0 && opt.cheats[CH_REGEN] && guys[i]->type && guys[i]->hp && NotFriendly(guys[i]->type))
		{
			preLife=guys[i]->hp;
			if(preLife<MonsterHP(guys[i]->type))
			{
				j=MonsterHP(guys[i]->type)/60;
				if(j<1)
					j=1;
				guys[i]->hp+=j;
				if(guys[i]->hp>MonsterHP(guys[i]->type))
					guys[i]->hp=MonsterHP(guys[i]->type);
				else if(lifeGuy==guys[i])
				{
					if(guys[i]->type!=MONS_FRANKENJ && guys[i]->type!=MONS_BIGGHOST && guys[i]->type!=MONS_ELDER &&
						guys[i]->type!=MONS_ELDER2 && guys[i]->type!=MONS_ELDER3 && guys[i]->type!=MONS_ELDER4 &&
						guys[i]->type!=MONS_LARRY && guys[i]->type!=MONS_BONKULA && guys[i]->type!=MONS_EVILIZER &&
						guys[i]->type!=MONS_STICKWITCH && guys[i]->type!=MONS_HUMANLARRY)
						ShowEnemyLife(MonsterName(guys[i]->type),(preLife)*128/MonsterHP(guys[i]->type),guys[i]->hp*128/MonsterHP(guys[i]->type),1);
				}
			}
		}
		if(guys[i]->type!=MONS_NONE && guys[i]->IsActive(map))
		{
			guys[i]->bright=map->map[guys[i]->mapx+guys[i]->mapy*map->width].templight;

			numRuns=1;
			numRenders=1;

			// vintage runs super fast
			if(opt.cheats[CH_VINTAGE])
			{
				numRuns++;
			}
			if(opt.cheats[CH_QUICK] && guys[i]->type==player.monsType)
			{
				numRuns++;
				numRenders++;
			}

			if(guys[i]->type==player.monsType && player.speed>0)
			{
				numRuns++;
				numRenders++;
				player.speed--;
			}
			else
			{
				if(guys[i]->type==MONS_WOLFMAN2 && guys[i]->mind2>0)
				{
					// wolfman speed-up howl
					numRuns++;
					numRenders++;
					guys[i]->mind2--;
				}

				// double update if in terror mode!!!  Unless you're friendly!
				if((player.cheatsOn&PC_TERROR) && NotFriendly(guys[i]->type))
					numRuns++;

				if(opt.cheats[CH_SLOMO] && (checkActive&1) && NotFriendly(guys[i]->type))
				{
					numRuns--;
				}
			}
			if(player.difficulty==DIFF_BEGINNER && checkActive==0 && guys[i]->hp>0 && NotFriendly(guys[i]->type))
			// in beginner mode, enemies move 20% slower, so don't update them on 1 of the 5 checkActives (it goes from 0 to 4)
			{
				numRuns--;
			}
			if(player.difficulty==DIFF_LOONY && (checkActive==0 || checkActive==2) && guys[i]->hp>0 && NotFriendly(guys[i]->type))
			// in loony mode, enemies move 40% faster, so update them extra on 2 of the 5 checkActives (it goes from 0 to 4)
			{
				numRuns++;
			}

			if(player.monsType==MONS_PLYRSUMMON && guys[i]->type>=MONS_SUMBOMBIE && guys[i]->mind3>0)
			{
				numRuns++;
				guys[i]->mind3--;	// sped up by Hot Dog flame
			}
			for(j=0;j<numRuns;j++)
			{
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
				if(numRenders>1)
				{
					guys[i]->Render(1);
					numRenders--;
				}
			}
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
			guys[i]->seq=ANIM_IDLE;
			guys[i]->frm=0;
			guys[i]->frmTimer=0;
			guys[i]->frmAdvance=128;
			guys[i]->hp=MonsterHP(type);
			guys[i]->facing=2;
			guys[i]->ouch=0;
			guys[i]->dx=0;
			guys[i]->dy=0;
			guys[i]->dz=0;
			guys[i]->bright=0;
			guys[i]->mind=0;
			guys[i]->mind1=0;
			guys[i]->mind2=0;
			guys[i]->mind3=0;
			guys[i]->reload=0;
			guys[i]->parent=NULL;
			guys[i]->CalculateRect();
			guys[i]->ID=i;
			guys[i]->mapx=(guys[i]->x>>FIXSHIFT)/TILE_WIDTH;
			guys[i]->mapy=(guys[i]->y>>FIXSHIFT)/TILE_HEIGHT;
			guys[i]->ice=0;
			guys[i]->iceClock=0;
			guys[i]->active=1;
			guys[i]->tag=0;

			if(type==MONS_HUMANLARRY)
				guys[i]->mind=30*8+2;

			return guys[i];
		}
	return NULL;
}

static_assert(offsetof(Guy, parent) == 64, "save compatibility broken; adjust this assertion if you are sure");
static_assert(offsetof(Guy, hp) + 28 <= sizeof(Guy), "save compatibility broken; adjust this assertion if you are sure");

void SaveGuys(FILE *f)
{
	int i,num;

	num=0;

	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type!=MONS_NONE && guys[i]->type!=player.monsType)
		{
			num++;
		}
	}

	fwrite(&num,sizeof(int),1,f);
	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type!=MONS_NONE && guys[i]->type!=player.monsType)
		{
			// Always write 32-bit-compatible saves.
			fwrite(guys[i], 64, 1, f);
			// Write 0 for Guy* parent. The only monster in LL1 that uses parent is Polterguy and you can't save on his map.
			// If you ever need to save this, do like LL2 does and save the Guy's ID instead.
			dword zero = 0;
			fwrite(&zero, 4, 1, f);
			fwrite(&guys[i]->hp, 28, 1, f);
		}
	}
}

void LoadGuys(FILE *f)
{
	int i,num;

	ExitGuys();
	InitGuys(MAX_MAPMONS);

	fread(&num,sizeof(int),1,f);

	bool saveIs64Bit = false;

	for(i=0;i<num;i++)
	{
		fread(guys[i], 64, 1, f);
		fseek(f, 4, SEEK_CUR);  // Skip 4 bytes of Guy* parent.
		guys[i]->parent = nullptr;  // Set parent to null.
		fread(&guys[i]->hp, 28, 1, f);

		// The above loads 32-bit saves, but there may be 64-bit save files
		// floating about, since the incompatibility was not discovered right
		// away. Use an incorrect rectx as a signal that the save is in the
		// bad format and load it anyways. It will be re-saved correctly.
		if (i == 0)
		{
			int oldRectX = guys[i]->rectx;
			guys[i]->CalculateRect();
			if (oldRectX != guys[i]->rectx)
			{
				saveIs64Bit = true;
				printf("Warning: assuming 64-bit save based on rectx %d != %d\n", oldRectX, guys[i]->rectx);
			}
		}
		if (saveIs64Bit)
		{
			fseek(f, -28 + 4, SEEK_CUR);  // Seek back 28, then +4 for the second half of Guy* parent.
			fread(&guys[i]->hp, 28, 1, f);  // Read the second half of Guy again.
			fseek(f, 4, SEEK_CUR);  // Skip 4 bytes of padding.
		}

		if(guys[i]->type==MONS_HELPERBAT)
			guys[i]->type=MONS_NONE;
	}
	player.fireFlags&=(~FF_HELPERHERE);

	if(player.monsType==MONS_PLYRSUMMON)
	{
		InitSummons();
		for(i=0;i<maxGuys;i++)
		{
			if(guys[i]->type>=MONS_SUMBOMBIE)
			{
				AddToSummoned(guys[i]);
			}
		}
	}
}

void CameraOnPlayer(byte sum)
{
	int i;
	Guy *g;

	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type==player.monsType)	// special case: loony is the goodguy
		{
			goodguy=guys[i];
			if(player.destx!=0)
			{
				guys[i]->x=(player.destx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
				guys[i]->y=(player.desty*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
				player.destx=0;
				player.desty=0;
			}
			PutCamera(guys[i]->x,guys[i]->y);
			guys[i]->hp=player.hearts;

			if(player.monsType==MONS_PLYRSUMMON && sum)
			{
				for(i=0;i<maxGuys;i++)
					if(guys[i]->type==player.monsType)
					{
						DeploySummons(guys[i]);
					}
			}
			return;
		}
	}
	// if you got here, there was no loony
	g=AddGuy((player.destx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(player.desty*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,
		0,player.monsType);
	goodguy=g;
	player.destx=0;
	player.desty=0;
	PutCamera(g->x,g->y);
	g->hp=player.hearts;

	if(player.monsType==MONS_PLYRSUMMON && sum)
	{
		for(i=0;i<maxGuys;i++)
			if(guys[i]->type==player.monsType)
			{
				DeploySummons(guys[i]);
			}
	}
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

void Frogs(Map *map,world_t *world)
{
	Guy *g;
	static byte tick;
	int x,y;

	if(player.worldNum==WORLD_BOWLING)
		return;

	tick++;

	if(tick<255)
		return;

	// left
	GetCamera(&x,&y);
	x=x-320-80-Random(320);
	y=y-240+Random(480);
	g=AddGuy(x*FIXAMT,y*FIXAMT,0,MONS_FROG);
	if(g && (!g->CanWalk(g->x,g->y,map,world)))
		g->type=MONS_NONE;	// vanish if this spot is occupied
	// right
	GetCamera(&x,&y);
	x=x+320+80+Random(320);
	y=y-240+Random(480);
	g=AddGuy(x*FIXAMT,y*FIXAMT,0,MONS_FROG);
	if(g && (!g->CanWalk(g->x,g->y,map,world)))
		g->type=MONS_NONE;	// vanish if this spot is occupied
	// top
	GetCamera(&x,&y);
	x=x-320+Random(640);
	y=y-240-80-Random(240);
	g=AddGuy(x*FIXAMT,y*FIXAMT,0,MONS_FROG);
	if(g && (!g->CanWalk(g->x,g->y,map,world)))
		g->type=MONS_NONE;	// vanish if this spot is occupied
	// bottom
	GetCamera(&x,&y);
	x=x-320+Random(640);
	y=y+240+80+Random(240);
	g=AddGuy(x*FIXAMT,y*FIXAMT,0,MONS_FROG);
	if(g && (!g->CanWalk(g->x,g->y,map,world)))
		g->type=MONS_NONE;	// vanish if this spot is occupied
}

void RespawnGuy(Map *map)
{
	byte noRespawn[]={
		MONS_NONE,MONS_LOONY,MONS_LOONYTOAD,MONS_FRANKENJ,MONS_BIGGHOST,
		MONS_LARRY,MONS_HUMANLARRY,MONS_HELPERBAT,MONS_VILLAGER,MONS_VILLAGER2,
		MONS_VILLAGER3,MONS_VILLAGER4,MONS_VILLAGER5,MONS_VILLAGER6,
		MONS_ELDER,MONS_ELDER2,MONS_ELDER3,MONS_ELDER4,MONS_TUMBLEROCK,
		MONS_TUMBLEROCK2,MONS_TUMBLEROCK3,MONS_TUMBLEROCK4,MONS_LIGHTBALL,
		MONS_LIGHTBALL2,MONS_LIGHTBALL3,MONS_LIGHTBALL4,MONS_LIGHTBALL5,
		MONS_EVILTREE,MONS_EVILTREE2,MONS_BHOMATIC,MONS_BUBBLE,MONS_JUNK,
		MONS_JUNK2,MONS_JUNK3,MONS_PLYRBONKULA,MONS_PLYRSWAMPDOG,MONS_PLYRWITCH,
		MONS_BONKULA,MONS_EVILIZER,MONS_EVILPUMP,MONS_PUMPKIN,MONS_PUMPKIN2,MONS_PUMPKIN3,
		MONS_ZOMBIE3,MONS_PLYRWOLF,MONS_LOONY2,
		MONS_PLYRSUMMON,MONS_PLYRTHIEF,MONS_SUMBOMBIE,MONS_SUMGHOST,MONS_SUMWOLF,MONS_SUMBAT,
		MONS_SUMMUMMY,MONS_SUMFROG,MONS_SUMDOG,255,
	};

	static byte tick=0;
	int i,tries;
	byte ok,t,tag;
	Guy *g;

	if(player.worldNum!=WORLD_NORMAL && player.worldNum!=WORLD_REMIX && player.worldNum!=WORLD_RANDOMIZER)
	{
		return;
	}
	tick++;
	if(tick<100)
		return;
	tick=0;

	tries=1000;
	while(tries--)
	{
		i=Random(MAX_MAPMONS);

		t=0;
		ok=1;
		while(noRespawn[t]!=255)
		{
			if(map->badguy[i].type==noRespawn[t])
			{
				ok=0;
				break;
			}
			t++;
		}
		if(!ok)
			continue;

		tag=map->map[map->badguy[i].x+map->badguy[i].y*map->width].tag;
		g=AddGuy((map->badguy[i].x*TILE_WIDTH+(TILE_WIDTH/2))<<FIXSHIFT,
			   (map->badguy[i].y*TILE_HEIGHT+(TILE_HEIGHT/2))<<FIXSHIFT,
			   0,map->badguy[i].type);
		if(g)
		{
			// since collisions aren't checked against inactive guys on map 0,
			// this increase and decrease of levelnum tricks it to check them for this
			player.levelNum++;
			if(!g->CanWalk(g->x,g->y,map,&curWorld))
				g->type=MONS_NONE;
			else
			{
				g->tag=tag;
				tries=0;
			}
			player.levelNum--;
		}
	}
}

void AddMapGuys(Map *map)
{
	int i;
	byte ok,t,tag;
	Guy *g;

	fakeGuy.mind=0;
	if(player.worldNum==WORLD_SURVIVAL)
	{
		AddSurvivalMapGuys(map);
		return;
	}
	for(i=0;i<MAX_MAPMONS;i++)
	{
		if(map->badguy[i].type)
		{
			tag=map->map[map->badguy[i].x+map->badguy[i].y*map->width].tag;
			ok=1;
			if(player.worldNum==WORLD_NORMAL || player.worldNum==WORLD_REMIX || player.worldNum == WORLD_RANDOMIZER)
			{
				if((map->badguy[i].type==MONS_EVILTREE || map->badguy[i].type==MONS_EVILTREE3) && player.var[VAR_QUESTDONE+QUEST_TREES] &&
					player.levelNum==0)
				{
					// if tree killing quest is complete, then all evil trees are
					// replaced with dead evil tree objects, in the main map.
					map->map[map->badguy[i].x+map->badguy[i].y*map->width].item=ITM_TREE2;
					ok=0;
				}
				else if(player.levelNum==0 && (map->badguy[i].type==MONS_PUMPKIN || map->badguy[i].type==MONS_PUMPKIN2 ||
					map->badguy[i].type==MONS_PUMPKIN3 || map->badguy[i].type==MONS_PUMPKIN4)
					&& (player.var[VAR_QUESTDONE+QUEST_CROPS] || player.var[VAR_QUESTDONE+QUEST_ZOMBIES]==0))
				{
					// if you haven't yet killed the zombies, then the pumpkins don't exist
					// if you have killed the pumpkins, then the pumpkins DEFINITELY don't exist!
					ok=0;
				}
				else if((map->badguy[i].type>=MONS_ZOMBIE && map->badguy[i].type<=MONS_ZOMBIE3) || (map->badguy[i].type==MONS_ZOMBIE4))
				{
					if(!player.var[VAR_QUESTDONE+QUEST_ZOMBIES])
					{
						// if you are in a cleansed crypt, do not put in zombies
						if(player.levelNum>=10 && player.levelNum<=12 &&
							player.var[VAR_ZOMBIE+player.levelNum-10])
							ok=0;
					}
					else
						ok=0;
				}
				else if(map->badguy[i].type>=MONS_VILLAGER && map->badguy[i].type<=MONS_VILLAGER6)
				{
					// any Zombiton people should not be added (except the mayor, or the farmer -
					// as long as he's not in Benny's Cocktails) if the zombie stomp quest is not
					// done
					t=map->map[map->badguy[i].x+map->badguy[i].y*map->width].tag;
					if(!player.var[VAR_QUESTDONE+QUEST_ZOMBIES] && (
						(t==10 && player.levelNum!=0) || t==11 || t==12 || t==17))
						ok=0;

				}
				else if(map->badguy[i].type==MONS_FRANKENJ)
				{
					if(player.var[VAR_QUESTDONE+QUEST_FRANK])
						ok=0;
				}
				else if(map->badguy[i].type==MONS_BIGGHOST)
				{
					if(player.var[VAR_QUESTDONE+QUEST_TOWER])
						ok=0;
				}
				else if(map->badguy[i].type==MONS_ELDER)
				{
					if(player.var[VAR_DEADELDER])
						ok=0;
				}
				else if(map->badguy[i].type==MONS_ELDER2)
				{
					if(player.var[VAR_DEADELDER+1])
						ok=0;
				}
				else if(map->badguy[i].type==MONS_ELDER3)
				{
					if(player.var[VAR_DEADELDER+2])
						ok=0;
				}
				else if(map->badguy[i].type==MONS_ELDER4)
				{
					if(player.var[VAR_DEADELDER+3])
						ok=0;
				}
				else if(map->badguy[i].type==MONS_BONKULA || map->badguy[i].type==MONS_MECHABONK)
				{
					if(player.var[VAR_DEADBONK])
						ok=0;
				}
			}
			if(map->badguy[i].type==MONS_LOONY && !editing)
				map->badguy[i].type=player.monsType;
			if(ok || editing)
			{
				if(!SurpriseMonster(map->badguy[i].type,tag,map->badguy[i].x,map->badguy[i].y,map,&curWorld))
				{
					g=AddGuy((map->badguy[i].x*TILE_WIDTH+(TILE_WIDTH/2))<<FIXSHIFT,
						   (map->badguy[i].y*TILE_HEIGHT+(TILE_HEIGHT/2))<<FIXSHIFT,
						   0,map->badguy[i].type);
					if(g)
					{
						g->tag=tag;
						if(map->badguy[i].type>=MONS_TUMBLEROCK && map->badguy[i].type<=MONS_TUMBLEROCK4)
						{
							g->facing=g->tag;
						}

					}
				}
			}
		}
	}
}

void SurprisePlace(byte type,byte tag,int x,int y,Map *map,world_t *world)
{
	Guy *g;
	int tx,ty;
	int tries;

	tries=50;

	while(tries--)
	{
		tx=x*TILE_WIDTH+TILE_WIDTH/2;
		ty=y*TILE_HEIGHT+TILE_HEIGHT/2;
		tx+=-TILE_WIDTH*2+Random(TILE_WIDTH*4);
		ty+=-TILE_HEIGHT*2+Random(TILE_HEIGHT*4);
		g=AddGuy(tx<<FIXSHIFT,ty<<FIXSHIFT,0,type);
		if(g && g->CanWalk(g->x,g->y,map,world))
		{
			g->tag=tag;
			return;
		}
		else if(g)
			g->type=MONS_NONE;
	}
}

byte SurpriseMonster(byte origType,byte tag,int x,int y,Map *map,world_t *world)
{
	byte badTable[]={
		MONS_NONE,MONS_LOONY,MONS_LOONYTOAD,MONS_BIGGHOST,
		MONS_LARRY,MONS_HUMANLARRY,MONS_HELPERBAT,MONS_VILLAGER,MONS_VILLAGER2,
		MONS_VILLAGER3,MONS_VILLAGER4,MONS_VILLAGER5,MONS_VILLAGER6,
		MONS_ELDER2,MONS_ELDER3,MONS_ELDER4,MONS_TUMBLEROCK,
		MONS_TUMBLEROCK2,MONS_TUMBLEROCK3,MONS_TUMBLEROCK4,MONS_LIGHTBALL,
		MONS_LIGHTBALL2,MONS_LIGHTBALL3,MONS_LIGHTBALL4,MONS_LIGHTBALL5,
		MONS_EVILTREE,MONS_EVILTREE2,MONS_BUBBLE,MONS_JUNK,
		MONS_JUNK2,MONS_JUNK3,MONS_PLYRBONKULA,MONS_PLYRSWAMPDOG,MONS_PLYRWITCH,
		MONS_BONKULA,MONS_EVILIZER,MONS_PLYRWOLF,MONS_LOONY2,
		MONS_PLYRSUMMON,MONS_PLYRTHIEF,MONS_SUMBOMBIE,MONS_SUMGHOST,MONS_SUMWOLF,MONS_SUMBAT,
		MONS_SUMMUMMY,MONS_SUMFROG,MONS_SUMDOG,255,
	};
	int amount;
	byte myType;
	int i;

	if(opt.cheats[CH_SURPRISE]==0 || editing || origType==player.monsType)
		return 0;

	// can't surprise some monsters
	i=0;
	while(1)
	{
		if(origType==badTable[i])
			return 0;
		i++;
		if(badTable[i]==255)
			break;
	}

	amount=MonsterLevel(origType);

	// if the monster is a very tough one, possibly replace it with a group
	if(amount>=25 && Random(3)==1)
	{
		while(amount>0)
		{
			myType=(byte)Random(NUM_MONSTERS);

			if(MonsterLevel(myType)>amount)
				continue;

			// make sure it is an ok monster
			i=0;
			while(1)
			{
				if(myType==badTable[i])
				{
					myType=0;
					break;
				}
				i++;
				if(badTable[i]==255)
					break;
			}
			if(myType==0)
				continue;

			amount-=MonsterLevel(myType);
			SurprisePlace(myType,tag,x,y,map,world);
		}
	}
	// otherwise, just a one-for-one swap
	else
	{
		while(amount>0)
		{
			myType=(byte)Random(NUM_MONSTERS);

			i=amount*3/2;
			if(i<amount+2)
				i=amount+2;

			if(MonsterLevel(myType)>i || MonsterLevel(myType)<amount/2)
				continue;

			// make sure it is an ok monster
			i=0;
			while(1)
			{
				if(myType==badTable[i])
				{
					myType=0;
					break;
				}
				i++;
				if(badTable[i]==255)
					break;
			}
			if(myType==0)
				continue;

			amount=0;
			SurprisePlace(myType,tag,x,y,map,world);
		}
	}

	return 1;
}

byte CheckHit(byte size,int xx,int yy,Guy *him)
{
	int x2,y2;

	// can't hit the unhittable
	if((MonsterFlags(him->type)&MF_NOHIT))
		return 0;

	xx-=size;
	yy-=size;

	x2=him->x>>FIXSHIFT;
	y2=him->y>>FIXSHIFT;

	if((x2+him->rectx2)>=xx && (y2+him->recty2)>=yy && (x2+him->rectx)<=(xx+size*2) &&
		(y2+him->recty)<=(yy+size*2))
	{
		// and ghosts are unhittable until you have the potion
		if(him->type>=MONS_GHOST && him->type<=MONS_GHOST3 && player.var[VAR_POTION]==0)
		{
			if(player.var[VAR_QUESTASSIGN+QUEST_GHOST]==0)
				BeginChatting(250);
			return 0;
		}
		//DrawDebugBox(xx,yy,xx+size*2,yy+size*2);
		return 1;
	}

	return 0;
}

int FindVictim(int x,int y,byte size,int dx,int dy,byte damage,Map *map,world_t *world)
{
	int i;

	if(player.worldNum!=WORLD_BOWLING)
		TileHitCheck(x,y,size,map);

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type && guys[i]->hp && NotFriendly(guys[i]->type))
		{
			if(CheckHit(size,x,y,guys[i]))
			{
				guys[i]->GetShot(dx,dy,damage,map,world);
				return guys[i]->ID+1;
			}
		}

	return 0;
}

int FreezeVictim(int x,int y,byte size,int dx,int dy,int target,byte damage,Map *map,world_t *world)
{
	int i,j;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type && guys[i]->hp && NotFriendly(guys[i]->type) && guys[i]->ID!=target-1)
		{
			if(CheckHit(size,x,y,guys[i]))
			{
				MakeSound(SND_FREEZE,x*FIXAMT,y*FIXAMT,SND_CUTOFF,1200);
				if(guys[i]->ice<60000)
				{
					j=guys[i]->ice;
					if(player.monsType==MONS_PLYRSUMMON)
						guys[i]->ice+=damage-MonsterHP(guys[i]->type);
					else
						guys[i]->ice+=damage-(MonsterHP(guys[i]->type)*2);
					if(guys[i]->ice<j)
						guys[i]->ice=j;
					if(player.monsType!=MONS_PLYRSUMMON && player.wpnLevel>0)	// do damage too, and instafreeze
					{
						if(MonsterHP(guys[i]->type)<player.wpnLevel*150)
							guys[i]->ice=player.wpnLevel*400;
						guys[i]->GetShot(dx,dy,player.wpnLevel*2,map,world);
					}
				}
				return guys[i]->ID+1;
			}
		}

	return 0;
}

int FreezeVictimWitch(int x,int y,byte size,int dx,int dy,int target,byte damage,Map *map,world_t *world)
{
	int i,j;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type && guys[i]->hp && (guys[i]->type!=player.monsType) && guys[i]->ID!=target-1)
		{
			if(CheckHit(size,x,y,guys[i]))
			{
				MakeSound(SND_FREEZE,x*FIXAMT,y*FIXAMT,SND_CUTOFF,1200);
				j=guys[i]->ice;
				guys[i]->ice+=damage-(MonsterHP(guys[i]->type)*2);
				if(guys[i]->ice<j)
					guys[i]->ice=j;
				guys[i]->GetShot(dx,dy,SpellLevel(SPL_ICE)/4+1,map,world);
				return guys[i]->ID+1;
			}
		}

	return 0;
}

byte FindNewVictim(int x,int y,int *target,byte size,int dx,int dy,byte damage,Map *map,world_t *world)
{
	int i;

	TileHitCheck(x,y,size,map);

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
	if(player.monsType==MONS_PLYRSUMMON)
	{
		int i;

		for(i=0;i<maxGuys;i++)
			if(guys[i]->type && guys[i]->hp && !NotFriendly(guys[i]->type))
			{
				if(CheckHit(size,x,y,guys[i]))
				{
					guys[i]->GetShot(dx,dy,damage,map,world);
					return 1+(guys[i]->type==player.monsType);
				}
			}
	}
	else
	{
		if(!goodguy || (!goodguy->hp))
			return 0;
		if(player.invinc)
			return 0;

		if(CheckHit(size,x,y,goodguy))
		{
			goodguy->GetShot(dx,dy,damage,map,world);
			return 2;
		}
	}

	return 0;
}

byte FindSummonToHelp(int x,int y,byte size,byte how,Map *map,world_t *world)
{
	int lifeFactor;

	if(player.monsType==MONS_PLYRSUMMON)
	{
		int i;

		for(i=0;i<maxGuys;i++)
			if(guys[i]->type && guys[i]->hp && guys[i]->type>=MONS_SUMBOMBIE)
			{
				if(CheckHit(size,x,y,guys[i]))
				{
					if(how==1)	// speed guys up
					{
						guys[i]->mind3=(player.firePower+1)*15;
					}
					if(how==2)	// heal guy
					{
						lifeFactor=32+224*player.xtraByte/20;
						if(guys[i]->hp>=MonsterHP(guys[i]->type)*lifeFactor/256)
							continue;	// don't heal the healthy
						guys[i]->hp+=3;
						if(guys[i]->hp>MonsterHP(guys[i]->type)*lifeFactor/256)
							guys[i]->hp=MonsterHP(guys[i]->type)*lifeFactor/256;
						return 1;
					}
				}
			}
	}

	return 0;
}

byte ItemHitPlayer(int x,int y,byte size,int dx,int dy,byte type,Map *map,world_t *world)
{
	int i,sizeFactor;

	if(!goodguy)
		return 0;

	if(CheckHit(size,x,y,goodguy))
	{
		PlayerGetItem(type,x,y);
		return 1;
	}
	if(player.monsType==MONS_PLYRSUMMON)
	{
		sizeFactor=32+224*player.firePower/10;
		for(i=0;i<maxGuys;i++)
		{
			if(guys[i]->type==MONS_SUMBAT && CheckHit(size*sizeFactor/256,x,y,guys[i]))
			{
				PlayerGetItem(type,x,y);
				return 1;
			}
		}
	}

	return 0;
}

// this doesn't quit when it finds one victim, it keeps going
byte FindVictims(int x,int y,byte size,int dx,int dy,byte damage,Map *map,world_t *world)
{
	int i;
	byte result=0;

	TileHitCheck(x,y,size,map);

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type && guys[i]->hp && NotFriendly(guys[i]->type))
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

	TileHitCheck(x,y,size,map);

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type && guys[i]->hp && NotFriendly(guys[i]->type) && guys[i]->ouch==0)
		{
			if(CheckHit(size,x,y,guys[i]))
			{
				guys[i]->GetShot(dx,dy,damage,map,world);
				result=1;
			}
		}

	return result;
}

void TileHitCheck(int x,int y,byte size,Map *map)
{
	int mapx1,mapx2,mapy1,mapy2;
	int i,j;

	mapx1=(x-size)/TILE_WIDTH;
	mapy1=(y-size)/TILE_HEIGHT;
	mapx2=(x+size)/TILE_WIDTH;
	mapy2=(y+size)/TILE_HEIGHT;

	for(i=mapx1;i<=mapx2;i++)
	{
		for(j=mapy1;j<=mapy2;j++)
		{
			if(i>=0 && j>=0 && i<map->width && j<map->height &&
				ItemFlags(map->map[i+j*map->width].item)&IF_DESTROY)
			{
				switch(map->map[i+j*map->width].item)
				{
					case ITM_GRAVE:
					case ITM_GRAVE2:
						MakeSound(SND_GRAVESMASH,(i*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
							(j*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,SND_CUTOFF,600);
						ExplodeParticles2(PART_SNOW2,(i*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
												   (j*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,5*FIXAMT,20,8);
						map->map[i+j*map->width].item=ITM_NONE;
						BadgeCheck(BE_DESTROY,ITM_GRAVE,map);
						break;
					case ITM_CATTAIL:
						MakeSound(SND_CATTAILSMASH,(i*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
							(j*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,SND_CUTOFF,600);
						ExplodeParticles2(PART_SLIME,(i*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
												   (j*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,5*FIXAMT,10,4);
						map->map[i+j*map->width].item=ITM_NONE;
						BadgeCheck(BE_DESTROY,ITM_CATTAIL,map);
						break;
				}
			}
		}

	}
}

// this is a special victim finder for the slingshot mode
byte FindSlingVictim(int x,int y,byte size,byte damage,Map *map,world_t *world)
{
	int i;
	byte result=0;
	int x1,x2,y1,y2;

	TileHitCheck(x,y,size,map);

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type && guys[i]->hp && (guys[i]->type!=player.monsType))
		{
			x1=guys[i]->x/FIXAMT-GetMonsterSprite(guys[i]->type,guys[i]->seq,guys[i]->frm,guys[i]->facing)->ofsx;
			y1=guys[i]->y/FIXAMT-GetMonsterSprite(guys[i]->type,guys[i]->seq,guys[i]->frm,guys[i]->facing)->ofsy;
			x2=x1+GetMonsterSprite(guys[i]->type,guys[i]->seq,guys[i]->frm,guys[i]->facing)->width;
			y2=y1+GetMonsterSprite(guys[i]->type,guys[i]->seq,guys[i]->frm,guys[i]->facing)->height;
			if((x-size)<=x2 && (y-size)<=y2 && (x+size)>=x1 && (y+size)>=y1)
			{
				guys[i]->GetShot(0,0,damage,map,world);
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

	maxRange=(320+240)-100+player.wpnLevel*50;
	bestguy=65535;
	bestRange=maxRange;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type && guys[i]->hp && guys[i]->active && NotFriendly(guys[i]->type) &&
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

word LockOnEvilNotYou(int x,int y,word target)
{
	int i;
	int bestRange,range;
	word bestguy;
	int maxRange;

	maxRange=(320+240)-100+player.wpnLevel*50;
	bestguy=65535;
	bestRange=maxRange;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type && i!=(target-1) && guys[i]->hp && guys[i]->active && NotFriendly(guys[i]->type) &&
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

word LockOnSummon(int x,int y)
{
	int i;
	int bestRange,range;
	word bestguy;
	int maxRange;

	maxRange=400;
	bestguy=65535;
	bestRange=maxRange;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type && guys[i]->hp && guys[i]->active && guys[i]->type>=MONS_SUMBOMBIE &&
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
		if(guys[i]->type && guys[i]->hp && NotFriendly(guys[i]->type) && guys[i]->active &&
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
		if(guys[i]->type && guys[i]->hp && guys[i]->active && NotFriendly(guys[i]->type) &&
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
	{
		byte type = guys[i]->type;
		if (!(
			type == MONS_NONE ||
			// player
			type == player.monsType ||
			// allies
			type == MONS_HELPERBAT ||
			(type >= MONS_VILLAGER && type <= MONS_VILLAGER6) ||
			(type >= MONS_SUMBOMBIE && type <= MONS_SUMDOG) ||
			// invincible guys
			type == MONS_EVILTREE2 ||
			// used to also exclude vincible lightball, but that's irrelevant for existing checks
			type == MONS_LIGHTBALL || type == MONS_LIGHTBALL2 || type == MONS_LIGHTBALL4 || type == MONS_LIGHTBALL5 ||
			type == MONS_BUBBLE
			// evilizer, bonkula, wolfmen etc. are targets despite sometimes being invisible
		))
			return 1;
	}

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

void HealGoodguy(byte amt)
{
	if(player.hearts+amt>player.maxHearts)
	{
		player.hearts=player.maxHearts;
	}
	else
		player.hearts+=amt;
	goodguy->hp=player.hearts;
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

void ShiftGuys(char dx,char dy,Map *map)
{
	// this is for when the map is moved around in the map editor (I,J,K,M)
	int i;

	for(i=0;i<maxGuys;i++)
	{
		guys[i]->x+=dx*TILE_WIDTH*FIXAMT;
		guys[i]->y+=dy*TILE_HEIGHT*FIXAMT;
		if(guys[i]->x<0)
			guys[i]->x+=map->width*TILE_WIDTH*FIXAMT;
		if(guys[i]->x>=(map->width*TILE_WIDTH*FIXAMT))
			guys[i]->x-=map->width*TILE_WIDTH*FIXAMT;
		if(guys[i]->y<0)
			guys[i]->y+=map->height*TILE_HEIGHT*FIXAMT;
		if(guys[i]->y>=(map->height*TILE_HEIGHT*FIXAMT))
			guys[i]->y-=map->height*TILE_HEIGHT*FIXAMT;
	}
}

byte DyingSkellies(void)
{
	int i;

	for(i=0;i<maxGuys;i++)
		if(guys[i] && (guys[i]->type==MONS_BONEHEAD || guys[i]->type==MONS_BONEHEAD2 || guys[i]->type==MONS_BONEHEAD3)
			&& guys[i]->hp==0 && guys[i]->seq==ANIM_DIE)
			return 1;
	return 0;
}

void RaiseSkellies(void)
{
	int i;

	for(i=0;i<maxGuys;i++)
		if(guys[i] && (guys[i]->type==MONS_BONEHEAD || guys[i]->type==MONS_BONEHEAD2 || guys[i]->type==MONS_BONEHEAD3)
			&& guys[i]->hp==0 && guys[i]->seq==ANIM_DIE)
		{
			// come to life
			guys[i]->seq=ANIM_A3;
			guys[i]->frm=0;
			guys[i]->frmTimer=0;
			guys[i]->frmAdvance=128;
			guys[i]->hp=MonsterHP(guys[i]->type);
			guys[i]->ouch=0;
			guys[i]->mind=0;
			guys[i]->mind1=0;
			guys[i]->mind2=0;
			guys[i]->mind3=0;
			guys[i]->reload=0;
			guys[i]->parent=NULL;
			guys[i]->CalculateRect();
			guys[i]->tag=255;
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
		if(guys[i] && guys[i]->type!=player.monsType)
		{
			guys[i]->type=MONS_NONE;
		}
}

void Shock(Map *map,world_t *world,byte type,int x,int y,byte liteup)
{
	int i,j;
	int x1,x2;
	int goodx=0,goody=0;
	int elderx=0,eldery=0;
	Guy *elder;

	bulletHittingType=255;
	// find a water elder if he's there
	elder=NULL;
	for(i=0;i<maxGuys;i++)
		if(guys[i]->type==MONS_ELDER3)
			elder=guys[i];

	if(elder && elder->ouch)
		elder=NULL;	// don't hurt when ouched, that keeps the damage rate down

	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type==type && (guys[i]->mapx==x || guys[i]->mapy==y))
		{
			j=abs(guys[i]->mapx-x);
			if(j>0 && j<10)
			{
				// horizontal zap
				if(guys[i]->mapx<x)
				{
					x1=guys[i]->mapx;
					x2=x;
				}
				else
				{
					x1=x;
					x2=guys[i]->mapx;
				}
				for(j=x1;j<x2;j++)
				{
					map->TempTorch(j,y,(byte)Random(32));
					if(goodguy->mapx==j && goodguy->mapy==y)
					{
						if(goodguy->y/FIXAMT-(goodguy->mapy*TILE_HEIGHT)<TILE_HEIGHT/2)
						{
							goodguy->GetShot(0,0,2,map,world);
							goodx=0;
							goody=-FIXAMT*8;
						}
						else
						{
							goodguy->GetShot(0,0,2,map,world);
							goodx=0;
							goody=FIXAMT*8;
						}
					}
					if(elder && elder->mapx==j && elder->mapy==y)
					{
						if(elder->y/FIXAMT-(elder->mapy*TILE_HEIGHT)<TILE_HEIGHT/2)
						{
							elder->GetShot(0,0,10,map,world);
							elderx=0;
							eldery=-FIXAMT*8;
						}
						else
						{
							elder->GetShot(0,0,10,map,world);
							elderx=0;
							eldery=FIXAMT*8;
						}
					}
				}
				if((liteup&3)==3)
					LightningBolt((x1*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
								  (y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT-FIXAMT*20,
								  (x2*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
								  (y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT-FIXAMT*20);
			}
			else
			{
				j=abs(guys[i]->mapy-y);
				if(j>0 && j<10)
				{
					// vertical zap
					if(guys[i]->mapy<y)
					{
						x1=guys[i]->mapy;
						x2=y;
					}
					else
					{
						x1=y;
						x2=guys[i]->mapy;
					}
					for(j=x1;j<x2;j++)
					{
						map->TempTorch(x,j,(byte)Random(32));
						if(goodguy->mapx==x && goodguy->mapy==j)
						{
							if(goodguy->x/FIXAMT-(goodguy->mapx*TILE_WIDTH)<TILE_WIDTH/2)
							{
								goodguy->GetShot(0,0,2,map,world);
								goodx=-FIXAMT*8;
								goody=0;
							}
							else
							{
								goodguy->GetShot(0,0,2,map,world);
								goodx=FIXAMT*8;
								goody=0;
							}
						}
						if(elder && elder->mapx==x && elder->mapy==j)
						{
							if(elder->x/FIXAMT-(elder->mapx*TILE_WIDTH)<TILE_WIDTH/2)
							{
								elder->GetShot(0,0,10,map,world);
								elderx=-FIXAMT*8;
								eldery=0;
							}
							else
							{
								elder->GetShot(0,0,10,map,world);
								elderx=FIXAMT*8;
								eldery=0;
							}
						}
					}
					if((liteup&3)==3)
						LightningBolt((x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
									  (x1*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT-FIXAMT*20,
									  (x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
									  (x2*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT-FIXAMT*20);
				}
			}
		}
	}
	goodguy->x+=goodx;
	if(goodx!=0)
		if(!goodguy->CanWalk(goodguy->x,goodguy->y,map,world))
			goodguy->x-=goodx;
	goodguy->y+=goody;
	if(goody!=0)
		if(!goodguy->CanWalk(goodguy->x,goodguy->y,map,world))
			goodguy->y-=goody;
	if(elder)
	{
		elder->x+=elderx;
		if(elderx!=0)
			if(!elder->CanWalk(elder->x,elder->y,map,world))
				elder->x-=elderx;
		elder->y+=eldery;
		if(eldery!=0)
			if(!elder->CanWalk(elder->x,elder->y,map,world))
				elder->y-=eldery;
	}
}

void LightBallChange(byte tag,int value)
{
	int i;

	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type>=MONS_LIGHTBALL && guys[i]->type<=MONS_LIGHTBALL5 &&
			guys[i]->hp>0 && guys[i]->tag==tag)
		{
			switch(value)
			{
				case 0:
					// turn off
					guys[i]->type=MONS_LIGHTBALL5;
					DoMove(guys[i],ANIM_IDLE,128,0,0,0);
					break;
				case 1:
					// turn on
					guys[i]->type=MONS_LIGHTBALL4;
					DoMove(guys[i],ANIM_IDLE,128,0,0,0);
					break;
				case 2:
					// toggle
					if(guys[i]->type==MONS_LIGHTBALL5)
						guys[i]->type=MONS_LIGHTBALL4;
					else
						guys[i]->type=MONS_LIGHTBALL5;
					DoMove(guys[i],ANIM_IDLE,128,0,0,0);
					break;
			}
		}
	}
}

void LaunchJunk(Map *map,Guy *me)
{
	int i,t,x,y;
	Guy *g;

	t=0;
	while(t++<256)	// 256 tries to find something throwable
	{
		i=Random(map->width*map->height);
		if(map->map[i].item==ITM_BARREL || map->map[i].item==ITM_BARREL2)
		{
			map->map[i].item=ITM_NONE;
			y=i/map->width;
			x=i-y*map->width;
			x=((x*TILE_WIDTH)+TILE_WIDTH/2)<<FIXSHIFT;
			y=((y*TILE_HEIGHT)+TILE_HEIGHT/2)<<FIXSHIFT;
			g=AddGuy(x,y,2*FIXAMT,MONS_JUNK);
			if(g)
				g->parent=me;
			break;
		}
		if(map->map[i].item>=ITM_CHAIR && map->map[i].item<=ITM_CHAIR5)
		{
			map->map[i].item=ITM_NONE;
			y=i/map->width;
			x=i-y*map->width;
			x=((x*TILE_WIDTH)+TILE_WIDTH/2)<<FIXSHIFT;
			y=((y*TILE_HEIGHT)+TILE_HEIGHT/2)<<FIXSHIFT;
			g=AddGuy(x,y,2*FIXAMT,MONS_JUNK2);
			if(g)
				g->parent=me;
			break;
		}
		if(map->map[i].item==ITM_TABLE2)
		{
			map->map[i].item=ITM_NONE;
			y=i/map->width;
			x=i-y*map->width;
			x=((x*TILE_WIDTH)+TILE_WIDTH/2)<<FIXSHIFT;
			y=((y*TILE_HEIGHT)+TILE_HEIGHT/2)<<FIXSHIFT;
			g=AddGuy(x,y,2*FIXAMT,MONS_JUNK3);
			if(g)
				g->parent=me;
			break;
		}
	}
}

void Sneeze(void)
{
	int i;

	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type>=MONS_JUNK && guys[i]->type<=MONS_JUNK3)
		{
			guys[i]->mind=1;
			guys[i]->mind1=50;
			guys[i]->dx=guys[i]->dx*2;
			guys[i]->dy=guys[i]->dy*2;
		}
	}
}

void GirlChasePlayer(byte on)
{
	int i;

	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type>=MONS_VILLAGER4 && guys[i]->type<=MONS_VILLAGER6)
		{
			guys[i]->mind3=on;
		}
	}
}

byte NotFriendly(byte type)
{
	if(opt.cheats[CH_DEATH])
		return (type!=player.monsType && type!=MONS_HELPERBAT && type<MONS_SUMBOMBIE);
	else
		return (type!=player.monsType && (type<MONS_VILLAGER || type>MONS_VILLAGER6) && type!=MONS_HELPERBAT &&
			type<MONS_SUMBOMBIE);
}

byte BallHitCheck(bullet_t *ball)
{
	int i;
	int myx,myy,myx2,myy2;
	int youx,youy,youx2,youy2;

	myx=(ball->x>>FIXSHIFT)-8;
	myx2=(ball->x>>FIXSHIFT)+8;
	myy=(ball->y>>FIXSHIFT)-8;
	myy2=(ball->y>>FIXSHIFT)+8;

	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type && guys[i]->type!=player.monsType && guys[i]->active)
		{
			youx=guys[i]->rectx+guys[i]->x/FIXAMT;
			youx2=guys[i]->rectx2+guys[i]->x/FIXAMT;
			youy=guys[i]->recty+guys[i]->y/FIXAMT;
			youy2=guys[i]->recty2+guys[i]->y/FIXAMT;
			if(Intersect(myx,myy,myx2,myy2,youx,youy,youx2,youy2))
			{
				if(ball->x<guys[i]->x-FIXAMT*8)
				{
					if(ball->y<guys[i]->y-FIXAMT*8)
						return 5+1;
					else if(ball->y>guys[i]->y+FIXAMT*8)
						return 3+1;
					else
						return 4+1;
				}
				else if(ball->x>guys[i]->x+FIXAMT*8)
				{
					if(ball->y<guys[i]->y-FIXAMT*8)
						return 7+1;
					else if(ball->y>guys[i]->y+FIXAMT*8)
						return 1+1;
					else
						return 0+1;
				}
				else
				{
					if(ball->y<guys[i]->y-FIXAMT*8)
						return 6+1;
					else if(ball->y>guys[i]->y+FIXAMT*8)
						return 2+1;
					else
						return 9+1;
				}
			}
		}
	}
	return 0;
}

void FreezeAllBadguys(void)
{
	int i;

	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type && NotFriendly(guys[i]->type))
			guys[i]->ice=200;
	}
}

void FindSummons(void)
{
	int i;

	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type>=MONS_SUMBOMBIE && guys[i]->type<=MONS_SUMDOG)
			AddToSummoned(guys[i]);
	}
}

#include "guy.h"
#include "player.h"
#include "intface.h"

Guy **guys;
Guy *goodguy;
int maxGuys;

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

	if((MonsterFlags(him->type)&MF_ENEMYWALK) && (type!=MONS_BOUAPHA))
		return 0;
	if((MonsterFlags(him->type)&MF_FREEWALK) && (type==MONS_BOUAPHA))
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
			if((guys[i]) && (guys[i]->type) && (guys[i]->hp>0) &&
				(abs(guys[i]->mapx-destx)<8) && (abs(guys[i]->mapy-desty)<8))
				if(TileBonkCheck(xx,yy,guys[i]))
					return;	// the pushy block would hit the guy

		// the floor is pushonable, let's do it
		MakeSound(SND_BLOCKPUSH,me->x,me->y,SND_CUTOFF,1000);
		map->map[destx+desty*map->width].wall=map->map[x+y*map->width].wall;
		map->map[destx+desty*map->width].floor=map->map[x+y*map->width].floor;
		map->map[x+y*map->width].floor=world->terrain[map->map[x+y*map->width].floor].next;
		map->map[x+y*map->width].wall=0;
	}
}

byte Walkable(Guy *me,int x,int y,Map *map,world_t *world)
{
	byte result;
	mapTile_t *m=&map->map[x+y*map->width];

	result=1;

	if(me->type==MONS_BOUAPHA && (!PlayerGetItem(m->item)))
		m->item=ITM_NONE;

	if((world->terrain[m->floor].flags&(TF_WATER|TF_LAVA)) && (!(MonsterFlags(me->type)&(MF_WATERWALK|MF_FLYING))))
		result=0;

	if((world->terrain[m->floor].flags&TF_SOLID) && (!(MonsterFlags(me->type)&(MF_FLYING|MF_WALLWALK))))
		result=0; // impassable

	if((world->terrain[m->floor].flags&TF_PUSHY) && me->type==MONS_BOUAPHA)
	{
		TryToPush(me,x,y,map,world);
	}

	if(m->item>=ITM_DOOR1 && m->item<=ITM_DOOR2B && me->type==MONS_BOUAPHA)
	{
		if(m->item==ITM_DOOR1B || m->item==ITM_DOOR2B)
			if(PlayerKeys(3))
			{
				m->item=ITM_NONE;
				MakeSound(SND_DOOROPEN,(x*TILE_WIDTH)<<FIXSHIFT,(y*TILE_HEIGHT)<<FIXSHIFT,SND_CUTOFF,1500);
			}
		if(m->item==ITM_DOOR1G || m->item==ITM_DOOR2G)
			if(PlayerKeys(2))
			{
				m->item=ITM_NONE;
				MakeSound(SND_DOOROPEN,(x*TILE_WIDTH)<<FIXSHIFT,(y*TILE_HEIGHT)<<FIXSHIFT,SND_CUTOFF,1500);
			}
		if(m->item==ITM_DOOR1R || m->item==ITM_DOOR2R)
			if(PlayerKeys(1))
			{
				m->item=ITM_NONE;
				MakeSound(SND_DOOROPEN,(x*TILE_WIDTH)<<FIXSHIFT,(y*TILE_HEIGHT)<<FIXSHIFT,SND_CUTOFF,1500);
			}
		if(m->item==ITM_DOOR1 || m->item==ITM_DOOR2)
			if(PlayerKeys(0))
			{
				m->item=ITM_NONE;
				PlayerLoseKey(0);
				MakeSound(SND_DOOROPEN,(x*TILE_WIDTH)<<FIXSHIFT,(y*TILE_HEIGHT)<<FIXSHIFT,SND_CUTOFF,1500);
			}
	}

	if(MonsterFlags(me->type)&MF_FLYING)
	{
		if((m->wall || m->item>=MAX_SHOOTABLE_ITMS) && (!(MonsterFlags(me->type)&MF_WALLWALK)))
			result=0;
	}
	else if(MonsterFlags(me->type)&MF_AQUATIC)
	{
		if(!(world->terrain[m->floor].flags&(TF_WATER|TF_LAVA)))
			result=0;
		if(m->wall || m->item>=MAX_WALKABLE_ITMS)
			result=0;
	}
	else
	{
		if((m->wall || m->item>=MAX_WALKABLE_ITMS) && (!(MonsterFlags(me->type)&MF_WALLWALK)))
			result=0;
	}

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
				(abs(guys[i]->mapx-mapx)<8) && (abs(guys[i]->mapy-mapy)<8))
			{
				if(CoconutBonk(xx,yy,guys[i]))
				{
					return 0;	// hit heads with somebody
				}
			}

	return result;
}

void Guy::SeqFinished(void)
{
	if((seq==ANIM_DIE) || (seq==ANIM_A3 && type==MONS_BOUAPHA))
	{
		if(type==MONS_BOUAPHA)
		{
			// restart current level
			SendMessageToGame(MSG_RESET,0);
		}
		
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

void Guy::Update(Map *map,world_t *world)
{
	int oldmapx,oldmapy;
	byte b;
	static byte burnFlip=0;

	oldmapx=mapx;
	oldmapy=mapy;

	CalculateRect();

	if(type==MONS_BOUAPHA)	// special case, player controls Bouapha
		PlayerControlMe(this,&map->map[mapx+mapy*map->width],world);
	else
		MonsterControl(map,world);

	if(ouch>0)
		ouch--;

	x+=dx;

	if(!CanWalk(x,y,map,world))
	{
		x-=dx;
		if(type==MONS_ROLLER || type==MONS_CENTIHEAD || type==MONS_DRL || type==MONS_BOUAPHA)
			mind1=1;	// tell it that it hit a wall
	}
	
	y+=dy;
	
	if(!CanWalk(x,y,map,world))
	{
		y-=dy;
		if(type==MONS_ROLLER || type==MONS_CENTIHEAD || type==MONS_DRL || type==MONS_BOUAPHA)
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

	if(type==MONS_BOUAPHA)	// special case, Bouapha is the player, follow him
	{
		UpdateCamera(x>>FIXSHIFT,y>>FIXSHIFT,facing*32,map);
		if(map->flags&MAP_TORCHLIT)
		{
			b=GetPlayerGlow();
			if(b>32)
				map->BrightTorch(mapx,mapy,b,b/4);
			else
				map->TempTorch(mapx,mapy,b);
			if(b>32)
				b-=4;
			else if(b<32)
				b++;
			SetPlayerGlow(b);
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
		if((hp>0) && (world->terrain[map->map[mapx+mapy*map->width].floor].flags&TF_WATER)
			&& (!PlayerCanWaterwalk()))
		{
			facing=(4+facing)&7;
			hp=0;
			SetPlayerHP(hp);
			seq=ANIM_A3;
			dx=0;
			dy=0;
			frm=0;
			frmAdvance=255;
			frmTimer=0;
			action=ACTION_BUSY;
			MakeSound(SND_DROWN,x,y,SND_CUTOFF|SND_ONE,65536);
		}
		// standing on lava, OW!
		if((hp>0) && (world->terrain[map->map[mapx+mapy*map->width].floor].flags&TF_LAVA)
			&& (!PlayerCanWaterwalk()))
		{
			if(burnFlip)
			{
				GetShot(0,0,10,map,world);
				if(hp==0)	// it was the death of me
				{
					// lava death
				}
			}
			burnFlip=1-burnFlip;
		}
		if((oldmapx!=mapx || oldmapy!=mapy) &&
			(world->terrain[map->map[mapx+mapy*map->width].floor].flags&TF_STEP))
		{
			map->map[mapx+mapy*map->width].floor=world->terrain[map->map[mapx+mapy*map->width].floor].next;
		}
	}
	if(oldmapx!=mapx || oldmapy!=mapy)
		SpecialStepCheck(map,mapx,mapy,this);

	bright=map->map[mapx+mapy*map->width].templight;
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
	MonsterDraw(x,y,z,type,seq,frm,facing,bright*(light>0),ouch);
}

void Guy::MonsterControl(Map *map,world_t *world)
{
	switch(type)
	{
		case MONS_BONEHEAD:
			AI_Bonehead(this,map,world,goodguy);
			break;
		case MONS_BAT:
			AI_Bat(this,map,world,goodguy);
			break;
		case MONS_SPIDER:
			AI_Spider(this,map,world,goodguy);
			break;
		case MONS_BIGSPDR:
			AI_BigSpider(this,map,world,goodguy);
			break;
		case MONS_ZOMBIE:
			AI_Zombie(this,map,world,goodguy);
			break;
		case MONS_EGGSAC:
			AI_EggSac(this,map,world,goodguy);
			break;
		case MONS_MAMASPDR:
			AI_MamaSpider(this,map,world,goodguy);
			break;
		case MONS_PYGMY:
			AI_Pygmy(this,map,world,goodguy);
			break;
		case MONS_SERPENT:
			AI_Serpent(this,map,world,goodguy);
			break;
		case MONS_MATHEAD:
		case MONS_MATSKULL:
			AI_MattieSkullOrHead(this,map,world,goodguy);
			break;
		case MONS_MATBRAIN:
			AI_MattieBrain(this,map,world,goodguy);
			break;
		case MONS_MATBODY:
			AI_MattieBody(this,map,world,goodguy);
			break;
		case MONS_MATCLAW1:
		case MONS_MATCLAW2:
			AI_MattieClaw(this,map,world,goodguy);
			break;
		case MONS_MATTAIL:
			AI_MattieTail(this,map,world,goodguy);
			break;
		case MONS_GINGER:
			AI_Ginger(this,map,world,goodguy);
			break;
		case MONS_PUMPKIN:
			AI_Pumpkin(this,map,world,goodguy);
			break;
		case MONS_BABYTHING:
			AI_BabyThing(this,map,world,goodguy);
			break;
		case MONS_MOSS:
			AI_Moss(this,map,world,goodguy);
			break;
		case MONS_MOSSGRANDE:
			AI_MossGrande(this,map,world,goodguy);
			break;
		case MONS_MAGMAZOID:
			AI_Magmazoid(this,map,world,goodguy);
			break;
		case MONS_SHROOM:
			AI_Shroom(this,map,world,goodguy);
			break;
		case MONS_MUSH:
			AI_Mush(this,map,world,goodguy);
			break;
		case MONS_THING:
			AI_TheThing(this,map,world,goodguy);
			break;
		case MONS_THINGTENT:
		case MONS_THINGTENTTIP:
			AI_ThingTentacle(this,map,world,goodguy);
			break;
		case MONS_SUPERZOMBIE:
			AI_SuperZombie(this,map,world,goodguy);
			break;
		case MONS_STICKMAN:
			AI_StickMan(this,map,world,goodguy);
			break;
		case MONS_BABYSEAL:
			AI_BabySeal(this,map,world,goodguy);
			break;
		case MONS_ISOZOID:
			AI_Isozoid(this,map,world,goodguy);
			break;
		case MONS_SNOWGUY:
			AI_Snowguy(this,map,world,goodguy);
			break;
		case MONS_PENGUIN:
			AI_Penguin(this,map,world,goodguy);
			break;
		case MONS_ZOMBONI:
			AI_Zomboni(this,map,world,goodguy);
			break;
		case MONS_SVEN:
		case MONS_BJORN:
			AI_Yeti(this,map,world,goodguy);
			break;
		case MONS_GEOZOID:
			AI_Geozoid(this,map,world,goodguy);
			break;
		case MONS_MUMBLE:
			AI_Mumble(this,map,world,goodguy);
			break;
		case MONS_DJINNI:
			AI_Djinni(this,map,world,goodguy);
			break;
		case MONS_LAMP:
			AI_MagicLamp(this,map,world,goodguy);
			break;
		case MONS_CACTUS:
			AI_Cactus(this,map,world,goodguy);
			break;
		case MONS_ROLLER:
			AI_Roller(this,map,world,goodguy);
			break;
		case MONS_LICH:
			AI_Lich(this,map,world,goodguy);
			break;
		case MONS_DUSTDEVIL:
			AI_DustDevil(this,map,world,goodguy);
			break;
		case MONS_MECHABOUAPHA:
			AI_MechaBouapha(this,map,world,goodguy);
			break;
		case MONS_SPHXARM1:
		case MONS_SPHXARM2:
			AI_SphinxArm(this,map,world,goodguy);
			break;
		case MONS_SPHINX:
			AI_Sphinx(this,map,world,goodguy);
			break;
		case MONS_FREAKAZOID:
			AI_Freakazoid(this,map,world,goodguy);
			break;
		case MONS_CENTIBODY:
			AI_CentiBody(this,map,world,goodguy);
			break;
		case MONS_CENTIHEAD:
			AI_CentiHead(this,map,world,goodguy);
			break;
		case MONS_WACKO:
			AI_Wacko(this,map,world,goodguy);
			break;
		case MONS_BOILER:
			AI_Boiler(this,map,world,goodguy);
			break;
		case MONS_GREATPK:
			AI_GreatPumpkin(this,map,world,goodguy);
			break;
		case MONS_ULTRAZOID:
			AI_Ultrazoid(this,map,world,goodguy);
			break;
		case MONS_DRL:
			AI_DrLunatic(this,map,world,goodguy);
			break;
		case MONS_SDZL:
			AI_SDZL(this,map,world,goodguy);
			break;
		case MONS_SANTA:
			AI_Santa(this,map,world,goodguy);
			break;
	}
}

void Guy::GetShot(int dx,int dy,byte damage,Map *map,world_t *world)
{
	int i,formerHP;

	if(hp==0)
		return;	// can't shoot a dead guy

	if(type==0)
		return;	// shouldn't have a type 0 guy at all

	if(type==MONS_BOUAPHA && PlayerShield())
		return; // invincible when shielded

	if(MonsterFlags(type)&MF_INVINCIBLE)
		return;	// invincible

	formerHP=hp;
	ouch=4;
	hp-=damage;
	
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
		hp=0;
		seq=ANIM_DIE;
		this->dx=0;
		this->dy=0;
		frm=0;
		frmAdvance=64;
		frmTimer=0;
		action=ACTION_BUSY;
		if(type==MONS_MUSH || type==MONS_SVEN || type==MONS_BJORN)
			facing=2;	// these can only die facing forward, artistically speaking
		// possible item drop
		switch(type)
		{
			case MONS_ZOMBIE:	// zombies always drop a brain
				if(!map->DropItem(mapx,mapy,ITM_BRAIN))
					map->map[mapx+mapy*map->width].item=ITM_BRAIN;	// if the drop failed, just
																	// FORCE a brain to appear, covering
																	// whatever item was there
				break;
			case MONS_SUPERZOMBIE:	// super zombies always drop 2 brains
				if(!map->DropItem(mapx,mapy,ITM_BRAIN))
					map->map[mapx+mapy*map->width].item=ITM_BRAIN;
				if(!map->DropItem(mapx,mapy,ITM_BRAIN))
					map->map[mapx+(mapy+1)*map->width].item=ITM_BRAIN;	// hope it's not a wall
				break;
			case MONS_BOUAPHA:
			case MONS_MAMASPDR:	// already drops hundreds of baby spiders, that's enough
			case MONS_SERPENT:	// don't drop items, since you live on the water
			case MONS_FREAKAZOID: // don't drop items, since you live on the water
			case MONS_MAGMAZOID:// don't drop items, since you live in the lava
			case MONS_MATCLAW1:
			case MONS_MATCLAW2:
			case MONS_MATHEAD:
			case MONS_MATSKULL:
			case MONS_MUSH:
			case MONS_SVEN:
			case MONS_BJORN:
			case MONS_DUSTDEVIL:
			case MONS_LICH:
			case MONS_DRL:
			case MONS_SDZL:
			case MONS_ULTRAZOID:
			case MONS_THINGTENT:
			case MONS_THINGTENTTIP:
			case MONS_STICKMAN:
			case MONS_MECHABOUAPHA:
			case MONS_CENTIBODY:
			case MONS_CENTIHEAD:
			case MONS_BOILER:
			case MONS_GREATPK:
			case MONS_SANTA:
				// no drop
				break;
			case MONS_MATBRAIN:
			case MONS_SPHINX:
				// bosses which drop loony keys
				if(!map->DropItem(mapx,mapy,ITM_LOONYKEY))
					map->map[mapx+mapy*map->width].item=ITM_LOONYKEY;
				break;
			case MONS_THING:
				break;
			default:
				// drop random item
				if(map->flags&MAP_MANYITEMS)
					i=MGL_random(50);
				else
					i=MGL_random(200);
				if(i<2)
					map->DropItem(mapx,mapy,ITM_FLAME);
				else if(i<4)
					map->DropItem(mapx,mapy,ITM_BOMBS);
				else if(i<6)
					map->DropItem(mapx,mapy,ITM_AK8087);
				else if(i<8)
					map->DropItem(mapx,mapy,ITM_MISSILES);
				else if(i<12)
					map->DropItem(mapx,mapy,ITM_TAKEOUT);
				else if(i<13)
					map->DropItem(mapx,mapy,ITM_SHIELD);
				break;
		}
		
		// give him points for it
		PlayerGetPoints(MonsterPoints(type));
	}
	if(type!=MONS_BOUAPHA)
		ShowEnemyLife(MonsterName(type),formerHP*128/MonsterHP(type),hp*128/MonsterHP(type),(byte)(hp>0));
	else
		SetPlayerHP(hp);
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

	for(i=0;i<maxGuys;i++)
		delete guys[i];

	free(guys);
}

void UpdateGuys(Map *map,world_t *world)
{
	int i;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type!=MONS_NONE)
			guys[i]->Update(map,world);
}

void EditorUpdateGuys(Map *map)
{
	int i;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type!=MONS_NONE)
			guys[i]->EditorUpdate(map);
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
	int i,j;
	Guy *g,*g2;

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
			guys[i]->reload=0;
			guys[i]->parent=NULL;
			guys[i]->CalculateRect();
			guys[i]->ID=i;
			guys[i]->mapx=(guys[i]->x>>FIXSHIFT)/TILE_WIDTH;
			guys[i]->mapy=(guys[i]->y>>FIXSHIFT)/TILE_HEIGHT;
			if(type==MONS_BOUAPHA)	// special case: bouapha is the goodguy
			{
				goodguy=guys[i];
				PutCamera(x,y);
			}
			if(type==MONS_MATHEAD)	// Matilda, need to add all the parts
			{
				guys[i]->z=32*FIXAMT;
				g=AddGuy(x,y-32*FIXAMT,0,MONS_MATBODY);
				if(g)
					g->parent=guys[i];
				g=AddGuy(x-40*FIXAMT,y-1*FIXAMT,32*FIXAMT,MONS_MATCLAW1);
				if(g)
					g->parent=guys[i];
				g=AddGuy(x+40*FIXAMT,y-1*FIXAMT,32*FIXAMT,MONS_MATCLAW2);
				if(g)
					g->parent=guys[i];
				g=AddGuy(x,y-96*FIXAMT,32*FIXAMT,MONS_MATTAIL);
				if(g)
					g->parent=guys[i];
			}
			if(type==MONS_THING)	// The Thing needs to add tentacles
			{
				// lower left tentacle
				g=AddGuy(x-64*FIXAMT,y+48*FIXAMT,12*FIXAMT,MONS_THINGTENT);
				if(!g)
					return guys[i];
				g->parent=guys[i];
				g->facing=6;
				g->mind=0;	// mind tells them which tentacle they are (to constrain rotation)
				g->mind1=128;
				for(j=0;j<3;j++)
				{
					g2=g;
					// adding them all overlapping, they'll move into place when updated
					g=AddGuy(x-64*FIXAMT,y+48*FIXAMT,12*FIXAMT,MONS_THINGTENT);
					if(!g)
						return guys[i];
					g->parent=g2;
					g->facing=6;
					g->mind=0;
				}
				g2=AddGuy(x-64*FIXAMT,y+48*FIXAMT,12*FIXAMT,MONS_THINGTENTTIP);
				if(!g2)
					return guys[i];
				g2->parent=g;
				g2->facing=6;
				g2->mind=0;
				// lower right tentacle
				g=AddGuy(x+64*FIXAMT,y+48*FIXAMT,12*FIXAMT,MONS_THINGTENT);
				if(!g)
					return guys[i];
				g->parent=guys[i];
				g->facing=2;
				g->mind=1;
				g->mind1=128;
				for(j=0;j<3;j++)
				{
					g2=g;
					// adding them all overlapping, they'll move into place when updated
					g=AddGuy(x+64*FIXAMT,y+48*FIXAMT,12*FIXAMT,MONS_THINGTENT);
					if(!g)
						return guys[i];
					g->parent=g2;
					g->facing=2;
					g->mind=1;
				}
				g2=AddGuy(x+64*FIXAMT,y+48*FIXAMT,12*FIXAMT,MONS_THINGTENTTIP);
				if(!g2)
					return guys[i];
				g2->parent=g;
				g2->facing=2;
				g2->mind=1;
				// upper right tentacle
				g=AddGuy(x+64*FIXAMT,y-20*FIXAMT,12*FIXAMT,MONS_THINGTENT);
				if(!g)
					return guys[i];
				g->parent=guys[i];
				g->facing=14;
				g->mind=2;
				g->mind1=128;
				for(j=0;j<3;j++)
				{
					g2=g;
					// adding them all overlapping, they'll move into place when updated
					g=AddGuy(x+64*FIXAMT,y-20*FIXAMT,12*FIXAMT,MONS_THINGTENT);
					if(!g)
						return guys[i];
					g->parent=g2;
					g->facing=14;
					g->mind=2;
				}
				g2=AddGuy(x+64*FIXAMT,y-20*FIXAMT,12*FIXAMT,MONS_THINGTENTTIP);
				if(!g2)
					return guys[i];
				g2->parent=g;
				g2->facing=14;
				g2->mind=2;
				// upper left (and last!) tentacle
				g=AddGuy(x-64*FIXAMT,y-20*FIXAMT,12*FIXAMT,MONS_THINGTENT);
				if(!g)
					return guys[i];
				g->parent=guys[i];
				g->facing=10;
				g->mind=3;
				g->mind1=128;
				for(j=0;j<3;j++)
				{
					g2=g;
					// adding them all overlapping, they'll move into place when updated
					g=AddGuy(x-64*FIXAMT,y-20*FIXAMT,12*FIXAMT,MONS_THINGTENT);
					if(!g)
						return guys[i];
					g->parent=g2;
					g->facing=10;
					g->mind=3;
				}
				g2=AddGuy(x-64*FIXAMT,y-20*FIXAMT,12*FIXAMT,MONS_THINGTENTTIP);
				if(!g2)
					return guys[i];
				g2->parent=g;
				g2->facing=10;
				g2->mind=3;
			}
			if(type==MONS_SPHINX)
			{
				// left arm
				g=AddGuy(x-108*FIXAMT,y+22*FIXAMT,0,MONS_SPHXARM1);
				if(!g)
					return guys[i];
				g->parent=guys[i];
				// right arm
				g=AddGuy(x+108*FIXAMT,y+22*FIXAMT,0,MONS_SPHXARM2);
				if(!g)
					return guys[i];
				g->parent=guys[i];
			}
			if(type==MONS_CENTIHEAD)
			{
				// add on 15 body parts
				g=guys[i];
				for(j=0;j<15;j++)
				{
					g2=AddGuy(x,y,0,MONS_CENTIBODY);
					if(!g2)
						return guys[i];
					g2->parent=g;
					g=g2;
				}
			}
			return guys[i];
		}
	return NULL;
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
	int i;

	for(i=0;i<MAX_MAPMONS;i++)
		if(map->badguy[i].type)
		{
			AddGuy((map->badguy[i].x*TILE_WIDTH+(TILE_WIDTH/2))<<FIXSHIFT,
				   (map->badguy[i].y*TILE_HEIGHT+(TILE_HEIGHT/2))<<FIXSHIFT,
				   0,map->badguy[i].type);
		}
}

byte CheckHit(byte size,int xx,int yy,Guy *him)
{
	int x2,y2;

	xx-=size;
	yy-=size;

	x2=him->x>>FIXSHIFT;
	y2=him->y>>FIXSHIFT;

	if((x2+him->rectx2)>=xx && (y2+him->recty2)>=yy && (x2+him->rectx)<=(xx+size*2) &&
		(y2+him->recty)<=(yy+size*2))
		return 1;

	return 0;
}

byte FindVictim(int x,int y,byte size,int dx,int dy,byte damage,Map *map,world_t *world)
{
	int i;
	
	for(i=0;i<maxGuys;i++)
		if(guys[i]->type && guys[i]->hp && (guys[i]->type!=MONS_BOUAPHA))
		{
			if(CheckHit(size,x,y,guys[i]))
			{
				guys[i]->GetShot(dx,dy,damage,map,world);
				return 1;
			}
		}

	return 0;
}

byte FindGoodVictim(int x,int y,byte size,int dx,int dy,byte damage,Map *map,world_t *world)
{
	if(!goodguy || (!goodguy->hp))
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
		if(guys[i]->type && guys[i]->hp && (guys[i]->type!=MONS_BOUAPHA))
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

	bestguy=65535;
	bestRange=320+240;
	
	for(i=0;i<maxGuys;i++)
		if(guys[i]->type && guys[i]->hp && (guys[i]->type!=MONS_BOUAPHA))
		{
			range=abs(x-(guys[i]->x>>FIXSHIFT))+abs(y-(guys[i]->y>>FIXSHIFT));
			if((range<bestRange) || (range<320+240 && MGL_random(32)==0))
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

void HealGoodguy(byte amt)
{
	if(!goodguy)
		return;

	if(goodguy->hp+amt<128)
		goodguy->hp+=amt;
	else
		goodguy->hp=128;
}

// this checks to see if there is any moss on the chosen tile (x,y in tile coords)
byte MossCheck(int x,int y)
{
	int i;

	for(i=0;i<maxGuys;i++)
		if((guys[i]) && ((guys[i]->type==MONS_MOSS) || (guys[i]->type==MONS_MOSSGRANDE))
			&& (guys[i]->hp>0) && ((guys[i]->x>>FIXSHIFT)/TILE_WIDTH==x) &&
				((guys[i]->y>>FIXSHIFT)/TILE_HEIGHT==y))
				return 1;	// moss is in the way
	return 0;
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
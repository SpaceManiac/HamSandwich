#include "guy.h"
#include "player.h"
#include "intface.h"
#include "quest.h"
#include "editor.h"
#include "options.h"
#include "skill.h"
#include "leveldef.h"
#include "alchemy.h"
#include "arena.h"
#include "ch_loony.h"
#include "glasses.h"
#include "madcap.h"
#include "achieve.h"
#include "services.h"
#include "config.h"

Guy **guys;
Guy *goodguy;
int maxGuys;
static byte checkActive=0;
Guy *lifeGuy;
Guy fakeGuy;
static byte areMonsters;
Guy *meleeAttacker;
byte isCritical=0;
byte deathTag;

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
	if((MonsterFlags(him->type)&MF_FREEWALK) && (type==player.monsType || him->type==player.monsType))
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
		if(map->map[x+y*map->width].item)
		{
			// pushing an item
			map->map[destx+desty*map->width].item=map->map[x+y*map->width].item;
			map->map[x+y*map->width].item=IT_NONE;
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

	if(me->type==player.monsType && (!PlayerGetItem(m->item,m->tag,0)))
	{
		SpecialItemGetCheck(map,x,y);
		m->item=IT_NONE;
	}

	if(me->type==player.monsType && m->item>=IT_CHEST1 && m->item<=IT_CHEST5 && m->itemInfo==0 && player.var[VAR_CHEST+m->tag]==0)
	{
		if(EquipCount(IT_KEY1+m->item-IT_CHEST1))	// have the right key?
		{
			MakeNormalSound(SND_DOOROPEN);
			ParticleBoom(PART_FLOATYPOP,(x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,FIXAMT*20);
			m->itemInfo=1;
			player.var[VAR_CHEST+m->tag]=1;
			ChestPop(m->item,x,y);
		}
		else
		{
			NoRepeatNewMessage("You don't have the right key!",0);
		}
	}
	if(me->type==player.monsType && m->item>=IT_DOOR1 && m->item<=IT_DOOR5 && m->itemInfo==0)
	{
		if(EquipCount(IT_KEY1+m->item-IT_DOOR1))	// have the right key?
		{
			MakeNormalSound(SND_DOOROPEN);
			m->itemInfo=1;
		}
		else
		{
			NoRepeatNewMessage("You don't have the right key!",0);
		}
	}
	if(me->type==player.monsType && m->item==IT_ENERGYBARRIER)
	{
		if(EquipCount(IT_SOUNDING))	// have a crystal
		{
			MakeNormalSound(SND_DRAIN);
			m->item=IT_NONE;
			DropItemOfType(IT_SOUNDING,255);
			NewMessage("Energy Barrier dispelled!",0);
			PlayerSetVar(VAR_BARRIER+m->tag,1);
			ParticleBoom(PART_GREENPOP,(x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,FIXAMT*20);
		}
		else
		{
			me->GetShot(0,0,CurseDmg(1,EVIL),map,world);
		}
	}
	if(me->type==player.monsType && m->item==IT_STUMP)	// presumably half of a rope-able pair
	{
		result=0;
		if(EquipCount(IT_ROPE)>0)
		{
			if(RopeStump(map,x,y,1))
			{
				MakeNormalSound(SND_ROPESTUMP);
				DropItemOfType(IT_ROPE,1);
				PlayerSetVar(VAR_ROPED+map->map[x+y*map->width].tag,1);
				NewMessage("A lovely rope bridge!",0);
			}
		}
	}
	if(me->type==player.monsType && m->item==IT_CAULDRON)
	{
		result=0;
		if(!player.chatClock)
		{
			if(player.talentLevel[TLT_ALCHEMY]!=255)
				BeginChatting(254);	// do alchemy
			else
				NoRepeatNewMessage("You need the Alchemy Talent!",0);
		}
	}
	if(me->type==player.monsType && m->item==IT_ALLUSIONARY)
	{
		result=0;
		if(!player.chatClock)
		{
			if(ModifierOn(MOD_LITERACY))
			{
				EnterShop(SVC_ALLUS);
				player.chatClock=30;
			}
			else
			{
#ifdef DIRECTORS
				int i;
				result=1;
				for(i=0;i<GALLERY_PIX;i++)
					if(opt.galPix[i])
					{
						Gallery(GetDisplayMGL());
						player.chatClock=30;
						result=0;
						break;
					}
				if(result==1)
				{
					result=0;
					NoRepeatNewMessage("There are 12 strangely blank pages here.",0);
				}
#else
				NoRepeatNewMessage("The book is apparently unreadable.",0);
#endif
			}
		}
	}
	if(me->type==player.monsType && m->item==IT_SOUNDINGTREE && EquipCount(IT_SOUNDING)==0)
	{
		PlayerGetItem(IT_SOUNDING,0,0);
	}
	if(me->type==player.monsType && m->item==IT_CLOCK)
	{
		result=0;
		if(!player.chatClock)
		{
			if(player.talentLevel[TLT_CLOCKWORK]!=255)
			{
				if((player.var[VAR_CLOCKLIFE]+player.var[VAR_CLOCKLIFE2]*256)>0)	// living clockbot, repair it!
				{
					RepairClockbot(9999);
				}
				else
					BeginChatting(252);	// do clockwork
			}
			else
				NoRepeatNewMessage("You need the Clockwork Talent!",0);
		}
	}
	if(me->type==player.monsType && m->item==IT_OVEN)
	{
		result=0;
		if(!player.chatClock)
		{
			if(player.talentLevel[TLT_JUNKSMITH]!=255)
				BeginChatting(253);	// do junksmith
			else
				NoRepeatNewMessage("You need the Junksmith Talent!",0);
		}
	}
	if(me->type==player.monsType && m->item==IT_CASTLESWITCH)
	{
		result=0;
		if(m->itemInfo==0)
		{
			m->itemInfo=1;
			MakeNormalSound(SND_DOOROPEN);
		}
	}

	if(!(world->terrain[m->floor].flags&TF_ICE) && (MonsterFlags(me->type)&MF_ICEONLY))
	{
		result=0;	// ice-only monsters can only move on ice!
	}

	if((world->terrain[m->floor].flags&(TF_SKY|TF_CLIFF)) && (!(MonsterFlags(me->type)&(MF_FLYING))))
	{
		if(me->type!=player.monsType)
			result=0;
	}

	if((world->terrain[m->floor].flags&(TF_WATER|TF_LAVA)) && (!(MonsterFlags(me->type)&(MF_WATERWALK|MF_FLYING))))
	{
		result=0;
	}

	if(me->type==player.monsType && (world->terrain[m->floor].flags&(TF_WATER|TF_LAVA|TF_SKY) && !(itemInfo[m->item].flags&IF_WALK)))
	{
		/*
		if(me->z==0 &&
			(!(world->terrain[map->map[me->mapx+me->mapy*map->width].floor].flags&(TF_WATER|TF_LAVA|TF_SKY)) ||
			 (itemInfo[map->map[me->mapx+me->mapy*map->width].item].flags&IF_WALK)))
		{
			int mx,my;
			mx=me->x/(TILE_WIDTH*FIXAMT);
			my=me->y/(TILE_HEIGHT*FIXAMT);
			if((world->terrain[map->map[mx+my*map->width].floor].flags&(TF_WATER|TF_LAVA|TF_SKY)) &&
			 !(itemInfo[map->map[mx+my*map->width].item].flags&IF_WALK))
			result=0;	// can only go onto these spaces (from non-these spaces) while airborne (but only applies to getting your whole body on them, not your edges)
		}*/
	}

	if((world->terrain[m->floor].flags&TF_SOLID) && (!(MonsterFlags(me->type)&(MF_FLYING|MF_WALLWALK))))
		result=0; // impassable

	if((m->tag==255) && (!(MonsterFlags(me->type)&(MF_WALLWALK))) && (me->type!=player.monsType))
		result=0; // impassable to non-Loony and non-ghost
	if((m->tag==254) && (me->team!=1))
		result=0; // impassable to evil

	if((world->terrain[m->floor].flags&TF_PUSHY) && me->type==player.monsType)
	{
		TryToPush(me,x,y,map,world);
	}
	if((ItemFlags(m->item)&IF_PUSHABLE) && me->type==player.monsType)
	{
		TryToPush(me,x,y,map,world);
	}
	if((ItemFlags(m->item)&IF_STEAM) && me->type==player.monsType)
	{
		if(EquipCount(IT_UNDIES)==0)	// ouch!
		{
			NoRepeatNewMessage("That's some hot steam!",0);
			result=rand()%256;
			me->GetShotReal(Cosine(result)*20,Sine(result)*20,CurseDmg(8,EVIL),map,world);
			result=0;
		}
		else
		{
			if(player.levelNum==LVL_BASKERCAVE)
			{
				SendMessageToGame(MSG_GOTOMAP,LVL_GEYSER);
				player.destx=0;
				player.desty=0;
			}
			else
			{
				SendMessageToGame(MSG_GOTOMAP,LVL_BASKERCAVE);
				player.destx=20;
				player.desty=27;
			}
			result=0;
		}
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
			result=0;
		}
	}

	if(!result)	// bumped a wall, see if that triggers a special
	{
		SpecialStepCheck(map,x,y,me);
	}
	return result;
}

byte Guy::CanWalkNoMons(int xx,int yy,Map *map,world_t *world)
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
	{
		if(type!=MONS_CARAVAN)
			return 0;
		else
			return 1;
	}

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

	return result;
}

void Guy::Shoved(int xx,int yy,Map *map,world_t *world)
{
	int newx,newy;

	if(MonsterFlags(type)&MF_NOMOVE)
		return;

	if(type==MONS_BUNNYTROOP || type==MONS_BUNNYBOSS || type==MONS_BUNNYBOSS2 || type==MONS_THUG || type==MONS_BRUISER || type==MONS_JUNKSMITH)
	{
		if(team==GOOD && player.levelNum!=LVL_CLEARING)
			return;
	}
	if(type==MONS_BUNNYMAJOR && team==GOOD && player.levelNum!=LVL_CLEARING && player.var[VAR_SNUGGLY]==1)
		return;

	if((world->terrain[map->map[mapx+mapy*map->width].floor].flags&TF_ICE) && type==player.monsType)
		return;	// no shoving player on ice

	if(xx>x)
		newx=x-FIXAMT*2;
	else if(xx<x)
		newx=x+FIXAMT*2;
	else
		newx=x;

	if(yy>y)
		newy=y-FIXAMT*2;
	else if(yy<y)
		newy=y+FIXAMT*2;
	else
		newy=y;

	if(type==player.monsType)
	{
		monsType[type].flags&=(~(MF_WATERWALK));
	}
	if(!CanWalkNoMons(newx,y,map,world))
		newx=x;
	if(!CanWalkNoMons(x,newy,map,world))
		newy=y;

	if(type==player.monsType)
	{
		monsType[type].flags|=MF_WATERWALK;
	}
	x=newx;
	y=newy;
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
	{
		if(type!=MONS_CARAVAN)
			return 0;
		else
			return 1;
	}

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
				(player.levelNum!=0 || (player.worldNum!=WORLD_NORMAL && player.worldNum!=WORLD_REMIX) || guys[i]->active))
			{
				if(CoconutBonk(xx,yy,guys[i]))
				{
					if(type==player.monsType && player.chatClock==0 && (dx!=0 || dy!=0) &&
						guys[i]->type==MONS_VILLAGER && (guys[i]->mind!=2))
					{
						if(guys[i]->tag==12)
							player.var[VAR_GURUTALENT]=map->map[guys[i]->mapx+(guys[i]->mapy+1)*map->width].tag;
						BeginChatting(guys[i]->tag);
					}
					if(type==player.monsType && player.chatClock==0 && (dx!=0 || dy!=0) &&
						guys[i]->type==MONS_FURNACE && (guys[i]->mind!=2) && player.var[VAR_QUESTASSIGN+QUEST_FURNACE])
					{
						if(player.var[VAR_FURNACE]==0 && player.var[VAR_CAPTAIN2]==0 && EquipCount(IT_THING) && EquipCount(IT_THING2) && EquipCount(IT_WATCH))
						{
							// summon capitan to slay you
							Guy *g;

							player.var[VAR_CAPTAIN2]=1;
							g=AddGuy((14*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(23*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,600*FIXAMT,MONS_BUNNYBOSS2);
						}
						else if(player.var[VAR_CAPTAIN2]==3 && player.var[VAR_FURNACE]==0 && EquipCount(IT_THING) && EquipCount(IT_THING2) && EquipCount(IT_WATCH))
						{
							player.var[VAR_FURNACE]=1;
							DropItemOfType(IT_THING,1);
							DropItemOfType(IT_THING2,1);
							DropItemOfType(IT_WATCH,1);
							MakeNormalSound(SND_IGNITE);
							player.chatClock=0;
							BeginChatting(31);

							if(!SkillHave(SKILL_HEATSHIELD))
							{
								bullet_t *b;

								b=FireBullet(goodguy->ID,guys[i]->x,guys[i]->y+FIXAMT*48,FIXAMT*30,32,BLT_ITEM,0);
								if(b)
								{
									b->anim=IT_SCROLL;
									b->target=SKILL_HEATSHIELD;
									b->owner=0;
								}
							}
						}
					}
					if(type==player.monsType && player.chatClock==0 && (dx!=0 || dy!=0) &&
						guys[i]->type==MONS_MIMIC && (levelDef[player.levelNum].flags&LF_TOWN))
					{
						BeginChatting(251);
					}
					this->Shoved(guys[i]->x,guys[i]->y,map,world);
					guys[i]->Shoved(this->x,this->y,map,world);
					if(type==player.monsType && ModifierOn(MOD_TRAMPLE) && guys[i]->team==EVIL && player.reload==0 && !(MonsterFlags(guys[i]->type)&MF_NOMOVE))
					{
						byte a;
						a=AngleFrom(x,y,guys[i]->x,guys[i]->y);
						guys[i]->GetShot(Cosine(a)*10,Sine(a)*10,DamageAmt(player.level,GOOD),map,world);
						player.reload=10;
					}
					//return 0;	// hit heads with somebody
				}
			}

	return result;
}

byte Guy::WalkCheck(int xx,int yy,Map *map,world_t *world)
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
	{
		if(type!=MONS_CARAVAN)
			return 0;
		else
			return 1;
	}

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

	//if(MonsterFlags(type)&MF_FREEWALK)
		//return result;	// can't have a guy collision

	if(result)	// no wall collision, look for guy collision
		for(i=0;i<maxGuys;i++)
			if((guys[i]) && (guys[i]!=this) && (guys[i]->type) && (guys[i]->hp>0) &&
				(player.levelNum!=0 || (player.worldNum!=WORLD_NORMAL && player.worldNum!=WORLD_REMIX) || guys[i]->active))
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
	byte t;

	if(seq==ANIM_DIE)
	{
		if(type==player.monsType)
		{
			if(levelDef[player.levelNum].flags&LF_ARENA)
			{
				// can't actually die in the arena... exactly
				frm--;
				frmTimer=0;
				frmAdvance=0;
				LoseArena();
			}
			else
			{
				// restart current level
				SendMessageToGame(MSG_RESET,0);
				TalentDecrease();
				player.potionClock=0;
				player.potionType=0;
				player.potionPower=0;
			}
		}
		if(player.potionType==POT_EXPLOSIVO && team==EVIL && goodguy)	// badguy go boom
		{
			FireBullet(goodguy->ID,x,y,0,0,BLT_KABOOM,DamageAmt(PotionEffect(player.potionType,player.potionPower),GOOD));
			MakeSound(SND_BOOM,x,y,SND_CUTOFF|SND_RANDOM,760);
		}
		t=type;
		type=MONS_NONE;	// all gone
		if(player.levelNum==LVL_ARMSCAVE && player.var[VAR_SNUGGLY]==1 && player.var[VAR_QUESTASSIGN+QUEST_SHAKEDOWN] && player.var[VAR_SHAKEDOWN]==0 && !TaggedMonsterExists(2))
			PlayerSetVar(VAR_SHAKEDOWN,1);
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
		if(!AlwaysActiveMons(type) && (levelDef[player.levelNum].flags&LF_ARENA)==0 && (hp>0) && (abs(x-goodguy->x)>FIXAMT*640 || abs(y-goodguy->y)>FIXAMT*480))
		{
			active=0;
			// monsters are inactive when far away, except 'trap' type enemies, to keep them synced
			// also, threw in that dead enemies remain active, so that they can finish their
			// death sequence rather than freezing and finishing dying when you return
		}
		else
			active=1;
	}
	return active;
}

void Guy::FindTarget(Map *map,world_t *world)
{
	int i;
	int bestRange,range;

	target=NULL;
	bestRange=320+240;
	for(i=0;i<maxGuys;i++)
		if(guys[i]->type && guys[i]->hp && (guys[i]->team&team)==0 &&
			!(MonsterFlags(guys[i]->type)&(MF_NOHIT|MF_INVINCIBLE)))
		{
			range=abs(x/FIXAMT-(guys[i]->x/FIXAMT))+abs(y/FIXAMT-(guys[i]->y/FIXAMT));
			if((range<bestRange) || (range<320+240 && Random(32)==0))
			{
				if((this->type!=MONS_YOURBOT && this->type!=MONS_CRYOZOID) || map->CheckLOS(mapx,mapy,20,guys[i]->mapx,guys[i]->mapy))
				{
					target=guys[i];
					bestRange=range;
				}
			}
		}
}

// stuff to do when frozen
void Guy::FrozenUpdate(Map *map,world_t *world)
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
		frozen=0;
	}

	if(mindControl)
	{
		if((mindControl%8)==0)
			ParticleBoom(PART_FLOATYPOP,x,y,z+monsType[type].height);
		mindControl--;
		if(!mindControl)
			team=EVIL;
	}

	if(!target || target->team==team || target->type==MONS_NONE || target->hp==0 || Random(50)==0)
	{
		// lost your target one way or another
		FindTarget(map,world);
		if(target==NULL && team==EVIL && goodguy!=NULL)
			target=goodguy;
		if(type==MONS_GHOST && target!=NULL && !map->CheckLOS(mapx,mapy,20,target->mapx,target->mapy))
			target=NULL;
		if(target==NULL && team==GOOD && mindControl>1)
			mindControl--;	// deplete more rapidly when no target around
	}

	if(stun>0 && hp>0)
	{
		if(stun>1 && team==EVIL)
			TalentPoint(TLT_KNOCKOUT,1);
		stun--;
		if(stun==0)
			stun=-10030;
		if(type==player.monsType)	// Loony has many things to do even when stunned
		{
			PlayerControlMe(this,map,&map->map[mapx+mapy*map->width],world);
		}
	}
	else
	{
		if(stun<-10000)
		{
			stun++;
			if(stun==-10000)
				stun=0;
		}
		else if(stun<0)
		{
			stun=-stun;
			if(TerrainIs(mapx,mapy,TF_ICE))
				DoMove(this,ANIM_IDLE,128,0,dx,dy);
			else
				DoMove(this,ANIM_IDLE,128,0,0,0);
		}
		if(type==player.monsType)	// Loony has many things to do even when frozen
		{
			PlayerControlMe(this,map,&map->map[mapx+mapy*map->width],world);
		}
	}

	if(ignited>0)
	{
		map->BrightTorch(mapx,mapy,10,6);
		ignited--;
		if((ignited%30)==0)
		{
			bullet_t *b;
			b=FireBullet(goodguy->ID,x-FIXAMT*40+Random(FIXAMT*80+1),y-FIXAMT*40+Random(FIXAMT*80+1),0,0,BLT_FLAMEB,(word)(1.0f*SpellDamageBoost(SC_FIRE)/100.0f));
			if(b)
			{
				b->canthit=ID;	// can't be hit by your own flames
			}
		}
		if((ignited%60)==0)
		{
			meleeAttacker=NULL;	// get burned slowly
			bulletHittingType=BLT_IGNITER;
			GetShotReal(0,0,CurseDmg(1,GOOD),map,world);
			if(hp==0)
				ouch=5;
		}
	}
	if(ouch>0)
	{
		ouch--;
	}

	oldx=x;
	oldy=y;

	ddx=kbdx;
	ddy=kbdy;

	Clamp(&ddx,FIXAMT*TILE_WIDTH);
	Clamp(&ddy,FIXAMT*TILE_HEIGHT);

	x+=ddx;

	if(!CanWalk(x,y,map,world))
	{
		x-=ddx;
		if(WallBumpMons(type))
			wallBump|=1;	// tell it that it hit a wall
	}
	else if(type==player.monsType && z==0 && !TerrainIs(mapx,mapy,TF_ICE|TF_SKY|TF_WATER|TF_LAVA|TF_CLIFF))
	{
		// check to see if this would put you in water/stuff
		if(TerrainIs((x/(TILE_WIDTH*FIXAMT)),(y/(TILE_HEIGHT*FIXAMT)),TF_SKY|TF_LAVA|TF_WATER))
		{
			// bump if you would enter a danger square
			x-=ddx;
			if(WallBumpMons(type))
				wallBump|=1;	// tell it that it hit a wall
		}
	}

	y+=ddy;

	if(!CanWalk(x,y,map,world))
	{
		y-=ddy;
		if(WallBumpMons(type))
			wallBump|=2;	// tell it that it hit a wall
	}
	else if(type==player.monsType && z==0 && !TerrainIs(mapx,mapy,TF_ICE|TF_SKY|TF_WATER|TF_LAVA|TF_CLIFF))
	{
		// check to see if this would put you in water/stuff
		if(TerrainIs((x/(TILE_WIDTH*FIXAMT)),(y/(TILE_HEIGHT*FIXAMT)),TF_SKY|TF_LAVA|TF_WATER))
		{
			// bump if you would enter a danger square
			y-=ddy;
			if(WallBumpMons(type))
				wallBump|=2;	// tell it that it hit a wall
		}
	}

	if(MonsterFlags(type)&MF_FLYING)
	{
		if(z<20*FIXAMT)	// go up if you need to
			dz+=FIXAMT;
		else
			dz-=FIXAMT;

		Clamp(&dz,FIXAMT*4);
		z+=dz+kbdz;

		if(z<0)
			z=0;
	}
	else
	{
		z+=dz+kbdz;
		if(z<0)
		{
			if(type==player.monsType && dz<-FIXAMT*2)
			{
				if(world->terrain[map->map[mapx+mapy*map->width].floor].flags&TF_ICE)
				{
					if(EquipCount(IT_ICESKATE)==0)
					{
						// splash - broke through the ice
						ExplodeParticles2(PART_WATER,x,y,z,40,6);
						MakeNormalSound(SND_ICECRACK);
						MakeNormalSound(SND_SPLASH);
						// return you to last safe ground
						x=safeX;
						y=safeY;
						dx=0;
						dy=0;
						mapx=(x>>FIXSHIFT)/TILE_WIDTH;
						mapy=(y>>FIXSHIFT)/TILE_HEIGHT;

						meleeAttacker=NULL;
						if(player.parka.magic!=EM_PARACHUTE)
							GetShotReal(0,0,CurseDmg(2,EVIL),map,world);
					}
				}
				else
				{
					dx=0;
					if(TerrainIs(mapx,mapy,TF_SKY))
						dy=-dz;
					else
						dy=0;	// stop dead on landing to avoid the slide effect
					if(EquipCount(IT_SHOVEL))
						Dig(map,mapx,mapy);
				}
			}
			z=0;
			dz=0;
		}
		else
		{
			if(!(MonsterFlags(type)&MF_NOGRAV))
				dz-=FIXAMT*2;
		}
	}

	kbdx=kbdx*3/4;
	kbdy=kbdy*3/4;
	kbdz=kbdz*3/4;

	mapx=(x>>FIXSHIFT)/TILE_WIDTH;
	mapy=(y>>FIXSHIFT)/TILE_HEIGHT;

	if(z==0 && !TerrainIs(mapx,mapy,TF_WATER|TF_LAVA|TF_CLIFF|TF_SKY))
	{
		if(TerrainIs(mapx,mapy,TF_ICE) && !EquipCount(IT_ICESKATE))
		{
			// not safe on ice if you have no skates
		}
		else
		{
			safeX=x;
			safeY=y;
		}
	}

	if(type==player.monsType)	// special case, Loony is the player, follow him
	{
		if(player.levelNum==LVL_THRONE && player.var[VAR_TEMP+2]==1)
		{
			UpdateCamera((32*TILE_WIDTH+TILE_WIDTH/2),(40*TILE_HEIGHT+TILE_HEIGHT/2),facing*32,map);
		}
		else
			UpdateCamera(x>>FIXSHIFT,y>>FIXSHIFT,facing*32,map);
		if((map->flags&MAP_TORCHLIT) && (!WindingDown()))
		{
			b=GetPlayerGlow();
			if(b>32)
				map->BrightTorch(mapx,mapy,b,b/4);
			else
			{
				map->WeakTorch(mapx,mapy);

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
		if((hp>0) && z==0 && (world->terrain[map->map[mapx+mapy*map->width].floor].flags&TF_WATER) && !(itemInfo[map->map[mapx+mapy*map->width].item].flags&IF_WALK))
		{
			// splash
			ExplodeParticles2(PART_WATER,x,y,z,40,6);
			MakeNormalSound(SND_SPLASH);
			// return you to last safe ground
			x=safeX;
			y=safeY;
			dx=0;
			dy=0;
			mapx=(x>>FIXSHIFT)/TILE_WIDTH;
			mapy=(y>>FIXSHIFT)/TILE_HEIGHT;

			meleeAttacker=NULL;
			if(player.parka.magic!=EM_PARACHUTE)
				GetShotReal(0,0,CurseDmg(2,EVIL),map,world);
		}
		if((hp>0) && z==0 && (world->terrain[map->map[mapx+mapy*map->width].floor].flags&TF_LAVA) && !(itemInfo[map->map[mapx+mapy*map->width].item].flags&IF_WALK))
		{
			// splash
			ExplodeParticles2(PART_SMOKE,x,y,z,40,6);
			MakeNormalSound(SND_FALL);
			// return you to last safe ground
			x=safeX;
			y=safeY;
			dx=0;
			dy=0;
			mapx=(x>>FIXSHIFT)/TILE_WIDTH;
			mapy=(y>>FIXSHIFT)/TILE_HEIGHT;

			meleeAttacker=NULL;
			if(player.parka.magic!=EM_PARACHUTE)
				GetShotReal(0,0,CurseDmg(5,EVIL),map,world);
		}
		if((oldmapx!=mapx || oldmapy!=mapy))
		{
			if(world->terrain[map->map[mapx+mapy*map->width].floor].flags&TF_STEP)
			{
				map->map[mapx+mapy*map->width].floor=world->terrain[map->map[mapx+mapy*map->width].floor].next;
			}
			TalentPoint(TLT_MOBILITY,1);
		}
	}
	if(oldmapx!=mapx || oldmapy!=mapy)
	{
		SpecialStepCheck(map,mapx,mapy,this);
		if(type==player.monsType && !(world->terrain[map->map[oldmapx+oldmapy*map->width].floor].flags&TF_ICE) &&
			(world->terrain[map->map[mapx+mapy*map->width].floor].flags&TF_ICE) && z==0)
		{
			if(oldmapx<mapx)
			{
				dx=FIXAMT*6;
				dy=0;
			}
			else if(oldmapx>mapx)
			{
				dx=-FIXAMT*6;
				dy=0;
			}
			else
			{
				dx=0;
				if(oldmapy<mapy)
					dy=FIXAMT*6;
				else
					dy=-FIXAMT*6;
			}
		}
		if(type==player.monsType && TerrainIs(oldmapx,oldmapy,TF_SKY|TF_CLIFF) &&
			!TerrainIs(mapx,mapy,TF_SKY|TF_CLIFF) && z==0)
		{
			dx=0;
			dy=0;
		}
	}
	if(type==player.monsType && TerrainIs(mapx,mapy,TF_SKY|TF_CLIFF) && z==0)
	{
		dx=0;
		dy+=FIXAMT*2;
		Clamp(&dy,FIXAMT*8);
		if(TerrainIs(mapx,mapy,TF_CLIFF) && dy>FIXAMT*3)
			ExplodeParticles2(PART_DIRT,x,y,0,5,4);
	}
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

	if(team!=1 && !(MonsterFlags(type)&MF_NOMOVE) && (player.potionType==POT_FEARBEER || player.potionType==POT_COOLADE) && goodguy)
	{
		if(Distance(x,y,goodguy->x,goodguy->y)<PotionEffect(player.potionType,player.potionPower)*FIXAMT)
		{
			if(player.potionType==POT_FEARBEER)
			{
				b=AngleFrom(x,y,goodguy->x,goodguy->y)+128;
				kbdx+=Cosine(b)*2;
				kbdy+=Sine(b)*2;
			}
			else
			{
				frozen=30;
			}
		}
	}

	if(hp==0 && seq!=ANIM_DIE)
	{
		seq=ANIM_DIE;
		frm=0;
		frmTimer=0;
		frmAdvance=64;
		action=ACTION_BUSY;
	}

	if(mindControl)
	{
		if((mindControl%8)==0)
			ParticleBoom(PART_FLOATYPOP,x,y,z+monsType[type].height);
		mindControl--;
		if(!mindControl)
			team=2;
	}

	if(!target || target->team==team || target->type==MONS_NONE || target->hp==0 || Random(50)==0)
	{
		// lost your target one way or another
		FindTarget(map,world);
		if(target==NULL && team==EVIL && goodguy!=NULL)
			target=goodguy;
		if(type==MONS_GHOST && target!=NULL && !map->CheckLOS(mapx,mapy,20,target->mapx,target->mapy))
			target=NULL;
		if(target==NULL && team==GOOD && mindControl>1)
			mindControl--;	// deplete more rapidly when no target around
	}

	if(stun>0 && hp>0)
	{
		if(stun>1 && team==EVIL)
			TalentPoint(TLT_KNOCKOUT,1);
		stun--;
		if(stun==0)
			stun=-10030;
		if(type==player.monsType)	// Loony has many things to do even when stunned
		{
			PlayerControlMe(this,map,&map->map[mapx+mapy*map->width],world);
		}
	}
	else
	{
		if(type==player.monsType)	// special case, player controls Loony
		{
			PlayerControlMe(this,map,&map->map[mapx+mapy*map->width],world);
		}
		else
		{
			MonsterControl(map,world);
		}
		if(stun<-10000)
		{
			stun++;
			if(stun==-10000)
				stun=0;
		}
		else if(stun<0)
		{
			stun=-stun;
			if(TerrainIs(mapx,mapy,TF_ICE))
				DoMove(this,ANIM_IDLE,128,0,dx,dy);
			else
				DoMove(this,ANIM_IDLE,128,0,0,0);
		}
	}

	if(ignited>0)
	{
		map->BrightTorch(mapx,mapy,10,6);
		ignited--;
		if((ignited%30)==0)
		{
			bullet_t *b;
			b=FireBullet(goodguy->ID,x-FIXAMT*40+Random(FIXAMT*80+1),y-FIXAMT*40+Random(FIXAMT*80+1),0,0,BLT_FLAMEB,(word)(1.0f*SpellDamageBoost(SC_FIRE)/100.0f));
			if(b)
			{
				b->canthit=ID;	// can't be hit by your own flames
			}
		}
		if((ignited%60)==0)
		{
			meleeAttacker=NULL;	// get burned slowly
			bulletHittingType=BLT_IGNITER;
			GetShotReal(0,0,CurseDmg(1,GOOD),map,world);
			if(hp==0)
				ouch=5;
		}
	}
	if(ouch>0)
	{
		ouch--;
	}

	oldx=x;
	oldy=y;

	ddx=dx+kbdx;
	ddy=dy+kbdy;

	Clamp(&ddx,FIXAMT*TILE_WIDTH-FIXAMT);
	Clamp(&ddy,FIXAMT*TILE_HEIGHT-FIXAMT);

	if(frozen && ((type!=player.monsType) || (player.parka.magic!=EM_COOLGUY)))
	{
		ddx/=2;
		ddy/=2;
	}

	x+=ddx;

	if(!CanWalk(x,y,map,world))
	{
		x-=ddx;
		if(WallBumpMons(type))
			wallBump|=1;	// tell it that it hit a wall
	}
	else if(type==player.monsType && z==0 && !TerrainIs(mapx,mapy,TF_ICE|TF_SKY|TF_WATER|TF_LAVA|TF_CLIFF))
	{
		// check to see if this would put you in water/stuff
		if(TerrainIs((x/(TILE_WIDTH*FIXAMT)),(y/(TILE_HEIGHT*FIXAMT)),TF_SKY|TF_LAVA|TF_WATER))
		{
			// bump if you would enter a danger square
			x-=ddx;
			if(WallBumpMons(type))
				wallBump|=1;	// tell it that it hit a wall
		}
	}

	y+=ddy;

	if(!CanWalk(x,y,map,world))
	{
		y-=ddy;
		if(WallBumpMons(type))
			wallBump|=2;	// tell it that it hit a wall
	}
	else if(type==player.monsType && z==0 && !TerrainIs(mapx,mapy,TF_ICE|TF_SKY|TF_WATER|TF_LAVA|TF_CLIFF))
	{
		// check to see if this would put you in water/stuff
		if(TerrainIs((x/(TILE_WIDTH*FIXAMT)),(y/(TILE_HEIGHT*FIXAMT)),TF_SKY|TF_LAVA|TF_WATER))
		{
			// bump if you would enter a danger square
			y-=ddy;
			if(WallBumpMons(type))
				wallBump|=2;	// tell it that it hit a wall
		}
	}

	if(MonsterFlags(type)&MF_FLYING)
	{
		if(z<20*FIXAMT)	// go up if you need to
			dz+=FIXAMT;
		else
			dz-=FIXAMT;

		Clamp(&dz,FIXAMT*4);
		z+=dz+kbdz;

		if(z<0)
			z=0;
	}
	else
	{
		z+=dz+kbdz;
		if(z<0)
		{
			if(type==player.monsType && dz<-FIXAMT*2)
			{
				if(world->terrain[map->map[mapx+mapy*map->width].floor].flags&TF_ICE)
				{
					if(EquipCount(IT_ICESKATE)==0)
					{
						// splash - broke through the ice
						ExplodeParticles2(PART_WATER,x,y,z,40,6);
						MakeNormalSound(SND_ICECRACK);
						MakeNormalSound(SND_SPLASH);
						// return you to last safe ground
						x=safeX;
						y=safeY;
						dx=0;
						dy=0;
						mapx=(x>>FIXSHIFT)/TILE_WIDTH;
						mapy=(y>>FIXSHIFT)/TILE_HEIGHT;

						meleeAttacker=NULL;
						if(player.parka.magic!=EM_PARACHUTE)
							GetShotReal(0,0,CurseDmg(2,EVIL),map,world);
					}
				}
				else
				{
					dx=0;
					if(TerrainIs(mapx,mapy,TF_SKY))
						dy=-dz;
					else
						dy=0;	// stop dead on landing to avoid the slide effect
					if(EquipCount(IT_SHOVEL))
						Dig(map,mapx,mapy);
				}
			}
			z=0;
			dz=0;
		}
		else
		{
			if(!(MonsterFlags(type)&MF_NOGRAV))
				dz-=FIXAMT*2;
		}
	}

	kbdx=kbdx*3/4;
	kbdy=kbdy*3/4;
	kbdz=kbdz*3/4;

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

	if(z==0 && !TerrainIs(mapx,mapy,TF_WATER|TF_LAVA|TF_CLIFF|TF_SKY))
	{
		if(TerrainIs(mapx,mapy,TF_ICE) && !EquipCount(IT_ICESKATE))
		{
			// not safe on ice if you have no skates
		}
		else
		{
			safeX=x;
			safeY=y;
		}
	}

	if(type==player.monsType)	// special case, Loony is the player, follow him
	{
		if(player.levelNum==LVL_THRONE && player.var[VAR_TEMP+2]==1)
		{
			UpdateCamera((32*TILE_WIDTH+TILE_WIDTH/2),(40*TILE_HEIGHT+TILE_HEIGHT/2),facing*32,map);
		}
		else
			UpdateCamera(x>>FIXSHIFT,y>>FIXSHIFT,facing*32,map);
		if((map->flags&MAP_TORCHLIT) && (!WindingDown()))
		{
			b=GetPlayerGlow();
			if(b>32)
				map->BrightTorch(mapx,mapy,b,b/4);
			else
			{
				map->WeakTorch(mapx,mapy);

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
		if((hp>0) && z==0 && (world->terrain[map->map[mapx+mapy*map->width].floor].flags&TF_WATER) && !(itemInfo[map->map[mapx+mapy*map->width].item].flags&IF_WALK))
		{
			// splash
			ExplodeParticles2(PART_WATER,x,y,z,40,6);
			MakeNormalSound(SND_SPLASH);
			// return you to last safe ground
			x=safeX;
			y=safeY;
			dx=0;
			dy=0;
			mapx=(x>>FIXSHIFT)/TILE_WIDTH;
			mapy=(y>>FIXSHIFT)/TILE_HEIGHT;

			meleeAttacker=NULL;
			if(player.parka.magic!=EM_PARACHUTE)
				GetShotReal(0,0,CurseDmg(2,EVIL),map,world);
		}
		if((hp>0) && z==0 && ((world->terrain[map->map[mapx+mapy*map->width].floor].flags&TF_LAVA) ||
			((world->terrain[map->map[mapx+mapy*map->width].floor].flags&(TF_CLIFF|TF_SKY)) && mapy==map->height-1))
			&& !(itemInfo[map->map[mapx+mapy*map->width].item].flags&IF_WALK))
		{
			// splash
			ExplodeParticles2(PART_SMOKE,x,y,z,40,6);
			MakeNormalSound(SND_FALL);
			// return you to last safe ground
			x=safeX;
			y=safeY;
			dx=0;
			dy=0;
			mapx=(x>>FIXSHIFT)/TILE_WIDTH;
			mapy=(y>>FIXSHIFT)/TILE_HEIGHT;

			meleeAttacker=NULL;
			if(player.parka.magic!=EM_PARACHUTE)
				GetShotReal(0,0,CurseDmg(5,EVIL),map,world);
		}
		if((oldmapx!=mapx || oldmapy!=mapy))
		{
			if(world->terrain[map->map[mapx+mapy*map->width].floor].flags&TF_STEP)
			{
				map->map[mapx+mapy*map->width].floor=world->terrain[map->map[mapx+mapy*map->width].floor].next;
			}
			TalentPoint(TLT_MOBILITY,1);
		}
	}
	if(oldmapx!=mapx || oldmapy!=mapy)
	{
		SpecialStepCheck(map,mapx,mapy,this);
		if(type==player.monsType && !(world->terrain[map->map[oldmapx+oldmapy*map->width].floor].flags&TF_ICE) &&
			(world->terrain[map->map[mapx+mapy*map->width].floor].flags&TF_ICE) && z==0)
		{
			if(oldmapx<mapx)
			{
				dx=FIXAMT*6;
				dy=0;
			}
			else if(oldmapx>mapx)
			{
				dx=-FIXAMT*6;
				dy=0;
			}
			else
			{
				dx=0;
				if(oldmapy<mapy)
					dy=FIXAMT*6;
				else
					dy=-FIXAMT*6;
			}
		}
		if(type==player.monsType && TerrainIs(oldmapx,oldmapy,TF_SKY|TF_CLIFF) &&
			!TerrainIs(mapx,mapy,TF_SKY|TF_CLIFF) && z==0)
		{
			dx=0;
			dy=0;
		}
	}
	if(type==player.monsType && TerrainIs(mapx,mapy,TF_SKY|TF_CLIFF) && z==0)
	{
		dx=0;
		dy+=FIXAMT*2;
		Clamp(&dy,FIXAMT*8);
		if(TerrainIs(mapx,mapy,TF_CLIFF) && dy>FIXAMT*3)
			ExplodeParticles2(PART_DIRT,x,y,0,5,4);
	}
}

// this is a very half-assed version of update to be called by the editor instead of the game
void Guy::EditorUpdate(Map *map)
{
	byte b;

	mapx=(x>>FIXSHIFT)/TILE_WIDTH;
	mapy=(y>>FIXSHIFT)/TILE_HEIGHT;
	if(mapx>=0 && mapx<map->width && mapy>=0 && mapy<map->height)
		bright=map->map[mapx+mapy*map->width].templight;
	else
		bright=0;

	if(type==player.monsType)
	{
		if((map->flags&MAP_TORCHLIT) && (!WindingDown()))
		{
			b=GetPlayerGlow();
			if(b>32)
				map->BrightTorch(mapx,mapy,b,b/4);
			else
			{
				map->WeakTorch(mapx,mapy);

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
	}
}

void Guy::Render(byte light)
{
	word f;

	f=monsType[type].flags;
	if(!editing && mapx>=0 && mapx<curMap->width && mapy>=0 && mapy<curMap->height && TerrainIs(mapx,mapy,TF_SKY))
	{
		int i,newY;
		newY=y;
		for(i=mapy;i<curMap->height;i++)
		{
			if(!TerrainIs(mapx,i,TF_SKY))
			{
				newY=i*TILE_HEIGHT*FIXAMT;
				break;
			}
		}
		MonsterDrawShadow(x,newY,0,type,seq,frm,facing);
		monsType[type].flags|=MF_NOSHADOW;
	}
	MonsterDraw(x,y,z,type,seq,frm,facing,bright*(light>0),ouch,(int)frozen,(byte)(poison>0));
	monsType[type].flags=f;
}

void Guy::MonsterControl(Map *map,world_t *world)
{
	if(target==NULL && !(monsType[type].location&ML_ALWAYSACTIVE) && type!=MONS_BUNNYBOSS && type!=MONS_BUNNYTROOP && type!=MONS_BUNNYMAJOR && type!=MONS_THUG && type!=MONS_JUNKSMITH && type!=MONS_BRUISER && type!=MONS_GHOST && type!=MONS_VILLAGER && type!=MONS_BONEHEAD && type!=MONS_AXETRAP && type!=MONS_CRYOZOID && type!=MONS_TOASTY && type!=MONS_CARAVAN && type!=MONS_YOURBOT && type!=MONS_LIFEBERRY && type!=MONS_BIGPLANT && (type!=MONS_MOUSE4 || team==EVIL) && type!=MONS_PROTOTYPE && type!=MONS_MIMIC)
		return;

	if(type==MONS_GHOST && seq!=ANIM_DIE && goodguy && RangeToTarget(this,goodguy)>FIXAMT*600)
		DoMove(this,ANIM_DIE,128,1,0,0);

	monsType[type].aiFunc(this,map,world,target);
}

void Guy::GetStunned(word amt)
{
	if(stun<-10000)
		return;
	if(stun>=1)
		return;
	if(hp==0)
		return;
	if(type==0)
		return;
	if(type==player.monsType && player.invinc>5)
		return;
	if(monsType[type].flags&(MF_INVINCIBLE|MF_NOHIT))
		return;
	if(type==MONS_UBERTITAN && player.var[VAR_TEMP]<3)
		return;	// no stunning until the job is done!
	if(type==MONS_TOYPOWER || type==MONS_TOYCRYSTAL)
		return;
	if(monsType[type].location&ML_BOSS)
	{
		amt/=2;
		if(amt==0)
			return;
	}
	if(team==EVIL)
	{
		stun=stun*(int)(100+TalentBonus(TLT_KNOCKOUT))/100;
	}

	if(stun<3 && amt>3)
		FireBullet(ID,x,y+1,z+monsType[type].height*FIXAMT,0,BLT_STUN,0);
	if(type==player.monsType && player.spclMove==SM_WHIRL)
	{
		player.spclMove=SM_NONE;
		player.whirlClock=0;
	}
	stun=-amt;
}

void Guy::GetPoisoned(word amt)
{
	if(hp==0)
		return;
	if(monsType[type].flags&(MF_INVINCIBLE|MF_NOHIT))
		return;
	if(type==player.monsType && player.parka.magic==EM_CURING)
		amt/=2;
	if(type==player.monsType && player.invinc>5)
		return;

	if(team==EVIL && GlassEffect()==GE_POISON)
	{
		amt=(word)((float)amt*(100.0f+(float)GlassAmount())/100.0f);
	}
	if(monsType[type].location&ML_BOSS)
	{
		amt/=2;
		if(amt==0)
			return;
	}

	if(amt>poison)
		poison=amt;
}

void Guy::GetFrozen(word amt)
{
	if(hp==0)
		return;
	if(type==player.monsType && player.invinc>5)
		return;
	if(monsType[type].flags&(MF_INVINCIBLE|MF_NOHIT))
		return;
	if(type==player.monsType && player.parka.magic==EM_WARM)
		amt/=2;
	if(type==player.monsType && player.parka.magic==EM_WARM2)
		return;	// immune!
	if(type==MONS_UBERTITAN)
		return;	// immune
	if(team==EVIL && GlassEffect()==GE_FREEZE)
	{
		amt=(word)((float)amt*(100.0f+(float)GlassAmount())/100.0f);
	}
	if(monsType[type].location&ML_BOSS)
	{
		amt/=2;
		if(amt==0)
			return;
	}

	if(amt>frozen)
		frozen=amt;
}

void GuyFact(Guy *g)
{
	char s[64],s2[32];
	static dword d;

	if(player.var[VAR_JUSTHEF]<45)
		return;

	player.var[VAR_JUSTHEF]=0;

	sprintf(s,"%s ",monsType[g->type].name);
	switch(rand()%4)
	{
		case 0:
			sprintf(s2,"is level %d.",monsType[g->type].level);
			break;
		case 1:
			sprintf(s2,"has %d Armor.",monsType[g->type].armor);
			break;
		case 2:
			sprintf(s2,"has %d/%d Life.",g->hp,MonsterHP(g->type,g->team));
			break;
		case 3:
			sprintf(s2,"does %d damage.",monsType[g->type].damage);
			break;
	}
	strcat(s,s2);
	NewMessage(s,0);

}

byte Guy::GetShot(int dx,int dy,word damage,Map *map,world_t *world)
{
	int armor;

	if(meleeAttacker!=NULL)
		bulletHittingType=0;

	if(type==MONS_BIRD && player.var[VAR_BIRDCHAT]==0 && (levelDef[player.levelNum].flags&LF_ARENA)==0)
		return 0;

	if(hp==0)
		return 0;	// can't shoot a dead guy

	if(type==0)
		return 0;	// shouldn't have a type 0 guy at all

	if(damage==0)
		return 1;

	if(type==player.monsType && player.invinc)
		return 0; // invincible when shielded

	if(MonsterFlags(type)&MF_INVINCIBLE)
	{
		return 0;	// invincible
	}

	if((type==MONS_BUNNYBOSS || type==MONS_BUNNYBOSS2) && seq==ANIM_A1)
	{
		DoMove(this,ANIM_A2,128,1,0,0);	// block and retaliate!
		MakeSound(SND_PARRY,x,y,SND_CUTOFF|SND_RANDOM,1200);
		if(meleeAttacker!=NULL)
			FaceGoodguy(this,meleeAttacker);
		return 0;
	}

	if(type==MONS_TEDDY5)	// retaliate for any damage incurred
	{
		LightningBolt(x,y-FIXAMT*20,goodguy->x,goodguy->y-FIXAMT*20);
		goodguy->GetShot(0,0,damage,map,world);
	}
	if(type==player.monsType)
	{
		if(world->terrain[map->map[mapx+mapy*map->width].floor].flags&TF_ICE)
		{
			dx=0;
			dy=0;
		}
		player.stone=0;

		armor=player.armor;

		// dodge the attack!
		if((rand()%100)<SkillDodgeRate())
		{
			if(player.invinc<5)
				player.invinc=5;
			// add dodge sound and effects here
			MakeSound(SND_DODGE,x,y,SND_CUTOFF|SND_RANDOM,600);
			TalentPoint(TLT_FINESSE,1);
			if(meleeAttacker!=NULL)
			{
				if(TalentBonus(TLT_FINESSE)>0)
					meleeAttacker->GetStunned((word)TalentBonus(TLT_FINESSE));
			}
			return 0;
		}
		if(player.parry && player.var[VAR_PARRYCLOCK]==0)
		{
			// kaching!
			MakeSound(SND_PARRY,x,y,SND_CUTOFF|SND_RANDOM,600);
			player.parry=0;
			TalentPoint(TLT_FINESSE,1);
			if(meleeAttacker!=NULL)
			{
				if(TalentBonus(TLT_FINESSE)>0)
					meleeAttacker->GetStunned((word)TalentBonus(TLT_FINESSE));
			}
			player.var[VAR_PARRYCLOCK]=(byte)(30*2-SpecificSkillVal(1,SKILL_PARRY));
			return 0;
		}
		if((player.parka.magic==EM_STONE && (rand()%100)<5) || (player.parka.magic==EM_STONE2 && (rand()%100)<10))
		{
			if(player.invinc<5)
				player.invinc=5;
			MakeSound(SND_PARRY,x,y,SND_CUTOFF|SND_RANDOM,600);
			TalentPoint(TLT_FINESSE,1);
			if(meleeAttacker!=NULL)
			{
				if(TalentBonus(TLT_FINESSE)>0)
					meleeAttacker->GetStunned((word)TalentBonus(TLT_FINESSE));
			}
			return 0;
		}
		if(HeatShieldOn() && meleeAttacker!=NULL)
		{
			meleeAttacker->GetShot(0,0,(word)(SpecificSkillVal(0,SKILL_HEATSHIELD)*SpellDamageBoost(SC_FIRE)/100.0f),map,world);
		}
		if(player.parka.magic==EM_THORNY2)
		{
			int i;

			for(i=0;i<4;i++)
			{
				FireBullet(ID,x,y,z+FIXAMT*20,rand()%256,BLT_AXESHARD,bulDef[BLT_AXESHARD].damage);
			}
		}
		if(player.parka.magic==EM_EXPLOSIVE)
		{
			FireBullet(ID,x,y,z,rand()%256,BLT_SMALLBOOM,DamageAmt(4,GOOD));
		}
		if(player.parka.magic==EM_FROSTY2)
		{
			if(meleeAttacker)
				meleeAttacker->GetFrozen(30);
		}
		if(player.parka.magic==EM_ELECTRIC)
		{
			if(meleeAttacker)
			{
				Guy *m=meleeAttacker;
				meleeAttacker=NULL;
				bulletHittingType=0;
				m->GetShotReal(0,0,ShockDmg(5,GOOD),curMap,world);
				TalentPoint(TLT_ELECTROCUTION,5);
				LightningBolt(x,y-z,m->x,m->y-FIXAMT*20);
				meleeAttacker=m;
			}
		}
		if(player.amulet.magic==EM_ABSORBING || player.amulet.magic==EM_ABSORB2)
		{
			if(player.magic<player.maxMagic)
				player.magic++;
			if(player.amulet.magic==EM_ABSORB2 && player.magic<player.maxMagic)
				player.magic++;
		}
#ifdef DIRECTORS
		if(player.parka.magic==EM_DOOM)
		{
			if(meleeAttacker)
			{
				meleeAttacker->GetFrozen(30);
				meleeAttacker->GetStunned(30);
				meleeAttacker->GetPoisoned(30);
			}
			FireBullet(ID,x,y,0,0,BLT_KABOOM,DamageAmt(20,GOOD));
			MakeSound(SND_BOOM,x,y,SND_CUTOFF,300);
		}
#endif
	}
	else
		armor=monsType[type].armor;

	if(team==EVIL && player.var[VAR_MADCAP])
		armor*=MADCAP_ARMBOOST;
	if(team==EVIL && ModifierOn(MOD_TRAMPLE))
		armor*=2;
	if(team==GOOD)
		armor=(armor*(100+10*player.var[VAR_MADGEM+MC_SHIELD])/100);

	if(ignited)	// ignited enemies have reduced armor
		armor=armor*(int)((100.0f-SpecificSkillVal(1,SKILL_IGNITE))/100.0f);

	if(type==player.monsType && player.parka.magic==EM_OLDLACE && poison)
		armor+=5;

	if(player.axe.magic==EM_PIERCING && team==EVIL && (meleeAttacker==goodguy || bulletHittingType==BLT_AXE || bulletHittingType==BLT_MINIAXE))
		armor=(armor*80)/100;
	if(player.axe.magic==EM_PIERCE2 && team==EVIL && (meleeAttacker==goodguy || bulletHittingType==BLT_AXE || bulletHittingType==BLT_MINIAXE))
		armor=(armor*60)/100;

	if(team==EVIL && poison)
	{
		// poisoned guys have reduced armor based on Arsenic talent
		armor=(int)(((float)armor*(100.0f-TalentBonus(TLT_ARSENIC))/100.0f)+0.5f);
	}
	if(player.axeMode==0 && meleeAttacker==goodguy)	// using the axe, apply Chopping talent
		armor=(int)(((float)armor*(100.0f-TalentBonus(TLT_CHOPPING))/100.0f)+0.5f);
	if(type==player.monsType)
	{
		armor=(int)(((float)armor*(100.0f+TalentBonus(TLT_STONESKIN))/100.0f)+0.5f);
	}
	if(team==EVIL && isCritical)
	{
		armor=(int)(((float)armor*(100.0f-TalentBonus(TLT_CRITICISM))/100.0f)+0.5f);
	}

	if(team==EVIL && hp==MonsterHP(type,team) && (bulletHittingType==BLT_AXE || bulletHittingType==BLT_MINIAXE ||
		meleeAttacker==goodguy))
	{
		if(meleeAttacker==NULL)	// throwing gets half the bonus
			damage+=(int)((TalentBonus(TLT_SNEAKATTACK)+0.5f)/2.0f);
		else
			damage+=(int)TalentBonus(TLT_SNEAKATTACK);
	}

	if(damage>armor)
		damage-=armor;
	else
		damage=1;

	if(!(MonsterFlags(type)&MF_NOMOVE))
	{
		if(type!=player.monsType || player.spclMove!=SM_WHIRL)
		{
			kbdx+=dx;
			kbdy+=dy;
			if(world->terrain[map->map[mapx+mapy*map->width].floor].flags&TF_ICE)
				GetStunned(1);
		}
	}

	if(type==player.monsType && player.potionType==POT_STONESOUP)
	{
		armor=damage-PotionEffect(player.potionType,player.potionPower);
		if(armor<0)
			damage=0;
		else
			damage=(byte)armor;
	}
	if(type==player.monsType && player.potionType==POT_ALIENBLOOD)
	{
		for(armor=0;armor<4;armor++)
			FireBullet(ID,x,y,z+FIXAMT*20,rand()%256,BLT_ALIENBLD,DamageAmt(PotionEffect(player.potionType,player.potionPower),GOOD));
	}

	if(type==player.monsType)
		ToastyBoom();

	if(type==player.monsType && player.amulet.magic==EM_CURSED)
	{
		if(player.magic>0)
			player.magic--;
	}

	if(type==player.monsType && player.parka.magic==EM_LINKIN)
	{
		if(player.berserkClock<60*2)
			player.berserkClock=60*2;
	}

	if(type==MONS_YOURBOT && player.var[VAR_CLOCKBOT-IT_LOG+IT_GOLD])
	{
		FireBullet(ID,x,y,0,0,BLT_CLOCKBOOM2,(1+2*(ModifierOn(MOD_TICKING)))*DamageAmt(player.var[VAR_CLOCKBOT-IT_LOG+IT_GOLD]*2,GOOD));
	}

	if(team==EVIL && player.axe.magic==EM_JUSTHEF)
		GuyFact(this);

	if(team==EVIL && player.parka.magic==EM_PARALLEL && (rand()%100)==1 && !(monsType[type].location&ML_BOSS) && tag==0)
	{
		Guy *g;

		g=AddGuy(x,y,z,type);
		if(g)
		{
			g->hp=hp/2;
			hp/=2;
			if(g->hp==0)
				g->hp=1;
			if(hp==0)
				hp=1;
		}
	}
	if(team==EVIL && PlagueOn())
		GetPoisoned(60);

	GetShotReal(dx,dy,damage,map,world);
	return 1;
}

void Guy::GetShotReal(int dx,int dy,word damage,Map *map,world_t *world)
{
	int formerHP;

	if(meleeAttacker!=NULL)
		bulletHittingType=0;


	if(team==EVIL && (monsType[type].location&ML_BOSS)==0 && player.axe.magic==EM_BANHAMMER && (rand()%100)<5 &&
		(bulletHittingType==BLT_AXE || bulletHittingType==BLT_MINIAXE || (meleeAttacker!=NULL && meleeAttacker==goodguy)))
	{
		// banninate!
		type=MONS_NONE;
		ParticleBoom(PART_SMOKE,x,y,z);
		NumberParticle2(0,x,y+1,4,PART_BAN);
		return;
	}

	if(type==MONS_BIRD && player.var[VAR_BIRDCHAT]==0 && (levelDef[player.levelNum].flags&LF_ARENA)==0)
		return;

	if(type==MONS_FROSTGATE && (meleeAttacker==NULL || meleeAttacker->type!=MONS_PROTOTYPE))
		return;	// can only be hurt by prototype puppets

	if(hp==0)
		return;	// can't shoot a dead guy

	if(type==0)
		return;	// shouldn't have a type 0 guy at all

	if(type==player.monsType && player.invinc)
		return; // invincible when shielded

	if(team==EVIL && player.amulet.magic==EM_SUNANGEL && (bulletHittingType==BLT_BURNFLAME2 || bulletHittingType==BLT_FLAMEB2 || bulletHittingType==BLT_FLAME2))
		damage*=2;	// double flame damage!

	if(type==MONS_YOURBOT && ModifierOn(MOD_TICKING))
		damage/=2;	// half damage in this mode

	if(ModifierOn(MOD_EASY) && !ModifierOn(MOD_HARD))
	{
		if(team==EVIL)
			damage*=2;
		else
			damage/=2;
		if(damage<1)
			damage=1;
	}
	if(ModifierOn(MOD_HARD) && !ModifierOn(MOD_EASY))
	{
		if(team==EVIL)
			damage/=2;
		else
			damage*=2;
		if(damage<1)
			damage=1;
	}

	if(MonsterFlags(type)&MF_INVINCIBLE)
	{
		return;	// invincible
	}

	formerHP=hp;
	ouch=4;
	hp-=damage;

	if(team==EVIL)
		player.damageDone+=damage;

	// fire or explosion damage done to an enemy boost Combustion talent
	if(team==EVIL && meleeAttacker==NULL && (bulletHittingType==BLT_FLAME2 || bulletHittingType==BLT_AXEBOOM || bulletHittingType==BLT_BONEBOOM ||
		bulletHittingType==BLT_FLAMEB2 || bulletHittingType==BLT_KABOOM || bulletHittingType==BLT_PFIREBALL || bulletHittingType==BLT_IGNITER ||
		bulletHittingType==BLT_SMALLBOOM || bulletHittingType==BLT_BURNINATE || bulletHittingType==BLT_BURNFLAME2 || bulletHittingType==BLT_PFIREBALL2))
		TalentPoint(TLT_COMBUSTION,damage);

	if(type==player.monsType && player.amulet.magic==EM_SAFETY && formerHP>1 && hp<=0)
	{
		hp=1;	// can't die!
	}

	if(type==player.monsType && player.potionType==POT_SOULA && hp<=0)
	{
		hp=1;	// can't die!
	}

	if(type==player.monsType && EquipCount(IT_SOUNDING))
	{
		DropItemOfType(IT_SOUNDING,255);
		NewMessage("The Sounding Crystal broke!",0);
		MakeNormalSound(SND_CRYSTALBREAK);
	}

	if(type==MONS_YOURBOT)
	{
		if(hp>=0)
		{
			player.var[VAR_CLOCKLIFE]=hp&0xFF;
			player.var[VAR_CLOCKLIFE2]=(hp>>8);
		}
		else
		{
			player.var[VAR_CLOCKLIFE]=0;
			player.var[VAR_CLOCKLIFE2]=0;
		}
	}

	if(team==EVIL)
		NumberParticle(damage,x,y+1,4);
	else if(type==player.monsType)
		NumberParticle(damage,x,y+1,5);
	else
		NumberParticle(damage,x,y+1,3);

	if(type==MONS_BUNNYMAJOR && hp<=0)
		hp=1;
	if(type==player.monsType && hp<=0 && player.levelNum==LVL_CAPTAIN && player.var[VAR_SNUGGLY]==0 &&
		player.var[VAR_QUESTDONE+QUEST_LEADER] && !player.var[VAR_QUESTASSIGN+QUEST_FURNACE])
	{
		player.var[VAR_QUESTASSIGN+QUEST_FURNACE]=1;
		hp=1;
	}
	if(hp<=0)
	{
		hp=0;
		seq=ANIM_DIE;
		if(ModifierOn(MOD_HAUNTED) && team==EVIL && (type<MONS_BADGHOST || type>MONS_BADGHOST5))
		{
			byte g;

			// not using MonsterLevel, because we don't want this level boosted by Madcap mode
			g=(monsType[type].level/10)+MONS_BADGHOST;
			if(g>MONS_BADGHOST5)
				g=MONS_BADGHOST5;

			AddGuy(x,y,0,g);
		}

		this->dx=0;
		this->dy=0;

		frm=0;
		frmAdvance=64;
		frmTimer=0;
		action=ACTION_BUSY;
		reload=0;

		if(type==player.monsType && player.var[VAR_DEATHS]<255)
			player.var[VAR_DEATHS]++;

		if(team==EVIL && PlagueOn() && Distance(x,y,goodguy->x,goodguy->y)<=(int)(SpecificSkillVal(1,SKILL_PLAGUE)*FIXAMT))
		{
			Guy *g;
			g=SummonGuy(x,y,FIXAMT*40,MONS_MOUSE4);
			if(g)
			{
				g->team=GOOD;
				g->tag=255;
			}
		}
		// earn XP
		if(type!=player.monsType && (team==EVIL || mindControl) && (meleeAttacker==NULL || (meleeAttacker->type!=MONS_CARAVAN && meleeAttacker->type!=MONS_THUG &&
			meleeAttacker->type!=MONS_BRUISER && meleeAttacker->type!=MONS_BUNNYTROOP && meleeAttacker->type!=MONS_EVILBOT)))
		{
			switch(type)
			{
				case MONS_BADGER:
				case MONS_BADGER2:
				case MONS_BADGER3:
					PlayerAddKill(KC_BODZHA);
					break;
				case MONS_TEDDY:
				case MONS_TEDDY2:
				case MONS_TEDDY3:
				case MONS_TEDDY4:
				case MONS_TEDDY5:
					PlayerAddKill(KC_BEAR);
					break;
				case MONS_MOUSE:
				case MONS_MOUSE2:
				case MONS_MOUSE3:
				case MONS_ICEMICE:
				case MONS_MOUSE4:
				case MONS_MOUSE5:
					PlayerAddKill(KC_MICE);
					break;
				case MONS_PUPPET:
				case MONS_PUPPET2:
				case MONS_PUPPET3:
				case MONS_FRUITPUNCH:
					PlayerAddKill(KC_PUPPET);
					break;
				case MONS_SHROOM:
					PlayerAddKill(KC_SHROOM);
					break;
				case MONS_TINGUY:
				case MONS_TINGUY2:
				case MONS_TRIGUN:
				case MONS_TINPOT:
				case MONS_ARMYONE:
					PlayerAddKill(KC_SOLDIER);
					break;
				case MONS_MONKEY:
				case MONS_MONKEY2:
				case MONS_MONKEY3:
				case MONS_MONKEY4:
					PlayerAddKill(KC_MONKEY);
					break;
				case MONS_ROSE:
				case MONS_ROSE2:
				case MONS_LION:
				case MONS_BURNINGBUSH:
				case MONS_BIGPLANT:
				case MONS_ROSE3:
				case MONS_LION2:
				case MONS_ROSE4:
				case MONS_ROSE5:
				case MONS_DEATHBERRY:
					PlayerAddKill(KC_PLANT);
					break;
				case MONS_HOUND:
				case MONS_TITAN:
				case MONS_TITAN2:
				case MONS_UBERTITAN:
				case MONS_ICEBOX:
					PlayerAddKill(KC_ICE);
					break;
				case MONS_BRUISER:
				case MONS_THUG:
				case MONS_JUNKSMITH:
					PlayerAddKill(KC_ONION);
					break;
				case MONS_BUNNYTROOP:
				case MONS_BUNNYMAJOR:
				case MONS_BUNNYBOSS:
				case MONS_BUNNYBOSS2:
				case MONS_EVILBOT:
					PlayerAddKill(KC_BUNNY);
					break;
				case MONS_GHOSTDOG:
				case MONS_GHOSTMONKEY:
				case MONS_BADGHOST:
				case MONS_BADGHOST2:
				case MONS_BADGHOST3:
				case MONS_BADGHOST4:
				case MONS_BADGHOST5:
					PlayerAddKill(KC_GHOST);
					break;
				case MONS_CRAB:
				case MONS_STONEWALL:
				case MONS_SOCKCRAB:
					PlayerAddKill(KC_STONE);
					break;
			}
			if(tag!=255)	// no XP for resurrected guys
			{
				if(!(levelDef[player.levelNum].flags&LF_ARENA) && (monsType[type].location&ML_BOSS))
					PlayerGetBossPoints(MonsterLevel(type));
				else
				{
					if(type==MONS_JACK)
						PlayerGetMonsterPoints(1000);
					else
						PlayerGetMonsterPoints(MonsterLevel(type));
				}
				// drop items!
				if(!(levelDef[player.levelNum].flags&LF_ARENA) && (type<MONS_BADGHOST || type>MONS_BADGHOST5))
				{
					deathTag=tag;
					BadguyDrop(type,MonsterLevel(type),x,y);
					if(type==MONS_TITAN2)
						PlayerSetVar(VAR_TITANIC+tag-1,1);
				}
				else if((levelDef[player.levelNum].flags&LF_ARENA) && (type<MONS_BADGHOST || type>MONS_BADGHOST5))
				{
					ArenaKill(type,x,y);
				}
				if(player.var[VAR_PROVEKILLS]<255)
					PlayerSetVar(VAR_PROVEKILLS,player.var[VAR_PROVEKILLS]+1);
				QuestReportEvent(QE_KILL,type);
			}
			if(player.levelNum==LVL_MICKEYCELLAR && player.var[VAR_RATS])	// mickey's cellar, count down rat kills
				PlayerSetVar(VAR_RATS,player.var[VAR_RATS]-1);
			if(player.axeMode==0 && meleeAttacker==goodguy)	// killed them with the axe
				TalentPoint(TLT_CHOPPING,1);
			if(formerHP==MonsterHP(type,team) && tag!=255 && (bulletHittingType==BLT_AXE || bulletHittingType==BLT_MINIAXE || meleeAttacker==goodguy))
				TalentPoint(TLT_SNEAKATTACK,1);	// killed in one hit
			if(meleeAttacker==NULL)
			{
				if(bulletHittingType==BLT_PFIREBALL || bulletHittingType==BLT_IGNITER || bulletHittingType==BLT_BURNINATE || bulletHittingType==BLT_BURNFLAME2 || bulletHittingType==BLT_PFIREBALL2)
					TalentPoint(TLT_SCORCHING,damage);
				if(bulletHittingType==BLT_AXE || bulletHittingType==BLT_MINIAXE || bulletHittingType==BLT_AXEBOOM ||
					bulletHittingType==BLT_AXESHARD)	// killed them with the throwing axe
					TalentPoint(TLT_TOSSING,1);
				if(bulletHittingType==BLT_DRAINMASTER || bulletHittingType==BLT_STONESPIKE || bulletHittingType==BLT_CRYO ||
					bulletHittingType==BLT_IGNITER || bulletHittingType==BLT_SMALLBOOM || bulletHittingType==BLT_PFIREBALL ||
					bulletHittingType==BLT_SKELSHOT || bulletHittingType==BLT_BURNINATE || bulletHittingType==BLT_BURNFLAME2 ||
					bulletHittingType==BLT_GALE2 || bulletHittingType==BLT_BONEBOOM || bulletHittingType==BLT_PFIREBALL2 || bulletHittingType==BLT_COLDWAVE ||
					bulletHittingType==BLT_SHOCKORB || bulletHittingType==BLT_ICECUBE)
					TalentPoint(TLT_FOCUS,1);
				if(bulletHittingType==BLT_DRAINMASTER || bulletHittingType==BLT_SKELSHOT || bulletHittingType==BLT_POISON || bulletHittingType==BLT_BONEBOOM)
				{
					TalentPoint(TLT_SPOOKYBONES,monsType[type].level);
					if(TalentBonus(TLT_SPOOKYBONES))
						AddGuy(x,y,0,MONS_GHOST);
				}
				if(bulletHittingType==BLT_CLOCKBULLET || bulletHittingType==BLT_CLOCKBOOM || bulletHittingType==BLT_CLOCKBOOM2)
				{
					TalentPoint(TLT_CLOCKWORK,1);
					RepairClockbot(player.var[VAR_CLOCKBOT-IT_LOG+IT_CAN]);
				}
			}
			else if(meleeAttacker->type==MONS_GHOST || meleeAttacker->type==MONS_BONEHEAD || (meleeAttacker->type==MONS_MOUSE4 && meleeAttacker->mindControl==0))
			{
				TalentPoint(TLT_SPOOKYBONES,monsType[type].level);
				if(TalentBonus(TLT_SPOOKYBONES))
					AddGuy(x,y,0,MONS_GHOST);
			}
			else if(meleeAttacker->type==MONS_YOURBOT)
			{
				TalentPoint(TLT_CLOCKWORK,1);
				RepairClockbot(player.var[VAR_CLOCKBOT-IT_LOG+IT_CAN]);
			}
#ifdef DIRECTORS
			if(meleeAttacker!=NULL && meleeAttacker->type==MONS_LILBADGER)
			{
				picToDrop=2;
				if(opt.galPix[picToDrop-1]==0 && player.var[VAR_TEMP+15]==0)
				{
					bullet_t *b;

					b=FireBullet(goodguy->ID,goodguy->x,goodguy->y,FIXAMT*15,64,BLT_ITEM,0);
					if(b)
					{
						player.var[VAR_TEMP+15]=1;
						b->anim=IT_PICTURE;
						b->target=picToDrop;
						NewMessage("Way to go, tiny Bodzha!",0);
					}
				}
				picToDrop=0;
			}
#endif
			if(ModifierOn(MOD_VAMPIRE))
				HealGoodguy(3);
			if((rand()%100)<(int)TalentBonus(TLT_SCAVENGING))
				HealGoodguy(RecoverAmt(1));
		}
	}
	if(type!=player.monsType && type!=MONS_YOURBOT)
	{
		lifeGuy=this;
		if(type==MONS_SHROOM)
			ShowEnemyLife(MonsterName(type),formerHP*128/MonsterHP(originalType,team),hp*128/MonsterHP(originalType,team),(byte)(hp>0));
		else
			ShowEnemyLife(MonsterName(type),formerHP*128/MonsterHP(type,team),hp*128/MonsterHP(type,team),(byte)(hp>0));
	}
	else if(type==player.monsType)
	{
		float s;

		s=(damage*2.0f*(100.0f-SpecificSkillVal(0,SKILL_ENDURANCE)))/100.0f;
		if(player.stamina>=(byte)s)
			player.stamina-=(byte)s;
		else
			player.stamina=0;

		TalentPoint(TLT_STONESKIN,(word)damage);
		SetPlayerHP(hp);
		player.invinc=5;
		if(player.amulet.magic==EM_GUARDIAN)
			player.invinc=15;

		if(player.spclMove!=SM_WHIRL)
			player.spclMove=0;
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
	int i,j;
	static byte filmflip,poisonTick=0,freezeTick=0;
	byte numRuns,numRenders;

	if(freezeTick<30*4-TalentBonus(TLT_FREEZING))	// 4 seconds minus 8 ticks per level of Freezing
		freezeTick++;
	else
		freezeTick=0;

	if(poisonTick>0)
	{
		poisonTick--;
	}
	else
		poisonTick=30;

	UpdateMonsterTypes();

	player.timeToFinish++;

	checkActive++;
	if(checkActive>5)
	{
		checkActive=0;
	}

	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type!=MONS_NONE && guys[i]->IsActive(map))
		{
			if(guys[i]->mapx>=0 && guys[i]->mapx<curMap->width && guys[i]->mapy>=0 && guys[i]->mapy<curMap->height)
				guys[i]->bright=map->map[guys[i]->mapx+guys[i]->mapy*map->width].templight;
			else
				guys[i]->bright=0;

			numRuns=1;
			numRenders=1;

			if(player.potionType==POT_REDCOW && guys[i]->type==player.monsType)
			{
				numRuns++;
				numRenders++;
			}
			if(guys[i]->ignited>0)
			{
				numRuns++;
			}
			if(guys[i]->type==player.monsType && player.berserkClock>0)
			{
				numRuns++;
				numRenders++;
			}
			else
			{
			}
			/* difficulty tweakers... may be of use in modes
			if(player.difficulty==DIFF_BEGINNER && checkActive==0 && guys[i]->hp>0 && guys[i]->team==EVIL)
			// in beginner mode, enemies move 20% slower, so don't update them on 1 of the 5 checkActives (it goes from 0 to 4)
			{
				numRuns--;
			}
			if(player.difficulty==DIFF_LOONY && (checkActive==0 || checkActive==2) && guys[i]->hp>0 && guys[i]->team==EVIL)
			// in loony mode, enemies move 40% faster, so update them extra on 2 of the 5 checkActives (it goes from 0 to 4)
			{
				numRuns++;
			}
			*/

			if(guys[i]->type==MONS_TEDDY3)
			{
				numRuns++;	// berserker bear!
				numRenders++;
			}

			if(guys[i]->frozen)
			{
				guys[i]->frozen--;
				if(guys[i]->team==EVIL)
					TalentPoint(TLT_FREEZING,1);

				meleeAttacker=NULL;
				bulletHittingType=0;
				if(freezeTick==0 && TalentBonus(TLT_FREEZING)!=0 && guys[i]->team==EVIL)
					guys[i]->GetShotReal(0,0,CurseDmg(1,GOOD),map,world);
				if(guys[i]->type==player.monsType && player.parka.magic==EM_COOLGUY && (player.timeToFinish%30)==0)
				{
					if(player.magic<player.maxMagic)
						player.magic++;
				}
				if(guys[i]->hp==0)
					guys[i]->frozen=0;
			}
			if(guys[i]->poison>0)
			{
				if(guys[i]->team==EVIL)
					TalentPoint(TLT_ARSENIC,1);

				guys[i]->poison--;
				if(poisonTick==0 && guys[i]->hp>0)
				{
					meleeAttacker=NULL;
					bulletHittingType=0;
					j=1;
					if(player.parka.magic==EM_SICKENING && guys[i]->team==EVIL)
						j++;
					if(PlagueOn() && guys[i]->team==EVIL && Distance(guys[i]->x,guys[i]->y,goodguy->x,goodguy->y)<=(int)(SpecificSkillVal(1,SKILL_PLAGUE)*FIXAMT))
						j+=2;

					guys[i]->GetShotReal(0,0,CurseDmg(j,GOOD+EVIL-guys[i]->team),map,world);
					if(guys[i]->hp!=0)
						guys[i]->ouch=3;
				}
			}
			for(j=0;j<numRuns;j++)
			{
				if(guys[i]->type==0)
					break;
				if((guys[i]->frozen%2) && guys[i]->ouch!=4 && (guys[i]->type!=player.monsType || player.parka.magic!=EM_COOLGUY))
					guys[i]->FrozenUpdate(map,world);
				else
					guys[i]->Update(map,world);
				if(numRenders>1)
				{
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
			if(guys[i]->mapx>=0 && guys[i]->mapx<curMap->width && guys[i]->mapy>=0 && guys[i]->mapy<curMap->height)
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
	int c,g,e;

	if((monsType[type].location&ML_GOOD) || ((type==MONS_BRUISER || type==MONS_THUG || type==MONS_JUNKSMITH) && player.var[VAR_SNUGGLY]==0 && player.var[VAR_ONIONCHAT]) ||
		((type==MONS_BUNNYBOSS || type==MONS_BUNNYTROOP || type==MONS_BUNNYMAJOR) && player.var[VAR_SNUGGLY]==1 && player.var[VAR_ONIONCHAT]))
	{
		// is a good guy, can always add goodguys
	}
	else
	{
		c=0;
		e=0;
		g=0;
		for(i=0;i<maxGuys;i++)
			if(guys[i]->type!=MONS_NONE)
			{
				c++;
				if(guys[i]->team==GOOD && guys[i]->mindControl==0)
					g++;
				else
					e++;
			}
		if(e>maxGuys-10)
			return NULL;	// can't add if there are only 10 slots left!
	}
	for(i=0;i<maxGuys;i++)
		if(guys[i]->type==MONS_NONE)
		{
			guys[i]->ignited=0;
			guys[i]->target=NULL;
			guys[i]->mindControl=0;
			guys[i]->poison=0;
			guys[i]->frozen=0;
			guys[i]->stun=0;
			guys[i]->type=type;
			guys[i]->safeX=x;
			guys[i]->safeY=y;
			guys[i]->x=x;
			guys[i]->y=y;
			guys[i]->z=z;
			guys[i]->seq=ANIM_IDLE;
			guys[i]->frm=0;
			guys[i]->frmTimer=0;
			guys[i]->frmAdvance=128;
			guys[i]->facing=2;
			guys[i]->ouch=0;
			guys[i]->dx=0;
			guys[i]->dy=0;
			guys[i]->dz=0;
			guys[i]->kbdx=0;
			guys[i]->kbdy=0;
			guys[i]->kbdz=0;
			guys[i]->bright=0;
			guys[i]->mind=0;
			guys[i]->mind1=0;
			guys[i]->mind2=0;
			guys[i]->mind3=0;
			guys[i]->wallBump=0;
			guys[i]->reload=(rand()%60);
			guys[i]->parent=NULL;
			guys[i]->CalculateRect();
			guys[i]->ID=i;
			guys[i]->mapx=(guys[i]->x>>FIXSHIFT)/TILE_WIDTH;
			guys[i]->mapy=(guys[i]->y>>FIXSHIFT)/TILE_HEIGHT;
			guys[i]->active=1;
			guys[i]->tag=0;
			guys[i]->team=EVIL;
			if(monsType[guys[i]->type].location&ML_GOOD)
				guys[i]->team=GOOD;
			if((guys[i]->type==MONS_BRUISER || guys[i]->type==MONS_THUG || guys[i]->type==MONS_JUNKSMITH) && player.var[VAR_SNUGGLY]==0 && player.var[VAR_ONIONCHAT])
				guys[i]->team=GOOD;
			if((guys[i]->type==MONS_BUNNYBOSS || guys[i]->type==MONS_BUNNYTROOP || guys[i]->type==MONS_BUNNYMAJOR) && player.var[VAR_SNUGGLY]==1 && player.var[VAR_ONIONCHAT])
				guys[i]->team=GOOD;

			if((guys[i]->type==MONS_ROSE || guys[i]->type==MONS_ROSE2 || guys[i]->type==MONS_ROSE3 || guys[i]->type==MONS_ROSE4 || guys[i]->type==MONS_ROSE5) && !editing)
			{
				DoMove(guys[i],ANIM_A2,128,1,0,0);
			}
			if(guys[i]->type==MONS_TOYPOWER)
			{
				Guy *g;
				int j;

				for(j=0;j<6;j++)
				{
					g=AddGuy(guys[i]->x,guys[i]->y,guys[i]->z,MONS_TOYCRYSTAL);
					if(g)
					{
						g->tag=255;
						g->mind=(byte)(j*(256/6));
						g->mind1=(byte)j;
						g->reload=(rand()%45);
						g->parent=guys[i];
					}
				}
			}
			if(guys[i]->type==MONS_GHOST)
			{
				guys[i]->mind=64;
				guys[i]->mind2=(byte)(TalentBonus(TLT_SPOOKYBONES));
				guys[i]->mind3=20;
				guys[i]->reload=0;
				DoMove(guys[i],ANIM_A1,128,1,0,0);
			}
			if(guys[i]->type==MONS_MIMIC && (levelDef[player.levelNum].flags&LF_TOWN))
				guys[i]->team=GOOD;
			if(guys[i]->type==MONS_FRUITPUNCH)
			{
				guys[i]->tag=255;
				DoMove(guys[i],ANIM_A3,128,1,0,0);
			}
			guys[i]->hp=MonsterHP(type,guys[i]->team);

			return guys[i];
		}
	return NULL;
}

void SaveGuys(FILE *f)
{
	int i,num;
	Guy *tgt,*parent;

	num=0;

	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type!=MONS_NONE)
		{
			num++;
		}
	}

	fwrite(&num,sizeof(int),1,f);
	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type!=MONS_NONE)
		{
			tgt=guys[i]->target;
			if(guys[i]->target)
			{
				guys[i]->target=(Guy *)(size_t)(tgt->ID);	// convert the pointer into a numerical reference
			}
			else
				guys[i]->target=(Guy *)(65535);

			parent=guys[i]->parent;
			if(guys[i]->parent)
			{
				guys[i]->parent=(Guy *)(size_t)(parent->ID);	// convert the pointer into a numerical reference
			}
			else
				guys[i]->parent=(Guy *)(65535);

			fwrite(guys[i],sizeof(Guy),1,f);
			guys[i]->target=tgt;
			guys[i]->parent=parent;
		}
	}
}

void LoadGuys(FILE *f)
{
	int i,num;
	Guy g;

	ExitGuys();
	InitGuys(MAX_MAPMONS);

	fread(&num,sizeof(int),1,f);

	for(i=0;i<num;i++)
	{
		fread(&g,sizeof(Guy),1,f);
		(*guys[g.ID])=g;
		if(guys[g.ID]->target==(Guy *)(65535))
			guys[g.ID]->target=NULL;
		else
			guys[g.ID]->target=guys[(int)(guys[g.ID]->target)];
		if(guys[g.ID]->parent==(Guy *)(65535))
			guys[g.ID]->parent=NULL;
		else
			guys[g.ID]->parent=guys[(int)(guys[g.ID]->parent)];
	}
	player.fireFlags&=(~FF_HELPERHERE);
}

void CameraOnPlayer(byte sum)
{
	int i;
	Guy *g;

	goodguy=NULL;

	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type==player.monsType)	// special case: loony is the goodguy
		{
			goodguy=guys[i];
			if(player.destx!=0)
			{
				guys[i]->x=(player.destx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
				guys[i]->y=(player.desty*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
				guys[i]->mapx=(guys[i]->x>>FIXSHIFT)/TILE_WIDTH;
				guys[i]->mapy=(guys[i]->y>>FIXSHIFT)/TILE_HEIGHT;
				player.destx=0;
				player.desty=0;
			}
			PutCamera(guys[i]->x,guys[i]->y);
			guys[i]->hp=player.hearts;
		}
	}

	if(goodguy==NULL)
	{
		g=AddGuy((player.destx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(player.desty*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,
			0,player.monsType);
		goodguy=g;

		player.destx=0;
		player.desty=0;
		PutCamera(g->x,g->y);
		g->hp=player.hearts;
	}

	if(goodguy && sum)
	{
		byte bok;

		bok=0;
		for(i=0;i<10;i++)
			if(player.var[VAR_BOKFOUND+i])
				bok++;
		bok-=player.var[VAR_BOKGIVEN];
		for(i=0;i<bok;i++)
		{
			g=AddGuy(goodguy->x,goodguy->y,goodguy->z,MONS_BOKBOK);
			if(g)
			{
				g->mind2=1;	// alive
				g->mind3=rand()%256;
			}
		}
		if(player.levelNum>=LVL_GORGE && player.levelNum<=LVL_CASTLE2 && player.levelNum!=LVL_MTNCABIN)
			AddPuppetPals();
		if(ModifierOn(MOD_TOY))
		{
			g=AddGuy(goodguy->x,goodguy->y,goodguy->z,MONS_LILBADGER);
			if(g)
			{
				g->tag=255;
				g->mind2=1;
				g->mind3=rand()%256;
			}
		}

	}
	if(player.var[VAR_CLOCKLIFE]+player.var[VAR_CLOCKLIFE2]*256>0)	// you have a clock pal!
	{
		DeleteMonsterByType(MONS_YOURBOT);
		g=AddGuy(goodguy->x,goodguy->y,0,MONS_YOURBOT);
		g->hp=player.var[VAR_CLOCKLIFE]+player.var[VAR_CLOCKLIFE2]*256;
	}

	if((curWorld.terrain[curMap->map[goodguy->mapx+goodguy->mapy*curMap->width].floor].flags&TF_ICE) && EquipCount(IT_ICESKATE)==0)
	{
		if(player.levelNum==LVL_BASKERVILLE)
		{
			goodguy->dx=FIXAMT*4;
			goodguy->dy=0;
		}
		else
		{
			goodguy->dx=-FIXAMT*4;
			goodguy->dy=0;
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

byte RandomAdd(byte x,byte y,Map *map)
{
	byte tries;
	byte type;
	Guy *g;

	if(player.var[VAR_PRISONBREAK]>1)
		return 1;

	tries=100;
	while(tries-->0)
	{
		type=(rand()%15);
		// each successive type in the list is less likely
		if(type<5)	// 0, 1, 2, 3, 4
			type=levelDef[player.levelNum].badguyType[0];
		else if(type<9) // 5,6,7,8
			type=levelDef[player.levelNum].badguyType[1];
		else if(type<12)	// 9,10,11
			type=levelDef[player.levelNum].badguyType[2];
		else if(type<14)	// 12,13
			type=levelDef[player.levelNum].badguyType[3];
		else	// 14
			type=levelDef[player.levelNum].badguyType[4];

		if(type!=0)
		{
			tries=0;
			g=AddGuy((int)(x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(int)(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,0,type);
			if(g && !g->CanWalk(g->x,g->y,map,&curWorld))
			{
				g->type=MONS_NONE;
				return 0;
			}
			return 1;
		}
	}

	return 0;
}

void TitanizeMap(Map *map,byte bad)
{
	int amt,tries,type,x,y;
	Guy *g;

	if(bad==1)	// flood with titans!
	{
		amt=120;
		tries=100;
		while(amt>0)
		{
			tries--;
			if(tries==0)
			{
				amt--;
				tries=100;
			}

			x=rand()%map->width;
			y=rand()%map->height;
			if(map->map[x+y*map->width].wall || map->map[x+y*map->width].item)
				continue;
			g=AddGuy((int)(x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(int)(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,0,MONS_TITAN);
			if(g && !g->CanWalkNoMons(g->x,g->y,map,&curWorld))
			{
				g->type=MONS_NONE;
				continue;
			}
			amt--;
			tries=100;
		}
	}
	else // a few titans and put a few goodguys with the player if this is geyser grotto
	{
		amt=levelDef[player.levelNum].badguyCount/2;
		tries=100;
		while(amt>0)
		{
			tries--;
			if(tries==0)
			{
				amt--;
				tries=100;
			}

			x=rand()%map->width;
			y=rand()%map->height;
			if(map->map[x+y*map->width].wall || map->map[x+y*map->width].item)
				continue;
			g=AddGuy((int)(x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(int)(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,0,MONS_TITAN);
			if(g && !g->CanWalkNoMons(g->x,g->y,map,&curWorld))
			{
				g->type=MONS_NONE;
				continue;
			}
			amt--;
			tries=100;
		}
		if(player.levelNum==LVL_GEYSER && player.destx==0 && player.desty==0)	// you are coming in the geyser side of geyser grotto
		{
			amt=20;
			tries=100;
			if(player.var[VAR_SNUGGLY])
				type=MONS_BUNNYTROOP;
			else
				type=MONS_THUG;
			while(amt>0)
			{
				tries--;
				if(tries==0)
				{
					amt--;
					tries=100;
				}

				x=map->width*3/4+(rand()%(map->width/4));
				y=rand()%map->height;
				if(map->map[x+y*map->width].wall || map->map[x+y*map->width].item)
					continue;
				g=AddGuy((int)(x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(int)(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,0,type);
				if(g && !g->CanWalkNoMons(g->x,g->y,map,&curWorld))
				{
					g->type=MONS_NONE;
					continue;
				}
				amt--;
				tries=100;
			}
		}
	}
}

void ForestBadgers(Map *map,byte right)
{
	int amt,tries,x,y;
	Guy *g;

	amt=50-player.var[VAR_DEFILE+right];
	tries=100;
	while(amt>0)
	{
		tries--;
		if(tries==0)
		{
			amt--;
			tries=100;
		}

		if(right)
			x=93+(rand()%(map->width-93));
		else
			x=rand()%93;
		y=rand()%map->height;
		if(map->map[x+y*map->width].wall || map->map[x+y*map->width].item)
			continue;
		g=AddGuy((int)(x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(int)(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,0,MONS_BADGER2);
		if(g && !g->CanWalkNoMons(g->x,g->y,map,&curWorld))
		{
			g->type=MONS_NONE;
			continue;
		}
		amt--;
		tries=100;
	}
}

void RandomFillMap(Map *map)
{
	int amt,tries;
	byte x,y;

	if(player.var[VAR_PRISONBREAK]>1)
		return;

	if(player.levelNum>=LVL_GEYSER && player.levelNum<=LVL_TITANS && player.var[VAR_QUESTDONE+QUEST_FURNACE]==0)
	{
		amt=levelDef[player.levelNum].badguyCount/2;
		if(player.var[VAR_QUESTDONE+QUEST_QUESTS])	// have allies!
		{
			// fill the level as normal, and add a smattering of ice titans and allies
			TitanizeMap(map,0);
		}
		else
		{
			// no allies, flood the place with titans!
			TitanizeMap(map,1);
			DisableExits(map);
			return;	// don't need regular monsters for this
		}
	}
	else
		amt=levelDef[player.levelNum].badguyCount;
	if(GlassEffect()==GE_MONSTER)
		amt*=2;
	if(ModifierOn(MOD_CROWDED))
		amt*=4;

	player.shouldMonsters=0;
	if(player.var[VAR_QUESTASSIGN+QUEST_BOBSWIFE2] && !player.var[VAR_KLONKDEAD] && player.levelNum==LVL_SOUTHWOOD)
		return;
	if(player.var[VAR_QUESTASSIGN+QUEST_RUMBLE] && !player.var[VAR_RUMBLE] && player.levelNum==LVL_CLEARING)
		return;
	if(player.levelNum==LVL_ONIONCAMP && player.var[VAR_SNUGGLY]==0)
		return;
	if(player.levelNum==LVL_SNUGGLYHQ && player.var[VAR_SNUGGLY]==1)
		return;
	if(player.levelNum==LVL_HIGHWAY && player.var[VAR_SNUGGLY] && player.var[VAR_QUESTASSIGN+QUEST_HIGHWAY] && player.var[VAR_HIGHWAY]==0)
	{
		AddHighwayPatrolFoes(map);
		return;	// no spawns during the highway patrol
	}

	if(amt>config.numGuys)
		amt=config.numGuys;

	tries=100;
	while(amt>0)
	{
		tries--;
		if(tries==0)
		{
			amt--;
			tries=100;
		}

		x=rand()%map->width;
		y=rand()%map->height;
		if(map->map[x+y*map->width].wall || map->map[x+y*map->width].item)
			continue;
		if(player.levelNum==LVL_FJORD && y>map->height*3/4)
			continue;	// no mice on the bottom half!
		if(RandomAdd(x,y,map))
		{
			amt--;
			tries=100;
		}
	}
#ifdef DIRECTORS
	if((levelDef[player.levelNum].flags&(LF_ARENA|LF_TOWN))==0 && player.var[VAR_MADCAP] && Random(100)<2)
	{
		// artifact jack!
		tries=100;
		while(tries>0)
		{
			tries--;

			x=rand()%map->width;
			y=rand()%map->height;
			if(map->map[x+y*map->width].wall || map->map[x+y*map->width].item)
				continue;
			Guy *g=AddGuy((int)(x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(int)(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,0,MONS_JACK);

			if(g && !g->CanWalk(g->x,g->y,map,&curWorld))
			{
				g->type=MONS_NONE;
				tries--;
			}
			else if(!g)
				tries--;
			else if(g)
				break;
		}
	}
#endif

	areMonsters=0;
	for(amt=0;amt<maxGuys;amt++)
		if(guys[amt]->type)
			areMonsters++;

	player.shouldMonsters=areMonsters;
}

void RandomRespawnMap(Map *map)
{
	byte x,y;
	int tries;
	static byte tick=0;
	int i;

	if(player.var[VAR_QUESTASSIGN+QUEST_RUMBLE] && !player.var[VAR_RUMBLE] && player.levelNum==LVL_CLEARING)
	{
		if(TaggedMonsterExists(2))
			return;
		else
			PlayerSetVar(VAR_RUMBLE,1);	// did it!
	}

	if(ModifierOn(MOD_RESPAWN)==0 && player.potionType!=POT_MONSTER && (player.levelNum!=LVL_GEYSER || player.var[VAR_QUESTDONE+QUEST_QUESTS]==1))
		return;

	tick++;
	if(player.levelNum==LVL_GEYSER && player.var[VAR_QUESTDONE+QUEST_QUESTS]==0)
		tick=30;	// constant!

	if((tick<30 && player.potionType!=POT_MONSTER) || (tick<5 && player.potionType==POT_MONSTER))
		return;

	if(player.levelNum==LVL_SOUTHWOOD && player.var[VAR_KLONKDEAD]==0 && player.var[VAR_QUESTASSIGN+QUEST_BOBSWIFE2])
		return;	// no spawns until klonk is gone

	if(player.levelNum==LVL_CASTLE1 && player.potionType!=POT_MONSTER)
		return;

	if(player.levelNum==LVL_TOYBASEMENT && player.var[VAR_QUESTDONE+QUEST_FACTORY2]==0)
		return;	// no spawns until toy power is destroyed

	if(player.levelNum==LVL_HIGHWAY && player.var[VAR_SNUGGLY] && player.var[VAR_QUESTASSIGN+QUEST_HIGHWAY] && player.var[VAR_HIGHWAY]==0)
		return;	// no spawns during the highway patrol

	tick=0;	// only do this once per second, unless drinking Eau De Monster

	areMonsters=0;
	for(i=0;i<maxGuys;i++)
		if(guys[i]->type)
			areMonsters++;

	if(player.potionType==POT_MONSTER)
		areMonsters=0;	// trick it into thinking there are no monsters around!

	if(player.shouldMonsters>areMonsters)	// not as many monsters as there should be
	{
		if((rand()%player.shouldMonsters)>(player.shouldMonsters-areMonsters))
			return;	// nope, not serious enough
	}
	else
		return;

	tries=100;
	while(tries-->0)
	{
		if(player.potionType!=POT_MONSTER)
		{
			x=rand()%map->width;
			y=rand()%map->height;
		}
		else
		{
			if(goodguy)
			{
				x=goodguy->mapx-30+(rand()%61);
				y=goodguy->mapy-30+(rand()%61);
				if(x<0)
					x=0;
				if(y<0)
					y=0;
				if(x>=map->width)
					x=map->width-1;
				if(y>=map->height)
					y=map->height-1;
			}
			else
				x=y=0;
		}
		if(map->map[x+y*map->width].wall || map->map[x+y*map->width].item)
			continue;
		if(abs(goodguy->mapx-x)<20 && abs(goodguy->mapy-y)<20)
			continue;	// can't be onscreen

		if(RandomAdd(x,y,map))
			return;	// did it!
	}
}

void AddMapGuys(Map *map)
{
	int i;
	byte ok,tag;
	Guy *g;

	if(player.levelNum==LVL_MICKEYCELLAR)
	{
		if(player.var[VAR_QUESTDONE+QUEST_RATS] || player.var[VAR_RATS]==0)
		{
			for(i=0;i<MAX_MAPMONS;i++)
				if(map->badguy[i].type!=MONS_LOONY && map->badguy[i].type!=MONS_VILLAGER)
					map->badguy[i].type=0;	// remove all the rats, please
		}
		else
		{
			PlayerSetVar(VAR_RATS,17);	// otherwise, set the rat counter
		}
	}
	fakeGuy.mind=0;
	for(i=0;i<MAX_MAPMONS;i++)
	{
		if(map->badguy[i].type)
		{
			tag=map->map[map->badguy[i].x+map->badguy[i].y*map->width].tag;
			ok=1;
			if(player.worldNum==WORLD_NORMAL || player.worldNum==WORLD_REMIX)
			{
				// right here, remove any bosses that are already dead, etc.
				if(!editing)
				{
					if(map->badguy[i].type==MONS_PUPPET2 && player.var[VAR_MAGICHAT])
					{
						if(!SkillHave(SKILL_DEFENSE))
						{
							map->map[map->badguy[i].x+map->badguy[i].y*map->width].item=IT_SCROLL;
							map->map[map->badguy[i].x+map->badguy[i].y*map->width].tag=SKILL_DEFENSE;
						}
						continue;
					}
					else if(map->badguy[i].type==MONS_MOUSE3 && player.var[VAR_RATBOSS])
					{
						if(!SkillHave(SKILL_RESTORE))
						{
							map->map[map->badguy[i].x+map->badguy[i].y*map->width].item=IT_SCROLL;
							map->map[map->badguy[i].x+map->badguy[i].y*map->width].tag=SKILL_RESTORE;
						}
						continue;
					}
					else if(map->badguy[i].type==MONS_MONKEY2 && player.var[VAR_STINKYSOCK])
					{
						if(!SkillHave(SKILL_POISON))
						{
							map->map[map->badguy[i].x+map->badguy[i].y*map->width].item=IT_SCROLL;
							map->map[map->badguy[i].x+map->badguy[i].y*map->width].tag=SKILL_POISON;
						}
						continue;
					}
					else if(map->badguy[i].type==MONS_TEDDY3)
					{
						if(!player.var[VAR_QUESTASSIGN+QUEST_BIGBEAR])
							continue;	// don't add him at all
						else if(player.var[VAR_QUESTDONE+QUEST_BIGBEAR])
						{
							if(!SkillHave(SKILL_BERSERK))
							{
								map->map[map->badguy[i].x+map->badguy[i].y*map->width].item=IT_SCROLL;
								map->map[map->badguy[i].x+map->badguy[i].y*map->width].tag=SKILL_BERSERK;
							}
							continue;
						}
					}
					else if(map->badguy[i].type==MONS_VILLAGER && tag==12 && player.talentLevel[map->map[map->badguy[i].x+(map->badguy[i].y+1)*map->width].tag]!=255)
					{
						if(player.levelNum!=LVL_GURUCABIN)
							continue;	// remove gurus that you have the talent for already
					}
					else if(map->badguy[i].type==MONS_VILLAGER && tag==9 && player.var[VAR_WIFEUNLOCK])
						continue;
					else if(map->badguy[i].type==MONS_VILLAGER && tag==20 && !player.var[VAR_QUESTDONE+QUEST_ROPES])
						continue;
					else if(map->badguy[i].type==MONS_VILLAGER && tag==19 && player.var[VAR_QUESTDONE+QUEST_ROPES])
						continue;
					else if(map->badguy[i].type==MONS_VILLAGER && tag==10 && !player.var[VAR_WIFEUNLOCK])
						continue;
					else if(map->badguy[i].type==MONS_VILLAGER && player.levelNum==LVL_TOYOFFICE && player.var[VAR_QUESTDONE+QUEST_TITANS2])
						continue;
					else if(map->badguy[i].type==MONS_VILLAGER && tag==15 && !player.var[VAR_QUESTDONE+QUEST_BALLOON] && player.levelNum!=LVL_BASKERVILLE)
						continue;	// balloonist only appears in Baskerville prior to quest completion
					else if(map->badguy[i].type==MONS_ROSE && player.var[VAR_STONEROSE])
					{
						if(!SkillHave(SKILL_BRAMBLES))
						{
							map->map[map->badguy[i].x+map->badguy[i].y*map->width].item=IT_SCROLL;
							map->map[map->badguy[i].x+map->badguy[i].y*map->width].tag=SKILL_BRAMBLES;
						}
						continue;
					}
					else if(map->badguy[i].type==MONS_BURNINGBUSH && player.var[VAR_BUSHDEAD])
					{
						if(!SkillHave(SKILL_BURNINATE))
						{
							map->map[map->badguy[i].x+map->badguy[i].y*map->width].item=IT_SCROLL;
							map->map[map->badguy[i].x+map->badguy[i].y*map->width].tag=SKILL_BURNINATE;
						}
						continue;
					}
					else if(map->badguy[i].type==MONS_TRIGUN && player.var[VAR_TRIGUNDEAD])
					{
						if(!SkillHave(SKILL_TRIPLE))
						{
							map->map[map->badguy[i].x+map->badguy[i].y*map->width].item=IT_SCROLL;
							map->map[map->badguy[i].x+map->badguy[i].y*map->width].tag=SKILL_TRIPLE;
						}
						continue;
					}
					else if(map->badguy[i].type==MONS_KLONK && (player.var[VAR_KLONKDEAD] || !player.var[VAR_QUESTASSIGN+QUEST_BOBSWIFE2]))
					{
						if(player.var[VAR_KLONKDEAD] && GotItem(IT_KEY1)==0)	// you killed him but didn't pick up the key!
						{
							map->map[map->badguy[i].x+map->badguy[i].y*map->width].item=IT_KEY1;
						}
						continue;	// don't add him if he's dead, or if you haven't yet got the quest
					}
					else if(map->badguy[i].type==MONS_TOYMAKER && player.levelNum==LVL_TOYFACTORY && player.var[VAR_FACTORY1])
						continue;	// keep toymotrons on first floor dead
					else if(map->badguy[i].type==MONS_TOYMAKER && player.levelNum==LVL_TOYSTORY && player.var[VAR_QUESTDONE+QUEST_FACTORY])
						continue;	// keep toymotrons on first floor dead
					else if(map->badguy[i].type==MONS_TOYPOWER && player.var[VAR_QUESTDONE+QUEST_FACTORY2])
						continue;
					else if(map->badguy[i].type==MONS_JUNKSMITH && player.var[VAR_JUNKDEAD])
						continue;
					else if(map->badguy[i].type==MONS_EVILBOT && player.var[VAR_QUESTDONE+QUEST_CLOCKJUNK] && player.var[VAR_SNUGGLY]==1)
						continue;
					else if(player.levelNum==LVL_ARMSCAVE && (map->badguy[i].type==MONS_BRUISER || map->badguy[i].type==MONS_THUG))
					{
						if(player.var[VAR_SNUGGLY]==0 || player.var[VAR_QUESTASSIGN+QUEST_SHAKEDOWN]==0 || player.var[VAR_QUESTDONE+QUEST_SHAKEDOWN]==1 || player.var[VAR_SHAKEDOWN]==1)
							continue;
					}
					else if((map->badguy[i].type==MONS_EVILBOT || map->badguy[i].type==MONS_BUNNYTROOP) && player.levelNum==LVL_CRAMPEDCAVE && player.var[VAR_SNUGGLY])
						continue;
					else if(map->badguy[i].type==MONS_VILLAGER && map->map[map->badguy[i].x+map->badguy[i].y*map->width].tag==24 && player.var[VAR_SNUGGLY]==1 && player.var[VAR_QUESTDONE+QUEST_LEADER])
						continue;	// remove onion if he's been slain
					else if((map->badguy[i].type==MONS_BUNNYBOSS || map->badguy[i].type==MONS_BUNNYMAJOR) && player.var[VAR_SNUGGLY]==0)
					{
						if(player.var[VAR_QUESTDONE+QUEST_LEADER] && map->badguy[i].type==MONS_BUNNYBOSS)
						{
							if(!SkillHave(SKILL_TORNADO))
							{
								map->map[map->badguy[i].x+map->badguy[i].y*map->width].item=IT_SCROLL;
								map->map[map->badguy[i].x+map->badguy[i].y*map->width].tag=SKILL_TORNADO;
							}
							continue;
						}
						if(map->badguy[i].type==MONS_BUNNYMAJOR)
							continue;	// bunnyboss removed if he's dead (leaving scroll if needed).  Bunnyguy removed regardless (he pops in later)
					}
					else if(map->badguy[i].type==MONS_BUNNYBOSS && player.var[VAR_SNUGGLY]==1)
					{
						if(player.var[VAR_QUESTDONE+QUEST_FURNACE])
							continue;	// he's gone!
					}
					else if(map->badguy[i].type==MONS_BIGPLANT && player.var[VAR_GOURDDEAD])
						continue;
					else if(map->badguy[i].type==MONS_BOKBOK && player.var[VAR_BOKFOUND+map->map[map->badguy[i].x+map->badguy[i].y*map->width].tag-1])
						continue;	// remove bokboks found
					else if(map->badguy[i].type==MONS_FURNACE && player.var[VAR_FURNACE] && !SkillHave(SKILL_HEATSHIELD))
					{
						map->map[map->badguy[i].x+(map->badguy[i].y+4)*map->width].item=IT_SCROLL;
						map->map[map->badguy[i].x+(map->badguy[i].y+4)*map->width].tag=SKILL_HEATSHIELD;
					}
					else if(map->badguy[i].type==MONS_TITAN2 && player.var[VAR_TITANIC+map->map[map->badguy[i].x+map->badguy[i].y*map->width].tag-1])
					{
						switch(map->map[map->badguy[i].x+map->badguy[i].y*map->width].tag)
						{
							case 1:
							case 3:
							case 5:
							case 7:
							case 9:
								if(!player.var[VAR_PRESENT+77+(map->map[map->badguy[i].x+map->badguy[i].y*map->width].tag-1)/2])
								{
									map->map[map->badguy[i].x+map->badguy[i].y*map->width].item=IT_PRESENT;
									map->map[map->badguy[i].x+map->badguy[i].y*map->width].tag=77+(map->map[map->badguy[i].x+map->badguy[i].y*map->width].tag-1)/2;
								}
								break;
							case 2:
								if(!SkillHave(SKILL_LUCK))
								{
									map->map[map->badguy[i].x+map->badguy[i].y*map->width].item=IT_SCROLL;
									map->map[map->badguy[i].x+map->badguy[i].y*map->width].tag=SKILL_LUCK;
								}
								break;
							case 4:
								if(!SkillHave(SKILL_PERSUADE))
								{
									map->map[map->badguy[i].x+map->badguy[i].y*map->width].item=IT_SCROLL;
									map->map[map->badguy[i].x+map->badguy[i].y*map->width].tag=SKILL_PERSUADE;
								}
								break;
							case 6:
								if(!SkillHave(SKILL_TURRET))
								{
									map->map[map->badguy[i].x+map->badguy[i].y*map->width].item=IT_SCROLL;
									map->map[map->badguy[i].x+map->badguy[i].y*map->width].tag=SKILL_TURRET;
								}
								break;
							case 8:
								if(!SkillHave(SKILL_ICECUBE))
								{
									map->map[map->badguy[i].x+map->badguy[i].y*map->width].item=IT_SCROLL;
									map->map[map->badguy[i].x+map->badguy[i].y*map->width].tag=SKILL_ICECUBE;
								}
								break;
							case 10:
								if(!SkillHave(SKILL_PLAGUE))
								{
									map->map[map->badguy[i].x+map->badguy[i].y*map->width].item=IT_SCROLL;
									map->map[map->badguy[i].x+map->badguy[i].y*map->width].tag=SKILL_PLAGUE;
								}
								break;
						}
						continue;
					}
					else if(map->badguy[i].type==MONS_BARON && player.var[VAR_SUPERTITAN])
						continue;
					else if(map->badguy[i].type==MONS_FROSTGATE && player.var[VAR_FROSTGATE])
						continue;
					else if(map->badguy[i].type==MONS_MIMIC)
					{
						if(player.levelNum!=LVL_LAKE)
						{
							if(player.levelNum==LVL_SOUTHWOOD && (player.var[VAR_TAMED]&1)==0)
								continue;
							if(player.levelNum!=LVL_SOUTHWOOD && (player.var[VAR_TAMED]&2)==0)
								continue;
							if(player.levelNum==LVL_ONIONCAMP && player.var[VAR_SNUGGLY]==1)
								continue;
							if(player.levelNum==LVL_SNUGGLYHQ && player.var[VAR_SNUGGLY]==0)
								continue;
						}
					}
					if(map->badguy[i].type==MONS_VILLAGER && map->map[map->badguy[i].x+map->badguy[i].y*map->width].tag>=36 && map->map[map->badguy[i].x+map->badguy[i].y*map->width].tag<=40)
					{
						if(player.levelNum==LVL_SHROOMCAMP)
						{
							if(player.var[VAR_QUESTDONE+QUEST_FOREST] && !player.var[VAR_YOUTH])
								continue;	// no villagers!
						}
						else if(player.levelNum==LVL_FOREST)
						{
							if(!player.var[VAR_QUESTDONE+QUEST_FOREST] || player.var[VAR_YOUTH])
								continue;	// no villagers in forest except during ritual
						}
						if(map->map[map->badguy[i].x+map->badguy[i].y*map->width].tag==37)
						{
							// sharona may need bokboks
							int j;
							for(j=0;j<player.var[VAR_BOKGIVEN];j++)
							{
								g=AddGuy((map->badguy[i].x*TILE_WIDTH+(TILE_WIDTH/2))<<FIXSHIFT,
									   (map->badguy[i].y*TILE_HEIGHT+(TILE_HEIGHT/2))<<FIXSHIFT,
									   0,MONS_BOKBOK);
								if(g)
								{
									g->mind2=2;
									g->mind3=rand()%256;
								}
							}
						}
						if(map->map[map->badguy[i].x+map->badguy[i].y*map->width].tag==38 && player.var[VAR_YOUTH])
							tag=39;	// make him young!
					}
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
						if(g->type==MONS_VILLAGER)
							AI_Villager(g,curMap,&curWorld,goodguy);
					}
				}
			}
		}
	}

	if(!editing)
	{
		if(player.var[VAR_QUESTASSIGN+QUEST_RUMBLE] && !player.var[VAR_RUMBLE] && player.levelNum==LVL_CLEARING)
			AddRumbleGuys(map);
		else
			RandomFillMap(map);
		if(player.levelNum==LVL_FOREST)
		{
			if(player.destx<93)	// on the lefthand side
				ForestBadgers(map,0);
			else
				ForestBadgers(map,1);
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
	int amount;
	byte myType;
	int i;

	if(1)//editing || origType==player.monsType)
		return 0;

	// can't surprise some monsters
	if(!RandomSummonMons(origType))
		return 0;

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
			if(!RandomSummonMons(myType))
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
			if(!RandomSummonMons(myType))
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
		//DrawDebugBox(xx,yy,xx+size*2,yy+size*2);
		return 1;
	}

	return 0;
}

// donthitme is the ID of a guy who should not be hit by this (0=hit anybody)
// team is a bitflag of teams that should get hit - 1,2, or both
// startAt is where in the list to begin the search, for coming back for more hits
int FindVictim(int x,int y,int mymapx,int mymapy,byte size,int donthitme,byte team,int startAt,Map *map,world_t *world)
{
	int i;

	x/=FIXAMT;
	y/=FIXAMT;

	for(i=startAt;i<maxGuys;i++)
		if(guys[i]->type && guys[i]->hp && (guys[i]->team&team)!=0 && (guys[i]->ID+1)!=donthitme)
		{
			if(CheckHit(size,x,y,guys[i]) && map->CheckLOS(mymapx,mymapy,15,guys[i]->mapx,guys[i]->mapy))
			{
				return guys[i]->ID+1;
			}
		}

	return 0;
}

byte ItemHitPlayer(int x,int y,byte size,int dx,int dy,byte type,Map *map,world_t *world)
{
	if(!goodguy)
		return 0;

	if(CheckHit(size,x,y,goodguy))
	{
		PlayerGetItem(type,x,y);
		return 1;
	}

	return 0;
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
		if(guys[i]->type && (guys[i]->team==EVIL || (guys[i]->team==GOOD && guys[i]->mindControl)))
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

void HealGoodguy(word amt)
{
	word show;

	if(player.hearts==0)
		return;

	if(player.amulet.magic==EM_HEALING)
		amt=(byte)((int)amt*125/100);

	show=amt;

	if(player.hearts+amt>player.maxHearts)
	{
		show=player.maxHearts-player.hearts;
		player.hearts=player.maxHearts;
	}
	else
		player.hearts+=amt;
	if(show>0)
		NumberParticle(amt,goodguy->x,goodguy->y+1,1);
	goodguy->hp=player.hearts;
}

void HealGuy(Guy *g,word amt)
{
	word show;

	show=amt;

	if(g->hp+amt>MonsterHP(g->type,g->team))
	{
		show=MonsterHP(g->type,g->team)-g->hp;
		g->hp=MonsterHP(g->type,g->team);
	}
	else
		g->hp+=amt;
	if(show>0)
		NumberParticle(amt,g->x,g->y+1,1);
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
				guys[i]->frmAdvance=128;
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

void DeleteMonsterByType(byte type)
{
	int i;

	for(i=0;i<maxGuys;i++)
		if(guys[i] && guys[i]->type==type)
		{
			guys[i]->type=MONS_NONE;
		}
}

void DropJunkItem(byte type,int x,int y)
{
	bullet_t *b;
	byte odds;

	odds=10;
	if(type==IT_ICECUBE && player.var[VAR_QUESTASSIGN+QUEST_TITANS] && !player.var[VAR_QUESTDONE+QUEST_TITANS] && GotItem(IT_ICECUBE)<3)
		odds=70;
	// modify odds with Packrat talent
	odds+=(int)TalentBonus(TLT_PACKRAT);
	if((rand()%100)<odds)
	{
		b=FireBullet(goodguy->ID,x,y,FIXAMT*15,rand()%256,BLT_ITEM,0);
		if(b)
		{
			b->anim=type;
			TalentPoint(TLT_SCAVENGING,1);
		}
	}
}

void BadguyDrop(byte guytype,byte level,int x,int y)
{
	bullet_t *b;
	byte itemdrop,type;
	word money;
	int odds;
	int luck;

	if(guytype==MONS_UBERTITAN)
		return;

	luck=(int)SpecificSkillVal(1,SKILL_LUCK);
	if(player.parka.magic==EM_LUCKY)
		luck++;

	money=rand()%((level/5)+2+luck);

	luck=(int)SpecificSkillVal(0,SKILL_LUCK);
	if(player.parka.magic==EM_LUCKY)
		luck+=25;
	if(GlassEffect()==GE_LUCK)
		luck+=25;

	if(player.potionType==POT_LUCKENBRAU)
		luck+=PotionEffect(player.potionType,player.potionPower);

	itemdrop=rand()%100;
	odds=8*(100+luck)/100;
	if(monsType[guytype].location&ML_BOSS)
	{
		odds=100000;	// bosses always drop something
		money+=rand()%(level);	// and drop randomly up to level more money
		bossDrop=1;
	}
	if(ModifierOn(MOD_MERCHANT))
	{
		odds=0;
		money*=5;
	}

	if(itemdrop<odds)
	{
		b=FireBullet(goodguy->ID,x,y,FIXAMT*15,rand()%256,BLT_ITEM,0);
		if(b)
		{
			EquipCreateItem(level,&type,&b->target,&b->owner);
			bossDrop=0;
			b->anim=type;
			TalentPoint(TLT_SCAVENGING,1);
		}
	}
	// special items based on monster class
	switch(guytype)
	{
		case MONS_CARAVAN:
			if(player.var[VAR_QUESTASSIGN+QUEST_HIGHWAY] && !player.var[VAR_QUESTDONE+QUEST_HIGHWAY] && player.var[VAR_SNUGGLY]==0)
			{
				if((rand()%100)<50)
				{
					b=FireBullet(goodguy->ID,x,y,FIXAMT*15,rand()%256,BLT_ITEM,0);
					if(b)
					{
						b->anim=IT_STOLEN;
						TalentPoint(TLT_SCAVENGING,1);
					}
				}
			}
			break;
		case MONS_PUPPET:
		case MONS_PUPPET3:
			DropJunkItem(IT_LOG,x,y);
			if(player.var[VAR_QUESTASSIGN+QUEST_TOYS] && !player.var[VAR_QUESTDONE+QUEST_TOYS] && player.var[VAR_TOYSDEAD]<200)
				PlayerSetVar(VAR_TOYSDEAD,player.var[VAR_TOYSDEAD]+1);
			break;
		case MONS_MIMIC:
			DropJunkItem(IT_LOG,x,y);
			break;
		case MONS_BRUISER:
			DropJunkItem(IT_LOG,x,y);
			break;
		case MONS_THUG:
			DropJunkItem(IT_SOCK,x,y);
			break;
		case MONS_BUNNYTROOP:
			DropJunkItem(IT_STUFFING,x,y);
			break;
		case MONS_CRAB:
		case MONS_STONEWALL:
			DropJunkItem(IT_STONE,x,y);
			break;
		case MONS_BADGER:
		case MONS_BADGER2:
		case MONS_BIGBADGER:
			DropJunkItem(IT_CLAW,x,y);
			if(player.levelNum==LVL_FOREST && goodguy->mapx<93 && player.var[VAR_DEFILE]<50)
				PlayerSetVar(VAR_DEFILE,player.var[VAR_DEFILE]+1);
			if(player.levelNum==LVL_FOREST && goodguy->mapx>=93 && player.var[VAR_DEFILE+1]<50)
				PlayerSetVar(VAR_DEFILE+1,player.var[VAR_DEFILE+1]+1);
			break;
		case MONS_BADGER3:
			DropJunkItem(IT_CLAW,x,y);
			if(player.var[VAR_QUESTASSIGN+QUEST_TOYS2] && !player.var[VAR_QUESTDONE+QUEST_TOYS2] && GotItem(IT_TOYCRYSTAL)<20 && (rand()%100)<40)
			{
				b=FireBullet(goodguy->ID,x,y,FIXAMT*15,rand()%256,BLT_ITEM,0);
				if(b)
				{
					b->anim=IT_TOYCRYSTAL;
					b->target=0;
				}
			}
			break;
		case MONS_EVILBOT:
			DropJunkItem(IT_LOG+(rand()%9),x,y);	// randomly drop any type of junk
			break;
		case MONS_MOUSE:
		case MONS_MOUSE2:
		case MONS_ICEMICE:
		case MONS_MOUSE4:
		case MONS_MOUSE5:
		case MONS_TURRET:
			DropJunkItem(IT_GEAR,x,y);
			if(player.var[VAR_QUESTASSIGN+QUEST_TOYS] && !player.var[VAR_QUESTDONE+QUEST_TOYS] && player.var[VAR_TOYSDEAD]<200)
				PlayerSetVar(VAR_TOYSDEAD,player.var[VAR_TOYSDEAD]+1);;
			break;
		case MONS_TEDDY:
		case MONS_TEDDY2:
		case MONS_TEDDY4:
			DropJunkItem(IT_STUFFING,x,y);
			if(player.var[VAR_QUESTASSIGN+QUEST_BEARS] && !player.var[VAR_QUESTDONE+QUEST_BEARS] && guytype==MONS_TEDDY2 && player.var[VAR_BEARSDEAD]<30)
				PlayerSetVar(VAR_BEARSDEAD,player.var[VAR_BEARSDEAD]+1);
			if(player.var[VAR_QUESTASSIGN+QUEST_TOYS] && !player.var[VAR_QUESTDONE+QUEST_TOYS] && player.var[VAR_TOYSDEAD]<200)
				PlayerSetVar(VAR_TOYSDEAD,player.var[VAR_TOYSDEAD]+1);;
			break;
		case MONS_TINGUY:
		case MONS_TINGUY2:
		case MONS_TINPOT:
			DropJunkItem(IT_CAN,x,y);
			if(player.var[VAR_QUESTASSIGN+QUEST_TOYS] && !player.var[VAR_QUESTDONE+QUEST_TOYS] && player.var[VAR_TOYSDEAD]<200)
				PlayerSetVar(VAR_TOYSDEAD,player.var[VAR_TOYSDEAD]+1);;
			break;
		case MONS_MONKEY:
		case MONS_MONKEY3:
		case MONS_MONKEY4:
			DropJunkItem(IT_SOCK,x,y);
			if(player.var[VAR_QUESTASSIGN+QUEST_TOYS] && !player.var[VAR_QUESTDONE+QUEST_TOYS] && player.var[VAR_TOYSDEAD]<200)
				PlayerSetVar(VAR_TOYSDEAD,player.var[VAR_TOYSDEAD]+1);;
			break;
		case MONS_HOUND:
		case MONS_TITAN:
			DropJunkItem(IT_ICECUBE,x,y);
			break;
		case MONS_GHOSTDOG:
		case MONS_GHOSTMONKEY:
			DropJunkItem(IT_ECTO,x,y);
			break;
		case MONS_ROSE2:
		case MONS_LION:
		case MONS_ROSE3:
		case MONS_LION2:
			DropJunkItem(IT_SEED,x,y);
			break;
			// BOSSES WITH SPECIAL DROPS ----------------------------------
		case MONS_JACK:
			b=FireBullet(goodguy->ID,x,y,FIXAMT*15,rand()%256,BLT_ITEM,0);
			if(b)
			{
				b->anim=IT_ARTBIT;
				b->target=0;
			}
			break;
		case MONS_MOUSE3:	// ratatouille
			player.var[VAR_RATBOSS]=1;
			if(!SkillHave(SKILL_RESTORE))
			{
				b=FireBullet(goodguy->ID,x,y,FIXAMT*15,rand()%256,BLT_ITEM,0);
				if(b)
				{
					b->anim=IT_SCROLL;
					b->target=SKILL_RESTORE;
				}
			}
			if(player.var[VAR_QUESTASSIGN+QUEST_TOYS] && !player.var[VAR_QUESTDONE+QUEST_TOYS] && player.var[VAR_TOYSDEAD]<200)
				PlayerSetVar(VAR_TOYSDEAD,player.var[VAR_TOYSDEAD]+1);;
			break;
		case MONS_PUPPET2:	// Magic Hat boss
			if(!SkillHave(SKILL_DEFENSE))
			{
				b=FireBullet(goodguy->ID,x,y,FIXAMT*15,rand()%256,BLT_ITEM,0);
				if(b)
				{
					b->anim=IT_SCROLL;
					b->target=SKILL_DEFENSE;
				}
			}
			if(player.var[VAR_QUESTASSIGN+QUEST_TOYS] && !player.var[VAR_QUESTDONE+QUEST_TOYS] && player.var[VAR_TOYSDEAD]<200)
				PlayerSetVar(VAR_TOYSDEAD,player.var[VAR_TOYSDEAD]+1);
			player.var[VAR_MAGICHAT]=1;
			break;
		case MONS_MONKEY2:	// stinky sock boss
			if(!SkillHave(SKILL_POISON))
			{
				b=FireBullet(goodguy->ID,x,y,FIXAMT*15,rand()%256,BLT_ITEM,0);
				if(b)
				{
					b->anim=IT_SCROLL;
					b->target=SKILL_POISON;
				}
			}
			if(player.var[VAR_QUESTASSIGN+QUEST_TOYS] && !player.var[VAR_QUESTDONE+QUEST_TOYS] && player.var[VAR_TOYSDEAD]<200)
				PlayerSetVar(VAR_TOYSDEAD,player.var[VAR_TOYSDEAD]+1);
			player.var[VAR_STINKYSOCK]=1;
			break;
		case MONS_ROSE:	// Stone Rose boss
			if(!SkillHave(SKILL_BRAMBLES))
			{
				b=FireBullet(goodguy->ID,x,y,FIXAMT*15,rand()%256,BLT_ITEM,0);
				if(b)
				{
					b->anim=IT_SCROLL;
					b->target=SKILL_BRAMBLES;
				}
			}
			player.var[VAR_STONEROSE]=1;
			break;
		case MONS_KLONK:
			if(GotItem(IT_KEY1)==0)
			{
				b=FireBullet(goodguy->ID,x,y,FIXAMT*15,rand()%256,BLT_ITEM,0);
				if(b)
				{
					b->anim=IT_KEY1;
					b->target=0;
				}
			}
			player.var[VAR_KLONKDEAD]=1;
			break;
		case MONS_TEDDY3:	// berserker bear boss
			if(!SkillHave(SKILL_BERSERK))
			{
				b=FireBullet(goodguy->ID,x,y,FIXAMT*15,rand()%256,BLT_ITEM,0);
				if(b)
				{
					b->anim=IT_SCROLL;
					b->target=SKILL_BERSERK;
				}
			}
			PlayerSetVar(VAR_QUESTDONE+QUEST_BIGBEAR,1);
			if(player.var[VAR_QUESTASSIGN+QUEST_TOYS] && !player.var[VAR_QUESTDONE+QUEST_TOYS] && player.var[VAR_TOYSDEAD]<200)
				PlayerSetVar(VAR_TOYSDEAD,player.var[VAR_TOYSDEAD]+1);
			break;
		case MONS_TOYPOWER:
			PlayerSetVar(VAR_QUESTDONE+QUEST_FACTORY2,1);
			break;
		case MONS_BURNINGBUSH:
			if(!SkillHave(SKILL_BURNINATE))
			{
				b=FireBullet(goodguy->ID,x,y,FIXAMT*15,rand()%256,BLT_ITEM,0);
				if(b)
				{
					b->anim=IT_SCROLL;
					b->target=SKILL_BURNINATE;
				}
			}
			player.var[VAR_BUSHDEAD]=1;
			break;
		case MONS_TRIGUN:
			if(!SkillHave(SKILL_TRIPLE))
			{
				b=FireBullet(goodguy->ID,x,y,FIXAMT*15,rand()%256,BLT_ITEM,0);
				if(b)
				{
					b->anim=IT_SCROLL;
					b->target=SKILL_TRIPLE;
				}
			}
			player.var[VAR_TRIGUNDEAD]=1;
			if(player.var[VAR_QUESTASSIGN+QUEST_TOYS] && !player.var[VAR_QUESTDONE+QUEST_TOYS] && player.var[VAR_TOYSDEAD]<200)
				PlayerSetVar(VAR_TOYSDEAD,player.var[VAR_TOYSDEAD]+1);
			break;
		case MONS_JUNKSMITH:
			player.var[VAR_JUNKDEAD]=1;
			curMap->map[14+20*curMap->width].item=IT_NONE;	// clear the block so you can get in the junkyard
			break;
		case MONS_BIGPLANT:
			player.var[VAR_GOURDDEAD]=1;
			break;
		case MONS_TITAN2:
			DropJunkItem(IT_ICECUBE,x,y);
			switch(deathTag)
			{
				case 1:
				case 3:
				case 5:
				case 7:
				case 9:
					b=FireBullet(goodguy->ID,x,y,FIXAMT*15,rand()%256,BLT_ITEM,0);
					if(b)
					{
						b->anim=IT_PRESENT;
						b->target=77+(deathTag-1)/2;
					}
					break;
				case 2:
					if(!SkillHave(SKILL_LUCK))
					{
						b=FireBullet(goodguy->ID,x,y,FIXAMT*15,rand()%256,BLT_ITEM,0);
						if(b)
						{
							b->anim=IT_SCROLL;
							b->target=SKILL_LUCK;
						}
					}
					break;
				case 4:
					if(!SkillHave(SKILL_PERSUADE))
					{
						b=FireBullet(goodguy->ID,x,y,FIXAMT*15,rand()%256,BLT_ITEM,0);
						if(b)
						{
							b->anim=IT_SCROLL;
							b->target=SKILL_PERSUADE;
						}
					}
					break;
				case 6:
					if(!SkillHave(SKILL_TURRET))
					{
						b=FireBullet(goodguy->ID,x,y,FIXAMT*15,rand()%256,BLT_ITEM,0);
						if(b)
						{
							b->anim=IT_SCROLL;
							b->target=SKILL_TURRET;
						}
					}
					break;
				case 8:
					if(!SkillHave(SKILL_ICECUBE))
					{
						b=FireBullet(goodguy->ID,x,y,FIXAMT*15,rand()%256,BLT_ITEM,0);
						if(b)
						{
							b->anim=IT_SCROLL;
							b->target=SKILL_ICECUBE;
						}
					}
					break;
				case 10:
					if(!SkillHave(SKILL_PLAGUE))
					{
						b=FireBullet(goodguy->ID,x,y,FIXAMT*15,rand()%256,BLT_ITEM,0);
						if(b)
						{
							b->anim=IT_SCROLL;
							b->target=SKILL_PLAGUE;
						}
					}
					break;
			}
			break;
		case MONS_BIRD:
			AchieveNow(0);
			if(player.var[VAR_MADCAP])
				AchieveNow(22);
			if(player.playClock<30*60*60*4)
				AchieveNow(33);
			PlayerSetVar(VAR_QUESTDONE+QUEST_HAPPYSTICK,1);
			if(CalcPercent(&player)>=100.0f)
				AchieveNow(11);
			break;
	}

	if(ModifierOn(MOD_ARENA))
		money=0;

	DropMoney(money,x,y);
}

void DropMoney(int money,int x,int y)
{
	bullet_t *b;
	int i;

	if(money>=5)
	{
		for(i=0;i<money/5;i++)
		{
			b=FireBullet(goodguy->ID,x,y,FIXAMT*15,rand()%256,BLT_ITEM,0);
			if(b)
			{
				b->anim=IT_TENCOIN;
				b->timer=30*10;
			}
		}
		money=money%5;
	}
	for(i=0;i<money;i++)
	{
		b=FireBullet(goodguy->ID,x,y,FIXAMT*15,rand()%256,BLT_ITEM,0);
		if(b)
		{
			b->anim=IT_COIN;
			b->timer=30*10;
		}
	}
}

void NovaGuys(byte team,int x,int y,int radius,word dmg,byte stun,word knock)
{
	int i;
	int dx,dy;
	byte ang;

	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type && guys[i]->hp && guys[i]->team!=team && guys[i]->z==0)
		{
			if(Distance(x,y,guys[i]->x,guys[i]->y)<radius)
			{
				ang=AngleFrom(x,y,guys[i]->x,guys[i]->y);
				dx=Cosine(ang)*knock/FIXAMT;
				dy=Sine(ang)*knock/FIXAMT;
				meleeAttacker=NULL;
				if(guys[i]->GetShot(dx,dy,DamageAmt(dmg,team),curMap,&curWorld))
				{
					if(team==EVIL)
						guys[i]->GetFrozen(stun);
					else
						guys[i]->GetStunned(stun);
				}
			}
		}
	}
}

int BulletFindDrainTarget(byte team,byte myFace,int x,int y,int notme,Map *map,world_t *world)
{
	int i;
	int range;
	int score,bestScore;
	Guy *target;
	int ang;

	target=NULL;
	bestScore=999999999;
	for(i=0;i<maxGuys;i++)
		if(guys[i]->type && guys[i]->hp && (guys[i]->team&team)==0 && guys[i]->ID!=(notme-1) &&
			!(MonsterFlags(guys[i]->type)&(MF_NOHIT|MF_INVINCIBLE)) && map->CheckLOS(goodguy->mapx,goodguy->mapy,20,guys[i]->mapx,guys[i]->mapy))
		{
			ang=AngleDiff(myFace,AngleFrom(x,y,guys[i]->x,guys[i]->y));
			range=Distance(x,y,guys[i]->x,guys[i]->y);
			score=(range/FIXAMT)*(ang*2);
			if(range<320*FIXAMT && score<bestScore && NotBeingDrained(guys[i]->ID))
			{
				target=guys[i];
				bestScore=score;
			}
		}

	if(target==NULL)
		return 0;
	else
		return (target->ID+1);
}

int BulletFindTarget(byte team,byte myFace,int x,int y,int notme,Map *map,world_t *world)
{
	int i;
	int range;
	int score,bestScore;
	Guy *target;
	int ang;

	target=NULL;
	bestScore=999999999;
	for(i=0;i<maxGuys;i++)
		if(guys[i]->type && guys[i]->hp && (guys[i]->team&team)==0 && guys[i]->ID!=(notme-1) &&
			!(MonsterFlags(guys[i]->type)&(MF_NOHIT|MF_INVINCIBLE)))
		{
			ang=AngleDiff(myFace,AngleFrom(x,y,guys[i]->x,guys[i]->y));
			range=Distance(x,y,guys[i]->x,guys[i]->y);
			score=(range/FIXAMT)*(ang*2);
			if(range<320*FIXAMT && score<bestScore)
			{
				target=guys[i];
				bestScore=score;
			}
		}

	if(target==NULL)
		return 0;
	else
		return (target->ID+1);
}

int BulletFindTargetClosest(byte team,int x,int y,int notme,int maxRange,Map *map,world_t *world)
{
	int i;
	int range;
	int score,bestScore;
	Guy *target;

	target=NULL;
	bestScore=999999999;
	for(i=0;i<maxGuys;i++)
		if(guys[i]->type && guys[i]->hp && (guys[i]->team&team)==0 && guys[i]->ID!=(notme-1) &&
			!(MonsterFlags(guys[i]->type)&(MF_NOHIT|MF_INVINCIBLE)))
		{
			range=Distance(x,y,guys[i]->x,guys[i]->y);
			score=range;
			if(range<maxRange && score<bestScore)
			{
				target=guys[i];
				bestScore=score;
			}
		}

	if(target==NULL)
		return 0;
	else
		return (target->ID+1);
}

Guy *SummonGuy(int x,int y,int radius,byte type)
{
	int tries;
	Guy *g;
	int sx,sy;

	tries=100;
	g=NULL;

	while(tries)
	{
		sx=x-radius+(rand()%(radius*2+1));
		sy=y-radius+(rand()%(radius*2+1));
		g=AddGuy(sx,sy,0,type);
		if(g && g->CanWalkNoMons(sx,sy,curMap,&curWorld))
		{
			return g;
		}
		else if(g)
			g->type=MONS_NONE;
		tries--;
	}
	return g;
}

word GuyCount(byte type)
{
	word w;
	int i;

	w=0;
	for(i=0;i<maxGuys;i++)
		if(guys[i]->type==type && guys[i]->hp)
			w++;

	return w;
}

void KillWeakest(byte type)
{
	int id;
	int i;

	id=-1;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type==type && guys[i]->hp && (id==-1 || guys[id]->hp>guys[i]->hp))
		{
			id=i;
		}

	if(id!=-1)
	{
		guys[id]->hp=1;
		meleeAttacker=NULL;
		guys[id]->GetShotReal(0,0,9999,curMap,&curWorld);
	}
}

void GuruVanish(void)
{
	int i;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type==MONS_VILLAGER && guys[i]->tag==12 && curMap->map[guys[i]->mapx+(guys[i]->mapy+1)*curMap->width].tag==player.var[VAR_GURUTALENT])
		{
			guys[i]->mind=2;	// take off!
			guys[i]->z=FIXAMT;
			guys[i]->dz=0;
			guys[i]->mind1=0;
			guys[i]->facing=76;
			return;
		}
}

void ToastyBoom(void)
{
	int i;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type==MONS_TOASTY)
		{
			MakeSound(SND_BOOM,guys[i]->x,guys[i]->y,SND_CUTOFF|SND_RANDOM,500);
			FireBullet(goodguy->ID,guys[i]->x,guys[i]->y,0,0,BLT_SMALLBOOM,(word)(SpecificSkillVal(1,SKILL_TOASTIES)*SpellDamageBoost(SC_FIRE)/100.0f));
			guys[i]->type=MONS_NONE;
			return;
		}
}

void FreezeAllEnemies(void)
{
	int i;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type && guys[i]->hp && guys[i]->team==2 && guys[i]->frozen<30 && guys[i]->active)
			guys[i]->GetFrozen(30);
}

void HealAllGoodguys(byte amt)
{
	int i;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type && guys[i]->hp && guys[i]->team==1)
		{
			if(guys[i]->type!=player.monsType)
				HealGuy(guys[i],amt);
			else
				HealGoodguy(amt);
		}
}

void AddCaravans(void)
{
	Guy *g;

	if(player.levelNum==LVL_HIGHWAY && (player.timeToFinish%30)==0)
	{
		g=AddGuy((31*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(144*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,0,MONS_CARAVAN);
		if(player.var[VAR_SNUGGLY] && player.var[VAR_QUESTASSIGN+QUEST_HIGHWAY] && player.var[VAR_HIGHWAY]==0)
		{
			g->team=GOOD;	// on the highway mission, caravans are friends
			g->hp=1;		// and they take one hit to kill
			if(!TaggedMonsterExists(2))
			{
				PlayerSetVar(VAR_HIGHWAY,1);
				// did it!
			}
		}
	}
}

void AddHighwayPatrolFoes(Map *map)
{
	Guy *g;
	int i,x,y;
	byte tries;
	byte type;

	for(i=0;i<70;i++)
	{
		tries=200;
		type=MONS_BRUISER;
		if(i%7)
			type=MONS_THUG;

		while(tries-->0)
		{
			x=rand()%map->width;
			y=rand()%map->height;


			g=AddGuy((int)(x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(int)(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,0,type);
			if(g && !g->CanWalk(g->x,g->y,map,&curWorld))
			{
				g->type=MONS_NONE;
				continue;
			}
			else
			{
				g->tag=2;
				tries=0;
			}
		}
	}
}

void AddRumbleGuys2(Map *map,byte row,byte team)
{
	Guy *g;
	int i,x,y;
	byte tries;
	byte type;

	for(i=0;i<40+(20*(row==1));i++)
	{
		if(team==0 && i<(40+(20*(row==1)))/3)
			continue;	// 10 less guys for bunny team might help

		tries=200;
		if(team==0)
			type=MONS_EVILBOT;
		else
			type=MONS_BRUISER;
		if(i%7)
		{
			if(team==0)
				type=MONS_BUNNYTROOP;
			else
				type=MONS_THUG;
		}

		while(tries-->0)
		{
			x=rand()%map->width;

			y=rand()%5;
			if(row==0)
				y=6+y;
			else
				y=33-y;

			g=AddGuy((int)(x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(int)(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,0,type);
			if(g && !g->CanWalk(g->x,g->y,map,&curWorld))
			{
				g->type=MONS_NONE;
				continue;
			}
			else
			{
				if(row==0)
				{
					g->team=GOOD;
					g->tag=1;
				}
				else
				{
					g->team=EVIL;
					g->tag=2;
				}
				tries=0;
			}
		}
	}
}

void AddRumbleGuys(Map *map)
{
	AddRumbleGuys2(map,0,1-player.var[VAR_SNUGGLY]);
	AddRumbleGuys2(map,1,player.var[VAR_SNUGGLY]);
}

void TornadoGuys(byte team,int x,int y,int radius,Map *map,world_t *world)
{
	int i,mapx,mapy;
	int range;
	int ang;

	mapx=x/(TILE_WIDTH*FIXAMT);
	mapy=y/(TILE_HEIGHT*FIXAMT);

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type && guys[i]->hp && (guys[i]->team&team)==0 &&
			!(MonsterFlags(guys[i]->type)&(MF_NOMOVE|MF_NOHIT)) && map->CheckLOS(mapx,mapy,30,guys[i]->mapx,guys[i]->mapy))
		{
			range=Distance(x,y,guys[i]->x,guys[i]->y);
			if(range<radius)
			{
				ang=AngleFrom(guys[i]->x,guys[i]->y,x,y);

				// bosses and player are pulled half as much
				guys[i]->kbdx+=Cosine(ang)*5/(1+((monsType[guys[i]->type].location&ML_BOSS)!=0)+(guys[i]->type==player.monsType));
				guys[i]->kbdy+=Sine(ang)*5/(1+((monsType[guys[i]->type].location&ML_BOSS)!=0)+(guys[i]->type==player.monsType));
				Clamp(&guys[i]->kbdx,FIXAMT*80);
				Clamp(&guys[i]->kbdy,FIXAMT*80);
			}
		}
}

void ExplodePlagueRats(void)
{
	int i;
	for(i=0;i<maxGuys;i++)
		if(guys[i]->type==MONS_MOUSE4 && guys[i]->hp && (guys[i]->team==GOOD))
		{
			guys[i]->hp=1;
			guys[i]->reload=0;
			guys[i]->GetShotReal(0,0,9999,curMap,&curWorld);
		}
}

void RepairClockbot(word amt)
{
	int i,x;

	if(amt==0)
		return;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type==MONS_YOURBOT)
		{
			if(guys[i]->hp<MonsterHP(MONS_YOURBOT,GOOD))
			{
				x=guys[i]->hp+amt;
				if(x>MonsterHP(MONS_YOURBOT,GOOD))
					x=MonsterHP(MONS_YOURBOT,GOOD);
				NumberParticle(x-guys[i]->hp,guys[i]->x,guys[i]->y+1,1);
				guys[i]->hp=x;
				player.var[VAR_CLOCKLIFE]=guys[i]->hp&0xFF;
				player.var[VAR_CLOCKLIFE2]=(guys[i]->hp>>8);
				if(amt==9999)
				{
					MakeNormalSound(SND_PUPPETHAT);
					NewMessage("Clockbot repaired!",0);
				}
				return;
			}
		}
}

void EvilEye(byte mx,byte my,byte facing)
{
	int i;
	byte dist;
	int best;

	best=-1;
	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->hp && guys[i]->type && guys[i]->team==EVIL)
		{
			switch(facing)
			{
				case 0:
					if(guys[i]->mapy==my && guys[i]->mapx>mx && guys[i]->mapx-mx<=10)	// in line of sight!
					{
						if(best==-1 || (dist>guys[i]->mapx-mx))
						{
							if(curMap->CheckLOS(mx,my,10,guys[i]->mapx,guys[i]->mapy))
							{
								best=i;
								dist=guys[i]->mapx-mx;
							}
						}
					}
					break;
				case 1:
					if(guys[i]->mapx>mx && guys[i]->mapy>my && (guys[i]->mapx-mx)==(guys[i]->mapy-my) && (guys[i]->mapx-mx)<=10)	// in LOS
					{
						if(best==-1 || (dist>guys[i]->mapx-mx))
						{
							if(curMap->CheckLOS(mx,my,10,guys[i]->mapx,guys[i]->mapy))
							{
								best=i;
								dist=guys[i]->mapx-mx;
							}
						}
					}
					break;
				case 2:
					if(guys[i]->mapx==mx && guys[i]->mapy>my && guys[i]->mapy-my<=10)	// in line of sight!
					{
						if(best==-1 || (dist>guys[i]->mapy-my))
						{
							if(curMap->CheckLOS(mx,my,10,guys[i]->mapx,guys[i]->mapy))
							{
								best=i;
								dist=guys[i]->mapy-my;
							}
						}
					}
					break;
				case 3:
					if(guys[i]->mapx<mx && guys[i]->mapy>my && (mx-guys[i]->mapx)==(guys[i]->mapy-my) && (mx-guys[i]->mapx)<=10)	// in LOS
					{
						if(best==-1 || (dist>mx-guys[i]->mapx))
						{
							if(curMap->CheckLOS(mx,my,10,guys[i]->mapx,guys[i]->mapy))
							{
								best=i;
								dist=mx-guys[i]->mapx;
							}
						}
					}
					break;
				case 4:
					if(guys[i]->mapy==my && guys[i]->mapx<mx && mx-guys[i]->mapx<=10)	// in line of sight!
					{
						if(best==-1 || (dist>mx-guys[i]->mapx))
						{
							if(curMap->CheckLOS(mx,my,10,guys[i]->mapx,guys[i]->mapy))
							{
								best=i;
								dist=mx-guys[i]->mapx;
							}
						}
					}
					break;
				case 5:
					if(guys[i]->mapx<mx && guys[i]->mapy<my && (mx-guys[i]->mapx)==(my-guys[i]->mapy) && (mx-guys[i]->mapx)<=10)	// in LOS
					{
						if(best==-1 || (dist>mx-guys[i]->mapx))
						{
							if(curMap->CheckLOS(mx,my,10,guys[i]->mapx,guys[i]->mapy))
							{
								best=i;
								dist=mx-guys[i]->mapx;
							}
						}
					}
					break;
				case 6:
					if(guys[i]->mapx==mx && guys[i]->mapy<my && my-guys[i]->mapy<=10)	// in line of sight!
					{
						if(best==-1 || (dist>my-guys[i]->mapy))
						{
							if(curMap->CheckLOS(mx,my,10,guys[i]->mapx,guys[i]->mapy))
							{
								best=i;
								dist=my-guys[i]->mapy;
							}
						}
					}
					break;
				case 7:
					if(guys[i]->mapx>mx && guys[i]->mapy<my && (guys[i]->mapx-mx)==(my-guys[i]->mapy) && (guys[i]->mapx-mx)<=10)	// in LOS
					{
						if(best==-1 || (dist>guys[i]->mapx-mx))
						{
							if(curMap->CheckLOS(mx,my,10,guys[i]->mapx,guys[i]->mapy))
							{
								best=i;
								dist=guys[i]->mapx-mx;
							}
						}
					}
					break;
			}
		}
	}
	if(best!=-1)	// found someone to glare at
	{
		MakeSound(SND_IGNITE,guys[best]->x,guys[best]->y,SND_CUTOFF|SND_RANDOM,100);
		FireBullet(goodguy->ID,guys[best]->x-FIXAMT*24+(rand()%(FIXAMT*48+1)),guys[best]->y-FIXAMT*24+(rand()%(FIXAMT*48+1)),0,0,BLT_FLAMEB,CurseDmg(1,GOOD));
	}
}

void AddPuppetPals(void)
{
	int i;
	Guy *g;

	for(i=0;i<5;i++)
	{
		if(player.var[VAR_PUPPETLIFE+i])
		{
			g=AddGuy(goodguy->x,goodguy->y,goodguy->z,MONS_PROTOTYPE);
			if(g)
			{
				g->hp=player.var[VAR_PUPPETLIFE+i];
				g->tag=(byte)i;
			}
		}
	}
}

void RepairPuppets(void)
{
	int i,j;
	Guy *g;

	for(j=0;j<5;j++)
	{
		if(player.var[VAR_PUPPETLIFE+j]==0)
		{
			player.var[VAR_PUPPETLIFE+j]=PUPPETPAL_HEALTH;
			g=AddGuy(goodguy->x,goodguy->y,goodguy->z,MONS_PROTOTYPE);
			if(g)
			{
				g->hp=player.var[VAR_PUPPETLIFE+j];
				g->tag=(byte)j;
			}
		}
		else
		{
			for(i=0;i<maxGuys;i++)
				if(guys[i]->type==MONS_PROTOTYPE && guys[i]->tag==j)
				{
					guys[i]->hp=PUPPETPAL_HEALTH;
					player.var[VAR_PUPPETLIFE+j]=PUPPETPAL_HEALTH;
				}
		}
	}
}

void GroundStomp(word damage,Map *map,world_t *world)
{
	word g;

	g=GuyList_First();
	while(g!=65535)
	{
		if(GetGuy(g)->team==GOOD && GetGuy(g)->hp>0 && GetGuy(g)->z==0)
		{
			GetGuy(g)->GetShot(0,0,DamageAmt(damage,EVIL),map,world);
		}
		g=GuyList_Next();
	}
}

//-------------- GuyList

static word guyVal;

word GuyList_First(void)
{
	guyVal=0;
	while(guys[guyVal]->type==MONS_NONE && guyVal<maxGuys)
		guyVal++;

	if(guyVal>=maxGuys)
		return 65535;

	return guyVal;
}

word GuyList_Next(void)
{
	guyVal++;
	while(guyVal<maxGuys && guys[guyVal]->type==MONS_NONE)
		guyVal++;

	if(guyVal>=maxGuys)
		return 65535;

	return guyVal;
}


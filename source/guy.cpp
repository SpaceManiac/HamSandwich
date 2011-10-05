#include "guy.h"
#include "player.h"
#include "intface.h"
#include "editor.h"
#include "hiscore.h"
#include "shop.h"
#include "goal.h"

Guy **guys;
Guy *goodguy;
int maxGuys;
Guy *guyHit;
Guy *nobody;
byte *changed;

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

	if((MonsterFlags(type,aiType)&MF_FREEWALK) && him->aiType==MONS_BOUAPHA)
		return 0;
	if((MonsterFlags(type,aiType)&MF_ENEMYWALK) && him->aiType!=MONS_BOUAPHA)
		return 0;
	if((MonsterFlags(him->type,him->aiType)&MF_ENEMYWALK) && (aiType!=MONS_BOUAPHA))
		return 0;
	if((MonsterFlags(him->type,him->aiType)&MF_FREEWALK) && (aiType==MONS_BOUAPHA))
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
	mapTile_t *tile;

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
	tile=map->GetTile(destx,desty);

	if(tile->wall)
		return;	// can't push into a wall
	if(tile->item)
		return;	// can't push into any item
	if(GetTerrain(world,tile->floor)->flags&TF_PUSHON)
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
		tile->wall=map->GetTile(x,y)->wall;
		tile->floor=map->GetTile(x,y)->floor;
		map->GetTile(x,y)->floor=GetTerrain(world,tile->floor)->next;
		map->GetTile(x,y)->wall=0;
	}
}

byte TryToPushItem(int x,int y,int destx,int desty,Map *map,world_t *world)
{
	int i;
	int xx,yy;
	mapTile_t *tile;

	if(destx<0 || destx>=map->width || desty<0 || desty>=map->height)
		return 0;	// can't push off edge of map
	tile=map->GetTile(destx,desty);

	if(tile->wall)
		return 0;	// can't push into a wall
	if(tile->item)
		return 0;	// can't push into any item
	if(GetTerrain(world,tile->floor)->flags&TF_PUSHON)
	{
		// it passed every single test. the only thing left is to see if it would overlap
		// any badguys, which would not be good
		xx=destx*TILE_WIDTH;
		yy=desty*TILE_HEIGHT;
		for(i=0;i<maxGuys;i++)
			if((guys[i]) && (guys[i]->type) && (guys[i]->hp>0) &&
				(abs(guys[i]->mapx-destx)<8) && (abs(guys[i]->mapy-desty)<8))
				if(TileBonkCheck(xx,yy,guys[i]))
					return 0;	// the pushy block would hit the guy

		// the floor is pushonable, let's do it
		tile->item=map->GetTile(x,y)->item;
		map->GetTile(x,y)->item=0;
		return 1;
	}
	else
		return 0;
}

byte Walkable(Guy *me,int x,int y,Map *map,world_t *world)
{
	byte result;
	mapTile_t *m=map->GetTile(x,y);

	result=1;

	result=InteractWithItem(me,m,x,y);

	if(m->wall && !(MonsterFlags(me->type,me->aiType)&MF_WALLWALK))
		result=0;

	if((MonsterFlags(me->type,me->aiType)&MF_WALLWALK) && (GetTerrain(world,m->floor)->flags&TF_NOGHOST))
		result=0;

	if(!me->friendly && (GetTerrain(world,m->floor)->flags&TF_NOENEMY))
		result=0;

	if((GetTerrain(world,m->floor)->flags&(TF_WATER|TF_LAVA)) && (!(MonsterFlags(me->type,me->aiType)&(MF_WATERWALK|MF_FLYING))))
	{
		if(me->aiType!=MONS_BOUAPHA)
			result=0;
	}

	if((GetTerrain(world,m->floor)->flags&TF_SOLID) && (!(MonsterFlags(me->type,me->aiType)&(MF_FLYING|MF_WALLWALK))))
		result=0; // impassable

	if((GetTerrain(world,m->floor)->flags&TF_PUSHY) && me->aiType==MONS_BOUAPHA)
	{
		TryToPush(me,x,y,map,world);
	}

	if(MonsterFlags(me->type,me->aiType)&MF_FLYING)
	{
		if((m->wall || (GetItem(m->item)->flags&IF_BULLETPROOF)) && (!(MonsterFlags(me->type,me->aiType)&MF_WALLWALK)))
			result=0;
	}
	else if(MonsterFlags(me->type,me->aiType)&MF_AQUATIC)
	{
		if(!(GetTerrain(world,m->floor)->flags&(TF_WATER|TF_LAVA)))
			result=0;
		if(m->wall)
			result=0;
	}

	if(!result)	// bumped a wall, see if that triggers a special
	{
		if(x!=me->lastBumpX || y!=me->lastBumpY && me->type!=MONS_NOBODY)
			EventOccur(EVT_STEP,me->ID,x,y,me);
		me->lastBumpX=x;
		me->lastBumpY=y;
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

	if(MonsterFlags(type,aiType)&MF_FREEWALK)
		return result;	// can't have a guy collision

	if(aiType==MONS_BOUAPHA && (player.vehicle==VE_MINECART || player.vehicle==VE_YUGO))
	{
		// be bigger, so you can smack into badguys better
		rectx-=10;
		rectx2+=10;
		recty-=10;
		recty2+=10;
	}
	if(result)	// no wall collision, look for guy collision
		for(i=0;i<maxGuys;i++)
			if((guys[i]) && (guys[i]!=this) && (guys[i]->type) && (guys[i]->hp>0) &&
				(abs(guys[i]->mapx-mapx)<8) && (abs(guys[i]->mapy-mapy)<8))
			{
				if(guys[i]!=parent && guys[i]->parent!=this && CoconutBonk(xx,yy,guys[i]))
				{
					if(aiType==MONS_BOUAPHA && (player.vehicle==VE_MINECART || player.vehicle==VE_YUGO)
						&& parent && ((player.vehicle==VE_MINECART && parent->mind1>20) || player.vehicle==VE_YUGO))
					{
						// if you hit someone while riding a minecart or yugo, they get hurt bad
						guys[i]->GetShot(parent->dx,parent->dy,30,map,world);
						if(guys[i]->hp==0 && !editing && !player.cheated)
							profile.progress.runOver++;
					}
					else
					{
						if(aiType==MONS_BOUAPHA && (player.vehicle==VE_MINECART || player.vehicle==VE_YUGO))
						{
							// be bigger, so you can smack into badguys better
							rectx+=10;
							rectx2-=10;
							recty+=10;
							recty2-=10;
						}
						return 0;	// hit heads with somebody
					}
				}
			}

	if(aiType==MONS_BOUAPHA && (player.vehicle==VE_MINECART || player.vehicle==VE_YUGO))
	{
		// be bigger, so you can smack into badguys better
		rectx+=10;
		rectx2-=10;
		recty+=10;
		recty2-=10;
	}
	return result;
}

void Guy::SeqFinished(void)
{
	if((seq==ANIM_DIE) || (seq==ANIM_A3 && aiType==MONS_BOUAPHA && player.weapon!=WPN_PWRARMOR && player.weapon!=WPN_MINISUB))
	{
		if(aiType==MONS_BOUAPHA)
		{
			if(player.weapon==WPN_PWRARMOR)
			{
				player.weapon=0;
				seq=ANIM_IDLE;
				frm=0;
				frmAdvance=128;
				action=ACTION_IDLE;
				return;
			}
			else if(player.weapon==WPN_MINISUB)
			{
				player.weapon=0;
				seq=ANIM_IDLE;
				frm=0;
				frmAdvance=128;
				action=ACTION_IDLE;
				if(!CanWalk(x,y,curMap,&curWorld))
				{
					SendMessageToGame(MSG_RESET,0);
					NewMessage("Bad place to hop out!",30,1);
				}
				return;
			}
			else
				// restart current level
				SendMessageToGame(MSG_RESET,0);
		}

		type=MONS_NONE;
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
	byte tp;
	byte *anim;

	frm++;

	tp=type;

	if(aiType==MONS_BOUAPHA && player.weapon==WPN_PWRARMOR)
		anim=GetMonsterType(MONS_PWRBOUAPHA)->anim[seq];
	else if(aiType==MONS_BOUAPHA && player.weapon==WPN_MINISUB)
		anim=GetMonsterType(MONS_MINISUB)->anim[seq];
	else
		anim=MonsterAnim(type,seq);
	if(anim[frm]==255)
		// this sequence is done
		SeqFinished();

	if(type==MONS_NONE)
	{
		type=tp;
		type=MONS_NONE;
		return;	// seqfinished may have killed the guy
	}
}

void Guy::CalculateRect(void)
{
	sprite_t *spr;
	byte s;

	if(MonsterFlags(type,aiType)&MF_SPRITEBOX)
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
		if(aiType==MONS_BOUAPHA && player.weapon==WPN_PWRARMOR)
			s=MonsterSize(MONS_PWRBOUAPHA);
		if(aiType==MONS_BOUAPHA && player.weapon==WPN_MINISUB)
			s=MonsterSize(MONS_MINISUB);
		else
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

	if(type==MONS_NONE)
		return;

	if(type!=MONS_NOBODY)
	{
		if(x<0)
			x=0;
		if(y<0)
			y=0;
		if(x>=(map->width*TILE_WIDTH*FIXAMT))
			x=map->width*TILE_WIDTH*FIXAMT-1;
		if(y>=(map->height*TILE_HEIGHT*FIXAMT))
			y=map->height*TILE_HEIGHT*FIXAMT-1;
	}
	if(hp==0 && seq!=ANIM_DIE && (aiType!=MONS_BOUAPHA || seq!=ANIM_A3))
	{
		// MEGA HACK!?  Why on earth?!?!?!!?!?!!?  Something to do with mine cart kills
		// resurrecting the little bastard bats
		RemoveGuy(this);
		return;
	}

	if(parent && parent->type==0)
	{
		if(aiType==MONS_BOUAPHA)
			player.vehicle=0;
		parent=NULL;	// disparentize!
	}

	oldmapx=mapx;
	oldmapy=mapy;

	CalculateRect();

	if(frozen>0)
	{
		frozen--;
		if(hp==0)
			frozen=0;
		if(ouch)
		{
			if(frozen>3)
				frozen-=4;
			ouch--;
		}

		if(frozen)
			return;
	}

	if(!frozen)
	{
		if(aiType==MONS_BOUAPHA)	// special case, player controls Bouapha
			PlayerControlMe(this,map->GetTile(mapx,mapy),world);
		else
			MonsterControl(map,world);
	}

	if(ouch>0)
		ouch--;

	if(poison>0)
	{
		if(hp>0 && ((poison&7)==0))
		{
			if(aiType==MONS_BOUAPHA && !editing && !player.cheated)
				profile.progress.damageTaken++;

			hp--;
			if(hp==0)
			{
				hp=1;
				GetShot(0,0,1,map,world);
			}
		}
		poison--;
	}

	oldx=x;
	oldy=y;

	x+=dx;

	b=0;
	if(!CanWalk(x,y,map,world))
	{
		x-=dx;
		if(aiType==MONS_MINECART)
			x=(mapx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;

		if(aiType==MONS_ROLLER || aiType==MONS_ROLLER2 || aiType==MONS_CENTIHEAD || aiType==MONS_DRL ||
			aiType==MONS_BOUAPHA || aiType==MONS_VAMPIRE || aiType==MONS_PKSTEIN || aiType==MONS_TRICEROID ||
			aiType==MONS_BUNNY || aiType==MONS_LOONYSHIP || aiType==MONS_FRIENDLY || aiType==MONS_SNOWBALL ||
			aiType==MONS_SNOWBALL2 || aiType==MONS_ROLLER3 || aiType==MONS_ROLLER4 || aiType==MONS_DARKVAMP ||
			aiType==MONS_FRIENDLY2 || aiType==MONS_CRAZYPANTS || aiType==MONS_AUTOZOID || aiType==MONS_YUGO ||
			aiType==MONS_PATTY || aiType==MONS_PATROLLR || aiType==MONS_PATROLUD || aiType==MONS_DPATROLLR || aiType==MONS_DPATROLUD ||
			aiType==MONS_MINIWACKO || aiType==MONS_JACKFROST)
			mind1=1;	// tell it that it hit a wall
		b=1;
	}

	y+=dy;

	if(!CanWalk(x,y,map,world))
	{
		y-=dy;
		if(aiType==MONS_MINECART)
			y=(mapy*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;

		if(aiType==MONS_ROLLER || aiType==MONS_ROLLER2 || aiType==MONS_CENTIHEAD || aiType==MONS_DRL ||
			aiType==MONS_BOUAPHA || aiType==MONS_VAMPIRE || aiType==MONS_PKSTEIN || aiType==MONS_TRICEROID ||
			aiType==MONS_BUNNY || aiType==MONS_LOONYSHIP || aiType==MONS_FRIENDLY || aiType==MONS_SNOWBALL ||
			aiType==MONS_SNOWBALL2 || aiType==MONS_ROLLER3 || aiType==MONS_ROLLER4 || aiType==MONS_DARKVAMP ||
			aiType==MONS_FRIENDLY2 || aiType==MONS_CRAZYPANTS || aiType==MONS_AUTOZOID || aiType==MONS_YUGO ||
			aiType==MONS_PATTY || aiType==MONS_PATROLLR || aiType==MONS_PATROLUD || aiType==MONS_DPATROLLR || aiType==MONS_DPATROLUD ||
			aiType==MONS_MINIWACKO || aiType==MONS_JACKFROST)
			mind1+=2;	// tell it that it hit a wall
		b=1;
	}

	if(b==0)
	{
		// if you haven't bumped anything this frame, then forget what you last bumped
		lastBumpX=255;
		lastBumpY=255;
	}

	if(MonsterFlags(type,aiType)&MF_FLYING)
	{
		if(map->flags&MAP_UNDERWATER)
		{
			// swimming is slower than flying
			if(z<20*FIXAMT)	// go up if you need to
				dz+=FIXAMT/4;
			else
				dz-=FIXAMT/4;
			Clamp(&dz,FIXAMT*2);
		}
		else
		{
			if(z<20*FIXAMT)	// go up if you need to
				dz+=FIXAMT;
			else
				dz-=FIXAMT;
		}

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
			if(!(MonsterFlags(type,aiType)&MF_NOGRAV))
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
	{
		facing=0;
		return;	// NextFrame may have killed you
	}

	mapx=(x>>FIXSHIFT)/TILE_WIDTH;
	mapy=(y>>FIXSHIFT)/TILE_HEIGHT;

	if(aiType==MONS_BOUAPHA)	// special case, Bouapha is the player, follow him
	{
		if(player.vehicle==0 && !editing && !player.cheated)
		{
			profile.progress.footDistance+=abs(dx/FIXAMT)+abs(dy/FIXAMT);
		}
		int tdx,tdy;
		tdx=dx;
		tdy=dy;
		if(player.vehicle==VE_YUGO || player.vehicle==VE_MINECART)
		{
			if(goodguy->parent)
			{
				tdx=goodguy->parent->dx;
				tdy=goodguy->parent->dy;
			}
		}
		UpdateCamera(x>>FIXSHIFT,y>>FIXSHIFT,tdx,tdy,map);
		if((map->flags&MAP_TORCHLIT) || player.spotted)
		{
			if(player.spotted)
				player.spotted--;

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
		if(map->flags&MAP_WELLLIT)
		{
			map->DimTorch(mapx,mapy,20);
		}
		if(player.torch)
			map->TempTorch(mapx,mapy,64);

		// standing on water!!!!!!!  DROWN!!!!
		if((hp>0) && (z==0) && (GetTerrain(world,map->GetTile(mapx,mapy)->floor)->flags&TF_WATER)
			&& (!PlayerCanWaterwalk()) && (!(player.vehicle==VE_RAFT)) && (!(player.weapon==WPN_MINISUB)) &&
			(!(player.vehicle==VE_YUGO)) && ((MonsterFlags(goodguy->type,goodguy->aiType)&(MF_AQUATIC|MF_FLYING))==0))
		{
			// if there's a raft, hop on instead of dying
			if(!RaftNearby())
			{
				if(player.weapon==WPN_PWRARMOR)
					player.weapon=0;
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
				if(player.playAs==PLAY_LUNACHIK)
					MakeSound(SND_LUNADROWN,x,y,SND_CUTOFF|SND_ONE,65536);
				else
					MakeSound(SND_DROWN,x,y,SND_CUTOFF|SND_ONE,65536);
				if(player.shield)
					CompleteGoal(57);	// fell in water while shielded
			}
		}
		// standing on lava, OW!
		if((hp>0) && (z==0) && (GetTerrain(world,map->GetTile(mapx,mapy)->floor)->flags&TF_LAVA)
			&& (!PlayerCanWaterwalk()) && (!(player.vehicle==VE_RAFT)) && (!(player.vehicle==VE_YUGO)) && (!(player.weapon==WPN_PWRARMOR || player.weapon==WPN_MINISUB))
			 && ((MonsterFlags(goodguy->type,goodguy->aiType)&(MF_AQUATIC|MF_FLYING))==0))
		{
			if(!RaftNearby())
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
		}
		if((oldmapx!=mapx || oldmapy!=mapy) &&
			(GetTerrain(world,map->GetTile(mapx,mapy)->floor)->flags&TF_STEP))
		{
			map->GetTile(mapx,mapy)->floor=GetTerrain(world,map->GetTile(mapx,mapy)->floor)->next;
		}
	}
	if(oldmapx!=mapx || oldmapy!=mapy && type!=MONS_NOBODY)
		EventOccur(EVT_STEP,ID,mapx,mapy,this);

	if(mapx>=0 && mapy>=0 && mapx<map->width && mapy<map->height)
		bright=map->GetTile(mapx,mapy)->templight;
	else
		bright=-32;
}

// this is a very half-assed version of update to be called by the editor instead of the game
void Guy::EditorUpdate(Map *map)
{
	int mapx,mapy;

	mapx=(x>>FIXSHIFT)/TILE_WIDTH;
	mapy=(y>>FIXSHIFT)/TILE_HEIGHT;
	if(mapx>=0 && mapy>=0 && mapx<map->width && mapy<map->height)
		bright=map->GetTile(mapx,mapy)->templight;
	else
		bright=-32;
}

void Guy::Render(byte light)
{
	byte oldFrom,oldTo;
	char oldBrt;
	byte t;

	if(type==MONS_NONE)
		return;

	if(aiType==MONS_BOUAPHA && (player.vehicle==VE_YUGO || player.vehicle==VE_MINECART))
		return;	// don't render him if he's in a car or minecart

	if(aiType==MONS_MINECART && player.vehicle==VE_MINECART && goodguy->parent==this)
	{
		goodguy->x=x;
		goodguy->y=y+1;
		player.vehicle=VE_NONE;
		goodguy->Render(light);
		player.vehicle=VE_MINECART;
	}

	if(type==MONS_BOUAPHA)
	{
		if(player.playAs==PLAY_LUNATIC)
			t=MONS_DRL;
		else if(player.playAs==PLAY_HAPPY)
			t=MONS_STICKMAN;
		else if(player.playAs==PLAY_MECHA)
			t=MONS_PLAYMECHA;
		else if(player.playAs==PLAY_SHROOM)
			t=MONS_PLAYSHROOM;
		else if(player.playAs==PLAY_LUNACHIK)
			t=MONS_LUNACHICK;
		else
			t=MONS_BOUAPHA;

		if(player.weapon==WPN_PWRARMOR)
			t=MONS_PWRBOUAPHA;
		if(player.weapon==WPN_MINISUB)
			t=MONS_MINISUB;
	}
	else
		t=type;

	if(fromColor!=255)
	{
		oldFrom=GetMonsterType(t)->fromCol;
		oldTo=GetMonsterType(t)->toCol;
		GetMonsterType(t)->fromCol=fromColor;
		GetMonsterType(t)->toCol=toColor;
	}
	oldBrt=GetMonsterType(t)->brtChg;
	GetMonsterType(t)->brtChg=brtChange;
	MonsterDraw(x,y,z,type,aiType,seq,frm,facing,bright*(light>0),ouch,poison,frozen);
	if(fromColor!=255)
	{
		GetMonsterType(t)->fromCol=oldFrom;
		GetMonsterType(t)->toCol=oldTo;
	}
	GetMonsterType(t)->brtChg=oldBrt;
	if(editing==1 && EditorShowMonsItems())
	{
		RenderItem(x>>FIXSHIFT,(y+FIXAMT*2)>>FIXSHIFT,item,bright*(light>0),MAP_SHOWITEMS);
	}
}

void Guy::MonsterControl(Map *map,world_t *world)
{
	int i;

	if(!goodguy)
		return;	// badguys don't move if you're gone, saves a lot of crash possibilities

	if(mindControl)
	{
		mindControl--;
		if(!mindControl)
		{
			friendly=0;
			player.totalEnemies++;
			target=nobody;
		}
		if((mindControl&3)==0)
			MindControlSparks(x-FIXAMT*15+Random(FIXAMT*15),y-FIXAMT*15+Random(FIXAMT*15),Random(FIXAMT*10),
				FIXAMT*2+Random(FIXAMT*4));
	}

	if(aiType==MONS_BURNER || aiType==MONS_ROLLER || aiType==MONS_ROLLER2 || aiType==MONS_COFFIN || aiType==MONS_DARKCOFFIN ||
		aiType==MONS_ALIENEGG || aiType==MONS_ALIENEGG2 || aiType==MONS_EGGSAC || aiType==MONS_BOILER ||
		aiType==MONS_FRIENDLY || aiType==MONS_SNOWBALL || aiType==MONS_SNOWBALL2 || aiType==MONS_ROLLER3 ||
		aiType==MONS_ROLLER4 || aiType==MONS_FRIENDLY2 || (aiType>=MONS_PATCH && aiType<=MONS_PATCH4) ||
		(aiType>=MONS_SUCKER1 && aiType<=MONS_BLOWER4) || (aiType>=MONS_PARKEDCAR && aiType<=MONS_TRAFFIC2) || aiType==MONS_XENOEGG ||
		aiType==MONS_YUGO || aiType==MONS_CHEST)
	{
		// these monsters don't ever stop functioning, and can't be tricked by
		// invisibility
		target=goodguy;
	}
	else
	{
		// lose sight of the invisible goodguy
		if((player.invisibility || player.stealthy) && target==goodguy)
		{
			target=nobody;	// target the randomly moving, invisible, invincible, ghostly, nobody
		}

		// if you lost your target, or 2% chance of it even if you didn't,
		// time to pick a new target
		if(!target || target->friendly==friendly || target->type==MONS_NONE || target==nobody || Random(50)==0)
		{
			// lost your target one way or another
			if(friendly)
				i=LockOnEvil(x>>FIXSHIFT,y>>FIXSHIFT);
			else
				i=LockOnGood(x>>FIXSHIFT,y>>FIXSHIFT);

			if(i!=65535)
				target=guys[i];
			else
			{
				target=nobody;
			}
		}
		if(target==nobody && !friendly && player.invisibility==0 && player.stealthy==0 && goodguy)
			target=goodguy;
	}

	if(type==MONS_NONE)
		return;

	if(GetMonsterType(aiType)->AI)
		GetMonsterType(aiType)->AI(this,map,world,target);
}

void Guy::GetShot(int dx,int dy,byte damage,Map *map,world_t *world)
{
	int formerHP,newHP;
	byte t;

	t=type;

	if(hp==0)
		return;	// can't shoot a dead guy

	if(type==0)
		return;	// shouldn't have a type 0 guy at all

	if(aiType==MONS_BOUAPHA && PlayerShield())
		return; // invincible when shielded

	if(MonsterFlags(type,aiType)&MF_INVINCIBLE)
		return;	// invincible

	if(aiType==MONS_BOUAPHA && frozen)
		frozen/=2;

	if(profile.difficulty==0 && damage>0)
	{
		if(friendly)
			damage=damage/2;
		else
			damage=damage*2;
		if(damage==0)
			damage=1;
	}
	if(profile.difficulty==2 && damage>0)
	{
		if(friendly)
			damage=damage*2;
		else
			damage=damage/2;
		if(damage==0)
			damage=1;
	}
	if(aiType==MONS_BOUAPHA && (player.weapon==WPN_PWRARMOR || player.weapon==WPN_MINISUB))
	{
		// damage is done to the armor instead
		if(player.ammo>damage)
			player.ammo-=damage;
		else
			player.ammo=0;

		ouch=4;	// still do the ouch so you can see it
		return;
	}

	formerHP=hp;
	ouch=4;
	hp-=damage;

	if(aiType==MONS_BOUAPHA && (profile.progress.purchase[modeShopNum[MODE_LUDICROUS]]&SIF_ACTIVE))
		hp=0;	// die instantly!

	if(!(MonsterFlags(type,aiType)&MF_NOMOVE))
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

	if(!editing && !player.cheated && verified)
	{
		if(profile.difficulty==0)
		{
			if(aiType==MONS_BOUAPHA)
				profile.progress.damageTaken+=damage*2;
			else
				profile.progress.damageDone+=damage/2;
		}
		else if(profile.difficulty==1)
		{
			if(aiType==MONS_BOUAPHA)
				profile.progress.damageTaken+=damage;
			else
				profile.progress.damageDone+=damage;
		}
		else
		{
			if(aiType==MONS_BOUAPHA)
				profile.progress.damageTaken+=damage/2;
			else
				profile.progress.damageDone+=damage*2;
		}
	}

	newHP=hp;

	if(hp<=0)
	{
		hp=0;
		GoalKilledSomebody(this,type,frozen);
		frozen=0;
		newHP=0;
		seq=ANIM_DIE;
		if(aiType==MONS_BOUAPHA && parent && parent->aiType==MONS_SUPERZOMBIE)
		{
			parent=NULL;
		}
		this->dx=0;
		this->dy=0;
		frm=0;
		frmAdvance=64;
		frmTimer=0;
		action=ACTION_BUSY;
		if(type==MONS_MUSH || type==MONS_SVEN || type==MONS_BJORN || type==MONS_OLAF || type==MONS_SHOCKTR ||
			type==MONS_TROOPER2 || type==MONS_PATTY || type==MONS_SCARAB)
			facing=2;	// these can only die facing forward, artistically speaking
		// possible item drop
		if(type==MONS_ZOMBIE || type==MONS_MUTANT)	// zombies always drop a brain
		{
			if(!map->DropItem(mapx,mapy,ITM_BRAIN))
			{
				PlayerGetBrain(1);	// if you can't drop it, just give it to the player!
			}
		}
		else if(type==MONS_SUPERZOMBIE)	// super zombies always drop 2 brains
		{
			if(!map->DropItem(mapx,mapy,ITM_BRAIN))
			{
				PlayerGetBrain(1);
			}
			if(!map->DropItem(mapx,mapy,ITM_BRAIN))
			{
				PlayerGetBrain(1);
			}
		}
		if(aiType==MONS_GNOME)
		{
			if(mind3!=0)	// drop what you stole!
				if(!map->DropItem(mapx,mapy,mind3))
					map->GetTile(mapx,mapy)->item=mind3;
							// if the drop failed, just force it
		}
		if(aiType!=MONS_CRAZYPANTS || mind==3)
		{
			if(item==ITM_RANDOM)
			{
				if(Random(100*FIXAMT)<map->itemDrops)
					map->DropItem(mapx,mapy,GetRandomItem());
			}
			else if(item!=ITM_NONE)
			{
				if(!map->DropItem(mapx,mapy,item))
					map->GetTile(mapx,mapy)->item=item;	// force the drop if it failed
			}
		}

		ScoreEvent(SE_KILL,1);
		if(player.rage<127*256-512 && player.rageClock==0 && (player.weapon!=WPN_PWRARMOR && player.weapon!=WPN_MINISUB) &&
			player.ability[ABIL_RAGE])
			player.rage+=512;	// and crank up the rage
		if(!editing && !player.cheated && verified)
		{
			profile.progress.kills[type]++;
		}

		EventOccur(EVT_DIE,ID,mapx,mapy,this);

		if(!friendly)
		{
			if(player.combo==0 || player.comboClock==0)
			{
				player.combo=1;
				player.comboClock=60;
			}
			else
			{
				player.combo++;
				if(player.combo>9999)
					player.combo=9999;
				player.comboClock=60;
				if(player.combo>player.bestCombo)
					player.bestCombo=player.combo;
			}
			player.enemiesSlain++;
		}
	}
	if(aiType!=MONS_BOUAPHA)
	{
		if(hp<=maxHP)
			ShowEnemyLife(name,formerHP*128/maxHP,hp*128/maxHP,(byte)(newHP>0));
		else
			ShowEnemyLife(name,128,128,(byte)(newHP>0));
		if(friendly)
		{
			ScoreEvent(SE_OUCH,formerHP-hp);
		}
		else
		{
			ScoreEvent(SE_HURT,formerHP-hp);
		}
	}
	else
	{
		SetPlayerHP(newHP);
		if(newHP==0 && player.vehicle==VE_YUGO)
			player.vehicle=VE_NONE;
	}
}

//-----------------------------------------------------------------------

byte oldPlayAs;

void InitGuys(int max)
{
	int i;

	maxGuys=max;

	changed=(byte *)malloc(sizeof(byte)*maxGuys);
	guys=(Guy **)malloc(sizeof(Guy *)*maxGuys);
	for(i=0;i<maxGuys;i++)
		guys[i]=new Guy();
	goodguy=NULL;
	oldPlayAs=profile.playAs;
}

void ExitGuys(void)
{
	int i;

	for(i=0;i<maxGuys;i++)
		delete guys[i];

	free(changed);
	free(guys);
	profile.playAs=oldPlayAs;
}

void UpdateGuys(Map *map,world_t *world)
{
	int i;
	static byte speedClock=0;

	speedClock++;

	player.clock++;

	if(player.ability[ABIL_BRAIN] && profile.brainRadar && goodguy)
	{
		if(player.brainTime)
			player.brainTime--;
		else
		{
			player.brainTime=15;
			map->FindNearBrain(goodguy->mapx,goodguy->mapy);
			MakeRadar(player.brainX,player.brainY,1);
		}
	}

	if(player.ability[ABIL_CANDLE] && profile.candleRadar && goodguy)
	{
		if(player.candleTime)
			player.candleTime--;
		else
		{
			player.candleTime=15;
			map->FindNearCandle(goodguy->mapx,goodguy->mapy);
			MakeRadar(player.candleX,player.candleY,5);
		}
	}

	if(map->flags&MAP_UNDERWATER)
	{
		if(player.oxygen)
		{
			if(!(player.hammerFlags&HMR_OXYGEN))
				player.oxygen-=32;
			if(player.oxygen<0)
				player.oxygen=0;
		}
		else
		{
			if(goodguy->hp>0)
			{
				player.perfect=0;
				if(!editing && !player.cheated && verified)
					profile.progress.damageTaken++;

				goodguy->hp--;
				if(goodguy->ouch<2)
					goodguy->ouch=2;
				if((goodguy->hp&7)==0)
					MakeNormalSound(SND_BUBBLE);
				if(goodguy->hp==0)
				{
					goodguy->hp=1;
					goodguy->GetShot(0,0,1,map,world);	// ouch!  drowned
				}
			}
		}
	}
	if(map->flags&MAP_LAVA)
	{
		if(player.weapon!=WPN_MINISUB && player.weapon!=WPN_PWRARMOR && player.vehicle!=VE_YUGO)
		{
			player.lavaTimer--;
			if(!player.lavaTimer)
			{
				player.lavaTimer=15;
				if(goodguy->hp>0)
				{
					player.perfect=0;
					if(!editing && !player.cheated && verified)
						profile.progress.damageTaken++;

					goodguy->hp--;
					if(goodguy->ouch<2)
						goodguy->ouch=2;
					if(goodguy->hp==0 && goodguy->seq!=ANIM_DIE)
					{
						goodguy->hp=1;
						goodguy->GetShot(0,0,1,map,world);	// ouch!  burned up!
					}
				}
			}
		}
	}
	if(player.comboClock)
	{
		player.comboClock--;
		if(player.comboClock==0)
			player.combo=0;
	}
	ShouldCheckControls(1);
	for(i=0;i<maxGuys;i++)
		if(guys[i]->type!=MONS_NONE)
		{
			if(guys[i]->aiType==MONS_BOUAPHA && player.speed>0)
			{
				guys[i]->Update(map,world);
				if(guys[i]->type)
					guys[i]->Render(1);
				guys[i]->Update(map,world);
				player.speed--;
			}
			else
			{
				if(!player.timeStop || guys[i]->aiType==MONS_BOUAPHA || guys[i]->hp==0 ||
					guys[i]->aiType==MONS_MINECART || guys[i]->aiType==MONS_RAFT || guys[i]->aiType==MONS_YUGO)
				{
					if(((speedClock&3)==0) && guys[i]->aiType!=MONS_BOUAPHA && guys[i]->aiType!=MONS_RAFT &&
						guys[i]->aiType!=MONS_MINECART && guys[i]->aiType!=MONS_RAFT && guys[i]->aiType!=MONS_YUGO)
					{
						if(profile.difficulty==0)
						{
							// skip the update!
						}
						else if(profile.difficulty==2)
						{
							// double update!
							guys[i]->Update(map,world);
							guys[i]->Update(map,world);
						}
					}
					else
						guys[i]->Update(map,world);

					if(guys[i]->aiType==MONS_WOLF2 && guys[i]->type!=0 && guys[i]->mind3)
					{
						guys[i]->Render(1);
						guys[i]->Update(map,world);
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

void RenderGuys(byte light)
{
	int i;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type!=MONS_NONE && guys[i]->type!=MONS_NOBODY)
			guys[i]->Render(light);
}

void SetupTargets(void)
{
	int i,j;

	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type && guys[i]->friendly==1)
		{
			j=LockOnEvil(guys[i]->x>>FIXSHIFT,guys[i]->y>>FIXSHIFT);
			if(j!=65535)
				guys[i]->target=guys[j];
			else
				guys[i]->target=NULL;
		}
		else if(guys[i]->type)
		{
			j=LockOnGood(guys[i]->x>>FIXSHIFT,guys[i]->y>>FIXSHIFT);
			if(j!=65535)
				guys[i]->target=guys[j];
			else
				guys[i]->target=goodguy;
		}
	}
}

Guy *AddBaby(int x,int y,int z,byte type,Guy *me)
{
	Guy *g;

	if(me->mindControl)
	{
		g=AddGuy(x,y,z,type,1-me->friendly);
	}
	else
	{
		g=AddGuy(x,y,z,type,me->friendly);
	}
	if(g)
		g->item=me->item;

	return g;
}

Guy *AddGuy(int x,int y,int z,int type,byte friendly)
{
	int i,j,k;
	Guy *g,*g2;

	if(type==MONS_ANYBODY)
	{
		type=MONS_NOBODY;
		while(type==MONS_NONE || type==MONS_NOBODY || (GetMonsterType(type)->theme==MT_NONE))
			type=Random(NUM_MONSTERS);
	}
	else if(type==MONS_NONPLAYER)
	{
		type=MONS_NOBODY;
		while(type==MONS_NONE || type==MONS_NOBODY || type==MONS_BOUAPHA || (GetMonsterType(type)->theme==MT_NONE))
			type=Random(NUM_MONSTERS);
	}
	else if(type==MONS_BADGUY)
	{
		type=MONS_NOBODY;
		while(type==MONS_NONE || type==MONS_NOBODY || type==MONS_BOUAPHA || type==MONS_FRIENDLY || type==MONS_GOODTURRET ||
				type==MONS_WIZARD || type==MONS_GOODROBOT || type==MONS_GOODROBOT2 ||
				type==MONS_FRIENDLY2 || type==MONS_FOLLOWBUNNY || type==MONS_MINECART || type==MONS_RAFT ||
				type==MONS_YUGO || type==MONS_PUNKBUNNY || (GetMonsterType(type)->theme==MT_NONE))
			type=Random(NUM_MONSTERS);
	}
	else if(type==MONS_GOODGUY)
	{
		type=MONS_NOBODY;
		while(type==MONS_NONE || type==MONS_NOBODY || !(type==MONS_FRIENDLY || type==MONS_GOODTURRET ||
				type==MONS_WIZARD || type==MONS_GOODROBOT || type==MONS_GOODROBOT2 ||
				type==MONS_FRIENDLY2 || type==MONS_FOLLOWBUNNY || type==MONS_PUNKBUNNY || (GetMonsterType(type)->theme==MT_NONE)))
			type=Random(NUM_MONSTERS);
	}
	else if(type==MONS_TAGGED)
	{
		if(TaggedMonster()==NULL)
			return NULL;	// can't summon another nothing
		type=TaggedMonster()->type;
	}
	else if(type==MONS_PLAYER)
	{
		if(goodguy==NULL)
			return NULL;	// can't summon another nothing
		type=goodguy->type;
	}

	if(type>=NUM_MONSTERS)
		return NULL;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type==MONS_NONE)
		{
			if(type==MONS_BOUAPHA)	// special case: bouapha is the goodguy
			{
				goodguy=guys[i];
				if(editing!=1)
					PutCamera(x,y);
			}

			if(friendly==1 || (friendly==2 && (type==MONS_BOUAPHA || type==MONS_FRIENDLY || type==MONS_GOODTURRET ||
				type==MONS_WIZARD || type==MONS_GOODROBOT || type==MONS_GOODROBOT2 ||
				type==MONS_FRIENDLY2 || type==MONS_FOLLOWBUNNY || type==MONS_MINECART || type==MONS_RAFT ||
				type==MONS_YUGO || type==MONS_PUNKBUNNY)))
			{
				guys[i]->friendly=1;
				j=LockOnEvil(x>>FIXSHIFT,y>>FIXSHIFT);
				if(j!=65535)
					guys[i]->target=guys[j];
				else
					guys[i]->target=NULL;
				if(type==MONS_FRIENDLY2)
				{
					guys[i]->mind1=1;
					guys[i]->mind3=64;
				}
			}
			else
			{
				if((MonsterFlags(type,type)&(MF_NOHIT|MF_INVINCIBLE))==0)
					player.totalEnemies++;	// don't count invincible or unhittable guys

				guys[i]->friendly=0;
				j=LockOnGood(x>>FIXSHIFT,y>>FIXSHIFT);
				if(j!=65535)
					guys[i]->target=guys[j];
				else
					guys[i]->target=goodguy;
			}
			guys[i]->mindControl=0;
			guys[i]->poison=0;
			guys[i]->type=type;
			guys[i]->x=x;
			guys[i]->y=y;
			guys[i]->z=z;
			guys[i]->seq=ANIM_IDLE;
			guys[i]->frm=0;
			guys[i]->frmTimer=0;
			guys[i]->frmAdvance=128;
			guys[i]->hp=MonsterHP(type);
			guys[i]->maxHP=guys[i]->hp;
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
			guys[i]->frozen=0;
			guys[i]->mapx=(guys[i]->x>>FIXSHIFT)/TILE_WIDTH;
			guys[i]->mapy=(guys[i]->y>>FIXSHIFT)/TILE_HEIGHT;
			guys[i]->item=ITM_RANDOM;
			strcpy(guys[i]->name,MonsterName(type));
			guys[i]->fromColor=255;
			guys[i]->aiType=guys[i]->type;
			guys[i]->brtChange=GetMonsterType(guys[i]->type)->brtChg;

			if(type==MONS_ISOZOID && editing!=1)
			{
				guys[i]->seq=ANIM_A2;		// start hidden!
				guys[i]->frm=0;
				guys[i]->frmTimer=0;
				guys[i]->frmAdvance=1;
			}
			if(type==MONS_GENERATOR1)
				guys[i]->mind2=1;
			if(type==MONS_GENERATOR2)
				guys[i]->mind2=5;
			if(type==MONS_GENERATOR3)
				guys[i]->mind2=15;
			if(type==MONS_GENERATOR4)
				guys[i]->mind2=30;

			if(type==MONS_MATHEAD)	// Matilda, need to add all the parts
			{
				guys[i]->z=32*FIXAMT;
				g=AddGuy(x,y-32*FIXAMT,0,MONS_MATBODY,friendly);
				if(g)
					g->parent=guys[i];
				g=AddGuy(x-40*FIXAMT,y-1*FIXAMT,32*FIXAMT,MONS_MATCLAW1,friendly);
				if(g)
					g->parent=guys[i];
				g=AddGuy(x+40*FIXAMT,y-1*FIXAMT,32*FIXAMT,MONS_MATCLAW2,friendly);
				if(g)
					g->parent=guys[i];
				g=AddGuy(x,y-96*FIXAMT,32*FIXAMT,MONS_MATTAIL,friendly);
				if(g)
					g->parent=guys[i];
			}

			if(type==MONS_CRABPUFF || type==MONS_BEETLE)
			{
				guys[i]->seq=ANIM_MOVE;
				guys[i]->frm=Random(8);
			}
			if(type==MONS_MAT2HEAD)	// Matilda, need to add all the parts
			{
				guys[i]->z=32*FIXAMT;
				g=AddGuy(x,y-32*FIXAMT,0,MONS_MAT2BODY,friendly);
				if(g)
					g->parent=guys[i];

				for(k=0;k<2;k++)
				{
					// lower left tentacle
					g=AddGuy(x-40*FIXAMT,y-1*FIXAMT,32*FIXAMT,MONS_THINGTENT,friendly);
					if(!g)
						return guys[i];
					g->parent=guys[i];
					g->facing=6;
					// mind tells them which tentacle they are (to constrain rotation)
					g->mind=0+3*(k==1);
					g->mind1=128;
					for(j=0;j<3;j++)
					{
						g2=g;
						// adding them all overlapping, they'll move into place when updated
						g=AddGuy(x-40*FIXAMT,y-1*FIXAMT,32*FIXAMT,MONS_THINGTENT,friendly);
						if(!g)
							return guys[i];
						g->parent=g2;
						g->facing=6;
						g->mind=0;
					}
					g2=AddGuy(x-40*FIXAMT,y-1*FIXAMT,32*FIXAMT,MONS_THINGTENTTIP,friendly);
					if(!g2)
						return guys[i];
					g2->parent=g;
					g2->facing=6;
					g2->mind=0;

					// lower right tentacle
					g=AddGuy(x+40*FIXAMT,y-1*FIXAMT,32*FIXAMT,MONS_THINGTENT,friendly);
					if(!g)
						return guys[i];
					g->parent=guys[i];
					g->facing=2;
					g->mind=1+1*(k==1);
					g->mind1=128;
					for(j=0;j<3;j++)
					{
						g2=g;
						// adding them all overlapping, they'll move into place when updated
						g=AddGuy(x+40*FIXAMT,y-1*FIXAMT,32*FIXAMT,MONS_THINGTENT,friendly);
						if(!g)
							return guys[i];
						g->parent=g2;
						g->facing=2;
						g->mind=1;
					}
					g2=AddGuy(x+40*FIXAMT,y-1*FIXAMT,32*FIXAMT,MONS_THINGTENTTIP,friendly);
					if(!g2)
						return guys[i];
					g2->parent=g;
					g2->facing=2;
					g2->mind=1;
				}

				g=AddGuy(x,y-96*FIXAMT,32*FIXAMT,MONS_MAT2TAIL,friendly);
				if(g)
					g->parent=guys[i];
			}

			if(type==MONS_THING)	// The Thing needs to add tentacles
			{
				// lower left tentacle
				g=AddGuy(x-64*FIXAMT,y+48*FIXAMT,12*FIXAMT,MONS_THINGTENT,friendly);
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
					g=AddGuy(x-64*FIXAMT,y+48*FIXAMT,12*FIXAMT,MONS_THINGTENT,friendly);
					if(!g)
						return guys[i];
					g->parent=g2;
					g->facing=6;
					g->mind=0;
				}
				g2=AddGuy(x-64*FIXAMT,y+48*FIXAMT,12*FIXAMT,MONS_THINGTENTTIP,friendly);
				if(!g2)
					return guys[i];
				g2->parent=g;
				g2->facing=6;
				g2->mind=0;
				// lower right tentacle
				g=AddGuy(x+64*FIXAMT,y+48*FIXAMT,12*FIXAMT,MONS_THINGTENT,friendly);
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
					g=AddGuy(x+64*FIXAMT,y+48*FIXAMT,12*FIXAMT,MONS_THINGTENT,friendly);
					if(!g)
						return guys[i];
					g->parent=g2;
					g->facing=2;
					g->mind=1;
				}
				g2=AddGuy(x+64*FIXAMT,y+48*FIXAMT,12*FIXAMT,MONS_THINGTENTTIP,friendly);
				if(!g2)
					return guys[i];
				g2->parent=g;
				g2->facing=2;
				g2->mind=1;
				// upper right tentacle
				g=AddGuy(x+64*FIXAMT,y-20*FIXAMT,12*FIXAMT,MONS_THINGTENT,friendly);
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
					g=AddGuy(x+64*FIXAMT,y-20*FIXAMT,12*FIXAMT,MONS_THINGTENT,friendly);
					if(!g)
						return guys[i];
					g->parent=g2;
					g->facing=14;
					g->mind=2;
				}
				g2=AddGuy(x+64*FIXAMT,y-20*FIXAMT,12*FIXAMT,MONS_THINGTENTTIP,friendly);
				if(!g2)
					return guys[i];
				g2->parent=g;
				g2->facing=14;
				g2->mind=2;
				// upper left (and last!) tentacle
				g=AddGuy(x-64*FIXAMT,y-20*FIXAMT,12*FIXAMT,MONS_THINGTENT,friendly);
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
					g=AddGuy(x-64*FIXAMT,y-20*FIXAMT,12*FIXAMT,MONS_THINGTENT,friendly);
					if(!g)
						return guys[i];
					g->parent=g2;
					g->facing=10;
					g->mind=3;
				}
				g2=AddGuy(x-64*FIXAMT,y-20*FIXAMT,12*FIXAMT,MONS_THINGTENTTIP,friendly);
				if(!g2)
					return guys[i];
				g2->parent=g;
				g2->facing=10;
				g2->mind=3;
			}
			if(type==MONS_SPHINX)
			{
				// left arm
				g=AddGuy(x-108*FIXAMT,y+22*FIXAMT,0,MONS_SPHXARM1,friendly);
				if(!g)
				{
					return guys[i];
				}
				g->parent=guys[i];
				// right arm
				g=AddGuy(x+108*FIXAMT,y+22*FIXAMT,0,MONS_SPHXARM2,friendly);
				if(!g)
				{
					return guys[i];
				}
				g->parent=guys[i];
			}
			if(type==MONS_CENTIHEAD)
			{
				// add on 15 body parts
				g=guys[i];
				for(j=0;j<15;j++)
				{
					g2=AddGuy(x,y,0,MONS_CENTIBODY,friendly);
					if(!g2)
						return guys[i];
					g2->parent=g;
					g=g2;
				}
			}
			if(type==MONS_LOONYBOT)
			{
				g=AddGuy(x,y,0,MONS_LOONYCORE,friendly);
				if(g)
				{
					// the core is the parent of them all
					guys[i]->parent=g;
					g->item=guys[i]->item;
				}
				g=AddGuy(x-87*FIXAMT,y,48*FIXAMT,MONS_LOONYGUN,friendly);
				if(g)
				{
					g->parent=guys[i];
					g->mind3=0;
				}
				g=AddGuy(x+92*FIXAMT,y,48*FIXAMT,MONS_LOONYGUN,friendly);
				if(g)
				{
					g->parent=guys[i];
					g->mind3=1;
				}
			}

			return guys[i];
		}
	return NULL;
}

void DeleteGuy2(Guy *g)
{
	int i;

	RemoveGuy(g);
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
			RemoveGuy(guys[i]);
			for(j=0;j<maxGuys;j++)
				if(guys[j]->parent==guys[i])	// kill all the kids too
					DeleteGuy2(guys[j]);
		}
}


void AddMapGuys(Map *map)
{
	int i,j;
	Guy *g;
	byte genType[MAX_MAPMONS];

	// clear all guys
	for(i=0;i<maxGuys;i++)
	{
		guys[i]->type=MONS_NONE;
	}
	goodguy=NULL;

	// add a nobody in case the player goes invisible
	nobody=AddGuy(0,0,0,MONS_NOBODY,1);
	nobody->item=0;

	// discover what type for each generator, and remove the template monster
	// from the map
	if(editing!=1)
	{
		for(i=0;i<MAX_MAPMONS;i++)
			if(map->badguy[i].type==MONS_GENERATOR1 ||
			   map->badguy[i].type==MONS_GENERATOR2 ||
			   map->badguy[i].type==MONS_GENERATOR3 ||
			   map->badguy[i].type==MONS_GENERATOR4)
			{
				genType[i]=MONS_NONE;
				for(j=0;j<MAX_MAPMONS;j++)
					if(map->badguy[j].x==map->badguy[i].x && map->badguy[j].y==map->badguy[i].y+1 &&
						map->badguy[j].type)
					{
						genType[i]=map->badguy[j].type;
						map->badguy[j].type=MONS_NONE;
					}
			}
	}

	for(i=0;i<MAX_MAPMONS;i++)
		if(map->badguy[i].type)
		{
			if(map->badguy[i].type>=NUM_MONSTERS)
			{
				FatalError("Illegal monster on level!");
			}
			g=AddGuy((map->badguy[i].x*TILE_WIDTH+(TILE_WIDTH/2))<<FIXSHIFT,
				   (map->badguy[i].y*TILE_HEIGHT+(TILE_HEIGHT/2))<<FIXSHIFT,
				   0,map->badguy[i].type,2);
			if(g)
			{
				g->item=map->badguy[i].item;
				g->bright=map->GetTile(map->badguy[i].x,map->badguy[i].y)->light;
				if(g->type==MONS_BOUAPHA)
				{
					g->item=ITM_NONE;
					PutPlayerAtStart(g);
				}
				if(g->type==MONS_LOONYBOT)
				{
					if(g->parent)
						g->parent->item=g->item;
					g->item=0;
				}
			}
			if(editing!=1 && (map->badguy[i].type==MONS_GENERATOR1 ||
			   map->badguy[i].type==MONS_GENERATOR2 ||
			   map->badguy[i].type==MONS_GENERATOR3 ||
			   map->badguy[i].type==MONS_GENERATOR4))
			{
				if(g)
				{
					g->mind=genType[i];
					if(genType[i]==MONS_BOUAPHA || genType[i]==MONS_FRIENDLY || genType[i]==MONS_GOODTURRET ||
						genType[i]==MONS_WIZARD || genType[i]==MONS_GOODROBOT || genType[i]==MONS_GOODROBOT2 ||
						genType[i]==MONS_FRIENDLY2 || genType[i]==MONS_PUNKBUNNY || genType[i]==MONS_FOLLOWBUNNY ||
						genType[i]==MONS_MINECART || genType[i]==MONS_RAFT || genType[i]==MONS_YUGO)
					{
						player.totalEnemies--;
						g->friendly=1;
					}
					else
						g->friendly=0;
					g->facing=0;
				}
			}
		}

	if(goodguy==NULL)
	{
		// there was no guy set in the map
		g=AddGuy((TILE_WIDTH/2)*FIXAMT,(TILE_HEIGHT/2)*FIXAMT,0,MONS_BOUAPHA,2);
		if(g)
		{
			g->item=ITM_NONE;
			PutPlayerAtStart(g);
		}
	}
	SetupTargets();
}

byte CheckHit(byte size,int xx,int yy,Guy *him)
{
	int x2,y2;

	// can't hit the unhittable
	if(MonsterFlags(him->type,him->aiType)&MF_NOHIT)
		return 0;

	xx-=size;
	yy-=size;

	x2=him->x>>FIXSHIFT;
	y2=him->y>>FIXSHIFT;

	if((x2+him->rectx2)>=xx && (y2+him->recty2)>=yy && (x2+him->rectx)<=(xx+size*2) &&
		(y2+him->recty)<=(yy+size*2))
		return 1;

	return 0;
}

Guy *GetLastGuyHit(void)
{
	return guyHit;
}

byte FindVictim(int x,int y,byte size,int dx,int dy,byte damage,Map *map,world_t *world,byte friendly)
{
	int i;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type && guys[i]->hp && (guys[i]->friendly!=friendly))
		{
			if(CheckHit(size,x,y,guys[i]))
			{
				// stained glass knight blocking will reflect hits
				if(guys[i]->aiType==MONS_KNIGHT && guys[i]->seq==ANIM_A2 && guys[i]->frm>2)
				{
					MakeSound(SND_GLASSBLOCK,x<<FIXSHIFT,y<<FIXSHIFT,SND_CUTOFF,1200);
					ReflectShot();
				}
				else if(damage>0)
					guys[i]->GetShot(dx,dy,damage,map,world);
				guyHit=guys[i];
				return 1;
			}
		}

	return 0;
}

// this doesn't quit when it finds one victim, it keeps going
byte FindVictims(int x,int y,byte size,int dx,int dy,byte damage,Map *map,world_t *world,byte friendly)
{
	int i;
	byte result=0;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type && guys[i]->hp && (guys[i]->friendly!=friendly))
		{
			if(CheckHit(size,x,y,guys[i]))
			{
				guys[i]->GetShot(dx,dy,damage,map,world);
				guyHit=guys[i];
				result=1;
			}
		}

	return result;
}

// Same as above, but won't hit someone who is currently in ouch mode (to avoid rapid rehits)
byte FindVictims2(int x,int y,byte size,int dx,int dy,byte damage,Map *map,world_t *world,byte friendly)
{
	int i;
	byte result=0;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type && guys[i]->hp && (guys[i]->friendly!=friendly) && guys[i]->ouch==0)
		{
			if(CheckHit(size,x,y,guys[i]))
			{
				guys[i]->GetShot(dx,dy,damage,map,world);
				guyHit=guys[i];
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
		if(guys[i]->type && guys[i]->hp && (guys[i]->friendly==0) &&
			(!(MonsterFlags(guys[i]->type,guys[i]->aiType)&(MF_NOHIT|MF_INVINCIBLE))))
		{
			range=abs(x-(guys[i]->x>>FIXSHIFT))+abs(y-(guys[i]->y>>FIXSHIFT));
			if((range<bestRange) || (range<320+240 && Random(32)==0))
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
		if(guys[i]->type && guys[i]->hp && (guys[i]->friendly==0) &&
			(!(MonsterFlags(guys[i]->type,guys[i]->aiType)&(MF_NOHIT|MF_INVINCIBLE))))
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

word LockOnGood(int x,int y)
{
	int i;
	int bestRange,range;
	word bestguy;

	bestguy=65535;
	bestRange=320+240;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type && guys[i]->hp && (guys[i]->friendly==1) &&
			(!(MonsterFlags(guys[i]->type,guys[i]->aiType)&(MF_NOHIT|MF_INVINCIBLE))) && (guys[i]->aiType!=MONS_BOUAPHA ||
			(player.invisibility==0 && player.stealthy==0)))
		{
			range=abs(x-(guys[i]->x>>FIXSHIFT))+abs(y-(guys[i]->y>>FIXSHIFT));
			if((range<bestRange) || (range<320+240 && Random(32)==0))
			{
				bestguy=i;
				bestRange=range;
			}
		}

	return bestguy;
}

word LockOnGood2(int x,int y)
{
	int i,j;
	int range;

	for(j=0;j<128;j++)
	{
		i=Random(maxGuys);
		if(guys[i]->type && guys[i]->hp && (guys[i]->friendly==1) &&
			(!(MonsterFlags(guys[i]->type,guys[i]->aiType)&MF_NOHIT)))
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

Guy *GetGuyOfType(byte type)
{
	int i;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type==type && guys[i]->hp>0)
			return guys[i];

	return NULL;
}

Guy *GetGuyOfAIType(byte type)
{
	int i;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->aiType==type && guys[i]->hp>0 && guys[i]->type!=MONS_NONE)
			return guys[i];

	return NULL;
}

byte MonsterExists(byte type)
{
	int i;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type==type)
			return 1;

	return 0;
}

Guy *GetGuy(word w)
{
	if(w>=maxGuys)
		return NULL;
	if(!guys[w])
		return NULL;
	return guys[w];
}

void HealGoodguy(byte amt)
{
	if(!goodguy)
		return;

	if(goodguy->hp+amt<goodguy->maxHP)
	{
		if(!editing && !player.cheated && verified)
		{
			profile.progress.calories+=amt*4;
			if(profile.progress.calories>=50000)
				CompleteGoal(84);
		}
		goodguy->hp+=amt;
	}
	else
	{
		if(!editing && !player.cheated && verified)
		{
			profile.progress.calories+=(goodguy->maxHP-goodguy->hp)*4;
			if(profile.progress.calories>=50000)
				CompleteGoal(84);
		}
		goodguy->hp=goodguy->maxHP;
	}
}

// this checks to see if there is any moss on the chosen tile (x,y in tile coords)
byte MossCheck(int x,int y)
{
	int i;

	for(i=0;i<maxGuys;i++)
		if((guys[i]) && ((guys[i]->aiType==MONS_MOSS) || (guys[i]->aiType==MONS_MOSSGRANDE) ||
			(guys[i]->aiType==MONS_MOSS2))
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
				if(!guys[i]->friendly)
					player.enemiesSlain++;
				guys[i]->hp=0;
				guys[i]->seq=ANIM_DIE;
				guys[i]->action=ACTION_BUSY;
				guys[i]->frm=0;
				guys[i]->frmTimer=0;
				guys[i]->frmAdvance=64;
			}
		}
}

byte RaftNearby(void)
{
	int i,gx,gy,mx,my;

	gx=((goodguy->x-goodguy->dx)>>FIXSHIFT)/TILE_WIDTH;
	gy=((goodguy->y-goodguy->dy)>>FIXSHIFT)/TILE_HEIGHT;

	for(i=0;i<maxGuys;i++)
		if(guys[i] && guys[i]->type && guys[i]->aiType==MONS_RAFT)
		{
			if(RangeToTarget(guys[i],goodguy)<32*FIXAMT && player.vehicle==0)
			{
				guys[i]->mind=1;
				player.vehicle=VE_RAFT;
				goodguy->parent=guys[i];
				goodguy->z=FIXAMT*8;
				guys[i]->mind1=0;	// acceleration

				mx=((guys[i]->x)>>FIXSHIFT)/TILE_WIDTH;
				my=((guys[i]->y)>>FIXSHIFT)/TILE_HEIGHT;

				if(gx<mx)
					guys[i]->facing=0;
				else if(gy<my)
					guys[i]->facing=1;
				else if(gx>mx)
					guys[i]->facing=2;
				else
					guys[i]->facing=3;

				goodguy->x=guys[i]->x;
				goodguy->y=guys[i]->y+1;

				/*
				if(abs(goodguy->dx)>abs(goodguy->dy))
				{
					if(goodguy->dx<0)
						guys[i]->facing=2;
					else
						guys[i]->facing=0;
				}
				else
				{
					if(goodguy->dy<0)
						guys[i]->facing=3;
					else
						guys[i]->facing=1;
				}
				*/
				return 1;
			}
		}

	return 0;
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

void AddPygmy(Map *map,world_t *world,byte friendly)
{
	int x,y,i;
	int cx,cy;
	byte t;
	Guy *g;

	// decide which type to summon
	switch(Random(5))
	{
		case 0:
		case 1:
			t=MONS_PYGMY;
			break;
		case 2:
		case 3:
			t=MONS_PYGMY2;
			break;
		case 4:
		default:
			// shamans least likely
			t=MONS_PYGMY3;
			break;
	}

	GetCamera(&cx,&cy);
	// 30 tries to end up in a legal spot
	for(i=0;i<30;i++)
	{
		x=Random(map->width)*TILE_WIDTH+TILE_WIDTH/2;
		y=Random(map->height)*TILE_HEIGHT+TILE_HEIGHT/2;
		// make sure it's offscreen
		if(x<cx-340 || x>cx+340 || y<cy-260 || y>cy+260)
		{
			g=AddGuy(x*FIXAMT,y*FIXAMT,0,t,friendly);
			if(g && (!g->CanWalk(g->x,g->y,map,world)))
			{
				RemoveGuy(g);
			}
			else if(g)
			{
				// make him angry at bouapha
				g->mind=1;
				g->mind1=240;
				return;
			}
		}
	}
}

void AddNinja(Map *map,world_t *world,byte friendly)
{
	int x,y,i;
	int cx,cy;
	Guy *g;

	GetCamera(&cx,&cy);
	// 30 tries to end up in a legal spot
	for(i=0;i<30;i++)
	{
		x=Random(map->width)*TILE_WIDTH+TILE_WIDTH/2;
		y=Random(map->height)*TILE_HEIGHT+TILE_HEIGHT/2;
		// make sure it's offscreen
		if(x<cx-340 || x>cx+340 || y<cy-260 || y>cy+260)
		{
			g=AddGuy(x*FIXAMT,y*FIXAMT,0,MONS_GINGER,friendly);
			if(g && (!g->CanWalk(g->x,g->y,map,world)))
			{
				RemoveGuy(g);
			}
			else
				return;
		}
	}
}

byte ControlMind2(Guy *me)
{
	int i;

	me->friendly=1;	// put him on our team!
	me->mindControl=30*15;	// for 15 seconds
	player.totalEnemies--;
	me->target=NULL;

	if(me->aiType==MONS_SUPERZOMBIE)
		return 1;	// super zombies do not have their children converted, on the extremely
					// off chance that you mind control one while it is holding you
	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type!=MONS_NONE && guys[i]->type!=MONS_NOBODY && guys[i]->parent==me)
		{
			ControlMind2(guys[i]);
		}
	}
	return 1;
}

byte ControlMind(Guy *me)
{
	if(me->aiType==MONS_ROLLER || me->aiType==MONS_ROLLER2 || me->aiType==MONS_BURNER ||
	   me->aiType==MONS_LAMP || me->aiType==MONS_EGGSAC || me->aiType==MONS_BOILER ||
	   me->aiType==MONS_COFFIN || me->aiType==MONS_ALIENEGG || me->aiType==MONS_ROBOFACTORY ||
	   me->aiType==MONS_ALIENEGG2 || me->aiType==MONS_GENERATOR1|| me->aiType==MONS_GENERATOR2 ||
	   me->aiType==MONS_GENERATOR3 || me->aiType==MONS_GENERATOR4 || me->aiType==MONS_SNOWBLOW ||
	   me->aiType==MONS_SNOWBALL || me->aiType==MONS_SNOWBALL2 || (me->aiType>=MONS_SUCKER1 && me->aiType<=MONS_BLOWER4) ||
	   (me->aiType>=MONS_PARKEDCAR && me->aiType<=MONS_TRAFFIC2) || me->aiType==MONS_DARKCOFFIN || me->aiType==MONS_XENOEGG ||
	   (me->aiType>=MONS_PATCH && me->aiType<=MONS_PATCH4) || me->aiType==MONS_YUGO || me->aiType==MONS_CHEST)
		return 0;	// no mind to control

	// move up the chain to the top
	while(me->parent && me->parent->friendly==0 && me->parent->hp>0 && me->parent->type)
	{
		me=me->parent;
	}

	return ControlMind2(me);
}

byte FreezeGuy2(Guy *me)
{
	int i;

	me->frozen=30*5;

	if(me->aiType==MONS_SUPERZOMBIE)
		return 1;	// super zombies do not have their children frozen, on the extremely
					// off chance that you freeze one while it is holding you
	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type!=MONS_NONE && guys[i]->type!=MONS_NOBODY && guys[i]->parent==me)
		{
			FreezeGuy2(guys[i]);
		}
	}
	return 1;
}

byte FreezeGuy(Guy *me)
{
	if(me->aiType==MONS_BOUAPHA && (me->frozen || player.shield))
		return 0;	// can't freeze bouapha when he's already frozen or shielded

	// move up the chain to the top
	while(me->parent)
	{
		me=me->parent;
	}

	return FreezeGuy2(me);
}

void KillMonster(int x,int y,int type,byte nofx)
{
	int i;

	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type!=MONS_NONE && guys[i]->type!=MONS_NOBODY && (x==255 || (guys[i]->mapx==x && guys[i]->mapy==y)))
		{
			switch(type)
			{
				case MONS_ANYBODY:
					break;
				case MONS_GOODGUY:
					if(!guys[i]->friendly)
						continue;
					break;
				case MONS_BADGUY:
					if(guys[i]->friendly)
						continue;
					break;
				case MONS_NONPLAYER:
					if(guys[i]->aiType==MONS_BOUAPHA)
						continue;
					break;
				case MONS_PLAYER:
					if(guys[i]->aiType!=MONS_BOUAPHA)
						continue;
					break;
				case MONS_TAGGED:
					if(guys[i]!=TaggedMonster())
						continue;
					break;
				default:
					if(guys[i]->type!=type)
						continue;
					break;
			}
			if(guys[i]->aiType==MONS_BOUAPHA)
			{
				SendMessageToGame(MSG_RESET,0);
			}

			if(guys[i]->friendly==0 && !(MonsterFlags(guys[i]->type,guys[i]->aiType)&(MF_NOHIT|MF_INVINCIBLE)))
				player.enemiesSlain++;

			if(guys[i]->aiType==MONS_RAFT && guys[i]->mind!=0)
			{
				player.vehicle=VE_NONE;	// player was riding it
				goodguy->parent=NULL;
			}
			if(guys[i]->aiType==MONS_MINECART && guys[i]->mind!=0)
			{
				player.vehicle=VE_NONE;	// player was riding it
				goodguy->parent=NULL;
			}
			if(guys[i]->aiType==MONS_YUGO && guys[i]->mind==2)
			{
				player.vehicle=VE_NONE;	// player was riding it
				goodguy->parent=NULL;
			}

			guys[i]->type=MONS_NONE;

			// handle item drops
			if(guys[i]->type==MONS_ZOMBIE || guys[i]->type==MONS_MUTANT)	// zombies always drop a brain
			{
				if(!curMap->DropItem(guys[i]->mapx,guys[i]->mapy,ITM_BRAIN))
					curMap->GetTile(guys[i]->mapx,guys[i]->mapy)->item=ITM_BRAIN;	// force the drop if it failed
			}
			else if(guys[i]->type==MONS_SUPERZOMBIE)	// super zombies always drop 2 brains
			{
				if(!curMap->DropItem(guys[i]->mapx,guys[i]->mapy,ITM_BRAIN))
					curMap->GetTile(guys[i]->mapx,guys[i]->mapy)->item=ITM_BRAIN;	// force the drop if it failed
				if(!curMap->DropItem(guys[i]->mapx,guys[i]->mapy+1,ITM_BRAIN) && guys[i]->mapy+1<curMap->height)
					curMap->GetTile(guys[i]->mapx,guys[i]->mapy+1)->item=ITM_BRAIN;	// hope there's a legal coordinate and non-wall below me!
			}
			if(guys[i]->aiType==MONS_GNOME)
			{
				if(guys[i]->mind3!=0)	// drop what you stole!
					if(!curMap->DropItem(guys[i]->mapx,guys[i]->mapy,guys[i]->mind3))
						curMap->GetTile(guys[i]->mapx,guys[i]->mapy)->item=guys[i]->mind3;	// force the drop if it failed
			}

			if(guys[i]->item==ITM_RANDOM)
			{
				if(Random(100*FIXAMT)<curMap->itemDrops)
					curMap->DropItem(guys[i]->mapx,guys[i]->mapy,GetRandomItem());
			}
			else if(guys[i]->item!=ITM_NONE)
			{
				if(!curMap->DropItem(guys[i]->mapx,guys[i]->mapy,guys[i]->item))
					curMap->GetTile(guys[i]->mapx,guys[i]->mapy)->item=guys[i]->item;	// force the drop if it failed
			}
			if(!nofx)
				BlowUpGuy((guys[i]->x>>FIXSHIFT)-32,(guys[i]->y>>FIXSHIFT)-24,
					(guys[i]->x>>FIXSHIFT)+32,(guys[i]->y>>FIXSHIFT)+24,0,1);
		}

	}
}

void RemoveGuy(Guy *g)
{
	g->type=MONS_NONE;
	if(g->friendly==0)
		player.totalEnemies--;
}

void MonsterLife(byte fx,int x,int y,int type,int newLife)
{
	int i;

	if(newLife<1)
		newLife=1;

	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type!=MONS_NONE && guys[i]->type!=MONS_NOBODY && guys[i]->hp!=0 && (x==255 || (guys[i]->mapx==x && guys[i]->mapy==y)))
		{
			switch(type)
			{
				case MONS_ANYBODY:
					break;
				case MONS_GOODGUY:
					if(!guys[i]->friendly)
						continue;
					break;
				case MONS_BADGUY:
					if(guys[i]->friendly)
						continue;
					break;
				case MONS_NONPLAYER:
					if(guys[i]->aiType==MONS_BOUAPHA)
						continue;
					break;
				case MONS_PLAYER:
					if(guys[i]->aiType!=MONS_BOUAPHA)
						continue;
					break;
				case MONS_TAGGED:
					if(guys[i]!=TaggedMonster())
						continue;
					break;
				default:
					if(guys[i]->type!=type)
						continue;
					break;
			}

			if(guys[i]->aiType==MONS_BOUAPHA)
			{
				if(fx)
				{
					if(newLife>guys[i]->hp)
						HealRing(1,guys[i]->x,guys[i]->y,0,16,2);
					else
					{
						guys[i]->ouch=4;
						HealRing(4,guys[i]->x,guys[i]->y,0,16,2);
					}
				}
				guys[i]->hp=newLife;
				SetPlayerHP(newLife);
			}
			else
			{
				if(fx)
				{
					if(newLife>guys[i]->hp)
						HealRing(1,guys[i]->x,guys[i]->y,0,16,2);
					else
					{
						guys[i]->ouch=4;
						HealRing(4,guys[i]->x,guys[i]->y,0,16,2);
					}
				}
				guys[i]->hp=newLife;
			}
		}
	}
}

void MonsterMaxLife(byte fx,int x,int y,int type,int newLife)
{
	int i;

	if(newLife<1)
		newLife=1;

	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type!=MONS_NONE && guys[i]->type!=MONS_NOBODY && guys[i]->hp!=0 && (x==255 || (guys[i]->mapx==x && guys[i]->mapy==y)))
		{
			switch(type)
			{
				case MONS_ANYBODY:
					break;
				case MONS_GOODGUY:
					if(!guys[i]->friendly)
						continue;
					break;
				case MONS_BADGUY:
					if(guys[i]->friendly)
						continue;
					break;
				case MONS_NONPLAYER:
					if(guys[i]->aiType==MONS_BOUAPHA)
						continue;
					break;
				case MONS_PLAYER:
					if(guys[i]->aiType!=MONS_BOUAPHA)
						continue;
					break;
				case MONS_TAGGED:
					if(guys[i]!=TaggedMonster())
						continue;
					break;
				default:
					if(guys[i]->type!=type)
						continue;
					break;
			}

			if(fx)
			{
				if(newLife>guys[i]->maxHP)
					HealRing(1,guys[i]->x,guys[i]->y,0,16,2);
				else
				{
					HealRing(4,guys[i]->x,guys[i]->y,0,16,2);
				}
			}
			guys[i]->maxHP=newLife;
		}
	}
}

void SetMonsterName(byte fx,int x,int y,int type,char *name)
{
	int i;

	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type!=MONS_NONE && guys[i]->type!=MONS_NOBODY && guys[i]->hp!=0 && (x==255 || (guys[i]->mapx==x && guys[i]->mapy==y)))
		{
			switch(type)
			{
				case MONS_ANYBODY:
					break;
				case MONS_GOODGUY:
					if(!guys[i]->friendly)
						continue;
					break;
				case MONS_BADGUY:
					if(guys[i]->friendly)
						continue;
					break;
				case MONS_NONPLAYER:
					if(guys[i]->aiType==MONS_BOUAPHA)
						continue;
					break;
				case MONS_PLAYER:
					if(guys[i]->aiType!=MONS_BOUAPHA)
						continue;
					break;
				case MONS_TAGGED:
					if(guys[i]!=TaggedMonster())
						continue;
					break;
				default:
					if(guys[i]->type!=type)
						continue;
					break;
			}

			strcpy(guys[i]->name,name);
		}
	}
}

void SetMonsterColor(byte fx,int x,int y,int type,int colCode)
{
	int i;
	byte fromCol,toCol;

	fromCol=colCode%256;
	toCol=colCode/256;
	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type!=MONS_NONE && guys[i]->type!=MONS_NOBODY && guys[i]->hp!=0 && (x==255 || (guys[i]->mapx==x && guys[i]->mapy==y)))
		{
			switch(type)
			{
				case MONS_ANYBODY:
					break;
				case MONS_GOODGUY:
					if(!guys[i]->friendly)
						continue;
					break;
				case MONS_BADGUY:
					if(guys[i]->friendly)
						continue;
					break;
				case MONS_NONPLAYER:
					if(guys[i]->aiType==MONS_BOUAPHA)
						continue;
					break;
				case MONS_PLAYER:
					if(guys[i]->aiType!=MONS_BOUAPHA)
						continue;
					break;
				case MONS_TAGGED:
					if(guys[i]!=TaggedMonster())
						continue;
					break;
				default:
					if(guys[i]->type!=type)
						continue;
					break;
			}

			guys[i]->fromColor=fromCol;
			guys[i]->toColor=toCol;
		}
	}
}

void SetMonsterBright(byte fx,int x,int y,int type,int bright)
{
	int i;

	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type!=MONS_NONE && guys[i]->type!=MONS_NOBODY && guys[i]->hp!=0 && (x==255 || (guys[i]->mapx==x && guys[i]->mapy==y)))
		{
			switch(type)
			{
				case MONS_ANYBODY:
					break;
				case MONS_GOODGUY:
					if(!guys[i]->friendly)
						continue;
					break;
				case MONS_BADGUY:
					if(guys[i]->friendly)
						continue;
					break;
				case MONS_NONPLAYER:
					if(guys[i]->aiType==MONS_BOUAPHA)
						continue;
					break;
				case MONS_PLAYER:
					if(guys[i]->aiType!=MONS_BOUAPHA)
						continue;
					break;
				case MONS_TAGGED:
					if(guys[i]!=TaggedMonster())
						continue;
					break;
				default:
					if(guys[i]->type!=type)
						continue;
					break;
			}

			guys[i]->brtChange=bright;
		}
	}
}

void MonsterLifeAmt(byte fx,int x,int y,int type,int amt)
{
	int i,newLife;

	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type!=MONS_NONE && guys[i]->type!=MONS_NOBODY && guys[i]->hp!=0 && (x==255 || (guys[i]->mapx==x && guys[i]->mapy==y)))
		{
			switch(type)
			{
				case MONS_ANYBODY:
					break;
				case MONS_GOODGUY:
					if(!guys[i]->friendly)
						continue;
					break;
				case MONS_BADGUY:
					if(guys[i]->friendly)
						continue;
					break;
				case MONS_NONPLAYER:
					if(guys[i]->aiType==MONS_BOUAPHA)
						continue;
					break;
				case MONS_PLAYER:
					if(guys[i]->aiType!=MONS_BOUAPHA)
						continue;
					break;
				case MONS_TAGGED:
					if(guys[i]!=TaggedMonster())
						continue;
					break;
				default:
					if(guys[i]->type!=type)
						continue;
					break;
			}

			newLife=guys[i]->hp+amt;
			if(newLife<0)
				newLife=0;
			if(newLife>guys[i]->maxHP)
				newLife=guys[i]->maxHP;

			if(guys[i]->aiType==MONS_BOUAPHA)
			{
				if(fx)
				{
					if(amt>0)
						HealRing(1,guys[i]->x,guys[i]->y,0,16,2);
					else
					{
						guys[i]->ouch=4;
						HealRing(4,guys[i]->x,guys[i]->y,0,16,2);
					}
				}
				guys[i]->hp=newLife;
				SetPlayerHP(newLife);
			}
			else
			{
				if(fx)
				{
					if(amt>0)
						HealRing(1,guys[i]->x,guys[i]->y,0,16,2);
					else
					{
						guys[i]->ouch=4;
						HealRing(4,guys[i]->x,guys[i]->y,0,16,2);
					}
				}
				guys[i]->hp=newLife;
			}
			if(guys[i]->hp==0)
			{
				guys[i]->hp=1;
				guys[i]->GetShot(0,0,255,curMap,&curWorld);
			}
		}
	}
}

byte CheckMonsterLife(int x,int y,int type,int life,byte flags)
{
	int i;

	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type!=MONS_NONE && guys[i]->type!=MONS_NOBODY && (x==255 || (guys[i]->mapx==x && guys[i]->mapy==y)))
		{
			switch(type)
			{
				case MONS_ANYBODY:
					break;
				case MONS_GOODGUY:
					if(!guys[i]->friendly)
						continue;
					break;
				case MONS_BADGUY:
					if(guys[i]->friendly)
						continue;
					break;
				case MONS_NONPLAYER:
					if(guys[i]->aiType==MONS_BOUAPHA)
						continue;
					break;
				case MONS_PLAYER:
					if(guys[i]->aiType!=MONS_BOUAPHA)
						continue;
					break;
				case MONS_TAGGED:
					if(guys[i]!=TaggedMonster())
						continue;
					break;
				default:
					if(guys[i]->type!=type)
						continue;
					break;
			}

			if(guys[i]->hp==life)
				return 1;
			else if((flags&TF_MORE) && (guys[i]->hp>life))
				return 1;
			else if((flags&TF_LESS) && (guys[i]->hp<life))
				return 1;
		}
	}
	return 0;
}

byte CheckMonsterAwake(int x,int y,int type,byte flags)
{
	int i;
	int count,awake;

	count=0;
	awake=0;
	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type!=MONS_NONE && guys[i]->type!=MONS_NOBODY && (x==255 || (guys[i]->mapx==x && guys[i]->mapy==y)))
		{
			switch(type)
			{
				case MONS_ANYBODY:
					break;
				case MONS_GOODGUY:
					if(!guys[i]->friendly)
						continue;
					break;
				case MONS_BADGUY:
					if(guys[i]->friendly)
						continue;
					break;
				case MONS_NONPLAYER:
					if(guys[i]->aiType==MONS_BOUAPHA)
						continue;
					break;
				case MONS_PLAYER:
					if(guys[i]->aiType!=MONS_BOUAPHA)
						continue;
					break;
				case MONS_TAGGED:
					if(guys[i]!=TaggedMonster())
						continue;
					break;
				default:
					if(guys[i]->type!=type)
						continue;
					break;
			}

			count++;
			awake+=guys[i]->IsAwake();
		}
	}
	if(flags&TF_LESS)
	{
		return (count==awake);
	}
	else if(flags&TF_MORE)
	{
		return (count>0 && awake==0);
	}
	else
		return (count>0 && awake>0);
}

byte CheckMonsterColor(int x,int y,int type,byte color)
{
	int i;
	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type!=MONS_NONE && guys[i]->type!=MONS_NOBODY && (x==255 || (guys[i]->mapx==x && guys[i]->mapy==y)))
		{
			switch(type)
			{
				case MONS_ANYBODY:
					break;
				case MONS_GOODGUY:
					if(!guys[i]->friendly)
						continue;
					break;
				case MONS_BADGUY:
					if(guys[i]->friendly)
						continue;
					break;
				case MONS_NONPLAYER:
					if(guys[i]->aiType==MONS_BOUAPHA)
						continue;
					break;
				case MONS_PLAYER:
					if(guys[i]->aiType!=MONS_BOUAPHA)
						continue;
					break;
				case MONS_TAGGED:
					if(guys[i]!=TaggedMonster())
						continue;
					break;
				default:
					if(guys[i]->type!=type)
						continue;
					break;
			}

			if(guys[i]->toColor==color)
				return 1;
		}
	}
	return 0;
}

void ChangeMonsItem(byte fx,int x,int y,int type,int newItem)
{
	int i;

	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type!=MONS_NONE && guys[i]->type!=MONS_NOBODY && (x==255 || (guys[i]->mapx==x && guys[i]->mapy==y)))
		{
			switch(type)
			{
				case MONS_ANYBODY:
					break;
				case MONS_GOODGUY:
					if(!guys[i]->friendly)
						continue;
					break;
				case MONS_BADGUY:
					if(guys[i]->friendly)
						continue;
					break;
				case MONS_NONPLAYER:
					if(guys[i]->aiType==MONS_BOUAPHA)
						continue;
					break;
				case MONS_PLAYER:
					if(guys[i]->aiType!=MONS_BOUAPHA)
						continue;
					break;
				case MONS_TAGGED:
					if(guys[i]!=TaggedMonster())
						continue;
					break;
				default:
					if(guys[i]->type!=type)
						continue;
					break;
			}

			if(guys[i]->aiType==MONS_BOUAPHA)
			{
				int oldItem=curMap->GetTile(guys[i]->mapx,guys[i]->mapy)->item;
				curMap->GetTile(guys[i]->mapx,guys[i]->mapy)->item=newItem;
				TriggerItem(guys[i],curMap->GetTile(guys[i]->mapx,guys[i]->mapy),guys[i]->mapx,guys[i]->mapy);
				curMap->GetTile(guys[i]->mapx,guys[i]->mapy)->item=oldItem;
			}
			else
				guys[i]->item=newItem;
			if(fx)
				SmokeTile(guys[i]->mapx,guys[i]->mapy);
		}
	}
}

void ChangeMonster(byte fx,int x,int y,int type,int newtype)
{
	int i;
	byte f;
	Guy *g;
	byte drop;

	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type!=MONS_NONE && guys[i]->type!=MONS_NOBODY && (x==255 || (guys[i]->mapx==x && guys[i]->mapy==y)))
		{
			switch(type)
			{
				case MONS_ANYBODY:
					break;
				case MONS_GOODGUY:
					if(!guys[i]->friendly)
						continue;
					break;
				case MONS_BADGUY:
					if(guys[i]->friendly)
						continue;
					break;
				case MONS_NONPLAYER:
					if(guys[i]->aiType==MONS_BOUAPHA)
						continue;
					break;
				case MONS_PLAYER:
					if(guys[i]->aiType!=MONS_BOUAPHA)
						continue;
					break;
				case MONS_TAGGED:
					if(guys[i]!=TaggedMonster())
						continue;
					break;
				default:
					if(guys[i]->type!=type)
						continue;
					break;
			}

			f=guys[i]->friendly;
			if(guys[i]->hp!=0 && f==0 && !(MonsterFlags(guys[i]->type,guys[i]->aiType)&(MF_NOHIT|MF_INVINCIBLE)))
										// if this was a living evil guy
				player.totalEnemies--;	// then, subtract one enemy, since AddGuy will be adding one

			guys[i]->type=MONS_NONE;
			drop=guys[i]->item;
			g=AddGuy(guys[i]->x,guys[i]->y,guys[i]->z,newtype,f);
			if(g)
			{
				g->friendly=f;
				g->item=drop;
				if(fx)
				{
					if(g->friendly==0)
					{
						TeamChangeRing(4,g->x,g->y,0,16,4);
						MakeSound(SND_TURNEVIL,g->x,g->y,SND_CUTOFF,10);
					}
					else
					{
						TeamChangeRing(3,g->x,g->y,0,16,4);
						MakeSound(SND_TURNGOOD,g->x,g->y,SND_CUTOFF,10);
					}
				}
			}
		}
	}
}

void ChangeMonsterAI(byte fx,int x,int y,int type,int newtype)
{
	int i;
	Guy *g;

	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type!=MONS_NONE && guys[i]->type!=MONS_NOBODY && (x==255 || (guys[i]->mapx==x && guys[i]->mapy==y)))
		{
			switch(type)
			{
				case MONS_ANYBODY:
					break;
				case MONS_GOODGUY:
					if(!guys[i]->friendly)
						continue;
					break;
				case MONS_BADGUY:
					if(guys[i]->friendly)
						continue;
					break;
				case MONS_NONPLAYER:
					if(guys[i]->aiType==MONS_BOUAPHA)
						continue;
					break;
				case MONS_PLAYER:
					if(guys[i]->aiType!=MONS_BOUAPHA)
						continue;
					break;
				case MONS_TAGGED:
					if(guys[i]!=TaggedMonster())
						continue;
					break;
				default:
					if(guys[i]->type!=type)
						continue;
					break;
			}

			g=guys[i];
			guys[i]->aiType=newtype;
			guys[i]->mind=0;
			guys[i]->mind1=0;
			guys[i]->mind2=0;
			guys[i]->mind3=0;
			guys[i]->reload=0;
			guys[i]->dx=0;
			guys[i]->dy=0;
			guys[i]->dz=0;
			guys[i]->seq=ANIM_IDLE;
			guys[i]->frm=0;
			guys[i]->frmTimer=0;
			guys[i]->frmAdvance=128;
			guys[i]->facing=2;
			if(newtype==MONS_FRIENDLY2)
			{
				guys[i]->mind1=1;
				guys[i]->mind3=64;
			}
			if(fx)
			{
				if(g->friendly==0)
				{
					TeamChangeRing(4,g->x,g->y,0,16,4);
					MakeSound(SND_TURNEVIL,g->x,g->y,SND_CUTOFF,10);
				}
				else
				{
					TeamChangeRing(3,g->x,g->y,0,16,4);
					MakeSound(SND_TURNGOOD,g->x,g->y,SND_CUTOFF,10);
				}
			}
		}
	}
}

void ReallyChangeTeam(Guy *g,byte team)
{
	int i;
	byte f;

	if(!changed[g->ID])
	{
		f=g->friendly;
		if(team==2)
			g->friendly=1-g->friendly;
		else
			g->friendly=1-team;

		if(g->friendly && !f)
			player.enemiesSlain++;	// if you made an enemy friendly, you slew a monster
		else if(!g->friendly && f)
			player.totalEnemies++;	// if you made a friend an enemy, you've added a monster

		changed[g->ID]=1;
	}

	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->parent==g)
			ReallyChangeTeam(guys[i],team);	// change your kids
	}
}

void ChangeTeam(byte fx,int x,int y,int type,byte team)
{
	int i,tm;
	Guy *g;

	for(i=0;i<maxGuys;i++)
		changed[i]=0;

	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type!=MONS_NONE && guys[i]->type!=MONS_NOBODY && (x==255 || (guys[i]->mapx==x && guys[i]->mapy==y)) && !changed[i])
		{
			g=guys[i];
			switch(type)
			{
				case MONS_ANYBODY:
					break;
				case MONS_GOODGUY:
					if(!g->friendly)
						continue;
					break;
				case MONS_BADGUY:
					if(g->friendly)
						continue;
					break;
				case MONS_NONPLAYER:
					if(g->aiType==MONS_BOUAPHA)
						continue;
					break;
				case MONS_PLAYER:
					if(g->aiType!=MONS_BOUAPHA)
						continue;
					break;
				case MONS_TAGGED:
					if(guys[i]!=TaggedMonster())
						continue;
					break;
				default:
					if(g->type!=type)
						continue;
					break;
			}

			while(g->parent)
				g=g->parent;	// move up the chain

			if(fx)
			{
				if(team==2)
					tm=1-g->friendly;
				else
					tm=1-team;
				if(tm==0)
				{
					TeamChangeRing(4,g->x,g->y,0,16,4);
					MakeSound(SND_TURNEVIL,g->x,g->y,SND_CUTOFF,10);
				}
				else
				{
					TeamChangeRing(3,g->x,g->y,0,16,4);
					MakeSound(SND_TURNGOOD,g->x,g->y,SND_CUTOFF,10);
				}
			}
			ReallyChangeTeam(g,team);
		}
	}
}

Guy *FindMonster(int x,int y,int type)
{
	int i;
	Guy *g;

	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type!=MONS_NONE && guys[i]->type!=MONS_NOBODY && (x==255 || (guys[i]->mapx==x && guys[i]->mapy==y)))
		{
			g=guys[i];
			switch(type)
			{
				case MONS_ANYBODY:
					break;
				case MONS_GOODGUY:
					if(!g->friendly)
						continue;
					break;
				case MONS_BADGUY:
					if(g->friendly)
						continue;
					break;
				case MONS_NONPLAYER:
					if(g->aiType==MONS_BOUAPHA)
						continue;
					break;
				case MONS_PLAYER:
					if(g->aiType!=MONS_BOUAPHA)
						continue;
					break;
				case MONS_TAGGED:
					if(g!=TaggedMonster())
						continue;
					break;
				default:
					if(g->type!=type)
						continue;
					break;
			}

			while(g->parent)
				g=g->parent;	// move up the chain

			return g;
		}
	}
	return NULL;
}

void MoveKids(int dx,int dy,Guy *parent)
{
	int i;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->parent==parent)
		{
			guys[i]->x+=dx;
			guys[i]->y+=dy;
			MoveKids(dx,dy,guys[i]);
		}
}

byte SwapMe(int x,int y,byte size,Map *map)
{
	int i,dx,dy;
	Guy *g;

	if(!goodguy)
		return 0;

	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type && guys[i]->hp && guys[i]->aiType!=MONS_BOUAPHA)
		{
			g=guys[i];
			if(CheckHit(size,x>>FIXSHIFT,y>>FIXSHIFT,guys[i]))
			{
				while(g->parent)
				{
					g=g->parent;
				}
				dx=(goodguy->mapx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT-g->x;
				dy=(goodguy->mapy*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT-g->y;
				goodguy->x=(g->mapx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
				goodguy->y=(g->mapy*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
				MoveKids(dx,dy,g);
				g->x+=dx;
				g->y+=dy;
				Telefrag(goodguy);
				Telefrag(g);
				LightningBolt(goodguy->x,goodguy->y,g->x,g->y);
				LightningBolt(goodguy->x,goodguy->y,g->x,g->y);
				LightningBolt(goodguy->x,goodguy->y,g->x,g->y);
				map->BrightTorch(goodguy->x,goodguy->y,20,10);
				map->BrightTorch(g->x,g->y,20,10);
				MakeNormalSound(SND_LIGHTSON);
				return 1;
			}
		}
	}
	return 0;
}

int CountMonsters(int type)
{
	int i,cnt;

	cnt=0;
	if(type>0)	// actual monster types
	{
		for(i=0;i<maxGuys;i++)
			if(guys[i]->type==type)
				cnt++;
	}
	else if(type==MONS_ANYBODY)	// any monsters at all
	{
		for(i=0;i<maxGuys;i++)
			if(guys[i]->type>0 && guys[i]->type!=MONS_NOBODY)
				cnt++;
	}
	else if(type==MONS_GOODGUY)	// goodguys
	{
		for(i=0;i<maxGuys;i++)
			if(guys[i]->type>0 && guys[i]->type!=MONS_NOBODY && guys[i]->friendly)
				cnt++;
	}
	else if(type==MONS_BADGUY)	// badguys
	{
		for(i=0;i<maxGuys;i++)
			if(guys[i]->type>0 && guys[i]->type!=MONS_NOBODY && !guys[i]->friendly)
				cnt++;
	}
	else if(type==MONS_NONPLAYER)	// anyone but bouapha
	{
		for(i=0;i<maxGuys;i++)
			if(guys[i]->type>0 && guys[i]->type!=MONS_NOBODY && guys[i]->aiType!=MONS_BOUAPHA)
				cnt++;
	}
	else if(type==MONS_PLAYER)	// bouapha only
	{
		for(i=0;i<maxGuys;i++)
			if(guys[i]->type>0 && guys[i]->type!=MONS_NOBODY && guys[i]->aiType==MONS_BOUAPHA)
				cnt++;
	}
	else if(type==MONS_TAGGED)	// only the tagged monster himself
	{
		for(i=0;i<maxGuys;i++)
			if(guys[i]->type>0 && guys[i]==TaggedMonster())
				cnt++;
	}
	return cnt;
}

int CountMonstersInRect(int type,int x,int y,int x2,int y2)
{
	int i,cnt;

	cnt=0;
	if(type>0)	// actual monster types
	{
		for(i=0;i<maxGuys;i++)
			if(guys[i]->type==type && guys[i]->mapx>=x && guys[i]->mapy>=y && guys[i]->mapx<=x2 && guys[i]->mapy<=y2)
				cnt++;
	}
	else if(type==MONS_ANYBODY)	// any monsters at all
	{
		for(i=0;i<maxGuys;i++)
			if(guys[i]->type>0 && guys[i]->type!=MONS_NOBODY && guys[i]->mapx>=x && guys[i]->mapy>=y && guys[i]->mapx<=x2 && guys[i]->mapy<=y2)
				cnt++;
	}
	else if(type==MONS_GOODGUY)	// goodguys
	{
		for(i=0;i<maxGuys;i++)
			if(guys[i]->type>0 && guys[i]->type!=MONS_NOBODY && guys[i]->friendly && guys[i]->mapx>=x && guys[i]->mapy>=y && guys[i]->mapx<=x2 && guys[i]->mapy<=y2)
				cnt++;
	}
	else if(type==MONS_BADGUY)	// badguys
	{
		for(i=0;i<maxGuys;i++)
			if(guys[i]->type>0 && guys[i]->type!=MONS_NOBODY && !guys[i]->friendly && guys[i]->mapx>=x && guys[i]->mapy>=y && guys[i]->mapx<=x2 && guys[i]->mapy<=y2)
				cnt++;
	}
	else if(type==MONS_NONPLAYER)	// anyone but bouapha
	{
		for(i=0;i<maxGuys;i++)
			if(guys[i]->type>0 && guys[i]->type!=MONS_NOBODY && guys[i]->aiType!=MONS_BOUAPHA && guys[i]->mapx>=x && guys[i]->mapy>=y && guys[i]->mapx<=x2 && guys[i]->mapy<=y2)
				cnt++;
	}
	else if(type==MONS_PLAYER)	// bouapha only
	{
		for(i=0;i<maxGuys;i++)
			if(guys[i]->type>0 && guys[i]->type!=MONS_NOBODY && guys[i]->aiType==MONS_BOUAPHA && guys[i]->mapx>=x && guys[i]->mapy>=y && guys[i]->mapx<=x2 && guys[i]->mapy<=y2)
				cnt++;
	}
	else if(type==MONS_TAGGED)	// only the tagged monster's type
	{
		for(i=0;i<maxGuys;i++)
			if(TaggedMonster()!=NULL && guys[i]->type==TaggedMonster()->type && guys[i]->mapx>=x && guys[i]->mapy>=y && guys[i]->mapx<=x2 && guys[i]->mapy<=y2)
				cnt++;
	}
	return cnt;
}

void Telefrag(Guy *g)
{
	int i,xx,yy;

	xx=g->x>>FIXSHIFT;
	yy=g->y>>FIXSHIFT;
	for(i=0;i<maxGuys;i++)
		if((guys[i]) && (guys[i]!=g) && (guys[i]->type) && (guys[i]->hp>0) &&
			(abs(guys[i]->mapx-g->mapx)<8) && (abs(guys[i]->mapy-g->mapy)<8))
		{
			if(g->CoconutBonk(xx,yy,guys[i]))
			{
				// uh oh, somebody is in the way!
				if(guys[i]->aiType==MONS_BOUAPHA)
				{
					SendMessageToGame(MSG_RESET,0);
				}
				if(guys[i]->friendly==0 && !(MonsterFlags(guys[i]->type,guys[i]->aiType)&(MF_NOHIT|MF_INVINCIBLE)))
					player.enemiesSlain++;

				// handle item drops
				if(guys[i]->type==MONS_ZOMBIE || guys[i]->type==MONS_MUTANT)	// zombies always drop a brain
				{
					if(!curMap->DropItem(guys[i]->mapx,guys[i]->mapy,ITM_BRAIN))
						curMap->GetTile(guys[i]->mapx,guys[i]->mapy)->item=ITM_BRAIN;	// force the drop if it failed
				}
				else if(guys[i]->type==MONS_SUPERZOMBIE)	// super zombies always drop 2 brains
				{
					if(!curMap->DropItem(guys[i]->mapx,guys[i]->mapy,ITM_BRAIN))
						curMap->GetTile(guys[i]->mapx,guys[i]->mapy)->item=ITM_BRAIN;	// force the drop if it failed
					if(!curMap->DropItem(guys[i]->mapx,guys[i]->mapy+1,ITM_BRAIN) && guys[i]->mapy+1<curMap->height)
						curMap->GetTile(guys[i]->mapx,guys[i]->mapy+1)->item=ITM_BRAIN;	// hope there's a legal coordinate and non-wall below me!
				}
				if(guys[i]->aiType==MONS_GNOME)
				{
					if(guys[i]->mind3!=0)	// drop what you stole!
						if(!curMap->DropItem(guys[i]->mapx,guys[i]->mapy,guys[i]->mind3))
							curMap->GetTile(guys[i]->mapx,guys[i]->mapy)->item=guys[i]->mind3;	// force the drop if it failed
				}
				if(guys[i]->item==ITM_RANDOM)
				{
					if(Random(100*FIXAMT)<curMap->itemDrops)
						curMap->DropItem(guys[i]->mapx,guys[i]->mapy,GetRandomItem());
				}
				else if(guys[i]->item!=ITM_NONE)
				{
					if(!curMap->DropItem(guys[i]->mapx,guys[i]->mapy,guys[i]->item))
						curMap->GetTile(guys[i]->mapx,guys[i]->mapy)->item=guys[i]->item;	// force the drop if it failed
				}

				guys[i]->type=MONS_NONE;

				BlowUpGuy((guys[i]->x>>FIXSHIFT)-32,(guys[i]->y>>FIXSHIFT)-24,
					(guys[i]->x>>FIXSHIFT)+32,(guys[i]->y>>FIXSHIFT)+24,0,1);
			}
		}
}

void FindMonsterBrain(int myx,int myy)
{
	int i,j;

	if(player.brainX==255)
		j=255*255+255*255;
	else
		j=(myx-player.brainX)*(myx-player.brainX)+(myy-player.brainY)*(myy-player.brainY);

	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type && guys[i]->hp && guys[i]->aiType!=MONS_BOUAPHA)
		{
			if(guys[i]->type==MONS_ZOMBIE || guys[i]->type==MONS_ZOMBONI || guys[i]->type==MONS_MUTANT || guys[i]->type==MONS_SUPERZOMBIE)
			{
				if((myx-guys[i]->mapx)*(myx-guys[i]->mapx)+(myy-guys[i]->mapy)*(myy-guys[i]->mapy)<j)
				{
					player.brainX=guys[i]->mapx;
					player.brainY=guys[i]->mapy;
					j=(myx-player.brainX)*(myx-player.brainX)+(myy-player.brainY)*(myy-player.brainY);
				}
			}
			else if(guys[i]->item<ITM_RANDOM && GetItem(guys[i]->item)->effect==IE_BRAIN &&
				GetItem(guys[i]->item)->effectAmt>0 && ((myx-guys[i]->mapx)*(myx-guys[i]->mapx)+(myy-guys[i]->mapy)*(myy-guys[i]->mapy))<j)
			{
				player.brainX=guys[i]->mapx;
				player.brainY=guys[i]->mapy;
				j=(myx-player.brainX)*(myx-player.brainX)+(myy-player.brainY)*(myy-player.brainY);
			}
		}
	}
}

void FindMonsterCandle(int myx,int myy)
{
	int i,j;

	if(player.candleX==255)
		j=255*255+255*255;
	else
		j=(myx-player.candleX)*(myx-player.candleX)+(myy-player.candleY)*(myy-player.candleY);

	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type && guys[i]->hp && guys[i]->aiType!=MONS_BOUAPHA)
		{
			if(guys[i]->item<ITM_RANDOM && GetItem(guys[i]->item)->effect==IE_CANDLE &&
				GetItem(guys[i]->item)->effectAmt>0 && ((myx-guys[i]->mapx)*(myx-guys[i]->mapx)+(myy-guys[i]->mapy)*(myy-guys[i]->mapy))<j)
			{
				player.candleX=guys[i]->mapx;
				player.candleY=guys[i]->mapy;
				j=(myx-player.candleX)*(myx-player.candleX)+(myy-player.candleY)*(myy-player.candleY);
			}
		}
	}
}

byte Guy::IsAwake(void)
{
	if(frozen)
		return 0;	// frozen monsters are always asleep!

	if(target==nobody)
		return 0;	// monsters with no target are asleep

	switch(aiType)
	{
		case MONS_EGGSAC:
		case MONS_MOSSGRANDE:
		case MONS_LAMP:
		case MONS_COFFIN:
		case MONS_ALIENEGG:
		case MONS_ALIENEGG2:
		case MONS_STAREYBAT:	// only awake if staring at you
		case MONS_STARFISH:		// only awake if shooting at you
		case MONS_DARKCOFFIN:
		case MONS_XENOEGG:
			return (seq==ANIM_ATTACK);	// only awake if actively making a monster
			break;
		case MONS_ROBOFACTORY:
		case MONS_PATCH:
		case MONS_PATCH2:
		case MONS_PATCH3:
		case MONS_PATCH4:
			return (seq==ANIM_ATTACK || seq==ANIM_A1);	// only awake if actively making a monster
			break;
		case MONS_GENERATOR1:
		case MONS_GENERATOR2:
		case MONS_GENERATOR3:
		case MONS_GENERATOR4:
			return (mind2==1);	// only considered active during the last second before summoning
			break;
		case MONS_VAMPIRE:
		case MONS_DARKVAMP:
		case MONS_SPIKEY:
			return (mind!=2);	// considered asleep if garlicked
			break;
		case MONS_PYGMY:
		case MONS_PUMPKIN:
		case MONS_BABYTHING:
		case MONS_MUSH:
		case MONS_SUPERZOMBIE:
		case MONS_STICKMAN:
		case MONS_ISOZOID:
		case MONS_PENGUIN:
		case MONS_ZOMBONI:
		case MONS_SVEN:
		case MONS_BJORN:
		case MONS_MUMBLE:
		case MONS_LICH:
		case MONS_MECHABOUAPHA:
		case MONS_DRL:
		case MONS_MINECART:	// not being ridden=asleep
		case MONS_RAFT:		// same
		case MONS_GHOST:
		case MONS_PYGMY2:
		case MONS_PYGMY3:
		case MONS_KNIGHT:
		case MONS_COUNTESS:
		case MONS_TURRET:
		case MONS_KONGOR:
		case MONS_SQUASH:
		case MONS_MINIPYGMY:
		case MONS_LOONYBOT:
		case MONS_GOODTURRET:
		case MONS_SHAMAN2:
		case MONS_SHARK:
		case MONS_GNOME:
		case MONS_PUMPKIN2:
		case MONS_BOOMKIN:
		case MONS_PYGMYACCT:
		case MONS_CRAZYPANTS:
		case MONS_CONE:
		case MONS_PYGMYDIVER:
		case MONS_OLAF:
		case MONS_FOLLOWBUNNY:
		case MONS_AUTOZOID:
		case MONS_JELLOFISH:
		case MONS_PUNKIN:
		case MONS_SNKYSHRK2:
		case MONS_WHATSIT:
		case MONS_SHOCKGHOST:
		case MONS_MISLTURRET:
		case MONS_DEATHTURRET:
		case MONS_WEATHERMAN:
		case MONS_PUNKBUNNY:
		case MONS_JALAPENO:
			return (mind!=0);
			break;
		case MONS_LOONYGUN:
		case MONS_LOONYCORE:
			return (parent && parent->mind!=0);
			break;
		case MONS_LAZYBONE:
			return (seq!=ANIM_IDLE);
			break;
		case MONS_PUFFYFISH:
		case MONS_PUFFYFISH2:
			return (seq!=ANIM_MOVE);	// awake if puffed to any degree
			break;
		case MONS_YUGO:
		case MONS_PATROLLR:
		case MONS_PATROLUD:
		case MONS_DPATROLLR:
		case MONS_DPATROLUD:
			return (mind==2);	// awake if being driven
			break;
		default:
			return 1;	// by default, monsters that don't do a wake-up are awake
			break;
	}
}

byte BadguyRegions(int x,int y,int x2,int y2,int tx,int ty)
{
	int i,j;
	byte ok;

	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type && guys[i]->hp && guys[i]->mapx>=x && guys[i]->mapx<=x2 && guys[i]->mapy>=y && guys[i]->mapy<=y2)
		{
			ok=0;
			// this guy is in the first rect
			for(j=0;j<maxGuys;j++)
			{
				if(j!=i && guys[j]->type==guys[i]->type && guys[j]->hp && guys[j]->mapx==guys[i]->mapx+tx-x && guys[j]->mapy==guys[i]->mapy+ty-y)
				{
					ok=1;
					break;
				}
			}
			if(!ok)
				return 0;
		}

		if(guys[i]->type && guys[i]->hp && guys[i]->mapx>=tx && guys[i]->mapx<=tx+(x2-x) && guys[i]->mapy>=ty && guys[i]->mapy<=ty+(y2-y))
		{
			ok=0;
			// this guy is in the second rect
			for(j=0;j<maxGuys;j++)
			{
				if(j!=i && guys[j]->type==guys[i]->type && guys[j]->hp && guys[j]->mapx==guys[i]->mapx+x-tx && guys[j]->mapy==guys[i]->mapy+y-ty)
				{
					ok=1;
					break;
				}
			}
			if(!ok)
				return 0;
		}
	}
	return 1;
}

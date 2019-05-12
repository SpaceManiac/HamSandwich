#include "guy.h"
#include "player.h"
#include "intface.h"
#include "fairy.h"
#include "spell.h"
#include "challenge.h"

Guy **guys;
Guy *goodguy;
int maxGuys;
byte badguys;
int lastSafeX,lastSafeY;

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
	if((MonsterFlags(type)&MF_ENEMYWALK) && (him->type!=MONS_BOUAPHA))
		return 0;
	if((MonsterFlags(type)&MF_FREEWALK) && (him->type==MONS_BOUAPHA))
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

byte Guy::AttackCheck2(int xx,int yy,int xx2,int yy2,Guy *him)
{
	int x2,y2;

	if(!him)
		return 0;

	x2=him->x>>FIXSHIFT;
	y2=him->y>>FIXSHIFT;

	//DrawDebugBox(xx,yy,xx+size*2,yy+size*2);
	if((x2+him->rectx2)>=xx && (y2+him->recty2)>=yy && (x2+him->rectx)<=(xx2) &&
		(y2+him->recty)<=(yy2))
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

	if((me->type==MONS_BOUAPHA || me->type==MONS_LOG) && (!PlayerGetItem(m->item,me->x,me->y)))
		m->item=ITM_NONE;

	if((world->terrain[m->floor].flags&(TF_WATER|TF_LAVA)) && (!(MonsterFlags(me->type)&(MF_WATERWALK|MF_FLYING))))
		result=0;

	if((world->terrain[m->floor].flags&TF_SOLID) && ((!(MonsterFlags(me->type)&(MF_FLYING|MF_WALLWALK)))
		|| (player.levelNum==1 && player.worldNum==3)))
		result=0; // impassable to non-flyers, and also flyers if it is the final map level

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
		{
			if(me->type==MONS_BALL && m->wall && (world->terrain[m->floor].flags&TF_PUSHON))
			{
				m->wall=0;
				m->floor=(byte)MGL_random(3);
				MakeSound(SND_BOMBBOOM,(x*TILE_WIDTH)<<FIXSHIFT,(y*TILE_HEIGHT)<<FIXSHIFT,SND_CUTOFF,1500);
				ExplodeParticles(PART_DIRT,(x*TILE_WIDTH+TILE_WIDTH/2)<<FIXSHIFT,(y*TILE_HEIGHT+TILE_HEIGHT/2)<<FIXSHIFT,FIXAMT*10,20);
			}

			result=0;
		}
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

	if(type==MONS_FAIRY2)
		return 1;

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

	if(result)	// no wall collision, look for guy collision
		for(i=0;i<maxGuys;i++)
			if((guys[i]) && (guys[i]!=this) && (guys[i]->type) && (guys[i]->hp>0) &&
				(abs(guys[i]->mapx-mapx)<8) && (abs(guys[i]->mapy-mapy)<8))
			{
				if(CoconutBonk(xx,yy,guys[i]))
				{
					if(type==MONS_BOUAPHA && guys[i]->type==MONS_SPIDER && abs(dx)+abs(dy)>FIXAMT*4)
					{
						MakeSound(SND_ACIDSPLAT,x,y,SND_CUTOFF,20);
						guys[i]->GetShot(0,0,10,map,world);	// squish the spider!
					}
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
			if(player.fairyOn==FAIRY_SAVEY && beenReborn==0)
			{
				seq=ANIM_A4;
				frm=0;
				frmAdvance=128;
				action=ACTION_BUSY;
				player.life=player.maxLife/4;
				player.mana=player.maxMana/4;
				hp=player.life;
				player.shield=30*2;
				beenReborn=1;
				if(player.experience>player.needExp/4)	// lose 25% of the XP needed for next level
					player.experience-=player.needExp/4;
				else
					player.experience=0;

				x=lastSafeX;
				y=lastSafeY;
				// make resurrect sound
				MakeNormalSound(SND_RESURRECT);
				return;
			}
			else
			{
				// restart current level
				SendMessageToGame(MSG_RESET,0);
				ChallengeEvent(CE_DIE,0);
			}
		}

		type=MONS_NONE;	// all gone
	}
	if(seq==ANIM_A4 && type==MONS_BOBBY)	// blocked a shot
	{
		// jump back into the block animation
		seq=ANIM_A1;
		frm=placed;
		return;
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

	if(frozen && hp>0)
	{
		bright=map->map[mapx+mapy*map->width].templight;

		frozen--;
		if(ouch>0)
			ouch--;
		return;

	}

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
		if(type==MONS_BOUAPHA || type==MONS_TOWER || type==MONS_SPIDER || type==MONS_SLUG || type==MONS_BALL ||
			type==MONS_INCAGOLD || type==MONS_INCAGOLD2 || type==MONS_LOG || type==MONS_INCABOSS || type==MONS_OCTOBOSS ||
			type==MONS_GOAT1 || type==MONS_GOAT1B)
			mind1=1;	// tell it that it hit a wall
	}

	y+=dy;

	if(!CanWalk(x,y,map,world))
	{
		y-=dy;
		if(type==MONS_BOUAPHA || type==MONS_TOWER || type==MONS_SPIDER || type==MONS_SLUG || type==MONS_BALL ||
			type==MONS_INCAGOLD || type==MONS_INCAGOLD2 || type==MONS_LOG || type==MONS_INCABOSS || type==MONS_OCTOBOSS ||
			type==MONS_GOAT1 || type==MONS_GOAT1B)
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
			&& (!PlayerCanWaterwalk()) && !parent)
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
			&& (!PlayerCanWaterwalk()) && !parent && !PlayerShield())
		{
			BlowWigglySmoke(x,y,0,FIXAMT*2);
			BlowWigglySmoke(x,y,0,FIXAMT*4);
			if(burnFlip)
			{
				GetShot(0,0,12,map,world);
				if(hp==0)	// it was the death of me
				{
					// lava death
				}
			}
			burnFlip=1-burnFlip;
		}
		if(player.worldNum==2 && player.levelNum==16 && (oldmapx!=mapx || oldmapy!=mapy) &&
			mapx==11 && mapy==31 && map->map[mapx+mapy*map->width].floor==59)
		{
			// the special case for the vault puzzle
			if(VaultPuzzleCheck(map))
			{
				NewMessage("The vault opened!!",60);
				MakeNormalSound(SND_MENUCLICK);
				map->map[mapx+mapy*map->width].floor=60;
				OpenVaultDoor(map);
			}
			else
			{
				NewMessage("The switch won't budge!",60);
				oldmapx=mapx;
				oldmapy=mapy;	// don't let it switch the tile
			}
		}

		if(parent)	// riding a log
		{
			x=parent->x;
			y=parent->y+FIXAMT*4;
		}
		if(seq!=ANIM_DIE && seq!=ANIM_A3)
		{
			lastSafeX=x;
			lastSafeY=y;
		}
		if(player.worldNum==3 && player.levelNum==14 && (oldmapx!=mapx || oldmapy!=mapy) &&
			(map->map[mapx+mapy*map->width].floor==72 || map->map[mapx+mapy*map->width].floor==73))
			LightsOutPuzzle(map,mapx,mapy);
	}
	if((oldmapx!=mapx || oldmapy!=mapy) &&
		(world->terrain[map->map[mapx+mapy*map->width].floor].flags&TF_STEP) && !(MonsterFlags(type)&MF_FLYING) &&
		MySpecialType(type,map,mapx,mapy))
	{
		MakeSound(SND_MENUCLICK,(mapx*TILE_WIDTH)<<FIXSHIFT,(mapy*TILE_HEIGHT)<<FIXSHIFT,SND_CUTOFF,1000);
		map->map[mapx+mapy*map->width].floor=world->terrain[map->map[mapx+mapy*map->width].floor].next;
	}
	if(oldmapx!=mapx || oldmapy!=mapy)
		SpecialStepCheck(map,mapx,mapy,this);

	bright=map->map[mapx+mapy*map->width].templight;
}

void Guy::OverworldUpdate(Map *map,world_t *world)
{
	int oldmapx,oldmapy;
	byte b;
	static byte burnFlip=0;

	oldmapx=mapx;
	oldmapy=mapy;

	CalculateRect();

	if(type==MONS_BOUAPHA)	// special case, player controls Bouapha
		PlayerControlMe(this,&map->map[mapx+mapy*map->width],world);
	else if(player.levelNum!=1)	// not on the overworld
		MonsterControl(map,world);
	else
		OverworldControl(map,world);

	if(ouch>0)
		ouch--;

	x+=dx;

	if(!CanWalk(x,y,map,world))
	{
		x-=dx;
		if(type==MONS_BOUAPHA || type==MONS_INCAGOLD ||
			type==MONS_INCAGOLD2)
			mind1=1;	// tell it that it hit a wall
	}

	y+=dy;

	if(!CanWalk(x,y,map,world))
	{
		y-=dy;
		if(type==MONS_BOUAPHA || type==MONS_INCAGOLD ||
			type==MONS_INCAGOLD2)
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

void Guy::NoMoveUpdate(Map *map)
{
	int mapx,mapy;
	byte b;

	mapx=(x>>FIXSHIFT)/TILE_WIDTH;
	mapy=(y>>FIXSHIFT)/TILE_HEIGHT;
	bright=map->map[mapx+mapy*map->width].templight;

	if(type==MONS_BOUAPHA)
	{
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
	}
}

void Guy::Render(byte light)
{
	MonsterDraw(x,y,z,type,seq,frm,facing,bright*(light>0),mind1,ouch,frozen);
}

void Guy::OverworldControl(Map *map,world_t *world)
{
	if(type!=MONS_FRIENDLY && type!=MONS_FAIRY && type!=MONS_FAIRY2)
		AI_Overworld(this,map,world,goodguy);
	else if(type==MONS_FRIENDLY)
		AI_Friendly(this,map,world,goodguy);
	else if(type==MONS_FAIRY)
		AI_Fairy(this,map,world,goodguy);
	else
		AI_Fairy2(this,map,world,goodguy);
}

void Guy::MonsterControl(Map *map,world_t *world)
{
	Guy *goodguy;

	goodguy=NearestEnemy(this);

	switch(type)
	{
		case MONS_PINKEYE:
			AI_Pinkeye(this,map,world,goodguy);
			break;
		case MONS_LOOKEYLOO:
			AI_Lookeyloo(this,map,world,goodguy);
			break;
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
		case MONS_FRIENDLY:
			AI_Friendly(this,map,world,goodguy);
			break;
		case MONS_EYEGUY:
			AI_Eyeguy(this,map,world,goodguy);
			break;
		case MONS_PEEPER:
			AI_Peeper(this,map,world,goodguy);
			break;
		case MONS_TOWER:
			AI_Tower(this,map,world,goodguy);
			break;
		case MONS_SHRMLORD:
			AI_Shroomlord(this,map,world,goodguy);
			break;
		case MONS_MICRO:
			AI_Microfish(this,map,world,goodguy);
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
		case MONS_HUGEBAT:
			AI_HugeBat(this,map,world,goodguy);
			break;
		case MONS_FAIRY:
			AI_Fairy(this,map,world,goodguy);
			break;
		case MONS_FAIRY2:
			AI_Fairy2(this,map,world,goodguy);
			break;
		case MONS_BIGBAT:
			AI_BigBat(this,map,world,goodguy);
			break;
		case MONS_OCTOPUS:
			AI_Octopus(this,map,world,goodguy);
			break;
		case MONS_INCAGOLD:
		case MONS_INCAGOLD2:
			AI_Incagold(this,map,world,goodguy);
			break;
		case MONS_INCABOSS:
			AI_Incaboss(this,map,world,goodguy);
			break;
		case MONS_INCATONGUE:
			AI_Incatongue(this,map,world,goodguy);
			break;
		case MONS_LICH:
			AI_Lich(this,map,world,goodguy);
			break;
		case MONS_PTERO:
			AI_Ptero(this,map,world,goodguy);
			break;
		case MONS_GOLEM:
			AI_Golem(this,map,world,goodguy);
			break;
		case MONS_BOBBY:
			AI_BobbyKhan(this,map,world,goodguy);
			break;
		case MONS_CRAZYBUSH:
			AI_CrazyBush(this,map,world,goodguy);
			break;
		case MONS_INCABOT:
			AI_Incabot(this,map,world,goodguy);
			break;
		case MONS_INCAGEN:
			AI_Incagen(this,map,world,goodguy);
			break;
		case MONS_LOG:
			AI_Log(this,map,world,goodguy);
			break;
		case MONS_PEEPBOMB:
		case MONS_PEEPBOMB2:
			AI_PeepBomb(this,map,world,goodguy);
			break;
		case MONS_TRAPAZOID:
		case MONS_TRAPAZOID2:
		case MONS_WHACKAZOID:
			if(player.worldNum==2 && player.levelNum==19)
				AI_Whackazoid(this,map,world,goodguy);
			else
				AI_Trapazoid(this,map,world,goodguy);
			break;
		case MONS_SLUG:
			AI_Slug(this,map,world,goodguy);
			break;
		case MONS_SNAIL:
		case MONS_SNAIL2:
			AI_Snail(this,map,world,goodguy);
			break;
		case MONS_BALL:
			AI_Ball(this,map,world,goodguy);
			break;
		case MONS_OCTOBOSS:
			AI_OctoBoss(this,map,world,goodguy);
			break;
		case MONS_OCTOTENT:
		case MONS_OCTOTENT2:
			AI_OctoTentacle(this,map,world,goodguy);
			break;
		case MONS_GOAT1:
		case MONS_GOAT1B:
			AI_Rammy(this,map,world,goodguy);
			break;
		case MONS_GOAT2:
		case MONS_GOAT2B:
			AI_Hoppy(this,map,world,goodguy);
			break;
		case MONS_GOAT3:
			AI_Gruffy(this,map,world,goodguy);
			break;
		case MONS_STICKSHROOM:
			AI_StickShroom(this,map,world,goodguy);
			break;
		case MONS_STICKSPIDER:
			AI_StickSpider(this,map,world,goodguy);
			break;
		case MONS_STICKCORPSE:
			AI_StickCorpse(this,map,world,goodguy);
			break;
		case MONS_STICKBAT:
			AI_StickBat(this,map,world,goodguy);
			break;
		case MONS_DANCER:
			AI_Dancer(this,map,world,goodguy);
			break;
	}
}

void Guy::GetShot(int dx,int dy,int damage,Map *map,world_t *world)
{
	int i,j,formerHP;

	if(hp==0)
		return;	// can't shoot a dead guy

	if(type==0)
		return;	// shouldn't have a type 0 guy at all

	if(type==MONS_BOUAPHA && PlayerShield())
		return; // invincible when shielded

	if((MonsterFlags(type)&MF_INVINCIBLE) && (damage!=-1000))	// invincible monsters can be frozen
		return;	// invincible

	if(type==MONS_LOOKEYLOO && seq!=ANIM_ATTACK)
		return;	// can only hit lookey-loo when its eye is open

	if(damage==0)
		return;

	if(type==MONS_INCATONGUE)
	{
		monsType[MONS_INCABOSS].flags^=MF_INVINCIBLE;
		parent->GetShot(dx,dy,damage,map,world);
		monsType[MONS_INCABOSS].flags|=MF_INVINCIBLE;
		return;
	}

	if(type==MONS_BOUAPHA)
	{
		if(player.nightmare)
		{
			if(player.levelNum==11 && player.worldNum==3)
				i=damage*NIGHTMAREDMG/2;
			else
				i=damage*NIGHTMAREDMG;
		}
		else
			i=damage;

		i-=player.defense;

		if(player.fairyOn==FAIRY_RUNNY)
			i=(i*5)/4;
		if(player.fairyOn==FAIRY_BLOCKY || player.fairyOn==FAIRY_MIGHTY)
			i=(i*3)/4;
		if(player.fairyOn==FAIRY_TOUGHY)
			i=i/2;

		i=(i*(100-player.shieldStones)/100);
		if(i<1)
			i=1;
		if(player.stoneskin)
		{
			if(player.spell[6]==1)
				i/=2;		// stoneskin cuts damage in half
			else
				i/=8;		// steelskin divides it by 8
		}

		damage=i;
		if(player.gear&GEAR_POINTY)
		{
			for(i=0;i<3;i++)
				FireBullet(x,y,(byte)MGL_random(256),BLT_MINIFBALL);
		}
	}
	else if(damage!=-1000)
	{
		i=BulletHittingType();
		if(i==BLT_HAMMER || i==BLT_HAMMER2)
		{
			damage=(damage*(100+player.powerStones*10))/100;
			if(player.fairyOn==FAIRY_BLOCKY)
			{
				damage=damage*3/4;
				if(damage<1)
					damage=1;
			}
			if(player.fairyOn==FAIRY_MIGHTY)
			{
				damage=damage*5/4;
			}
		}
		else
		{
			// the damage is coming from a spell, most likely
			if(!(MonsterFlags(type)&MF_GOODGUY))
				damage=(damage*(100+player.spellStones*10))/100;
			// goodguys get hit with full damage
		}
	}

	if(type==MONS_BOBBY && ((seq==ANIM_A1 && frm>1 && frm<11) || (seq==ANIM_A4)))
	{
		if(seq==ANIM_A4)
			return;	// just ignore it

		placed=frm;	// keep track of the frame to come back to it
		seq=ANIM_A4;
		frm=0;
		frmTimer=0;
		frmAdvance=128;
		// make clang noise
		MakeSound(SND_BOBBYBLOCK,x,y,SND_CUTOFF,1200);
		return;
	}

	if(damage==-1000)	// freeze code
	{
		if(player.nightmare)
			i=SpellLevel()*20-MonsterHP(type)/NIGHTMAREHP;	// the more hp, the harder to freeze
		else
			i=SpellLevel()*20-MonsterHP(type);	// the more hp, the harder to freeze
		if(type!=MONS_BOUAPHA)
			ShowEnemyLife(MonsterName(type),hp*128/MonsterHP(type),hp*128/MonsterHP(type),(byte)(hp>0));
		if(i<0)
			return;
		else
		{
			frozen+=i;
			return;
		}
	}

	if(type==MONS_BOUAPHA && damage>0)
		ChallengeEvent(CE_OUCH,damage);
	else if(type!=MONS_PTERO && type!=MONS_GOLEM)
		ChallengeEvent(CE_HIT,damage);
	formerHP=hp;
	if(damage>0)
		ouch=4;
	hp-=damage;

	if(frozen)	// thaw when hit
	{
		if(frozen-damage*50<0)
		{
			if(frozen>5)
				frozen=5;	// so they can blink a bit
			else
				frozen=0;
		}
		else
			frozen-=damage*50;
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
		hp=0;
		seq=ANIM_DIE;
		this->dx=0;
		this->dy=0;
		frm=0;
		frmAdvance=64;
		frmTimer=0;
		action=ACTION_BUSY;
		if(type==MONS_MUSH)
			facing=2;	// these can only die facing forward, artistically speaking
		if(type==MONS_PEEPBOMB || type==MONS_PEEPBOMB2)
			facing=0;
		// possible item drop
		switch(type)
		{
			case MONS_PINKEYE:
			case MONS_BOUAPHA:
			case MONS_SERPENT:	// don't drop items, since you live on the water
			case MONS_MICRO:	// fish, no drop
			case MONS_MAGMAZOID:// don't drop items, since you live in the lava
			case MONS_FRIENDLY:
			case MONS_MUSH:
			case MONS_LICH:
			case MONS_THINGTENT:
			case MONS_THINGTENTTIP:
			case MONS_THING:
			case MONS_PTERO:
			case MONS_MOSS:
			case MONS_GOLEM:
			case MONS_BOBBY:
			case MONS_LOOKEYLOO:
			case MONS_WHACKAZOID:
			case MONS_OCTOBOSS:
			case MONS_OCTOTENT:
			case MONS_OCTOTENT2:
			case MONS_OCTOPUS:
				break;
			case MONS_GOAT1:
			case MONS_GOAT1B:
			case MONS_GOAT2:
			case MONS_GOAT2B:
			case MONS_GOAT3:	// goats drop brains
				map->map[mapx+mapy*map->width].item=ITM_BRAIN;
				break;
			default:
				// drop cash!
				j=MonsterHP(type);
				if(type==MONS_STICKSHROOM || type==MONS_STICKSPIDER || type==MONS_STICKCORPSE || type==MONS_STICKBAT)
					j=100;	// way more coins from stick monsters
				if(type==MONS_BIGSPDR)
					j*=2;	// double the coins from spitters
				if(type==MONS_EGGSAC || type==MONS_MAMASPDR)
					j/=8;	// quarter coins from egg sacs and mama spiders
				j=MGL_random(j/5+2);
				// don't allow it to be less than half the max
				if(j<MonsterHP(type)/10)
					j=MonsterHP(type)/10;

				if(j>50)
					j=50;	// max it at 50 coins per guy
				// 1% chance of 10-coin, up
				for(i=0;i<j;i++)
					if(MGL_random(100-70*(player.fairyOn==FAIRY_RICHEY)))
						FireBullet(x,y,0,BLT_COIN);
					else
						FireBullet(x,y,0,BLT_BIGCOIN);
				break;
		}

		// give him points for it
		if(type!=MONS_PTERO && type!=MONS_GOLEM && type!=MONS_BOUAPHA)
		{
			ChallengeEvent(CE_KILL,type);
			PlayerGetPoints(MonsterPoints(type));
		}
	}
	if(type!=MONS_BOUAPHA && type!=MONS_PTERO && type!=MONS_GOLEM)
		ShowEnemyLife(MonsterName(type),formerHP*128/MonsterHP(type),hp*128/MonsterHP(type),(byte)(hp>0));
	else if(type==MONS_BOUAPHA)
	{
		if(player.fairyOn==FAIRY_SMARTY && damage>=1)
		{
			damage/=2;
			if(damage==0)
				damage=1;
			if(player.mana>=damage)
				player.mana-=damage;
			else
				player.mana=0;
		}
		SetPlayerHP(hp);
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

	for(i=0;i<maxGuys;i++)
		delete guys[i];

	free(guys);
}

void UpdateGuys(Map *map,world_t *world)
{
	int i;

	badguys=0;
	for(i=0;i<maxGuys;i++)
		if(guys[i]->type!=MONS_NONE)
		{
			if((MonsterFlags(guys[i]->type)&MF_GOODGUY)==0)
				badguys=1;
			guys[i]->Update(map,world);
			if(guys[i]->type==MONS_BOUAPHA && player.berserk)
				guys[i]->Update(map,world);
		}
}

void OverworldUpdateGuys(Map *map,world_t *world)
{
	int i;

	badguys=0;
	for(i=0;i<maxGuys;i++)
		if(guys[i]->type!=MONS_NONE)
		{
			if((MonsterFlags(guys[i]->type)&MF_GOODGUY)==0)
				badguys=1;
			guys[i]->OverworldUpdate(map,world);
		}
}

byte BadguyCount(void)
{
	return badguys;
}

void EditorUpdateGuys(Map *map)
{
	int i;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type!=MONS_NONE)
			guys[i]->EditorUpdate(map);
}

void NoMoveUpdateGuys(Map *map)
{
	int i;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type!=MONS_NONE)
			guys[i]->NoMoveUpdate(map);
}

void RenderGuys(byte light)
{
	int i;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type!=MONS_NONE)
			guys[i]->Render(light);
}

void FairyChange(word former,word now)
{
	int i;

	player.fairyOn=now;
	if(Challenging())
		return;	// you do it from a menu, not in-game, if you're in challenge mode

	if(former==0 && now!=0)	// have to add the fairy
	{
		AddGuy(goodguy->x,goodguy->y,0,MONS_FAIRY2);
	}
	else if(now==0)	// have to remove the fairy
	{
		for(i=0;i<maxGuys;i++)
			if(guys[i]->type==MONS_FAIRY2)
				guys[i]->type=MONS_NONE;
	}
	else	// just change the fairy
	{
		// don't do anything!
	}
}

Guy *AddGuy(int x,int y,int z,byte type)
{
	int i,j;
	Guy *g;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type==MONS_NONE)
		{
			guys[i]->placed=0;
			guys[i]->type=type;
			guys[i]->x=x;
			guys[i]->y=y;
			guys[i]->z=z;
			guys[i]->seq=ANIM_IDLE;
			guys[i]->frm=0;
			guys[i]->frmTimer=0;
			guys[i]->frmAdvance=128;
			if(guys[i]->type!=MONS_BOUAPHA)
				guys[i]->hp=MonsterHP(type);
			else
				guys[i]->hp=player.maxLife;
			guys[i]->facing=2;
			guys[i]->ouch=0;
			guys[i]->dx=0;
			guys[i]->dy=0;
			guys[i]->dz=0;
			guys[i]->bright=0;
			guys[i]->mind=0;
			guys[i]->mind1=0;
			guys[i]->reload=1;
			guys[i]->parent=NULL;
			guys[i]->CalculateRect();
			guys[i]->ID=i;
			guys[i]->mapx=(guys[i]->x>>FIXSHIFT)/TILE_WIDTH;
			guys[i]->mapy=(guys[i]->y>>FIXSHIFT)/TILE_HEIGHT;
			guys[i]->frozen=0;
			if(type==MONS_BOUAPHA)	// special case: bouapha is the goodguy
			{
				goodguy=guys[i];
				PutCamera(x,y);
				if(player.fairyOn!=0)
				{
					g=AddGuy(guys[i]->x,guys[i]->y,0,MONS_FAIRY2);
				}
			}
			if(type==MONS_FAIRY2)
			{
				for(j=0;j<16;j++)
				{
					if((1<<j)==player.fairyOn)
						guys[i]->mind1=(byte)(j+1);
				}
			}
			if(type==MONS_BOBBY)
			{
				PutCamera(x,y);
				MakeNormalSound(SND_BOBBYLAUGH);
			}
			if(type==MONS_INCABOSS)
			{
				g=AddGuy(x,y,0,MONS_INCATONGUE);
				if(g)
					g->parent=guys[i];
			}

			if(type==MONS_FRIENDLY)	// friendly has to decide which friendly to be
			{
				switch(player.worldNum*50+player.levelNum)
				{
					case 1:
						// the blond guy
						guys[i]->mind=ANIM_MOVE;
						break;
					case 13:
					case 51:
					case 111:
						// the chick
						guys[i]->mind=ANIM_ATTACK;
						break;
					case 11:
					case 10:
					case 55:
					case 116:
						// the wizard
						guys[i]->mind=ANIM_A1;
						break;
				}
			}
			if(type==MONS_OCTOBOSS && player.levelNum!=1)
			{
				g=AddGuy(x-FIXAMT*140,y-FIXAMT*100,0,MONS_OCTOTENT);
				if(g)
					g->parent=guys[i];
				g=AddGuy(x-FIXAMT*180,y-FIXAMT*30,0,MONS_OCTOTENT);
				if(g)
					g->parent=guys[i];
				g=AddGuy(x-FIXAMT*180,y+FIXAMT*30,0,MONS_OCTOTENT);
				if(g)
					g->parent=guys[i];
				g=AddGuy(x-FIXAMT*140,y+FIXAMT*100,0,MONS_OCTOTENT);
				if(g)
					g->parent=guys[i];

				g=AddGuy(x+FIXAMT*140,y-FIXAMT*100,0,MONS_OCTOTENT2);
				if(g)
					g->parent=guys[i];
				g=AddGuy(x+FIXAMT*180,y-FIXAMT*30,0,MONS_OCTOTENT2);
				if(g)
					g->parent=guys[i];
				g=AddGuy(x+FIXAMT*180,y+FIXAMT*30,0,MONS_OCTOTENT2);
				if(g)
					g->parent=guys[i];
				g=AddGuy(x+FIXAMT*140,y+FIXAMT*100,0,MONS_OCTOTENT2);
				if(g)
					g->parent=guys[i];
			}
			if(type==MONS_LOOKEYLOO)
				guys[i]->reload=255;

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
	int x,y;
	Guy *g;

	for(i=0;i<MAX_MAPMONS;i++)
		if(map->badguy[i].type)
		{
			if(map->badguy[i].type==MONS_BOUAPHA && player.levelNum==1 && player.overworldX!=-2000)
			{
				x=player.overworldX;
				y=player.overworldY;
			}
			else
			{
				x=(map->badguy[i].x*TILE_WIDTH+(TILE_WIDTH/2))<<FIXSHIFT;
				y=(map->badguy[i].y*TILE_HEIGHT+(TILE_HEIGHT/2))<<FIXSHIFT;
			}

			g=AddGuy(x,y,0,map->badguy[i].type);
			if(g!=NULL)
				g->placed=1;
		}
	if(player.levelNum==1)
	{
		for(i=0;i<4;i++)
		{
			if(player.chaseFairy[i])
			{
				while(1)
				{
					x=MGL_random(MAX_SPECIAL);
					if(map->special[x].effect==SPC_GOTOMAP && abs(map->special[x].x-goodguy->mapx)+abs(map->special[x].y-goodguy->mapy)>3)
					{
						y=x;
						break;
					}
				}
				g=AddGuy((map->special[y].x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(map->special[y].y*TILE_HEIGHT+TILE_HEIGHT*3/2)*FIXAMT,0,MONS_FAIRY);
				if(g)
				{
					g->placed=1;
					g->mind1=player.chaseFairy[i];
				}
			}
		}
	}
}

void AddBattleGuys(Map *map,byte t)
{
	int i;
	Guy *g;
	byte amt,me;

	amt=MGL_random(10)+1;

	for(i=0;i<MAX_MAPMONS;i++)
	{
		if(map->badguy[i].type)
		{
			if(map->badguy[i].type==MONS_BOUAPHA)
			{
				g=AddGuy((map->badguy[i].x*TILE_WIDTH+(TILE_WIDTH/2))<<FIXSHIFT,
					   (map->badguy[i].y*TILE_HEIGHT+(TILE_HEIGHT/2))<<FIXSHIFT,
					   0,MONS_BOUAPHA);
			}
			else if(amt>0)
			{
				switch(t)
				{
					case MONS_INCABOT:
						if(MGL_random(20)==0)
							me=MONS_INCAGEN;
						else
							me=MONS_INCABOT;
						break;
					case MONS_CRAZYBUSH:
						me=MONS_CRAZYBUSH;
						break;
					case MONS_BIGSPDR:
						if(amt==1 || MGL_random(5)==0)	// always at least one big spider
							me=MONS_BIGSPDR;
						else
							me=MONS_SPIDER;	// and the rest are eensies
						break;
					case MONS_SHROOM:
						me=MONS_SHROOM;
						if(MGL_random(20)==0)
							me=MONS_SHRMLORD;
						break;
					case MONS_SHRMLORD:
						if(amt==1 || MGL_random(10)==0)	// always at least one shroomlord
							me=MONS_SHRMLORD;
						else
							me=MONS_SHROOM;	// and the rest are shrooms
						break;
					case MONS_BONEHEAD:
						if(MGL_random(20)==0)
							me=MONS_ZOMBIE;
						else
							me=MONS_BONEHEAD;
						break;
					case MONS_PEEPER:
					case MONS_EYEGUY:
						if(MGL_random(8-4*(t==MONS_PEEPER))==0)
							me=MONS_PEEPER;
						else
							me=MONS_EYEGUY;
						break;
					case MONS_BIGBAT:
						if(amt==1 || MGL_random(10)==0)	// always at least one scarier bat
							me=MONS_BIGBAT;
						else
							me=MONS_BAT;	// and the rest are scary bats
						break;
					case MONS_SNAIL:
						if(amt==1)	// mostly slugs
							me=MONS_SNAIL;
						else
							me=MONS_SLUG;
						break;
					case MONS_STICKSHROOM:
						me=MONS_STICKSHROOM;
						break;
					case MONS_STICKSPIDER:
						me=MONS_STICKSPIDER;
						break;
					case MONS_STICKCORPSE:
						me=MONS_STICKCORPSE;
						break;
					case MONS_STICKBAT:
						me=MONS_STICKBAT;
						break;
				}
				g=AddGuy((map->badguy[i].x*TILE_WIDTH+(TILE_WIDTH/2))<<FIXSHIFT,
					   (map->badguy[i].y*TILE_HEIGHT+(TILE_HEIGHT/2))<<FIXSHIFT,
					   0,me);

				amt--;
			}
		}
	}
}

void AddRandomGuy(Map *map,world_t *world,byte chapter,byte rnd)
{
	byte t;
	Guy *g;
	int x,y;
	int cx,cy;

	if(chapter<4)
		if((rnd>0) && MGL_random(200))
			return;	// only 1 in 200 chance of badguy appearing
	else
		if((rnd>0) && MGL_random(100))
			return;	// only 1 in 100 chance of badguy appearing on final chapter

	switch(chapter)
	{
		case 1:	// Over the River
			if(player.level>4)
				x=MGL_random(7);
			else
				x=MGL_random(6);
			if(x<3)
				t=MONS_BIGSPDR;
			else if(x<6)
				t=MONS_SHROOM;
			else
				t=MONS_CRAZYBUSH;
			if(PlayerHasAllSecrets(chapter-1) && MGL_random(20)==0)
				t=MONS_STICKSHROOM;
			break;
		case 2:	// through the woods
			x=MGL_random(6);
			if(x<2)
				t=MONS_BIGSPDR;
			else if(x<4)
				t=MONS_SHROOM;
			else if(x<5)
				t=MONS_SHRMLORD;
			else
				t=MONS_INCABOT;
			if(PlayerHasAllSecrets(chapter-1) && MGL_random(20)==0)
				t=MONS_STICKSPIDER;
			break;
		case 3:	// Castle Heinous
			x=MGL_random(5);
			if(x<2)
				t=MONS_BONEHEAD;
			else if(x<4)
				t=MONS_EYEGUY;
			else
				t=MONS_PEEPER;
			if(PlayerHasAllSecrets(chapter-1) && MGL_random(20)==0)
				t=MONS_STICKCORPSE;
			break;
		case 4:
			x=MGL_random(7);
			if(x<3)
				t=MONS_BONEHEAD;
			else if(x<5)
				t=MONS_BIGBAT;
			else
				t=MONS_SNAIL;
			if(PlayerHasAllSecrets(chapter-1) && MGL_random(20)==0)
				t=MONS_STICKBAT;
			break;
		default:
			t=MONS_BONEHEAD;
			break;
	}

	x=MGL_random(map->width);
	y=MGL_random(map->height);
	GetCamera(&cx,&cy);

	if(abs(x*TILE_WIDTH-cx)<400 && abs(y*TILE_HEIGHT-cy)<400)
		return;	// can't appear on screen!

	g=AddGuy((x*TILE_WIDTH+(TILE_WIDTH/2))<<FIXSHIFT,
		   (y*TILE_HEIGHT+(TILE_HEIGHT/2))<<FIXSHIFT,
		   0,t);

	if(!g)
		return;

	if(g->CanWalk(g->x,g->y,map,world))
		return;
	else
		g->type=MONS_NONE;	// sorry, can't be there
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

word FindVictimNot(int x,int y,byte size,int dx,int dy,int damage,word who,Map *map,world_t *world)
{
	int i;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type && guys[i]->hp && guys[i]->ID!=who && (MonsterFlags(guys[i]->type)&(MF_GOODGUY|MF_NOHIT))==0 &&
			(guys[i]->type!=MONS_OCTOPUS || guys[i]->mind==1))
		{
			if(CheckHit(size,x,y,guys[i]))
			{
				guys[i]->GetShot(dx,dy,damage,map,world);
				return guys[i]->ID;
			}
		}

	return 65535;
}

byte FindVictim(int x,int y,byte size,int dx,int dy,int damage,Map *map,world_t *world)
{
	int i;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type && guys[i]->hp && (MonsterFlags(guys[i]->type)&(MF_GOODGUY|MF_NOHIT|MF_INVINCIBLE))==0 &&
			(guys[i]->type!=MONS_OCTOPUS || guys[i]->mind==1))
		{
			if(CheckHit(size,x,y,guys[i]))
			{
				guys[i]->GetShot(dx,dy,damage,map,world);
				return 1;
			}
		}

	return 0;
}

byte FindGoodVictim(int x,int y,byte size,int dx,int dy,int damage,Map *map,world_t *world)
{
	int i;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type && guys[i]->hp && (MonsterFlags(guys[i]->type)&MF_GOODGUY)!=0 && (MonsterFlags(guys[i]->type)&MF_NOHIT)==0)
		{
			if(CheckHit(size,x,y,guys[i]))
			{
				guys[i]->GetShot(dx,dy,damage,map,world);
				if(guys[i]->type==MONS_BOUAPHA)
					return 2;
				else
					return 1;
			}
		}

	return 0;
}

// this doesn't quit when it finds one victim, it keeps going
byte FindVictims(int x,int y,byte size,int dx,int dy,int damage,Map *map,world_t *world)
{
	int i;
	byte result=0;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type && guys[i]->hp && (MonsterFlags(guys[i]->type)&(MF_GOODGUY|MF_NOHIT))==0 &&
			(guys[i]->type!=MONS_OCTOPUS || guys[i]->mind==1))
		{
			if((damage!=-1000 || guys[i]->frozen==0) && CheckHit(size,x,y,guys[i]))
			{
				guys[i]->GetShot(dx,dy,damage,map,world);
				result=1;
			}
		}

	return result;
}

word LockOnEvil(Map *map,int x,int y)
{
	int i;
	int bestRange,range;
	word bestguy;

	bestguy=65535;
	bestRange=320+240;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type && guys[i]->hp && ((MonsterFlags(guys[i]->type)&(MF_GOODGUY|MF_NOHIT|MF_INVINCIBLE))==0) &&
			guys[i]->type!=MONS_FRIENDLY)
		{
			range=abs(x-(guys[i]->x>>FIXSHIFT))+abs(y-(guys[i]->y>>FIXSHIFT));
			if((range<bestRange) || (range<320+240 && MGL_random(32)==0))
			{
				if(map->FindGuy2((x/TILE_WIDTH),(y/TILE_HEIGHT),15,guys[i]))
				{
					bestguy=i;
					bestRange=range;
				}
			}
		}

	return bestguy;
}

Guy *FindNearestOfType(int x,int y,byte type)
{
	int i;
	int bestRange,range;
	Guy *him;

	him=NULL;
	bestRange=320+240;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type==type && guys[i]->hp)
		{
			range=abs(x-(guys[i]->x>>FIXSHIFT))+abs(y-(guys[i]->y>>FIXSHIFT));
			if((range<bestRange) || him==NULL)
			{
				him=guys[i];
				bestRange=range;
			}
		}

	return him;
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
		if(guys[i]->type==type || (type==255 && guys[i]->type!=MONS_NONE && guys[i]->type!=MONS_FRIENDLY && guys[i]->type!=MONS_FAIRY2 && (MonsterFlags(guys[i]->type)&(MF_GOODGUY|MF_NOHIT|MF_INVINCIBLE))==0 && guys[i]->hp>0))
			return 1;

	return 0;
}

byte MonsterExistsExcept(byte type)
{
	int i;

	for(i=0;i<maxGuys;i++)
		if((guys[i]->type!=type && guys[i]->type!=MONS_NONE && guys[i]->type!=MONS_FRIENDLY && (MonsterFlags(guys[i]->type)&(MF_GOODGUY|MF_NOHIT|MF_INVINCIBLE))==0 && guys[i]->hp>0))
			return 1;

	return 0;
}

void HealGoodguy(byte amt)
{
	if(!goodguy)
		return;

	if(goodguy->hp+amt<player.maxLife)
		goodguy->hp+=amt;
	else
		goodguy->hp=player.maxLife;
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

void KillPinkeyes(void)
{
	int i;

	for(i=0;i<maxGuys;i++)
		if(guys[i] && guys[i]->type==MONS_PINKEYE)
		{
			FireBullet(guys[i]->x,guys[i]->y,0,BLT_BOOM);
			guys[i]->type=MONS_NONE;
		}
}

Guy *NearestEnemy(Guy *me)
{
	Guy *victim=NULL;
	int range=2222222222;
	int i,j;
	word f;

	f=MonsterFlags(me->type)&MF_GOODGUY;
	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type && (MonsterFlags(guys[i]->type)&MF_GOODGUY)!=f && (MonsterFlags(guys[i]->type)&(MF_NOHIT|MF_INVINCIBLE))==0
			&& guys[i]->type!=MONS_FAIRY2)
		{
			j=abs(me->x-guys[i]->x)+abs(me->y-guys[i]->y);
			if(victim==NULL || j<range)
			{
				range=j;
				victim=guys[i];
			}
		}
	}
	return victim;
}

Guy *GetGoodguy(void)
{
	return goodguy;
}

void FairyDoThing(byte what)
{
	int i;
	Guy *g;

	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->type==MONS_FAIRY2)
		{
			switch(what)
			{
				case 1:	// shoot
					g=NearestEnemy(guys[i]);
					if(g)
					{
						FaceGoodguy(guys[i],g);
						FireBullet(guys[i]->x,guys[i]->y,guys[i]->facing*32-10+MGL_random(21),BLT_MINIFBALL);
					}
					break;
			}
			return;
		}
	}
}

byte PeepAtKid(int x,int y,Map *map,byte face)
{
	int i;
	int checkx,checky,mapx,mapy;

	checkx=x;
	checky=y;

	if(goodguy==NULL)
		return 0;

	for(i=0;i<64;i++)
	{
	 	//AddParticle(checkx,checky,0,0,0,0,10,PART_SLIME,1);

		if(abs(checkx-goodguy->x)/FIXAMT+abs(checky-goodguy->y)/FIXAMT<(16+i))
		{
			mapx=x/(TILE_WIDTH*FIXAMT);
			mapy=y/(TILE_HEIGHT*FIXAMT);
			if(map->FindGuy(mapx,mapy,20,goodguy))
				return 1;
			else
				return 0;
		}

		checkx+=Cosine(face)*5;
		checky+=Sine(face)*5;

		mapx=checkx/(TILE_WIDTH*FIXAMT);
		mapy=checky/(TILE_HEIGHT*FIXAMT);

		if(mapx<0 || mapy<0 || mapx>=map->width || mapy>=map->height)
			return 0;	// went off the map

		if(map->map[mapx+mapy*map->width].wall)
			return 0;	// hit a wall
	}

	return 0;
}

Guy *GetGuy(word ID)
{
	int i;

	for(i=0;i<maxGuys;i++)
	{
		if(guys[i]->ID==ID)
			return guys[i];
	}

	return NULL;
}

static dword whackClock,whackTotalClock;
static word whacked;
static int prizeAwarded;

void WhackazoidDisplay(void)
{
	char s[32];

	sprintf(s,"Time: %d:%02d",(whackTotalClock+15)/(30*60),((whackTotalClock+15)/30)%60);
	Print(2,400,s,0,2);
	sprintf(s,"Zoids: %d",whacked);
	Print(2,420,s,0,2);
}

void WhackazoidUpdate(byte init)
{
	int i,tries;
	int lastGuy;

	lastGuy=0;

	for(i=0;i<maxGuys;i++)
		if(guys[i]->type==0)
		{
			lastGuy=i-1;
			break;
		}

	if(init)
	{
		whackClock=30*1;
		whackTotalClock=30*60;
		whacked=0;
		prizeAwarded=0;
	}
	else
	{
		if(!whackTotalClock)
		{
			if(prizeAwarded<4 && goodguy->hp>0)
			{
				goodguy->hp=1;
				player.life=1;
				goodguy->GetShot(0,0,1000,CurrentMap(),&curWorld);
			}
			else
				NewMessage("Time's up!",60);
			return;	// time's up, nothing more happens!
		}
		whackTotalClock--;
		whackClock--;
		if(!whackClock)
		{
			if(30*3-((30*6)-(int)whackTotalClock/9)>10)
				whackClock=30*3-((30*6)-whackTotalClock/9);
			else
				whackClock=10;

			i=MGL_random(lastGuy+1);
			tries=100;
			while(guys[i]->type!=MONS_TRAPAZOID2 && tries-->0)
				i=MGL_random(lastGuy+1);
			if(tries>0)
			{
				guys[i]->type=MONS_WHACKAZOID;
				guys[i]->action=ACTION_BUSY;
				guys[i]->seq=ANIM_ATTACK;
				guys[i]->frm=0;
				guys[i]->frmTimer=0;
				guys[i]->frmAdvance=128-(byte)(whackTotalClock/15);
				guys[i]->hp=1;
			}
		}
	}
}

void WhackedAZoid(Map *map)
{
	byte prizeLevel[8]={5,10,15,20,25,30,35,40};
	byte prizeX[8]={2,4,6,8,11,13,15,17};
	byte prizeItm[8]={ITM_PANTS,ITM_BRAIN,ITM_PANTS,ITM_BRAIN,ITM_BIGCOIN,ITM_HAMMERUP,ITM_BIGCOIN,ITM_FAIRYBELL};

	whacked++;

	if(Challenging())
	{
		prizeItm[6]=ITM_CHLGCRYSTAL;
		if(prizeAwarded<2)
			SetChallengeCrystals(1);
	}
	else
		prizeItm[6]=ITM_BIGCOIN;

	if(prizeAwarded<8)
	{
		if(whacked>=prizeLevel[prizeAwarded])
		{
			if(prizeItm[prizeAwarded]==ITM_FAIRYBELL &&
				(player.haveFairy&(1<<(FairyForThisLevel(player.levelNum+player.worldNum*50)-1))))
			{
				map->map[prizeX[prizeAwarded]+14*map->width].item=ITM_BIGCOIN;	// replace fairy bell
			}
			else
				map->map[prizeX[prizeAwarded]+14*map->width].item=prizeItm[prizeAwarded];
			if(prizeAwarded==3)
				NewMessage("You whacked enough zoids!",60);
			prizeAwarded++;
			MakeNormalSound(SND_PURCHASE);
		}
	}
}

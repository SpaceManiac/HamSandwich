#include "ch_toad.h"
#include "quest.h"
#include "options.h"
#include "bowling.h"

// The Toad!

// has only a tongue jab attack rather than shots it fires
// does DOUBLE damage
// moves faster than normal
// increasing fire range increases the amount of knockback he does

#define PLYR_ACCEL	(FIXAMT*2)
#define PLYR_DECEL	(FIXAMT)
#define PLYR_MAXSPD (FIXAMT*8)

void ToadControlMe(Guy *me,Map *map,mapTile_t *mapTile,world_t *world)
{
	byte c;
	int x,y,d;

	UpdateInterface(player.hearts);

	if(player.reload)
		player.reload--;
	if(player.chatClock)
		player.chatClock--;

	if(player.saveClock)
		player.saveClock--;

	if(player.wpnReload)
		player.wpnReload--;

	if(player.shield)
		player.shield--;

	if(player.invinc)
		player.invinc--;

	if(player.pushPower)
		player.pushPower--;

	HandlePoison(me);

	if(player.stone)
	{
		player.stone--;
		return;
	}

	// ice is slippery
	if(!(world->terrain[mapTile->floor].flags&TF_ICE))
	{
		Dampen(&me->dx,PLYR_DECEL);
		Dampen(&me->dy,PLYR_DECEL);
	}
	else
	{
		if(me->mind1)	// bumped a wall while on ice
		{
			if(me->mind1&1)
				me->dx=-me->dx/8;
			if(me->mind1&2)
				me->dy=-me->dy/8;
		}
	}
	me->mind1=0;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_LOONYOUCH,me->x,me->y,SND_CUTOFF|SND_ONE,2000);
		else if(me->seq==ANIM_DIE)	// so it doesn't do this if you're drowning
			MakeSound(SND_LOONYDIE,me->x,me->y,SND_CUTOFF|SND_ONE,2000);
	}

	// triggering stuff
	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_A1 && me->frm==2 && player.wpnReload==0)
			PlayerFireWeapon(me);

		if(me->seq==ANIM_ATTACK && me->frm>=3 && me->frm<=4 &&  player.reload==0)
		{
			d=player.firePower*3+3;
			if(opt.cheats[CH_HEAVYHIT])
			{
				d+=player.fireRate*3;
				d+=player.fireRange*3;
			}
			bulletHittingType=BLT_BALLLIGHTNING;
			for(c=10;c<70;c+=10)
			{
				x=me->x+Cosine(me->facing*32)*c;
				y=me->y+Sine(me->facing*32)*c;
				if(!opt.cheats[CH_HEAVYHIT])
				{
					if(FindVictims(x>>FIXSHIFT,y>>FIXSHIFT,15,Cosine(me->facing*32)*player.fireRange,
								   Sine(me->facing*32)*player.fireRange,d,map,world))
					{
						// nothing special to do here
						// WHAT ABOUT KNOCKBACK JAMUL
						player.reload=5;
						break;
					}
				}
				else
				{
					if(FindVictims(x>>FIXSHIFT,y>>FIXSHIFT,15,Cosine(me->facing*32)*3,
								   Sine(me->facing*32)*3,d,map,world))
					{
						// nothing special to do here
						player.reload=5;
						break;
					}
				}
			}
		}

		if(me->seq==ANIM_DIE)
		{
			me->facing=(me->facing+1)&7;
		}

		return;
	}

	// not busy, let's see if you want to do something
	c=GetControls();

	DoPlayerFacing(c,me);

	if((c&CONTROL_B1) && player.reload==0)	// pushed hammer throw button
	{
		if(player.worldNum==WORLD_BOWLING)
		{
			ThrowBowlingBall();
			return;
		}
		player.shotsFired++;
		if(!opt.cheats[CH_HEAVYHIT])
		{
			DoMove(me,ANIM_ATTACK,128+player.fireRate*20,1,0,0);
			x=player.fireRate*80;
			me->frm=(x/256);
			me->frmTimer=x%256;
		}
		else
		{
			DoMove(me,ANIM_ATTACK,128,1,0,0);
		}
		player.boredom=0;
		return;
	}
	if((c&CONTROL_B2) && player.wpnReload==0 && player.worldNum!=WORLD_LOONYBALL && player.worldNum!=WORLD_BOWLING)	// pushed wpn use button
	{
		if(player.weapon==ITM_WHOTPANTS || player.weapon==ITM_WLIGHTNING)
		{
			PlayerFireWeapon(me);
		}
		else
		{
			me->action=ACTION_BUSY;
			me->seq=ANIM_A1;
			me->frm=0;
			if(player.weapon==ITM_WCACTUS)
				me->frm=1;
			me->frmTimer=0;
			me->frmAdvance=200;
			player.boredom=0;
			return;
		}
	}

	// if you are moving indeed
	if(c&(CONTROL_UP|CONTROL_DN|CONTROL_LF|CONTROL_RT))
	{
		if(!(world->terrain[mapTile->floor].flags&TF_ICE))
		{
			me->dx+=Cosine(me->facing*32)*PLYR_ACCEL/FIXAMT;
			me->dy+=Sine(me->facing*32)*PLYR_ACCEL/FIXAMT;
			if((Cosine(me->facing*32)<0 && me->dx>0) || (Cosine(me->facing*32)>0 && me->dx<0))
				me->dx=0;
			if((Sine(me->facing*32)<0 && me->dy>0) || (Sine(me->facing*32)>0 && me->dy<0))
				me->dy=0;
		}
		else // ice is slippery
		{
			me->dx+=Cosine(me->facing*32)/4;
			me->dy+=Sine(me->facing*32)/4;
		}

		if(!(world->terrain[mapTile->floor].flags&TF_MUD))
		{
			// on non-mud, everything is fun...
			Clamp(&me->dx,PLYR_MAXSPD);
			Clamp(&me->dy,PLYR_MAXSPD);
		}
		else
		{
			// but on mud, you can BARELY MOVE
			if(player.var[VAR_BOOTS])
			{
				// boots sure help though
				Clamp(&me->dx,PLYR_MAXSPD*3/4);
				Clamp(&me->dy,PLYR_MAXSPD*3/4);
			}
			else
			{
				if(!player.var[VAR_QUESTASSIGN+QUEST_BOOTS])
					BeginChatting(255);	// talk to yourself about boots
				Clamp(&me->dx,FIXAMT/2);
				Clamp(&me->dy,FIXAMT/2);
			}
			SpurtParticles(PART_DIRT,0,me->x,me->y,0,((me->facing*32)+128)&255,8);
			SpurtParticles(PART_DIRT,1,me->x,me->y,0,((me->facing*32)+128)&255,8);
		}

		if(mapTile->item==ITM_MAGICPLANT && mapTile->itemInfo==0 && player.var[VAR_FERTILIZER])
		{
			// begin a-growin'!!
			mapTile->itemInfo=1;
			MakeSound(SND_PLANTGROW,me->x,me->y,SND_CUTOFF,600);
		}
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
		}
		player.boredom=0;
	}
	else	// you aren't trying to move
	{
		// ice is slippery
		if(!(world->terrain[mapTile->floor].flags&TF_ICE))
		{
			Dampen(&me->dx,PLYR_DECEL/2);
			Dampen(&me->dy,PLYR_DECEL/2);
		}
		if(me->seq==ANIM_MOVE)
		{
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}

	// boredom handler
	if(me->action==ACTION_IDLE)
	{
		player.boredom++;
		if(player.boredom>200+(int)Random(3200))
		{
			MakeSound(SND_LOONYBORED,me->x,me->y,SND_CUTOFF|SND_ONE,2000);
			me->seq=ANIM_A2;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=72;
			player.boredom=0;
			if(opt.cheats[CH_VINTAGE])
				NewMessage("Ho hum.",60);
		}
	}

	CallFarley(me);
	SetAreaName(world);
}

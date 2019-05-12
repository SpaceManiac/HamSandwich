#include "ch_girl.h"
#include "quest.h"
#include "options.h"
#include "bowling.h"
#include "badge.h"

#define PLYR_ACCEL	(FIXAMT*2)
#define PLYR_DECEL	(FIXAMT*3/4)
#define PLYR_MAXSPD (FIXAMT*8)

char bombNames[8][16]={
		"",				// no item
		"Explosivo",	// bombs
		"Piercing",		// lightning
		"Paralysis",	// ice wand
		"Shrapnel",		// cactus
		"Homing",		// boomerang
		"Poison",		// whoopee
		"Scorcher",		// hot pants
	};

char *BombName(byte spl)
{

	return bombNames[spl];
}

byte BombCost(byte spl)
{
	byte baseCost[8]={20,20,20,20,20,20,20,20};

	return baseCost[spl];
}

void RenderKnifeUps(MGLDraw *mgl)
{
	int i,w;
	int max;

	for(i=0;i<7;i++)
	{
		if(player.spellXP[i])
		{
			max=(player.firePower+1)*128;
			DrawFillBox(574,20*i+125,638,20*i+143,4*32+4);
			DrawBox(574,20*i+125,638,20*i+143,31);
			w=player.spellXP[i]*64/max;
			if(w>0)
			{
				if(w>63)
					w=63;
				DrawFillBox(575,20*i+126,574+w,20*i+142,4*32+16);
			}
			Print(577,20*i+130,bombNames[i+1],-32,1);
			Print(576,20*i+129,bombNames[i+1],0,1);
		}
	}
}

void GirlFire(Guy *me)
{
	int i;

	FireBullet(me->x+Cosine(me->facing*32)*8,me->y+Sine(me->facing*32)*8,(me->facing*32-4+(byte)Random(9))&255,BLT_THFKNIFE);
	player.reload=11-player.fireRate;

	player.xtraVar-=FIXAMT*4;
	if(player.xtraVar>=(dword)((player.fireRange+1)*32*FIXAMT) || player.xtraVar==0)
	{
		player.xtraVar=0;
		player.xtraByte=1;
	}
	player.shotsFired++;
	for(i=0;i<7;i++)
		if(player.spellXP[i])
		{
			player.specialShotsFired++;
			break;
		}
}

void GirlKnifeUp(Guy *me)
{
	int w,i;

	if(player.weapon==0)
		return;

	w=player.weapon-ITM_WBOMB;
	if(player.money<BombCost(w))
		return;

	player.xtraVar=(player.fireRange+1)*32*FIXAMT;
	player.xtraByte=0;
	player.money-=BombCost(w);
	player.spellXP[w]=(player.firePower+1)*128;

	w=0;
	for(i=0;i<7;i++)
		if(player.spellXP[i])
			w++;

	if(w==7)
		EarnBadge(BADGE_THIEF);
}

void GirlControlMe(Guy *me,mapTile_t *mapTile,world_t *world)
{
	byte c,face,t;
	static byte oldc=255;

	// use up the powerups
	for(t=0;t<8;t++)
	{
		if(player.spellXP[t])
			player.spellXP[t]--;
	}
	UpdateInterface(player.hearts);

	// regenerate some ammo if reloading
	if(player.xtraByte && player.xtraVar<(dword)((player.fireRange+1)*32*FIXAMT))
	{
		player.xtraVar+=(FIXAMT)*(player.fireRange+1);
	}
	if(player.xtraVar>=(dword)((player.fireRange+1)*32*FIXAMT))
	{
		player.xtraVar=(player.fireRange+1)*32*FIXAMT;
		player.xtraByte=0;
	}

	if(me->ouch==4)
	{	// start reloading!
		player.xtraByte=1;
	}

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
			MakeSound(SND_STICKOUCH,me->x,me->y,SND_CUTOFF|SND_ONE,2000);
		else if(me->seq==ANIM_DIE)	// so it doesn't do this if you're drowning
			MakeSound(SND_VILLDIE2,me->x,me->y,SND_CUTOFF|SND_ONE,2000);
	}

	// triggering stuff
	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			me->facing=(me->facing+1)&7;
		}
		return;
	}

	// not busy, let's see if you want to do something
	c=GetControls();

	face=me->facing;
	DoPlayerFacing(c,me);
	if(!(c&(CONTROL_B1)))
		face=me->facing;

	if((c&CONTROL_B1) && player.reload==0 && player.xtraByte==0)	// pushed hammer throw button
	{
		if(player.worldNum==WORLD_BOWLING)
		{
			ThrowBowlingBall();
			return;
		}

		if((c&(CONTROL_UP|CONTROL_DN|CONTROL_LF|CONTROL_RT))==0)
		{
			me->seq=ANIM_ATTACK;
			me->frmTimer=0;
			me->frmAdvance=255;
			me->frm=0;
		}
		player.boredom=0;
		t=me->facing;
		me->facing=face;
		GirlFire(me);
		me->facing=t;
	}
	if((c&CONTROL_B2) &&(!(oldc&CONTROL_B2)) && player.worldNum!=WORLD_LOONYBALL && player.worldNum!=WORLD_BOWLING)	// pushed wpn use button
	{
		// switch bombs
		DoMove(me,ANIM_A1,128,0,me->dx,me->dy);
		GirlKnifeUp(me);
		MakeNormalSound(SND_STICKATTACK);
		player.reload=10;
	}

	// if you are moving indeed
	if(c&(CONTROL_UP|CONTROL_DN|CONTROL_LF|CONTROL_RT))
	{
		if(!(world->terrain[mapTile->floor].flags&TF_ICE))
		{
			me->dx+=Cosine(me->facing*32)*3/2;
			me->dy+=Sine(me->facing*32)*3/2;
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
			// sudden magic boost
			if(player.xtraVar<(dword)((player.fireRange+1)*32*FIXAMT))
			{
				player.xtraVar+=FIXAMT*32;
				if(player.xtraVar>(dword)((player.fireRange+1)*32*FIXAMT))
					player.xtraVar=(player.fireRange+1)*32*FIXAMT;
			}
			MakeSound(SND_VILLAGERYAWN,me->x,me->y,SND_CUTOFF|SND_ONE,2000);
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
	oldc=c;
	me->facing=face;
}

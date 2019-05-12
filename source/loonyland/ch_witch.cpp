#include "ch_witch.h"
#include "quest.h"
#include "options.h"
#include "bowling.h"
#include "badge.h"

#define PLYR_ACCEL	(FIXAMT)
#define PLYR_DECEL	(FIXAMT*3/4)
#define PLYR_MAXSPD (FIXAMT*6)

char spellNames[8][16]={
		"Blast",		// no item
		"Kaboom",		// bombs
		"Shock",		// lightning
		"Ice Bolt",		// ice wand
		"Healing",		// cactus
		"Buzzsaw",		// boomerang
		"Plague",		// whoopee
		"Speed",		// hot pants
	};

char *SpellName(byte spl)
{

	return spellNames[spl];
}

byte SpellCost(byte spl)
{
	byte baseCost[8]={1,10,10,3,3,8,4,8};
	byte addCost[8]= {1,5, 5, 3,5,6,5,3};

	return baseCost[spl]+addCost[spl]*(SpellLevel(spl));
}

byte SpellPct(byte spl)
{
	word spellLevels[10]={0,50,150,500,900,
		1600,2500,4000,6000,10000};
	int i,l;

	l=0;
	for(i=0;i<9;i++)
		if(player.spellXP[spl]<spellLevels[i+1])
		{
			l=i;
			break;
		}
	if(player.spellXP[spl]>spellLevels[8])
		l=9;

	if(l==9)
		return 100;
	else
		return (byte)((player.spellXP[spl]-spellLevels[l])*100/(spellLevels[l+1]-spellLevels[l]));
}

byte SpellLevel(byte spl)
{
	word spellLevels[10]={0,50,150,500,900,
		1600,2500,4000,6000,10000};
	int i;

	for(i=0;i<9;i++)
		if(player.spellXP[spl]<spellLevels[i+1])
			return i;

	return 9;
}

void PowerupSpell(byte spl,byte amt)
{
	byte lvl,i,n;
	char s[32];

	lvl=SpellLevel(spl);
	if(player.spellXP[spl]<30000)
		player.spellXP[spl]+=amt;
	if(SpellLevel(spl)>lvl && player.monsType==MONS_PLYRWITCH)
	{
		MakeNormalSound(SND_POWERUP);
		sprintf(s,"%s Level Up!",SpellName(spl));
		NewBigMessage(s,60);
		n=0;
		for(i=0;i<8;i++)
			if(SpellLevel(i)==9)
				n++;
		if(n==8)
			EarnBadge(BADGE_WITCHCRAFT);
	}
}

void WitchFire(Guy *me)
{
	byte sl;
	int i,amt;

	// need juice to cast
	if(player.xtraVar<(dword)SpellCost(player.xtraByte)*FIXAMT)
	{
		GlassShatter(me->x-10*FIXAMT,me->y-10*FIXAMT,me->x+10*FIXAMT,me->y+10*FIXAMT,FIXAMT*20,10);
		MakeNormalSound(SND_WITCHOUT);
		player.reload=10;
		return;
	}

	player.xtraVar-=SpellCost(player.xtraByte)*FIXAMT;

	sl=SpellLevel(player.xtraByte);

	player.shotsFired++;
	switch(player.xtraByte)
	{
		case SPL_BLAST: // Blast spell
			if(opt.cheats[CH_FROGWPN])
				MakeSound(SND_FROGLICK,me->x,me->y,SND_CUTOFF,1200);
			else
				MakeSound(SND_LOONYSHOOT,me->x,me->y,SND_CUTOFF,1200);
			FireBullet(me->x,me->y,me->facing*32,BLT_WITCHBLAST);
			if(player.fireFlags&FF_TRIPLE)
			{
				FireBullet(me->x,me->y,((me->facing+2)*32)&255,BLT_WITCHBLAST);
				FireBullet(me->x,me->y,((me->facing+6)*32)&255,BLT_WITCHBLAST);
			}
			if(!opt.cheats[CH_HEAVYHIT])
			{
				if(sl<5)
					player.reload=20-sl*2;
				else
					player.reload=11-(sl-5);
			}
			else
				player.reload=20;
			break;
		case SPL_KABOOM:
			FireBullet(me->x,me->y,0,BLT_WITCHKABOOM);
			player.reload=15;
			break;
		case SPL_SHOCK:
			MakeSound(SND_ZAP,me->x,me->y,SND_CUTOFF,1200);
			FireBullet(me->x,me->y,0,BLT_WITCHSHOCK);
			player.reload=30;
			break;
		case SPL_BUZZSAW:
			MakeSound(SND_BUZZSAW,me->x,me->y,SND_CUTOFF,1200);
			amt=256/((sl+1)*2);
			for(i=0;i<(sl+1)*2;i++)
			{
				amt=256*i/((sl+1)*2);
				FireBullet(me->x,me->y,amt,BLT_WITCHBUZZSAW);
			}
			player.reload=60;
			break;
		case SPL_ICE:
			MakeSound(SND_CACTUS,me->x,me->y,SND_CUTOFF,1200);
			amt=sl*3+3;
			for(i=0;i<amt;i++)
			{
				FireBullet(me->x,me->y,me->facing*32+256-64+(byte)Random(129),BLT_WITCHICE);
			}
			player.reload=15;
			break;
		case SPL_PLAGUE:
			MakeSound(SND_SKELGAS,me->x,me->y,SND_CUTOFF,1200);
			FireBullet(me->x,me->y,me->facing*32,BLT_WITCHPLAGUE);
			player.reload=30;
			break;
		case SPL_HEALING:
			player.poison=0;
			if(player.hearts<player.maxHearts)
			{
				player.hearts+=(sl+1);
				i=sl+1;
				if(player.hearts>player.maxHearts)
				{
					i-=(player.hearts-player.maxHearts);
					player.hearts=player.maxHearts;
				}
				goodguy->hp=player.hearts;
				PowerupSpell(SPL_HEALING,(7+sl*3)*i);
			}
			player.invinc=30*sl;
			player.reload=30;
			for(i=0;i<(sl+1)*5;i++)
				SpecialSnow(me->x,me->y);
			MakeSound(SND_MUMMYSUMMON,me->x,me->y,SND_CUTOFF,1200);
			break;
		case SPL_SPEED:
			MakeSound(SND_SPEED,me->x,me->y,SND_CUTOFF,1200);
			FireBullet(me->x,me->y,0,BLT_WITCHSPEED);
			PowerupSpell(SPL_SPEED,10);
			player.reload=60;
			break;
	}
}

void WitchControlMe(Guy *me,mapTile_t *mapTile,world_t *world)
{
	byte c;
	static byte oldc=255;

	UpdateInterface(player.hearts);

	// regenerate some mana
	if(player.xtraVar<(dword)((player.fireRange+1)*32*FIXAMT))
	{
		player.xtraVar+=(player.fireRate+player.firePower+2)*2;
		if(player.xtraVar>(dword)((player.fireRange+1)*32*FIXAMT))
			player.xtraVar=(player.fireRange+1)*32*FIXAMT;
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
			MakeSound(SND_STICKDIE,me->x,me->y,SND_CUTOFF|SND_ONE,2000);
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

	DoPlayerFacing(c,me);

	if((c&CONTROL_B1) && player.reload==0)	// pushed hammer throw button
	{
		if(player.worldNum==WORLD_BOWLING)
		{
			ThrowBowlingBall();
			return;
		}
		me->seq=ANIM_ATTACK;
		me->frmTimer=0;
		me->frmAdvance=255;
		me->frm=0;
		player.boredom=0;
		WitchFire(me);
	}
	if((c&CONTROL_B2) &&(!(oldc&CONTROL_B2)) && player.worldNum!=WORLD_LOONYBALL && player.worldNum!=WORLD_BOWLING)	// pushed wpn use button
	{
		// switch spells
		DoMove(me,ANIM_A1,128,0,me->dx,me->dy);
		MakeNormalSound(SND_STICKATTACK);
		while(1)
		{
			// go to next spell that you DO have
			player.xtraByte++;
			if(player.xtraByte>7)
				player.xtraByte=0;
			if(player.xtraByte==0 || player.var[VAR_WEAPON+player.xtraByte-1])
				break;
		}
		player.reload=10;	// get rid of any excess reload, but induce some extra
							// to prevent manic spell slinging
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
			MakeSound(SND_WITCHBORED,me->x,me->y,SND_CUTOFF|SND_ONE,2000);
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
}

#include "spell.h"
#include "guy.h"
#include "control.h"
#include "game.h"
#include "display.h"
#include "bullet.h"
#include "world.h"
#include "player.h"
#include "particle.h"
#include "fairy.h"
#include "challenge.h"

byte spellCost[20]={
	1,2,	// Energy Barrage/Storm
	2,15,	// Dragon's Flame/Liquify
	7,10,	// Seeker Bolt/Barrage
	10,12,	// Ice Blast/Beam
	25,35,	// Inferno/Hyper Inferno
	20,30,	// Summon
	30,40,	// Stoneskin/Steelskin
	30,45,	// Berserk/Rage
	20,40,	// Healing/Megahealing
	128,128,	// Armageddon
};

byte EnoughMana(void)
{
	byte cost;

	if(player.curSpell==0)
		return 1;
	else
	{
		cost=spellCost[player.curSpell*2+(player.spell[player.curSpell]-1)];
		if(player.fairyOn==FAIRY_CHEAPY)
		{
			cost/=2;	// half-price!
			if(cost==0)
				cost=1;
		}
		if(player.fairyOn==FAIRY_MIGHTY)
		{
			cost=cost*9/10;	// 10% off
			if(cost==0)
				cost=1;
		}

		return (player.mana>=cost);
	}
}

byte SpellLevel(void)
{
	byte lvl;

	lvl=player.level;

	if(player.fairyOn==FAIRY_CASTY)
		lvl+=10;
	if(lvl>50)
		lvl=50;

	return lvl;
}

void CastSpell(Guy *me)
{
	byte c;
	byte cost;
	int i,j;
	static byte flameFlip=0;
	Guy *g;

	if(player.life==0)
		return;	// no shooting when you're dead

	cost=spellCost[player.casting*2+(player.spell[player.casting]-1)];

	if(player.fairyOn==FAIRY_CHEAPY)
	{
		cost/=2;	// half-price!
		if(cost==0)
			cost=1;
	}
	if(player.fairyOn==FAIRY_MIGHTY)
	{
		cost=cost*9/10;	// 10% off
		if(cost==0)
			cost=1;
	}
	if(player.mana-cost<0)
	{
		ExplodeParticles2(PART_HAMMER,me->x,me->y,me->z,4,8);
		ExplodeParticles2(PART_WATER,me->x,me->y,me->z,4,8);
		ExplodeParticles2(PART_YELLOW,me->x,me->y,me->z,4,8);
		ExplodeParticles2(PART_SNOW2,me->x,me->y,me->z,4,8);
		ExplodeParticles2(PART_SLIME,me->x,me->y,me->z,4,8);
		MakeNormalSound(SND_FAILSPELL);
		// not enough mana to cast it!
		return;
	}
	player.mana-=cost;
	ChallengeEvent(CE_SPELL,cost);

	if(player.fairyOn==FAIRY_CASTY && player.life>1)
	{
		GetGoodguy()->hp--;
		player.life--;
	}

	switch(player.casting)
	{
		case 0:	// energy barrage/storm
			if(player.spell[0]==1)
			{
				FireBullet(me->x,me->y,me->facing,BLT_LASER);
				if(SpellLevel()>35)
					FireBullet(me->x,me->y,me->facing,BLT_LASER);
			}
			else	// storm - shoot 8-ish ways
			{
				for(i=0;i<8;i++)
				{
					j=i*32-32+MGL_random(64);
					j=j&255;
					FireExactBullet(me->x,me->y,FIXAMT*20,Cosine(j)*(12+(SpellLevel()/10)),
						Sine(j)*(12+(SpellLevel()/10)),0,0,30,j/16,BLT_LASER);
				}
				MakeNormalSound(SND_BULLETFIRE);
			}

			me->z+=FIXAMT*MGL_random(4);
			me->dx+=FIXAMT/2-MGL_random(65535);
			me->dy+=FIXAMT/2-MGL_random(65535);
			c=GetControls();
			if((c&CONTROL_B2) && (player.mana-cost>=0))	// fire is held
			{
				if(SpellLevel()<25)
				{
					player.wpnReload=1;
					me->frm=2;
					i=12-SpellLevel()/2;
					if(i==0)
						i=1;

					me->frmAdvance=(256*2)/(i);
				}
				else
				{
					player.wpnReload=1;
					me->frmTimer=0;
				}
			}
			else
			{
				player.wpnReload=5;
				flameFlip=0;
			}
			DoPlayerFacing(c,me);
			break;
		case 1:	// dragon's flame, liquify
			if(player.spell[1]==1)
			{
				FireBullet(me->x,me->y,me->facing,BLT_FLAME);
				c=GetControls();
				if(c&CONTROL_B2)	// fire is held
				{
					if(SpellLevel()<25)
					{
						player.wpnReload=1;
						me->frm=2;
						i=12-SpellLevel()/2;
						if(i==0)
							i=1;

						me->frmAdvance=(256*2)/(i);
					}
					else
					{
						player.wpnReload=1;
						me->frmTimer=0;
					}
				}
				else
					player.wpnReload=5;
				DoPlayerFacing(c,me);
			}
			else
			{
				for(i=0;i<SpellLevel()/15+1;i++)
					FireBullet(me->x+Cosine(me->facing*32)*32,me->y+Sine(me->facing*32)*32,me->facing*32,BLT_LIQUIFY);
				MakeNormalSound(SND_FLAMEGO);
				player.wpnReload=5;
			}
			break;
		case 2: // seeker bolt/barrage
			FireBullet(me->x,me->y,me->facing,BLT_MISSILE);
			if(player.spell[2]==2)
			{
				FireBullet(me->x,me->y,(me->facing-1)&7,BLT_MISSILE);
				FireBullet(me->x,me->y,(me->facing+1)&7,BLT_MISSILE);
			}
			if(SpellLevel()<25)
				player.wpnReload=15-(SpellLevel()/2);
			else
				player.wpnReload=2;
			break;
		case 3: // ice blast/beam
			if(player.spell[3]==1)
			{
				FireBullet(me->x,me->y,me->facing,BLT_ICECLOUD);
			}
			else
			{
				FireBullet(me->x,me->y,me->facing*32,BLT_ICEBEAM);
				MakeNormalSound(SND_ICEBEAM);
			}
			player.wpnReload=15;
			break;
		case 4:	// inferno
			if(player.spell[4]==1)
			{
				j=256/(SpellLevel()/4);
				for(i=0;i<256;i+=j)
				{
					c=me->facing*32+i;
					FireExactBullet(me->x,me->y,0,Cosine(c)*4,Sine(c)*4,0,0,i/8+1,0,BLT_BOMB);
				}
			}
			else	// hyper inferno
			{
				for(i=0;i<8;i++)
				{
					FireExactBullet(me->x,me->y,0,Cosine(i*32)*4,Sine(i*32)*4,0,0,8,0,BLT_BOMB);
					FireExactBullet(me->x,me->y,0,Cosine(i*32)*6,Sine(i*32)*6,0,0,16,0,BLT_BOMB);
					FireExactBullet(me->x+Cosine(i*32)*64,me->y+Sine(i*32)*48,0,0,0,0,0,14,i*32,BLT_LIQUIFY);
					FireExactBullet(me->x+Cosine(i*32+16)*64,me->y+Sine(i*32+16)*48,0,0,0,0,0,14,i*32,BLT_LIQUIFY);
					FireExactBullet(me->x+Cosine(i*32)*96,me->y+Sine(i*32)*72,0,0,0,0,0,20,i*32,BLT_LIQUIFY);
					FireExactBullet(me->x+Cosine(i*32+16)*96,me->y+Sine(i*32+16)*72,0,0,0,0,0,20,i*32,BLT_LIQUIFY);
				}
				MakeNormalSound(SND_INFERNAL);
			}
			break;
		case 5: // summon ptero
			if(player.spell[5]==1)
			{
				SetPlayerGlow(64);
				MakeNormalSound(SND_PTEROSUMMON);
				AddGuy(me->x,me->y,FIXAMT*20,MONS_PTERO);
			}
			else
			{
				SetPlayerGlow(64);
				MakeNormalSound(SND_GOLEMPOUND);
				i=me->x+Cosine(me->facing*32)*64;
				j=me->y+Sine(me->facing*32)*48;
				FloaterParticles(i,j,0,20,0,64);
				FloaterParticles(i,j,0,30,0,32);
				FloaterParticles(i,j,0,40,0,32);
				g=AddGuy(i,j,0,MONS_GOLEM);
				if(g && g->CanWalk(g->x,g->y,CurrentMap(),&curWorld))
				{
					g->seq=ANIM_A1;
					g->frmAdvance=128;
					g->action=ACTION_BUSY;
				}
				else
				{
					if(g)
						g->type=MONS_NONE;
					player.mana+=cost;
				}
			}
			player.wpnReload=10;
			break;
		case 6:	// stone/steelskin
			SetPlayerGlow(40);
			if(player.spell[6]==1)
				player.stoneskin+=SpellLevel()*20;
			else
				player.stoneskin+=SpellLevel()*5;
			MakeNormalSound(SND_STONESKIN);
			player.wpnReload=10;
			break;
		case 7:	// berserk
			SetPlayerGlow(40);
			player.berserk+=SpellLevel()*10;
			MakeNormalSound(SND_BERSERK);
			player.wpnReload=10;
			break;
		case 8: // healing
			MakeNormalSound(SND_LOONYKEY);
			if(player.life==player.maxLife)
			{
				player.mana+=cost;
				return;
			}
			SetPlayerGlow(80);
			if(player.spell[8]==1)
			{
				PlayerHeal(SpellLevel());
				ExplodeParticles2(PART_SLIME,me->x,me->y,MGL_randoml(FIXAMT*20),SpellLevel(),6);
			}
			else
			{
				PlayerHeal(128);
				ExplodeParticles2(PART_SLIME,me->x,me->y,MGL_randoml(FIXAMT*20),50,10);
			}
			player.wpnReload=10;
			break;
		case 9: // Armageddon!
			BeginArmageddon();
			break;
	}
}

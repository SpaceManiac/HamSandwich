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

int castCounter;

byte SpellCost(byte spell)
{
	byte cost;

	if (player.downgradeSpell[spell])
		cost=spellCost[spell * 2];
	else
		cost=spellCost[spell * 2 + (player.spell[spell] - 1)];

	if (player.fairyOn == FAIRY_CHEAPY)
	{
		cost /= 2;	// half-price!
		if (cost == 0)
			cost = 1;
	}
	if (player.fairyOn == FAIRY_MIGHTY)
	{
		cost = cost * 9 / 10;	// 10% off
		if (cost == 0)
			cost = 1;
	}
	return cost;
}

byte EnoughMana(void)
{
	byte cost;

	if (player.curSpell == 0)
		return 1;
	else
	{
		cost = SpellCost(player.curSpell);
		return (player.mana >= cost);
	}
}

byte SpellLevel(void)
{
	byte lvl;

	lvl=player.level;

	if(player.fairyOn==FAIRY_CASTY)
		lvl+=10;
	if(lvl>MAX_PLAYERLEVEL)
		lvl=MAX_PLAYERLEVEL;

	return lvl;
}

void ResetCastCounter(void)
{
	castCounter = 0;
}

void CastSpell(Guy *me)
{
	byte c;
	byte cost;
	int i,j;
	Guy *g;
	int fakeLevel;

	if(player.life==0)
		return;	// no shooting when you're dead
	cost = SpellCost(player.casting);

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

	c = GetControls();
	bool spellHeld = (c & CONTROL_B2);
	if (!ClassicMode() && player.enableQuickCast)
	{
		const Uint8* state = SDL_GetKeyboardState(nullptr);
		if (state[SDL_SCANCODE_1 + player.casting])
			spellHeld = true;	// hold down the spell # for quick cast
	}

	switch(player.casting)
	{
		case SPL_ENERGY:	// energy barrage/storm
			if (ClassicMode())
				fakeLevel = SpellLevel();
			else
				fakeLevel = 1 + (SkillValue(SKILL_ENERGYRATE) * 49) / 150;
			if(player.spell[SPL_ENERGY]==1 || player.downgradeSpell[SPL_ENERGY])
			{
				FireBullet(me->x,me->y,me->facing,BLT_LASER);
				castCounter++;
				if (ClassicMode() && fakeLevel > 35)
				{
					FireBullet(me->x, me->y, me->facing, BLT_LASER);
					castCounter++;
				}
			}
			else	// storm - shoot 8-ish ways
			{
				for(i=0;i<8;i++)
				{
					j=i*32-32+MGL_random(64);
					j=j&255;
					FireExactBullet(me->x,me->y,FIXAMT*20,Cosine(j)*(12+(fakeLevel/10)),
						Sine(j)*(12+(fakeLevel/10)),0,0,30,j/16,BLT_LASER);
					castCounter++;
				}
				MakeNormalSound(SND_BULLETFIRE);
			}

			me->z+=FIXAMT*MGL_random(4);
			me->dx+=FIXAMT/2-MGL_random(65535);
			me->dy+=FIXAMT/2-MGL_random(65535);
			if(spellHeld && (player.mana-cost>=0))	// fire is held
			{
				if (fakeLevel < 25)
				{
					player.wpnReload = 1;
					me->frm = 2;
					i = 12 - fakeLevel / 2;
					if (i == 0)
						i = 1;

					me->frmAdvance = (256 * 2) / (i);
				}
				else
				{
					player.wpnReload = 1;
					me->frmTimer = 0;
				}
			}
			else
			{
				player.wpnReload=5;
				if (!ClassicMode())
				{
					int frms = (int)(30.0f * SkillValue(SKILL_ENERGYBARRIER) * (castCounter/10));
					if (player.spell[SPL_ENERGY] == 2 && !player.downgradeSpell[SPL_ENERGY])
						frms /= 4;	// quarter as long for energy storm since it fires WAY more shots
					if (frms > 255) frms = 255;
					if (player.shield < (byte)frms) player.shield = (byte)frms;
				}
			}
			DoPlayerFacing(c,me);
			break;
		case SPL_FLAME:	// dragon's flame, liquify
			if(player.spell[SPL_FLAME]==1 || player.downgradeSpell[SPL_FLAME])
			{
				FireBullet(me->x,me->y,me->facing,BLT_FLAME);
				if(spellHeld)	// fire is held
				{
					if(SpellLevel()<25)
					{
						if (ClassicMode())
						{
							me->frm=2;
							i=12-SpellLevel()/2;
							if(i==0)
								i=1;

							me->frmAdvance=(256*2)/(i);
						}
						else // modern mode is always fast firing, but its damage ticks at an increasing rate with levels
						{
							player.wpnReload = 1;
							me->frmTimer = 5 - SkillValue(SKILL_FLAMEON);
						}
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
				int cnt = SpellLevel() / 15 + 1;
				if (!ClassicMode())
					cnt = (int)((SkillValue(SKILL_FLAMEON)*1.2f)/2 + 1);
				for(i=0;i<cnt;i++)
					FireBullet(me->x+Cosine(me->facing*32)*32,me->y+Sine(me->facing*32)*32,me->facing*32,BLT_LIQUIFY);
				MakeNormalSound(SND_FLAMEGO);
				player.wpnReload=5;
			}
			if (!ClassicMode() && SkillValue(SKILL_BACKDRAFT) > 0)	// reflect nearby bullets
			{
				FloaterParticles(me->x, me->y, 4, SkillValue(SKILL_BACKDRAFT)+16, 0, 10);
				BackdraftEffect(me, SkillValue(SKILL_BACKDRAFT)+16);	// cheating the radius a little to account for kid's pudge
			}
			break;
		case SPL_SEEKER: // seeker bolt/barrage
			FireBullet(me->x,me->y,me->facing,BLT_MISSILE);
			if(player.spell[SPL_SEEKER]==2 && !player.downgradeSpell[SPL_SEEKER])
			{
				FireBullet(me->x,me->y,(me->facing-1)&7,BLT_MISSILE);
				FireBullet(me->x,me->y,(me->facing+1)&7,BLT_MISSILE);
			}
			if (ClassicMode())
			{
				if (SpellLevel() < 25)
					player.wpnReload = 15 - (SpellLevel() / 2);
				else
					player.wpnReload = 2;
			}
			else
			{
				i = SkillValue(SKILL_SEEKER) * 6 / 2;
				if (i > 14) i = 14;
				player.wpnReload = 15 - i;
				if (player.wpnReload < 1)
					player.wpnReload = 1;
			}
			break;
		case SPL_ICE: // ice blast/beam
			if(player.spell[SPL_ICE]==1 || player.downgradeSpell[SPL_ICE])
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
		case SPL_INFERNO:	// inferno
			if (!ClassicMode())
				StunAllOnscreen((byte)(SkillValue(SKILL_SHOCKWAVE) * 30));

			if(player.spell[SPL_INFERNO]==1 || player.downgradeSpell[SPL_INFERNO])
			{
				byte l = SpellLevel() / 4;
				if (!ClassicMode())
					l = 2 + SkillValue(SKILL_INFERNO) * 2;
				if (l < 1) l = 1;
				j=256/l;
				for(i=0;i<256;i+=j)
				{
					c=me->facing*32+i;
					FireExactBullet(me->x,me->y,0,Cosine(c)*4,Sine(c)*4,0,0,i/8+1,0,BLT_BOMB);
					if (!ClassicMode() && Random(100) < SkillValue(SKILL_MAYHEM))
					{
						int x, y;
						x = me->x - HALFWID*FIXAMT + Random(SCRWID)*FIXAMT;
						y = me->y - HALFHEI*FIXAMT + Random(SCRHEI)*FIXAMT;
						FireExactBullet(x, y, 0, 0, 0, 0, 0, i / 8 + 1+Random(10), 0, BLT_BOMB);
					}
				}
			}
			else	// hyper inferno
			{
				int power = SpellLevel();
				if (!ClassicMode())
					power = 5+SkillValue(SKILL_INFERNO) * 9;
				int adv = (32+17-10) - (power / 3);
				i = 0;
				for(i=0;i<256;i+=adv)
				{
					FireExactBullet(me->x,me->y,0,Cosine(i)*4,Sine(i)*4,0,0,8,0,BLT_BOMB);
					FireExactBullet(me->x,me->y,0,Cosine(i)*6,Sine(i)*6,0,0,16,0,BLT_BOMB);
					for(j=0;j<2;j++)
						if (!ClassicMode() && Random(100) < SkillValue(SKILL_MAYHEM))
						{
							int x, y;
							x = me->x - HALFWID * FIXAMT + Random(SCRWID) * FIXAMT;
							y = me->y - HALFHEI * FIXAMT + Random(SCRHEI) * FIXAMT;
							FireExactBullet(x, y, 0, 0, 0, 0, 0, 8+j*8+Random(10), 0, BLT_BOMB);
						}

					if (ClassicMode())
						j = SpellLevel() * 14/50;
					else
						j = 4 + SkillValue(SKILL_INFERNO) * 2;

					FireExactBullet(me->x+Cosine(i)*64,me->y+Sine(i)*48,0,0,0,0,0,j,i,BLT_LIQUIFY2);
					FireExactBullet(me->x+Cosine(i+16)*64,me->y+Sine(i+16)*48,0,0,0,0,0,j,i,BLT_LIQUIFY2);
					j = j * 20 / 14;
					FireExactBullet(me->x+Cosine(i)*96,me->y+Sine(i)*72,0,0,0,0,0,j,i,BLT_LIQUIFY2);
					FireExactBullet(me->x+Cosine(i+16)*96,me->y+Sine(i+16)*72,0,0,0,0,0,j,i,BLT_LIQUIFY2);
				}
				MakeNormalSound(SND_INFERNAL);
			}
			break;
		case SPL_SUMMON: // summon ptero
			if(player.spell[SPL_SUMMON]==1 || player.downgradeSpell[SPL_SUMMON])
			{
				SetPlayerGlow(64);
				MakeNormalSound(SND_PTEROSUMMON);
				AddGuy(me->x,me->y,FIXAMT*20,MONS_PTERO);
				if (!ClassicMode() && SkillValue(SKILL_DISTRACTION) > 0)
					player.taunted = (byte)(30.0f * SkillValue(SKILL_DISTRACTION));
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
					if (!ClassicMode() && SkillValue(SKILL_DISTRACTION) > 0)
						player.taunted = (byte)(30.0f * SkillValue(SKILL_DISTRACTION));
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
		case SPL_ARMOR:	// stone/steelskin
			if(!ClassicMode() && SkillValue(SKILL_PARRY))
			SetPlayerGlow(40);
			if (ClassicMode())
			{
				if (player.spell[SPL_ARMOR] == 1 || player.downgradeSpell[SPL_ARMOR])
					player.stoneskin += SpellLevel() * 20;
				else
					player.stoneskin += SpellLevel() * 5;
			}
			else
			{
				if (player.spell[SPL_ARMOR] == 1 || player.downgradeSpell[SPL_ARMOR])
					player.stoneskin += (int)(SkillValue(SKILL_ARMOR) * 30.0f * 5.0f / 100.0f);
				else
					player.stoneskin += (int)(SkillValue(SKILL_ARMOR) * 50.0f / 100.0f);
				if (SkillValue(SKILL_HEALSUMMONS) > 0)
					player.summonDmgBoost = 5 * 30;
				if (SkillValue(SKILL_PARRY) > 0)
				{
					player.parry = 15;
					player.shield = 15;
				}
			}
			MakeNormalSound(SND_STONESKIN);
			player.wpnReload=10;
			break;
		case SPL_BERSERK:	// berserk
			SetPlayerGlow(40);
			if (ClassicMode())
				player.berserk += SpellLevel() * 10;
			else
			{
				player.berserk += (int)(SkillValue(SKILL_BERSERK) * 60);
				if (player.berserk > (int)SkillValue(SKILL_BERSERK) * 60 * 2)
					player.berserk = (int)SkillValue(SKILL_BERSERK) * 60 * 2;
			}
			MakeNormalSound(SND_BERSERK);
			player.wpnReload=10;
			break;
		case SPL_HEAL: // healing
			MakeNormalSound(SND_LOONYKEY);
			if(player.life==player.maxLife && (ClassicMode() || SkillValue(SKILL_HEALSUMMONS)==0))
			{
				player.mana+=cost;
				return;
			}
			
			SetPlayerGlow(80);
			if(player.spell[SPL_HEAL]==1 || player.downgradeSpell[SPL_HEAL])
			{
				if (ClassicMode())
					PlayerHeal(SpellLevel());
				else
				{
					int heal = SkillValue(SKILL_HEAL) * 30.0f / 100.0f;
					if(SkillValue(SKILL_HEALSUMMONS) > 0)
						HealSummons(heal);
					if (SkillValue(SKILL_RESTORATION) > 0)
						AddToRestorationBuffer((float)heal);
					else
						PlayerHeal(heal);
				}
				ExplodeParticles2(PART_SLIME,me->x,me->y,MGL_randoml(FIXAMT*20),SpellLevel(),6);
			}
			else
			{
				if(ClassicMode())
					PlayerHeal(128);
				else
				{
					int heal = SkillValue(SKILL_HEAL) * 64.0f / 100.0f;
					if (SkillValue(SKILL_HEALSUMMONS) > 0)
						HealSummons(heal);
					if (SkillValue(SKILL_RESTORATION) > 0)
						AddToRestorationBuffer((float)heal);
					else
						PlayerHeal(heal);
					player.poison = 0;
				}
				ExplodeParticles2(PART_SLIME,me->x,me->y,MGL_randoml(FIXAMT*20),50,10);
			}
			player.wpnReload=10;
			break;
		case SPL_ARMAGEDDON: // Armageddon!
			if(ClassicMode())
				BeginArmageddon();
			else
			{
				if (SkillValue(SKILL_MANAGETTIN) > 0 && ArmageddonIsUnderway())
					MakeNormalSound(SND_FAILSPELL);	// no recasting dude!
				else
					BeginArmageddon();
			}
			break;
	}
}

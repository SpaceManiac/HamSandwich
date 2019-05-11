#include "ch_loony.h"
#include "quest.h"
#include "options.h"
#include "skill.h"
#include "spellpick.h"
#include "alchemy.h"
#include "leveldef.h"
#include "glasses.h"
#include "madcap.h"
#include "achieve.h"

#define PLYR_ACCEL	(FIXAMT*3/2)
#define PLYR_DECEL	(FIXAMT*3/4)
#define PLYR_MAXSPD (FIXAMT*6)

#define MAGIC_RATE	(30*3)
#define STAMINA_RATE	(20)

void LoonyFire(Guy *me)
{
	word staCost,magCost;

	if(me->z>0 && player.axeMode==1 && SpecificSkillVal(0,SKILL_AIRSTRIKE))	// trying to airstrike
	{
		if(player.stamina>=SpecificSkillCost(0,SKILL_AIRSTRIKE))
		{
			player.stamina-=(byte)SpecificSkillCost(0,SKILL_AIRSTRIKE);
			TalentPoint(TLT_VIVACITY,(byte)SpecificSkillCost(0,SKILL_AIRSTRIKE));
			DoMove(me,ANIM_A4,512,1,me->dx,me->dy);
			player.boredom=0;
			player.shotsFired++;
			FireBullet(me->ID,me->x,me->y,FIXAMT*20+me->z,me->facing*32,BLT_BOOMAXE,DamageAmt(SpecificSkillVal(0,SKILL_AIRSTRIKE),GOOD));
			player.reload=10;
			return;
		}
	}

	staCost=StaminaCost();
	magCost=MagicCost();

	if(ModifierOn(MOD_TIRELESS))
	{
		staCost=0;
		magCost=1;
	}

	if(player.stamina<staCost || player.magic<magCost)
	{
		// fail effect somehow?
		return;
	}

	player.stamina-=staCost;
	TalentPoint(TLT_VIVACITY,(byte)staCost);
	player.magic-=magCost;

	DoMove(me,ANIM_ATTACK,64+player.axeSpeed*18,1,me->dx,me->dy);
	player.boredom=0;

	if(me->z>0)	// in the air
	{
		me->seq=ANIM_A4;
		me->frmAdvance=128;
	}
	player.shotsFired++;

	if(player.axe.magic==EM_GHOSTLY)
	{
		Guy *g;
		g=AddGuy(me->x+Cosine(me->facing*32)*30-FIXAMT*20+(rand()%(FIXAMT*40+1)),me->y+Sine(me->facing*32)*30-FIXAMT*20+(rand()%(FIXAMT*40+1)),me->z+FIXAMT*20,MONS_GHOST);
		if(g)
			g->mind2=2;
	}

	if(player.axeMode==1)
	{
		bullet_t *b;

		me->frmAdvance=512;

		b=FireBullet(me->ID,me->x,me->y,FIXAMT*20+me->z,me->facing*32,BLT_AXE,bulDef[BLT_AXE].damage);
		if(me->z>0 && b)	// in the air
		{
			b->dz=-FIXAMT*3;
		}
		if(SpecificSkillVal(0,SKILL_TRIPLE))
		{
			b=FireBullet(me->ID,me->x,me->y,FIXAMT*20+me->z,me->facing*32+28,BLT_MINIAXE,bulDef[BLT_MINIAXE].damage);
			if(me->z>0 && b)	// in the air
			{
				b->dz=-FIXAMT*3;
			}
			b=FireBullet(me->ID,me->x,me->y,FIXAMT*20+me->z,me->facing*32-28,BLT_MINIAXE,bulDef[BLT_MINIAXE].damage);
			if(me->z>0 && b)	// in the air
			{
				b->dz=-FIXAMT*3;
			}
		}
		player.reload=31-player.axeSpeed*5/4;
		if(player.reload>31)
			player.reload=0;
		MakeSound(SND_LOONYSWING,me->x,me->y,SND_CUTOFF|SND_RANDOM,1200);
	}
	else
	{
		player.reload=0;
		player.parry=(byte)SpecificSkillVal(0,SKILL_PARRY);
		if(player.axe.magic==EM_FLAMING || player.axe.magic==EM_SCORCHING)
		{
			// axe fireballs do half the damage the axe does (ignoring other bonuses like Strength skill)
			staCost=DamageAmt(player.axe.value,GOOD)/2;
			if(staCost==0)
				staCost=1;
			FireBullet(me->ID,me->x,me->y,FIXAMT*20+me->z,me->facing*32-10+(rand()%21),BLT_PFIREBALL,staCost);
			if(player.axe.magic==EM_SCORCHING)
			{
				FireBullet(me->ID,me->x,me->y,FIXAMT*20+me->z,me->facing*32-16-10+(rand()%21),BLT_PFIREBALL,staCost);
				FireBullet(me->ID,me->x,me->y,FIXAMT*20+me->z,me->facing*32+16-10+(rand()%21),BLT_PFIREBALL,staCost);
			}
		}
		MakeSound(SND_LOONYSWING,me->x,me->y,SND_CUTOFF|SND_RANDOM,1200);
	}
}

void LoonyHitGuy(Guy *me,int n,world_t *world)
{
	int dx,dy;
	int amt;
	word dmg;
	int x,y;
	byte nebrogub;
	byte ang;

	nebrogub=0;
	if(player.axe.magic==EM_NEBROGUB)
		nebrogub=rand()%4;

	if(me->z>0)
		amt=2+(int)SpecificSkillVal(0,SKILL_BASH);
	else
		amt=2;
	if(player.axe.magic==EM_SMASHING)
		amt+=2;
	if(GlassEffect()==GE_KNOCK)
		amt+=GlassAmount();

	ang=AngleFrom(me->x,me->y,GetGuy((word)(n-1))->x,GetGuy((word)(n-1))->y);
	dx=Cosine(ang)*amt;
	dy=Sine(ang)*amt;

	dmg=player.damage+(me->z>0); // +1 damage for air attacks
	if(me->z>0)
		dmg+=(int)SpecificSkillVal(1,SKILL_BASH);
	if(player.potionType==POT_MUDMILK)
		dmg+=PotionEffect(player.potionType,player.potionPower);
	if((rand()%100)<SpecificSkillVal(0,SKILL_CRITICAL)+1)
	{
		// add sparkling critical stuff and noise and all
		MakeNormalSound(SND_CRITICAL);
		FireBullet(me->ID,GetGuy(n-1)->x,GetGuy(n-1)->y+FIXAMT,FIXAMT*30,0,BLT_CRITICAL,0);
		dmg=(dmg*100+dmg*(word)SpecificSkillVal(1,SKILL_CRITICAL))/100;
		isCritical=1;
		TalentPoint(TLT_CRITICISM,1);
	}

	if(SpecificSkillVal(0,SKILL_FLAME)>0 && me->z>0)
	{
		if(!player.var[VAR_FLAMEPAID] && player.magic>=SpecificSkillCost(1,SKILL_FLAME))
		{
			player.magic-=(byte)SpecificSkillCost(1,SKILL_FLAME);
			player.var[VAR_FLAMEPAID]=1;
		}
		if(player.var[VAR_FLAMEPAID])
		{
			x=GetGuy((word)(n-1))->x;
			y=GetGuy((word)(n-1))->y;
			x=x-FIXAMT*30+(rand()%(FIXAMT*60+1));
			y=y-FIXAMT*20+(rand()%(FIXAMT*40+1));
			FireBullet(me->ID,x,y,0,0,BLT_FLAME,CurseDmg(1,GOOD));
		}
	}
	if((player.axe.magic==EM_VAMPIRIC && (rand()%100)<5) || (player.axe.magic==EM_VAMPIRIC2 && (rand()%100)<10))
	{
		HealGoodguy(RecoverAmt(1));
	}
	if((rand()%100)<SpecificSkillVal(0,SKILL_VAMPIRE))
	{
		HealGoodguy(RecoverAmt(1));
	}

	if(nebrogub)
	{
		dmg=rand()%(dmg*2+1);
	}

	if(player.amulet.magic==EM_IMAJICA)
	{
		dmg/=2;
		if(dmg<1)
			dmg=1;
	}

	meleeAttacker=goodguy;
	GetGuy((word)(n-1))->GetShot(dx,dy,(word)DamageAmt(dmg,GOOD),curMap,world);
	if(player.axe.magic==EM_NUCULAR)
	{
		MakeSound(SND_BOOM,GetGuy((word)(n-1))->x,GetGuy((word)(n-1))->y,SND_CUTOFF|SND_RANDOM,1200);
		FireBullet(me->ID,GetGuy((word)(n-1))->x,GetGuy((word)(n-1))->y,0,0,BLT_KABOOM,DamageAmt(10,GOOD));
	}
	isCritical=0;
	x=SkillStunAmount();
	if(player.axe.magic==EM_SMASHING)
		x+=5;
	if(GlassEffect()==GE_STUN)
		x+=GlassAmount();

	if(nebrogub==1)
		x+=10;

	GetGuy((word)(n-1))->GetStunned(x);

	if(player.axe.magic==EM_POISONED)
		GetGuy((word)(n-1))->GetPoisoned(2*30);
	if(nebrogub==2)
		GetGuy((word)(n-1))->GetPoisoned(1*30);
	if(player.axe.magic==EM_POISON2)
		GetGuy((word)(n-1))->GetPoisoned(5*30);
	if(player.axe.magic==EM_FROSTY)
		GetGuy((word)(n-1))->GetFrozen(30);
	if(player.axe.magic==EM_FREEZING)
		GetGuy((word)(n-1))->GetFrozen(30*2);
	if(nebrogub==3)
		GetGuy((word)(n-1))->GetFrozen(15);

	if(GetGuy((word)(n-1))->hp>0 && me->z>0 && !(monsType[GetGuy((word)(n-1))->type].location&ML_NOMIND) &&
		((rand()%100)<SpecificSkillVal(0,SKILL_PERSUADE) || player.axe.magic==EM_PERSUADER))
	{
		if(player.magic>=SpecificSkillCost(1,SKILL_PERSUADE))
		{
			amt=(int)SpecificSkillVal(1,SKILL_PERSUADE)-monsType[GetGuy((word)(n-1))->type].level*10;
			if(amt>0)
			{
				MakeNormalSound(SND_PERSUADE);
				if(monsType[GetGuy((word)(n-1))->type].location&ML_BOSS)
					amt/=2;	// bosses get half as much
				GetGuy((word)(n-1))->stun=1;
				GetGuy((word)(n-1))->team=1;
				GetGuy((word)(n-1))->mindControl=(word)amt;
				player.magic-=(byte)SpecificSkillCost(1,SKILL_PERSUADE);
			}
		}
	}
	if(player.axe.magic==EM_THORNY)
	{
		bullet_t *b;
		int i;

		for(i=0;i<2;i++)
		{
			b=FireBullet(me->ID,GetGuy((word)(n-1))->x,GetGuy((word)(n-1))->y,GetGuy((word)(n-1))->z+FIXAMT*20,rand()%256,BLT_AXESHARD,bulDef[BLT_AXESHARD].damage);
			if(b)
				b->canthit=n;
		}
	}
}

void LoonyHitGuyThrow(float factor,word dmg,Guy *me,int n,world_t *world)
{
	int dx,dy;
	int amt;
	word poison;
	byte nebrogub;
	byte ang;

	nebrogub=0;
	if(player.axe.magic==EM_NEBROGUB)
		nebrogub=rand()%4;

	amt=(int)(2*factor);
	if(player.axe.magic==EM_SMASHING)
		amt+=2;
	amt+=(int)(TalentBonus(TLT_TOSSING)*factor);

	if(GlassEffect()==GE_KNOCK)
		amt+=GlassAmount();

	if(player.potionType==POT_MUDMILK)
		dmg+=(word)(PotionEffect(player.potionType,player.potionPower)*factor);

	ang=AngleFrom(me->x,me->y,GetGuy((word)(n-1))->x,GetGuy((word)(n-1))->y);
	dx=Cosine(ang)*amt;
	dy=Sine(ang)*amt;

	if(nebrogub)
	{
		dmg=rand()%(dmg*2+1);
	}

	if(player.amulet.magic==EM_IMAJICA)
	{
		dmg/=2;
		if(dmg<1)
			dmg=1;
	}

	meleeAttacker=NULL;
	GetGuy((word)(n-1))->GetShot(dx,dy,DamageAmt(dmg,GOOD),curMap,world);
	if(player.axe.magic==EM_NUCULAR)
	{
		MakeSound(SND_BOOM,GetGuy((word)(n-1))->x,GetGuy((word)(n-1))->y,SND_CUTOFF|SND_RANDOM,1200);
		FireBullet(me->ID,GetGuy((word)(n-1))->x,GetGuy((word)(n-1))->y,0,0,BLT_KABOOM,DamageAmt(10,GOOD));
	}

	amt=0;
	if(player.axe.magic==EM_SMASHING)
		amt+=5;

	if(GlassEffect()==GE_STUN)
		amt+=GlassAmount();

	if(nebrogub==1)
		amt+=10;

	if(amt>0)
		GetGuy((word)(n-1))->GetStunned(amt);

	poison=(word)(SpecificSkillVal(0,SKILL_POISON)*factor);
	if(player.axe.magic==EM_POISONED)
		poison+=2*30;
	if(player.axe.magic==EM_POISON2)
		poison+=5*30;
	if(nebrogub==2)
		poison+=30;
	GetGuy((word)(n-1))->GetPoisoned(poison);
	if(player.axe.magic==EM_FROSTY)
		GetGuy((word)(n-1))->GetFrozen(30);
	if(player.axe.magic==EM_FREEZING)
		GetGuy((word)(n-1))->GetFrozen(30*2);
	if(nebrogub==3)
		GetGuy((word)(n-1))->GetFrozen(15);
	if((player.axe.magic==EM_VAMPIRIC && (rand()%100)<5) || (player.axe.magic==EM_VAMPIRIC2 && (rand()%100)<10))
	{
		HealGoodguy(RecoverAmt(1));
	}
}

void LoonyWhirlHit(Guy *me,world_t *world)
{
	int dx,dy,x,y;
	word dmg;
	int id;
	byte ang;

	id=-1;
	while(id!=0)
	{
		if(id==-1)
			id=0;
		id=FindVictim(me->x,me->y,me->mapx,me->mapy,48+48*(player.axe.magic==EM_WHIRLING),0,EVIL,id,curMap,world);
		if(id)
		{
			ang=AngleFrom(me->x,me->y,GetGuy((word)(id-1))->x,GetGuy((word)(id-1))->y);
			dx=Cosine(ang)*6;
			dy=Sine(ang)*6;
			dmg=player.damage;
			if(player.potionType==POT_MUDMILK)
				dmg+=(word)(PotionEffect(player.potionType,player.potionPower));

			if(player.axe.magic==EM_NEBROGUB)
			{
				dmg=rand()%(dmg*2+1);
			}
			if(player.amulet.magic==EM_IMAJICA)
			{
				dmg/=2;
				if(dmg<1)
					dmg=1;
			}
			meleeAttacker=goodguy;
			GetGuy((word)(id-1))->GetShot(dx,dy,DamageAmt(dmg,GOOD),curMap,world);
			if(SpecificSkillVal(0,SKILL_FLAME)>0)
			{
				if(!player.var[VAR_FLAMEPAID] && player.magic>=SpecificSkillCost(1,SKILL_FLAME))
				{
					player.magic-=(byte)SpecificSkillCost(1,SKILL_FLAME);
					player.var[VAR_FLAMEPAID]=1;
				}
				if(player.var[VAR_FLAMEPAID])
				{
					x=GetGuy((word)(id-1))->x;
					y=GetGuy((word)(id-1))->y;
					x=x-FIXAMT*30+(rand()%(FIXAMT*60+1));
					y=y-FIXAMT*20+(rand()%(FIXAMT*40+1));
					FireBullet(me->ID,x,y,0,0,BLT_FLAME,CurseDmg(1,GOOD));
				}
			}
		}
	}
}

byte PlaceBramble(int x,int y,int cx,int cy,int value,Map *map)
{
	if(map->map[x+y*map->width].item==0 && map->map[x+y*map->width].wall==0)
	{
		if(Distance(x*FIXAMT,y*FIXAMT,cx*FIXAMT,cy*FIXAMT)<=(int)(SpecificSkillVal(0,SKILL_BRAMBLES)*FIXAMT) && (rand()%3)==0)
		{
			bullet_t *b;
			int xx,yy;

			xx=(x*TILE_WIDTH+(rand()%TILE_WIDTH))*FIXAMT;
			yy=(y*TILE_HEIGHT+(rand()%TILE_HEIGHT))*FIXAMT;
			b=FireBullet(goodguy->ID,xx,yy,0,0,BLT_BRAMBLE1,0);
		}
	}
	return 1;
}

float SpellDamageBoost(byte school)
{
	float dmgBoost;
	int i,start;
	byte b;

	dmgBoost=100.0f+TalentBonus(TLT_FOCUS)+player.amulet.speed*5;
	if(player.amulet.magic==EM_MIGHTY)
		dmgBoost+=20.0f;
	if(player.amulet.magic==EM_IMAJICA)
		dmgBoost+=50.0f;
	if(GlassEffect()==GE_MAGICDAM)
		dmgBoost+=(float)GlassAmount();

	b=0;
	if(school==SC_NATURE)	// nature
		start=SKILL_SHROOM;
	if(school==SC_FIRE)	// fire
		start=SKILL_BURNINATE;
	if(school==SC_WIND)	// wind
		start=SKILL_SHOCK;
	if(school==SC_ICE)	// ice
		start=SKILL_RESTORE;
	if(school==SC_DEATH)	// death
		start=SKILL_DRAIN;

	for(i=start;i<start+4;i++)
		b+=SpecificSkillLevel(i);

	dmgBoost+=(float)b;

	if(player.axe.magic==EM_JORD)
		dmgBoost*=0.5f;

	if(player.var[VAR_MADCAP])
	{
		switch(school)
		{
			case SC_FIRE:
				i=VAR_MADGEM+MC_FIRE;
				break;
			case SC_ICE:
				i=VAR_MADGEM+MC_ICE;
				break;
			case SC_NATURE:
				i=VAR_MADGEM+MC_NATURE;
				break;
			case SC_DEATH:
				i=VAR_MADGEM+MC_DEATH;
				break;
			case SC_WIND:
				i=VAR_MADGEM+MC_WIND;
				break;
		}

		dmgBoost=(dmgBoost*(100.0f+(float)player.var[i]*10.0f)/100.0f);
	}
	return dmgBoost;
}

void CastSpell(Guy *me)
{
	int i,x,y;
	byte sp;
	Guy *g;
	word w;
	bullet_t *b;

	sp=player.spell;
	if(SpecificSkillLevel(sp)==0 || player.magic<SpecificSkillCost(1,sp))
	{
		// you don't have this spell or lack the magic needed
		// effects please
		ParticleBoom(PART_WHITEPOP,me->x,me->y,FIXAMT*30);
		MakeNormalSound(SND_MAGICFAIL);
		return;
	}

	player.magic-=(byte)SpecificSkillCost(1,sp);
	if(curWorld.terrain[curMap->map[me->mapx+me->mapy*curMap->width].floor].flags&TF_ICE)
		DoMove(me,ANIM_A1,128,1,me->dx,me->dy);
	else
		DoMove(me,ANIM_A1,128,1,0,0);

	if(player.amulet.magic==EM_SOLID && (rand()%100)<10)
	{
		ParticleBoom(PART_WHITEPOP,me->x,me->y,FIXAMT*30);
		MakeNormalSound(SND_MAGICFAIL);
		return;
	}
	if(player.amulet.magic==EM_FRIGID && sp>=SKILL_RESTORE && sp<=SKILL_ICECUBE)
	{
		FreezeAllEnemies();
	}
	if(sp>=SKILL_BURNINATE && sp<=SKILL_HEATSHIELD)	// add Scorching talent fireballs if needed
	{
		for(i=0;i<(int)TalentBonus(TLT_SCORCHING);i++)
		{
			b=FireBullet(me->ID,me->x,me->y,FIXAMT*20,rand()%256,BLT_PFIREBALL2,(word)(1.0f*SpellDamageBoost(SC_FIRE)/100.0f));
		}
	}
	player.specialShotsFired++;

	switch(sp)
	{
		case SKILL_RESTORE:
			HealGoodguy((byte)(SpecificSkillVal(0,SKILL_RESTORE)*SpellDamageBoost(SC_ICE)/100.0f));
			player.invinc=(byte)SpecificSkillVal(1,SKILL_RESTORE);
			me->poison=0;
			me->frozen=0;
			MakeColdRingParticle(me->x,me->y,0,64);
			DestroyBullets(me->x,me->y,2,100*FIXAMT);
			MakeNormalSound(SND_BIGHEAL);
			break;
		case SKILL_BONEHEAD:
			// summon bonehead
			g=SummonGuy(me->x,me->y,FIXAMT*64,MONS_BONEHEAD);
			if(!g)
			{
				// refund for failing to summon
				MakeNormalSound(SND_MAGICFAIL);
				ParticleBoom(PART_WHITEPOP,me->x,me->y,FIXAMT*30);
				player.magic+=(byte)SpecificSkillCost(1,sp);
				return;
			}
			else
			{
				MakeNormalSound(SND_SKELSUMMON);
				if(GuyCount(MONS_BONEHEAD)>(int)SpecificSkillVal(0,SKILL_BONEHEAD))
					KillWeakest(MONS_BONEHEAD);
				DoMove(g,ANIM_A1,128,1,0,0);
				MakeRingParticle(g->x,g->y,0,32,32);
			}
			break;
		case SKILL_BONEBOMB:
			w=GuyList_First();
			while(w!=65535)
			{
				if(GetGuy(w)->type==MONS_BONEHEAD && GetGuy(w)->hp)
				{
					MakeNormalSound(SND_BONEBOMB);
					FireBullet(goodguy->ID,GetGuy(w)->x,GetGuy(w)->y,0,0,BLT_BONEBOOM,(word)(SpecificSkillVal(0,SKILL_BONEBOMB)*(SpellDamageBoost(SC_DEATH))/100.0f));
					GetGuy(w)->hp=1;
					meleeAttacker=NULL;
					GetGuy(w)->GetShotReal(0,0,9999,curMap,&curWorld);
				}

				w=GuyList_Next();
			}
			break;
		case SKILL_SHROOM:
			w=GuyList_First();
			while(w!=65535)
			{
				if(GetGuy(w)->team!=me->team && GetGuy(w)->hp && !(monsType[GetGuy(w)->type].flags&(MF_INVINCIBLE|MF_NOHIT)) && !(monsType[GetGuy(w)->type].location&(ML_BOSS|ML_NOMIND)) && (GetGuy(w)->type!=MONS_SHROOM))
				{
					if(Distance(me->x,me->y,GetGuy(w)->x,GetGuy(w)->y)<(int)SpecificSkillVal(1,SKILL_SHROOM)*FIXAMT)
					{
						MakeSound(SND_SHROOMIFY,GetGuy(w)->x,GetGuy(w)->y,SND_CUTOFF|SND_RANDOM,200);
						GetGuy(w)->originalType=GetGuy(w)->type;
						GetGuy(w)->shroomTime=(word)SpecificSkillVal(0,SKILL_SHROOM);
						GetGuy(w)->type=MONS_SHROOM;
						DoMove(GetGuy(w),ANIM_IDLE,128,0,0,0);
						MakeRingParticle(GetGuy(w)->x,GetGuy(w)->y,0,32,32);
					}
				}

				w=GuyList_Next();
			}
			if(TalentBonus(TLT_GREENTHUMB)>0)
				HealGoodguy(RecoverAmt((word)(0.5f+TalentBonus(TLT_GREENTHUMB))));
			if(!levelDef[player.levelNum].flags&LF_TOWN)
				TalentPoint(TLT_GREENTHUMB,1);
			break;
		case SKILL_SHOCK:
			x=me->x+Cosine(me->facing*32)*24;
			y=me->y+Sine(me->facing*32)*24;
			FireBullet(goodguy->ID,x,y,FIXAMT*20,me->facing*32,BLT_SHOCKORB,(word)(SpecificSkillVal(0,SKILL_SHOCK)*(SpellDamageBoost(SC_WIND))/100.0f));
			if(!levelDef[player.levelNum].flags&LF_TOWN)
				TalentPoint(TLT_WINDBAG,1);
			break;
		case SKILL_GALE:
			// launch off a wall of Gale shots
			MakeNormalSound(SND_GALE);
			for(i=0;i<8;i++)
			{
				x=me->x+Cosine(me->facing*32+64)*i*24;
				y=me->y+Sine(me->facing*32+64)*i*24;
				FireBullet(goodguy->ID,x,y,0,me->facing*32,BLT_GALE1,0);
				x=me->x+Cosine(me->facing*32+64+128)*i*24;
				y=me->y+Sine(me->facing*32+64+128)*i*24;
				FireBullet(goodguy->ID,x,y,0,me->facing*32,BLT_GALE1,0);
			}
			if(!levelDef[player.levelNum].flags&LF_TOWN)
				TalentPoint(TLT_WINDBAG,1);
			break;
		case SKILL_BERSERK:
			MakeNormalSound(SND_BERSERK);
			player.berserkClock=(word)SpecificSkillVal(0,SKILL_BERSERK);
			if(!levelDef[player.levelNum].flags&LF_TOWN)
				TalentPoint(TLT_WINDBAG,1);
			break;
		case SKILL_TORNADO:
			// launch off a wall of Gale shots
			MakeNormalSound(SND_TORNADO);
			FireBullet(me->ID,me->x,me->y,0,me->facing*32,BLT_TORNADO,0);
			if(!levelDef[player.levelNum].flags&LF_TOWN)
				TalentPoint(TLT_WINDBAG,1);
			break;
		case SKILL_SPIKE:
			// stone spike
			for(i=0;i<SpecificSkillVal(0,SKILL_SPIKE);i++)
			{
				x=me->facing*32-8*(int)SpecificSkillVal(0,SKILL_SPIKE)+(rand()%((int)(16*((int)SpecificSkillVal(0,SKILL_SPIKE))+1)));
				FireBullet(me->ID,me->x,me->y,0,x,BLT_STONESPIKE,(word)(SpecificSkillVal(1,SKILL_SPIKE)*(SpellDamageBoost(SC_NATURE))/100.0f));
			}
			MakeNormalSound(SND_STONERUMBLE);
			if(TalentBonus(TLT_GREENTHUMB)>0)
				HealGoodguy(RecoverAmt((word)(0.5f+TalentBonus(TLT_GREENTHUMB))));
			if(!levelDef[player.levelNum].flags&LF_TOWN)
				TalentPoint(TLT_GREENTHUMB,1);
			break;
		case SKILL_BRAMBLES:
			curMap->LOS(me->mapx,me->mapy,30,0,PlaceBramble);
			MakeNormalSound(SND_PLANTSURFACE);
			if(TalentBonus(TLT_GREENTHUMB)>0)
				HealGoodguy(RecoverAmt((word)(0.5f+TalentBonus(TLT_GREENTHUMB))));
			if(!levelDef[player.levelNum].flags&LF_TOWN)
				TalentPoint(TLT_GREENTHUMB,1);
			break;
		case SKILL_CRYOZOID:
			// summon cryozoid
			g=SummonGuy(me->x,me->y,FIXAMT*64,MONS_CRYOZOID);
			if(!g)
			{
				// refund for failing to summon
				MakeNormalSound(SND_MAGICFAIL);
				ParticleBoom(PART_WHITEPOP,me->x,me->y,FIXAMT*30);
				player.magic+=(byte)SpecificSkillCost(1,sp);
				return;
			}
			else
			{
				MakeNormalSound(SND_CRYOZOID);
				DoMove(g,ANIM_A1,128,1,0,0);
				g->mind=(byte)SpecificSkillVal(0,SKILL_CRYOZOID);
				g->mind1=30;
			}
			break;
		case SKILL_COLDWAVE:
			// launch off a set of cold shots
			MakeNormalSound(SND_GALE);
			for(i=1;i<4;i++)
			{
				x=me->x+Cosine(me->facing*32+i*20)*30;
				y=me->y+Sine(me->facing*32+i*20)*30;
				FireBullet(goodguy->ID,x,y,FIXAMT*20,me->facing*32,BLT_COLDWAVE,(word)(SpecificSkillVal(0,SKILL_COLDWAVE)*(SpellDamageBoost(SC_ICE))/100.0f));
				x=me->x+Cosine(me->facing*32+256-i*20)*30;
				y=me->y+Sine(me->facing*32+256-i*20)*30;
				FireBullet(goodguy->ID,x,y,FIXAMT*20,me->facing*32,BLT_COLDWAVE,(word)(SpecificSkillVal(0,SKILL_COLDWAVE)*(SpellDamageBoost(SC_ICE))/100.0f));
			}
			x=me->x+Cosine(me->facing*32)*30;
			y=me->y+Sine(me->facing*32)*30;
			FireBullet(goodguy->ID,x,y,FIXAMT*20,me->facing*32,BLT_COLDWAVE,(word)(SpecificSkillVal(0,SKILL_COLDWAVE)*(SpellDamageBoost(SC_ICE))/100.0f));
			break;
		case SKILL_ICECUBE:
			MakeNormalSound(SND_GALE);
			x=me->x+Cosine(me->facing*32)*(int)SpecificSkillVal(1,SKILL_ICECUBE);
			y=me->y+Sine(me->facing*32)*(int)SpecificSkillVal(1,SKILL_ICECUBE);
			FireBullet(goodguy->ID,x,y,FIXAMT*180,me->facing*32,BLT_BLIZZARD,(word)((SpecificSkillLevel(SKILL_ICECUBE))*(SpellDamageBoost(SC_ICE))/100.0f));
			break;
		case SKILL_IGNITE:
			MakeNormalSound(SND_FIRESHOOT);
			FireBullet(me->ID,me->x,me->y,30*FIXAMT,me->facing*32,BLT_IGNITER,0);
			break;
		case SKILL_TOASTIES:
			DeleteMonsterByType(MONS_TOASTY);
			for(i=0;i<(int)SpecificSkillVal(0,SKILL_TOASTIES);i++)
			{
				g=SummonGuy(goodguy->x,goodguy->y,FIXAMT*96,MONS_TOASTY);
				if(g)
				{
					g->reload=rand()%(30*3);
				}
			}
			MakeNormalSound(SND_IGNITE);
			break;
		case SKILL_DRAIN:
			i=BulletFindDrainTarget(me->team,me->facing,me->x,me->y,0,curMap,&curWorld);
			if(i==0)
			{
				// refund for having no target
				MakeNormalSound(SND_MAGICFAIL);
				ParticleBoom(PART_WHITEPOP,me->x,me->y,FIXAMT*30);
				player.magic+=(byte)SpecificSkillCost(1,sp);
				return;
			}
			else
			{
				b=FireBullet(i-1,GetGuy((word)i-1)->x,GetGuy((word)i-1)->y,GetGuy((word)i-1)->z+monsType[GetGuy((word)i-1)->type].height*FIXAMT,0,BLT_DRAINMASTER,(word)(SpecificSkillVal(0,SKILL_DRAIN)*(SpellDamageBoost(SC_DEATH))/100.0f));
				MakeNormalSound(SND_DRAIN);
			}
			break;
		case SKILL_LIFEBERRY:
			g=SummonGuy(me->x,me->y,FIXAMT*64,MONS_LIFEBERRY);
			if(!g)
			{
				// refund for failing to summon
				MakeNormalSound(SND_MAGICFAIL);
				ParticleBoom(PART_WHITEPOP,me->x,me->y,FIXAMT*30);
				player.magic+=(byte)SpecificSkillCost(1,sp);
				return;
			}
			else
			{
				MakeNormalSound(SND_PLANTSURFACE);
				DoMove(g,ANIM_MOVE,128,1,0,0);
				g->mind=(byte)SpecificSkillVal(1,SKILL_LIFEBERRY);
				g->mind1=30;
				if(TalentBonus(TLT_GREENTHUMB)>0)
					HealGoodguy(RecoverAmt((word)(0.5f+TalentBonus(TLT_GREENTHUMB))));
				if(!levelDef[player.levelNum].flags&LF_TOWN)
					TalentPoint(TLT_GREENTHUMB,1);
			}
			break;
		case SKILL_BURNINATE:
			MakeNormalSound(SND_BURNINATE);
			x=me->x+Cosine(me->facing*32)*24;
			y=me->y+Sine(me->facing*32)*24;
			FireBullet(goodguy->ID,x,y,FIXAMT*20,me->facing*32,BLT_BURNINATE,(word)(SpecificSkillVal(0,SKILL_BURNINATE)*(SpellDamageBoost(SC_FIRE))/100.0f));
			break;
		case SKILL_PLAGUE:
			MakeNormalSound(SND_POISON);
			for(x=0;x<PLAYER_SKILLS;x++)
				if(player.skillHave[x]==SKILL_PLAGUE)
					player.skillClock[x]=(int)SpecificSkillVal(0,SKILL_PLAGUE);
			break;
		case SKILL_HEATSHIELD:
			MakeNormalSound(SND_IGNITE);
			for(x=0;x<PLAYER_SKILLS;x++)
				if(player.skillHave[x]==SKILL_HEATSHIELD)
					player.skillClock[x]=(int)SpecificSkillVal(1,SKILL_HEATSHIELD);
			break;
	}
	if(player.amulet.magic==EM_BOKBOK)
	{
		for(i=0;i<SpecificSkillLevel(sp);i++)
		{
			g=SummonGuy(me->x,me->y,FIXAMT*20,MONS_BOKBOK2);
			if(g)
			{
				g->mind=60;
				g->mind2=3;
				g->tag=255;
				g->mind3=rand()%256;
			}
		}
	}
	if(!levelDef[player.levelNum].flags&LF_TOWN)
		TalentPoint(TLT_MEDITATION,(byte)SpecificSkillCost(1,sp));
}

byte MagicRate(void)
{
	int rate;

	rate=MAGIC_RATE;

	if(player.axe.magic==EM_MYSTIC)
		rate=rate*3/4;

	if(levelDef[player.levelNum].flags&LF_TOWN)
		rate/=2;	// double regen in town

	if(GlassEffect()==GE_MAGICGEN)
		rate=(byte)((float)rate*(100.0f-(float)GlassAmount())/100.0f);

	rate=(byte)((float)rate*(100.0f-SpecificSkillVal(1,SKILL_ENERGY))/100.0f);

	if(goodguy && goodguy->seq==ANIM_IDLE && goodguy->dx==0 && goodguy->dy==0 && goodguy->ouch==0)
		rate-=(byte)TalentBonus(TLT_MEDITATION);

	if(player.potionType==POT_BRAINJUICE)
		rate-=PotionEffect(POT_BRAINJUICE,player.potionPower);

	if(rate<1)
		rate=1;

	return (byte)rate;
}

byte CalcShockRate(byte oldRate)
{
	float f;

	f=100.0f-TalentBonus(TLT_ELECTROCUTION);
	f=(float)oldRate*f/100.0f;

	return (byte)f;
}

void LoonyControlCommon(Guy *me,mapTile_t *mapTile,world_t *world)
{
	int i;

	if(ModifierOn(MOD_DISCO))
	{
		monsType[player.monsType].offc[0]=2;
		if(player.monsType==MONS_PLAYBUNNY)
			monsType[player.monsType].offc[0]=4;
		else if(player.monsType==MONS_PLAYTHUG)
			monsType[player.monsType].offc[0]=0;
		monsType[player.monsType].offc[1]=rand()%8;
		monsType[player.monsType].offc[2]=-4+(rand()%9);
		if((player.timeToFinish%30)==0)
			NovaGuys(GOOD,goodguy->x,goodguy->y,FIXAMT*50,0,20,0);
	}
	if(ModifierOn(MOD_GANGSTA) && ModifierOn(MOD_KNIGHT))
	{
		// randomly swap if you have both
		if((player.timeToFinish%30)==0)
		{
			if(rand()%2)
				player.monsType=MONS_PLAYTHUG;
			else
				player.monsType=MONS_PLAYBUNNY;
			goodguy->type=player.monsType;
		}
	}
	if(ModifierOn(MOD_VAMPIRE))
	{
		if((player.timeToFinish%30)==0)
		{
			if(goodguy->hp==1)
				goodguy->GetShotReal(0,0,1,curMap,world);
			else if(goodguy->hp>1)
			{
				player.hearts--;
				goodguy->hp--;
				NumberParticle(1,goodguy->x,goodguy->y+1,5);
			}
		}
	}

	UpdateInterface();

	if(player.parka.magic==EM_DARK)
		curMap->DarkTorch(me->mapx,me->mapy,-10,6);

	if(player.var[VAR_PRISONBREAK]>1)
	{
		player.var[VAR_PRISONBREAK]--;
		if(player.var[VAR_PRISONBREAK]==2)
		{
			PrisonBreak(curMap,0);
		}
	}
	if(player.potionClock)
	{
		if(player.potionType==POT_HEALENBRAU)
		{
			HealGoodguy(RecoverAmt(PotionEffect(player.potionType,player.potionPower)));
			MakeColdRingParticle(me->x,me->y,0,64);
		}
		if(player.potionType==POT_MAGICOLA)
		{
			NumberParticle(PotionEffect(player.potionType,player.potionPower),goodguy->x,goodguy->y+1,6);
			player.magic+=PotionEffect(player.potionType,player.potionPower);
			if(player.magic>player.maxMagic)
				player.magic=player.maxMagic;
			MakeColdRingParticle(me->x,me->y,0,48);
		}
		if(player.potionType==POT_LIFEADE)
		{
			if((player.potionClock%PotionEffect(player.potionType,player.potionPower))==0)
				HealGoodguy(RecoverAmt(1));
		}
		if(player.potionType==POT_JOLTING && (player.potionClock%(CalcShockRate(PotionEffect(player.potionType,player.potionPower)))==0))
		{
			int t;
			t=BulletFindTargetClosest(me->team,me->x,me->y,0,90*FIXAMT,curMap,world);
			LightningBolt(me->x,me->y-me->z-FIXAMT*20,me->x-10*FIXAMT+(rand()%(20*FIXAMT+1)),me->y-me->z-FIXAMT*10-20*FIXAMT+(rand()%(40*FIXAMT+1)));
			if(t!=0)
			{
				meleeAttacker=NULL;
				bulletHittingType=0;
				GetGuy(t-1)->GetShotReal(0,0,ShockDmg(1,GOOD),curMap,world);
				TalentPoint(TLT_ELECTROCUTION,1);
				LightningBolt(me->x,me->y-me->z-FIXAMT*20,GetGuy(t-1)->x,GetGuy(t-1)->y-FIXAMT*20);
			}
		}
		if(player.potionType==POT_ROCKETSAUCE && (player.potionClock%PotionEffect(player.potionType,player.potionPower)==0))
		{
			FireBullet(me->ID,me->x,me->y,me->z,rand()%256,BLT_FLAMEB,CurseDmg(1,GOOD));
		}
		if(player.potionType==POT_STAMINO)
			player.stamina=player.maxStamina;

		player.potionClock--;
		if(!player.potionClock)
		{
			player.potionPower=0;
			player.potionType=0;
			PlayerCalcStats();
		}
	}

	if(player.var[VAR_PARRYCLOCK])
		player.var[VAR_PARRYCLOCK]--;
	if(player.parry)
		player.parry--;
	if(player.reload)
		player.reload--;
	if(player.chatClock)
		player.chatClock--;

	if(player.saveClock)
		player.saveClock--;

	if(player.invinc)
		player.invinc--;

	if(player.pushPower)
		player.pushPower--;

	if(player.berserkClock)
	{
		player.berserkClock--;
		if(player.amulet.magic==EM_HOTFOOT && (rand()%10)==0)
		{
			FireBullet(me->ID,me->x,me->y,me->z,rand()%256,BLT_FLAMEB,CurseDmg(1,GOOD));
		}
		player.stamina=player.maxStamina;
		if(!player.berserkClock)
			player.stamina=0;
	}

	if(player.axe.magic==EM_JUSTHEF)
		player.var[VAR_JUSTHEF]++;

	player.parkaClock++;
	player.amuletClock++;
	if(player.parka.magic==EM_HEALTHY)
	{
		if(player.parkaClock>=(20*30))
		{
			player.parkaClock=0;
			HealGoodguy(RecoverAmt(1));
		}
	}
	if(player.parka.magic==EM_HEALTHY2)
	{
		if(player.parkaClock>=(15*30))
		{
			player.parkaClock=0;
			HealGoodguy(RecoverAmt(1));
		}
	}
	if(player.parka.magic==EM_ENERGETIC)
	{
		if(player.parkaClock>=5*30)
		{
			player.parkaClock=0;
			if(player.stamina<player.maxStamina)
				player.stamina++;
		}
	}
	if(player.amulet.magic==EM_CHARGED)
	{
		if(player.amuletClock>=20*30)
		{
			player.amuletClock=0;
			if(player.magic<player.maxMagic)
				player.magic++;
		}
	}
	if(player.amulet.magic==EM_THINKING)
	{
		if(player.amuletClock>=15*30)
		{
			player.amuletClock=0;
			if(player.magic<player.maxMagic)
				player.magic++;
		}
	}
	if(player.amulet.magic==EM_SHOCKING)
	{
		if(player.amuletClock>=CalcShockRate(30))
		{
			int t;
			player.amuletClock=0;

			t=BulletFindTargetClosest(me->team,me->x,me->y,0,100*FIXAMT,curMap,world);
			if(t!=0)
			{
				meleeAttacker=NULL;
				bulletHittingType=0;
				GetGuy(t-1)->GetShotReal(0,0,ShockDmg(1,GOOD),curMap,world);
				TalentPoint(TLT_ELECTROCUTION,1);
				LightningBolt(me->x,me->y-me->z-FIXAMT*20,GetGuy(t-1)->x,GetGuy(t-1)->y-FIXAMT*20);
			}
		}
	}
	if(player.amulet.magic==EM_REGEN)
	{
		if((player.timeToFinish%(30*10))==0)
		{
			HealAllGoodguys(RecoverAmt(2));
		}
	}
	if(player.amulet.magic==EM_EVILEYE)
	{
		if((player.timeToFinish%30)==0)
		{
			EvilEye(me->mapx,me->mapy,me->facing);
		}
	}
	if(GlassEffect()==GE_LIFEGEN)
	{
		if((player.timeToFinish%(GlassAmount()*30))==0)
		{
			HealGoodguy(RecoverAmt(1));
		}
	}
	if(player.berserkClock==0)
	{
		player.magicClock++;
		if(player.magicClock>MagicRate())
		{
			player.magicClock=0;
			if(player.magic<player.maxMagic)
				player.magic++;
		}
	}
	if(player.spclMove!=SM_WHIRL)
	{
		player.staminaClock++;
		i=(int)STAMINA_RATE-(int)SpecificSkillVal(1,SKILL_ENDURANCE);
		if(player.axe.magic==EM_MYSTIC)
			i*=2;
		if(GlassEffect()==GE_STAMGEN)
			i=(int)((float)i*(100.0f-(float)GlassAmount())/100.0f);

		if(player.staminaClock>i)
		{
			player.staminaClock=0;
			if(player.stamina<player.maxStamina)
				player.stamina++;
		}
	}

	SkillsUpdate();
}

void LoonyControlStunned(Guy *me,mapTile_t *mapTile,world_t *world)
{
	int decel;

	LoonyControlCommon(me,mapTile,world);

	decel=PLYR_DECEL;

	if(player.spclMove==SM_WHIRL)
	{
		player.spclMove=0;
		player.whirlClock=0;
	}
	// ice is slippery
	if(!(world->terrain[mapTile->floor].flags&TF_ICE) && player.spclMove!=SM_WHIRL)
	{
		if(me->z==0)
		{
			Dampen(&me->dx,decel);
			Dampen(&me->dy,decel);
		}
	}
	else
	{
		if(me->wallBump)	// bumped a wall while on ice
		{
			if(me->wallBump&1)
				me->dx=-me->dx;
			if(me->wallBump&2)
				me->dy=-me->dy;
		}
	}
	me->wallBump=0;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_LOONYOUCH,me->x,me->y,SND_CUTOFF|SND_ONE|SND_RANDOM,2000);
		else if(me->seq==ANIM_DIE)	// so it doesn't do this if you're drowning
			MakeSound(SND_LOONYDIE,me->x,me->y,SND_CUTOFF|SND_ONE|SND_RANDOM,2000);
	}

	if(!(world->terrain[mapTile->floor].flags&TF_ICE))
	{
		Dampen(&me->dx,decel/2);
		Dampen(&me->dy,decel/2);
	}

	player.boredom=0;
}

void LoonyControlMe(Guy *me,mapTile_t *mapTile,world_t *world)
{
	byte c;
	int x,y,i;
	int maxSpd,accel,decel;

	LoonyControlCommon(me,mapTile,world);

	if(player.stone)
	{
		player.stone--;
		return;
	}
#ifdef DIRECTORS
	if(player.parka.magic==EM_IMAGINARY && (rand()%100)<4)
	{
		if(player.invinc<5)
			player.invinc=5;
	}
	if(player.amulet.magic==EM_SP && (rand()%100)<10)
	{
		FireBullet(me->ID,me->x,me->y,FIXAMT*30+me->z,rand()%256,BLT_PFIREBALL2,1);
	}
#endif

	maxSpd=PLYR_MAXSPD;
	accel=PLYR_ACCEL;
	decel=PLYR_DECEL;
	if(player.parka.magic==EM_SWIFT)
	{
		maxSpd=(maxSpd*5)/4;
		accel=(accel*5)/4;
		decel=(decel*5)/4;
	}
	maxSpd+=(int)TalentBonus(TLT_MOBILITY);

	// ice is slippery
	if(!(world->terrain[mapTile->floor].flags&TF_ICE) && player.spclMove!=SM_WHIRL)
	{
		if(me->z==0)
		{
			Dampen(&me->dx,decel);
			Dampen(&me->dy,decel);
		}
	}
	else
	{
		if(me->wallBump)	// bumped a wall while on ice
		{
			if(me->wallBump&1)
				me->dx=-me->dx;
			if(me->wallBump&2)
				me->dy=-me->dy;
		}
	}
	me->wallBump=0;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_LOONYOUCH,me->x,me->y,SND_CUTOFF|SND_ONE|SND_RANDOM,2000);
		else if(me->seq==ANIM_DIE)	// so it doesn't do this if you're drowning
			MakeSound(SND_LOONYDIE,me->x,me->y,SND_CUTOFF|SND_ONE|SND_RANDOM,2000);
	}

	// triggering stuff
	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK)
		{
			if(player.spclMove==SM_WHIRL)
			{
				player.whirlClock--;
				if(!player.whirlClock)
				{
					player.spclMove=SM_NONE;
					me->frm=4;
					me->frmTimer=0;
					return;
				}
				if(((player.whirlClock+5)%((byte)SpecificSkillVal(0,SKILL_WHIRL)))==0)
				{
					LoonyWhirlHit(me,world);
				}
				me->facing=(me->facing+1)&7;
				me->frm=3;
				me->frmTimer=0;
				c=GetControls();
				if(EquipCount(IT_ICESKATE) || !(world->terrain[mapTile->floor].flags&TF_ICE))
				{
					if(c&CONTROL_UP)
						me->dy-=FIXAMT;
					if(c&CONTROL_DN)
						me->dy+=FIXAMT;
					if(c&CONTROL_LF)
						me->dx-=FIXAMT;
					if(c&CONTROL_RT)
						me->dx+=FIXAMT;
					Dampen(&me->dx,FIXAMT/8);
					Dampen(&me->dy,FIXAMT/8);
					Clamp(&me->dx,maxSpd);
					Clamp(&me->dy,maxSpd);
				}
			}
			else
			{
				c=GetTaps();
				if((c&CONTROL_B2) && player.spclMove==0 && SpecificSkillVal(0,SKILL_WHIRL)!=0 && player.axeMode==0)
				{
					// begin whirling!
					if(player.stamina>=SpecificSkillCost(0,SKILL_WHIRL))
					{
						player.stamina-=(byte)SpecificSkillCost(0,SKILL_WHIRL);
						TalentPoint(TLT_VIVACITY,(byte)SpecificSkillCost(0,SKILL_WHIRL));
						player.whirlClock=(byte)SpecificSkillVal(1,SKILL_WHIRL);
						player.spclMove=SM_WHIRL;
						me->frm=3;
						me->frmTimer=0;
						me->frmAdvance=32;
						return;
					}
				}
				if(player.axeMode==0 && (me->frm==3 || me->frm==4) && player.reload==0)
				{
					x=me->x+Cosine(me->facing*32)*32;
					y=me->y+Sine(me->facing*32)*24;
					i=-1;
					while(i!=0)
					{
						if(i==-1)
							i=0;
						i=FindVictim(x,y,me->mapx,me->mapy,24,0,2,i,curMap,world);
						CheckForChopTree(curMap,x/(TILE_WIDTH*FIXAMT),y/(TILE_HEIGHT*FIXAMT));
						if(i)
						{
							LoonyHitGuy(me,i,world);
						}
					}
					player.reload=20;
				}
			}
		}

		if(me->seq==ANIM_A3)	// jumping
		{
			if(me->z==0)
			{
				DoMove(me,ANIM_IDLE,128,0,me->dx,me->dy);
				if(!(world->terrain[mapTile->floor].flags&TF_SKY))
					MakeSound(SND_LOONYLAND,me->x,me->y,SND_CUTOFF|SND_RANDOM,600);
				else
				{
					me->seq=ANIM_A3;
					me->frm=9;
				}
			}
			else
			{
				c=GetTaps();
				if((c&CONTROL_B1) && player.reload==0)
				{
					LoonyFire(me);
				}
				if((c&CONTROL_B2) && player.spclMove==0 && SpecificSkillVal(0,SKILL_STOMP)>=1 && player.axeMode==0 && !(world->terrain[mapTile->floor].flags&TF_SKY))
				{
					if(player.stamina>=SpecificSkillCost(0,SKILL_STOMP))
					{
						player.stamina-=(byte)SpecificSkillCost(0,SKILL_STOMP);
						player.spclMove=SM_NOVA;
						if(me->dz>-FIXAMT*4)
							me->dz=-FIXAMT*4;
						DoMove(me,ANIM_A4,128,1,0,0);
					}
				}
				if((c&CONTROL_B2) && player.spclMove==0 && SpecificSkillVal(0,SKILL_TURRET)>=1 && player.axeMode==1 && !(world->terrain[mapTile->floor].flags&TF_SKY))
				{
					if(player.magic>=SpecificSkillCost(1,SKILL_TURRET))
					{
						player.magic-=(byte)SpecificSkillCost(1,SKILL_TURRET);
						player.spclMove=SM_TURRET;
						if(me->dz>-FIXAMT*4)
							me->dz=-FIXAMT*4;
						DoMove(me,ANIM_A4,128,1,0,0);
					}
				}
			}
		}
		if(me->seq==ANIM_A4)	// air attack
		{
			if(player.spclMove==SM_NOVA)	// when you hit the ground, send out a nova
			{
				if(me->z==0)
				{
					byte stun;
					word knock;

					player.spclMove=SM_NONE;
					DoMove(me,ANIM_IDLE,128,0,0,0);
					FireBullet(me->ID,me->x,me->y,0,0,BLT_NOVA,0);
					stun=(byte)SpecificSkillVal(0,SKILL_STOMP)*10;
					knock=(word)SpecificSkillVal(0,SKILL_STOMP)*8*FIXAMT;
					ShakeScreen(20);
					NovaGuys(me->team,me->x,me->y,(int)SpecificSkillVal(1,SKILL_STOMP)*FIXAMT,(word)SpecificSkillVal(0,SKILL_STOMP)*2,stun,knock);
					me->z=1;
					me->dz=FIXAMT*5;
					me->dx=0;
					me->dy=0;
				}
			}
			else if(player.spclMove==SM_TURRET)	// when you hit the ground, make an Axe Trap
			{
				if(me->z==0)
				{
					player.spclMove=SM_NONE;
					DoMove(me,ANIM_IDLE,128,0,0,0);
					AddGuy(me->x,me->y,0,MONS_AXETRAP);
					MakeRingParticle(me->x,me->y,0,40,32);
				}
			}
			else if(me->frm==2 && player.reload==0 && player.axeMode==0)
			{
				x=me->x+Cosine(me->facing*32)*32;
				y=me->y+Sine(me->facing*32)*24;
				i=-1;
				while(i!=0)
				{
					if(i==-1)
						i=0;
					i=FindVictim(x,y,me->mapx,me->mapy,24,0,2,i,curMap,world);
					if(i)
					{
						LoonyHitGuy(me,i,world);
					}
				}
				player.reload=3;
			}
			if(me->z==0)
			{
				DoMove(me,ANIM_IDLE,128,0,me->dx,me->dy);
				MakeSound(SND_LOONYLAND,me->x,me->y,SND_CUTOFF|SND_RANDOM,600);
			}
		}


		if(me->seq==ANIM_DIE)
		{
			me->facing=(me->facing+1)&7;
		}
		if(me->seq==ANIM_MOVE && me->z==0)
			me->action=ACTION_IDLE;

		return;
	}
	if(player.spclMove==SM_WHIRL)
	{
		player.spclMove=0;
		player.whirlClock=0;
	}

	player.var[VAR_FLAMEPAID]=0;

	if((me->seq==ANIM_IDLE || me->seq==ANIM_MOVE) && player.axeMode==0)
		player.reload=0;

	// not busy, let's see if you want to do something
	c=GetControls();

	if(c==0)	// not pushing anything, recover stamina faster
	{
		player.staminaClock++;
	}

	DoPlayerFacing(c,me);

	if((c&CONTROL_B1) && player.reload==0)	// pushed attack button
	{
		LoonyFire(me);
		if(me->action==ACTION_BUSY)
			return;
	}
	// pushed jump
	if((c&CONTROL_B2) && me->z==0 && !TerrainIs(me->mapx,me->mapy,TF_SKY|TF_CLIFF))
	{
		GetTaps();
		DoMove(me,ANIM_A3,128,1,0,0);
		MakeSound(SND_LOONYJUMP,me->x,me->y,SND_CUTOFF|SND_RANDOM,600);
		player.boredom=0;
		me->z=1;
		me->dz=15*FIXAMT;
		if((c&(CONTROL_UP|CONTROL_DN|CONTROL_LF|CONTROL_RT)) && (EquipCount(IT_ICESKATE) || !(world->terrain[mapTile->floor].flags&TF_ICE)))
		{
			me->dx=Cosine(me->facing*32)*PLYR_MAXSPD/FIXAMT;
			me->dy=Sine(me->facing*32)*PLYR_MAXSPD*TILE_HEIGHT/(TILE_WIDTH*FIXAMT);
		}
		else
		{
			me->dx=0;
			me->dy=0;
		}
		return;
	}
	if((c&CONTROL_B3))	// pushed magic
	{
		EnterSpellPick();
		return;
	}

	// if you are moving indeed
	if(c&(CONTROL_UP|CONTROL_DN|CONTROL_LF|CONTROL_RT))
	{
		if(!(world->terrain[mapTile->floor].flags&TF_ICE))
		{
			if(!TerrainIs(me->mapx,me->mapy,TF_SKY|TF_CLIFF))
			{
				me->dx+=Cosine(me->facing*32)*accel/FIXAMT;
				me->dy+=Sine(me->facing*32)*accel/FIXAMT;
				if((Cosine(me->facing*32)<0 && me->dx>0) || (Cosine(me->facing*32)>0 && me->dx<0))
					me->dx=0;
				if((Sine(me->facing*32)<0 && me->dy>0) || (Sine(me->facing*32)>0 && me->dy<0))
					me->dy=0;
			}
		}
		else // ice is slippery
		{
			if(EquipCount(IT_ICESKATE))
			{
				me->dx+=Cosine(me->facing*32)/4;
				me->dy+=Sine(me->facing*32)/4;
			}
		}

		Clamp(&me->dx,maxSpd);
		Clamp(&me->dy,maxSpd);

		if(TerrainIs(me->mapx,me->mapy,TF_SKY))
		{
			me->seq=ANIM_A3;
			me->frm=9;
		}
		else if(me->seq!=ANIM_MOVE)
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
			Dampen(&me->dx,decel/2);
			Dampen(&me->dy,decel/2);
		}
		if(me->seq==ANIM_MOVE)
		{
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}

	if(TerrainIs(me->mapx,me->mapy,TF_SKY))
	{
		me->seq=ANIM_A3;
		me->frm=9;
		player.boredom=0;
	}

	if(me->seq==ANIM_MOVE && me->frm==12)
	{
		me->frm=0;
	}

	// boredom handler
	if(me->action==ACTION_IDLE)
	{
		player.boredom++;
		if(player.boredom>200+(int)Random(3200))
		{
			me->seq=ANIM_A2;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=72;
			player.boredom=0;
		}
	}
}

#include "player.h"
#include "options.h"
#include "title.h"
#include "quest.h"
#include "equip.h"
#include "skill.h"
#include "help.h"
#include "alchemy.h"
#include "leveldef.h"
#include "glasses.h"
#include "madcap.h"
#include "achieve.h"
#include "services.h"

// characters
#include "ch_loony.h"

player_t player;
byte playerGlow;	// for torch-lit levels, and for exciting moments
byte tportclock;

void InitPlayer(byte initWhat,byte world,byte level)
{
	int i;

	if(initWhat==INIT_GAME)	// initialize everything, this is to start a whole new game
	{
		strcpy(player.profile,GetSavedName());

		memset(player.log,0,sizeof(char)*MAX_LOG*LOG_LEN);
		player.logTime=0;
		player.logBright=0;
		NewMessage("Welcome to Loonyland!",0);
		memset(&player.var[0], 0, 1024);
		player.var[VAR_MODIFIER+0]=GetSavedMod(0);
		player.var[VAR_MODIFIER+1]=GetSavedMod(1);
		player.var[VAR_MODIFIER+2]=GetSavedMod(2);

		if(ModifierOn(MOD_EXPERT))
		{
			for(i=VAR_TIP;i<VAR_FROSTGATE;i++)
				player.var[i]=1;	// mark off all tips
			player.var[VAR_TIP+TIP_ENDING-1]=0;	// let the ending message come up to warn you again
		}
		if(ModifierOn(MOD_IRONMAN))
			player.var[VAR_TIP+TIP_DIE-1]=1;	// ironmen don't want to see that tip

		player.spell=0;

		SetupShopLists();
		InitialEquip();
		SortEquipment();
		InitialSkills();
		InitialTalents();

		player.startHearts=10;
		player.startMagic=10;
		player.level=1;
		player.playClock=0;

		PlayerCalcStats();
		player.hearts=player.maxHearts;
		player.maxStamina=player.hearts;
		player.magic=player.maxMagic;
		player.stamina=player.maxStamina;
		player.staminaClock=0;
		player.magicClock=0;

		player.xp=0;
		player.xpNeed=100;
		player.axeMode=0;
		player.skillPts=0;

		player.destx=0;
		player.desty=0;
		player.money=0;
		player.fireRange=0;
		player.fireFlags=0;
		player.worldNum=world;
		player.cheatsOn=0;
		player.parkaClock=0;
		player.amuletClock=0;

		player.var[VAR_RATS]=17;
		memcpy(&player.var[VAR_CONTROLS],opt.control,sizeof(byte)*2*8);
		memcpy(&player.var[VAR_CONTROLS+2*8],opt.joyCtrl,sizeof(byte)*4);	// copy in the controls
		player.var[VAR_SOUND]=opt.sound;
		player.var[VAR_MUSIC]=opt.music;

		player.timeLimit=0;
		// end-game stats
		player.monsterPoints=0;
		player.bestCombo=0;
		player.shotsFired=0;
		player.specialShotsFired=0;
		player.gemsGotten=0;
		player.hitsTaken=0;
		player.timeToFinish=0;
		player.batLevel=0;
		player.xtraVar=0;
		player.crystalBall=0;
		player.xtraByte=0;
		player.monsType=MONS_LOONY;
		player.kills=0;

		player.potionsDrunk=0;
		player.herbsPicked=0;
		player.holesDug=0;
		player.damageDone=0;

		player.potionClock=0;
		player.potionType=0;
		player.potionPower=0;
		player.lens[0]=255;
		player.lens[1]=255;

		if(ModifierOn(MOD_HEIR))
		{
			while(player.axe.type!=IT_GOLDENAXE || player.parka.type!=IT_GOLDENPARKA || player.amulet.type!=IT_GOLDENAMULET)
			{
				byte type;
				int val1,val2;
				equip_t eq;

				bossDrop=1;
				EquipCreateItem(5,&type,&val1,&val2);
				bossDrop=0;
				eq.type=type;
				eq.value=(val1)%256;
				eq.speed=((val1)/256)%256;
				eq.magic=(val1)/65536;
				eq.sharpened=0;
				eq.count=1;
				eq.skill[0]=(val2)%256;
				eq.skill[1]=((val2)/256)%256;
				eq.skill[2]=((val2)/65536)%256;
				if(type==IT_GOLDENAXE)
					player.axe=eq;
				else if(type==IT_GOLDENPARKA)
					player.parka=eq;
				else if(type==IT_GOLDENAMULET)
					player.amulet=eq;
			}
		}
		if(ModifierOn(MOD_TICKING))
		{

		}
		strcpy(player.addonName,FetchAddOnName());
	}
	if(initWhat>=INIT_WORLD) // initialize the things that go with each world
	{
	}

	if(levelDef[level].flags&LF_TOWN)
		player.lastTown=level;
	player.levelNum=level;
	player.clockRunning=0;
	player.boredom=0;
	player.reload=0;
	playerGlow=0;
	player.pushPower=0;
	player.speed=0;
	player.chatClock=0;
	player.stone=0;
	player.timeLimit=0;
	player.parry=0;
	player.spclMove=0;
	PlayerCalcStats();
	SortEquipment();

	if(ModifierOn(MOD_GANGSTA))
		player.monsType=MONS_PLAYTHUG;
	else if(ModifierOn(MOD_KNIGHT))
		player.monsType=MONS_PLAYBUNNY;

	if(player.var[VAR_SUPPLIES]<20 && player.var[VAR_SNUGGLY]==0)
		player.var[VAR_SUPPLIES]=0;
	// turn off plague if it's running
	for(i=0;i<PLAYER_SKILLS;i++)
		if(player.skillHave[i]==SKILL_PLAGUE)
			player.skillClock[i]=0;
}

void MadcapPlayer(void)
{
	int i;

	MadcapArena();
	for(i=0;i<1024;i++)
		if((i<VAR_TIP || i>=VAR_FROSTGATE) && (i<VAR_MADGEM || i>VAR_CPOINTS) && i!=VAR_TAMED && i!=VAR_GLASSES && i!=VAR_CLOCKLIFE && i!=VAR_CLOCKLIFE2 &&
			(i<VAR_CONTROLS))
			player.var[i]=0;	// keep tips gone

	player.var[VAR_TAMED]&=2;	// undo the magicsmith's mimic, but not yours
	player.var[VAR_RATS]=17;
	player.var[VAR_MADCAP]=1;
	player.var[VAR_TIP+0]=0;
	player.var[VAR_TIP+TIP_ENDING-1]=0;	// let the ending message come up to warn you again

	player.talentLevel[TLT_GLADIATOR]=255;
	player.talentPts[TLT_GLADIATOR]=0;

	// strip out all quest items, from stash too
	for(i=0;i<MAX_EQUIP;i++)
	{
		if(player.items[i].count>0 && (ItemCost(&player.items[i],1)==0 || player.items[i].type==IT_ROPE || player.items[i].type==IT_TOYCRYSTAL))	// unsellable item, must be important!
		{
			player.items[i].count=0;
			player.items[i].type=IT_NONE;
		}
	}
	SortEquipment();
	for(i=0;i<SHOP_SIZE;i++)
	{
		if(player.shopInv[SHOP_STASH][i].count>0 && (ItemCost(&player.shopInv[SHOP_STASH][i],1)==0 || player.shopInv[SHOP_STASH][i].type==IT_ROPE || player.shopInv[SHOP_STASH][i].type==IT_TOYCRYSTAL))	// unsellable item, must be important!
		{
			player.shopInv[SHOP_STASH][i].count=0;
			player.shopInv[SHOP_STASH][i].type=IT_NONE;
		}
	}
	SortEquipment();
	SortShop(SHOP_STASH);
}

void ExitPlayer(void)
{
}

void PlayerClearTempVars(void)
{
	int i;

	// do this between levels
	for(i=VAR_TEMP;i<256;i++)
		player.var[i]=0;
}

float CalcPercent(player_t *p)
{
	int i,totalpoints,gotpoints;

	gotpoints=0;
	totalpoints=0;

	for(i=VAR_QUESTDONE;i<VAR_QUESTDONE+NUM_QUESTS;i++)
	{
		totalpoints+=5;
		if(p->var[i])
			gotpoints+=5;
	}
	for(i=VAR_PRESENT;i<VAR_PRESENT+99;i++)
	{
		totalpoints++;
		if(p->var[i])
			gotpoints++;
	}

	for(i=0;i<MAX_SKILLS;i++)
	{
		totalpoints++;
		if(p->skillHave[i])
			gotpoints++;
	}
	for(i=0;i<MAX_TALENTS;i++)
	{
		totalpoints++;
		if(p->talentLevel[i]!=255)
			gotpoints++;
	}

	if(gotpoints==totalpoints)
		return (float)100.0f;
	return (float)(100.0*gotpoints)/(float)totalpoints;
}

void PlayerWinLevel(byte w,byte l,byte isSecret)
{
}

void SetPlayerHP(int hp)
{
	if(hp<player.hearts)
		player.hitsTaken+=player.hearts-hp;
	player.hearts=(word)hp;
}

void PlayerSetVar(int v,int val)
{
	int oldval;

	oldval=player.var[v];
	player.var[v]=val;

	// this is a quest completion variable
	if(v>=VAR_QUESTDONE && v<VAR_QUESTDONE+NUM_QUESTS)
	{
		if(oldval==0 && val)
		{
			// didn't previously complete it
			NewBigMessage(QuestName(v-VAR_QUESTDONE),90,1);
			MakeNormalSound(SND_QUESTDONE);

			if(player.var[VAR_QUESTASSIGN+(v-VAR_QUESTDONE)]==0)
				player.var[VAR_QUESTASSIGN+(v-VAR_QUESTDONE)]=1;
			if(!player.var[VAR_MADCAP])
				PlayerGetExactXP(100);
			else
				PlayerGetExactXP(1000);
			if(v==VAR_QUESTDONE+QUEST_FACTORY)
			{
				KillAllMonsters(MONS_TOYMAKER);
			}
			QuestReportEvent(QE_QUEST,v-VAR_QUESTDONE);
			if(ModifierOn(MOD_SKILLBOR))
			{
				char s[32];

				if(player.skillPts<255)
					player.skillPts++;
				if(player.skillPts==1)
					sprintf(s,"You have %d Skill Point.",player.skillPts);
				else
					sprintf(s,"You have %d Skill Points.",player.skillPts);
				NewMessage(s,0);
			}
		}
	}
	if(v>=VAR_QUESTASSIGN && v<VAR_QUESTASSIGN+NUM_QUESTS)
	{
		if(oldval==0 && val)
		{
			// hasn't been previously assigned
			NewBigMessage(QuestName(v-VAR_QUESTASSIGN),90,0);
			player.var[v]=val;
			MakeNormalSound(SND_NEWQUEST);
			TipEvent(TIP_QUEST);
		}
	}

	QuestReportEvent(QE_VAR,v);
}

void SuperHeal(void)
{
	player.hearts=player.maxHearts;
	goodguy->hp=player.hearts;
}

// returns 1 if the item should not disappear, 0 if it should
byte PlayerGetItem(byte itm,int val1,int val2)
{
	char s[32];


	if(itm>=MAX_ITMS)
	{
		sprintf(s,"BAD ITEM #%d/%d/%d",itm,val1,val2);
		NewMessage(s,0);
		return 1;
	}

	if(!(ItemFlags(itm)&IF_GET))
		return 1;

	if(itm>=IT_POT1 && itm<=IT_POT20)	// easier than putting all 20 in the case statement
	{
		if(EquipAdd(itm,val1,0))
		{
			TipEvent(TIP_POTION);
			MakeNormalSound(SND_POTIONGET);
#ifdef DIRECTORS
			AllPotionCheck();
#endif
			return 0;
		}
		else
			return 1;
	}
	if(itm>=IT_LENSA && itm<=IT_LENSM)	// easier than putting all 13 in the case statement
	{
		if(EquipAdd(itm,0,0))
		{
			MakeNormalSound(SND_LENSGET);
			return 0;
		}
		else
			return 1;
	}

	switch(itm)
	{
		case IT_PICTURE:
#ifdef DIRECTORS
			if(opt.galPix[val1-1]==0)
			{
				opt.galPix[val1-1]=1;
				NewMessage("You unlocked a Gallery picture!",0);
				SaveOptions();
				picToDrop=0;
			}
#endif
			return 0;
			break;
		case IT_SUPPLIES:
		case IT_SUPPLIES2:
			if(player.levelNum==LVL_CRAMPEDCAVE && (player.var[VAR_SNUGGLY]==1 || player.var[VAR_ONIONCHAT]==0))
				return 1;	// no picking them up!

			if(player.var[VAR_SUPPLIES]<20)
			{
				PlayerSetVar(VAR_SUPPLIES,player.var[VAR_SUPPLIES]+1);
				if(player.var[VAR_SUPPLIES]==20 && player.var[VAR_SNUGGLY]==1)
					PlayerSetVar(VAR_QUESTDONE+QUEST_SUPPLIES,1);
			}

			FXRing(0,goodguy->x,goodguy->y,0,16,3);
			MakeNormalSound(SND_BOOM);
			return 0;
			break;
		case IT_AXE:
		case IT_MAGICAXE:
		case IT_GOLDENAXE:
		case IT_AMULET:
		case IT_MAGICAMULET:
		case IT_GOLDENAMULET:
		case IT_PARKA:
		case IT_MAGICPARKA:
		case IT_GOLDENPARKA:
			TipEvent(TIP_EQUIP);
			if(itm!=IT_AXE && itm!=IT_AMULET && itm!=IT_PARKA)
				TipEvent(TIP_MAGIC);
			if(EquipAdd(itm,val1,val2))
			{
				MakeNormalSound(SND_JUNKGET);
				return 0;
			}
			else
				return 1;
			break;
		case IT_COIN:
			PlayerGetMoney(1);
			TalentPoint(TLT_MISER,1);
			MakeNormalSound(SND_GETCOIN);
			return 0;
			break;
		case IT_TENCOIN:
			PlayerGetMoney(5);
			MakeNormalSound(SND_GETCOIN);
			MakeNormalSound(SND_GETCOIN);
			TalentPoint(TLT_MISER,5);
			return 0;
			break;
		case IT_SCROLL:
			LearnSkill((byte)val1);
			TipEvent(TIP_SKILL);
			if(HaveTwoOfASchool())
				TipEvent(TIP_SYNERGY);

			MakeNormalSound(SND_SPECIALGET);
			QuestReportEvent(QE_SKILL,val1);
			return 0;
			break;
		case IT_PRESENT:
			if(EquipAdd(IT_PRESENT,0,0))
			{
				if(ModifierOn(MOD_SKILLBOR))
				{
					if(player.skillPts<255)
						player.skillPts++;
					if(player.skillPts==1)
						sprintf(s,"You have %d Skill Point.",player.skillPts);
					else
						sprintf(s,"You have %d Skill Points.",player.skillPts);
					NewMessage(s,0);
				}
				PlayerSetVar(val1+VAR_PRESENT,1);
				if(player.var[VAR_MADCAP])
					PlayerGetExactXP(500);
				else
					PlayerGetExactXP(50);
				MakeNormalSound(SND_SPECIALGET);
				return 0;
			}
			break;
			// herbs
		case IT_HERBA:
		case IT_HERBB:
		case IT_HERBC:
		case IT_HERBD:
		case IT_HERBE:
		case IT_HERBF:
			if(EquipAdd(itm,0,0))
			{
				player.herbsPicked++;
				TalentPoint(TLT_HERBALISM,1);
				// make herby noises
				MakeNormalSound(SND_JUNKGET);
				return 0;
			}
			else
				return 1;
			break;
			// junk items
		case IT_LOG:
		case IT_CAN:
		case IT_GEAR:
		case IT_STUFFING:
		case IT_SOCK:
		case IT_ICECUBE:
		case IT_SEED:
		case IT_SILVER:
		case IT_GOLD:
		case IT_RUBY:
		case IT_EMERALD:
		case IT_DIAMOND:
		case IT_ECTO:
		case IT_STONE:
		case IT_CLAW:
			if(EquipAdd(itm,0,0))
			{
				if(itm>=IT_LOG && itm<=IT_CLAW)
					TalentPoint(TLT_PACKRAT,1);

				// make junk get noises
				MakeNormalSound(SND_JUNKGET);
				return 0;
			}
			else
				return 1;
			break;
		case IT_KEY1:
		case IT_KEY2:
		case IT_KEY3:
		case IT_KEY4:
		case IT_KEY5:
		case IT_TOOLBOX:
		case IT_SHARPSTONE:
		case IT_LETTER:
		case IT_STOLEN:
		case IT_PACKAGE:
		case IT_CANDY:
		case IT_BOOK:
		case IT_SOUNDING:
		case IT_BCRYSTAL:
		case IT_THING2:
		case IT_TOYCRYSTAL:
		case IT_ICESKATE:
		case IT_ARTBIT:
			if(EquipAdd(itm,0,0))
			{
				// key get/special item get noises
				MakeNormalSound(SND_SPECIALGET);
				return 0;
			}
			else
				return 1;
			break;
		case IT_THING:
			if(EquipAdd(itm,0,0))
			{
				// key get/special item get noises
				PlayerSetVar(VAR_QUESTDONE+QUEST_FRAGILE,1);
				MakeNormalSound(SND_SPECIALGET);
				return 0;
			}
			else
				return 1;
			break;
		case IT_WATCH:
			if(EquipAdd(itm,0,0))
			{
				// key get/special item get noises
				PlayerSetVar(VAR_QUESTDONE+QUEST_PLANTS,1);
				MakeNormalSound(SND_SPECIALGET);
				return 0;
			}
			else
				return 1;
			break;
		case IT_GLASSES:
			player.var[VAR_GLASSES]=1;
			TipEvent(TIP_GLASSES);
			MakeNormalSound(SND_SPECIALGET);
			return 0;
			break;
		case IT_SWORD:
			ExplodeParticles(PART_YELLOW,goodguy->x,goodguy->y,0,10);
			MakeNormalSound(SND_SPECIALGET);
			player.var[VAR_SWORDS]++;
			return 0;
			break;
	}
	return 1;
}

void PlayerLevelUp(void)
{
	char s[64];
	byte sp;

	if(player.level==50)
		return;

	player.level++;
	player.xpNeed=player.xpNeed+80*player.level;
	if(ModifierOn(MOD_SKILLBOR))
	{
		// no skill points!
	}
	else if(ModifierOn(MOD_EVEN))
	{
		player.skillPts+=3;
	}
	else
	{
		sp=(player.level/10)+1;
		player.skillPts+=sp;
	}
	PlayerCalcStats();
	sprintf(s,"LEVEL UP! You are level %d.",player.level);
	NewMessage(s,0);
	if(!ModifierOn(MOD_SKILLBOR))
	{
		if(player.skillPts==1)
			sprintf(s,"You have %d Skill Point.",player.skillPts);
		else
			sprintf(s,"You have %d Skill Points.",player.skillPts);
		NewMessage(s,0);
	}
	MakeNormalSound(SND_LEVELUP);
	player.hearts=player.maxHearts;
	player.magic=player.maxMagic;
	goodguy->hp=player.hearts;
	PlayerCalcStats();
	player.stamina=player.maxStamina;

	if(goodguy)
	{
		FXRing(5,goodguy->x,goodguy->y,goodguy->z,20,1);
		FXRing(5,goodguy->x,goodguy->y,goodguy->z,20,2);
	}
	TipEvent(TIP_LEVELUP);
}

void PlayerGetExactXP(int amt)
{
	dword oldXP;

	if(amt>1000)
		amt=1000;

	if(player.parka.magic==EM_TRAINEE)
		amt=amt*3/2;
	oldXP=player.xp;
	player.xp+=(int)amt;
	if(oldXP/MADCAP_XP<player.xp/MADCAP_XP && player.var[VAR_MADCAP])
	{
		char s[64];
		byte amt;

		amt=(player.xp/MADCAP_XP)-(oldXP/MADCAP_XP);
		if(player.var[VAR_CPOINTS]+amt<255)
			player.var[VAR_CPOINTS]+=amt;
		else
			player.var[VAR_CPOINTS]=255;

		if(player.var[VAR_CPOINTS]==1)
			sprintf(s,"You now have %d Crystal Point!",player.var[VAR_CPOINTS]);
		else
			sprintf(s,"You now have %d Crystal Points!",player.var[VAR_CPOINTS]);
		NewMessage(s,0);
		MakeNormalSound(SND_TALENTUP);
		if(goodguy)
		{
			FXRing(7,goodguy->x,goodguy->y,goodguy->z,10,1);
			FXRing(7,goodguy->x,goodguy->y,goodguy->z,10,2);
		}
	}
	if(player.xp>=1000000)
		AchieveNow(44);
	NumberParticle2((short)amt,goodguy->x,goodguy->y+1,7,PART_XP);
	while(player.xp>=player.xpNeed && player.level<50)
		PlayerLevelUp();
}

void PlayerGetMonsterPoints(int level)
{
	float pp;
	float mul;

	pp=10.0f*(((float)level/(float)player.level)*((float)level/(float)player.level))+0.5f;
	if(pp<1)
		pp=1;

	mul=(float)(rand()%(SkillTrainingAmount()+1));
	mul+=100;
	if(player.potionType==POT_SMARTSAUCE)
		mul+=PotionEffect(player.potionType,player.potionPower);
	pp=(pp*mul)/100.0f;

	if(player.axe.magic==EM_JORD)
		pp*=2.0f;
	if(ModifierOn(MOD_ARENA))
	{
		if(levelDef[player.levelNum].flags&LF_ARENA)
			pp*=5.0f;
		else
			pp=0;
	}
	PlayerGetExactXP((int)pp);
}

void PlayerGetBossPoints(int level)
{
	float pp;
	float mul;

	pp=10.0f*(((float)level/(float)player.level)*((float)level/(float)player.level))+0.5f;
	if(pp<1)
		pp=1;

	mul=(float)(rand()%(SkillTrainingAmount()+1));
	mul+=100;
	if(player.potionType==POT_SMARTSAUCE)
		mul+=PotionEffect(player.potionType,player.potionPower);
	pp=(pp*mul)/100.0f;

	if(player.axe.magic==EM_JORD)
		pp*=2.0f;
	pp*=5.0f;

	if(ModifierOn(MOD_ARENA))
	{
		if(levelDef[player.levelNum].flags&LF_ARENA)
			pp*=5.0f;
		else
			pp=0;
	}
	PlayerGetExactXP((int)pp);
}

byte PlayerPushMore(void)
{
	player.pushPower+=2;
	if(player.pushPower>=5)
	{
		player.pushPower=0;
		return 1;
	}
	else
		return 0;
}

byte PlayerSavePush(void)
{
	player.pushPower+=2;
	if(player.pushPower>=5)
	{
		player.pushPower=0;
		return 1;
	}
	else
		return 0;
}

byte GetPlayerGlow(void)
{
	return playerGlow;
}

void SetPlayerGlow(byte v)
{
	playerGlow=v;
}

byte GetTportClock(void)
{
	return tportclock;
}

void SetTportClock(byte tp)
{
	tportclock=tp;
}

void DoPlayerFacing(byte c,Guy *me)
{
	if(c&CONTROL_UP)
	{
		me->facing=6;
		if(c&CONTROL_LF)
			me->facing=5;
		else if(c&CONTROL_RT)
			me->facing=7;
	}
	else if(c&CONTROL_DN)
	{
		me->facing=2;
		if(c&CONTROL_LF)
			me->facing=3;
		else if(c&CONTROL_RT)
			me->facing=1;
	}
	else
	{
		if(c&CONTROL_LF)
			me->facing=4;
		else if(c&CONTROL_RT)
			me->facing=0;
	}
}

void PlayerControlMe(Guy *me,Map *map,mapTile_t *mapTile,world_t *world)
{
	if(player.stamina<player.maxStamina/2)
		TipEvent(TIP_STAMINA);

	if(tportclock)
		tportclock--;

	if(me->stun>0)
		LoonyControlStunned(me,mapTile,world);
	else if(me->frozen && player.parka.magic!=EM_COOLGUY && (me->frozen%4)!=0 && me->ouch!=4)
		LoonyControlStunned(me,mapTile,world);
	else
		LoonyControlMe(me,mapTile,world);
}

void PlayerCalcStats(void)
{
	player.damage=player.axe.value+SkillDamageBonus();
	player.armor=player.parka.value+SkillArmorBonus();
	if(GlassEffect()==GE_DAMAGE)
		player.damage+=GlassAmount();

	player.axeSpeed=player.axe.speed+SkillSpeedBonus();
	if(GlassEffect()==GE_SPEED)
		player.axeSpeed+=GlassAmount();

	player.maxHearts=player.startHearts+SkillLifeBonus()+player.level*2;
	if(player.parka.magic==EM_VIBRANT)
		player.maxHearts+=5;
	if(GlassEffect()==GE_LIFE)
		player.maxHearts+=GlassAmount();

	player.maxHearts=(player.maxHearts*(100+player.var[VAR_MADGEM+MC_LIFE]*10)/100);

	player.maxStamina=player.startHearts+player.parka.speed+player.level;
	if(player.axe.magic==EM_BEEFY)
		player.maxStamina+=5;
	if(GlassEffect()==GE_STAMINA)
		player.maxStamina+=GlassAmount();

	player.maxMagic=player.amulet.value+player.startMagic+SkillMagicBonus()+player.level;
	if(player.amulet.magic==EM_POWERFUL)
		player.maxMagic+=5;

	if(GlassEffect()==GE_MAGIC)
		player.maxMagic+=GlassAmount();

	if(player.axe.magic==EM_MAGICAL)
	{
		player.axeSpeed-=3;
		if(player.axeSpeed<1 || player.axeSpeed>250)
			player.axeSpeed=1;
		player.maxMagic+=10;
	}
	if(player.axe.magic==EM_DEFEND)
	{
		player.damage-=3;
		if(player.damage<1 || player.damage>250)
			player.damage=1;
		player.armor+=3;
	}
	if(player.parka.magic==EM_VIOLENT)
	{
		player.armor-=3;
		if(player.armor>250)
			player.armor=0;
		player.damage+=3;
	}
	if(player.parka.magic==EM_SICKENING)
	{
		player.maxHearts-=10;
		if(player.maxHearts<5 || player.maxHearts>65520)
			player.maxHearts=5;
	}
	if(player.parka.magic==EM_ITCHY)
	{
		player.armor-=3;
		if(player.armor>250)
			player.armor=0;
		player.maxMagic+=10;
	}
	if(player.amulet.magic==EM_ANGRY)
	{
		player.maxMagic-=3;
		if(player.maxMagic>250 || player.maxMagic==0)
			player.maxMagic=1;
		player.damage+=2;
	}
	if(player.amulet.magic==EM_SOLID)
	{
		player.armor+=3;
	}
	if(player.amulet.magic==EM_CURSED)
	{
		player.armor+=2;
	}
	if(GlassEffect()==GE_ARMOR)
		player.armor+=GlassAmount();

	if(player.magic>player.maxMagic)
		player.magic=player.maxMagic;
	if(player.hearts>player.maxHearts)
		player.hearts=player.maxHearts;
	if(player.stamina>player.maxStamina)
		player.stamina=player.maxStamina;

	SetupAxeBullet();
	monsType[MONS_AXETRAP].hp=(int)SpecificSkillVal(0,SKILL_TURRET);
	monsType[MONS_AXETRAP].armor=(int)SpecificSkillLevel(SKILL_TURRET);
	monsType[MONS_BONEHEAD].hp=5*(int)SpecificSkillVal(0,SKILL_BONEHEAD);
	if(monsType[MONS_BONEHEAD].hp==0)
		monsType[MONS_BONEHEAD].hp=1;
	monsType[MONS_BONEHEAD].speed=(int)((8*FIXAMT*SpecificSkillVal(1,SKILL_BONEHEAD))/256.0f);
	monsType[MONS_BONEHEAD].armor=(byte)SpecificSkillVal(0,SKILL_BONEHEAD)*2;

	monsType[MONS_YOURBOT].hp=20+5*player.var[VAR_CLOCKBOT+IT_LOG-IT_LOG];
	monsType[MONS_YOURBOT].armor=2+player.var[VAR_CLOCKBOT+IT_STONE-IT_LOG]*2;
	monsType[MONS_YOURBOT].damage=2+player.var[VAR_CLOCKBOT+IT_SOCK-IT_LOG];

	if(player.axeMode==1 && SpecificSkillLevel(SKILL_THROWING)==0)
		player.axeMode=0;
}

void PlayerBackToLastTown(byte *mapNum)
{
	player.destx=0;
}

void MinedSpot(Map *map,int x,int y)
{
	int sx,sy;
	byte n,t;
	bullet_t *b;

	sx=(x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
	sy=(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
	ExplodeParticles2(PART_DIRT,sx,sy,0,30,6);
	MakeNormalSound(SND_DIG);
	while(1)
	{
		n=rand()%100;
		t=IT_NONE;
		if(n<20)	// 20% chance of silver
			t=IT_SILVER;
		else if(n<20+10)	// 10% chance of gold
			t=IT_GOLD;
		else if(n<20+10+5)	// 5% chance of ruby
			t=IT_RUBY;
		else if(n<20+10+5+2)	// 2% chance of emerald
			t=IT_EMERALD;
		else if(n<20+10+5+2+1)	// 1% chance of diamond
			t=IT_DIAMOND;
		// leaving 62% chance of JACK
		if(t==IT_NONE && (rand()%100)<(byte)TalentBonus(TLT_MINING))
			continue;	// give it another try if it failed and your mining skill allows it

		if(t==IT_NONE)
			break;

		TalentPoint(TLT_MINING,1);
		b=FireBullet(goodguy->ID,sx,sy,0,rand()%256,BLT_ITEM,0);
		if(b)
			b->anim=t;

		if((rand()%100)<(byte)TalentBonus(TLT_MINING))
			continue;	// chance of redigging even if you got an item, based on the talent!
		else
			break;
	}
}

void Dig(Map *map,int x,int y)
{
	int i,j;
	int xx,yy;
	bullet_t *b;

	if(!goodguy)
		return;

	for(i=x-1;i<=x+1;i++)
		for(j=y-1;j<=y+1;j++)
		{
			if(i>=0 && j>=0 && i<map->width && j<map->height)
			{
				xx=(i*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
				yy=(j*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
				if(map->map[i+j*map->width].item==IT_BURIEDASB)
				{
					MakeNormalSound(SND_DIG);
					player.holesDug++;
					map->map[i+j*map->width].item=IT_NONE;
					ExplodeParticles2(PART_SNOW2,xx,yy,2,20,2);
					b=FireBullet(goodguy->ID,xx,yy,FIXAMT*15,rand()%256,BLT_ITEM,0);
					if(b)
					{
						b->anim=IT_HERBA;
					}
				}
				else if(map->map[i+j*map->width].item==IT_BURIEDSTONE)
				{
					MakeNormalSound(SND_DIG);
					player.holesDug++;
					map->map[i+j*map->width].item=IT_NONE;
					ExplodeParticles2(PART_SNOW2,xx,yy,2,20,2);
					b=FireBullet(goodguy->ID,xx,yy,FIXAMT*15,rand()%256,BLT_ITEM,0);
					if(b)
					{
						b->anim=IT_SHARPSTONE;
					}
				}
				else if(map->map[i+j*map->width].item==IT_CRACK || map->map[i+j*map->width].item==IT_CRACKUNDER)
				{
					player.holesDug++;
					map->map[i+j*map->width].item=IT_NONE;
					MinedSpot(map,i,j);
				}
				else if(map->map[i+j*map->width].item==IT_SUPCRACK)
				{
					player.holesDug++;
					map->map[i+j*map->width].item=IT_NONE;
					ExplodeParticles2(PART_DIRT,xx,yy,0,30,6);
					MakeNormalSound(SND_DIG);
					b=FireBullet(goodguy->ID,xx,yy,FIXAMT*15,rand()%256,BLT_ITEM,0);
					if(b)
					{
						b->anim=IT_SUPPLIES+(rand()%2);
					}
				}
			}
		}

	i=FindVictim((x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,goodguy->mapx,goodguy->mapy,64,goodguy->ID+1,GOOD,0,map,&curWorld);
	if(i!=0)
	{
		if(GetGuy(i-1)->tag==19 && GetGuy(i-1)->type==MONS_VILLAGER)
		{
			MakeNormalSound(SND_DIG);
			player.holesDug++;

			GetGuy(i-1)->tag=20;
			FXRing(0,GetGuy(i-1)->x,GetGuy(i-1)->y,0,16,3);
			FXRing(0,GetGuy(i-1)->x,GetGuy(i-1)->y,FIXAMT*20,16,4);
			FXRing(0,GetGuy(i-1)->x,GetGuy(i-1)->y,FIXAMT*40,16,5);
			FXRing(0,GetGuy(i-1)->x,GetGuy(i-1)->y,FIXAMT*60,16,6);
		}
	}

}

void PlayerGetMoney(int amt)
{
	if(player.money+amt<MAX_MONEY)
		player.money+=amt;
	else
		player.money=MAX_MONEY;
	player.gemsGotten+=amt;
}

void PlayerAddKill(byte type)
{
	word w;

	w=player.var[VAR_KILLCOUNTS+type];
	w+=(player.var[VAR_KILLCOUNTS+type+1]*256);

	if(w<65535)
		w++;

	// check here for achievements
	if(type==KC_PLANT && w>=5000)
		AchieveNow(18);
	if(type==KC_MONKEY && w>=5000)
		AchieveNow(36);
	if(type==KC_PUPPET && w>=5000)
		AchieveNow(45);
	if(type==KC_BODZHA && w>=5000)
		AchieveNow(54);
	if(type==KC_GHOST && w>=2000)
		AchieveNow(63);
	if(type==KC_BEAR && w>=5000)
		AchieveNow(72);
	if(type==KC_ICE && w>=1000)
		AchieveNow(81);
	if(type==KC_MICE && w>=5000)
		AchieveNow(90);
	player.kills++;
#ifdef DIRECTORS
	if(type==KC_ONION && w>=500)
	{
		picToDrop=1;
		if(opt.galPix[picToDrop-1]==0 && player.var[VAR_TEMP+15]==0)
		{
			bullet_t *b;

			b=FireBullet(goodguy->ID,goodguy->x,goodguy->y,FIXAMT*15,64,BLT_ITEM,0);
			if(b)
			{
				player.var[VAR_TEMP+15]=1;
				b->anim=IT_PICTURE;
				b->target=picToDrop;
				NewMessage("500 Gangsters beaten!",0);
			}
		}
		picToDrop=0;
	}
	if(type==KC_BUNNY && w>=500)
	{
		picToDrop=3;
		if(opt.galPix[picToDrop-1]==0 && player.var[VAR_TEMP+15]==0)
		{
			bullet_t *b;

			b=FireBullet(goodguy->ID,goodguy->x,goodguy->y,FIXAMT*15,64,BLT_ITEM,0);
			if(b)
			{
				player.var[VAR_TEMP+15]=1;
				b->anim=IT_PICTURE;
				b->target=picToDrop;
				NewMessage("500 Knights beaten!",0);
			}
		}
		picToDrop=0;
	}
	if(player.kills>=10000)
	{
		picToDrop=4;
		if(opt.galPix[picToDrop-1]==0 && player.var[VAR_TEMP+15]==0)
		{
			bullet_t *b;

			b=FireBullet(goodguy->ID,goodguy->x,goodguy->y,FIXAMT*15,64,BLT_ITEM,0);
			if(b)
			{
				player.var[VAR_TEMP+15]=1;
				b->anim=IT_PICTURE;
				b->target=picToDrop;
				NewMessage("10,000 monsters slain!",0);
			}
		}
		picToDrop=0;
	}
#endif
	player.var[VAR_KILLCOUNTS+type]=(byte)(w%256);
	player.var[VAR_KILLCOUNTS+type+1]=(byte)(w/256);
}

#include "player.h"
#include "string_extras.h"
#include "options.h"
#include "title.h"
#include "quest.h"
#include "slingshot.h"
#include "badge.h"
#include "bossbash.h"
#include "randomizer.h"
#include "loonyArchipelago.h"

// characters
#include "ch_loony.h"
#include "ch_toad.h"
#include "ch_bonkula.h"
#include "ch_swampdog.h"
#include "ch_witch.h"
#include "ch_wolf.h"
#include "ch_summon.h"
#include "ch_girl.h"

player_t player;
byte playerGlow;	// for torch-lit levels, and for exciting moments
byte tportclock;

void InitPlayer(byte initWhat,byte world,byte level)
{
	int i;
	static const byte startLife[]={15,10,5,3,1,5};
	static_assert(std::size(startLife) == NUM_DIFFICULTY);

	if(initWhat==INIT_GAME)	// initialize everything, this is to start a whole new game
	{
		player.destx=0;
		player.desty=0;
		player.money=0;
		player.maxMoney=50;
		player.firePower=0;
		player.fireRate=0;
		player.fireRange=0;
		player.fireFlags=0;
		player.worldNum=world;
		player.weapon=ITM_NONE;
		player.wpnCost=0;
		player.lastSave=255;
		player.cheatsOn=0;

		for(i=0;i<256;i++)
			player.var[i]=0;
		for(i=0;i<4;i++)
			player.keys[i]=0;

		player.timeLimit=0;
		// end-game stats
		player.monsterPoints=0;
		player.bestCombo=0;
		player.shotsFired=0;
		player.specialShotsFired=0;
		player.guysKilled=0;
		player.gemsGotten=0;
		player.numSaves=0;
		player.hitsTaken=0;
		player.timeToFinish=0;
		player.batLevel=0;
		player.xtraVar=0;
		player.crystalBall=0;
		player.xtraByte=0;

		if(opt.cheats[CH_HARDCORE] && (world == WORLD_NORMAL || world == WORLD_REMIX))
			player.cheatsOn|=PC_HARDCORE;

		if(opt.cheats[CH_ALLACCESS])
		{
			player.cheatsOn|=PC_ALLACCESS;
			player.var[VAR_VAMPYWALL]=1;
			player.var[VAR_TOWNOPEN] = 1;
			player.var[VAR_PAIDBRIDGE] = 1;
			player.var[VAR_BRIDGEOPEN] = 1;
			player.var[VAR_GATEOUT] = 1;
			player.keys[0] = 9;

			if (!ArchipelagoMode)
			{
				player.var[VAR_KEY] = 1;
				player.var[VAR_KEY + 1] = 1;
				player.var[VAR_KEY + 2] = 1;
				player.var[VAR_FERTILIZER] = 1;
				player.var[VAR_STICK] = 1;
				player.var[VAR_TORCH] = 1;
				for (i = 0; i < 4; i++)
				{
					player.var[VAR_MYSORB + i] = 1;
					player.var[VAR_BATSTATUE + i] = 1;
				}
				for (i = 0; i < 8; i++)
					player.var[VAR_VAMPBUST + i] = 1;
				player.var[VAR_TALISMAN] = 1;
				player.var[VAR_BOOTS] = 1;
				player.var[VAR_WEAPON + 0] = 1;	// bombs, need those to get in places
				player.keys[1] = 1;
				player.keys[2] = 1;
				player.keys[3] = 1;
			}
		}
		if(opt.cheats[CH_RAPIDFIRE] && !ArchipelagoMode) //todo figure out what to do here
		{
			// start with all arrows
			player.fireRate=10;
			for(i=0;i<10;i++)
				player.var[VAR_PANTS+i]=1;
		}

		player.difficulty=opt.difficulty;

		player.hearts=startLife[player.difficulty];
		player.startHearts=player.hearts;
		player.maxHearts=player.hearts;
		player.maxMaxHearts=player.hearts+20;

		player.monsType=MONS_LOONY;
		if(opt.cheats[CH_BONKULA])
		{
			player.monsType=MONS_PLYRBONKULA;
			player.cheatsOn|=PC_PLAYBONK;
			player.hearts+=5;
			player.maxHearts+=5;
			player.startHearts+=5;
			player.maxMaxHearts+=5;
		}
		if(opt.cheats[CH_TOAD])
		{
			player.monsType=MONS_LOONYTOAD;
			player.cheatsOn|=PC_PLAYTOAD;
		}
		if(opt.cheats[CH_SWAMPDOG])
		{
			player.monsType=MONS_PLYRSWAMPDOG;
			player.cheatsOn|=PC_PLAYDOG;
			player.xtraByte=0;
			player.xtraVar=SD_LEVELUP;
		}
		if(opt.cheats[CH_SUMMON])
		{
			player.monsType=MONS_PLYRSUMMON;
			player.cheatsOn|=PC_PLAYSUMMON;
			player.xtraByte=0;
			// start with 5 extra hearts, but never gain any more
			player.hearts+=5;
			player.startHearts=player.hearts;
			player.maxHearts=player.hearts;
			player.maxMaxHearts=player.hearts;
			InitSummons();
		}
		if(opt.cheats[CH_THIEF])
		{
			player.monsType=MONS_PLYRTHIEF;
			player.cheatsOn|=PC_PLAYTHIEF;
			for(i=0;i<8;i++)
				player.spellXP[i]=0;
			player.xtraByte=1;
			player.xtraVar=0;
		}

		if(opt.cheats[CH_WEREWOLF])
		{
			player.monsType=MONS_LOONY2;
			player.cheatsOn|=PC_PLAYWOLF;
			player.xtraVar=0;
		}
		if(opt.cheats[CH_WITCH])
		{
			player.monsType=MONS_PLYRWITCH;
			player.cheatsOn|=PC_PLAYWITCH;
			player.xtraByte=0;	// which spell you are using
			for(i=0;i<8;i++)
				player.spellXP[i]=0;
			player.xtraVar=32*FIXAMT;	// current mana
				// max mana is (arrows+1)*32*FIXAMT
		}

		// survival specials
		if(player.worldNum==WORLD_SURVIVAL)
		{
			for(i=0;i<7;i++)
				player.var[VAR_WEAPON+i]=1;
			player.var[VAR_POTION]=1;
			player.var[VAR_QUESTASSIGN+QUEST_SILVER]=1;
			player.var[VAR_QUESTDONE+QUEST_SILVER]=1;
			player.var[VAR_SILVERSLING] = 1;
			player.maxMoney=200;
		}
		else if(player.worldNum==WORLD_SLINGSHOT)
		{
			player.firePower=1;
		}
		else if(player.worldNum==WORLD_BOSSBASH)
		{
			player.var[VAR_QUESTASSIGN+QUEST_SILVER]=1;
			player.var[VAR_QUESTDONE+QUEST_SILVER]=1;
			player.var[VAR_SILVERSLING] = 1;
			player.var[VAR_POTION]=1;
			player.var[VAR_TORCH]=1;
			for(i=0;i<7;i++)
				player.var[VAR_WEAPON+i]=1;

			player.firePower=BashPower()*2;
			player.fireRate=BashPower()*2;
			player.fireRange=BashPower()*2;
			player.maxMoney=50+BashPower()*25+25*(BashPower()==5);
			if(player.maxMoney>200)
				player.maxMoney=200;

			if(player.monsType!=MONS_PLYRSUMMON)
			{
				player.maxHearts+=BashPower()*4;
				player.hearts=player.maxHearts;
			}
			if(BashPower()>0)
				player.money=player.maxMoney/2;
			if(BashPower()==5)
				player.money=player.maxMoney;
			if(BashPower()>3)
			{
				player.var[VAR_HELPERBAT]=1;
				player.var[VAR_QUESTASSIGN+QUEST_FARLEY]=1;
				player.var[VAR_QUESTDONE+QUEST_FARLEY]=1;
				player.fireFlags|=FF_HELPER|FF_REFLECT;
				player.var[VAR_REFLECT]=1;
				if(BashPower()==5)
				{
					player.var[VAR_TRIPLEFIRE]=1;
					player.fireFlags|=FF_TRIPLE;
					player.batLevel=255;
				}
			}
			player.weapon=ITM_WBOMB+(byte)Random(7);
			player.wpnCost=WeaponCost(player.weapon,0);
			player.wpnLevel=0;
			if(player.monsType==MONS_PLYRWITCH)
			{
				switch(BashPower())
				{
					case 0:
						break;
					case 1:
						for(i=0;i<8;i++)
							player.spellXP[i]=150;
						break;
					case 2:
						for(i=0;i<8;i++)
							player.spellXP[i]=900;
						break;
					case 3:
						for(i=0;i<8;i++)
							player.spellXP[i]=2500;
						break;
					case 4:
						for(i=0;i<8;i++)
							player.spellXP[i]=4000;
						break;
					case 5:
						for(i=0;i<8;i++)
							player.spellXP[i]=10000;
						break;
				}
				player.xtraByte=0;
				player.xtraVar=(player.fireRange+1)*32*FIXAMT;
			}
			if(player.monsType==MONS_PLYRSUMMON)
			{
				switch(BashPower())
				{
					case 0:
						break;
					case
						1:
						player.xtraByte=4;
						break;
					case 2:
						player.xtraByte=8;
						break;
					case 3:
						player.xtraByte=12;
						break;
					case 4:
						player.xtraByte=16;
						break;
					case 5:
						player.xtraByte=20;
						break;
				}
				player.weapon=0;
			}
		}

		if(opt.cheats[CH_RNDSURVIVAL])
			player.cheatsOn|=PC_INFSURV;

		if(opt.cheats[CH_MAXPOWER])
		{
			if (ArchipelagoMode)
			{
				//todo figure out what to do here
			}
			else
			{
				player.cheatsOn |= PC_MAXPOWER;
				// crank it up
				for (i = 0; i < 10; i++)
				{
					player.var[VAR_LIGHTNING + i] = 1;
					player.var[VAR_ARROW + i] = 1;
					player.var[VAR_PANTS + i] = 1;
				}
				player.var[VAR_REFLECT] = 1;
				player.var[VAR_TRIPLEFIRE] = 1;
				for (i = 0; i < 7; i++)
					player.var[VAR_WEAPON + i] = 1;
				player.var[VAR_POTION] = 1;
				if (player.worldNum != WORLD_RANDOMIZER) {
					player.var[VAR_QUESTASSIGN + QUEST_SILVER] = 1;
					player.var[VAR_QUESTDONE + QUEST_SILVER] = 1;
				}
				player.var[VAR_SILVERSLING] = 1;
				player.var[VAR_HELPERBAT] = 1;
				player.var[VAR_QUESTASSIGN + QUEST_FARLEY] = 1;
				player.var[VAR_QUESTDONE + QUEST_FARLEY] = 1;
				player.batLevel = 255;
				player.firePower = 10;
				player.fireRate = 10;
				player.fireRange = 10;
				player.fireFlags |= FF_REFLECT | FF_TRIPLE | FF_HELPER;

				if (player.monsType == MONS_PLYRSWAMPDOG)
				{
					player.xtraByte = 50;
					player.xtraVar = SD_LEVELUP * 50;
				}
				if (player.monsType == MONS_PLYRWITCH)
				{
					for (i = 0; i < 8; i++)
						player.spellXP[i] = 30000;
				}
			}
		}

		if(opt.cheats[CH_TERROR])
		{
			player.cheatsOn|=PC_TERROR;
			// set up terror mode, major advantages here
			if(player.monsType!=MONS_PLYRSUMMON)
			{
				player.maxMaxHearts+=20;
				player.hearts+=20;
				player.maxHearts+=20;
				player.startHearts+=20;
			}
			else
				player.xtraByte=20;
			if (ArchipelagoMode)
			{

			}
			else
			{
				player.var[VAR_REFLECT] = 1;
				player.var[VAR_TRIPLEFIRE] = 1;
				player.var[VAR_HELPERBAT] = 1;
				player.var[VAR_QUESTASSIGN + QUEST_FARLEY] = 1;
				player.var[VAR_QUESTDONE + QUEST_FARLEY] = 1;
				player.batLevel = 255;
				player.firePower = 10;
				player.fireRate = 10;
				player.fireRange = 10;
				player.fireFlags |= FF_REFLECT | FF_TRIPLE | FF_HELPER;
			}

			if(player.monsType==MONS_PLYRSWAMPDOG)
			{
				player.xtraByte=50;
				player.xtraVar=SD_LEVELUP*player.xtraByte;
			}
			if(player.monsType==MONS_PLYRWITCH)
			{
				for(i=0;i<8;i++)
					player.spellXP[i]=30000;
			}
		}
		if(player.worldNum==WORLD_BOWLING)
		{
			player.firePower=0;
			player.fireRange=0;
			player.fireRate=0;
			player.monsterPoints=0;
		}
		if (player.worldNum == WORLD_RANDOMIZER) {
			memcpy(&player.var[VAR_SEEDSTART], GetSeed().data(), MAX_SEED_LENGTH);
		}

		if(player.monsType==MONS_PLYRSWAMPDOG)
			FakeLevelUp();
	}
	if(initWhat>=INIT_WORLD) // initialize the things that go with each world
	{
		ArchipelagoLoadPlayer();
	}

	player.levelNum=level;
	player.clockRunning=0;
	player.boredom=0;
	player.reload=0;
	player.wpnReload=0;
	player.shield=0;
	playerGlow=0;
	player.pushPower=0;
	player.poison=0;
	player.speed=0;
	player.chatClock=0;
	player.stone=0;
	player.combo=0;
	player.comboClock=0;
	player.timeLimit=0;
	player.fireFlags&=(~FF_HELPERHERE);
	if(opt.cheats[CH_HOMING])
		player.fireFlags|=FF_HOMING;
	else
		player.fireFlags&=(~FF_HOMING);
	if(opt.cheats[CH_GUIDED])
		player.fireFlags|=FF_GUIDED;
	else
		player.fireFlags&=(~FF_GUIDED);
	player.areaName[0]='\0';
	
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

	if(!(player.cheatsOn&PC_ALLACCESS))
		player.keys[0]=0;
}

float CalcPercent(const player_t *p)
{
	int i,totalpoints,gotpoints;

	gotpoints=0;
	totalpoints=0;

	for(i=0;i<=135;i++)
	{
		totalpoints++;
		if(p->var[i])
			gotpoints++;
		else
		{
			i=i;
		}
	}
	// for the quest completes, make them worth 5 times as much as other things
	for(i=70;i<=89;i++)
	{
		totalpoints+=4;
		if(p->var[i])
			gotpoints+=4;
		else
			i=i;
	}

	for(i=150;i<=153;i++)
	{
		totalpoints++;
		if(p->var[i])
			gotpoints++;
		else
			i=i;
	}
	for(i=157;i<=162;i++)
	{
		totalpoints++;
		if(p->var[i])
			gotpoints++;
		else
			i=i;
	}
	for(i=164;i<=179;i++)
	{
		totalpoints++;
		if(p->var[i])
			gotpoints++;
		else
			i=i;
	}
	for(i=181;i<=197;i++)
	{
		totalpoints++;
		if(p->var[i])
			gotpoints++;
		else
			i=i;
	}
	for(i=200;i<=200;i++)
	{
		totalpoints++;
		if(p->var[i])
			gotpoints++;
		else
			i=i;
	}
	for(i=204;i<=209;i++)
	{
		totalpoints++;
		if(p->var[i])
			gotpoints++;
		else
			i=i;
	}
	for(i=211;i<=211;i++)
	{
		totalpoints++;
		if(p->var[i])
			gotpoints++;
		else
			i=i;
	}
	return (float)(100.0*gotpoints)/(float)totalpoints;
}

byte PlayerShield(void)
{
	return player.shield;
}

byte PlayerHasHammer(void)
{
	return 1;
}

byte PlayerIsPoisoned(void)
{
	return player.poison;
}

void PoisonPlayer(byte amt)
{
	if(player.shield)
		return;
	if(player.poison)
		return;
	if(opt.cheats[CH_SUPERSURV])
		return;

	if(player.poison+amt>255)
		player.poison=255;
	else
		player.poison+=amt;
	player.invinc=30*3;
}

void PlayerWinLevel(byte w,byte l,byte isSecret)
{
}

void SetPlayerHP(int hp)
{
	if(hp<player.hearts)
		player.hitsTaken+=player.hearts-hp;
	player.hearts=(byte)hp;
}

void PlayerSetVar(int v,int val)
{
	int oldval;
	char m[32];

	oldval=player.var[v];
	player.var[v]=val;

	if (DataStorageVars.find(v) != DataStorageVars.end())
	{
		//SendArchipelagoPlayerVar(v, val);
	}

	// this is a quest completion variable
	if(v>=VAR_QUESTDONE && v<VAR_QUESTDONE+NUM_QUESTS)
	{
		if(oldval==0 && val)
		{
			if (ArchipelagoMode)
			{
				SendCheckedLocQuest(v - VAR_QUESTDONE);
			}

			// didn't previously complete it
			sprintf(m,"%s Complete!",QuestName(v-VAR_QUESTDONE));
			NewBigMessage(m,90);
			if(v!=VAR_QUESTDONE+QUEST_HILL)	// no sound for this one
				MakeNormalSound(SND_FINISHQUEST);

			if(player.var[VAR_QUESTASSIGN+(v-VAR_QUESTDONE)]==0)
				player.var[VAR_QUESTASSIGN+(v-VAR_QUESTDONE)]=1;
			if(v==VAR_QUESTDONE+QUEST_ORBS)
			{
				MakeNormalSound(SND_GRATE);
				ShakeScreen(60);	// earthquake for raising of lake
			}
			if(v==VAR_QUESTDONE+QUEST_HILL)
			{
				if (ArchipelagoMode)
				{
					WinArchipelago();
				}
				SendMessageToGame(MSG_WINGAME,0);
			}
		}
	}
	if(v>=VAR_QUESTASSIGN && v<VAR_QUESTASSIGN+NUM_QUESTS)
	{
		if(oldval==0 && val)
		{
			// hasn't been previously assigned
			sprintf(m,"%s!",QuestName(v-VAR_QUESTASSIGN));
			NewBigMessage(m,90);
			player.var[v]=val;
			MakeNormalSound(SND_GETQUEST);
		}
	}
	if(v>=VAR_WEAPON && v<VAR_WEAPON+7)
	{
		if(oldval==0 && val)
		{
			// got a weapon for the first time
			if(player.monsType==MONS_PLYRWITCH)
			{
				sprintf(m,"Learned %s Spell!",SpellName(v-VAR_WEAPON+1));
			}
			else if(player.monsType==MONS_PLYRSUMMON)
			{
				sprintf(m,"Learned %s Summon!",SummonName(v-VAR_WEAPON+1));
			}
			else if(player.monsType==MONS_PLYRTHIEF)
			{
				sprintf(m,"Learned %s Skill!",BombName(v-VAR_WEAPON+1));
			}
			else
				sprintf(m,"%s Equipped!",WeaponName(v-VAR_WEAPON));
			NewBigMessage(m,90);
		}
	}
	if(v>=VAR_MUSHROOM && v<VAR_MUSHROOM+10)
	{
		MakeNormalSound(SND_MUSHGET);
		if(ShroomCount()==10)
		{
			PlayerSetVar(VAR_QUESTDONE+QUEST_SHROOM,1);
		}
		else
		{
			if(ShroomCount()<9)
				sprintf(m,"%d Mushrooms left!",10-ShroomCount());
			else
				sprintf(m,"Just one more Mushroom!");
			NewBigMessage(m,90);
		}
	}
	if(v>=VAR_MYSORB && v<VAR_MYSORB+4)
	{
		MakeNormalSound(SND_KEYGET);
		NewBigMessage("Mystery Orb obtained!",90);
	}
	if(v>=VAR_ORBSTAND && v<VAR_ORBSTAND+4)
	{
		NewBigMessage("Mystery Orb placed!",90);
	}
	if(v>=VAR_BATSTATUE && v<VAR_BATSTATUE+4)
	{
		MakeNormalSound(SND_KEYGET);
		NewBigMessage("Bat Statue obtained!",90);
	}
	if(v>=VAR_VAMPBUST && v<VAR_VAMPBUST+8)
	{
		MakeNormalSound(SND_KEYGET);
		NewBigMessage("Vampire Bust obtained!",90);
	}

	if(v==VAR_CAT)
	{
		if(oldval==0 && val)
		{
			// got the cat!
			if(player.var[VAR_QUESTASSIGN+QUEST_CAT]==1)
			{
				MakeNormalSound(SND_ZOMBIESDEAD);
				NewBigMessage("Found a lost cat!",90);
			}
			else
				PlayerSetVar(VAR_QUESTASSIGN+QUEST_CAT,1);
		}
	}
	if(v==VAR_FERTILIZER)
	{
		if(oldval==0 && val)
		{
			MakeNormalSound(SND_POWERUP);
			NewBigMessage("Got Super Grow Fertilizer!",90);
		}
	}
	if(v==VAR_REFLECT)
	{
		if(oldval==0 && val)
		{
			MakeNormalSound(SND_POWERUP);
			NewBigMessage("Got Reflect Gem!",90);
			player.fireFlags|=FF_REFLECT;
		}
	}
	if (v == VAR_TRIPLEFIRE && oldval == 0)
	{
		player.fireFlags |= FF_TRIPLE;
	}
	if(v>=VAR_ZOMBIE && v<=VAR_ZOMBIE+2)
	{
		if(oldval==0 && val)
		{
			NewBigMessage("Crypt Cleansed!",90);
			KillAllMonsters(MONS_ZOMBIE);
			KillAllMonsters(MONS_ZOMBIE2);
			curMap->flags|=MAP_LITDUNGEON;	// light it up!
			curMap->flags&=(~MAP_TORCHLIT);	// light it up!
			MakeNormalSound(SND_ZOMBIESDEAD);
			if(player.var[VAR_ZOMBIE]+player.var[VAR_ZOMBIE+1]+player.var[VAR_ZOMBIE+2]==3)
				PlayerSetVar(VAR_QUESTDONE+QUEST_ZOMBIES,1);
		}
	}
	if(v==VAR_DAISY)
	{
		if (!ArchipelagoMode)
		{
			PlayerSetVar(VAR_QUESTDONE + QUEST_DAISY, 1);
		}	
	}
	if(v==VAR_TOWNOPEN && oldval==0)
	{
		MakeNormalSound(SND_ZOMBIESDEAD);
	}
	if(v==VAR_HELPERBAT && oldval==0)
	{
		MakeNormalSound(SND_ZOMBIESDEAD);
		NewBigMessage("Farley has joined you!",90);
	}
	if(v==VAR_SILVER && oldval==0)
	{
		MakeNormalSound(SND_KEYGET);
		NewBigMessage("Got a bar of silver!",90);
	}
	if (v == VAR_SKULLKEY && oldval == 0)
	{
		player.keys[1] = 1;
	}
	if (v == VAR_BATKEY && oldval == 0)
	{
		player.keys[2] = 1;
	}
	if (v == VAR_PUMPKINKEY && oldval == 0)
	{
		player.keys[3] = 1;
	}
	BadgeCheck(BE_VARSET,0,curMap);
}

byte WeaponCost(byte wpn,byte level)
{
	static const byte wpnCost[7][3]={
		{3,7,15},	// bombs
		{1,4,8},	// lightning
		{3,7,12},	// ice
		{1,2,5},	// cactus
		{4,8,16},	// boomerang
		{3,7,20},	// whoopee
		{1,5,10},	// hot pants
	};

	if (wpn < ITM_WBOMB || wpn > ITM_WHOTPANTS)
		return 0;
	else if (level==2 && opt.cheats[CH_ULTRAWEAPON])
		return wpnCost[wpn-ITM_WBOMB][level]*3;
	else
		return wpnCost[wpn-ITM_WBOMB][level];
}

// true if the item was really gotten, false if it went through (weapon lock)
bool PlayerGetWeapon(byte wpn)
{
	if(player.monsType==MONS_PLYRWITCH)
	{
		MakeNormalSound(SND_WPNGET);
		PowerupSpell(wpn-ITM_WBOMB+1,5);
		player.wpnLevel=0;
	}
	else if(player.cheatsOn&PC_PLAYWOLF)
	{
		MakeNormalSound(SND_WPNGET);
		player.money++;
		player.gemsGotten++;
		BadgeCheck(BE_GEMSUP,0,curMap);
		if(player.money>player.maxMoney)
		{
			player.money=player.maxMoney;
		}
	}
	else if(player.monsType==MONS_PLYRSUMMON)
	{
		MakeNormalSound(SND_WPNGET);
		player.money++;
		player.gemsGotten++;
		BadgeCheck(BE_GEMSUP,0,curMap);
		if(player.money>player.maxMoney)
		{
			player.money=player.maxMoney;
		}
	}
	else
	{
		if((bool)(player.fireFlags&FF_WPNLOCK) != (bool)(GetControls() & CONTROL_B3) && player.var[VAR_WEAPON + wpn - ITM_WBOMB])
			return false;		// weapon lock prevents you from switching weapons

		MakeNormalSound(SND_WPNGET);

		if(player.weapon==wpn)
		{
			if(player.wpnLevel<2)
			{
				player.wpnLevel++;
				player.wpnCost=WeaponCost(player.weapon,player.wpnLevel);
			}
		}
		else
		{
			player.weapon=wpn;
			player.wpnLevel=0;
			player.wpnCost=WeaponCost(player.weapon,player.wpnLevel);
		}
	}
	return true;
}

void PlaceOrbOnStand(int x,int y)
{
	int i;
	byte t;
	byte orbs,usedOrbs;

	orbs=0;
	usedOrbs=0;
	for(i=0;i<4;i++)
	{
		if(player.var[VAR_MYSORB+i])
			orbs++;
		if(player.var[VAR_ORBSTAND+i])
			usedOrbs++;
	}
	if(orbs>usedOrbs)
	{
		// there is a free orb to place
		t=curMap->map[x+y*curMap->width].tag;
		PlayerSetVar(VAR_ORBSTAND+t,1);
		if(usedOrbs==3)
		{
			// this is the fourth orb!!
			PlayerSetVar(VAR_QUESTDONE+QUEST_ORBS,1);
		}
		curMap->TempTorch(x,y,32);
		MakeNormalSound(SND_ZOMBIESDEAD);

		// Instantly set the stand to filled rather than waiting on the special
		// to do it, in case Quick Mode is on and the player bumps twice.
		curMap->map[x+y*curMap->width].item = ITM_ORBSTAND2;
	}
}

void PlaceBatOnStand(int x,int y)
{
	int i;
	byte t;
	byte bats,usedBats;

	bats=0;
	usedBats=0;
	for(i=0;i<4;i++)
	{
		if(player.var[VAR_BATSTATUE+i])
			bats++;
		if(player.var[VAR_BATSTAND+i])
			usedBats++;
	}
	if(bats>usedBats)
	{
		// there is a free bat to place
		t=curMap->map[x+y*curMap->width].tag;
		PlayerSetVar(VAR_BATSTAND+t,1);
		curMap->TempTorch(x,y,32);
		MakeNormalSound(SND_GRATE);
	}
}

void PlaceVampOnStand(int x,int y)
{
	int i;
	byte t;
	byte vamps,usedVamps;

	vamps=0;
	usedVamps=0;
	for(i=0;i<8;i++)
	{
		if(player.var[VAR_VAMPBUST+i])
			vamps++;
		if(player.var[VAR_VAMPSTAND+i])
			usedVamps++;
	}
	if(vamps>usedVamps)
	{
		// there is a free bat to place
		t=curMap->map[x+y*curMap->width].tag-20;
		PlayerSetVar(VAR_VAMPSTAND+t,1);
		curMap->TempTorch(x,y,32);
		MakeNormalSound(SND_KEYGET);
		if(usedVamps==7)
		{
			// this is the last bust!
			player.var[240]=1;
		}
		curMap->map[x+y*curMap->width].item=ITM_BUSTONSTAND;
	}
}

void SuperHeal(void)
{
	NewBigMessage("MAX LIFE & GEMS!",90);
	player.hearts=player.maxHearts;
	player.money=player.maxMoney;
	goodguy->hp=player.hearts;
	MakeNormalSound(SND_POWERUP);
}

byte PlayerGetItem(byte itm,int x,int y)
{
	if(itm==ITM_ORBSTAND)
	{
		PlaceOrbOnStand(x,y);
	}
	if(itm==ITM_BATSTAND)
	{
		PlaceBatOnStand(x,y);
	}
	if(itm==ITM_BUSTSTAND)
	{
		PlaceVampOnStand(x,y);
	}
	if(ItemFlags(itm)&IF_GET)
	{
		if (ArchipelagoMode && ItemFlags(itm) & IF_ARCHIPELAGO) {
			if (itm < ITM_WBOMB || itm > ITM_WHOTPANTS) //weapons
			{
				if (player.var[itm - ITM_WBOMB + VAR_WEAPON] == 0) //let it process normally if its not your first
				{
					SendCheckedLocPickup(curMap->name, player.levelNum, x, y, itm);
					return 0;
				}
			}
			else if (itm > ITM_BATDOLL && itm <= ITM_WOLFDOLL) //dolls
			{
				SendCheckedLocDoll(itm - ITM_BATDOLL);
			}
			else //other ap
			{
				SendCheckedLocPickup(curMap->name, player.levelNum, x, y, itm);
				return 0;
			}
		}
		if(player.hearts==0)
			return 1;

		BadgeCheck(BE_GETITEM,itm,curMap);
		switch(itm)
		{
			case ITM_FIREPOWERUP:
				MakeRingParticle(goodguy->x,goodguy->y,0,32,100);
				if(player.monsType==MONS_PLYRSWAMPDOG || (player.cheatsOn&PC_TERROR))
				{
					SuperHeal();
				}
				else if(player.firePower<10)
				{
					if(player.monsType==MONS_PLYRWITCH)
						NewBigMessage("MAGIC RECOVERY UP!",90);
					else if(player.monsType==MONS_PLYRSUMMON)
						NewBigMessage("SUMMON STRENGTH UP!",90);
					else if(player.monsType==MONS_PLYRTHIEF)
						NewBigMessage("POWER-UP EXTEND!",90);
					else
						NewBigMessage("DAMAGE UP!",90);
					player.firePower++;
					MakeNormalSound(SND_POWERUP);
				}
				break;
			case ITM_RANGEUP:
				MakeRingParticle(goodguy->x,goodguy->y,0,32,100);
				if(player.monsType==MONS_PLYRSWAMPDOG || (player.cheatsOn&PC_TERROR))
				{
					SuperHeal();
				}
				else if(player.fireRange<10)
				{
					if(player.monsType==MONS_PLYRWITCH)
						NewBigMessage("MAX MAGIC UP!",90);
					else if(player.monsType==MONS_PLYRSUMMON)
						NewBigMessage("MAX SUMMONS UP!",90);
					else if(player.monsType==MONS_PLYRTHIEF)
						NewBigMessage("AMMO EXTEND!",90);
					else
						NewBigMessage("RANGE UP!",90);
					player.fireRange++;
					MakeNormalSound(SND_POWERUP);
				}
				break;
			case ITM_FIRERATEUP:
				MakeRingParticle(goodguy->x,goodguy->y,0,32,100);
				if(player.monsType==MONS_PLYRSWAMPDOG || (player.cheatsOn&PC_TERROR))
				{
					SuperHeal();
				}
				else if(player.fireRate<10)
				{
					if(player.monsType==MONS_PLYRWITCH)
						NewBigMessage("MAGIC RECOVERY UP!",90);
					else if(player.monsType==MONS_PLYRSUMMON)
						NewBigMessage("SUMMON SPEED UP!",90);
					else
						NewBigMessage("FIRE RATE UP!",90);
					player.fireRate++;
					MakeNormalSound(SND_POWERUP);
				}
				break;
			case ITM_TINYHEART:
				if(player.monsType==MONS_PLYRWITCH)
				{
					player.xtraVar+=FIXAMT*2;
					if(player.xtraVar>(dword)((player.fireRange+1)*32*FIXAMT))
						player.xtraVar=(player.fireRange+1)*32*FIXAMT;
					MakeNormalSound(SND_MAGICUP);
				}
				else if(player.hearts<player.maxHearts)
				{
					player.hearts++;
					goodguy->hp=player.hearts;
				}
				player.poison=0;
				break;
			case ITM_BIGHEART:
				if(player.monsType==MONS_PLYRWITCH)
				{
					player.xtraVar+=FIXAMT*10;
					if(player.xtraVar>(dword)((player.fireRange+1)*32*FIXAMT))
						player.xtraVar=(player.fireRange+1)*32*FIXAMT;
					MakeNormalSound(SND_MAGICUP);
				}
				else
				{
					player.hearts+=5;
					if(player.hearts>=player.maxHearts)
					{
						player.hearts=player.maxHearts;
					}
					goodguy->hp=player.hearts;
				}
				player.poison=0;
				break;
			case ITM_SUPERHEART:
				MakeRingParticle(goodguy->x,goodguy->y,0,32,100);
				if(player.monsType==MONS_PLYRSWAMPDOG || player.monsType==MONS_PLYRSUMMON)
				{
					SuperHeal();
					if(player.monsType==MONS_PLYRSUMMON && player.xtraByte<20)
					{
						player.xtraByte++;	// this is your 'number of hearts'
						NewBigMessage("SUMMON LIFE UP!",90);
					}
				}
				else
				{
					if(player.maxHearts<player.maxMaxHearts)
					{
						NewBigMessage("LIFE UP!",90);
						player.maxHearts++;
						MakeNormalSound(SND_POWERUP);
					}
					player.hearts=player.maxHearts;
					goodguy->hp=player.hearts;
					if(player.monsType==MONS_PLYRWITCH)
						player.xtraVar=(player.fireRange+1)*32*FIXAMT;
				}
				player.poison=0;
				break;
			case ITM_LITTLEGEM:
				if(player.money<player.maxMoney)
				{
					player.money++;
				}
				player.gemsGotten++;
				BadgeCheck(BE_GEMSUP,0,curMap);
				MakeNormalSound(SND_GEMGET);
				break;
			case ITM_BIGGEM:
				if(player.money<player.maxMoney)
				{
					player.money+=5;
					if(player.money>player.maxMoney)
					{
						player.money=player.maxMoney;
					}
				}
				player.gemsGotten+=5;
				BadgeCheck(BE_GEMSUP,0,curMap);
				MakeNormalSound(SND_BIGGEMGET);
				break;
			case ITM_ZOMBGEM:
				if(player.money<player.maxMoney)
				{
					player.money+=100;
					if(player.money>player.maxMoney)
					{
						player.money=player.maxMoney;
					}
				}
				player.gemsGotten+=100;
				BadgeCheck(BE_GEMSUP,0,curMap);
				NewBigMessage("100 Gems!",90);
				MakeNormalSound(SND_BIGGEMGET);
				break;
			case ITM_SUPERGEM:
				MakeRingParticle(goodguy->x,goodguy->y,0,32,100);
				if(player.maxMoney<MAX_MONEY)
				{
					player.maxMoney+=25;
					player.money+=25;
					player.gemsGotten+=25;
					BadgeCheck(BE_GEMSUP,0,curMap);
					NewBigMessage("MAXIMUM GEMS UP!",90);
					MakeNormalSound(SND_POWERUP);
				}
				else
				{
					if(player.money<player.maxMoney)
					{
						player.money+=50;
						if(player.money>player.maxMoney)
						{
							player.money=player.maxMoney;
						}
					}
					player.gemsGotten+=50;
					BadgeCheck(BE_GEMSUP,0,curMap);
					MakeNormalSound(SND_BIGGEMGET);
				}
				break;
			case ITM_KEY:
				if(player.keys[0]<9)
					player.keys[0]++;
				MakeNormalSound(SND_KEYGET);
				break;
			case ITM_KEY2:	// skull
				MakeRingParticle(goodguy->x,goodguy->y,0,32,100);
				player.keys[1]=1;
				MakeNormalSound(SND_POWERUP);
				NewBigMessage("Skull Key Obtained!",90);
				break;
			case ITM_KEY3:	// bat
				MakeRingParticle(goodguy->x,goodguy->y,0,32,100);
				player.keys[2]=1;
				MakeNormalSound(SND_POWERUP);
				NewBigMessage("Bat Key Obtained!",90);
				break;
			case ITM_KEY4:	// pumpkin
				MakeRingParticle(goodguy->x,goodguy->y,0,32,100);
				player.keys[3]=1;
				MakeNormalSound(SND_POWERUP);
				NewBigMessage("Pumpkin Key Obtained!",90);
				break;
			case ITM_TRIPLEFIRE:
				MakeRingParticle(goodguy->x,goodguy->y,0,32,100);
				player.fireFlags|=FF_TRIPLE;
				MakeNormalSound(SND_POWERUP);
				NewBigMessage("Triple Fire Crystal!",90);
				break;
			case ITM_TALISMAN:
				PlayerSetVar(VAR_QUESTASSIGN+QUEST_WITCH,1);
				break;
			case ITM_CAT:
				MakeRingParticle(goodguy->x, goodguy->y, 0, 32, 100);
				NewBigMessage("Cat!", 90);
				MakeNormalSound(SND_POWERUP);
				break;
			case ITM_BOOTS:
				MakeRingParticle(goodguy->x, goodguy->y, 0, 32, 100);
				NewBigMessage("Mud Boots!", 90);
				MakeNormalSound(SND_POWERUP);
				break;
			case ITM_FERTILIZER:
				MakeRingParticle(goodguy->x, goodguy->y, 0, 32, 100);
				NewBigMessage("Fertilizer!", 90);
				MakeNormalSound(SND_POWERUP);
				break;
			case ITM_GHOSTPOTION:
			case ITM_POTION:
				MakeRingParticle(goodguy->x, goodguy->y, 0, 32, 100);
				NewBigMessage("Ghost Slaying Potion!", 90);
				MakeNormalSound(SND_POWERUP);
				break;
			case ITM_LANTERN:
				MakeRingParticle(goodguy->x, goodguy->y, 0, 32, 100);
				NewBigMessage("Lantern!", 90);
				MakeNormalSound(SND_POWERUP);
				break;
			case ITM_REFLECTGEM:
				MakeRingParticle(goodguy->x, goodguy->y, 0, 32, 100);
				NewBigMessage("Reflect Gem!", 90);
				MakeNormalSound(SND_POWERUP);
				break;
			case ITM_SILVERSLING:
				MakeRingParticle(goodguy->x, goodguy->y, 0, 32, 100);
				NewBigMessage("Silver Bullets!", 90);
				MakeNormalSound(SND_POWERUP);
				break;
			case ITM_STICK:
				MakeRingParticle(goodguy->x, goodguy->y, 0, 32, 100);
				NewBigMessage("A Stick!", 90);
				MakeNormalSound(SND_POWERUP);
				break;

		}
		if(itm>=ITM_WBOMB && itm<=ITM_WHOTPANTS)
		{
			return !PlayerGetWeapon(itm);
		}
		if(itm>=ITM_BATDOLL && itm<=ITM_WOLFDOLL)
		{
			MakeNormalSound(SND_POWERUP);
			NewBigMessage("Got a Monster Doll!",90);
			PlayerSetVar(VAR_DOLL+itm-ITM_BATDOLL,1);
		}
		if(itm==ITM_MYSORB && player.worldNum==WORLD_BOWLING)
		{
			player.money=1;
			MakeNormalSound(SND_KEYGET);
		}
		if(itm==ITM_CAT && player.worldNum==WORLD_BOWLING)
		{
			player.money=1;
			MakeNormalSound(SND_CATDRIBBLE+Random(2));
		}
		if(itm==ITM_BADGE)
		{
			if (player.worldNum == WORLD_NORMAL || player.worldNum == WORLD_RANDOMIZER)
				EarnBadge(BADGE_HIDDEN);
			else if (player.worldNum == WORLD_REMIX)
				EarnBadge(BADGE_HIDDEN2);
		}
		return 0;
	}
	else
		return 1;
}

byte ShroomCount(void)
{
	int i;
	byte b;

	b=0;
	for(i=VAR_MUSHROOM;i<VAR_MUSHROOM+10;i++)
		if(player.var[i])
			b++;

	return b;
}

byte OrbCount(void)
{
	int i;
	byte b;

	b=0;
	for(i=VAR_MYSORB;i<VAR_MYSORB+4;i++)
		if(player.var[i])
			b++;

	return b;
}

byte OrbsUsed(void)
{
	int i;
	byte b;

	b=0;
	for(i=VAR_ORBSTAND;i<VAR_ORBSTAND+4;i++)
		if(player.var[i])
			b++;

	return b;
}

byte BatCount(void)
{
	int i;
	byte b;

	b=0;
	for(i=VAR_BATSTATUE;i<VAR_BATSTATUE+4;i++)
		if(player.var[i])
			b++;

	return b;
}

byte BatsUsed(void)
{
	int i;
	byte b;

	b=0;
	for(i=VAR_BATSTAND;i<VAR_BATSTAND+4;i++)
		if(player.var[i])
			b++;

	return b;
}

byte VampCount(void)
{
	int i;
	byte b;

	b=0;
	for(i=VAR_VAMPBUST;i<VAR_VAMPBUST+8;i++)
		if(player.var[i])
			b++;

	return b;
}

byte VampsUsed(void)
{
	int i;
	byte b;

	b=0;
	for(i=VAR_VAMPSTAND;i<VAR_VAMPSTAND+8;i++)
		if(player.var[i])
			b++;

	return b;
}

void PlayerGetMonsterPoints(int level)
{
	int pp;

	pp=player.firePower+player.fireRange+player.fireRate;
	pp=level-pp;
	if(pp<1)
		pp=1;
	player.monsterPoints+=pp;
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

void PlayerFireWeapon(Guy *me)
{
	int i,f;

	if(player.money<player.wpnCost)
	{
		player.wpnReload=3;
		return;
	}
	else
	{
		player.money-=player.wpnCost;
	}
	if(player.wpnLevel==2 && opt.cheats[CH_ULTRAWEAPON])
	{
		PlayerFireUltraWeapon(me);
		return;
	}

	player.specialShotsFired++;
	switch(player.weapon)
	{
		case ITM_WWHOOPEE:
			FireBullet(me->x,me->y,me->facing*32,BLT_WHOOPEE+player.wpnLevel);
			if(player.wpnLevel==2)
			{
				FireBullet(me->x,me->y,((me->facing+7)*32)&255,BLT_WHOOPEE+player.wpnLevel);
				FireBullet(me->x,me->y,((me->facing+1)*32)&255,BLT_WHOOPEE+player.wpnLevel);
			}
			MakeSound(SND_WHOOPEE1+Random(3),me->x,me->y,SND_CUTOFF,800);
			player.wpnReload=15;
			break;
		case ITM_WCACTUS:
			switch(player.wpnLevel)
			{
				case 0:
					for(i=0;i<3;i++)
					{
						f=(me->facing*32+(256-8)+Random(16))&255;
						if(Random(10)==0)
							FireBullet(me->x,me->y,f,BLT_CACTUS2);
						else
							FireBullet(me->x,me->y,f,BLT_CACTUS);
					}
					break;
				case 1:
					for(i=0;i<7;i++)
					{
						f=(me->facing*32+(256-3*8-4+i*8)+(256-8)+Random(16))&255;
						if(Random(2)==0)
							FireBullet(me->x,me->y,f,BLT_CACTUS2);
						else
							FireBullet(me->x,me->y,f,BLT_CACTUS);
					}
					break;
				case 2:
					for(i=0;i<16;i++)
					{
						f=(me->facing*32+(256-8*4+i*4)+(256-8)+Random(16))&255;
						FireBullet(me->x,me->y,f,BLT_CACTUS2);
					}
					break;
			}
			player.wpnReload=2;
			MakeSound(SND_CACTUS,me->x,me->y,SND_CUTOFF,800);
			break;
		case ITM_WBOOMERANG:
			switch(player.wpnLevel)
			{
				case 0:
					FireBullet(me->x,me->y,me->facing*32,BLT_BOOMERANG);
					break;
				case 1:
					FireBullet(me->x,me->y,(me->facing*32+32)&255,BLT_BOOMERANG);
					FireBullet(me->x,me->y,me->facing*32,BLT_BOOMERANG);
					FireBullet(me->x,me->y,(me->facing*32+256-32)&255,BLT_BOOMERANG);
					break;
				case 2:
					FireBullet(me->x,me->y,(me->facing*32+32)&255,BLT_BOOMERANG);
					FireBullet(me->x,me->y,(me->facing*32+16)&255,BLT_BOOMERANG);
					FireBullet(me->x,me->y,me->facing*32,BLT_BOOMERANG);
					FireBullet(me->x,me->y,(me->facing*32+256-16)&255,BLT_BOOMERANG);
					FireBullet(me->x,me->y,(me->facing*32+256-32)&255,BLT_BOOMERANG);
					break;
			}
			player.wpnReload=8;
			MakeSound(SND_CACTUS,me->x,me->y,SND_CUTOFF,800);
			break;
		case ITM_WICE:
			switch(player.wpnLevel)
			{
				case 0:
					FireBullet(me->x,me->y,me->facing*32,BLT_ICE);
					break;
				case 1:
					FireBullet(me->x,me->y,me->facing*32,BLT_ICE);
					break;
				case 2:
					FireBullet(me->x,me->y,me->facing*32,BLT_ICE);
					break;
			}
			player.wpnReload=4;
			MakeSound(SND_CACTUS,me->x,me->y,SND_CUTOFF,800);
			break;
		case ITM_WBOMB:
			switch(player.wpnLevel)
			{
				case 0:
					FireBullet(me->x,me->y,me->facing*32,BLT_BOMB);
					break;
				case 1:
					FireBullet(me->x,me->y,me->facing*32,BLT_BOMB);
					break;
				case 2:
					FireBullet(me->x,me->y,me->facing*32,BLT_BOMB);
					break;
			}
			player.wpnReload=12;
			MakeSound(SND_CACTUS,me->x,me->y,SND_CUTOFF,800);
			break;
		case ITM_WHOTPANTS:
			FireBullet(me->x,me->y,(me->facing*32+128)&255,BLT_HOTPANTS);
			player.speed=12+10*player.wpnLevel;
			player.wpnReload=4+player.wpnLevel*2;
			break;
		case ITM_WLIGHTNING:
			for(i=0;i<player.wpnLevel*2+1;i++)
				FireBullet(me->x,me->y,0,BLT_LIGHTNING);
			player.wpnReload=player.wpnLevel*2+1;
			break;
	}
}

void PlayerFireUltraWeapon(Guy *me)
{
	int i;

	player.specialShotsFired++;
	switch(player.weapon)
	{
		case ITM_WWHOOPEE:
			for(i=0;i<32;i++)
			{
				FireBullet(me->x,me->y,i*8,BLT_WHOOPEE3);
			}
			MakeSound(SND_WHOOPEE1+Random(3),me->x,me->y,SND_CUTOFF,800);
			player.wpnReload=10;
			break;
		case ITM_WCACTUS:
			for(i=0;i<64;i++)
			{
				FireBullet(me->x,me->y,i*4,BLT_CACTUS2);
			}
			player.wpnReload=10;
			MakeSound(SND_CACTUS,me->x,me->y,SND_CUTOFF,800);
			break;
		case ITM_WBOOMERANG:
			for(i=0;i<32;i++)
				FireBullet(me->x,me->y,i*8,BLT_BOOMERANG);
			player.wpnReload=10;
			MakeSound(SND_CACTUS,me->x,me->y,SND_CUTOFF,800);
			break;
		case ITM_WICE:
			FreezeAllBadguys();
			player.wpnReload=10;
			MakeSound(SND_FREEZE,me->x,me->y,SND_CUTOFF,800);
			break;
		case ITM_WBOMB:
			for(i=0;i<16;i++)
			{
				FireBullet(me->x-320*FIXAMT+Random(640*FIXAMT),me->y-240*FIXAMT+Random(480*FIXAMT),0,BLT_GOODBOOM2);
			}
			player.wpnReload=12;
			break;
		case ITM_WHOTPANTS:
			for(i=0;i<32;i++)
				FireBullet(me->x-320*FIXAMT+Random(640*FIXAMT),me->y-240*FIXAMT+Random(480*FIXAMT),(byte)Random(256),BLT_HOTPANTS);
			player.speed=255;
			player.wpnReload=10;
			break;
		case ITM_WLIGHTNING:
			for(i=0;i<20;i++)
				FireBullet(me->x-320*FIXAMT+Random(640*FIXAMT),me->y-240*FIXAMT+Random(480*FIXAMT),0,BLT_LIGHTNING);
			player.wpnReload=10;
			break;
	}
}

std::string GetPlayerSeed()
{
	return std::string((const char*)&player.var[VAR_SEEDSTART], MAX_SEED_LENGTH);
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
	if(tportclock)
		tportclock--;

	if(opt.cheats[CH_SIDEKICK] && !ArchipelagoMode)
	{
		player.var[VAR_QUESTASSIGN+QUEST_FARLEY]=1;
		player.var[VAR_QUESTDONE+QUEST_FARLEY]=1;
	}
	else if (player.var[VAR_QUESTDONE+QUEST_FARLEY] && !player.var[VAR_HELPERBAT])
	{
		// If you save with the Sidekick badge on then turn it off, instead of
		// going into an inconsistent state forever, revert Farley to as if you
		// have not completed his quest. This prevents weirdness and also makes
		// it so using the badge once doesn't lock you out of 100% completion.
		player.var[VAR_QUESTDONE+QUEST_FARLEY] = 0;
	}

	if(player.worldNum==WORLD_SLINGSHOT)
	{
		PlayerControlSlingShot(me,mapTile,world);
		return;
	}

	if(opt.cheats[CH_GEMS] && player.worldNum!=WORLD_LOONYBALL &&
		player.worldNum!=WORLD_BOWLING)
	{
		player.money=player.maxMoney;
	}

	switch(player.monsType)
	{
		case MONS_LOONY:
		case MONS_LOONY2:
		case MONS_PLYRWOLF:
			if(player.cheatsOn&PC_PLAYWOLF)
				WolfLoonyControlMe(me,mapTile,world);
			else
				LoonyControlMe(me,mapTile,world);
			break;
		case MONS_LOONYTOAD:
			ToadControlMe(me,map,mapTile,world);
			break;
		case MONS_PLYRBONKULA:
			BonkulaControlMe(me,map,mapTile,world);
			break;
		case MONS_PLYRSWAMPDOG:
			SwampdogControlMe(me,mapTile,world);
			break;
		case MONS_PLYRWITCH:
			WitchControlMe(me,mapTile,world);
			break;
		case MONS_PLYRSUMMON:
			SummonControlMe(me,mapTile,world);
			break;
		case MONS_PLYRTHIEF:
			GirlControlMe(me,mapTile,world);
			break;
	}
}

void SetAreaName(world_t *world)
{
	byte c;

	if(opt.cheats[CH_CRYSTAL])
	{
		player.crystalBall=curMap->FindPowerUps((goodguy->x>>FIXSHIFT)/TILE_WIDTH,(goodguy->y>>FIXSHIFT)/TILE_HEIGHT);
	}

	// decide which location the player is for naming purposes
	if(player.levelNum==0 && (player.worldNum==WORLD_NORMAL || player.worldNum==WORLD_REMIX || player.worldNum==WORLD_RANDOMIZER))
	{
		c=255;
		// decide based on region in the map
		if(goodguy->mapx>=0 && goodguy->mapy>=0 &&
			goodguy->mapx<=72 && goodguy->mapy<=76)
			c=2;	// rocky cliffs
		if(goodguy->mapx>=107 && goodguy->mapy>=0 &&
			goodguy->mapx<=199 && goodguy->mapy<=126)
			c=1;	// slurpy swamp
		if(goodguy->mapx>=107 && goodguy->mapy>=127 &&
			goodguy->mapx<=199 && goodguy->mapy<=199)
			c=4;	// wicked woods
		if(goodguy->mapx>=73 && goodguy->mapy>=0 &&
			goodguy->mapx<=106 && goodguy->mapy<=77)
			c=6;	// vampy land

		if(goodguy->mapx>=0 && goodguy->mapy>=77 &&
			goodguy->mapx<=105 && goodguy->mapy<=199)
			c=9;	// plains
		if(goodguy->mapx>=70 && goodguy->mapy>=110 &&
			goodguy->mapx<=106 && goodguy->mapy<=199)
			c=11;	// woods under terror glade

		if(goodguy->mapx>=74 && goodguy->mapy>=78 &&
			goodguy->mapx<=106 && goodguy->mapy<=108)
			c=0;	// Luniton
		if(goodguy->mapx>=70 && goodguy->mapy>=109 &&
			goodguy->mapx<=111 && goodguy->mapy<=144)
			c=3;	// terror glade
		if(goodguy->mapx>=0 && goodguy->mapy>=176 &&
			goodguy->mapx<=36 && goodguy->mapy<=199)
			c=8;	// zombiton
		if(goodguy->mapx>=0 && goodguy->mapy>=77 &&
			goodguy->mapx<=45 && goodguy->mapy<=120)
			c=5;	// mystery lake
		if(goodguy->mapx>=78 && goodguy->mapy>=160 &&
			goodguy->mapx<=97 && goodguy->mapy<=178)
			c=10;	// farm

		if(goodguy->mapx>=0 && goodguy->mapy>=143 &&
			goodguy->mapx<=22 && goodguy->mapy<=175)
			c=7;	// cemetery
		if(goodguy->mapx>=48 && goodguy->mapy>=180 &&
			goodguy->mapx<=89 && goodguy->mapy<=199)
			c=7;	// cemetery

		if(c!=255)
		{
			strcpy(player.areaName,GetAreaName(c));

			// environmental sound effects
			if(Random(80)==0)
			{
				switch(c)
				{
					case 1: // swamp
						MakeSound(SND_SWAMPGURGLE+Random(5),
							goodguy->x-400*FIXAMT+Random(800*FIXAMT),
							goodguy->y-400*FIXAMT+Random(800*FIXAMT),SND_CUTOFF,1);
						break;
					case 2: // cliffs
						MakeSound(SND_CLIFFSWIND+Random(5),
							goodguy->x-400*FIXAMT+Random(800*FIXAMT),
							goodguy->y-400*FIXAMT+Random(800*FIXAMT),SND_CUTOFF,1);
						break;
					case 3: // terror glade
					case 4: // woods
					case 6: // vampy land
					case 11: // ghastly glen
						MakeSound(SND_WOODSLIFE+Random(5),
							goodguy->x-400*FIXAMT+Random(800*FIXAMT),
							goodguy->y-400*FIXAMT+Random(800*FIXAMT),SND_CUTOFF,1);
						break;
				}
			}
		}
	}
	else
	{
		strcpy(player.areaName,world->map[player.levelNum]->name);
	}
}

void CallFarley(Guy *me)
{
	if(player.worldNum==WORLD_BOWLING)
		return;
	if(opt.cheats[CH_NOFARLEY])
		return;

	if((opt.cheats[CH_SIDEKICK]  && !ArchipelagoMode ||
		(player.hearts==player.maxHearts && (player.fireFlags&FF_HELPER))) &&
		!(player.fireFlags&FF_HELPERHERE) && (!MonsterExists(MONS_HELPERBAT)))
	{
		AddGuy(me->x,me->y,20*FIXAMT,MONS_HELPERBAT);
		player.fireFlags|=FF_HELPERHERE;
	}
}

void HandlePoison(Guy *me)
{
	byte rate;

	if(player.difficulty==DIFF_BEGINNER)
		rate=64;
	else if(player.difficulty==DIFF_LOONY)
		rate=16;
	else
		rate=32;

	if(opt.cheats[CH_SUPERSURV])
		player.poison=0;

	if(player.poison)
	{
		// poison every second
		if(player.hearts>0 && ((player.poison%rate)==0))
		{
			me->ouch=4;
			player.hearts--;
			player.hitsTaken++;
			me->hp--;
			if(opt.cheats[CH_SIDEKICK])
			{
				if(player.batLevel>32)
					player.batLevel-=32;
				else
					player.batLevel=0;
			}
			if(player.hearts==0)
			{
				me->seq=ANIM_DIE;
				me->dx=0;
				me->dy=0;
				me->frm=0;
				me->frmAdvance=64;
				me->frmTimer=0;
				me->action=ACTION_BUSY;
			}
		}
		player.poison--;
	}
}

void PlayerCalcStats()
{
	for (int i = 0; i < 20; i++)
	{
		if (player.var[VAR_HEART + i] == 0)
		{
			player.maxHearts = player.startHearts + i;
			break;
		}
	}
	for (int i = 0; i < 10; i++)
	{
		if (player.var[VAR_LIGHTNING + i] == 0)
		{
			player.firePower = i;
			break;
		}
	}
	for (int i = 0; i < 10; i++)
	{
		if (player.var[VAR_ARROW + i] == 0)
		{
			player.fireRange = i;
			break;
		}
	}
	for (int i = 0; i < 10; i++)
	{
		if (player.var[VAR_PANTS + i] == 0)
		{
			player.fireRate = i + (opt.cheats[CH_RAPIDFIRE] * 2);
			break;
		}
	}
	for (int i = 0; i < 6; i++)
	{
		if (player.var[VAR_GEM + i] == 0)
		{
			player.maxMoney = 50 + i*25;
			break;
		}
	}
	for (int i = 0; i < 3; i++)
	{
		if (player.var[VAR_KEY + i] == 1) {

			player.keys[i + 1] = 1;
		}
	}
	if (player.var[VAR_TRIPLEFIRE])
	{
		player.fireFlags |= FF_TRIPLE;
	}
	if (player.var[VAR_REFLECT])
	{
		player.fireFlags |= FF_REFLECT;
	}
}

void DescribeSave(span<char> dst, const player_t* player)
{
	// Slot number and area name
	dst = ham_sprintf(dst, "%d: %s", player->lastSave + 1, player->areaName);
	dst = ham_strcpy(dst, " - ");
	// Percentage
	dst = ham_sprintf(dst, "%03.1f%%", CalcPercent(player));
	// World name (if needed)
	if (player->worldNum == WORLD_REMIX)
	{
		dst = ham_strcpy(dst, " Remix");
	}
	else if (player->worldNum == WORLD_RANDOMIZER)
	{
		dst = ham_strcpy(dst, " Rando");
	}
	// Difficulty name, including Hardcore and Terror
	// ...except there's no room to have both difficulty and character
	/*
	dst = ham_strcpy(dst, " - ");
	if (player->cheatsOn & PC_HARDCORE)
		dst = ham_strcpy(dst, "Hardcore+");
	if (player->cheatsOn & PC_TERROR)
		dst = ham_strcpy(dst, "Terror+");
	dst = ham_sprintf(dst, "%s", DifficultyName(player->difficulty));
	*/
	// Character name
	if (player->cheatsOn & PC_PLAYTOAD)
		dst = ham_sprintf(dst, " - %s", PlayerCharacterName(PC_Toad));
	else if (player->cheatsOn & PC_PLAYBONK)
		dst = ham_sprintf(dst, " - %s", PlayerCharacterName(PC_Bonkula));
	else if (player->cheatsOn & PC_PLAYDOG)
		dst = ham_sprintf(dst, " - %s", PlayerCharacterName(PC_Swampdog));
	else if (player->cheatsOn & PC_PLAYWITCH)
		dst = ham_sprintf(dst, " - %s", PlayerCharacterName(PC_Witch));
	else if (player->cheatsOn & PC_PLAYWOLF)
		dst = ham_sprintf(dst, " - %s", PlayerCharacterName(PC_Werewolf));
	else if (player->cheatsOn & PC_PLAYSUMMON)
		dst = ham_sprintf(dst, " - %s", PlayerCharacterName(PC_Summon));
	else if (player->cheatsOn & PC_PLAYTHIEF)
		dst = ham_sprintf(dst, " - %s", PlayerCharacterName(PC_Thief));
	// Just hardcore since that's more relevant to save+load system than other difficulties
	if (player->cheatsOn & PC_HARDCORE)
		dst = ham_strcpy(dst, " - Hardcore");
}

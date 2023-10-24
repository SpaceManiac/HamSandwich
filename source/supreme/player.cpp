#include "winpch.h"
#include "player.h"
#include "music.h"
#include "rage.h"
#include "title.h"
#include "progress.h"
#include "hiscore.h"
#include "shop.h"
#include "editor.h"
#include "goal.h"
#include "log.h"

#define PLYR_ACCEL	(FIXAMT)
#define PLYR_DECEL	(FIXAMT*3/4)
#define PLYR_MAXSPD (FIXAMT*6)

player_t player;
byte playerGlow;	// for torch-lit levels, and for exciting moments
byte tportclock;
int pStartX=-1,pStartY=-1;
static byte oldControls,newControls,checkTheControls;

int maxAmmo[MAX_WEAPONS]={1000,20,99,5,50,1000,15,40,20,30,8,3,1,40,10,3,50,5,1000,5,1};

void ShouldCheckControls(byte n)
{
	byte c;

	checkTheControls=n;
	c=GetControls();
	oldControls=newControls;
	newControls=c;
}

byte ControlCheck(byte c)
{
	if(!(oldControls&c) && !(newControls&c))
		return 0;	// untouched
	if((oldControls&c) && (newControls&c))
		return 1;	// held
	if((oldControls&c) && !(newControls&c))
		return 2;	// just released
	if(!(oldControls&c) && (newControls&c))
		return 3;	// tapped
	return 0;
}

void InitPlayer(byte level,const char *fname)
{
	int i;

	if (fname != player.worldName)  // Genuine pointer check to avoid self-overwriting.
		strcpy(player.worldName, fname);

	player.worldProg=GetWorldProgress(player.worldName);

	player.levelNum=level;
	player.levelProg=GetLevelProgress(player.worldName,player.levelNum);

	for(i=0;i<4;i++)
		player.keys[i]=0;

	for(i=0;i<8;i++)
		player.var[i]=0;

	oldControls=0;
	newControls=0;
	player.perfect=1;
	player.totalEnemies=0;
	player.enemiesSlain=0;
	player.stealthy=0;
	player.lavaTimer=15;
	player.brains=0;
	player.coins=0;
	player.candles=0;
	player.boredom=0;
	player.hammers=0;
	player.hamSpeed=16;
	player.weapon=WPN_NONE;
	player.lastWeapon=WPN_NONE;
	player.ammo=0;
	player.reload=10;
	player.wpnReload=10;
	player.hammerFlags=0;
	player.life=128;
	player.shield=0;
	playerGlow=0;
	player.pushPower=0;
	player.vehicle=0;
	player.garlic=0;
	player.speed=0;
	player.rageClock=0;
	player.rage=0;
	player.varbar=0;
	player.varbarMax=0;
	player.invisibility=0;
	player.jetting=0;
	player.clock=0;
	player.combo=0;
	player.comboClock=0;
	player.oxygen=127*256;
	player.timeStop=0;
	player.ammoCrate=0;
	player.cheated=0;
	player.bestCombo=0;
	player.cheesePower=0;

	for(i=1;i<6;i++)
	{
		player.ability[i]=ItemPurchased(SHOP_ABILITY,i);
	}

	player.brainTime=30*30;
	player.brainX=255;
	player.candleX=255;
	player.candleTime=30*30;
	player.spotted=0;
	player.playAs=profile.playAs;
}

void ExitPlayer(void)
{
	player.playAs=profile.playAs;
	player.shield=0;
	player.speed=0;
	player.invisibility=0;
	SaveProfile();
}

void PlayerRenderInterface(MGLDraw *mgl)
{
	RenderInterface(mgl);
}

byte PlayerShield(void)
{
	return player.shield;
}

byte PlayerHasHammer(void)
{
	return (player.hammers>0);
}

int PlayerBrains(void)
{
	return player.brains;
}

void PoisonVictim(Guy *me,byte amt)
{
	if(me==goodguy && player.shield)
		return;	// can't be poisoned when invulnerable
	if(me==goodguy && profile.difficulty==0)
	{
		amt/=2;
		if(amt==0)
			amt=1;
	}
	if(me->poison+amt>255)
		me->poison=255;
	else
		me->poison+=amt;
}

void PlayerWinLevel(byte isSecret)
{
	PrintToLog("PlayerWinLevel",0);
	player.levelProg->flags|=LF_PASSED;
	if(curMap->numCandles==0)
		player.levelProg->flags|=LF_CANDLES;	// get candle credit if there aren't any

	PrintToLog("StoreWorldResults",0);
	StoreWorldResults(player.worldProg,&curWorld);
	PrintToLog("TryHighScore",0);
	if(!player.cheated)
	{
		player.gotRecords = TryHighScore();
		// Steam Leaderboard uploads are deferred until you quit the world in
		// order to stay under Steam's 10 uploads per 10 minutes rate limit.
		// NB: marks for upload even if you got local 2nd place...
		if (player.gotRecords)
			player.pendingLeaderboardUpload = true;
	}
	else
		player.gotRecords=0;
	PrintToLog("GoalWinLevel",0);
	GoalWinLevel();
	PrintToLog("SaveProfile",0);
	SaveProfile();
}

void SetPlayerHP(int hp)
{
	if(hp>player.life)
		ScoreEvent(SE_HEAL,hp-player.life);
	else
		ScoreEvent(SE_OUCH,player.life-hp);
	player.life=hp;
	if(hp<goodguy->maxHP)
		player.perfect=0;
}

void KeyChainAllCheck(void)
{
	if((player.worldProg->keychains&(KC_KEYCH1|KC_KEYCH2|KC_KEYCH3|KC_KEYCH4))==(KC_KEYCH1|KC_KEYCH2|KC_KEYCH3|KC_KEYCH4))
	{
		NewBigMessage("I collected all four!",60);
		CompleteGoal(75);
		playerGlow=127;
		HealRing(7,goodguy->x,goodguy->y,FIXAMT*20,255,5);
		MakeNormalSound(SND_ALLKEYCHAIN);
	}
}

int WeaponMaxAmmo(byte wpn)
{
	return maxAmmo[wpn];
}

byte PlayerGetWeapon(byte wpn,int x,int y)
{
	int cx,cy;

	GetCamera(&cx,&cy);
	cx<<=FIXSHIFT;
	cy<<=FIXSHIFT;

	if(player.weapon && wpn!=player.weapon && profile.progress.wpnLock)
		return 0;	// can't pick up weapons while armed!

	if((player.weapon==WPN_PWRARMOR || player.weapon==WPN_MINISUB) && (wpn!=player.weapon))
	{
		return 0;	// can't pick up other weapons
	}

	if(player.weapon==wpn && player.ammo==maxAmmo[wpn])
		return 0;	// don't pick it up if you've already got it

	if(wpn==WPN_PWRARMOR)
	{
		player.weapon=WPN_PWRARMOR;
		player.ammo=maxAmmo[wpn];
		goodguy->seq=ANIM_A2;
		goodguy->frm=0;
		goodguy->frmTimer=0;
		goodguy->frmAdvance=64;
		goodguy->action=ACTION_BUSY;
	}
	else if(wpn==WPN_MINISUB)
	{
		player.weapon=WPN_MINISUB;
		player.ammo=maxAmmo[wpn];
		goodguy->seq=ANIM_A2;
		goodguy->frm=0;
		goodguy->frmTimer=0;
		goodguy->frmAdvance=64;
		goodguy->action=ACTION_BUSY;
	}
	else
	{
		player.lastWeapon=wpn;
		player.weapon=wpn;
		player.ammo=maxAmmo[wpn];
	}

	ScoreEvent(SE_PICKUP,1);

	return 1;
}

byte PlayerPowerup(int powerup)
{
	if(powerup>0)
	{
		switch(powerup)
		{
			case PU_REVERSE:
				player.hammerFlags|=HMR_REVERSE;
				break;
			case PU_REFLECT:
				player.hammerFlags|=HMR_REFLECT;
				break;
			case PU_SHIELD:
				player.shield=240;
				break;
			case PU_GARLIC:
				player.garlic=255;
				break;
			case PU_SPEED:
				player.speed=255;
				break;
			case PU_INVISO:
				player.invisibility=255;
				break;
			case PU_AMMO:
				if(player.weapon==0)
					return 0;
				player.ammoCrate=255;
				break;
			case PU_AMMO2:
				if((player.weapon==0 && player.lastWeapon==0) || player.ammo==maxAmmo[player.weapon])
					return 0;
				if(player.weapon==0)
					player.weapon=player.lastWeapon;
				player.ammo=maxAmmo[player.weapon];
				break;
			case PU_CHEESE:
				player.cheesePower=255;
				break;
			case PU_POISON:
				if(goodguy->poison+128<255)
					goodguy->poison+=128;
				else
					goodguy->poison=255;
				break;
		}
	}
	else
	{
		switch(-powerup)
		{
			case PU_REVERSE:
				player.hammerFlags^=HMR_REVERSE;
				break;
			case PU_REFLECT:
				player.hammerFlags^=HMR_REFLECT;
				break;
			case PU_SHIELD:
				player.shield=0;
				break;
			case PU_GARLIC:
				player.garlic=0;
				break;
			case PU_SPEED:
				player.speed=0;
				break;
			case PU_INVISO:
				player.invisibility=0;
				break;
			case PU_AMMO:
				player.ammoCrate=0;
				break;
			case PU_AMMO2:
				player.ammo=0;
				player.weapon=0;
				break;
			case PU_POISON:
				goodguy->poison=0;
				break;
		}
	}
	return 1;
}

void PlayerRadioactiveFood(void)
{
	goodguy->poison=0;
	switch(Random(6))
	{
		case 0:
			NewMessage("Radioactive Energy!!",75,0);
			PlayerHeal(30);
			player.speed=255;
			break;
		case 1:
			NewMessage("Szechwan Surprise!",75,0);
			PlayerHeal(128);
			break;
		case 2:
			NewMessage("Filling... with... RAGE!!",75,0);
			PlayerHeal(50);
			player.rage=127*256;
			break;
		case 3:
			NewMessage("Toxic Mu Shu!",75,0);
			goodguy->poison=255;
			break;
		case 4:
			NewMessage("Thermonuclear Heartburn!",75,0);
			goodguy->GetShot(0,0,50,curMap,&curWorld);
			break;
		case 5:
			NewMessage("Atomic Inviso-Power!",75,0);
			player.invisibility=255;
			break;
	}
}

void PlayerChineseFood(void)
{
	goodguy->poison=0;
	switch(Random(8))
	{
		case 0:
			NewMessage("Just fried rice",75,0);
			PlayerHeal(5);
			break;
		case 1:
			NewMessage("Mmm, Kung Pao!",75,0);
			PlayerHeal(25);
			break;
		case 2:
			NewMessage("Szechwan Shredded Pork!!",75,0);
			PlayerHeal(50);
			break;
		case 3:
			NewMessage("Moo Shi Beef!",75,0);
			PlayerHeal(25);
			break;
		case 4:
			NewMessage("Ick, mostly veggies",75,0);
			PlayerHeal(1);
			break;
		case 5:
			NewMessage("Hmm, tastes kind of citrusy",75,0);
			PlayerHeal(15);
			break;
		case 6:
			NewMessage("Joy! Wontons!",75,0);
			PlayerHeal(20);
			break;
		case 7:
			NewMessage("GeneralTsao has outdone himself",75,0);
			PlayerHeal(40);
			break;
	}
}

void PlayerGetBrain(int amt)
{
	int total;

	if(amt>0)
	{
		player.brainTime=30*30;
		player.brainX=255;
	}

	player.brains+=amt;
	if(amt>0 && !editing && !player.cheated)
	{
		profile.progress.totalBrains+=amt;
		if(profile.progress.totalBrains>=500)
			CompleteGoal(76);
		if(profile.progress.totalBrains>=5000)
			CompleteGoal(77);
	}

	ScoreEvent(SE_BRAIN,amt);
	if(player.brains<0)
		player.brains=0;

	total=curMap->numBrains;
	if(total>0 && player.brains>=total)
	{
		player.brains=total;
		if(player.playAs==PLAY_BOUAPHA)
			MakeNormalSound(SND_KOOLKAT);
		else if(player.playAs==PLAY_LUNATIC)
			MakeNormalSound(SND_DRLLAUGH);
		else if(player.playAs==PLAY_HAPPY)
			MakeNormalSound(SND_HAPPYDIE);
		else if(player.playAs==PLAY_MECHA)
			MakeNormalSound(SND_FACTORYGO);
		else if(player.playAs==PLAY_SHROOM)
			MakeNormalSound(SND_MUSHMAD);
		else if(player.playAs==PLAY_LUNACHIK)
			MakeNormalSound(SND_LUNABRAINS);

		playerGlow=127;
	}
}

void PlayerGetCandle(int amt)
{
	int total;

	if(amt>0)
	{
		player.candleTime=30*30;
		player.candleX=255;
	}

	player.candles+=amt;
	if(amt>0 && !editing && !player.cheated)
	{
		profile.progress.totalCandles+=amt;
		if(profile.progress.totalCandles>=500)
			CompleteGoal(78);
		if(profile.progress.totalCandles>=5000)
			CompleteGoal(79);
	}

	ScoreEvent(SE_CANDLE,amt);
	if(player.candles<0)
		player.candles=0;

	total=curMap->numCandles;
	if(total>0 && player.candles>=total)
	{
		player.candles=total;
		MakeNormalSound(SND_ALLCANDLE);
		player.levelProg->flags|=LF_CANDLES;
		StoreWorldResults(player.worldProg,&curWorld);
		SaveProfile();
		playerGlow=127;
	}
}

void PlayerGetCoin(int amt)
{
	player.coins+=amt;
	if(player.coins<0)
		player.coins=0;
	if(player.coins>99)
		player.coins=99;
	if(player.coins>=20)
		CompleteGoal(80);
}

void ToggleWaterwalk(void)
{
	player.hammerFlags^=HMR_WATERWALK;
}

byte PlayerCanWaterwalk(void)
{
	return (player.hammerFlags&HMR_WATERWALK);
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

void PlayerLoseKey(byte w)
{
	if(player.keys[w])
		player.keys[w]--;
}

byte PlayerKeyChain(byte w)
{
	if(w==0)
		return ((player.worldProg->keychains&KC_KEYCH1)!=0);
	else if(w==1)
		return ((player.worldProg->keychains&KC_KEYCH2)!=0);
	else if(w==2)
		return ((player.worldProg->keychains&KC_KEYCH3)!=0);
	else if(w==3)
		return ((player.worldProg->keychains&KC_KEYCH4)!=0);
	else if(w==4)
		return ((player.worldProg->keychains&KC_LOONY)!=0);
	else
		return 0;
}

byte PlayerKeys(byte w)
{
	return player.keys[w];
}

byte GetPlayerGlow(void)
{
	return playerGlow;
}

void SetPlayerGlow(byte v)
{
	playerGlow=v;
}

byte PlayerGetMusicSettings(void)
{
	return profile.music;
}

void PlayerSetMusicSettings(byte m)
{
	profile.music=m;
}

void PlayerThrowHammer(Guy *me)
{
	if(!editing && !player.cheated && verified)
		profile.progress.hammersThrown+=player.hammers;
	GoalFire();

	ScoreEvent(SE_SHOOT,player.hammers);
	if(player.hammerFlags&HMR_REVERSE)
		ScoreEvent(SE_SHOOT,player.hammers);

	if(player.playAs==PLAY_BOUAPHA || player.playAs==PLAY_MECHA || player.playAs==PLAY_LUNACHIK)
	{
		HammerLaunch(me->x,me->y,me->facing,player.hammers,player.hammerFlags);
	}
	else if(player.playAs==PLAY_LUNATIC)
	{
		MakeSound(SND_BALLLIGHTNING,me->x,me->y,SND_CUTOFF,600);
		FireBullet(me->x,me->y,me->facing,BLT_BALLLIGHTNING,1);
		if(player.hammerFlags&HMR_REVERSE)
			FireBullet(me->x,me->y,(byte)Random(8),BLT_BALLLIGHTNING,1);
	}
	else if(player.playAs==PLAY_HAPPY)
	{
		HappyLaunch(me->x,me->y,me->facing,player.hammers,player.hammerFlags);
	}
	else if(player.playAs==PLAY_SHROOM)
	{
		ShroomSpew(me->x,me->y,me->facing,player.hammers,player.hammerFlags);
	}

	player.reload=player.hamSpeed+2;
}

void PlayerHeal(byte amt)
{
	if(profile.difficulty==0)
	{
		if(amt>127)
			amt=255;
		else
			amt*=2;
	}

	HealRing(1,goodguy->x,goodguy->y,0,16,2);
	HealGoodguy(amt);

	if(player.life+amt<goodguy->maxHP)
	{
		ScoreEvent(SE_HEAL,amt);
		player.life+=amt;
	}
	else
	{
		ScoreEvent(SE_HEAL,goodguy->maxHP-player.life);
		player.life=goodguy->maxHP;
	}
	if(goodguy)
		goodguy->poison=0;
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

void PlayerFireWeapon(Guy *me)
{
	byte c;

	if(player.life==0)
		return;	// no shooting when you're dead

	switch(player.weapon)
	{
		case WPN_MISSILES:
			if(player.ammo)
			{
				ScoreEvent(SE_SHOOT,1);
				FireBullet(me->x,me->y,me->facing,BLT_MISSILE,1);
				player.ammo--;
				if(!editing && !player.cheated && verified)
					profile.progress.shotsFired++;
			}
			player.wpnReload=2;
			break;
		case WPN_BOMBS:
			if(player.ammo)
			{
				ScoreEvent(SE_SHOOT,1);
				FireBullet(me->x,me->y,me->facing*32,BLT_BOMB,1);
				MakeSound(SND_BOMBTHROW,me->x,me->y,SND_CUTOFF,1200);
				player.ammo--;
				if(!editing && !player.cheated && verified)
					profile.progress.shotsFired++;
			}
			player.wpnReload=15;
			break;
		case WPN_AK8087:
			if(player.ammo)
			{
				ScoreEvent(SE_SHOOT,1);
				FireBullet(me->x,me->y,me->facing,BLT_LASER,1);
				player.ammo--;
				if(!editing && !player.cheated && verified)
					profile.progress.shotsFired++;
			}
			me->z+=FIXAMT*Random(4);
			me->dx+=FIXAMT/2-Random(FIXAMT);
			me->dy+=FIXAMT/2-Random(FIXAMT);
			c=GetControls();
			if(c&CONTROL_B2)	// fire is held
			{
				player.wpnReload=1;
				me->frmTimer=0;
			}
			else
			{
				player.wpnReload=5;
			}
			DoPlayerFacing(c,me);
			break;
		case WPN_FLAME:
			if(player.ammo)
			{
				ScoreEvent(SE_SHOOT,1);
				if(curMap->flags&MAP_UNDERWATER)
				{
					if(Random(2)==0)
					{
						MakeSound(SND_BLOWBUBBLE,me->x,me->y,SND_CUTOFF,1);
						FireBullet(me->x,me->y,(me->facing*32-6+Random(13))&255,BLT_BUBBLE,1);
					}
				}
				else
					FireBullet(me->x,me->y,me->facing,BLT_FLAME,1);
				player.ammo--;
				if(!editing && !player.cheated && verified)
					profile.progress.shotsFired++;
			}
			c=GetControls();
			if(c&CONTROL_B2)	// fire is held
			{
				player.wpnReload=1;
				me->frmTimer=0;
			}
			else
				player.wpnReload=5;
			DoPlayerFacing(c,me);
			break;
		case WPN_BIGAXE:
			if(player.ammo)
			{
				ScoreEvent(SE_SHOOT,1);
				FireBullet(me->x,me->y,me->facing,BLT_BIGAXE,1);
				MakeSound(SND_BOMBTHROW,me->x,me->y,SND_CUTOFF,1200);
				player.ammo--;
				if(!editing && !player.cheated && verified)
					profile.progress.shotsFired++;
			}
			player.wpnReload=10;
			break;
		case WPN_FREEZE:
			if(player.ammo)
			{
				ScoreEvent(SE_SHOOT,1);
				FireBullet(me->x,me->y,me->facing*32,BLT_FREEZE,1);
				player.ammo--;
				if(!editing && !player.cheated && verified)
					profile.progress.shotsFired++;
			}
			player.wpnReload=10;
			break;
		case WPN_LIGHTNING:
			if(player.ammo)
			{
				ScoreEvent(SE_SHOOT,1);
				// fire lightning
				FireBullet(me->x,me->y,me->facing,BLT_LIGHTNING,1);
				player.ammo--;
				if(!editing && !player.cheated && verified)
					profile.progress.shotsFired++;
			}
			c=GetControls();
			if(c&CONTROL_B2)	// fire is held
			{
				player.wpnReload=1;
				me->frmTimer=0;
			}
			else
			{
				player.wpnReload=5;
			}
			DoPlayerFacing(c,me);
			break;
		case WPN_SPEAR:
			if(player.ammo)
			{
				ScoreEvent(SE_SHOOT,1);
				MakeSound(SND_BOMBTHROW,me->x,me->y,SND_CUTOFF,1200);
				FireBullet(me->x,me->y,me->facing,BLT_SPEAR,1);
				player.ammo--;
				if(!editing && !player.cheated && verified)
					profile.progress.shotsFired++;
			}
			player.wpnReload=5;
			break;
		case WPN_MACHETE:
			if(player.ammo)
			{
				ScoreEvent(SE_SHOOT,1);
				MakeSound(SND_SLASH,me->x,me->y,SND_CUTOFF,1200);
				FireBullet(me->x+Cosine(me->facing*32)*32,me->y+Sine(me->facing*32)*32,
					me->facing,BLT_SLASH,1);
				player.ammo--;
				if(!editing && !player.cheated && verified)
					profile.progress.shotsFired++;
			}
			player.wpnReload=2;
			break;
		case WPN_MINES:
			if(player.ammo)
			{
				ScoreEvent(SE_SHOOT,1);
				MakeSound(SND_MINELAY,me->x,me->y,SND_CUTOFF,1200);
				FireBullet(me->x-Cosine(me->facing*32)*32,me->y-Sine(me->facing*32)*32,
					me->facing,BLT_MINE,1);
				player.ammo--;
				if(!editing && !player.cheated && verified)
					profile.progress.shotsFired++;
			}
			player.wpnReload=15;
			break;
		case WPN_TURRET:
			if(player.ammo)
			{
				Guy *g;

				ScoreEvent(SE_SHOOT,1);
				g=AddGuy(me->x+Cosine(me->facing*32)*32,me->y+Sine(me->facing*32)*32,
					FIXAMT*10,MONS_GOODTURRET,1);
				if(g==NULL || !g->CanWalk(g->x,g->y,curMap,&curWorld))
				{
					MakeSound(SND_TURRETBZZT,me->x,me->y,SND_CUTOFF,1200);
					if(g)
						g->type=MONS_NONE;
				}
				else
				{
					MakeSound(SND_MINELAY,me->x,me->y,SND_CUTOFF,1200);
					player.ammo--;
					if(!editing && !player.cheated && verified)
						profile.progress.shotsFired++;
				}
				player.wpnReload=15;
			}
			break;
		case WPN_MINDCONTROL:
			if(player.ammo)
			{
				ScoreEvent(SE_SHOOT,1);
				MakeSound(SND_MINDWIPE,me->x,me->y,SND_CUTOFF,1200);
				FireBullet(me->x+Cosine(me->facing*32)*32,me->y+Sine(me->facing*32)*32,
					me->facing,BLT_MINDWIPE,1);
				player.ammo--;
				player.wpnReload=15;
				if(!editing && !player.cheated && verified)
					profile.progress.shotsFired++;
			}
			break;
		case WPN_REFLECTOR:
			if(player.ammo)
			{
				ScoreEvent(SE_SHOOT,1);
				MakeSound(SND_LIGHTSON,me->x,me->y,SND_CUTOFF,1200);
				FireBullet(me->x,me->y,me->facing,BLT_REFLECT,1);
				player.ammo--;
				if(!editing && !player.cheated && verified)
					profile.progress.shotsFired++;
				c=GetControls();
				if(c&CONTROL_B2)	// fire is held
				{
					player.wpnReload=0;
					me->frmTimer=0;
				}
				else
				{
					player.wpnReload=10;
				}
			}
			break;
		case WPN_JETPACK:
			if(player.ammo)
			{
				ScoreEvent(SE_SHOOT,1);
				player.jetting=5;
				player.ammo--;
				if(!editing && !player.cheated && verified)
					profile.progress.shotsFired++;
				player.wpnReload=3;
			}
			break;
		case WPN_SWAPGUN:
			if(player.ammo)
			{
				ScoreEvent(SE_SHOOT,1);
				MakeSound(SND_LIGHTSON,me->x,me->y,SND_CUTOFF,1200);
				FireBullet(me->x,me->y,me->facing,BLT_SWAP,1);
				player.ammo--;
				if(!editing && !player.cheated && verified)
					profile.progress.shotsFired++;
				player.wpnReload=10;
			}
			break;
		case WPN_TORCH:
			if(player.ammo)
			{
				ScoreEvent(SE_SHOOT,1);
				MakeSound(SND_FLAMEGO,me->x,me->y,SND_CUTOFF,1200);
				player.ammo--;
				if(!editing && !player.cheated && verified)
					profile.progress.shotsFired++;
				player.wpnReload=3;
				if(playerGlow<100)
					playerGlow=100;
			}
			break;
		case WPN_SCANNER:
			if(player.ammo)
			{
				ScoreEvent(SE_SHOOT,1);
				MakeSound(SND_LIGHTSON,me->x,me->y,SND_CUTOFF,1200);
				FireBullet(me->x,me->y,me->facing,BLT_SCANNER,1);
				player.ammo--;
				if(!editing && !player.cheated && verified)
					profile.progress.shotsFired++;
				player.wpnReload=20;
			}
			break;
		case WPN_STOPWATCH:
			if(player.ammo && !player.timeStop)	// can't use it when time is already stopped
			{
				ScoreEvent(SE_SHOOT,1);
				MakeSound(SND_LIGHTSON,me->x,me->y,SND_CUTOFF,1200);
				player.ammo--;
				if(!editing && !player.cheated && verified)
					profile.progress.shotsFired++;
				player.wpnReload=20;
				player.timeStop=30*3;
			}
			break;
	}
	if(player.ammoCrate)
		player.ammo=maxAmmo[player.weapon];

	if(!player.ammo)
		player.weapon=0;
	GoalFire();
}

void PlayerFirePowerArmor(Guy *me,byte mode)
{
	int x,y,x2,y2;
	byte f;

	switch(mode)
	{
		case 1:
			ScoreEvent(SE_SHOOT,1);
			MakeSound(SND_ARMORSHOOT,me->x,me->y,SND_CUTOFF,1200);
			f=me->facing*32-64;
			x=me->x+Cosine(me->facing*32)*20;
			y=me->y+Sine(me->facing*32)*20;

			x2=x+Cosine(f)*32;
			y2=y+Sine(f)*32;
			FireBullet(x2,y2,me->facing*32-8+Random(17),BLT_BIGSHELL,1);
			x2=x-Cosine(f)*32;
			y2=y-Sine(f)*32;
			FireBullet(x2,y2,me->facing*32-8+Random(17),BLT_BIGSHELL,1);
			if(player.ammo>2)
				player.ammo-=2;
			if(!editing && !player.cheated && verified)
				profile.progress.shotsFired++;
			break;
		case 2:
			ScoreEvent(SE_SHOOT,4);
			QuadMissile(me->x,me->y,me->facing,1);
			if(player.ammo>25)
				player.ammo-=25;
			else
				player.ammo=0;
			if(!editing && !player.cheated && verified)
				profile.progress.shotsFired++;
			break;
		case 3:
			MakeSound(SND_SUBSHOOT,me->x,me->y,SND_CUTOFF,1200);
			ScoreEvent(SE_SHOOT,1);
			f=me->facing*32-64;
			x=me->x+Cosine(me->facing*32)*24;
			y=me->y+Sine(me->facing*32)*24;

			x2=x+Cosine(f)*8;
			y2=y+Sine(f)*8;
			FireBullet(x2,y2,me->facing,BLT_TORPEDO,1);
			x2=x-Cosine(f)*8;
			y2=y-Sine(f)*8;
			FireBullet(x2,y2,me->facing,BLT_TORPEDO,1);
			if(!editing && !player.cheated && verified)
				profile.progress.shotsFired++;
			break;
		case 4:
			ScoreEvent(SE_SHOOT,1);
			MakeSound(SND_SUBMINE,me->x,me->y,SND_CUTOFF,1200);
			x=me->x+Cosine((me->facing*32+128)&255)*24;
			y=me->y+Sine((me->facing*32+128)&255)*24;

			FireBullet(x,y,me->facing,BLT_MINE,1);
			if(!editing && !player.cheated && verified)
				profile.progress.shotsFired++;
			break;
	}
	GoalFire();
}

void PlayerControlMe(Guy *me,mapTile_t *mapTile,world_t *world)
{
	byte c;
	int x,y,i;

	player.stealthy=0;

	player.life=me->hp;

	if(player.hammerFlags&HMR_SPEED)
		player.speed=255;

	if(player.rage)
	{
		if(player.rage>5)
			player.rage-=6;
		else
			player.rage=0;
	}

	if(player.torch)
		player.torch--;

	if(player.timeStop)
		player.timeStop--;

	if(player.ammoCrate)
	{
		player.ammoCrate--;
		if(player.weapon)
			player.ammo=maxAmmo[player.weapon];
	}

	if(player.rageClock && GetGameMode()!=GAMEMODE_RAGE)
		DoRage(me);

	if(player.invisibility)
		player.invisibility--;

	if(player.cheesePower)
		player.cheesePower--;

	if(player.jetting && me->seq!=ANIM_DIE && me->seq!=ANIM_A3)
	{
		me->dx+=Cosine(me->facing*32)*6;
		me->dy+=Sine(me->facing*32)*6;
		Clamp(&me->dx,FIXAMT*20);
		Clamp(&me->dy,FIXAMT*20);

		if(me->z<FIXAMT*20)
			me->z+=FIXAMT*4;
		me->dz=0;

		MakeSound(SND_FLAMEGO,me->x,me->y,SND_CUTOFF,1200);
		for(i=0;i<3;i++)
		{
			c=((me->facing+4)&7)*32;
			x=me->x+Cosine(c)*10-FIXAMT*10+Random(FIXAMT*20);
			y=me->y+Sine(c)*10-FIXAMT*10+Random(FIXAMT*20);
			if(curMap->flags&MAP_UNDERWATER)
				FireBullet(x,y,((me->facing+4)&7)*32,BLT_BUBBLE,1);
			else
				FireBullet(x,y,(me->facing+4)&7,BLT_FLAME,1);
		}
		player.jetting--;
	}

	if(player.weapon==WPN_PWRARMOR)
	{
		PlayerControlPowerArmor(me,mapTile,world);
		return;
	}
	if(player.weapon==WPN_MINISUB)
	{
		PlayerControlMiniSub(me,mapTile,world);
		return;
	}

	if(mapTile->templight<-5 && (curMap->flags&MAP_STEALTH))
	{
		player.stealthy=1;
	}

	if(player.reload)
		player.reload--;
	if(player.wpnReload)
		player.wpnReload--;

	if(player.garlic)
	{
		player.garlic--;
		StinkySteam(me->x-FIXAMT*20+Random(FIXAMT*40),me->y-FIXAMT*20+Random(FIXAMT*40),
					me->z+FIXAMT*40,FIXAMT*2);
	}

	if(player.shield)
		player.shield--;

	if(player.pushPower)
		player.pushPower--;

	if(tportclock)
		tportclock--;

	// ice is slippery
	if(!(GetTerrain(world,mapTile->floor)->flags&TF_ICE) || (player.hammerFlags&HMR_NOSKID))
	{
		if((player.jetting || (me->z>0 && player.vehicle==0)) && me->mind1)
		{
			if(me->mind1&1)
			{
				me->dx=-me->dx;
				switch(me->facing)
				{
					case 0:
						me->facing=4;
						break;
					case 1:
						me->facing=3;
						break;
					case 2:
					case 6:
						break;
					case 3:
						me->facing=1;
						break;
					case 4:
						me->facing=0;
						break;
					case 5:
						me->facing=7;
						break;
					case 7:
						me->facing=5;
						break;
				}
			}
			if(me->mind1&2)
			{
				me->dy=-me->dy;
				switch(me->facing)
				{
					case 0:
					case 4:
						break;
					case 1:
						me->facing=7;
						break;
					case 2:
						me->facing=6;
						break;
					case 3:
						me->facing=5;
						break;
					case 5:
						me->facing=3;
						break;
					case 6:
						me->facing=2;
						break;
					case 7:
						me->facing=1;
						break;
				}
			}
		}
		if(me->z==0 || player.jetting)
		{
			Dampen(&me->dx,PLYR_DECEL);
			Dampen(&me->dy,PLYR_DECEL);
		}
	}
	else
	{
		if(!player.jetting && me->mind1)	// bumped a wall while on ice
		{
			if(me->mind1&1)
				me->dx=-me->dx/8;
			if(me->mind1&2)
				me->dy=-me->dy/8;
		}
		if((player.jetting || (me->z>0 && player.vehicle==0)) && me->mind1)
		{
			if(me->mind1&1)
			{
				me->dx=-me->dx;
				switch(me->facing)
				{
					case 0:
						me->facing=4;
						break;
					case 1:
						me->facing=3;
						break;
					case 2:
					case 6:
						break;
					case 3:
						me->facing=1;
						break;
					case 4:
						me->facing=0;
						break;
					case 5:
						me->facing=7;
						break;
					case 7:
						me->facing=5;
						break;
				}
			}
			if(me->mind1&2)
			{
				me->dy=-me->dy;
				switch(me->facing)
				{
					case 0:
					case 4:
						break;
					case 1:
						me->facing=7;
						break;
					case 2:
						me->facing=6;
						break;
					case 3:
						me->facing=5;
						break;
					case 5:
						me->facing=3;
						break;
					case 6:
						me->facing=2;
						break;
					case 7:
						me->facing=1;
						break;
				}
			}
		}
	}
	me->mind1=0;

	if(me->ouch==4)
	{
		if(player.playAs==PLAY_BOUAPHA)
		{
			if(me->hp>0)
				MakeSound(SND_BOUAPHAOUCH,me->x,me->y,SND_CUTOFF|SND_ONE,2000);
			else if(me->seq==ANIM_DIE)	// so it doesn't do this if you're drowning
				MakeSound(SND_BOUAPHADIE,me->x,me->y,SND_CUTOFF|SND_ONE,2000);
		}
		if(player.playAs==PLAY_LUNACHIK)
		{
			if(me->hp>0)
				MakeSound(SND_LUNAOUCH,me->x,me->y,SND_CUTOFF|SND_ONE,2000);
			else if(me->seq==ANIM_DIE)	// so it doesn't do this if you're drowning
				MakeSound(SND_LUNADIE,me->x,me->y,SND_CUTOFF|SND_ONE,2000);
		}
		else if(player.playAs==PLAY_LUNATIC)
		{
			if(me->hp>0)
				MakeSound(SND_DRLOUCH,me->x,me->y,SND_CUTOFF|SND_ONE,2000);
			else if(me->seq==ANIM_DIE)	// so it doesn't do this if you're drowning
				MakeSound(SND_DRLDIE,me->x,me->y,SND_CUTOFF|SND_ONE,2000);
		}
		else if(player.playAs==PLAY_MECHA)
		{
			if(me->hp>0)
				MakeSound(SND_ROBOBOUAPHAOUCH,me->x,me->y,SND_CUTOFF|SND_ONE,2000);
			else if(me->seq==ANIM_DIE)	// so it doesn't do this if you're drowning
			{
				x=me->x>>FIXSHIFT;
				y=me->y>>FIXSHIFT;
				BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
			}
		}
		else if(player.playAs==PLAY_SHROOM)
		{
			if(me->hp>0)
				MakeSound(SND_MUSHOUCH,me->x,me->y,SND_CUTOFF|SND_ONE,2000);
			else if(me->seq==ANIM_DIE)	// so it doesn't do this if you're drowning
			{
				MakeSound(SND_MUSHDIE,me->x,me->y,SND_CUTOFF|SND_ONE,2000);
				x=me->x>>FIXSHIFT;
				y=me->y>>FIXSHIFT;
				BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
			}
		}
	}

	if(me->parent)	// being grabbed by a Super Zombie or something
	{
		if(me->parent->aiType==MONS_SUPERZOMBIE)
		{
			me->x=me->parent->x+Cosine(me->parent->facing*32)*60;
			me->y=me->parent->y+Sine(me->parent->facing*32)*60;

			me->dx=0;
			me->dy=0;
			me->dz=0;
			if(me->z>40*FIXAMT)
				me->z=40*FIXAMT;
			if(me->parent->frm<4)
				me->z+=FIXAMT*3;
			if(me->parent->frm>18 || me->parent->seq!=ANIM_A3)
			{
				me->z-=FIXAMT*4;
				if(me->parent->frm==21 || me->parent->seq!=ANIM_A3)
				{
					me->z=0;
					me->parent=NULL;
					me->action=ACTION_IDLE;
					if(me->hp==0)
					{
						me->seq=ANIM_DIE;
						me->frm=0;
						me->frmTimer=0;
						me->frmAdvance=64;
						me->action=ACTION_BUSY;
					}
					return;
				}
			}
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=512;
			}
			return;
		}
		else if(me->parent->aiType==MONS_MINECART)
		{
			me->dx=0;
			me->dy=0;
			if(me->ID>me->parent->ID)	// I'm being executed later
			{
				me->x=me->parent->x;
				me->y=me->parent->y+1;
			}
			else
			{
				me->x=me->parent->x+me->parent->dx;
				me->y=me->parent->y+me->parent->dy*2+1;
			}
			me->z=FIXAMT*8;
		}
		else if(me->parent->aiType==MONS_YUGO)
		{
			me->x=me->parent->x;
			me->y=me->parent->y;
			me->z=0;
		}
		else
		{
			me->parent=NULL;
		}
	}

	if((GetTerrain(world,mapTile->floor)->flags&TF_RUBBER) && me->z==0)
	{
		me->dz=16*FIXAMT;
		me->dx+=Cosine(me->facing*32);
		me->dy+=Sine(me->facing*32);
		Clamp(&me->dx,PLYR_MAXSPD);
		Clamp(&me->dy,PLYR_MAXSPD);
		MakeSound(SND_BOING+Random(3),me->x,me->y,SND_CUTOFF,2);
	}
	// triggering stuff
	if(me->action==ACTION_BUSY)
	{
		// throw hammer if need be, use item if need be
		if(me->seq==ANIM_ATTACK && me->frm==6 && player.reload==0 && player.hammers>0)
			PlayerThrowHammer(me);
		if(me->seq==ANIM_ATTACK && me->frm==7)
			me->reload-=2;
		if(me->seq==ANIM_A1 && me->frm==2 && player.wpnReload==0)
			PlayerFireWeapon(me);

		if(me->seq==ANIM_A3)
		{
			if(me->frm<11)
			{
				me->z=FIXAMT*8;	// hover while spinning feet in the air before plunging into water
				me->dz=FIXAMT;
			}
			else
			{
				ExplodeParticles(PART_WATER,me->x,me->y,0,16);
			}
		}
		if(me->seq==ANIM_DIE)
		{
			me->facing=(me->facing+1)&7;

			if(player.playAs==PLAY_MECHA)
			{
				x=me->x>>FIXSHIFT;
				y=me->y>>FIXSHIFT;
				BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
			}
		}

		return;
	}

	// not busy, let's see if you want to do something
	c=newControls;
	if(!player.jetting)
		DoPlayerFacing(c,me);

	if((c&(CONTROL_B1|CONTROL_B2))==(CONTROL_B1|CONTROL_B2) && (player.rage/256)>=player.life && player.ability[ABIL_RAGE])
	{
		// RAGE!!!!!!!
		ScoreEvent(SE_RAGE,1);
		if(!editing && !player.cheated && verified)
			profile.progress.rages++;
		player.rage=0;
		player.rageClock=15;
		if(player.shield==0)
			player.shield=30;
		EnterRage();
	}
	if((c&CONTROL_B1) && player.reload==0)	// pushed hammer throw button
	{
		if(me->type!=MONS_BOUAPHA || (player.ability[ABIL_MOVESHOOT] && profile.moveNShoot))
		{
			if(!(c&(CONTROL_UP|CONTROL_DN|CONTROL_LF|CONTROL_RT)))
			{
				me->seq=ANIM_ATTACK;	// even if unarmed
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=255;
				me->frm+=4-(player.hamSpeed>>2);
			}
			player.boredom=0;
			if(player.hammers>0)
				PlayerThrowHammer(me);
			player.reload+=(10-(4-(player.hamSpeed>>2)));;
		}
		else
		{
			me->action=ACTION_BUSY;
			me->seq=ANIM_ATTACK;	// even if unarmed
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=255;
			me->frm+=4-(player.hamSpeed>>2);
			player.boredom=0;
			return;
		}
	}
	if((c&CONTROL_B2) && player.wpnReload==0 && player.weapon)	// pushed wpn use button
	{
		if(player.weapon==WPN_TORCH)
		{
			if(player.ammo)
			{
				ScoreEvent(SE_SHOOT,1);
				MakeSound(SND_FLAMEGO,me->x,me->y,SND_CUTOFF,1200);
				player.ammo--;
				player.wpnReload=17;
				player.torch=30;
				if(player.ammo==0 && !player.ammoCrate)
					player.weapon=0;
			}
		}
		else
		{
			me->action=ACTION_BUSY;
			me->seq=ANIM_A1;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=200;
			player.boredom=0;
			return;
		}
	}
	if(c&CONTROL_B2)
	{
		if(player.vehicle==VE_YUGO && me->parent && me->parent->mind3==0)
			me->parent->mind3=120;
	}
	// if you are moving indeed
	if((c&(CONTROL_UP|CONTROL_DN|CONTROL_LF|CONTROL_RT)) && (!player.jetting || me->z==0) && (player.vehicle!=VE_RAFT && player.vehicle!=VE_MINECART))
	{
		if(!(GetTerrain(world,mapTile->floor)->flags&TF_ICE) || (player.hammerFlags&HMR_NOSKID))
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

		if(!(GetTerrain(world,mapTile->floor)->flags&TF_MUD) || (player.hammerFlags&HMR_NOSKID))
		{
			Clamp(&me->dx,PLYR_MAXSPD);
			Clamp(&me->dy,PLYR_MAXSPD);

			if(player.reload>0 && !(GetTerrain(world,mapTile->floor)->flags&TF_ICE))
			{
				Clamp(&me->dx,PLYR_MAXSPD/2);
				Clamp(&me->dy,PLYR_MAXSPD/2);
			}
		}
		else
		{
			Clamp(&me->dx,PLYR_MAXSPD/2);
			Clamp(&me->dy,PLYR_MAXSPD/2);
			if(player.reload>0 && !(GetTerrain(world,mapTile->floor)->flags&TF_ICE))
			{
				Clamp(&me->dx,PLYR_MAXSPD/4);
				Clamp(&me->dy,PLYR_MAXSPD/4);
			}
			if(me->z==0)
			{
				SpurtParticles(PART_DIRT,0,me->x,me->y,0,((me->facing*32)+128)&255,8);
				SpurtParticles(PART_DIRT,1,me->x,me->y,0,((me->facing*32)+128)&255,8);
			}
		}

		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
		player.boredom=0;
	}
	else	// you aren't trying to move
	{
		// ice is slippery
		if(me->z==0 && me->dz==0 && (!(GetTerrain(world,mapTile->floor)->flags&TF_ICE)  || (player.hammerFlags&HMR_NOSKID)))
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
	if(me->action==ACTION_IDLE && !player.jetting)
	{
		player.boredom++;
		if(player.boredom>(int)(200+Random(3200)))
		{
			if(player.playAs==PLAY_LUNACHIK)
				MakeSound(SND_LUNABORED,me->x,me->y,SND_CUTOFF|SND_ONE,2000);
			else
				MakeSound(SND_BOUAPHABORED,me->x,me->y,SND_CUTOFF|SND_ONE,2000);
			me->seq=ANIM_A2;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=64;
			player.boredom=0;
		}
	}
}

void PlayerControlPowerArmor(Guy *me,mapTile_t *mapTile,world_t *world)
{
	byte c;

	player.oxygen=127*256;

	if(player.reload)
		player.reload--;
	if(player.wpnReload)
		player.wpnReload--;

	if(player.garlic)
	{
		player.garlic--;
		StinkySteam(me->x-FIXAMT*20+Random(FIXAMT*40),me->y-FIXAMT*20+Random(FIXAMT*40),
					me->z+FIXAMT*40,FIXAMT*2);
	}

	if(player.shield)
		player.shield=0;

	if(player.pushPower)
		player.pushPower--;

	if(tportclock)
		tportclock--;

	if(me->poison)
		// can't be poisoned in armor
		me->poison=0;

	// ice is not slippery for armor
	Dampen(&me->dx,PLYR_DECEL);
	Dampen(&me->dy,PLYR_DECEL);
	me->mind1=0;
	player.boredom=0;

	if(player.ammo)
		player.ammo--;

	if(player.ammo==0 && me->seq!=ANIM_DIE)
	{
		me->seq=ANIM_DIE;
		me->frm=0;
		me->frmAdvance=64;
		me->frmTimer=0;
		me->action=ACTION_BUSY;
	}

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_ROBOBOUAPHAOUCH,me->x,me->y,SND_CUTOFF|SND_ONE,2000);

		// can't be killed in power armor
		//else if(me->seq==ANIM_DIE)	// so it doesn't do this if you're drowning
		//	MakeSound(SND_ROBOBOUAPHADIE,me->x,me->y,SND_CUTOFF|SND_ONE,2000);
	}

	if(me->parent)	// being grabbed by a Super Zombie or something
	{
		if(me->parent->aiType==MONS_MINECART)
		{
			me->x+=me->parent->dx;
			me->y+=me->parent->dy;
		}
		else	// note Super Zombie can't grab you
		{
			me->parent=NULL;
		}
	}

	// not busy, let's see if you want to do something
	c=newControls;

	// triggering stuff
	if(me->action==ACTION_BUSY)
	{
		// throw hammer or fire missile
		if(me->seq==ANIM_ATTACK && me->frm==3 && me->frmTimer<64 && player.reload==0)
		{
			PlayerFirePowerArmor(me,1);
		}
		if(me->seq==ANIM_ATTACK && me->frm==4 && player.ammo>0)
		{
			if(c&CONTROL_B1)
			{
				me->frm=2;
				me->frmTimer=128;
				player.reload=0;
			}
			else
				player.reload=16;	// if you stop firing, it takes a while to start again
		}
		if(me->seq==ANIM_A1 && me->frm==2 && player.wpnReload==0)
		{
			PlayerFirePowerArmor(me,2);
			player.wpnReload=40;
		}

		if(me->seq==ANIM_DIE)
		{
			me->frmAdvance=64;
			if(me->frmTimer<65)
				FireBullet(me->x-FIXAMT*64+Random(FIXAMT*128),me->y-FIXAMT*64+Random(FIXAMT*128),
					0,BLT_BOOM,1);
		}

		return;
	}

	DoPlayerFacing(c,me);

	if((c&CONTROL_B1) && player.reload==0)	// pushed fire button
	{
		me->action=ACTION_BUSY;
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		return;
	}
	if((c&CONTROL_B2) && player.wpnReload==0)	// pushed missile button
	{
		me->action=ACTION_BUSY;
		me->seq=ANIM_A1;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=200;
		return;
	}

	// if you are moving indeed
	if(c&(CONTROL_UP|CONTROL_DN|CONTROL_LF|CONTROL_RT))
	{
		me->dx+=Cosine(me->facing*32)*3/2;
		me->dy+=Sine(me->facing*32)*3/2;
		if((Cosine(me->facing*32)<0 && me->dx>0) || (Cosine(me->facing*32)>0 && me->dx<0))
			me->dx=0;
		if((Sine(me->facing*32)<0 && me->dy>0) || (Sine(me->facing*32)>0 && me->dy<0))
			me->dy=0;

		// move half speed in armor
		if(!(GetTerrain(world,mapTile->floor)->flags&TF_MUD))
		{
			Clamp(&me->dx,PLYR_MAXSPD/2);
			Clamp(&me->dy,PLYR_MAXSPD/2);
		}
		else
		{
			Clamp(&me->dx,PLYR_MAXSPD/4);
			Clamp(&me->dy,PLYR_MAXSPD/4);
			SpurtParticles(PART_DIRT,0,me->x,me->y,0,((me->facing*32)+128)&255,8);
			SpurtParticles(PART_DIRT,1,me->x,me->y,0,((me->facing*32)+128)&255,8);
		}

		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
		player.boredom=0;
	}
	else	// you aren't trying to move
	{
		Dampen(&me->dx,PLYR_DECEL/2);
		Dampen(&me->dy,PLYR_DECEL/2);
		if(me->seq==ANIM_MOVE)
		{
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
}

void PlayerControlMiniSub(Guy *me,mapTile_t *mapTile,world_t *world)
{
	byte c;

	player.oxygen=127*256;

	if(player.reload)
		player.reload--;
	if(player.wpnReload)
		player.wpnReload--;

	if(player.garlic)
	{
		player.garlic--;
		StinkySteam(me->x-FIXAMT*20+Random(FIXAMT*40),me->y-FIXAMT*20+Random(FIXAMT*40),
					me->z+FIXAMT*40,FIXAMT*2);
	}

	if(player.shield)
		player.shield=0;

	if(player.pushPower)
		player.pushPower--;

	if(tportclock)
		tportclock--;

	if(me->poison)
		// can't be poisoned in sub
		me->poison=0;

	// ice is not slippery for sub
	player.boredom=0;

	if(player.ammo)
		player.ammo--;

	if(player.ammo==0 && me->seq!=ANIM_DIE)
	{
		me->seq=ANIM_DIE;
		me->frm=0;
		me->frmAdvance=64;
		me->frmTimer=0;
		me->action=ACTION_BUSY;
	}

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_ROBOBOUAPHAOUCH,me->x,me->y,SND_CUTOFF|SND_ONE,2000);

		// can't be killed in power armor
	}

	if(me->parent)	// being grabbed by a Super Zombie or something
	{
		if(me->parent->aiType==MONS_MINECART)
		{
			me->x+=me->parent->dx;
			me->y+=me->parent->dy;
		}
		else	// note Super Zombie can't grab you
		{
			me->parent=NULL;
		}
	}

	// not busy, let's see if you want to do something
	c=newControls;

	// triggering stuff
	if(me->action==ACTION_BUSY)
	{
		// throw hammer or fire missile
		if(me->seq==ANIM_A1 && me->frm==2 && player.wpnReload==0)
		{
			PlayerFirePowerArmor(me,2);
			player.wpnReload=40;
		}

		if(me->seq==ANIM_DIE)
		{
			me->frmAdvance=64;
			if(me->frmTimer<65)
				FireBullet(me->x-FIXAMT*64+Random(FIXAMT*128),me->y-FIXAMT*64+Random(FIXAMT*128),
					0,BLT_BOOM,1);
		}

		return;
	}

	DoPlayerFacing(c,me);

	if((c&CONTROL_B1) && player.reload==0)	// pushed fire button
	{
		PlayerFirePowerArmor(me,3);
		if(player.ammo>2)
			player.ammo-=2;
		else
			player.ammo=0;
		player.reload=4;
	}
	if((c&CONTROL_B2) && player.wpnReload==0)	// pushed mine button
	{
		PlayerFirePowerArmor(me,4);
		if(player.ammo>20)
			player.ammo-=20;
		else
			player.ammo=0;
		player.wpnReload=30;
	}

	// always move forward
	Dampen(&me->dx,PLYR_DECEL/2);
	Dampen(&me->dy,PLYR_DECEL/2);

	me->dx+=Cosine(me->facing*32);
	me->dy+=Sine(me->facing*32);

	Clamp(&me->dx,PLYR_MAXSPD/2);
	Clamp(&me->dy,PLYR_MAXSPD/2);

	if(!editing && !player.cheated && verified)
		profile.progress.driveDistance+=abs(me->dx/FIXAMT)+abs(me->dy/FIXAMT);

	if(c&(CONTROL_UP|CONTROL_DN|CONTROL_LF|CONTROL_RT))
		player.boredom=0;

	if(me->mind1&1)
		me->dx=-me->dx;
	if(me->mind1&2)
		me->dy=-me->dy;
	me->mind1=0;
}

byte StealWeapon(void)
{
	byte r;

	if(player.hammers==0 && (player.hammerFlags&HMR_REVERSE)==0 && (player.hammerFlags&HMR_REFLECT)==0 &&
		player.hamSpeed==16)
		return 0;	// player has nothing to steal!

	while(1)
	{
		r=(byte)Random(4);
		switch(r)
		{
			case 0:
				if(player.hammers>0)
				{
					player.hammers--;
					return ITM_HAMMERUP;
				}
				break;
			case 1:
				if(player.hamSpeed<16)
				{
					player.hamSpeed+=4;
					return ITM_PANTS;
				}
				break;
			case 2:
				if(player.hammerFlags&HMR_REVERSE)
				{
					player.hammerFlags&=(~HMR_REVERSE);
					return ITM_REVERSE;
				}
				break;
			case 3:
				if(player.hammerFlags&HMR_REFLECT)
				{
					player.hammerFlags&=(~HMR_REFLECT);
					return ITM_REFLECT;
				}
				break;
		}
	}
	return 0;
}

void SetPlayerStart(int x,int y)
{
	pStartX=x;
	pStartY=y;
}

void PutPlayerAtStart(Guy *g)
{
	if(pStartX!=-1)
	{
		g->x=(pStartX*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
		g->y=(pStartY*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
		PutCamera(g->x,g->y);
	}
}

static const char wpnName[MAX_WEAPONS][32] = {
	"None",
	"Missiles",
	"AK-8087",
	"Bombs",
	"Flamethrower",
	"Power Armor",
	"Big Axe",
	"Lightning Rod",
	"Spears",
	"Machete",
	"Mines",
	"Turrets",
	"Mind Control Ray",
	"Reflect Shield",
	"Jetpack",
	"Swapgun",
	"Torch",
	"Scanner",
	"Mini-Sub",
	"Freeze Ray",
	"Stopwatch",
};

const char* GetWeaponName(byte weapon)
{
	if (weapon < MAX_WEAPONS)
		return wpnName[weapon];
	return "???";
}

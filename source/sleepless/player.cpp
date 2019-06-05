#include "winpch.h"
#include "player.h"
#include "music.h"
#include "title.h"
#include "progress.h"
#include "shop.h"
#include "editor.h"
#include "goal.h"
#include "log.h"
#include "journal.h"
#include "fishing.h"

player_t player;
byte playerGlow;	// for torch-lit levels, and for exciting moments
byte tportclock;
byte oldControls,newControls,checkTheControls;

byte ammoCost[6]={10,	// flamebringer
				   2,	// lightreaver
				  25,	// planetsmasher
				   2,	// sparkthrower
				  15,	// earsplitter
				  20};	// bonecrusher

static char wpnName[][16]={
	"None",
	"Flamebringer",
	"Lightreaver",
	"Planetsmasher",
	"Sparkthrower",
	"Earsplitter",
	"Bonecrusher",
};

static char wpnDesc[][48]={
	"",
	"Sets monsters on fire!",
	"Reflects shots and heals!",
	"Creates black holes and portals!",
	"Spews monster-seeking sparks!",
	"Flattens monsters through walls!",
	"Summons and controls a skeleton!",
};

void ShouldCheckControls(byte n)
{
	byte c;
	int msx,msy;

	checkTheControls=n;
	c=GetControls();

	if(c&(CONTROL_UP|CONTROL_DN|CONTROL_LF|CONTROL_RT))	// if you press any direction key, then center the mouse
		GetDisplayMGL()->SetMouse(320,240);

	if(GetDisplayMGL()->MouseDown())
		c|=CONTROL_B1;
	if(GetDisplayMGL()->RMouseDown())
		c|=CONTROL_B2;

	GetDisplayMGL()->GetMouse(&msx,&msy);

	if(msx<300)
		msx=300;
	if(msx>340)
		msx=340;
	if(msy<220)
		msy=220;
	if(msy>260)
		msy=260;
	GetDisplayMGL()->SetMouse(msx,msy);
	if(msx==300)
		c|=CONTROL_LF;
	if(msx==340)
		c|=CONTROL_RT;
	if(msy==220)
		c|=CONTROL_UP;
	if(msy==260)
		c|=CONTROL_DN;

	oldControls=newControls;
	newControls=c;
}

byte FakeGetControls(void)
{
	return newControls;
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

	strcpy(player.worldName,fname);
	player.worldProg=GetWorldProgress(player.worldName);

	player.levelNum=level;
	player.levelProg=GetLevelProgress(player.worldName,player.levelNum);

	for(i=0;i<4;i++)
		player.keys[i]=0;

	for(i=0;i<8;i++)
		player.var[i]=0;

	oldControls=0;
	newControls=0;

	player.xp=0;
	player.needXP=60;
	player.level=1;
	GetMonsterType(MONS_BOUAPHA)->hp=PlayerMaxLife();

	player.hammerCharge=0;
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
	player.life=PlayerMaxLife();
	player.shield=0;
	playerGlow=0;
	player.pushPower=0;
	player.vehicle=0;
	player.garlic=0;
	player.speed=0;
	player.rageClock=0;
	player.rage=0;
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

	for(i=0;i<6;i++)
	{
		player.ability[i]=0;
	}

	for(i=0;i<6;i++)
	{
		player.weaponLvl[i]=0;
		player.wpnXP[i]=0;
	}

	player.brainTime=30*30;
	player.brainX=255;
	player.candleX=255;
	player.candleTime=30*30;
	player.spotted=0;
	player.playAs=0;

	for(i=0;i<MAX_MAPS;i++)
		player.beenThere[i]=0;

	for(i=0;i<64;i++)
		player.journal[i]=0;
	player.journal[0]=1;

	player.jpage=0;
	player.enterX=-1;
	player.mapFrom=0;
}

void ExitPlayer(void)
{
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
	if(me->poison<amt)
		me->poison=amt;
}

void PlayerWinLevel(byte isSecret)
{
	PrintToLog("PlayerWinLevel",0);
	player.levelProg->flags|=LF_PASSED;
	if(curMap->numCandles==0)
		player.levelProg->flags|=LF_CANDLES;	// get candle credit if there aren't any

	PrintToLog("StoreWorldResults",0);
	StoreWorldResults(player.worldProg,&curWorld);
	PrintToLog("SaveProfile",0);
	SaveProfile();
}

void SetPlayerHP(int hp)
{
	player.life=hp;
	if(hp<goodguy->maxHP)
		player.perfect=0;
}

void KeyChainAllCheck(void)
{
	if((player.worldProg->keychains&(KC_KEYCH1|KC_KEYCH2|KC_KEYCH3|KC_KEYCH4))==(KC_KEYCH1|KC_KEYCH2|KC_KEYCH3|KC_KEYCH4))
	{
		NewBigMessage("I collected all four!",60);
		playerGlow=127;
		HealRing(7,goodguy->x,goodguy->y,FIXAMT*20,255,5);
		MakeNormalSound(SND_ALLKEYCHAIN);
	}
}

int WeaponMaxAmmo(byte wpn)
{
	return 100;
}

byte PlayerGetWeapon(byte wpn,int x,int y)
{
	int cx,cy;

	GetCamera(&cx,&cy);
	cx<<=FIXSHIFT;
	cy<<=FIXSHIFT;

	if(wpn<1 || wpn>6)
		return 0;

	player.weaponLvl[wpn-1]=1;
	playerGlow=127;
	HealRing(7,goodguy->x,goodguy->y,FIXAMT*20,255,5);

	player.lastWeapon=wpn;
	player.weapon=wpn;

	ColorizeBouapha();

	return 1;
}

byte PlayerPowerup(char powerup)
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
			case PU_CHEESE:
				player.cheesePower=255;
				break;
			case PU_POISON:
				if(goodguy->poison+128<255)
					goodguy->poison+=128;
				else
					goodguy->poison=255;
				break;
			case PU_BRAIN:
				player.ability[ABIL_BRAIN]=1;
				break;
			case PU_SOLAR:
				player.ability[ABIL_SOLAR]=1;
				break;
			case PU_FIRSTAID:
				player.ability[ABIL_FIRSTAID]=1;
				break;
			case PU_TRAINING:
				player.ability[ABIL_TRAINING]=1;
				break;
			case PU_BLOCK:
				player.ability[ABIL_SHIELD]=1;
				break;
			case PU_BLASTING:
				player.hammerFlags|=HMR_BLAST;
				break;
			case PU_BIONIC:
				player.hammerFlags|=HMR_BIONIC;
				break;
			case PU_OTHER:
				//player.ability[ABIL_FIRSTAID]=1;
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
	if(amt>0)
	{
		player.brainTime=30*30;
		player.brainX=255;
	}

	player.brains+=amt;
	if(amt>0 && !editing && !player.cheated)
	{
		profile.progress.totalBrains+=amt;
	}

	if(player.brains<0)
		player.brains=0;

	if(player.brains==100)
	{
		MakeNormalSound(SND_KOOLKAT);
		playerGlow=127;
		CompleteGoal(7);
	}
}

void PlayerGetCandle(int amt)
{
	player.candles+=amt;
	if(amt>0 && !editing && !player.cheated)
	{
		profile.progress.totalCandles+=amt;
	}

	if(player.candles<0)
		player.candles=0;

	if(player.candles==100)
	{
		MakeNormalSound(SND_ALLCANDLE);
		playerGlow=127;
		CompleteGoal(8);
	}
}

void PlayerGetCoin(int amt)
{
	player.coins+=amt;
	if(player.coins<0)
		player.coins=0;
	if(player.coins>99)
		player.coins=99;
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
	byte hammerCount;

	GoalFire();

	hammerCount=player.hammerCharge/100;
	if(hammerCount>5)
		hammerCount=5;
	if(hammerCount<1)
		hammerCount=1;

	if(player.hammerCharge<(500-player.hammers*45))
		player.hammerCharge=0;
	else
		player.hammerCharge-=(500-player.hammers*45);

	HammerLaunch(me->x,me->y,me->facing,hammerCount,player.hammerFlags);

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
		player.life+=amt;
	}
	else
	{
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
	int i;
	static byte flameFlip=0;
	Guy *g;

	// fishing!
	if(player.journal[50]>0)
	{
		player.brainTime=30*5;
		player.journal[50]--;
		if(player.journal[50]==0)
		{
			me->frm=3;
			player.wpnReload=20;
			return;
		}

		me->frmTimer=0;
		if(me->frm>2)
			me->frm--;

		if((newControls&CONTROL_B2) && player.journal[51]<200 && player.journal[52]==1)	// still holding it down
		{
			player.journal[51]+=6;
			return;
		}

		if(player.journal[53])
		{
			player.journal[53]--;
			if(player.journal[53]>=6 && (newControls&CONTROL_B1) && !(oldControls&CONTROL_B1))
			{
				CatchFish(me->x+Cosine(me->facing*32)*player.journal[51],me->y+Sine(me->facing*32)*player.journal[51],curMap,&curWorld);
				me->frm=3;
				return;
			}
		}
		if(player.journal[53]<6 && (newControls&CONTROL_B1) && !(oldControls&CONTROL_B1))
		{
			player.journal[50]=0;
			me->frm=3;
			player.wpnReload=20;
			NewMessage("Nothing on the line!",60,1);
		}

		return;
	}

	if(player.life==0)
		return;	// no shooting when you're dead

	if(player.weapon==WPN_NONE && player.wpnReload==0)
	{
		if(player.ammo>=10)
		{
			MakeNormalSound(SND_FISHCAST);
			player.ammo-=10;
			player.journal[50]=255;	// how long you have to fish
			player.journal[51]=20;	// how far from you it is
			player.journal[52]=1;	// whether you are allowed to keep extending
			me->frmAdvance=60;
			g=AddGuy(me->x,me->y,FIXAMT*20,MONS_BOBBER,1);
			if(g)
				g->dz=FIXAMT;
			else
			{
				player.journal[50]=0;
			}
		}
		return;
	}

	if(player.ammo<ammoCost[player.weapon-1])
		return;
	player.ammo-=ammoCost[player.weapon-1];

	switch(player.weapon)
	{
		case WPN_FLAME:
			FireBullet(me->x,me->y,me->facing*32,BLT_IGNITE,1);
			player.wpnReload=5;
			break;
		case WPN_PORTAL:
			if(SpecialPaintPurple(me->mapx,me->mapy))
			{
				player.wpnReload=15;
			}
			else
			{
				FireBullet(me->x,me->y,me->facing*32,BLT_HOLESHOT,1);
				player.wpnReload=15;
			}
			break;
		case WPN_REFLECT:
			MakeSound(SND_LIGHTSON,me->x,me->y,SND_CUTOFF,1200);
			FireBullet(me->x,me->y,me->facing,BLT_REFLECT,1);
			if(Random(20)<player.weaponLvl[WPN_REFLECT-1] && goodguy->hp<goodguy->maxHP)
			{
				HealGoodguy(1);
				PlayerGetWeaponXP(WPN_REFLECT,1);
			}

			c=FakeGetControls();
			player.shield=4;
			if(c&CONTROL_B2)	// fire is held
			{
				player.wpnReload=0;
			}
			else
			{
				player.wpnReload=10;
			}
			break;
		case WPN_SPARKS:
			MakeSound(SND_CACTONSHOOT,me->x,me->y,SND_CUTOFF,200);
			for(i=0;i<(player.weaponLvl[WPN_SPARKS-1]/2)+1;i++)
				FireBullet(me->x+Cosine(me->facing*32)*32,me->y+Sine(me->facing*32)*24,me->facing*32,BLT_SCANSHOT,1);

			me->z+=FIXAMT*Random(4);
			me->dx+=FIXAMT/2-Random(FIXAMT);
			me->dy+=FIXAMT/2-Random(FIXAMT);
			c=FakeGetControls();
			if(c&CONTROL_B2)	// fire is held
			{
				player.wpnReload=2;
				me->frmTimer=0;
				me->frm--;
			}
			else
			{
				player.wpnReload=5;
				flameFlip=0;
			}
			DoPlayerFacing(c,me);
			break;
		case WPN_SONIC:
			FireBullet(me->x+Cosine(me->facing*32)*10,me->y+Sine(me->facing*32)*10,me->facing*32,BLT_BIGAXE,1);
			MakeSound(SND_SDZLSPIT,me->x,me->y,SND_CUTOFF,1200);
			player.wpnReload=30;
			break;
		case WPN_BONEHEAD:
			g=GetGuyOfType(MONS_GOODBONE);
			if(g)
			{
				byte a;

				a=Random(256);
				g->type=MONS_NONE;
				MakeSound(SND_BOMBBOOM,g->x,g->y,SND_CUTOFF,400);
				for(i=0;i<player.weaponLvl[WPN_BONEHEAD-1]*3;i++)
					FireBullet(g->x,g->y,(a+(256*i)/((player.weaponLvl[WPN_BONEHEAD-1]*3)))&255,BLT_GREEN,g->friendly);
			}
			g=AddGuy(me->x+Cosine(me->facing*32)*32,me->y+Sine(me->facing*32)*32,
				FIXAMT*10,MONS_GOODBONE,1);
			if(g==NULL || !g->CanWalk(g->x,g->y,curMap,&curWorld))
			{
				MakeSound(SND_TURRETBZZT,me->x,me->y,SND_CUTOFF,1200);
				if(g)
				{
					g->type=MONS_NONE;
					player.ammo+=ammoCost[WPN_BONEHEAD-1];
				}
			}
			else
			{
				MakeSound(SND_SKELALIVE,me->x,me->y,SND_CUTOFF,1200);
				g->seq=ANIM_A4;
				g->frm=0;
				g->frmTimer=0;
				g->frmAdvance=128;
				g->facing=me->facing;
				g->action=ACTION_BUSY;
				g->dx=Cosine(g->facing*32)*8;
				g->dy=Sine(g->facing*32)*8;
			}
			player.wpnReload=30;
			break;
	}

	GoalFire();
}

void PlayerControlMe(Guy *me,mapTile_t *mapTile,world_t *world)
{
	byte c;

	if(player.enterX==-2 && !(curMap->flags&MAP_LAVA))
	{
		player.enterX=me->mapx;
		player.enterY=me->mapy;
	}
	profile.progress.totalPercent=(float)PlayerPercent();

	if(player.hammerCharge<500 && player.hammers>0)
		player.hammerCharge+=player.hammers;

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

	if(player.invisibility)
		player.invisibility--;

	if(player.cheesePower)
		player.cheesePower--;

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
	if(!(GetTerrain(world,mapTile->floor)->flags&TF_ICE))
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
		if(me->hp>0)
			MakeSound(SND_BOUAPHAOUCH,me->x,me->y,SND_CUTOFF|SND_ONE,2000);
		else if(me->seq==ANIM_DIE)	// so it doesn't do this if you're drowning
			MakeSound(SND_BOUAPHADIE,me->x,me->y,SND_CUTOFF|SND_ONE,2000);
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
		{
			PlayerFireWeapon(me);
		}
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
		}

		return;
	}

	// not busy, let's see if you want to do something
	c=newControls;

	if(!player.jetting)
		DoPlayerFacing(c,me);

	if((c&CONTROL_B1) && player.reload==0)	// pushed hammer throw button
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
	if((c&CONTROL_B2) && player.wpnReload==0 && (player.weapon || (player.ability[ABIL_FISH] && player.hammers>0)))	// pushed wpn use button
	{
		if(player.weapon!=WPN_REFLECT)	// reflect requires no animation
		{
			me->action=ACTION_BUSY;
			me->seq=ANIM_A1;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=200;
			player.boredom=0;
			return;
		}
		else
			PlayerFireWeapon(me);
	}
	if(c&CONTROL_B2)
	{
		if(player.vehicle==VE_YUGO && me->parent && me->parent->mind3==0)
			me->parent->mind3=120;
	}
	// if you are moving indeed
	if((c&(CONTROL_UP|CONTROL_DN|CONTROL_LF|CONTROL_RT)) && (!player.jetting || me->z==0) && (player.vehicle!=VE_RAFT && player.vehicle!=VE_MINECART))
	{
		if(!(GetTerrain(world,mapTile->floor)->flags&TF_ICE))
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

		if(!(GetTerrain(world,mapTile->floor)->flags&TF_MUD))
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
		if(me->z==0 && me->dz==0 && (!(GetTerrain(world,mapTile->floor)->flags&TF_ICE)))
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
			MakeSound(SND_BOUAPHABORED,me->x,me->y,SND_CUTOFF|SND_ONE,2000);
			me->seq=ANIM_A2;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=64;
			player.boredom=0;
		}
	}
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
	player.enterX=x;
	player.enterY=y;
}

void PutPlayerAtStart(Guy *g)
{
	if(player.enterX==-2)
	{
		// leave him be!  He's already there
	}
	else if(player.enterX!=-1)
	{
		g->x=(player.enterX*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
		g->y=(player.enterY*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
	}
	else
	{
		int i;

		for(i=0;i<MAX_MAPMONS;i++)
			if(curMap && curMap->badguy[i].type==MONS_BOUAPHA)
			{
				g->x=(curMap->badguy[i].x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
				g->y=(curMap->badguy[i].y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
			}
	}
}

void PlayerGetXP(int amt)
{
	if(player.level==20)
		return;

	if(player.ability[ABIL_TRAINING] && Random(4)==0)
		amt*=2;

	player.xp+=amt;
	while(player.xp>=player.needXP && player.level<20)
	{
		player.level++;
		player.xp-=player.needXP;
		player.needXP=player.needXP*3/2;	// increase XP needed by 50%

		if(player.needXP>20000)
			player.needXP=20000;
		GetMonsterType(MONS_BOUAPHA)->hp=PlayerMaxLife();
		goodguy->maxHP=MonsterHP(MONS_BOUAPHA);

		player.life=PlayerMaxLife();
		goodguy->hp=goodguy->maxHP;
		NewBigMessage("Level Up!",30);
		MakeNormalSound(SND_LEVELUP);
		NewMessage("Damage and Life increased.",90,0);
	}
	if(player.level==20)
		CompleteGoal(0);
}

char *WeaponName(byte n)
{
	return wpnName[n];
}

char *WeaponDesc(byte n)
{
	return wpnDesc[n];
}

void UpdateGoodBone(void)
{
	if(player.weaponLvl[WPN_BONEHEAD-1]==0)
		return;

	GetMonsterType(MONS_GOODBONE)->hp=player.weaponLvl[WPN_BONEHEAD-1]*10;
}

void PlayerGetWeaponXP(byte wpn,word amt)
{
	char s[32];

	if(player.ability[ABIL_TRAINING] && Random(4)==0)
		amt*=2;

	wpn--;
	if(wpn<0 || wpn>5)
		return;

	if(player.weaponLvl[wpn]==0)
		return;	// just in case

	if(player.weaponLvl[wpn]<10)
	{
		player.wpnXP[wpn]+=amt;
		while(player.wpnXP[wpn]>=NeedWeaponXP(wpn+1) && player.weaponLvl[wpn]<10)
		{
			player.wpnXP[wpn]-=NeedWeaponXP(wpn+1);
			player.weaponLvl[wpn]++;
			sprintf(s,"%s Up!",WeaponName(wpn+1));
			MakeNormalSound(SND_WEAPONUP);
			NewBigMessage(s,90);
			UpdateGoodBone();
		}
	}
	if(player.weaponLvl[wpn]==10)
		CompleteGoal(wpn+1);
}

word NeedWeaponXP(byte wpn)
{
	return (player.weaponLvl[wpn-1]*100);
}

byte PlayerWpnColor(void)
{
	if(player.weapon==WPN_FLAME)
		return 5;
	if(player.weapon==WPN_REFLECT)
		return 7;
	if(player.weapon==WPN_PORTAL)
		return 6;
	if(player.weapon==WPN_SPARKS)
		return 1;
	if(player.weapon==WPN_SONIC)
		return 3;
	if(player.weapon==WPN_BONEHEAD)
		return 0;

	return 4;
}

void ColorizeBouapha(void)
{
	GetMonsterType(MONS_BOUAPHA)->fromCol=4;
	GetMonsterType(MONS_BOUAPHA)->toCol=PlayerWpnColor();
	goodguy->fromColor=4;
	goodguy->toColor=PlayerWpnColor();
}

byte PlayerMaxLife(void)
{
	if(player.level*5+30>128)
		return 128;
	else
		return player.level*5+30;
}

byte GetAmmoCost(byte wpn)
{
	if(player.weapon==WPN_NONE)
	{
		if(player.ability[ABIL_FISH])
			return 10;
		else
			return 101;	// always red
	}
	return ammoCost[wpn];
}

void PlayerCalcScore(void)
{
	// -1 per 30/th of a second
	// +100 per brain collected
	// +1000 per candle
	// +10000 per bonus item equipped (to refund you for spending the candles)
	// +50000 per mystical hammer
	// +3000 per pants or hammer
	// -1000 per death
	// +1 per XP point
	// +500 per level of each mystical hammer
	// +2000 per journal page
	int i;
	byte pnts;

	i=16;
	pnts=0;
	while(i>(int)player.hamSpeed)
	{
		pnts++;
		if(i>4)
			i-=2;
		else if(i>0)
			i--;
	}

	player.score=player.brains*100+player.candles*1000+player.hammers*3000+pnts*3000+player.xp;
	for(i=1;i<NUM_PAGES;i++)
		if(player.journal[i])
			player.score+=2000;

	for(i=0;i<6;i++)
		player.score+=player.weaponLvl[i]*500;
	if(player.ability[ABIL_BRAIN])
		player.score+=10000;
	if(player.ability[ABIL_SOLAR])
		player.score+=10000;
	if(player.ability[ABIL_SHIELD])
		player.score+=10000;
	if(player.ability[ABIL_TRAINING])
		player.score+=10000;
	if(player.ability[ABIL_FIRSTAID])
		player.score+=10000;
	if(player.hammerFlags&HMR_BIONIC)
		player.score+=10000;
	if(player.hammerFlags&HMR_BLAST)
		player.score+=10000;
	if(player.hammerFlags&HMR_REVERSE)
		player.score+=10000;
	if(player.hammerFlags&HMR_REFLECT)
		player.score+=10000;

	if(player.score>(dword)(1000*profile.progress.kills[MONS_BOUAPHA]))
		player.score-=1000*profile.progress.kills[MONS_BOUAPHA];
	else
		player.score=0;

	if(player.score>profile.progress.totalTime)
		player.score-=profile.progress.totalTime;
	else
		player.score=0;
}

byte PlayerPercent(void)
{
	// percentage is:
	// 10pts per brain (100)
	// 10pts per candle (100)
	// 100pts per hammer (6)
	// 20pts per hammer-up (10)
	// 20pts per pants (10)
	// 100ps per key (RGB only, 3)
	// 10 points for actually winning the game (player.journal[63] is used to store this info)

	word total,possTotal;
	int i;
	byte result;

	possTotal=10*100+10*100+100*6+100*3+10+20*10+20*10;
	total=player.brains*10+player.candles*10+player.keys[1]*100+player.keys[2]*100+player.keys[3]*100+player.hammers*20;
	for(i=0;i<6;i++)
		if(player.weaponLvl[i]>0)
			total+=100;
	if(player.journal[63])
		total+=10;

	if(player.hammers==10)
		total+=100;	// if you have all hammers, then you MUST have the Mr. Klein hammer, and so you need a candle refund

	byte pnts;

	i=16;
	pnts=0;
	while(i>(int)player.hamSpeed)
	{
		pnts++;
		if(i>4)
			i-=2;
		else if(i>0)
			i--;
	}
	total+=pnts*20;

	if(player.ability[ABIL_BRAIN])
		total+=100;
	if(player.ability[ABIL_SOLAR])
		total+=100;
	if(player.ability[ABIL_SHIELD])
		total+=100;
	if(player.ability[ABIL_TRAINING])
		total+=100;
	if(player.ability[ABIL_FIRSTAID])
		total+=100;
	if(player.hammerFlags&HMR_BIONIC)
		total+=100;
	if(player.hammerFlags&HMR_BLAST)
		total+=100;
	if(player.hammerFlags&HMR_REVERSE)
		total+=100;
	if(player.hammerFlags&HMR_REFLECT)
		total+=100;

	result=(total*100)/possTotal;
	if(result>=100 && total<possTotal)
		result=99;
	if(result>100)
		result=100;

	if(result==100)
		CompleteGoal(13);

	return result;
}

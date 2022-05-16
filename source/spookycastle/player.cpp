#include "player.h"
#include "appdata.h"

#define PLYR_ACCEL	(FIXAMT)
#define PLYR_DECEL	(FIXAMT*3/4)
#define PLYR_MAXSPD (FIXAMT*6)

player_t player;
byte playerGlow;	// for torch-lit levels, and for exciting moments
byte tportclock;

void InitPlayer(byte initWhat,byte world,byte level)
{
	int i,j;

	if(initWhat==INIT_GAME)	// initialize everything, this is to start a whole new game
	{
		player.score=0;
		player.musicSettings=MUSIC_OFF;
		for(i=0;i<6;i++)
		{
			for(j=0;j<MAX_MAPS;j++)
				player.levelPassed[i][j]=0;
			for(j=0;j<4;j++)
				player.keychain[i][j]=0;
			player.totalCompletion[i]=100;
			player.complete[i]=0;
			player.lunacyKey[i]=0;
		}
	}
	if(initWhat>=INIT_WORLD) // initialize the things that go with each world
	{
		player.levelsPassed=0;
		player.worldNum=world;
		for(i=0;i<MAX_MAPS;i++)
			if(player.levelPassed[world][i])
				player.levelsPassed++;
	}

	player.levelNum=level;
	player.prevScore=player.score;	// back up the score (if you give up or die, it is reset)

	for(i=0;i<4;i++)
		player.keys[i]=0;

	player.brains=0;
	player.boredom=0;
	player.hammers=0;
	player.hamSpeed=16;
	player.weapon=WPN_NONE;
	player.ammo=0;
	player.reload=0;
	player.wpnReload=0;
	player.hammerFlags=0;
	player.life=128;
	player.shield=0;
	playerGlow=0;
	player.pushPower=0;
	player.poison=0;
}

void ExitPlayer(void)
{
}

void PlayerLoadGame(byte which)
{
	FILE *f;

	f=AppdataOpen("loony.sav");
	if(!f)
	{
		InitPlayer(INIT_GAME,0,0);
	}
	else
	{
		fseek(f,sizeof(player_t)*which,SEEK_SET);
		fread(&player,sizeof(player_t),1,f);
		fclose(f);
	}
}

void PlayerSaveGame(byte which)
{
	FILE *f;
	player_t p[3];
	int i;

	f=AppdataOpen("loony.sav");
	if(!f)
	{
		memset(p,0,sizeof(player_t)*3);	// make an empty player
		for(i=0;i<6;i++)
		{
			p[0].totalCompletion[i]=100;
			p[1].totalCompletion[i]=100;
			p[2].totalCompletion[i]=100;
		}
		f=AppdataOpen_Write("loony.sav");
		fwrite(p,sizeof(player_t),3,f);
		fclose(f);
		f=AppdataOpen("loony.sav");
	}
	fread(p,sizeof(player_t),3,f);
	fclose(f);
	memcpy(&p[which],&player,sizeof(player_t));
	f=AppdataOpen_Write("loony.sav");
	fwrite(p,sizeof(player_t),3,f);
	fclose(f);
}

void PlayerSetWorldWorth(byte world,int amt)
{
	player.totalCompletion[world]=amt;
}

void PlayerRenderInterface(MGLDraw *mgl)
{
	int b;

	b=TotalBrains();
	if(b!=0)
		b=128-(player.brains*128/b);

	RenderInterface(player.life,player.hammerFlags,player.hammers,b,player.score,
					player.weapon,player.ammo,player.hamSpeed,mgl);
}

void SetCustomName(const char *name)
{
	strncpy(player.customName,name,32);
}

char *GetCustomName(void)
{
	return player.customName;
}

float PlayerGetPercent(byte world)
{
	if(player.totalCompletion[world]==0)
		return 1.0;
	else
		return (float)player.complete[world]/(float)player.totalCompletion[world];
}

float PlayerGetGamePercent(void)
{
	int i,amt,total;

	amt=0;
	total=0;
	for(i=0;i<5;i++)
	{
		total+=player.totalCompletion[i];
		amt+=player.complete[i];
	}
	return (float)amt/(float)total;
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

byte PlayerIsPoisoned(void)
{
	return player.poison;
}

void PoisonPlayer(byte amt)
{
	if(player.poison+amt>255)
		player.poison=255;
	else
		player.poison+=amt;
}

void PlayerResetScore(void)
{
	player.score=player.prevScore;
}

byte PlayerPassedLevel(byte world,byte map)
{
	return player.levelPassed[world][map];
}

void PlayerWinLevel(byte w,byte l,byte isSecret)
{
	if(!player.levelPassed[w][l])
	{
		player.complete[w]+=100;	// get some percentage points
		if(!isSecret)
			player.levelsPassed++;	// secret levels aren't counted in this (it's for triggering specials)
	}
	else
		PlayerResetScore();	// you can't get points for a level you've already passed
	player.levelPassed[w][l]=1;
}

byte GetPlayerWorld(void)
{
	return player.worldNum;
}

void SetPlayerHP(int hp)
{
	player.life=(byte)hp;
}

byte PlayerLevelsPassed(void)
{
	return player.levelsPassed;
}

void KeyChainAllCheck(void)
{
	if(player.keychain[player.worldNum][0]==1 &&
		player.keychain[player.worldNum][1]==1 &&
		player.keychain[player.worldNum][2]==1 &&
		player.keychain[player.worldNum][3]==1)
		NewBigMessage("I collected all four!",30);
}

byte PlayerGetItem(byte itm)
{
	int cx,cy;

	GetCamera(&cx,&cy);
	cx<<=FIXSHIFT;
	cy<<=FIXSHIFT;
	switch(itm)
	{
		case ITM_HAMMERUP:
			if(player.hammers<5)
				player.hammers++;
			MakeNormalSound(SND_HAMMERUP);
			NewMessage("HAMMER UP!",75);
			player.score+=150;
			return 0;
			break;
		case ITM_PANTS:
			if(player.hamSpeed>0)
				player.hamSpeed-=4;
			NewMessage("PANTS OF POWER!",75);
			MakeNormalSound(SND_PANTS);
			player.score+=100;
			return 0;
			break;
		case ITM_REVERSE:
			player.hammerFlags|=HMR_REVERSE;
			NewMessage("REVERSE HAMMER!",75);
			MakeNormalSound(SND_REVERSE);
			player.score+=100;
			return 0;
			break;
		case ITM_REFLECT:
			player.hammerFlags|=HMR_REFLECT;
			NewMessage("REFLECT HAMMER!",75);
			MakeNormalSound(SND_REFLECT);
			player.score+=100;
			return 0;
			break;
		case ITM_MISSILES:
			if(player.weapon==WPN_MISSILES && player.ammo==20)	// don't get it
				return 1;
			MakeNormalSound(SND_WEAPON);
			NewMessage("MISSILE PACK!",75);
			player.weapon=WPN_MISSILES;
			player.ammo=20;
			player.score+=50;
			return 0;
			break;
		case ITM_FLAME:
			if(player.weapon==WPN_FLAME && player.ammo==50)	// don't get it
				return 1;
			MakeNormalSound(SND_WEAPON);
			NewMessage("FLAMETHROWER!",75);
			player.weapon=WPN_FLAME;
			player.ammo=50;
			player.score+=50;
			return 0;
			break;
		case ITM_BOMBS:
			if(player.weapon==WPN_BOMBS && player.ammo==5)	// don't get it
				return 1;
			MakeNormalSound(SND_WEAPON);
			NewMessage("CHERRY BOMBS!",75);
			player.weapon=WPN_BOMBS;
			player.ammo=5;
			player.score+=50;
			return 0;
			break;
		case ITM_AK8087:
			if(player.weapon==WPN_AK8087 && player.ammo==99)	// don't get it
				return 1;
			MakeNormalSound(SND_WEAPON);
			NewMessage("AK-8087!",75);
			player.weapon=WPN_AK8087;
			player.ammo=99;
			player.score+=50;
			return 0;
			break;
		case ITM_TAKEOUT:
			MakeNormalSound(SND_FOOD);
			switch(MGL_random(8))
			{
				case 0:
					NewMessage("Just fried rice",75);
					PlayerHeal(5);
					break;
				case 1:
					NewMessage("Mmm, Kung Pao!",75);
					PlayerHeal(25);
					break;
				case 2:
					NewMessage("Szechwan Shredded Pork!!",75);
					PlayerHeal(50);
					break;
				case 3:
					NewMessage("Moo Shi Beef!",75);
					PlayerHeal(25);
					break;
				case 4:
					NewMessage("Ick, mostly veggies",75);
					PlayerHeal(1);
					break;
				case 5:
					NewMessage("Hmm, tastes kind of citrusy",75);
					PlayerHeal(15);
					break;
				case 6:
					NewMessage("Joy! Wontons!",75);
					PlayerHeal(20);
					break;
				case 7:
					NewMessage("GeneralTsao has outdone himself",75);
					PlayerHeal(40);
					break;
			}
			player.score+=50;
			return 0;
			break;
		case ITM_SHIELD:
			MakeNormalSound(SND_SHIELD);
			player.shield=240;
			player.score+=50;
			NewMessage("Energy Barrier!",30);
			return 0;
			break;
		case ITM_BRAIN:
			MakeNormalSound(SND_GETBRAIN);
			player.brains++;
			if(player.brains==TotalBrains())
			{
				MakeNormalSound(SND_KOOLKAT);
				playerGlow=127;
			}
			player.score+=25;
			return 0;
			break;
		case ITM_KEY:
			if(player.keys[0]<3)
			{
				player.keys[0]++;
				MakeNormalSound(SND_GETKEY);
				return 0;
			}
			else
				return 1;	// don't pick it up
			break;
		case ITM_KEYR:
			MakeNormalSound(SND_GETKEY);
			player.keys[1]=1;
			return 0;
			break;
		case ITM_KEYG:
			MakeNormalSound(SND_GETKEY);
			player.keys[2]=1;
			return 0;
			break;
		case ITM_KEYB:
			MakeNormalSound(SND_GETKEY);
			player.keys[3]=1;
			return 0;
			break;
		case ITM_KEYCH1:
			MakeNormalSound(SND_GETKEYCHAIN);
			player.keychain[player.worldNum][0]=1;
			KeyChainAllCheck();
			NewMessage("Cool! A pumpkin keychain!",75);
			return 0;
			break;
		case ITM_KEYCH2:
			MakeNormalSound(SND_GETKEYCHAIN);
			player.keychain[player.worldNum][1]=1;
			KeyChainAllCheck();
			NewMessage("Cool! A hammer keychain!",75);
			return 0;
			break;
		case ITM_KEYCH3:
			MakeNormalSound(SND_GETKEYCHAIN);
			player.keychain[player.worldNum][2]=1;
			KeyChainAllCheck();
			NewMessage("Cool! A rocket keychain!",75);
			return 0;
			break;
		case ITM_KEYCH4:
			MakeNormalSound(SND_GETKEYCHAIN);
			player.keychain[player.worldNum][3]=1;
			KeyChainAllCheck();
			NewMessage("Cool! A squash keychain!",75);
			return 0;
			break;
		case ITM_LOONYKEY:
			MakeNormalSound(SND_LOONYKEY);
			if(player.worldNum<4)
			{
				player.lunacyKey[player.worldNum]=1;
				ShowVictoryAnim(player.worldNum);
			}
			SendMessageToGame(MSG_WINLEVEL,1);	// win upon getting the key
			return 0;
			break;
	}
	return 1;
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

byte PlayerHasLunacyKey(byte w)
{
	return player.lunacyKey[w];
}

void PlayerLoseKey(byte w)
{
	if(player.keys[w])
		player.keys[w]--;
}

byte PlayerKeyChain(byte w)
{
	return player.keychain[player.worldNum][w];
}

byte PlayerKeys(byte w)
{
	return player.keys[w];
}

void PlayerGetPoints(int amt)
{
	player.score+=amt;
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
	return player.musicSettings;
}

void PlayerSetMusicSettings(byte m)
{
	player.musicSettings=MUSIC_OFF;
}

void PlayerThrowHammer(Guy *me)
{
	HammerLaunch(me->x,me->y,me->facing,player.hammers,player.hammerFlags);
	player.reload=player.hamSpeed+2;
}

void PlayerHeal(byte amt)
{
	HealGoodguy(amt);

	if(player.life+amt<128)
		player.life+=amt;
	else
		player.life=128;
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
	static byte flameFlip=0;

	if(player.life==0)
		return;	// no shooting when you're dead

	switch(player.weapon)
	{
		case WPN_MISSILES:
			if(player.ammo)
			{
				FireBullet(me->x,me->y,me->facing,BLT_MISSILE);
				player.ammo--;
			}
			player.wpnReload=2;
			break;
		case WPN_BOMBS:
			if(player.ammo)
			{
				FireBullet(me->x,me->y,me->facing,BLT_BOMB);
				player.ammo--;
			}
			player.wpnReload=15;
			break;
		case WPN_AK8087:
			if(player.ammo)
			{
				FireBullet(me->x,me->y,me->facing,BLT_LASER);
				player.ammo--;
			}
			me->z+=FIXAMT*MGL_random(4);
			me->dx+=FIXAMT/2-MGL_random(65535);
			me->dy+=FIXAMT/2-MGL_random(65535);
			c=GetControls();
			if(c&CONTROL_B2)	// fire is held
			{
				player.wpnReload=1;
				me->frmTimer=0;
			}
			else
			{
				player.wpnReload=5;
				flameFlip=0;
			}
			DoPlayerFacing(c,me);
			break;
		case WPN_FLAME:
			if(player.ammo)
			{
				FireBullet(me->x,me->y,me->facing,BLT_FLAME);
				player.ammo--;
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
	}
	if(!player.ammo)
		player.weapon=0;
}

void PlayerControlMe(Guy *me,mapTile_t *mapTile,world_t *world)
{
	byte c;

	if(player.reload)
		player.reload--;
	if(player.wpnReload)
		player.wpnReload--;

	if(player.shield)
		player.shield--;

	if(player.pushPower)
		player.pushPower--;

	if(tportclock)
		tportclock--;

	if(player.poison)
	{
		// poison every few frames
		if(player.life>0 && ((player.poison&7)==0))
		{
			player.life--;
			me->hp--;
			if(player.life==0)
			{
				MakeSound(SND_BOUAPHADIE,me->x,me->y,SND_CUTOFF|SND_ONE,2000);
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
			MakeSound(SND_BOUAPHAOUCH,me->x,me->y,SND_CUTOFF|SND_ONE,2000);
		else if(me->seq==ANIM_DIE)	// so it doesn't do this if you're drowning
			MakeSound(SND_BOUAPHADIE,me->x,me->y,SND_CUTOFF|SND_ONE,2000);
	}

	if(me->parent)	// being grabbed by a Super Zombie
	{
		if(me->parent->type!=MONS_SUPERZOMBIE)
		{
			me->parent=NULL;
			return;
		}
		me->dz=0;
		if(me->parent->frm<4)
			me->z+=FIXAMT*3;
		else if(me->parent->frm>18)
		{
			me->z-=FIXAMT*4;
			if(me->parent->frm==21)
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
		}

		return;
	}

	// not busy, let's see if you want to do something
	c=GetControls();

	DoPlayerFacing(c,me);

	if((c&CONTROL_B1) && player.reload==0)	// pushed hammer throw button
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
	if((c&CONTROL_B2) && player.wpnReload==0 && player.weapon)	// pushed wpn use button
	{
		me->action=ACTION_BUSY;
		me->seq=ANIM_A1;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=200;
		player.boredom=0;
		return;
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

		Clamp(&me->dx,PLYR_MAXSPD);
		Clamp(&me->dy,PLYR_MAXSPD);

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
		if(player.boredom>200+MGL_random(3200))
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

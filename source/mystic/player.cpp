#include "player.h"
#include "spell.h"
#include "fairy.h"
#include "challenge.h"
#include "options.h"
#include "water.h"

#define PLYR_ACCEL	(FIXAMT)
#define PLYR_DECEL	(FIXAMT*3/4)

player_t player;
byte playerGlow;	// for torch-lit levels, and for exciting moments
byte tportclock;
int armageddon=0;
static word compassBigClock;
static byte vampyClock,compassClock;
byte beenReborn;
static byte fairyReload;
static int chlgCrystals;

void InitPlayer(byte initWhat,byte world,byte level)
{
	int i,j;

	beenReborn=0;
	vampyClock=0;
	fairyReload=0;
	compassClock=0;
	compassBigClock=0;
	if(initWhat==INIT_GAME)	// initialize everything, this is to start a whole new game
	{
		player.vaultOpened=0;
		player.curSpell=0;
		player.nightmare=0;
		player.powerStones=0;
		player.spellStones=0;
		player.shieldStones=0;
		for(i=0;i<20;i++)
			player.gotSpell[i]=0;

		player.score=0;
		player.musicSettings=2;
		player.level=1;
		player.experience=0;
		for(j=0;j<4;j++)
			player.keychain[j]=0;

		for(i=0;i<6;i++)
		{
			for(j=0;j<MAX_MAPS;j++)
				player.levelPassed[i][j]=0;
			player.totalCompletion[i]=100;
			player.complete[i]=0;
			player.lunacyKey[i]=0;
		}
		player.levelsPassed=0;
		for(i=0;i<10;i++)
			player.spell[i]=0;
		player.boots=0;
		player.hat=0;
		player.staff=0;
		player.money=0;
		player.maxSpeed=FIXAMT*4;
		player.defense=0;
		player.damage=2;
		player.maxLife=15;
		player.maxMana=15;
		player.gear=0;
		player.gameClock=0;
		player.overworldX=-2000;
		player.rechargeClock=4;
		player.haveFairy=0;
		player.fairyOn=0;
		player.templePuzzleDone=0;
		for(i=0;i<4;i++)
			player.chaseFairy[i]=0;
	}
	if(initWhat>=INIT_WORLD) // initialize the things that go with each world
	{
		player.levelsPassed=0;
		player.worldNum=world;
		for(i=0;i<MAX_MAPS;i++)
			if(player.levelPassed[world][i] && curWorld.map[i] && !(curWorld.map[i]->flags&MAP_SECRET))
				player.levelsPassed++;
	}

	player.levelNum=level;
	player.prevScore=player.score;	// back up the score (if you give up or die, it is reset)
	player.prevLevel=player.level;
	player.prevExp=player.experience;
	player.prevMoney=player.money;

	for(i=0;i<4;i++)
		player.keys[i]=0;

	player.brains=0;
	player.boredom=0;
	player.hammers=0;
	player.hamSpeed=HAMMER_MIN_SPEED;
	player.weapon=WPN_NONE;
	player.ammo=0;
	player.reload=5;
	player.wpnReload=0;
	player.hammerFlags=0;
	player.life=player.maxLife;
	player.mana=player.maxMana;
	player.shield=0;
	playerGlow=0;
	player.pushPower=0;
	player.poison=0;
	player.needExp=player.level*player.level*10+player.level*10;
	player.stoneskin=0;
	player.berserk=0;
	SetPlayerSpeed();
	SetPlayerDamage();
	SetPlayerDefense();

	if(PlayerHasSword())
		SetKidSprite(1);
	else
		SetKidSprite(0);

	armageddon=0;
}

void ExitPlayer(void)
{
}

void ResetPlayerLevels(void)
{
	int i,j;

	for(i=0;i<6;i++)
		for(j=0;j<MAX_MAPS;j++)
			player.levelPassed[i][j]=0;

	player.templePuzzleDone=0;
	player.lunacyKey[3]=0;
	player.vaultOpened=0;
}

byte PlayerHasSword(void)
{
	return(player.keychain[0] && player.keychain[1] && player.keychain[2] && player.keychain[3]);
}

void PlayerLoadGame(byte which)
{
	FILE *f;

	f=fopen("mystic.sav","rb");
	if(!f)
	{
		InitPlayer(INIT_GAME,0,0);
	}
	else
	{
		fseek(f,sizeof(player_t)*which,SEEK_SET);
		fread(&player,sizeof(player_t),1,f);
		fclose(f);
		VolumeSong(player.musicSettings);
	}
	player.prevMoney=player.money;
	player.prevExp=player.experience;
	player.prevLevel=player.level;
}

void PlayerSaveGame(byte which)
{
	FILE *f;
	player_t p[5];
	int i;

	player.prevMoney=player.money;
	f=fopen("mystic.sav","rb");
	if(!f)
	{
		memset(p,0,sizeof(player_t)*5);	// make an empty player
		for(i=0;i<6;i++)
		{
			p[0].totalCompletion[i]=100;
			p[1].totalCompletion[i]=100;
			p[2].totalCompletion[i]=100;
			p[3].totalCompletion[i]=100;
			p[4].totalCompletion[i]=100;
		}
		f=fopen("mystic.sav","wb");
		fwrite(p,sizeof(player_t),5,f);
		fclose(f);
		f=fopen("mystic.sav","rb");
	}
	fread(p,sizeof(player_t),5,f);
	fclose(f);
	memcpy(&p[which],&player,sizeof(player_t));
	f=fopen("mystic.sav","wb");
	fwrite(p,sizeof(player_t),5,f);
	fclose(f);
}

void SetPlayerSpeed(void)
{
	switch(player.boots)
	{
		case 0:
			player.maxSpeed=FIXAMT*4;
			break;
		case 1:
			player.maxSpeed=FIXAMT*5;
			break;
		case 2:
			player.maxSpeed=FIXAMT*6;
			break;
		case 3:
			player.maxSpeed=FIXAMT*7;
			break;
		case 4:
			player.maxSpeed=FIXAMT*8;
			break;
	}
}

void SetPlayerDamage(void)
{
	switch(player.staff)
	{
		case 0:
			player.damage=2;
			break;
		case 1:
			player.damage=3;
			break;
		case 2:
			player.damage=5;
			break;
		case 3:
			player.damage=7;
			break;
		case 4:
			player.damage=10;
			break;
	}
}

void SetPlayerDefense(void)
{
	switch(player.hat)
	{
		case 0:
			player.defense=0;
			break;
		case 1:
			player.defense=1;
			break;
		case 2:
			player.defense=3;
			break;
		case 3:
			player.defense=5;
			break;
		case 4:
			player.defense=8;
			break;
	}
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
	if(player.poison+amt>30*2)
		player.poison=30*2;
	else
		player.poison+=amt;
}

void PlayerResetScore(void)
{
	if(!Challenging())
	{
		player.score=player.prevScore;
		player.level=player.prevLevel;
		player.experience=player.prevExp;
		player.money=player.prevMoney;
	}
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

byte KeyChainAllCheck(void)
{
	if(player.keychain[0]==1 &&
		player.keychain[1]==1 &&
		player.keychain[2]==1 &&
		player.keychain[3]==1)
	{
		player.staff=4;	// max out the staff, so you can't buy new staves
		player.spell[9]=1;
		NewBigMessage("The sword is complete!",30);
		ShowVictoryAnim(10);
		SetKidSprite(1);
		return 1;
	}
	return 0;
}

byte PlayerHasSpell(void)
{
	int i;

	switch(player.worldNum*50+player.levelNum)
	{
		case 11:
		case 111:
			i=0;	// energy barrage
			break;
		case 2:
		case 156:
			i=1;	// dragon's flame
			break;
		case 3:
		case 157:
			i=2;	// Seeker bolt
			break;
		case 4:
		case 106:
			i=3;	// ice blast
			break;
		case 52:
		case 113:
			i=4;	// inferno
			break;
		case 56:
		case 159:
			i=5;	// summon
			break;
		case 61:
		case 160:
			i=6;	// stoneskin
			break;
		case 55:
		case 116:
			i=7;	// berserk
			break;
		case 53:
		case 109:
			i=8;	// healing
			break;
		case 10:
			i=9;	// armageddon
			break;
	}

	if(player.gotSpell[i+10*(player.worldNum>1)])
		return 1;
	/*
	if(player.worldNum<2 && player.spell[i]>=1)
		return 1;
	if(player.spell[i]>=2)
		return 1;
	*/
	return 0;
}

byte SpellBookForThisLevel(byte level,byte chapter)
{
	byte i;

	i=255;
	switch(player.worldNum*50+player.levelNum)
	{
		case 11:
		case 111:
			i=0;	// energy barrage
			break;
		case 2:
		case 156:
			i=1;	// dragon's flame
			break;
		case 3:
		case 157:
			i=2;	// Seeker bolt
			break;
		case 4:
		case 106:
			i=3;	// ice blast
			break;
		case 52:
		case 113:
			i=4;	// inferno
			break;
		case 56:
		case 159:
			i=5;	// summon
			break;
		case 61:
		case 160:
			i=6;	// stoneskin
			break;
		case 55:
		case 116:
			i=7;	// berserk
			break;
		case 53:
		case 109:
			i=8;	// healing
			break;
		case 1000:
			i=9;	// armageddon
			break;
	}
	return i;
}

byte FairyForThisLevel(word lvl)
{
	switch(lvl)
	{
		case 12:
			return 1;	// grabby is in the hut in beginnerton
		case 15:
			return 2;	// runny is in The Docks
		case 16:
			return 3;	// casty is in Plants Gone Wild
		case 17:
			return 4;	// blocky is in Trick Shot Challenge
		case 50+13:
			return 5;	// vampy is in hidden temple
		case 50+14:
			return 6;	// smashy is in lost in the woods
		case 50+15:
			return 7;	// cheapy is in log ride
		case 50+16:
			return 8;	// toughy is in temple of kings
		case 50*2+17:
			return 9;	// richey is in twisty passages
		case 50*2+18:
			return 10;	// smarty is in the peeping bomb level
		case 50*2+19:
			return 11;	// healey is in whack-a-zoid
		case 50*2+20:
			return 12;	// savey is in the sewers
		case 50*3+12:
			return 13;	// shooty is in red river rafting
		case 50*3+13:
			return 14; 	// haggly is in smashball
		case 50*3+14:
			return 15;	// scary is in puzzling caverns
		case 50*3+15:
			return 16;	// mighty is in gastro-pods

	}
	return 0;
}

void AddChaseFairy(void)
{
	int i;

	if(Challenging())	// in challenge mode, you get the fairy immediately, no chasing
	{
		player.haveFairy|=(1<<((FairyForThisLevel(player.worldNum*50+player.levelNum))-1));
		return;
	}
	for(i=0;i<4;i++)
	{
		if(player.chaseFairy[i]==FairyForThisLevel(player.worldNum*50+player.levelNum))
			player.chaseFairy[i]=0;
	}

	for(i=0;i<4;i++)
		if(player.chaseFairy[i]==0)
		{
			player.chaseFairy[i]=FairyForThisLevel(player.worldNum*50+player.levelNum);
			break;
		}
}

void CheckForAllSecrets(void)
{
	int i;

	for(i=0;i<9;i++)
		if(player.gotSpell[i]==0)
			return;
	for(i=10;i<19;i++)
		if(player.gotSpell[i]==0)
			return;
	if((player.haveFairy&65535)!=65535)
		return;
	for(i=0;i<4;i++)
		if(player.keychain[i]==0)
			return;

	// got em all!

	if(opt.challenge==0)
	{
		InitSpeech(24);
		opt.challenge=1;
	}
}

byte PlayerHasAllSecrets(byte chapter)
{
	byte spells[4][5]={{0,1,2,3,255},{4,5,6,7,8},{0,3,4,7,8},{1,2,5,6,255}};
	int i;

	for(i=0;i<5;i++)
	{
		if(spells[chapter][i]!=255 && !player.gotSpell[spells[chapter][i]+10*(chapter>1)])
			return 0;
	}

	for(i=0;i<4;i++)
	{
		if(!(player.haveFairy&(1<<(i+chapter*4))))
			return 0;
	}

	return (player.keychain[chapter]);	// it all hinges on this item now!
}

byte PlayerGetItem(byte itm,int x,int y)
{
	int cx,cy;
	int i;

	GetCamera(&cx,&cy);
	cx<<=FIXSHIFT;
	cy<<=FIXSHIFT;
	if(itm<ITM_LETTERM || itm>ITM_LETTERC)
		ChallengeEvent(CE_GET,itm);
	switch(itm)
	{
		case ITM_CHLGCRYSTAL:
			chlgCrystals--;
			if(chlgCrystals==0)
			{
				MakeNormalSound(SND_ALLCHLGCRYSTAL);
				playerGlow=60;
			}
			else
				MakeNormalSound(SND_CHLGCRYSTAL);
			FloaterParticles(x,y,1,32,-1,8);
			FloaterParticles(x,y,1,10,1,8);
			return 0;
			break;
		case ITM_HAMMERUP:
			if(player.hammers<5)
				player.hammers++;
			MakeNormalSound(SND_HAMMERUP);
			NewMessage("ENERGY ORB!",75);
			FloaterParticles(x,y,5,32,-1,16);
			FloaterParticles(x,y,5,10,1,16);
			return 0;
			break;
		case ITM_PANTS:
			if(player.hamSpeed>0)
				player.hamSpeed-=HAMMER_UP_AMOUNT;
			NewMessage("SPEED GEM!",75);
			MakeNormalSound(SND_PANTS);
			FloaterParticles(x,y,4,32,-1,16);
			FloaterParticles(x,y,4,10,1,16);
			return 0;
			break;
		case ITM_REVERSE:
			player.hammerFlags|=HMR_REVERSE;
			NewMessage("REVERSE GEM!",75);
			MakeNormalSound(SND_REVERSE);
			FloaterParticles(x,y,3,32,-1,16);
			FloaterParticles(x,y,3,10,1,16);
			return 0;
			break;
		case ITM_REFLECT:
			player.hammerFlags|=HMR_REFLECT;
			NewMessage("REFLECT GEM!",75);
			MakeNormalSound(SND_REFLECT);
			FloaterParticles(x,y,5,32,-1,16);
			FloaterParticles(x,y,5,10,1,16);
			return 0;
			break;
		case ITM_SPELLBOOK:
			MakeNormalSound(SND_WEAPON);
			i=SpellBookForThisLevel(player.levelNum,player.worldNum);
			if(i==255)
				return 0;	// no spellbook should BE in this level!
			if(player.spell[i]==1)
			{
				NewBigMessage(spellName[i*2+1],75);
				player.spell[i]=2;
				player.curSpell=i;
			}
			else if(player.spell[i]==0)
			{
				NewBigMessage(spellName[i*2],75);
				player.spell[i]=1;
				player.curSpell=i;
			}
			if(player.worldNum<2)
				player.gotSpell[i]=1;
			else
				player.gotSpell[i+10]=1;

			FloaterParticles(x,y,1,32,-1,16);
			FloaterParticles(x,y,5,10,2,16);
			FloaterParticles(x,y,5,64,-3,16);
			FloaterParticles(x,y,1,1,4,16);
			CheckForAllSecrets();
			return 0;
			break;
		case ITM_COIN:
			if(player.money<50000)
				player.money++;
			MakeSound(SND_MONEY,x,y,SND_CUTOFF,500);
			FloaterParticles(x,y,5,24,0,4);
			return 0;
			break;
		case ITM_BIGCOIN:
			if(player.money+10<50000)
				player.money+=10;
			else
				player.money=50000;
			MakeSound(SND_MONEY,x,y,SND_CUTOFF,500);
			MakeSound(SND_MONEY,x,y,SND_CUTOFF,500);
			FloaterParticles(x,y,5,24,0,4);
			FloaterParticles(x,y,5,32,0,4);
			return 0;
			break;
		case ITM_HEALTHPOT:
			if(player.life==player.maxLife)
				return 1;	// can't pick it up if fully healed
			MakeNormalSound(SND_LOONYKEY);
			NewMessage("Life Potion!",75);
			PlayerHeal(40);
			//ExplodeParticles2(PART_SLIME,x,y,MGL_randoml(FIXAMT*20),10,6);
			FloaterParticles(x,y,1,24,0,8);
			return 0;
			break;
		case ITM_MANAPOT:
			if(player.mana==player.maxMana)
				return 1;	// can't pick it up if fully magicked
			MakeNormalSound(SND_FOOD);
			NewMessage("Mana Restore Potion!",75);
			player.mana+=25;
			if(player.mana>player.maxMana)
				player.mana=player.maxMana;
			ManaParticles(x,y);
			return 0;
			break;
		case ITM_BRAIN:
			compassBigClock=0;
			MakeNormalSound(SND_GETBRAIN);
			player.brains++;
			if(player.brains==TotalBrains())
			{
				MakeNormalSound(SND_KOOLKAT);
				playerGlow=127;
			}
			player.score+=25;
			FloaterParticles(x,y,1,24,0,8);
			FloaterParticles(x,y,1,1,3,8);
			return 0;
			break;
		case ITM_KEY:
			if(player.keys[0]<3)
			{
				player.keys[0]++;
				MakeNormalSound(SND_GETKEY);
				return 0;
				FloaterParticles(x,y,5,24,0,8);
			}
			else
				return 1;	// don't pick it up
			break;
		case ITM_KEYR:
			MakeNormalSound(SND_GETKEY);
			player.keys[1]=1;
			FloaterParticles(x,y,4,24,0,8);
			return 0;
			break;
		case ITM_KEYG:
			MakeNormalSound(SND_GETKEY);
			player.keys[2]=1;
			FloaterParticles(x,y,1,24,0,8);
			return 0;
			break;
		case ITM_KEYB:
			MakeNormalSound(SND_GETKEY);
			player.keys[3]=1;
			FloaterParticles(x,y,3,24,0,8);
			return 0;
			break;
		case ITM_KEYCH1:
			MakeNormalSound(SND_GETKEYCHAIN);
			player.keychain[0]=1;
			if(!KeyChainAllCheck())
				NewMessage("A piece of the Armageddon Sword!",75);
			playerGlow=127;
			FloaterParticles(x,y,5,32,-1,16);
			FloaterParticles(x,y,5,10,2,16);
			FloaterParticles(x,y,5,64,-3,16);
			FloaterParticles(x,y,5,1,4,16);
			CheckForAllSecrets();
			return 0;
			break;
		case ITM_KEYCH2:
			MakeNormalSound(SND_GETKEYCHAIN);
			player.keychain[1]=1;
			if(!KeyChainAllCheck())
				NewMessage("A piece of the Armageddon Sword!",75);
			playerGlow=127;
			FloaterParticles(x,y,5,32,-1,16);
			FloaterParticles(x,y,5,10,2,16);
			FloaterParticles(x,y,5,64,-3,16);
			FloaterParticles(x,y,5,1,4,16);
			CheckForAllSecrets();
			return 0;
			break;
		case ITM_KEYCH3:
			MakeNormalSound(SND_GETKEYCHAIN);
			player.keychain[2]=1;
			if(!KeyChainAllCheck())
				NewMessage("A piece of the Armageddon Sword!",75);
			playerGlow=127;
			FloaterParticles(x,y,5,32,-1,16);
			FloaterParticles(x,y,5,10,2,16);
			FloaterParticles(x,y,5,64,-3,16);
			FloaterParticles(x,y,5,1,4,16);
			CheckForAllSecrets();
			return 0;
			break;
		case ITM_KEYCH4:
			MakeNormalSound(SND_GETKEYCHAIN);
			player.keychain[3]=1;
			if(!KeyChainAllCheck())
				NewMessage("A piece of the Armageddon Sword!",75);
			playerGlow=127;
			FloaterParticles(x,y,5,32,-1,16);
			FloaterParticles(x,y,5,10,2,16);
			FloaterParticles(x,y,5,64,-3,16);
			FloaterParticles(x,y,5,1,4,16);
			CheckForAllSecrets();
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
		case ITM_FAIRYBELL:
			MakeNormalSound(SND_FAIRYGET);
			AddChaseFairy();
			NewMessage("The bell awakens a fairy!",75);
			playerGlow=127;
			FloaterParticles(x,y,5,32,-1,16);
			FloaterParticles(x,y,5,10,2,16);
			FloaterParticles(x,y,5,64,-3,16);
			FloaterParticles(x,y,5,1,4,16);
			return 0;
			break;
		case ITM_LETTERM:
		case ITM_LETTERY:
		case ITM_LETTERS:
		case ITM_LETTERT:
		case ITM_LETTERI:
		case ITM_LETTERC:
			if(CurrentLetter()==(itm-ITM_LETTERM))
			{
				ChallengeEvent(CE_GET,ITM_LETTERM);
				MakeNormalSound(SND_PURCHASE);
				FloaterParticles(x,y,0,20,0,32);
				FloaterParticles(x,y,0,40,0,32);
				return 1;
			}
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
	return player.keychain[w];
}

byte PlayerKeys(byte w)
{
	return player.keys[w];
}

void PlayerGetPoints(int amt)
{
	int y;

	if(player.gear&GEAR_WISDOM)
		amt=amt*3/2;
	if(player.fairyOn==FAIRY_GRABBY)
		amt=amt*3/4;
	if(player.fairyOn==FAIRY_SMARTY)
		amt=amt*5/4;

	if(player.nightmare)
		amt=amt*5;	// 5x XP in nightmare mode

	ChallengeEvent(CE_POINTS,amt);
	player.score+=amt;
	if(player.level<50)// && player.levelPassed[player.worldNum][player.levelNum]==0)
	{
		player.experience+=amt;
		y=0;
		while(player.experience>=player.needExp)
		{
			AddParticle(GetGoodguy()->x,GetGoodguy()->y,FIXAMT*20+y*FIXAMT*10,0,0,0,60,PART_LVLUP,0);
			MakeNormalSound(SND_LEVELUP);
			y++;
			player.experience-=player.needExp;
			player.level++;
			player.maxLife=14+player.level;
			if(player.gear&GEAR_HEART)
				player.maxLife*=2;
			player.maxMana=14+player.level;
			if(player.gear&GEAR_MOON)
				player.maxMana*=2;
			player.needExp=player.level*player.level*10+player.level*10;
			if(player.level==50)
			{
				player.needExp=0;
				player.experience=0;
				break;
			}
		}
	}
	else
	{
		if(amt<5)
			amt=5;
		amt/=5;
		if(player.money+amt<=50000)
			player.money+=amt;	// get money!
		else
			player.money=50000;
	}
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
	return opt.musicVol;
}

void PlayerSetMusicSettings(byte m)
{
	player.musicSettings=MUSIC_OFF;
}

void PlayerThrowHammer(Guy *me)
{
	if(player.fairyOn==FAIRY_CHEAPY)
	{
		player.reload=player.hamSpeed+2;
		BlowSmoke(me->x+Cosine(me->facing*32)*32,me->y+Sine(me->facing*32)*32,FIXAMT*20,FIXAMT/2);
		return;	// no fireballs allowed
	}

	if(player.fairyOn==FAIRY_SMASHY)
	{
		if(player.mana>0)
			player.mana--;
	}
	ChallengeEvent(CE_SHOOT,player.hammers);
	if(PlayerHasSword())
		SkullLaunch(me->x,me->y,me->facing,player.hammers,player.hammerFlags);
	else
		HammerLaunch(me->x,me->y,me->facing,player.hammers,player.hammerFlags);
	player.reload=player.hamSpeed+2;
}

void PlayerHeal(byte amt)
{
	if(player.life==0)
		return;

	HealGoodguy(amt);

	if(player.life+amt<player.maxLife)
		player.life+=amt;
	else
		player.life=player.maxLife;
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

void ArmageddonUpdate(Map *map)
{
	int x,y;

	if(armageddon>0)
	{
		x=GetGoodguy()->x;
		y=GetGoodguy()->y;
		armageddon--;
		Armageddon(map,x,y);
		ShakeScreen(10);
	}
}

void PlayerControlMe(Guy *me,mapTile_t *mapTile,world_t *world)
{
	byte c;
	int i,j;
	Guy *g;

	if(compassBigClock<30*20)
		compassBigClock++;
	else
	{
		compassClock++;
		if(compassClock>30)
		{
			compassClock=0;
			if(player.gear&GEAR_COMPASS)
			{
				if(player.levelNum==1)	// overworld
				{
					g=FindNearestOfType(me->x/FIXAMT,me->y/FIXAMT,MONS_FAIRY);
					if(g)
					{
						brainX=g->mapx;
						brainY=g->mapy;
					}
					else
					{
						brainX=255;
					}
				}
				else
					CurrentMap()->FindNearBrain(me->mapx,me->mapy);
				if(brainX!=255)
					CompassTrail(me->mapx,me->mapy,brainX,brainY);
			}
		}
	}

	vampyClock++;
	if(vampyClock>30*3 && player.fairyOn==FAIRY_VAMPY && player.levelNum!=1)	// not on the hub level
	{
		vampyClock=0;
		if(player.life>1)
		{
			GetGoodguy()->hp--;
			player.life--;
		}
	}
	if(vampyClock>30*3 && player.fairyOn==FAIRY_RICHEY && player.levelNum!=1)	// not on the hub level
	{
		vampyClock=0;
		if(player.experience>0)
		{
			player.experience--;
		}
	}
	if(vampyClock>30*1+15 && player.fairyOn==FAIRY_MIGHTY && player.levelNum!=1)	// not on the hub level
	{
		vampyClock=0;
		if(player.life>1)
		{
			GetGoodguy()->hp--;
			player.life--;
		}
		if(player.mana>0)
			player.mana--;
	}

	if(vampyClock>30*3 && player.fairyOn==FAIRY_HEALEY && player.life<player.maxLife/2 && player.life>0 && player.levelNum!=1)	// not on the hub level
	{
		vampyClock=0;
		if(player.money>3)
		{
			player.money-=3;
			player.life+=3;
			GetGoodguy()->hp+=3;
			ExplodeParticles2(PART_SLIME,me->x,me->y,MGL_randoml(FIXAMT*20),6,6);
		}
	}
	if(player.levelNum==1)
	{
		player.overworldX=me->x;
		player.overworldY=me->y;
	}

	if(player.life>0)
	{
		if(player.rechargeClock>0)
			player.rechargeClock--;
		else
		{
			player.rechargeClock=60;
			if(player.gear&GEAR_SOUL)
				PlayerHeal(1);
			if(player.gear&GEAR_SUN)
			{
				if(player.mana<player.maxMana)
					player.mana++;
			}
		}
	}

	if(player.berserk)
	{
		byte frame = GetMonsterFrameNum(MONS_BOUAPHA,me->seq,me->frm,me->facing);
		if (frame != 254)
			TrailMe(me->x, me->y, frame);
		player.berserk--;
	}
	if(player.stoneskin)
		player.stoneskin--;

	if(fairyReload)
		fairyReload--;

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

	// triggering stuff
	if(me->action==ACTION_BUSY)
	{
		// throw hammer if need be, use item if need be
		if(me->seq==ANIM_ATTACK && me->frm==6 && player.reload==0 && player.hammers>0 && (player.gear&GEAR_SOCKS)==0)
			PlayerThrowHammer(me);
		if(me->seq==ANIM_ATTACK && me->frm==7 && (player.gear&GEAR_SOCKS)==0)
			me->reload-=2;
		if(me->seq==ANIM_A1 && me->frm==2 && player.wpnReload==0)
			CastSpell(me);
		if(me->seq==ANIM_A2 && me->frm==4 && player.wpnReload==0)
			CastSpell(me);

		if(me->seq==ANIM_A3)
		{
			WaterRipple(me->x/FIXAMT,me->y/FIXAMT,128);
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
		if(me->seq==ANIM_A4)
			me->facing=(me->facing+7)&7;

		return;
	}

	// not busy, let's see if you want to do something
	c=GetControls();

	DoPlayerFacing(c,me);

	if(player.levelNum==1)
	{
		// can't cast spells or even fake a fireball when on the overworld level
		player.reload=1;
		if(player.wpnReload==0)
			player.wpnReload=1;
	}

	// shooty provides backup shots
	if((c&CONTROL_B1) && player.hammers>0 && player.fairyOn==FAIRY_SHOOTY && fairyReload==0)
	{
		fairyReload=15;
		FairyDoThing(1);
	}

	if((c&CONTROL_B1) && player.reload==0)	// pushed hammer throw button
	{
		if(player.gear&GEAR_SOCKS)
		{
			if(!(c&(CONTROL_LF|CONTROL_RT|CONTROL_UP|CONTROL_DN)))
			{
				me->seq=ANIM_ATTACK;	// even if unarmed
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=255;
				me->frm+=4-(player.hamSpeed>>2);
			}
			if(player.hammers>0)
				PlayerThrowHammer(me);
			player.reload=player.hamSpeed+6;//-(4-(player.hamSpeed>>2));
		}
		else
		{
			me->action=ACTION_BUSY;
			me->seq=ANIM_ATTACK;	// even if unarmed
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=255;
			me->frm+=4-(player.hamSpeed>>2);
		}
		player.boredom=0;
		return;
	}
	if(player.levelNum==1 && player.wpnReload==1 && (c&CONTROL_B2) && player.haveFairy>0 && GetGameMode()==GAMEMODE_PLAY)
	{
		player.wpnReload=15;
		SendMessageToGame(MSG_GOTOMAP,51);
	}

	if((c&CONTROL_B2) && player.wpnReload==0 && player.spell[player.curSpell])	// pushed spell button
	{
		me->action=ACTION_BUSY;
		if(player.curSpell<4)
			me->seq=ANIM_A2;
		else
			me->seq=ANIM_A1;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=200;
		player.boredom=0;
		player.casting=player.curSpell;	// so if you switch the active spell, it still casts this one
		return;
	}

	// if you are moving indeed
	if(c&(CONTROL_UP|CONTROL_DN|CONTROL_LF|CONTROL_RT))
	{
		if(me->parent)	// drift the raft
		{
			if(c&CONTROL_LF)
				me->parent->dx-=FIXAMT/6;
			if(c&CONTROL_RT)
				me->parent->dx+=FIXAMT/6;
			Clamp(&me->parent->dx,FIXAMT*3);
		}

		if(player.berserk && player.spell[7]==2)	// insane rage leaves a flame trail
		{
			if((player.berserk&3)==0)
				FireExactBullet(me->x,me->y,0,0,0,0,0,20,(byte)MGL_random(256),BLT_LIQUIFY);
		}

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

		i=player.maxSpeed;
		if(player.fairyOn==FAIRY_RUNNY)
			i+=FIXAMT*2;
		if(player.fairyOn==FAIRY_MIGHTY)
			i=i*5/4;
		if(player.fairyOn==FAIRY_TOUGHY)
			i/=2;

		if(player.reload>1)
		{
			Clamp(&me->dx,i/2);
			Clamp(&me->dy,i/2);
		}
		else
		{
			Clamp(&me->dx,i);
			Clamp(&me->dy,i);
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

	c=GetTaps();
	if(c&CONTROL_B4)
	{
		j=0;
		for(i=0;i<10;i++)
			if(player.spell[i]==0)
				j++;

		if(j<10)
		{
			player.curSpell++;
			if(player.curSpell==10)
				player.curSpell=0;
			while(player.spell[player.curSpell]==0)
			{
				player.curSpell++;
				if(player.curSpell==10)
					player.curSpell=0;
			}
		}
	}
	if(c&CONTROL_B3)
	{
		j=0;
		for(i=0;i<10;i++)
			if(player.spell[i]==0)
				j++;

		if(j<10)
		{
			player.curSpell--;
			if(player.curSpell==255)
				player.curSpell=9;
			while(player.spell[player.curSpell]==0)
			{
				player.curSpell--;
				if(player.curSpell==255)
					player.curSpell=9;
			}
		}
	}

	/*
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
	*/
}

void BeginArmageddon(void)
{
	armageddon=30*10;
}

byte GetVampyClock(void)
{
	return vampyClock;
}

void SetChallengeCrystals(int amt)
{
	chlgCrystals=amt;
}

int GetChallengeCrystals(void)
{
	return chlgCrystals;
}


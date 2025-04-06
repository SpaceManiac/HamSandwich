#include "player.h"
#include <math.h>
#include "spell.h"
#include "fairy.h"
#include "challenge.h"
#include "options.h"
#include "water.h"
#include "appdata.h"
#include "skills.h"
#include "vfs.h"

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
float restorationBuffer,spellRestorationBuffer;
float spellRestorationOutput;	// separating the direct restoration from the Heal spell from others, because it can grant barrier
byte manaRuneValue;

void AddToRestorationBuffer(float amt)
{
	spellRestorationBuffer += amt;
}

void InitPlayer(byte initWhat,byte world,byte level)
{
	int i,j;

	beenReborn=0;
	vampyClock=0;
	fairyReload=0;
	compassClock=0;
	compassBigClock=0;
	manaRuneValue = 0;
	if(initWhat==INIT_GAME)	// initialize everything, this is to start a whole new game
	{
		Difficulty diff = player.difficulty;
		memset(&player, 0, sizeof(player_t));
		player.saveVersion = SAVE_VERSION;
		player.saveCode[0] = 'K';
		player.saveCode[1] = 'I';
		player.saveCode[2] = 'D';
		player.saveCode[3] = '\0';

		player.level=1;
		player.difficulty = diff;
		player.boots=0 + BrutalMode();
		player.hat = 0 + BrutalMode();
		player.staff = 0 + BrutalMode();
		if (BrutalMode() && !ClassicMode())
			player.gear |= GEAR_HEART;
		player.maxSpeed=FIXAMT*4;
		player.damage=2;
		PlayerUpdateLife();
		player.overworldX=-2000;
		player.rechargeClock=4;
		player.runePouches = 1;
	}
	if(initWhat>=INIT_WORLD) // initialize the things that go with each world
	{
		player.levelsPassed=0;
		player.worldNum=world;
		for(i=0;i<MAX_MAPS;i++)
			if(PlayerPassedLevel(world,i) && curWorld.map[i] && !(curWorld.map[i]->flags&MAP_SECRET))
				player.levelsPassed++;
	}

	ResetStoredHealMana();
	ResetStoredSeekerMana();
	player.ectoplasm = 0;
	player.castCount = 0;
	player.storedFlames = 0;
	player.flameCounter = 0;
	player.barrier = 0;
	restorationBuffer = 0;
	spellRestorationBuffer = 0;
	spellRestorationOutput = 0;
	player.levelNum=level;
	player.prevLevel=player.level;
	player.prevExp=player.experience;
	player.prevMoney=player.money;
	player.prevBigMoney = player.bigMoney;
	for(i=0;i<4;i++)
		player.keys[i]=0;

	player.usedFireballs = 0;
	for(i=0;i<10;i++)
		player.usedSpells[i] = 0;
	player.cumulativeBerserk = 0;
	player.totalKills = 0;
	player.timeSinceLastInferno = 255;
	player.seekerKills = 0;
	player.parry = 0;
	player.taunted = 0;
	player.summonDmgBoost = 0;
	player.brains=0;
	player.boredom=0;
	player.hammers=0;
	player.hamSpeed=HAMMER_MIN_SPEED;
	player.ammo=0;
	player.reload=5;
	player.summonKills = 0;
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
	for(int i=0;i<3;i++)
		player.puzzleVar[i] = 0;
	if (player.runePouches == 0)
		player.runePouches = 1;

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

	for (i = 0; i < 6; i++)
		for (j = 0; j < MAX_MAPS; j++)
			ResetLevelPassedFlags(i, j, false);

	player.templePuzzleDone=0;
	player.bobbyDoorOpen=0;
	player.vaultOpened=0;
}

byte PlayerHasSword(void)
{
	return(player.swordPiece[0] && player.swordPiece[1] && player.swordPiece[2] && player.swordPiece[3]);
}

void PlayerLoadGame(byte which)
{
	char s[32];
	sprintf(s, "mystic%d.sav", which+1);

	auto f = AppdataOpen(s);
	if(!f)
	{
		InitPlayer(INIT_GAME,0,0);
	}
	else
	{
		SDL_RWread(f,&player,sizeof(player_t),1);
		f.reset();
	}
	player.prevMoney=player.money;
	player.prevExp=player.experience;
	player.prevLevel=player.level;
	player.prevBigMoney = player.bigMoney;
}

void PlayerSaveGame(byte which)
{
	char s[32];

	player.prevMoney=player.money;
	player.prevBigMoney = player.bigMoney;
	sprintf(s, "mystic%d.sav", which+1);
	auto f=AppdataOpen_Write(s);
	player.saveVersion = SAVE_VERSION;
	player.saveCode[0] = 'K';
	player.saveCode[1] = 'I';
	player.saveCode[2] = 'D';
	player.saveCode[3] = '\0';
	SDL_RWwrite(f,&player,sizeof(player_t),1);
	f.reset();
	AppdataSync();
}

void AutoSave()
{
	// Overwrite BK5 with BK4, BK4 with BK3, BK3 with BK2, BK2 with BK1.
	Vfs()->rename("mystic249.sav", "mystic250.sav");
	Vfs()->rename("mystic248.sav", "mystic249.sav");
	Vfs()->rename("mystic247.sav", "mystic248.sav");
	Vfs()->rename("mystic246.sav", "mystic247.sav");
	// Save to BK1.
	PlayerSaveGame(245); // mystic246.sav
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

void PlayerRenderInterface(MGLDraw *mgl)
{
	int b;

	b=TotalBrains();
	if(b!=0)
		b=128-(player.brains*128/b);

	RenderInterface(player.life,player.hammerFlags,player.hammers,b,player.score,
					player.weapon,player.ammo,player.hamSpeed,mgl);
}

int CalcGamePercent(const player_t* p)
{
	int runes=0, spells=0, swords=0, fairies=0;
	int skillPts = 0, runePouches = 0;
	for (int i = 0; i < (int)Rune::NUM_RUNES; i++)
		if (p->runes[i] > RUNE_EMPTY)
			runes++;
	for (int i = 0; i < 10; i++)
		spells += p->spell[i];
	for (int i = 0; i < 16; i++)
	{
		if (p->haveFairy & (1 << i))
		{
			fairies++;
			if (!(p->chaseFairy & (1 << i)))
				fairies++;
		}
	}
	skillPts = player.skillPts;
	for (int i = 0; i < MAX_SKILLS; i++)
		skillPts += player.skill[i];
	if (skillPts > 65) skillPts = 65;	// the extra skill points in madcap are above and beyond

	swords = p->swordPiece[0] + p->swordPiece[1] + p->swordPiece[2] + p->swordPiece[3];
	runePouches = player.runePouches;
	int totalNeeded = 24 + // runes
		19 + // spells
		4 + // swords
		32 + // fairies (2x, one for the bell, one for the fairy)
		5 +	// rune pouches
		65;	// skill points
	if (p->difficulty == Difficulty::CLASSIC || p->difficulty == Difficulty::BRUTAL_CLASSIC || p->difficulty==Difficulty::VERY_CLASSIC)
	{	// classic has no runes
		totalNeeded -= 24+65+5;
		skillPts = 0;
		runePouches = 0;
		runes = 0;
		if (p->difficulty == Difficulty::VERY_CLASSIC)
		{
			fairies = 0;
			totalNeeded -= 32;
		}
	}

	return (runes + spells + swords + fairies+skillPts+runePouches)*100 / totalNeeded;
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
	if (player.life > 0 && player.shield == 0)
	{
		if (player.poison + amt > 30 * 2)
			player.poison = 30 * 2;
		else
			player.poison += amt;
	}
}

void PlayerResetScore(void)
{
	if(!Challenging() && ClassicMode())
	{
		player.level=player.prevLevel;
		player.experience=player.prevExp;
		player.money=player.prevMoney;
		player.bigMoney = player.prevBigMoney;
	}
}

void PlayerWinLevel(byte w,byte l,byte isSecret)
{
	if(!PlayerPassedLevel(w, l))
	{
		if(!isSecret)
			player.levelsPassed++;	// secret levels aren't counted in this (it's for triggering specials)
		(*GetLevelPassedFlag(player.worldNum, player.levelNum)) |= LP_PASSED;
	}
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

byte SwordAllCheck(void)
{
	if(PlayerHasSword())
	{
		player.staff=4;	// max out the staff, so you can't buy new staves
		player.spell[9]=1;
		NewBigMessage("The sword is complete!",30);
		SendMessageToGame(MSG_SHOWANIM, 10);
		SetKidSprite(1);
		return 1;
	}
	return 0;
}

bool PlayerHasTier2Spell(void)
{
	for (int i = 0; i < 10; i++)
		if (player.spell[i] == 2)
			return true;
	return false;
}

byte PlayerHasSpell(void)
{
	int i;

	switch(player.worldNum*50+player.levelNum)
	{
		case 11:
		case 111:
		case 21:	// this spell is in 2 different chapter 1 levels, because I remade beginnerton
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
	switch(chapter*50+level)
	{
		case 11:
		case 111:
		case 21:
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
		case 21:
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
		player.haveFairy|=(1<<(FairyForThisLevel(player.worldNum*50+player.levelNum)-1));
		return;
	}
	player.chaseFairy |= (1<<(FairyForThisLevel(player.worldNum * 50 + player.levelNum)-1));
}

void CheckForAllSecrets(void)
{
	int i;

	byte runes = 0;
	for(int j=0;j<4;j++)
		for (i = 0; i < MAX_MAPS; i++)
			if (GotRuneInLevel(j,i))
				runes++;
	if (runes < 24)
		return;	// gotta get all the runes
	for(i=0;i<9;i++)
		if(player.gotSpell[i]==0)
			return;
	for(i=10;i<19;i++)
		if(player.gotSpell[i]==0)
			return;
	if((player.haveFairy&65535)!=65535)
		return;
	// if you got here, then you have every fairy
	EarnAchieve(Achievement::ALL_FAIRIES);

	if (!PlayerHasSword())
		return;
	// if you got here, you have every piece of the sword
	EarnAchieve(Achievement::GETSWORD);

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

	byte runes = 0;
	for (i = 0; i < MAX_MAPS; i++)
		if (GotRuneInLevel(chapter,i))
			runes++;
	if (runes < 6) return 0;

	return (player.swordPiece[chapter]);	// it all hinges on this item now!
}

byte PlayerGetItem(byte itm,int x,int y)
{
	int i;

	if(itm<ITM_LETTERM || itm>ITM_LETTERC)
		ChallengeEvent(CE_GET,itm);
	switch(itm)
	{
		case ITM_GRIMBLEWEED:
			if (player.puzzleVar[0] < 99)
				player.puzzleVar[0]++;
			FloaterParticles(x, y, 1, 10, 1, 8);
			MakeNormalSound(SND_BUSHOUCH);
			return 0;
			break;
		case ITM_TOADSTOOL:
			if (player.puzzleVar[2] < 99)
				player.puzzleVar[2]++;
			FloaterParticles(x, y, 1, 10, 1, 8);
			MakeNormalSound(SND_BUSHOUCH);
			return 0;
			break;
		case ITM_RUNEPOUCH:
			GetRunePouchInLevel();
			MakeNormalSound(SND_LOONYKEY);
			if (!ClassicMode() && player.runePouches<5)
				player.runePouches++;
			else
				GainMoney(100);
			NewMessage("RUNE POUCH GET!", 75);
			FloaterParticles(x, y, 1, 10, 1, 8);
			return 0;
			break;
		case ITM_SKILLSHARD:
			GetSkillShardInLevel();
			if (!ClassicMode())
			{
				if (Challenging())
					player.skillPts += 2;
				else
					player.skillPts++;
			}
			else
				GainMoney(100);
			MakeNormalSound(SND_INVINCCHEAT);
			NewMessage("A SKILL SHARD!", 75);
			FloaterParticles(x, y, 1, 10, 1, 8);
			return 0;
			break;
		case ITM_SILENTRUNE:
			PickUpRune();
			return 0;
			break;
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
			if (player.hammers < 5)
			{
				player.hammers++;
				MakeNormalSound(SND_HAMMERUP);
				NewMessage("ENERGY ORB!", 75);
				FloaterParticles(x, y, 5, 32, -1, 16);
				FloaterParticles(x, y, 5, 10, 1, 16);
				return 0;
			}
			else
				return 1;	// can't pick it up if already at max
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
			GetSpellInLevel();

			FloaterParticles(x,y,1,32,-1,16);
			FloaterParticles(x,y,5,10,2,16);
			FloaterParticles(x,y,5,64,-3,16);
			FloaterParticles(x,y,1,1,4,16);
			CheckForAllSecrets();
			return 0;
			break;
		case ITM_COIN:
			GainMoney(1);
			MakeSound(SND_MONEY,x,y,SND_CUTOFF,500);
			FloaterParticles(x,y,5,24,0,4);
			return 0;
			break;
		case ITM_BIGCOIN:
			GainMoney(10);
			MakeSound(SND_MONEY,x,y,SND_CUTOFF,500);
			FloaterParticles(x,y,5,24,0,4);
			FloaterParticles(x,y,5,32,0,4);
			return 0;
			break;
		case ITM_DIAMOND:
			GainMoney(100);
			MakeSound(SND_MONEY, x, y, SND_CUTOFF, 500);
			FloaterParticles(x, y, 5, 24, 0, 4);
			FloaterParticles(x, y, 5, 32, 0, 4);
			return 0;
			break;
		case ITM_MONEYBAG:
			GainMoney(25);
			MakeSound(SND_MONEY, x, y, SND_CUTOFF, 500);
			FloaterParticles(x, y, 5, 24, 0, 4);
			FloaterParticles(x, y, 5, 32, 0, 4);
			return 0;
			break;
		case ITM_HEALTHPOT:
			if (player.life == player.maxLife)
			{
				if(ClassicMode() || SkillValue(SKILL_POTIONS)==0 || player.mana==player.maxMana)
					return 1;	// can't pick it up if fully healed. If you have potion skill, you can pick it up as long as your mana isn't full
			}
			MakeNormalSound(SND_LOONYKEY);
			NewMessage("Life Potion!",75);
			PlayerHeal(40);
			if (!ClassicMode())
			{
				player.mana += SkillValue(SKILL_POTIONS);
				if (player.mana > player.maxMana)
					player.mana = player.maxMana;
			}
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
			if (!ClassicMode())
				player.mana += SkillValue(SKILL_POTIONS);
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
		case ITM_SWORD1:
			MakeNormalSound(SND_GETKEYCHAIN);
			player.swordPiece[0]=1;
			GetSwordInLevel();
			if(!PlayerHasSword())
				NewMessage("A piece of the Armageddon Sword!",75);
			playerGlow=127;
			FloaterParticles(x,y,5,32,-1,16);
			FloaterParticles(x,y,5,10,2,16);
			FloaterParticles(x,y,5,64,-3,16);
			FloaterParticles(x,y,5,1,4,16);
			CheckForAllSecrets();
			SwordAllCheck();
			return 0;
			break;
		case ITM_SWORD2:
			MakeNormalSound(SND_GETKEYCHAIN);
			player.swordPiece[1]=1;
			GetSwordInLevel();
			if(!PlayerHasSword())
				NewMessage("A piece of the Armageddon Sword!",75);
			playerGlow=127;
			FloaterParticles(x,y,5,32,-1,16);
			FloaterParticles(x,y,5,10,2,16);
			FloaterParticles(x,y,5,64,-3,16);
			FloaterParticles(x,y,5,1,4,16);
			CheckForAllSecrets();
			SwordAllCheck();
			return 0;
			break;
		case ITM_SWORD3:
			MakeNormalSound(SND_GETKEYCHAIN);
			player.swordPiece[2]=1;
			GetSwordInLevel();
			if(!PlayerHasSword())
				NewMessage("A piece of the Armageddon Sword!",75);
			playerGlow=127;
			FloaterParticles(x,y,5,32,-1,16);
			FloaterParticles(x,y,5,10,2,16);
			FloaterParticles(x,y,5,64,-3,16);
			FloaterParticles(x,y,5,1,4,16);
			CheckForAllSecrets();
			SwordAllCheck();
			return 0;
			break;
		case ITM_SWORD4:
			MakeNormalSound(SND_GETKEYCHAIN);
			player.swordPiece[3]=1;
			GetSwordInLevel();
			if(!PlayerHasSword())
				NewMessage("A piece of the Armageddon Sword!",75);
			playerGlow=127;
			FloaterParticles(x,y,5,32,-1,16);
			FloaterParticles(x,y,5,10,2,16);
			FloaterParticles(x,y,5,64,-3,16);
			FloaterParticles(x,y,5,1,4,16);
			CheckForAllSecrets();
			SwordAllCheck();
			return 0;
			break;
		case ITM_FAIRYBELL:
			GetFairyBellInLevel();
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

void PlayerLoseKey(byte w)
{
	if(player.keys[w])
		player.keys[w]--;
}

byte PlayerHasSwordPiece(byte w)
{
	return player.swordPiece[w];
}

byte PlayerKeys(byte w)
{
	return player.keys[w];
}

void PlayerUpdateLife(void)
{
	byte formerMaxLife = player.maxLife;
	byte formerMaxMana = player.maxMana;
	// classic, gain life and mana with levels
	if (player.difficulty == Difficulty::CLASSIC || player.difficulty==Difficulty::BRUTAL_CLASSIC || player.difficulty==Difficulty::VERY_CLASSIC)
	{
		player.maxLife = 14 + player.level;
		player.maxMana = 14 + player.level;
	}
	// modern, gain life and mana from skills
	else if(player.difficulty==Difficulty::MODERN || player.difficulty==Difficulty::BRUTAL_MODERN)
	{
		player.maxLife = 14+player.skill[SKILL_LIFE]*10;
		player.maxMana = 14+player.skill[SKILL_MANA]*10;
	}
	if (player.gear & GEAR_HEART)
		player.maxLife *= 2;
	if (player.gear & GEAR_MOON)
		player.maxMana *= 2;
	int lifeDiff = player.maxLife - formerMaxLife;
	int manaDiff = player.maxMana - formerMaxMana;
	if (GetGoodguy())
	{
		if (lifeDiff > 0)
			GetGoodguy()->hp += lifeDiff;
		if (GetGoodguy()->hp > player.maxLife)
			GetGoodguy()->hp = player.maxLife;
		SetPlayerHP(GetGoodguy()->hp);
	}
	if (manaDiff > 0)
		player.mana += manaDiff;
	if (player.mana > player.maxMana)
		player.mana = player.maxMana;
}

void PlayerLevelUp(int y)
{
	if (player.level == MAX_PLAYERLEVEL)
		return;

	AddParticle(GetGoodguy()->x, GetGoodguy()->y, FIXAMT * 20 + y * FIXAMT * 10, 0, 0, 0, 60, PART_LVLUP, 0);
	MakeNormalSound(SND_LEVELUP);
	player.experience -= player.needExp;
	player.level++;
	if (!ClassicMode())
		player.skillPts++;
	PlayerUpdateLife();
	player.needExp = player.level * player.level * 10 + player.level * 10;
	if (player.level == MAX_PLAYERLEVEL)
	{
		EarnAchieve(Achievement::LEVEL50);
		player.needExp = 0;
		player.experience = 0;
	}
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

	if (player.nightmare)
		amt = amt * 5;	// 5x XP in nightmare mode
	else if (BrutalMode())
		amt = amt * 2;	// 2x XP in brutal mode

	ChallengeEvent(CE_POINTS,amt);
	player.score+=amt;
	if(player.level<50)
	{
		player.experience+=amt;
		y=0;
		while(player.experience>=player.needExp)
		{
			PlayerLevelUp(y);
			y++;
			if (player.level == MAX_PLAYERLEVEL)
				break;
		}
	}
	else if(!VeryClassicMode())
	{
		if(amt<5)
			amt=5;
		amt/=5;
		GainMoney(amt);
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

void PlayerThrowHammer(Guy *me)
{
	if(player.fairyOn==FAIRY_CHEAPY)
	{
		player.reload=player.hamSpeed+2;
		BlowSmoke(me->x+Cosine(me->facing*32)*32,me->y+Sine(me->facing*32)*32,FIXAMT*20,FIXAMT/2);
		return;	// no fireballs allowed
	}

	if (player.worldNum == 3 && player.levelNum == 19 && player.puzzleVar[0] == HORK_NOFIREBALLS)
	{
		player.reload = player.hamSpeed + 2;
		BlowSmoke(me->x + Cosine(me->facing * 32) * 32, me->y + Sine(me->facing * 32) * 32, FIXAMT * 20, FIXAMT / 2);
		return;	// no fireballs allowed
	}

	if(player.fairyOn==FAIRY_SMASHY)
	{
		if(player.mana>0)
			player.mana--;
	}

	ChallengeEvent(CE_SHOOT,player.hammers);
	HammerLaunch(me->x,me->y,me->facing,player.hammers,player.hammerFlags,(PlayerHasSword() && !player.disableSword));
	if (!ClassicMode() && RuneValue(Rune::SHOTGUN) > 0)
		player.reload = player.hamSpeed * 2 + 4;
	else
		player.reload = player.hamSpeed + 2;
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

void PlayerHealWithSpell(byte amt)
{
	if (player.life == 0)
		return;

	HealGoodguy(amt);

	if (player.life + amt < player.maxLife)
		player.life += amt;
	else
	{
		amt -= (player.maxLife - player.life);
		player.life = player.maxLife;
		if (!ClassicMode())
		{
			byte limit = (byte)RuneValue(Rune::HEAL);
			if (limit > 0)
			{
				player.barrier += amt;
				if (player.barrier > limit)
					player.barrier = limit;
			}
		}
	}
}

byte GetTportClock(void)
{
	return tportclock;
}

void SetTportClock(byte tp)
{
	tportclock=tp;
}

void DoPlayerFacing(Guy *me)
{
	if(ButtonHeld(CONTROL_UP))
	{
		me->facing=6;
		if(ButtonHeld(CONTROL_LF))
			me->facing=5;
		else if(ButtonHeld(CONTROL_RT))
			me->facing=7;
	}
	else if(ButtonHeld(CONTROL_DN))
	{
		me->facing=2;
		if(ButtonHeld(CONTROL_LF))
			me->facing=3;
		else if(ButtonHeld(CONTROL_RT))
			me->facing=1;
	}
	else
	{
		if(ButtonHeld(CONTROL_LF))
			me->facing=4;
		else if(ButtonHeld(CONTROL_RT))
			me->facing=0;
	}
}

void ArmageddonUpdate(Map *map)
{
	int x,y;

	if(armageddon>0)
	{
		if (!ClassicMode() && SkillValue(SKILL_MANAGETTIN) > 0)
			player.mana = player.maxMana;

		x=GetGoodguy()->x;
		y=GetGoodguy()->y;
		armageddon--;
		Armageddon(map,x,y);
		ShakeScreen(10);
		if (!ClassicMode() && armageddon == 0 && SkillValue(SKILL_MANAGETTIN) > 0)
			player.mana = 0;
	}
}

void BeginCast(Guy *me,byte spell)
{
	if (me->action!=ACTION_BUSY && player.levelNum!=1 && player.wpnReload == 0 && player.spell[player.curSpell])	// pushed spell button
	{
		me->action = ACTION_BUSY;
		if (player.curSpell < 4)
			me->seq = ANIM_A2;
		else
			me->seq = ANIM_A1;
		me->frm = 0;
		me->frmTimer = 0;
		me->frmAdvance = 200;
		player.boredom = 0;
		player.casting = spell;	// so if you switch the active spell, it still casts this one
		ResetCastCounter();
	}
}

void PlayerControlMe(Guy *me,mapTile_t *mapTile,world_t *world)
{
	int i,j;
	Guy *g;
	byte wannaCast = 255;

	for (int i = 0; i < 10; i++)
	{
		if (ButtonHeld(1 << (CTL_ID_QC_1 + i)))	// have a spell key held down
		{
			if (player.spell[i])
			{
				if (opt.quickCast == QUICKCAST_SELECTONLY || opt.quickCast == QUICKCAST_CASTANDSELECT)
					player.curSpell = i;
				if (opt.quickCast == QUICKCAST_CASTANDSELECT || opt.quickCast == QUICKCAST_CASTONLY)
					wannaCast = (byte)i;
			}
		}
	}

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

	if (!ClassicMode() && player.storedFlames > 0)
	{
		player.storedFlames--;
		FireBullet(me->x, me->y, me->facing, BLT_FLAME);
		if (SkillValue(SKILL_FLAMEON) > 0 && Random(10) < SkillValue(SKILL_FLAMEON))	// up to 50% chance of a double flame as skill goes up
			FireBullet(me->x, me->y, me->facing, BLT_FLAME);

		player.flameCounter++;
		if (RuneValue(Rune::FLAME3) > 0 && player.flameCounter >= (int)RuneValue(Rune::FLAME3))
		{
			player.flameCounter = 0;
			FireBullet(me->x, me->y, (me->facing + 2) & 7, BLT_FLAME);
			FireBullet(me->x, me->y, (me->facing + 6) & 7, BLT_FLAME);
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
		if(CanAffordMoney(3))
		{
			GainMoney(-3);
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
			if (!ClassicMode() && RuneValue(Rune::MANA))
			{
				manaRuneValue++;
				if (manaRuneValue >= (int)RuneValue(Rune::MANA))
				{
					manaRuneValue -= (int)RuneValue(Rune::MANA);
					if (player.mana < player.maxMana)
						player.mana++;
				}
			}
		}
	}
	if (!ClassicMode())
	{
		if (player.ectoClock > 0)
			player.ectoClock--;
		else
		{
			player.ectoClock = 20;
			if (player.ectoplasm > 0)
				player.ectoplasm--;
		}
		if(SkillValue(SKILL_RESTORATION))
			restorationBuffer += 1.0f / 90.0f;	// 1 life every 3 seconds
		if (RuneValue(Rune::RECOVER))
			restorationBuffer += 1.0f / (RuneValue(Rune::RECOVER) * 30);	// 1 life every N seconds from recover

		if (restorationBuffer >= 1)
		{
			byte amt = (byte)floor(restorationBuffer);
			PlayerHeal((byte)amt);
			restorationBuffer-= (float)amt;
		}

		float maxPerTick = SkillValue(SKILL_RESTORATION) / 30.0f;	// at max, you can heal the heal/s from your healing spell

		float amt;
		if (spellRestorationBuffer > maxPerTick)
			amt = maxPerTick;
		else
			amt = spellRestorationBuffer;
		spellRestorationBuffer -= amt;
		spellRestorationOutput += amt;
		if (spellRestorationOutput >= 1)
		{
			byte amt = (byte)floor(spellRestorationOutput);
			PlayerHealWithSpell((byte)amt);
			spellRestorationOutput -= (float)amt;
		}
	}
	if (player.summonDmgBoost)
		player.summonDmgBoost--;
	if (player.taunted)
		player.taunted--;
	if (player.parry > 0)
		player.parry--;

	if (player.berserk)
	{
		byte frame = GetMonsterFrameNum(MONS_BOUAPHA, me->seq, me->frm, me->facing);
		if (frame != 254)
			TrailMe(me->x, me->y, me->z, frame);
		player.berserk--;
		player.cumulativeBerserk++;
		if (player.cumulativeBerserk >= 60 * 30 * 2)	// 1 minute = 2 minutes, because berserk double-updates you
			EarnAchieve(Achievement::BERSERK);
	}
	else
		player.cumulativeBerserk = 0;

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
	if (me->ouch2 == 4)
	{
		MakeSound(SND_DEFLECT, me->x, me->y, SND_CUTOFF | SND_ONE, 2000);
	}

	// triggering stuff
	if(me->action==ACTION_BUSY)
	{
		// throw hammer if need be, use item if need be
		if(me->seq==ANIM_ATTACK && me->frm==6 && player.reload==0 && player.hammers>0 && ((player.gear&GEAR_SOCKS)==0 || player.disableMoveNShoot))
			PlayerThrowHammer(me);
		if(me->seq==ANIM_ATTACK && me->frm==7 && (player.gear&GEAR_SOCKS)==0)
			me->reload-=2;
		if(me->seq==ANIM_A1 && me->frm==2 && player.wpnReload==0 && player.casting!=255)
			CastSpell(me);
		if(me->seq==ANIM_A2 && me->frm==4 && player.wpnReload==0 && player.casting!=255)
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
	DoPlayerFacing(me);

	if(player.levelNum==1)
	{
		// can't cast spells or even fake a fireball when on the overworld level
		player.reload=1;
		if(player.wpnReload==0)
			player.wpnReload=1;
	}

	// shooty provides backup shots
	if(ButtonHeld(CONTROL_B1) && player.hammers>0 && player.fairyOn == FAIRY_SHOOTY && fairyReload == 0)
	{
		fairyReload=15;
		FairyDoThing(1);
	}

	if(ButtonHeld(CONTROL_B1) && player.reload==0)	// pushed hammer throw button
	{
		if((player.gear&GEAR_SOCKS) && !player.disableMoveNShoot)
		{
			if(!ButtonHeld(CONTROL_LF|CONTROL_RT|CONTROL_UP|CONTROL_DN))
			{
				me->seq=ANIM_ATTACK;	// even if unarmed
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=255;
				me->frm+=4-(player.hamSpeed>>2);
			}
			if(player.hammers>0)
				PlayerThrowHammer(me);
			player.reload=player.hamSpeed+6;
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
	if(player.levelNum==1 && player.wpnReload==1 && ButtonTapped(CONTROL_B2) && player.haveFairy>0 && GetGameMode() == GAMEMODE_PLAY)
	{
		player.wpnReload=15;
		SendMessageToGame(MSG_GOTOMAP,51);
	}

	if (ButtonHeld(CONTROL_B2))
		wannaCast = player.curSpell;

	if(wannaCast!=255 && player.wpnReload == 0 && player.spell[wannaCast])	// pushed spell button
	{
		BeginCast(me,wannaCast);
		return;
	}

	// if you are moving indeed
	if(ButtonHeld(CONTROL_UP|CONTROL_DN|CONTROL_LF|CONTROL_RT))
	{
		if(me->parent)	// drift the raft
		{
			if(ButtonHeld(CONTROL_LF))
				me->parent->dx-=FIXAMT/6;
			if(ButtonHeld(CONTROL_RT))
				me->parent->dx+=FIXAMT/6;
			Clamp(&me->parent->dx,FIXAMT*3);
		}

		if(player.berserk && player.spell[SPL_BERSERK]==2 && !player.downgradeSpell[SPL_BERSERK])	// insane rage leaves a flame trail
		{
			if ((player.berserk & 3) == 0)
			{
				byte dur = 20;
				if (!ClassicMode() && RuneValue(Rune::BERSERK2) > 0)
					dur = (byte)(20.0f * RuneValue(Rune::BERSERK2) / 100.0f);
				FireExactBullet(me->x, me->y, 0, 0, 0, 0, 0, dur, (byte)MGL_random(256), BLT_LIQUIFY3);
			}
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

	if (player.levelNum == 19 && player.worldNum == 1 && me->action != ACTION_BUSY &&
		((ButtonTapped(CONTROL_B3) && ButtonTapped(CONTROL_B4)) ||
			(ButtonTapped(CONTROL_B4) && ButtonHeld(CONTROL_B3)))) // you hit both, so drop a trap
	{
		if (player.hammers == 0)
		{
			MakeNormalSound(SND_FAILSPELL);
			NewMessage("I'm out of Energy Orbs!", 30);
		}
		else
		{
			FireBullet(me->x, me->y, 0, BLT_GHOSTTRAP);
			player.hammers--;
			MakeNormalSound(SND_INCAGEN);
		}
	}

	// super cool execute move
	if (!ClassicMode() && SkillValue(SKILL_MURDALIZE)>0 && me->action!=ACTION_BUSY &&
		((ButtonTapped(CONTROL_B3) && ButtonTapped(CONTROL_B4)) ||
		(ButtonTapped(CONTROL_B4) && ButtonHeld(CONTROL_B3)))) // you hit both
	{
		int x, y;
		x = me->x + Cosine(me->facing * 32) * 30;
		y = me->y + Sine(me->facing * 32) * 30;
		word id = FindMurdalizeGuy(x>>FIXSHIFT, y>>FIXSHIFT,MURDALIZE_RANGE);
		if (id != 65535) // there is somebody in the target spot
		{
			byte frame = GetMonsterFrameNum(MONS_BOUAPHA, me->seq, me->frm, me->facing);
			if (frame != 254)
				TrailMe(me->x, me->y, me->z, frame);
			me->action = ACTION_BUSY;
			me->seq = ANIM_A1;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 200;
			float a = atan2f(GetGuy(id)->y - me->y, GetGuy(id)->x - me->x) * 128.0f / 3.14159f;
			if (a < 0)
				a += 256;
			me->facing = (a + 16) / 32;
			if (me->facing > 7) me->facing -= 8;
			me->dx = Cosine((int)a) * 10;
			me->dy = Sine((int)a) * 10;
			me->z = FIXAMT * HALFHEI;	// drop from the sky with the meteor!
			me->dz = -FIXAMT * 20;
			player.berserk = 20;
			player.casting = 255;
			player.boredom = 0;
			GetGuy(id)->GetShot(0, 0, 1000000, CurrentMap(), world);
			FireBullet(GetGuy(id)->x, GetGuy(id)->y, 0, BLT_COMETBOOM2);
			MakeSound(SND_FLAMEGO, x, y, SND_CUTOFF, 100);
		}
	}
	else if(ButtonTapped(CONTROL_B4))
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
	else if(ButtonTapped(CONTROL_B3))
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
}

void BeginArmageddon(void)
{
	armageddon=30*10;
}

bool ArmageddonIsUnderway(void)
{
	return (armageddon > 0);
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

bool BrutalMode(void)
{
	return player.difficulty == Difficulty::BRUTAL_CLASSIC || player.difficulty == Difficulty::BRUTAL_MODERN;
}

bool ClassicMode(void)
{
	return player.difficulty == Difficulty::BRUTAL_CLASSIC || player.difficulty == Difficulty::CLASSIC || player.difficulty == Difficulty::VERY_CLASSIC;
}

bool VeryClassicMode()
{
	return player.difficulty == Difficulty::VERY_CLASSIC;
}

const char* DifficultySuffix(Difficulty diff)
{
	switch (diff)
	{
		case Difficulty::CLASSIC:
			return " [C]";
		case Difficulty::MODERN:
			return " [M]";
		case Difficulty::BRUTAL_CLASSIC:
			return " [BC]";
		case Difficulty::BRUTAL_MODERN:
			return " [BM]";
		case Difficulty::VERY_CLASSIC:
			return " [99]";
		default:
			return "";
	}
}

void GainMoney(int amt)
{
	int total = player.money + player.bigMoney * 65536;
	total += amt;
	total = std::max(0, total);
	total = std::min(total, VeryClassicMode() ? 65535 : 9999999);
	player.money = (word)(total % 65536);
	player.bigMoney = (word)(total / 65536);
}

bool CanAffordMoney(int amt)
{
	int total = player.money + player.bigMoney * 65536;

	return (total >= amt);
}

int TotalMoney(void)
{
	return player.money + player.bigMoney * 65536;
}

void GetRuneInLevel(void)
{
	(*GetLevelPassedFlag(player.worldNum, player.levelNum)) |= LP_GOTRUNE;
}

void GetFairyBellInLevel(void)
{
	(*GetLevelPassedFlag(player.worldNum, player.levelNum)) |= LP_GOTFAIRY;
}

void GetSwordInLevel(void)
{
	(*GetLevelPassedFlag(player.worldNum, player.levelNum)) |= LP_GOTSWORD;
}

void GetSpellInLevel(void)
{
	(*GetLevelPassedFlag(player.worldNum, player.levelNum)) |= LP_GOTSPELL;
}

void GetRunePouchInLevel(void)
{
	(*GetLevelPassedFlag(player.worldNum, player.levelNum)) |= LP_GOTRUNEPOUCH;
}

void GetSkillShardInLevel(void)
{
	(*GetLevelPassedFlag(player.worldNum, player.levelNum)) |= LP_GOTSKILLSHARD;
}

bool GotSkillShardInLevel(byte world, byte level)
{
	return (*GetLevelPassedFlag(world, level)) & LP_GOTSKILLSHARD;
}

bool GotRunePouchInLevel(byte world, byte level)
{
	return (*GetLevelPassedFlag(world, level)) & LP_GOTRUNEPOUCH;
}

bool GotRuneInLevel(byte world,byte level)
{
	return (*GetLevelPassedFlag(world,level)) & LP_GOTRUNE;
}

bool GotFairyBellInLevel(byte world, byte level)
{
	return (*GetLevelPassedFlag(world,level)) & LP_GOTFAIRY;
}

bool GotSpellInLevel(byte world, byte level)
{
	return (*GetLevelPassedFlag(world,level)) & LP_GOTSPELL;
}

bool GotSwordInLevel(byte world, byte level)
{
	return (*GetLevelPassedFlag(world,level)) & LP_GOTSWORD;
}

byte PlayerPassedLevel(byte world, byte map)
{
	return map != 50 && ((*GetLevelPassedFlag(world,map)) & LP_PASSED);
}

byte* GetLevelPassedFlag(int chapter, int level)
{
	if (level < MAX_MAPS - 6)
		return &player.levelPassed[chapter][level];
	else
		return &player.levelPassed2[chapter][level - (MAX_MAPS - 6)];
}

void ResetLevelPassedFlags(int chapter, int level, bool resetAll)
{
	if (!resetAll)
		(*GetLevelPassedFlag(chapter, level)) &= (~(LP_PASSED|LP_GOTSKILLSHARD));	// we reset skill shards as well
	else
		(*GetLevelPassedFlag(chapter, level)) = 0;
}

void PickUpRune(void)
{
	GetRuneInLevel();
	if (ClassicMode())
	{
		NewMessage("I'm gonna pawn this sucker!", 75);
		GainMoney(20);
		MakeNormalSound(SND_MONEY);
		return;
	}
	playerGlow = 64;
	FloaterParticles(GetGoodguy()->x, GetGoodguy()->y, 1, 32, -1, 8);
	FloaterParticles(GetGoodguy()->x, GetGoodguy()->y, 1, 10, 1, 8);
	MakeNormalSound(SND_ALLCHLGCRYSTAL);
	byte num = Random((int)Rune::NUM_RUNES);
	byte started = num;
	while (1)
	{
		if (player.runes[num] == RUNE_EMPTY)
			break;
		num++;
		if (num >= (byte)Rune::NUM_RUNES)
			num = 0;
		if (num == started) // there were no ungotten runes
		{
			NewMessage("The rune fades into the mists!", 75);
			return;
		}
	}
	char s[64];
	sprintf(s, "%s Rune obtained!", RuneName((Rune)num));
	player.runes[(int)num] = RUNE_ASLEEP;
	NewMessage(s, 75);
	CheckForAllSecrets();
}

byte HighestWorldReached(void)
{
	byte world = 0;
	if (PlayerPassedLevel(0, 14))
		world++;
	if (PlayerPassedLevel(1, 12))
		world++;
	if (PlayerPassedLevel(2, 15))
		world++;
	return world;
}

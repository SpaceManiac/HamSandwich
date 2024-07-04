#include "goal.h"
#include "progress.h"
#include "editor.h"
#include "monster.h"
#include "player.h"
#include "bullet.h"
#include "shop.h"
#include "theater.h"
#include "gallery.h"
#include "steam.h"

char goalDesc[][48]={
	"Good Score","earning 5,000 points",
	"Crazy Score","earning 20,000 points",
	"Abject Failure","getting ZERO points",
	"Perfection","getting a Perfect Bonus",
	"Annihilation","getting 100% Destruction",
	"Utter Perfection","Perfect & 100% Destruction",
	"Quickie","finishing in under 15 seconds",
	"Zaptastic","earning 5,000 points in Zappo Raiders",
	"ULTRA ZAP!","earning 10,000 points in Zappo Raiders",
	"Goal In The Net","earning 100 points in the Net Game",
	//10
	"Combo Plate","getting a 20 Combo",
	"Super Combo Platter","getting a 100 Combo",
	"Family-Size Combo","getting a 500 Combo",
	"Variety Pack","beating one of everything",
	"Lunatic Addict","reaching 1 hour of playtime",
	"Obsessive-Compulsive","reaching 10 hours of playtime",
	"Long Walk","walking a mile in Bouapha's shoes",
	"Worn Out Moccassins","traveling over 100 miles on foot",
	"Over The Bounding Main","rafting over one mile",
	"Road Trip","driving and carting over five miles",
	//20
	"Worlds Apart","buying all Worlds",
	"Frozen With Fury","buying all Frozen Yogurts",
	"Smart Shopper","buying all Major Awards",
	"Massive Cheater","buying all Cheats",
	"Two Thumbs Up","seeing all Movies",
	"Privacy Violation","opening every locker",
	"Shop Til You Drop","emptying out the entire mall",
	"Dig Man Fever","getting 500,000 points on Dig Man IV",
	"Cave Crasher","getting 75,000 points on Cave Zoomer",
	"Locker Knocker","opening a certain locker",
	//30
	"Spider Squish","beating Matilda",
	"Supreme Spider Squish","beating Matilda-X",
	"Thingamajig","beating The Thing",
	"Himalayan Hijinks","beating Sven & Bjorn",
	"Desert Duel","beating Sphinxter",
	"Monkeyshines","beating Kongor",
	"Treehouse Of Terror","beating The Countess",
	"Mad Scientist","beating Dr. Lunatic",
	"Crab Legs For All!","beating Crabby Patty",
	"Crossing The Streams","beating Dozer",
	//40
	"Robot Rampage","beating the Loonybot 5000",
	"French Toast","beating King Cone",
	"Unhappy Stick","beating Happy Stick Man 10 times",
	"Self-Destruction","beating 10 Evil Clones",
	"Sucks, Don't It?","beating the Dust Devil",
	"D.U.I.","Running over 100 monsters",
	"Monster Slayer","beating 1,000 monsters",
	"Mega Monster Slayer","beating 10,000 monsters",
	"Ultra Monster Slayer","beating 50,000 monsters",
	"Ultimate Supreme Slayer","beating 100,000 monsters (wow!)",
	//50
	"Buddy System","beating a level with an alternate character",
	"Shroom Of Doom","beating a level as Shtupid Shroom",
	"Bullet Time","beating a monster while time is stopped",
	"Molten Irony","beating a fiery monster with the Toaster",
	"Bitterly Cold Irony","beating an icy monster while it's frozen",
	"Vampire Slayer","beating 200 of all Vampire types combined",
	"Mmm, Garlic Stake","spearing a Vampire while Garlic'd",
	"Deep Blue Shield","falling in water with an Energy Barrier on",
	"Huge Moron","winning \"Who Doesn't Want To Be A Moron?\"",
	"Lock Around The Clock","opening a certain locker",
	//60
	"Blue In The Face","holding your breath for an hour",
	"Passing On The Right","passing 25 levels",
	"Passing Grade","passing 100 levels",
	"Hail Mary Pass!","passing 1,000 levels",
	"One Hundred Club","getting 100% on a world",
	"100 Club Treasurer","getting 100% on 10 worlds",
	"100 Club President","getting 100% on 50 worlds",
	"Supreme Completion!","getting 100% on all worlds",
	"Door Prize","opening a certain locker",
	"Other Door Prize","opening a different locker",
	//70
	"Secret Service","entering a secret level",
	"Zorkin' Around","completing the Super Secret Surprise",
	"Lotta Loonykeys","getting 10 Keys Of Lunacy",
	"Key To Success","collecting 100 keys of any color",
	"Knock Knock","opening 100 doors",
	"Caught 'Em All!","getting all 4 keychains",
	"The Man With 500 Brains","collecting 500 brains",
	"BRRAAAIINNSS!","collecting 5,000 brains",
	"Candelabra","collecting 500 candles",
	"CAAANNNDDLLES!","collecting 5,000 candles",
	//80
	"Heads & Tails","collecting 20 coins in a level",
	"Bouapha Rockefeller","earning 10,000 coins total",
	"King Midas","earning 100,000 coins total",
	"Penny Pincher","holding 25,000 coins in hand",
	"Pig Out","eating 50,000 calories",
	"Scan-O-Rama","scanning every monster",
	"Petting Zoo Peril","surviving the Petting Zoo",
	"Cruisin'","masterful driving skills at the Mall",
	"World Peace","solving world hunger",
	"Lawnmower","chopping 100 grasses",
	//90
	"Burst Their Bubble","beating something with bubbles",
	"Hammer Time","throwing 5,000 hammers",
	"Half Past Hammer Time","throwing 50,000 hammers",
	"Raging Rage","raging 50 times",
	"Ragingly Raging Rage","raging 500 times",
	"Thrash Rock","smashing a Roly Poly to bits",
	"Trigger Happy","firing 5,000 special weapons",
	"Trigger Ecstatic","firing 50,000 special weapons",
	"Keep Trying","dying 100 times",
	"GOOOOAAALLLL!!","getting the other 99 goals",
};

void PrintGoalInfo(int x,int y,byte goal)
{
	char s[128];

	sprintf(s,"Goal #%03d: ",goal+1);

	if(profile.progress.goal[goal])
	{
		strcat(s,goalDesc[goal*2]);
	}
	else
		strcat(s,"???????");

	PrintGlow(x,y,s,0,2);
	if(profile.progress.goal[goal])
	{
		sprintf(s,"Earned for %s.",goalDesc[goal*2+1]);
		PrintGlowRect(x+30,y+16,620,y+20+60,18,s,2);
	}
}

void CompleteGoal(byte goal)
{
	int i,t;

	if(goal!=99)
	{
		t=0;
		for(i=0;i<100;i++)
			if(profile.progress.goal[i])
				t++;
		if(t>=99)
			CompleteGoal(99);
	}

	if(profile.progress.goal[goal])
		return;

	if(editing)
		return;

	if(tutorial)
		return;

	profile.progress.goal[goal]=1;
	SteamManager::Get()->CompleteGoal(goal);
	coro::launch(std::bind(ShowGoalEarned, goal));

	if(shopping)
		SetupGalPix(curMap);
}

void GoalKilledSomebody(Guy *g,byte type,byte frozen)
{
	dword totalKills;
	int i;

	if(type==MONS_BOUAPHA)
	{
		if(profile.progress.kills[MONS_BOUAPHA]>=100)
			CompleteGoal(98);
	}
	else if(type==MONS_MATBRAIN)
		CompleteGoal(30);
	else if(type==MONS_MAT2BRAIN)
		CompleteGoal(31);
	else if(type==MONS_THING)
		CompleteGoal(32);
	else if(type==MONS_SVEN || type==MONS_BJORN)
	{
		if((type==MONS_BJORN && profile.progress.kills[MONS_SVEN]>0) ||
		   (type==MONS_SVEN && profile.progress.kills[MONS_BJORN]>0))
			CompleteGoal(33);
	}
	else if(type==MONS_SPHINX)
		CompleteGoal(34);
	else if(type==MONS_KONGOR)
		CompleteGoal(35);
	else if(type==MONS_COUNTESS)
		CompleteGoal(36);
	else if(type==MONS_DRL)
		CompleteGoal(37);
	else if(type==MONS_PATTY)
		CompleteGoal(38);
	else if(type==MONS_DOZER)
		CompleteGoal(39);
	else if(type==MONS_LOONYCORE)
		CompleteGoal(40);
	else if(type==MONS_KINGCONE)
		CompleteGoal(41);
	else if(type==MONS_STICKMAN)
	{
		if(profile.progress.kills[MONS_STICKMAN]>=10)
			CompleteGoal(42);
	}
	else if(type==MONS_EVILCLONE)
	{
		if(profile.progress.kills[MONS_EVILCLONE]>=10)
			CompleteGoal(43);
	}
	else if(type==MONS_DUSTDEVIL)
		CompleteGoal(44);

	if(profile.progress.runOver>=100)
		CompleteGoal(45);

	if(player.timeStop)
		CompleteGoal(52);

	if(type==MONS_MAGMAZOID || type==MONS_BOILER || type==MONS_BOOMKIN || type==MONS_JALAPENO || type==MONS_UNDERMAGMA ||
		type==MONS_SCARAB || type==MONS_HOTSHROOM)
	{
		if(GetBulletAttackType()==BLT_FLAME || GetBulletAttackType()==BLT_FLAME2)
			CompleteGoal(53);	// burned a fire monster to death
	}

	if(type==MONS_ROLLER || type==MONS_ROLLER2)
		CompleteGoal(95);
	if(type==MONS_FROSTBITER || type==MONS_SNOWGUY || type==MONS_JACKFROST)
	{
		if(frozen)
			CompleteGoal(54);
	}

	if(type==MONS_VAMPIRE || type==MONS_COUNTESS || type==MONS_DARKVAMP || type==MONS_SPIKEY)
	{
		if(player.garlic && GetBulletAttackType()==BLT_SPEAR)
			CompleteGoal(56);
		totalKills=profile.progress.kills[MONS_VAMPIRE]+profile.progress.kills[MONS_DARKVAMP]+
				profile.progress.kills[MONS_COUNTESS]+profile.progress.kills[MONS_SPIKEY];
		if(totalKills>=200)
			CompleteGoal(55);
	}

	if(GetBulletAttackType()==BLT_BUBBLE && g->friendly==0)
		CompleteGoal(90);	// killed an enemy with bubbles

	totalKills=0;
	for(i=2;i<NUM_PROFILE_MONSTERS;i++)	// skip Bouapha
		totalKills+=profile.progress.kills[i];


	if(totalKills>1000)
		CompleteGoal(46);
	if(totalKills>10000)
		CompleteGoal(47);
	if(totalKills>50000)
		CompleteGoal(48);
	if(totalKills>100000)
		CompleteGoal(49);

	if(player.combo>=20)
		CompleteGoal(10);
	if(player.combo>=100)
		CompleteGoal(11);
	if(player.combo>=500)
		CompleteGoal(12);

	totalKills=0;
	for(i=2;i<NUM_PROFILE_MONSTERS;i++)	// skip Bouapha
	{
		if(!(MonsterFlags(i,i)&(MF_INVINCIBLE|MF_NOHIT)) && !(MonsterTheme(i)&(MT_GOOD|MT_BITS)) &&
			(MonsterTheme(i)))
		{
			// only count those that can be hurt, aren't body parts, and aren't in the 'good' group
			if(!profile.progress.kills[i])
			{
				totalKills=1;
				break;
			}
		}
	}
	if(!totalKills)	// every one of them has been beaten!
		CompleteGoal(13);
}

void GoalTallyPage(int score,byte perfect,byte destruct,int time)
{
	if(tutorial)
		return;

	if(score>=5000)
		CompleteGoal(0);
	if(score>=20000)
		CompleteGoal(1);
	if(score==0)
		CompleteGoal(2);
	if(perfect)
		CompleteGoal(3);
	if(destruct)
		CompleteGoal(4);
	if(perfect && destruct)
		CompleteGoal(5);
	if(time<15*30)
		CompleteGoal(6);
	if(profile.playAs!=PLAY_BOUAPHA)
		CompleteGoal(50);
	if(profile.playAs==PLAY_SHROOM)
		CompleteGoal(51);

	if(profile.progress.totalCoins>=10000)
		CompleteGoal(81);
	if(profile.progress.totalCoins>=100000)
		CompleteGoal(82);
	if(profile.progress.totalCoins-profile.progress.coinsSpent>=25000)
		CompleteGoal(83);
}

void GoalTimeDist(void)
{
	if(profile.progress.totalTime>=30*60*60)
		CompleteGoal(14);
	if(profile.progress.totalTime>=30*60*60*10)
		CompleteGoal(15);
	if(profile.progress.footDistance>=168960)
		CompleteGoal(16);
	if(profile.progress.footDistance>=168960*100)
		CompleteGoal(17);
	if(profile.progress.raftDistance>=168960)
		CompleteGoal(18);
	if(profile.progress.driveDistance+profile.progress.cartDistance>=168960*5)
		CompleteGoal(19);
	if(profile.progress.underwaterTime>=30*60*60)
		CompleteGoal(60);
}

void GoalPurchase(void)
{
	if(AllOfTypePurchased(SHOP_WORLD))
		CompleteGoal(20);
	if(AllOfTypePurchased(SHOP_ABILITY))
		CompleteGoal(21);
	if(AllOfTypePurchased(SHOP_MAJOR))
		CompleteGoal(22);
	if(AllOfTypePurchased(SHOP_CHEAT))
		CompleteGoal(23);
	if(AllMoviesSeen())
		CompleteGoal(24);
	if(AllLockersOpen())
		CompleteGoal(25);
	if(AllPurchased())
		CompleteGoal(26);

	if(profile.progress.totalCoins>=10000)
		CompleteGoal(81);
	if(profile.progress.totalCoins>=100000)
		CompleteGoal(82);
	if(profile.progress.totalCoins-profile.progress.coinsSpent>=25000)
		CompleteGoal(83);
}

void GoalWinLevel(void)
{
	dword d;

	d=CountLevelsPassed();
	CalcFinishedWorlds();

	if(d>=25)
		CompleteGoal(61);
	if(d>=100)
		CompleteGoal(62);
	if(d>=1000)
		CompleteGoal(63);
	if(profile.progress.finishedWorlds>0)
		CompleteGoal(64);
	if(profile.progress.finishedWorlds>=10)
		CompleteGoal(65);
	if(profile.progress.finishedWorlds>=50)
		CompleteGoal(66);
	if(profile.progress.finishedWorlds>=profile.progress.totalWorlds-1)
		CompleteGoal(67);
}

void GoalFire(void)
{
	if(profile.progress.hammersThrown>=5000)
		CompleteGoal(91);
	if(profile.progress.hammersThrown>=50000)
		CompleteGoal(92);
	if(profile.progress.rages>=50)
		CompleteGoal(93);
	if(profile.progress.rages>=500)
		CompleteGoal(94);
	if(profile.progress.shotsFired>=5000)
		CompleteGoal(96);
	if(profile.progress.shotsFired>=50000)
		CompleteGoal(97);
}

// -------------------------------------------------
static byte noKeyTime;

void InitShowGoal(MGLDraw *mgl,byte num)
{
	char txt[64];

	noKeyTime=60;

	MakeNormalSound(SND_ALLCANDLE);
	GetDisplayMGL()->LoadBMP("graphics/galgoal.bmp");

	CenterPrint(320,110,"Gallery Goal Complete!",0,0);
	sprintf(txt,"\"%s\"",goalDesc[num*2]);
	CenterPrint(320,220,txt,0,0);
	sprintf(txt,"Earned for %s.",goalDesc[num*2+1]);
	CenterPrint(320,340,txt,-32,1);

	GetTaps();
	GetArrowTaps();
	mgl->LastKeyPressed();
}

void ExitShowGoal(void)
{
	GetDisplayMGL()->ClearScreen();
	//GetDisplayMGL()->Flip();  // TODO: is this needed?

	RestoreGameplayGfx();
}

byte UpdateShowGoal(int *lastTime,MGLDraw *mgl)
{
	byte t;
	char k;

	k=mgl->LastKeyPressed();
	t=GetTaps()|GetArrowTaps();

	if(*lastTime>TIME_PER_FRAME*5)
		*lastTime=TIME_PER_FRAME*5;

	while(*lastTime>=TIME_PER_FRAME)
	{
		mgl->Process();
		*lastTime-=TIME_PER_FRAME;
		if(noKeyTime)
			noKeyTime--;
	}

	if(!noKeyTime)
	{
		if(k==27)
			return 1;

		if(t&(CONTROL_B1|CONTROL_B2))
			return 1;
	}

	return 0;
}

void RenderShowGoal(MGLDraw *mgl)
{
//	int i;

//	mgl->Flip();

	//for(i=0;i<480;i++)
	//	memcpy(&mgl->GetScreen()[i*mgl->GetWidth()],&image[i*640],640);
}

TASK(void) ShowGoalEarned(byte num)
{
	byte done=0;
	int lastTime=1;

	InitShowGoal(GetDisplayMGL(),num);

	while(!done)
	{
		lastTime+=TimeLength();
		StartClock();
		done=UpdateShowGoal(&lastTime,GetDisplayMGL());
		RenderShowGoal(GetDisplayMGL());

		AWAIT GetDisplayMGL()->Flip();

		if(!GetDisplayMGL()->Process())
			done=1;
		EndClock();
	}

	ExitShowGoal();
}

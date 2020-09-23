#include "goal.h"
#include "progress.h"
#include "editor.h"
#include "monster.h"
#include "player.h"
#include "bullet.h"
#include "shop.h"
#include "theater.h"
#include "gallery.h"

char goalDesc[][256]={
	//0
	"Good Score","finishing a level with at least 5,000 points",
	"Epic Score","finishing a level with at least 20,000 points",
	"Abject Failure","finishing a level with 0 points - you're not too good at this!",
	"Perfectionist","finishing a level without taking any damage",
	"Destructionist","finishing a level with 100% destruction... there can only be one!",
	"Quick Draw","finishing a level in 15 seconds or less",
	"Zaptastic","getting a score of 5,000 on Zappo Raiders",
	"Diggin' It","getting a score of 500,000 on Dig Man IV",
	"Carpel Tunnel","getting a score of 75,000 on Cave Zoomer",
	"Utter Perfection","finishing a level without taking damage, achieving 100% Destruction, and scoring at least 7,000 points - the power of three.",
	//10
	"Combo Plate","getting a 20x combo",
	"Super Combo Platter","getting a 100x combo",
	"Addicted?","playing for more than four hours... having fun?",
	"Overpass","finishing 25 levels",
	"Secret Service","finishing a secret level",
	"Heads & Tails","collecting at least 20 coins in a level",
	"One Hundred Club","100%ing your first world... conglaturation!",
	"Walk A Mile","walking one mile in-game... now go walk a mile in real life!",
	"Knock Knock Joke","opening 100 doors",
	"Gamer Gamer","getting a score of 15,000 on Zappo Raiders",
	//20
	"Chain Letter","obtaining all four keychains in a single world",
	"Einstein","collecting 500 brains overall",
	"Jack Be Nimble","collecting 500 candles overall",
	"Nickelback","finishing a level with exactly five coins in tow... get it?",
	"Deep Blue Shield","falling in water while using an Energy Barrier",
	"Passing Grade","finishing 100 levels overall",
	"Fortune 500","100%ing five worlds",
	"Drivin' My Life Away","driving & carting over five in-game miles - more eco-friendly than the real deal.",
	"Take the Plunge","rafting one in-game mile (I'm still waiting on that yacht!)",
	"Family Meal","somehow getting a 250x combo - nice one!!",
	//30
	"Buddy System","finishing ten levels using unlockable characters",
	"Blue In The Face","holding your breath for an hour (in-game, NOT in real life)",
	"Fahrenheit 128","defeating a fiery monster with the Toaster",
	"Freeze Frame","defeating an icy monster while it's frozen",
	"Pick Your Poison","defeating a poisonous monster with the Spore Cannon",
	"Stubble Burning","burning 200 bales of hay (or other flammable materials)",
	"Lawnmower","chopping 200 grasses with the Machete",
	"Bullet Time","defeating a monster while time is stopped",
	"Huge Moron","winning 'Who Doesn't Want to be a Moron?'",
	"Penny Pincher","holding 25,000 coins on hand... go donate to world hunger or something!",
	//40
	"Spider Squish","defeating Matilda in the Spooky Forest",
	"Thingamajig","defeating The Thing in the Cavernous Caves",
	"Himalayan Hijinks","defeating all three Yeti Brothers - Sven, Bjorn & Olaf - at the Icy Mountains",
	"Riddle Me This","defeating the Sphinxter in the Dusty Desert",
	"Monkeyshines","defeating KONGOR!! on Pygmy Island",
	"Treehouse of Terror","defeating both Countess & Lady von Spatula in the Haunted Mansion",
	"Crossing the Streams","completing the Forest Shrine and defeating Dozer the Dozerian",
	"Chick Magnet","defeating LunaChick and her robot form, LunaChick Prime",
	"Tropic of Cancer","defeating both Crabby Patty & Scary Scarab",
	"Mad Scientist","defeating Dr. Lunatic at Space Station Loony & the Crazy Asylum of Madness",
	//50
	"Optimum Prime","defeating Optimum Octon",
	"Gruff & Tumble","defeating the Brothers Gruff - Rammy, Hoppy & Gruffy",
	"Frankenfurters","defeating Frankenjulie and her miniature version",
	"Bite the Bullet","defeating both Harry & Larry",
	"Ghosts of a Chance","defeating Polterguy & Ghastly Stench",
	"Respect Your Elders","defeating the Vampire Elders - Stoney, Gusty, Toasty, Drippy & Summony",
	"Stick it To Em'","defeat all the Happy Stick Variations (barring final bosses)",
	"Do Evil Unto Evil","defeating the terrible Evilizer... as Dr. Lunatic",
	"Wrath of Khan","defeating Bobby Khan... as Kid Mystic",
	"Marshmallow Head","defeating a boss on Jamulio Mode... now THAT's dumb!",
	//60
	"Variety Pack","defeating one of each monster",
	"Self-Destruct Sequence","defeating 50 Evil Clones",
	"Hit & Run","running over 100 monsters with a Yu-Go or Mine Cart",
	"Slayer","defeating 2,000 monsters",
	"Anger Management","raging 50 times",
	"Garlic at Stake","skewering a vampire while doused in garlic",
	"What Goes Around...","defeating a monster with their own bullet",
	"Battin' A Thousand","100%ing ten worlds... now you're going places!",
	"Worn Out Moccasins","waking thirty in-game miles",
	"Conspiracy Wizard","typing the secret code at the Shopping Mall",
	//70
	"Underground Slayer","defeating a whopping 20,000 monsters",
	"Obsessive","playing for 20+ hours - you are now a true fan",
	"I Dream of Cheese","eating 7% of your body weight",
	"Stop! Hammer Time","throwing 5,000 hammers",
	"Trigger Finger","firing special weapons 5,000 times",
	"DOOM Shroom","finishing ten levels as... Shtupid Shroom?!",
	"Brainiac Maniac","collecting 5,000 brains overall - you're the Zom-Boss now",
	"Candleabra","collecting 5,000 candles overall",
	"Hail Mary Pass","finishing 500 levels",
	"Supreme Meal","consuming over 50,000 calories in food",
	//80
	"Petting Zoo Peril","somehow surviving the Petting Zoo at SPISMall",
	"Cruisin'","radical driving skills at SPISMall!",
	"Midas Touch","obtaining 50,000 coins overall",
	"Nam Sdrawkcab","completing five levels in Backwards Mode",
	"Absolute Boredom","standing still for a minute straight",
	"Thrash Rock","decimating a strong, sturdy Roly Poly (enjoy the free card!)",
	"Completionist","100%ing 40 worlds - yowza!!",
	"Half Past Hammer Time","throwing 25,000 hammers",
	"Cry Some More","firing special weapons 25,000 times",
	"Peace On Earth","solving world peace (now if only we can too!)",
	//90
	"When Worlds Collide","buying all the awesome world at SPISMall!",
	"Neapolitan","buying all the ice cream flavors at SPISMall",
	"Smart Shopper","buying all the presents at SPISMall",
	"Cheaters Never Die","buying all the cheats at SPISMall",
	"Fresh Tomatoes","unlocking and watching all the movies at SPISMall",
	"Privacy Violation","opening all the lockers at SPISMall - kinda creepy",
	"Mode & Median","unlocking all the special 'modes' at SPISMall",
	"Scan-O-Rama","filling out the Supreme Monster Database - thanks for your service!",
	"Shop Til' You Drop","completely buying out the SPISMall - you're our favorite customer!!",
	"Zorkin' Around","completing Bouapha's Text Adventure",
	//100
	"Keep Tryin'","dying 100 times",
	"It's Not Unusual","acquiring a purple/red monster card",
	"IDSPISPOPD","defeating 10,000 pumpkins of any varieties!",
	"Ultra Monster Slayer","defeating 100,000 monsters - WOAH!!!",
	"Bullet Sponge","taking 500,000 damage overall",
	"Rip & Tear","dealing 500,000 damage overall",
	"Purist","resisting the urge to cheat and stockpiling 1,000 cheat points",
	"Caught Em' All!","completing your deck of Monster Cards",
	"Millionaire","holding 1,000,000 coins in hand",
	"Offshore Account","earning 10,000,000 points overall",
	//110
	"The River Styx","defeating the true final boss",
	"R-R-R-REMIX!!","defeating the six main remix bosses",
	"Featuring Ludicrous","finishing a level on Ludicrious mode with at least 5,000 points",
	"Fast & Furious","finishing a level on Ludicrious mode with at least 7,000 points",
	"The Gang's All Here!","finishing a level with all the bells and whistles activated",
	"Rage Against the Lunatic","raging 500 times",
	"Dark, Darker Yet Darker","[REDACTED]",
	"Cornering the Market","obtaining 250 of any monster card Tier 2 or up",
	"Luck of the Draw","getting lucky while playing Dr. Lunatic: Operation SCARE!",
	"Supreme Overlord","being a true, blue Dr. Lunatic fan. Thank you so much!!",
	//120
	
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
	if(profile.progress.goal[goal])
		return;

	if(editing)
		return;

	if(tutorial)
		return;
	
	if(goal!=119)
	{
		t=0;
		for(i=0;i<119;i++)
			if(profile.progress.goal[i])
				t++;
		if(t>=119)
			CompleteGoal(119);
	}


	profile.progress.goal[goal]=1;
	ShowGoalEarned(goal);

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
			CompleteGoal(100);
	}
	else if(type==MONS_MATBRAIN || MONS_MAT2BRAIN)
		CompleteGoal(40);
	else if(type==MONS_THING)
		CompleteGoal(41);
	else if(type==MONS_SVEN || type==MONS_BJORN || type==MONS_OLAF)
	{
		if(profile.progress.kills[MONS_SVEN]>0 &&
		profile.progress.kills[MONS_BJORN]>0 && 
		profile.progress.kills[MONS_OLAF]>0)
			CompleteGoal(42);
	}
	else if(type==MONS_SPHINX)
		CompleteGoal(43);
	else if(type==MONS_KONGOR)
		CompleteGoal(44);
	else if(type==MONS_COUNTESS)
		CompleteGoal(45);
	else if(type==MONS_DOZER)
		CompleteGoal(46);
	else if(type==MONS_LUNABOSS || type==MONS_LUNAMECHA)
	{
		if(profile.progress.kills[MONS_LUNABOSS]>0 &&
		profile.progress.kills[MONS_LUNAMECHA]>0)
			CompleteGoal(47);
	}
	else if(type==MONS_PATTY || type==MONS_SCARAB)
	{
		if(profile.progress.kills[MONS_PATTY]>0 &&
		profile.progress.kills[MONS_SCARAB]>0)
			CompleteGoal(48);
	}
	else if(type==MONS_LOONYCORE || type==MONS_DRL || type==MONS_SDZL)
	{
		if(profile.progress.kills[MONS_LOONYCORE]>0 &&
		profile.progress.kills[MONS_DRL]>0 && 
		profile.progress.kills[MONS_SDZL]>0)
			CompleteGoal(49);
	}
	else if(type==MONS_STICKMAN || type==MONS_STICKTREE || type==MONS_STICKWITCH || type==MONS_STKSHROOM ||
	type==MONS_STKSPIDER || type==MONS_STKCORPSE || type==MONS_STKBAT || type==MONS_DANCER ||
	type==MONS_STKFISH || type==MONS_STKSNOW)
	{
		if(profile.progress.kills[MONS_STICKMAN]>0 &&
		profile.progress.kills[MONS_STICKTREE]>0 && 
		profile.progress.kills[MONS_STICKWITCH]>0 &&
		profile.progress.kills[MONS_STKSHROOM]>0 && 
		profile.progress.kills[MONS_STKSPIDER]>0 && 
		profile.progress.kills[MONS_STKCORPSE]>0 && 
		profile.progress.kills[MONS_STKBAT]>0 && 
		profile.progress.kills[MONS_DANCER]>0 &&
		profile.progress.kills[MONS_STKFISH]>0 && 
		profile.progress.kills[MONS_STKSNOW]>0)
		CompleteGoal(55);
	}
	else if(type==MONS_EVILCLONE)
	{
		if(profile.progress.kills[MONS_EVILCLONE]>=50)
			CompleteGoal(61);
	}
	else if(type==MONS_OCTOBOSS)
		CompleteGoal(50);
	else if(type==MONS_GOAT1 || type==MONS_GOAT2 || type==MONS_GOAT3)
	{
		if(profile.progress.kills[MONS_GOAT1]>0 &&
		profile.progress.kills[MONS_GOAT2]>0 && 
		profile.progress.kills[MONS_GOAT3]>0)
			CompleteGoal(51);
	}
	else if(type==MONS_FRANK || type==MONS_MINIFRANK)
	{
		if(profile.progress.kills[MONS_FRANK]>0 &&
		profile.progress.kills[MONS_MINIFRANK]>0)
			CompleteGoal(52);
	}
	else if(type==MONS_LARRY || type==MONS_HARRY)
	{
		if(profile.progress.kills[MONS_LARRY]>0 &&
		profile.progress.kills[MONS_HARRY]>0)
			CompleteGoal(53);
	}
	else if(type==MONS_BIGGHOST || type==MONS_GHASTLY)
	{
		if(profile.progress.kills[MONS_BIGGHOST]>0 &&
		profile.progress.kills[MONS_GHASTLY]>0)
			CompleteGoal(54);
	}
	else if(type==MONS_ELDER || MONS_ELDER2 || MONS_ELDER3 || MONS_ELDER4 || MONS_ELDER5)
	{
		if(profile.progress.kills[MONS_ELDER]>0 &&
		profile.progress.kills[MONS_ELDER2]>0 && 
		profile.progress.kills[MONS_ELDER3]>0 &&
		profile.progress.kills[MONS_ELDER4]>0 && 
		profile.progress.kills[MONS_ELDER5]>0)
		CompleteGoal(56);
	}
	else if(type==MONS_EVILIZER && (profile.playAs==PLAY_LUNATIC || profile.playAs==PLAY_LOONY))
		CompleteGoal(57);
	else if(type==MONS_BOBBY && profile.playAs==PLAY_MYSTIC)
		CompleteGoal(58);
	else if(((MonsterTheme(i)&(MT_BOSS)&&type!=MONS_MATHEAD&&type!=MONS_MAT2BRAIN)||(type==MONS_MATBRAIN||type==MONS_MAT2BRAIN) && profile.difficulty>=4))
		CompleteGoal(59);

	if(profile.progress.runOver>=100)
		CompleteGoal(62);

	if(player.timeStop)
		CompleteGoal(37);

	//fahrenheit 128
	if(type==MONS_MAGMAZOID || type==MONS_BOILER || type==MONS_BOOMKIN || type==MONS_JALAPENO || type==MONS_UNDERMAGMA ||
		type==MONS_SCARAB || type==MONS_HOTSHROOM || type==MONS_INCABOSS || type==MONS_FIREBUG || type==MONS_FIREBAT ||
		type==MONS_MADCOW2 || type==MONS_PUMPKINFLY || type==MONS_PUMPKINFLY2 || type==MONS_NUMBSKULL || type==MONS_BOMBIE ||
		type==MONS_ELDER4 || type==MONS_SKULLY || type==MONS_FROGURT || type==MONS_HOTDOG || type==MONS_BOMBIELORD ||
		type==MONS_PYGMYFIRE || type==MONS_CENTIBBODY || type==MONS_CENTIBHEAD || type==MONS_HOTFOOT)
	{
		if(GetBulletAttackType()==BLT_FLAME || GetBulletAttackType()==BLT_FLAME2 || GetBulletAttackType()==BLT_FLAME3 ||
		GetBulletAttackType()==BLT_FLAME4 || GetBulletAttackType()==BLT_FLAME5 || GetBulletAttackType()==BLT_SITFLAME ||
		GetBulletAttackType()==BLT_BADSITFLAME || GetBulletAttackType()==BLT_FLAMEWALL)
			CompleteGoal(32);	// burned a fire monster to death
	}

	if((type==MONS_ROLLER || type==MONS_ROLLER2)&&GetBulletAttackType()!=BLT_NONE)
		CompleteGoal(85);
	
	//freeze frame
	if(type==MONS_FROSTBITER || type==MONS_SNOWGUY || type==MONS_JACKFROST || type==MONS_GLASSJAW || type==MONS_ARCTICWOLF || 
		type==MONS_KELVOZOID || type==MONS_STKSNOW)
	{
		if(frozen)
			CompleteGoal(33);
	}
	
	//pick your poison
	if(type==MONS_SHROOM || type==MONS_MUSH || type==MONS_LICH || type==MONS_PYGMY3|| type==MONS_DARKVAMP ||
		type==MONS_CREEPAZOID || type==MONS_SHROOMLORD || type==MONS_STKSHROOM || type==MONS_GANGRENE ||
		type==MONS_MUTANT || type==MONS_FROG3 || type==MONS_SWAMPSTUMP || type==MONS_GASBAT)
	{
		if(GetBulletAttackType()==BLT_FLAME || GetBulletAttackType()==BLT_SPORE)
			CompleteGoal(34);	// burned a fire monster to death
	}
	
	//vampire
	if(type==MONS_VAMPIRE || type==MONS_COUNTESS || type==MONS_DARKVAMP || type==MONS_SPIKEY ||
	type==MONS_DRACULITE || type==MONS_VAMPLORD || type==MONS_ELDER || type==MONS_ELDER2 ||
	type==MONS_ELDER3 || type==MONS_ELDER4 || type==MONS_ELDER5 || type==MONS_NIGHTVAMP || 
	type==MONS_VAMPJR || type==MONS_ESHKAH || type==MONS_BONKULA || type==MONS_MECHABONK)
	{
		if(player.garlic && GetBulletAttackType()==BLT_SPEAR)
			CompleteGoal(56);
	}

	if(GetBulletAttackType()==BLT_BUBBLE && g->friendly==0)
		CompleteGoal(90);	// killed an enemy with bubbles

	totalKills=0;
	for(i=2;i<NUM_MONSTERS;i++)	// skip Bouapha
		totalKills+=profile.progress.kills[i];


	if(totalKills>2000)
		CompleteGoal(63);
	if(totalKills>20000)
		CompleteGoal(70);
	if(totalKills>100000)
		CompleteGoal(103);

	if(player.combo>=20)
		CompleteGoal(10);
	if(player.combo>=100)
		CompleteGoal(11);
	if(player.combo>=250)
		CompleteGoal(29);

	totalKills=0;
	for(i=2;i<NUM_MONSTERS;i++)	// skip Bouapha
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
		CompleteGoal(60);
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
		CompleteGoal(9);
	if(time<16*30)
		CompleteGoal(6);
	if(profile.playAs!=PLAY_BOUAPHA)
		CompleteGoal(30);
	if(profile.playAs==PLAY_SHROOM)
		CompleteGoal(75);

	if(profile.progress.totalCoins>=50000)
		CompleteGoal(82);
	if(profile.progress.totalCoins-profile.progress.coinsSpent>=1000000)
		CompleteGoal(109);
	if(profile.progress.totalCoins-profile.progress.coinsSpent>=25000)
		CompleteGoal(39);
}

void GoalTimeDist(void)
{
	if(profile.progress.totalTime>=30*60*60*4)
		CompleteGoal(12);
	if(profile.progress.totalTime>=30*60*60*20)
		CompleteGoal(71);
	if(profile.progress.footDistance>=168960)
		CompleteGoal(17);
	if(profile.progress.footDistance>=168960*30)
		CompleteGoal(68);
	if(profile.progress.raftDistance>=168960)
		CompleteGoal(28);
	if(profile.progress.driveDistance+profile.progress.cartDistance>=168960*5)
		CompleteGoal(27);
	if(profile.progress.underwaterTime>=30*60*60)
		CompleteGoal(31);
}

void GoalPurchase(void)
{
	if(AllOfTypePurchased(SHOP_WORLD))
		CompleteGoal(90);
	if(AllOfTypePurchased(SHOP_ABILITY))
		CompleteGoal(91);
	if(AllOfTypePurchased(SHOP_MAJOR))
		CompleteGoal(92);
	if(AllOfTypePurchased(SHOP_CHEAT))
		CompleteGoal(93);
	if(AllMoviesSeen())
		CompleteGoal(94);
	if(AllLockersOpen())
		CompleteGoal(95);
	if(AllPurchased())
		CompleteGoal(98);

	if(profile.progress.totalCoins>=10000)
		CompleteGoal(81);
	if(profile.progress.totalCoins>=100000)
		CompleteGoal(82);
	if(profile.progress.totalCoins-profile.progress.coinsSpent>=25000)
		CompleteGoal(83);
	if(profile.progress.totalCoins-profile.progress.coinsSpent>=1000000)
		CompleteGoal(110);
}

void GoalWinLevel(void)
{
	dword d;

	d=CountLevelsPassed();
	CalcFinishedWorlds();
	
	if (Random(1000)==1)
		CompleteGoal(118);

	if(d>=25)
		CompleteGoal(13);
	if(d>=100)
		CompleteGoal(25);
	if(d>=500)
		CompleteGoal(78);
	if(profile.progress.finishedWorlds>0)
		CompleteGoal(16);
	if(profile.progress.finishedWorlds>=5)
		CompleteGoal(26);
	if(profile.progress.finishedWorlds>=10)
		CompleteGoal(67);
	if(profile.progress.finishedWorlds>=40)
		CompleteGoal(86);
}

void GoalFire(void)
{
	if(profile.progress.hammersThrown>=5000)
		CompleteGoal(73);
	if(profile.progress.hammersThrown>=25000)
		CompleteGoal(87);
	if(profile.progress.rages>=50)
		CompleteGoal(64);
	if(profile.progress.rages>=500)
		CompleteGoal(115);
	if(profile.progress.shotsFired>=5000)
		CompleteGoal(74);
	if(profile.progress.shotsFired>=25000)
		CompleteGoal(88);
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
	GetDisplayMGL()->Flip();

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

void ShowGoalEarned(byte num)
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

		GetDisplayMGL()->Flip();

		if(!GetDisplayMGL()->Process())
			done=1;
		EndClock();
	}

	ExitShowGoal();
}

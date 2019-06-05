#include "achieve.h"
#include "skill.h"
#include "talents.h"
#include "player.h"
#include "options.h"
#include "title.h"

typedef struct achieve_t
{
	char name[24];
	char desc[32];
	byte type;
	word amt;
} achieve_t;

typedef struct mod_t
{
	char name[16];
	char desc[128];
} mod_t;

achieve_t achievement[100]={
	// row 1
	{"Ureb","Lbh jba gur tnzr!",ACH_SPECIAL,0},
	{"Pubc Fhrl","Yriry 10 Pubccvat Gnyrag.",ACH_TALENT,TLT_CHOPPING},
	{"Nkr Znfgre","Yriry 15 Nkr Znfgrel Fxvyy.",ACH_SKILL,SKILL_AXEMASTERY},
	{"Onfushy","Yriry 15 Onfu Fxvyy.",ACH_SKILL,SKILL_BASH},
	{"Qrsraqre","Yriry 15 Qrsrafr Fxvyy.",ACH_SKILL,SKILL_DEFENSE},
	{"Fgbzcrqr","Yriry 15 Fgbzc Fxvyy.",ACH_SKILL,SKILL_STOMP},
	{"Ra Tneqr","Yriry 15 Cneel Fxvyy.",ACH_SKILL,SKILL_PARRY},
	{"Fghaavat","Yriry 15 Fgha Fxvyy.",ACH_SKILL,SKILL_STUN},
	{"X.B.","Yriry 10 Xabpxbhg Gnyrag.",ACH_TALENT,TLT_KNOCKOUT},
	{"Gvgyr Ubyqre","Jba 100 Neran Onggyrf.",ACH_SPECIAL,0},
	// row 2
	{"Ubg Fghss","Yriry 10 Fpbepuvat Gnyrag.",ACH_TALENT,TLT_SCORCHING},
	{"Pbzcyrgvfg","Lbh tbg 100% pbzcyrgr!",ACH_SPECIAL,0},
	{"Gjb Guhzof Hc","Yriry 10 Pevgvpvfz Gnyrag.",ACH_TALENT,TLT_CRITICISM},
	{"Cvpxl Cvpxl","Yriry 15 Pevgvpny Fxvyy.",ACH_SKILL,SKILL_CRITICAL},
	{"Gbhtu Yhpx","Yriry 15 Gbhtuarff Fxvyy.",ACH_SKILL,SKILL_TOUGHNESS},
	{"Fgebat Yvxr Uhyx","Yriry 15 Fgeratgu Fxvyy.",ACH_SKILL,SKILL_STRENGTH},
	{"Fyhecl","Yriry 15 Inzcvevfz Fxvyy.",ACH_SKILL,SKILL_VAMPIRE},
	{"Zvfrel","Yriry 10 Zvfre Gnyrag.",ACH_TALENT,TLT_MISER},
	{"Jrrqxvyyre","Fyrj 5,000 Cynagf.",ACH_SPECIAL,0},
	{"Negshy Qbqtvat","Yriry 10 Svarffr Gnyrag.",ACH_TALENT,TLT_FINESSE},
		// row 3
	{"Vtavgvba","Yriry 15 Vtavgr Fxvyy.",ACH_SKILL,SKILL_IGNITE},
	{"Xvaqyvat","Yriry 10 Pbzohfgvba Gnyrag.",ACH_TALENT,TLT_COMBUSTION},
	{"Znqzna","Lbh svavfurq Znqpnc zbqr!",ACH_SPECIAL,0},
	{"Zl Fubrf Uheg","Yriry 10 Zbovyvgl Gnyrag.",ACH_TALENT,TLT_MOBILITY},
	{"Zbagntr","Yriry 15 Genvavat Fxvyy.",ACH_SKILL,SKILL_TRAINING},
	{"Cevapr Punezvat","Yriry 15 Crefhnfvba Fxvyy.",ACH_SKILL,SKILL_PERSUADE},
	{"Jura Va Ebzr","Yriry 10 Tynqvngbe Gnyrag.",ACH_TALENT,TLT_GLADIATOR},
	{"Fxvyy Znfgre","Yriry 18 va nal fxvyy!",ACH_SPECIAL,0},
	{"Onmnne Orunivbe","Yriry 10 Unttyvat Gnyrag.",ACH_TALENT,TLT_HAGGLING},
	{"Trg Bhggn Qbqtr","Yriry 15 Qbqtr Fxvyy.",ACH_SKILL,SKILL_DODGE},
	// row 4
	{"Crnfnagf Orjner","Yriry 15 Oheavangr Fxvyy.",ACH_SKILL,SKILL_BURNINATE},
	{"Fubpx Inyhr","Yriry 15 Fubpx Fxvyy.",ACH_SKILL,SKILL_SHOCK},
	{"Obbtnybb","Yriry 10 Ryrpgebphgvba Gnyrag.",ACH_TALENT,TLT_ELECTROCUTION},
	{"Fcrrq Qrzba","Lbh svavfurq va haqre 4 ubhef.",ACH_SPECIAL,0},
	{"Tbvat Ahgf","Yriry 15 Orefrex Fxvyy.",ACH_SKILL,SKILL_BERSERK},
	{"Jbbqfzna","Yriry 15 Genpxvat Fxvyy.",ACH_SKILL,SKILL_TRACKING},
	{"Zbaxrl Fbpxvat","Fznfurq 5,000 Fbpx Zbaxrlf.",ACH_SPECIAL,0},
	{"Ihygher","Yriry 10 Fpniratvat Gnyrag.",ACH_TALENT,TLT_SCAVENGING},
	{"Ebnfg Ornfg","Yriry 15 Synzr Fxvyy.",ACH_SKILL,SKILL_FLAME},
	{"Enoovg'f Sbbg","Yriry 15 Yhpx Fxvyy.",ACH_SKILL,SKILL_LUCK},
	// row 5
	{"Gbnfgznfgre","Yriry 15 Gbnfgvrf Fxvyy.",ACH_SKILL,SKILL_TOASTIES},
	{"N Zvtugl Jvaq","Yriry 15 Tnyr Fxvyy.",ACH_SKILL,SKILL_GALE},
	{"Juvgr Abvfr","Yriry 15 Fgngvp Fxvyy.",ACH_SKILL,SKILL_STATIC},
	{"Cbyvgvpny Nfcvengvbaf","Yriry 10 Jvaqont Gnyrag.",ACH_TALENT,TLT_WINDBAG},
	{"Byqre & Jvfre","Lbh tbg 1 zvyyvba KC!",ACH_SPECIAL,0},
	{"Chccrg Chapure","Fznfurq 5,000 Chccrgf.",ACH_SPECIAL,0},
	{"Fcel Thl","Yriry 10 Ivinpvgl Gnyrag.",ACH_TALENT,TLT_VIVACITY},
	{"Unat Va Gurer","Yriry 15 Raqhenapr Fxvyy.",ACH_SKILL,SKILL_ENDURANCE},
	{"Obzoneqvre","Yriry 15 Nvefgevxr Fxvyy.",ACH_SKILL,SKILL_AIRSTRIKE},
	{"Gevpx Fubbgre","Yriry 15 Evpbpurg Fxvyy.",ACH_SKILL,SKILL_RICOCHET},
	// row 6
	{"Jnez & Pbml","Yriry 15 Urng Fuvryq Fxvyy.",ACH_SKILL,SKILL_HEATSHIELD},
	{"Qreivfu","Yriry 15 Juvey Fxvyy.",ACH_SKILL,SKILL_WHIRL},
	{"Abg Va Xnafnf","Yriry 15 Gbeanqb Fxvyy.",ACH_SKILL,SKILL_TORNADO},
	{"Fhpxre Chapu","Yriry 10 Farnx Nggnpx Gnyrag.",ACH_TALENT,TLT_SNEAKATTACK},
	{"Obqmun Oynfgre","Fznfurq 5,000 Obqmunf.",ACH_SPECIAL,0},
	{"Fglyvfu","Jrne nyy Negvsnpgf.",ACH_SPECIAL,0},
	{"Ebqragvn","Yriry 10 Cnpxeng Gnyrag.",ACH_TALENT,TLT_PACKRAT},
	{"Fgbby Cvtrba","Yriry 15 Ubzvat Fxvyy.",ACH_SKILL,SKILL_HOMING},
	{"Guerr Gvzrf N Ybbal","Yriry 15 Gevcyr Fxvyy.",ACH_SKILL,SKILL_TRIPLE},
	{"Onjql Cvrepvatf","Yriry 15 Cvrepvat Fxvyy.",ACH_SKILL,SKILL_PIERCE},
	// row 7
	{"Qe. Mbvqoret","Yriry 15 Pelbmbvq Fxvyy.",ACH_SKILL,SKILL_CRYOZOID},
	{"Wnpx Sebfg","Yriry 15 Pbyq Jnir Fxvyy.",ACH_SKILL,SKILL_COLDWAVE},
	{"Qvt Qbja Qrrc","Yriry 10 Zvavat Gnyrag.",ACH_TALENT,TLT_MINING},
	{"Tubfgohfgre","Ohfgrq 2,000 Tubfgf.",ACH_SPECIAL},
	{"Obb!","Yriry 10 Fcbbxvarff Gnyrag.",ACH_TALENT,TLT_SPOOKYBONES},
	{"Gerruhttre","Yriry 10 Terra Guhzo Gnyrag.",ACH_TALENT,TLT_GREENTHUMB},
	{"Pbyyrpgbe","Ubyqvat 255 bs na vgrz.",ACH_SPECIAL,0},
	{"Ntrag Fzvgu","Yriry 10 Whaxfzvgu Gnyrag.",ACH_TALENT,TLT_JUNKSMITH},
	{"Vg'f N Genc!","Yriry 15 Nkr Genc Fxvyy.",ACH_SKILL,SKILL_TURRET},
	{"Ohggresvatref","Yriry 15 Funggre Fxvyy.",ACH_SKILL,SKILL_SHATTER},
	// row 8
	{"Ba Gur Ebpxf","Yriry 15 Vpr Phor Fxvyy.",ACH_SKILL,SKILL_ICECUBE},
	{"Pbyq Urnegrq","Yriry 10 Serrmvat Gnyrag.",ACH_TALENT,TLT_FREEZING},
	{"Orne Vf Qevivat","Fznfurq 5,000 Ornef.",ACH_SPECIAL,0},
	{"Nggragvir","Yriry 10 Zragny Sbphf Gnyrag.",ACH_TALENT,TLT_FOCUS},
	{"Qrnqurnqf","Yriry 15 Obarurnq Fxvyy.",ACH_SKILL,SKILL_BONEHEAD},
	{"Oreel Tbbq","Yriry 15 Yvsroreel Fxvyy.",ACH_SKILL,SKILL_LIFEBERRY},
	{"Jrrq Cvpxre","Yriry 10 Ureonyvfz Gnyrag.",ACH_TALENT,TLT_HERBALISM},
	{"Ovt Sna","Jber 'Unj Zhzh' Yrafrf.",ACH_SPECIAL,0},
	{"Pybooreva' Gvzr","Yriry 10 Pybpxjbex Gnyrag.",ACH_TALENT,TLT_CLOCKWORK},
	{"Nkr Njnl","Yriry 15 Guebjvat Fxvyy.",ACH_SKILL,SKILL_THROWING},
	// row 9
	{"Vaivapvoyr","Yriry 10 Fgbarfxva Gnyrag.",ACH_TALENT,TLT_STONESKIN},
	{"Qrsebfg","Gunjrq 1,000 vpr rarzvrf.",ACH_SPECIAL,0},
	{"Puvyqcebbs Pnc","Yriry 10 Nefravp Gnyrag.",ACH_TALENT,TLT_ARSENIC},
	{"Fxhyy & Obarf","Yriry 15 Cbvfba Fxvyy.",ACH_SKILL,SKILL_POISON},
	{"Dhnenagvar","Yriry 15 Cynthr Fxvyy.",ACH_SKILL,SKILL_PLAGUE},
	{"Znq Obzore","Yriry 15 Obar Obzo Fxvyy.",ACH_SKILL,SKILL_BONEBOMB},
	{"Oevne Cngpu","Yriry 15 Oenzoyrf Fxvyy.",ACH_SKILL,SKILL_BRAMBLES},
	{"Co -> Nh","Yriry 10 Nypurzl Gnyrag.",ACH_TALENT,TLT_ALCHEMY},
	{"Fpebbtr","Uryq $99999 va unaq.",ACH_SPECIAL,0},
	{"Fgebatnez","Yriry 10 Gbffvat Gnyrag.",ACH_TALENT,TLT_TOSSING},
	// row 10
	{"Crfg Pbageby","Fznfurq 5,000 Zvpr.",ACH_SPECIAL,0},
	{"Buzzzz","Yriry 10 Zrqvgngvba Gnyrag.",ACH_TALENT,TLT_MEDITATION},
	{"Xnezvp Onynapr","Yriry 15 Erfgbengvba Fxvyy.",ACH_SKILL,SKILL_RESTORE},
	{"Bhgjvg & Bhgynfg","Yriry 15 Fheiviny Fxvyy.",ACH_SKILL,SKILL_SURVIVAL},
	{"Syhfurq","Yriry 15 Qenva Fxvyy.",ACH_SKILL,SKILL_DRAIN},
	{"Rneguzbire","Yriry 15 Fgbar Fcvxr Fxvyy.",ACH_SKILL,SKILL_SPIKE},
	{"Sha Thl","Yriry 15 Fuebbz Fxvyy.",ACH_SKILL,SKILL_SHROOM},
	{"Zntvp Gevpx","Yriry 15 Raretl Fxvyy.",ACH_SKILL,SKILL_ENERGY},
	{"Gjvfg Bs Yrzba","Yriry 10 Zvkbybtl Gnyrag.",ACH_TALENT,TLT_MIXOLOGY},
	{"Bofrffvir","100 bs rirel Znqpnc Pelfgny.",ACH_SPECIAL,0},
};

mod_t mod[MAX_MODS]={
	{"None","You haven't chosen a Modifier yet."},
	{"Easy","You do double damage, and enemies do half damage."},
	{"Hard","You do half damage, enemies do double damage.  Combining with Easy is the same as neither!"},
	{"Onslaught","Enemies constantly respawn - it's more dangerous, but you'll end up with more items and XP."},
	{"Expert","You begin the game with all Skills and Talents.  Choose your own progression!"},
	{"Literacy","You may read the Allusionary in the Halls Of History."},
	{"Crowded","Face four times as many enemies as normal.  Twice as many in the Arena."},
	{"Toy","A Toy Bodzha follows you around, looking cute and providing very little help."},
	{"Rush","The game runs twice as fast."},
	{"Ironman","Dying or finishing Madcap mode ends your game, and gives you a score to show off your skillz."},
	{"Disco","You wear a stunning Disco Parka.  So stunning that nearby enemies get stunned regularly."},
	{"Vampire","Your life drains over time, but you gain health with every enemy you beat."},
	{"Liquid","Enemies are much more likely to drop potions and less likely to drop anything else."},
	{"Gangsta","Look like an OG (Onion Gangster)."},
	{"Arena","Arena battles pay 5x as much and give 5x XP, but you earn no XP or money outside the Arena."},
	{"Knight","You've joined up with the Order Of The Snuggly Bunny for good!"},
	{"Skillbor","You gain no Skill Points from levels.  Instead, gain one for each Quest or Present."},
	{"Trample","Bumping enemies does damage to them equal to your Level.  Enemies have double Armor."},
	{"Tireless","Attacks cost no Stamina, but always cost 1 Magic."},
	{"Heir","Begin the game with a set of Golden items."},
	{"Haunted","Slain enemies create Ghosts you also have to beat."},
	{"Ticking","Enemies have double life, but Clockwork Robot does triple damage and takes half damage."},
	{"Merchant","Enemies drop no equipment, but 5x money.  All shops may carry Golden items."},
	{"Even","Gain 3 Skill Points per level instead of an increasing amount."},
};

byte modHoriz[10]={MOD_LITERACY,MOD_KNIGHT,MOD_SKILLBOR,MOD_TRAMPLE,MOD_TIRELESS,MOD_HEIR,MOD_HAUNTED,MOD_TICKING,MOD_MERCHANT,MOD_EVEN};
byte modVert[10]={MOD_EXPERT,MOD_CROWDED,MOD_TOY,MOD_RUSH,MOD_IRONMAN,MOD_DISCO,MOD_VAMPIRE,MOD_LIQUID,MOD_GANGSTA,MOD_ARENA};

byte ModifierOn(byte mod)
{
	return (byte)(player.var[VAR_MODIFIER+0]==mod || player.var[VAR_MODIFIER+1]==mod || player.var[VAR_MODIFIER+2]==mod);
}

const char *ModifierName(byte m)
{
	if(m<MAX_MODS)
		return mod[m].name;
	else
		return "-";
}

const char *ModifierDesc(byte m)
{
	if(m<MAX_MODS)
		return mod[m].desc;
	else
		return "-";
}

void AdjustModifier(byte *me,byte w,char dir,byte *list)
{
	byte newMod;

	newMod=*me;

	while(1)
	{
		newMod+=dir;
		if(newMod>MAX_MODS)
		{
			if(dir==1)
				newMod=255;
			else
				newMod=MAX_MODS;
			continue;
		}
		if(opt.modifier[newMod]==0)
			continue;

		if(newMod!=0 &&		// "None" can be repeated as often as you like
			((w!=0 && list[0]==newMod) ||
			 (w!=1 && list[1]==newMod) ||
			 (w!=2 && list[2]==newMod)))
			 continue;
		break;
	}
	(*me)=newMod;
}

// ---------------------------- ACHIEVEMENT DISPLAY DURING PLAY

static char lastKey=0;
static byte btnwait;
static byte newAch,newMod[2];
static char txt[32];

char *ConvertText(char *src)
{
	int i;
	byte b;

	txt[strlen(src)]='\0';
	for(i=0;i<(int)strlen(src);i++)
	{
		if(src[i]>='a' && src[i]<='z')
		{
			b=(byte)src[i];
			b+=13;
			if(b>(byte)'z')
				b-=26;
			txt[i]=(char)b;
		}
		else if(src[i]>='A' && src[i]<='Z')
		{
			b=(byte)src[i];
			b+=13;
			if(b>(byte)'Z')
				b-=26;
			txt[i]=(char)b;
		}
		else
			txt[i]=src[i];
	}
	return txt;
}

void RenderAchieve(MGLDraw *mgl)
{
	char s[32];

	// 436 wide
	PauseBox(102,100,538,160,32*5+13);

	strcpy(s,"New Achievement!!");
	CenterPrintColor(320-1,70-1,s,0,-32,0);
	CenterPrintColor(320+1,70-1,s,0,-32,0);
	CenterPrintColor(320-1,70+1,s,0,-32,0);
	CenterPrintColor(320+1,70+1,s,0,-32,0);
	CenterPrintGlow(320,70,s,0,0);

	CenterPrintGlow(320,104,ConvertText(achievement[newAch].name),0,0);

	CenterPrint(320,134,ConvertText(achievement[newAch].desc),0,1);

	if(newMod[0]!=0)
	{
		PauseBox(102,300,538,360,32*1+13);

		if(newMod[1])
			strcpy(s,"Unlocked 2 Game Modifiers!");
		else
			strcpy(s,"Unlocked A Game Modifier!");
		CenterPrintColor(320-1,270-1,s,0,-32,0);
		CenterPrintColor(320+1,270-1,s,0,-32,0);
		CenterPrintColor(320-1,270+1,s,0,-32,0);
		CenterPrintColor(320+1,270+1,s,0,-32,0);
		CenterPrintGlow(320,270,s,0,0);

		CenterPrintGlow(320,304,mod[newMod[0]].name,0,0);

		PrintRectBlack2(107,334,533,360,mod[newMod[0]].desc,14,0);

		if(newMod[1]!=0)
		{
			PauseBox(102,370,538,430,32*1+13);

			CenterPrintGlow(320,374,mod[newMod[1]].name,0,0);

			PrintRectBlack2(107,404,533,430,mod[newMod[1]].desc,14,0);
		}
	}
}

void InitAchieve(byte ach,byte mod1,byte mod2)
{
	newAch=ach;
	newMod[0]=mod1;
	newMod[1]=mod2;

	btnwait=30;
	MakeNormalSound(SND_QUESTDONE);
}

byte UpdateAchieve(MGLDraw *mgl)
{
	byte c;

	if(btnwait)
	{
		btnwait--;
		c=GetTaps();
		mgl->LastKeyPressed();
		return 1;
	}

	c=GetTaps();

	if((c&(CONTROL_B1|CONTROL_B2|CONTROL_B3)))	// pressed a button
	{
		return 0;
	}
	return 1;
}

// --------------- earning achievements

void CheckForNewMods(byte newAch,byte *m)
{
	int i;
	byte vcnt,hcnt;

	vcnt=0;
	hcnt=0;
	for(i=0;i<10;i++)
	{
		if(opt.achieve[(newAch%10)+i*10])
			vcnt++;
		if(opt.achieve[(newAch/10)*10+i])
			hcnt++;
	}
	m[0]=0;
	m[1]=0;
	if(vcnt==10)
	{
		m[0]=modVert[(newAch%10)];
		if(opt.modifier[m[0]])
			m[0]=0;
		opt.modifier[m[0]]=1;
	}
	if(hcnt==10)
	{
		m[1]=modHoriz[(newAch/10)];
		if(opt.modifier[m[1]])
			m[1]=0;
		opt.modifier[m[1]]=1;
	}
	if(m[0]==0 && m[1]!=0)
	{
		m[0]=m[1];
		m[1]=0;
	}
}

void Achieved(byte type,byte val)
{
	int i;
	byte m[2];

	if(type==ACH_SKILL || type==ACH_TALENT)
	{
		for(i=0;i<100;i++)
			if(achievement[i].type==type && achievement[i].amt==val)
			{
				if(opt.achieve[i])
					return;	// already did it!
				else
				{
					opt.achieve[i]=1;
					CheckForNewMods(i,m);
					EnterAchieve(i,m[0],m[1]);
				}
			}
	}
	else	// for ACH_SPECIAL ones, val is just which specific one was achieved
	{
		if(opt.achieve[val])
			return;	// already did it
		else
		{
			opt.achieve[val]=1;
			CheckForNewMods(val,m);
			EnterAchieve(val,m[0],m[1]);
		}
	}
	SaveOptions();
}

void AchieveCheck(void)
{
	int i;

	for(i=1;i<51;i++)
	{
		if(SpecificSkillLevel(i)>=15)
			Achieved(ACH_SKILL,i);
		if(SpecificSkillLevel(i)>=18)
			Achieved(ACH_SPECIAL,27);
	}
	for(i=0;i<30;i++)
		if(player.talentLevel[i]==10)
			Achieved(ACH_TALENT,i);
	for(i=0;i<MAX_EQUIP;i++)
		if(player.items[i].count==255)
			Achieved(ACH_SPECIAL,66);
	if(player.money==MAX_MONEY)
		Achieved(ACH_SPECIAL,88);
}

void AchieveNow(byte n)
{
	Achieved(ACH_SPECIAL,n);
}

// --------------------- achievements menu on title screen

static byte subcursor=0;
static byte subMode;
static byte oldc;
static sprite_set_t *achSpr;
static int glowPos[20];
static char glowDir[20];

void ExitAchieveMenu(byte *backScr)
{
	int i;

	delete achSpr;
	GetDisplayMGL()->LoadBMP(TITLEBMP);
	for(i=0;i<480;i++)
		memcpy(&backScr[i*640],GetDisplayMGL()->GetScreen()+GetDisplayMGL()->GetWidth()*i,640);
}

void InitAchieveMenu(byte *backScr)
{
	int i;
	byte b;

	MakeNormalSound(SND_PAUSE);
	lastKey=0;
	subMode=SUBMODE_OPTIONS;
	subcursor=0;
	oldc=255;
	achSpr=new sprite_set_t("graphics/achspr.jsp");

	for(i=0;i<480;i++)
	{
		b=(byte)((i*32)/480);
		memset(&backScr[i*640],b,640);
	}
	for(i=0;i<20;i++)
	{
		glowPos[i]=-100+rand()%800;
		glowDir[i]=-10+(rand()%21);
		while(glowDir[i]==0)
			glowDir[i]=-10+(rand()%21);
	}
}

void HandleAchieveKeyPresses(MGLDraw *mgl)
{
	char k;

	k=mgl->LastKeyPressed();
	if(k)
		lastKey=k;
}

byte UpdateAchieveMenu(MGLDraw *mgl)
{
	byte c;
	int i;
	static byte reptCounter=0;

	c=GetControls()|GetArrows();

	reptCounter++;
	if((!oldc) || (reptCounter>10))
		reptCounter=0;

	for(i=0;i<20;i++)
	{
		glowPos[i]+=glowDir[i];
		if(glowPos[i]>700 || glowPos[i]<-100)
		{
			glowDir[i]=-10+(rand()%21);
			while(glowDir[i]==0)
				glowDir[i]=-10+(rand()%21);
			if(glowDir[i]<0)
				glowPos[i]=700;
			if(glowDir[i]>0)
				glowPos[i]=-100;
		}
	}
	if(subMode==SUBMODE_OPTIONS)
	{
		if((c&CONTROL_UP) && (!reptCounter))
		{
			subcursor-=10;
			if(subcursor>99)
				subcursor+=100;
		}
		if((c&CONTROL_DN) && (!reptCounter))
		{
			subcursor+=10;
			if(subcursor>99)
				subcursor-=100;
		}
		if((c&CONTROL_LF) && (!reptCounter))
		{
			if((subcursor%10)==0)
				subcursor+=9;
			else
				subcursor--;
		}
		if((c&CONTROL_RT) && (!reptCounter))
		{
			if((subcursor%10)==9)
				subcursor-=9;
			else
				subcursor++;
		}
		if((c&CONTROL_B1) && (!(oldc&CONTROL_B1)))
		{
			return 1;	// exit
		}
		if((c&CONTROL_B2) && (!(oldc&CONTROL_B2)))
		{
			// exit the achievement menu
			return 1;
		}
	}

	oldc=c;

	HandleAchieveKeyPresses(mgl);
	if(lastKey==27)	// hit ESC to exit
	{
		return 1;

		lastKey=0;
	}
	return 0;
}

static void AchieveBadge(int x,int y,byte w,byte cursOn)
{
	if(opt.achieve[w])
	{
		achSpr->GetSprite(1)->Draw(x+32,y+26,GetDisplayMGL());
	}
	else
	{
		achSpr->GetSprite(0+2*cursOn)->Draw(x+32,y+26,GetDisplayMGL());
	}
}

void RenderAchieveMenu(byte *backScr)
{
	int i,j;

	char s[128];

	for(i=0;i<480;i++)
		memcpy(GetDisplayMGL()->GetScreen()+GetDisplayMGL()->GetWidth()*i,&backScr[i*640],640);

	for(i=0;i<10;i++)
	{
		if(opt.modifier[modHoriz[i]])
		{
			GetDisplayMGL()->BrightBox(0,i*40+32+19-4,639,i*40+32+19+4,4);
			GetDisplayMGL()->BrightBox(0,i*40+32+19-3,639,i*40+32+19+3,4);
			GetDisplayMGL()->BrightBox(0,i*40+32+19-2,639,i*40+32+19+2,4);
			GetDisplayMGL()->BrightBox(0,i*40+32+19-1,639,i*40+32+19+1,4);
			GetDisplayMGL()->BrightBox(glowPos[i],i*40+32+19-5,glowPos[i]+10,i*40+32+19+5,1);
			GetDisplayMGL()->BrightBox(glowPos[i]+2,i*40+32+19-5,glowPos[i]+9,i*40+32+19+5,1);
			GetDisplayMGL()->BrightBox(glowPos[i]+4,i*40+32+19-5,glowPos[i]+8,i*40+32+19+5,2);
			GetDisplayMGL()->BrightBox(glowPos[i]+6,i*40+32+19-5,glowPos[i]+7,i*40+32+19+5,3);
		}
		if(opt.modifier[modVert[i]])
		{
			GetDisplayMGL()->BrightBox(i*64+32-4,0,i*64+32+4,479,4);
			GetDisplayMGL()->BrightBox(i*64+32-3,0,i*64+32+3,479,4);
			GetDisplayMGL()->BrightBox(i*64+32-2,0,i*64+32+2,479,4);
			GetDisplayMGL()->BrightBox(i*64+32-1,0,i*64+32+1,479,4);
			GetDisplayMGL()->BrightBox(i*64+32-5,glowPos[i+10],i*64+32+5,glowPos[i+10]+10,1);
			GetDisplayMGL()->BrightBox(i*64+32-5,glowPos[i+10]+2,i*64+32+5,glowPos[i+10]+9,1);
			GetDisplayMGL()->BrightBox(i*64+32-5,glowPos[i+10]+4,i*64+32+5,glowPos[i+10]+8,2);
			GetDisplayMGL()->BrightBox(i*64+32-5,glowPos[i+10]+6,i*64+32+5,glowPos[i+10]+7,3);
		}
	}
	CenterPrintDark(320-1,1-1,"Achievements",0);
	CenterPrintDark(320+1,1-1,"Achievements",0);
	CenterPrintDark(320+1,1+1,"Achievements",0);
	CenterPrintDark(320-1,1+1,"Achievements",0);
	CenterPrintGlow(320,1,"Achievements",0,0);

	j=0;
	for(i=0;i<100;i++)
		if(opt.achieve[i])
			j++;

	sprintf(s,"%d/100",j);
	RightPrintDark(638-1,1-1,s,0);
	RightPrintDark(638+1,1-1,s,0);
	RightPrintDark(638+1,1+1,s,0);
	RightPrintDark(638-1,1+1,s,0);
	RightPrintGlow(638,1,s,0,0);

	for(i=0;i<10;i++)
		for(j=0;j<10;j++)
		{
			AchieveBadge(i*64,32+j*40,i+j*10,(subcursor==(i+j*10)));
		}

	if(opt.achieve[subcursor])
		achSpr->GetSprite(3)->Draw((subcursor%10)*64+32,(subcursor/10)*40+32+26,GetDisplayMGL());

	PauseBox(0,435,639,479,10);

	if(opt.achieve[subcursor])
	{
		PrintGlow(5,440,ConvertText(achievement[subcursor].name),0,1);
		RightPrintGlow(635,440,ConvertText(achievement[subcursor].desc),0,1);

		if(opt.modifier[modHoriz[subcursor/10]] && opt.modifier[modVert[subcursor%10]])
			sprintf(s,"Completing this row and column unlocked Modifiers \"%s\" and \"%s\".",mod[modHoriz[(subcursor/10)]].name,mod[modVert[(subcursor%10)]].name);
		else if(opt.modifier[modVert[subcursor%10]])
			sprintf(s,"Completing this column unlocked Modifier \"%s\".",mod[modVert[(subcursor%10)]].name);
		else if(opt.modifier[modHoriz[subcursor/10]])
			sprintf(s,"Completing this row unlocked Modifier \"%s\".",mod[modHoriz[(subcursor/10)]].name);
		else
			s[0]='\0';

		PrintGlow(5,452,s,0,1);
	}
	else
	{
		PrintGlow(5,440,"Unknown",0,1);
		RightPrintGlow(635,440,"You haven't earned this one yet!",0,1);
	}

	PrintGlow(5,464,"Left, Right, Up and Down move around the chart.  Fire, Jump, or ESC will exit.",0,1);
}

void AchieveMenu(MGLDraw *mgl,byte *backScr)
{
	byte done=0;
	int lastTime;

	InitAchieveMenu(backScr);
	lastTime=1;

	while(!done)
	{
		lastTime+=TimeLength();
		StartClock();
		if(lastTime>TIME_PER_FRAME*30)
			lastTime=TIME_PER_FRAME*30;

		while(lastTime>=TIME_PER_FRAME && !done)
		{
			done=UpdateAchieveMenu(mgl);
			lastTime-=TIME_PER_FRAME;
		}
		RenderAchieveMenu(backScr);
		mgl->Flip();
		EndClock();

		if(!mgl->Process())
			done=1;
	}

	ExitAchieveMenu(backScr);
}

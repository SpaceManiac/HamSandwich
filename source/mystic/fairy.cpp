#include "fairy.h"
#include "display.h"
#include "player.h"
#include "sound.h"
#include "guy.h"

#define FAIRYX 185
#define FAIRYY 70
#define FAIRYDX 70
#define FAIRYDY 65

static sprite_set_t *shopSpr;

static byte shopCursor=0;
static byte *shopScr;

typedef struct fakeFairy_t
{
	int x,y;
	int dx,dy;
	int frm;
	byte facing;
} fakeFairy_t;

static fakeFairy_t fairies[16];

static char fairyName[16][32]=
{
	"Grabby Fairy",
	"Runny Fairy",
	"Casty Fairy",
	"Blocky Fairy",
	"Vampy Fairy",
	"Smashy Fairy",
	"Cheapy Fairy",
	"Toughy Fairy",
	"Richey Fairy",
	"Smarty Fairy",
	"Healey Fairy",
	"Savey Fairy",
	"Shooty Fairy",
	"Haggly Fairy",
	"Scary Fairy",
	"Mighty Fairy",
};

static char fairyDesc[16*5][64]=
	{
		//#define FAIRY_GRABBY	(1)	// picks up coins that touch him, but decreases experience
		"Greedy but generous,",
		"Grabby pulls coins near",
		"and gives them to",
		"you. But you'll also",
		"earn less experience.",

		//#define FAIRY_RUNNY		(2)	// makes you run faster than normal, but take 25% more damage when hit.
		"He will make you able",
		"to run faster, but",
		"you will get hurt a",
		"bit more than usual.",
		"",

		//#define FAIRY_CASTY		(4)	// makes your spells act like they are 5 levels higher than they are, but cost 1 HP per cast as well as the mana
		"Casty helps your spells",
		"become more powerful,",
		"but makes them cost",
		"a tiny bit of life in",
		"addition to magic.",

		//#define FAIRY_BLOCKY	(8)	// take 75% damage when hit, but only inflict 75% damage.
		"With a magical shield,",
		"he'll protect you from",
		"some damage, but the",
		"shield also blocks 1/4",
		"of your fireball power.",

		// chapter II
		// #define FAIRY_VAMPY		(16) // gain 1 HP each time you hit someone with a fireball, but lose 1 HP per 3 seconds
		"This evil fairy sucks",
		"life from monsters",
		"you hit, healing you.",
		"But she'll suck your",
		"life over time too!",

		//#define FAIRY_SMASHY	(32) // inflict 1.5x damage, but each fireball costs 1 MP to shoot (can't fire at all when out, or maybe normal damage?)
		"He imbues your shots",
		"with extra power...",
		"but he gets the magic",
		"to do it from you!",
		"",

		//#define FAIRY_CHEAPY	(64) // spells cost half as much, but you can't fire fireballs
		"What would you pay",
		"to cast spells for only",
		"HALF the usual cost?",
		"$19.95? $29.73? How",
		"about your fireballs?",

		//#define FAIRY_TOUGHY	(128) // take 50% damage, but move half speed
		"Enchants you with",
		"stone magic, making",
		"you twice as tough,",
		"but only able to go",
		"half of normal speed.",

		// chapter III
		//#define FAIRY_RICHEY	(256)	// enemies drop more coins than normal
		"Your enemies will",
		"drop more coins than",
		"usual, but your XP",
		"will slowly drain",
		"over time.",

		//#define FAIRY_SMARTY	(512)	// you earn 25% more XP per kill, but lose mana when hit as well as life (1/2 as much)
		"Level up faster, but",
		"lose some mana",
		"along with life",
		"whenever you get hit.",
		"",

		//#define FAIRY_HEALEY	(1024)	// if your life is below 50%, it recovers slowly
		"If your health is low,",
		"Healey will help you",
		"to recover slowly.",
		"There is a small fee",
		"for this service.",

		//#define FAIRY_SAVEY		(2048)	// if you die, he resurrects you where you died, with 1/4 health and mana, invincible for 1 second (only one resurrect per level)
		"Once per level, Savey",
		"can revive you!",
		"You'll come back with",
		"1/4 life and mana,",
		"and less XP.",

		// chapter IV
		//#define FAIRY_SHOOTY	(4096)	// fires to help you out, when you do (so you need hammers)
		"The most aggressive",
		"fairy of all, Shooty",
		"will back you up with",
		"extra firepower when",
		"you have fireballs.",

		//#define FAIRY_HAGGLY	(8192)	// you get 10% off at the shop
		"With his powers of",
		"persuasion, Haggly",
		"saves you money in",
		"the Shop.",
		"",

		//#define FAIRY_SCARY		(16384)	// makes monsters run away from you in the overworld if you get near
		"Thanks to a terrifying",
		"mask, Scary keeps the",
		"monsters at bay in",
		"the overworld.",
		"",

		//#define FAIRY_MIGHTY	(32768)	// your life and mana drain semi-slowly, but you move 25% faster, and do 25% more damage, and take 25% less damage
		"He'll grant you great",
		"power, but keeping it",
		"up requires a drain on",
		"your life and magic.",
		"",
	};

static byte oldc=0;

void InitFairyBox(void)
{
	int i;
	shopSpr=new sprite_set_t("graphics/shop.jsp");

	for(i=0;i<16;i++)
	{
		fairies[i].x=-8*FIXAMT+MGL_randoml(16*FIXAMT+1);
		fairies[i].y=-8*FIXAMT+MGL_randoml(16*FIXAMT+1);
		fairies[i].dx=-FIXAMT+MGL_randoml(FIXAMT*2+1);
		fairies[i].dy=-FIXAMT+MGL_randoml(FIXAMT*2+1);
		fairies[i].facing=2;
		fairies[i].frm=MGL_randoml(9*FIXAMT);
	}
}

void ExitFairyBox(void)
{
	delete shopSpr;
}

sprite_t *GetFairySpr(byte spr)
{
	return shopSpr->GetSprite(spr);
}

void EnterFairyBox(void)
{
	int i;

	for(i=0;i<16;i++)
		if((1<<i)==player.fairyOn)
			shopCursor=i;

	InitFairyBox();
	oldc=255;
	PlaySong(SONG_SHOP);
	shopScr=(byte *)malloc(640*480);
	if(!shopScr)
		FatalError("Out of memory!!");
	shopSpr->GetSprite(36)->Draw(278,200,GetDisplayMGL());
	memcpy(shopScr,GetDisplayMGL()->GetScreen(),640*480);
}

void LeaveFairyBox(void)
{
	free(shopScr);
	ExitFairyBox();
}

void FairyCaughtText(byte w)
{
	char s[64];

	sprintf(s,"You caught %s!",fairyName[w-1]);
	NewMessage(s,60);
}

void RenderFairyBox(void)
{
	int i;
	int j,k;
	MGLDraw *mgl;
	word w;

	mgl=GetDisplayMGL();

	memcpy(mgl->GetScreen(),shopScr,640*480);

	w=1;
	for(j=0;j<4;j++)
		for(i=0;i<4;i++)
		{
			shopSpr->GetSprite(1-((player.haveFairy&w)>0))->Draw((i)*FAIRYDX+FAIRYX,FAIRYY+j*FAIRYDY,mgl);
			// render the fairy itself, if you have it
			if(player.haveFairy&w)
				InstaRenderMonster2(i*FAIRYDX+FAIRYX+25+(fairies[i+j*4].x/FIXAMT),FAIRYY+j*FAIRYDY+40+(fairies[i+j*4].y/FIXAMT),MONS_FAIRY,0,(fairies[i+j*4].frm/FIXAMT),fairies[i].facing,(i+j*4+1),0+8*(shopCursor==i+j*4),mgl);
			else
			{
				for(k=0;k<4;k++)
					if(player.chaseFairy[k]==i+j*4+1)
						shopSpr->GetSprite(0)->Draw((i)*FAIRYDX+FAIRYX,FAIRYY+j*FAIRYDY,mgl);	// make it gold
			}
			w*=2;
		}

	// cursor
	shopSpr->GetSprite(2)->Draw((shopCursor-(shopCursor/4*4))*FAIRYDX+FAIRYX,(shopCursor/4)*FAIRYDY+FAIRYY,mgl);

	w=1<<shopCursor;

	if(player.haveFairy&w)
	{
		CenterPrint(320,318,fairyName[(shopCursor)],0,2);

		CenterPrintGlow(343,fairyDesc[shopCursor*5],2);
		CenterPrintGlow(343+18,fairyDesc[shopCursor*5+1],2);
		CenterPrintGlow(343+36,fairyDesc[shopCursor*5+2],2);
		CenterPrintGlow(343+54,fairyDesc[shopCursor*5+3],2);
		CenterPrintGlow(343+72,fairyDesc[shopCursor*5+4],2);

		//PrintBrightGlow(FAIRYX-3,343,fairyDesc[shopCursor*5],-4,2);
		//PrintBrightGlow(FAIRYX-3,343+18,fairyDesc[shopCursor*5+1],-4,2);
		//PrintBrightGlow(FAIRYX-3,343+36,fairyDesc[shopCursor*5+2],-4,2);
		//PrintBrightGlow(FAIRYX-3,343+54,fairyDesc[shopCursor*5+3],-4,2);
		//PrintBrightGlow(FAIRYX-3,343+72,fairyDesc[shopCursor*5+4],-4,2);
	}
	else
	{
		CenterPrint(320,318,"???",0,2);
	}
	CenterPrint(320,23,"Fairy Box",0,0);
}

byte UpdateFairyBox(MGLDraw *mgl)
{
	byte c;
	static byte reptCounter=0;
	int i;

	for(i=0;i<16;i++)
	{
		fairies[i].x+=fairies[i].dx;
		fairies[i].y+=fairies[i].dy;
		if(fairies[i].x<0)
			fairies[i].dx+=FIXAMT/8+MGL_random(FIXAMT/32);
		else
			fairies[i].dx-=(FIXAMT/8+MGL_random(FIXAMT/32));
		if(fairies[i].y<0)
			fairies[i].dy+=FIXAMT/8+MGL_random(FIXAMT/32);
		else
			fairies[i].dy-=(FIXAMT/8+MGL_random(FIXAMT/32));
		if(fairies[i].dx<-FIXAMT/2)
			fairies[i].facing=3;
		else if(fairies[i].dx>FIXAMT/2)
			fairies[i].facing=1;
		else
			fairies[i].facing=2;

		fairies[i].frm+=FIXAMT/4;
		if(fairies[i].frm>=FIXAMT*10)
			fairies[i].frm-=FIXAMT*10;
	}

	c=GetControls();

	reptCounter++;
	if((!oldc) || (reptCounter>10))
		reptCounter=0;

	if((c&CONTROL_UP) && (!reptCounter))
	{
		MakeNormalSound(SND_MENUCLICK);
		shopCursor-=4;
		if(shopCursor>=16)
			shopCursor+=16;
	}
	if((c&CONTROL_DN) && (!reptCounter))
	{
		MakeNormalSound(SND_MENUCLICK);
		shopCursor+=4;
		if(shopCursor>=16)
			shopCursor-=16;
	}
	if((c&CONTROL_LF) && (!reptCounter))
	{
		MakeNormalSound(SND_MENUCLICK);
		shopCursor--;
		if((shopCursor%4)==3)
			shopCursor+=4;
		if(shopCursor>50)
			shopCursor=3;
	}
	if((c&CONTROL_RT) && (!reptCounter))
	{
		MakeNormalSound(SND_MENUCLICK);
		shopCursor++;
		if((shopCursor%4)==0)
			shopCursor-=4;
	}
	if((c&CONTROL_B1) && (!(oldc&CONTROL_B1)))
	{
		if(player.haveFairy&(1<<shopCursor))
		{
			MakeNormalSound(SND_MENUCLICK);
			FairyChange(player.fairyOn,(1<<shopCursor));
		}
		else
			FairyChange(player.fairyOn,0);

		return 1;
	}
	if((c&CONTROL_B2) && (!(oldc&CONTROL_B2)))
	{
		FairyChange(player.fairyOn,0);
		return 1;
	}
	oldc=c;

	JamulSoundUpdate();

	if(mgl->LastKeyPressed()==27)
	{
		FairyChange(player.fairyOn,0);
		return 1;
	}
	else return 0;
}

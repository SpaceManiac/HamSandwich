#include "scanner.h"
#include "editor.h"
#include "monsnotes.h"
#include "dialogbits.h"
#include "goal.h"
#include "cards.h"

dword scanQueue[MAX_SCAN];
dword scanQPos,firstScan;
byte noKeyTime;
byte scanStarted;
byte *backgd;
char groupTxt[128],nameTxt[64],cardTxt[64],rarityTxt[64],originTxt[64];
char themeNames[][16]={"Goodguy","Badguy","Forest","Desert","Icy","Caves","Space",
					   "Mansion","Island","Urban","Aquatic","Asylum","Traps/Puzzles","Humanoid","Animals",
					   "Veggies","Pumpkins","Zombies","Skeletons","Vampires","Zoids",
					   "Aliens/Thingies","Generators","Vehicles","Flying","Aquatic", "Spiders",
					   "Bits","Bosses","Minibosses","Wacky!"};
static int monsY=0;

void InitScan(void)
{
	scanQPos=0;
	firstScan=0;
	scanStarted=0;
}

void BeginScanning(void)
{
	int i;
	dword d;
	byte firstTheme;

	if(!scanStarted)
	{
		GetDisplayMGL()->LoadBMP("graphics/profmenu.bmp", NULL);
		backgd=(byte *)malloc(640*480);
		for(i=0;i<480;i++)
			memcpy(&backgd[i*640],&GetDisplayMGL()->GetScreen()[i*GetDisplayMGL()->GetWidth()],640);

		scanStarted=1;
	}

	if(MonsterFlags(scanQueue[firstScan],scanQueue[firstScan])&(MF_INVINCIBLE|MF_NOHIT))
		sprintf(nameTxt,"%s    // Life: N/A",MonsterName(scanQueue[firstScan]));
	else
		sprintf(nameTxt,"%s    // Life: %d",MonsterName(scanQueue[firstScan]),MonsterHP(scanQueue[firstScan]));

	strcpy(groupTxt,"Class: ");
	d=1;
	firstTheme=1;
	for(i=0;i<28;i++)
	{
		if(MonsterTheme(scanQueue[firstScan])&d)
		{
			if(!firstTheme)
				strcat(groupTxt,"/");
			strcat(groupTxt,themeNames[i]);
			firstTheme=0;
		}
		d*=2;
	}
}

void BeginCarding(void)
{
	int i;
	dword d;
	byte firstTheme;
	dword ch;
	const char *orgn;
	
	ch = scanQueue[firstScan];

	if(!scanStarted)
	{
		GetDisplayMGL()->LoadBMP("graphics/galaxy.bmp", NULL);
		backgd=(byte *)malloc(640*480);
		for(i=0;i<480;i++)
			memcpy(&backgd[i*640],&GetDisplayMGL()->GetScreen()[i*GetDisplayMGL()->GetWidth()],640);

		scanStarted=1;
	}
	
	if(ch<60)
		orgn="Dr. Lunatic";
	else if(ch<91)
		orgn="Expando Pack";
	else if(ch<131)
		orgn="Fun Pack";
	else if(ch<211)
		orgn="Supreme with Cheese";
	else if(ch<248)
		orgn="Kid Mystic";
	else if(ch<289 && ch!=249)
		orgn="Sleepless Hollow";
	else if(ch<341)
		orgn="Loonyland: Halloween Hill";
	else if(ch>412 && ch<488)
		orgn="Loonyland II: Winter Woods";
	else
		orgn="Operation S.C.A.R.E.";
	
	sprintf(nameTxt,"%s",MonsterName(ch));
	sprintf(rarityTxt,"No.%i  /  Rarity %i",GetMonsterCard(ch),GetMonsterCardRarity(ch)+1);
	sprintf(originTxt,"From %s",orgn);
	sprintf(cardTxt,"Cards: %i",profile.progress.carded[ch]);

	strcpy(groupTxt,"Class: ");
	d=1;
	firstTheme=1;
	for(i=0;i<28;i++)
	{
		if(MonsterTheme(scanQueue[firstScan])&d)
		{
			if(!firstTheme)
				strcat(groupTxt,"/");
			strcat(groupTxt,themeNames[i]);
			firstTheme=0;
		}
		d*=2;
	}
}

void EndScanning(void)
{
	if(scanStarted)
	{
		scanStarted=0;
		free(backgd);
	}
}

void ScanGuy(Guy *me, Guy *g)
{
	dword type;
	Guy *g2;

	g2=g;
	if(g2->aiType!=MONS_DJINNI)
	{
		while(g2->parent)
			g2=g2->parent;
	}

	type=g2->type;

	if(!profile.progress.scanned[g->type] && !editing)
	{
		// show the scanner info for this monster
		SendMessageToGame(MSG_SCANMONSTER,0);
		profile.progress.scanned[g->type]=1;
		switch(g->type)
		{
			case MONS_ROLLER:
			case MONS_ROLLER2:
				profile.progress.scanned[MONS_ROLLER]=1;
				profile.progress.scanned[MONS_ROLLER2]=1;
				break;
			case MONS_ROLLER3:
			case MONS_ROLLER4:
				profile.progress.scanned[MONS_ROLLER3]=1;
				profile.progress.scanned[MONS_ROLLER4]=1;
				break;
			case MONS_SNOWBALL:
			case MONS_SNOWBALL2:
				profile.progress.scanned[MONS_SNOWBALL]=1;
				profile.progress.scanned[MONS_SNOWBALL2]=1;
				break;
			case MONS_FRIENDLY:
			case MONS_FRIENDLY2:
			case MONS_FOLLOWBUNNY:
				profile.progress.scanned[MONS_FRIENDLY]=1;
				profile.progress.scanned[MONS_FRIENDLY2]=1;
				profile.progress.scanned[MONS_FOLLOWBUNNY]=1;
				break;
			case MONS_PATCH:
			case MONS_PATCH2:
				profile.progress.scanned[MONS_PATCH]=1;
				profile.progress.scanned[MONS_PATCH2]=1;
				break;
			case MONS_PARKEDCAR:
			case MONS_PARKED2:
				profile.progress.scanned[MONS_PARKEDCAR]=1;
				profile.progress.scanned[MONS_PARKED2]=1;
				break;
			case MONS_PATROLLR:
			case MONS_PATROLUD:
				profile.progress.scanned[MONS_PATROLLR]=1;
				profile.progress.scanned[MONS_PATROLUD]=1;
				break;
			case MONS_DPATROLLR:
			case MONS_DPATROLUD:
				profile.progress.scanned[MONS_DPATROLLR]=1;
				profile.progress.scanned[MONS_DPATROLUD]=1;
				break;
			case MONS_MATHEAD:
			case MONS_MATSKULL:
			case MONS_MATBRAIN:
			case MONS_MATBODY:
			case MONS_MATCLAW1:
			case MONS_MATCLAW2:
			case MONS_MATTAIL:
				profile.progress.scanned[MONS_MATHEAD]=1;
				profile.progress.scanned[MONS_MATSKULL]=1;
				profile.progress.scanned[MONS_MATBRAIN]=1;
				profile.progress.scanned[MONS_MATBODY]=1;
				profile.progress.scanned[MONS_MATCLAW1]=1;
				profile.progress.scanned[MONS_MATCLAW2]=1;
				profile.progress.scanned[MONS_MATTAIL]=1;
				break;
			case MONS_MAT2HEAD:
			case MONS_MAT2SKULL:
			case MONS_MAT2BRAIN:
			case MONS_MAT2BODY:
			case MONS_MAT2TAIL:
				profile.progress.scanned[MONS_MAT2HEAD]=1;
				profile.progress.scanned[MONS_MAT2SKULL]=1;
				profile.progress.scanned[MONS_MAT2BRAIN]=1;
				profile.progress.scanned[MONS_MAT2BODY]=1;
				profile.progress.scanned[MONS_MAT2TAIL]=1;
				break;
			case MONS_THING:
			case MONS_THINGTENT:
			case MONS_THINGTENTTIP:
				profile.progress.scanned[MONS_THING]=1;
				profile.progress.scanned[MONS_THINGTENT]=1;
				profile.progress.scanned[MONS_THINGTENTTIP]=1;
				break;
			case MONS_CENTIBODY:
			case MONS_CENTIHEAD:
				profile.progress.scanned[MONS_CENTIBODY]=1;
				profile.progress.scanned[MONS_CENTIHEAD]=1;
				break;
				break;
			case MONS_CENTIBBODY:
			case MONS_CENTIBHEAD:
				profile.progress.scanned[MONS_CENTIBBODY]=1;
				profile.progress.scanned[MONS_CENTIBHEAD]=1;
				break;
			case MONS_SPHINX:
			case MONS_SPHXARM1:
			case MONS_SPHXARM2:
				profile.progress.scanned[MONS_SPHINX]=1;
				profile.progress.scanned[MONS_SPHXARM1]=1;
				profile.progress.scanned[MONS_SPHXARM2]=1;
				break;
			case MONS_LOONYBOT:
			case MONS_LOONYCORE:
			case MONS_LOONYGUN:
				profile.progress.scanned[MONS_LOONYBOT]=1;
				profile.progress.scanned[MONS_LOONYCORE]=1;
				profile.progress.scanned[MONS_LOONYGUN]=1;
				break;
			case MONS_GENERATOR1:
			case MONS_GENERATOR2:
			case MONS_GENERATOR3:
			case MONS_GENERATOR4:
				profile.progress.scanned[MONS_GENERATOR1]=1;
				profile.progress.scanned[MONS_GENERATOR2]=1;
				profile.progress.scanned[MONS_GENERATOR3]=1;
				profile.progress.scanned[MONS_GENERATOR4]=1;
				break;
			case MONS_PUFFYFISH:
			case MONS_PUFFYFISH2:
				profile.progress.scanned[MONS_PUFFYFISH]=1;
				profile.progress.scanned[MONS_PUFFYFISH2]=1;
				break;
			case MONS_MINIMATTIE:
			case MONS_MINIMATBODY:
			case MONS_MINIMATCLAW1:
			case MONS_MINIMATCLAW2:
			case MONS_MINIMATTAIL:
				profile.progress.scanned[MONS_MINIMATTIE]=1;
				profile.progress.scanned[MONS_MINIMATBODY]=1;
				profile.progress.scanned[MONS_MINIMATCLAW1]=1;
				profile.progress.scanned[MONS_MINIMATCLAW2]=1;
				profile.progress.scanned[MONS_MINIMATTAIL]=1;
				break;
		}
		scanQueue[scanQPos]=type;
		scanQPos++;
		if(scanQPos==MAX_SCAN)
			scanQPos=0;
		noKeyTime=30;
		GetTaps();
	}

	// now fire at the victim, if it's evil
	if(g->friendly!=goodguy->friendly)
		FireScanShots(me, g);
}

byte UpdateScan(MGLDraw *mgl)
{
	int i,total;

	if(!scanStarted)
		BeginScanning();

	monsY-=4;

	if(noKeyTime)
	{
		noKeyTime--;
		GetTaps();
		mgl->LastKeyPressed();
		mgl->MouseTap();
		return 1;	// can't quit scanning in the first second
	}

	if((GetTaps()&(CONTROL_B1|CONTROL_B2)) || (mgl->LastKeyPressed()) || (mgl->MouseTap()))
	{
		firstScan++;
		if(firstScan==MAX_SCAN)
			firstScan=0;
		if(firstScan==scanQPos)
		{
			EndScanning();

			total=NUM_MONSTERS;
			for(i=0;i<NUM_MONSTERS;i++)
			{
				if(profile.progress.scanned[i] || !MonsterTheme(i))
					total--;
			}
			if(!total)
				CompleteGoal(85);

			return 0;	// done scanning!
		}
		noKeyTime=30;
		BeginScanning();
	}

	return 1;
}

byte UpdateCard(MGLDraw *mgl)
{
	int i,total;

	if(!scanStarted)
		BeginCarding();

	monsY-=4;

	if(noKeyTime)
	{
		noKeyTime--;
		GetTaps();
		mgl->LastKeyPressed();
		mgl->MouseTap();
		return 1;	// can't quit scanning in the first second
	}

	if((GetTaps()&(CONTROL_B1|CONTROL_B2)) || (mgl->LastKeyPressed()) || (mgl->MouseTap()))
	{
		firstScan++;
		if(firstScan==MAX_SCAN)
			firstScan=0;
		if(firstScan==scanQPos)
		{
			EndScanning();

			total=NUM_MONSTERS;
			for(i=0;i<NUM_MONSTERS;i++)
			{
				if(profile.progress.scanned[i] || !MonsterTheme(i))
					total--;
			}
			if(!total)
				CompleteGoal(85);

			return 0;	// done scanning!
		}
		noKeyTime=30;
		BeginCarding();
	}

	return 1;
}

void RenderScan(MGLDraw *mgl)
{
	int i,add,b;

	if(scanStarted)
	{
		for(i=0;i<480;i++)
			memcpy(&GetDisplayMGL()->GetScreen()[i*GetDisplayMGL()->GetWidth()],&backgd[i*640],640);
	}

	SetSpriteConstraints(10,10,629,469);

	add=InstaRenderScannedMonster(510,-500,scanQueue[firstScan],0,mgl)+15;

	if(monsY<-add)
	{
		while(monsY<480+add)
			monsY+=add;
	}

	i=monsY;
	while(i>-add)
		i-=add;
	while(1)
	{
		b=abs(i-240)/20;
		if(b>32)
			b=32;
		InstaRenderScannedMonster(510,i,scanQueue[firstScan],(char)-b,mgl);
		i+=add;
		if(i>480+add)
			break;
	}
	SetSpriteConstraints(0,0,639,479);

	PrintGlowRect(20,20,620,60,20,groupTxt,2);
	PrintGlow(20,80,nameTxt,0,2);
	PrintGlowRect(20,120,400,460,20,MonsterNotes(scanQueue[firstScan]),2);
}

void RenderCard(MGLDraw *mgl)
{
	int i,add,b;
	
	if(scanStarted)
	{
		for(i=0;i<480;i++)
			memcpy(&GetDisplayMGL()->GetScreen()[i*GetDisplayMGL()->GetWidth()],&backgd[i*640],640);
	}

	SetSpriteConstraints(10,10,639,479);

	add=InstaRenderMonsterAnimated(510,-500,scanQueue[firstScan],0,mgl)+15;

	if(monsY<-add)
	{
		while(monsY<480+add)
			monsY+=add;
	}

	i=monsY;
	while(i>-add)
		i-=add;
	while(1)
	{
		b=abs(i-240)/20;
		if(b>32)
			b=32;
		InstaRenderMonsterAnimated(510,i,scanQueue[firstScan],(char)-b,mgl);
		i+=add;
		if(i>480+add)
			break;
	}
	SetSpriteConstraints(0,0,639,479);

	PrintRect(20,420,620,460,20,groupTxt,3);
	Print(20,80,nameTxt,0,4);
	Print(20,116,rarityTxt,0,3);
	Print(20,138,originTxt,0,3);
	Print(20,160,cardTxt,0,3);
	//PrintRect(20,120,400,460,20,MonsterNotes(scanQueue[firstScan]),3);
}

//-----------------------------------------------

static dword cursor;
static byte numScanned[NUM_MONSTHEMES],totalToScan[NUM_MONSTHEMES];
static int totalTotal,totalScanned;
static dword monsList[180],monsListLen,monsListPos,curMons,curTheme;
static sprite_set_t *bestSpr;
static int msx,msy,oldmsx,oldmsy;
static int msBright,msDBright;

void InitBestiary(MGLDraw *mgl)
{
	int i,j;
	dword d;

	msBright=0;
	msDBright=1;

	mgl->LoadBMP("graphics/profmenu.bmp");
	backgd=(byte *)malloc(640*480);
	for(i=0;i<480;i++)
		memcpy(&backgd[i*640],&mgl->GetScreen()[i*mgl->GetWidth()],640);

	for(i=0;i<NUM_MONSTHEMES;i++)
	{
		numScanned[i]=0;
		totalToScan[i]=0;
	}
	totalScanned=0;
	totalTotal=0;

	monsListLen=0;
	for(i=1;i<NUM_MONSTERS;i++)
	{
		d=1;

		if(MonsterTheme(i))	// don't count ones with no theme
		{
			if(profile.progress.scanned[i])
				totalScanned++;
			totalTotal++;
		}
		for(j=0;j<NUM_MONSTHEMES;j++)
		{
			if(MonsterTheme(i)&d)
			{
				if(profile.progress.scanned[i])
					numScanned[j]++;
				totalToScan[j]++;
			}
			d<<=1;
		}

		if(MonsterTheme(i)&MT_GOOD)
		{
			monsList[monsListLen++]=i;
		}
	}

	monsListPos=0;
	cursor=0;
	curMons=MONS_BOUAPHA;
	curTheme=0;

	bestSpr=new sprite_set_t("graphics/pause.jsp");
	GetTaps();
	GetArrowTaps();
	mgl->LastKeyPressed();
	mgl->GetMouse(&oldmsx,&oldmsy);
	mgl->MouseTap();
	PurgeMonsterSprites();
}

void ExitBestiary(void)
{
	free(backgd);
	delete bestSpr;
	PurgeMonsterSprites();
}

void BestiarySelectTheme(dword c)
{
	dword d;
	int i,flip;

	monsListLen=0;
	monsListPos=0;

	d=1;
	for(i=0;i<c;i++)
		d*=2;

	for(i=1;i<NUM_MONSTERS;i++)
	{
		if(MonsterTheme(i)&d)
		{
			monsList[monsListLen++]=i;
		}
	}

	// sort the monsters
	flip=1;
	while(flip)
	{
		flip=0;
		for(i=0;i<monsListLen-1;i++)
		{
			if(strcmp(MonsterName(monsList[i]),MonsterName(monsList[i+1]))>0)
			{
				// need to swap
				flip=1;
				d=monsList[i];
				monsList[i]=monsList[i+1];
				monsList[i+1]=d;
			}
		}
	}
	if(monsListLen>12)
		monsList[monsListLen++]=255;

	curTheme=c;
	MakeNormalSound(SND_MENUSELECT);
}

void BestiarySelectMonster(dword c)
{
	if((c==11 && monsListPos+c+1<monsListLen) || (monsList[c+monsListPos]==255))
	{
		// more monsters
		monsListPos+=11;
		if(cursor-100+monsListPos>=monsListLen)
			cursor=100;
		MakeNormalSound(SND_MENUSELECT);
		if(monsListPos>=monsListLen)
			monsListPos=0;
	}
	else
	{
		if(profile.progress.scanned[monsList[c+monsListPos]])
		{
			MakeNormalSound(SND_MENUSELECT);
			curMons=monsList[c+monsListPos];
		}
		else
			MakeNormalSound(SND_TURRETBZZT);
		PurgeMonsterSprites();
	}
}

byte UpdateBestiary(int *lastTime,MGLDraw *mgl)
{
	dword c;

	if(*lastTime>TIME_PER_FRAME*5)
		*lastTime=TIME_PER_FRAME*5;

	while(*lastTime>=TIME_PER_FRAME)
	{
		mgl->Process();
		monsY-=2;
		msBright+=msDBright;
		if(msBright>10)
			msDBright=-1;
		if(msBright<-2)
			msDBright=1;

		*lastTime-=TIME_PER_FRAME;
	}

	mgl->GetMouse(&msx,&msy);
	if(mgl->LastKeyPressed()==27)
		return 1;

	c=GetTaps()|GetArrowTaps();

	if(c&CONTROL_UP)
	{
		if(cursor<100)
		{
			cursor-=4;
			if(cursor>100)
			{
				cursor=100+11;
				if(cursor-100>(monsListLen-monsListPos))
					cursor=monsListLen-monsListPos+100;
			}
		}
		else
		{
			cursor--;
			if(cursor==99)
				cursor=NUM_MONSTHEMES-4;
		}
	}
	if(c&CONTROL_DN)
	{
		if(cursor<100)
		{
			cursor+=4;
			if(cursor>=NUM_MONSTHEMES)
				cursor=100;
		}
		else
		{
			cursor++;
			if(cursor-100>(monsListLen-monsListPos) || cursor-100>11)
					cursor=0;
		}
	}
	if(c&CONTROL_LF)
	{
		if(cursor<100)
		{
			cursor--;
			if((cursor&3)==3)
				cursor+=4;
		}
	}
	if(c&CONTROL_RT)
	{
		if(cursor<100)
		{
			cursor++;
			if((cursor&3)==0)
				cursor-=4;
		}
	}

	if(mgl->MouseTap() || (c&CONTROL_B1))
	{
		if(cursor<100)
		{
			BestiarySelectTheme(cursor);
		}
		else
			BestiarySelectMonster(cursor-100);
	}

	return 0;
}

void RenderBestiary(MGLDraw *mgl)
{
	int i;
	int x,y;
	char txt[32];
	int add,b;
	int msx2,msy2;

	for(i=0;i<480;i++)
		memcpy(&GetDisplayMGL()->GetScreen()[i*GetDisplayMGL()->GetWidth()],&backgd[i*640],640);

	x=21;
	y=14;
	for(i=0;i<NUM_MONSTHEMES;i++)
	{
		if(oldmsx!=msx || oldmsy!=msy)
		{
			if(PointInRect(msx,msy,x,y,x+145,y+18))
				cursor=i;
		}
		sprintf(txt,"%d%%",numScanned[i]*100/totalToScan[i]);
		if(cursor==i)
		{
			mgl->FillBox(x,y,x+145,y+18,32*1+8);
			mgl->Box(x,y,x+145,y+18,31);
			mgl->Box(x-1,y-1,x+146,y+19,31);
		}
		else
		{
			if(curTheme==i)
				mgl->FillBox(x,y,x+145,y+18,32*1+8);
			mgl->Box(x,y,x+145,y+18,32*1+16);
		}

		PrintGlowLimited(x+2,y+2,x+143,themeNames[i],0,2);
		PrintGlowLimited(x+143-GetStrLength(txt,2),y+2,x+143,txt,0,2);

		x+=150;
		if(x>629-145)
		{
			x=21;
			y+=24;
		}
	}
	mgl->FillBox(14,203,640-14,203,32*1+16);

	sprintf(txt,"%d%% Scanned",totalScanned*100/totalTotal);
	PrintGlow(600-GetStrLength(txt,2),184,txt,0,2);
	x=21;
	y=207;

	for(i=0;i<12;i++)
	{
		if(oldmsx!=msx || oldmsy!=msy)
		{
			if(PointInRect(msx,msy,x,y,x+145,y+18))
				cursor=i+100;
		}

		if(cursor==i+100)
		{
			mgl->FillBox(x,y,x+145,y+18,32*1+8);
			mgl->Box(x,y,x+145,y+18,31);
			mgl->Box(x-1,y-1,x+146,y+19,31);
		}
		else
		{
			if(curMons==monsList[i+monsListPos])
				mgl->FillBox(x,y,x+145,y+18,32*1+8);
			mgl->Box(x,y,x+145,y+18,32*1+16);
		}

		if((i==11 && i+monsListPos+1<monsListLen) || (monsList[i+monsListPos]==255))
			strcpy(txt,"More Monsters...");
		else if(profile.progress.scanned[monsList[i+monsListPos]])
			strcpy(txt,MonsterName(monsList[i+monsListPos]));
		else
			strcpy(txt,"? ? ? ? ? ?");
		PrintGlowLimited(x+2,y+2,x+143,txt,0,2);
		y+=21;
		if(i+monsListPos>=monsListLen-1)
			break;
	}

	SetSpriteConstraints(148,208,629,469);

	add=InstaRenderScannedMonster(570,-500,curMons,0,mgl)+15;

	if(monsY<-add)
	{
		while(monsY<480+add)
			monsY+=add;
	}

	i=monsY;
	while(i>-add)
		i-=add;
	while(1)
	{
		b=abs(i-360)/20;
		if(b>32)
			b=32;
		InstaRenderScannedMonster(570,i,curMons,(char)-b,mgl);
		i+=add;
		if(i>480+add)
			break;
	}
	SetSpriteConstraints(0,0,639,479);

	PrintGlowRect(170,208,500,453,16,MonsterNotes(curMons),2);

	sprintf(txt,"Defeated: %d",profile.progress.kills[curMons]);
	PrintGlow(170,455,txt,0,2);

	if(MonsterFlags(curMons,curMons)&(MF_NOHIT|MF_INVINCIBLE))
		strcpy(txt,"Life: N/A");
	else
		sprintf(txt,"Life: %d",MonsterHP(curMons));
	PrintGlow(500-GetStrLength(txt,2),455,txt,0,2);

	SetSpriteConstraints(13,13,627,467);
	msx2=msx;
	msy2=msy;
	if(msx2<13)
		msx2=13;
	if(msy2<13)
		msy2=13;
	if(msx2>622)
		msx2=622;
	if(msy2>462)
		msy2=462;
	bestSpr->GetSprite(0)->DrawBright(msx2,msy2,mgl,msBright/2);
	SetSpriteConstraints(0,0,639,479);

	oldmsx=msx;
	oldmsy=msy;
}

void Bestiary(MGLDraw *mgl)
{
	byte done=0;
	int lastTime=1;

	InitBestiary(mgl);
	BestiarySelectTheme(0);
	GetTaps();
	while(!done)
	{
		lastTime+=TimeLength();
		StartClock();

		done=UpdateBestiary(&lastTime,mgl);
		RenderBestiary(mgl);
		mgl->Flip();

		if(!mgl->Process())
			done=1;
		EndClock();
	}

	ExitBestiary();
}

float ScanPercent(void)
{
	int i;
	int scanned;

	scanned=0;

	for(i=0;i<NUM_MONSTERS;i++)
	{
		if(MonsterTheme(i))	// don't count ones with no theme
		{
			if(profile.progress.scanned[i])
				scanned++;
		}
		else
			scanned++;	// ones with no theme are always scanned
	}

	return((float)scanned*100.0f/(float)NUM_MONSTERS);
}

//This is for cards
void GetGuyCard(Guy *g)
{
	dword type1;
	Guy *g2;
	int chance;
	int rand;
	rand = Random(1000);

	g2=g;
	if(g2->aiType!=MONS_DJINNI)
	{
		while(g2->parent)
			g2=g2->parent;
	}

	type1=g2->type;
	
	chance = GetMonsterCardChance(g->type);
	if(chance>=rand && !editing)
	{
		MakeNormalSound(SND_ALLCANDLE);
		// show the scanner info for this monster
		SendMessageToGame(MSG_MONSTERCARD,0);
		profile.progress.carded[g2->type]+=1;
		scanQueue[scanQPos]=type1;
		scanQPos++;
		if(scanQPos==MAX_SCAN)
			scanQPos=0;
		noKeyTime=30;
		GetTaps();
	}
}

float CardPercent(void)
{
	int i;
	int scanned;

	scanned=0;

	for(i=0;i<NUM_MONSTERS;i++)
	{
		if(MonsterTheme(i))	// don't count ones with no theme
		{
			if(profile.progress.carded[i]>0)
				scanned++;
		}
		else
			scanned++;	// ones with no theme are always scanned
	}

	return((float)scanned*100.0f/(float)NUM_MONSTERS);
}
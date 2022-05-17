#include "profmenu.h"
#include "profile.h"
#include "options.h"
#include "clock.h"
#include "game.h"
#include "title.h"
#include "music.h"

#define SHEET_OFSX 0
#define SHEET_OFSY 0

#define PE_LILGUY	0
#define PE_BIGGUY	1
#define PE_SHEET	2

#define NUMLILGUYS	10

#define CURBIGGUY	(NUMLILGUYS)
#define OTHERBIGGUY	(NUMLILGUYS+1)
#define CURSHEET	(NUMLILGUYS+2)
#define OTHERSHEET	(NUMLILGUYS+3)
#define NUM_PROFELEM (NUMLILGUYS+4)

// the gift shop numbers of each character (surfboy being 255)
static byte giftShopNum[]={
		255,
		5,
		7,
		14,
		8,
		6,
		9,
		4,
		11,
		255
	};

typedef struct profElem_t
{
	byte type;
	byte number;
	int x,y;
	int dx,dy;
	int tx,ty;
} profElem_t;

static byte cursor;
static byte oldc;
static dword oldBtn;
static byte controlX,controlY;
static byte prfMode;
static sprite_set_t *profSpr;
static byte flash=0;
static int clipY;
static byte repeatClock[5];
static byte yesno;
static byte oldMouseBtn;
static byte makeSound=0;

static int clipChoices[]={-55,95,140,155,170,185,200,235,260,285,310,335,355};

profile_t prof[8];
profElem_t profElem[16];

// char names: surfboy, gothgirl, robot, happy stick, santa, cherub, shroom, pig, freako
byte charAges[NUMLILGUYS]={15,13,1,5,200,244,1,19,3,12};
char dispositions[NUMLILGUYS][16]={"Gnarly","Surly","Calculating","Happy","Jolly","Sweet",
								   "Squishy","Pig-Headed","Freakish","Peaceful"};
char height[NUMLILGUYS][8]={"3\'5\"","4\'0\"","3\'3\"","4\'2\"","4\'6\"","3\'1\"",
							"1\'2\"","4\'4\"","3\'9\"","4\'1\""};
char weight[NUMLILGUYS][8]={"102lbs","90lbs","350lbs","8oz","240lbs","70lbs",
							"12oz","137lbs","-5kg","87lbs"};
byte colorSrc[NUMLILGUYS]={7,6,5,7,4,5,5,1,4,5};

// local function defs
void SetProfElem(int n,byte type,byte num,int x,int y,int tx,int ty);

void InitProfileMenu(void)
{
	int i;

	JamulSoundPurge();	// each level, that should be good
	Music_Load(profile.songChoice[SONG_MENU]);
	Music_Play();

	oldc=255;
	controlX=10;
	cursor=0;
	prfMode=0;
	oldMouseBtn=1;
	clipY=440;

	for(i=0;i<5;i++)
		repeatClock[i]=0;

	LoadOptions();

	profSpr=new sprite_set_t("graphics\\profile.jsp");

	for(i=0;i<8;i++)
	{
		if(!LoadProfile(i))
		{
			DefaultProfile(&prof[i]);
		}
		else
			prof[i]=profile;
	}

	profile=prof[opt.curProfile];
	for(i=0;i<NUMLILGUYS;i++)
	{
		if(i<NUMLILGUYS/2)
			SetProfElem(i,PE_LILGUY,i,i*60+20,-500,i*60+20,100);
		else
			SetProfElem(i,PE_LILGUY,i,(i-NUMLILGUYS/2)*60+20,-500,(i-NUMLILGUYS/2)*60+20,300);
	}
	profElem[profile.character].ty=-500*FIXAMT;
	SetProfElem(CURBIGGUY,PE_BIGGUY,profile.character,130,700,130,300);
	SetProfElem(OTHERBIGGUY,PE_BIGGUY,0,130,700,130,700);

	SetProfElem(OTHERSHEET,PE_SHEET,opt.curProfile,640,480+90,302,232);
	SetProfElem(CURSHEET,PE_SHEET,(opt.curProfile+1)&7,640,480+90,640,480+90);

	// TODO GetDisplayMGL()->SetColorblind(profile.colorblind);
	GetDisplayMGL()->RealizePalette();
}

void ExitProfileMenu(void)
{
	int i;

	for(i=0;i<8;i++)
	{
		profile=prof[i];
		SaveProfile(i);
	}
	profile=prof[opt.curProfile];
	SaveOptions();
	ResetClock(0);
	delete profSpr;
}

void SetProfElem(int n,byte type,byte num,int x,int y,int tx,int ty)
{
	profElem[n].type=type;
	profElem[n].number=num;
	profElem[n].x=x*FIXAMT;
	profElem[n].y=y*FIXAMT;
	profElem[n].tx=tx*FIXAMT;
	profElem[n].ty=ty*FIXAMT;
	profElem[n].dx=0;
	profElem[n].dy=0;
}

void UpdateProfElem(int n)
{
	if(profElem[n].x<profElem[n].tx)
		profElem[n].dx+=FIXAMT*4;
	if(profElem[n].x>profElem[n].tx)
		profElem[n].dx-=FIXAMT*4;
	if(profElem[n].y<profElem[n].ty)
		profElem[n].dy+=FIXAMT*4;
	if(profElem[n].y>profElem[n].ty)
		profElem[n].dy-=FIXAMT*4;

	profElem[n].x+=profElem[n].dx;
	profElem[n].y+=profElem[n].dy;

	if((profElem[n].x>profElem[n].tx && profElem[n].dx>0) ||
	   (profElem[n].x<profElem[n].tx && profElem[n].dx<0))
	{
		profElem[n].x=profElem[n].tx;
		profElem[n].dx=0;
	}
	if((profElem[n].y>profElem[n].ty && profElem[n].dy>0) ||
	   (profElem[n].y<profElem[n].ty && profElem[n].dy<0))
	{
		profElem[n].y=profElem[n].ty;
		profElem[n].dy=0;
	}
}

void UpdateProfElems(void)
{
	int i;

	flash++;
	for(i=0;i<NUM_PROFELEM;i++)
		UpdateProfElem(i);
}

void SwitchGuy(byte oldG,byte newG)
{
	profElem_t tmp;

	if(oldG==newG)
		return;

	// swap the big guys
	tmp=profElem[CURBIGGUY];
	profElem[CURBIGGUY]=profElem[OTHERBIGGUY];
	profElem[OTHERBIGGUY]=tmp;

	profElem[CURBIGGUY].number=newG;
	profElem[CURBIGGUY].ty=300*FIXAMT;
	profElem[OTHERBIGGUY].number=oldG;
	profElem[OTHERBIGGUY].ty=700*FIXAMT;

	// and swap the little guys
	if(oldG<NUMLILGUYS/2)
		profElem[oldG].ty=100*FIXAMT;
	else
		profElem[oldG].ty=300*FIXAMT;

	profElem[newG].ty=-500*FIXAMT;
	profile.color=colorSrc[newG];
	profile.bright=0;
}

byte NextValidGuy(byte oldG)
{
	int i;

	i=oldG+1;
	if(i==NUMLILGUYS)
		i=0;
	while(i!=oldG)
	{
		if(giftShopNum[i]==255 || profile.giftShop[giftShopNum[i]])
			return i;
		i++;
		if(i==NUMLILGUYS)
			i=0;
	}
	return oldG;
}

byte LastValidGuy(byte oldG)
{
	int i;

	i=oldG-1;
	if(i==-1)
		i=NUMLILGUYS-1;
	while(i!=oldG)
	{
		if(giftShopNum[i]==255 || profile.giftShop[giftShopNum[i]])
			return i;
		i--;
		if(i==-1)
			i=NUMLILGUYS-1;
	}
	return oldG;
}

void SwitchEmployee(byte oldG,byte newG)
{
	if(oldG==newG)
		return;

	if(prof[oldG].character!=prof[newG].character)
		SwitchGuy(prof[oldG].character,prof[newG].character);

	SetProfElem(OTHERSHEET,PE_SHEET,newG,640,480+90,302,232);
	SetProfElem(CURSHEET,PE_SHEET,oldG,302,232,640,480+90);
	profile=prof[newG];
	// TODO GetDisplayMGL()->SetColorblind(profile.colorblind);
	GetDisplayMGL()->RealizePalette();
	Music_Load(profile.songChoice[SONG_MENU]);
	Music_Play();
}

byte Mode0MouseCheck(MGLDraw *mgl)
{
	int msx,msy;
	int i,j;
	byte b;

	mgl->GetMouse(&msx,&msy);
	b=mgl->MouseDown();

	if(!b)	// not clicking, don't do anything!
	{
		if(makeSound)
		{
			makeSound=0;
			MakeNormalSound(SND_CHERUBBORED);
		}

		oldMouseBtn=b;
		return 0;
	}
	else	// mouse is down!
	{
		// bright knob
		if(PointInRect(msx,msy,500,183,584,204))
		{
			cursor=1;
			profile.bright=(msx-510)-32;
			if(profile.bright<-28)
				profile.bright=-28;
			if(profile.bright>28)
				profile.bright=28;
		}
		// sound volume
		if(PointInRect(msx,msy,340,324,360+64,346))
		{
			cursor=7;
			i=(msx-350)*4;
			if(i>255)
				i=255;
			if(i<0)
				i=0;
			profile.sound=i;
			makeSound=1;
			SetVolumes(profile.sound,profile.music);
		}
		// music volume
		if(PointInRect(msx,msy,340,349,360+64,371))
		{
			cursor=8;
			i=(msx-350)*4;
			if(i>255)
				i=255;
			if(i<0)
				i=0;
			profile.music=i;
			SetVolumes(profile.sound,profile.music);
		}
		if(!oldMouseBtn) // only taps allowed here
		{
			// check for tab selection
			for(i=0;i<8;i++)
				if(PointInRect(msx,msy,258+i*46,10,304+i*46,30))
				{
					j=opt.curProfile;
					opt.curProfile=i;
					SwitchEmployee(j,opt.curProfile);
					cursor=0;
					MakeNormalSound(SND_MENUSELECT);
				}
			// color box
			if(PointInRect(msx,msy,350,184,390,204))
			{
				profile.color++;
				if(profile.color>7)
					profile.color=0;
				cursor=1;
				MakeNormalSound(SND_MENUSELECT);
			}
			// keys
			for(i=0;i<5;i++)
			{
				if(PointInRect(msx,msy,291,234+i*15,403,249+i*15))
				{
					mgl->LastKeyPressed();
					LastScanCode();
					prfMode=2;
					cursor=i+2;
					MakeNormalSound(SND_MENUSELECT);
				}
			}
			// colorblind
			if(PointInRect(msx,msy,286,374,450,396))
			{
				cursor=9;
				profile.colorblind=1-profile.colorblind;
				// TODO mgl->SetColorblind(profile.colorblind);
				mgl->RealizePalette();
				MakeNormalSound(SND_MENUSELECT);
			}
			// change looks
			if(PointInRect(msx,msy,286,399,450,421))
			{
				cursor=10;
				j=profile.character;
				profile.character=NextValidGuy(profile.character);
				SwitchGuy(j,profile.character);
				MakeNormalSound(SND_MENUSELECT);
			}
			// downsize
			if(PointInRect(msx,msy,286,423,450,446))
			{
				cursor=11;
				yesno=1;
				prfMode=1;
				MakeNormalSound(SND_MENUSELECT);
			}
			if(PointInRect(msx,msy,286,448,410,471))
			{
				cursor=12;
				oldMouseBtn=b;
				MakeNormalSound(SND_MENUSELECT);
				return 1;
			}
		}
	}

	oldMouseBtn=b;
	return 0;
}

byte Mode1MouseCheck(MGLDraw *mgl)
{
	int msx,msy;
	byte b;

	mgl->GetMouse(&msx,&msy);
	b=mgl->MouseDown();

	if(PointInRect(msx,msy,374,328,406,358))
		yesno=0;
	if(PointInRect(msx,msy,443,313,480,346))
		yesno=1;

	if(!b)	// not clicking, don't do anything!
	{
		oldMouseBtn=b;
		return 0;
	}
	else	// mouse is down!
	{
		if(!oldMouseBtn)
		{
			if(PointInRect(msx,msy,374,328,406,358))
			{
				yesno=0;
				oldMouseBtn=b;
				return 1;
			}
			if(PointInRect(msx,msy,443,313,480,346))
			{
				yesno=1;
				oldMouseBtn=b;
				return 1;
			}
		}
	}
	oldMouseBtn=b;
	return 0;
}

byte UpdateProfileMenu(int *lastTime,MGLDraw *mgl)
{
	char c;
	byte c2;
	byte b;
	int i;

	JamulSoundUpdate();

	if(*lastTime>TIME_PER_FRAME*MAX_RUNS)
		*lastTime=TIME_PER_FRAME*MAX_RUNS;
	while(*lastTime>=TIME_PER_FRAME)
	{
		Music_Update();

		UpdateProfElems();

		switch(prfMode)
		{
			case 0:	// just going through options
				if(Mode0MouseCheck(mgl))
					return 1;

				c=mgl->LastKeyPressed();
				c2=GetArrows()|GetControls();

				if((c>='a' && c<='z') || (c>='A' && c<='Z') || c=='_' ||
					c=='!' || (c>='0' && c<='9'))
				{
					if(strlen(profile.name)<12)
					{
						profile.name[strlen(profile.name)+1]='\0';
						profile.name[strlen(profile.name)]=c;
					}
				}
				else if(c==8)	// backspace
				{
					if(strlen(profile.name))
					{
						profile.name[strlen(profile.name)-1]='\0';
					}
				}
				if((c2&CONTROL_UP) && !(oldc&CONTROL_UP))
				{
					cursor--;
					if(cursor>12)
						cursor=12;
					MakeNormalSound(SND_MENUCLICK);
				}
				if((c2&CONTROL_DN) && !(oldc&CONTROL_DN))
				{
					cursor++;
					if(cursor>12)
						cursor=0;
					MakeNormalSound(SND_MENUCLICK);
				}
				if((c2&CONTROL_LF) && !(oldc&CONTROL_LF))
				{
					switch(cursor)
					{
						case 0:	// current employee
							b=opt.curProfile;
							opt.curProfile--;
							if(opt.curProfile>7)
								opt.curProfile=7;
							SwitchEmployee(b,opt.curProfile);
							MakeNormalSound(SND_MENUSELECT);
							break;
						case 1:	// color/bright
							profile.bright--;
							if(profile.bright<-20)
								profile.bright=-20;
							MakeNormalSound(SND_MENUSELECT);
							break;
						case 7: // sound vol
							if(profile.sound>15)
								profile.sound-=16;
							else
								profile.sound=0;
							MakeNormalSound(SND_MENUSELECT);
							SetVolumes(profile.sound,profile.music);
							break;
						case 8: // music vol
							if(profile.music>15)
								profile.music-=16;
							else
								profile.music=0;
							MakeNormalSound(SND_MENUSELECT);
							SetVolumes(profile.sound,profile.music);
							break;
						case 9:	// colorblind
							profile.colorblind=1-profile.colorblind;
							// TODO mgl->SetColorblind(profile.colorblind);
							mgl->RealizePalette();
							MakeNormalSound(SND_MENUSELECT);
							break;
						case 10:	// change looks
							b=profile.character;
							profile.character=LastValidGuy(profile.character);
							SwitchGuy(b,profile.character);
							MakeNormalSound(SND_MENUSELECT);
							break;
					}
				}
				if((c2&CONTROL_RT) && !(oldc&CONTROL_RT))
				{
					switch(cursor)
					{
						case 0:	// current employee
							b=opt.curProfile;
							opt.curProfile++;
							if(opt.curProfile>7)
								opt.curProfile=0;
							SwitchEmployee(b,opt.curProfile);
							MakeNormalSound(SND_MENUSELECT);
							break;
						case 1:	// color/bright
							profile.bright++;
							if(profile.bright>20)
								profile.bright=20;
							MakeNormalSound(SND_MENUSELECT);
							break;
						case 7: // sound vol
							if(profile.sound<256-16)
								profile.sound+=16;
							else
								profile.sound=255;
							MakeNormalSound(SND_MENUSELECT);
							SetVolumes(profile.sound,profile.music);
							break;
						case 8: // music vol
							if(profile.music<256-16)
								profile.music+=16;
							else
								profile.music=255;
							MakeNormalSound(SND_MENUSELECT);
							SetVolumes(profile.sound,profile.music);
							break;
						case 9:	// colorblind
							profile.colorblind=1-profile.colorblind;
							// TODO mgl->SetColorblind(profile.colorblind);
							mgl->RealizePalette();
							MakeNormalSound(SND_MENUSELECT);
							break;
						case 10:	// change looks
							b=profile.character;
							profile.character=NextValidGuy(profile.character);
							SwitchGuy(b,profile.character);
							MakeNormalSound(SND_MENUSELECT);
							break;
					}
				}
				if((c2&CONTROL_B1) && !(oldc&CONTROL_B1))
				{
					switch(cursor)
					{
						case 0:	// current employee
							// nothing
							break;
						case 1:	// color/bright
							profile.color++;
							if(profile.color>7)
								profile.color=0;
							MakeNormalSound(SND_MENUSELECT);
							break;
						case 2:	// up key
						case 3:	// dn key
						case 4:	// lf key
						case 5:	// rt key
						case 6: // fire key
							prfMode=2;	// key enter mode
							MakeNormalSound(SND_MENUSELECT);
							break;
						case 9:	// colorblind
							profile.colorblind=1-profile.colorblind;
							// TODO mgl->SetColorblind(profile.colorblind);
							mgl->RealizePalette();
							MakeNormalSound(SND_MENUSELECT);
							break;
						case 10:	// change looks
							b=profile.character;
							profile.character=NextValidGuy(profile.character);
							SwitchGuy(b,profile.character);
							MakeNormalSound(SND_MENUSELECT);
							break;
						case 11: // downsize
							prfMode=1;	// asking if you're sure
							yesno=1;
							MakeNormalSound(SND_MENUSELECT);
							break;
						case 12:	// exit
							MakeNormalSound(SND_MENUSELECT);
							return 1;
							break;
					}
				}

				prof[opt.curProfile]=profile;
				if(c==27)
				{
					return 1;
				}
				break;
			case 1:	// are you sure you want to downsize?
				if(Mode1MouseCheck(mgl))
				{
					if(yesno==0)
					{
						b=profile.character;
						DefaultProfile(&profile);
						prof[opt.curProfile]=profile;
						SwitchGuy(b,profile.character);
						MakeNormalSound(SND_MENUCANCEL);
					}
					prfMode=0;
				}
				else
				{
					c=mgl->LastKeyPressed();
					c2=GetArrows()|GetControls();

					if(c==27)
						prfMode=0;
					if((c2&CONTROL_LF) && !(oldc&CONTROL_LF))
					{
						yesno=0;
						MakeNormalSound(SND_MENUCLICK);
					}
					if((c2&CONTROL_RT) && !(oldc&CONTROL_RT))
					{
						yesno=1;
						MakeNormalSound(SND_MENUCLICK);
					}
					if((c2&CONTROL_B1) && !(oldc&CONTROL_B1))
					{
						if(yesno==0)
						{
							b=profile.character;
							DefaultProfile(&profile);
							prof[opt.curProfile]=profile;
							SwitchGuy(b,profile.character);
							MakeNormalSound(SND_MENUCANCEL);
						}
						prfMode=0;
					}
				}
				break;
			case 2: // entering a new keyboard control
				c=LastScanCode();
				c2=GetArrows()|GetControls();

				if(oldc!=0)
					break;

				if(c!=1 && c!=0)
				{
					// up arrow can ONLY be used for up
					if(c==72 && cursor!=2)
					{
						MakeNormalSound(SND_MENUCANCEL);
						c=mgl->LastKeyPressed();
						break;
					}
					// down arrow can ONLY be used for down
					if(c==80 && cursor!=3)
					{
						MakeNormalSound(SND_MENUCANCEL);
						c=mgl->LastKeyPressed();
						break;
					}

					// left arrow can ONLY be used for left
					if(c==75 && cursor!=4)
					{
						MakeNormalSound(SND_MENUCANCEL);
						c=mgl->LastKeyPressed();
						break;
					}

					// right arrow can ONLY be used for right
					if(c==77 && cursor!=5)
					{
						MakeNormalSound(SND_MENUCANCEL);
						c=mgl->LastKeyPressed();
						break;
					}

					// enter can ONLY be used for fire
					if(c==28 && cursor!=6)
					{
						MakeNormalSound(SND_MENUCANCEL);
						c=mgl->LastKeyPressed();
						break;
					}

					profile.keys[cursor-2]=c;
					for(i=0;i<5;i++)
						if(i!=cursor-2 && profile.keys[i]==profile.keys[cursor-2])
							profile.keys[i]=0;
					prfMode=0;
					MakeNormalSound(SND_MENUSELECT);
				}
				else if(c==1) // ESC cancels
				{
					prfMode=0;
					MakeNormalSound(SND_MENUCANCEL);
				}
				c=mgl->LastKeyPressed();
				break;
		}


		oldc=c2;

		if(oldc&CONTROL_UP)
			repeatClock[0]++;
		else
			repeatClock[0]=0;
		if(oldc&CONTROL_DN)
			repeatClock[1]++;
		else
			repeatClock[1]=0;
		if(oldc&CONTROL_LF)
			repeatClock[2]++;
		else
			repeatClock[2]=0;
		if(oldc&CONTROL_RT)
			repeatClock[3]++;
		else
			repeatClock[3]=0;
		if(oldc&CONTROL_B1)
			repeatClock[4]++;
		else
			repeatClock[4]=0;

		if(repeatClock[0]>10)
		{
			repeatClock[0]=0;
			oldc&=(~CONTROL_UP);
		}
		if(repeatClock[1]>10)
		{
			repeatClock[1]=0;
			oldc&=(~CONTROL_DN);
		}
		if(repeatClock[2]>10)
		{
			repeatClock[2]=0;
			oldc&=(~CONTROL_LF);
		}
		if(repeatClock[3]>10)
		{
			repeatClock[3]=0;
			oldc&=(~CONTROL_RT);
		}
		if(repeatClock[4]>10)
		{
			repeatClock[4]=0;
			oldc&=(~CONTROL_B1);
		}

		if(abs(clipChoices[cursor]-clipY)>4)
			clipY+=(clipChoices[cursor]-clipY)/4;
		else
			clipY=clipChoices[cursor];

		*lastTime-=TIME_PER_FRAME;
	}

	return 0;
}

void RenderTab(int i,byte on,MGLDraw *mgl)
{
	if(on)
		profSpr->GetSprite(7)->DrawBright(302+i*46+2,232,mgl,-32);
	profSpr->GetSprite(7)->DrawOffColor(302+i*46,232,mgl,2,i,on*8);
	PrintLimited(280+i*46-15,18,280+i*46+33,prof[i].name,-32,1);
}

void RenderDataItem(int x,int y,const char *txt,const char *value,int length,MGLDraw *mgl)
{
	PrintUnGlow(x,y,txt,0);
	mgl->FillBox(x+GetStrLength(txt,0)+3,y+18,x+GetStrLength(txt,0)+3+length,y+18,0);
	mgl->FillBox(x+GetStrLength(txt,0)+3,y+19,x+GetStrLength(txt,0)+3+length,y+19,64+24);
	Print(x+GetStrLength(txt,0)+6,y+6,value,-32,1);
}

void RenderName(int x,int y,const char *txt,const char *value,int length,MGLDraw *mgl)
{
	int i;

	PrintUnGlow(x,y,txt,0);
	mgl->FillBox(x+GetStrLength(txt,0)+3,y+18,x+GetStrLength(txt,0)+3+length,y+18,0);
	mgl->FillBox(x+GetStrLength(txt,0)+3,y+19,x+GetStrLength(txt,0)+3+length,y+19,64+24);

	Print(x+GetStrLength(txt,0)+6,y+6,value,-32,1);

	if((flash/2)&1)
	{
		i=x+GetStrLength(txt,0)+6+GetStrLength(value,1);
		mgl->FillBox(i,y+8,i+3,y+15,0);
	}
}

void RenderSheetControls(int x,int y,MGLDraw *mgl,profile_t *prof)
{
	int i;
	char s[4];

	//291,210
	PrintUnGlow(x,y,"Keyboard Controls",0);

	PrintUnGlow(x+3,y+25,"Up",1);
	PrintUnGlow(x+4,y+25,"Up",1);

	PrintUnGlow(x+3,y+40,"Down",1);
	PrintUnGlow(x+4,y+40,"Down",1);

	PrintUnGlow(x+3,y+55,"Left",1);
	PrintUnGlow(x+4,y+55,"Left",1);

	PrintUnGlow(x+3,y+70,"Right",1);
	PrintUnGlow(x+4,y+70,"Right",1);

	PrintUnGlow(x+3,y+85,"Fire",1);
	PrintUnGlow(x+4,y+85,"Fire",1);

	mgl->Box(x,y+22,x+140,y+97,0);

	mgl->Box(x,y+37,x+140,y+52,0);
	mgl->Box(x,y+67,x+140,y+82,0);

	mgl->Box(x+50,y+22,x+140,y+97,0);

	for(i=0;i<3;i++)
		if(Random(4)==0)
			s[i]='?';
		else
			s[i]=' ';
	s[3]='\0';

	for(i=0;i<5;i++)
	{
		if(prfMode==2 && cursor==i+2)
			CenterPrint(x+95,y+25+i*15,s,-32,1);
		else
			CenterPrint(x+95,y+25+i*15,ScanCodeText(prof->keys[i]),-32,1);
	}
}

void RenderProfileStars(int x,int y,profile_t *prof,MGLDraw *mgl)
{
	char s[10];

	RenderStar(0,5,x*FIXAMT,y*FIXAMT,mgl);
	sprintf(s,"%03d/%03d",prof->starsLeft,prof->stars);
	PrintUnGlow(x+25,y-8,s,0);
}

void RenderDataSheet(int x,int y,profile_t *prof,MGLDraw *mgl)
{
	char tmp[16];

	profSpr->GetSprite(2)->Draw(x+SHEET_OFSX,y+SHEET_OFSY,mgl);

	x-=302;
	y-=232;

	RenderName(x+291,y+64,"Name:",prof->name,100,mgl);

	mgl->Box(x+288,y+92,x+610,y+177,0);

	sprintf(tmp,"%d",charAges[prof->character]);
	RenderDataItem(x+291,y+94,"Age:",tmp,50,mgl);

	if(prof->character==CHAR_GOTH || prof->character==CHAR_HIPPIE)
		strcpy(tmp,"F");
	else if(prof->character==CHAR_ROBOT || prof->character==CHAR_SHROOM || prof->character==CHAR_FREAK)
		strcpy(tmp,"N/A");
	else
		strcpy(tmp,"M");
	RenderDataItem(x+450,y+94,"Gender:",tmp,50,mgl);

	RenderDataItem(x+291,y+124,"Height:",height[prof->character],50,mgl);
	RenderDataItem(x+450,y+124,"Weight:",weight[prof->character],50,mgl);
	RenderDataItem(x+291,y+154,"Disposition:",dispositions[prof->character],70,mgl);

	PrintUnGlow(x+291,y+184,"Color:",0);
	mgl->FillBox(x+350,y+184,x+390,y+204,0);
	mgl->Box(x+351,y+185,x+389,y+203,31);
	mgl->FillBox(x+353,y+187,x+387,y+201,prof->color*32+16);

	PrintUnGlow(x+450,y+184,"Bright:",0);
	mgl->FillBox(x+510,y+194,x+510+64,y+195,0);
	mgl->FillBox(x+510+prof->bright+30,y+184,x+510+prof->bright+34,y+204,0);
	mgl->FillBox(x+510+prof->bright+31,y+185,x+510+prof->bright+33,y+203,31);

	RenderSheetControls(x+291,y+212,mgl,prof);

	RenderProfileStars(x+480,y+230,prof,mgl);

	// the menu

	PrintUnGlow(x+291,y+325,"Sound",0);
	mgl->FillBox(x+350-2,y+335,x+350+65,y+336,0);
	mgl->FillBox(x+350+(prof->sound/4)-2,y+325,x+350+(prof->sound/4)+2,y+345,0);
	mgl->FillBox(x+350+(prof->sound/4)-1,y+326,x+350+(prof->sound/4)+1,y+344,31);

	PrintUnGlow(x+291,y+350,"Music",0);
	mgl->FillBox(x+350-2,y+360,x+350+65,y+361,0);
	mgl->FillBox(x+350+(prof->music/4)-2,y+350,x+350+(prof->music/4)+2,y+370,0);
	mgl->FillBox(x+350+(prof->music/4)-1,y+351,x+350+(prof->music/4)+1,y+369,31);

	if(profile.colorblind)
		PrintUnGlow(x+291,y+375,"Colorblind Mode: On",0);
	else
		PrintUnGlow(x+291,y+375,"Colorblind Mode: Off",0);

	PrintUnGlow(x+291,y+400,"Change Character",0);
	PrintUnGlow(x+291,y+425,"Downsize Employee",0);
	PrintUnGlow(x+291,y+450,"Save And Exit",0);

	profSpr->GetSprite(3)->Draw(x+302,y+clipY,mgl);
}

void RenderProfElem(profElem_t *p,MGLDraw *mgl)
{
	switch(p->type)
	{
		case PE_LILGUY:
			if(giftShopNum[p->number]==255 || profile.giftShop[giftShopNum[p->number]])
				profSpr->GetSprite(8+2*p->number+1)->DrawBright(p->x/FIXAMT,p->y/FIXAMT,mgl,-6);
			break;
		case PE_BIGGUY:
			profSpr->GetSprite(8+2*p->number)->DrawOffColor(p->x/FIXAMT,p->y/FIXAMT,mgl,colorSrc[p->number],profile.color,profile.bright);
			break;
		case PE_SHEET:
			RenderDataSheet(p->x/FIXAMT,p->y/FIXAMT,&prof[p->number],mgl);
			break;
	}
}

void RenderProfileMenu(MGLDraw *mgl)
{
	int i;
	int msx,msy;
	char s[32];

	Music_Update();

	/*
	if(!GM_doDraw)
		return;
	*/

	mgl->FillBox(0,0,251,479,32*3+6);
	mgl->FillBox(252,32,639,479,83);
	mgl->FillBox(252,0,679,31,32*3+6);

	for(i=0;i<NUM_PROFELEM;i++)
		RenderProfElem(&profElem[i],mgl);

	for(i=0;i<8;i++)
		if(i!=opt.curProfile)
			RenderTab(i,0,mgl);
	RenderTab(opt.curProfile,1,mgl);

	if(prfMode==1)	// downsize yes/no
	{
		profSpr->GetSprite(4)->DrawBright(325,245,mgl,-32);
		profSpr->GetSprite(4)->Draw(320,240,mgl);
		sprintf(s,"%s?",profile.name);
		PrintAngleUnGlow(350,255,"Really downsize",0);
		PrintAngleUnGlow(370,275,s,0);
		profSpr->GetSprite(5)->DrawBright(382,332,mgl,-32);
		if(yesno==0)
			profSpr->GetSprite(5)->Draw(380,330,mgl);
		profSpr->GetSprite(6)->DrawBright(452,317,mgl,-32);
		if(yesno==1)
			profSpr->GetSprite(6)->Draw(450,315,mgl);
		CenterPrint(130,430,"Left or Right to select,",0,0);
		CenterPrint(130,455,"Enter to confirm",0,0);
	}
	else
		// menu help
		switch(cursor)
		{
			case 0: // current employee
				CenterPrint(130,430,"Press Left or Right to change",0,0);
				CenterPrint(130,455,"Current Employee",0,0);
				break;
			case 1: // color/bright
				CenterPrint(130,430,"Press Enter to change color,",0,0);
				CenterPrint(130,455,"Left or Right to change bright",0,0);
				break;
			case 2: // change up key
				CenterPrint(130,430,"Press Enter to change the",0,0);
				CenterPrint(130,455,"\"Up\" Key",0,0);
				break;
			case 3: // change dn key
				CenterPrint(130,430,"Press Enter to change the",0,0);
				CenterPrint(130,455,"\"Down\" Key",0,0);
				break;
			case 4: // change lf key
				CenterPrint(130,430,"Press Enter to change the",0,0);
				CenterPrint(130,455,"\"Left\" Key",0,0);
				break;
			case 5: // change rt key
				CenterPrint(130,430,"Press Enter to change the",0,0);
				CenterPrint(130,455,"\"Right\" Key",0,0);
				break;
			case 6: // change fire key
				CenterPrint(130,430,"Press Enter to change the",0,0);
				CenterPrint(130,455,"\"Fire\" Key",0,0);
				break;
			case 7: // sound volume
				CenterPrint(130,430,"Press Left or Right",0,0);
				CenterPrint(130,455,"to adjust Sound Volume",0,0);
				break;
			case 8: // music volume
				CenterPrint(130,430,"Press Left or Right",0,0);
				CenterPrint(130,455,"to adjust Music Volume",0,0);
				break;
			case 9: // colorblind
				CenterPrint(130,430,"Press Enter to turn",0,0);
				CenterPrint(130,455,"Colorblind Mode on or off",0,0);
				break;
			case 10: // change look
				CenterPrint(130,430,"Press Left or Right",0,0);
				CenterPrint(130,455,"to change character",0,0);
				break;
			case 11: // downsize
				CenterPrint(130,430,"Press Enter to delete this",0,0);
				CenterPrint(130,455,"Employee Profile",0,0);
				break;
			case 12: // Exit
				CenterPrint(130,430,"Press Enter to Save & Exit",0,0);
				CenterPrint(130,455,"and use this Profile",0,0);
				break;
		}

	mgl->GetMouse(&msx,&msy);

	profSpr->GetSprite(0)->Draw(msx,msy,mgl);
}

void ProfileMenu(MGLDraw *mgl)
{
	byte done=0;
	int lastTime;

	InitProfileMenu();
	lastTime=1;

	while(!done)
	{
		lastTime+=TimeLength();
		StartClock();
		done=UpdateProfileMenu(&lastTime,mgl);
		RenderProfileMenu(mgl);
		mgl->Flip();
		EndClock();

		if(!mgl->Process())
			done=1;

	}

	ExitProfileMenu();
}

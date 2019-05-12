#include "ch_summon.h"
#include "quest.h"
#include "options.h"
#include "bowling.h"
#include "display.h"

#define PLYR_ACCEL	(FIXAMT)
#define PLYR_DECEL	(FIXAMT*3/4)
#define PLYR_MAXSPD (FIXAMT*6)

char summonNames[8][16]={
		"Devil Doll",	// no weapon
		"Bombie",		// bombs
		"Horror",		// lightning
		"Arctic Wolf",		// ice wand
		"Backup Bat",		// cactus
		"Healer",		// boomerang
		"Croaker",		// whoopee
		"Hot Dog",		// hot pants
	};

Guy *summonGuys[11];
byte numSummons;
byte summonType[11];
int summonLife[11];

void GrabSummons(void)
{
	int i;
	// when you exit a map, this grabs all the summoned guys first, for safekeeping

	for(i=0;i<11;i++)
	{
		if(summonGuys[i])
		{
			summonType[i]=summonGuys[i]->type;
			summonLife[i]=summonGuys[i]->hp;
		}
		else
			summonType[i]=MONS_NONE;
	}
}

void DeploySummons(Guy *goodguy)
{
	int i;
	Guy *g;

	numSummons=0;
	for(i=0;i<11;i++)
	{
		if(summonType[i])
		{
			g=AddGuy(goodguy->x,goodguy->y,0,summonType[i]);
			if(g)
			{
				g->hp=summonLife[i];
				numSummons++;
			}
			summonGuys[i]=g;
		}
		else
			summonGuys[i]=NULL;
	}
}

void AddToSummoned(Guy *g)
{
	int i;

	for(i=0;i<11;i++)
	{
		if(summonGuys[i]==NULL)
		{
			summonGuys[i]=g;
			summonType[i]=g->type;
			summonLife[i]=g->hp;
			numSummons++;
			return;
		}
	}
}

void RenderSummonLife(MGLDraw *mgl)
{
	int i,j,n,w;
	int max,lifeFactor;
	byte colTab[]={5,3,7,6,1,0,4};
	char shortNames[8][16]={
		"",				// no weapon
		"Bombie",		// bombs
		"Horror",		// lightning
		"Wolf",			// ice wand
		"Bat",			// cactus
		"Healer",		// boomerang
		"Croaker",		// whoopee
		"Hot Dog",		// hot pants
	};

	lifeFactor=32+224*player.xtraByte/20;

	n=0;
	for(i=0;i<numSummons;i++)
	{
		for(j=n;j<11;j++)
			if(summonGuys[j])
			{
				max=MonsterHP(summonGuys[j]->type)*lifeFactor/256;
				DrawFillBox(574,20*i+125,638,20*i+143,colTab[summonGuys[j]->type-MONS_SUMBOMBIE]*32+4);
				DrawBox(574,20*i+125,638,20*i+143,31);
				w=summonGuys[j]->hp*64/max;
				if(w>0)
				{
					if(w>63)
						w=63;
					DrawFillBox(575,20*i+126,574+w,20*i+142,colTab[summonGuys[j]->type-MONS_SUMBOMBIE]*32+10);
				}
				Print(577,20*i+130,shortNames[summonGuys[j]->type-MONS_SUMBOMBIE+1],-32,1);
				Print(576,20*i+129,shortNames[summonGuys[j]->type-MONS_SUMBOMBIE+1],0,1);
				n=j+1;
				break;
			}
	}
}

void InitSummons(void)
{
	int i;

	for(i=0;i<11;i++)
	{
		summonGuys[i]=NULL;
		summonType[i]=0;
	}
	numSummons=0;
}

char *SummonName(byte spl)
{

	return summonNames[spl];
}

byte SummonCost(byte spl)
{
	byte cost[]={0,		// no weapon
				 10,	// bombs
				 5,		// lightning
				 5,		// ice wand
				 2,		// cactus
				 20,	// boomerang
				 3,		// whoopee
				 10,	// hot pants
	};

	return cost[spl];
}

void SummonFire(Guy *me)
{
	int i;

	if(opt.cheats[CH_FROGWPN])
		MakeSound(SND_FROGLICK,me->x,me->y,SND_CUTOFF,1200);
	else
		MakeSound(SND_LOONYSHOOT,me->x,me->y,SND_CUTOFF,1200);

	FireBullet(me->x,me->y,me->facing*32,BLT_DEVILDOLL);
	if(player.fireFlags&FF_TRIPLE)
	{
		FireBullet(me->x,me->y,((me->facing+2)*32)&255,BLT_DEVILDOLL);
		FireBullet(me->x,me->y,((me->facing+6)*32)&255,BLT_DEVILDOLL);
	}
	player.reload=15;

	for(i=0;i<11;i++)
	{
		if(summonGuys[i] && summonGuys[i]->type==MONS_SUMBAT)
			FireBullet(summonGuys[i]->x,summonGuys[i]->y,summonGuys[i]->facing*32,BLT_DEVILDOLL);
	}
}

void SummonGuy(Guy *me)
{
	Guy *g;
	int i;
	int lifeFactor;

	lifeFactor=32+224*player.xtraByte/20;

	if(player.money<SummonCost(player.weapon-ITM_WBOMB+1) || numSummons>=player.fireRange+1)
	{
		MakeNormalSound(SND_ELDERSHOOT);
		return;
	}

	g=AddGuy(me->x,me->y,0,player.weapon-ITM_WBOMB+MONS_SUMBOMBIE);
	if(g)
	{
		numSummons++;
		for(i=0;i<11;i++)
		{
			if(summonGuys[i]==NULL)
			{
				summonGuys[i]=g;
				g->hp=MonsterHP(g->type)*lifeFactor/256;
				player.money-=SummonCost(player.weapon-ITM_WBOMB+1);
				MakeNormalSound(SND_ELDERSUMMON);
				return;
			}
		}
		// if you got here, for some reason there actually isn't a free spot in the list
		g->type=MONS_NONE;	// so undo the summoning, no charge, no sound at all so I can see when this happens
	}
}

void UpdateSummons(void)
{
	int i;
	static byte dmgCount=0;

	dmgCount++;

	if(dmgCount==60)
		dmgCount=0;

	for(i=0;i<11;i++)
	{
		if(summonGuys[i]!=NULL)
		{
			if(dmgCount==0 && summonGuys[i]->hp)
			{
				summonGuys[i]->hp--;
				if(summonGuys[i]->hp==0)
					DoMove(summonGuys[i],ANIM_DIE,128,1,0,0);
			}
			if(summonGuys[i]->hp==0)
			{
				numSummons--;
				summonGuys[i]=NULL;
			}
		}
	}
}

void SummonControlMe(Guy *me,mapTile_t *mapTile,world_t *world)
{
	byte c;

	static byte oldc=255;

	UpdateInterface(player.hearts);

	if(player.reload)
		player.reload--;
	if(player.chatClock)
		player.chatClock--;

	if(player.saveClock)
		player.saveClock--;

	if(player.wpnReload)
		player.wpnReload--;

	if(player.shield)
		player.shield--;

	if(player.invinc)
		player.invinc--;

	if(player.pushPower)
		player.pushPower--;

	HandlePoison(me);

	if(player.stone)
	{
		player.stone--;
		return;
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
			MakeSound(SND_ELDEROUCH,me->x,me->y,SND_CUTOFF|SND_ONE,2000);
		else if(me->seq==ANIM_DIE)	// so it doesn't do this if you're drowning
			MakeSound(SND_ELDERDIE,me->x,me->y,SND_CUTOFF|SND_ONE,2000);
	}

	// triggering stuff
	if(me->action==ACTION_BUSY)
	{
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
		if(player.weapon==0 || player.worldNum==WORLD_LOONYBALL || player.worldNum==WORLD_BOWLING)
		{
			if(player.worldNum==WORLD_BOWLING)
			{
				ThrowBowlingBall();
				return;
			}
			me->seq=ANIM_ATTACK;
			me->frmTimer=0;
			me->frmAdvance=255;
			me->frm=0;
			player.boredom=0;
			SummonFire(me);
		}
		else
		{
			DoMove(me,ANIM_A1,128,0,me->dx,me->dy);
			SummonGuy(me);
			player.reload=4;
			me->action=ACTION_BUSY;
			return;
		}
	}
	if((c&CONTROL_B2) &&(!(oldc&CONTROL_B2)) && player.worldNum!=WORLD_LOONYBALL && player.worldNum!=WORLD_BOWLING)	// pushed wpn use button
	{
		// change summon
		DoMove(me,ANIM_A2,256,0,me->dx,me->dy);
		MakeNormalSound(SND_STICKATTACK);
		while(1)
		{
			// go to next spell that you DO have
			player.weapon++;
			if(player.weapon==1)
				player.weapon=ITM_WBOMB;
			if(player.weapon==ITM_WHOTPANTS+1)
				player.weapon=0;
			if(player.weapon==0 || player.var[VAR_WEAPON+player.weapon-ITM_WBOMB])
				break;
		}
		player.reload=10;	// get rid of any excess reload, but induce some extra
							// to prevent manic summon slinging
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

		if(!(world->terrain[mapTile->floor].flags&TF_MUD))
		{
			// on non-mud, everything is fun...
			Clamp(&me->dx,PLYR_MAXSPD);
			Clamp(&me->dy,PLYR_MAXSPD);
		}
		else
		{
			// but on mud, you can BARELY MOVE
			if(player.var[VAR_BOOTS])
			{
				// boots sure help though
				Clamp(&me->dx,PLYR_MAXSPD*3/4);
				Clamp(&me->dy,PLYR_MAXSPD*3/4);
			}
			else
			{
				if(!player.var[VAR_QUESTASSIGN+QUEST_BOOTS])
					BeginChatting(255);	// talk to yourself about boots
				Clamp(&me->dx,FIXAMT/2);
				Clamp(&me->dy,FIXAMT/2);
			}
			SpurtParticles(PART_DIRT,0,me->x,me->y,0,((me->facing*32)+128)&255,8);
			SpurtParticles(PART_DIRT,1,me->x,me->y,0,((me->facing*32)+128)&255,8);
		}

		if(mapTile->item==ITM_MAGICPLANT && mapTile->itemInfo==0 && player.var[VAR_FERTILIZER])
		{
			// begin a-growin'!!
			mapTile->itemInfo=1;
			MakeSound(SND_PLANTGROW,me->x,me->y,SND_CUTOFF,600);
		}
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
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
		if(player.boredom>200+(int)Random(3200))
		{
			MakeSound(SND_ELDERSHOOT,me->x,me->y,SND_CUTOFF|SND_ONE,2000);
			me->seq=ANIM_A2;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=72;
			player.boredom=0;
			if(opt.cheats[CH_VINTAGE])
				NewMessage("Ho hum.",60);
		}
	}

	CallFarley(me);
	SetAreaName(world);
	oldc=c;
}

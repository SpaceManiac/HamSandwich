#include "player.h"
#include "options.h"
#include "title.h"
#include "floor.h"
#include "pushing.h"
#include "score.h"

player_t player;
byte playerGlow;	// for torch-lit levels, and for exciting moments
byte tportclock;
static byte oldc=255;

void InitPlayer(byte initWhat,byte world,byte level)
{
	int i,j;

	for(i=0;i<9;i++)
		player.item[i]=0;

	player.score=0;
	
	if(initWhat==INIT_GAME)	// initialize everything, this is to start a whole new game
	{
		player.totalScore=0;

		player.worldNum=world;
		player.monsType=MONS_LOONY;
		player.bestCombo=0;
	}
	if(initWhat>=INIT_WORLD) // initialize the things that go with each world
	{
	}

	player.clock=0;
	player.miniClock=0;
	player.invinc=0;
	player.levelNum=level;
	player.reload=0;
	player.wpnReload=0;
	playerGlow=0;
	player.pushPower=0;
	player.combo=0;
	player.comboClock=0;
	player.escapeBonus=0;
	player.destructionBonus=0;
	player.dumpBonus=0;
	player.wantBomb=0;
	player.noPushAnim=0;
	for(i=0;i<8;i++)
	{
		player.deadPencils[i]=0;
		player.pencils[i]=0;
		player.blownUp[i]=0;
		player.dumpedObj[i]=0;
		for(j=0;j<4;j++)
		{
			player.dumpedMons[i][j]=0;
			player.blownUpMons[i][j]=0;
			player.rescueMons[i][j]=0;
		}
	}
	//SetupPlayerSprite();
	player.boredom=0;
	player.maxBoredom=30*5;
}

void ExitPlayer(void)
{
}

float CalcPercent(player_t *p)
{
	return (float)10.0;
}

void PlayerWinLevel(byte w,byte l,byte isSecret)
{
}

byte PlayerPushMore(void)
{
	return 1;

	player.pushPower+=2;
	if(player.pushPower>=3)
	{
		player.pushPower=0;
		return 1;
	}
	else
		return 0;
}

byte PlayerSavePush(void)
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

byte GetPlayerGlow(void)
{
	return playerGlow;
}

void SetPlayerGlow(byte v)
{
	playerGlow=v;
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

byte DropBomb(int x,int y,Map *map)
{
	int mx,my,telex,teley;
	mapTile_t *m;
	Guy *g;

	mx=(x/(TILE_WIDTH*FIXAMT));
	my=(y/(TILE_HEIGHT*FIXAMT));

	// can't drop out of bounds
	if(x<0 || y<0 || mx>=map->width || my>=map->height)
		return 0;

	m=&map->map[mx+my*map->width];

	if(FloorIsSolid(m->floor))
		return 0;	// not on solid floor

	if(m->item)
		return 0;

	if(FloorIsTeleporter(m->floor))
	{
		// uh oh, it must drop at the OTHER end!
		if(map->SelectTeleporter(mx,my,&telex,&teley))
		{
			LightningBolt(x,y,(telex*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
				(teley*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT);
			LightningBolt(x,y,(telex*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
				(teley*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT);
			LightningBolt(x,y,(telex*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
				(teley*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT);
			m=&map->map[telex+teley*map->width];
			x=(telex*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
			y=(teley*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
			MakeSound(SND_TELEPORT,x,y,SND_CUTOFF,100);
		}
		else
			MakeSound(SND_NOTELEPORT,x,y,SND_CUTOFF,100);
	}

	if(FloorIsHole(m->floor))
	{
		player.dumpedObj[(player.item[0]%8)]++;
		m->floor+=20;
		SmokePuff(x,y,2);
		MakeSound(SND_FALLINHOLE,x,y,SND_CUTOFF,100);
		return 1;
	}
	if(FloorIsPit(m->floor))
	{
		player.dumpedObj[(player.item[0]%8)]++;
		SmokePuff(x,y,2);
		MakeSound(SND_FALLINPIT,x,y,SND_CUTOFF,100);
		return 1;
	}
	m->item=ITM_ACTIVEBOMB;
	g=AddGuy(x,y,0,MONS_BOMB);
	g->color=player.item[0]%8;
	m->itemInfo=g->color;
	MakeNormalSound(SND_BOMBDROP);

	return 1;
}

void PlayerUseItem(Guy *me,Map *map)
{
	int i;
	int mx,my;

	switch(player.item[0]/8)
	{
		case PE_BOMB:
			// have a bomb...
			me->tx=me->x+Cosine(me->facing*32)*TILE_WIDTH;
			me->ty=me->y+Sine(me->facing*32)*TILE_HEIGHT;
			if(DropBomb(me->tx,me->ty,map))
			{
				// make drop bomb sound
				
				// remove the item
				for(i=1;i<9;i++)
					player.item[i-1]=player.item[i];
				player.item[8]=0;
			}
			else
			{
				MakeNormalSound(SND_NOITEM);
			}
			me->tx=me->x;
			me->ty=me->y;
			break;
		case PE_DETONATE:
			DetonateAllBombs(player.item[0]%8,map);
			MakeNormalSound(SND_DETONATOR);
			// remove the item
			for(i=1;i<9;i++)
				player.item[i-1]=player.item[i];
			player.item[8]=0;
			break;
		case PE_ACID:
			mx=me->x+Cosine(me->facing*32)*TILE_WIDTH;
			my=me->y+Sine(me->facing*32)*TILE_HEIGHT;
			if(mx<0)
				mx=-FIXAMT*TILE_WIDTH;
			if(my<0)
				my=-FIXAMT*TILE_HEIGHT;
			mx=(mx/(FIXAMT*TILE_WIDTH));
			my=(my/(FIXAMT*TILE_HEIGHT));
			MakeNormalSound(SND_ACID);
			if(!AcidSplash(map,player.item[0]%8,mx,my))
			{
				// make smoke, since nothing got melted to make it for you
				BlowSmoke(mx,my,player.item[0]%8);
				player.destructionBonus--;	// undo the addition caused by the smoke

			}
			// remove the item
			if(GameType()!=GAME_CLEARANCE)
			{
				for(i=1;i<9;i++)
					player.item[i-1]=player.item[i];
				player.item[8]=0;
			}
			break;
		case PE_LASER:
			mx=me->x+Cosine(me->facing*32)*TILE_WIDTH;
			my=me->y+Sine(me->facing*32)*TILE_HEIGHT;
			mx=(mx/(FIXAMT*TILE_WIDTH));
			my=(my/(FIXAMT*TILE_HEIGHT));
			FireLaser(mx,my,me->facing,player.item[0]%8,map);
			// remove the item
			for(i=1;i<9;i++)
				player.item[i-1]=player.item[i];
			player.item[8]=0;
			break;
		case PE_KEY:
		case PE_NONE:
			MakeNormalSound(SND_NOITEM);
			break;
	}
}

void PlayerControlMe(Guy *me,Map *map,mapTile_t *mapTile,world_t *world)
{
	byte c;
	int telex,teley;
	static byte cherubTick;

	cherubTick++;
	if(profile.character==CHAR_CHERUB && (cherubTick&1)==0)
		CherubFX(me->x,me->y,profile.color);

	player.miniClock++;
	if(player.miniClock==30)
	{
		player.miniClock=0;
		if(player.clock<map->timer)
			player.clock++;
		if(player.clock>=map->timer && map->timer>0)
		{
			if(map->goal.type==GOAL_LIVE)
				SendMessageToGame(MSG_WINLEVEL,0);
			else if(!Score_ComboGoing())
			{
				player.invinc=0;
				PlayerExplode(map);
			}
			return;
		}
		else if(player.clock>=map->timer-10 && map->timer>0)
		{
			MakeNormalSound(SND_TIMERBEEP);
		}
	}

	if(tportclock)
		tportclock--;

	if(player.reload)
		player.reload--;

	if(player.wpnReload)
		player.wpnReload--;

	if(player.invinc)
	{
		player.invinc--;
		if(player.invinc==30 || player.invinc==60 || player.invinc==90)
			MakeNormalSound(SND_SHIELDDOWN);
	}

	if(player.pushPower)
		player.pushPower--;

	me->mind1=0;

	if(me->ouch==4)
	{
		/*
		if(me->hp>0)
			MakeSound(SND_LOONYOUCH,me->x,me->y,SND_CUTOFF|SND_ONE,2000);
		else if(me->seq==ANIM_DIE)	// so it doesn't do this if you're drowning
			MakeSound(SND_LOONYDIE,me->x,me->y,SND_CUTOFF|SND_ONE,2000);
			*/
	}

	// triggering stuff
	if(me->action==ACTION_BUSY)
	{
		return;
	}

	if(me->dx>0 && me->x>me->tx)
		me->x=me->tx;
	if(me->dx<0 && me->x<me->tx)
		me->x=me->tx;
	if(me->dy>0 && me->y>me->ty)
		me->y=me->ty;
	if(me->dy<0 && me->y<me->ty)
		me->y=me->ty;

	// at destination, standing around
	c=GetControls()|GetArrows();

	if((c&CONTROL_B1) && !(oldc&CONTROL_B1))
		player.wantBomb=1;

	if(me->x==me->tx && me->y==me->ty)
	{
		if(me->dx!=0 || me->dy!=0)
		{
			// you just got here, since those are non-zero
			curMap->GoodieCheck(me->mapx,me->mapy,GOODIE_STEP);
			if(FloorIsTeleporter(map->map[me->mapx+me->mapy*map->width].floor))
			{
				if(map->SelectTeleporter(me->mapx,me->mapy,&telex,&teley))
				{
					LightningBolt(me->x,me->y,(telex*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
						(teley*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT);
					LightningBolt(me->x,me->y,(telex*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
						(teley*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT);
					LightningBolt(me->x,me->y,(telex*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
						(teley*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT);

					map->map[me->mapx+me->mapy*map->width].item=ITM_NONE;
					me->x=(telex*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
					me->y=(teley*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
					me->mapx=telex;
					me->mapy=teley;
					me->tx=me->x;
					me->ty=me->y;
					map->map[me->mapx+me->mapy*map->width].item=ITM_PLAYER;
					curMap->GoodieCheck(me->mapx,me->mapy,GOODIE_STEP);
					// add teleport FX
					MakeSound(SND_TELEPORT,me->x,me->y,SND_CUTOFF,100);
				}
				else
				{
					// fail teleport effect
					MakeSound(SND_NOTELEPORT,me->x,me->y,SND_CUTOFF,100);
				}
			}
			if(FloorIsSpikes(map->map[me->mapx+me->mapy*map->width].floor) && !player.invinc)
			{
				PlayerExplode(map);
				me->dx=0;
				me->dy=0;
				return;
			}
		}

		me->dx=0;
		me->dy=0;

		if(player.wantBomb)
		{
			// tried to drop bomb
			PlayerUseItem(me,map);
			player.boredom=0;
			player.wantBomb=0;
		}

		if(c&(CONTROL_UP|CONTROL_DN|CONTROL_LF|CONTROL_RT))
		{
			player.boredom=0;
			// a direction was pushed, let's move accordingly
			if(c&CONTROL_UP)
				me->facing=6;
			else if(c&CONTROL_DN)
				me->facing=2;
			else if(c&CONTROL_LF)
				me->facing=4;
			else
				me->facing=0;
			
			me->tx=me->x+Cosine(me->facing*32)*TILE_WIDTH;
			me->ty=me->y+Sine(me->facing*32)*TILE_HEIGHT;
			me->tx=me->tx/(TILE_WIDTH*FIXAMT);
			me->ty=me->ty/(TILE_HEIGHT*FIXAMT);
			me->tx=(me->tx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
			me->ty=(me->ty*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
			// check to be sure that direction is open...
			if(me->tx>0 && me->ty>0 && PlayerMoveTo(me,me->tx/(TILE_WIDTH*FIXAMT),me->ty/(TILE_HEIGHT*FIXAMT)))
			{
				curMap->map[(me->x/(TILE_WIDTH*FIXAMT))+(me->y/(TILE_HEIGHT*FIXAMT))*curMap->width].item=ITM_NONE;
				curMap->map[(me->tx/(TILE_WIDTH*FIXAMT))+(me->ty/(TILE_HEIGHT*FIXAMT))*curMap->width].item=ITM_PLAYER;
			}
			else
			{
				me->tx=me->x;
				me->ty=me->y;
				me->dx=0;
				me->dy=0;
				if(me->seq!=ANIM_ATTACK && player.noPushAnim==0)
				{
					me->seq=ANIM_ATTACK;
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=256;
				}
			}
		}
		else
		{
			// no direction is pressed
			PlayerSoundEffect(255);
			player.noPushAnim=0;
		}
	}
	
	if(me->dx!=0 || me->dy!=0)
	{
		player.noPushAnim=0;
		if(me->seq==ANIM_A1 || me->seq==ANIM_IDLE)
			DoMove(me,ANIM_MOVE,128,0,me->dx,me->dy);
		player.boredom=0;
	}
	else
	{
		if(me->seq==ANIM_MOVE)
			DoMove(me,ANIM_IDLE,128,0,0,0);
		if(me->seq==ANIM_ATTACK)
			player.noPushAnim=1;

		player.boredom++;
		if(player.boredom>player.maxBoredom)
		{
			DoMove(me,ANIM_A1,128,0,0,0);
			PlayerSoundEffect(SE_BORED);
			player.boredom=0;
			player.maxBoredom=30*10+Random(30*20);
		}
	}
	oldc=c;
}

void PlayerSortItems(void)
{
	int i;
	byte done;

	done=0;
	while(!done)
	{
		done=1;
		for(i=0;i<8;i++)
		{
			if((player.item[i+1]/8==PE_BOMB || player.item[i+1]/8==PE_ACID ||
				player.item[i+1]/8==PE_LASER || player.item[i+1]/8==PE_DETONATE)
				&& player.item[i]/8==PE_KEY)
			{
				done=player.item[i+1];
				player.item[i+1]=player.item[i];
				player.item[i]=done;
				done=0;
			}
		}
	}
}

void PlayerGetItem(mapTile_t *m,int x,int y)
{
	int i;
	byte type;

	switch(m->item)
	{
		case ITM_PENCIL:
			player.pencils[m->itemInfo%8]++;
			Score_BlowUp(SCORE_PENCIL,m->itemInfo%8);
			PencilFX((x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,m->itemInfo%8);
			m->item=ITM_NONE;
			MakeNormalSound(SND_ITEMGET);
			return;
			break;
		case ITM_BOMB:
			type=PE_BOMB;
			break;
		case ITM_KEY:
			type=PE_KEY;
			break;
		case ITM_DETONATE:
			type=PE_DETONATE;
			break;
		case ITM_SHIELD:
			player.invinc=10*30;
			m->item=ITM_NONE;
			MakeNormalSound(SND_SHIELD);
			return;
			break;
		case ITM_ACID:
			type=PE_ACID;
			break;
		case ITM_LASER:
			type=PE_LASER;
			break;
	}
	for(i=0;i<9;i++)
	{
		if(player.item[i]==0)
		{
			player.item[i]=(m->itemInfo%8)+type*8;
			m->item=ITM_NONE;
			PlayerSortItems();
			MakeNormalSound(SND_ITEMGET);
			return;
		}
	}
	// if you got here, you are full of stuff already, so no pickup, it's an obstacle
}

void PlayerUnlockDoor(mapTile_t *m)
{
	int i,j;

	for(i=0;i<9;i++)
		if((player.item[i]/8)==PE_KEY && 
			((player.item[i]%8)==(m->itemInfo%8) || (player.item[i]%8)==0))
		{
			m->item=ITM_OPENDOOR;
			for(j=i;j<8;j++)
				player.item[j]=player.item[j+1];
			player.item[8]=0;
			MakeNormalSound(SND_SWITCHFLIP);
			return;
		}
}

byte PlayerMoveTo(Guy *me,int x,int y)
{
	mapTile_t *m;

	if(x<0 || y<0 || x>=curMap->width || y>=curMap->height)
		return 0;

	m=&curMap->map[x+y*curMap->width];

	if(m->item==ITM_DOOR)
	{
		PlayerUnlockDoor(m);
	}

	if(!FloorCanWalk(m->floor))
		return 0;

	if(m->item==ITM_KEY || m->item==ITM_BOMB || m->item==ITM_DETONATE || m->item==ITM_SHIELD ||
		m->item==ITM_LASER || m->item==ITM_ACID || m->item==ITM_PENCIL)
	{
		PlayerGetItem(m,x,y);
	}

	// toggle doors
	if(m->item==ITM_SWITCH || m->item==ITM_SWITCH2)
	{
		PushSwitch(curMap,m);
		return 0;
	}
	
	if(m->item==ITM_CRANK)
	{
		TurnCrank(curMap,m);
		return 0;
	}

	if(m->item==0)
	{
		me->dx=Cosine(me->facing*32)*8;
		me->dy=Sine(me->facing*32)*6;
		PlayerSoundEffect(SE_STEP);
		return 1;
	}
	else
	{
		if(PushObject(x,y,me,m,curMap))
		{
			me->dx=Cosine(me->facing*32)*4;
			me->dy=Sine(me->facing*32)*3;
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
			PlayerSoundEffect(SE_PUSH);
			return 1;
		}
		else
			PlayerSoundEffect(SE_BUMP);
	}
	return 0;
}

void PlayerExplode(Map *map)
{
	int tx,ty;
	Guy *g;

	if(player.invinc)
		return;	// no dying now!

	if(goodguy->type!=MONS_LOONY)
		return;

	if(goodguy->seq==ANIM_DIE)
		return;

	tx=goodguy->x/(TILE_WIDTH*FIXAMT);
	ty=goodguy->y/(TILE_HEIGHT*FIXAMT);

	PlayerSoundEffect(SE_DIE);
	goodguy->seq=ANIM_DIE;
	goodguy->action=ACTION_BUSY;
	goodguy->frm=0;
	goodguy->frmAdvance=128;
	goodguy->frmTimer=0;
	goodguy->facing=2;
	goodguy->dx=0;
	goodguy->dy=0;
	g=AddGuy((tx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
			 (ty*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,
			 0,MONS_BOOM);
	g->color=0;
	map->map[tx+ty*map->width].item=ITM_ACTIVEBOMB;
}

void PlayerSoundEffect(byte fx)
{
	static byte flip=1;
	byte sndTable[]={SND_SURFSTEP-SND_ROBOTSTEP,	// surfboy
				     SND_GOTHSTEP-SND_ROBOTSTEP,	// goth
				     SND_ROBOTSTEP-SND_ROBOTSTEP,	// robot
				     SND_STICKSTEP-SND_ROBOTSTEP,	// stick
				     SND_SANTASTEP-SND_ROBOTSTEP,	// santa
				     SND_CHERUBSTEP-SND_ROBOTSTEP,	// cherub
				     SND_SHROOMSTEP-SND_ROBOTSTEP,	// shroom
				     SND_PIGSTEP-SND_ROBOTSTEP,		// pig
				     SND_FREAKSTEP-SND_ROBOTSTEP,	// freak
					 SND_HIPSTEP-SND_ROBOTSTEP,		// hippie
	};

	if(fx==SE_STEP)
	{
		if(flip)
		{
			MakeNormalSound(SND_ROBOTSTEP+sndTable[profile.character]);
			flip=0;
			return;
		}
	}
	if(fx==SE_PUSH)
		MakeNormalSound(SND_ROBOTPUSH+sndTable[profile.character]);
	if(fx==SE_DIE)
		MakeNormalSound(SND_ROBOTDIE+sndTable[profile.character]);
	if(fx==SE_BORED)
		MakeNormalSound(SND_ROBOTBORED+sndTable[profile.character]);

	if(!flip)
		flip=1;
}

void SetPlayerNoBomb(void)
{
	oldc=255;
}
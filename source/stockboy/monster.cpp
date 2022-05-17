#include "monster.h"
#include "player.h"
#include "options.h"
#include "bullet.h"
#include "pushing.h"
#include "floor.h"
#include "profile.h"

#define BLOB_WAIT_TIME 30
#define BLOB_TURN_TIME 15
#define BUNNY_WAIT_TIME 5
#define BUNNY_TURN_TIME 2
#define INCHY_WAIT_TIME 10
#define INCHY_TURN_TIME 5
#define PIGGY_WAIT_TIME	5

monsterType_t monsType[NUM_MONSTERS]=
	{
		{"Null",0,
		 0,"",0,0,0,0,0,0},
		{"SurfBoy",7,
		 30,"graphics\\stockboy.jsp",0,MF_WATERWALK,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,255},	// attack
				{22,23,24,25,26,27,28,29,29,29,29,29,29,29,29,255},		// die
				{15,16,17,18,19,20,21,20,19,18,19,20,21,20,19,18,18,17,16,15,255},		// A1 = shrug
			}},
		{"Ball",6,		// this number indicates which color to replace when recoloring
		 8,"graphics\\ball.jsp",0,MF_ONEFACE,
			{
				{0,1,2,3,4,5,6,7,255},	// idle=roll right
				{8,9,10,11,12,13,14,15,255},	// move=roll down
				{7,6,5,4,3,2,1,0,255},	// attack=roll left
				{0,255},	// die
				{15,14,13,12,11,10,9,8,255},	// A1=roll up
			}},
		{"Bomb",0,
		 10,"graphics\\bomb.jsp",0,MF_ONEFACE,
			{
				{0,1,255},	// idle=sizzle
				{0,255},	// move=unused
				{0,255},	// attack=unused
				{0,255},	// die
			}},
		{"Boom",0,
		 10,"!3",0,MF_ONEFACE|MF_GLOW|MF_NOSHADOW,
			{
				{2,3,4,5,6,7,8,9,9,9,255},	// idle=BOOM!!
				{0,255},	// move=unused
				{0,255},	// attack=unused
				{0,255},	// die
			}},
		{"Blobby",3,
		 9,"graphics\\blob.jsp",0,0,
			{
				{0,1,2,3,2,1,255},	// idle
				{4,5,4,6,7,8,7,6,5,4,0,255},	// move
				{0,255},	// attack=unused
				{0,255},	// die
			}},
		{"Fluffy",6,
		 5,"graphics\\bunny.jsp",0,0,
			{
				{0,255},	// idle
				{1,2,2,1,0,0,3,4,4,3,0,0,255},	// move
				{0,255},	// attack=unused
				{0,255},	// die
			}},
		{"Inchy",3,
		 11,"graphics\\inchy.jsp",0,0,
			{
				{0,255},	// idle
				{1,2,3,4,5,6,7,8,9,10,0,255},	// move
				{0,255},	// attack=unused
				{0,255},	// die
			}},
		{"Piggy",6,
		 7,"graphics\\piggy.jsp",0,0,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{0,255},	// attack=unused
				{0,255},	// die
			}},
		{"SurfBoy",7,
		 30,"graphics\\stockboy.jsp",0,MF_WATERWALK,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,255},	// attack
				{22,23,24,25,26,27,28,29,29,29,29,29,29,29,29,255},		// die
				{15,16,17,18,19,20,21,20,19,18,19,20,21,20,19,18,18,17,16,15,255},		// A1 = shrug
			}},
		{"GothGirl",6,
		 27,"graphics\\goth.jsp",0,MF_WATERWALK,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,255},	// attack
				{20,21,22,23,24,25,26,26,26,26,26,26,26,26,26,255},		// die
				{15,16,17,18,19,19,19,19,19,19,19,19,19,19,18,17,16,15,255},		// A1 = shrug
			}},
		{"Ro-Bot",5,
		 35,"graphics\\robot.jsp",0,MF_WATERWALK,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,255},	// attack
				{29,30,31,32,33,34,34,34,34,34,34,34,34,34,34,255},		// die
				{15,16,17,18,19,20,21,22,23,24,25,26,27,28,255},		// A1 = shrug
			}},
		{"Happy Stick Boy",7,
		 12,"graphics\\stick.jsp",0,MF_WATERWALK,
			{
				{0,255},	// idle
				{1,0,2,0,255},	// move
				{6,7,6,7,6,7,6,7,255},	// attack
				{ 8, 8, 8, 8, 8, 8, 8, 9, 9,10,10,11,11,254,254,255},		// die
				{3,3,3,4,5,5,5,4,5,4,5,5,5,4,4,3,3,255},		// A1 = shrug
			}},
		{"Santa Claus",4,
		 32,"graphics\\santa.jsp",0,MF_WATERWALK,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{11,12,13,14,15,16,17,255},	// attack
				{7,7,8,8,9,9,10,10,10,10,10,10,10,10,10,255},		// die
				{18,19,20,21,22,23,23,23,23,24,25,26,27,28,29,30,31,255},		// A1 = shrug
			}},
		{"Cherub",5,
		 47,"graphics\\cherub.jsp",0,MF_WATERWALK|MF_FLYING,
			{
				{0,1,2,3,2,1,0,4,5,6,5,4,255},	// idle
				{7,8,9,10,9,8,7,11,12,13,12,11,255},	// move
				{14,15,16,17,18,19,20,21,22,23,24,255},	// attack
				{39,40,41,42,43,44,45,46,46,46,46,46,46,46,255},		// die
				{25,26,27,28,29,30,31,32,33,34,35,36,37,38,255},		// A1 = shrug
			}},
		{"Shroom",5,
		 21,"graphics\\shroom.jsp",0,MF_WATERWALK,
			{
				{0,255},	// idle
				{1,2,3,2,1,0,4,5,6,5,4,0,255},	// move
				{7,8,9,10,11,12,13,14,255},	// attack
				{15,16,17,18,19,20,20,20,20,20,20,20,20,20,255},		// die
				{7,8,9,10,9,8,7,255},		// A1 = shrug
			}},
		{"War Pig",1,
		 19,"graphics\\warpig.jsp",0,MF_WATERWALK,
			{
				{0,255},	// idle
				{6,7,6,0,8,9,8,0,255},	// move
				{10,11,12,12,13,13,13,14,14,14,255},	// attack
				{15,16,17,18,18,18,18,18,18,18,18,18,18,18,255},		// die
				{1,2,3,4,5,4,3,4,5,4,3,2,1,0,255},		// A1 = shrug
			}},
		{"Freako",4,
		 24,"graphics\\freako.jsp",0,MF_WATERWALK,
			{
				{0,255},	// idle
				{0,1,2,3,2,1,0,4,5,6,5,4,255},	// move
				{7,8,9,10,11,12,13,14,15,255},	// attack
				{16,17,18,19,20,21,22,23,23,23,23,23,23,23,255},		// die
				{16,17,18,19,18,17,16,255},		// A1 = shrug
			}},
		{"Hippie",5,
		 27,"graphics\\hippie.jsp",0,MF_WATERWALK,
			{
				{0,255},	// idle
				{0,1,2,3,2,1,0,4,5,6,5,4,255},	// move
				{7,8,9,10,11,12,13,14,255},	// attack
				{20,21,22,23,24,25,26,26,26,26,26,26,26,26,255},		// die
				{15,16,17,18,19,19,19,19,19,19,18,17,16,15,255},		// A1 = shrug
			}},
};

void InitMonsters(void)
{
	int i;

	for(i=0;i<NUM_MONSTERS;i++)
	{
		monsType[i].spr=NULL;
	}
}

void ExitMonsters(void)
{
	int i;

	for(i=1;i<NUM_MONSTERS;i++)
	{
		if(monsType[i].spr)
			delete monsType[i].spr;
		monsType[i].spr=NULL;
	}
}

void SetupPlayerSprite(void)
{
	monsType[MONS_LOONY]=monsType[profile.character+MONS_SURFBOY];
	if(monsType[MONS_LOONY].spr)
	{
		delete monsType[MONS_LOONY].spr;
		monsType[MONS_LOONY].spr=NULL;
		monsType[profile.character+MONS_SURFBOY].spr=NULL;
	}
}

void PurgeMonsterSprites(void)
{
	int i;

	for(i=0;i<NUM_MONSTERS;i++)
	{
		if(monsType[i].sprName[0]=='!')
			monsType[i].spr=NULL;	// it is sharing with someone else

		if(monsType[i].spr)
			delete monsType[i].spr;
		monsType[i].spr=NULL;
	}
}

byte *MonsterAnim(byte type,byte anim)
{
	return monsType[type].anim[anim];
}

word MonsterFlags(byte type)
{
	return monsType[type].flags;
}

byte MonsterFrames(byte type)
{
	return monsType[type].framesPerDir;
}

byte MonsterColor(byte type)
{
	return monsType[type].srcColor;
}

char *MonsterName(byte type)
{
	return monsType[type].name;
}

void SetMonsterFlags(byte type,word flags)
{
	monsType[type].flags=flags;
}

void LoadMonsterSprite(byte type)
{
	dword start,end;
	int t;

	start=timeGetTime();
	// load if not loaded
	if(monsType[type].spr==NULL)
	{
		if(monsType[type].sprName[0]=='!')
		{
			// steal another monster's sprites
			t=atoi(&monsType[type].sprName[1]);
			if(monsType[t].spr==NULL)
			{
				monsType[t].spr=new sprite_set_t(monsType[t].sprName);
				if(monsType[t].spr==NULL)
					FatalError("Out of memory!");
			}
			monsType[type].spr=monsType[t].spr;
		}
		else
		{
			monsType[type].spr=new sprite_set_t(monsType[type].sprName);
			if(monsType[type].spr==NULL)
				FatalError("Out of memory!");
		}
	}
	end=timeGetTime();
	AddGarbageTime(end-start);
}

sprite_t *GetMonsterSprite(byte type,byte seq,byte frm,byte facing)
{
	int v;

	LoadMonsterSprite(type);

	v=monsType[type].anim[seq][frm];

	if(v==254)
		return NULL;	// 254 means no sprite for this frame

	if(!(monsType[type].flags&MF_ONEFACE))
		v+=(facing/2)*monsType[type].framesPerDir;

	if(monsType[type].flags&MF_FACECMD)
		v+=facing;

	return monsType[type].spr->GetSprite(v);
}

void MonsterDraw(int x,int y,int z,byte type,byte seq,byte frm,byte facing,byte dstColor,char bright,byte ouch,int ice)
{
	sprite_t *curSpr;
	int v;
	byte isBouapha;

	if(type==player.monsType)
	{
		isBouapha=1;
	}
	else
		isBouapha=0;

	LoadMonsterSprite(type);

	if(type==MONS_BOOM)
		return;

	v=monsType[type].anim[seq][frm];

	if(v==254)
		return;	// don't draw this frame

	if(!(monsType[type].flags&MF_ONEFACE))
		v+=(facing/2)*monsType[type].framesPerDir;

	if(isBouapha)
	{
		if(player.invinc&1)
		{
			curSpr=monsType[type].spr->GetSprite(v);
			if(!curSpr)
				return;
			SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,255,bright,curSpr,DISPLAY_DRAWME|DISPLAY_GLOW);
			return;
		}
	}

	if(monsType[type].flags&MF_FACECMD)
		v+=facing;

	curSpr=monsType[type].spr->GetSprite(v);
	if(!curSpr)
		return;

	if(!(monsType[type].flags&MF_NOSHADOW))
		SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,0,255,0,curSpr,DISPLAY_DRAWME|DISPLAY_SHADOW);

	if(ouch==0)
	{
		if(ice)
		{
			SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,7,bright,curSpr,DISPLAY_DRAWME);
		}
		else
		{
			if(!(monsType[type].flags&(MF_GHOST|MF_GLOW)))
			{
				if(type!=MONS_LOONY)
					OffSprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,monsType[type].srcColor,dstColor,
								bright,curSpr,DISPLAY_DRAWME|DISPLAY_OFFCOLOR|DISPLAY_COLORBLIND);
				else
					OffSprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,monsType[type].srcColor,dstColor,
								bright,curSpr,DISPLAY_DRAWME|DISPLAY_OFFCOLOR);
			}
			else if(monsType[type].flags&MF_GHOST)
				SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,255,bright,
						curSpr,DISPLAY_DRAWME|DISPLAY_GHOST);
			else if(monsType[type].flags&MF_GLOW)
				SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,255,bright,
						curSpr,DISPLAY_DRAWME|DISPLAY_GLOW);
		}
	}
	else
		SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,4,bright,curSpr,DISPLAY_DRAWME);
}

void InstaRenderMonster(int x,int y,byte type,byte facing,byte color,char bright,MGLDraw *mgl)
{
	sprite_t *curSpr;
	int v;

	LoadMonsterSprite(type);

	v=monsType[type].anim[ANIM_IDLE][0];
	if(!(monsType[type].flags&MF_ONEFACE))
		v+=facing*monsType[type].framesPerDir;

	curSpr=monsType[type].spr->GetSprite(v);
	if(!curSpr)
		return;

	curSpr->DrawOffColor(x,y,mgl,monsType[type].srcColor,color,bright);
}

void InstaRenderMonsterShadow(int x,int y,byte type,byte facing,MGLDraw *mgl)
{
	sprite_t *curSpr;
	int v;

	LoadMonsterSprite(type);

	v=monsType[type].anim[ANIM_IDLE][0];
	if(!(monsType[type].flags&MF_ONEFACE))
		v+=facing*monsType[type].framesPerDir;

	curSpr=monsType[type].spr->GetSprite(v);
	if(!curSpr)
		return;

	curSpr->DrawShadow(x,y,mgl);
}

void UpdateMonsterTypes(void)
{
}

// AI auxiliary functions to make it simple
//---------------------------------------------

inline void FaceGoodguy(Guy *me,Guy *goodguy)
{
	if(goodguy->x<me->x-FIXAMT*16)
	{
		if(goodguy->y<me->y-FIXAMT*16)
			me->facing=5;
		else if(goodguy->y>me->y+FIXAMT*16)
			me->facing=3;
		else
			me->facing=4;
	}
	else if(goodguy->x>me->x+FIXAMT*16)
	{
		if(goodguy->y<me->y-FIXAMT*16)
			me->facing=7;
		else if(goodguy->y>me->y+FIXAMT*16)
			me->facing=1;
		else
			me->facing=0;
	}
	else
	{
		if(goodguy->y<me->y-FIXAMT*16)
			me->facing=6;
		else if(goodguy->y>me->y+FIXAMT*16)
			me->facing=2;
	}
}

inline int RangeToTarget(Guy *me,Guy *goodguy)
{
	return abs(me->x-goodguy->x)+abs(me->y-goodguy->y);
}

inline int RangeToCoords(Guy *me,int x,int y)
{
	return abs(me->x-x)+abs(me->y-y);
}

// this version doesn't insta-face, it rotates toward the right facing, and it has much
// more leeway than the 16 pixels of the other (it's for bigger creatures)
inline void FaceGoodguy2(Guy *me,Guy *goodguy)
{
	int desired;
	int diff,dir;

	if(goodguy->x<me->x-FIXAMT*32)
	{
		if(goodguy->y<me->y-FIXAMT*32)
			desired=5;
		else if(goodguy->y>me->y+FIXAMT*32)
			desired=3;
		else
			desired=4;
	}
	else if(goodguy->x>me->x+FIXAMT*32)
	{
		if(goodguy->y<me->y-FIXAMT*32)
			desired=7;
		else if(goodguy->y>me->y+FIXAMT*32)
			desired=1;
		else
			desired=0;
	}
	else
	{
		if(goodguy->y<me->y-FIXAMT*32)
			desired=6;
		else
			desired=2;
	}

	if(desired==me->facing)
		return;

	if(desired>me->facing)
	{
		diff=desired-me->facing;
		if(diff>4)
		{
			dir=-1;
			diff=8-diff;
		}
		else
			dir=1;
	}
	else
	{
		diff=me->facing-desired;
		if(diff>4)
		{
			dir=1;
			diff=8-diff;
		}
		else
			dir=-1;
	}
	me->facing=(me->facing+dir)&7;
}

// for evil pumps... extra leeway and range
inline void FaceGoodguy3(Guy *me,Guy *goodguy)
{
	int xamt,yamt;

	xamt=10*FIXAMT+abs(me->x-goodguy->x)/10;
	yamt=10*FIXAMT+abs(me->y-goodguy->y)/10;

	if(goodguy->x<me->x-yamt)
	{
		if(goodguy->y<me->y-xamt)
			me->facing=5;
		else if(goodguy->y>me->y+xamt)
			me->facing=3;
		else
			me->facing=4;
	}
	else if(goodguy->x>me->x+yamt)
	{
		if(goodguy->y<me->y-xamt)
			me->facing=7;
		else if(goodguy->y>me->y+xamt)
			me->facing=1;
		else
			me->facing=0;
	}
	else
	{
		if(goodguy->y<me->y-xamt)
			me->facing=6;
		else if(goodguy->y>me->y+xamt)
			me->facing=2;
	}
}

void BasicAI(Guy *me,byte ouchSound,byte dieSound,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(ouchSound,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(dieSound,me->x,me->y,SND_CUTOFF,1200);
	}
}

void DoMove(Guy *me,int move,int frmAdv,byte busy,int dx,int dy)
{
	me->seq=move;
	me->frm=0;
	me->frmTimer=0;
	me->frmAdvance=frmAdv;
	if(busy)
		me->action=ACTION_BUSY;
	else
		me->action=ACTION_IDLE;
	me->dx=dx;
	me->dy=dy;
}

byte CheckMovement(Guy *me)
{
	if(me->dx>0 && me->x>me->tx)
		me->x=me->tx;
	if(me->dx<0 && me->x<me->tx)
		me->x=me->tx;
	if(me->dy>0 && me->y>me->ty)
		me->y=me->ty;
	if(me->dy<0 && me->y<me->ty)
		me->y=me->ty;

	return (me->x==me->tx && me->y==me->ty);
}

// here be the AIs for each monster type
//--------------------------------------------------------------------------------------

void AI_Ball(Guy *me,Map *map,world_t *world)
{
	int dxt[]={1,0,-1,0};
	int dyt[]={0,1,0,-1};
	byte b;

	if(!CheckMovement(me))
		return;

	// otherwise we have reached the next tile
	map->GoodieCheck(me->mapx,me->mapy,GOODIE_PUSH);

	if(FloorIsHole(map->map[me->mapx+me->mapy*map->width].floor))
	{
		player.dumpedObj[me->color]++;
		me->type=MONS_NONE;
		map->map[me->mapx+me->mapy*map->width].item=ITM_NONE;
		map->map[me->mapx+me->mapy*map->width].floor+=20;	// fill the hole
		SmokePuff(me->x,me->y,2);
		MakeSound(SND_FALLINHOLE,me->x,me->y,SND_CUTOFF,100);
		return;
	}
	if(FloorIsPit(map->map[me->mapx+me->mapy*map->width].floor))
	{
		player.dumpedObj[me->color]++;
		me->type=MONS_NONE;
		map->map[me->mapx+me->mapy*map->width].item=ITM_NONE;
		SmokePuff(me->x,me->y,2);
		// ITEM FALL IN PIT!!!
		MakeSound(SND_FALLINPIT,me->x,me->y,SND_CUTOFF,100);
		return;
	}
	if(FloorIsTeleporter(map->map[me->mapx+me->mapy*map->width].floor))
	{
		int telex,teley;

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
			map->map[me->mapx+me->mapy*map->width].item=ITM_MOVINGBALL;
			// add teleport FX
			MakeSound(SND_TELEPORT,me->x,me->y,SND_CUTOFF,100);
		}
		else
		{
			// fail teleport effect
			MakeSound(SND_NOTELEPORT,me->x,me->y,SND_CUTOFF,100);
		}
	}

	// first let's check for arrows underneath us to change our direction
	b=FloorArrowFacing(map->map[me->mapx+me->mapy*map->width].floor);
	if(b!=NOT_AN_ARROW)
	{
		// it is an arrow, change facing
		me->facing=b;
		me->dx=dxt[b/2]*FIXAMT*8;
		me->dy=dyt[b/2]*FIXAMT*6;
	}

	me->tx=me->mapx+dxt[me->facing/2];
	me->ty=me->mapy+dyt[me->facing/2];

	map->map[me->mapx+me->mapy*map->width].item=ITM_BALL;
	map->map[me->mapx+me->mapy*map->width].itemInfo=me->color;

	if(!CanBePushedTo(me->mapx,me->mapy,me->tx,me->ty,map))
	{
		// can't go any further
		me->type=MONS_NONE;
		// just leave the ball here, since we already did
		return;
	}
	else
	{
		// we can keep going
		map->map[me->mapx+me->mapy*map->width].item=ITM_NONE;
		map->map[me->tx+me->ty*map->width].item=ITM_MOVINGBALL;
		map->map[me->tx+me->ty*map->width].itemInfo=me->color;

		me->tx=(me->tx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
		me->ty=(me->ty*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
	}
}

void AI_Bomb(Guy *me,Map *map,world_t *world)
{
	if(Random(2)==0)
		MiniBurn(me->x-FIXAMT*10,me->y,FIXAMT*16);

	if(me->mind==0)
		MakeTextParticle(me->x/FIXAMT,me->y/FIXAMT,TXT_5);
	else if(me->mind==30)
		MakeTextParticle(me->x/FIXAMT,me->y/FIXAMT,TXT_4);
	else if(me->mind==30*2)
		MakeTextParticle(me->x/FIXAMT,me->y/FIXAMT,TXT_3);
	else if(me->mind==30*3)
		MakeTextParticle(me->x/FIXAMT,me->y/FIXAMT,TXT_2);
	else if(me->mind==30*4)
		MakeTextParticle(me->x/FIXAMT,me->y/FIXAMT,TXT_1);
	else if(me->mind==30*5)
	{
		// KABOOM!!
		me->type=MONS_BOOM;
		DoMove(me,ANIM_IDLE,256,0,0,0);
		me->mind=0;
		me->frm=(byte)Random(4);
		return;
	}
	if(Random(2)==0)
		MakeSound(SND_BOMBFIZZ,me->x,me->y,SND_CUTOFF,100);
	else if(Random(4)==0)
		MakeSound(SND_BOMBFIZZ2,me->x,me->y,SND_CUTOFF,100);

	me->mind++;
	me->frmAdvance=256;
}

void AI_Boom(Guy *me,Map *map,world_t *world)
{
	me->frmAdvance=256;

	if(me->frm==4)
		MakeSound(SND_BOMBBOOM+Random(3),me->x,me->y,SND_CUTOFF,1200);

	if(me->mind==0)
	{
		player.destructionBonus++;
		Explosion(me->x,me->y,me->z,me->color);
		map->GoodieCheck(me->mapx,me->mapy,GOODIE_BOOM);
		me->mind=1;
	}
	if(me->frm==8)
	{
		BlowUpNeighbors(map,me->mapx,me->mapy,me->color);
		me->type=MONS_NONE;
		map->map[me->mapx+me->mapy*map->width].item=ITM_NONE;
	}
}

void AI_Blobby(Guy *me,Map *map,world_t *world)
{
	if(me->mind)
	{
		// sit and think
		if(me->mind==BLOB_TURN_TIME)
			me->facing=(byte)Random(4)*2;	// pick a new random facing shortly before becoming active
		me->mind--;
		return;
	}

	if(CheckMovement(me))
	{
		if(!MonsterAtDestination(me,map))
			return;

		me->mind2=1;
		if(me->mind1==0)
		{
			me->mind=BLOB_WAIT_TIME;
			me->mind1=1;	// tell it not to wait next time around
			me->dx=0;
			me->dy=0;
		}
		else
		{
			me->mind1=0;

			me->dx=0;
			me->dy=0;

			// standing at destination
			if(MonsterMoveTo(me,me->mapx+GetDX(me->facing/2),me->mapy+GetDY(me->facing/2)))
			{
				me->dx=0;
				me->dy=0;
				me->tx=me->x+TILE_WIDTH*FIXAMT*GetDX(me->facing/2);
				me->ty=me->y+TILE_HEIGHT*FIXAMT*GetDY(me->facing/2);
				me->dx=Cosine(me->facing*32)*4;
				me->dy=Sine(me->facing*32)*3;
				DoMove(me,ANIM_MOVE,256,0,me->dx,me->dy);
				MakeSound(SND_WALK_BLOBBY,me->x,me->y,SND_CUTOFF,10);
				map->map[me->mapx+me->mapy*map->width].item=ITM_NONE;
				me->tgtx=me->mapx+GetDX(me->facing/2);
				me->tgty=me->mapy+GetDY(me->facing/2);
				map->map[me->mapx+GetDX(me->facing/2)+(me->mapy+GetDY(me->facing/2))*map->width].item=ITM_MONS1;
				map->map[me->mapx+GetDX(me->facing/2)+(me->mapy+GetDY(me->facing/2))*map->width].itemInfo=me->color;
			}
			else
			{
				me->tgtx=me->mapx;
				me->tgty=me->mapy;
				me->mind=BLOB_WAIT_TIME;
				me->mind1=1;	// tell it not to set the dx,dy
			}
		}
	}
	else
		me->mind2=0;

	if(me->seq==ANIM_IDLE)
		me->frmAdvance=64;
}

void AI_Bunny(Guy *me,Map *map,world_t *world)
{
	if(me->mind)
	{
		// sit and think
		if(me->mind==BUNNY_TURN_TIME)
			me->facing=(me->facing+2)&7;	// pick a new random facing shortly before becoming active
		me->mind--;
		return;
	}

	if(me->seq!=ANIM_MOVE)
		DoMove(me,ANIM_MOVE,128,0,me->dx,me->dy);

	if(CheckMovement(me))
	{
		me->mind2=1;
		me->dx=0;
		me->dy=0;

		if(!MonsterAtDestination(me,map))
			return;

		// standing at destination
		if(MonsterMoveTo(me,me->mapx+GetDX(me->facing/2),me->mapy+GetDY(me->facing/2)))
		{
			me->tx=me->x+TILE_WIDTH*FIXAMT*GetDX(me->facing/2);
			me->ty=me->y+TILE_HEIGHT*FIXAMT*GetDY(me->facing/2);
			me->dx=Cosine(me->facing*32)*2;
			me->dy=Sine(me->facing*32)*3/2;
			map->map[me->mapx+me->mapy*map->width].item=ITM_NONE;
			me->tgtx=me->mapx+GetDX(me->facing/2);
			me->tgty=me->mapy+GetDY(me->facing/2);
			map->map[me->mapx+GetDX(me->facing/2)+(me->mapy+GetDY(me->facing/2))*map->width].item=ITM_MONS2;
			map->map[me->mapx+GetDX(me->facing/2)+(me->mapy+GetDY(me->facing/2))*map->width].itemInfo=me->color;
		}
		else
		{
			DoMove(me,ANIM_IDLE,64,0,0,0);
			me->mind=BUNNY_WAIT_TIME;
			MakeSound(SND_MONS_BUMP,me->x,me->y,SND_ONE,10);
		}
	}
	else
		me->mind2=0;

	if(me->seq==ANIM_IDLE)
		me->frmAdvance=64;
}

void AI_Inchy(Guy *me,Map *map,world_t *world)
{
	if(me->mind)
	{
		// sit and think
		if(me->mind==INCHY_TURN_TIME)
			me->facing=(me->facing+4)&7;	// about-face!
		me->mind--;
		return;
	}

	if(me->seq!=ANIM_MOVE)
		DoMove(me,ANIM_MOVE,128,0,me->dx,me->dy);

	if(CheckMovement(me))
	{
		me->mind2=1;
		me->dx=0;
		me->dy=0;

		if(!MonsterAtDestination(me,map))
			return;

		// standing at destination
		if(MonsterMoveTo(me,me->mapx+GetDX(me->facing/2),me->mapy+GetDY(me->facing/2)))
		{
			me->tx=me->x+TILE_WIDTH*FIXAMT*GetDX(me->facing/2);
			me->ty=me->y+TILE_HEIGHT*FIXAMT*GetDY(me->facing/2);
			me->dx=Cosine(me->facing*32)*2;
			me->dy=Sine(me->facing*32)*3/2;
			map->map[me->mapx+me->mapy*map->width].item=ITM_NONE;
			me->tgtx=me->mapx+GetDX(me->facing/2);
			me->tgty=me->mapy+GetDY(me->facing/2);
			map->map[me->mapx+GetDX(me->facing/2)+(me->mapy+GetDY(me->facing/2))*map->width].item=ITM_MONS3;
			map->map[me->mapx+GetDX(me->facing/2)+(me->mapy+GetDY(me->facing/2))*map->width].itemInfo=me->color;
			MakeSound(SND_WALK_INCHY,me->x,me->y,SND_CUTOFF,10);
		}
		else
		{
			DoMove(me,ANIM_IDLE,64,0,0,0);
			me->mind=INCHY_WAIT_TIME;
			MakeSound(SND_MONS_BUMP,me->x,me->y,SND_ONE,10);
		}
	}
	else
		me->mind2=0;

	if(me->seq==ANIM_IDLE)
		me->frmAdvance=64;
}

void AI_Piggy(Guy *me,Map *map,world_t *world)
{
	byte f;

	if(me->mind)
	{
		me->mind--;
		return;
	}

	if(me->seq!=ANIM_MOVE)
		DoMove(me,ANIM_MOVE,256,0,me->dx,me->dy);

	if(CheckMovement(me))
	{
		me->mind2=1;
		me->dx=0;
		me->dy=0;

		if(abs(me->x-goodguy->x)>abs(me->y-goodguy->y))
		{
			if(me->x>goodguy->x)
				me->facing=4;
			else
				me->facing=0;
		}
		else
		{
			if(me->y>goodguy->y)
				me->facing=6;
			else
				me->facing=2;
		}

		if(!MonsterAtDestination(me,map))
			return;

		// standing at destination
		if(MonsterMoveTo(me,me->mapx+GetDX(me->facing/2),me->mapy+GetDY(me->facing/2)))
		{
			me->tx=me->x+TILE_WIDTH*FIXAMT*GetDX(me->facing/2);
			me->ty=me->y+TILE_HEIGHT*FIXAMT*GetDY(me->facing/2);
			me->dx=Cosine(me->facing*32)*2;
			me->dy=Sine(me->facing*32)*3/2;
			map->map[me->mapx+me->mapy*map->width].item=ITM_NONE;
			me->tgtx=me->mapx+GetDX(me->facing/2);
			me->tgty=me->mapy+GetDY(me->facing/2);
			map->map[me->mapx+GetDX(me->facing/2)+(me->mapy+GetDY(me->facing/2))*map->width].item=ITM_MONS4;
			map->map[me->mapx+GetDX(me->facing/2)+(me->mapy+GetDY(me->facing/2))*map->width].itemInfo=me->color;
		}
		else
		{
			if(me->facing==0 || me->facing==4)	// tried to move horizontally and failed
			{
				if(me->y>goodguy->y)
					me->facing=6;
				else if(me->y<goodguy->y)
					me->facing=2;
				else
				{	// truly stuck
					DoMove(me,ANIM_IDLE,64,0,0,0);
					me->mind=PIGGY_WAIT_TIME;
					me->tgtx=me->mapx;
					me->tgty=me->mapy;
					return;
				}
			}
			else	// tried to move vertically and failed
			{
				if(me->x>goodguy->x)
					me->facing=4;
				else if(me->x<goodguy->x)
					me->facing=0;
				else
				{	// truly stuck
					DoMove(me,ANIM_IDLE,64,0,0,0);
					me->mind=PIGGY_WAIT_TIME;
					me->tgtx=me->mapx;
					me->tgty=me->mapy;
					return;
				}
			}

			f=FloorArrowFacing(map->map[me->mapx+me->mapy*map->width].floor);
			if(f!=NOT_AN_ARROW)
			{
				me->facing=f;
			}

			// now try moving in this new direction
			if(MonsterMoveTo(me,me->mapx+GetDX(me->facing/2),me->mapy+GetDY(me->facing/2)))
			{
				me->tx=me->x+TILE_WIDTH*FIXAMT*GetDX(me->facing/2);
				me->ty=me->y+TILE_HEIGHT*FIXAMT*GetDY(me->facing/2);
				me->dx=Cosine(me->facing*32)*2;
				me->dy=Sine(me->facing*32)*3/2;
				map->map[me->mapx+me->mapy*map->width].item=ITM_NONE;
				me->tgtx=me->mapx+GetDX(me->facing/2);
				me->tgty=me->mapy+GetDY(me->facing/2);
				map->map[me->mapx+GetDX(me->facing/2)+(me->mapy+GetDY(me->facing/2))*map->width].item=ITM_MONS4;
				map->map[me->mapx+GetDX(me->facing/2)+(me->mapy+GetDY(me->facing/2))*map->width].itemInfo=me->color;
			}
			else
			{
				// not even that worked
				DoMove(me,ANIM_IDLE,64,0,0,0);
				me->mind=PIGGY_WAIT_TIME;
				me->tgtx=me->mapx;
				me->tgty=me->mapy;
			}
		}
	}
	else
		me->mind2=0;

	if(me->seq==ANIM_IDLE)
		me->frmAdvance=64;
}

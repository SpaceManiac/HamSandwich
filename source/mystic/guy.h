#ifndef GUY_H
#define GUY_H

#include "winpch.h"
#include "map.h"
#include "monster.h"
#include "jamulspr.h"
#include "display.h"

#define ACTION_IDLE	0
#define ACTION_BUSY 1

#define NIGHTMAREHP	(20)
#define NIGHTMAREDMG (10)

class Guy
{
	public:
		Guy(void);
		~Guy(void);

		void Update(Map *map,world_t *world);
		void EditorUpdate(Map *map);
		void NoMoveUpdate(Map *map);
		void OverworldUpdate(Map *map,world_t *world);
		void Render(byte light);
		byte CanWalk(int xx,int yy,Map *map,world_t *world);
		void NextFrame(void);
		void SeqFinished(void);
		void MonsterControl(Map *map,world_t *world);
		void OverworldControl(Map *map,world_t *world);
		byte CoconutBonk(int xx,int yy,Guy *him);
		byte AttackCheck(byte size,int xx,int yy,Guy *him);
		byte AttackCheck2(int xx,int yy,int xx2,int yy2,Guy *him);
		void AttackThem(void);
		void GetShot(int dx,int dy,int damage,Map *map,world_t *world);
		void CalculateRect(void);

		int x,y,z;
		int dx,dy,dz;
		byte mapx,mapy;
		byte facing;

		byte mind;		// brain variables for AI
		byte mind1;

		byte reload;

		byte ouch;
		byte action;
		word frmTimer;
		word frmAdvance;
		byte frm;
		byte seq;
		char bright;
		word frozen;	// when non-zero, monster is encased in ice for a time

		byte placed;	// if this monster was placed on the overworld, this is 1- makes it immobile
		Guy *parent;
		int hp;
		byte type;
		int rectx,recty,rectx2,recty2;	// for collision checks
		word ID;	// just a copy of the guy's number
};

void InitGuys(int max);
void ExitGuys(void);
void UpdateGuys(Map *map,world_t *world);
void OverworldUpdateGuys(Map *map,world_t *world);
void EditorUpdateGuys(Map *map);
void NoMoveUpdateGuys(Map *map);
void RenderGuys(byte light);
Guy *AddGuy(int x,int y,int z,byte type);
void DeleteGuy(int x,int y,byte type);
void AddMapGuys(Map *map);
byte FindVictim(int x,int y,byte size,int dx,int dy,int damage,Map *map,world_t *world);
byte FindVictims(int x,int y,byte size,int dx,int dy,int damage,Map *map,world_t *world);
byte FindGoodVictim(int x,int y,byte size,int dx,int dy,int damage,Map *map,world_t *world);
word LockOnEvil(Map *map,int x,int y);
byte GetGuyPos(word guy,int *x,int *y);
byte MonsterExists(byte type);
void HealGoodguy(byte amt);
byte MossCheck(int x,int y);
void KillKids(Guy *g);
void AddRandomGuy(Map *map,world_t *world,byte chapter,byte rnd);
void AddBattleGuys(Map *map,byte t);
Guy *NearestEnemy(Guy *me);
Guy *GetGoodguy(void);
byte BadguyCount(void);
void KillPinkeyes(void);
void FairyChange(word former,word now);
word FindVictimNot(int x,int y,byte size,int dx,int dy,int damage,word who,Map *map,world_t *world);
void FairyDoThing(byte what);
byte PeepAtKid(int x,int y,Map *map,byte face);
Guy *GetGuy(word ID);

void WhackazoidUpdate(byte init);
void WhackedAZoid(Map *map);
void WhackazoidDisplay(void);
byte MonsterExistsExcept(byte type);
Guy *FindNearestOfType(int x,int y,byte type);

#endif

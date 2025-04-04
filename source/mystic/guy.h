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
#define BRUTALDMG (5)
#define BRUTALHP (5)
#define MODERNCHAP34HP (2)

#define BUMP_WALL (0)
#define BUMP_GUY  (1)
#define BUMP_NONE (2)

class Guy final
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
		byte WalkCheckOnly(int xx, int yy, Map* map, world_t* world);
		void NextFrame(void);
		void SeqFinished(void);
		void MonsterControl(Map *map,world_t *world);
		void OverworldControl(Map *map,world_t *world);
		byte CoconutBonk(int xx,int yy,Guy *him);
		byte AttackCheck(byte size,int xx,int yy,Guy *him);
		byte AttackCheck2(int xx,int yy,int xx2,int yy2,Guy *him);
		void GetShot(int dx,int dy,int damage,Map *map,world_t *world);
		void CalculateRect(Map *map);
		void IceShardSpew(Map* map, world_t* world);
		void ResetRedRiver(void);
		bool IsInTileRect(int tx, int ty, int tx2, int ty2);

		int x,y,z;
		int dx,dy,dz;
		byte mapx,mapy;
		byte facing;

		byte mind;		// brain variables for AI
		byte mind1;
		byte mind2;
		byte mind3;

		byte birthState;
		byte reload;
		byte frostBite;
		byte ouch,ouch2;
		byte stun;
		byte action;
		word frmTimer;
		word frmAdvance;
		byte frm;
		byte seq;
		word frzDamage;
		char bright;
		word frozen;	// when non-zero, monster is encased in ice for a time
		word melted;
		byte partialDamage;
		byte executable;	// can I be murdalized?
		byte placed;	// if this monster was placed on the overworld, this is 1- makes it immobile
		Guy *parent;
		int hp;
		byte type;
		word myNumberParticle;
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
word FindMurdalizeGuy(int x, int y, word maxRange);
byte GetGuyPos(word guy,int *x,int *y);
byte MonsterExists(byte type);
void HealGoodguy(byte amt);
void HealAllMinions(int amt);
byte MossCheck(int x,int y);
void KillKids(Guy *g);
void AddRandomGuy(Map *map,world_t *world,byte chapter,byte rnd);
void AddBattleGuys(Map *map,byte t);
Guy *NearestEnemy(Guy *me,bool LOS);
Guy *GetGoodguy(void);
byte BadguyCount(void);
void KillPinkeyes(void);
void KillAllBadguys(void);
void FairyChange(word former,word now);
word FindVictimNot(int x,int y,byte size,int dx,int dy,int damage,word who,Map *map,world_t *world);
void FairyDoThing(byte what);
byte PeepAtKid(int x,int y,Map *map,byte face);
Guy *GetGuy(word ID);
void StunAllOnscreen(byte duration);
void HealSummons(byte amt);
void ChangeAllGuysOfType(byte type, byte newType);
void ChangeMind2OfType(byte type, byte newMind2);
void HealBadguys(word amt);
void SetLastSafeXY(int x, int y);

void WhackazoidUpdate(byte init);
void WhackedAZoid(Map *map);
void WhackazoidDisplay(void);
byte MonsterExistsExcept(byte type);
Guy *FindNearestOfType(int x,int y,byte type);
void SetupHorkboxes(void);
void HorkUpLoot(int x, int y);
Guy* GetFirstFriendly(void);

#endif

#ifndef GUY_H
#define GUY_H

#include "winpch.h"
#include "map.h"
#include "monster.h"
#include "jamulspr.h"
#include "display.h"
#include "bullet.h"

#define ACTION_IDLE	0
#define ACTION_BUSY 1

class Guy
{
	public:
		Guy(void);
		~Guy(void);

		void Update(Map *map,world_t *world);
		void EditorUpdate(Map *map);
		void Render(byte light);
		byte CanWalk(int xx,int yy,Map *map,world_t *world);
		void NextFrame(Map *map);
		void SeqFinished(Map *map);
		void MonsterControl(Map *map,world_t *world);
		byte CoconutBonk(int xx,int yy,Guy *him);
		byte AttackCheck(byte size,int xx,int yy,Guy *him);
		void GetShot(int dx,int dy,byte damage,Map *map,world_t *world);
		void CalculateRect(void);
		byte IsActive(Map *map);

		int x,y,z;
		int oldx,oldy;
		int dx,dy,dz;
		byte mapx,mapy;
		byte facing;
		byte active;

		byte mind;		// brain variables for AI
		byte mind1;
		byte mind2;
		byte mind3;

		byte reload;

		int  ice;
		int  iceClock;
		byte ouch;
		byte action;
		word frmTimer;
		word frmAdvance;
		byte frm;
		byte seq;
		char bright;

		Guy *parent;
		int hp;
		byte type;
		byte tag;
		int rectx,recty,rectx2,recty2;	// for collision checks
		word ID;	// just a copy of the guy's number
};

extern Guy *goodguy;

void InitGuys(int max);
void ExitGuys(void);
void UpdateGuys(Map *map,world_t *world);
void EditorUpdateGuys(Map *map);
void RenderGuys(byte light);
Guy *AddGuy(int x,int y,int z,byte type);
Guy *GetGuy(word w);
void DeleteGuy(int x,int y,byte type);
void AddMapGuys(Map *map);
int  FindVictim(int x,int y,byte size,int dx,int dy,byte damage,Map *map,world_t *world);
byte FindVictims(int x,int y,byte size,int dx,int dy,byte damage,Map *map,world_t *world);
byte FindVictims2(int x,int y,byte size,int dx,int dy,byte damage,Map *map,world_t *world);
byte FindGoodVictim(int x,int y,byte size,int dx,int dy,byte damage,Map *map,world_t *world);
byte FindNewVictim(int x,int y,int *target,byte size,int dx,int dy,byte damage,Map *map,world_t *world);
byte FindSlingVictim(int x,int y,byte size,byte damage,Map *map,world_t *world);
byte FindSummonToHelp(int x,int y,byte size,byte how,Map *map,world_t *world);
byte ItemHitPlayer(int x,int y,byte size,int dx,int dy,byte type,Map *map,world_t *world);
int FreezeVictim(int x,int y,byte size,int dx,int dy,int target,byte damage,Map *map,world_t *world);
int FreezeVictimWitch(int x,int y,byte size,int dx,int dy,int target,byte damage,Map *map,world_t *world);
word LockOnEvil(int x,int y);
word LockOnEvil2(int x,int y);
word LockOnEvil3(int x,int y,int maxRange);
word LockOnEvilNotYou(int x,int y,word target);
word LockOnSummon(int x,int y);
byte GetGuyPos(word guy,int *x,int *y);
byte MonsterExists(byte type);
byte TaggedMonsterExists(byte tag);
void HealGoodguy(byte amt);
void KillKids(Guy *g);
void GuySwap(int sx,int sy,int width,int height,int dx,int dy);
void ShiftGuys(char dx,char dy,Map *map);
void RaiseSkellies(void);
byte DyingSkellies(void);
void CameraOnPlayer(byte sum);
void LoadGuys(SDL_RWops *f);
void SaveGuys(SDL_RWops *f);
void PrepGuys(Map *map);
void KillAllMonsters(byte type);
void Shock(Map *map,world_t *world,byte type,int x,int y,byte liteup);
void LightBallChange(byte tag,int value);
void LaunchJunk(Map *map,Guy *me);
void Sneeze(void);
void GirlChasePlayer(byte on);
byte AnyMonsterExists(void);
void DeleteMonsters(void);
byte SurpriseMonster(byte origType,byte tag,int x,int y,Map *map,world_t *world);
byte NotFriendly(byte type);
void TileHitCheck(int x,int y,byte size,Map *map);
byte BallHitCheck(bullet_t *ball);
void SurprisePlace(byte type,byte tag,int x,int y,Map *map,world_t *world);
void RespawnGuy(Map *map);
void Frogs(Map *map,world_t *world);
void FreezeAllBadguys(void);
void FindSummons(void);

#endif

#ifndef GUY_H
#define GUY_H

#include "winpch.h"
#include "map.h"
#include "monster.h"
#include "jamulspr.h"
#include "display.h"
//#include "bullet.h"

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
		void NextFrame(void);
		void SeqFinished(void);
		void MonsterControl(Map *map,world_t *world);
		byte AttackCheck(byte size,int xx,int yy,Guy *him);
		void AttackThem(void);
		void GetShot(int dx,int dy,byte damage,Map *map,world_t *world);
		byte IsActive(Map *map);

		int x,y,z;
		int oldx,oldy;
		int dx,dy,dz;
		byte mapx,mapy;
		byte tgtx,tgty;
		byte oldmapx,oldmapy;
		int tx,ty;
		byte facing;
		byte active;
		byte color;

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
byte FindVictim(int x,int y,byte size,int dx,int dy,byte damage,Map *map,world_t *world);
byte FindVictims(int x,int y,byte size,int dx,int dy,byte damage,Map *map,world_t *world);
byte FindVictims2(int x,int y,byte size,int dx,int dy,byte damage,Map *map,world_t *world);
byte FindGoodVictim(int x,int y,byte size,int dx,int dy,byte damage,Map *map,world_t *world);
byte FindNewVictim(int x,int y,int *target,byte size,int dx,int dy,byte damage,Map *map,world_t *world);
int FreezeVictim(int x,int y,byte size,int dx,int dy,int target,byte damage,Map *map,world_t *world);
word LockOnEvil(int x,int y);
word LockOnEvil2(int x,int y);
word LockOnEvil3(int x,int y,int maxRange);
byte GetGuyPos(word guy,int *x,int *y);
byte MonsterExists(byte type);
byte TaggedMonsterExists(byte tag);
void KillKids(Guy *g);
void GuySwap(int sx,int sy,int width,int height,int dx,int dy);
void ShiftGuys(char dx,char dy,Map *map);
void CameraOnPlayer(void);
void LoadGuys(FILE *f);
void SaveGuys(FILE *f);
void PrepGuys(Map *map);
void KillAllMonsters(byte type);
byte AnyMonsterExists(void);
void DeleteMonsters(void);
byte NotFriendly(byte type);
Guy *FindBall(int x,int y);
Guy *FindBomb(int x,int y);
Guy *FindMonster(int x,int y);
byte MonsterMoveTo(Guy *me,int x,int y);
byte MonsterExplode(int x,int y,Map *map,byte greyOK);
byte MonsterAtDestination(Guy *me,Map *map);
void SummonGuy(mapTile_t *m,int x,int y);

#endif
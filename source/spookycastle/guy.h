#ifndef GUY_H
#define GUY_H

#include "winpch.h"
#include "map.h"
#include "monster.h"
#include "jamulspr.h"
#include "display.h"

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
		void NextFrame(void);
		void SeqFinished(void);
		void MonsterControl(Map *map,world_t *world);
		byte CoconutBonk(int xx,int yy,Guy *him);
		byte AttackCheck(byte size,int xx,int yy,Guy *him);
		void AttackThem(void);
		void GetShot(int dx,int dy,byte damage,Map *map,world_t *world);
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

		Guy *parent;
		int hp;
		byte type;
		int rectx,recty,rectx2,recty2;	// for collision checks
		word ID;	// just a copy of the guy's number
};

void InitGuys(int max);
void ExitGuys(void);
void UpdateGuys(Map *map,world_t *world);
void EditorUpdateGuys(Map *map);
void RenderGuys(byte light);
Guy *AddGuy(int x,int y,int z,byte type);
void DeleteGuy(int x,int y,byte type);
void AddMapGuys(Map *map);
byte FindVictim(int x,int y,byte size,int dx,int dy,byte damage,Map *map,world_t *world);
byte FindVictims(int x,int y,byte size,int dx,int dy,byte damage,Map *map,world_t *world);
byte FindGoodVictim(int x,int y,byte size,int dx,int dy,byte damage,Map *map,world_t *world);
word LockOnEvil(int x,int y);
byte GetGuyPos(word guy,int *x,int *y);
byte MonsterExists(byte type);
void HealGoodguy(byte amt);
byte MossCheck(int x,int y);
void KillKids(Guy *g);

#endif
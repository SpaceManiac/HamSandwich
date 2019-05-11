#ifndef GUY_H
#define GUY_H

#include "winpch.h"
#include "map.h"
#include "monster.h"
#include "jamulspr.h"
#include "display.h"
#include "bullet.h"
#pragma pack(4)

#define ACTION_IDLE	0
#define ACTION_BUSY 1

#define GOOD	(1)
#define EVIL	(2)

class Guy
{
	public:
		Guy(void);
		~Guy(void);

		void Update(Map *map,world_t *world);
		void EditorUpdate(Map *map);
		void Render(byte light);
		byte CanWalk(int xx,int yy,Map *map,world_t *world);
		byte CanWalkNoMons(int xx,int yy,Map *map,world_t *world);
		void NextFrame(void);
		void SeqFinished(void);
		void MonsterControl(Map *map,world_t *world);
		byte CoconutBonk(int xx,int yy,Guy *him);
		byte AttackCheck(byte size,int xx,int yy,Guy *him);
		void AttackThem(void);
		byte GetShot(int dx,int dy,word damage,Map *map,world_t *world);
		void GetShotReal(int dx,int dy,word damage,Map *map,world_t *world);	// ignores armor, dodging, dmg adjust, etc.
		void CalculateRect(void);
		void Shoved(int xx,int yy,Map *map,world_t *world);
		void GetStunned(word amt);
		void GetPoisoned(word amt);
		void GetFrozen(word amt);
		void FindTarget(Map *map,world_t *world);
		byte WalkCheck(int xx,int yy,Map *map,world_t *world);	// like CanWalk, but doesn't shove guys around, just tells you if
																// you are bumping one or a wall or anything else
		void FrozenUpdate(Map *map,world_t *world);

		byte IsActive(Map *map);

		int safeX,safeY;	// last time you were safely on the ground
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
		byte wallBump;
		byte reload;

		byte ouch;
		byte action;
		word frmTimer;
		word frmAdvance;
		byte frm;
		byte seq;
		char bright;

		Guy *target;
		Guy *parent;
		int hp;
		byte type;
		byte tag;
		int rectx,recty,rectx2,recty2;	// for collision checks
		word ID;	// just a copy of the guy's number
		byte team;

		short stun;
		int kbdx,kbdy,kbdz;	// knockback movement
		word poison;	// hurts you every time %30=0 (once per second, 1 damage)
		word mindControl;	// timer for how long you are team-swapped
		word frozen;		// timer for how long you are frozen (actually not frozen but slowed)
		byte originalType;	// if shroomed, what were you?
		word shroomTime;	// how long to stay shroomed
		word ignited;		// how long to stay on fire
};

extern Guy *goodguy;
extern Guy *meleeAttacker;
extern byte isCritical;

void InitGuys(int max);
void ExitGuys(void);
void UpdateGuys(Map *map,world_t *world);
void EditorUpdateGuys(Map *map);
void RenderGuys(byte light);
Guy *AddGuy(int x,int y,int z,byte type);
Guy *GetGuy(word w);
void DeleteGuy(int x,int y,byte type);
void AddMapGuys(Map *map);
int FindVictim(int x,int y,int mymapx,int mymapy,byte size,int donthitme,byte team,int startAt,Map *map,world_t *world);
byte ItemHitPlayer(int x,int y,byte size,int dx,int dy,byte type,Map *map,world_t *world);
byte GetGuyPos(word guy,int *x,int *y);
byte MonsterExists(byte type);
byte TaggedMonsterExists(byte tag);
void HealGoodguy(word amt);
void HealGuy(Guy *g,word amt);
void KillKids(Guy *g);
void GuySwap(int sx,int sy,int width,int height,int dx,int dy);
void ShiftGuys(char dx,char dy,Map *map);
void CameraOnPlayer(byte sum);
void LoadGuys(FILE *f);
void SaveGuys(FILE *f);
void PrepGuys(Map *map);
void KillAllMonsters(byte type);
byte AnyMonsterExists(void);
void DeleteMonsters(void);
byte SurpriseMonster(byte origType,byte tag,int x,int y,Map *map,world_t *world);
void SurprisePlace(byte type,byte tag,int x,int y,Map *map,world_t *world);
void BadguyDrop(byte guytype,byte level,int x,int y);
void RandomRespawnMap(Map *map);
void NovaGuys(byte team,int x,int y,int radius,word dmg,byte stun,word knock);
int BulletFindTarget(byte team,byte myFace,int x,int y,int notme,Map *map,world_t *world);
int BulletFindTargetClosest(byte team,int x,int y,int notme,int maxRange,Map *map,world_t *world);
int BulletFindDrainTarget(byte team,byte myFace,int x,int y,int notme,Map *map,world_t *world);
Guy *SummonGuy(int x,int y,int radius,byte type);
void KillWeakest(byte type);
word GuyCount(byte type);
void GuruVanish(void);
void DeleteMonsterByType(byte type);
void ToastyBoom(void);
void FreezeAllEnemies(void);
void AddCaravans(void);
void AddHighwayPatrolFoes(Map *map);
void AddRumbleGuys(Map *map);
void TornadoGuys(byte team,int x,int y,int radius,Map *map,world_t *world);
void RepairClockbot(word amt);
void HealAllGoodguys(byte amt);
void EvilEye(byte mx,byte my,byte facing);
void ExplodePlagueRats(void);
void AddPuppetPals(void);
void RepairPuppets(void);
void GroundStomp(word damage,Map *map,world_t *world);
void DropMoney(int money,int x,int y);

word GuyList_First(void);
word GuyList_Next(void);

#endif

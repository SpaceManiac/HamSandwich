#ifndef GUY_H
#define GUY_H

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
		byte IsAwake(void);

		int x,y,z;
		int oldx,oldy;
		int dx,dy,dz;
		byte mapx,mapy;
		byte lastBumpX,lastBumpY;
		byte facing;

		byte mind;		// brain variables for AI
		byte mind1;
		byte mind2;
		byte mind3;

		byte reload;
		byte poison;

		byte ouch;
		byte action;
		word frmTimer;
		word frmAdvance;
		byte frm;
		byte seq;
		char bright;
		byte friendly;

		word mindControl;
		Guy *target;
		Guy *parent;
		int hp,maxHP;
		byte type;
		int rectx,recty,rectx2,recty2;	// for collision checks
		word ID;	// just a copy of the guy's number
		byte item;	// what item you're carrying
		byte frozen;

		byte aiType;
		byte fromColor,toColor;
		char brtChange;
		char name[32];

		byte spawnNum;
		word ignited;

		// TODO: custom monster jsp
		//sprite_set_t* customSpr;
};

extern Guy *goodguy;
extern byte cheatsOn;

void InitGuys(int max);
void ExitGuys(void);
void UpdateGuys(Map *map,world_t *world);
void EditorUpdateGuys(Map *map);
void RenderGuys(byte light);
Guy *AddGuy(int x,int y,int z,int type,byte friendly);
Guy *AddBaby(int x,int y,int z,byte type,Guy *me);
Guy *GetGuy(word w);
void DeleteGuy(int x,int y,byte type);
void AddMapGuys(Map *map);
byte FindVictim(int x,int y,byte size,int dx,int dy,byte damage,Map *map,world_t *world,byte friendly);
byte FindVictims(int x,int y,byte size,int dx,int dy,byte damage,Map *map,world_t *world,byte friendly);
byte FindVictims2(int x,int y,byte size,int dx,int dy,byte damage,Map *map,world_t *world,byte friendly);
byte FindVictimsRand(int x,int y,byte size,int dx,int dy,byte damage,Map *map,world_t *world,byte friendly);
word LockOnEvil(int x,int y);
word LockOnEvil2(int x,int y);
word LockOnGood(int x,int y);
word LockOnGood2(int x,int y);
byte GetGuyPos(word guy,int *x,int *y);
byte MonsterExists(byte type);
void HealGoodguy(byte amt);
byte MossCheck(int x,int y);
void KillKids(Guy *g);
byte RaftNearby(void);
void GuySwap(int sx,int sy,int width,int height,int dx,int dy);
void ShiftGuys(char dx,char dy,Map *map);
void AddPygmy(Map *map,world_t *world,byte friendly);
Guy *GetLastGuyHit(void);
void KillMonster(int x,int y,int type,byte nofx);
void MonsterLife(byte fx,int x,int y,int type,int newLife);
void MonsterMaxLife(byte fx,int x,int y,int type,int newLife);
byte CheckMonsterLife(int x,int y,int type,int life,byte flags);
byte SwapMe(int x,int y,byte size,Map *map);
int CountMonsters(int type);
int CountMonstersInRect(int type,int x,int y,int x2,int y2);
Guy *GetGuyOfType(byte type);
void RemoveGuy(Guy *g);
void Telefrag(Guy *g);
byte FreezeGuy(Guy *me);
byte TryToPushItem(int x,int y,int destx,int desty,Map *map,world_t *world);
void ChangeMonster(byte fx,int x,int y,int type,int newtype);
void ChangeMonsterAI(byte fx,int x,int y,int type,int newtype);
void ChangeTeam(byte fx,int x,int y,int type,byte team);
void ChangeMonsItem(byte fx,int x,int y,int type,int newItem);
byte CheckMonsterAwake(int x,int y,int type,byte flags);
Guy *FindMonster(int x,int y,int type);
byte BadguyRegions(int x,int y,int x2,int y2,int tx,int ty);
void MonsterLifeAmt(byte fx,int x,int y,int type,int amt);
void SetMonsterName(byte fx,int x,int y,int type,char *name);
void SetMonsterGraphics(byte fx,int x,int y,int type,char *name);
void SetMonsterColor(byte fx,int x,int y,int type,int colCode);
void SetMonsterBright(byte fx,int x,int y,int type,int bright);
byte CheckMonsterColor(int x,int y,int type,byte color);
Guy *GetGuyOfAIType(byte type);
void KillMonsterRect(int x,int y,int x2,int y2,int type,byte nofx);
byte MonstersAlive(byte type);

byte Mind1At(int x,int y);
void FindMonsterBrain(int myx,int myy);
void FindMonsterCandle(int myx,int myy);

void SaveGuys(SDL_RWops *f);
void LoadGuys(SDL_RWops *f);

void RespawnGuys(Map *map);
byte EatHay(Guy *me,byte rad);
void EasyStart(void);

byte LightVictim(int x,int y,byte size,Map *map,world_t *world,byte friendly);
void SuckInEvil(int x,int y);
byte WorthXP(byte type);
byte DoesRespawn(byte type);

byte Walkable(Guy *me,int x,int y,Map *map,world_t *world);
void SpreadCharge(Guy *me);

void GetSpook(void);
void PutSpook(void);

byte ArrangeBats(byte facing);
byte SmashTrees(Guy *me,byte rad);

#endif

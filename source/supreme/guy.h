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

		void Update(Guy *me,Map *map,world_t *world);
		void EditorUpdate(Map *map);
		void Render(dword light);
		byte CanWalk(Guy *me,int xx,int yy,Map *map,world_t *world);
		void NextFrame(Guy *me);
		void SeqFinished(Guy *me);
		void MonsterControl(Map *map,world_t *world);
		byte CoconutBonk(int xx,int yy,Guy *him);
		byte AttackCheck(byte size,int xx,int yy,Guy *him);
		byte AttackCheck2(int xx,int yy,int xx2,int yy2,Guy *him);
		void AttackThem(void);
		void GetShot(Guy *culprit,Guy *me,int dx,int dy,byte damage,Map *map,world_t *world);
		void CalculateRect(void);
		byte IsAwake(void);

		int x,y,z;
		int oldx,oldy;
		int dx,dy,dz;
		int ax,ay; //acceleration provided by badguys
		byte mapx,mapy;
		byte lastBumpX,lastBumpY;
		byte facing;

		byte mind;		// brain variables for AI
		byte mind1;
		byte mind2;
		byte mind3;
		byte mind4;
		byte mind5;

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
		int age;
		int type;
		int rectx,recty,rectx2,recty2;	// for collision checks
		word ID;	// just a copy of the guy's number
		byte item;	// what item you're carrying
		byte frozen;
		
		//NEW STUFF
		byte ignited; //burn damage (red)
		byte weak; //weakness affliction (dark red)
		byte strong; //steelskin defense (dark grey)
		byte confuse; //are they confused (light pink)
		byte special; //for special situations
		Guy *attacker;

		int aiType;
		byte fromColor,toColor;
		char brtChange;
		char name[32];

		sprite_set_t* customSpr; // custom sprites
};

extern Guy *goodguy;
extern Guy **guys;

void InitGuys(int max);
void ExitGuys(void);
void UpdateGuys(Map *map,world_t *world);
void EditorUpdateGuys(Map *map);
void RenderGuys(byte light);
Guy *AddGuy(int x,int y,int z,int type,byte friendly);
Guy *AddBaby(int x,int y,int z,int type,Guy *me);
Guy *GetGuy(dword w);
void DeleteGuy(int x,int y,int type);
void AddMapGuys(Map *map);
byte FindVictim(Guy *me,int x,int y,byte size,int dx,int dy,byte damage,Map *map,world_t *world,byte friendly);
byte FindVictims(Guy *me,int x,int y,byte size,int dx,int dy,byte damage,Map *map,world_t *world,byte friendly);
byte FindVictims2(Guy *me,int x,int y,byte size,int dx,int dy,byte damage,Map *map,world_t *world,byte friendly);
word LockOnEvil(int x,int y);
word LockOnEvil2(int x,int y);
word LockOnGood(int x,int y);
word LockOnGood2(int x,int y);
byte GetGuyPos(word guy,int *x,int *y);
byte MonsterExists(int type);
void HealGoodguy(byte amt);
byte MossCheck(int x,int y);
void KillKids(Guy *g);
void RemoveKids(Guy *g); //NEW! The Silent Killer
void KillBodyParts(Guy *g); //NEW! only kills body parts, used for imitater superboss
byte RaftNearby(void);
byte LogNearby(void); //NEW!
void GuySwap(int sx,int sy,int width,int height,int dx,int dy);
void ShiftGuys(char dx,char dy,Map *map);
void AddPygmy(Map *map,world_t *world,byte friendly);
void AddNinja(Map *map,world_t *world,byte friendly);
Guy *GetLastGuyHit(void);
byte ControlMind(Guy *me);
void KillMonster(int x,int y,int type,byte nofx);
void MonsterLife(byte fx,int x,int y,int type,int newLife);
void MonsterMaxLife(byte fx,int x,int y,int type,int newLife);
byte CheckMonsterLife(int x,int y,int type,int life,byte flags);
byte SwapMe(int x,int y,byte size,Map *map);
int CountMonsters(int type);
int CountMonstersInRect(int type,int x,int y,int x2,int y2);
Guy *GetGuyOfType(int type);
void RemoveGuy(Guy *g);
void Telefrag(Guy *g);
byte FreezeGuy(Guy *me);
byte SwitchBody(Guy *me);
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
Guy *GetGuyOfAIType(int type);

void FindMonsterBrain(int myx,int myy);
void FindMonsterCandle(int myx,int myy);

//new!
byte PeepAtKid(int x,int y,Map *map,byte face);
byte ArrangeBats(byte facing);
byte SmashTrees(Guy *me,byte rad);
byte EatHay(Guy *me,byte rad);
void SuckInEvil(int x,int y, byte friendly);
void SuckInGood(int x,int y, byte friendly);
void PushOutEvil(int x,int y, byte friendly);
void Tornado(int x,int y, byte friendly);
void HealOthers(int x,int y, int f, int h);
void PushOthers(Guy *g, byte h, Map *map);
byte Walkable(Guy *me,int x,int y,Map *map,world_t *world);
void SpreadCharge(Guy *me);
void GetSpook(void);
void PutSpook(void);
void LaunchJunk(Map *map,Guy *me);
void Sneeze(void);
void RaiseSkellies(Guy *me);
byte DyingSkellies(Guy *me);
void RaiseMummies(Guy *me);
byte DyingMummies(Guy *me);
byte CheckMonsterOuch(int x,int y,int type,byte flags);
byte CheckMonsterAge(int x,int y,int type,int age,byte flags);
int GetMaxGuys(void);

#endif

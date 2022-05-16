#include "player.h"
#include "mgldraw.h"
#include "display.h"
#include "level.h"
#include "guy.h"
#include "options.h"

typedef struct player_t
{
	byte lives;
	int score;
	byte wpn;
	byte wingmen;
	byte power;	// gun power level
	byte shield;
	byte ammo;
	byte level;
	byte world;
	byte invincible;
} player_t;

player_t player;
int mouseX,mouseY;

void InitPlayer(void)
{
	player.lives=GetStartLives()-1;
	player.score=0;
	player.wpn=WPN_NONE;
	player.wingmen=0;
	if(GetDifficulty()==DIFF_EASY)
		player.power=1;
	else
		player.power=0;

	player.ammo=0;
#ifdef DEMO
	player.level=6*5+1;
	player.world=WORLD_VARAKKIS;
#else
	player.level=1;
	player.world=1;
#endif
	player.invincible=0;
	player.shield=0;
	mouseX=0;
	mouseY=0;
	// TODO MS_moveTo(320,240);
}

void ExitPlayer(void)
{
}

void ContinuePlayer(void)
{
	player.lives=GetStartLives()-1;
	player.score=0;
	player.wpn=WPN_NONE;
	player.wingmen=0;
	if(GetDifficulty()==DIFF_EASY)
		player.power=1;
	else
		player.power=0;

	player.ammo=0;
	player.invincible=0;
	player.shield=0;
	mouseX=0;
	mouseY=0;
	// TODO MS_moveTo(320,240);
}

// this resets everything that should be reset when you die
void PlayerDeathReset(void)
{
	if(GetDifficulty()==DIFF_EASY)
		player.power=1;
	else
		player.power=0;
	player.wingmen=0;
	player.wpn=0;
	player.ammo=0;
	player.shield=0;
	player.invincible=60;
	KillWingmen();
}

int GetScore(void)
{
	return player.score;
}

void SetShield(byte v)
{
	player.shield=v;
}

void SetWingmen(byte v)
{
	player.wingmen=v;
}

byte GetWingmen(void)
{
	return player.wingmen;
}

byte GetShield(void)
{
	return player.shield;
}

byte GetLives(void)
{
	return player.lives;
}

byte AddLife(char v)
{
	player.lives+=v;
	if(player.lives>200)
	{
		player.lives=0;
		return 1;
	}

	// 8 life limit, fair?
	if(player.lives>8)
		player.lives=8;

	return 0;
}

void SetWeapon(byte wpn,byte ammo)
{
	player.wpn=wpn;
	player.ammo=ammo;
}

byte GetWeapon(void)
{
	return player.wpn;
}

byte GetAmmo(void)
{
	return player.ammo;
}

void UseAmmo(byte amt)
{
	if(player.ammo-amt<=0)
	{
		player.ammo=0;
		player.wpn=0;
	}
	else
		player.ammo-=amt;
}

void BoostPower(void)
{
	player.power++;
}

byte GetPower(void)
{
	return player.power;
}

void PlayerGetPoints(int p)
{
	player.score+=p;
}

byte GetInvincible(void)
{
	return player.invincible;
}

void SetInvincible(byte b)
{
	player.invincible=b;
}

byte PlayerWinLevel(void)
{
	player.level++;

#ifdef JUNIOR
	if(player.level==26)
		return 1;
#elif TRIAL
	if(player.level==16)
		return 1;
#elif DEMO
	if(player.level==6*5+6)
		return 1;
#else
	if(player.level==51)
		return 1;
#endif

	if((player.level-1)%5==0)
		player.world++;
	SetupLevel(player.level);
	return 0;
}

void PlayerSkipWorld(void)
{
	player.level=(player.world)*5;
	PlayerWinLevel();
}

byte GetLevel(void)
{
	return player.level;
}

byte GetWorld(void)
{
	return player.world;
}

void UpdatePlayer(void)
{
	if(player.invincible>0)
		player.invincible--;
}

void UpdateMouse(void)
{
	int msx,msy;

	// TODO MS_getPos(&msx,&msy);
	// TODO MS_moveTo(320,240);

	mouseX=(msx-320);
	mouseY=(msy-240);
}

void GetMouse(int *x,int *y)
{
	*x=mouseX;
	*y=mouseY;
}

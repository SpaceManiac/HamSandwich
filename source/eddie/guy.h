#ifndef GUY_H
#define GUY_H

#include "jamultypes.h"

#define GUY_NONE		0
#define GUY_PLAYER		1
#define GUY_CENTIHEAD	2
#define GUY_CENTIBODY	3
#define GUY_SHROOM		4
#define GUY_BULLET		5
#define GUY_MISSILE		6
#define GUY_SPIDER		7
#define GUY_RADIO		8	// the little radios that give special missions
#define GUY_CHAINBULLET 9
#define GUY_SHOTBULLET  10
#define GUY_LASER		11
#define GUY_NUKE		12
#define GUY_NUKEBOOM	13
#define GUY_WINGMAN		14
#define GUY_PRESENT		15
#define GUY_ICESPIDER	16
#define GUY_SNOWMAN		17
#define GUY_AQUAHEAD    18
#define GUY_AQUABODY	19
#define GUY_AQUATAIL	20
#define GUY_FLY			21
#define GUY_FLYSPIT		22
#define GUY_STRIDER		23
#define GUY_FROG		24
#define GUY_TURRET		25
#define GUY_YELLOWBULLET 26	// an enemy bullet
#define GUY_ROBOHEAD	27
#define GUY_ROBOBODY	28
#define GUY_REPAIR		29
#define GUY_SPIDERBOT	30
#define GUY_VOLCANO		31
#define GUY_ROCK		32	// spit out of a volcano
#define GUY_SPARKY		33
#define GUY_FLAME		34	// sparkies shoot these
#define GUY_FIREANT		35
#define GUY_BIGROID		36	// big asteroid
#define GUY_MEDROID		37	// medium asteroid
#define GUY_SMLROID		38	// small asteroid
#define GUY_LOBSTER		39	// robotic space lobster
#define GUY_LLOBCLAW	40	// left lobster claw
#define GUY_RLOBCLAW	41	// right lobster claw
#define GUY_LOBTAIL		42
#define GUY_FIREBALL	43
#define GUY_GREENLASER	44	// the source of the green laser
#define GUY_GREENLASER2	45	// the body of the green lasers
#define GUY_POWERUP		46	// in space, these drift down and give you powerups
#define GUY_SPINSHIP	47
#define GUY_CLAWSHIP	48
#define GUY_SMALLSHIP	49
#define GUY_SQUIDSHIP	50
#define GUY_PWRUPUFO	51
#define GUY_FBALLSHIP	52
#define GUY_DOUBLESHIP	53	// double barreled horde ship
#define GUY_TRICERATOPS 54
#define GUY_PTERODACTYL 55	// technically a pteranodon
#define GUY_EYEBALL1	56
#define GUY_EYEBALL2	57
#define GUY_EYEBALL3	58
#define GUY_EYEBALL4	59	// these four are the eyeball monster in descending size order
#define GUY_EYEGUY		60	// the fireball spitting eye monster
#define GUY_TUMBLEWEED	61
#define GUY_SCORPION	62
#define GUY_SANDWORM	63
#define GUY_ALIENHEAD	64
#define GUY_ALIENBODY	65	// alienpede parts
#define GUY_BABYALIEN	66
#define GUY_FLYALIEN	67	// flying alien
#define GUY_REDBULLET	68
#define GUY_QUEEN		69
#define GUY_QUEENHEAD	70
#define GUY_QUEENTAIL	71
#define GUY_ACIDGLOB	72	// big grenade-style glob
#define GUY_ACIDSPIT	73
#define GUY_FLYINGEGG	74	// egg grenade
#define GUY_BLUEBULLET	75
#define GUY_SATELLITE	76

void InitGuys(void);
void InitGuysLeaveShrooms(byte shroomtype);
void ExitGuys(void);
byte UpdateGuys(void);
void RenderGuys(void);
byte AddGuy(byte type,int x,int y,int value);
int AddGuyExact(byte type,int x,int y,byte val);
void MakeCentipede(int x,int y,int value,byte length);
void PlantShroom(int x,int y);
void PlantMonster(byte type,int x,int y);
void PlantMonster2(byte type,int x,int y);
void GiveWingman(void);
void KillWingmen(void);
void PutPresents(int num);
void MakeLobster(int x,int y);
void MakeQueen(void);
void SetHordeDX(int dx,int destx,int guynum);
int FindTarget(byte type,int *x,int *y);
void GiveBirthToAlien(int x,int y,byte lefty);

/*
void UpdatePlayer(guy_t *me);
void UpdateBullet(guy_t *me);
void UpdateLaser(guy_t *me);
void UpdateMissile(guy_t *me);
void UpdateCentihead(guy_t *me);
void UpdateCentibody(guy_t *me);
void UpdateSpider(guy_t *me);
void UpdateRadio(guy_t *me);
void UpdatePowerup(guy_t *me);
void UpdatePresent(guy_t *me);
void UpdateNukeBoom(guy_t *me);
void UpdateWingMan(guy_t *me);
void UpdateSnowman(guy_t *me);
void UpdateFly(guy_t *me);
void UpdateEnemyBullet(guy_t *me);
void UpdateStrider(guy_t *me);
void UpdateFrog(guy_t *me);
void UpdateTurret(guy_t *me);
void UpdateRepairDroid(guy_t *me);
void UpdateVolcano(guy_t *me);
void UpdateRock(guy_t *me);
void UpdateSparky(guy_t *me);
void UpdateFlame(guy_t *me);
void UpdateFireAnt(guy_t *me);
void UpdateAsteroid(guy_t *me);
void UpdateLobsterTail(guy_t *me);
void UpdateLobster(guy_t *me);
void UpdateLobsterClaw(guy_t *me);
void UpdateGreenLaser(guy_t *me);
void UpdateGreenLaser2(guy_t *me);
void UpdateSpinnyShip(guy_t *me);
void UpdateClawShip(guy_t *me);
void UpdateSmallShip(guy_t *me);
void UpdateSquidShip(guy_t *me);
void UpdatePowerUpUFO(guy_t *me);
void UpdateFireballShip(guy_t *me);
void UpdateDoubleBarrelShip(guy_t *me);
void UpdateTriceratops(guy_t *me);
void UpdatePterodactyl(guy_t *me);
void UpdateEyeball(guy_t *me);
void UpdateEyeGuy(guy_t *me);
void UpdateScorpion(guy_t *me);
void UpdateTumbleweed(guy_t *me);
void UpdateSandworm(guy_t *me);
void UpdateBabyAlien(guy_t *me);
void UpdateFlyAlien(guy_t *me);
void UpdateQueen(guy_t *me);
void UpdateQueenHead(guy_t *me);
void UpdateQueenTail(guy_t *me);
void UpdateAcidGlob(guy_t *me);
void UpdateSatellite(guy_t *me);
*/

#endif
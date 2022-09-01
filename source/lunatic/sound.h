#ifndef SOUND_H
#define SOUND_H

#include "jamulsound.h"
#include "jamultypes.h"

enum {
    SND_MENUCLICK = 0,
    SND_MENUSELECT = 1,
    SND_PAUSE = 2,
    SND_SAVEGAME = 3,
    SND_LOADGAME = 4,
    SND_HAMMERBONK = 5,
    SND_HAMMERREFLECT = 6,
    SND_BULLETREFLECT = 7,
    SND_BOMBREFLECT = 8,
    SND_MISSILEBOOM = 9,
    SND_ACIDSPLAT = 10,
    SND_ENERGYBONK = 11,	// little red bullets hitting the wall
    SND_BOMBBOOM = 12,
    SND_BULLETHIT = 13,
    SND_MEGABEAMHIT = 14,
    SND_BOMBTHROW = 15,
    SND_MISSILELAUNCH = 16,
    SND_FLAMEGO = 17,       // each piece of flame being thrown does this
    SND_BULLETFIRE = 18,	// machine gun (1 shot)
    SND_KOOLKAT = 19,       // when you get all the brains
    SND_MEGABEAM = 20,      // being fired
    SND_HAMMERTOSS = 21,
    SND_CHEATWIN = 22,      // skip level via cheat code
    SND_HEALTHCHEAT = 23,
    SND_INVINCCHEAT = 24,	// doesn't exist yet, but someday
    SND_DROWN = 25,
    SND_ITEMDROP = 26,
    SND_GOTOMAP = 27,
    SND_WINLEVEL = 28,
    SND_WALLDOWN = 29,
    SND_WALLUP = 30,
    SND_LIGHTSON = 31,
    SND_TELEPORT = 32,
    SND_MESSAGE = 33,
    // now the frigging monster sounds...
    // bonehead
    SND_SKELKICK = 34,
    SND_SKELSHOOT = 35,
    SND_SKELOUCH = 36,
    SND_SKELDIE = 37,
    // scary bat
    SND_BATEYES = 38,
    SND_BATDIVE = 39,
    SND_BATOUCH = 40,
    SND_BATDIE = 41,
    // eensy weensy spider
    SND_SPD1OUCH = 42,
    SND_SPD1DIE = 43,
    SND_SPD1GNAW = 44,
    // spitter spider
    SND_SPD2OUCH = 45,
    SND_SPD2DIE = 46,
    SND_SPD2SPIT = 47,
    // zombie
    SND_ZOMBIEOUCH = 48,
    SND_ZOMBIEDIE = 49,
    SND_ZOMBIELEAP = 50,
    SND_ZOMBIEMOAN = 51,	// not implemented yet, but random moans from the zombies while they wander
    // egg sac
    SND_EGGSACDIE = 52,
    SND_EGGSACBIRTH = 53,
    // mama spider
    SND_SPD3OUCH = 54,
    SND_SPD3DIE = 55,
    SND_SPD3CHOMP = 56,
    SND_SPD3SPEW = 57,
    // pygmy
    SND_PYGMYOUCH = 58,
    SND_PYGMYDIE = 59,
    SND_PYGMYYAWN = 60,
    SND_PYGMYSPIN = 61,
    SND_PYGMYSTAB = 62,
    SND_PYGMYANGRY = 63,
    // aquazoid
    SND_SERPENTOUCH = 64,
    SND_SERPENTDIE = 65,
    SND_SERPENTSPIT = 66,
    // Matilda!!!!!
    SND_MATTIEOUCH = 67,
    SND_MATTIETRUEDIE = 68,
    SND_MATTIEDIE = 69,     // this is when her head or skull dies
    SND_MATTIECLAW = 70,
    SND_MATTIESHOOT = 71,
    SND_MATTIESTOMP = 72,
    // Ninjabread
    SND_GINGEROUCH = 73,
    SND_GINGERDIE = 74,
    SND_GINGERJKICK = 75,
    SND_GINGERCOMBO = 76,
    // pumpkin
    SND_PUMPKINDIE = 77,	// no ouch sound- they usually die in one hit, if not who cares
    // sudden leap back into non-monster sounds
    // item pickup sounds
    SND_HAMMERUP = 78,
    SND_PANTS = 79,
    SND_REVERSE = 80,
    SND_REFLECT = 81,
    SND_WEAPON = 82,
    SND_FOOD = 83,
    SND_SHIELD = 84,
    SND_GETBRAIN = 85,
    SND_GETKEY = 86,
    SND_GETKEYCHAIN = 87,
    SND_LOONYKEY = 88,
    SND_BOUAPHAOUCH = 89,
    SND_BOUAPHADIE = 90,
    SND_BOUAPHABORED = 91,
    SND_DOOROPEN = 92,
    SND_WORLDTURN = 93,
    SND_WORLDPICK = 94,
    SND_BLOCKPUSH = 95,
    // sounds for Mush
    SND_MUSHMAD = 96,
    SND_MUSHMISSILE = 97,
    SND_MUSHSPORES = 98,
    SND_MUSHOUCH = 99,
    SND_MUSHDIE = 100,
    // Super zombie sounds
    SND_SZGRAB = 101,
    // thingie snore
    SND_BABYSNORE = 102,
    // Happy stick man
    SND_HAPPYFLEX = 103,
    SND_HAPPYOUCH = 104,
    SND_HAPPYDIE = 105,
    SND_HAPPYCHOMP = 106,
    // The Thing
    SND_THINGVOMIT = 107,	// spitting up a baby that is
    // zomboni
    SND_ZOMBONIZOOM = 108,
    // baby SEAL
    SND_SEALOUCH = 109,
    SND_SEALDIE = 110,
    // pengulon
    SND_PENGDIE = 111,
    SND_PENGPECK = 112,
    // snowguy
    SND_SNOWOUCH = 113,
    SND_SNOWDIE = 114,
    SND_SNOWTHROW = 115,
    // yeti bros
    SND_SVENHI = 116,
    SND_BJORNHI = 117,
    SND_YETIOUCH = 118,
    SND_YETIDIE = 119,
    SND_YETIYODEL = 120,
    // cacton
    SND_CACTONSHOOT = 121,
    SND_CACTONDIE = 122,
    // djinni
    SND_DJINNIOUCH = 123,
    SND_DJINNIDIE = 124,
    SND_DJINNIPUNCH = 125,
    SND_DJINNIBOO = 126,
    // magic lamp
    SND_LAMPPOOF = 127,
    SND_LAMPDIE = 128,
    // mumble
    SND_MUMBLECRUSH = 129,
    // roly poly
    SND_ROLYPOLYWALL = 130,
    SND_ROCKBOUNCE = 131,	// rocks bouncing off wall, not really roly poly, but kind of
    // sphinx
    SND_SPHINXSNEEZE = 132,
    SND_SPHINXSTOMP = 133,
    // wacko
    SND_WACKOSTAB = 134,
    SND_WACKOOUCH = 135,
    SND_WACKODIE = 136,
    // boiler
    SND_BOILERHIT = 137,
    // great pumpkin
    SND_GREATPKOUCH = 138,
    SND_GREATPKDIE = 139,
    SND_GREATPKVOMIT = 140,
    // Dr. L
    SND_DRLLAUGH = 141,
    SND_DRLFEAR = 142,
    SND_DRLOUCH = 143,
    SND_DRLDIE = 144,
    // SDZL
    SND_SDZLOUCH = 145,
    SND_SDZLDIE = 146,
    SND_SDZLSLASH = 147,
    SND_SDZLPOUND = 148,
    SND_SDZLSPIT = 149,
    // Santa
    SND_SANTAOUCH = 150,
    SND_SANTADIE = 151,
    SND_SANTAHIT = 152,

    // EXPANSION SOUNDS
    // minecart
    SND_MINECART = 153,

    // ------------- NOT AN EXPANSION SOUND:::  THIS IS THE HAMUMU THEME
    SND_HAMUMU = 154,
    // vampire
    SND_VAMPATTK = 155,
    SND_VAMPFEAR = 156,
    SND_VAMPOUCH = 157,
    SND_VAMPDIE = 158,

    // pwr armor
    SND_ARMORSHOOT = 159,

    // triceroid
    SND_TRICERAROAR = 160,
    // kongor
    SND_CHESTPOUND = 161,
    SND_ROBOOUCH = 162,
    SND_ROBODIE = 163,
    SND_FACTORYDIE = 164,
    SND_COFFIN = 165,
    SND_GHOSTOUCH = 166,
    SND_GHOSTDIE = 167,
    SND_GHOSTTPORT = 168,
    SND_GHOSTYELL = 169,
    SND_LEFTYOUCH = 170,
    SND_LEFTYDIE = 171,
    SND_LEFTYWAVE = 172,    // "hello" muffled
    SND_UNGADANCE = 173,
    SND_SHAMANOUCH = 174,
    SND_SHAMANDIE = 175,
    SND_SHAMANZAP = 176,
    SND_SHAMANDANCE = 177,
    SND_PKSTEINBUMP = 178,
    SND_GLASSBREAK = 179,
    SND_GLASSHIT = 180,
    SND_GLASSDIE = 181,
    SND_GLASSBLOCK = 182,
    SND_TRICEROUCH = 183,
    SND_TRICERDIE = 184,
    SND_TRICERBUMP = 185,
    SND_COUNTOUCH = 186,
    SND_COUNTDIE = 187,
    SND_COUNTHELLO = 188,
    SND_COUNTSUMMON = 189,
    SND_COUNTSHOOT = 190,
    SND_COUNTCHARGE = 191,
    SND_EGGOUCH = 192,
    SND_EGGDIE = 193,
    SND_EGGOPEN = 194,
    SND_ALIENOUCH = 195,
    SND_ALIENDIE = 196,
    SND_ALIENSPIT = 197,
    SND_INVIDCHARGE = 198,
    SND_ROBOSHOOT = 199,
    SND_FACTORYGO = 200,
    SND_KONGOUCH = 201,
    SND_KONGDIE = 202,
    SND_KONGHELLO = 203,
    // bouapha weapons
    SND_LIGHTNING = 204,
    SND_SLASH = 205,
    SND_CLANG = 206,
    SND_MINELAY = 207,
    SND_RAGE = 208,         // NOT JUST IN EXPANSION!
    SND_SPEEDUP = 209,		// get particle accelerator
    SND_ROBOBOUAPHAOUCH = 210,
    SND_ROBOBOUAPHAON = 211,
    SND_ULTRAPYGMYOUCH = 212,
    SND_ULTRAPYGMYDIE = 213,
    SND_ULTRAPYGMYYAWN = 214,
    SND_ULTRAPYGMYSTAB = 215,
    SND_ULTRAPYGMYANGRY = 216,
    SND_ROBOSTOMP = 217,
    SND_SHAMANANGRY = 218,
    SND_ZAP = 219,
    SND_BALLLIGHTNING = 220,

    // fun pack
    SND_BUNNYDIE = 256,
    SND_MINDWIPE = 257,
    SND_SHARKATTACK = 258,
    SND_SHARKOUCH = 259,
    SND_SHARKDIE = 260,
    SND_WIZHIT = 261,
    SND_WIZOUCH = 262,
    SND_WIZDIE = 263,
    SND_HAMMERDN = 264,
    SND_PANTSDN = 265,
    SND_LAYEGG = 266,
    SND_GNOMESEE = 267,
    SND_GNOMELAUGH = 268,
    SND_GENERATE = 269,
    SND_CLONEOUCH = 270,
    SND_CLONEDIE = 271,
    SND_TURRETBZZT = 272,
    SND_SKELALIVE = 273
};

// --------------------------------

void InitSound(void);
void ExitSound(void);
void MakeSound(int snd, int x, int y, int flags, int priority);
void MakeNormalSound(int snd);

#endif

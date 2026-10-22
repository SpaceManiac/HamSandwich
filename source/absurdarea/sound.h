#ifndef SOUND_H
#define SOUND_H

#include "jamulsound.h"
#include "jamultypes.h"
#pragma pack(1)

// interface sounds
#define SND_MENUCLICK	(0)
#define SND_MENUSELECT	(1)
#define SND_MENUCANCEL	(2)
#define SND_PAUSE		(3)
#define SND_ENTERMAP	(4)	// entering one of those swooshy things to change map
#define SND_MESSAGE		(5)	// new message in the list, little boop
#define SND_NEWQUEST	(6)
#define SND_QUESTDONE	(7)
// general action sounds
#define SND_BOOM		(8)
#define SND_JUNKGET		(9)
// loony sounds
#define SND_LOONYJUMP		(10)
#define SND_LOONYLAND		(11)	// land from a jump
#define SND_LOONYSWING		(12)
#define SND_GETCOIN			(13)
#define SND_SELLITEM		(14)
#define SND_BEATENUP		(15)
#define SND_CANTBUY			(16)
#define SND_EQUIP			(17)
#define SND_BONEBOMB		(18)
#define SND_SHROOMIFY		(19)
#define SND_LOONYOUCH		(20)
#define SND_LOONYDIE		(21)
#define SND_BERSERK			(22)
#define SND_DRINKPOTION		(23)
#define SND_SPELLMENU		(24)
#define SND_SPELLTURN		(25)	// turning the spell menu
// mouse
#define SND_MOUSEOUCH		(26)
#define SND_MOUSEDIE		(27)
#define SND_CRYOZOID		(28)
#define SND_MOUSESHOOT		(29)
// puppet
#define SND_PUPPETOUCH		(30)
#define SND_PUPPETDIE		(31)
#define SND_PUPPETPUNCH		(32)
#define SND_PUPPETHAT		(33)	// magic hat summoning a mouse
// tin guy
#define SND_TINOUCH			(34)
#define SND_TINDIE			(35)
#define SND_STONERUMBLE		(36)
#define SND_TINSHOOT		(37)
// bear
#define SND_BEAROUCH		(38)
#define SND_BEARDIE			(39)
#define SND_BEARFLOP		(40)
#define SND_BEARSHOOT		(41)
// bonehead
#define SND_SKELOUCH		(42)
#define SND_SKELDIE			(43)
#define SND_SKELSWING		(44)
#define SND_SKELSUMMON		(45)

#define SND_NEWSKILL		(46)
#define SND_LEVELUP			(47)

#define SND_DOOROPEN		(48)	// chests too
#define SND_FIRESHOOT		(49)	// launch fireball
#define SND_PARRY			(50)
#define SND_DODGE			(51)

// sock monkey
#define SND_MONKEYOUCH		(52)
#define SND_MONKEYDIE		(53)
#define SND_MONKEYDASH		(54)
#define SND_MONKEYLOAD		(55)	// loading up to shoot
#define SND_MONKEYSHOOT		(56)	// and shooting

#define SND_TALENTUP		(57)

// klonk
#define SND_KLONKOUCH		(58)
#define SND_KLONKDIE		(59)
#define SND_KLONKSMASH		(60)
#define SND_IGNITE			(61)

#define SND_SPLASH			(62)
#define SND_MAGICFAIL		(63)
#define SND_FALL			(64)	// fall into a bottomless pit
#define SND_ARENAWIN		(65)
#define SND_ARENALOSE		(66)
#define SND_ICECRACK		(67)	// ice cracking sound for when you jump onto ice, combines with splash

// plant monsters
#define SND_PLANTOUCH		(68)
#define SND_PLANTDIE		(69)
#define SND_PLANTSHOOT		(70)
#define SND_PLANTBURY		(71)
#define SND_PLANTSURFACE	(72)
// hound
#define SND_HOUNDOUCH		(73)
#define SND_HOUNDDIE		(74)
#define SND_HOUNDSPIT		(75)
#define SND_HOUNDCHARGE		(76)

#define SND_POISON			(77)	// get hit by a poisoning projectile - splurt!
#define SND_CHOPTREE		(78)
#define SND_GALE			(79)	// gale spell, wind whoosh
#define SND_CRYOSHOOT		(80)	// cryozoid shoot

// the toy machines
#define SND_CRYSTALZAP		(81)	// crystal firing it's lightning
#define SND_POWERLOB		(82)	// power firing a grenade
#define SND_CRYSTALBREAK	(83)	// crystal shattering
#define SND_POWERDIE		(84)	// the power machine dying
#define SND_CRYSTALOUCH		(85)	// ting!

#define SND_SPECIALGET		(86)	// got a quest item
#define SND_BERRYSPIT		(87)	// lifeberry bush spit out a lifeberry
#define SND_BERRYEAT		(88)	// eat a lifeberry
#define SND_POTIONGET		(89)
#define SND_LENSGET			(90)
#define SND_SHARPEN			(91)
#define SND_ROPESTUMP		(92)
#define SND_BIGHEAL			(93)	// cast Restoration or get healed by Healiana or Curaga
#define SND_DIG				(94)
#define SND_CRITICAL		(95)
#define SND_GHOSTDIE		(96)
// thug
#define SND_THUGOUCH		(97)
#define SND_THUGDIE			(98)
#define SND_THUGSMACK		(99)

// villagers
#define SND_GURUFLOAT		(100)	// since villager 0 and 1 don't exist, I can reuse their hello/bored slots
#define SND_VILLAGERHELLO	(100)	// continues on for each villager, just add tag to 100
#define SND_VILLAGERBORED	(150)	// same as above

#define SND_BURNINATE		(200)
#define SND_JUNKCOCK		(201)	// junksmith cocking his gun before firing
#define SND_SHOCK			(202)	// electrocute noise, when anything makes a lightning bolt, quiet
#define SND_TORNADO			(203)	// for tornado spell and Capitan's whirling strike
#define SND_SMITH			(204)	// junksmith or make a clockbot or repair it
#define SND_DRAIN			(205)	// drain spell sucking life
#define SND_RICOCHET		(206)

// crab
#define SND_CRABOUCH		(207)
#define SND_CRABDIE			(208)
#define SND_CRABCLACK		(209)

// badger
#define SND_BADGEROUCH		(210)
#define SND_BADGERDIE		(211)
#define SND_BADGERSLASH		(212)
#define SND_BADGERSPIN		(213)
#define SND_BADGERDIG		(214)

// stonewall
#define SND_STONEOUCH		(215)
#define SND_STONEDIE		(216)
#define SND_STONESTOMP		(217)	// ***
#define SND_STONESPLAT		(218)	// ***

// titan
#define SND_TITANOUCH		(219)
#define SND_TITANDIE		(220)
#define SND_TITANROAR		(221)
#define SND_TITANSMASH		(222)

// gourdzilla
#define SND_ZILLAOUCH		(223)	// ***
#define SND_ZILLADIE		(224)	// ***

#define SND_BOKBOK			(225)	// bokbok cheeping hello
#define SND_CUBEHIT			(226)	// ice cube hitting guy/ground
#define SND_PERSUADE		(227)

// baron
#define SND_BARONOUCH		(228)
#define SND_DRAWSTONE		(229)	// pull a stone from the ground to throw
#define SND_STONEHIT		(230)	// stone shattering on impact

// ubertitan
#define SND_KILLBOROUCH		(231)
#define SND_KILLBORDIE		(232)
#define SND_BARONDIE		(233)
#define SND_KILLBORSLASH	(234)

// happy stick
#define SND_STICKDIE		(235)
#define SND_STICKOUCH		(236)

// gorka
#define SND_GORKASLASH		(237)
#define SND_GORKATOSS		(238)

#define SND_KILLBORSPELL	(239)

// the hamumu sound
#define SND_HAMUMU		200
#define SND_ENDSONG		201

//--------------------------------

void InitSound(void);
void ExitSound(void);
void MakeSound(int snd,int x,int y,int flags,int priority);
void MakeNormalSound(int snd);
void MakeNormalSound2(int snd);
void MakeSpaceSound(int snd,int priority);
void LoopingSound(int snd);
void SoundSystemExists(void);

#endif

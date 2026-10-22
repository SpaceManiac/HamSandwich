#ifndef PLAYER_H
#define PLAYER_H

#include "jamultypes.h"
#include "guy.h"
#include "control.h"
#include "game.h"
#include "display.h"
#include "bullet.h"
#include "world.h"
#include "intface.h"
#include "equip.h"
#include "talents.h"
#include "shop.h"
#include "upgrade.h"

#define MAX_MONEY	99999
#define COMBO_TIME	30*2

#define MAX_LOG	(30)
#define LOG_LEN	(64)

// initializing constants (pass to InitPlayer)
#define INIT_GAME  2
#define INIT_WORLD 1
#define INIT_LEVEL 0

// var constants to keep things clear
	// quests
#define VAR_QUESTASSIGN 0	// 0-49 = quest (N) has been assigned
#define VAR_QUESTDONE	50	// 50-99 = quest (N) is complete
	// not quests
#define VAR_PROVEKILLS	100	// # of kills you've gotten while on a Prove Yourself quest
#define VAR_PRESENT		101	// 387-485 = have you collected present (N-387)?

#define VAR_MAGICHAT	200	// 200=killed magic hat
#define VAR_STONEROSE	201	// 201=killed stone rose
#define VAR_VISITED		202	// 202-216=have been to different places
#define VAR_WIFEUNLOCK	217	// have unlocked Bob's wife
#define VAR_FACTORY		218	// factory destroyed
#define VAR_STINKYSOCK	219	// killed stinky sock boss (also exists as a normal guy, later on, either one works)
#define VAR_TREECHOP	220	// 220-221 = the two choppable trees have been chopped
#define VAR_RATS		222	 // how many rats remain in Mickey's cellar.  If 0, you can cash in the quest

	// temp variables
#define VAR_TEMP		240	// 240-255 = temporary variables - cleared when leaving a level
#define VAR_CHEST		256	// 256-305 = treasure chests opened
#define VAR_KLONKDEAD	306	// 306 = klonk has been killed
#define VAR_TOYSDEAD	307	// 307 = toys killed for mickey's quest
#define VAR_BEARSDEAD	308	// 308 = bears killed for hank's quest
#define VAR_SNUGGLY		309	// 309 = 0 for Onion side, 1 for Snuggly side
#define VAR_CHAMPION	310	// 310 = beat the champion of the arena
#define VAR_GLASSES		311	// 311 = have gotten the fancy glasses
#define VAR_SWORDS		312	// 312 = swords collected in arena collection level
#define VAR_ROPED		313	// 313-320 = rope bridges have been built (313+tag number)
#define VAR_RATBOSS		321	// 321 = ratatouille dead
#define VAR_ONIONCHAT	322	// 322=have had the chat with onion to decide
#define VAR_FACTORY1	323	// 323=have cleared first floor of factory
#define VAR_BUSHDEAD	324	// 324=killed burning bush
#define VAR_TRIGUNDEAD	325	// 325=trigun is dead
#define VAR_SHAKEDOWN	326	// 326-329=merchants have been shaken down (the 0th one is also used by the Snugglies for their quest)
#define VAR_SUPPLIES	330	// 330=supply base destroyed
#define VAR_RUMBLE		331	// 331=have won the rumble
#define VAR_PRISONBREAK 332	// 332=have broken out of prison
#define VAR_JUNKDEAD	333	// 333=have killed the junksmith
#define VAR_HIGHWAY		334	// 334=have successfully done the highway mission (snuggly side)
#define VAR_CLOCKBOT	335	// 335-349=how many of each item are used in your clockwork bot
#define VAR_CLOCKLIFE	350	// 350=clockwork bot's current health for traipsing between areas
#define VAR_GURUTALENT	351	// 351=which talent you are currently talking to the guru about
#define VAR_BOOKGIVEN	352	// 352=have given the library book to historian
#define VAR_BARRIER		353	// 353-357=have opened the barriers of the same tag
#define VAR_GOURDDEAD	358	// 358=killed gourdzilla
#define VAR_DEFILE		359	// 359-360=bodzhas left alive on the two sides of shroom forest
#define VAR_YOUTH		361	// 361=Shrumford has youngified
#define VAR_BOKFOUND	362	// 362-371=bokbok picked up
#define VAR_BOKGIVEN	372	// 372=number of found bokboks you've delivered
#define VAR_FURNACE		373	// 373=furnace lit
#define VAR_JUSTHEF		374	// 374=used by the justhef axe as a timer
#define VAR_TITANIC		375	// 375-384=titanic titans have been slain
#define VAR_CAPTAIN2	385	// 385=1, captain has arrived at furnace, 2=is active, 3=is dead
#define VAR_TIP			386	// 386-410= tips have been shown
#define VAR_FROSTGATE	411	// 411= has frostburn front door been smashed
#define VAR_PUPPETLIFE  412	// 412-416=current life of helper puppets
#define VAR_BARONDEAD	417	// the baron has been beaten
#define VAR_SUPERTITAN	418	// melty titan has been beaten
#define VAR_SWITCH		419	// 419-422=have flipped the switches in Frostburn
#define VAR_TAMED		423	// 423=have tamed a mimic for magicsmith (bit 1 and 2 for hers and yours respectively)
#define VAR_SWITCH2		424	// 424-427=castle floor 2 switches
#define VAR_SPELLSET	428	// 428-432=which spell is currently in place in each of the 5 spots of spell menu
#define VAR_BIRDCHAT	433	// the bird has spoken to you
#define VAR_MADCAP		434	// in madcap mode
#define VAR_MADGEM		435	// 435-445=how many of each type of madcap gem you have
#define VAR_MODIFIER	446	// 446-449=the three modifiers you are using
#define VAR_CPOINTS		450	// 450=crystal points on hand
#define VAR_PARRYCLOCK	451	// 451=how long until you can parry again
#define VAR_CLOCKLIFE2	452	// 452=other half of clocklife var, since a byte won't do it
#define VAR_CONTROLS	453	// 453-472=control settings for this profile
#define VAR_SOUND		483	// 483=player sound volume
#define VAR_MUSIC		484	// 484=player music volume
#define VAR_COMMENTARY	485	// 485=commentary is on... only in collector's edition!
#define VAR_KILLCOUNTS	486	// 486-511=kill counters (two bytes each)
#define VAR_ARENAWINS	512	// 512=arena wins
#define VAR_DEATHS		513	// 513=deaths
#define VAR_FLAMEPAID	514	// 514=have paid for the flame skill this attack
#define VAR_KLONKSMASH	515	// 515=how many trees klonk has smashed

//ABSURD AREA VARIABLES
#define VAA_CROWDED		520 // 520=is one of the variables Crowded?
#define VAA_HALLSEXIT	521 // 521=which exit from the Halls of Mastery?
#define VAA_TRIALTYPE	522 // 522=active trial type
#define VAA_TOWNLOC		523 // 523=which location in a town or trial?
//524-539: unused
#define VAA_REWARD		540 // 540=which reward did you pick?
#define VAA_REWARDTYPE	541 // 541=which type of reward? (1 parka, 2 axe, 3 amulet)
#define VAA_REWARDGET	542 // 542=have you obtained the reward?
#define VAA_FACTORY1	543 // 543=destroyed first part of the factory?
#define VAA_FACTORY2	544 // 544=destroyed second part of the factory?
#define VAA_TRIALSTAT	545 // 545-549: status of trial N-545 (0 nothing, 1 found, 2 partial, 3 passed) //, 4 claimed)
#define VAA_INFESTED	550 // 550=discovered the Infested Woods?
#define VAA_MARIONETTES	551	// 551=discovered the Island?
#define VAA_STORAGE		552 // 552=unblocked the Storage Cabin in Einsburg?
#define VAA_BEARLAIR	553 // 553=opened the Bear Lair?
#define VAA_VAULTWALL	554 // 554=broken the Vault wall?
#define VAA_NEARFORT	555 // 555=near the fort entrance?
#define VAA_ARENASIDE	556 // 556=found the Tweestad arena side entrance?
#define VAA_TRAPDOOR	557 // 557=opened the Tweestad Curaga trapdoor?
#define VAA_KINYOVA		558 // 558=opened the Kinyova cave?
#define VAA_MONKEYS		559	// 559=opened the Monkey Grotto?
#define VAA_FACTORYGATE	560 // 560=opened the Toy Factory gate?
#define VAA_PEAKDEBRIS	561 // 561=dug out the debris blocking the peak?
#define VAA_ICEFLOOD	562 // 562=flooded the ice hole?
#define VAA_ICEBALL		563 // 563=pushed the iceball into the ice hole?
#define VAA_ICEROCK		564 // 564=pushed the rock onto the ice hole?
#define VAA_POISONPIT	565 // 565=opened the Poisonous Pit entrance?
#define VAA_CAVITYROCK	566 // 566=pushed down the Caliginous Cavities rock?
#define VAA_BODZHANEST	567 // 567=found the Bodzha Nest entrance?
#define VAA_CASTLEROCK	568 // 568=pushed down the Sankyo Castle rocks?
#define VAA_WHAPUNI		569 // 569=entered Whapuni with friends
#define VAA_CASTLEWALK	570 // 570=opened the chest walkway in sankyo castle
#define VAA_AAPRESENT	571 // 571-591=has any present tagged N-571 been collected? Which?
#define VAA_PUPPETLIFE2 592 // 592-596=upperhalf of puppet life
#define VAA_BARONCACHE	597 // 597=opened the secret hiding spot in baron room
#define VAA_BUNNYDRESS	598 // 598=got the potion from Bunny chamber
#define VAA_CLOUDLADDER	599 // 599=dropped the cloud ladder

#define VAA_UPGRADENUM	600 // 600=how many upgrades chosen?
#define VAA_UPGRADESTAT	601 // 601-630=has each specific upgrade been chosen?
#define VAA_OBJECTIVES	631 // 631-659=has each objective been completed?
#define VAA_WOODYAXE	660 // 660=get woody's axe 3/3 beef critical
#define VAA_CELLARSNOW	661 // 661=cellar snowman door cache (1/1 safety lifeberry) (cache moved)
#define VAA_POLARPARKA	662 // 662=polar pond parka 4/2 toasty survival
#define VAA_GRIZCACHE	663 // 663=grizzly bear cache H (rest moved)
#define VAA_LUCKYPARK	664	// 664=Vault 5/5 lucky parka of luck
#define VAA_VAULTCACHE1	665 // 665=vault $20 cache
#define VAA_VAULTCACHE2	666 // 666=vault $100 cache
#define VAA_VAULTCACHE3	667 // 667=vault $50 plus ruby money only (ruby moved)
#define VAA_VAULTCACHE4	668 // 668=vault diamond
#define VAA_VAULTROPE	669 // 669=rope in vault
#define VAA_ARMYCACHE	670 // 670=potion storage in army camp - heal level 4 (rest moved)
#define VAA_FARMAMULET	671 // 671=feral farm 8/2 druidic brambles
#define VAA_ARENACACHE	672	// 672=arena cache, 5/9 thorny axe of vampirism (rest moved)
#define VAA_ARENAROPE	673 // 673=arena rope
#define VAA_CIVIECACHE	674 // 674=arena civie cache, 8/20 energetic parka of luck, toughness, energy (rest moved)
#define VAA_MIMICAMULET 675 // 675=got mimic amulet 2/8 boney bonehead
#define VAA_CREEPAMULET	676 // 676=got spooky amulet boney amulet drain, bonehead, bone bomb
#define VAA_SHROOMULET	677 // 677=got sharona amulet 6/2 holy amulet lifeberry, brambles, shroom
#define VAA_CUREPOTIONS 678 // 678=got curaga's potions (rest moved)
#define VAA_CUREBIT		679 // 679=got curaga's artifact bit
#define VAA_CUREAMUET	680 // 680=got curaga's amulet 16/12 nothing
#define VAA_HATCHET		681 // 681=got kinyova hatchet (1/1 sharpened nothing)
#define VAA_KINYOVAXE	682 // 682=got kinyova's axe (12/12 nothing)
#define VAA_SHOCKCACHE	683 // 683=got shock cache and parka 7/9 electric energy, static, shock orb (potions moved)
#define VAA_SOUPCACHE	684 // 684=got soup (milk moved)
#define VAA_ROSEPARKA	685 // 685=got rosetta parka + soup (11/15 nothing) (bonus soup moved)
#define VAA_STORMULET	686 // 686=got storm amulet 10/2 shocking gale, tornado, berserk
#define VAA_TOYCACHE	687 // 687=got toy factory super secret inc boom parka (10/10 nuclear strength, burninate, airstrike)  (all items moved)
#define VAA_ICECACHE	688 // 688=got ice cache inc freeze amulet 7/5 mirrored cryozoid, cold wave, ice cube (potions moved)
#define VAA_POISONAM	689 // 689=got poison amulet 9/1 cursed plague
#define VAA_POISONAXE	690 // 690=got poison axe 8/2 ebola poison
#define VAA_POISONPARK	691 // 691=got poison parka 9/1 sickening energy
#define VAA_PITBLOODNW	692 // 692=poison pit NW alien bloods, second moved
#define VAA_PITBLOODNE	693 // 693=poison pit NE alien blood
#define VAA_DRAFTYADENW	694	// 694=drafty den NW coolade, second moved
#define VAA_DRAFTYFLAKE 695 // 695=drafty den flake door cache coolade, rest moved
#define VAA_ULTIMATE	696 // 696=ultimate reward, artbit, K L M (lenses moved)
#define VAA_PEAKLIONS	697 // 697=beaten thornpaw lions on peak
#define VAA_MAGICJUNKED	698 // 698=junksmithed a magic or better item
#define VAA_CLOCKGRADE	699	// 699=upgraded a bot for the clockmaker

#define VAA_RUMBLING	700 // 700=are we rumbling? 0=no, 1=onion, 2=bunny)
#define VAA_OUTPOST		700 // 701-706=state of outpost N-700
#define VAA_CLOCKGOT	707 // 707: unlocked clockmaker?
#define VAA_FIREAXE		708 // 708: got fire axe
#define VAA_FIREAMULET	709 // 709: got fire amulet
//AA cache redo variables!
#define VAA_CELLARDIAM	710 // 710: cellar snowman door diamond
#define VAA_CELLARSTONE	711 // 711: cellar stone soup
#define VAA_CELLARLIFE	712 // 712: cellar lifeade
#define VAA_CELLARASB1	713 // 713: cellar asbestos 1
#define VAA_CELLARASB2	714 // 714: cellar asbestos 2
#define VAA_GRIZWATER	715 // 715: water in grizzly cache
#define VAA_GRIZEMERALD	716 // 716: emerald in griz cache
#define VAA_GRIZRUBY1	717 // 717: grizzly ruby 1
#define VAA_GRIZRUBY2	718 // 718: grizzly ruby 2
#define VAA_GRIZGOLD1	719 // 719: grizzly gold 1
#define VAA_GRIZGOLD2	720 // 720: grizzly gold 2
#define VAA_GRIZGOLD3	721 // 721: grizzly gold 3
#define VAA_VAULTRUBY	722 // 722: vault ruby in ne cache
#define VAA_ARMYPOT2	723 // 723: army potions 2
#define VAA_ARMYPOT3	724 // 723: army potions 3
#define VAA_ARMYPOT4	725 // 723: army potions 4
#define VAA_ARMYPOT5	726 // 723: army potions 5
#define VAA_ARMYPOT6	727 // 723: army potions 6
#define VAA_ARMYPOT7	728 // 723: army potions 7
#define VAA_ARMYPOT8	729 // 723: army potions 8
#define VAA_ARMYPOT9	730 // 723: army potions 9
#define VAA_ARMYPOT10	731 // 723: army potions 10
#define VAA_ARMYPOT11	732 // 723: army potions 11
#define VAA_ARMYPOT12	733 // 723: army potions 12
#define VAA_ARMYPOT13	734 // 723: army potions 13
#define VAA_ARMYPOT14	735 // 723: army potions 14
#define VAA_ARMYPOT15	736 // 723: army potions 15
#define VAA_ARMYPOT16	737 // 723: army potions 16
#define VAA_ARMYPOT17	738 // 723: army potions 17
#define VAA_ARMYPOT18	739 // 723: army potions 18
#define VAA_ARENADIAM	740 // 740: diamond in arena cache
#define VAA_CIVVIEDIAM	741 // 741: diamond in civvie arena cache
#define VAA_CIVVIEART	742 // 742: artbit in civvie arena cache
#define VAA_CURAGAJUICE 743 // 743: second curaga brainjuice
#define VAA_CURAGACOLA1 744 // 744: first curaga cola
#define VAA_CURAGACOLA2 745 // 745: second curaga cola
#define VAA_SHOCKSODA1	746 // 746: first hiemal shock
#define VAA_SHOCKSODA2	747 // 747: second hiemal shock
#define VAA_ROSEMILK	748 // 748: rosetta milk
#define VAA_MORESOUP	749 // 749: rosetta bonus soup
#define VAA_TOYPARKA	750 // 750: boom parka
#define VAA_TOYARTBIT	751 // 751: artbit in toy cache
#define VAA_TOYPOTION	752 // 752: explosivo in toy cache
#define VAA_TOYLENS		753 // 753: lens in toy cache
#define VAA_COOLADE1	754 // 754: drafty coolade 1
#define VAA_COOLADE2	755 // 755: drafty coolade 2
#define VAA_COOLDIAMOND	756 // 756: flake door diamond
#define VAA_COOLLENSL	757 // 756: flake door L
#define VAA_DENNWCOOL	758 // 758: draft den NW coolade 2
#define VAA_POISONBLOOD 759 // 759: poison pit NW alien blood 2
#define VAA_ULTIMATEK	760 // 760: ultimate reward K
#define VAA_ULTIMATEL	761 // 761: ultimate reward L
#define VAA_ULTIMATEM	762 // 762: ultimate reward M
#define VAA_RIMEAXE		763 // 763: rime axe in draftyden
#define VAA_FRIDGEPARKA	764 // 764: parka of refrig in drafty
#define VAA_ONIONSANKYO 765 // 765: beaten onion hideout in sankyo
#define VAA_BUNNYSANKYO 766 // 766: beaten bunny stronghold in sankyo
#define VAA_FROSTSANKYO 767 // 767: beaten frost keep in sankyo
#define VAA_BEARSANKYO	768 // 768: beaten bear mansion in sankyo
#define VAA_VOLCANEXPLO	769 // 769: volcano explosivo
#define VAA_VOLROCKET1	770 // 770: first volcano rocket sauce
#define VAA_VOLROCKET2	771 // 771: second volcano rocket sauce
#define VAA_TREASURYK	772 // 772: treasury lens
#define VAA_TREASCOW1	773 // 773: treasury mad cow 1
#define VAA_TREASCOW2	774 // 774: treasury mad cow 2
#define VAA_TREASLUCK	775 // 775: treasury luckenbrau
#define VAA_TREASSMRT	776 // 776: treasury smart sauce
#define VAA_TREASLIFE	777 // 777: treasury lifeade
#define VAA_TREASDIAM1	778 // 778: treasury diamond 1
#define VAA_TREASDIAM2	779 // 779: treasury diamond 2
#define VAA_TREASDIAM3	780 // 780: treasury diamond 3
#define VAA_TREASEMER1	781 // 781: treasury emerald 1
#define VAA_TREASEMER2	782 // 782: treasury emerald 2
#define VAA_TREASEMER3	783 // 783: treasury emerald 3
#define VAA_TREASEMER4	784 // 784: treasury emerald 4
#define VAA_TREASEMER5	785 // 785: treasury emerald 5
#define VAA_TREASRUBY1	786 // 786: treasury ruby 1
#define VAA_TREASRUBY2	787 // 787: treasury ruby 2
#define VAA_TREASRUBY3	788 // 788: treasury ruby 3
#define VAA_TREASRUBY4	789 // 789: treasury ruby 4
#define VAA_TREASRUBY5	790 // 790: treasury ruby 5
#define VAA_CASTLEGOLD	791 // 791: castle cash cache
#define VAA_CASTLEDEF1	792 // 792: castle deflector 1
#define VAA_CASTLEDEF2	793 // 793: castle deflector 2
#define VAA_CASTLEDEF3	794 // 794: castle deflector 3
#define VAA_CASTLECOL1	795 // 795: castle cola 1
#define VAA_CASTLECOL2	796 // 796: castle cola 2
#define VAA_CASTLECOL3	797 // 797: castle cola 3
// 798-799: unused

#define VAA_TRIAL1BIT	800 // 800-807: trial rewards for trial 1
#define VAA_TRIAL1POT1	801 // 800-807: trial rewards for trial 1
#define VAA_TRIAL1POT2	802 // 800-807: trial rewards for trial 1
#define VAA_TRIAL1POT3	803 // 800-807: trial rewards for trial 1
#define VAA_TRIAL1POT4	804 // 800-807: trial rewards for trial 1
#define VAA_TRIAL1POT5	805 // 800-807: trial rewards for trial 1
#define VAA_TRIAL1POT6	806 // 800-807: trial rewards for trial 1
#define VAA_TRIAL1EQUIP	807 // 800-807: trial rewards for trial 1
//808-809: unused
#define VAA_TRIAL2BIT	810 // 810-817: trial rewards for trial 2
#define VAA_TRIAL2POT1	811 // 810-817: trial rewards for trial 2
#define VAA_TRIAL2POT2	812 // 810-817: trial rewards for trial 2
#define VAA_TRIAL2POT3	813 // 810-817: trial rewards for trial 2
#define VAA_TRIAL2POT4	814 // 810-817: trial rewards for trial 2
#define VAA_TRIAL2POT5	815 // 810-817: trial rewards for trial 2
#define VAA_TRIAL2POT6	816 // 810-817: trial rewards for trial 2
#define VAA_TRIAL2EQUIP	817 // 810-817: trial rewards for trial 2
//818-819: unused
#define VAA_TRIAL3BIT	820 // 820-827: trial rewards for trial 3
#define VAA_TRIAL3POT1	821 // 820-827: trial rewards for trial 3
#define VAA_TRIAL3POT2	822 // 820-827: trial rewards for trial 3
#define VAA_TRIAL3POT3	823 // 820-827: trial rewards for trial 3
#define VAA_TRIAL3POT4	824 // 820-827: trial rewards for trial 3
#define VAA_TRIAL3POT5	825 // 820-827: trial rewards for trial 3
#define VAA_TRIAL3POT6	826 // 820-827: trial rewards for trial 3
#define VAA_TRIAL3EQUIP	827 // 820-827: trial rewards for trial 3
//828-829: unused
#define VAA_TRIAL4BIT	830 // 830-837: trial rewards for trial 4
#define VAA_TRIAL4POT1	831 // 830-837: trial rewards for trial 4
#define VAA_TRIAL4POT2	832 // 830-837: trial rewards for trial 4
#define VAA_TRIAL4POT3	833 // 830-837: trial rewards for trial 4
#define VAA_TRIAL4POT4	834 // 830-837: trial rewards for trial 4
#define VAA_TRIAL4POT5	835 // 830-837: trial rewards for trial 4
#define VAA_TRIAL4POT6	836 // 830-837: trial rewards for trial 4
#define VAA_TRIAL4EQUIP	837 // 830-837: trial rewards for trial 4
//838-839: unused
#define VAA_TRIAL5BIT	840 // 840-847: trial rewards for trial 5
#define VAA_TRIAL5POT1	841 // 840-847: trial rewards for trial 5
#define VAA_TRIAL5POT2	842 // 840-847: trial rewards for trial 5
#define VAA_TRIAL5POT3	843 // 840-847: trial rewards for trial 5
#define VAA_TRIAL5POT4	844 // 840-847: trial rewards for trial 5
#define VAA_TRIAL5POT5	845 // 840-847: trial rewards for trial 5
#define VAA_TRIAL5POT6	846 // 840-847: trial rewards for trial 5
#define VAA_TRIAL5EQUIP	847 // 840-847: trial rewards for trial 5
//848: unused
#define VAA_TRIALREFILL 849 // 849: need to refill health and magic at trial entrance
// special achievement nonsense
#define VAA_ARENA		850 // 850-909: how many times winning each arena match?
#define VAA_BRUTAL		910 // 910: stored bonus damage for the Brutal modifier
#define VAA_COMBOOM		911 // 911: combo for explosivo. wow... did not plan the variable number like that
#define VAA_SKILLBONUS	912 // 912: overflow for skill points!

#define PLAYER_SKILLS	(50)	// the most skills the player can have.  Presumably the map will be setup appropriately
#define MAX_LEVEL		(50)	// the highest level you can level up to

// special moves
#define SM_NONE		(0)
#define SM_NOVA		(1)
#define SM_WHIRL	(2)
#define SM_TURRET	(3)

// places you can visit (added to VAR_VISITED)
#define PLC_EISBURG	(0)
#define PLC_ARENA	(1)
#define PLC_BASKER	(2)
#define PLC_CLIFFS	(3)
#define PLC_HIGHWAY	(4)
#define PLC_GORGE	(5)

// kill count numbers
#define KC_GHOST	(0)
#define KC_ICE		(2)
#define KC_BEAR		(4)
#define KC_MICE		(6)
#define KC_BODZHA	(8)
#define KC_PUPPET	(10)
#define KC_MONKEY	(12)
#define KC_STONE	(14)
#define KC_SHROOM	(16)
#define KC_SOLDIER	(18)
#define KC_PLANT	(20)
#define KC_ONION	(22)
#define KC_BUNNY	(24)

#pragma pack(push, 1)
typedef struct player_t
{
	char profile[16];
	int money;
	byte worldNum;
	byte levelNum;
	int  boredom;

	word hearts;
	word maxHearts;
	//byte hearts;
	//byte maxHearts;
	//byte firePower;
	//byte fireRate;
	byte fireFlags;
	byte fireRange;

	byte reload;
	byte pushPower;	// for pushing pushy blocks
	byte chatClock;
	byte saveClock;
	byte stone;

	int  invinc;
	int  destx,desty;	// where the player should be put, used between levels, and for
						// saved games
	byte speed;	// accelerated

	byte difficulty;
	dword monsterPoints;
	dword batLevel;
	int  monsType;
	word startHearts;
	dword cheatsOn;		// flags for which whole-game-only cheats are on in this game
	dword xtraVar;		// an extra variable for some characters
	byte crystalBall;
	byte xtraByte;		// an extra byte for some characters
	byte clockRunning;	// if the time limit clock should be going

	dword timeLimit;	// time passed for certain short-term timed things, like boss-killing
	// end-game stats
	dword timeToFinish;
	dword bestCombo;
	dword shotsFired;
	dword specialShotsFired;
	dword shouldMonsters;
	dword gemsGotten;
	dword hitsTaken;
	byte  rank;
	// vars
	byte  var[982];

	char addonName[12];
	word  arenaHearts,arenaMagic,arenaStamina;
	dword playClock;
	dword potionsDrunk;
	dword kills;
	dword herbsPicked;
	dword holesDug;
	dword damageDone;

	// new stuff, don't remove
	dword xp,xpNeed;
	byte level;
	byte skillPts;

	byte damage;	// the sum of your axe damage and any boosts from skills
	byte axeSpeed;	// the sum of your axe speed and skill boosts
	byte axeMode;	// 0=melee, 1=throw
	byte armor;		// defense, from parka plus boosts from skills

	byte magic,maxMagic,startMagic;
	byte stamina,maxStamina;	// maxStamina is always your current health, plus a bonus from any stamina-boosting skills (which just add to the percent of your original health you can count)
	byte staminaClock,magicClock;

	equip_t axe;
	equip_t parka;
	equip_t amulet;

	byte lens[2];

	equip_t items[MAX_EQUIP];
	equip_t shopInv[SHOP_MAX][SHOP_SIZE];

	byte skillHave[PLAYER_SKILLS];
	byte skillLvl[PLAYER_SKILLS];
	word skillClock[PLAYER_SKILLS];

	word talentPts[MAX_TALENTS];	// how many points you have toward the next level of the talent
	byte talentLevel[MAX_TALENTS];

	byte parry;	// while this is non-zero, anything that would hit you is blocked, and bullets are reflected
	byte spclMove;	// what special move you're doing now
	byte whirlClock;
	byte spell;
	byte lastTown;	// when you load a saved game, you appear in the last town you visited
	// timers for various spells
	word shockClock;
	word berserkClock;
	word parkaClock;	// timer for parka's magic power if needed
	word amuletClock;	// timer for amulet's magic power if needed

	word potionClock;
	byte potionType;
	byte potionPower;

	// message log
	char log[MAX_LOG][LOG_LEN];
	byte logBright,logTime;

	// arena stuff
	byte arenaLvl;	// which match is it?
	word arenaTime;
	byte arenaWave;	// which wave you are on if multiple
	word arenaSpawn;
} player_t;
#pragma pack(pop)

extern player_t player;

void InitPlayer(byte initWhat,byte world,byte level);
void ExitPlayer(void);
void PlayerControlMe(Guy *me,Map *map,mapTile_t *mapTile,world_t *world);

void DoPlayerFacing(byte c,Guy *me);
byte PlayerGetItem(byte itm,int val1,int val2);
void TryToGetItem(byte item, int val1, int val2); // AA addition for copying
void PlayerWinLevel(byte w,byte l,byte isSecret);
void SetPlayerHP(int hp);
void SetPlayerGlow(byte v);
byte GetPlayerGlow(void);
byte PlayerPushMore(void);
void SetTportClock(byte tp);
byte GetTportClock(void);
float CalcPercent(player_t *p);
void PlayerSetVar(int v,int val);
void PlayerClearTempVars(void);
void PlayerGetMonsterPoints(int level);
void PlayerGetBossPoints(int level);

void PlayerGetExactXP(int amt);
void PlayerGetSkillPoints(byte num);

void PlayerCalcStats(void);
void PlayerBackToLastTown(byte *mapNum);
void Dig(Map *map,int x,int y);
void PlayerGetMoney(int amt);

void MadcapPlayer(void);
void PlayerAddKill(byte type);

byte PlayerUpgradeStatus(byte n);

#endif

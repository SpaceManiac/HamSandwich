#include "winpch.h"
#include "sound.h"
#include "display.h"
#include "repair.h"
#include "progress.h"
#include "music.h"
#include "shop.h"

soundDesc_t soundInfo[MAX_SOUNDS]={
	{SND_NONE,"No Sound At All!!",ST_EFFECT},
	{SND_MENUCLICK,"Menu Click",ST_INTFACE},
	{SND_MENUSELECT,"Menu Select",ST_INTFACE},
	{SND_PAUSE,"Pause Game",ST_INTFACE},
	{SND_SAVEGAME,"Save Game",ST_INTFACE|ST_VOCAL},
	{SND_LOADGAME,"Load Game",ST_INTFACE|ST_VOCAL},
	{SND_HAMMERBONK,"Hammer Bonk",ST_EFFECT},
	{SND_HAMMERREFLECT,"Hammer Reflect",ST_EFFECT},
	{SND_BULLETREFLECT,"Bullet Reflect",ST_EFFECT},
	{SND_BOMBREFLECT,"Bomb Bounce",ST_EFFECT},
	{SND_MISSILEBOOM,"Missile Boom",ST_EFFECT},
	{SND_ACIDSPLAT,"Acid Splat",ST_EFFECT},
	{SND_ENERGYBONK,"Red Bullet Hit",ST_EFFECT},
	{SND_BOMBBOOM,"Bomb Boom",ST_EFFECT},
	{SND_BULLETHIT,"Bullet Hit",ST_EFFECT},
	{SND_MEGABEAMHIT,"Death Beam Hit",ST_EFFECT},
	{SND_BOMBTHROW,"Bomb Throw",ST_PLAYER|ST_VOCAL},
	{SND_MISSILELAUNCH,"Missile Launch",ST_EFFECT|ST_PLAYER},
	{SND_FLAMEGO,"Flame",ST_EFFECT},
	{SND_BULLETFIRE,"Bullet Fire",ST_EFFECT},
	{SND_KOOLKAT,"Kool Kat Bad",ST_VOCAL|ST_INTFACE|ST_PLAYER},
	{SND_MEGABEAM,"Death Beam",ST_EFFECT},
	{SND_HAMMERTOSS,"Throw Hammer",ST_PLAYER},
	{SND_CHEATWIN,"Cheat!!!",ST_INTFACE|ST_VOCAL},
	{SND_HEALTHCHEAT,"Aaahhh",ST_INTFACE|ST_VOCAL},
	{SND_INVINCCHEAT,"Sickly Yay",ST_VOCAL},
	{SND_DROWN,"Drown",ST_PLAYER|ST_VOCAL},
	{SND_ITEMDROP,"Item Drop",ST_INTFACE},
	{SND_GOTOMAP,"Goto Map",ST_INTFACE},
	{SND_WINLEVEL,"Win Level",ST_INTFACE|ST_VOCAL},
	{SND_WALLDOWN,"Zap Wall",ST_EFFECT},
	{SND_WALLUP,"Make Wall",ST_EFFECT},
	{SND_LIGHTSON,"Lights On",ST_EFFECT},
	{SND_TELEPORT,"Teleport",ST_EFFECT},
	{SND_MESSAGE,"Message Appears",ST_INTFACE},
	{SND_SKELKICK,"Bonehead Kick",ST_MONSTER},
	{SND_SKELSHOOT,"Bonehead Shoot",ST_MONSTER},
	{SND_SKELOUCH,"Bonehead Ouch",ST_MONSTER},
	{SND_SKELDIE,"Bonehead Die",ST_MONSTER},
	{SND_BATEYES,"Bat Attack",ST_MONSTER},
	{SND_BATDIVE,"Bat Dive",ST_MONSTER},
	{SND_BATOUCH,"Bat Ouch",ST_MONSTER},
	{SND_BATDIE,"Bat Die",ST_MONSTER},
	{SND_SPD1OUCH,"",0},
	{SND_SPD1DIE,"Spider Die",ST_MONSTER},
	{SND_SPD1GNAW,"Eensy Bite",ST_MONSTER},
	{SND_SPD2OUCH,"Spider Ouch",ST_MONSTER},
	{SND_SPD2DIE,"Spider Die",ST_MONSTER},
	{SND_SPD2SPIT,"Spider Spit",ST_MONSTER},
	{SND_ZOMBIEOUCH,"Zombie Ouch",ST_MONSTER},
	{SND_ZOMBIEDIE,"Zombie Die",ST_MONSTER},
	{SND_ZOMBIELEAP,"Zombie Leap",ST_MONSTER|ST_VOCAL},
	{SND_EGGSACDIE,"Egg Sac Die",ST_MONSTER},
	{SND_EGGSACBIRTH,"Egg Sac Hatch",ST_MONSTER},
	{SND_SPD3OUCH,"Mama Spider Ouch",ST_MONSTER},
	{SND_SPD3DIE,"Mama Spider Die",ST_MONSTER},
	{SND_SPD3CHOMP,"Mama Spider Bite",ST_MONSTER},
	{SND_SPD3SPEW,"Mama Spider Spit",ST_MONSTER},
	{SND_PYGMYOUCH,"Pygmy Ouch",ST_MONSTER},
	{SND_PYGMYDIE,"Pygmy Die",ST_MONSTER},
	{SND_PYGMYYAWN,"Pygmy Yawn",ST_MONSTER|ST_VOCAL},
	{SND_PYGMYSPIN,"Pygmy Spin",ST_MONSTER},
	{SND_PYGMYSTAB,"Pygmy Stab",ST_MONSTER},
	{SND_PYGMYANGRY,"Unga Bunga",ST_MONSTER|ST_VOCAL},
	{SND_SERPENTOUCH,"Zoid Ouch",ST_MONSTER},
	{SND_SERPENTDIE,"Zoid Die",ST_MONSTER},
	{SND_SERPENTSPIT,"Zoid Spit",ST_MONSTER},
	{SND_MATTIEOUCH,"Matilda Ouch",ST_MONSTER},
	{SND_FRANKOUCH2,"Mini-Frankenjulie Ouch",ST_MONSTER},
	{SND_MATTIEDIE,"Matilda Die",ST_MONSTER},
	{SND_MATTIECLAW,"Matilda Claw",ST_MONSTER},
	{SND_MATTIESHOOT,"Matilda Shoot",ST_MONSTER},
	{SND_MATTIESTOMP,"Matilda Stomp",ST_MONSTER},
	{SND_GINGEROUCH,"Ninjabread Ouch",ST_MONSTER},
	{SND_GINGERDIE,"Ninjabread Die",ST_MONSTER},
	{SND_GINGERJKICK,"Ninjabread Jumpkick",ST_MONSTER},
	{SND_GINGERCOMBO,"Ninjabread Combo",ST_MONSTER},
	{SND_PUMPKINDIE,"Pumpkin Pop",ST_MONSTER},
	{SND_HAMMERUP,"Hammer Up",ST_INTFACE|ST_VOCAL|ST_PLAYER},
	{SND_PANTS,"Pants!",ST_INTFACE|ST_VOCAL|ST_PLAYER},
	{SND_REVERSE,"Reverse Hammer",ST_EFFECT},
	{SND_REFLECT,"Sproingy Spring",ST_EFFECT},
	{SND_WEAPON,"Get Weapon",ST_INTFACE|ST_PLAYER},
	{SND_FOOD,"Get Food",ST_INTFACE|ST_PLAYER},
	{SND_SHIELD,"Energy Shield",ST_INTFACE|ST_PLAYER},
	{SND_GETBRAIN,"Get Brain",ST_INTFACE|ST_PLAYER},
	{SND_GETKEY,"Get Key",ST_INTFACE|ST_PLAYER},
	{SND_GETKEYCHAIN,"Bingo!",ST_INTFACE|ST_VOCAL|ST_PLAYER},
	{SND_LOONYKEY,"Get Loonykey",ST_INTFACE|ST_PLAYER|ST_VOCAL},
	{SND_BOUAPHAOUCH,"Bouapha Ouch",ST_PLAYER},
	{SND_BOUAPHADIE,"Bouapha Die",ST_PLAYER},
	{SND_BOUAPHABORED,"Ho Hum",ST_PLAYER|ST_VOCAL},
	{SND_DOOROPEN,"Door Open",ST_EFFECT},
	{SND_WORLDTURN,"World Select Turn",ST_INTFACE},
	{SND_WORLDPICK,"Select A World",ST_INTFACE},
	{SND_BLOCKPUSH,"Push A Block",ST_PLAYER|ST_EFFECT},
	{SND_MUSHMAD,"Mush Angry",ST_MONSTER|ST_VOCAL},
	{SND_MUSHMISSILE,"Mush Missile",ST_MONSTER},
	{SND_MUSHSPORES,"Mush Spores",ST_MONSTER},
	{SND_MUSHOUCH,"Mush Ouch",ST_MONSTER},
	{SND_MUSHDIE,"Mush Die",ST_MONSTER},
	{SND_SZGRAB,"Super Zombie BRAINS",ST_MONSTER|ST_VOCAL},
	{SND_BABYSNORE,"Thingie Snore",ST_MONSTER|ST_VOCAL},
	{SND_HAPPYFLEX,"Happy Stick Flex",ST_MONSTER|ST_VOCAL},
	{SND_HAPPYOUCH,"Happy Stick Ouch",ST_MONSTER},
	{SND_HAPPYDIE,"Happy Stick Die",ST_MONSTER},
	{SND_HAPPYCHOMP,"Happy Stick Chomp",ST_MONSTER},
	{SND_THINGVOMIT,"Thing Tongue",ST_MONSTER},
	{SND_ZOMBONIZOOM,"Zomboni Engine",ST_MONSTER},
	{SND_SEALOUCH,"Seal Ouch",ST_MONSTER},
	{SND_SEALDIE,"Seal Die",ST_MONSTER},
	{SND_PENGDIE,"Pengulon Die",ST_MONSTER},
	{SND_PENGPECK,"Pengulon Peck",ST_MONSTER},
	{SND_SNOWOUCH,"Snowguy Ouch",ST_MONSTER},
	{SND_SNOWDIE,"Snowguy Die",ST_MONSTER},
	{SND_SNOWTHROW,"Snowguy Throw",ST_MONSTER},
	{SND_SVENHI,"Yeti: Sven Hello",ST_MONSTER|ST_VOCAL},
	{SND_BJORNHI,"Yeti: Bjorn Hello",ST_MONSTER|ST_VOCAL},
	{SND_YETIOUCH,"Yeti Ouch",ST_MONSTER},
	{SND_YETIDIE,"Yeti Die",ST_MONSTER},
	{SND_YETIYODEL,"Yeti Yodel",ST_MONSTER|ST_VOCAL},
	{SND_CACTONSHOOT,"Cacton Shoot",ST_MONSTER},
	{SND_CACTONDIE,"Cacton Die",ST_MONSTER},
	{SND_DJINNIOUCH,"Djinni Ouch",ST_MONSTER},
	{SND_DJINNIDIE,"Djinni Die",ST_MONSTER},
	{SND_DJINNIPUNCH,"Djinni Punch",ST_MONSTER},
	{SND_DJINNIBOO,"Djinni Boo",ST_MONSTER|ST_VOCAL},
	{SND_LAMPPOOF,"Magic Lamp Steam",ST_MONSTER},
	{SND_LAMPDIE,"Magic Lamp Vanish",ST_MONSTER},
	{SND_MUMBLECRUSH,"Mumble Smack",ST_MONSTER},
	{SND_ROLYPOLYWALL,"Roly Poly Clonk",ST_MONSTER|ST_EFFECT},
	{SND_ROCKBOUNCE,"Rock Bounce",ST_EFFECT},
	{SND_SPHINXSNEEZE,"Sphinxter Sneeze",ST_MONSTER|ST_VOCAL},
	{SND_SPHINXSTOMP,"Sphinxter Stomp",ST_MONSTER},
	{SND_CRAZYSTAB,"Crazy Stab",ST_MONSTER},
	{SND_WACKOOUCH,"Wacko Ouch",ST_MONSTER},
	{SND_WACKODIE,"Wacko Die",ST_MONSTER|ST_VOCAL},
	{SND_BOILERHIT,"Boiler Ouch",ST_MONSTER},
	{SND_GREATPKOUCH,"Great Pumpkin Ouch",ST_MONSTER},
	{SND_GREATPKDIE,"Great Pumpkin Die",ST_MONSTER},
	{SND_GREATPKVOMIT,"Great Pumpkin Spew",ST_MONSTER},
	{SND_DRLLAUGH,"Dr. Lunatic Get 'Em",ST_MONSTER|ST_VOCAL},
	{SND_DRLFEAR,"Dr. Lunatic Scared",ST_MONSTER|ST_VOCAL},
	{SND_DRLOUCH,"Dr. Lunatic Ouch",ST_MONSTER},
	{SND_DRLDIE,"Dr. Lunatic Die",ST_MONSTER},
	{SND_SDZLOUCH,"SDZL Ouch",ST_MONSTER},
	{SND_SDZLDIE,"SDZL Die",ST_MONSTER},
	{SND_SDZLSLASH,"SDZL Slash",ST_MONSTER},
	{SND_SDZLPOUND,"SDZL Ground Pound",ST_MONSTER},
	{SND_SDZLSPIT,"SDZL Spit",ST_MONSTER},
	{SND_SANTAOUCH,"Santa Ouch",ST_MONSTER},
	{SND_SANTADIE,"Santa Die",ST_MONSTER},
	{SND_SANTAHIT,"Santa Smack",ST_MONSTER},
	{SND_MINECART,"Mine Cart Roll",ST_MONSTER|ST_EFFECT},
	{SND_HAMUMU,"Hamumu Theme",ST_INTFACE},
	{SND_VAMPATTK,"Vampire Attack",ST_MONSTER},
	{SND_VAMPFEAR,"Vampire Garlic",ST_MONSTER|ST_VOCAL},
	{SND_VAMPOUCH,"Vampire Ouch",ST_MONSTER},
	{SND_VAMPDIE,"Vampire Die",ST_MONSTER},
	{SND_ARMORSHOOT,"Power Armor Shoot",ST_PLAYER},
	{SND_TRICERAROAR,"Triceroid Roar",ST_MONSTER},
	{SND_CHESTPOUND,"Kongor Chest Pound",ST_MONSTER},
	{SND_ROBOOUCH,"Robot Ouch",ST_MONSTER},
	{SND_ROBODIE,"Robot Die",ST_MONSTER},
	{SND_FACTORYDIE,"Robofactory Die",ST_MONSTER},
	{SND_COFFIN,"Coffin Open",ST_MONSTER|ST_EFFECT|ST_VOCAL},
	{SND_GHOSTOUCH,"Ghost Ouch",ST_MONSTER},
	{SND_GHOSTDIE,"Ghost Die",ST_MONSTER},
	{SND_GHOSTTPORT,"Ghost Teleport",ST_MONSTER},
	{SND_GHOSTYELL,"Ghost Scream",ST_MONSTER},
	{SND_LEFTYOUCH,"Lefty Ouch",ST_MONSTER},
	{SND_LEFTYDIE,"Lefty Die",ST_MONSTER},
	{SND_LEFTYWAVE,"Lefty Hello",ST_MONSTER|ST_VOCAL},
	{SND_UNGADANCE,"Unga Bunga Slow",ST_MONSTER|ST_VOCAL},
	{SND_SHAMANOUCH,"Shaman Ouch",ST_MONSTER},
	{SND_SHAMANDIE,"Shaman Die",ST_MONSTER},
	{SND_SHAMANZAP,"Shaman Zap",ST_MONSTER},
	{SND_SHAMANDANCE,"Unga Bunga High",ST_MONSTER|ST_VOCAL},
	{SND_PKSTEINBUMP,"Pumpkinstein Bump",ST_MONSTER},
	{SND_GLASSBREAK,"Glass Shatter",ST_EFFECT},
	{SND_GLASSHIT,"Stan Glass Ouch",ST_MONSTER},
	{SND_GLASSDIE,"Stan Glass Die",ST_MONSTER|ST_VOCAL},
	{SND_GLASSBLOCK,"Stan Glass Block",ST_MONSTER},
	{SND_TRICEROUCH,"Triceroid Ouch",ST_MONSTER},
	{SND_TRICERDIE,"Triceroid Die",ST_MONSTER},
	{SND_TRICERBUMP,"Triceroid Bump",ST_MONSTER},
	{SND_COUNTOUCH,"Countess Ouch",ST_MONSTER},
	{SND_COUNTDIE,"Countess Die",ST_MONSTER},
	{SND_COUNTHELLO,"Countess Good Eve",ST_MONSTER|ST_VOCAL},
	{SND_COUNTSUMMON,"Countess Arise",ST_MONSTER|ST_VOCAL},
	{SND_COUNTSHOOT,"Countess Shoot",ST_MONSTER},
	{SND_COUNTCHARGE,"Countess Charge",ST_MONSTER},
	{SND_EGGOUCH,"Xeno Egg Ouch",ST_MONSTER},
	{SND_EGGDIE,"Xeno Egg Die",ST_MONSTER},
	{SND_EGGOPEN,"Xeno Egg Hatch",ST_MONSTER},
	{SND_ALIENOUCH,"Xeno Ouch",ST_MONSTER},
	{SND_ALIENDIE,"Xeno Die",ST_MONSTER},
	{SND_ALIENSPIT,"Xeno Spit",ST_MONSTER},
	{SND_INVIDCHARGE,"Shock Trooper Dash",ST_MONSTER},
	{SND_ROBOSHOOT,"Robot Shoot",ST_MONSTER},
	{SND_FACTORYGO,"Robofactory Create",ST_MONSTER},
	{SND_KONGOUCH,"Kongor Ouch",ST_MONSTER},
	{SND_KONGDIE,"Kongor Die",ST_MONSTER},
	{SND_KONGHELLO,"Kongor KONGOR!",ST_MONSTER|ST_VOCAL},
	{SND_LIGHTNING,"Lightning Zap",ST_EFFECT},
	{SND_SLASH,"Machete Slash",ST_EFFECT},
	{SND_CLANG,"Machete Clang",ST_EFFECT},
	{SND_MINELAY,"Mine Drop",ST_PLAYER},
	{SND_RAGE,"RAGE!",ST_PLAYER|ST_INTFACE|ST_VOCAL},
	{SND_SPEEDUP,"Accelerate",ST_EFFECT|ST_INTFACE},
	{SND_ROBOBOUAPHAOUCH,"Power Armor Ouch",ST_PLAYER},
	{SND_ROBOBOUAPHAON,"Power Armor Activate",ST_PLAYER},
	{SND_ULTRAPYGMYOUCH,"Ultrapygmy Ouch",ST_MONSTER},
	{SND_ULTRAPYGMYDIE,"Ultrapygmy Die",ST_MONSTER},
	{SND_ULTRAPYGMYYAWN,"Ultrapygmy Yawn",ST_MONSTER},
	{SND_ULTRAPYGMYSTAB,"Ultrapygmy Stab",ST_MONSTER},
	{SND_ULTRAPYGMYANGRY,"Unga Bunga Quick",ST_MONSTER|ST_VOCAL},
	{SND_ROBOSTOMP,"Loonybot Stomp",ST_MONSTER},
	{SND_SHAMANANGRY,"Shaman Hey",ST_MONSTER|ST_VOCAL},
	{SND_ZAP,"Electrocute",ST_EFFECT},
	{SND_BALLLIGHTNING,"Ball Lightning",ST_MONSTER},
	{SND_VICTORY,"Victory Song",ST_INTFACE},
	{SND_COINGET,"Coin Collect",ST_INTFACE},
	{SND_BATSTARE,"Bat Stare",ST_MONSTER},
	{SND_CANDLEGET,"Candle Get",ST_INTFACE},
	{SND_ALLCANDLE,"Candle All Get",ST_INTFACE},
	{SND_CRAZYDASH,"Crazypants Dash",ST_VOCAL|ST_MONSTER},
	{SND_CRAZYSPIN,"Crazypants Spin",ST_VOCAL|ST_MONSTER},
	{SND_PYGMYCASE,"Pygmy Briefcase",ST_MONSTER},
	{SND_CRAZYMAD,"Crazypants Mad",ST_VOCAL|ST_MONSTER},
	{SND_YERFDOG,"Yerfdog Yerf",ST_MONSTER},
	{SND_SCUBA,"Scuba Tank",ST_INTFACE},
	{SND_BUBBLE,"Bubble Bloop",ST_EFFECT},
	{SND_DIVERANGRY,"Pygmy Diver Unga",ST_VOCAL|ST_MONSTER},
	{SND_DIVERDIE,"Pygmy Diver Die",ST_VOCAL|ST_MONSTER},
	{SND_GLUP,"Drown A Bit",ST_VOCAL|ST_MONSTER},
	{SND_OLAFHI,"Yeti: Olaf Hello",ST_VOCAL|ST_MONSTER},
	{SND_FOLLOWHI,"Bunny Awaken",ST_MONSTER},
	{SND_CARSTART,"Car Start",ST_EFFECT},
	{SND_CARGO,"Car Running",ST_EFFECT},
	{SND_JELLOOUCH,"Jellofish Ouch",ST_MONSTER},
	{SND_KINGDIE,"Zis Is Unacceptable",ST_VOCAL|ST_MONSTER},
	{SND_KINGCONES,"King Cone: Cones",ST_VOCAL|ST_MONSTER},
	{SND_KINGSCOFF,"King Cone: Scoff",ST_VOCAL|ST_MONSTER},
	{SND_FISHDIE,"Glurk!",ST_MONSTER},
	{SND_SUBSTART,"Mini-Sub Startup",ST_EFFECT},
	{SND_SUBSHOOT,"Mini-Sub Torpedo",ST_EFFECT},
	{SND_SUBMINE,"Mini-Sub Depth Charge",ST_EFFECT},
	{SND_OYOYOY,"Punkin Oy Oy Oy!",ST_VOCAL|ST_MONSTER},
	{SND_CUKEDANCE,"Sea Cuke Dance",ST_MONSTER},
	{SND_RUFFIAN,"Sea Cuke Ruffian!",ST_VOCAL|ST_MONSTER},
	{SND_HAVEATYOU,"Sea Cuke Have At You!",ST_VOCAL|ST_MONSTER},
	{SND_CUKEDIE,"Sea Cuke Oh My Heavens",ST_VOCAL|ST_MONSTER},
	{SND_CUKEOUCH,"Sea Cuke Ouch",ST_MONSTER},
	{SND_WILLYIDLE,"Wet Willy Bored",ST_VOCAL|ST_MONSTER},
	{SND_WILLYIDLE2,"Wet Willy Bored 2",ST_VOCAL|ST_MONSTER},
	{SND_BUNNYDIE,"Bunny Die",ST_MONSTER},
	{SND_MINDWIPE,"Mind Control",ST_EFFECT},
	{SND_SHARKATTACK,"Shark Attack",ST_MONSTER},
	{SND_SHARKOUCH,"Shark Ouch",ST_MONSTER},
	{SND_SHARKDIE,"Shark Die",ST_MONSTER},
	{SND_WIZHIT,"Wacky Wiz Alakazam",ST_MONSTER|ST_VOCAL},
	{SND_WIZOUCH,"Wacky Wizard Oy",ST_MONSTER|ST_VOCAL},
	{SND_WIZDIE,"Wacky Wizard Die",ST_MONSTER|ST_VOCAL},
	{SND_HAMMERDN,"Hammer Down",ST_PLAYER|ST_VOCAL|ST_INTFACE},
	{SND_PANTSDN,"Pants Of Futility",ST_PLAYER|ST_VOCAL|ST_INTFACE},
	{SND_LAYEGG,"Xeno Egg Lay",ST_MONSTER},
	{SND_GNOMESEE,"Gnome There He Is",ST_MONSTER|ST_VOCAL},
	{SND_GNOMELAUGH,"Gnome Ha Ha",ST_MONSTER|ST_VOCAL},
	{SND_GENERATE,"Generator",ST_MONSTER},
	{SND_CLONEOUCH,"Evil Clone Ouch",ST_MONSTER},
	{SND_CLONEDIE,"Evil Clone Die",ST_MONSTER},
	{SND_TURRETBZZT,"BZZT",ST_INTFACE},
	{SND_SKELALIVE,"Bonehead Revive",ST_MONSTER},
	{SND_WILLYOUCH,"Wet Willy Ouch",ST_MONSTER},
	{SND_WILLYDIE,"Wet Willy Die",ST_MONSTER},
	{SND_CRABCLICK,"Crab Puff Clack",ST_MONSTER},
	{SND_PATCHDIE,"Pumpkin Patch Die",ST_MONSTER},
	{SND_KINGTICKLE,"King Cone: Tickling",ST_VOCAL|ST_MONSTER},
	{SND_PATTYOUCH,"Crabby Patty Ouch",ST_MONSTER},
	{SND_PATTYDIE,"Crabby Patty Die",ST_MONSTER},
	{SND_BLOWBUBBLE,"Blow A Bubble",ST_EFFECT},
	{SND_PATTYSUMMON,"Crabby Patty: Get Em!",ST_VOCAL|ST_MONSTER},
	{SND_DOZERSPIN,"Dozer Saw Spin",ST_MONSTER},
	{SND_DOZEROUCH,"Dozer Ouch",ST_MONSTER},
	{SND_AMMOCRATE,"Ammo Supreme!",ST_VOCAL|ST_EFFECT},
	{SND_BUBBLEPOP,"Bubble Pop",ST_EFFECT},
	{SND_SCARIEROUCH,"Scarier Bat Ouch",ST_MONSTER},
	{SND_SCARIERDIE,"Scarier Bat Die",ST_MONSTER},
	{SND_SCARIERSPIT,"Scarier Bat Spit",ST_MONSTER},
	{SND_UBEROUCH,"Uberbat Ouch",ST_MONSTER},
	{SND_UBERDIE,"Uberbat Die",ST_MONSTER},
	{SND_UBERSPIT,"Uberbat Spit",ST_MONSTER},
	{SND_WOLFOUCH,"Werepuppy Ouch",ST_MONSTER},
	{SND_WOLFDIE,"Werepuppy Die",ST_MONSTER},
	{SND_WOLFHOWL,"Werepuppy Howl",ST_MONSTER},
	{SND_WOLFPOUNCE,"Werepuppy Pounce",ST_MONSTER},
	{SND_WOLFCHARGE,"Weremutt Charge",ST_MONSTER},
	{SND_CLAP,"Clap",ST_EFFECT},
	{SND_MORONRIGHT,"That's Right!",ST_VOCAL},
	{SND_MORONRIGHT2,"Correct!",ST_VOCAL},
	{SND_MORONWRONG,"Good Try But Wrong",ST_VOCAL},
	{SND_MORONHELLO,"Welcome To Game Show",ST_VOCAL},
	{SND_MORONWIN,"Congratulations!",ST_VOCAL},
	{SND_FREEZE,"Freeze Ray",ST_EFFECT},
	{SND_LUNASHOOT,"LunaChick Shoot",ST_PLAYER},
	{SND_LUNAOUCH,"LunaChick Ouch",ST_PLAYER},
	{SND_LUNADIE,"LunaChick Die",ST_PLAYER},
	{SND_LUNADROWN,"LunaChick Drown",ST_PLAYER},
	{SND_LUNABORED,"LunaChick Bored",ST_PLAYER|ST_VOCAL},
	{SND_LUNABRAINS,"LunaChick Whoohoo!",ST_PLAYER|ST_VOCAL},
	{SND_BUNNYTHEME,"Bunny The Slayer Theme",ST_INTFACE},
	{SND_HAPPYWEEN,"Happy Halloween!",ST_VOCAL|ST_INTFACE},
	{SND_MARKMOVIE,"Mushroom Movie Soundtrack",ST_INTFACE},
	{SND_TZONE,"Spooky Tune",ST_INTFACE},
	{SND_BOING,"Boing",ST_EFFECT},
	{SND_BOING2,"Boing 2",ST_EFFECT},
	{SND_BOING3,"Boing 3",ST_EFFECT},
	{SND_TALLY,"Win Level Tally",ST_INTFACE},
	{SND_SHOPBELL,"Shop Bell",ST_EFFECT|ST_INTFACE},
	{SND_LOCKER,"Open A Locker",ST_INTFACE},
	{SND_BUYITEM,"Buy An Item",ST_INTFACE},
	{SND_TURNEVIL,"Turn Evil",ST_INTFACE},
	{SND_TURNGOOD,"Turn Good",ST_INTFACE},
	{SND_ALLKEYCHAIN,"Bingo-o-o-o",ST_VOCAL|ST_INTFACE},
	{SND_MINIZOIDSPIT,"Microzoid Spit",ST_MONSTER},
	{SND_MINIZOIDDIE,"Microzoid Die",ST_MONSTER},
	{SND_MINIWACKOOUCH,"Little Crazy Ouch",ST_MONSTER},
	{SND_MINIWACKODIE,"Little Crazy Die",ST_MONSTER|ST_VOCAL},
	{SND_JACKATTACK,"Jack Frost Attack",ST_MONSTER},
	{SND_JACKDIE,"Jack Frost Die",ST_MONSTER},
	{SND_KOOLKATKM,"KM Kool Kat Bad",ST_PLAYER|ST_VOCAL},
	{SND_DROWNKM,"KM Drown",ST_PLAYER},
	{SND_WINKM,"KM Win Level",ST_PLAYER},
	{SND_KMOUCH,"KM Ouch",ST_PLAYER},
	{SND_KMDIE,"KM Die",ST_PLAYER},
	{SND_PEEPDIE,"Oculoid Die",ST_MONSTER},
	{SND_PEEPOUCH,"Oculoid Ouch",ST_MONSTER},
	{SND_PEEPSHOOT,"Oculoid Shoot",ST_MONSTER},
	{SND_EYEGUYOUCH,"Eye Guy Ouch",ST_MONSTER},
	{SND_EYEGUYDIE,"Eye Guy Die",ST_MONSTER},
	{SND_TOWEROUCH,"Tower Ouch",ST_MONSTER},
	{SND_TOWERDIE,"Tower Die",ST_MONSTER},
	{SND_LOOKEYOUCH,"Lookey-Loo Ouch",ST_MONSTER},
	{SND_LOOKEYDIE,"Lookey-Loo Die",ST_MONSTER},
	{SND_GOLEMPOUND,"Golem Pound",ST_MONSTER},
	{SND_GOLEMOUCH,"Golem Ouch",ST_MONSTER},
	{SND_GOLEMDIE,"Golem Die",ST_MONSTER},
	{SND_BOBBYOUCH,"Bobby Khan Ouch",ST_MONSTER},
	{SND_BOBBYDIE,"Bobby Khan Die",ST_MONSTER},
	{SND_BOBBYSLASH,"Bobby Khan Slash",ST_MONSTER},
	{SND_BOBBYSTOMP,"Bobby Khan Stomp",ST_MONSTER},
	{SND_BOBBYBLOCK,"Bobby Khan Block",ST_MONSTER},
	{SND_EYEGUYSHT,"Eye Guy Shoot",ST_MONSTER},
	{SND_OCTONSHOOT,"Octon Shoot",ST_MONSTER},
	{SND_OCTONSPLASH,"Octon Splash",ST_MONSTER},
	{SND_OCTONOUCH,"Octon Ouch",ST_MONSTER},
	{SND_OCTONDIE,"Octon Die",ST_MONSTER},
	{SND_BUSHDIE,"Crazy Bush Die",ST_MONSTER},
	{SND_BUSHOUCH,"Crazy Bush Ouch",ST_MONSTER},
	{SND_INCABEAM,"IncaBot Beam",ST_MONSTER},
	{SND_INCAGEN,"IncaBot Generate",ST_MONSTER},
	{SND_INCAHIT,"IncaBot Ouch",ST_MONSTER},
	{SND_INCADIE,"IncaBot Die",ST_MONSTER},
	{SND_INCATONGUE,"Blocko Tongue",ST_MONSTER},
	{SND_SNAILHIT,"Snail Hit",ST_MONSTER},
	{SND_SNAILDIE,"Snail Die",ST_MONSTER},
	{SND_GOATSHOOT,"Goat Shoot",ST_MONSTER},
	{SND_GOATHIT,"Goat Hit",ST_MONSTER},
	{SND_GOATDIE,"Goat Die",ST_MONSTER},
	{SND_GOATRUN,"Goat Run",ST_MONSTER},
	{SND_GOATDIZZY,"Goat Dizzy",ST_MONSTER},
	{SND_PEEPALARM,"Peeping Bomb Alarm",ST_MONSTER},
	{SND_SKELGAS,"Gangrene Gas",ST_MONSTER},
	{SND_SKELSUMMON,"Numbskull Summon",ST_MONSTER},
	{SND_LOONYOUCH,"Young Loony Ouch",ST_PLAYER},
	{SND_LOONYDIE,"Young Loony Die",ST_PLAYER},
	{SND_LOONYBORED,"Young Loony Bored",ST_PLAYER},
	{SND_VILLAGERSCRATCH,"Villager Bored",ST_MONSTER},
	{SND_VILLAGERYAWN,"Villager Yawn",ST_MONSTER},
	{SND_VILLDIE1,"Villager Die (Male)",ST_MONSTER},
	{SND_VILLDIE2,"Villager Die (Female)",ST_MONSTER},
	{SND_SWAMPOUCH,"Swampdog Ouch",ST_MONSTER},
	{SND_SWAMPDIE,"Swampdog Die",ST_MONSTER},
	{SND_SWAMPALERT,"Swampdog Alert",ST_MONSTER},
	{SND_SWAMPJAB,"Swampdog Jab",ST_MONSTER},
	{SND_SWAMPCHARGE,"Swampdog Charge",ST_MONSTER},
	{SND_SWAMPSHOOT,"Swampdog Shoot",ST_MONSTER},
	{SND_GHOSTSPIN,"Ghost Spin",ST_MONSTER},
	{SND_GHOSTPUNCH,"Ghost Punch",ST_MONSTER},
	{SND_STICKTAUNT,"H.S.Witch Taunt",ST_MONSTER},
	{SND_STICKOUCH,"H.S.Witch Ouch",ST_MONSTER},
	{SND_STICKDIE,"H.S.Witch Die",ST_MONSTER|ST_VOCAL},
	{SND_FRANKOUCH,"Frankenjulie Ouch",ST_MONSTER},
	{SND_FRANKDIE,"Frankenjulie Die",ST_MONSTER},
	{SND_FRANKHEAD,"Frankenjulie Head Shot",ST_MONSTER},
	{SND_FRANKARMS,"Frankenjulie Arm Shot",ST_MONSTER},
	{SND_FRANKMISSILE,"Frankenjulie Missile",ST_MONSTER},
	{SND_POLTEROUCH,"Polterguy Ouch",ST_MONSTER},
	{SND_POLTERDIE,"Polterguy Die",ST_MONSTER},
	{SND_POLTERSNEEZE,"Polterguy Sneeze",ST_MONSTER},
	{SND_POLTERGET,"Polterguy Junk Shot",ST_MONSTER},
	{SND_PUMPKINOUCH,"Ice Pumpkin Die",ST_MONSTER},
	{SND_MUMMYSHOOT,"Mummy Shoot",ST_MONSTER},
	{SND_MUMMYSUMMON,"Mummy Summon",ST_MONSTER},
	{SND_TREEOUCH,"Terror Tree Ouch",ST_MONSTER},
	{SND_TREEDIE,"Terror Tree Die",ST_MONSTER},
	{SND_TREEFROG,"Terror Tree Summon",ST_MONSTER},
	{SND_ELDEROUCH,"Elder Ouch",ST_MONSTER},
	{SND_ELDERDIE,"Elder Die",ST_MONSTER},
	{SND_ELDERSHOOT,"Elder Shoot",ST_MONSTER},
	{SND_ELDERCHARGE,"Elder Charge",ST_MONSTER},
	{SND_ELDERSUMMON,"Elder Summon",ST_MONSTER},
	{SND_BONKOUCH,"Bonkula Ouch",ST_MONSTER},
	{SND_BONKDIE,"Bonkula Die",ST_MONSTER},
	{SND_BONKSWING,"Bonkula Swing",ST_MONSTER},
	{SND_BONKHIT,"Bonkula Hit",ST_MONSTER},
	{SND_EVILIZER,"Evilizer Ambience",ST_MONSTER},
	{SND_WATERSPURT,"Water Spurt",ST_MONSTER},
	{SND_WATERSPLASH,"Water Splash",ST_MONSTER},
	{SND_LARRYOUCH,"Larry Ouch",ST_MONSTER},
	{SND_LARRYDARTS,"Larry Darts",ST_MONSTER},
	{SND_LARRYPOUNCE,"Larry Pounce",ST_MONSTER},
	{SND_LARRYHOWL,"Larry Howl",ST_MONSTER},
	{SND_FROGOUCH,"Frog Ouch",ST_MONSTER},
	{SND_FROGDIE,"Frog Die",ST_MONSTER},
	{SND_FROGLICK,"Frog Lick",ST_MONSTER},
	{SND_COWMOO,"Cow MOO!",ST_MONSTER},
	{SND_COWOUCH,"Cow Ouch",ST_MONSTER},
	{SND_COWDIE,"Cow Die",ST_MONSTER},
	{SND_CHEER,"Combo Cheer",ST_EFFECT},
	{SND_DEATHRAY,"Death Ray Shoot",ST_EFFECT},
	{SND_INFERNAL,"Infernal Blast",ST_EFFECT},
	{SND_SEKRIT,"Secret Level Enter",ST_EFFECT},
	{SND_BEAM2,"Mini Beam Fire",ST_EFFECT},
	{SND_YERFOUCH,"Yerfdog Ouch",ST_MONSTER},
	{SND_YERFLEAVE,"Yerfdog 'Die'",ST_MONSTER},
	{SND_PUMPKOUCH,"Pumpkin Ouch",ST_MONSTER},
	{SND_PUMPKDIE,"Pumpkin Die",ST_MONSTER},
	{SND_SZOMBIEDIE,"Super Zombie Die",ST_MONSTER},
	{SND_SZOMBIEOUCH,"Super Zombie Ouch",ST_MONSTER},
	{SND_SZOMBIELEAP,"Super Zombie Leap",ST_MONSTER},
};

byte soundAvailable=0;
static int numSounds,numCustom;
static byte *customSound[MAX_CUSTOM_SOUNDS];
static long customLength[MAX_CUSTOM_SOUNDS];

void SoundSystemExists(void)
{
	soundAvailable=1;
}

void InitSound(void)
{
	JamulSoundPurge();
	numSounds=NUM_ORIG_SOUNDS;
	memset(customSound,0,sizeof(byte *)*MAX_CUSTOM_SOUNDS);
	numCustom=0;
}

void ExitSound(void)
{
	ClearCustomSounds();
	// don't need to do nothing, it's handled by jamulsoundexit
}

void ClearCustomSounds(void)
{
	int i;

	for(i=0;i<MAX_CUSTOM_SOUNDS;i++)
	{
		soundInfo[i+CUSTOM_SND_START].theme=0;
		if(customSound[i])
			free(customSound[i]);
		customSound[i]=NULL;
	}
	numCustom=0;
}

int GlobalFlags()
{
	int result = 0;
	if (profile.progress.purchase[modeShopNum[MODE_REVERSE]]&SIF_ACTIVE)
		result |= SND_BACKWARDS;
	if (profile.progress.purchase[modeShopNum[MODE_MANIC]]&SIF_ACTIVE)
		result |= SND_DOUBLESPEED;
	return result;
}

void MakeSound(int snd,int x,int y,int flags,int priority)
{
	long pan,vol;
	int cx,cy;

	if(snd==0)
		return;

	if(!soundAvailable)
		return;
	if(profile.sound==0)
		return;

	GetCamera(&cx,&cy);
	x>>=FIXSHIFT;
	y>>=FIXSHIFT;
	pan=(x-cx)*128/320;
	vol=-(abs(y-cy)*128/240);
	if(pan<-127)
	{
		vol+=(pan+127);
		pan=-127;
	}
	if(pan>127)
	{
		vol-=(pan-127);
		pan=127;
	}
	if(vol<-255)
		return;
	GoPlaySound(snd,pan,vol,flags|GlobalFlags(),priority);
}

void MakeNormalSound(int snd)
{
	if(!soundAvailable)
		return;

	if(profile.sound==0)
		return;

	GoPlaySound(snd,0,0,SND_MAXPRIORITY|SND_CUTOFF|SND_ONE|GlobalFlags(),MAX_SNDPRIORITY);
}

void MakeCustomSound(int snd,int x,int y,int flags,int priority)
{
	long pan,vol;
	int cx,cy;

	if(snd==0)
		return;

	if(!soundAvailable)
		return;
	if(profile.sound==0)
		return;

	GetCamera(&cx,&cy);
	x>>=FIXSHIFT;
	y>>=FIXSHIFT;
	pan=(x-cx)*128/320;
	vol=-(abs(y-cy)*128/240);
	if(pan<-127)
	{
		vol+=(pan+127);
		pan=-127;
	}
	if(pan>127)
	{
		vol-=(pan-127);
		pan=127;
	}
	if(vol<-255)
		return;
	GoPlaySound(soundInfo[snd].num,pan,vol,flags|GlobalFlags(),priority);
}

void MakeNormalCustomSound(int snd)
{
	if(snd==0)
		return;

	if(!soundAvailable)
		return;

	if(profile.sound==0)
		return;

	GoPlaySound(soundInfo[snd].num,0,0,SND_MAXPRIORITY|SND_CUTOFF|SND_ONE|GlobalFlags(),MAX_SNDPRIORITY);
}

soundDesc_t *GetSoundInfo(int snd)
{
	return &soundInfo[snd];
}

int GetNumSounds(void)
{
	return MAX_SOUNDS;
}

int GetNumCustomSounds(void)
{
	return numCustom;
}

byte *GetCustomSound(int n)
{
	return customSound[n];
}

long GetCustomLength(int n)
{
	return customLength[n];
}

byte AddCustomSound(const char *fname)
{
	FILE *f;

	if(numCustom==MAX_CUSTOM_SOUNDS)
		return 0;

	f=fopen(fname,"rb");
	if(!f)
		return 0;
	fseek(f,0,SEEK_END);
	customLength[numCustom]=ftell(f);
	if(customLength[numCustom]<=0)
	{
		fclose(f);
		return 0;
	}
	customSound[numCustom]=(byte *)malloc(customLength[numCustom]);
	if(!customSound[numCustom])
	{
		fclose(f);
		return 0;
	}
	rewind(f);
	fread(customSound[numCustom],sizeof(byte),customLength[numCustom],f);
	fclose(f);

	strcpy(soundInfo[CUSTOM_SND_START+numCustom].name,"New Sound");
	soundInfo[CUSTOM_SND_START+numCustom].num=CUSTOM_SND_START+numCustom;
	soundInfo[CUSTOM_SND_START+numCustom].theme=ST_CUSTOM;

	numCustom++;
	return 1;
}

soundDesc_t *AddCustomSound(byte *data, size_t length)
{
	if(numCustom==MAX_CUSTOM_SOUNDS)
		return nullptr;

	customSound[numCustom] = data;
	customLength[numCustom] = length;

	strcpy(soundInfo[CUSTOM_SND_START+numCustom].name,"New Sound");
	soundInfo[CUSTOM_SND_START+numCustom].num=CUSTOM_SND_START+numCustom;
	soundInfo[CUSTOM_SND_START+numCustom].theme=ST_CUSTOM;
	return &soundInfo[CUSTOM_SND_START + (numCustom++)];
}

byte ReplaceCustomSound(int n,const char *fname)
{
	FILE *f;

	if(customSound[n])
		free(customSound[n]);

	f=fopen(fname,"rb");
	if(!f)
		return 0;
	fseek(f,0,SEEK_END);
	customLength[n]=ftell(f);
	if(customLength[n]<=0)
	{
		fclose(f);
		return 0;
	}
	customSound[n]=(byte *)malloc(customLength[n]);
	if(!customSound[n])
	{
		fclose(f);
		return 0;
	}
	rewind(f);
	fread(customSound[n],sizeof(byte),customLength[n],f);
	fclose(f);

	return 1;
}

void DeleteCustomSound(int n)
{
	int i;

	free(customSound[n]);

	for(i=n;i<MAX_CUSTOM_SOUNDS-1;i++)
	{
		customSound[i]=customSound[i+1];
		customLength[i]=customLength[i+1];
		soundInfo[CUSTOM_SND_START+i]=soundInfo[CUSTOM_SND_START+i+1];
		soundInfo[CUSTOM_SND_START+i].num=CUSTOM_SND_START+i;
	}
	customSound[MAX_CUSTOM_SOUNDS-1]=NULL;
	soundInfo[CUSTOM_SND_START+MAX_CUSTOM_SOUNDS-1].theme=0;
	numCustom--;

	JamulSoundPurge();
	RepairCustomSounds(CUSTOM_SND_START+n);	// all references to sound N become null references, and all
											// references above N move down one
}

void SaveCustomSounds(FILE *f)
{
	int i;

	fwrite(&numCustom,1,sizeof(int),f);
	for(i=0;i<numCustom;i++)
	{
		fwrite(&soundInfo[CUSTOM_SND_START+i],1,sizeof(soundDesc_t),f);	// write out the descriptor
		fwrite(&customLength[i],sizeof(long),1,f);	// write out the data length
		fwrite(customSound[i],sizeof(byte),customLength[i],f);	// write out the data
	}
}

void LoadCustomSounds(FILE *f)
{
	int i;

	ClearCustomSounds();

	fread(&numCustom,1,sizeof(int),f);
	for(i=0;i<numCustom;i++)
	{
		fread(&soundInfo[CUSTOM_SND_START+i],1,sizeof(soundDesc_t),f);
		fread(&customLength[i],sizeof(long),1,f);
		customSound[i]=(byte *)malloc(customLength[i]);
		fread(customSound[i],sizeof(byte),customLength[i],f);
	}
}

int AppendCustomSounds(FILE *f)
{
	int i,more,start;

	fread(&more,1,sizeof(int),f);

	if(numCustom+more>MAX_CUSTOM_SOUNDS)
		return -1;

	start=numCustom;
	numCustom+=more;
	for(i=start;i<numCustom;i++)
	{
		fread(&soundInfo[CUSTOM_SND_START+i],1,sizeof(soundDesc_t),f);
		soundInfo[CUSTOM_SND_START+i].num=CUSTOM_SND_START+i;
		fread(&customLength[i],sizeof(long),1,f);
		customSound[i]=(byte *)malloc(customLength[i]);
		fread(customSound[i],sizeof(byte),customLength[i],f);
	}

	return (start);
}

int GetCustomSoundByName(const char *name)
{
	int i;

	for(i=0;i<numCustom;i++)
	{
		if(!strcmp(soundInfo[CUSTOM_SND_START+i].name,name))
			return i+CUSTOM_SND_START;
	}

	return -1;
}

void MakeSpaceSound(int snd,int priority)
{
	if(!soundAvailable)
		return;
	if(profile.sound==0)
		return;

	GoPlaySound(snd,0,0,SND_CUTOFF|GlobalFlags(),priority);
}

SDL_RWops* SoundLoadOverride(int num)
{
	if (num < CUSTOM_SND_START || num > CUSTOM_SND_START+GetNumCustomSounds())
		return nullptr;

	byte* buf = GetCustomSound(num - CUSTOM_SND_START);
	if (!buf)
		return nullptr;

	return SDL_RWFromConstMem(buf, GetCustomLength(num - CUSTOM_SND_START));
}

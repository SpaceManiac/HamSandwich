#include "randomizer.h"
#include "mgldraw.h"
#include "control.h"
#include "display.h"
#include "player.h"
#include "title.h"
#include "plasma.h"
#include "appdata.h"
#include <filesystem>
#include "map.h"
#include <set>


static byte cursor;
static byte oldc;
static dword oldBtn;
static byte optMode;
static std::string seed;
std::set<int> ownedItems;

#define MAX_SEED_LENGTH 32;

location locations[105] = {
{false, 0, 194, 5, 24, 25, "Swamp Mud Path", []() {return ownedItems.count(VAR_BOOTS);}},
{false, 0, 187, 112, 11, 12, "Bog Beast Home", []() {return true;}},
{false, 0, 2, 46, 69, 70, "Rocky Cliffs below Upper Caverns", []() {return HaveAnyBigGem();}},
{false, 0, 131, 26, 10, 9, "Sapling Shrine", []() {return ownedItems.count(VAR_BOOTS);}},
{false, 0, 83, 145, 33, 34, "Terror Glade", []() {return true;}},
{false, 0, 1, 73, 47, 48, "Rocky Cliffs Vine", []() {return ownedItems.count(VAR_FERTILIZER);}},
{false, 0, 30, 39, 73, 74, "Rocky Cliffs Grand Pharoh", []() {return HaveAnyBigGem();}},
{false, 0, 69, 59, 66, 67, "Rocky Cliffs Rock Corner", []() {return HaveAnyBigGem() && ownedItems.count(VAR_WEAPON);}},
{false, 0, 115, 70, 18, 19, "Mushroom outside town", []() {return true;}},
{false, 0, 159, 85, 20, 21, "mushroom east", []() {return true;}},
{false, 0, 117, 33, 22, 23, "Top left mushroom spot", []() {return true;}},
{false, 0, 168, 98, 28, 29, "mushroom south east", []() {return true;}},
{false, 0, 197, 161, 56, 57, "East Woods", []() {return true;}},
{false, 0, 16, 61, 49, 50, "Rocky Cliffs Ledge", []() {return HaveAnyBigGem();}},
{false, 0, 28, 10, 63, 65, "Rocky Cliffs Peak", []() {return HaveAnyBigGem();}},
{false, 0, 134, 182, 97, 98, "Cat Tree", []() {return true;}},
{false, 2, 27, 16, 1, 2, "Backroom", []() {return true;}},
{false, 2, 22, 7, 3, 4, "Bedroom", []() {return true;}},
{false, 3, 10, 8, 1, 3, "Barrel Maze", []() {return true;}},
{false, 4, 19, 5, 1, 2, "Top Door", []() {return ownedItems.count(VAR_SKULLKEY);}},
{false, 4, 39, 38, 3, 4, "Post Room", []() {return true;}},
{false, 4, 33, 06, 9, 10, "Window Drip", []() {return true;}},
{false, 4, 3, 33, 7, 8, "Green room", []() {return true;}},
{false, 4, 25, 48, 5, 11, "Arena", []() {return true;}},
{false, 4, 2, 58, 17, 18, "Kill Wall", []() {return true;}},
{false, 5, 53, 41, 0, 1, "Swampdog Door", []() {return ownedItems.count(VAR_PUMPKINKEY);}},
{false, 5, 57, 37, 13, 14, "Scribble Wall", []() {return true;}},
{false, 5, 26, 25, 4, 5, "Tiny Passage", []() {return true;}},
{false, 5, 50, 26, 7, 8, "fire frogs", []() {return true;}},
{false, 5, 17, 64, 9, 10, "Torch Island", []() {return ownedItems.count(VAR_BOOTS);}},
{false, 5, 44, 49, 11, 12, "Small Room", []() {return true;}},
{false, 6, 32, 45, 2, 3, "Scratch Wall", []() {return ownedItems.count(VAR_BOOTS);}},
{false, 6, 44, 37, 15, 17, "Bat Mound", []() {return ownedItems.count(VAR_BOOTS) && ownedItems.count(VAR_BATKEY);}},
{false, 6, 3, 7, 23, 24, "Stair room", []() {return ownedItems.count(VAR_BOOTS);}},
{false, 6, 43, 58, 18, 19, "Rock Prison", []() {return ownedItems.count(VAR_BOOTS) && ownedItems.count(VAR_WEAPON);}},
{false, 7, 11, 16, 1, 2, "Tiny Cabin", []() {return ownedItems.count(VAR_SKULLKEY);}},
{false, 8, 19, 15, 3, 4, "Bedside ", []() {return true;}},
{false, 10, 2, 44, 2, 3, "Pumpkin Door", []() {return HaveLightSource() && ownedItems.count(VAR_PUMPKINKEY);}},
{false, 10, 5, 36, 4, 5, "Maze", []() {return HaveLightSource();}},
{false, 11, 51, 82, 11, 12, "Big Closed Room", []() {return HaveLightSource() && ownedItems.count(VAR_BOOTS);}},
{false, 12, 49, 73, 15, 16, "Spike Vine", []() {return HaveLightSource() && ownedItems.count(VAR_FERTILIZER);}},
{false, 12, 7, 97, 0, 1, "Boulders", []() {return HaveLightSource();}},
{false, 13, 13, 6, 1, 2, "Barrel Mess", []() {return true;}},
{false, 14, 40, 13, 6, 7, "Lightning Rod Secret", []() {return HaveLightSource() && HaveAllOrbs();}},
{false, 14, 73, 20, 8, 9, "Bat Door", []() {return HaveLightSource() && ownedItems.count(VAR_BATKEY);}},
{false, 15, 58, 77, 12, 13, "SE corner", []() {return HaveLightSource() && HaveAllOrbs();}},
{false, 15, 51, 71, 10, 11, "Rhombus", []() {return HaveLightSource() && HaveAllOrbs();}},
{false, 16, 15, 6, 3, 4, "Boss Reward", []() {return HaveLightSource() && HaveAllOrbs();}},
{false, 17, 59, 29, 3, 4, "Barracks", []() {return ownedItems.count(VAR_POTION) && ownedItems.count(VAR_BATKEY);}},
{false, 18, 8, 5, 6, 7, "Top Left", []() {return ownedItems.count(VAR_POTION);}},
{false, 20, 13, 37, 1, 2, "Boss Reward", []() {return ownedItems.count(VAR_POTION);}},
{false, 21, 20, 13, 1, 2, "DoorDoorDoorDoorDoorDoor", []() {return ownedItems.count(VAR_POTION) && HaveLightSource() && ownedItems.count(VAR_BATKEY) && 
																	ownedItems.count(VAR_SKULLKEY) && ownedItems.count(VAR_PUMPKINKEY);}},
{false, 22, 39, 2, 1, 2, "Shaft", []() {return HaveLightSource() && HaveAnyBigGem();}},
{false, 23, 12, 3, 2, 3, "Bombulus", []() {return HaveAnyBigGem();}},
{false, 24, 50, 44, 2, 3, "Lockpick", []() {return HaveLightSource() && HaveAnyBigGem();}},
{false, 25, 61, 2, 5, 6, "Happy Stick Hidden", []() {return ownedItems.count(VAR_TALISMAN);}},
{false, 25, 35, 34, 0, 1, "Happy Stick Reward", []() {return ownedItems.count(VAR_TALISMAN);}},
{false, 26, 2, 5, 1, 2, "Wolf Top Left", []() {return HaveLightSource() && ownedItems.count(VAR_SILVERSLING);}},
{false, 26, 6, 14, 6, 7, "Pumpkin Door", []() {return HaveLightSource() && ownedItems.count(VAR_SILVERSLING) && ownedItems.count(VAR_PUMPKINKEY);}},
{false, 26, 40, 88, 3, 4, "Grow Room", []() {return HaveLightSource() && ownedItems.count(VAR_SILVERSLING);}},
{false, 28, 88, 38, 3, 4, "The three hombres", []() {return HaveLightSource() && HaveAnyBigGem();}},
{false, 29, 35, 39, 1, 2, "Left Vine", []() {return HaveLightSource() && HaveAnyBigGem() && ownedItems.count(VAR_FERTILIZER);}},
{false, 29, 103, 22, 3, 4, "Right Vine", []() {return HaveLightSource() && HaveAnyBigGem() && ownedItems.count(VAR_FERTILIZER);}},
{false, 30, 117, 76, 3, 4, "Pharoh bat Room", []() {return HaveLightSource() && HaveAnyBigGem() && ownedItems.count(VAR_BATKEY);}},
{false, 30, 135, 6, 10, 11, "2 blue Pharos", []() {return HaveLightSource() && HaveAnyBigGem();}},
{false, 30, 84, 75, 5, 6, "GARGOYLE ROOM", []() {return HaveLightSource() && HaveAnyBigGem();}},
{false, 31, 63,12 , 11, 12, "Vampire Guard", []() {return HaveLightSource() && HaveAnyBigGem();}},
{false, 31, 27, 21, 13, 14, "maze top left", []() {return HaveLightSource() && HaveAnyBigGem();}},
{false, 31, 105, 16, 15, 16, "Top Right Gauntlet", []() {return HaveLightSource() && HaveAnyBigGem();}},
{false, 31, 9, 92, 22, 23, "Bat Closet", []() {return HaveLightSource() && HaveAnyBigGem();}},
{false, 32, 22, 37, 6, 7, "Candle Room", []() {return HaveLightSource() && HaveAnyBigGem() && ownedItems.count(VAR_SKULLKEY);}},
{false, 32, 89, 22, 8, 9, "Top Right Top", []() {return HaveLightSource() && HaveAnyBigGem() && ownedItems.count(VAR_SKULLKEY);}},
{false, 32, 89, 89, 2, 3, "Bottom Right Middle", []() {return HaveLightSource() && HaveAnyBigGem() && ownedItems.count(VAR_SKULLKEY);}},
{false, 32, 52, 106, 4, 5, "Bat room", []() {return HaveLightSource() && HaveAnyBigGem() && ownedItems.count(VAR_SKULLKEY);}},
{false, 33, 18, 6, 1, 2, "Gold Skull", []() {return true;}},
{false, 34, 63, 64, 13, 14, "Middle", []() {return HaveLightSource() && HaveAnyBigGem() && ownedItems.count(VAR_SKULLKEY) && ownedItems.count(VAR_BATKEY);}},
{false, 34, 94, 33, 17, 18, "Behind the Pews", []() {return HaveLightSource() && HaveAnyBigGem() && ownedItems.count(VAR_SKULLKEY) && ownedItems.count(VAR_BATKEY);}},
{false, 34, 101, 110, 5, 6, "AMBUSH!", []() {return HaveLightSource() && HaveAnyBigGem() && ownedItems.count(VAR_SKULLKEY) && ownedItems.count(VAR_BATKEY);}},
{false, 34, 103, 19, 10, 9, "Halloween", []() {return HaveLightSource() && HaveAnyBigGem() && ownedItems.count(VAR_SKULLKEY) && ownedItems.count(VAR_BATKEY);}},
{false, 34, 18, 11, 23, 24, "So many bats", []() {return HaveLightSource() && HaveAnyBigGem() && ownedItems.count(VAR_SKULLKEY) && ownedItems.count(VAR_BATKEY);}},
{false, 35, 94, 117, 8, 9, "Right Path", []() {return HaveLightSource() && HaveAnyBigGem() && ownedItems.count(VAR_SKULLKEY) && ownedItems.count(VAR_BATKEY) && ownedItems.count(VAR_PUMPKINKEY);}},
{false, 35, 34, 117, 6, 7, "Left Path", []() {return HaveLightSource() && HaveAnyBigGem() && ownedItems.count(VAR_SKULLKEY) && ownedItems.count(VAR_BATKEY) && ownedItems.count(VAR_PUMPKINKEY);}},
{false, 35, 104, 68, 3, 4, "Ballroom Right", []() {return HaveLightSource() && HaveAnyBigGem() && ownedItems.count(VAR_SKULLKEY) && ownedItems.count(VAR_BATKEY) && ownedItems.count(VAR_PUMPKINKEY);}},
{false, 35, 111, 102, 10, 11, "Right Secret Wall", []() {return HaveLightSource() && HaveAnyBigGem() && ownedItems.count(VAR_SKULLKEY) && ownedItems.count(VAR_BATKEY) && ownedItems.count(VAR_PUMPKINKEY);}},
{false, 35, 23, 68, 1, 2, "Ballroom Left", []() {return HaveLightSource() && HaveAnyBigGem() && ownedItems.count(VAR_SKULLKEY) && ownedItems.count(VAR_BATKEY) && ownedItems.count(VAR_PUMPKINKEY);}},
{false, 37, 13, 18, 3, 4, "Boss", []() {return HaveLightSource() && HaveAnyBigGem() && HaveAllBats() && HaveAnySpecialWeapon();}},
{false, 38, 13, 18, 3, 4, "Boss", []() {return HaveLightSource() && HaveAnyBigGem() && HaveAllBats();}},
{false, 39, 13, 18, 3, 4, "Boss", []() {return HaveLightSource() && HaveAnyBigGem() && HaveAllBats();}},
{false, 40, 13, 18, 3, 4, "Boss", []() {return HaveLightSource() && HaveAnyBigGem() && HaveAllBats();}},
{false, 42, 16, 28, 3, 4, "Bonkula", []() {return HaveLightSource() && HaveAnyBigGem() && ownedItems.count(VAR_SKULLKEY) && ownedItems.count(VAR_BATKEY) && ownedItems.count(VAR_PUMPKINKEY) && HaveAllVamps();}},
{false, 43, 5, 22, 1, 2, "Bat Door", []() {return ownedItems.count(VAR_BATKEY);}},
{false, 43, 18, 22, 4, 5, "Pebbles", []() {return true;}},
{false, 45, 55, 49, 0, 1, "Entrance", []() {return ownedItems.count(VAR_BOOTS);}},
{false, 45, 55, 36, 5, 6, "End", []() {return HaveLightSource() && ownedItems.count(VAR_FERTILIZER);}},
{true, 0, 0, 0, 0, 0, "Ghostbusting" , []() {return HaveAnyBigGem() && ownedItems.count(VAR_DAISY);}},
{true, 1, 0, 0, 0, 0, "Hairy Larry" , []() {return HaveLightSource() && ownedItems.count(VAR_SILVERSLING);}},
{true, 2, 0, 0, 0, 0, "Scaredy Cat" , []() {return ownedItems.count(VAR_CAT);}},
{true, 3, 0, 0, 0, 0, "Silver Bullet" , []() {return ownedItems.count(VAR_SILVER) && CanCleanseCrypts();}},
{true, 4, 0, 0, 0, 0, "Smashing Pumpkins" , []() {return CanCleanseCrypts();}},
{true, 5, 0, 0, 0, 0, "Sticky Shoes" , []() {return true;}},
{true, 6, 0, 0, 0, 0, "The Collection" , []() {return ownedItems.count(VAR_SILVERSLING) && ownedItems.count(VAR_POTION) && HaveAnyBigGem() && HaveLightSource();}},
{true, 7, 0, 0, 0, 0, "The Rescue" , []() {return HaveLightSource();}},
{true, 8, 0, 0, 0, 0, "Tree Trimming" , []() {return true;}},
{true, 9, 0, 0, 0, 0, "Witch Mushrooms" , []() {return HaveAllMushrooms();}},
{true, 10, 0, 0, 0, 0, "Zombie Stomp" , []() {return CanCleanseCrypts();}}
	
};

void InitRandomizerMenu(void)
{
	oldc = 255;
	cursor = 0;
	InitPlasma(7);
	seed = "";
}

void ExitRandomizerMenu(void)
{
	ExitPlasma();
}

byte UpdateRandomizerMenu(int *lastTime, MGLDraw *mgl)
{
	char c;
	byte c2;
	dword btn, j;
	int i;

	if (*lastTime > TIME_PER_FRAME * 30)
		*lastTime = TIME_PER_FRAME * 30;
	while (*lastTime >= TIME_PER_FRAME)
	{
		switch (optMode)
		{
		case 0: // just going through options
			c = mgl->LastKeyPressed();
			c2 = GetControls() | GetArrows();

			if (c == 27)
			{
				return 1;
			}

			if ((c2 & CONTROL_UP) && (!(oldc & CONTROL_UP)))
			{
				cursor--;
				if (cursor > 1)
					cursor = 1;
				MakeNormalSound(SND_MENUCLICK);
			}
			if ((c2 & CONTROL_DN) && (!(oldc & CONTROL_DN)))
			{
				cursor++;
				if (cursor > 1)
					cursor = 0;
				MakeNormalSound(SND_MENUCLICK);
			}
			if ((c2 & (CONTROL_B1 | CONTROL_B2 | CONTROL_B3)) && (!(oldc & (CONTROL_B1 | CONTROL_B2 | CONTROL_B3))))
			{
				switch (cursor)
				{
				case 0:
					MakeNormalSound(SND_MENUSELECT);
					LastScanCode();
					optMode=1;
					break;
				case 1:
					MakeNormalSound(SND_MENUSELECT);
					return 1;
					break;
				}
			}
			if(c == SDLK_j){
				AssumedFill();
			}
			break;
		case 1: // Typing in something
			c = mgl->LastKeyPressed();
			if(c==SDLK_ESCAPE)	// ESC key
			{
				optMode=0;
				c2=255;
				oldc=255;
				mgl->LastKeyPressed();
				MakeNormalSound(SND_MENUCANCEL);
				return 0;
			}
			else if (c==SDLK_BACKSPACE && seed.length() > 0)
			{
				seed.pop_back();
				MakeNormalSound(SND_MENUSELECT);
			}
			else if(c!=0)
			{
				if (seed.length() >= 32){
					MakeNormalSound(SND_MENUCANCEL);
				}
				else
				{
					if((c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9'))
					{
						seed += c;
						MakeNormalSound(SND_MENUSELECT);
					}
				}
			}
			c2=255;
			break;
		}

		oldc = c2;

		UpdatePlasma();

		*lastTime -= TIME_PER_FRAME;
	}

	return 0;
}

void RenderRandomizerMenu(MGLDraw *mgl)
{

	int wid;
	byte *pos;
	int i;

	wid = mgl->GetWidth();
	pos = mgl->GetScreen() + 40 * wid;
	for (i = 40; i < 480 - 40; i++)
	{
		memset(pos, 7 * 32 + 2, 640);
		pos += wid;
	}

	RenderPlasma(mgl, 1);
	RenderPlasma(mgl, 0);

	CenterPrintGlow(320, 2, "Game Randomizer", 0, 2);

	PrintColor(240, 80, "Seed", 7, -10, 0);
	PrintColor(300,80, seed.c_str() ,7,-10,0);
	if (cursor == 0)
	{
		PrintColor(239, 79, "Seed", 0, 0, 0);
		PrintColor(299 ,79, seed.c_str() ,0,0,0);
	}

	PrintColor(240, 170, "Exit To Main Menu", 7, -10, 0);
	if (cursor == 1)
		PrintColor(239, 169, "Exit To Main Menu", 0, 0, 0);
}

//----------------

TASK(void)
RandomizerMenu(MGLDraw *mgl)
{
	byte done = 0;
	int lastTime;

	InitRandomizerMenu();
	lastTime = 1;

	while (!done)
	{
		lastTime += TimeLength();
		StartClock();
		done = UpdateRandomizerMenu(&lastTime, mgl);
		RenderRandomizerMenu(mgl);
		AWAIT mgl->Flip();
		EndClock();

		if (!mgl->Process())
			done = 1;
	}

	ExitRandomizerMenu();
}

TASK(void)
AssumedFill(){

	location loc;
	loc.requirements = []() {return HaveLightSource() && ownedItems.count(VAR_CAT);};

	ownedItems = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,
	90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,
	160,161,162,164,165,166,167,189,218,220};
	//const auto copyOptions = std::filesystem::copy_options::update_existing;
	if (std::filesystem::exists("rando.llw")){
		std::filesystem::remove("rando.llw");
	}
	std::filesystem::copy("loony.llw", "rando.llw");


	world_t world;
	LoadWorld(&world,"rando.llw");

	
	//fix that awful, singular, flipped special
	//Castle Vampy III "halloween" specials
	special_t tempSpecial = world.map[34]->special[9];
	world.map[34]->special[9].x = world.map[34]->special[10].x;
	world.map[34]->special[9].y = world.map[34]->special[10].y;
	world.map[34]->special[10].x = tempSpecial.x;
	world.map[34]->special[10].y = tempSpecial.y;

	//change cattree special
	world.map[0]->special[97].trigger = TRG_GETITEM;

	SaveWorld(&world, "rando.llw");

	/*for (int i = 0; i < 94; i++)
	{
		int s1 = doublecheck[i][0];
		int s2 = doublecheck[i][1];
		int x = doublecheck[i][2];
		int y = doublecheck[i][3];
		int mapnum = doublecheck[i][4];
		if (!(world.map[mapnum]->special[s1].x == x && world.map[mapnum]->special[s1].y == y)){
			printf("issue with map %d, %d isn't right x, y\n", mapnum, s1);
		}
		if (!(world.map[mapnum]->special[s2].x + 1 == x && world.map[mapnum]->special[s1].y == y)){
			printf("issue with map %d, s2 %d isn't right x, y\n", mapnum, s2);
		}
		if (!(world.map[mapnum]->special[s1].trigger==TRG_GETITEM)){
			printf("issue with map %d, s1 %d isn't right trigger\n", mapnum, s1);
		}
		if (!(world.map[mapnum]->special[s2].trigger==TRG_VARON)){
			printf("issue with map %d, s2 %d isn't right trigger\n", mapnum, s2);
		}
	}*/
	
/*
	for(int i = 0; i < world.numMaps; i++)
	{
		if (world.map[i] == NULL)
		{
			continue;
		}
		Map m = world.map[i];
		printf("%s\n", m.name);
		int specialNum = 0;
		for (special_t s : m.special)
		{
			if (s.trigger == TRG_GETITEM){
				printf("\t%d, %X, %d, %d, %d\n", specialNum, s.effect, s.x, s.y, s.value);
				
			}
			specialNum++;
		}
	}*/

	//world.map[i]->special[y]

	//prefill section, keys to bosses

	
	//pick a random item
	//pick a random location

	//if its a quest, set entry in quest reward to item id
	//if its a physical spot, edit specials to use new item id

	//go until out of items

	//verify beatable

}

bool HaveLightSource(){
	return (ownedItems.count(VAR_LANTERN) || ownedItems.count(VAR_STICK) && ownedItems.count(VAR_BOOTS) || ownedItems.count(VAR_TORCH));
}

bool HaveAnyBigGem(){
	return (ownedItems.count(VAR_GEM)||ownedItems.count(VAR_GEM+1)||ownedItems.count(VAR_GEM+2)||
			ownedItems.count(VAR_GEM+3)||ownedItems.count(VAR_GEM+4)||ownedItems.count(VAR_GEM+5));
}

bool HaveAllOrbs(){
	return (ownedItems.count(VAR_MYSORB) && ownedItems.count(VAR_MYSORB+1) && ownedItems.count(VAR_MYSORB+2) && ownedItems.count(VAR_MYSORB+3));
}

bool HaveAllBats(){
	return (ownedItems.count(VAR_BATSTATUE) && ownedItems.count(VAR_BATSTATUE+1) && ownedItems.count(VAR_BATSTATUE+2) && ownedItems.count(VAR_BATSTATUE+3));
}

bool HaveAllVamps(){
	return (ownedItems.count(VAR_BATSTATUE) && ownedItems.count(VAR_BATSTATUE+1) && ownedItems.count(VAR_BATSTATUE+2) && ownedItems.count(VAR_BATSTATUE+3) &&
		ownedItems.count(VAR_BATSTATUE+4) && ownedItems.count(VAR_BATSTATUE+5) && ownedItems.count(VAR_BATSTATUE+6) && ownedItems.count(VAR_BATSTATUE+7));
}

bool HaveAnySpecialWeapon(){
	return (ownedItems.count(VAR_WEAPON) || ownedItems.count(VAR_WEAPON+1) || ownedItems.count(VAR_WEAPON+2) || ownedItems.count(VAR_WEAPON+4) ||
		ownedItems.count(VAR_WEAPON+5) || ownedItems.count(VAR_WEAPON+6));
}

bool HaveAllMushrooms(){
	return (ownedItems.count(VAR_MUSHROOM) && ownedItems.count(VAR_MUSHROOM+1) && ownedItems.count(VAR_MUSHROOM+2) && ownedItems.count(VAR_MUSHROOM+3) &&
		ownedItems.count(VAR_MUSHROOM+4) && ownedItems.count(VAR_MUSHROOM+5) && ownedItems.count(VAR_MUSHROOM+6) && ownedItems.count(VAR_MUSHROOM+7) &&
		 ownedItems.count(VAR_MUSHROOM+8) &&  ownedItems.count(VAR_MUSHROOM+9));
}

bool CanCleanseCrypts(){
	return (HaveLightSource && ownedItems.count(VAR_BOOTS));
}

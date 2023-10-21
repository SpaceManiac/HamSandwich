#include "worldselect.h"
#include <algorithm>
#include <memory>
#include "world.h"
#include "winpch.h"
#include "clock.h"
#include "game.h"
#include "music.h"
#include "dialogbits.h"
#include "shop.h"
#include "hiscore.h"
#include "appdata.h"
#include "erase_if.h"
#include "vanilla_extract.h"
#include "steam.h"
#include "leaderboard.h"
#include "strnatcmp.h"

enum Done : byte
{
	WS_CONTINUE,
	WS_EXIT,
	WS_PLAY,
};

enum Mode : byte
{
	MODE_PICKWORLD,
	MODE_SCROLL,
	MODE_MENU,
	MODE_CONFIRM_ERASE_PROGRESS,
	MODE_CONFIRM_ERASE_SCORES,
};

enum class ButtonId
{
	WorldList,
	PlayThisWorld,
	ResetThisWorld,
	ResetHighScores,
	ExitToMenu,
	RecordScoresTimes,
	SteamLeaderboardScore,
	SteamLeaderboardTime,
	ViewInWorkshop,
	PrevLevel,
	NextLevel,
	Yes,
	No,
	VerifyWorld,
	SortName,
	SortAuthor,
	SortComplete,
};

constexpr int NAME_X = 20;
constexpr int AUTH_X = 300;
constexpr int PERCENT_X = 580;

constexpr int GAP_HEIGHT = 18;
constexpr int SCROLLBAR_HEIGHT = 320;
constexpr int WORLDS_PER_SCREEN = 18;
constexpr int CLICK_SCROLL_AMT = WORLDS_PER_SCREEN * 3 / 4;

constexpr int WBTN_HEIGHT = 19;

struct worldDesc_t
{
	char fname[64];
	char name[64];
	char author[64];
	float percentage;
	bool dimmed;
};

static char curName[64];
static Mode mode;
static std::vector<worldDesc_t> list;
static int listPos, choice;
static byte *backgd;
static byte sortType,sortDir;
static int scrollY,scrollHeight,scrollOffset;
static int msx,msy;
static byte mouseB=255;
static std::unique_ptr<sprite_set_t> wsSpr;
static char msBright,msDBright;
static score_t top3[3];
static byte level,scoreMode=0,numScores,noScoresAtAll;
static world_t tmpWorld;
static vanilla::VfsMeta worldMeta;
static int mouseZ;
static int oldGamepad = ~0;
static bool mouseMode = false;

static ButtonId curButton;

#ifdef WTG
#define WORLD_DEBUGGING
static byte showFilenames = 1;
#elif !defined(NDEBUG)
#define WORLD_DEBUGGING
static byte showFilenames = 0;
#endif

#ifdef WORLD_DEBUGGING
static byte numMapsVerified = 0;
#endif

inline void FlipEm(worldDesc_t *me,worldDesc_t *you)
{
	std::swap(*me, *you);
}

byte Compare(worldDesc_t *me,worldDesc_t *you,byte field,byte bkwds)
{
	byte f;
	char tmp1[64] = {}, tmp2[64] = {};

	if(me->dimmed!=you->dimmed)
		return 0;	// no flipping between ones of different dim status
	if(!bkwds)
	{
		if(!strcmp(me->fname,"tutorial.dlw"))	// the tutorial comes before all else
			return 0;
		if(!strcmp(you->fname,"tutorial.dlw"))
		{
			FlipEm(me,you);
			return 1;
		}
	}
	else
	{
		if(!strcmp(you->fname,"tutorial.dlw"))	// the tutorial comes before all else (vice versa when backwards, so still on top)
			return 0;
		if(!strcmp(me->fname,"tutorial.dlw"))
		{
			FlipEm(me,you);
			return 1;
		}
	}

	f=0;
	switch(field)
	{
		case 0:
			SDL_strlcpy(tmp1,me->name,64);
			SDL_strlcpy(tmp2,you->name,64);
			f=(strnatcasecmp(tmp1,tmp2)>0);
			break;
		case 1:
			SDL_strlcpy(tmp1,me->author,64);
			SDL_strlcpy(tmp2,you->author,64);
			f=(strnatcasecmp(tmp1,tmp2)>0);
			break;
		case 2:
			f=(me->percentage>you->percentage);
			break;
	}

	if(f)
		FlipEm(me,you);

	return f;
}

void SortWorlds(byte field,byte backwards)
{
	int i;
	int flipped;

	flipped=1;

	while(flipped)
	{
		flipped=0;
		for(i=0;i<(int)list.size()-1;i++)
		{
			if(backwards)
			{
				if(Compare(&list[i+1],&list[i],field,1))
				{
					flipped++;
					if(choice==i)
						choice=i+1;
					else if(choice==i+1)
						choice=i;
				}
			}
			else
			{
				if(Compare(&list[i],&list[i+1],field,0))
				{
					flipped++;
					if(choice==i)
						choice=i+1;
					else if(choice==i+1)
						choice=i;
				}
			}
			if(list[i].dimmed && !list[i+1].dimmed)
			{
				flipped++;
				FlipEm(&list[i],&list[i+1]);
			}
		}
	}

	if(choice<listPos)
	{
		listPos=choice;
		if(listPos<0)
			listPos=0;
		if(listPos>(int)list.size()-WORLDS_PER_SCREEN)
			listPos=list.size()-WORLDS_PER_SCREEN;
	}
	if(choice>=listPos+WORLDS_PER_SCREEN)
	{
		listPos=choice-WORLDS_PER_SCREEN+1;
		if(listPos<0)
			listPos=0;
		if(listPos>(int)list.size()-WORLDS_PER_SCREEN)
			listPos=list.size()-WORLDS_PER_SCREEN;
	}
}

void InputWorld(const char *fname)
{
	worldDesc_t newItem;
	SDL_strlcpy(newItem.fname, fname, SDL_arraysize(newItem.fname));

	char fullname[64];
	snprintf(fullname, SDL_arraysize(fullname), "worlds/%s", fname);
	GetWorldName(fullname, newItem.name, newItem.author);

	worldData_t *w = GetWorldProgressNoCreate(newItem.fname);
	newItem.percentage = w ? w->percentage : 0.0f;

	newItem.dimmed = !CanPlayWorld(fname);

	list.push_back(newItem);
	profile.progress.totalWorlds = list.size();
}

TASK(void) ScanWorlds(void)
{
	std::vector<std::string> files = ListDirectory("worlds", ".dlw", 32);
	erase_if(files, [](const std::string& name) {
		return name == "backup_load.dlw"
			|| name == "backup_exit.dlw"
			|| name == "backup_save.dlw";
	});

	int count = files.size();
	int done = 0;

	dword start = timeGetTime();
	for (const std::string& name : files)
	{
		InputWorld(name.c_str());
		done++;

		dword now = timeGetTime();
		if (now - start > 50)  // 50 ms = 20 fps
		{
			start = now;
			GetDisplayMGL()->FillBox(20,440,20+(done*600)/count,450,32*1+16);
			AWAIT GetDisplayMGL()->Flip();
		}
	}
}

void CalcScrollBar(void)
{
	scrollHeight=SCROLLBAR_HEIGHT*WORLDS_PER_SCREEN/(list.size() ? list.size() : 1);
	if(scrollHeight<10)
		scrollHeight=10;
	if(scrollHeight>=SCROLLBAR_HEIGHT)
		scrollHeight=SCROLLBAR_HEIGHT-1;

	scrollY=SCROLLBAR_HEIGHT*listPos/(list.size() ? list.size() : 1);
	if(scrollY+scrollHeight>SCROLLBAR_HEIGHT)
		scrollY=SCROLLBAR_HEIGHT-scrollHeight;
}

void ReverseCalcScrollBar(void)
{
	if(scrollY<0)
		scrollY=0;
	if(scrollY+scrollHeight+1>SCROLLBAR_HEIGHT)
		scrollY=SCROLLBAR_HEIGHT-scrollHeight-1;

	if(scrollY>0)
		listPos=scrollY*list.size()/SCROLLBAR_HEIGHT+1;
	else
		listPos=0;
}

void FetchScores(byte backwards)
{
	byte curL;

	if(noScoresAtAll)
		return;

	if(level>=tmpWorld.numMaps)
		level=0;

	curL=level;

	while(1)
	{
		if(scoreMode==0)
			numScores=GetTopScores(top3,tmpWorld.map[level]);
		else
			numScores=GetTopTimes(top3,tmpWorld.map[level]);
		if(numScores)
			break;

		if(!backwards)
		{
			level++;
			if(level>=tmpWorld.numMaps)
				level=0;
			if(level==curL)
			{
				noScoresAtAll=1;
				break;
			}
			if(tmpWorld.map[level]->flags&MAP_HUB)
			{
				level++;
				if(level>=tmpWorld.numMaps)
					level=0;
			}
		}
		else
		{
			level--;
			if(level>=tmpWorld.numMaps)
				level=tmpWorld.numMaps-1;
			if(level==curL)
			{
				noScoresAtAll=1;
				break;
			}
			if(tmpWorld.map[level]->flags&MAP_HUB)
			{
				level--;
				if(level>=tmpWorld.numMaps)
					level=tmpWorld.numMaps-1;
			}
		}
		if(level==curL)
		{
			noScoresAtAll=1;
			break;
		}
	}

#ifdef WORLD_DEBUGGING
	numMapsVerified = 0;
	for (int i = 0; i < tmpWorld.numMaps; ++i)
		if (VerifyLevel(tmpWorld.map[i]))
			++numMapsVerified;
#endif
}

static void PrepWorld()
{
	char s[128];
	sprintf(s,"worlds/%s",list[choice].fname);
	LoadWorld(&tmpWorld,s);
	level=0;
	noScoresAtAll=0;
	FetchScores(0);
	AppdataVfs().query_bottom(s, &worldMeta);
	SteamManager::Get()->PrepWorldLeaderboard(s);
}

static void SelectLastWorld(void)
{
	int i;

	choice=0;

	for(i=0; i<(int)list.size(); i++)
		if(!strcmp(list[i].fname,profile.lastWorld))
			choice=i;

	if(listPos<=choice-WORLDS_PER_SCREEN)
		listPos=choice-WORLDS_PER_SCREEN+1;
	if(listPos>choice)
		listPos=choice;

	if (choice < (int)list.size())
		PrepWorld();
}

void MoveToNewWorld(void)
{
	FreeWorld(&tmpWorld);
	PrepWorld();
}

TASK(void) InitWorldSelect(MGLDraw *mgl)
{
	int i;

	mgl->LoadBMP("graphics/profmenu.bmp");
	backgd=(byte *)malloc(640*480);

	for(i=0;i<480;i++)
		memcpy(&backgd[i*640],&mgl->GetScreen()[i*mgl->GetWidth()],640);

	PrintGlow(20,20,"Loading...",0,2);
	AWAIT mgl->Flip();

	sortType=0;
	sortDir=0;
	listPos=0;

	list.clear();
	AWAIT ScanWorlds();
	SortWorlds(sortType,sortDir);
	SelectLastWorld();
	CalcScrollBar();
	mgl->GetMouse(&msx,&msy);
	mode=MODE_PICKWORLD;
	wsSpr = std::make_unique<sprite_set_t>("graphics/pause.jsp");
	msBright=0;
	msDBright=1;
	PlaySongForce("003worldpicker.ogg");
	mouseZ=mgl->mouse_z;

	curButton = ButtonId::WorldList;
	oldGamepad = ~0;
}

void ExitWorldSelect(void)
{
	free(backgd);
	list.clear();
	FreeWorld(&tmpWorld);
	wsSpr.reset();
}

TASK(Done) UpdateWorldSelect(int *lastTime,MGLDraw *mgl)
{
	static byte oldc = ~0;
	int i;

	int oldMsx = msx, oldMsy = msy;
	mgl->GetMouse(&msx, &msy);
	bool mb = mgl->MouseTap();
	if (mb || msx != oldMsx || msy != oldMsy)
	{
		mouseMode = true;
		curButton = ButtonId::WorldList;

		if (mode == MODE_PICKWORLD)
		{
			// clicking on the sort possibilities
			if (PointInRect(msx,msy,NAME_X,20,AUTH_X-20,37))
			{
				curButton = ButtonId::SortName;
			}
			else if (PointInRect(msx,msy,AUTH_X,20,PERCENT_X-GetStrLength("Complete",2)-20,37))
			{
				curButton = ButtonId::SortAuthor;
			}
			else if (PointInRect(msx,msy,PERCENT_X-GetStrLength("Complete",2),20,PERCENT_X+4,37))
			{
				curButton = ButtonId::SortComplete;
			}
			// play world
			else if (PointInRect(msx,msy,20,371,20+150,371+WBTN_HEIGHT))
			{
				curButton = ButtonId::PlayThisWorld;
			}
			// reset world
			else if (PointInRect(msx,msy,20,395,20+150,395+WBTN_HEIGHT))
			{
				curButton = ButtonId::ResetThisWorld;
			}
			// reset high scores
			else if (PointInRect(msx,msy,20,419,20+150,419+WBTN_HEIGHT))
			{
				curButton = ButtonId::ResetHighScores;
			}
			// exit to menu
			else if (PointInRect(msx,msy,20,443,20+150,443+WBTN_HEIGHT))
			{
				curButton = ButtonId::ExitToMenu;
			}
			// hi score buttons
			else if (PointInRect(msx,msy,180,371,180+150,371+WBTN_HEIGHT) && !list[choice].dimmed)
			{
				curButton = ButtonId::RecordScoresTimes;
			}
			else if (PointInRect(msx,msy,335,371,335+20,371+WBTN_HEIGHT) && !list[choice].dimmed)
			{
				curButton = ButtonId::PrevLevel;
			}
			else if (PointInRect(msx,msy,592,371,592+20,371+WBTN_HEIGHT) && !list[choice].dimmed)
			{
				curButton = ButtonId::NextLevel;
			}
			// Steam features
			else if (PointInRect(msx,msy,180,395,180+150,395+WBTN_HEIGHT) && !list[choice].dimmed && worldMeta.steamWorkshopId)
			{
				curButton = ButtonId::ViewInWorkshop;
			}
			else if (PointInRect(msx,msy,180,419,180+150,419+WBTN_HEIGHT) && !list[choice].dimmed && SteamManager::Get()->LeaderboardIdScore())
			{
				curButton = ButtonId::SteamLeaderboardScore;
			}
			else if (PointInRect(msx,msy,180,443,180+150,443+WBTN_HEIGHT) && !list[choice].dimmed && SteamManager::Get()->LeaderboardIdTime())
			{
				curButton = ButtonId::SteamLeaderboardTime;
			}
		}
		else if (mode == MODE_CONFIRM_ERASE_PROGRESS || mode == MODE_CONFIRM_ERASE_SCORES)
		{
			if (PointInRect(msx,msy,70,270,70+50,270+WBTN_HEIGHT))
			{
				curButton = ButtonId::Yes;
			}
			else if (PointInRect(msx,msy,600-30-50,270,600-30-50+50,270+WBTN_HEIGHT))
			{
				curButton = ButtonId::No;
			}
		}
	}

	byte c = GetControls() | GetArrows();
	dword gamepad = GetGamepadButtons();
	if (gamepad)
		mouseMode = false;

	if(*lastTime>TIME_PER_FRAME*5)
		*lastTime=TIME_PER_FRAME*5;
	while(*lastTime>=TIME_PER_FRAME)
	{
		msBright+=msDBright;
		if(msBright>10)
			msDBright=-1;
		if(msBright<-2)
			msDBright=1;
		mgl->Process();
		*lastTime-=TIME_PER_FRAME;
	}

	if(mode==MODE_PICKWORLD)
	{
		if ((c & ~oldc) & CONTROL_UP)
		{
			switch (curButton)
			{
				case ButtonId::WorldList:
					if(choice>0)
					{
						choice--;
						MoveToNewWorld();
					}
					if(choice<listPos)
					{
						listPos--;
						CalcScrollBar();
					}
					break;
				case ButtonId::PlayThisWorld:
					curButton = ButtonId::SortName;
					break;
				case ButtonId::ResetThisWorld:
					curButton = ButtonId::PlayThisWorld;
					break;
				case ButtonId::ResetHighScores:
					curButton = ButtonId::ResetThisWorld;
					break;
				case ButtonId::ExitToMenu:
					curButton = ButtonId::ResetHighScores;
					break;
				case ButtonId::ViewInWorkshop:
					curButton = ButtonId::RecordScoresTimes;
					break;
				case ButtonId::SteamLeaderboardScore:
					if (worldMeta.steamWorkshopId)
						curButton = ButtonId::ViewInWorkshop;
					else
						curButton = ButtonId::RecordScoresTimes;
					break;
				case ButtonId::SteamLeaderboardTime:
					if (SteamManager::Get()->LeaderboardIdScore())
						curButton = ButtonId::SteamLeaderboardScore;
					else if (worldMeta.steamWorkshopId)
						curButton = ButtonId::ViewInWorkshop;
					else
						curButton = ButtonId::RecordScoresTimes;
					break;
				default: break;
			}
		}

		if ((c & ~oldc) & CONTROL_DN)
		{
			switch (curButton)
			{
				case ButtonId::WorldList:
					if(choice<(int)list.size()-1)
					{
						choice++;
						MoveToNewWorld();
					}
					if(choice>listPos+17)
					{
						listPos++;
						CalcScrollBar();
					}
					break;
				case ButtonId::PlayThisWorld:
					curButton = ButtonId::ResetThisWorld;
					break;
				case ButtonId::ResetThisWorld:
					curButton = ButtonId::ResetHighScores;
					break;
				case ButtonId::ResetHighScores:
					curButton = ButtonId::ExitToMenu;
					break;
				case ButtonId::SortName:
				case ButtonId::SortAuthor:
				case ButtonId::SortComplete:
					curButton = ButtonId::PlayThisWorld;
					break;
				case ButtonId::RecordScoresTimes:
					if (worldMeta.steamWorkshopId)
						curButton = ButtonId::ViewInWorkshop;
					else if (SteamManager::Get()->LeaderboardIdScore())
						curButton = ButtonId::SteamLeaderboardScore;
					else if (SteamManager::Get()->LeaderboardIdTime())
						curButton = ButtonId::SteamLeaderboardTime;
					break;
				case ButtonId::ViewInWorkshop:
					if (SteamManager::Get()->LeaderboardIdScore())
						curButton = ButtonId::SteamLeaderboardScore;
					else if (SteamManager::Get()->LeaderboardIdTime())
						curButton = ButtonId::SteamLeaderboardTime;
					break;
				default: break;
			}
		}

		if ((c & ~oldc) & CONTROL_RT)
		{
			switch (curButton)
			{
				case ButtonId::PlayThisWorld:
					if (!list[choice].dimmed)
						curButton = ButtonId::RecordScoresTimes;
					break;
				case ButtonId::ResetThisWorld:
					if (!list[choice].dimmed)
					{
						if (worldMeta.steamWorkshopId)
							curButton = ButtonId::ViewInWorkshop;
						else
							curButton = ButtonId::RecordScoresTimes;
					}
					break;
				case ButtonId::ResetHighScores:
					if (!list[choice].dimmed)
					{
						if (SteamManager::Get()->LeaderboardIdScore())
							curButton = ButtonId::SteamLeaderboardScore;
						else if (worldMeta.steamWorkshopId)
							curButton = ButtonId::ViewInWorkshop;
						else
							curButton = ButtonId::RecordScoresTimes;
					}
					break;
				case ButtonId::ExitToMenu:
					if (!list[choice].dimmed)
					{
						if (SteamManager::Get()->LeaderboardIdTime())
							curButton = ButtonId::SteamLeaderboardTime;
						else if (SteamManager::Get()->LeaderboardIdScore())
							curButton = ButtonId::SteamLeaderboardScore;
						else if (worldMeta.steamWorkshopId)
							curButton = ButtonId::ViewInWorkshop;
						else
							curButton = ButtonId::RecordScoresTimes;
					}
					break;
				case ButtonId::RecordScoresTimes:
				case ButtonId::ViewInWorkshop:
				case ButtonId::SteamLeaderboardScore:
				case ButtonId::SteamLeaderboardTime:
					if (!noScoresAtAll)
						curButton = ButtonId::PrevLevel;
					break;
				case ButtonId::PrevLevel:
					curButton = ButtonId::NextLevel;
					break;
				case ButtonId::SortName:
					curButton = ButtonId::SortAuthor;
					break;
				case ButtonId::SortAuthor:
					curButton = ButtonId::SortComplete;
					break;
				default: break;
			}
		}

		if ((c & ~oldc) & CONTROL_LF)
		{
			switch (curButton)
			{
				case ButtonId::RecordScoresTimes:
					curButton = ButtonId::PlayThisWorld;
					break;
				case ButtonId::PrevLevel:
					curButton = ButtonId::RecordScoresTimes;
					break;
				case ButtonId::NextLevel:
					curButton = ButtonId::PrevLevel;
					break;
				case ButtonId::SortAuthor:
					curButton = ButtonId::SortName;
					break;
				case ButtonId::SortComplete:
					curButton = ButtonId::SortAuthor;
					break;
				case ButtonId::ViewInWorkshop:
					curButton = ButtonId::ResetThisWorld;
					break;
				case ButtonId::SteamLeaderboardScore:
					curButton = ButtonId::ResetHighScores;
					break;
				case ButtonId::SteamLeaderboardTime:
					curButton = ButtonId::ExitToMenu;
					break;
				default: break;
			}
		}

		if (mb || ((c & ~oldc) & CONTROL_B1))
		{
			switch (curButton)
			{
				case ButtonId::WorldList:
					if (mb)
						break;
					[[fallthrough]];
				case ButtonId::PlayThisWorld:
					if(list[choice].dimmed)
						MakeNormalSound(SND_TURRETBZZT);
					else
					{
						oldc=255;
						CO_RETURN WS_PLAY;
					}
					break;

				case ButtonId::SortName:
					if(sortType==0)
						sortDir=1-sortDir;
					else
					{
						sortType=0;
						sortDir=0;
					}
					SortWorlds(sortType,sortDir);
					CalcScrollBar();
					break;
				case ButtonId::SortAuthor:
					if(sortType==1)
						sortDir=1-sortDir;
					else
					{
						sortType=1;
						sortDir=0;
					}
					SortWorlds(sortType,sortDir);
					CalcScrollBar();
					break;
				case ButtonId::SortComplete:
					if(sortType==2)
						sortDir=1-sortDir;
					else
					{
						sortType=2;
						sortDir=0;
					}
					SortWorlds(sortType,sortDir);
					CalcScrollBar();
					break;

				case ButtonId::ResetThisWorld:
					if(list[choice].dimmed)
						MakeNormalSound(SND_TURRETBZZT);
					else
					{
						mode = MODE_CONFIRM_ERASE_PROGRESS;
						if (!mouseMode) curButton = ButtonId::No;
					}
					break;
				case ButtonId::ResetHighScores:
					if(list[choice].dimmed)
						MakeNormalSound(SND_TURRETBZZT);
					else
					{
						mode = MODE_CONFIRM_ERASE_SCORES;
						if (!mouseMode) curButton = ButtonId::No;
					}
					break;
				case ButtonId::ExitToMenu:
					oldc=255;
					CO_RETURN WS_EXIT;
					break;

				case ButtonId::RecordScoresTimes:
					scoreMode=1-scoreMode;
					noScoresAtAll=0;
					FetchScores(0);
					break;
				case ButtonId::PrevLevel:
					level--;
					if(level>=tmpWorld.numMaps)
						level=tmpWorld.numMaps-1;
					FetchScores(1);
					break;
				case ButtonId::NextLevel:
					level++;
					if(level>=tmpWorld.numMaps)
						level=0;
					FetchScores(0);
					break;

				case ButtonId::ViewInWorkshop: {
					std::string url = "https://steamcommunity.com/sharedfiles/filedetails/?id=";
					url.append(std::to_string(worldMeta.steamWorkshopId));
					SteamManager::Get()->OpenURLOverlay(url.c_str());
					break;
				}

				case ButtonId::SteamLeaderboardScore:
					AWAIT ViewWorldLeaderboard(mgl, &tmpWorld, WorldLeaderboardKind::Score, SteamManager::Get()->LeaderboardIdScore());
					mouseZ=mgl->mouse_z;
					oldc = ~0;
					CO_RETURN WS_CONTINUE;

				case ButtonId::SteamLeaderboardTime:
					AWAIT ViewWorldLeaderboard(mgl, &tmpWorld, WorldLeaderboardKind::Time, SteamManager::Get()->LeaderboardIdTime());
					mouseZ=mgl->mouse_z;
					oldc = ~0;
					CO_RETURN WS_CONTINUE;

				default: break;
			}
		}

		if ((c & ~oldc) & CONTROL_B2)
		{
			if (curButton == ButtonId::WorldList)
				curButton = ButtonId::PlayThisWorld;
			else
				curButton = ButtonId::WorldList;
		}

		byte scan = LastScanCode();
		if (scan == SDL_SCANCODE_PAGEUP || (gamepad & ~oldGamepad & (1 << SDL_CONTROLLER_BUTTON_LEFTSHOULDER))) {
			listPos = std::max(listPos - WORLDS_PER_SCREEN, 0);
			choice = std::max(choice - WORLDS_PER_SCREEN, 0);
			CalcScrollBar();
			MoveToNewWorld();
		}
		if (scan == SDL_SCANCODE_PAGEDOWN || (gamepad & ~oldGamepad & (1 << SDL_CONTROLLER_BUTTON_RIGHTSHOULDER))) {
			listPos = std::min(listPos + WORLDS_PER_SCREEN, (int)list.size() - WORLDS_PER_SCREEN);
			choice = std::min(choice + WORLDS_PER_SCREEN, (int)list.size() - 1);
			CalcScrollBar();
			MoveToNewWorld();
		}
#ifdef WORLD_DEBUGGING
		else if (scan == SDL_SCANCODE_F3)
		{
			showFilenames = !showFilenames;
		}
#endif

		int mv=mouseZ-mgl->mouse_z;
		mouseZ=mgl->mouse_z;
		if(mv<0)
		{
			listPos+=mv;
			if(listPos<0)
				listPos=0;
			CalcScrollBar();
		}
		else if(mv>0)
		{
			listPos+=mv;
			if(listPos>(int)list.size()-WORLDS_PER_SCREEN)
				listPos=list.size()-WORLDS_PER_SCREEN;
			if(listPos<0)
				listPos=0;
			CalcScrollBar();
		}

		if(mb)
		{
			// clicking on a world
			for(i=0;i<18;i++)
			{
				if(i+listPos<(int)list.size())
				{
					if(PointInRect(msx,msy,17,39+i*GAP_HEIGHT,599,39+GAP_HEIGHT+i*GAP_HEIGHT-1))
					{
						if(i+listPos!=choice)
						{
							choice=i+listPos;
							MoveToNewWorld();
						}
						else
						{
							if(list[choice].dimmed)
								MakeNormalSound(SND_TURRETBZZT);
							else
							{
								oldc=255;
								CO_RETURN WS_PLAY;
							}
						}
					}
				}
			}
			// scroll bar click
			if(PointInRect(msx,msy,605,40,620,42+SCROLLBAR_HEIGHT))
			{
				if(msy>=scrollY+41 && msy<=41+scrollY+scrollHeight-1)
				{
					mode=MODE_SCROLL;
					scrollOffset=(msy-scrollY);
				}
				else if(msy<scrollY+41)
				{
					listPos-=CLICK_SCROLL_AMT;
					if(listPos<0)
						listPos=0;
					CalcScrollBar();
				}
				else if(msy>41+scrollY+scrollHeight-1)
				{
					listPos+=CLICK_SCROLL_AMT;
					if(listPos>(int)list.size()-WORLDS_PER_SCREEN)
						listPos=list.size()-WORLDS_PER_SCREEN;
					if(listPos<0)
						listPos=0;
					CalcScrollBar();
				}
			}
		}

#ifdef WORLD_DEBUGGING
		if (showFilenames && scan == SDL_SCANCODE_KP_PLUS)
		{
			printf("Verifying world: %s\n", tmpWorld.map[0]->name);

			std::set<dword> seen;
			std::vector<dword> ordered;

			owned::SDL_RWops input = AssetOpen_SDL_Owned("worlds/levels.dat");
			for (dword item; SDL_RWread(input, &item, sizeof(dword), 1) == 1;)
			{
				seen.insert(item);
				ordered.push_back(item);
			}
			input.reset();

			for (int i = 0; i < tmpWorld.numMaps; ++i)
			{
				dword item = ChecksumMap(tmpWorld.map[i]);
				if (seen.find(item) == seen.end())
				{
					seen.insert(item);
					ordered.push_back(item);
					printf("Adding: %s\n", tmpWorld.map[i]->name);
				}
				else
				{
					printf("Already verified: %s\n", tmpWorld.map[i]->name);
				}
			}

			FILE* output = AssetOpen_Write("worlds/levels.dat");
			for (dword item : ordered)
			{
				fwrite(&item, sizeof(dword), 1, output);
			}
			fclose(output);

			FetchScores(0);
		}
#endif
	}
	else if(mode==MODE_SCROLL)
	{
		if((mgl->mouse_b&1)==0)
			mode=MODE_PICKWORLD;
		else
		{
			scrollY=msy-scrollOffset;
			ReverseCalcScrollBar();
		}
	}
	else if(mode==MODE_CONFIRM_ERASE_PROGRESS || mode==MODE_CONFIRM_ERASE_SCORES)
	{
		if (c & ~oldc & (CONTROL_LF | CONTROL_RT))
		{
			switch (curButton)
			{
				case ButtonId::Yes:
				default:
					curButton = ButtonId::No;
					break;
				case ButtonId::No:
					curButton = ButtonId::Yes;
					break;
			}
		}

		if (mb || (c & ~oldc & CONTROL_B1))
		{
			switch (curButton)
			{
				case ButtonId::Yes:
					MakeNormalSound(SND_MENUSELECT);
					if(mode==MODE_CONFIRM_ERASE_PROGRESS)
						EraseWorldProgress(list[choice].fname);
					else
						EraseHighScores(&tmpWorld);
					ExitWorldSelect();
					AWAIT InitWorldSelect(mgl);
					break;
				case ButtonId::No:
					MakeNormalSound(SND_MENUSELECT);
					mode = MODE_PICKWORLD;
					curButton = ButtonId::WorldList;
					break;
				default: break;
			}
		}

		if ((c & ~oldc & CONTROL_B2) || (gamepad & ~oldGamepad & (1 << SDL_CONTROLLER_BUTTON_BACK)))
		{
			MakeNormalSound(SND_MENUSELECT);
			mode = MODE_PICKWORLD;
			curButton = ButtonId::WorldList;
		}
	}

	oldc = c;
	oldGamepad = gamepad;

	if(mgl->LastKeyPressed() == 27)
	{
		oldc=255;
		CO_RETURN WS_EXIT;
	}

	mouseB=mgl->mouse_b;
	CO_RETURN WS_CONTINUE;
}

void RenderWorldSelectButton(int x,int y,int wid,const char *txt,MGLDraw *mgl, ButtonId id)
{
	if(curButton == id)
	{
		mgl->Box(x,y,x+wid,y+WBTN_HEIGHT,32+31);
		mgl->FillBox(x+1,y+1,x+wid-1,y+WBTN_HEIGHT-1,32+8);
	}
	else
		mgl->Box(x,y,x+wid,y+WBTN_HEIGHT,32+16);

	PrintGlow(x+2,y+2,txt,0,2);
}

void RenderWorldSelect(MGLDraw *mgl)
{
	int i,msx2,msy2;
	char s[16],b;

	for(i=0;i<480;i++)
		memcpy(&mgl->GetScreen()[i*mgl->GetWidth()],&backgd[i*640],640);

	if (curButton == ButtonId::SortName)
		mgl->FillBox(NAME_X,20,AUTH_X-20,37,32+8);
	if (curButton == ButtonId::SortAuthor)
		mgl->FillBox(AUTH_X,20,PERCENT_X-GetStrLength("Complete",2)-20,37,32+8);
	if (curButton == ButtonId::SortComplete)
		mgl->FillBox(PERCENT_X-GetStrLength("Complete",2),20,PERCENT_X+4,37,32+8);

	PrintGlow(NAME_X,20,"World",6,2);
	PrintGlow(AUTH_X,20,"Author",6,2);
	PrintGlow(PERCENT_X-GetStrLength("Complete",2),20,"Complete",6,2);

	// lines to separate stuff
	mgl->FillBox(20,37,620,37,32+16);
	mgl->FillBox(20,364,620,364,32+16);

	// the world list
	for(i=0;i<18;i++)
	{
		if(i+listPos<(int)list.size())
		{
			if(choice==i+listPos)
			{
				if (mouseMode || curButton == ButtonId::WorldList)
					mgl->FillBox(17,39+i*GAP_HEIGHT,599,39+GAP_HEIGHT+i*GAP_HEIGHT-1,32+8);
				else
					mgl->Box(17,39+i*GAP_HEIGHT,599,39+GAP_HEIGHT+i*GAP_HEIGHT-1,32+16);
			}

			if(mouseMode && PointInRect(msx,msy,17,39+i*GAP_HEIGHT,599,39+GAP_HEIGHT+i*GAP_HEIGHT-1))
			{
				mgl->Box(17,39+i*GAP_HEIGHT,599,39+GAP_HEIGHT+i*GAP_HEIGHT-1,32+16);
			}
			if(list[i+listPos].dimmed)
				b=-10;
			else
				b=0;

#ifdef WORLD_DEBUGGING
			if (showFilenames)
			{
				// Debug/WTG mode: show name, fname, author
				PrintGlow(NAME_X,40+i*GAP_HEIGHT,list[i+listPos].name,b,1);
				PrintGlow(AUTH_X-70,40+i*GAP_HEIGHT,list[i+listPos].fname,b,1);

				if (choice == i+listPos)
				{
					char msg[64];
					if (numMapsVerified == 0)
					{
						sprintf(msg, "V: no");
					}
					else if (numMapsVerified == tmpWorld.numMaps)
					{
						sprintf(msg, "V: all maps");
					}
					else
					{
						sprintf(msg, "V: %d of %d maps", numMapsVerified, tmpWorld.numMaps);
					}
					PrintGlow(AUTH_X+120,40+i*GAP_HEIGHT,msg,0,1);
				}
				else
				{
					PrintGlow(AUTH_X+120,40+i*GAP_HEIGHT,list[i+listPos].author,b,1);
				}
			}
			else
#endif
			{
				// Normal mode: show name, author, and % completion
				PrintGlow(NAME_X,40+i*GAP_HEIGHT,list[i+listPos].name,b,2);
				// Shift over the author if the world name is a little long
				int endX = NAME_X + GetStrLength(list[i+listPos].name, 2) + 8;
				PrintGlow(std::max(AUTH_X, endX),40+i*GAP_HEIGHT,list[i+listPos].author,b,2);
				if(list[i+listPos].percentage==0.0f)
					strcpy(s,"0%");
				else if(list[i+listPos].percentage==100.0f)
					strcpy(s,"100%");
				else
					sprintf(s,"%0.1f%%",list[i+listPos].percentage);
				PrintGlow(PERCENT_X-GetStrLength(s,2),40+i*GAP_HEIGHT,s,b,2);
			}
		}
		else
			break;
	}

	// the scrollbar
	mgl->Box(605,39,620,39+3+SCROLLBAR_HEIGHT,32+16);
	mgl->FillBox(606,40,619,39+2+SCROLLBAR_HEIGHT,32+6);
	mgl->FillBox(607,41+scrollY,618,41+scrollY+scrollHeight,32+20);

	// buttons
	RenderWorldSelectButton(20,371,150,"Play This World",mgl, ButtonId::PlayThisWorld);
	RenderWorldSelectButton(20,395,150,"Reset This World",mgl, ButtonId::ResetThisWorld);
	RenderWorldSelectButton(20,419,150,"Reset High Scores",mgl, ButtonId::ResetHighScores);
	RenderWorldSelectButton(20,443,150,"Exit To Menu",mgl, ButtonId::ExitToMenu);

	if(list[choice].dimmed)
	{
		PrintGlow(200,411,"You need to buy this world in the SpisMall to play it!",0,2);
	}
	else
	{
		// high score section
		if(scoreMode==1)
			RenderWorldSelectButton(180,371,150,"Local Times",mgl, ButtonId::RecordScoresTimes);
		else
			RenderWorldSelectButton(180,371,150,"Local Scores",mgl, ButtonId::RecordScoresTimes);

		if(!noScoresAtAll)
		{
			RenderWorldSelectButton(335,371,20,"<<",mgl, ButtonId::PrevLevel);
			PrintGlowLimited(359,373,590,tmpWorld.map[level]->name,0,2);
			RenderWorldSelectButton(592,371,20,">>",mgl, ButtonId::NextLevel);

			// now the scores themselves
			for(i=0;i<3;i++)
			{
				s[2]='\0';
				s[1]='.';
				s[0]=i+'1';	// equivalent to sprintf(s,"%d.",i);
				PrintGlow(335,395+i*20,s,0,2);
				if(i>=numScores)
					PrintGlow(350,395+i*20,"?????",0,2);
				else
					PrintGlow(350,395+i*20,top3[i].name,0,2);
				if(scoreMode==1)
				{
					if(i>=numScores)
						strcpy(s,"0:00.00");
					else
						sprintf(s,"%u:%02u.%02u",top3[i].score/(30*60),(top3[i].score/30)%60,(top3[i].score%30)*100/30);
				}
				else
				{
					if(i>=numScores)
						strcpy(s,"0");
					else
						sprintf(s,"%u",top3[i].score);
				}
				PrintGlow(615-GetStrLength(s,2),395+i*20,s,0,2);
			}
		}

		if (worldMeta.steamWorkshopId)
		{
			RenderWorldSelectButton(180,395,150, "View in Workshop", mgl, ButtonId::ViewInWorkshop);
		}

		if (SteamManager::Get()->LeaderboardIdScore())
		{
			RenderWorldSelectButton(180,419,150, "Steam Top Scores", mgl, ButtonId::SteamLeaderboardScore);
		}
		if (SteamManager::Get()->LeaderboardIdTime())
		{
			RenderWorldSelectButton(180,443,150, "Steam Top Times", mgl, ButtonId::SteamLeaderboardTime);
		}
	}

	if(mode==MODE_CONFIRM_ERASE_PROGRESS || mode==MODE_CONFIRM_ERASE_SCORES)
	{
		mgl->FillBox(40,150,600,300,32*1+4);
		mgl->Box(40,150,600,300,32*1+16);
		if(mode==MODE_CONFIRM_ERASE_PROGRESS)
			PrintGlowRect(50,160,590,250,18,"Are you sure you want to reset this world?  That will erase all of your progress "
										   "in the world, but leave high scores unchanged.",2);
		else
			PrintGlowRect(50,160,590,250,18,"Are you sure you want to reset the high scores for this world?  That will erase "
										   "the high scores, but keep your progress in the world.",2);
		RenderWorldSelectButton(70,270,50,"Yes",mgl, ButtonId::Yes);
		RenderWorldSelectButton(600-30-50,270,50,"No",mgl, ButtonId::No);
	}

	if (mouseMode)
	{
		SetSpriteConstraints(13,13,627,467);
		msx2=msx;
		msy2=msy;
		if(msx2<13)
			msx2=13;
		if(msy2<13)
			msy2=13;
		if(msx2>622)
			msx2=622;
		if(msy2>462)
			msy2=462;
		wsSpr->GetSprite(0)->DrawBright(msx2,msy2,mgl,msBright/2);
		ClearSpriteConstraints();
	}
}

TASK(byte) WorldSelectMenu(MGLDraw *mgl)
{
	Done done=WS_CONTINUE;
	int lastTime=1;
	char fname[32];

	AWAIT InitWorldSelect(mgl);

	if(list.empty())
		CO_RETURN 1;	// just skip it if there are no worlds!

	while(done==WS_CONTINUE)
	{
		lastTime+=TimeLength();
		StartClock();
		done=AWAIT UpdateWorldSelect(&lastTime,mgl);
		RenderWorldSelect(mgl);
		AWAIT mgl->Flip();

		if(!mgl->Process())
			done=WS_EXIT;

		if(done==WS_PLAY)
		{
			strcpy(fname,list[choice].fname);
			ExitWorldSelect();
			if(AWAIT PlayWorld(mgl,fname)==0)
			{
				CO_RETURN 1;
			}
			AWAIT InitWorldSelect(mgl);
			done=WS_CONTINUE;
		}
		EndClock();
	}

	ExitWorldSelect();
	CO_RETURN 1;
}

const char *WorldFName(void)
{
	return curName;
}

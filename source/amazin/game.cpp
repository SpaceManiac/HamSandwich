#include "game.h"
#include "cheat.h"
#include "clock.h"
#include "display.h"
#include "guy.h"
#include "highscore.h"
#include "loop.h"
#include "map.h"
#include "options.h"
#include "particle.h"
#include "pause.h"
#include "player.h"
#include "register.h"
#include "sound.h"
#include "status.h"
#include "timeattack.h"
#include "title.h"

byte g_EndAnim[2];
byte g_EndAnimPumpkins;
byte g_LevelWon;
byte g_RivalryRound;
GameMode g_GameMode;
byte g_NumPlayers;
byte g_MapNum;
bool g_RivalryHasWinner;
byte g_SongTimer;
int g_RecordTimeFrames;
byte g_RivalryWinner;
bool g_IsTimeAttack;
int g_LevelIntroX;

bool GameUpdate();
void GameRender();

void GameInitHappy();
void GameInitDank();
void GameInitRivalry();

void GameInit(GameMode gameMode, bool isTimeAttack, byte mapNum, byte numPlayers)
{
	dword time;

	g_IsTimeAttack = isTimeAttack;
	g_GameMode = gameMode;
	g_NumPlayers = numPlayers;
	CheatInit();
	PlayerInit();
	time = timeGetTime();
	srand(time);
	SetUpdate(GameUpdate);
	SetRender(GameRender);
	DisplayListClear();
	StatusInit();
	g_MapNum = mapNum;
	g_RivalryRound = 0;
	g_RivalryHasWinner = false;
	g_RecordTimeFrames = 0;
	if (g_GameMode == GameMode::HappyFields)
	{
		GameInitHappy();
	}
	else if (g_GameMode == GameMode::DankDungeons)
	{
		GameInitDank();
	}
	else if (g_GameMode == GameMode::SiblingRivalry)
	{
		GameInitRivalry();
	}
	g_SongTimer = 0x3c;
	mgl->LastKeyPressed();
}

void GameExit()
{
	StatusExit();
}

void GameInitHappy()
{
	bool bVar1;
	byte y;
	int i;
	int x;
	char buf[32];

	sprintf(buf, "data/happy%03d.map", (unsigned int)g_MapNum);
	MapLoad(buf);
	MapRenderToBackground(GameMode::HappyFields);
	MapSetupStart();
	GuysInit();
	g_LevelWon = 0;
	x = 0;
	y = 0;
	auto map = &g_Map[0][0];
	for (i = 0; i < 0x10a; i = i + 1)
	{
		if ((map[i].flags & TileFlags::P1) != 0)
		{
			bVar1 = ConfigGetBouaphettaMode();
			GuyAdd((byte)x, y, GuyType(bVar1 + byte(GuyType::Bouapha)));
		}
		if (((map[i].flags & TileFlags::P2) != 0) && (g_NumPlayers == 2))
		{
			bVar1 = ConfigGetBouaphettaMode();
			GuyAdd((byte)x, y, GuyType(byte(GuyType::Bouaphetta) - bVar1));
		}
		if ((map[i].flags & TileFlags::PK) != 0)
		{
			GuyAdd((byte)x, y, GuyType::Smoove);
			GuyAdd((byte)x, y, GuyType::Chuckles);
			GuyAdd((byte)x, y, GuyType::Helga);
			GuyAdd((byte)x, y, GuyType::Pete);
		}
		x = x + 1;
		if (0x12 < x)
		{
			x = 0;
			y = y + 1;
		}
	}
	MapSpawnItems();
	g_LevelIntroX = -0xdc;
	g_SongTimer = 0x5a;
	ParticlesInit();
}

void GameInitDank()
{
	bool bVar1;
	byte y;
	unsigned int h;
	unsigned int w;
	int i;
	int x;

	w = g_MapNum + 5;
	if (0x13 < w)
	{
		w = 0x13;
	}
	h = ((int)(unsigned int)g_MapNum >> 1) + 5;
	if (0xe < h)
	{
		h = 0xe;
	}
	MapRandomize(w, h);
	MapRenderToBackground(GameMode::DankDungeons);
	MapSetupStart();
	GuysInit();
	g_LevelWon = 0;
	x = 0;
	y = 0;
	auto map = &g_Map[0][0];
	for (i = 0; i < 0x10a; i = i + 1)
	{
		if ((map[i].flags & TileFlags::P1) != 0)
		{
			bVar1 = ConfigGetBouaphettaMode();
			GuyAdd((byte)x, y, GuyType(bVar1 + byte(GuyType::Bouapha)));
		}
		if (((map[i].flags & TileFlags::P2) != 0) && (g_NumPlayers == 2))
		{
			bVar1 = ConfigGetBouaphettaMode();
			GuyAdd((byte)x, y, GuyType(byte(GuyType::Bouaphetta) - bVar1));
		}
		if ((map[i].flags & TileFlags::PK) != 0)
		{
			GuyAdd((byte)x, y, GuyType::Smoove);
			GuyAdd((byte)x, y, GuyType::Chuckles);
			GuyAdd((byte)x, y, GuyType::Helga);
			GuyAdd((byte)x, y, GuyType::Pete);
		}
		x = x + 1;
		if (0x12 < x)
		{
			x = 0;
			y = y + 1;
		}
	}
	MapSpawnItems();
	g_LevelIntroX = -0xdc;
	g_SongTimer = 0x5a;
	ParticlesInit();
}

void GameInitRivalry(void)
{
	bool bVar1;
	byte cVar2;
	unsigned int uVar2;
	unsigned int local_30;
	int i;
	unsigned int x;
	char buf[32];
	byte y;

	bVar1 = RegistrationCurrentlyValid();
	if (bVar1)
	{
		cVar2 = ConfigGetRivalryMode();
		if (cVar2 == 1)
		{
			uVar2 = MGL_random(0x14);
			sprintf(buf, "data/happy%03d.map", (uVar2 & 0xffff) + 1);
			MapLoad(buf);
		}
		else if (cVar2 == 2)
		{
			y = ConfigGetRivalryMapSize();
			x = (unsigned int)y * 4 + 2;
			if (0x13 < x)
			{
				x = 0x13;
			}
			y = ConfigGetRivalryMapSize();
			local_30 = (unsigned int)y * 3 + 2;
			if (0xe < local_30)
			{
				local_30 = 0xe;
			}
			MapRandomize(x, local_30);
		}
	}
	else
	{
		MapLoad("data/happy001.map");
	}
	MapFilterForRivalry();
	MapRenderToBackground(GameMode::SiblingRivalry);
	MapSetupStart();
	GuysInit();
	g_MapNum = ConfigGetRivalryLevel();
	g_Player[0].candles = 0;
	g_Player[1].candles = 0;
	g_RivalryRound = g_RivalryRound + 1;
	g_LevelWon = 0;
	x = 0;
	y = 0;
	auto map = &g_Map[0][0];
	for (i = 0; i < 0x10a; i = i + 1)
	{
		if ((map[i].flags & TileFlags::P1) != 0)
		{
			bVar1 = ConfigGetBouaphettaMode();
			GuyAdd((byte)x, y, GuyType(bVar1 + byte(GuyType::Bouapha)));
		}
		if (((map[i].flags & TileFlags::P2) != 0) && (g_NumPlayers == 2))
		{
			bVar1 = ConfigGetBouaphettaMode();
			GuyAdd((byte)x, y, GuyType(byte(GuyType::Bouaphetta) - bVar1));
		}
		if (((map[i].flags & TileFlags::PK) != 0) && (bVar1 = ConfigGetRivalryPumpkins(), bVar1))
		{
			GuyAdd((byte)x, y, GuyType::Smoove);
			GuyAdd((byte)x, y, GuyType::Chuckles);
			GuyAdd((byte)x, y, GuyType::Helga);
			GuyAdd((byte)x, y, GuyType::Pete);
		}
		x = x + 1;
		if (0x12 < (int)x)
		{
			x = 0;
			y = y + 1;
		}
	}
	MapSpawnItems();
	g_LevelIntroX = -0xdc;
	g_SongTimer = 0x5a;
	ParticlesInit();
}

bool GameUpdate(void)
{
	bool isPaused;
	bool bVar1;
	char k;
	Cheat cheat;
	unsigned int uVar2;

	isPaused = IsPaused();
	if (!isPaused)
	{
		k = mgl->LastKeyPressed();
		if (k == '\x1b')
		{
			PauseInit();
		}
		if (k != '\0')
		{
			cheat = CheatKeyPressed(k);
			if (cheat == Cheat::Skippy)
			{
				g_LevelWon = 1;
			}
			else if (cheat == Cheat::Umlaut)
			{
				if (g_Player[0].lives < 45)
				{
					g_Player[0].lives = g_Player[0].lives + 5;
				}
				else
				{
					g_Player[0].lives = 50;
				}
			}
			else if (cheat == Cheat::Yippee)
			{
				CheatYippee();
			}
		}
		if (g_SongTimer == '\0')
		{
			bVar1 = MapUpdate(0);
			if ((bVar1) && (g_LevelWon == 0))
			{
				g_LevelWon = 0x78;
				PlaySong(100);
				if (g_GameMode != GameMode::None)
				{
					if (g_GameMode < GameMode::SiblingRivalry)
					{
						g_EndAnim[0] = 3;
						g_EndAnim[1] = 3;
						g_EndAnimPumpkins = 4;
					}
					else if (g_GameMode == GameMode::SiblingRivalry)
					{
						if (g_Player[1].candles < g_Player[0].candles)
						{
							g_EndAnim[0] = 3;
							g_EndAnim[1] = 4;
							g_Player[0].score = g_Player[0].score + 1;
							g_RivalryWinner = 0;
						}
						else if (g_Player[0].candles < g_Player[1].candles)
						{
							g_EndAnim[1] = 3;
							g_EndAnim[0] = 4;
							g_Player[1].score = g_Player[1].score + 1;
							g_RivalryWinner = 1;
						}
						else
						{
							g_EndAnim[1] = 4;
							g_EndAnim[0] = 4;
							g_RivalryWinner = 2;
						}
						uVar2 = ConfigGetRivalryMatches();
						if ((g_Player[0].score == (uVar2 & 0xff)) ||
						    (uVar2 = ConfigGetRivalryMatches(), g_Player[1].score == (uVar2 & 0xff)))
						{
							g_RivalryHasWinner = true;
						}
						g_EndAnimPumpkins = 4;
					}
				}
			}
			if (g_LevelWon == 0)
			{
				g_RecordTimeFrames = g_RecordTimeFrames + 1;
			}
			GuysUpdate();
			ParticlesUpdate();
		}
		else
		{
			g_SongTimer = g_SongTimer + -1;
			if (g_SongTimer == '\0')
			{
				StopSong();
				PlaySong(int(g_GameMode) + 100);
			}
		}
		if ((g_LevelWon != 0) && (g_LevelWon = g_LevelWon - 1, g_LevelWon == 0))
		{
			if ((g_GameMode != GameMode::SiblingRivalry) && (g_Player[0].lives == 0))
			{
				g_RivalryHasWinner = true;
			}
			if ((g_RivalryHasWinner != false) && (g_IsTimeAttack == false))
			{
				GameExit();
				if (g_GameMode != GameMode::SiblingRivalry)
				{
					HighScoreInit();
					return true;
				}
				TitleInit();
				return true;
			}
			if (g_IsTimeAttack != false)
			{
				GameExit();
				if (g_Player[0].lives != 0)
				{
					HighScoreInit();
					return true;
				}
				TimeAttackInit();
				return true;
			}
			if (g_GameMode == GameMode::HappyFields)
			{
				g_MapNum = g_MapNum + 1;
				if ((g_MapNum == 0x15) ||
				    ((g_MapNum == 6 && (bVar1 = RegistrationCurrentlyValid(), !bVar1))))
				{
					GameExit();
					CongratsInit();
					return true;
				}
				GameInitHappy();
			}
			else if (g_GameMode == GameMode::DankDungeons)
			{
				if ((g_MapNum == 5) && (bVar1 = RegistrationCurrentlyValid(), !bVar1))
				{
					GameExit();
					CongratsInit();
					return true;
				}
				g_MapNum = g_MapNum + 1;
				GameInitDank();
			}
			else if (g_GameMode == GameMode::SiblingRivalry)
			{
				GameInitRivalry();
			}
		}
		if (g_SongTimer == '\0')
		{
			if (g_LevelIntroX < 800)
			{
				g_LevelIntroX = g_LevelIntroX + 0x14;
			}
		}
		else
		{
			g_LevelIntroX = g_LevelIntroX + 0x14;
			if (0x140 < g_LevelIntroX)
			{
				g_LevelIntroX = 0x140;
			}
		}
		return true;
	}
	bVar1 = PauseUpdate();
	if (bVar1)
	{
		return true;
	}
	GameExit();
	if (g_IsTimeAttack == false)
	{
		TitleInit();
		return true;
	}
	TimeAttackInit();
	return true;
}

void GameRender()
{
	bool bVar1;
	char buf[32];

	memcpy(mgl->GetScreen(), g_Background, 0x4b000);
	MapRender(false);
	GuysRender();
	DisplayListRender();
	ParticlesRender();
	StatusRender();
	if ((g_LevelWon != 0) && (g_GameMode != GameMode::None))
	{
		if (g_GameMode < GameMode::SiblingRivalry)
		{
			if (g_Player[0].lives == 0)
			{
				CenterPrintBright(0x142, 0xde, "GAME OVER", -0x20);
				CenterPrintMultiline(0x140, 0xdc, "GAME OVER");
			}
			else
			{
				CenterPrintBright(0x142, 0xca, "Level Complete!", -0x20);
				CenterPrintMultiline(0x140, 200, "Level Complete!");
			}
		}
		else if (g_GameMode == GameMode::SiblingRivalry)
		{
			if (g_RivalryHasWinner == false)
			{
				if (g_RivalryWinner == 0)
				{
					CenterPrintBright(0x142, 0xde, "Player 1 Wins!", -0x20);
					CenterPrintMultiline(0x140, 0xdc, "Player 1 Wins!");
				}
				else if (g_RivalryWinner == 1)
				{
					CenterPrintBright(0x142, 0xde, "Player 2 Wins!", -0x20);
					CenterPrintMultiline(0x140, 0xdc, "Player 2 Wins!");
				}
				else if (g_RivalryWinner == 2)
				{
					CenterPrintBright(0x142, 0xde, "It's a tie!", -0x20);
					CenterPrintMultiline(0x140, 0xdc, "It's a tie!");
				}
			}
			else if (g_RivalryWinner == 0)
			{
				CenterPrintBright(0x142, 0xde, "Player 1 is the CHAMPION!!", -0x20);
				CenterPrintMultiline(0x140, 0xdc, "Player 1 is the CHAMPION!!");
			}
			else if (g_RivalryWinner == 1)
			{
				CenterPrintBright(0x142, 0xde, "Player 2 is the CHAMPION!", -0x20);
				CenterPrintMultiline(0x140, 0xdc, "Player 2 is the CHAMPION!");
			}
		}
	}
	if (g_LevelIntroX < 800)
	{
		if (g_GameMode == GameMode::HappyFields)
		{
			CenterPrintBright(g_LevelIntroX + 2, 0xca, "Happy Fields", -0x20);
			CenterPrintMultiline(g_LevelIntroX, 200, "Happy Fields");
			sprintf(buf, "Level %02d", (unsigned int)g_MapNum);
			CenterPrintBright(g_LevelIntroX + 2, 0xe8, buf, -0x20);
			CenterPrintMultiline(g_LevelIntroX, 0xe6, buf);
			CenterPrintBright(g_LevelIntroX + 2, 0x11a, "Get Ready!", -0x20);
			CenterPrintMultiline(g_LevelIntroX, 0x118, "Get Ready!");
		}
		else if (g_GameMode == GameMode::DankDungeons)
		{
			CenterPrintBright(g_LevelIntroX + 2, 0xca, "Dank Dungeons", -0x20);
			CenterPrintMultiline(g_LevelIntroX, 200, "Dank Dungeons");
			sprintf(buf, "Level %02d", (unsigned int)g_MapNum);
			CenterPrintBright(g_LevelIntroX + 2, 0xe8, buf, -0x20);
			CenterPrintMultiline(g_LevelIntroX, 0xe6, buf);
			CenterPrintBright(g_LevelIntroX + 2, 0x11a, "Get Ready!", -0x20);
			CenterPrintMultiline(g_LevelIntroX, 0x118, "Get Ready!");
		}
		else if (g_GameMode == GameMode::SiblingRivalry)
		{
			CenterPrintBright(g_LevelIntroX + 2, 0xca, "Sibling Rivalry", -0x20);
			CenterPrintMultiline(g_LevelIntroX, 200, "Sibling Rivalry");
			sprintf(buf, "Round %02d", (unsigned int)g_RivalryRound);
			CenterPrintBright(g_LevelIntroX + 2, 0xe8, buf, -0x20);
			CenterPrintMultiline(g_LevelIntroX, 0xe6, buf);
			CenterPrintBright(g_LevelIntroX + 2, 0x11a, "Get Ready!", -0x20);
			CenterPrintMultiline(g_LevelIntroX, 0x118, "Get Ready!");
		}
	}
	bVar1 = IsPaused();
	if (bVar1)
	{
		PauseRender();
	}
	return;
}

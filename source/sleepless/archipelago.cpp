#include "archipelago.h"
#include "winpch.h"
#include <set>
#include <aegea.h>
#include "game.h"
#include "player.h"
#include "string_extras.h"
#include "particle.h"
#include "shop.h"
#include "goal.h"

namespace
{
	class Archipelago gArchipelagoManager;
	std::unique_ptr<ArchipelagoClient> ap;

#ifndef NDEBUG
	std::string serverAddress = "localhost:38281";
#else
	std::string serverAddress = "archipelago.gg:";
#endif
	std::string slotName;
	std::string password;

	constexpr int64_t BASE_ID = 10000;  // TODO
	constexpr int64_t AP_LOC_GLARCH = 4;
	constexpr int64_t AP_ITM_JOURNAL = 26;

	constexpr int MESSAGE_TIME = 30 * 5;
	int messageCooldown = MESSAGE_TIME;
	bool countdownDone = false;

	std::string saveName;

	constexpr byte ITM_JOURNAL = 150;
	constexpr byte ITM_FLAMEBRINGER = 153;
	constexpr byte ITM_LIGHTREAVER = 162;
	constexpr byte ITM_PLANETSMASHER = 163;
	constexpr byte ITM_SPARKTHROWER = 169;
	constexpr byte ITM_EARSPLITTER = 170;
	constexpr byte ITM_BONECRUSHER = 171;
	constexpr byte ITM_BRAIN_DETECTOR = 173;
	constexpr byte ITM_SOLAR_COLLECTOR = 174;
	constexpr byte ITM_FIRST_AID_KIT = 175;
	constexpr byte ITM_SOLID_SHIELD = 176;
	constexpr byte ITM_BLASTING_CAP = 177;
	constexpr byte ITM_BIONIC_ARM = 178;
	constexpr byte ITM_TRAINING_GUIDE = 180;
	constexpr byte ITM_ELECTROREEL = 186;

	const std::set<byte> replaceItems = {
		ITM_PANTS, ITM_HAMMERUP, ITM_REVERSE, ITM_REFLECT,
		ITM_BRAIN, ITM_CANDLE, ITM_KEY, ITM_KEYR, ITM_KEYG, ITM_KEYB,
		ITM_FLAMEBRINGER, ITM_LIGHTREAVER, ITM_PLANETSMASHER,
		ITM_SPARKTHROWER, ITM_EARSPLITTER, ITM_BONECRUSHER,
		ITM_BRAIN_DETECTOR, ITM_SOLAR_COLLECTOR, ITM_FIRST_AID_KIT,
		ITM_SOLID_SHIELD, ITM_BLASTING_CAP, ITM_BIONIC_ARM, ITM_TRAINING_GUIDE,
		ITM_JOURNAL, ITM_ELECTROREEL,
	};

	#include "archipelago_data.inl"
}

// ----------------------------------------------------------------------------

class Archipelago* Archipelago()
{
	return ap ? &gArchipelagoManager : nullptr;
}

std::string_view Archipelago::Status()
{
	return
		!ap ? "Not connected" :
		!ap->error_message().empty() ? ap->error_message() :
		!ap->is_active() ? ap->connection_status() :
		"Active";
}

const char* Archipelago::SaveName()
{
	return saveName.c_str();
}

byte Archipelago::ReplaceItemAppearance(byte item, byte map, byte x, byte y, byte select)
{
	if (replaceItems.find(item) == replaceItems.end())
	{
		return item;
	}

	// Handle the Sleepless Hollow well preview spot
	if (map == 1 && x == 41 && y == 61)
	{
		x = 60;
		y = 74;
	}

	auto iter = locations.find(std::make_tuple(map, select != 1 ? 255 : x, select != 1 ? select : y));
	if (iter == locations.end())
	{
		return ITM_BADCHINESE;  // Oh no
	}
	int64_t location = BASE_ID + iter->second;

	if (ap->location_is_checked(location))
	{
		// It got "picked up" externally to the game save, go invis.
		return ITM_NONE;
	}

	if (auto item = ap->item_at_location(location))
	{
		if (item->player == ap->player_id())
		{
			auto iter = items.find(item->item - BASE_ID);
			if (iter != items.end())
			{
				return iter->second;
			}
		}
	}

	return ITM_LOONYKEY;
}

extern byte AllJournals(); // items.cpp
extern byte playerGlow; // player.cpp

// sideEffects=false means play sounds and particles only
// sideEffects=true means also show message and swap current selected hammer
static void VanillaGetItemFx(int64_t item_id, bool sideEffects, int x, int y)
{
	item_id -= BASE_ID;
	auto iter = items.find(item_id);
	if (iter == items.end())
	{
		// Don't know how to handle this. Probably bad.
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "archipelago VanillaGetItemFx(%lld) not in list", (long long)item_id);
		return;
	}

	MakeCustomSound(GetItem(iter->second)->sound,(x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,SND_CUTOFF,1000);
	if(sideEffects && GetItem(iter->second)->msg[0])
		NewMessage(GetItem(iter->second)->msg,75,0);

	switch (iter->second)
	{
		case ITM_FLAMEBRINGER:
		case ITM_LIGHTREAVER:
		case ITM_PLANETSMASHER:
		case ITM_SPARKTHROWER:
		case ITM_EARSPLITTER:
		case ITM_BONECRUSHER:
			if (sideEffects)
			{
				// Sets weaponLvl=1 immediately... think of it as network prediction.
				PlayerGetWeapon(GetItem(iter->second)->effectAmt, goodguy->mapx, goodguy->mapy);
			}
			else
			{
				playerGlow=127;
				HealRing(7,goodguy->x,goodguy->y,FIXAMT*20,255,5);
			}
			break;
		case ITM_ELECTROREEL:
			if (sideEffects)
			{
				player.journal[20] = 1; // prediction
				player.jpage = 20;
				if(AllJournals())
					CompleteGoal(17);
				PauseGame();
			}
			break;
		case ITM_JOURNAL:
			if (sideEffects && AP_ITM_JOURNAL <= item_id && item_id <= AP_ITM_JOURNAL + 19)
			{
				player.journal[item_id - AP_ITM_JOURNAL + 1] = 1; // prediction
				player.jpage = item_id - AP_ITM_JOURNAL+ 1;
				if(AllJournals())
					CompleteGoal(17);
				PauseGame();
			}
			break;
		default:
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "archipelago VanillaGetItemFx(%lld) -> %d not in switch", (long long)item_id, (int)iter->second);
			break;
		// Everything else just needs sound + message.
	}
}

static void VanillaGetLocationFx(int64_t location, int x, int y)
{
	ColorRing(8,(x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
				(y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,FIXAMT*5,16,4);

	auto item = ap->item_at_location(location);
	if (!item || item->player != ap->player_id())
	{
		// It's for someone else, don't play any prediction fx.
		return;
	}

	VanillaGetItemFx(item->item, true, x, y);
}

bool Archipelago::ReplaceItemEffect(byte item, byte map, byte x, byte y, byte select)
{
	if (replaceItems.find(item) == replaceItems.end())
	{
		return false;
	}

	// Handle the Sleepless Hollow well preview spot
	if (map == 1 && x == 41 && y == 61)
	{
		return true;
	}

	auto iter = locations.find(std::make_tuple(map, select != 1 ? 255 : x, select != 1 ? select : y));
	if (iter == locations.end())
	{
		return ITM_BADCHINESE;  // Oh no
	}
	int64_t location = BASE_ID + iter->second;

	if (ap->check_location(location))
	{
		VanillaGetLocationFx(location, x, y);
	}
	return true;
}

void Archipelago::GetGlarch()
{
	if (ap->check_location(BASE_ID + AP_LOC_GLARCH))
	{
		VanillaGetLocationFx(BASE_ID + AP_LOC_GLARCH, goodguy->mapx, goodguy->mapy);
	}
}

void Archipelago::Update()
{
	if (!ap)
	{
		return;
	}

	ap->update();

	// Handle reconnection by refreshing stuff.
	if (ap->pop_connected())
	{
		ap->scout_locations();

		std::string newSaveName = "";
		newSaveName += ap->seed_name();
		newSaveName += "_";
		newSaveName += std::to_string(ap->player_id());
		if (saveName != newSaveName)
		{
			saveName = newSaveName;

			// Copy control and sound settings to avoid interruptions.
			// Controls are not even configurable from within AP.
			byte control[2][6];
			byte joyCtrl[2];
			byte sound;
			byte music;
			memcpy(control, profile.control, 2 * 6);
			memcpy(joyCtrl, profile.joyCtrl, 2);
			sound = profile.sound;
			music = profile.music;

			LoadProfile("Archipelago");

			memcpy(profile.control, control, 2 * 6);
			memcpy(profile.joyCtrl, joyCtrl, 2);
			profile.sound = sound;
			profile.music = music;
			ApplyControlSettings();
			JamulSoundVolume(profile.sound);
			SetMusicVolume(profile.music);

			byte map = GetWorldProgress("hollow.shw")->levelOn;
			LoadWorld(&curWorld, "worlds/hollow.shw");
			FreeWorld(&curWorld);  // Load+Free leaves items intact for GetItem
			InitPlayer(map, "hollow.shw");
			LoadState(map, 3);
		}

		// Reset progressive hammers, pants, brains, candles, and yellow keys
		player.hammers = 0;
		player.hamSpeed = 16;
		player.brains = 0;
		player.candles = 0;
		player.keys[0] = (byte)(256 - player.coins);
	}

	// Items
	while (auto item = ap->pop_received_item())
	{
		int64_t item_id = item->item - BASE_ID;
		auto iter = items.find(item_id);
		if (iter == items.end())
		{
			// Don't know how to handle this.
			continue;
		}
		switch (iter->second)
		{
			case ITM_PANTS:
				if(player.hamSpeed>4)
					player.hamSpeed-=2;
				else if(player.hamSpeed>0)
					player.hamSpeed--;
				break;
			case ITM_HAMMERUP:
				player.hammers++;
				break;
			case ITM_FLAMEBRINGER:
			case ITM_LIGHTREAVER:
			case ITM_PLANETSMASHER:
			case ITM_SPARKTHROWER:
			case ITM_EARSPLITTER:
			case ITM_BONECRUSHER: {
				byte wpn = GetItem(iter->second)->effectAmt;
				if (!player.weaponLvl[wpn-1])
					player.weaponLvl[wpn-1] = 1;
				break;
			}
			case ITM_BRAIN:
				player.brains++;
				break;
			case ITM_CANDLE:
				player.candles++;
				break;
			case ITM_BIONIC_ARM:
				PlayerPowerup(PU_BIONIC);
				break;
			case ITM_BLASTING_CAP:
				PlayerPowerup(PU_BLASTING);
				break;
			case ITM_REVERSE:
				PlayerPowerup(PU_REVERSE);
				break;
			case ITM_REFLECT:
				PlayerPowerup(PU_REFLECT);
				break;
			case ITM_SOLID_SHIELD:
				PlayerPowerup(PU_SHIELD);
				break;
			case ITM_TRAINING_GUIDE:
				PlayerPowerup(PU_TRAINING);
				break;
			case ITM_SOLAR_COLLECTOR:
				PlayerPowerup(PU_SOLAR);
				break;
			case ITM_FIRST_AID_KIT:
				PlayerPowerup(PU_FIRSTAID);
				break;
			case ITM_BRAIN_DETECTOR:
				PlayerPowerup(PU_BRAIN);
				break;
			case ITM_KEYR:
				player.keys[1] = 1;
				break;
			case ITM_KEYG:
				player.keys[2] = 1;
				break;
			case ITM_KEYB:
				player.keys[3] = 1;
				break;
			case ITM_KEY:
				player.keys[0]++;
				break;
			case ITM_ELECTROREEL:
				player.ability[ABIL_FISH] = 1;
				player.journal[20] = 1;
				if(AllJournals())
					CompleteGoal(17);
				break;
			case ITM_JOURNAL:
				if (AP_ITM_JOURNAL <= item_id && item_id <= AP_ITM_JOURNAL + 19)
				{
					player.journal[item_id - AP_ITM_JOURNAL + 1] = 1;
					if(AllJournals())
						CompleteGoal(17);
				}
				break;
		}
	}

	// Messages
	if (GetGameMode() == GAMEMODE_PLAY)
	{
		if (messageCooldown > 0)
		{
			messageCooldown--;
		}
		else if (auto message = ap->pop_message())
		{
			if (message->type == ArchipelagoClient::Message::ItemSend)
			{
				if (message->item.player == ap->player_id() && message->item.location == BASE_ID + AP_LOC_GLARCH)
				{
					// Glarch gets a unique message.
					std::string msg = "It had swallowed a ";
					msg += ap->item_name(*message);
					msg += "!";
					NewMessage(msg.c_str(),90,1);
					messageCooldown = MESSAGE_TIME;
				}
				else
				{
					if (message->receiving == ap->player_id())
					{
						if (message->item.player != ap->player_id())
						{
							// Got [item] from [player].
							std::string text = "Got ";
							text += ap->item_name(*message);
							text += " from ";
							text += ap->slot_player_alias(message->item.player);
							NewMessage(text.c_str(), MESSAGE_TIME, 1);
							messageCooldown = MESSAGE_TIME;

							// Play sound/particle FX based on item, but override the message.
							VanillaGetItemFx(message->item.item, false, goodguy->mapx, goodguy->mapy);
						}
						// If player == us, we showed a vanilla message already
						// as we check_location'd.
					}
					else if (message->item.player == ap->player_id())
					{
						// Sent [item] to [player].
						std::string text = "Sent ";
						text += ap->item_name(*message);
						text += " to ";
						text += ap->slot_player_alias(message->receiving);
						NewMessage(text.c_str(), MESSAGE_TIME, 1);
						MakeNormalSound(SND_MESSAGE);
						messageCooldown = MESSAGE_TIME;
					}
				}
			}
			else if (message->type == ArchipelagoClient::Message::Countdown)
			{
				if (message->countdown == 0)
				{
					countdownDone = true;
				}
			}
		}
	}
}

// ----------------------------------------------------------------------------

TASK(void)
ArchipelagoMenu(MGLDraw* mgl)
{
	bool running = true;

	mgl->LastKeyPressed();
	byte oldc = ~0;
	byte cursor = 1;
	std::string* typing = nullptr;

	// ------------------------------------------------------------------------
	// Init
	std::string oldProfile = profile.name;
	saveName = "error";
	InitMessage();

	while (running)
	{
		// --------------------------------------------------------------------
		// Update
		bool playOk = ap && ap->is_active() && ap->error_message().empty();
		// Handle disconnects by bumping cursor back up.
		while (cursor >= 5 && cursor < 7 && !playOk)
		{
			--cursor;
		}
		int typingY = -1;

		byte c = GetControls() | GetArrows();
		byte k = mgl->LastKeyPressed();
		if (!typing)
		{
			if (c & ~oldc & CONTROL_UP)
			{
				if (cursor == 0)
				{
					cursor = 5;
				}
				else
				{
					--cursor;
				}
				while (cursor >= 5 && cursor < 6 && !playOk)
				{
					--cursor;
				}
			}
			if (c & ~oldc & CONTROL_DN)
			{
				++cursor;
				while (cursor >= 5 && cursor < 6 && !playOk)
				{
					++cursor;
				}
				if (cursor > 5)
				{
					cursor = 0;
				}
			}
			if (c & ~oldc & (CONTROL_B1 | CONTROL_B2 | CONTROL_B3))
			{
				if (cursor == 0)
				{
					running = false;
				}
				else if (cursor == 1)
				{
					typing = &serverAddress;
					MakeNormalSound(SND_MENUSELECT);
				}
				else if (cursor == 2)
				{
					typing = &slotName;
					MakeNormalSound(SND_MENUSELECT);
				}
				else if (cursor == 3)
				{
					typing = &password;
					MakeNormalSound(SND_MENUSELECT);
				}
				else if (cursor == 4)
				{
					static ArchipelagoCache::FileSystem cache { "appdata/archipelago/" };
					ap = std::make_unique<ArchipelagoClient>("Sleepless Hollow", serverAddress, slotName, password);
					ap->use_cache(&cache);
				}
				else if (cursor == 5)
				{
					AWAIT PlayWorld(mgl, "hollow.shw");
					oldc = ~0;
					continue;
				}
			}
			if (k == SDLK_ESCAPE)
			{
				running = false;
			}
		}
		else
		{
			if (k == SDLK_ESCAPE || k == SDLK_RETURN) // ESC key
			{
				k = 0;
				typing = nullptr;
				oldc = ~0;
				mgl->LastKeyPressed();
				MakeNormalSound(SND_MENUSELECT);
			}
			else if (k == SDLK_BACKSPACE && !typing->empty())
			{
				typing->pop_back();
			}
			else if (k >= ' ' && k <= '~')
			{
				*typing += k;
			}

			typingY = typing ? GetStrLength(typing == &password ? std::string(password.size(), '*') : *typing, 1) : 0;
		}
		oldc = c;

		countdownDone = false;
		gArchipelagoManager.Update();
		if (countdownDone)
		{
			AWAIT PlayWorld(mgl, "hollow.shw");
			oldc = ~0;
			continue;
		}

		// --------------------------------------------------------------------
		// Render
		mgl->ClearScreen();
		CenterPrint(320,2,"Archipelago Randomizer",0,0);

		DrawFillBox(250-80,40-3+20*(cursor+1+(cursor>4)),390+80,40+17-3+20*(cursor+1+(cursor>4)),10);

		CenterPrint(320,3*20,"Exit To Main Menu",0,1);

		RightPrint(320,4*20,"Server address: ",0,1);
		Print(320,4*20,serverAddress,0,1);
		RightPrint(320,5*20,"Slot name: ",0,1);
		Print(320,5*20,slotName,0,1);
		RightPrint(320,6*20,"Password: ",0,1);
		Print(320,6*20,std::string(password.size(), '*'),0,1);
		if (typing && typingY >= 0)
		{
			Print(320 + typingY, (cursor + 3)*20, "_", 0, 1);
		}

		CenterPrint(320,7*20,"Connect",0,1);

		if (playOk) CenterPrint(320,9*20,"Play",0,1);

		CenterPrint(320, 14*20, gArchipelagoManager.Status(), 0, 1);

		// --------------------------------------------------------------------
		AWAIT mgl->Flip();
		if (!mgl->Process())
		{
			break;
		}
	}

	// ------------------------------------------------------------------------
	// Exit
	ap.reset();
	LoadProfile(oldProfile.c_str());
}

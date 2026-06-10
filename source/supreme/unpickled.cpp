#include "unpickled.h"
#include <span>
#include "editor.h"
#include "mgldraw.h"
#include "config.h"
#include "winpch.h"
#include "vanilla_extract.h"
#include "leveldialog.h"
#include "appdata.h"
#include "sound.h"
#include "owned_mixer.h"
#include <SDL3_image/SDL_image.h>
#include "log.h"

#ifdef __EMSCRIPTEN__
void UnpickledMain()
{
}
#else

static bool CheckImageFile(owned::SDL_IOStream stream)
{
	owned::SDL_Surface b { IMG_Load_IO(stream.release(), true) };
	return b != nullptr;
}

static bool CheckAudioFile(owned::SDL_IOStream stream)
{
	owned::MIX_AudioDecoder decoder = owned::MIX_CreateAudioDecoder_IO(stream.get(), 0);
	if (!decoder)
	{
		return false;
	}

	SDL_AudioSpec spec = {
		.format = SDL_AUDIO_S16LE,
		.channels = 2,
		.freq = 44100,
	};

	std::vector<uint8_t> pcm;
	while (true)
	{
		size_t start = pcm.size();
		size_t end = (start + 4096) * 2;
		pcm.resize(end);
		int read = MIX_DecodeAudio(decoder.get(), &pcm[start], end - start, &spec);
		if (read < 0)
		{
			return false;
		}
		pcm.resize(start + read);
		if (read == 0)
		{
			break;
		}
	}

	for (size_t i = 0; i < pcm.size(); ++i)
	{
		if (pcm[i])
		{
			return true;
		}
	}
	SDL_SetError("Decoded to silence");
	return false;
}

void UnpickledMain()
{
	MGLDraw* mgl = GetDisplayMGL();
	mgl->ClearScreen();
	PALETTE pal;
	mgl->LoadBMP("graphics/title.bmp", pal);

	std::string fullname, pngName;

	InitGuys(config.numGuys + 1);
	InitBullets();
	config.camera = false;

	dword start = timeGetTime();

	int bmpTotal = 0, bmpBad = 0, musTotal = 0, musBad = 0, sndTotal = 0, sndBad = 0;

	LogDebug("Checking images");
	for (const auto &fname : ListDirectory("user", ".bmp"))
	{
		++bmpTotal;
		fullname = "user/";
		fullname.append(fname);
		if (!CheckImageFile(AppdataOpen(fullname.c_str())))
		{
			LogError("%s: bad image: %s", fullname.c_str(), SDL_GetError());
			++bmpBad;
		}

		if (!mgl->Process()) break;
	}

	LogDebug("Checking music");
	for (const auto &fname : ListDirectory("music", ".ogg"))
	{
		++musTotal;
		fullname = "music/";
		fullname.append(fname);
		if (!CheckAudioFile(AppdataOpen(fullname.c_str())))
		{
			LogError("%s: bad audio: %s", fullname.c_str(), SDL_GetError());
			++musBad;
		}

		if (!mgl->Process()) break;
	}

	LogDebug("Checking worlds");
	for (const auto &fname : ListDirectory("worlds", ".dlw"))
	{
		fullname = "worlds/";
		fullname.append(fname);
		if (!LoadWorld(&curWorld, fullname.c_str()))
		{
			LogError("bad LoadWorld(%s)", fullname.c_str());
			continue;
		}
		SetCurrentTilegfx(&curWorld.tilegfx);

		int num = GetNumCustomSounds();
		for (int i = 0; i < num; ++i)
		{
			// Exempt dummy rainbow.dlw sounds.
			if (GetSoundInfo(CUSTOM_SND_START)->theme == 0)
				continue;

			++sndTotal;
			std::span<const byte> bytes = GetCustomSound(i);
			if (!CheckAudioFile(owned::SDL_IOFromConstMem(bytes.data(), bytes.size())))
			{
				LogError("%s: custom sound %d \"%s\": bad audio: %s", fname.c_str(), i, GetSoundInfo(CUSTOM_SND_START + i)->name, SDL_GetError());
				++sndBad;
			}
		}

		for (int i = 0; i < curWorld.numMaps; ++i)
		{
			curMap=new Map(curWorld.map[i]);

			player.boredom=0;
			player.hammers=0;
			player.clock=0;
			player.playAs=0;
			player.worldProg = new worldData_t;
			player.worldProg->levels = 0;
			memset(player.worldProg->var, 0, sizeof(player.worldProg->var));
			player.vehicle=0;
			SetPlayerStart(-1, -1);

			curMap->Init(&curWorld);
			// Skip light fade-in
			for (int y = 0; y < curMap->height; ++y)
				for (int x = 0; x < curMap->width; ++x)
					curMap->GetTile(x, y)->templight = curMap->GetTile(x, y)->light;

			if (curMap->flags & (MAP_TORCHLIT | MAP_WELLLIT))
			{
				// Let torch emit from player
				for (int i = 0; i < 60; ++i)
					goodguy->Update(curMap, &curWorld);
				// And light up those guys
				EditorUpdateGuys(curMap);
			}

			// Focus camera on Bouapha, clamped to map boundaries
			PutCamera(goodguy->x, goodguy->y);
			UpdateCamera(goodguy->x>>FIXSHIFT, goodguy->y>>FIXSHIFT, 0, 0, curMap);

			// Run one frame of specials so gfx, recolors, and so on can appear.
			GetSpecialsFromMap(curMap->special);
			CheckSpecials(curMap);
			mgl->SetPalette(pal);  // Fix up in case the above showed a pic.

			mgl->ClearScreen();
			RenderGuys(true);
			RenderItAll(&curWorld, curMap, MAP_SHOWWALLS | MAP_SHOWLIGHTS | MAP_SHOWBADGUYS | MAP_SHOWPICKUPS | MAP_SHOWOTHERITEMS);

			pngName = "./appdata/unpickled/worlds/";
			pngName.append(fname);
			pngName.append("/pic_");
			string_appendf(&pngName, "%d", i);
			pngName.append(".png");
			vanilla::mkdir_parents(pngName);
			mgl->SavePNG(pngName.c_str());

			dword now = timeGetTime();
			if (now - start > 1000)
			{
				start = now;
				mgl->Flip();
			}

			mgl->RealizePalette();  // Fix up in case the above set underwater or underlava palette.
			RenderLevelDialogZoom(mgl, &curWorld, i);

			pngName = "./appdata/unpickled/worlds/";
			pngName.append(fname);
			pngName.append("/mini_");
			string_appendf(&pngName, "%d", i);
			pngName.append(".png");
			vanilla::mkdir_parents(pngName);
			mgl->SavePNG(pngName.c_str());
			mgl->ResizeBuffer(SCRWID, SCRHEI);

			delete player.worldProg;
			delete curMap;

			if (!mgl->Process()) break;
		}

		FreeWorld(&curWorld);

		if (!mgl->Process()) break;
	}

	LogDebug("Images: %d bad / %d total", bmpBad, bmpTotal);
	LogDebug("Music: %d bad / %d total", musBad, musTotal);
	LogDebug("Sound: %d bad / %d total", sndBad, sndTotal);
}

#endif

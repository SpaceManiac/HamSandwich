#include "unpickled.h"
#include "editor.h"
#include "mgldraw.h"
#include "config.h"
#include "winpch.h"
#include "vanilla_extract.h"
#include "leveldialog.h"
#include "appdata.h"

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

	std::vector<std::string> vec = ListDirectory("worlds", ".dlw");
	for (const auto &fname : vec)
	{
		fullname = "worlds/";
		fullname.append(fname);
		if (!LoadWorld(&curWorld, fullname.c_str()))
		{
			fprintf(stderr, "bad LoadWorld(%s)\n", fullname.c_str());
			//continue;
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
			pngName.append(std::to_string(i));
			pngName.append(".png");
			vanilla::mkdir_parents(pngName);
			mgl->SavePNG(pngName.c_str());

			dword now = timeGetTime();
			if (now - start > 1000)
			{
				start = now;
				GetDisplayMGL()->Flip();
			}

			mgl->RealizePalette();  // Fix up in case the above set underwater or underlava palette.
			InitLevelDialog(&curWorld, i);
			RenderLevelDialogZoom(mgl);
			pngName = "./appdata/unpickled/worlds/";
			pngName.append(fname);
			pngName.append("/mini_");
			pngName.append(std::to_string(i));
			pngName.append(".png");
			vanilla::mkdir_parents(pngName);
			mgl->SavePNG(pngName.c_str());
			mgl->ResizeBuffer(SCRWID, SCRHEI);

			delete player.worldProg;
			delete curMap;

			if (!mgl->Process()) break;
		}
		if (!mgl->Process()) break;
	}
}

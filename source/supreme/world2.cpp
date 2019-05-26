#include "world.h"
#include "title.h"
#include "hamworld.h"
#include "monster.h"
#include <memory>

struct big_savetile
{
	size_t floor, wall, item;
	char light;
};

static void SaveItem(hamworld::Section *f, item_t *item)
{
	f->write_string(item->name);

	f->stream.put(item->xofs);
	f->stream.put(item->yofs);
	f->write_varint(item->sprNum);
	f->stream.put(item->bright);

	// Convert single replacement to full mapping.
	byte colors[8] = {0, 1, 2, 3, 4, 5, 6, 7};
	colors[item->fromColor] = item->toColor;
	f->stream.write((char*) colors, 8);

	f->write_varint(item->rarity);
	f->write_varint(item->flags);  // no extension flags
	f->write_varint(item->theme);
	f->write_varint(item->trigger);
	f->write_varint(item->effect);
	f->write_varint(item->effectAmt);
	f->write_varint(item->sound);
	f->write_string(item->msg);
}

static void LoadItem(hamworld::Section *f, item_t *item)
{
	f->read_string(item->name);

	item->xofs = f->stream.get();
	item->yofs = f->stream.get();
	item->sprNum = f->read_varint();
	item->bright = f->stream.get();

	// Convert full mapping back to single replacement.
	// Lossy if the mapping involves more than one replacement.
	byte colors[8];
	f->stream.read((char*) colors, 8);
	for (int i = 0; i < 8; ++i)
		if (colors[i] != i)
		{
			item->fromColor = i;
			item->toColor = colors[i];
			break;
		}

	item->rarity = f->read_varint();
	item->flags = f->read_varint();  // ignore extension flags
	item->theme = f->read_varint();
	item->trigger = f->read_varint();
	item->effect = f->read_varint();
	item->effectAmt = f->read_varint();
	item->sound = f->read_varint();
	f->read_string(item->msg);
}

static void SaveMapMons(hamworld::Section *f, mapBadguy_t *mons)
{
	f->write_varint(mons->x);
	f->write_varint(mons->y);
	f->write_varint(mons->type);
	f->write_varint(mons->item);
	f->write_varint(0);  // no extension flags
}

static void LoadMapMons(hamworld::Section *f, mapBadguy_t *mons)
{
	mons->x = f->read_varint();
	mons->y = f->read_varint();
	mons->type = f->read_varint();
	mons->item = f->read_varint();
	f->read_varint();  // ignore extension flags
}

static void SaveMapTile(hamworld::Section *f, big_savetile *t)
{
	f->write_varint(t->floor);
	f->write_varint(t->wall);
	f->write_varint(t->item);
	f->stream.put(t->light);
	f->write_varint(0);  // no extension flags
}

static void LoadMapTile(hamworld::Section *f, big_savetile *t)
{
	t->floor = f->read_varint();
	t->wall = f->read_varint();
	t->item = f->read_varint();
	t->light = f->stream.get();
	f->read_varint();  // ignore extension flags
}

static void SaveMapData(hamworld::Section *f, Map *map)
{
	std::vector<big_savetile> mapCopy(map->width * map->height);
	for (size_t i = 0; i < mapCopy.size(); ++i)
	{
		mapCopy[i].floor = map->map[i].floor;
		mapCopy[i].wall = map->map[i].wall;
		mapCopy[i].item = map->map[i].item;
		mapCopy[i].light = map->map[i].light;
	}

	char writeRun;
	size_t runStart = 0;
	size_t sameStart = 0;
	size_t sameLength = 1;
	size_t diffLength = 1;
	size_t pos = 1;
	big_savetile *src = &mapCopy[0];

	while(pos < mapCopy.size())
	{
		big_savetile *m = &mapCopy[pos];
		if(!memcmp(m, src, sizeof(big_savetile)))	// the tiles are identical
		{
			sameLength++;
			if(sameLength==128)	// max run length is 127
			{
				// write out the run, and start a new run with this last one
				writeRun=-127;
				f->stream.put(writeRun);
				SaveMapTile(f, src);
				sameStart=pos;
				runStart=pos;
				sameLength=1;
				diffLength=1;
			}
			else if(sameLength==2 && runStart!=sameStart)	// 2 in a row the same, time to start this as a same run
			{
				writeRun=sameStart-runStart;
				f->stream.put(writeRun);
				for (size_t i = runStart; i < sameStart; ++i)
					SaveMapTile(f, &mapCopy[i]);
				runStart=sameStart;
				diffLength=1;
			}
		}
		else // this one is different
		{
			if(sameLength<2)
			{
				// just continuing the non-same run
				sameLength=1;
				sameStart=pos;
				src=m;
				diffLength++;
				if(diffLength==128)	// max run length is 127
				{
					writeRun=127;
					f->stream.put(writeRun);
					for (size_t i = runStart; i < runStart + 127; ++i)
						SaveMapTile(f, &mapCopy[i]);
					diffLength=1;
					runStart=pos;
				}
			}
			else	// end a same run, then start a diff run
			{
				writeRun=-sameLength;
				f->stream.put(writeRun);
				SaveMapTile(f, src);
				sameStart=pos;
				runStart=pos;
				sameLength=1;
				diffLength=1;
				src=&mapCopy[pos];
			}
		}

		pos++;
	}

	// write out the final leftovers
	if(sameLength>1)
	{
		// final run is same run
		writeRun=-sameLength;
		f->stream.put(writeRun);
		SaveMapTile(f, src);
	}
	else
	{
		// either it's one lonely unique tile, or a different run, either way...
		writeRun=diffLength;
		f->stream.put(writeRun);
		for (size_t i = runStart; i < runStart + diffLength; ++i)
			SaveMapTile(f, &mapCopy[i]);
	}
}

static void LoadMapData(hamworld::Section *f, Map *map)
{
	std::vector<big_savetile> mapCopy(map->width * map->height);

	size_t pos=0;
	while(pos< mapCopy.size())
	{
		char readRun = f->stream.get();
		if(readRun<0)	// repeat run
		{
			readRun=-readRun;
			LoadMapTile(f, &mapCopy[pos]);
			for(int i=1;i<readRun;i++)
				memcpy(&mapCopy[pos+i],&mapCopy[pos],sizeof(big_savetile));
			pos+=readRun;
		}
		else	// unique run
		{
			for(int i=0; i < readRun; ++i)
				LoadMapTile(f, &mapCopy[pos + i]);
			pos+=readRun;
		}
	}

	for(size_t pos=0; pos<mapCopy.size(); pos++)
	{
		map->map[pos].floor=mapCopy[pos].floor;
		map->map[pos].wall=mapCopy[pos].wall;
		map->map[pos].item=mapCopy[pos].item;
		map->map[pos].light=mapCopy[pos].light;
		map->map[pos].select=1;
		map->map[pos].opaque=0;
		map->map[pos].templight=0;
	}
}

byte Ham_SaveWorld(world_t* world, const char *fname)
{
	printf("Ham_SaveWorld(%s)\n", fname);

	// Prepare custom item table
	hamworld::Section item_definitions;
	item_definitions.write_varint(NUM_ORIGINAL_ITEMS);
	item_definitions.write_varint(NumItems() - NUM_ORIGINAL_ITEMS);
	for (int i = NUM_ORIGINAL_ITEMS; i < NumItems(); ++i)
	{
		item_definitions.write_string("");  // savename not yet implemented
		SaveItem(&item_definitions, GetItem(i));
	}

	hamworld::Section rle_tilegfx;
	rle_tilegfx.write_varint(world->numTiles);
	SaveTiles(rle_tilegfx.stream);

	hamworld::Section terrain;
	terrain.write_varint(world->numTiles);
	for (int i = 0; i < world->numTiles; ++i)
	{
		terrain_t t = world->terrain[i];
		terrain.write_varint(t.flags);  // no extension flags
		terrain.write_varint(t.next);
	}

	std::vector<hamworld::Section> maps;
	for (int i = 0; i < world->numMaps; ++i)
	{
		Map* map = world->map[i];

		hamworld::Section mapsec;
		mapsec.write_varint(i);  // map ID
		mapsec.write_varint(map->width);
		mapsec.write_varint(map->height);
		mapsec.write_string(map->name);
		mapsec.write_string(map->song);
		mapsec.stream.write((const char*) &map->itemDrops, 2);
		mapsec.write_varint(map->numBrains);
		mapsec.write_varint(map->numCandles);
		mapsec.write_varint(map->flags);  // no extension flags

		// badguys
		size_t badguy_count = 0;
		for (size_t i = 0; i < MAX_MAPMONS; ++i)
			if (map->badguy[i].type)
				++badguy_count;
		mapsec.write_varint(badguy_count);
		for (size_t i = 0; i < MAX_MAPMONS; ++i)
		{
			if (!map->badguy[i].type)
				continue;
			SaveMapMons(&mapsec, &map->badguy[i]);
		}

		// specials

		// map data
		SaveMapData(&mapsec, map);
		maps.push_back(std::move(mapsec));
	}

	hamworld::Save save(fname);
	save.header(world->author, world->map[0]->name, "Supreme with Cheese " VERSION_NO);

	if (NumItems() > NUM_ORIGINAL_ITEMS)
	{
		save.section("item_definitions", item_definitions.save());
	}
	//save.section("customsounds", "");

	save.section("rle_tilegfx", rle_tilegfx.save());

	save.section("terrain", terrain.save());

	for (hamworld::Section& map : maps)
	{
		save.section("map", map.save());
	}

	return 1;
}

byte Ham_LoadWorld(world_t* world, const char *fname)
{
	printf("Ham_LoadWorld(%s)\n", fname);
	hamworld::Load load(fname);

	std::string app;
	if (!load.header(world->author, nullptr, &app))
	{
		printf("bad header\n");
		return 0;
	}
	printf("author: %s\n", world->author);
	printf("app: %s\n", app.c_str());

	memset(world->map, 0, sizeof(world->map));

	ExitItems();
	InitItems();

	world->numMaps = 0;

	hamworld::Section section;
	std::string section_name;
	while (load.section(&section_name, &section) && !section_name.empty())
	{
		printf("read section '%s'\n", section_name.c_str());
		if (section_name == "item_definitions")
		{
			size_t start = section.read_varint();
			if (start != NUM_ORIGINAL_ITEMS)
			{
				printf("error: item definition offest NYI (expected %d, got %d)\n", NUM_ORIGINAL_ITEMS, start);
				return false;
			}
			size_t item_count = section.read_varint();
			for (size_t i = 0; i < item_count; ++i)
			{
				section.read_string(nullptr);  // ignore savename

				int new_item = NewItem();
				LoadItem(&section, GetItem(new_item));
			}
		}
		else if (section_name == "rle_tilegfx")
		{
			SetNumTiles(section.read_varint());
			LoadTiles(section.stream);
		}
		else if (section_name == "terrain")
		{
			world->numTiles = section.read_varint();
			for (size_t i = 0; i < world->numTiles; ++i)
			{
				world->terrain[i].flags = section.read_varint();  // no extension flags
				world->terrain[i].next = section.read_varint();
			}
		}
		else if (section_name == "map")
		{
			Map *map = world->map[world->numMaps] = new Map(0, "");
			section.read_varint();  // skip uid
			map->Resize(section.read_varint(), section.read_varint());
			section.read_string(map->name);
			section.read_string(map->song);
			map->flags = section.read_varint();  // ignore extension flags
			section.stream.read((char*) &map->itemDrops, 2);
			map->numBrains = section.read_varint();
			map->numCandles = section.read_varint();

			memset(map->badguy, 0, sizeof(mapBadguy_t) * MAX_MAPMONS);
			size_t badguy_count = section.read_varint();
			for (size_t i = 0; i < badguy_count; ++i)
				LoadMapMons(&section, &map->badguy[i]);

			LoadMapData(&section, map);
			world->numMaps++;
		}
		else
		{
			printf("bad section: %s\n", section_name.c_str());
			return 0;
		}
	}

	return 1;
}

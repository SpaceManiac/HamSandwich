#include "world.h"
#include "title.h"
#include "hamworld.h"
#include "sound.h"
#include "log.h"

#define APPNAME "Supreme with Cheese " VERSION_NO

struct big_savetile
{
	size_t floor, wall, item;
	char light;
};

static void SaveItem(hamworld::Section *f, const item_t *item)
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
	f->write_varint(item->flags);
	f->write_varint(item->theme);
	f->write_varint(item->trigger);
	f->write_varint(item->effect);
	f->write_varint(item->effectAmt);
	f->write_varint(item->sound);
	f->write_string(item->msg);
	f->write_varint(0);  // no extension flags
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
	item->flags = f->read_varint();
	item->theme = f->read_varint();
	item->trigger = f->read_varint();
	item->effect = f->read_varint();
	item->effectAmt = f->read_varint();
	item->sound = f->read_varint();
	f->read_string(item->msg);
	f->read_varint();  // ignore extension flags
}

static void SaveSound(hamworld::Section *f, const soundDesc_t *desc, size_t len, const byte *data)
{
	f->write_string(desc->name);
	f->write_varint(desc->theme);
	f->write_string({ (char*) data, len });
	f->write_varint(0);  // no extension flags
}

soundDesc_t *AddCustomSound(byte *, size_t);

static void LoadSound(hamworld::Section *f)
{
	std::string name;
	f->read_string(&name);
	f->read_varint();  // ignore theme

	size_t size = f->read_varint();
	byte *data = (byte *) malloc(size);
	f->stream.read((char*) data, size);

	soundDesc_t *desc = AddCustomSound(data, size);
	if (desc)
		StringDestination(desc->name).assign(name);

	f->read_varint();  // ignore extension flags
}

static void SaveMapMons(hamworld::Section *f, const mapBadguy_t *mons)
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

static void SaveMapSpecial(hamworld::Section *f, const special_t *spcl)
{
	f->write_varint(spcl->x);
	f->write_varint(spcl->y);
	f->write_varint(spcl->uses);

	size_t trigger_count = 0;
	for (size_t i = 0; i < NUM_TRIGGERS; ++i)
		if (spcl->trigger[i].type)
			++trigger_count;

	f->write_varint(trigger_count);
	for (size_t i = 0; i < NUM_TRIGGERS; ++i)
	{
		const trigger_t *elem = &spcl->trigger[i];
		if (!elem->type)
			continue;
		f->write_varint(elem->flags);
		f->write_varint(elem->type);
		f->write_varint(elem->x);
		f->write_varint(elem->y);
		f->write_varint((uint32_t) elem->value);
		f->write_varint((uint32_t) elem->value2);

		// Pull trigger text from effect array.
		if (elem->type == TRG_EQUATION || elem->type == TRG_EQUVAR)
			f->write_string(spcl->effect[i].text);
		else
			f->write_string("");
	}

	size_t effect_count = 0;
	for (size_t i = 0; i < NUM_EFFECTS; ++i)
		if (spcl->effect[i].type)
			++effect_count;

	f->write_varint(effect_count);
	for (size_t i = 0; i < NUM_EFFECTS; ++i)
	{
		const effect_t *elem = &spcl->effect[i];
		if (!elem->type)
			continue;
		f->write_varint(elem->flags);
		f->write_varint(elem->type);
		f->write_varint(elem->x);
		f->write_varint(elem->y);
		f->write_varint((uint32_t) elem->value);
		f->write_varint((uint32_t) elem->value2);
		f->write_string(elem->text);
	}

	f->write_varint(0);  // no extension flags
}

static void LoadMapSpecial(hamworld::Section *f, special_t *spcl)
{
	spcl->x = f->read_varint();
	spcl->y = f->read_varint();
	spcl->uses = f->read_varint();

	size_t trigger_count = f->read_varint();
	for (size_t i = 0; i < trigger_count; ++i)
	{
		trigger_t *elem = &spcl->trigger[i];
		elem->flags = f->read_varint();
		elem->type = f->read_varint();
		elem->x = f->read_varint();
		elem->y = f->read_varint();
		elem->value = (uint32_t) f->read_varint();
		elem->value2 = (uint32_t) f->read_varint();

		// Put trigger text into effect array.
		if (elem->type == TRG_EQUATION || elem->type == TRG_EQUVAR)
			f->read_string(spcl->effect[i].text);
		else
			f->read_string(nullptr);
	}

	size_t effect_count = f->read_varint();
	for (size_t i = 0; i < effect_count; ++i)
	{
		effect_t *elem = &spcl->effect[i];
		elem->flags = f->read_varint();
		elem->type = f->read_varint();
		elem->x = f->read_varint();
		elem->y = f->read_varint();
		elem->value = (uint32_t) f->read_varint();
		elem->value2 = (uint32_t) f->read_varint();
		f->read_string(elem->text);
	}

	f->read_varint();  // ignore extension flags
}

static void SaveMapTile(hamworld::Section *f, const big_savetile *t)
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

static void SaveMapData(hamworld::Section *f, const Map *map)
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
	// Prepare custom item table
	hamworld::Section item_definitions;
	item_definitions.write_varint(NUM_ORIGINAL_ITEMS);
	item_definitions.write_varint(NumItems() - NUM_ORIGINAL_ITEMS);
	for (int i = NUM_ORIGINAL_ITEMS; i < NumItems(); ++i)
	{
		item_definitions.write_string("");  // savename not yet implemented
		SaveItem(&item_definitions, GetItem(i));
	}

	hamworld::Section sound_definitions;
	sound_definitions.write_varint(CUSTOM_SND_START);
	sound_definitions.write_varint(GetNumCustomSounds());
	for (int i = 0; i < GetNumCustomSounds(); ++i)
	{
		sound_definitions.write_string("");  // savename not yet implemented
		SaveSound(&sound_definitions, GetSoundInfo(CUSTOM_SND_START + i),
			GetCustomLength(i), GetCustomSound(i));
	}

	hamworld::Section rle_tilegfx;
	rle_tilegfx.write_varint(world->numTiles);
	SaveTiles(rle_tilegfx.stream);

	hamworld::Section terrain;
	terrain.write_varint(world->numTiles);
	for (int i = 0; i < world->numTiles; ++i)
	{
		terrain_t t = world->terrain[i];
		terrain.write_varint(t.flags);
		terrain.write_varint(t.next);
		terrain.write_varint(0);  // no extension flags
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
		mapsec.write_varint(map->flags);

		// badguys
		size_t badguy_count = 0;
		for (size_t i = 0; i < MAX_MAPMONS; ++i)
			if (map->badguy[i].type)
				++badguy_count;
		mapsec.write_varint(badguy_count);
		for (size_t i = 0; i < MAX_MAPMONS; ++i)
			if (map->badguy[i].type)
				SaveMapMons(&mapsec, &map->badguy[i]);

		// specials
		size_t special_count = 0;
		for (size_t i = 0; i < MAX_SPECIAL; ++i)
			if (map->special[i].x != 255)
				++special_count;
		mapsec.write_varint(special_count);
		for (size_t i = 0; i < MAX_SPECIAL; ++i)
			if (map->special[i].x != 255)
				SaveMapSpecial(&mapsec, &map->special[i]);

		// map data
		SaveMapData(&mapsec, map);
		mapsec.write_varint(0);  // no extension flags
		maps.push_back(std::move(mapsec));
	}

	hamworld::Save save(fname);
	save.header(world->author, world->map[0]->name, APPNAME);

	if (NumItems() > NUM_ORIGINAL_ITEMS)
	{
		save.section("item_definitions", item_definitions.save());
	}
	if (GetNumCustomSounds() > 0)
	{
		save.section("sound_definitions", sound_definitions.save());
	}

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
	hamworld::Load load(fname);

	std::string app;
	if (!load.header(world->author, nullptr, &app))
	{
		LogError("Ham_LoadWorld(%s): bad header", fname);
		return 0;
	}

	memset(world->map, 0, sizeof(world->map));

	ExitItems();
	InitItems();
	ClearCustomSounds();

	world->numMaps = 0;

	hamworld::Section section;
	std::string section_name;
	while (load.section(&section_name, &section) && !section_name.empty())
	{
		if (section_name == "item_definitions")
		{
			size_t start = section.read_varint();
			if (start != NUM_ORIGINAL_ITEMS)
			{
				LogError("Ham_LoadWorld(%s): item definition offest NYI (expected %d, got %u)", fname, NUM_ORIGINAL_ITEMS, (unsigned int)(start));
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
		else if (section_name == "sound_definitions")
		{
			size_t start = section.read_varint();
			if (start != CUSTOM_SND_START)
			{
				LogError("Ham_LoadWorld(%s): sound definition offest NYI (expected %d, got %u)", fname, CUSTOM_SND_START, (unsigned int)(start));
				return false;
			}
			size_t sound_count = section.read_varint();
			for (size_t i = 0; i < sound_count; ++i)
			{
				section.read_string(nullptr);  // ignore savename
				LoadSound(&section);
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
				world->terrain[i].flags = section.read_varint();
				world->terrain[i].next = section.read_varint();
				section.read_varint();  // ignore extension flags
			}
		}
		else if (section_name == "map")
		{
			Map *map = world->map[world->numMaps++] = new Map(0, "");
			section.read_varint();  // skip uid
			size_t w = section.read_varint();
			size_t h = section.read_varint();
			map->Resize(w, h);
			section.read_string(map->name);
			section.read_string(map->song);
			section.stream.read((char*) &map->itemDrops, 2);
			map->numBrains = section.read_varint();
			map->numCandles = section.read_varint();
			map->flags = section.read_varint();

			memset(map->badguy, 0, sizeof(mapBadguy_t) * MAX_MAPMONS);
			size_t badguy_count = section.read_varint();
			for (size_t i = 0; i < badguy_count; ++i)
				LoadMapMons(&section, &map->badguy[i]);

			InitSpecials(map->special);
			size_t special_count = section.read_varint();
			for (size_t i = 0; i < special_count; ++i)
				LoadMapSpecial(&section, &map->special[i]);

			LoadMapData(&section, map);

			section.read_varint();  // ignore extension flags
		}
		else
		{
			LogError("Ham_LoadWorld(%s): unknown section: %s", fname, section_name.c_str());
			return 0;
		}
	}

	SetupRandomItems();

	return 1;
}

bool Ham_GetWorldName(const char *fname, char *buffer, char *authbuffer)
{
	hamworld::Load load(fname);

	std::string app;
	if (!load.header({authbuffer, 32}, {buffer, 32}, &app))
	{
		return false;
	}

	return true;
}

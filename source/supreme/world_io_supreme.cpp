#include "world_io_supreme.h"
#include <SDL_rwops.h>
#include "appdata.h"
#include "world.h"
#include "map.h"
#include "tile.h"
#include "sound.h"

bool Supreme_GetWorldName(SDL_RWops *f, StringDestination name, StringDestination author)
{
	char authbuffer[32], namebuffer[32];

	if (SDL_RWseek(f, 8, RW_SEEK_SET) < 0)
		return false;
	if (SDL_RWread(f, authbuffer, 32, 1) < 1)
		return false;
	if (SDL_RWread(f, namebuffer, 32, 1) < 1)
		return false;

	authbuffer[31] = 0;
	author.assign(authbuffer);
	namebuffer[31] = 0;
	name.assign(namebuffer);

	return true;
}

struct io_terrain_t
{
	word flags;
	word next;
};

static terrain_t LoadOneTerrain(io_terrain_t io_terrain)
{
	terrain_t r = { io_terrain.flags, static_cast<word>(io_terrain.next & 0x3ff) };
	if (io_terrain.next & (1 << 15))
		r.flags |= TF_SHADOWLESS;
	return r;
}

static io_terrain_t SaveOneTerrain(terrain_t terrain)
{
	io_terrain_t r = { static_cast<word>(terrain.flags & 0xffffff), terrain.next };
	if (terrain.flags & TF_SHADOWLESS)
		r.next |= (1 << 15);
	return r;
}

void LoadTerrain(world_t *world, const char *fname, SDL_RWops *f)
{
	for (int i = 0; i < world->numTiles; ++i)
	{
		io_terrain_t io_terrain;
		SDL_RWread(f, &io_terrain, sizeof(io_terrain_t), 1);
		world->terrain[i] = LoadOneTerrain(io_terrain);
	}

	// In 2012, Shadowless Wall was added as an additional meaning of the
	// Transparent Roof flag. However, only about 10 worlds since then have
	// used the feature, while about 150 worlds from before that point are at
	// some risk of unexpected appearance due to the change. Therefore, with
	// the flags being split in 2023, only autofill the new flag for those
	// specific worlds, indicated by the presence of this marker file.
	std::string buf = fname;
	buf.append(".shadowless");
	if (AppdataOpen(buf.c_str()))
	{
		for (int i = 0; i < world->numTiles; ++i)
		{
			if (world->terrain[i].flags & TF_TRANS)
				world->terrain[i].flags |= TF_SHADOWLESS;
		}
	}
}

bool Supreme_LoadWorld(world_t *world, const char *fname, SDL_RWops *f)
{
	char code[9];
	SDL_RWread(f, code, 8, 1);
	code[8] = '\0';
	if (strcmp(code, "SUPREME!"))
		return false;

	SDL_RWread(f,&world->author,sizeof(char),32);
	SDL_RWseek(f, 32, RW_SEEK_CUR);  // name of the world, not needed here
	SDL_RWread(f,&world->numMaps,1,1);
	SDL_RWread(f,&world->totalPoints,1,sizeof(int));
	SDL_RWread(f,&world->numTiles,1,sizeof(word));	// tile count
	SetNumTiles(world->numTiles);

	LoadTiles(f);
	LoadTerrain(world, fname, f);

	for(int i = 0; i < MAX_MAPS; i++)
		world->map[i]=NULL;

	for(int i = 0; i < world->numMaps; i++)
	{
		world->map[i]=new Map(f);
		if(!world->map[i])
		{
			return false;
		}
	}

	LoadItems(f);
	LoadCustomSounds(f);
	SetupRandomItems();
	return true;
}

bool Supreme_SaveWorld(const world_t *world, SDL_RWops *f)
{
	char code[9]="SUPREME!";
	SDL_RWwrite(f,code,8,sizeof(char));	// identifier code

	SDL_RWwrite(f,&world->author,sizeof(char),32);
	SDL_RWwrite(f,&world->map[0]->name,sizeof(char),32);
	SDL_RWwrite(f,&world->numMaps,1,1);
	SDL_RWwrite(f,&world->totalPoints,1,sizeof(int));
	SDL_RWwrite(f,&world->numTiles,1,sizeof(word));

	SaveTiles(f);

	for(int i = 0; i < world->numTiles; ++i)
	{
		io_terrain_t io_terrain = SaveOneTerrain(world->terrain[i]);
		SDL_RWwrite(f, &io_terrain, sizeof(io_terrain_t), 1);
	}

	for(int i = 0; i < world->numMaps; i++)
		world->map[i]->Save(f);

	SaveItems(f);
	SaveCustomSounds(f);

	return true;
}

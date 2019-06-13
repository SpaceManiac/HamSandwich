#include "guy.h"
#include "player.h"
#include "intface.h"
#include "options.h"
#include "editor.h"
#include "log.h"

Guy **guys;
Guy *goodguy;
int maxGuys;
Guy *guyHit;
Guy *nobody;

// ------------------------------------------------------------------------
// CLASS GUY

Guy::Guy(void)
{
	type = MONS_NONE;
}

Guy::~Guy(void)
{
}

byte Guy::CoconutBonk(int xx, int yy, Guy *him)
{
	int x2, y2;

	if ((MonsterFlags(him->type) & MF_ENEMYWALK) && (type != MONS_BOUAPHA))
		return 0;
	if ((MonsterFlags(him->type) & MF_FREEWALK) && (type == MONS_BOUAPHA))
		return 0;

	x2 = him->x >> FIXSHIFT;
	y2 = him->y >> FIXSHIFT;

	if ((x2 + him->rectx2) >= (xx + rectx) && (y2 + him->recty2) >= (yy + recty) &&
			(x2 + him->rectx) <= (xx + rectx2) && (y2 + him->recty) <= (yy + recty2))
	{
		return 1;
	}
	return 0;
}

// checks to see if a rect of size, located at (xx,yy) hits HIM

byte Guy::AttackCheck(byte size, int xx, int yy, Guy *him)
{
	int x2, y2;

	if (!him)
		return 0;

	xx -= size;
	yy -= size;

	x2 = him->x >> FIXSHIFT;
	y2 = him->y >> FIXSHIFT;

	if ((x2 + him->rectx2) >= xx && (y2 + him->recty2) >= yy && (x2 + him->rectx) <= (xx + size * 2) &&
			(y2 + him->recty) <= (yy + size * 2))
		return 1;

	return 0;
}

// check to see if the chosen tile intersects HIMs rectangle

byte TileBonkCheck(int x, int y, Guy *him)
{
	int himX, himY;

	if (!him)
		return 0;

	himX = him->x >> FIXSHIFT;
	himY = him->y >> FIXSHIFT;

	if ((himX + him->rectx2) >= x && (himY + him->recty2) >= y && (himX + him->rectx)<(x + TILE_WIDTH) &&
			(himY + him->recty)<(y + TILE_HEIGHT))
		return 1;

	return 0;
}

void TryToPush(Guy *me, int x, int y, Map *map, world_t *world)
{
	int destx, desty, i;
	int xx, yy;

	if (!PlayerPushMore())
		return; // not pushing hard enough yet

	if (abs(me->dx) > abs(me->dy)) // horizontal push
	{
		desty = y;
		if (me->dx > 0) // right push
		{
			destx = x + 1;
			if (me->mapx != x - 1 || me->mapy != y)
				return; // no push
		}
		else
		{
			destx = x - 1; // left push
			if (me->mapx != x + 1 || me->mapy != y)
				return; // no push
		}
	}
	else // vertical push
	{
		destx = x;
		if (me->dy > 0) // down push
		{
			desty = y + 1;
			if (me->mapx != x || me->mapy != y - 1)
				return; // no push
		}
		else
		{
			desty = y - 1; // up push
			if (me->mapx != x || me->mapy != y + 1)
				return; // no push
		}
	}

	if (destx < 0 || destx >= map->width || desty < 0 || desty >= map->height)
		return; // can't push off edge of map
	if (map->map[destx + desty * map->width].wall)
		return; // can't push into a wall
	if (map->map[destx + desty * map->width].item)
		return; // can't push into any item
	if (world->terrain[map->map[destx + desty * map->width].floor].flags & TF_PUSHON)
	{
		// it passed every single test. the only thing left is to see if it would overlap
		// any badguys, which would not be good
		xx = destx*TILE_WIDTH;
		yy = desty*TILE_HEIGHT;
		for (i = 0; i < maxGuys; i++)
			if ((guys[i]) && (guys[i]->type) && (guys[i]->hp > 0) &&
					(abs(guys[i]->mapx - destx) < 8) && (abs(guys[i]->mapy - desty) < 8))
				if (TileBonkCheck(xx, yy, guys[i]))
					return; // the pushy block would hit the guy

		// the floor is pushonable, let's do it
		MakeSound(SND_BLOCKPUSH, me->x, me->y, SND_CUTOFF, 1000);
		map->map[destx + desty * map->width].wall = map->map[x + y * map->width].wall;
		map->map[destx + desty * map->width].floor = map->map[x + y * map->width].floor;
		map->map[x + y * map->width].floor = world->terrain[map->map[x + y * map->width].floor].next;
		map->map[x + y * map->width].wall = 0;
	}
}

byte Walkable(Guy *me, int x, int y, Map *map, world_t *world)
{
	byte result;
	mapTile_t *m = &map->map[x + y * map->width];

	result = 1;

	if (me->type == MONS_BOUAPHA && (!PlayerGetItem(m->item, x, y)))
		m->item = ITM_NONE;

	if ((world->terrain[m->floor].flags & (TF_WATER | TF_LAVA)) && (!(MonsterFlags(me->type)&(MF_WATERWALK | MF_FLYING))))
		result = 0;

	if ((world->terrain[m->floor].flags & TF_SOLID) && (!(MonsterFlags(me->type)&(MF_FLYING | MF_WALLWALK))))
		result = 0; // impassable

	if ((world->terrain[m->floor].flags & TF_PUSHY) && me->type == MONS_BOUAPHA)
	{
		TryToPush(me, x, y, map, world);
	}

	if (m->item >= ITM_DOOR1 && m->item <= ITM_DOOR2B && me->type == MONS_BOUAPHA)
	{
		if (m->item == ITM_DOOR1B || m->item == ITM_DOOR2B)
			if (PlayerKeys(3))
			{
				m->item = ITM_NONE;
				MakeSound(SND_DOOROPEN, (x * TILE_WIDTH) << FIXSHIFT, (y * TILE_HEIGHT) << FIXSHIFT, SND_CUTOFF, 1500);
			}
		if (m->item == ITM_DOOR1G || m->item == ITM_DOOR2G)
			if (PlayerKeys(2))
			{
				m->item = ITM_NONE;
				MakeSound(SND_DOOROPEN, (x * TILE_WIDTH) << FIXSHIFT, (y * TILE_HEIGHT) << FIXSHIFT, SND_CUTOFF, 1500);
			}
		if (m->item == ITM_DOOR1R || m->item == ITM_DOOR2R)
			if (PlayerKeys(1))
			{
				m->item = ITM_NONE;
				MakeSound(SND_DOOROPEN, (x * TILE_WIDTH) << FIXSHIFT, (y * TILE_HEIGHT) << FIXSHIFT, SND_CUTOFF, 1500);
			}
		if (m->item == ITM_DOOR1 || m->item == ITM_DOOR2)
			if (PlayerKeys(0))
			{
				m->item = ITM_NONE;
				PlayerLoseKey(0);
				MakeSound(SND_DOOROPEN, (x * TILE_WIDTH) << FIXSHIFT, (y * TILE_HEIGHT) << FIXSHIFT, SND_CUTOFF, 1500);
			}
	}

	if (m->item == ITM_MINEBLOCK && me->type == MONS_MINECART)
	{
		m->item = ITM_NONE;
		ExplodeParticles2(PART_DIRT, me->x, me->y, me->z, 10, 10);
	}

	if (MonsterFlags(me->type) & MF_FLYING)
	{
		if ((m->wall || (m->item >= MAX_SHOOTABLE_ITMS && m->item < NEW_PICKUP_ITMS)) && (!(MonsterFlags(me->type) & MF_WALLWALK)))
			result = 0;
	}
	else if (MonsterFlags(me->type) & MF_AQUATIC)
	{
		if (!(world->terrain[m->floor].flags & (TF_WATER | TF_LAVA)))
			result = 0;
		if (m->wall || (m->item >= MAX_WALKABLE_ITMS && m->item < NEW_PICKUP_ITMS))
			result = 0;
	}
	else
	{
		if ((m->wall || (m->item >= MAX_WALKABLE_ITMS && m->item < NEW_PICKUP_ITMS)) && (!(MonsterFlags(me->type) & MF_WALLWALK)))
			result = 0;
	}

	if (!result) // bumped a wall, see if that triggers a special
	{
		SpecialStepCheck(map, x, y, me);
	}
	return result;
}

byte Guy::CanWalk(int xx, int yy, Map *map, world_t *world)
{
	byte result;
	int mapx1, mapx2, mapy1, mapy2;
	int i, j;

	xx >>= FIXSHIFT;
	yy >>= FIXSHIFT;

	mapx1 = (xx + rectx) / TILE_WIDTH;
	mapy1 = (yy + recty) / TILE_HEIGHT;
	mapx2 = (xx + rectx2) / TILE_WIDTH;
	mapy2 = (yy + recty2) / TILE_HEIGHT;

	// boundary conditions
	if (mapx1 < 0 || mapy1 < 0 || mapx2 >= map->width || mapy2 >= map->height || xx < 0 || yy < 0)
		return 0;

	result = 1;
	for (i = mapx1; i <= mapx2; i++)
	{
		for (j = mapy1; j <= mapy2; j++)
		{
			if (!Walkable(this, i, j, map, world))
			{
				result = 0;
				break;
			}
		}
		if (result == 0)
			break;
	}

	if (MonsterFlags(type) & MF_FREEWALK)
		return result; // can't have a guy collision

	if (type == MONS_BOUAPHA && player.vehicle == VE_MINECART)
	{
		// be bigger, so you can smack into badguys better
		rectx -= 10;
		rectx2 += 10;
		recty -= 10;
		recty2 += 10;
	}
	if (result) // no wall collision, look for guy collision
		for (i = 0; i < maxGuys; i++)
			if ((guys[i]) && (guys[i] != this) && (guys[i]->type) && (guys[i]->hp > 0) &&
					(abs(guys[i]->mapx - mapx) < 8) && (abs(guys[i]->mapy - mapy) < 8))
			{
				if (CoconutBonk(xx, yy, guys[i]))
				{
					if (type == MONS_BOUAPHA && player.vehicle == VE_MINECART && parent && parent->mind1 > 20)
					{
						// if you hit someone while riding a minecart, they get hurt bad
						guys[i]->GetShot(parent->dx, parent->dy, 30, map, world);
					}
					else
						return 0; // hit heads with somebody
				}
			}

	return result;
}

void Guy::SeqFinished(void)
{
	if ((seq == ANIM_DIE) || (seq == ANIM_A3 && type == MONS_BOUAPHA && player.weapon != WPN_PWRARMOR))
	{
		if (type == MONS_BOUAPHA)
		{
			if (player.weapon == WPN_PWRARMOR)
			{
				player.weapon = 0;
				seq = ANIM_IDLE;
				frm = 0;
				frmAdvance = 128;
				action = ACTION_IDLE;
				return;
			}
			else
				// restart current level
				SendMessageToGame(MSG_RESET, 0);
		}

		type = MONS_NONE; // all gone
	}
	seq = ANIM_IDLE;
	frm = 0;
	frmAdvance = 128;
	action = ACTION_IDLE;
}

void Guy::AttackThem(void)
{
}

void Guy::NextFrame(void)
{
	frm++;

	if (MonsterAnim(type, seq)[frm] == 255)
		// this sequence is done
		SeqFinished();

	if (type == MONS_NONE)
		return; // seqfinished may have killed the guy
}

void Guy::CalculateRect(void)
{
	sprite_t *spr;
	byte s;

	if (MonsterFlags(type) & MF_SPRITEBOX)
	{
		spr = GetMonsterSprite(type, seq, frm, facing);
		if (spr)
		{
			rectx = -spr->ofsx;
			rectx2 = spr->width - spr->ofsx;
			recty = -spr->ofsy;
			recty2 = spr->height - spr->ofsy;
		}
		else
		{
			rectx = 0;
			recty = 0;
			rectx2 = 0;
			recty2 = 0;
		}
	}
	else // normal method
	{
		s = MonsterSize(type);
		rectx = -s;
		rectx2 = s;
		recty = -s * 3 / 4;
		recty2 = s * 3 / 4;
	}
}

void Guy::Update(Map *map, world_t *world)
{
	int oldmapx, oldmapy;
	byte b;
	static byte burnFlip = 0;

	if (hp == 0 && seq != ANIM_DIE && type == MONS_BAT)
	{
		// MEGA HACK!?  Why on earth?!?!?!!?!?!!?  Something to do with mine cart kills
		// resurrecting bats
		type = MONS_NONE;
		return;
	}

	oldmapx = mapx;
	oldmapy = mapy;

	CalculateRect();

	if (type == MONS_BOUAPHA) // special case, player controls Bouapha
		PlayerControlMe(this, &map->map[mapx + mapy * map->width], world);
	else
		MonsterControl(map, world);

	if (ouch > 0)
		ouch--;

	if (poison > 0)
	{
		if (hp > 0 && ((poison & 7) == 0))
		{
			hp--;
			if (hp == 0)
			{
				hp = 1;
				GetShot(0, 0, 1, map, world);
			}
		}
		poison--;
	}

	oldx = x;
	oldy = y;

	x += dx;

	if (!CanWalk(x, y, map, world))
	{
		x -= dx;
		if (type == MONS_ROLLER || type == MONS_ROLLER2 || type == MONS_CENTIHEAD || type == MONS_DRL ||
				type == MONS_BOUAPHA || type == MONS_VAMPIRE || type == MONS_PKSTEIN || type == MONS_TRICEROID ||
				type == MONS_BUNNY || type == MONS_LOONYSHIP || type == MONS_FRIENDLY || type == MONS_SNOWBALL ||
				type == MONS_SNOWBALL2 || type == MONS_ROLLER3 || type == MONS_ROLLER4 || type == MONS_DARKVAMP ||
				type == MONS_FRIENDLY2)
			mind1 = 1; // tell it that it hit a wall
	}

	y += dy;

	if (!CanWalk(x, y, map, world))
	{
		y -= dy;
		if (type == MONS_ROLLER || type == MONS_ROLLER2 || type == MONS_CENTIHEAD || type == MONS_DRL ||
				type == MONS_BOUAPHA || type == MONS_VAMPIRE || type == MONS_PKSTEIN || type == MONS_TRICEROID ||
				type == MONS_BUNNY || type == MONS_LOONYSHIP || type == MONS_FRIENDLY || type == MONS_SNOWBALL ||
				type == MONS_SNOWBALL2 || type == MONS_ROLLER3 || type == MONS_ROLLER4 || type == MONS_DARKVAMP ||
				type == MONS_FRIENDLY2)
			mind1 += 2; // tell it that it hit a wall
	}

	if (MonsterFlags(type) & MF_FLYING)
	{
		if (z < 20 * FIXAMT) // go up if you need to
			dz += FIXAMT;
		else
			dz -= FIXAMT;

		Clamp(&dz, FIXAMT * 4);
		z += dz;

		if (z < 0)
			z = 0;
	}
	else
	{
		z += dz;
		if (z < 0)
		{
			z = 0;
			dz = 0;
		}
		else
		{
			if (!(MonsterFlags(type) & MF_NOGRAV))
				dz -= FIXAMT * 2;
		}
	}

	frmTimer += frmAdvance;
	while (frmTimer > 255)
	{
		frmTimer -= 255;
		NextFrame();
	}
	if (type == MONS_NONE)
		return; // NextFrame may have killed you

	mapx = (x >> FIXSHIFT) / TILE_WIDTH;
	mapy = (y >> FIXSHIFT) / TILE_HEIGHT;

	if (type == MONS_BOUAPHA) // special case, Bouapha is the player, follow him
	{
		UpdateCamera(x >> FIXSHIFT, y >> FIXSHIFT, facing * 32, map);
		if (map->flags & MAP_TORCHLIT)
		{
			b = GetPlayerGlow();
			if (b > 32)
				map->BrightTorch(mapx, mapy, b, b / 4);
			else
				map->TempTorch(mapx, mapy, b);
			if (b > 32)
				b -= 4;
			else if (b < 32)
				b++;
			SetPlayerGlow(b);
		}
		else
		{
			b = GetPlayerGlow();
			if (b > 0)
			{
				map->BrightTorch(mapx, mapy, b, b / 4);
				if (b > 4)
					b -= 4;
				else
					b--;
				SetPlayerGlow(b);
			}
		}

		// standing on water!!!!!!!  DROWN!!!!
		if ((hp > 0) && (z == 0) && (world->terrain[map->map[mapx + mapy * map->width].floor].flags & TF_WATER)
				&& (!PlayerCanWaterwalk()) && (!(player.vehicle == VE_RAFT)))
		{
			// if there's a raft, hop on instead of dying
			if (!RaftNearby())
			{
				if (player.weapon == WPN_PWRARMOR)
					player.weapon = 0;
				facing = (4 + facing)&7;
				hp = 0;
				SetPlayerHP(hp);
				seq = ANIM_A3;
				dx = 0;
				dy = 0;
				frm = 0;
				frmAdvance = 255;
				frmTimer = 0;
				action = ACTION_BUSY;
				MakeSound(SND_DROWN, x, y, SND_CUTOFF | SND_ONE, 65536);
			}
		}
		// standing on lava, OW!
		if ((hp > 0) && (z == 0) && (world->terrain[map->map[mapx + mapy * map->width].floor].flags & TF_LAVA)
				&& (!PlayerCanWaterwalk()) && (!(player.vehicle == VE_RAFT)) && (!(player.weapon == WPN_PWRARMOR)))
		{
			if (!RaftNearby())
			{
				if (burnFlip)
				{
					GetShot(0, 0, 10, map, world);
					if (hp == 0) // it was the death of me
					{
						// lava death
					}
				}
				burnFlip = 1 - burnFlip;
			}
		}
		if ((oldmapx != mapx || oldmapy != mapy) &&
				(world->terrain[map->map[mapx + mapy * map->width].floor].flags & TF_STEP))
		{
			map->map[mapx + mapy * map->width].floor = world->terrain[map->map[mapx + mapy * map->width].floor].next;
		}
	}
	if (oldmapx != mapx || oldmapy != mapy)
		SpecialStepCheck(map, mapx, mapy, this);

	if (mapx < map->width && mapy < map->height)
		bright = map->map[mapx + mapy * map->width].templight;
	else
		bright = -32;
}

// this is a very half-assed version of update to be called by the editor instead of the game

void Guy::EditorUpdate(Map *map)
{
	int mapx, mapy;

	mapx = (x >> FIXSHIFT) / TILE_WIDTH;
	mapy = (y >> FIXSHIFT) / TILE_HEIGHT;
	if (mapx >= 0 && mapy >= 0 && mapx < map->width && mapy < map->height)
		bright = map->map[mapx + mapy * map->width].templight;
	else
		bright = -32;
}

void Guy::Render(byte light)
{
	MonsterDraw(x, y, z, type, seq, frm, facing, bright * (light > 0), ouch, poison);
}

void Guy::MonsterControl(Map *map, world_t *world)
{
	int i;

	if (!goodguy)
		return; // badguys don't move if you're gone, saves a lot of crash possibilities

	if (mindControl)
	{
		mindControl--;
		if (!mindControl)
		{
			friendly = 0;
			target = nobody;
		}
		if ((mindControl & 3) == 0)
			StinkySteam(x - FIXAMT * 15 + MGL_randoml(FIXAMT * 15), y - FIXAMT * 15 + MGL_randoml(FIXAMT * 15), MGL_randoml(FIXAMT * 10),
				FIXAMT * 2 + MGL_randoml(FIXAMT * 4));
	}

	if (type == MONS_BURNER || type == MONS_ROLLER || type == MONS_ROLLER2 ||
			type == MONS_ALIENEGG || type == MONS_ALIENEGG2 || type == MONS_EGGSAC || type == MONS_BOILER ||
			type == MONS_FRIENDLY || type == MONS_SNOWBALL || type == MONS_SNOWBALL2 || type == MONS_ROLLER3 ||
			type == MONS_ROLLER4 || type == MONS_FRIENDLY2)
	{
		// these monsters don't ever stop functioning, and can't be tricked by
		// invisibility
		target = goodguy;
	}
	else
	{
		// lose sight of the invisible goodguy
		if (player.invisibility && target == goodguy)
		{
			target = nobody; // target the randomly moving, invisible, invincible, ghostly, nobody
		}

		// if you lost your target, or 2% chance of it even if you didn't,
		// time to pick a new target
		if (!target || target->friendly == friendly || target->type == MONS_NONE || target == nobody || MGL_random(50) == 0)
		{
			// lost your target one way or another
			if (friendly)
				i = LockOnEvil(x >> FIXSHIFT, y >> FIXSHIFT);
			else
				i = LockOnGood(x >> FIXSHIFT, y >> FIXSHIFT);

			if (i != 65535)
				target = guys[i];
			else
			{
				target = nobody;
			}
		}
	}

	if (!target) return;

	monsterAi_t ai = MonsterAi(type);
	if (ai == NULL) {
		LogDebug("null AI on %s", MonsterName(type));
		return;
	}

	ai(this, map, world, target);
}

void Guy::GetShot(int dx, int dy, byte damage, Map *map, world_t *world)
{
	int i, formerHP, newHP;
	byte t;

	t = type;

	if (hp == 0)
		return; // can't shoot a dead guy

	if (type == 0)
		return; // shouldn't have a type 0 guy at all

	if (type == MONS_BOUAPHA && PlayerShield())
		return; // invincible when shielded

	if (MonsterFlags(type) & MF_INVINCIBLE)
		return; // invincible

	if (type == MONS_BOUAPHA && player.weapon == WPN_PWRARMOR)
	{
		// damage is done to the armor instead
		if (player.ammo > damage)
			player.ammo -= damage;
		else
			player.ammo = 0;

		ouch = 4; // still do the ouch so you can see it
		return;
	}

	formerHP = hp;
	ouch = 4;
	hp -= damage;

	if (!(MonsterFlags(type) & MF_NOMOVE))
	{
		x += dx;
		if (dx != 0)
			if (!CanWalk(x, y, map, world))
				x -= dx;
		y += dy;
		if (dy != 0)
			if (!CanWalk(x, y, map, world))
				y -= dy;
	}

	newHP = hp;

	if (hp <= 0)
	{
		hp = 0;
		newHP = 0;
		seq = ANIM_DIE;
		this->dx = 0;
		this->dy = 0;
		frm = 0;
		frmAdvance = 64;
		frmTimer = 0;
		action = ACTION_BUSY;
		if (type == MONS_MUSH || type == MONS_SVEN || type == MONS_BJORN || type == MONS_SHOCKTR ||
				type == MONS_TROOPER2)
			facing = 2; // these can only die facing forward, artistically speaking
		// possible item drop
		switch (type) {
			case MONS_ZOMBIE: // zombies always drop a brain
				if (!map->DropItem(mapx, mapy, ITM_BRAIN))
					map->map[mapx + mapy * map->width].item = ITM_BRAIN; // if the drop failed, just
				// FORCE a brain to appear, covering
				// whatever item was there
				break;
			case MONS_SUPERZOMBIE: // super zombies always drop 2 brains
				if (!map->DropItem(mapx, mapy, ITM_BRAIN))
					map->map[mapx + mapy * map->width].item = ITM_BRAIN;
				if (!map->DropItem(mapx, mapy, ITM_BRAIN))
					map->map[mapx + (mapy + 1) * map->width].item = ITM_BRAIN; // hope it's not a wall
				break;
			case MONS_BOUAPHA:
			case MONS_MAMASPDR: // already drops hundreds of baby spiders, that's enough
			case MONS_SERPENT: // don't drop items, since you live on the water
			case MONS_FREAKAZOID: // don't drop items, since you live on the water
			case MONS_MAGMAZOID:// don't drop items, since you live in the lava
			case MONS_MATCLAW1:
			case MONS_MATCLAW2:
			case MONS_MATHEAD:
			case MONS_MATSKULL:
			case MONS_MUSH:
			case MONS_SVEN:
			case MONS_BJORN:
			case MONS_DUSTDEVIL:
			case MONS_LICH:
			case MONS_DRL:
			case MONS_SDZL:
			case MONS_ULTRAZOID:
			case MONS_THINGTENT:
			case MONS_THINGTENTTIP:
			case MONS_STICKMAN:
			case MONS_MECHABOUAPHA:
			case MONS_CENTIBODY:
			case MONS_CENTIHEAD:
			case MONS_BOILER:
			case MONS_GREATPK:
			case MONS_SANTA:
			case MONS_COFFIN:
			case MONS_MATBRAIN:
			case MONS_THING:
			case MONS_SPHINX:
			case MONS_SNOWBALL:
			case MONS_SNOWBALL2:
			case MONS_FRIENDLY:
			case MONS_GOODTURRET:
			case MONS_WIZARD:
				// no drop
				break;
			case MONS_GNOME:
				if (mind3 != 0) // drop what you stole!
					if (!map->DropItem(mapx, mapy, mind3))
						map->map[mapx + mapy * map->width].item = mind3;
				// if the drop failed, just force it
				break;
			default:
				// drop random item
				if (map->flags & MAP_MANYITEMS)
					i = MGL_random(50);
				else
					i = MGL_random(200);
#ifndef EXPANDO
				if (i < 2)
					map->DropItem(mapx, mapy, ITM_FLAME);
				else if (i < 4)
					map->DropItem(mapx, mapy, ITM_BOMBS);
				else if (i < 6)
					map->DropItem(mapx, mapy, ITM_AK8087);
				else if (i < 8)
					map->DropItem(mapx, mapy, ITM_MISSILES);
				else if (i < 12)
					map->DropItem(mapx, mapy, ITM_TAKEOUT);
				else if (i < 13)
					map->DropItem(mapx, mapy, ITM_SHIELD);
#else
				if (i < 1)
					map->DropItem(mapx, mapy, ITM_FLAME);
				else if (i < 2)
					map->DropItem(mapx, mapy, ITM_BOMBS);
				else if (i < 3)
					map->DropItem(mapx, mapy, ITM_AK8087);
				else if (i < 4)
					map->DropItem(mapx, mapy, ITM_MISSILES);
				else if (i < 6)
					map->DropItem(mapx, mapy, ITM_TAKEOUT);
				else if (i < 7)
					map->DropItem(mapx, mapy, ITM_SHIELD);
				else if (i < 8)
					map->DropItem(mapx, mapy, ITM_BIGAXE);
				else if (i < 9)
					map->DropItem(mapx, mapy, ITM_LIGHTNING);
				else if (i < 10)
					map->DropItem(mapx, mapy, ITM_SPEAR);
				else if (i < 11)
					map->DropItem(mapx, mapy, ITM_MACHETE);
				else if (i < 12)
					map->DropItem(mapx, mapy, ITM_MINES);
				else if (i < 13)
					map->DropItem(mapx, mapy, ITM_ORBITER);
				else if (i < 14)
				{
					i = MGL_random(20);
					if (i < 2)
						map->DropItem(mapx, mapy, ITM_REFLECTOR);
					else if (i < 3)
						map->DropItem(mapx, mapy, ITM_MINDCONTROL);
					else if (i < 9)
						map->DropItem(mapx, mapy, ITM_BADCHINESE);
				}
#endif
				break;
		}

		// give him points for it
		PlayerGetPoints(MonsterPoints(type));
		SpecialKillCheck(map, type);
		if (player.rage < 127 * 256 - 500 && player.rageClock == 0 && player.weapon != WPN_PWRARMOR)
			player.rage += 512; // and crank up the rage
	}
	if (type != MONS_BOUAPHA)
	{
		ShowEnemyLife(MonsterName(t), formerHP * 128 / MonsterHP(t), hp * 128 / MonsterHP(t), (byte) (newHP > 0));
	}
	else
		SetPlayerHP(newHP);
}

// -----------------------------------------------------------------------

void InitGuys(int max)
{
	int i;

	maxGuys = max;

	guys = (Guy **) malloc(sizeof (Guy *) * maxGuys);
	for (i = 0; i < maxGuys; i++)
		guys[i] = new Guy();
	goodguy = NULL;
	oldPlayAs = opt.playAs;
}

void ExitGuys(void)
{
	int i;

	for (i = 0; i < maxGuys; i++)
		delete guys[i];

	free(guys);
	opt.playAs = oldPlayAs;
}

void UpdateGuys(Map *map, world_t *world)
{
	int i;

	for (i = 0; i < maxGuys; i++)
		if (guys[i]->type != MONS_NONE)
		{
			if (guys[i]->type == MONS_BOUAPHA && player.speed > 0)
			{
				guys[i]->Update(map, world);
				guys[i]->Render(1);
				guys[i]->Update(map, world);
				player.speed--;
			}
			else
				guys[i]->Update(map, world);
		}
}

void EditorUpdateGuys(Map *map)
{
	int i;

	for (i = 0; i < maxGuys; i++)
		if (guys[i]->type != MONS_NONE)
			guys[i]->EditorUpdate(map);
}

void RenderGuys(byte light)
{
	int i;

	for (i = 0; i < maxGuys; i++)
		if (guys[i]->type != MONS_NONE)
			guys[i]->Render(light);
}

void SetupTargets(void)
{
	int i, j;

	for (i = 0; i < maxGuys; i++)
	{
		if (guys[i]->type && guys[i]->friendly == 1)
		{
			j = LockOnEvil(guys[i]->x >> FIXSHIFT, guys[i]->y >> FIXSHIFT);
			if (j != 65535)
				guys[i]->target = guys[j];
			else
				guys[i]->target = NULL;
		}
		else if (guys[i]->type)
		{
			j = LockOnGood(guys[i]->x >> FIXSHIFT, guys[i]->y >> FIXSHIFT);
			if (j != 65535)
				guys[i]->target = guys[j];
			else
				guys[i]->target = goodguy;
		}
	}
}

Guy *AddGuy(int x, int y, int z, byte type)
{
	int i, j, k;
	Guy *g, *g2;

	if (type >= NUM_MONSTERS)
		return NULL;

	for (i = 0; i < maxGuys; i++)
		if (guys[i]->type == MONS_NONE)
		{
			if (type == MONS_BOUAPHA) // special case: bouapha is the goodguy
			{
				goodguy = guys[i];
				PutCamera(x, y);
			}

			if (type == MONS_BOUAPHA || type == MONS_FRIENDLY || type == MONS_GOODTURRET ||
					type == MONS_WIZARD || type == MONS_GOODROBOT || type == MONS_GOODROBOT2 ||
					type == MONS_FRIENDLY2)
			{
				guys[i]->friendly = 1;
				j = LockOnEvil(x >> FIXSHIFT, y >> FIXSHIFT);
				if (j != 65535)
					guys[i]->target = guys[j];
				else
					guys[i]->target = NULL;
				if (type == MONS_FRIENDLY2)
				{
					guys[i]->mind1 = 1;
					guys[i]->mind3 = 64;
				}

			}
			else
			{
				guys[i]->friendly = 0;
				j = LockOnGood(x >> FIXSHIFT, y >> FIXSHIFT);
				if (j != 65535)
					guys[i]->target = guys[j];
				else
					guys[i]->target = goodguy;
			}
			guys[i]->mindControl = 0;
			guys[i]->poison = 0;
			guys[i]->type = type;
			guys[i]->x = x;
			guys[i]->y = y;
			guys[i]->z = z;
			guys[i]->seq = ANIM_IDLE;
			guys[i]->frm = 0;
			guys[i]->frmTimer = 0;
			guys[i]->frmAdvance = 128;
			guys[i]->hp = MonsterHP(type);
			guys[i]->facing = 2;
			guys[i]->ouch = 0;
			guys[i]->dx = 0;
			guys[i]->dy = 0;
			guys[i]->dz = 0;
			guys[i]->bright = 0;
			guys[i]->mind = 0;
			guys[i]->mind1 = 0;
			guys[i]->mind2 = 0;
			guys[i]->mind3 = 0;
			guys[i]->reload = 0;
			guys[i]->parent = NULL;
			guys[i]->CalculateRect();
			guys[i]->ID = i;
			guys[i]->mapx = (guys[i]->x >> FIXSHIFT) / TILE_WIDTH;
			guys[i]->mapy = (guys[i]->y >> FIXSHIFT) / TILE_HEIGHT;

			if (type == MONS_GENERATOR1)
				guys[i]->mind2 = 1;
			if (type == MONS_GENERATOR2)
				guys[i]->mind2 = 5;
			if (type == MONS_GENERATOR3)
				guys[i]->mind2 = 15;
			if (type == MONS_GENERATOR4)
				guys[i]->mind2 = 30;

			if (type == MONS_MATHEAD) // Matilda, need to add all the parts
			{
				guys[i]->z = 32 * FIXAMT;
				g = AddGuy(x, y - 32 * FIXAMT, 0, MONS_MATBODY);
				if (g)
					g->parent = guys[i];
				g = AddGuy(x - 40 * FIXAMT, y - 1 * FIXAMT, 32 * FIXAMT, MONS_MATCLAW1);
				if (g)
					g->parent = guys[i];
				g = AddGuy(x + 40 * FIXAMT, y - 1 * FIXAMT, 32 * FIXAMT, MONS_MATCLAW2);
				if (g)
					g->parent = guys[i];
				g = AddGuy(x, y - 96 * FIXAMT, 32 * FIXAMT, MONS_MATTAIL);
				if (g)
					g->parent = guys[i];
			}

			if (type == MONS_MAT2HEAD) // Matilda, need to add all the parts
			{
				guys[i]->z = 32 * FIXAMT;
				g = AddGuy(x, y - 32 * FIXAMT, 0, MONS_MAT2BODY);
				if (g)
					g->parent = guys[i];

				for (k = 0; k < 2; k++)
				{
					// lower left tentacle
					g = AddGuy(x - 40 * FIXAMT, y - 1 * FIXAMT, 32 * FIXAMT, MONS_THINGTENT);
					if (!g)
						return guys[i];
					g->parent = guys[i];
					g->facing = 6;
					// mind tells them which tentacle they are (to constrain rotation)
					g->mind = 0 + 3 * (k == 1);
					g->mind1 = 128;
					for (j = 0; j < 3; j++)
					{
						g2 = g;
						// adding them all overlapping, they'll move into place when updated
						g = AddGuy(x - 40 * FIXAMT, y - 1 * FIXAMT, 32 * FIXAMT, MONS_THINGTENT);
						if (!g)
							return guys[i];
						g->parent = g2;
						g->facing = 6;
						g->mind = 0;
					}
					g2 = AddGuy(x - 40 * FIXAMT, y - 1 * FIXAMT, 32 * FIXAMT, MONS_THINGTENTTIP);
					if (!g2)
						return guys[i];
					g2->parent = g;
					g2->facing = 6;
					g2->mind = 0;

					// lower right tentacle
					g = AddGuy(x + 40 * FIXAMT, y - 1 * FIXAMT, 32 * FIXAMT, MONS_THINGTENT);
					if (!g)
						return guys[i];
					g->parent = guys[i];
					g->facing = 2;
					g->mind = 1 + 1 * (k == 1);
					g->mind1 = 128;
					for (j = 0; j < 3; j++)
					{
						g2 = g;
						// adding them all overlapping, they'll move into place when updated
						g = AddGuy(x + 40 * FIXAMT, y - 1 * FIXAMT, 32 * FIXAMT, MONS_THINGTENT);
						if (!g)
							return guys[i];
						g->parent = g2;
						g->facing = 2;
						g->mind = 1;
					}
					g2 = AddGuy(x + 40 * FIXAMT, y - 1 * FIXAMT, 32 * FIXAMT, MONS_THINGTENTTIP);
					if (!g2)
						return guys[i];
					g2->parent = g;
					g2->facing = 2;
					g2->mind = 1;
				}

				g = AddGuy(x, y - 96 * FIXAMT, 32 * FIXAMT, MONS_MAT2TAIL);
				if (g)
					g->parent = guys[i];
			}

			if (type == MONS_THING) // The Thing needs to add tentacles
			{
				// lower left tentacle
				g = AddGuy(x - 64 * FIXAMT, y + 48 * FIXAMT, 12 * FIXAMT, MONS_THINGTENT);
				if (!g)
					return guys[i];
				g->parent = guys[i];
				g->facing = 6;
				g->mind = 0; // mind tells them which tentacle they are (to constrain rotation)
				g->mind1 = 128;
				for (j = 0; j < 3; j++)
				{
					g2 = g;
					// adding them all overlapping, they'll move into place when updated
					g = AddGuy(x - 64 * FIXAMT, y + 48 * FIXAMT, 12 * FIXAMT, MONS_THINGTENT);
					if (!g)
						return guys[i];
					g->parent = g2;
					g->facing = 6;
					g->mind = 0;
				}
				g2 = AddGuy(x - 64 * FIXAMT, y + 48 * FIXAMT, 12 * FIXAMT, MONS_THINGTENTTIP);
				if (!g2)
					return guys[i];
				g2->parent = g;
				g2->facing = 6;
				g2->mind = 0;
				// lower right tentacle
				g = AddGuy(x + 64 * FIXAMT, y + 48 * FIXAMT, 12 * FIXAMT, MONS_THINGTENT);
				if (!g)
					return guys[i];
				g->parent = guys[i];
				g->facing = 2;
				g->mind = 1;
				g->mind1 = 128;
				for (j = 0; j < 3; j++)
				{
					g2 = g;
					// adding them all overlapping, they'll move into place when updated
					g = AddGuy(x + 64 * FIXAMT, y + 48 * FIXAMT, 12 * FIXAMT, MONS_THINGTENT);
					if (!g)
						return guys[i];
					g->parent = g2;
					g->facing = 2;
					g->mind = 1;
				}
				g2 = AddGuy(x + 64 * FIXAMT, y + 48 * FIXAMT, 12 * FIXAMT, MONS_THINGTENTTIP);
				if (!g2)
					return guys[i];
				g2->parent = g;
				g2->facing = 2;
				g2->mind = 1;
				// upper right tentacle
				g = AddGuy(x + 64 * FIXAMT, y - 20 * FIXAMT, 12 * FIXAMT, MONS_THINGTENT);
				if (!g)
					return guys[i];
				g->parent = guys[i];
				g->facing = 14;
				g->mind = 2;
				g->mind1 = 128;
				for (j = 0; j < 3; j++)
				{
					g2 = g;
					// adding them all overlapping, they'll move into place when updated
					g = AddGuy(x + 64 * FIXAMT, y - 20 * FIXAMT, 12 * FIXAMT, MONS_THINGTENT);
					if (!g)
						return guys[i];
					g->parent = g2;
					g->facing = 14;
					g->mind = 2;
				}
				g2 = AddGuy(x + 64 * FIXAMT, y - 20 * FIXAMT, 12 * FIXAMT, MONS_THINGTENTTIP);
				if (!g2)
					return guys[i];
				g2->parent = g;
				g2->facing = 14;
				g2->mind = 2;
				// upper left (and last!) tentacle
				g = AddGuy(x - 64 * FIXAMT, y - 20 * FIXAMT, 12 * FIXAMT, MONS_THINGTENT);
				if (!g)
					return guys[i];
				g->parent = guys[i];
				g->facing = 10;
				g->mind = 3;
				g->mind1 = 128;
				for (j = 0; j < 3; j++)
				{
					g2 = g;
					// adding them all overlapping, they'll move into place when updated
					g = AddGuy(x - 64 * FIXAMT, y - 20 * FIXAMT, 12 * FIXAMT, MONS_THINGTENT);
					if (!g)
						return guys[i];
					g->parent = g2;
					g->facing = 10;
					g->mind = 3;
				}
				g2 = AddGuy(x - 64 * FIXAMT, y - 20 * FIXAMT, 12 * FIXAMT, MONS_THINGTENTTIP);
				if (!g2)
					return guys[i];
				g2->parent = g;
				g2->facing = 10;
				g2->mind = 3;
			}
			if (type == MONS_SPHINX)
			{
				// left arm
				g = AddGuy(x - 108 * FIXAMT, y + 22 * FIXAMT, 0, MONS_SPHXARM1);
				if (!g)
					return guys[i];
				g->parent = guys[i];
				// right arm
				g = AddGuy(x + 108 * FIXAMT, y + 22 * FIXAMT, 0, MONS_SPHXARM2);
				if (!g)
					return guys[i];
				g->parent = guys[i];
			}
			if (type == MONS_CENTIHEAD)
			{
				// add on 15 body parts
				g = guys[i];
				for (j = 0; j < 15; j++)
				{
					g2 = AddGuy(x, y, 0, MONS_CENTIBODY);
					if (!g2)
						return guys[i];
					g2->parent = g;
					g = g2;
				}
			}
			if (type == MONS_LOONYBOT)
			{
				g = AddGuy(x, y, 0, MONS_LOONYCORE);
				if (g)
				{
					// the core is the parent of them all
					guys[i]->parent = g;
				}
				g = AddGuy(x - 87 * FIXAMT, y, 48 * FIXAMT, MONS_LOONYGUN);
				if (g)
				{
					g->parent = guys[i];
					g->mind3 = 0;
				}
				g = AddGuy(x + 92 * FIXAMT, y, 48 * FIXAMT, MONS_LOONYGUN);
				if (g)
				{
					g->parent = guys[i];
					g->mind3 = 1;
				}
			}
			return guys[i];
		}
	return NULL;
}

void DeleteGuy2(Guy *g)
{
	int i;

	g->type = MONS_NONE;
	for (i = 0; i < maxGuys; i++)
		if (guys[i] && guys[i]->parent == g)
		{
			DeleteGuy2(guys[i]);
		}
}

void DeleteGuy(int x, int y, byte type)
{
	int i, j;

	for (i = 0; i < maxGuys; i++)
		if ((guys[i]->type == type) && (guys[i]->x == x) && (guys[i]->y == y))
		{
			guys[i]->type = MONS_NONE;
			for (j = 0; j < maxGuys; j++)
				if (guys[j]->parent == guys[i]) // kill all the kids too
					DeleteGuy2(guys[j]);
		}
}

void AddMapGuys(Map *map)
{
	int i, j;
	Guy *g;
	byte genType[MAX_MAPMONS];

	// add a nobody in case the player goes invisible
	nobody = AddGuy(0, 0, 0, MONS_NOBODY);

	// discover what type for each generator, and remove the template monster
	// from the map
	if (!editing)
	{
		for (i = 0; i < MAX_MAPMONS; i++)
			if (map->badguy[i].type == MONS_GENERATOR1 ||
					map->badguy[i].type == MONS_GENERATOR2 ||
					map->badguy[i].type == MONS_GENERATOR3 ||
					map->badguy[i].type == MONS_GENERATOR4)
			{
				genType[i] = MONS_NONE;
				for (j = 0; j < MAX_MAPMONS; j++)
					if (map->badguy[j].x == map->badguy[i].x && map->badguy[j].y == map->badguy[i].y + 1 &&
							map->badguy[j].type)
					{
						genType[i] = map->badguy[j].type;
						map->badguy[j].type = MONS_NONE;
					}
			}
	}

	for (i = 0; i < MAX_MAPMONS; i++)
		if (map->badguy[i].type)
		{
			if (map->badguy[i].type >= NUM_MONSTERS)
			{
				FatalError("Illegal monster on level!");
			}
			g = AddGuy((map->badguy[i].x * TILE_WIDTH + (TILE_WIDTH / 2)) << FIXSHIFT,
					(map->badguy[i].y * TILE_HEIGHT + (TILE_HEIGHT / 2)) << FIXSHIFT,
					0, map->badguy[i].type);
			if (!editing && (map->badguy[i].type == MONS_GENERATOR1 ||
					map->badguy[i].type == MONS_GENERATOR2 ||
					map->badguy[i].type == MONS_GENERATOR3 ||
					map->badguy[i].type == MONS_GENERATOR4))
			{
				if (g)
				{
					g->mind = genType[i];
					if (genType[i] == MONS_BOUAPHA || genType[i] == MONS_FRIENDLY || genType[i] == MONS_GOODTURRET ||
							genType[i] == MONS_WIZARD || genType[i] == MONS_GOODROBOT || genType[i] == MONS_GOODROBOT2)
						g->friendly = 1;
					else
						g->friendly = 0;
					g->facing = 0;
				}
			}
		}

	SetupTargets();
}

byte CheckHit(byte size, int xx, int yy, Guy *him)
{
	int x2, y2;

	// can't hit the unhittable
	if (MonsterFlags(him->type) & MF_NOHIT)
		return 0;

	xx -= size;
	yy -= size;

	x2 = him->x >> FIXSHIFT;
	y2 = him->y >> FIXSHIFT;

	if ((x2 + him->rectx2) >= xx && (y2 + him->recty2) >= yy && (x2 + him->rectx) <= (xx + size * 2) &&
			(y2 + him->recty) <= (yy + size * 2))
		return 1;

	return 0;
}

Guy *GetLastGuyHit(void)
{
	return guyHit;
}

byte FindVictim(int x, int y, byte size, int dx, int dy, byte damage, Map *map, world_t *world, byte friendly)
{
	int i;

	for (i = 0; i < maxGuys; i++)
		if (guys[i]->type && guys[i]->hp && (guys[i]->friendly != friendly))
		{
			if (CheckHit(size, x, y, guys[i]))
			{
				// stained glass knight blocking will reflect hits
				if (guys[i]->type == MONS_KNIGHT && guys[i]->seq == ANIM_A2 && guys[i]->frm > 2)
				{
					MakeSound(SND_GLASSBLOCK, x << FIXSHIFT, y << FIXSHIFT, SND_CUTOFF, 1200);
					ReflectShot();
				}
				else
					guys[i]->GetShot(dx, dy, damage, map, world);
				guyHit = guys[i];
				return 1;
			}
		}

	return 0;
}

// this doesn't quit when it finds one victim, it keeps going

byte FindVictims(int x, int y, byte size, int dx, int dy, byte damage, Map *map, world_t *world, byte friendly)
{
	int i;
	byte result = 0;

	for (i = 0; i < maxGuys; i++)
		if (guys[i]->type && guys[i]->hp && (guys[i]->friendly != friendly))
		{
			if (CheckHit(size, x, y, guys[i]))
			{
				guys[i]->GetShot(dx, dy, damage, map, world);
				guyHit = guys[i];
				result = 1;
			}
		}

	return result;
}

// Same as above, but won't hit someone who is currently in ouch mode (to avoid rapid rehits)

byte FindVictims2(int x, int y, byte size, int dx, int dy, byte damage, Map *map, world_t *world, byte friendly)
{
	int i;
	byte result = 0;

	for (i = 0; i < maxGuys; i++)
		if (guys[i]->type && guys[i]->hp && (guys[i]->friendly != friendly) && guys[i]->ouch == 0)
		{
			if (CheckHit(size, x, y, guys[i]))
			{
				guys[i]->GetShot(dx, dy, damage, map, world);
				guyHit = guys[i];
				result = 1;
			}
		}

	return result;
}

word LockOnEvil(int x, int y)
{
	int i;
	int bestRange, range;
	word bestguy;

	bestguy = 65535;
	bestRange = 320 + 240;

	for (i = 0; i < maxGuys; i++)
		if (guys[i]->type && guys[i]->hp && (guys[i]->friendly == 0) &&
				(!(MonsterFlags(guys[i]->type)&(MF_NOHIT | MF_INVINCIBLE))))
		{
			range = abs(x - (guys[i]->x >> FIXSHIFT)) + abs(y - (guys[i]->y >> FIXSHIFT));
			if ((range < bestRange) || (range < 320 + 240 && MGL_random(32) == 0))
			{
				bestguy = i;
				bestRange = range;
			}
		}

	return bestguy;
}

word LockOnEvil2(int x, int y)
{
	int i, j;
	int range;

	for (j = 0; j < 128; j++)
	{
		i = MGL_random(maxGuys);
		if (guys[i]->type && guys[i]->hp && (guys[i]->friendly == 0) &&
				(!(MonsterFlags(guys[i]->type)&(MF_NOHIT | MF_INVINCIBLE))))
		{
			range = abs(x - (guys[i]->x >> FIXSHIFT)) + abs(y - (guys[i]->y >> FIXSHIFT));
			if (range < 160 + 120)
			{
				return i;
			}
		}
	}
	return 65535;
}

word LockOnGood(int x, int y)
{
	int i;
	int bestRange, range;
	word bestguy;

	bestguy = 65535;
	bestRange = 320 + 240;

	for (i = 0; i < maxGuys; i++)
		if (guys[i]->type && guys[i]->hp && (guys[i]->friendly == 1) &&
				(!(MonsterFlags(guys[i]->type)&(MF_NOHIT | MF_INVINCIBLE))) && (guys[i]->type != MONS_BOUAPHA ||
				player.invisibility == 0))
		{
			range = abs(x - (guys[i]->x >> FIXSHIFT)) + abs(y - (guys[i]->y >> FIXSHIFT));
			if ((range < bestRange) || (range < 320 + 240 && MGL_random(32) == 0))
			{
				bestguy = i;
				bestRange = range;
			}
		}

	return bestguy;
}

word LockOnGood2(int x, int y)
{
	int i, j;
	int range;

	for (j = 0; j < 128; j++)
	{
		i = MGL_random(maxGuys);
		if (guys[i]->type && guys[i]->hp && (guys[i]->friendly == 1) &&
				(!(MonsterFlags(guys[i]->type) & MF_NOHIT)))
		{
			range = abs(x - (guys[i]->x >> FIXSHIFT)) + abs(y - (guys[i]->y >> FIXSHIFT));
			if (range < 160 + 120)
			{
				return i;
			}
		}
	}
	return 65535;
}

byte GetGuyPos(word guy, int *x, int *y)
{
	if (guy == 65535)
		return 0;
	if (guys[guy] == NULL)
		return 0;

	*x = guys[guy]->x;
	*y = guys[guy]->y;

	return 1;
}

byte MonsterExists(byte type)
{
	int i;

	for (i = 0; i < maxGuys; i++)
		if (guys[i]->type == type)
			return 1;

	return 0;
}

Guy *GetGuy(word w)
{
	return guys[w];
}

void HealGoodguy(byte amt)
{
	if (!goodguy)
		return;

	if (goodguy->hp + amt < 128)
		goodguy->hp += amt;
	else
		goodguy->hp = 128;
}

// this checks to see if there is any moss on the chosen tile (x,y in tile coords)

byte MossCheck(int x, int y)
{
	int i;

	for (i = 0; i < maxGuys; i++)
		if ((guys[i]) && ((guys[i]->type == MONS_MOSS) || (guys[i]->type == MONS_MOSSGRANDE) ||
				(guys[i]->type == MONS_MOSS2))
				&& (guys[i]->hp > 0) && ((guys[i]->x >> FIXSHIFT) / TILE_WIDTH == x) &&
				((guys[i]->y >> FIXSHIFT) / TILE_HEIGHT == y))
			return 1; // moss is in the way
	return 0;
}

void KillKids(Guy *g)
{
	int i;

	for (i = 0; i < maxGuys; i++)
		if (guys[i] && guys[i]->parent == g)
		{
			KillKids(guys[i]); // kill my children too
			if (guys[i]->hp > 0)
			{
				guys[i]->hp = 0;
				guys[i]->seq = ANIM_DIE;
				guys[i]->action = ACTION_BUSY;
				guys[i]->frm = 0;
				guys[i]->frmTimer = 0;
				guys[i]->frmAdvance = 64;
			}
		}
}

byte RaftNearby(void)
{
	int i, gx, gy, mx, my;

	gx = ((goodguy->x - goodguy->dx) >> FIXSHIFT) / TILE_WIDTH;
	gy = ((goodguy->y - goodguy->dy) >> FIXSHIFT) / TILE_HEIGHT;

	for (i = 0; i < maxGuys; i++)
		if (guys[i] && guys[i]->type == MONS_RAFT)
		{
			if (RangeToTarget(guys[i], goodguy) < 32 * FIXAMT && player.vehicle == 0)
			{
				guys[i]->mind = 1;
				player.vehicle = VE_RAFT;
				goodguy->parent = guys[i];
				goodguy->z = FIXAMT * 8;
				guys[i]->mind1 = 0; // acceleration

				mx = ((guys[i]->x) >> FIXSHIFT) / TILE_WIDTH;
				my = ((guys[i]->y) >> FIXSHIFT) / TILE_HEIGHT;

				if (gx < mx)
					guys[i]->facing = 0;
				else if (gy < my)
					guys[i]->facing = 1;
				else if (gx > mx)
					guys[i]->facing = 2;
				else
					guys[i]->facing = 3;

				goodguy->x = guys[i]->x;
				goodguy->y = guys[i]->y + 1;

				return 1;
			}
		}

	return 0;
}

void GuySwap(int sx, int sy, int width, int height, int dx, int dy)
{
	int i;

	sx *= (TILE_WIDTH * FIXAMT);
	sy *= (TILE_HEIGHT * FIXAMT);
	dx *= (TILE_WIDTH * FIXAMT);
	dy *= (TILE_HEIGHT * FIXAMT);
	width *= (TILE_WIDTH * FIXAMT);
	height *= (TILE_HEIGHT * FIXAMT);

	for (i = 0; i < maxGuys; i++)
	{
		if (guys[i]->x >= sx && guys[i]->y >= sy && guys[i]->x <= (sx + width) && guys[i]->y <= (sy + height))
		{
			// in target area, swap
			guys[i]->x += (-sx + dx);
			guys[i]->y += (-sy + dy);
		}
		else if (guys[i]->x >= dx && guys[i]->y >= dy && guys[i]->x <= (dx + width) && guys[i]->y <= (dy + height))
		{
			// in other target area, swap
			guys[i]->x += (-dx + sx);
			guys[i]->y += (-dy + sy);
		}
	}
}

void ShiftGuys(char dx, char dy, Map *map)
{
	// this is for when the map is moved around in the map editor (I,J,K,M)
	int i;

	for (i = 0; i < maxGuys; i++)
	{
		guys[i]->x += dx * TILE_WIDTH*FIXAMT;
		guys[i]->y += dy * TILE_HEIGHT*FIXAMT;
		if (guys[i]->x < 0)
			guys[i]->x += map->width * TILE_WIDTH * FIXAMT;
		if (guys[i]->x >= (map->width * TILE_WIDTH * FIXAMT))
			guys[i]->x -= map->width * TILE_WIDTH * FIXAMT;
		if (guys[i]->y < 0)
			guys[i]->y += map->height * TILE_HEIGHT * FIXAMT;
		if (guys[i]->y >= (map->height * TILE_HEIGHT * FIXAMT))
			guys[i]->y -= map->height * TILE_HEIGHT * FIXAMT;
	}
}

void AddPygmy(Map *map, world_t *world)
{
	int x, y, i;
	int cx, cy;
	byte t;
	Guy *g;

	// decide which type to summon
	switch (MGL_random(5)) {
		default:
		case 0:
		case 1:
			t = MONS_PYGMY;
			break;
		case 2:
		case 3:
			t = MONS_PYGMY2;
			break;
		case 4:
			// shamans least likely
			t = MONS_PYGMY3;
			break;
	}

	GetCamera(&cx, &cy);
	// 30 tries to end up in a legal spot
	for (i = 0; i < 30; i++)
	{
		x = MGL_random(map->width) * TILE_WIDTH + TILE_WIDTH / 2;
		y = MGL_random(map->height) * TILE_HEIGHT + TILE_HEIGHT / 2;
		// make sure it's offscreen
		if (x < cx - 340 || x > cx + 340 || y < cy - 260 || y > cy + 260)
		{
			g = AddGuy(x*FIXAMT, y*FIXAMT, 0, t);
			if (g && (!g->CanWalk(g->x, g->y, map, world)))
			{
				g->type = MONS_NONE;
			}
			else if (g)
			{
				// make him angry at bouapha
				g->mind = 1;
				g->mind1 = 240;
				return;
			}
		}
	}
}

void AddNinja(Map *map, world_t *world)
{
	int x, y, i;
	int cx, cy;
	Guy *g;

	GetCamera(&cx, &cy);
	// 30 tries to end up in a legal spot
	for (i = 0; i < 30; i++)
	{
		x = MGL_random(map->width) * TILE_WIDTH + TILE_WIDTH / 2;
		y = MGL_random(map->height) * TILE_HEIGHT + TILE_HEIGHT / 2;
		// make sure it's offscreen
		if (x < cx - 340 || x > cx + 340 || y < cy - 260 || y > cy + 260)
		{
			g = AddGuy(x*FIXAMT, y*FIXAMT, 0, MONS_GINGER);
			if (g && (!g->CanWalk(g->x, g->y, map, world)))
			{
				g->type = MONS_NONE;
			}
			else
				return;
		}
	}
}

byte ControlMind(Guy *me)
{
	int i;

	if (me->type == MONS_ROLLER || me->type == MONS_ROLLER2 || me->type == MONS_BURNER ||
			me->type == MONS_LAMP || me->type == MONS_EGGSAC || me->type == MONS_BOILER ||
			me->type == MONS_COFFIN || me->type == MONS_ALIENEGG || me->type == MONS_ROBOFACTORY ||
			me->type == MONS_ALIENEGG2 || me->type == MONS_GENERATOR1 || me->type == MONS_GENERATOR2 ||
			me->type == MONS_GENERATOR3 || me->type == MONS_GENERATOR4 || me->type == MONS_SNOWBLOW ||
			me->type == MONS_SNOWBALL || me->type == MONS_SNOWBALL2)
		return 0; // no mind to control

	// move up the chain to the top
	while (me->parent && me->parent->friendly == 0)
	{
		me = me->parent;
	}

	me->friendly = 1; // put him on our team!
	me->mindControl = 30 * 15; // for 15 seconds
	me->target = NULL;

	if (me->type == MONS_SUPERZOMBIE)
		return 1; // super zombies do not have their children converted, on the extremely
	// off chance that you mind control one while it is holding you
	for (i = 0; i < maxGuys; i++)
	{
		if (guys[i]->type != MONS_NONE && guys[i]->parent == me)
		{
			ControlMind(guys[i]);
		}
	}
	return 1;
}

void KillAllMonsters(byte type)
{
	int i;

	for (i = 0; i < maxGuys; i++)
	{
		if (guys[i]->type == type)
		{
			if (guys[i]->type == MONS_BOUAPHA)
			{
				SendMessageToGame(MSG_RESET, 0);
			}
			guys[i]->type = MONS_NONE;
			BlowUpGuy((guys[i]->x >> FIXSHIFT) - 32, (guys[i]->y >> FIXSHIFT) - 24,
					(guys[i]->x >> FIXSHIFT) + 32, (guys[i]->y >> FIXSHIFT) + 24, 0, 1);
		}
	}
}

void KillMonster(int x, int y)
{
	int i;

	for (i = 0; i < maxGuys; i++)
	{
		if (guys[i]->type != MONS_NONE && guys[i]->mapx == x && guys[i]->mapy == y)
		{
			if (guys[i]->type == MONS_BOUAPHA)
			{
				SendMessageToGame(MSG_RESET, 0);
			}
			guys[i]->type = MONS_NONE;
			BlowUpGuy((guys[i]->x >> FIXSHIFT) - 32, (guys[i]->y >> FIXSHIFT) - 24,
					(guys[i]->x >> FIXSHIFT) + 32, (guys[i]->y >> FIXSHIFT) + 24, 0, 1);
			return;
		}

	}
}

void ChangeAllMonsters(byte type, byte newtype)
{
	int i;
	byte f;
	Guy *g;

	for (i = 0; i < maxGuys; i++)
	{
		if (guys[i]->type == type)
		{
			f = guys[i]->friendly;
			guys[i]->type = MONS_NONE;
			g = AddGuy(guys[i]->x, guys[i]->y, guys[i]->z, newtype);
			if (g)
				g->friendly = f;
		}
	}
}

void ChangeMonster(int x, int y, byte type)
{
	int i;
	byte f;
	Guy *g;

	for (i = 0; i < maxGuys; i++)
	{
		if (guys[i]->type != MONS_NONE && guys[i]->mapx == x && guys[i]->mapy == y)
		{
			f = guys[i]->friendly;
			guys[i]->type = MONS_NONE;
			g = AddGuy(guys[i]->x, guys[i]->y, guys[i]->z, type);
			if (g)
				g->friendly = f;

			return;
		}

	}
}

void MoveKids(int dx, int dy, Guy *parent)
{
	int i;

	for (i = 0; i < maxGuys; i++)
		if (guys[i]->parent == parent)
		{
			guys[i]->x += dx;
			guys[i]->y += dy;
			MoveKids(dx, dy, guys[i]);
		}
}

byte SwapMe(int x, int y, byte size, Map *map)
{
	int i, dx, dy;
	Guy *g;

	if (!goodguy)
		return 0;

	for (i = 0; i < maxGuys; i++)
	{
		if (guys[i]->type && guys[i]->hp && guys[i]->type != MONS_BOUAPHA)
		{
			g = guys[i];
			if (CheckHit(size, x >> FIXSHIFT, y >> FIXSHIFT, guys[i]))
			{
				while (g->parent)
				{
					g = g->parent;
				}
				dx = (goodguy->mapx * TILE_WIDTH + TILE_WIDTH / 2) * FIXAMT - g->x;
				dy = (goodguy->mapy * TILE_HEIGHT + TILE_HEIGHT / 2) * FIXAMT - g->y;
				goodguy->x = (g->mapx * TILE_WIDTH + TILE_WIDTH / 2) * FIXAMT;
				goodguy->y = (g->mapy * TILE_HEIGHT + TILE_HEIGHT / 2) * FIXAMT;
				MoveKids(dx, dy, g);
				g->x += dx;
				g->y += dy;
				LightningBolt(goodguy->x, goodguy->y, g->x, g->y);
				LightningBolt(goodguy->x, goodguy->y, g->x, g->y);
				LightningBolt(goodguy->x, goodguy->y, g->x, g->y);
				map->BrightTorch(goodguy->x, goodguy->y, 20, 10);
				map->BrightTorch(g->x, g->y, 20, 10);
				MakeNormalSound(SND_LIGHTSON);
				return 1;
			}
		}
	}
	return 0;
}

void MonsterTally(void)
{
#ifdef LOG
	int i;

	for (i = 0; i < maxGuys; i++)
	{
		if (guys[i]->type)
		{
			fprintf(logFile, "GUY %d: Type %d:%s", i, guys[i]->type, MonsterName(guys[i]->type));
		}
	}
	fflush(logFile);
#endif
}

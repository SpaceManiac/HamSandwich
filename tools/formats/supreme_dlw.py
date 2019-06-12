# supreme_dlw.py
# used in: Supreme with Cheese
# file extension: .dlw
# Supreme world file format

from construct import *

# ----------------------------------------------------------------------
# constructs and adapters

class PackedStringAdapter(Adapter):
	def _encode(self, obj, ctx, path):
		return obj.encode('ascii').ljust(self.subcon.length, b'\0')

	def _decode(self, obj, ctx, path):
		idx = obj.find(0)
		if idx >= 0:
			obj = obj[:idx]
		return obj.decode('ascii')


def PackedString(sz):
	return PackedStringAdapter(Bytes(sz))


class RleBitmap(Construct):
	def __init__(self, func):
		super().__init__()
		self.func = func

	def isEncoded(self, method, row):
		return (method[row // 8] >> (row % 8)) & 1

	def _parse(self, stream, context, path):
		rows = []
		method = self.func(context) if callable(self.func) else self.func
		for row in range(24):
			if self.isEncoded(method, row):
				cols = bytearray(32)
				c = 0
				while c < 32:
					run, val = stream.read(2)
					for _ in range(run):
						cols[c] = val
						c += 1
				rows.append(bytes(cols))
			else:
				rows.append(stream.read(32))
		return rows

	def _build(self, obj, stream, context, path):
		method = self.func(context) if callable(self.func) else self.func
		for row in range(24):
			cols = obj[row]
			if self.isEncoded(method, row):
				value, run = cols[0], 1
				for pixel in cols[1:]:
					if pixel == value:
						run += 1
					else:
						stream.write(bytes([run, value]))
						value, run = pixel, 1
				stream.write(bytes([run, value]))
			else:
				stream.write(bytes(cols))

	def _sizeof(self, context):
		raise SizeofError


class RleLevel(Construct):
	def __init__(self, w, h):
		super().__init__()
		self.w = w
		self.h = h

	def _parse(self, stream, context, path):
		RUN = Int8sl
		width = self.w(context) if callable(self.w) else self.w
		height = self.h(context) if callable(self.h) else self.h
		rawTiles = []
		while len(rawTiles) < width * height:
			run = RUN.parse(stream.read(1))
			if run < 0:
				tile = levelTile.parse_stream(stream)
				for i in range(-run):
					rawTiles.append(tile)
			else:
				for i in range(run):
					rawTiles.append(levelTile.parse_stream(stream))
		rows = []
		for row in range(height):
			cols = []
			for col in range(width):
				cols.append(rawTiles[row * width + col])
			rows.append(cols)
		return rows

	def _build(self, obj, stream, context, path):
		RUN = Int8sl
		tiles = []
		for row in obj:
			for col in row:
				tiles.append(levelTile.build(col))

		runStart = sameStart = 0
		sameLength = diffLength = 1
		src = 0

		for pos, tile in enumerate(tiles):
			if pos == 0: continue
			if tile == tiles[src]: # the same
				sameLength += 1
				if sameLength == 128: # continue a same run
					stream.write(RUN.build(-127))
					stream.write(tiles[src])
					sameStart = runStart = pos
					sameLength = diffLength = 1
				elif sameLength == 2 and runStart != sameStart: # end a different run and start a same
					stream.write(RUN.build(sameStart - runStart))
					for i in range(sameStart - runStart):
						stream.write(tiles[runStart + i])
					runStart = sameStart
					diffLength = 1
			else: # different
				if sameLength < 2: # continue a different run
					sameLength = 1
					sameStart = pos
					src = pos
					diffLength += 1
					if diffLength == 128:
						stream.write(RUN.build(127))
						for i in range(127):
							stream.write(tiles[runStart + i])
						diffLength = 1
						runStart = pos
				else: # end a same run and start a different
					stream.write(RUN.build(-sameLength))
					stream.write(tiles[src])
					sameStart = runStart = pos
					sameLength = diffLength = 1
					src = pos
		if sameLength > 1: # finish up a same run
			stream.write(RUN.build(-sameLength))
			stream.write(tiles[src])
		else: # finish up a different run
			stream.write(RUN.build(diffLength))
			for i in range(diffLength):
				print(runStart, i, len(tiles))
				stream.write(tiles[runStart + i])

	def _sizeof(self, context):
		raise SizeofError


class ItemContainer(Construct):
	def __init__(self, func):
		super().__init__()
		self.func = func

	def _parse(self, stream, context, path):
		result = []
		itemId = 0
		count = self.func(context) if callable(self.func) else self.func
		for i in range(count):
			if itemId != 255:
				itemId = ord(stream.read(1))
				data = item.parse_stream(stream)
				data.itemId = itemId
				result.append(data)
			else:
				data = item.parse_stream(stream)
				data.itemId = 255
				result.append(data)
		return result

	def _build(self, obj, stream, context, path):
		part = 0
		for data in obj:
			if data.itemId == 255:
				if part == 0:
					stream.write(chr(255))
					part = 1
				stream.write(item.build(data))
			else:
				stream.write(chr(data.itemId))
				stream.write(item.build(data))

	def _sizeof(self, context):
		raise SizeofError


class ItemDropAdapter(Adapter):
	def _encode(self, obj, ctx, path):
		ival = int(obj)
		return bytes([int(obj % 1 * 25), int(obj)])

	def _decode(self, obj, ctx, path):
		return obj[1] + obj[0] / 256.0


ItemDrop = ItemDropAdapter(Bytes(2))


# ----------------------------------------------------------------------
# structures
monster = Struct(
	"x" / Int8ul,
	"y" / Int8ul,
	"type" / Int8ul,
	"item" / Int8ul,
)

trigger = Struct(
	"parameter" / Int8ul,
	"type" / Int8ul,
	"x" / Int8ul,
	"y" / Int8ul,
	"index1" / Int32ul,
	"index2" / Int32ul,
)

effect = trigger + Struct(
	"text" / PackedString(32),
)

special = Struct(
	"x" / Int8ul,
	"y" / Int8ul,
	"uses" / Int8ul,
	"length" / BitStruct(
		"effects" / BitsInteger(5),
		"triggers" / BitsInteger(3),
	),
	"trigger" / trigger[this.length.triggers],
	"effect" / effect[this.length.effects],
)

levelTile = Struct(
	"floor" / Int16ul,
	"wall" / Int16ul,
	"item" / Int8ul,
	"light" / Int8sl,
)

level = Struct(
	"width" / Int8ul,
	"height" / Int8ul,
	"name" / PackedString(32),
	"song" / PackedString(32),
	"monster" / PrefixedArray(Int8ul, monster),
	"special" / PrefixedArray(Int8ul, special),
	"flags" / BitStruct(
		"underwater" / Flag,
		"starry" / Flag,
		"lantern" / Flag,
		"torch" / Flag,
		"secret" / Flag,
		"hub" / Flag,
		"raining" / Flag,
		"snowing" / Flag,
		"reserved5" / Flag,
		"reserved4" / Flag,
		"reserved3" / Flag,
		"reserved2" / Flag,
		"reserved1" / Flag,
		"reserved0" / Flag,
		"stealth" / Flag,
		"underlava" / Flag,
	),
	"brains" / Int16ul,
	"candles" / Int16ul,
	"item_drop" / ItemDrop,
	"tiles" / RleLevel(this.width, this.height),
)

def should_rle(row):
	size = 0
	last = -1
	for c in row:
		if c != last:
			last = c
			size += 2
	return size < len(row)

def compute_rle_method(ctx):
	method = [0, 0, 0]
	entry = 0
	bit = 1
	for row in ctx.bitmap:
		if should_rle(row):
			method[entry] |= bit
		bit *= 2
		if bit >= 256:
			entry += 1
			bit = 1

	return method

tile_image = Struct(
	"method" / Default(Bytes(3), compute_rle_method),
	"bitmap" / RleBitmap(this.method),
)

tile_data = Struct(
	"flags" / BitStruct(
		"animate" / Flag,
		"canpushon" / Flag,
		"pushable" / Flag,
		"lava" / Flag,
		"water" / Flag,
		"muddy" / Flag,
		"icy" / Flag,
		"impassible" / Flag,
		"bouncy" / Flag,
		"enemyProof" / Flag,
		"ghostProof" / Flag,
		"bunnyPath" / Flag,
		"minecartPath" / Flag,
		"transparentRoof" / Flag,
		"animateHit" / Flag,
		"animateStep" / Flag,
	),
	"next_tile" / Int16ul,
)

item = Struct(
	"name" / PackedString(32),
	"offset_x" / Int8sl,
	"offset_y" / Int8sl,
	"sprite" / Int16ul,
	"fromColor" / Int8ul,
	"toColor" / Int8ul,
	"light" / Int8sl,
	"rarity" / Int8ul,
	"flags" / BitStruct(
		Padding(1),
		"useTileGraphic" / Flag,
		"loonyColor" / Flag,
		"pickup" / Flag,
		"bulletproof" / Flag,
		"impassible" / Flag,
		"glowing" / Flag,
		"shadow" / Flag,
		Padding(8)
	),
	"themes" / BitStruct(
		"crate" / Flag,
		"rock" / Flag,
		"tree" / Flag,
		"door" / Flag,
		"bulletproof" / Flag,
		"obstacle" / Flag,
		"decoration" / Flag,
		"pickup" / Flag,
		"chair" / Flag,
		"entrance" / Flag,
		"food" / Flag,
		"collectible" / Flag,
		"key" / Flag,
		"powerup" / Flag,
		"weapon" / Flag,
		"sign" / Flag,
		Padding(7),
		"custom" / Flag,
		Padding(8)
	),
	"trigger" / BitStruct(
		"always" / Flag,
		"minecart" / Flag,
		"machete" / Flag,
		"friendbump" / Flag,
		"enemybump" / Flag,
		"playerbump" / Flag,
		"shoot" / Flag,
		"pickup" / Flag,
		Padding(8)
	),
	"effect" / Int8ul,
	"effect_data" / Int32sl,
	"message" / PackedString(64),
	"sound" / Int16ul,
)

sound = Struct(
	"sound_id" / Int16ul,
	"name" / PackedString(32),
	"theme" / BitStruct(
		Padding(2),
		"custom" / Flag,
		"vocal" / Flag,
		"effect" / Flag,
		"monster" / Flag,
		"player" / Flag,
		"intface" / Flag,
		Padding(8)
	),
	"data_size" / Int32ul,
	"data" / Bytes(this.data_size),
)

def compute_total_points(ctx):
	return sum(100 for level in ctx.level if not level.flags.hub)

supreme_dlw = Struct(
	Constant(b"SUPREME!"),
	"author" / PackedString(32),
	"name" / PackedString(32),
	"level_count" / Default(Int8ul, len_(this.level)),
	"total_points" / Default(Int32ul, compute_total_points),
	"tile_count" / Default(Int16ul, len_(this.tile_image)),
	"tile_image" / tile_image[this.tile_count],
	"tile_data" / tile_data[this.tile_count],
	"level" / level[this.level_count],
	"item_count" / Default(Int16ul, len_(this.items)),
	"items" / ItemContainer(this.item_count),
	"sound" / PrefixedArray(Int32ul, sound),
)

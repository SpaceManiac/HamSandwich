-- Main script of JSP import/export.
-- In a separate file so it can be developed without restarting aseprite.
local origin_slice_none <const> = "(top-left corner)"
local origin_slice_imported <const> = "Origin"
local palette_id <const> = "Dr. Lunatic"
local font_max_chars <const> = 128

-------------------------------------------------------------------------------
-- Helpers

-- Export path cache.
-- Can't use table because Aseprite assumes all keys are valid identifiers when
-- serializing, rather than escaping with `["x"] = ` syntax.
local function read_export_path(plugin, filename)
	if not plugin.preferences.export_paths then
		return
	end
	for _, entry in ipairs(plugin.preferences.export_paths) do
		if entry[1] == filename then
			return entry[2]
		end
	end
end

local function write_export_path(plugin, filename, value)
	if not plugin.preferences.export_paths then
		plugin.preferences.export_paths = {}
	end
	for _, entry in ipairs(plugin.preferences.export_paths) do
		if entry[1] == filename then
			entry[2] = value
			return
		end
	end
	table.insert(plugin.preferences.export_paths, { filename, value })
end

local function deleteInvisible(sprite, layers)
	for i = #layers, 1, -1 do
		local layer = layers[i]
		if not layer.isVisible then
			sprite:deleteLayer(layer)
		elseif layer.layers then
			deleteInvisible(sprite, layer.layers)
		end
	end
end

-------------------------------------------------------------------------------
-- Import

local function do_import_jsp(file)
	-- Read frame count
	local count = string.unpack("<I2", file:read(2))
	local headers = {}
	local min_x, min_y, max_x, max_y = math.maxinteger, math.maxinteger, math.mininteger, math.mininteger
	for i = 1, count do
		-- Read frame header
		local width, height, ofs_x, ofs_y, size = string.unpack("<I2I2i2i2I4", file:read(16))
		headers[i] = {
			width = width,
			height = height,
			ofs_x = ofs_x,
			ofs_y = ofs_y,
			size = size,
		}
		min_x = math.min(min_x, -ofs_x)
		min_y = math.min(min_y, -ofs_y)
		max_x = math.max(max_x, -ofs_x + width)
		max_y = math.max(max_y, -ofs_y + height)
	end

	-- Prepare sprite
	local sprite = Sprite(max_x - min_x, max_y - min_y, ColorMode.INDEXED)
	sprite:setPalette(Palette { fromResource = palette_id })
	sprite.properties.jspedit = {
		origin_slice = origin_slice_imported,
	}
	sprite:deleteCel(sprite.cels[1])

	-- Read cels
	for frm = 1, count do
		local image = Image(headers[frm].width, headers[frm].height, ColorMode.INDEXED)
		-- Read frame data
		local data = file:read(headers[frm].size)
		local src, x, y = 1, 0, 0
		while src < #data do
			local byte = string.unpack("<B", data, src)
			if byte & 128 ~= 0 then
				-- transparent run
				x = x + (byte & 127)
				src = src + 1
			else
				-- solid run
				src = src + 1
				for j = 1, byte do
					image:drawPixel(x, y, string.unpack("<B", data, src))
					x = x + 1
					src = src + 1
				end
			end
			if x >= headers[frm].width then
				x = 0
				y = y + 1
			end
		end

		local frame = sprite.frames[frm] or sprite:newEmptyFrame()
		sprite:newCel(sprite.layers[1], frame, image, Point(-headers[frm].ofs_x - min_x, -headers[frm].ofs_y - min_y))
	end

	-- Create slice to mark origin
	local slice = sprite:newSlice(Rectangle(-min_x - 16, -min_y - 12, 32, 24))
	slice.name = origin_slice_imported
	slice.color = Color { r = 0, g = 255, b = 0 }

	-- Success
	return sprite
end

local function do_import_jft(file)
	-- Read header
	local
		num_chars,
		first_char,
		height,
		space_size,
		gap_size,
		gap_height,
		data_size
	= string.unpack("<BBBBBBxxI4", file:read(528))

	-- Prepare sprite
	local sprite = Sprite(1, height, ColorMode.INDEXED)
	sprite:setPalette(Palette { fromResource = palette_id })
	sprite.properties.jftedit = {
		first_char = first_char,
		space_size = space_size,
		gap_size = gap_size,
		gap_height = gap_height,
	}
	sprite:deleteCel(sprite.cels[1])

	-- Read cels
	for chr = 1, num_chars do
		local width = string.unpack("<B", file:read(1))
		if width > sprite.width then
			sprite.width = width
		end

		local image = Image(width, height, ColorMode.INDEXED)
		for y = 0, height - 1 do
			for x = 0, width - 1 do
				local pixel = string.unpack("<B", file:read(1))
				if pixel ~= 0 then
					image:drawPixel(x, y, pixel)
				end
			end
		end

		local frame = sprite.frames[chr] or sprite:newEmptyFrame()
		sprite:newCel(sprite.layers[1], frame, image)
	end

	-- Success
	return sprite
end

local function import(plugin)
	local dlg = Dialog("Import JSP or JFT")

	dlg:file {
		id = "source",
		label = "Source:",
		filetypes = { "jsp", "jft" },
		load = true,
		focus = true,
	}

	dlg:button {
		text = "Import",
		onclick = function()
			local source = dlg.data.source
			if not source then
				return
			end
			-- Open file and route to relevant parser
			local file <close> = io.open(source, "rb")
			if not file then
				return
			end
			local sprite
			if source:match(".jsp$") then
				sprite = do_import_jsp(file)
			elseif source:match(".jft$") then
				sprite = do_import_jft(file)
			else
				return
			end
			-- Success
			sprite.filename = app.fs.fileTitle(source)
			app.frame = 1
			dlg:close()
		end
	}

	dlg:show()
end

local function export(plugin)
	local site = app.site
	if not site.sprite then
		return
	end

	local sprite_jspedit = site.sprite.properties.jspedit or {}

	local slices = { origin_slice_none }
	for i = 1, #site.sprite.slices do
		slices[i + 1] = site.sprite.slices[i].name
	end

	local dlg = Dialog("Export JSP")

	dlg:file {
		id = "destination",
		label = "Destination:",
		filetypes = { "jsp" },
		filename = read_export_path(plugin, site.sprite.filename),
		save = true,
		focus = true,
	}

	dlg:combobox {
		id = "origin_slice",
		label = "Origin slice",
		options = slices,
		option = sprite_jspedit.origin_slice,
	}

	dlg:button {
		text = "Export",
		onclick = function()
			local destination = dlg.data.destination
			if not destination or destination == "" then
				return
			end
			local slice_name = dlg.data.origin_slice

			local origin_x, origin_y = 0, 0
			local jspedit_props_changed = false
			if slice_name == origin_slice_none then
				if sprite_jspedit.origin_slice ~= nil then
					sprite_jspedit.origin_slice = nil
					jspedit_props_changed = true
				end
			else
				if sprite_jspedit.origin_slice ~= slice_name then
					sprite_jspedit.origin_slice = slice_name
					jspedit_props_changed = true
				end
				for _, slice in ipairs(site.sprite.slices) do
					if slice.name == slice_name then
						origin_x = math.floor(slice.bounds.x + slice.bounds.width / 2)
						origin_y = math.floor(slice.bounds.y + slice.bounds.height / 2)
						break
					end
				end
			end

			-- Duplicate sprite so we can flatten it.
			local sprite = Sprite(site.sprite)
			deleteInvisible(sprite, sprite.layers)
			sprite:flatten()
			sprite:setPalette(Palette { fromResource = palette_id })

			local layer = sprite.layers[1]
			local frames = {}
			for frm = 1, #sprite.frames do
				-- Get image for frame
				local cel = layer:cel(frm)
				local image, ofs_x, ofs_y
				if cel then
					image = cel.image
					ofs_x = origin_x - cel.position.x
					ofs_y = origin_y - cel.position.y
				else
					image = Image(1, 1, ImageMode.INDEXED)
					ofs_x, ofs_y = 0, 0
				end

				-- RLE-encode image
				local data = ""
				for y = 0, image.height - 1 do
					local x = 0
					while x < image.width do
						local pixel = image:getPixel(x, y)
						if pixel == 0 then
							-- Begin transparent run
							x = x + 1
							local run = 1
							while x < image.width and run < 127 and image:getPixel(x, y) == 0 do
								x = x + 1
								run = run + 1
							end
							data = data .. string.pack("B", run | 128)
						else
							-- Begin solid run
							local pixels = string.pack("B", pixel)
							x = x + 1
							local run = 1
							while x < image.width and run < 127 and image:getPixel(x, y) ~= 0 do
								pixels = pixels .. string.pack("B", image:getPixel(x, y))
								x = x + 1
								run = run + 1
							end
							data = data .. string.pack("B", run) .. pixels
						end
					end
				end

				frames[frm] = {
					width = image.width,
					height = image.height,
					ofs_x = ofs_x,
					ofs_y = ofs_y,
					data = data,
				}
			end

			-- Write headers and data
			local f <close> = io.open(destination, "wb")
			if not f then
				return
			end
			f:write(string.pack("<I2", #frames))
			for frm = 1, #frames do
				f:write(string.pack(
					"<I2I2i2i2I4I4",
					frames[frm].width,
					frames[frm].height,
					frames[frm].ofs_x,
					frames[frm].ofs_y,
					#frames[frm].data,
					0
				))
			end
			for frm = 1, #frames do
				f:write(frames[frm].data)
			end

			-- success, close dialog and commit preferences
			sprite:close()
			dlg:close()
			app.sprite = site.sprite
			if jspedit_props_changed then
				site.sprite.properties.jspedit = sprite_jspedit
			end
			write_export_path(plugin, site.sprite.filename, destination)
		end,
	}

	dlg:show()
end

local function export_jft(plugin)
	local site = app.site
	if not site.sprite then
		return
	end

	local sprite_jftedit = site.sprite.properties.jftedit or {}

	local dlg = Dialog("Export JFT")

	dlg:file {
		id = "destination",
		label = "Destination:",
		filetypes = { "jft" },
		filename = read_export_path(plugin, site.sprite.filename),
		save = true,
		focus = true,
	}

	dlg:number {
		id = "first_char",
		label = "First character code:",
		text = tostring(sprite_jftedit.first_char or 0),
	}

	dlg:number {
		id = "space_size",
		label = "Space width:",
		text = tostring(sprite_jftedit.space_size or 0),
	}

	dlg:number {
		id = "gap_size",
		label = "Gap between characters:",
		text = tostring(sprite_jftedit.gap_size or 0),
	}

	dlg:number {
		id = "gap_height",
		label = "Line height:",
		text = tostring(sprite_jftedit.gap_height or 0),
	}

	dlg:button {
		text = "Export",
		onclick = function()
			local destination = dlg.data.destination
			if not destination or destination == "" then
				return
			end

			-- Duplicate sprite so we can flatten it.
			local sprite = Sprite(site.sprite)
			deleteInvisible(sprite, sprite.layers)
			sprite:flatten()
			sprite:setPalette(Palette { fromResource = palette_id })

			-- Fetch properties
			local first_char = dlg.data.first_char
			local space_size = dlg.data.space_size
			local gap_size = dlg.data.gap_size
			local gap_height = dlg.data.gap_height

			local height = sprite.height
			local num_chars = #sprite.frames
			if num_chars > font_max_chars then
				return error("Font cannot have more than " .. tostring(font_max_chars) .. " characters")
			end

			local layer = sprite.layers[1]
			local data = ""
			for chr = 1, num_chars do
				-- Get image for frame
				local cel = layer:cel(chr)
				if cel then
					-- Write width and pixels
					local image = cel.image
					local start_x = cel.bounds.x
					local start_y = cel.bounds.y
					local width = image.width
					data = data .. string.pack("<B", width)
					for y = 0, height - 1 do
						for x = 0, width - 1 do
							local pixel = image:getPixel(x - start_x, y - start_y)
							if pixel >= 256 then
								pixel = 0
							end
							data = data .. string.pack("<B", pixel)
						end
					end
				else
					-- Write zero-width missing character
					data = data .. string.pack("<B", 0)
				end
			end

			-- Write header and data
			local f <close> = io.open(destination, "wb")
			if not f then
				return
			end
			f:write(string.pack(
				"<BBBBBBxxI4c516",
				num_chars,
				first_char,
				height,
				space_size,
				gap_size,
				gap_height,
				#data,
				""
			))
			f:write(data)
			f:close()

			-- success, close dialog and commit preferences
			sprite:close()
			dlg:close()
			if first_char ~= sprite_jftedit.first_char or space_size ~= sprite_jftedit.space_size or gap_size ~= sprite_jftedit.gap_size or sprite_jftedit ~= sprite_jftedit.gap_height then
				sprite_jftedit.first_char = first_char
				sprite_jftedit.space_size = space_size
				sprite_jftedit.gap_size = gap_size
				sprite_jftedit.gap_height = gap_height
				site.sprite.properties.jftedit = sprite_jftedit
			end
			app.sprite = site.sprite
			write_export_path(plugin, site.sprite.filename, destination)
		end,
	}

	dlg:show()
end

return {
	import = import,
	export = export,
	export_jft = export_jft,
}

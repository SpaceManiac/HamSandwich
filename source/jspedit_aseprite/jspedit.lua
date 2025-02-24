-- Main script of JSP import/export.
-- In a separate file so it can be developed without restarting aseprite.
local origin_slice_none <const> = "(top-left corner)"
local origin_slice_imported <const> = "Origin"

local function import(plugin)
	local dlg = Dialog("Import JSP")

	dlg:file {
		id = "source",
		label = "Source:",
		filetypes = { "jsp" },
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

			local f <close> = io.open(source, "rb")
			if not f then
				return
			end
			-- Read frame count
			local count = string.unpack("<I2", f:read(2))
			local headers = {}
			local min_x, min_y, max_x, max_y = math.maxinteger, math.maxinteger, math.mininteger, math.mininteger
			for i = 1, count do
				-- Read frame header
				local width, height, ofs_x, ofs_y, size = string.unpack("<I2I2i2i2I4", f:read(16))
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

			local sprite = Sprite(max_x - min_x, max_y - min_y, ColorMode.INDEXED)
			sprite:setPalette(Palette { fromResource = "Dr. Lunatic" })
			sprite.properties.jspedit = {
				export_filepath = source,
				origin_slice = origin_slice_imported,
			}

			sprite:deleteCel(sprite.cels[1])
			for frm = 1, count do
				local image = Image(headers[frm].width, headers[frm].height, ColorMode.INDEXED)
				-- Read frame data
				local data = f:read(headers[frm].size)
				local src, x, y = 1, 0, 0
				while src < #data do
					local byte = string.byte(data, src)
					if byte & 128 ~= 0 then
						-- transparent run
						x = x + (byte & 127)
						src = src + 1
					else
						-- solid run
						src = src + 1
						for j = 1, byte do
							image:drawPixel(x, y, string.byte(data, src))
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
			local slice = sprite:newSlice(Rectangle(-min_x, -min_y, 16, 16))
			slice.name = origin_slice_imported
			slice.color = Color { r = 0, g = 255, b = 0 }

			-- Success, close
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
		filename = sprite_jspedit.export_filepath,
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
			local slice_name = dlg.data.origin_slice

			local origin_x, origin_y = 0, 0
			if slice_name == origin_slice_none then
				sprite_jspedit.origin_slice = nil
			else
				sprite_jspedit.origin_slice = slice_name
				for _, slice in ipairs(site.sprite.slices) do
					if slice.name == slice_name then
						origin_x, origin_y = slice.bounds.x, slice.bounds.y
						break
					end
				end
			end

			-- Duplicate sprite so we can flatten it.
			local sprite = Sprite(site.sprite)
			sprite:flatten()
			sprite:setPalette(Palette { fromResource = "Dr. Lunatic" })
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
			sprite_jspedit.export_filepath = destination
			site.sprite.properties.jspedit = sprite_jspedit
			app.sprite = site.sprite
		end,
	}

	dlg:show()
end

return {
	import = import,
	export = export,
}

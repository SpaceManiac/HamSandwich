-- emscripten.lua
-- Emscripten/EMCC toolset adapter for Premake

local p = premake
local clang = p.tools.clang
local emcc

-- New configurators.
p.api.register {
	name = "installers",
	scope = "project",
	kind = "table",
	tokens = true
}

p.api.register {
	name = "assetdirs",
	scope = "project",
	kind = "list:string",
	tokens = true
}

p.api.register {
	name = "webfiles",
	scope = "project",
	kind = "list:table",
	tokens = true
}

p.api.register {
	name = "web_title",
	scope = "project",
	kind = "string",
	tokens = true
}

-- Global filters.
filter { "toolset:emcc", "kind:WindowedApp or ConsoleApp" }
	-- Without an extension, emcc produces only bytecode. We want the WASM and
	-- JavaScript output. If this is ".html", emcc will also produce a generic
	-- template HTML page, but we want to use our own HTML page instead.
	targetextension ".js"
filter { "toolset:emcc", "kind:StaticLib" }
	-- emar seems to like to add duplicate copies into the .a file, so delete
	-- it beforehand.
	prelinkcommands {
		"$(SILENT) rm -f %{cfg.buildtarget.relpath}"
	}
filter {}

-- Add emcc to the list of allowed tools.
table.insert(p.option.get("cc").allowed, { "emcc", "Emscripten (emcc/em++)" })

-- emcc is based largely on clang.
emcc = table.merge(clang, {
	-- The compiler names are changed.
	tools = {
		cc = "emcc",
		cxx = "em++",
		ar = "emar",
	},

	gettoolname = function(cfg, tool)
		return emcc.tools[tool]
	end,

	-- Ports table: transparently transform certain "links" names into the
	-- emcc-specific flags to activate those ports.
	ports = {
		SDL2main = { },  -- No flags, but don't link it.
		SDL2 = { compile = {"-s USE_SDL=2"}, link = {"-s USE_SDL=2"} },
		SDL2_mixer = { compile = {"-s USE_SDL_MIXER=2"} },
		SDL2_image = { compile = {"-s USE_SDL_IMAGE=2", "-s SDL2_IMAGE_FORMATS=['bmp']"}, link = {"-s USE_SDL_IMAGE=2", "-s SDL2_IMAGE_FORMATS=['bmp']"} },
		z = { compile = {"-s USE_ZLIB=1"}, link = {"-s USE_ZLIB=1"} }
	},

	getports = function(cfg, subtab)
		local ports = {}

		for _, lib in ipairs(cfg.links) do
			local flags = emcc.ports[lib]
			if flags then
				flags = flags[subtab]
				if flags then
					table.move(flags, 1, #flags, 1 + #ports, ports)
				end
			end
		end

		return ports
	end,

	-- CFLAGS, CXXFLAGS, and LDFLAGS should all have the ports flags.
	getcflags = function(cfg)
		return table.join(clang.getcflags(cfg), emcc.getports(cfg, "compile"))
	end,

	getcxxflags = function(cfg)
		return table.join(clang.getcxxflags(cfg), emcc.getports(cfg, "compile"))
	end,

	getldflags = function(cfg)
		-- Emscripten requires -g to also be passed to the link command.
		local ldflags = table.join(clang.getldflags(cfg), emcc.getports(cfg, "link"))
		if cfg.symbols == "On" then
			table.insert(ldflags, "-g")
		end
		return ldflags
	end,

	-- Those "links" entries which are replaced by ports should not be
	-- transformed into `-l` flags.
	getlinks = function(cfg)
		local flags = {}
		for _, flag in ipairs(clang.getlinks(cfg)) do
			if flag:sub(1, 2) ~= "-l" or not emcc.ports[flag:sub(3)] then
				table.insert(flags, flag)
			end
		end
		table.insert(flags, "-lidbfs.js")
		return flags
	end,
})

p.tools.emcc = emcc

emscripten = {}

-- Asset packfile target
function emscripten._file_packager_makesettings(prj)
	if #prj.assetdirs > 0 then
		local data = "%{cfg.targetdir}/%{prj.name}.data"
		local data_js = "%{cfg.objdir}/%{prj.name}.data.js"
		local data_d = data_js .. ".d"

		local build_command = "python3"
			.. " ../tools/build/file_packager_deps.py"
			.. " " .. data
			.. " --js-output=" .. data_js
			.. " --from-emcc"  -- Hack to disable "Remember to..." output
			--.. " --use-preload-plugins"
			.. " --preload"  -- List of paths follows
		for _, dir in ipairs(prj.assetdirs) do
			build_command = build_command .. " '../" .. dir .. "@'"
		end

		local output = "# Begin emcc file_packager handling\n"
		output = output .. "all:  # cheap hack\n"
		output = output .. "LDDEPS += " .. data .. "\n"
		output = output .. "ALL_LDFLAGS += --pre-js " .. data_js .. " -s FORCE_FILESYSTEM=1\n"
		output = output .. data .. ":\n"
		output = output .. "\t@echo %{prj.name}.data\n"
		output = output .. "\t$(SILENT) " .. build_command .. "\n"
		output = output .. "-include " .. data_d .. "\n"
		output = output .. "# End emcc file_packager handling\n"
		return output
	end
end

filter "toolset:emcc"
	makesettings "%{emscripten._file_packager_makesettings(prj)}"
filter {}

-- HTML assets
function emscripten._encode_metadata(prj)
	local meta = {
		title = prj.web_title or prj.android_appname,
		projectName = prj.name,
		installers = prj.installers,
		hasAssets = #prj.assetdirs > 0,
	}
	local str, err = json.encode(meta)
	if err then
		print(err)
		return "{}"
	else
		return str
	end
end

function emscripten.decode_webfiles(webfiles)
	local files = {}
	for _, pattern in ipairs(webfiles) do
		if type(pattern) == "table" then
			for outname, inname in pairs(pattern) do
				files[outname] = inname
			end
		else
			for _, name in ipairs(os.matchfiles(pattern)) do
				files[path.getname(name)] = name
			end
		end
	end
	return files
end

function emscripten._web_asset_makesettings(prj)
	if prj.kind == "ConsoleApp" or prj.kind == "WindowedApp" then
		p.generate(prj, prj.name .. ".meta.json", function(prj)
			p.w(emscripten._encode_metadata(prj))
		end)

		local files = emscripten.decode_webfiles(prj.webfiles)
		local output = "# Begin emcc web asset handling\n"
		for outname, inname in pairs(files) do
			inname = "../../" .. inname

			output = output .. "all: %{cfg.targetdir}/" .. outname .. "\n"
			if outname == "index.html" then
				output = output .. "%{cfg.targetdir}/" .. outname .. ': ' .. inname .. ' %{wks.location}/%{prj.name}.meta.json ../../tools/build/embed-metadata.py\n'
				output = output .. "\t@echo " .. outname .. "\n"
				output = output .. '\t$(SILENT) python3 ../../tools/build/embed-metadata.py __HAMSANDWICH_METADATA__ %{wks.location}/%{prj.name}.meta.json <"$<" >"$@"\n'
			else
				output = output .. "%{cfg.targetdir}/" .. outname .. ': ' .. inname .. '\n'
				output = output .. "\t@echo " .. outname .. "\n"
				output = output .. '\t$(SILENT) cp "$<" "$@"\n'
			end
		end
		output = output .. "# End emcc web asset handling\n"
		return output
	end
end

filter "toolset:emcc"
	makesettings "%{emscripten._web_asset_makesettings(prj)}"
filter {}

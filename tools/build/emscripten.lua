-- emscripten.lua
-- Emscripten/EMCC toolset adapter for Premake

local p = premake
local clang = p.tools.clang
local emcc

-- Global filters.
filter { "toolset:emcc", "kind:WindowedApp or ConsoleApp" }
	-- Without an extension, emcc produces only bytecode. We want the WASM and
	-- JavaScript output. If this is ".html", emcc will also produce a generic
	-- template HTML page, but we want to use our own HTML page instead.
	targetextension ".js"
filter {}

-- Add emcc to the list of allowed tools.
table.insert(p.option.get("cc").allowed, { "emcc", "Emscripten (emcc/em++)" })

-- emcc is based largely on clang.
emcc = table.merge(clang, {
	-- The compiler names are changed.
	tools = {
		cc = "emcc",
		cxx = "em++",
		ar = "ar",
	},

	gettoolname = function(cfg, tool)
		return emcc.tools[tool]
	end,

	-- Ports table: transparently transform certain "links" names into the
	-- emcc-specific flags to activate those ports.
	ports = {
		SDL2main = {},  -- No flags, but don't link it.
		SDL2 = { "-s USE_SDL=2" },
		SDL2_mixer = { "-s USE_SDL_MIXER=2" },
		SDL2_image = { "-s USE_SDL_IMAGE=2" },
	},

	getports = function(cfg)
		local ports = {}

		for _, lib in ipairs(cfg.links) do
			local flags = emcc.ports[lib]
			if flags then
				table.move(flags, 1, #flags, 1 + #ports, ports)
			end
		end

		return ports
	end,

	-- CFLAGS, CXXFLAGS, and LDFLAGS should all have the ports flags.
	getcflags = function(cfg)
		return table.join(clang.getcflags(cfg), emcc.getports(cfg))
	end,

	getcxxflags = function(cfg)
		return table.join(clang.getcxxflags(cfg), emcc.getports(cfg))
	end,

	getldflags = function(cfg)
		-- Emscripten requires -g to also be passed to the link command.
		local ldflags = table.join(clang.getldflags(cfg), emcc.getports(cfg))
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
			-- Emscripten's upstream-LLVM target doesn't seem to link .a files
			-- properly, so instead link all the .o files individually.
			if flag:sub(-2) == ".a" then
				table.insert(flags, "$(wildcard " .. flag:gsub("/[^/]*$", "") .. "/obj/*.o)")
			elseif flag:sub(1, 2) ~= "-l" or not emcc.ports[flag:sub(3)] then
				table.insert(flags, flag)
			end
		end
		return flags
	end,
})

p.tools.emcc = emcc

emscripten = {}

-- Helper function for creating a packfile target.
-- This is probably the wrong way to do this.
function emscripten.assetdir(dir)
	local data = "%{cfg.targetdir}/%{prj.name}.data"
	local datajs = "%{cfg.objdir}/%{prj.name}.data.js"

	local build_command = ""
	local lddeps = ""
	local inputs = {}

	build_command = "python3"
		.. " $(EMSDK)/fastcomp/emscripten/tools/file_packager.py"
		.. " " .. data
		.. " --js-output=" .. datajs
		.. " --no-heap-copy"
		.. " --from-emcc"  -- Hack to disable "Remember to..." output
		--.. " --use-preload-plugins"
		.. " --preload"  -- List of paths follows
		.. " '../" .. dir .. "@'"

	makesettings("LDDEPS += ../" .. dir)

	filter "toolset:emcc"
		prelinkmessage "Packing assets..."
		prelinkcommands {
			"$(SILENT) " .. build_command
		}

		linkoptions {
			"--pre-js " .. datajs,
			"-s FORCE_FILESYSTEM=1",
		}

	filter {}
end

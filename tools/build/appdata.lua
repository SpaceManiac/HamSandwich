-- appdata.lua
-- Custom HamSandwich-specific appdata configurators.
-- run-config.lua and emscripten.lua rely on this.

local p = premake

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
	name = "appdata_name",
	scope = "project",
	kind = "string",
	tokens = true
}

-- Expand "installers" and "assetdirs" commands into "debugenvs"
local function expand_to_debugenvs(project, block)
	local i = 0
	local function push_asset_spec(mountpoint, kind, location)
		local debugenv = "HSW_ASSETS_" .. i .. "=" .. mountpoint .. "@" .. kind .. "@" .. location
		i = i + 1

		if block.debugenvs then
			block.debugenvs[1 + #block.debugenvs] = debugenv
		else
			block.debugenvs = { debugenv }
		end
	end

	if block.assetdirs then
		for _, v in ipairs(block.assetdirs) do
			push_asset_spec("", "stdio", "../../" .. v)
		end
	end

	if block.installers then
		for _, inst in ipairs(block.installers) do
			push_asset_spec(inst.mountpoint or "", inst.kind, "../../installers/" .. inst.filename)
		end
	end
end

p.override(p.project, "bake", function(base, self)
	-- Keep the list stable while it's being iterated
	local blocks = {}
	for k, v in pairs(self.blocks) do
		blocks[k] = v
	end

	for _, block in pairs(blocks) do
		expand_to_debugenvs(self, block)
	end

	return base(self)
end)

-- Make the metadata available to the C++ code.
local function is_application(prj)
	return prj.kind == "ConsoleApp" or prj.kind == "WindowedApp"
end

local function metadata_cpp(cfg)
	local appdata_folder_name = cfg.appdata_name or cfg.project.name

	p.w('#include "metadata.h"')
	p.w()

	p.push('static const char* default_asset_specs[] = {')
	if cfg.assetdirs and #cfg.assetdirs > 0 then
		p.w('%s,', json.encode("@stdio@../../assets/" .. appdata_folder_name))
	end
	for _, inst in ipairs(cfg.installers) do
		p.w('%s,', json.encode((inst.mountpoint or "") .. "@" .. inst.kind .. "@installers/" .. inst.filename))
	end
	p.w("nullptr,")
	p.pop('};')

	p.push('static const HamSandwichMetadata metadata = {')
	-- appdata_folder_name
	p.w('%s,', json.encode(appdata_folder_name))
	-- default_asset_specs
	p.w('default_asset_specs,')
	p.pop('};')
	p.w()

	p.w('const HamSandwichMetadata* GetHamSandwichMetadata()')
	p.push('{')
	p.w('return &metadata;')
	p.pop('}');
end

p.override(p.oven, "bakeFiles", function(base, prj)
	local actually_need_it = (
		_ACTION == "gmake2" or
		_ACTION == "android-studio" or
		_ACTION:sub(1, 4) == "vs20"
	)

	if is_application(prj) then
		for cfg in p.project.eachconfig(prj) do
			-- Stick in the workspace root to avoid spamming folders in Visual Studio
			local fname = prj.workspace.location .. "/metadata_" .. prj.name .. "_" .. cfg.name:gsub("|", "_") .. ".cpp"
			table.insert(cfg.files, fname)
			if actually_need_it then
				p.generate(cfg, fname, metadata_cpp)
			end
		end
	end

	return base(prj)
end)

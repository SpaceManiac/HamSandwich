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
		for k, v in pairs(block.installers) do
			push_asset_spec(v.mountpoint or "", v.kind, "../../build/installers/" .. k)
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

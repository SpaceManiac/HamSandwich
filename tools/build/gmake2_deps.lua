-- gmake2_deps.lua
-- Provides a "gmake2_deps" premake action which lists all source directories
-- so the Makefile can be regenerated as needed.

local p = premake
local m = {}
p.modules.gmake2_deps = m

newaction {
	trigger = "gmake2_deps",
	description = "Create Makefile.d files for gmake2",
	onWorkspace = function(wks)
		-- print("onWorkspace", wks.name)
		p.generate(wks, "Makefile.d", m.workspace_d)
	end,
}

function m.workspace_d(wks)
	local seen = {}
	for prj in p.workspace.eachproject(wks) do
		for cfg in p.project.eachconfig(prj) do
			for _, file in ipairs(cfg.files) do
				local dir = path.getdirectory(path.getabsolute(file))
				if dir ~= cfg.objdir then
					seen[dir] = true
				end
			end
			for _, file in pairs(emscripten.decode_webfiles(cfg.webfiles)) do
				local dir = path.getdirectory(path.getabsolute(file))
				if dir ~= cfg.objdir then
					seen[dir] = true
				end
			end
		end
	end
	local rel = path.getrelative(os.getcwd(), wks.location)
	for dir, _ in pairs(seen) do
		p.w("%s/Makefile: %s", rel, dir)
	end
end

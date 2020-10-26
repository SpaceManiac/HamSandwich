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
	for prj in p.workspace.eachproject(wks) do
		local first = true
		local seen = {}
		for cfg in p.project.eachconfig(prj) do
			for _, file in ipairs(cfg.files) do
				local dir = path.getdirectory(file)
				if not seen[dir] then
					seen[dir] = true
					p.w("%s/Makefile: %s", path.getrelative(os.getcwd(), prj.location), dir)
				end
			end
		end
		if not first then
			p.w("endif")
		end
	end
end

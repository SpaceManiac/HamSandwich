--[[ binary-package-info.lua
Provides a "binary-package-info" premake action which creates a PowerShell
script and text files useful for exporting projects. ]]

local p = premake
local m = {}
p.modules.run_config = m

newaction {
	trigger = "binary-package-info",
	description = "Format metadata for build-binary-package.ps1",
	onWorkspace = function(wks)
		-- print("onWorkspace", wks.name)
		p.generate(wks, "binary-package-info.ps1", m.workspace_binary_package_info)
	end,
}

function m.make_relative_path(wks, str)
	return path.join(
		"$PWD",
		path.getrelative(
			os.getcwd(),
			str
		)
	)
end

function m.is_application(prj)
	return prj.kind == "ConsoleApp" or prj.kind == "WindowedApp"
end

function m.workspace_binary_package_info(wks)
	local seen = {}

	p.w('# globals: $allprojects $exeprojects')

	p.push('$ExeProjects = @{')

	local comma = ""
	for prj in p.workspace.eachproject(wks) do
		if m.is_application(prj) then
			for cfg in p.project.eachconfig(prj) do
				p.push('%s"%s|%s" = @{', comma, prj.name, cfg.name:gsub("|", "_"))
				comma = "; "

				p.push('"installers" = (')
				local comma2 = ""
				for k, v in pairs(cfg.installers) do
					p.w('%s@{"filename" = %s; "link" = %s} `', comma2, json.encode(k), json.encode(v.link))
					comma2 = ", "
				end
				p.pop(')')

				p.pop('}')
			end
		end
	end

	p.pop('}')
end

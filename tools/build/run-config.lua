--[[ run-config.lua
Provides a "run-config" premake action which creates a shell script containing
information about the workspace, including the debug configuration. ]]

local p = premake
local m = {}
p.modules.run_config = m

newaction {
	trigger = "run-config",
	description = "Create run-config.sh script",
	onWorkspace = function(wks)
		-- print("onWorkspace", wks.name)
		p.generate(wks, "run-config.sh", m.workspace_run_config)
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

function m.workspace_run_config(wks)
	local seen = {}
	p.w('# globals: $allprojects $exeprojects')

	local exeprojects = ''
	local allprojects = ''
	for prj in p.workspace.eachproject(wks) do
		allprojects = allprojects .. prj.name .. " "
		if m.is_application(prj) then
			exeprojects = exeprojects .. prj.name .. " "
		end
	end
	p.w('allprojects=(%s)', allprojects)
	p.w('exeprojects=(%s)', exeprojects)

	p.push('debug_configure() {')
	p.w('# input: $project $config $PWD')
	p.w('# output: $debugdir $debugcommand $debugargs $debugenv')
	p.push('case "$project|$config" in')
	for prj in p.workspace.eachproject(wks) do
		if m.is_application(prj) then
			for cfg in p.project.eachconfig(prj) do
				p.push('"%s|%s")', prj.name, cfg.name:gsub("|", "_"))
				p.w('debugdir=%s', m.make_relative_path(wks, cfg.debugdir))
				p.w('debugcommand=%s', m.make_relative_path(wks, cfg.debugcommand or cfg.buildtarget.abspath))

				local debugargs = ""
				for _, arg in ipairs(cfg.debugargs) do
					debugargs = debugargs .. json.encode(arg) .. " "
				end
				p.w('debugargs=(%s)', debugargs)

				local debugenvs = ""
				for _, env in ipairs(cfg.debugenvs) do
					debugenvs = debugenvs .. json.encode(env) .. " "
				end
				p.w('debugenvs=(%s)', debugenvs)

				p.pop(';;')
			end
		end
	end
	p.push('*)')
	p.w('echo "$0: no executable for config=$config project=$project"')
	p.w('exit 1')
	p.pop(';;')
	p.pop('esac')
	p.pop('}')
end

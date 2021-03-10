-- vscode.lua
-- Provides a "vscode" premake action which creates `tasks.json` and `launch.json`.

local p = premake
local m = {}
p.modules.vscode = m

newaction {
	trigger = "vscode",
	description = "Create Visual Studio Code task files",
	toolset = "gcc",

	onWorkspace = function(wks)
		local wks2 = table.merge({}, wks, {original_location = wks.location, location = ".vscode"})
		p.generate(wks2, "tasks.json", m.tasks_json)
		p.generate(wks2, "launch.json", m.launch_json)
	end,
}

newoption {
	trigger = "config",
	value = "CONFIG",
	description = "Default configuration for Visual Studio Code",
	default = (_OPTIONS.os or _TARGET_OS) == "linux" and "debug|x86_64" or "debug|x86",
}

function m.is_application(prj)
	-- Only premake apps become Android apps, everything else is an NDK
	-- module only.
	return prj.kind == "ConsoleApp" or prj.kind == "WindowedApp"
end

function m.getcfg(prj, cfgname)
	for cfg in p.project.eachconfig(prj) do
		if cfg.name == cfgname then
			return cfg
		end
	end
end

function m.make_relative_path(wks, str)
	return path.join(
		"${workspaceFolder}",
		path.getrelative(
			os.getcwd(),
			path.join(wks.original_location, (str:gsub("^%$%(ProjectDir%)/", "")))
		)
	)
end

function m.tasks_json(wks)
	p.push '{'
	p.w([["version": "2.0.0",
	"options": {
		"cwd": "${workspaceFolder}",
		"env": {
			"MAKEFLAGS": "config=%s -j8"
		}
	},
	"problemMatcher": [
		{
			"owner": "cpp",
			"fileLocation": ["relative", "${workspaceFolder}/build"],
			"pattern": {
				"regexp": "^(.*):(\\d+):(\\d+):\\s+(?:fatal )?(warning|error):\\s+(.*)$",
				"file": 1,
				"line": 2,
				"column": 3,
				"severity": 4,
				"message": 5
			}
		},
		{
			"owner": "cpp",
			"fileLocation": "autoDetect",
			"pattern": {
				"regexp": "^(.*):(\\d+): (undefined reference to .*)$",
				"file": 1,
				"line": 2,
				"message": 3,
			}
		}
	],
	]], _OPTIONS.config:gsub("|", "_"))

	p.push '"tasks": ['
	for prj in p.workspace.eachproject(wks) do
		local cfg = m.getcfg(prj, _OPTIONS.config)
		p.push '{'
		p.w('"label": "make %s",', prj.name)
		p.w('"command": "make %s toolset=%s",', prj.name, cfg.toolset)
		p.w('"type": "shell",')
		p.w('"group": "build",')
		p.pop '},'
	end
	p.pop '],'
	p.pop '}'
end

function m.launch_json(wks)
	p.push '{'
	p.w '"version": "0.2.0",'
	p.push '"configurations": ['

	for prj in p.workspace.eachproject(wks) do
		if m.is_application(prj) then
			local cfg = m.getcfg(prj, _OPTIONS.config)

			local program = m.make_relative_path(wks, cfg.debugcommand or cfg.buildtarget.relpath)
			local args = table.join(cfg.debugargs)
			local cwd = m.make_relative_path(wks, cfg.debugdir)

			local environment = {}
			for _, v in ipairs(cfg.debugenvs) do
				local name, value = v:match("^([^=]+)=(.+)$")
				if name then
					table.insert(environment, { name = name, value = value })
				end
			end

			p.push '{'
			p.w([["name": "%s",
			"preLaunchTask": "make %s",
			"type": "cppdbg",
			"request": "launch",
			"setupCommands": [
				{
					"text": "-enable-pretty-printing",
					"ignoreFailures": true,
				},
			],
			"program": %s,
			"args": %s,
			"cwd": %s,
			"environment": %s,]], prj.name, prj.name, json.encode(program), json.encode(args), json.encode(cwd), json.encode(environment))
			p.pop '},'
		end
	end

	p.pop '],'
	p.pop '}'
end

return m

-- premake5.lua
workspace "HamSandwich"
	configurations { "debug", "release" }
	location "build"

function base_project(name)
	project(name)
		kind "WindowedApp"
		language "C++"
		cppdialect "C++17"
		architecture "x86"
		targetdir("build/%{cfg.toolset}-%{cfg.buildcfg}/%{prj.name}/")
		objdir("build/%{cfg.toolset}-%{cfg.buildcfg}/%{prj.name}/obj/")

		files {
			"source/" .. name .. "/**.h",
			"source/" .. name .. "/**.cpp",
		}

		filter "configurations:debug"
			kind "ConsoleApp"
			defines { "_DEBUG" }
			symbols "On"

		filter "configurations:release"
			defines { "NDEBUG" }
			optimize "On"

		filter { "toolset:gcc", "system:Windows" }
			linkoptions { "-static-libgcc", "-static-libstdc++" }

		filter "action:vs*"
			defines { "_CRT_SECURE_NO_WARNINGS", "NOMINMAX" }
			includedirs { "build/include/" }
			libdirs { "build/lib/x86/" }
			debugargs { "window" }
			debugenvs { "PATH=$(ProjectDir)/lib/x86/;%PATH%" }
			debugdir "$(ProjectDir)/game/%{prj.name}"

		filter {}
end

function library(name)
	base_project(name)
		kind "StaticLib"
end

function sdl2_project(name)
	base_project(name)
		filter { "system:Windows", "not action:vs*" }
			links "mingw32"
		filter "system:Windows"
			links { "ws2_32", "winmm" }
		filter {}

		links { "SDL2main", "SDL2", "SDL2_mixer", "SDL2_image" }
end

function excludefiles(files)
	list = {}
	for i = 1, #files do
		list[i] = "source/%{prj.name}/" .. files[i]
	end
	removefiles(list)
end

function icon_file(icon)
	-- Workaround for bug in premake5's gmake2 generator, which does
	-- not count .res (object) files as resources, only .rc (source)
	filter { "system:Windows", "toolset:not clang" }
		files { "source/%{prj.name}/" .. icon .. ".rc" }

	filter { "system:Windows", "action:gmake2", "toolset:not clang" }
		linkoptions { "%{cfg.objdir}/" .. icon .. ".res" }

	-- Support for embedding the icon in the file on non-Windows systems
	filter { "system:not Windows" }
		files { "source/%{prj.name}/" .. icon .. ".rc" }
		files { "%{cfg.objdir}/" .. icon .. ".rc.o" }

	filter { "system:not Windows", "files:**.rc" }
		buildmessage "%{file.name}"
		buildcommands { 'python3 ../tools/build/rescomp.py "%{file.path}" "%{cfg.objdir}/%{file.basename}.rc.cpp"' }
		buildoutputs { "%{cfg.objdir}/" .. icon .. ".rc.cpp" }
		buildinputs { "tools/build/rescomp.py" }

	filter {}
end

function pch(name)
	--filter "action:vs*"
	--	pchheader(name .. ".h")
	--	pchsource("source/%{prj.name}/" .. name .. ".cpp")
	filter "action:not vs*"
		pchheader("source/%{prj.name}/" .. name .. ".h")
	filter {}
end

function depends(name)
	includedirs { "source/" .. name }
	links(name)
end

library "ham"
	filter "toolset:gcc"
		buildoptions { "-Wall", "-Wextra", "-Wno-char-subscripts" }

sdl2_project "lunatic"
	icon_file "lunatic"
	depends "ham"
	pch "winpch"
	defines { "EXPANDO" }

	filter "toolset:gcc"
		buildoptions { "-Wall", "-Wextra", "-Wno-unused-parameter" }

sdl2_project "supreme"
	icon_file "lunatic"
	depends "ham"
	pch "winpch"

	excludefiles {
		"monsterlist.cpp",
		"monsterai1.cpp",
		"monsterai2.cpp",
		"monsterai3.cpp",
		"monsterai4.cpp",
		"textitems.cpp",
		"textrooms.cpp",
	}

	filter "toolset:gcc"
		buildoptions {
			"-Wall",
			"-Wno-write-strings",
			"-Wno-char-subscripts",
			"-Wno-unused-variable",
			"-Wno-unused-but-set-variable",
		}

sdl2_project "sleepless"
	icon_file "lunatic"
	depends "ham"
	pch "winpch"

	excludefiles {
		"monsterlist.cpp",
		"monsterai1.cpp",
		"monsterai2.cpp",
		"monsterai3.cpp",
		"monsterai4.cpp",
		"monsterhollow.cpp",
	}

	filter "toolset:gcc"
		buildoptions {
			"-Wall",
			"-Wno-write-strings",
			"-Wno-char-subscripts",
			"-Wno-unused-variable",
			"-Wno-unused-but-set-variable",
		}

sdl2_project "loonyland"
	icon_file "loonyland"
	depends "ham"
	pch "winpch"

	filter "toolset:gcc"
		buildoptions { "-Wno-write-strings" }

sdl2_project "loonyland2"
	icon_file "loonyland2"
	depends "ham"
	pch "winpch"
	defines { "DIRECTORS" }
	excludefiles {
		"monster_ai.cpp",
	}

sdl2_project "mystic"
	icon_file "mystic"
	depends "ham"
	pch "winpch"

-- premake5.lua
workspace "HamSandwich"
	configurations { "debug", "release" }
	location "build"

function base_project(name)
	project(name)
		kind "WindowedApp"
		language "C++"
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

		filter "toolset:gcc"
			linkoptions { "-static-libgcc", "-static-libstdc++" }

		filter "action:vs*"
			defines { "_CRT_SECURE_NO_WARNINGS" }
			includedirs { "build/include/" }
			libdirs { "build/lib/x86/" }

		filter {}
end

function library(name)
	base_project(name)
		kind "StaticLib"
end

function sdl2_project(name)
	base_project(name)
		filter { "system:Windows", "toolset:gcc" }
			links "mingw32"
		filter "system:Windows"
			links { "ws2_32", "winmm" }
		filter {}

		links { "SDL2main", "SDL2", "SDL2_mixer", "SDL2_image" }
end

function removefiles_in(dir, files)
	list = {}
	for i = 1, #files do
		list[i] = dir .. files[i]
	end
	removefiles(list)
end

function icon_file(icon)
	-- Workaround for bug in premake5's gmake2 generator, which does
	-- not count .res (object) files as resources, only .rc (source)
	filter "system:Windows"
		files { "source/" .. icon .. "/**.rc" }
	filter { "system:Windows", "action:gmake2" }
		linkoptions { "%{cfg.toolset}-%{cfg.buildcfg}/%{prj.name}/obj/" .. icon .. ".res" }
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

	removefiles_in("source/supreme/", {
		"monsterlist.cpp",
		"monsterai1.cpp",
		"monsterai2.cpp",
		"monsterai3.cpp",
		"monsterai4.cpp",
		"textitems.cpp",
		"textrooms.cpp",
		"options.cpp",
		"spcldialog.cpp",
	})

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

	removefiles_in("source/sleepless/", {
		"monsterlist.cpp",
		"monsterai1.cpp",
		"monsterai2.cpp",
		"monsterai3.cpp",
		"monsterai4.cpp",
		"monsterhollow.cpp",
		"textitems.cpp",
		"textrooms.cpp",
		"options.cpp",
		"spcldialog.cpp",
		"tally.cpp",
		"worldselect.cpp",
	})

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
	removefiles_in("source/loonyland2/", {
		"monster_ai.cpp",
	})

	filter "toolset:gcc"
		buildoptions { "-Wno-write-strings" }

sdl2_project "mystic"
	icon_file "mystic"
	depends "ham"
	pch "winpch"

	filter "toolset:gcc"
		buildoptions { "-Wno-write-strings" }

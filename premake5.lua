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

		linkoptions { "-static-libgcc", "-static-libstdc++" }

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

		filter {}
end

function library(name)
	base_project(name)
		kind "StaticLib"
end

function sdl2_project(name)
	base_project(name)
		filter "system:Windows"
			links { "mingw32", "SDL2main", "SDL2", "ws2_32", "winmm", "SDL2_mixer", "SDL2_image" }

		filter {}
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
		linkoptions { "%{cfg.toolset}-%{cfg.buildcfg}/%{prj.name}/obj/" .. icon .. ".res" }
	filter {}
end

function depends(name)
	includedirs { "source/" .. name }
	links(name)
end

library "common"

sdl2_project "lunatic"
	icon_file "lunatic"
	depends "common"
	defines { "EXPANDO" }
	buildoptions { "-Wall", "-Wextra", "-Wno-unused-parameter" }

sdl2_project "supreme"
	icon_file "lunatic"
	depends "common"
	buildoptions {
		"-Wall",
		"-Wno-write-strings",
		"-Wno-char-subscripts",
		"-Wno-unused-variable",
		"-Wno-unused-but-set-variable",
	}
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

sdl2_project "sleepless"
	icon_file "lunatic"
	buildoptions {
		"-Wall",
		"-Wno-write-strings",
		"-Wno-char-subscripts",
		"-Wno-unused-variable",
		"-Wno-unused-but-set-variable",
	}
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

sdl2_project "loonyland"
	icon_file "loonyland"
	depends "common"
	buildoptions { "-Wno-write-strings" }

sdl2_project "loonyland2"
	icon_file "loonyland2"
	depends "common"
	buildoptions { "-Wno-write-strings" }
	defines { "DIRECTORS" }
	removefiles_in("source/loonyland2/", {
		"monster_ai.cpp",
	})

sdl2_project "mystic"
	icon_file "mystic"
	depends "common"
	buildoptions { "-Wno-write-strings" }
	removefiles_in("source/mystic/", {
		"old_control.cpp",
		"Copy of title.cpp",
	})

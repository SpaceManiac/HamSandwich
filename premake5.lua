-- premake5.lua
workspace "HamSandwich"
	configurations { "debug", "release" }
	location "build"

function allegro_project(name)
	project(name)
		kind "WindowedApp"
		language "C++"
		architecture "x86"
		targetdir("build/%{cfg.toolset}-%{cfg.buildcfg}/" .. name .. "/")
		objdir("build/%{cfg.toolset}-%{cfg.buildcfg}/" .. name .. "/obj/")

		includedirs { "include/" }
		libdirs { "include/" }
		linkoptions { "-static-libgcc", "-static-libstdc++" }

		files {
			"source/" .. name .. "/**.h",
			"source/" .. name .. "/**.cpp",
			"source/" .. name .. "/**.rc",
		}

		filter "configurations:debug"
			defines { "_DEBUG" }
			symbols "On"

		filter "configurations:release"
			defines { "NDEBUG" }
			optimize "On"

		filter "system:Windows"
			links { "winmm", "alleg44", "ws2_32", "logg", "vorbisfile", "vorbis", "ogg", "vorbisenc" }

		filter { "system:Windows", "toolset:gcc" }
			defines { "ALLEGRO_MINGW32" }

		filter {}
end

function removefiles_in(dir, files)
	list = {}
	for i = 1, #files do
		list[i] = dir .. files[i]
	end
	removefiles(list)
end

allegro_project "lunatic"

allegro_project "supreme"
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

allegro_project "sleepless"
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

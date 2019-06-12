-- premake5.lua
dofile "tools/build/android_studio.lua"

workspace "HamSandwich"
	location "build"
	configurations { "debug", "release" }

	filter { "action:android-studio" }
		location "build/android"
		android_abis { "armeabi-v7a" }

function base_project(name)
	project(name)
		kind "WindowedApp"
		language "C++"
		cppdialect "C++17"
		architecture "x86"
		targetdir("build/%{cfg.toolset}-%{cfg.buildcfg}/%{prj.name}/")
		objdir("build/%{cfg.toolset}-%{cfg.buildcfg}/%{prj.name}/obj/")
		android_package "com.platymuus.hamsandwich.%{prj.name}"

		android_assetdirs {
			"build/game/%{prj.name}/",
			"assets/android/",
		}

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
			-- At least some versions of VS2017 don't recognize "C++17".
			cppdialect "C++latest"
			defines { "_CRT_SECURE_NO_WARNINGS", "NOMINMAX", "SDL_UNPREFIXED" }
			-- The MSVC dependency script puts the SDL2 binaries here.
			includedirs {
				"build/SDL2-msvc/include",
				"build/SDL2_mixer-msvc/include",
				"build/SDL2_image-msvc/include",
			}
			libdirs {
				"build/SDL2-msvc/lib/x86",
				"build/SDL2_mixer-msvc/lib/x86",
				"build/SDL2_image-msvc/lib/x86",
			}
			-- These emulate the `./run` script when running within VS.
			debugargs { "window" }
			debugenvs { "PATH=$(ProjectDir)/lib/x86/;%PATH%" }
			debugdir "$(ProjectDir)/game/%{prj.name}"

		filter "action:android-studio"
			defines { "SDL_UNPREFIXED" }
			buildoptions { "-fsigned-char", "-fexceptions" }

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

	-- Convert the icon to a resource
	filter { "action:android-studio" }
		android_icon("source/%{prj.name}/" .. icon .. ".ico")

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
	android_appname "Dr. Lunatic"
	icon_file "lunatic"
	depends "ham"
	pch "winpch"
	defines { "EXPANDO" }

	filter "toolset:gcc"
		buildoptions { "-Wall", "-Wextra", "-Wno-unused-parameter" }

sdl2_project "supreme"
	android_appname "Supreme With Cheese"
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
			"-Wno-char-subscripts",
			"-Wno-unused-variable",
			"-Wno-unused-but-set-variable",
		}

sdl2_project "sleepless"
	android_appname "Sleepless Hollow"
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
			"-Wno-char-subscripts",
			"-Wno-unused-variable",
			"-Wno-unused-but-set-variable",
		}

sdl2_project "loonyland"
	android_appname "Loonyland: Halloween Hill"
	icon_file "loonyland"
	depends "ham"
	pch "winpch"

sdl2_project "loonyland2"
	android_appname "Loonyland 2: Winter Woods"
	icon_file "loonyland2"
	depends "ham"
	pch "winpch"
	defines { "DIRECTORS" }
	excludefiles {
		"monster_ai.cpp",
	}

sdl2_project "mystic"
	android_appname "Kid Mystic"
	icon_file "mystic"
	depends "ham"
	pch "winpch"

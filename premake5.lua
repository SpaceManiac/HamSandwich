-- premake5.lua
dofile "tools/build/gmake2_deps.lua"
dofile "tools/build/android_studio.lua"
dofile "tools/build/emscripten.lua"
dofile "tools/build/vscode.lua"
dofile "tools/build/run-config.lua"

sdl2_platforms = {
	x86 = "x86",
	x86_64 = "x64",
}

local is_msvc = _ACTION and _ACTION:sub(1, 4) == "vs20"

local toolset
if _OPTIONS.cc then
	toolset = _OPTIONS.cc
elseif not is_mscv and (_OPTIONS.os or _TARGET_OS) == premake.MACOSX then
	toolset = "clang"
else
	local current = premake.action.current()
	toolset = current and current.toolset or "gcc"
end

workspace "HamSandwich"
	location("build/" .. toolset)
	configurations { "debug", "release" }
	platforms { "x86", "x86_64" }

	filter { "action:android-studio" }
		location "build/android"
		android_abis { "armeabi-v7a" }

function base_project()
	language "C++"
	cppdialect "C++17"
	architecture "x86"
	targetdir "%{wks.location}/%{cfg.buildcfg}-%{cfg.platform}/%{prj.name}/"
	objdir "%{cfg.targetdir}/obj/"

	files {
		"source/%{prj.name}/**.h",
		"source/%{prj.name}/**.cpp",
		"source/%{prj.name}/**.c",
	}

	filter "platforms:x86_64"
		architecture "x86_64"

	filter "configurations:debug"
		kind "ConsoleApp"
		defines { "_DEBUG" }
		symbols "On"

	filter "configurations:release"
		defines { "NDEBUG" }
		optimize "On"

	filter { "toolset:gcc", "system:Windows" }
		linkoptions { "-static-libgcc", "-static-libstdc++" }

	filter "action:vs20*"
		cppdialect "C++17"
		defines { "_CRT_SECURE_NO_WARNINGS", "NOMINMAX" }
		-- The MSVC dependency script puts the SDL2 binaries here.
		includedirs {
			"build/SDL2-msvc/include",
			"build/SDL2_mixer-msvc/include",
			"build/SDL2_image-msvc/include",
		}
		libdirs {
			"build/SDL2-msvc/lib/%{sdl2_platforms[cfg.platform]}",
			"build/SDL2_mixer-msvc/lib/%{sdl2_platforms[cfg.platform]}",
			"build/SDL2_image-msvc/lib/%{sdl2_platforms[cfg.platform]}",
		}

	filter "action:android-studio"
		buildoptions { "-fsigned-char", "-fexceptions" }

	filter { "toolset:emcc" }
		linkoptions {
			"-s ALLOW_MEMORY_GROWTH=1",
			"--use-preload-cache",
			"-s ENVIRONMENT=web",
			"-s FORCE_FILESYSTEM=1",
			"-s EXTRA_EXPORTED_RUNTIME_METHODS=['ENV']"
		}

		-- coroutine support
		defines { "USE_COROUTINES" }
		buildoptions { "-fcoroutines-ts", "-Werror=unused-result" }

	filter {}
end

function sdl2_project()
	base_project()

	kind "WindowedApp"
	filter "configurations:debug"
		kind "ConsoleApp"
	filter {}

	-- These emulate the `./run` script when running within VS.
	debugdir "%{wks.location}/../game/%{prj.name}"
	debugargs { "window" }

	filter "toolset:emcc"
		debugdir "%{cfg.targetdir}"
	filter "action:vs20*"
		debugenvs {
			"PATH=" ..
				"%{prj.location}/../SDL2-msvc/lib/%{sdl2_platforms[cfg.platform]}/;" ..
				"%{prj.location}/../SDL2_mixer-msvc/lib/%{sdl2_platforms[cfg.platform]}/;" ..
				"%{prj.location}/../SDL2_image-msvc/lib/%{sdl2_platforms[cfg.platform]}/;" ..
				"%PATH%",
		}
	filter {}

	-- Android application metadata.
	android_package "com.platymuus.hamsandwich.%{prj.name}"
	android_assetdirs {
		"build/assets/%{prj.name}/",
		"assets/android/",
	}

	-- Emscripten metadata.
	webfiles "assets/emscripten/*"
	filter { "toolset:emcc", "configurations:debug" }
		linkoptions { "--emrun" }
	filter {}

	-- Link SDL2 in the correct sequence.
	filter { "system:Windows", "not action:vs*", "toolset:not emcc" }
		links "mingw32"
	filter { "system:Windows", "toolset:not emcc" }
		links { "ws2_32", "winmm" }
	filter {}

	links { "SDL2main", "SDL2", "SDL2_mixer", "SDL2_image" }
end

function icon_file(icon)
	webfiles { ["favicon.ico"] = "source/%{prj.name}/" .. icon .. ".ico" }

	-- Workaround for bug in premake5's gmake2 generator, which does
	-- not count .res (object) files as resources, only .rc (source)
	filter { "system:Windows", "toolset:not clang", "toolset:not emcc" }
		files { "source/%{prj.name}/" .. icon .. ".rc" }

	filter { "system:Windows", "action:gmake2", "toolset:not clang", "toolset:not emcc" }
		linkoptions { "%{cfg.objdir}/" .. icon .. ".res" }

	-- Support for embedding the icon in the file on non-Windows systems
	filter { "system:not Windows or toolset:emcc" }
		files { "source/%{prj.name}/" .. icon .. ".rc" }
		makesettings("OBJECTS += $(OBJDIR)/" .. icon .. ".rc.o")

	filter { "system:not Windows or toolset:emcc", "files:**.rc" }
		buildmessage "%{file.name}"
		buildcommands { 'python3 ../../tools/build/rescomp.py "%{file.path}" "%{cfg.objdir}/%{file.basename}.rc.cpp"' }
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

function sdl2_config_cflags()
	local shell = ""
	if _TARGET_OS == "windows" then  -- should really be host OS
		shell = "sh "
	end
	result, status = os.outputof(shell .. "sdl2-config --cflags")
	assert(status == 0, "error running `sdl2-config --cflags`:\n" .. (result or "nil"))
	sdl2_config_cflags = function ()
		return result
	end
	return result
end

local function uses_sdl2(recursive)
	if _ACTION == "gmake2" and _OPTIONS.cc ~= "emcc" then
		buildoptions { sdl2_config_cflags() }
	end
	filter { "kind:not StaticLib or action:android-studio or toolset:emcc" }
		links { "SDL2" }
	filter {}
end

local function uses_sdl2_image(recursive)
	filter { "kind:not StaticLib or action:android-studio or toolset:emcc" }
		links { "SDL2_image" }
	filter {}
end

local function uses_sdl2_mixer(recursive)
	filter { "kind:not StaticLib or action:android-studio or toolset:emcc" }
		links { "SDL2_mixer" }
	filter {}
end

local function uses_z(recursive)
	filter { "kind:not StaticLib or action:android-studio or toolset:emcc" }
		links { "z" }
	filter {}
end

if is_msvc then
	local function nmake_command(args)
		return 'cmd /C "cd %{cfg.targetdir} & nmake TOP=../../../../external/zlib -f ../../../../external/zlib/win32/Makefile.msc ' .. args .. '"'
	end

	project "z"
		kind "Makefile"
		targetdir "%{wks.location}/%{cfg.buildcfg}-%{cfg.platform}/%{prj.name}/"
		objdir "%{cfg.targetdir}"  -- zlib's Makefile.msc assumes this matches targetdir
		targetname "zlib.lib"

		buildcommands { nmake_command "%{cfg.targetname}" }
		rebuildcommands { nmake_command "/A %{cfg.targetname}" }
		cleancommands { nmake_command "clean" }

	uses_z = function(recursive)
		if not recursive then
			includedirs "external/zlib"
		end
		filter { "kind:not StaticLib" }
			dependson { "z" }
			links { "%{wks.location}/%{cfg.buildcfg}-%{cfg.platform}/z/zlib.lib" }
		filter {}
	end
end

project "vanilla_extract"
	base_project()
	kind "StaticLib"
	uses_z()
	uses_sdl2()

	filter "action:not vs20*"
		buildoptions { "-Wall", "-Wextra" }

local function uses_vanilla_extract(recursive)
	if not recursive then
		includedirs "source/vanilla_extract"
	end
	filter { "kind:not StaticLib" }
		links { "vanilla_extract" }
	filter {}
	uses_z(true)
	uses_sdl2(true)
end

project "ham"
	base_project()
	kind "StaticLib"
	uses_vanilla_extract()
	uses_sdl2()
	uses_sdl2_image()
	uses_sdl2_mixer()

	filter "action:not vs20*"
		buildoptions { "-Wall", "-Wextra" }

local function uses_ham(recursive)
	if not recursive then
		includedirs "source/ham"
	end
	filter { "kind:not StaticLib" }
		links { "ham" }
	filter {}
	uses_vanilla_extract(true)
	uses_sdl2(true)
	uses_sdl2_image(true)
	uses_sdl2_mixer(true)
end

project "lunatic"
	sdl2_project()
	android_appname "Dr. Lunatic"
	icon_file "lunatic"
	uses_ham()
	pch "winpch"
	defines { "EXPANDO" }

	installers {
		["lunatic_install.exe"] = {
			kind = "nsis",
			sha256sum = "b8013176ea8050db20a2b170a5273d5287ccde4b4923affb7c610bda89326c84",
			link = "https://hamumu.itch.io/dr-lunatic",
		}
	}

	filter "action:not vs20*"
		buildoptions { "-Wall", "-Wextra", "-Wno-unused-parameter" }

project "supreme"
	sdl2_project()
	android_appname "Supreme With Cheese"
	icon_file "lunatic"
	uses_ham()
	pch "winpch"

	installers {
		["supreme8_install.exe"] = {
			kind = "nsis",
			sha256sum = "1c105ad826be1e0697b5de8483c71ff943d04bce91fe3547b6f355e9bc1c42d4",
			link = "https://hamumu.itch.io/dr-lunatic-supreme-with-cheese",
		}
	}

	webfiles { ["splash.jpg"] = "assets/splashes/supreme.jpg" }

	filter "toolset:gcc"
		buildoptions {
			"-Wall",
			"-Wno-unused-variable",
			"-Wno-unused-but-set-variable",
		}

project "sleepless"
	sdl2_project()
	android_appname "Sleepless Hollow"
	icon_file "lunatic"
	uses_ham()
	pch "winpch"

	installers {
		["hollow_betainstall.exe"] = {
			kind = "nsis",
			sha256sum = "41660802318356fba53a21b4d368e191b3197030fb9e8eb833788f45c01c6f99",
			link = "https://hamumu.itch.io/sleepless-hollow",
		}
	}

	webfiles { ["splash.jpg"] = "assets/splashes/sleepless.jpg" }

	filter "toolset:gcc"
		buildoptions {
			"-Wall",
			"-Wno-unused-variable",
			"-Wno-unused-but-set-variable",
		}

project "loonyland"
	sdl2_project()
	android_appname "Loonyland: Halloween Hill"
	web_title "Loonyland"
	icon_file "loonyland"
	uses_ham()
	pch "winpch"

	installers {
		["loonyland_install.EXE"] = {
			kind = "inno",
			sha256sum = "cf3cdc555297e41f6c2da61d89815dbbc740d6fc677c83ec6c6e1acfa117de34",
			link = "https://hamumu.itch.io/loonyland-halloween-hill",
		},
		["loonyland_editor.exe"] = {
			kind = "nsis",
			sha256sum = "865550d077e984ca28324aaf4291211aa4009cdad9f2b74144179c6342f2be39",
			link = "https://hamumu.itch.io/loonyland-halloween-hill",
		}
	}

	webfiles { ["splash.jpg"] = "assets/splashes/loonyland.jpg" }

project "loonyland2"
	sdl2_project()
	android_appname "Loonyland 2: Winter Woods"
	web_title "Loonyland 2"
	icon_file "loonyland2"
	uses_ham()
	pch "winpch"
	filter "action:not vs20*"
		defines { "DIRECTORS" }
	filter "action:vs20*"
		defines { "DIRECTORS", "WINDOWS_IGNORE_PACKING_MISMATCH" }
	filter {}

	installers {
		["LL2CEinstall.exe"] = {
			kind = "nsis",
			sha256sum = "0806e1615eb94332bf805128d2d3857af420e93ee6f48692eebf17c05e9b14e2",
			link = "https://hamumu.itch.io/loonyland-2-winter-woods",
		}
	}

	webfiles { ["splash.jpg"] = "assets/splashes/loonyland2.jpg" }

project "mystic"
	sdl2_project()
	android_appname "Kid Mystic"
	icon_file "mystic"
	uses_ham()
	pch "winpch"

	installers {
		["mystic_install.exe"] = {
			kind = "inno",
			sha256sum = "c2d618176d23b974c01c00690b6afb0aaebd4c863dfff0bf8b1f66db1bdc2f65",
			link = "https://hamumu.itch.io/kid-mystic",
		}
	}

	webfiles { ["splash.jpg"] = "assets/splashes/mystic.jpg" }

-- premake5.lua
dofile "tools/build/gmake2_deps.lua"
dofile "tools/build/android_studio.lua"
dofile "tools/build/emscripten.lua"
dofile "tools/build/vscode.lua"

workspace "HamSandwich"
	location "build"
	configurations { "debug", "release", "debug64", "release64" }

	filter { "action:android-studio" }
		location "build/android"
		android_abis { "armeabi-v7a" }

local _current_project
function base_project(name)
	_current_project = name
	project(name)
		kind "WindowedApp"
		language "C++"
		cppdialect "C++17"
		architecture "x86"
		targetdir "%{wks.location}/%{cfg.toolset}-%{cfg.buildcfg}/%{prj.name}/"
		objdir "%{cfg.targetdir}/obj/"

		-- These emulate the `./run` script when running within VS.
		debugdir "%{wks.location}/game/%{prj.name}"
		debugargs { "window" }

		defines { 'PROJECT_NAME="%{prj.name}"' }

		files {
			"source/%{prj.name}/**.h",
			"source/%{prj.name}/**.cpp",
			"source/%{prj.name}/**.c",
		}

		filter "configurations:*64"
			architecture "x86_64"

		filter "configurations:debug*"
			kind "ConsoleApp"
			defines { "_DEBUG" }
			symbols "On"

		filter "configurations:release*"
			defines { "NDEBUG" }
			optimize "On"

		filter { "toolset:gcc", "system:Windows" }
			linkoptions { "-static-libgcc", "-static-libstdc++" }

		filter "action:vs20*"
			-- At least some versions of VS2017 don't recognize "C++17".
			cppdialect "C++latest"
			defines { "_CRT_SECURE_NO_WARNINGS", "NOMINMAX", "SDL_UNPREFIXED" }
			-- The MSVC dependency script puts the SDL2 binaries here.
			includedirs {
				"build/SDL2-msvc/include",
				"build/SDL2_mixer-msvc/include",
				"build/SDL2_image-msvc/include",
				"build/zlib-1.2.11",
			}
			libdirs {
				"build/SDL2-msvc/lib/x86",
				"build/SDL2_mixer-msvc/lib/x86",
				"build/SDL2_image-msvc/lib/x86",
				"build/zlib",
			}
			debugenvs { "PATH=$(ProjectDir)/lib/x86/;%PATH%" }

		filter "action:android-studio"
			defines { "SDL_UNPREFIXED" }
			buildoptions { "-fsigned-char", "-fexceptions" }

		filter { "toolset:emcc", "configurations:debug*" }
			linkoptions { "--emrun" }

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

function library(name)
	base_project(name)
		kind "StaticLib"
end

function sdl2_project(name)
	base_project(name)
		-- Android application metadata.
		android_package "com.platymuus.hamsandwich.%{prj.name}"
		android_assetdirs {
			"build/assets/%{prj.name}/",
			"assets/android/",
		}

		-- Emscripten metadata.
		webfiles "assets/emscripten/*"

		-- Link SDL2 in the correct sequence.
		filter { "system:Windows", "not action:vs*", "toolset:not emcc" }
			links "mingw32"
		filter { "system:Windows", "toolset:not emcc" }
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

local _recursive_links = {}
local _original_links = links
function links(name)
	if type(name) == 'table' then
		for _, v in ipairs(name) do
			links(v)
		end
		return
	end

	local our_links = _recursive_links[_current_project]
	if our_links == nil then
		our_links = {}
		_recursive_links[_current_project] = our_links
	end
	table.insert(our_links, name)

	_original_links(name)
	local their_links = _recursive_links[name]
	if their_links ~= nil then
		includedirs { "source/" .. name }
		for _, v in ipairs(their_links) do
			_original_links(v)
			table.insert(our_links, v)
		end
	end
end

library "libextract"
	links { "SDL2", "zlib" }

	filter "action:not vs20*"
		buildoptions { "-Wall", "-Wextra" }

library "ham"
	links { "libextract", "SDL2", "SDL2_mixer", "SDL2_image" }

	filter "action:not vs20*"
		buildoptions { "-Wall", "-Wextra" }

sdl2_project "lunatic"
	android_appname "Dr. Lunatic"
	icon_file "lunatic"
	links "ham"
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

sdl2_project "supreme"
	android_appname "Supreme With Cheese"
	icon_file "lunatic"
	links "ham"
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

sdl2_project "sleepless"
	android_appname "Sleepless Hollow"
	icon_file "lunatic"
	links "ham"
	pch "winpch"

	excludefiles {
		"monsterlist.cpp",
		"monsterai1.cpp",
		"monsterai2.cpp",
		"monsterai3.cpp",
		"monsterai4.cpp",
		"monsterhollow.cpp",
	}

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

sdl2_project "loonyland"
	android_appname "Loonyland: Halloween Hill"
	web_title "Loonyland"
	icon_file "loonyland"
	links "ham"
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

sdl2_project "loonyland2"
	android_appname "Loonyland 2: Winter Woods"
	web_title "Loonyland 2"
	icon_file "loonyland2"
	links "ham"
	pch "winpch"
	filter "action:not vs20*"
		defines { "DIRECTORS" }
	filter "action:vs20*"
		defines { "DIRECTORS", "WINDOWS_IGNORE_PACKING_MISMATCH" }
	filter {}

	excludefiles {
		"monster_ai.cpp",
	}

	installers {
		["LL2CEinstall.exe"] = {
			kind = "nsis",
			sha256sum = "0806e1615eb94332bf805128d2d3857af420e93ee6f48692eebf17c05e9b14e2",
			link = "https://hamumu.itch.io/loonyland-2-winter-woods",
		}
	}

	webfiles { ["splash.jpg"] = "assets/splashes/loonyland2.jpg" }

sdl2_project "mystic"
	android_appname "Kid Mystic"
	icon_file "mystic"
	links "ham"
	pch "winpch"

	installers {
		["mystic_install.exe"] = {
			kind = "inno",
			sha256sum = "c2d618176d23b974c01c00690b6afb0aaebd4c863dfff0bf8b1f66db1bdc2f65",
			link = "https://hamumu.itch.io/kid-mystic",
		}
	}

-- premake5.lua
workspace "JspEdit3"
	configurations { "debug", "release" }
	location "build"

project "jspedit"
    kind "WindowedApp"
    language "C++"
    cppdialect "C++17"
    architecture "x86"

    files { "source/**.cpp" }

    targetdir("build/%{cfg.toolset}-%{cfg.buildcfg}-%{cfg.architecture}/")
    objdir("build/%{cfg.toolset}-%{cfg.buildcfg}-%{cfg.architecture}/obj/")

    filter "configurations:debug"
        kind "ConsoleApp"
        defines { "_DEBUG" }
        symbols "On"

    filter "configurations:release"
        defines { "NDEBUG" }
        optimize "On"

    filter "toolset:gcc"
        buildoptions { "-Wall", "-Wextra", "-Werror" }
        linkoptions { "-static-libgcc", "-static-libstdc++" }

    -- SDL2 linking
    filter { "system:Windows", "not action:vs*" }
        links "mingw32"
    filter "system:Windows"
        links { "ws2_32", "winmm" }
    filter {}
    links { "SDL2main", "SDL2", "SDL2_image", "SDL2_ttf", "nfd" }

	-- Workaround for bug in premake5's gmake2 generator, which does
	-- not count .res (object) files as resources, only .rc (source)
    filter { "system:Windows", "toolset:not clang" }
        files { "source/resources.rc" }

	filter { "system:Windows", "action:gmake2", "toolset:not clang" }
		linkoptions { "%{cfg.objdir}/resources.res" }

	-- Support for embedding the icon in the file on non-Windows systems
	filter { "system:not Windows", "toolset:not clang" }
		files { "source/resources.rc" }
		files { "%{cfg.objdir}/resources.rc.o" }

	filter { "system:not Windows", "toolset:not clang", "files:**.rc" }
		buildmessage "%{file.name}"
		buildcommands { 'python3 ../rescomp.py "%{file.path}" "%{cfg.objdir}/%{file.basename}.rc.cpp"' }
		buildoutputs { "%{cfg.objdir}/resources.rc.cpp" }
		buildinputs { "rescomp.py" }

	filter {}

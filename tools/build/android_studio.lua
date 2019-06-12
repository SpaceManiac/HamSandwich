-- android_studio.lua
-- Provides an "android-studio" premake action which prepares an NDK build.

local p = premake
local m = {}
p.modules.android_studio = m

newaction {
	trigger = "android-studio",
	description = "Create Android SDK/NDK Gradle project",

	onStart = function()
		--print("onStart")
	end,

	onWorkspace = function(wks)
		-- print("onWorkspace", wks.name)
		p.generate(wks, "settings.gradle", m.settings_gradle)
		p.generate(wks, "build.gradle", m.build_gradle)
		p.generate(wks, "Application.mk", m.application_mk)
		p.generate(wks, "_config.mk", m.config_mk)
	end,

	onProject = function(prj)
		if m.is_application(prj) then
			p.generate(prj, prj.name .. "/AndroidManifest.xml", m.manifest_xml)
			p.generate(prj, prj.name .. "/Application.mk", m.prj_application_mk)
			p.generate(prj, prj.name .. "/Android.mk", m.android_mk)
			p.generate(prj, prj.name .. "/java/" .. prj.name .. "Activity.java", m.activity_java)

			-- TODO: this is probably the wrong time to do this?
			if prj.android_icon then
				os.execute('python3'
					.. ' tools/build/android_icons.py'
					.. ' "' .. prj.android_icon .. '"'
					.. ' "' .. prj.workspace.location .. '/' .. prj.name .. '/res"'
					.. ' "icon"')
			end
		else
			p.generate(prj, "lib" .. prj.name .. ".mk", m.android_mk)
		end
	end,

	execute = function()
		--print("execute")
	end,

	onEnd = function()
		--print("onEnd")
	end,
}

p.api.register {
	name = "android_package",
	scope = "project",
	kind = "string",
	tokens = true
}

m.DEFAULT_SDK = 19
p.api.register {
	name = "android_sdk",
	scope = "workspace",
	kind = "string"
}

m.DEFAULT_MIN_SDK = 16
p.api.register {
	name = "android_min_sdk",
	scope = "workspace",
	kind = "string"
}

p.api.register {
	name = "android_abis",
	scope = "workspace",
	kind = "list:string",
	allowed = {
		"armeabi",
		"armeabi-v7a",
		"arm64-v8a",
		"x86",
		"x86_64"
	}
}

p.api.register {
	name = "android_appname",
	scope = "project",
	kind = "string"
}

p.api.register {
	name = "android_icon",
	scope = "project",
	kind = "string",
	tokens = true
}

p.api.register {
	name = "android_assetdirs",
	scope = "project",
	kind = "list:string",
	tokens = true
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

-- WORKSPACE FILES --------------------------------------------------

function m.settings_gradle(wks)
	for prj in p.workspace.eachproject(wks) do
		if m.is_application(prj) then
			p.x('include "%s"', prj.name)
		end
	end
end

function m.build_gradle(wks)
	local sdkVersion = wks.android_sdk or m.DEFAULT_SDK
	local minSdkVersion = wks.android_min_sdk or m.DEFAULT_MIN_SDK

	p.push("buildscript {")
	p.push("repositories {")
	p.w("jcenter()")
	p.w("google()")
	p.pop("}")  -- repositories
	p.push("dependencies {")
	p.w("classpath 'com.android.tools.build:gradle:3.1.0'")
	p.pop("}")  -- dependencies
	p.pop("}")  -- buildscript

	p.push("subprojects {")
	p.push("repositories {")
	p.w("jcenter()")
	p.w("google()")
	p.pop("}")  -- repositories
	p.w("apply plugin: 'com.android.application'")
	-- All subprojects share the same 'build' directory to reduce duplicated
	-- work, but they do not share the same NDK json cache.
	p.w("buildDir = '../build'")
	p.push("android {")
	p.w("compileSdkVersion %d", sdkVersion)
	p.push("defaultConfig {")
	p.w("versionCode 1")
	p.w("versionName '1.0'")
	p.w("minSdkVersion %d", minSdkVersion)
	p.w("targetSdkVersion %d", sdkVersion)
	p.w("externalNativeBuild.ndkBuild.arguments 'APP_PLATFORM=android-%d'", minSdkVersion)
	p.pop("}")  -- defaultConfig
	--p.x("buildTypes.release.minifyEnabled false")
	--p.x("lintOptions.abortOnError false")
	p.push("sourceSets.main {")
	p.w("manifest.srcFile 'AndroidManifest.xml'")
	p.w("java.srcDir '../../SDL2/android-project/app/src/main/java'")
	p.w("java.srcDir 'java'")
	p.w("res.srcDir 'res'")
	p.pop("}")  -- sourceSets.main
	p.w("externalNativeBuild.ndkBuild.path 'Android.mk'")
	p.pop("}")  -- android
	p.pop("}")  -- subprojects

	for prj in p.workspace.eachproject(wks) do
		if m.is_application(prj) then
			p.push("project(':%s') {", prj.name)
			p.push("android {")
			p.push("defaultConfig {")
			p.w("applicationId '%s'", prj.android_package)
			p.pop("}")  -- defaultConfig
			for _, dir in ipairs(prj.android_assetdirs) do
				p.w("sourceSets.main.assets.srcDir '../../../%s'", dir)
			end
			p.pop("}")  -- android
			p.pop("}")  -- project(':%s')
		end
	end
end

function m.application_mk(wks)
	local minSdkVersion = wks.android_min_sdk or m.DEFAULT_MIN_SDK

	p.w("APP_PLATFORM = android-%d", minSdkVersion)
	p.w("APP_STL := c++_shared")  -- or c++_static

	local abis = ""
	for _, abi in ipairs(wks.android_abis) do
		abis = abis .. " " .. abi
	end
	p.w("APP_ABI :=%s", abis)
end

function m.config_mk(wks)
	-- SDL2_image support flags
	p.w("SUPPORT_PNG := true")
	p.w("SUPPORT_JPG := false")
	p.w("SUPPORT_WEBP := false")

	-- SDL2_mixer support flags
	p.w("SUPPORT_OGG := true")
	p.w("SUPPORT_FLAC := false")
	p.w("SUPPORT_MP3_MPG123 := false")
	p.w("SUPPORT_MOD_MODPLUG := false")
	p.w("SUPPORT_MID_TIMIDITY := false")
end

-- PROJECT FILES ----------------------------------------------------

function m.prj_application_mk(prj)
	p.w("include ../Application.mk")
end

function m.manifest_xml(prj)
	local sdkVersion = prj.workspace.android_sdk or m.DEFAULT_SDK
	local minSdkVersion = prj.workspace.android_min_sdk or m.DEFAULT_MIN_SDK
	local title = prj.android_appname or prj.name

	p.w('<?xml version="1.0" encoding="utf-8"?>')
	p.push('<manifest xmlns:android="http://schemas.android.com/apk/res/android"')
	p.w('package="%s"', prj.android_package)
	p.w('android:versionCode="1"')
	p.w('android:versionName="1.0"')
	p.w('android:installLocation="auto"')
	p.w('>')

	p.w('<uses-sdk android:minSdkVersion="%d" android:targetSdkVersion="%d" />', minSdkVersion, sdkVersion)

	-- OpenGL ES 2.0: <uses-feature android:glEsVersion="0x00020000" />

	-- Allow writing to external storage
	p.w('<uses-permission android:name="android.permisison.WRITE_EXTERNAL_STORAGE" />')
	-- other examples: VIBRATE, RECORD_AUDIO

	p.push('<application android:label="%s"', title)
	if prj.android_icon then
		p.w('android:icon="@mipmap/icon"')
	end
	p.w('android:allowBackup="true"')
	p.w('android:theme="@android:style/Theme.NoTitleBar.Fullscreen"')
	p.w('android:hardwareAccelerated="true"')
	p.w('>')

	p.w('<meta-data android:name="SDL_ENV.SDL_ACCELEROMETER_AS_JOYSTICK" android:value="0"/>')

	-- Must match the Java class and package name
	p.push('<activity android:name="%sActivity"', prj.name)
	p.w('android:label="%s"', title)
	p.w('android:configChanges="keyboard|keyboardHidden|orientation|screenSize"')
	p.w('>')

	p.push('<intent-filter>')
	p.w('<action android:name="android.intent.action.MAIN" />')
	p.w('<category android:name="android.intent.category.LAUNCHER" />')
	p.pop('</intent-filter>')

	p.pop('</activity>')
	p.pop('</application>')
	p.pop('</manifest>')
end

function m.android_mk(prj)
	-- TODO: handle multiple configurations
	local cfg = m.getcfg(prj, 'debug')

	if m.is_application(prj) then
		-- SDL2 includes, TODO base on prj dependencies
		p.w('include ../_config.mk')
		p.w('include ../../SDL2/Android.mk')
		p.w('include ../../SDL2_mixer/Android.mk')
		p.w('include ../../SDL2_image/Android.mk')

		-- local project includes
		for _, dep_prj in pairs(p.project.getdependencies(prj)) do
			p.w('include ../lib%s.mk', dep_prj.name)
		end
	end

	p.w('LOCAL_PATH := $(call my-dir)')
	p.w('include $(CLEAR_VARS)')
	p.w('LOCAL_MODULE := %s', prj.name)

	-- TODO base SDL part on prj dependencies
	p.w('LOCAL_SHARED_LIBRARIES := SDL2 SDL2_mixer SDL2_image')

	if m.is_application(prj) then
		local statics = ''
		for _, dep_prj in pairs(p.project.getdependencies(prj)) do
			statics = statics .. ' ' .. dep_prj.name
		end
		if statics then
			p.w('LOCAL_STATIC_LIBRARIES :=%s', statics)
		end
	end

	-- TODO: replace this with obeying cfg.includes below
	if not m.is_application(prj) then
		p.w('LOCAL_EXPORT_C_INCLUDES := ../../../source/%s', prj.name)
	end

	local cflags = ""
	for _, flag in ipairs(cfg.buildoptions) do
		cflags = cflags .. " " .. flag
	end
	for _, def in ipairs(cfg.defines) do
		cflags = cflags .. " -D" .. def
	end
	if cfg.cppdialect then
		cflags = cflags .. " -std=" .. cfg.cppdialect:lower()
	end

	p.w('LOCAL_CFLAGS :=%s', cflags)

	if m.is_application(prj) then
		p.w('LOCAL_LDLIBS := -llog')
	end

	local files = ""
	for _, file in ipairs(cfg.files) do
		local ext = path.getextension(file)
		if ext == ".c" or ext == ".cpp" then
			files = files .. " " .. file
		end
	end
	p.w('LOCAL_SRC_FILES :=%s', files)

	if m.is_application(prj) then
		p.w('include $(BUILD_SHARED_LIBRARY)')
	else
		p.w('include $(BUILD_STATIC_LIBRARY)')
	end
end

function m.activity_java(prj)
	p.w('package %s;', prj.android_package)
	p.w('import org.libsdl.app.SDLActivity;')
	p.push('public class %sActivity extends SDLActivity {', prj.name)

	p.w('@Override')
	p.push('protected String[] getLibraries() {')
	p.push('return new String[] {')

	-- TODO: base this on prj links
	p.w('"SDL2",')
	p.w('"SDL2_image",')
	p.w('"SDL2_mixer",')
	p.w('"%s"', prj.name)

	p.pop('};')  -- String[]
	p.pop('}')  -- getLibraries

	p.pop('}')  -- class
end

return m

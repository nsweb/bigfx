
function setBigfxTarget( _configuration, _platform, _basepath )
	local strtarget = string.format( "%s/bin/%s_%s/", _basepath, _configuration, _platform ) 
	local strobj = string.format( "%s/intermediate/%s_%s", _basepath, _configuration, _platform ) 
	configuration {_configuration, _platform}
		targetdir( strtarget )
		objdir( strobj )
end

function setBigfxLibs( _configuration, _platform, _bigfxpath )
	local platformname = _platform
	if _platform == "x64" then
		platformname = "Win64"
	end

	-- local glewpath
	-- if _platform == "native" then
	-- 	glewpath = string.format( "%s/3rdparty/glew-2.1.0/lib", _basepath )
	-- else
	-- 	glewpath = string.format( "%s/3rdparty/glew-2.1.0/lib/release/%s", _basepath, _platform )
	-- end

	-- local sdl2path = string.format( "%s/3rdparty/SDL2-2.0.16/lib/%s", _basepath, _platform)
	-- local sdl2options = string.format( "-F %s/3rdparty/SDL2-2.0.16/lib/%s", _basepath, _platform)

    local bgfxpath = string.format( "%s/3rdparty/bgfx/.build/%s_%s/bin", _bigfxpath, platformname, _ACTION)
	configuration {_configuration, _platform}
		--libdirs { glewpath, sdl2path, bgfxpath }
        libdirs { bgfxpath }
end

function createBigfxProject(_bigfxpath)
    project "bigfx"

    kind "StaticLib"
    language "C++"

    files { _bigfxpath .. "/src/**.h", _bigfxpath .. "/src/**.cpp" }

    local thirdpartypath = _bigfxpath .. "/3rdparty"
    local bxpath = thirdpartypath .. "/bx"
    includedirs {
                --PROJ_DIR .. "/3rdparty/SDL2-2.0.16/include",
                --PROJ_DIR .. "/3rdparty/glew-2.1.0/include",
                thirdpartypath .. "/zlib-1.2.11",
                thirdpartypath .. "/jsmn",
                --PROJ_DIR .. "/3rdparty/imgui",
                bxpath .. "/include",
                bxpath .. "/include/compat/msvc",
                thirdpartypath }

    defines { "_CRT_SECURE_NO_WARNINGS", "JSMN_STATIC", "BX_CONFIG_DEBUG=1", "ENTRY_CONFIG_IMPLEMENT_MAIN=1" } --"_USRDLL",
    flags { "NoNativeWChar", "NoEditAndContinue", "StaticRuntime", "NoExceptions" }

    local buildpath = _bigfxpath .. "/build"
    configuration "windows"
        setBigfxTarget( "Debug", "x64", buildpath )
        setBigfxTarget( "Release", "x64", buildpath )

    configuration "macosx"
        setBigfxTarget( "Debug", "native", buildpath )
        setBigfxTarget( "Release", "native", buildpath )

    configuration "Debug"
        defines { "_DEBUG" }
        flags { "Symbols" }

    configuration "Release"
        defines { "NDEBUG" }
        flags { "Optimize", "Symbols" }

    configuration "macosx"
        buildoptions { "-std=c++11" } --, "-stdlib=libc++" }
end

function createBgfxProject(_bgfxpath)
	project "bgfx" 

		kind "StaticLib"
		language "C++"

        files { _bgfxpath .. "**.h", _bgfxpath .. "*.cpp", _bgfxpath .. "*.sc" }
end

function createBigfxSolution()
	solution "bigfx"
        configurations { "Debug", "Release" }

        platforms { "x64", "native" }

        local bigfxpath = path.getabsolute("..")
        local bgfxpath = bigfxpath .. "/3rdparty/bgfx"
        createBgfxProject(bgfxpath)
        createBigfxProject(bigfxpath)
end

if solution() == nil then
    print("creating bifgx solution ...");
    createBigfxSolution()
end

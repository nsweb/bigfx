
function SetTarget( _configuration, _platform, _basepath )
	local strtarget = string.format( "%s/bin/%s_%s/", _basepath, _configuration, _platform ) 
	local strobj = string.format( "%s/intermediate/%s_%s", _basepath, _configuration, _platform ) 
	configuration {_configuration, _platform}
		targetdir( strtarget )
		objdir( strobj )
end

function SetLibs( _configuration, _platform, _basepath, _enginepath )
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

    local bgfxpath = string.format( "%s/.build/%s_%s/bin", _enginepath, platformname, _ACTION)
	configuration {_configuration, _platform}
		--libdirs { glewpath, sdl2path, bgfxpath }
        libdirs { bgfxpath }
end


--------------------------------------------------------
solution "bigfx"
	configurations { "Debug", "Release" }

	platforms { "x64", "native" }

    ---------------------------------------------------------
	project "bgfx" 
		kind "StaticLib"
		language "C++"

        local bgfxpath = path.getabsolute("../3rdparty")
        files { bgfxpath .. "**.h", bgfxpath .. "*.cpp", bgfxpath .. "*.sc" }

	---------------------------------------------------------
	project "bigfx" 
		kind "StaticLib"
		language "C++"

        local projpath = path.getabsolute("..")
        files { projpath .. "/src/**.h", projpath .. "/src/**.cpp" }

        local thirdpartypath = projpath .. "/3rdparty"
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
		
		configuration "windows"
			SetTarget( "Debug", "x64", projpath .. "/build" )
			SetTarget( "Release", "x64", projpath .. "/build" )
			
		configuration "macosx"
			SetTarget( "Debug", "native", projpath .. "/build" )
			SetTarget( "Release", "native", projpath .. "/build" )
			
		configuration "Debug"
			defines { "_DEBUG" }
			flags { "Symbols" }
 
		configuration "Release"
			defines { "NDEBUG" }
			flags { "Optimize", "Symbols" } 

		configuration "macosx"
            buildoptions { "-std=c++11" } --, "-stdlib=libc++" }


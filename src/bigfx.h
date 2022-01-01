
// bigfx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef BB_BIGBALL_H
#define BB_BIGBALL_H

// defined with this macro as being exported.
#ifdef BIGBALL_EXPORTS
#define BIGFX_API __declspec(dllexport)
#define BIGBALL_TEMPLATE 
#elif defined( BIGBALL_IMPORTS )
#define BIGFX_API __declspec(dllimport)
#define BIGBALL_TEMPLATE extern
#else
#define BIGFX_API
#define BIGBALL_TEMPLATE 
#endif


#if _WIN32 || _WIN64
	#include "targetver.h"

	//#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
	// Windows Header Files:
	//#include <windows.h>
#endif

#ifdef _DEBUG
	#define BB_BUILD_DEBUG		1
	#define BB_BUILD_RELEASE	0
#else
	#define BB_BUILD_DEBUG		0
	#define BB_BUILD_RELEASE	1
#endif

#include "bx/bx.h"
#include "core/core.h"
#include "core/memory.h"
#include "core/log.h"
#include "core/templates.h"
#include "math/functions.h"
#include "core/array.h"
#include "core/string.h"
#include "core/name.h"
#include "core/map.h"
#include "math/constants.h"
#include "math/vector.h"
#include "math/transform.h"


#endif // BB_BIGBALL_H
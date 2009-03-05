// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers



// TODO: reference additional headers your program requires here
#include <Windows.h>
extern "C" 
{
	#include "lua.h"
	#include "lauxlib.h"
	#include "lualib.h"
}

#include <map>
#include <set>
#include <list>
#include <stack>
#include <string>
#include <sstream>
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers



// TODO: reference additional headers your program requires here
#include <Windows.h>
#include <tchar.h>
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

#include <process.h>

#define BUFSIZE 1024*32

#ifdef _UNICODE
typedef std::wstring _string;
#else
typedef std::string _string;
#endif

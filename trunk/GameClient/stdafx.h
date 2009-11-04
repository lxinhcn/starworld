// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#ifndef WINVER
#define WINVER         0x0410
#endif
#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0410 
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT   0x0500 
#endif

// Windows Header Files:
#include <windows.h>
#include <atlconv.h>

// C RunTime Header Files
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <math.h>

// STL lib
#include <list>
#include <stack>
#include <queue>
#include <map>
#include <string>
#include <vector>
#include <strstream>

#include <DbgHelp.h>

#include <hge.h>
#include <hgesprite.h>
#include <hgeanim.h>
#include <hgefont.h>

// ¸¨Öú¿â
#include "helplib.h"
#include "UILib.h"
using namespace UILib;


#include <loki/Singleton.h>

#ifdef _DEBUG
#pragma comment( lib, "atlsd.lib" )
#else
#pragma comment( lib, "atls.lib" )
#endif
#pragma comment( lib, "dbghelp.lib" )

// TODO: reference additional headers your program requires here
//#ifdef _DEBUG
//#include <crtdbg.h>
//#include <vld.h>
//#define ASSERT		_ASSERTE
//#define ASSERT_MSG	_RPT0
//#else
//#define ASSERT 
//#define ASSERT_MSG 
//#endif
//
//#ifdef _UNICODE
//	typedef std::wstring _string;
//#else
//	typedef std::string _string;
//#endif
//


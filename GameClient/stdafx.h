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

#include <hge.h>
#include <hgesprite.h>
#include <hgeanim.h>
#include <hgefont.h>

// ¸¨Öú¿â
#include "CommonLib.h"
using namespace XGC;
using namespace XGC::common;

// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料
#define _WIN32_WINNT 0x0500
// TODO: 在此处引用程序要求的附加头文件
#include <Windows.h>
#include <MMSystem.h>
#include <WinSock2.h>
#include <atlconv.h>
// crt
#include <malloc.h>
#include <stdio.h>
#include <time.h>
#include <tchar.h>
#include <process.h>

// stl
#include <stack>
#include <queue>
#include <string>
#include <map>
#include <list>
#include <algorithm>

// common head file
#include "macro_define.h"
#include "logger.h"
#pragma comment( lib, "ws2_32.lib" )
#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "atls.lib" )

#ifdef TOOLLIB_EXPORTS
#define TOOLLIB_API __declspec(dllexport)
#else
#define TOOLLIB_API __declspec(dllimport)
#endif	//TOOLLIB_EXPORTS

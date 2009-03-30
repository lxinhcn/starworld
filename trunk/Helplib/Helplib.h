#pragma once

#ifndef _WIN32_WINNT		// 允许使用特定于 Windows XP 或更高版本的功能。
#define _WIN32_WINNT 0x0501	// 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif						

#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料
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
#include <map>
#include <list>
#include <stack>
#include <queue>
#include <vector>
#include <string>
#include <algorithm>

// common head file
#include "defines.h"
#include "Include\console.h"
#include "Include\console_buffer.h"
#include "include\utility.h"
#include "include\logger.h"
#include "include\logger_adapter.h"
#include "include\auto_list.h"
#include "include\critical_lock.h"
#include "include\dynamic_buffer.h"
#include "include\file_helper.h"
#include "include\iocp.h"
#include "include\lookaside.h"
#include "include\net_helper.h"
#include "include\netevent.h"
#include "include\netpackage.h"
#include "include\netstream.h"
#include "include\netselect.h"
#include "include\tls.h"
#include "include\xtimer.h"

#pragma comment( lib, "ws2_32.lib" )
#pragma comment( lib, "winmm.lib" )
#ifdef _DEBUG
	#pragma comment( lib, "atlsd.lib" )
#else
	#pragma comment( lib, "atls.lib" )
#endif

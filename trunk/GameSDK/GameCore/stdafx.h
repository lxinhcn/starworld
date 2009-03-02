// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <windows.h>

// TODO: 在此处引用程序要求的附加头文件
// C 运行时头文件
#include <stdlib.h>
#include <crtdbg.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <time.h>

#include <list>
#include <stack>
#include <queue>
#include <map>
#include <string>
#include <vector>

#include <loki/TypeManip.h>
#include <loki/Singleton.h>
#include <loki/Function.h>

#ifdef _DEBUG
#include <crtdbg.h>
#define new new( 4/*_CLIENT_BLOCK*/, __FILE__, __LINE__ )
#define ASSERT( expr )	_ASSERTE( expr )
#define ASSERT_MSG( expr, msg )		_RPT0( expr, msg )
#else
#define ASSERT 
#define ASSERT_MSG 
#endif

// TODO: 在此处引用程序要求的附加头文件
#define SAFE_DELETE(p)	if( p ){	delete ( p ); ( p ) = NULL; }
#define SAFE_DELETE_ARRAY(p)	if( p ){	delete[] ( p ); ( p ) = NULL; }
#define SAFE_RELEASE(p)	if( p ){	( p )->Release();	( p ) = NULL; }
#pragma comment( lib, "winmm.lib" )
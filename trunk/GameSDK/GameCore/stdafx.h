// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���ǳ��õ��������ĵ���Ŀ�ض��İ����ļ�
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�:
#include <windows.h>

// TODO: �ڴ˴����ó���Ҫ��ĸ���ͷ�ļ�
// C ����ʱͷ�ļ�
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

// TODO: �ڴ˴����ó���Ҫ��ĸ���ͷ�ļ�
#define SAFE_DELETE(p)	if( p ){	delete ( p ); ( p ) = NULL; }
#define SAFE_DELETE_ARRAY(p)	if( p ){	delete[] ( p ); ( p ) = NULL; }
#define SAFE_RELEASE(p)	if( p ){	( p )->Release();	( p ) = NULL; }
#pragma comment( lib, "winmm.lib" )
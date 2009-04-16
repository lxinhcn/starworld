#ifndef _MACRO_DEFINE_H
#define _MACRO_DEFINE_H

#ifdef TOOLLIB_EXPORTS
#define TOOLLIB_API __declspec(dllexport)
#else
#define TOOLLIB_API __declspec(dllimport)
#endif	//TOOLLIB_EXPORTS

//--------------------------------------------------------------------------------------------------------//
// 取数组元素个数
//--------------------------------------------------------------------------------------------------------//
#define SAFE_DELETE( p )		delete (p); (p) = NULL;
#define SAFE_DELETE_ARRAY( p )	delete[] (p); (p) = NULL;
#define SAFE_RELEASE(p)			if(p){(p)->Release();(p) = NULL;}

//--------------------------------------------------------------------------------------------------------//
// 定义转义字符串类型,用于支持UNICODE
//--------------------------------------------------------------------------------------------------------//
#if defined( _UNICODE )
	typedef std::wstring						_string;
	typedef wchar_t								_tchar;
#else
	typedef std::string							_string;
	typedef char								_tchar;
#endif // _UNICODE

#if !defined(_point64)
	#if !defined(__midl) && (defined(_X86_) || defined(_M_IX86)) && _MSC_VER >= 1300
		#define _point64 __w64
	#else
		#define _point64
	#endif
#endif

#ifndef _DEFINES
#define _DEFINES
	typedef wchar_t		_wchar;
	typedef char		int8;
	typedef short 		int16;
	typedef int			int32;
	typedef __int64		int64;
	typedef long		long32;

	typedef unsigned char		byte;
	typedef unsigned short		uint16;
	typedef unsigned int		uint32;
	typedef unsigned long		ulong, ulong32;
	typedef unsigned __int64	uint64;

	typedef const _tchar*		_lpctstr;
	typedef _tchar*				_lptstr;

	typedef const char*			_lpcstr;
	typedef char*				_lpstr;

	typedef const wchar_t*		_lpcwstr;
	typedef wchar_t*			_lpwstr;

	typedef unsigned long	_point64 long_ptr, *plong_ptr;
	typedef unsigned int	_point64 uint_ptr, *puint_ptr;

#endif

//--------------------------------------------------------------------------------------------------------//
// ASSERT 定义
//--------------------------------------------------------------------------------------------------------//
#ifdef _DEBUG
	#include <crtdbg.h>
	#ifdef ASSERT
	#undef ASSERT
	#endif

	#ifdef ASSERT_MSG
	#undef ASSERT_MSG
	#endif

	#ifdef ASSERT_POINTER
	#undef ASSERT_POINTER
	#endif

	#ifdef DEBUG_NEW
	#undef DEBUG_NEW
	#endif

	#define ASSERT(expr)				_ASSERT(expr)
	#define ASSERT_MSG(expr, msg)		_ASSERT_EXPR(expr,msg)
	#define ASSERT_POINTER(expr)		_ASSERT_EXPR(expr, _T("无效的指针"))
	#define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
	#define NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
#else

	#ifdef ASSERT
	#undef ASSERT
	#endif

	#ifdef ASSERT_MSG
	#undef ASSERT_MSG
	#endif

	#ifdef ASSERT_POINTER
	#undef ASSERT_POINTER
	#endif

	#ifdef DEBUG_NEW
	#undef DEBUG_NEW
	#endif

	#define ASSERT(expr)				(void(0))
	#define ASSERT_MSG(expr, msg)		(void(0))
	#define ASSERT_POINTER(expr)		(void(0))
	#define DEBUG_NEW
	#define NEW
#endif // _DEBUG
//--------------------------------------------------------------------------------------------------------//
#endif // _MACRO_DEFINE_H
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

//--------------------------------------------------------------------------------------------------------//
// 定义转义字符串类型,用于支持UNICODE
//--------------------------------------------------------------------------------------------------------//
#if defined( _UNICODE )
	typedef std::wstring						_string;
#else
	typedef std::string							_string;
#endif // _UNICODE

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
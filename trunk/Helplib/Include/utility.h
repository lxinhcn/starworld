#pragma once
#ifndef _COMMON_H
#define _COMMON_H
#include "macro_define.h"

/////////////////////////////////////////////////////////////////////////////
// Verify that a null-terminated string points to valid memory

namespace debug_helper
{
	inline BOOL IsValidString(LPCWSTR psz, size_t nMaxLength = INT_MAX)
	{
#if defined(_DEBUG)
		// Implement ourselves because ::IsBadStringPtrW() isn't implemented on Win9x.
		if ((psz == NULL) || (nMaxLength == 0))
			return FALSE;

		LPCWSTR pch;
		LPCWSTR pchEnd;
		__try
		{
			wchar_t ch;

			pch = psz;
			pchEnd = psz+nMaxLength-1;
			ch = *(volatile wchar_t*)pch;
			while ((ch != L'\0') && (pch != pchEnd))
			{
				pch++;
				ch = *(volatile wchar_t*)pch;
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return FALSE;
		}

		return TRUE;
#else
		(nMaxLength);
		return (psz != NULL);
#endif	
	}

	// Verify that a null-terminated string points to valid memory
	inline BOOL IsValidString(LPCSTR psz, size_t nMaxLength = UINT_MAX)
	{
#if defined(_DEBUG)
		if (psz == NULL)
			return FALSE;
		return ::IsBadStringPtrA(psz, nMaxLength) == 0;
#else
		(nMaxLength);
		return (psz != NULL);
#endif
	}

	// Verify that a pointer points to valid memory
	inline BOOL IsValidAddress(const void* p, size_t nBytes,
		BOOL bReadWrite = TRUE)
	{
#if defined(_DEBUG)
		return ((p != NULL) && !IsBadReadPtr(p, nBytes) &&
			(!bReadWrite || !IsBadWritePtr(const_cast<LPVOID>(p), nBytes)));
#else
		nBytes,bReadWrite;
		return (p != NULL);
#endif
	}

	template<typename T>
		inline void AssertValidObject(const T *pOb)
	{
		ATLASSERT(pOb);
		ATLASSERT(AtlIsValidAddress(pOb, sizeof(T)));
		if(pOb)
			pOb->AssertValid();
	}
}

namespace helper
{
	//--------------------------------------------------------------------------------------------------------//
	// 翻转数据字节顺序
	// src 源数据
	//--------------------------------------------------------------------------------------------------------//
	template< typename T >
	T ReversalValue( T src )
	{
		T value = 0;
		size_t size = sizeof( T );
		char* pDest = reinterpret_cast< char* >( &value );
		char* pSrc = reinterpret_cast< char* >( &src );
		pSrc += size;
		while( size-- )	*pDest++ = *--pSrc;
		return value;
	}

	TOOLLIB_API bool GetModulePath( HMODULE hModule, LPTSTR pszPath, DWORD nOutSize );

	//--------------------------------------------------------------------------------------------------------//
	// 规格化路径，将路径规格化为绝对路径 类似于"E:\tools\Downloads\"
	// 规格化后的路径如果传入绝对路径则只判断路径是否合法，如果传入相对路径则以当前程序所在的目录为当前目录
	// 目录规格化后都以'\'结尾。
	// [in]lpszPath				:	需要规格化的路径
	// [out]lpszNormalizePath	:	规格化后的输出缓冲区
	// [in]dwSize				:	输出缓冲区大小
	//--------------------------------------------------------------------------------------------------------//
	TOOLLIB_API bool NormalizePath( LPCTSTR lpszPath, LPTSTR lpszNormalizePath, DWORD dwSize );

	// 判断是否UTF8文本
	TOOLLIB_API INT IsTextUTF8( LPSTR lpstrInputStream, INT iLen );

	#define R2VA(modulebase, rva)  (((PBYTE)modulebase) + rva) // Relative Virtual Address to Virtual Address conversion.
	// 为API打补丁（IAT）
	TOOLLIB_API BOOL patchimport( HMODULE importmodule, LPCSTR exportmodulename, LPCSTR exportmodulepath, LPCSTR importname, LPCVOID replacement );

	// 清除补丁
	TOOLLIB_API VOID restoreimport (HMODULE importmodule, LPCSTR exportmodulename, LPCSTR exportmodulepath, LPCSTR importname, LPCVOID replacement);

	class TOOLLIB_API CErrorDescribe
	{
	public:
		CErrorDescribe();
		~CErrorDescribe();

		LPCTSTR GetDescribe();
		DWORD	GetErrorCode();

	private:
		DWORD	dwErrorCode;
		LPVOID	lpszDescribe;
	};
}
#endif // _COMMON_H
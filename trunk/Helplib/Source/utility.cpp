#include "StdAfx.h"
#include <DbgHelp.h>
#include "utility.h"

#pragma comment( lib, "dbghelp.lib" )

namespace helper
{
	TOOLLIB_API bool GetModulePath( HMODULE hModule, LPTSTR pszPath, DWORD nOutSize )
	{
		DWORD nLen = ::GetModuleFileName( hModule, pszPath, nOutSize );

		if( nLen )
		{
			while( nLen && pszPath[nLen] != '\\' ) --nLen;
			pszPath[nLen+1] = 0;
			return true;
		}
		return false;
	}

	//--------------------------------------------------------------------------------------------------------//
	// 规格化路径，将路径规格化为绝对路径 类似于"E:\tools\Downloads\"
	// 规格化后的路径如果传入绝对路径则只判断路径是否合法，如果传入相对路径则以当前程序所在的目录为当前目录
	// 目录规格化后都以'\'结尾。
	// [in]lpszPath				:	需要规格化的路径
	// [out]lpszNormalizePath	:	规格化后的输出缓冲区
	// [in]dwSize				:	输出缓冲区大小
	//--------------------------------------------------------------------------------------------------------//
	bool NormalizePath( LPCTSTR lpszPath, LPTSTR lpszNormalizePath, DWORD dwSize )
	{
		// 该函数由于对szModulePath 的缓冲区判断不严格，可能导致一个栈溢出。需要更严格的缓冲区长度判断。
		if( lpszNormalizePath == NULL || dwSize == 0 ) return false;
		lpszNormalizePath[0] = 0;
		//TCHAR szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szName[_MAX_FNAME], szExt[_MAX_EXT];
		//_tsplitpath_s( lpszPath
		//	, szDrive, _countof(szDrive)
		//	, szDir, _countof(szDir)
		//	, szName, _countof(szName)
		//	, szExt, _countof(szExt) );

		if( lpszPath[0] == '.' )
		{
			TCHAR szModulePath[_MAX_PATH+_MAX_FNAME];
			::GetModuleFileName( NULL, szModulePath, sizeof( szModulePath )/sizeof( szModulePath[0] ) );

			// 去掉文件名
			TCHAR* p = _tcsrchr( szModulePath, _T('\\') );
			if( p ) *p = 0;

			int i = 0;
			int dot = 0;
			CONST TCHAR* pDirector = lpszPath;
			do
			{
				if( lpszPath[i] == '\\' || lpszPath[i] == 0 )
				{
					if( dot == 2 ) // 后退一级目录
					{
						TCHAR* p = _tcsrchr( szModulePath, _T('\\') );
						if( p ) *p = 0; 
						else 
						{
							return false;
						}
					}
					else if( dot == 0 ) // 相对目录的子目录
					{
						_tcsncat( szModulePath, pDirector, lpszPath + i - pDirector );
						ASSERT( _tcslen( lpszNormalizePath ) < dwSize );
					}
					dot = 0;
					pDirector = &lpszPath[i];
				}
				else if( lpszPath[i] == '.' )
				{
					++dot;
				}
				else
				{
					dot = 0;
				}

				if( dot > 2 ) 
				{
					return false;
				}
			}while( lpszPath[i++] );

			_tcsncpy( lpszNormalizePath, szModulePath, dwSize );
			ASSERT( _tcslen( lpszNormalizePath ) < dwSize );
		}
		else
		{
			_tcsncpy( lpszNormalizePath, lpszPath, dwSize );
			ASSERT( _tcslen( lpszNormalizePath ) < dwSize );
		}
		// 格式化目录，最后的一个字符一定要是'\'
		//TCHAR* right = _tcsrchr( lpszNormalizePath, _T('\\') );
		//if( *(right+1) != 0 )
		//{
		//	_tcsncat( lpszNormalizePath, _T("\\"), 1 );
		//	ASSERT( _tcslen( lpszNormalizePath ) < dwSize );
		//}
		return true;
	}
	/* IsTextUTF8
	*
	* UTF-8 is the encoding of Unicode based on Internet Society RFC2279
	* ( See http://www.cis.ohio-state.edu/htbin/rfc/rfc2279.html )
	*
	* Basicly:
	* 0000 0000-0000 007F - 0xxxxxxx  (ascii converts to 1 octet!)
	* 0000 0080-0000 07FF - 110xxxxx 10xxxxxx    ( 2 octet format)
	* 0000 0800-0000 FFFF - 1110xxxx 10xxxxxx 10xxxxxx (3 octet format)
	* (this keeps going for 32 bit unicode) 
	* 
	*
	* Return value:  TRUE, if the text is in UTF-8 format.
	*                FALSE, if the text is not in UTF-8 format.
	*                We will also return FALSE is it is only 7-bit ascii, so the right code page
	*                will be used.
	*
	*                Actually for 7 bit ascii, it doesn't matter which code page we use, but
	*                notepad will remember that it is utf-8 and "save" or "save as" will store
	*                the file with a UTF-8 BOM.  Not cool.
	*/
	// 这段代码是有bug的，微软记事本很著名的“联通”的bug也是这段代码引起的，暂时还没找到更好的识别无BOM的UTF8文本的更好的办法
	INT IsTextUTF8( LPSTR lpstrInputStream, INT iLen )
	{
		INT   i;
		DWORD cOctets;  // octets to go in this UTF-8 encoded character
		UCHAR chr;
		BOOL  bAllAscii= TRUE;

		cOctets= 0;
		for( i=0; i < iLen; i++ ) {
			chr= *(lpstrInputStream+i);

			if( (chr&0x80) != 0 ) bAllAscii= FALSE;

			if( cOctets == 0 )  {
				//
				// 7 bit ascii after 7 bit ascii is just fine.  Handle start of encoding case.
				//
				if( chr >= 0x80 ) {  
					//
					// count of the leading 1 bits is the number of characters encoded
					//
					do {
						chr <<= 1;
						cOctets++;
					}
					while( (chr&0x80) != 0 );

					cOctets--;                        // count includes this character
					if( cOctets == 0 ) return FALSE;  // must start with 11xxxxxx
				}
			}
			else {
				// non-leading bytes must start as 10xxxxxx
				if( (chr&0xC0) != 0x80 ) {
					return FALSE;
				}
				cOctets--;                           // processed another octet in encoding
			}
		}

		//
		// End of text.  Check for consistency.
		//

		if( cOctets > 0 ) {   // anything left over at the end is an error
			return FALSE;
		}

		if( bAllAscii ) {     // Not utf-8 if all ascii.  Forces caller to use code pages for conversion
			return FALSE;
		}

		return TRUE;
	}


	//////////////////////////////////////////////////////////////////////////
	CErrorDescribe::CErrorDescribe()
		: lpszDescribe( NULL )
		, dwErrorCode( GetLastError() )
	{
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR)&lpszDescribe,
			0,
			NULL );
		// Process any inserts in lpMsgBuf.
		// ...
	}

	CErrorDescribe::~CErrorDescribe()
	{
		// Free the buffer.
		LocalFree( lpszDescribe );
	}

	LPCTSTR CErrorDescribe::GetDescribe()
	{
		return (LPCTSTR)lpszDescribe;
	}

	DWORD CErrorDescribe::GetErrorCode()
	{
		return dwErrorCode;
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//
	BOOL patchimport( HMODULE importmodule, LPCSTR exportmodulename, LPCSTR exportmodulepath, LPCSTR importname, LPCVOID replacement )
	{
		HMODULE                  exportmodule;
		IMAGE_THUNK_DATA        *iate;
		IMAGE_IMPORT_DESCRIPTOR *idte;
		FARPROC                  import;
		DWORD                    protect;
		IMAGE_SECTION_HEADER    *section;
		ULONG                    size;

		ASSERT(exportmodulename != NULL);

		// Locate the importing module's Import Directory Table (IDT) entry for the
		// exporting module. The importing module actually can have several IATs --
		// one for each export module that it imports something from. The IDT entry
		// gives us the offset of the IAT for the module we are interested in.
		idte = (IMAGE_IMPORT_DESCRIPTOR*)ImageDirectoryEntryToDataEx((PVOID)importmodule, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &size, &section);
		if (idte == NULL) 
		{
			// This module has no IDT (i.e. it imports nothing).
			return FALSE;
		}
		while (idte->OriginalFirstThunk != 0x0) {
			if (_stricmp((PCHAR)R2VA(importmodule, idte->Name), exportmodulename) == 0) 
			{
				// Found the IDT entry for the exporting module.
				break;
			}
			idte++;
		}
		if (idte->OriginalFirstThunk == 0x0) 
		{
			// The importing module does not import anything from the exporting
			// module.
			return FALSE;
		}

		// Get the *real* address of the import. If we find this address in the IAT,
		// then we've found the entry that needs to be patched.
		if (exportmodulepath != NULL) 
		{
			// Always try to use the full path if available. There seems to be a bug
			// (or is this a new feature of the "side-by-side" kruft?) where
			// GetModuleHandle sometimes fails if the full path is not supplied for
			// DLLs in the side-by-side cache.
			exportmodule = GetModuleHandleA(exportmodulepath);
		}
		else 
		{
			// No full path available. Try using just the module name by itself.
			exportmodule = GetModuleHandleA(exportmodulename);
		}
		ASSERT(exportmodule != NULL);
		import = GetProcAddress(exportmodule, importname);
		ASSERT(import != NULL); // Perhaps the named export module does not actually export the named import?

		// Locate the import's IAT entry.
		iate = (IMAGE_THUNK_DATA*)R2VA(importmodule, idte->FirstThunk);
		while (iate->u1.Function != 0x0) {
			if (iate->u1.Function == (DWORD_PTR)import) {
				// Found the IAT entry. Overwrite the address stored in the IAT
				// entry with the address of the replacement. Note that the IAT
				// entry may be write-protected, so we must first ensure that it is
				// writable.
				VirtualProtect(&iate->u1.Function, sizeof(iate->u1.Function), PAGE_READWRITE, &protect);
				iate->u1.Function = (DWORD)(DWORD_PTR)replacement;
				VirtualProtect(&iate->u1.Function, sizeof(iate->u1.Function), protect, &protect);

				// The patch has been installed in the import module.
				return TRUE;
			}
			iate++;
		}

		// The import's IAT entry was not found. The importing module does not
		// import the specified import.
		return FALSE;
	}

	VOID restoreimport (HMODULE importmodule, LPCSTR exportmodulename, LPCSTR exportmodulepath, LPCSTR importname, LPCVOID replacement)
	{
		HMODULE                  exportmodule;
		IMAGE_THUNK_DATA        *iate;
		IMAGE_IMPORT_DESCRIPTOR *idte;
		FARPROC                  import;
		DWORD                    protect;
		IMAGE_SECTION_HEADER    *section;
		ULONG                    size;

		ASSERT(exportmodulename != NULL);

		// Locate the importing module's Import Directory Table (IDT) entry for the
		// exporting module. The importing module actually can have several IATs --
		// one for each export module that it imports something from. The IDT entry
		// gives us the offset of the IAT for the module we are interested in.
		idte = (IMAGE_IMPORT_DESCRIPTOR*)ImageDirectoryEntryToDataEx((PVOID)importmodule, TRUE,
			IMAGE_DIRECTORY_ENTRY_IMPORT, &size, &section);
		if (idte == NULL) {
			// This module has no IDT (i.e. it imports nothing).
			return;
		}
		while (idte->OriginalFirstThunk != 0x0) {
			if (_stricmp((PCHAR)R2VA(importmodule, idte->Name), exportmodulename) == 0) {
				// Found the IDT entry for the exporting module.
				break;
			}
			idte++;
		}
		if (idte->OriginalFirstThunk == 0x0) {
			// The importing module does not import anything from the exporting
			// module.
			return;
		}

		// Get the *real* address of the import.
		if (exportmodulepath != NULL) {
			// Always try to use the full path if it'savailable. There seems to be a
			// bug (or is this a new feature of the "side-by-side" kruft?) where
			// GetModuleHandle sometimes fails if the full path is not supplied for
			// DLLs in the side-by-side cache.
			exportmodule = GetModuleHandleA(exportmodulepath);
		}
		else {
			// No full path available. Try using just the module name by itself.
			exportmodule = GetModuleHandleA(exportmodulename);
		}
		ASSERT(exportmodule != NULL);
		import = GetProcAddress(exportmodule, importname);
		ASSERT(import != NULL); // Perhaps the named export module does not actually export the named import?

		// Locate the import's original IAT entry (it currently has the replacement
		// address in it).
		iate = (IMAGE_THUNK_DATA*)R2VA(importmodule, idte->FirstThunk);
		while (iate->u1.Function != 0x0) {
			if (iate->u1.Function == (DWORD_PTR)replacement) {
				// Found the IAT entry. Overwrite the address stored in the IAT
				// entry with the import's real address. Note that the IAT entry may
				// be write-protected, so we must first ensure that it is writable.
				VirtualProtect(&iate->u1.Function, sizeof(iate->u1.Function), PAGE_READWRITE, &protect);
				iate->u1.Function = (DWORD)(DWORD_PTR)import;
				VirtualProtect(&iate->u1.Function, sizeof(iate->u1.Function), protect, &protect);
				break;
			}
			iate++;
		}
	}
	//////////////////////////////////////////////////////////////////////////

	std::string ws2s(const std::wstring& ws)
	{
		const wchar_t* _Source = ws.c_str();
		size_t _Dsize = ws.size() + 1;
		char *_Dest = ( char* )_alloca(_Dsize);
		memset(_Dest,0,_Dsize);
		wcstombs(_Dest,_Source,_Dsize);
		return _Dest;
	}

	std::string ws2s(const wchar_t* ws)
	{
		const wchar_t* _Source = ws;
		size_t _Dsize = wcslen(ws) + 1;
		char *_Dest = ( char* )_alloca(_Dsize);
		memset(_Dest,0,_Dsize);
		wcstombs(_Dest,_Source,_Dsize);
		return _Dest;
	}

	std::wstring s2ws(const std::string& s)
	{
		const char* _Source = s.c_str();
		size_t _Dsize = s.size() + 1;
		wchar_t *_Dest = (wchar_t*)_alloca(_Dsize*sizeof(wchar_t) );
		wmemset(_Dest, 0, _Dsize);
		mbstowcs(_Dest,_Source,_Dsize);
		return _Dest;
	}

	std::wstring s2ws(const char* s)
	{
		const char* _Source = s;
		size_t _Dsize = strlen( s ) + 1;
		wchar_t *_Dest = (wchar_t*)_alloca(_Dsize*sizeof(wchar_t) );
		wmemset(_Dest, 0, _Dsize);
		mbstowcs(_Dest,_Source,_Dsize);
		return _Dest;
	}
}
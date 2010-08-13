#pragma once
#ifndef _XUI_IME_H
#define _XUI_IME_H

#include <Windows.h>
#include <ObjBase.h>
#include <Dimm.h>
#include "XUI_Header.h"

#define MAX_CANDLIST 10
#define MAX_COMPSTRING_SIZE 256

#define GETLANG()		LOWORD(XUI_IME::m_hklCurrent)
#define GETPRIMLANG()	((WORD)PRIMARYLANGID(GETLANG()))
#define GETSUBLANG()	SUBLANGID(GETLANG())

#define LANG_CHS MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED)
#define LANG_CHT MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_TRADITIONAL)

#define IMEUI_STATE_OFF		0
#define IMEUI_STATE_ON		1
#define IMEUI_STATE_ENGLISH	2

#define INDICATOR_NON_IME	0
#define INDICATOR_CHS		1
#define INDICATOR_CHT		2
#define INDICATOR_KOREAN	3
#define INDICATOR_JAPANESE	4
#define INDICATOR_ENGLISH	5

namespace XGC
{
	namespace ui
	{
		class XUI_IME
		{
		public:
			XUI_IME(void);
			~XUI_IME(void);

			static bool Initialize();
			static void RenderImeWindow();

			static void CheckInputLocale();
			static void OnInputLangChange();
			static void CheckToggleState();
			static void ResetCompositionString();

			static HINSTANCE	m_hDllImm32;	// IMM32 DLL handle
			static HINSTANCE	m_hDllIme;		// IME DLL handle
			static HINSTANCE	m_hDllVer;		// Version DLL handle
			static HIMC			m_hImcDef;		// Default input context
			static HKL			m_hklCurrent;	// Current keyboard layout of the process

			static iRect		m_rcWindow;
			struct CCandList
			{
				CCandList();
				std::list< std::wstring >	l;
				std::string strBuffer;
				_int32	nCaretPos;
				_int32	nFirstSelected; // First character position of the selected string in HoriCand
				_int32	nHoriSelectedLen; // Length of the selected string in HoriCand
				_uint32	dwCount;       // Number of valid entries in the candidate list
				_uint32	dwSelection;   // Currently selected candidate entry relative to page top
				_uint32	dwPageSize;
				_int32	nReadingError; // Index of the error character
				bool	bShowWindow;   // Whether the candidate list window is visible
				iRect	rcCandidate;   // Candidate rectangle computed and filled each time before rendered
			};

			static CCandList	m_CandList;	// 输入法绘制结构。
			static wchar_t		m_CompString[MAX_COMPSTRING_SIZE];
			static wchar_t		m_CompStringAttr[MAX_COMPSTRING_SIZE];

			static INPUTCONTEXT* (WINAPI * _ImmLockIMC)( HIMC );
			static BOOL		(WINAPI * _ImmUnlockIMC)( HIMC );
			static LPVOID	(WINAPI * _ImmLockIMCC)( HIMCC );
			static BOOL		(WINAPI * _ImmUnlockIMCC)( HIMCC );
			static BOOL		(WINAPI * _ImmDisableTextFrameService)( DWORD );
			static LONG		(WINAPI * _ImmGetCompositionStringW)( HIMC, DWORD, LPVOID, DWORD );
			static DWORD	(WINAPI * _ImmGetCandidateListW)( HIMC, DWORD, LPCANDIDATELIST, DWORD );
			static HIMC		(WINAPI * _ImmGetContext)( HWND );
			static BOOL		(WINAPI * _ImmReleaseContext)( HWND, HIMC );
			static HIMC		(WINAPI * _ImmAssociateContext)( HWND, HIMC );
			static BOOL		(WINAPI * _ImmGetOpenStatus)( HIMC );
			static BOOL		(WINAPI * _ImmSetOpenStatus)( HIMC, BOOL );
			static BOOL		(WINAPI * _ImmGetConversionStatus)( HIMC, LPDWORD, LPDWORD );
			static HWND		(WINAPI * _ImmGetDefaultIMEWnd)( HWND );
			static UINT		(WINAPI * _ImmGetDescriptionW)( HKL, LPWSTR, UINT );
			static UINT		(WINAPI * _ImmGetIMEFileNameA)( HKL, LPSTR, UINT );
			static UINT		(WINAPI * _ImmGetVirtualKey)( HWND );
			static BOOL		(WINAPI * _ImmNotifyIME)( HIMC, DWORD, DWORD, DWORD );
			static BOOL		(WINAPI * _ImmSetConversionStatus)( HIMC, DWORD, DWORD );
			static BOOL		(WINAPI * _ImmSimulateHotKey)( HWND, DWORD );
			static BOOL		(WINAPI * _ImmIsIME)( HKL );

			// Function pointers: Traditional Chinese IME
			static UINT (WINAPI * _GetReadingString)( HIMC, UINT, LPWSTR, PINT, BOOL*, PUINT );
			static BOOL (WINAPI * _ShowReadingWindow)( HIMC, BOOL );

			// Function pointers: Verion library imports
			static BOOL (APIENTRY * _VerQueryValueA)( const LPVOID, LPSTR, LPVOID *, PUINT );
			static BOOL (APIENTRY * _GetFileVersionInfoA)( LPSTR, DWORD, DWORD, LPVOID );
			static DWORD (APIENTRY * _GetFileVersionInfoSizeA)( LPSTR, LPDWORD );
		};
	}
};
#endif
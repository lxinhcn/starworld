#pragma once
#ifndef _XUI_IME_H
#define _XUI_IME_H
#include "GuiHeader.h"
namespace UILib
{
	class XUI_IME
	{
	public:
		XUI_IME(void);
		~XUI_IME(void);

		static bool Initialize();

		static HINSTANCE	m_hDllImm32;	// IMM32 DLL handle
		static HINSTANCE	m_hDllVer;		// Version DLL handle
		static HIMC			m_hImcDef;		// Default input context
		static HKL			m_hklCurrent;	// Current keyboard layout of the process

		struct CCandList
		{
			CCandList();
			wchar_t awszCandidate[MAX_CANDLIST][256];
			std::string strBuffer;
			int32   nFirstSelected; // First character position of the selected string in HoriCand
			int32   nHoriSelectedLen; // Length of the selected string in HoriCand
			uint32	dwCount;       // Number of valid entries in the candidate list
			uint32	dwSelection;   // Currently selected candidate entry relative to page top
			uint32	dwPageSize;
			int32   nReadingError; // Index of the error character
			bool  bShowWindow;   // Whether the candidate list window is visible
			x_rect rcCandidate;   // Candidate rectangle computed and filled each time before rendered
		};

		static CCandList m_Candlist;	// 输入法绘制结构。

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
};
#endif
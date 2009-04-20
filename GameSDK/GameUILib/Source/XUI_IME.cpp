#include "XUI_IME.h"
#include "GuiSystem.h"

#define GETPROCADDRESS( Module, APIName )	*(FARPROC*)&_##APIName = GetProcAddress( Module, #APIName )
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

namespace UILib
{
	static _lpctstr g_aszIndicator[] =
	{
		TEXT("A"),
#ifdef UNICODE
		L"\x7B80\x0000",
		L"\x7E41\x0000",
		L"\xac00\x0000",
		L"\x3042\x0000",
#else
		"\xd6\xd0\x00",
		"\xa4\xa4\x00",
		"\xb0\xa1\x00",
		"\x82\xa0\x00",
#endif
		TEXT("En"),
	};
	static _lpctstr	g_pszIndicatior = g_aszIndicator[0];
	static bool		g_bVerticalCand = true;
	static bool		g_bChineseIME = true;
	static uint32	g_dwState = IMEUI_STATE_OFF;
	static UINT		g_uCodePage = 0;

	XUI_IME::CCandList	XUI_IME::m_Candlist;	// ���뷨���ƽṹ��
	HINSTANCE			XUI_IME::m_hDllImm32	= NULL;	// IMM32 DLL handle
	HINSTANCE			XUI_IME::m_hDllIme		= NULL;	// IMM32 DLL handle
	HINSTANCE			XUI_IME::m_hDllVer		= NULL;		// Version DLL handle
	HIMC				XUI_IME::m_hImcDef		= NULL;		// Default input context
	HKL					XUI_IME::m_hklCurrent	= NULL;	// Current keyboard layout of the process
	x_rect				XUI_IME::m_rcWindow( 640, 480, 730, 500 );

	INPUTCONTEXT*	(WINAPI * XUI_IME::_ImmLockIMC)( HIMC );
	BOOL			(WINAPI * XUI_IME::_ImmUnlockIMC)( HIMC );
	LPVOID			(WINAPI * XUI_IME::_ImmLockIMCC)( HIMCC );
	BOOL			(WINAPI * XUI_IME::_ImmUnlockIMCC)( HIMCC );
	BOOL			(WINAPI * XUI_IME::_ImmDisableTextFrameService)( DWORD );
	LONG			(WINAPI * XUI_IME::_ImmGetCompositionStringW)( HIMC, DWORD, LPVOID, DWORD );
	DWORD			(WINAPI * XUI_IME::_ImmGetCandidateListW)( HIMC, DWORD, LPCANDIDATELIST, DWORD );
	HIMC			(WINAPI * XUI_IME::_ImmGetContext)( HWND );
	BOOL			(WINAPI * XUI_IME::_ImmReleaseContext)( HWND, HIMC );
	HIMC			(WINAPI * XUI_IME::_ImmAssociateContext)( HWND, HIMC );
	BOOL			(WINAPI * XUI_IME::_ImmGetOpenStatus)( HIMC );
	BOOL			(WINAPI * XUI_IME::_ImmSetOpenStatus)( HIMC, BOOL );
	BOOL			(WINAPI * XUI_IME::_ImmGetConversionStatus)( HIMC, LPDWORD, LPDWORD );
	HWND			(WINAPI * XUI_IME::_ImmGetDefaultIMEWnd)( HWND );
	UINT			(WINAPI * XUI_IME::_ImmGetIMEFileNameA)( HKL, LPSTR, UINT );
	UINT			(WINAPI * XUI_IME::_ImmGetVirtualKey)( HWND );
	BOOL			(WINAPI * XUI_IME::_ImmNotifyIME)( HIMC, DWORD, DWORD, DWORD );
	BOOL			(WINAPI * XUI_IME::_ImmSetConversionStatus)( HIMC, DWORD, DWORD );
	BOOL			(WINAPI * XUI_IME::_ImmSimulateHotKey)( HWND, DWORD );
	BOOL			(WINAPI * XUI_IME::_ImmIsIME)( HKL );

	// Function pointers: Traditional Chinese IME
	UINT			(WINAPI * XUI_IME::_GetReadingString)( HIMC, UINT, LPWSTR, PINT, BOOL*, PUINT );
	BOOL			(WINAPI * XUI_IME::_ShowReadingWindow)( HIMC, BOOL );

	// Function pointers: Verion library imports
	BOOL			(APIENTRY * XUI_IME::_VerQueryValueA)( const LPVOID, LPSTR, LPVOID *, PUINT );
	BOOL			(APIENTRY * XUI_IME::_GetFileVersionInfoA)( LPSTR, DWORD, DWORD, LPVOID );
	DWORD			(APIENTRY * XUI_IME::_GetFileVersionInfoSizeA)( LPSTR, LPDWORD );

	XUI_IME::XUI_IME(void)
	{
	}

	XUI_IME::~XUI_IME(void)
	{
	}

	bool XUI_IME::Initialize()
	{
		if( m_hDllImm32 ) // Only need to do once
			return true;

		WCHAR wszPath[MAX_PATH+1];
		if( !::GetSystemDirectory( wszPath, MAX_PATH+1 ) )
			return false;
		wcsncat( wszPath, L"\\imm32.dll", MAX_PATH );
		m_hDllImm32 = LoadLibrary( wszPath );
		if( m_hDllImm32 )
		{
			GETPROCADDRESS( m_hDllImm32, ImmLockIMC );
			GETPROCADDRESS( m_hDllImm32, ImmUnlockIMC );
			GETPROCADDRESS( m_hDllImm32, ImmLockIMCC );
			GETPROCADDRESS( m_hDllImm32, ImmUnlockIMCC );
			GETPROCADDRESS( m_hDllImm32, ImmDisableTextFrameService );
			GETPROCADDRESS( m_hDllImm32, ImmGetCompositionStringW );
			GETPROCADDRESS( m_hDllImm32, ImmGetCandidateListW );
			GETPROCADDRESS( m_hDllImm32, ImmGetContext );
			GETPROCADDRESS( m_hDllImm32, ImmReleaseContext );
			GETPROCADDRESS( m_hDllImm32, ImmAssociateContext );
			GETPROCADDRESS( m_hDllImm32, ImmGetOpenStatus );
			GETPROCADDRESS( m_hDllImm32, ImmSetOpenStatus );
			GETPROCADDRESS( m_hDllImm32, ImmGetConversionStatus );
			GETPROCADDRESS( m_hDllImm32, ImmGetDefaultIMEWnd );
			GETPROCADDRESS( m_hDllImm32, ImmGetIMEFileNameA );
			GETPROCADDRESS( m_hDllImm32, ImmGetVirtualKey );
			GETPROCADDRESS( m_hDllImm32, ImmNotifyIME );
			GETPROCADDRESS( m_hDllImm32, ImmSetConversionStatus );
			GETPROCADDRESS( m_hDllImm32, ImmSimulateHotKey );
			GETPROCADDRESS( m_hDllImm32, ImmIsIME );
			GETPROCADDRESS( m_hDllImm32, GetReadingString );
			GETPROCADDRESS( m_hDllImm32, ShowReadingWindow );

		}

		if( !::GetSystemDirectory( wszPath, MAX_PATH+1 ) )
			return false;

		wcsncat( wszPath, L"\\version.dll", MAX_PATH );

		m_hDllVer = LoadLibrary( wszPath );
		if( m_hDllVer )
		{
			GETPROCADDRESS( m_hDllVer, VerQueryValueA );
			GETPROCADDRESS( m_hDllVer, GetFileVersionInfoA );
			GETPROCADDRESS( m_hDllVer, GetFileVersionInfoSizeA );
		}

		HWND hWnd = GuiSystem::Instance().GetHWND();

		m_hImcDef = _ImmGetContext( hWnd );
		_ImmReleaseContext( hWnd, m_hImcDef );

		CheckInputLocale();
		CheckToggleState();

		return true;
	}

	XUI_IME::CCandList::CCandList()
		: nFirstSelected(0) // First character position of the selected string in HoriCand
		, nHoriSelectedLen(0) // Length of the selected string in HoriCand
		, dwCount(0)       // Number of valid entries in the candidate list
		, dwSelection(0)   // Currently selected candidate entry relative to page top
		, dwPageSize(0)
		, nReadingError(0) // Index of the error character
		, bShowWindow(true)   // Whether the candidate list window is visible
		, rcCandidate( x_rect( 0, 0, 100, 240 ) )
	{
		ZeroMemory( awszCandidate, sizeof(awszCandidate) );
		strBuffer = "������";
	}

	void XUI_IME::RenderImeWindow()
	{
		XUI_IFont* pFont = GuiSystem::Instance().GetDefaultFont();

		XUI_DrawRect( m_rcWindow, XUI_ARGB(0xcc,0xaa, 0xaa, 0xaa), XUI_ARGB(0xcc,0x77, 0x77, 0x77) );
		if( g_dwState == IMEUI_STATE_ON )
		{
			XUI_DrawText( g_pszIndicatior, pFont, (float)m_rcWindow.left, (float)m_rcWindow.top + 1 );
		}
		else
		{
			XUI_DrawText( g_aszIndicator[INDICATOR_ENGLISH], pFont, (float)m_rcWindow.left, (float)m_rcWindow.top + 1 );
		}
		x_size strSize = pFont->GetStringSize( g_pszIndicatior );

		//XUI_DrawCharacter( )
	}

	void XUI_IME::CheckInputLocale()
	{
		static HKL hklPrev = 0;
		m_hklCurrent = GetKeyboardLayout( 0 );
		if ( hklPrev == m_hklCurrent )
		{
			return;
		}
		hklPrev = m_hklCurrent;
		switch ( GETPRIMLANG() )
		{
			// Simplified Chinese
		case LANG_CHINESE:
			g_bVerticalCand = true;
			switch ( GETSUBLANG() )
			{
			case SUBLANG_CHINESE_SIMPLIFIED:
				g_pszIndicatior = g_aszIndicator[INDICATOR_CHS];
				//g_bVerticalCand = GetImeId() == 0;
				g_bVerticalCand = false;
				break;
			case SUBLANG_CHINESE_TRADITIONAL:
				g_pszIndicatior = g_aszIndicator[INDICATOR_CHT];
				break;
			default:	// unsupported sub-language
				g_pszIndicatior = g_aszIndicator[INDICATOR_NON_IME];
				break;
			}
			break;
			// Korean
		case LANG_KOREAN:
			g_pszIndicatior = g_aszIndicator[INDICATOR_KOREAN];
			g_bVerticalCand = false;
			break;
			// Japanese
		case LANG_JAPANESE:
			g_pszIndicatior = g_aszIndicator[INDICATOR_JAPANESE];
			g_bVerticalCand = true;
			break;		   
		default:
			g_pszIndicatior = g_aszIndicator[INDICATOR_NON_IME];
		}
		char szCodePage[8];
		int iRc = GetLocaleInfoA( MAKELCID( GETLANG(), SORT_DEFAULT ), LOCALE_IDEFAULTANSICODEPAGE, szCodePage, _countof( szCodePage ) ); iRc;
		g_uCodePage = atol( szCodePage );
	}

	void XUI_IME::CheckToggleState()
	{
		CheckInputLocale();

		bool bIme = _ImmIsIME( m_hklCurrent ) != 0
			&& ( ( 0xF0000000 & (DWORD_PTR)m_hklCurrent ) == 0xE0000000 ); // Hack to detect IME correctly. When IME is running as TIP, ImmIsIME() returns true for CHT US keyboard.
		g_bChineseIME = ( GETPRIMLANG() == LANG_CHINESE ) && bIme;

		HIMC himc;
		HWND hWnd = GuiSystem::Instance().GetHWND();
		if( NULL != ( himc = _ImmGetContext( hWnd ) ) ) 
		{
			if (g_bChineseIME) 
			{
				DWORD dwConvMode, dwSentMode;
				_ImmGetConversionStatus(himc, &dwConvMode, &dwSentMode);
				g_dwState = ( dwConvMode & IME_CMODE_NATIVE ) ? IMEUI_STATE_ON : IMEUI_STATE_ENGLISH;
			}
			else
			{
				g_dwState = ( bIme && _ImmGetOpenStatus( himc ) != 0 ) ? IMEUI_STATE_ON : IMEUI_STATE_OFF;
			}
			_ImmReleaseContext( hWnd, himc);
		}
		else
			g_dwState = IMEUI_STATE_OFF;
	}

	void XUI_IME::OnInputLangChange()
	{
		CheckInputLocale();
		CheckToggleState();

		char szImeFile[MAX_PATH + 1];

		_GetReadingString = NULL;
		_ShowReadingWindow = NULL;
		if( _ImmGetIMEFileNameA( m_hklCurrent, szImeFile, sizeof(szImeFile)/sizeof(szImeFile[0]) - 1 ) == 0 )
			return;

		if( m_hDllIme ) FreeLibrary( m_hDllIme );
		m_hDllIme = LoadLibraryA( szImeFile );
		if ( !m_hDllIme )
			return;
		_GetReadingString = (UINT (WINAPI*)(HIMC, UINT, LPWSTR, PINT, BOOL*, PUINT))
			( GetProcAddress( m_hDllIme, "GetReadingString" ) );
		_ShowReadingWindow =(BOOL (WINAPI*)(HIMC, BOOL))
			( GetProcAddress( m_hDllIme, "ShowReadingWindow" ) );

		if( _ShowReadingWindow )
		{
			HWND hWnd = GuiSystem::Instance().GetHWND();
			HIMC himc = _ImmGetContext( hWnd );
			if( NULL != himc ) 
			{
				_ShowReadingWindow( himc, FALSE );
				_ImmReleaseContext( hWnd, himc );
			}
		}
	}

	void XUI_IME::ResetCompositionString()
	{
		m_Candlist.strBuffer.empty();
	}
};
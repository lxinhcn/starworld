#include "Ime.h"
#ifdef UNICODE
typedef wchar_t _tchar;
#else
typedef char _tchar;
#endif
#define GETPROCADDRESS( Module, APIName )	*(FARPROC*)&_##APIName = GetProcAddress( Module, #APIName )
static const wchar_t* g_aszIndicator[] =
{
	L"En",
	L"A",
	L"\x7B80\x0000",
	L"\x7E41\x0000",
	L"\xac00\x0000",
	L"\x3042\x0000",
};

static const wchar_t*	g_pszIndicatior = g_aszIndicator[0];
static wchar_t	g_pszDescript[260];
static bool		g_bVerticalCand = true;
static bool		g_bChineseIME = true;
static unsigned int	g_dwState = IMEUI_STATE_OFF;
static unsigned int	g_uCodePage = 0;

wchar_t				IME::m_CompString[MAX_COMPSTRING_SIZE];
wchar_t				IME::m_CompStringAttr[MAX_COMPSTRING_SIZE];

IME::CCandList	IME::m_CandList;	// 输入法绘制结构。
HINSTANCE			IME::m_hDllImm32	= NULL;	// IMM32 DLL handle
HINSTANCE			IME::m_hDllIme		= NULL;	// IMM32 DLL handle
HINSTANCE			IME::m_hDllVer		= NULL;		// Version DLL handle
HIMC				IME::m_hImcDef		= NULL;		// Default input context
HKL					IME::m_hklCurrent	= NULL;	// Current keyboard layout of the process
RECT				IME::m_rcWindow = { 640, 480, 730, 500 };

INPUTCONTEXT*	(WINAPI * IME::_ImmLockIMC)( HIMC );
BOOL			(WINAPI * IME::_ImmUnlockIMC)( HIMC );
LPVOID			(WINAPI * IME::_ImmLockIMCC)( HIMCC );
BOOL			(WINAPI * IME::_ImmUnlockIMCC)( HIMCC );
BOOL			(WINAPI * IME::_ImmDisableTextFrameService)( DWORD );
LONG			(WINAPI * IME::_ImmGetCompositionStringW)( HIMC, DWORD, LPVOID, DWORD );
DWORD			(WINAPI * IME::_ImmGetCandidateListW)( HIMC, DWORD, LPCANDIDATELIST, DWORD );
HIMC			(WINAPI * IME::_ImmGetContext)( HWND );
BOOL			(WINAPI * IME::_ImmReleaseContext)( HWND, HIMC );
HIMC			(WINAPI * IME::_ImmAssociateContext)( HWND, HIMC );
BOOL			(WINAPI * IME::_ImmGetOpenStatus)( HIMC );
BOOL			(WINAPI * IME::_ImmSetOpenStatus)( HIMC, BOOL );
BOOL			(WINAPI * IME::_ImmGetConversionStatus)( HIMC, LPDWORD, LPDWORD );
HWND			(WINAPI * IME::_ImmGetDefaultIMEWnd)( HWND );
UINT			(WINAPI * IME::_ImmGetDescriptionW)( HKL, LPWSTR, UINT );
UINT			(WINAPI * IME::_ImmGetIMEFileNameA)( HKL, LPSTR, UINT );
UINT			(WINAPI * IME::_ImmGetVirtualKey)( HWND );
BOOL			(WINAPI * IME::_ImmNotifyIME)( HIMC, DWORD, DWORD, DWORD );
BOOL			(WINAPI * IME::_ImmSetConversionStatus)( HIMC, DWORD, DWORD );
BOOL			(WINAPI * IME::_ImmSimulateHotKey)( HWND, DWORD );
BOOL			(WINAPI * IME::_ImmIsIME)( HKL );

// Function pointers: Traditional Chinese IME
UINT			(WINAPI * IME::_GetReadingString)( HIMC, UINT, LPWSTR, PINT, BOOL*, PUINT );
BOOL			(WINAPI * IME::_ShowReadingWindow)( HIMC, BOOL );

// Function pointers: Verion library imports
BOOL			(APIENTRY * IME::_VerQueryValueA)( const LPVOID, LPSTR, LPVOID *, PUINT );
BOOL			(APIENTRY * IME::_GetFileVersionInfoA)( LPSTR, DWORD, DWORD, LPVOID );
DWORD			(APIENTRY * IME::_GetFileVersionInfoSizeA)( LPSTR, LPDWORD );

IME::IME(void)
{
}

IME::~IME(void)
{
}

bool IME::Initialize( HWND hWnd )
{
	if( m_hDllImm32 ) // Only need to do once
		return true;

	wchar_t wszPath[MAX_PATH+1];
	g_pszDescript[0] = 0;

	if( !::GetSystemDirectoryW( wszPath, MAX_PATH+1 ) )
		return false;
	wcsncat( wszPath, L"\\imm32.dll", MAX_PATH );
	m_hDllImm32 = LoadLibraryW( wszPath );
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
		GETPROCADDRESS( m_hDllImm32, ImmGetDescriptionW );
		GETPROCADDRESS( m_hDllImm32, ImmGetVirtualKey );
		GETPROCADDRESS( m_hDllImm32, ImmNotifyIME );
		GETPROCADDRESS( m_hDllImm32, ImmSetConversionStatus );
		GETPROCADDRESS( m_hDllImm32, ImmSimulateHotKey );
		GETPROCADDRESS( m_hDllImm32, ImmIsIME );
		GETPROCADDRESS( m_hDllImm32, GetReadingString );
		GETPROCADDRESS( m_hDllImm32, ShowReadingWindow );

	}

	if( !::GetSystemDirectoryW( wszPath, MAX_PATH+1 ) )
		return false;

	wcsncat( wszPath, L"\\version.dll", MAX_PATH );

	m_hDllVer = LoadLibraryW( wszPath );
	if( m_hDllVer )
	{
		GETPROCADDRESS( m_hDllVer, VerQueryValueA );
		GETPROCADDRESS( m_hDllVer, GetFileVersionInfoA );
		GETPROCADDRESS( m_hDllVer, GetFileVersionInfoSizeA );
	}

	m_hImcDef = _ImmGetContext( hWnd );
	_ImmReleaseContext( hWnd, m_hImcDef );

	CheckInputLocale();
	CheckToggleState( hWnd );

	return true;
}

IME::CCandList::CCandList()
	: nFirstSelected(0) // First character position of the selected string in HoriCand
	, nHoriSelectedLen(0) // Length of the selected string in HoriCand
	, dwCount(0)       // Number of valid entries in the candidate list
	, dwSelection(0)   // Currently selected candidate entry relative to page top
	, dwPageSize(0)
	, nReadingError(0) // Index of the error character
	, bShowWindow(true)   // Whether the candidate list window is visible
{
	rcCandidate.left = 0;
	rcCandidate.top = 0; 
	rcCandidate.right = 120;
	rcCandidate.bottom = 240;
}

void IME::RenderImeWindow()
{
	//int IndicatiorWidth = XUI_GetStringWidth( hFont, g_pszIndicatior );
	//int DescriptWidth = XUI_GetStringWidth( hFont, g_pszDescript );
	//int nFontHeight = XUI_GetCharacterHeight( hFont );

	//m_rcWindow.right = m_rcWindow.left + IndicatiorWidth + DescriptWidth + 4;
	//m_rcWindow.bottom = m_rcWindow.top + nFontHeight + 2;

	//XUI_DrawRect( m_rcWindow, XUI_ARGB(0xcc,0xaa, 0xaa, 0xaa), XUI_ARGB(0xcc,0x77, 0x77, 0x77) );
	//XUI_DrawText( g_pszIndicatior, hFont, m_rcWindow.left + 1, m_rcWindow.top + 1 );
	//XUI_DrawText( g_pszDescript, hFont, m_rcWindow.left + 3 + IndicatiorWidth, m_rcWindow.top + 1 );
}

short IME::IsImeInput()
{
	return g_pszIndicatior - g_aszIndicator[0];
}

short IME::CheckInputLocale()
{
	static HKL hklPrev = 0;
	m_hklCurrent = GetKeyboardLayout( 0 );
	if ( hklPrev != m_hklCurrent )
	{
		hklPrev = m_hklCurrent;
		UINT ret = _ImmGetDescriptionW( hklPrev, g_pszDescript, _countof(g_pszDescript) );
		g_pszDescript[ret] = 0;
		// Hack to detect IME correctly. When IME is running as TIP, ImmIsIME() returns true for CHT US keyboard.
		if( _ImmIsIME( hklPrev ) != 0 && ( ( 0xF0000000 & (DWORD_PTR)m_hklCurrent ) == 0xE0000000 ) )
		{
			switch ( GETPRIMLANG() )
			{
				// Simplified Chinese
			case LANG_CHINESE:
				g_bVerticalCand = true;
				switch ( GETSUBLANG() )
				{
				case SUBLANG_CHINESE_SIMPLIFIED:
					g_pszIndicatior = g_aszIndicator[INDICATOR_CHS];
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
		}
		else
			g_pszIndicatior = g_aszIndicator[INDICATOR_ENGLISH];
	}

	char szCodePage[8];
	int iRc = GetLocaleInfoA( MAKELCID( GETLANG(), SORT_DEFAULT ), LOCALE_IDEFAULTANSICODEPAGE, szCodePage, _countof( szCodePage ) ); iRc;
	g_uCodePage = atol( szCodePage );

	return g_pszIndicatior - g_pszIndicatior;
}

void IME::CheckToggleState( HWND hWnd )
{
	CheckInputLocale();

	bool bIme = _ImmIsIME( m_hklCurrent ) != 0
		&& ( ( 0xF0000000 & (DWORD_PTR)m_hklCurrent ) == 0xE0000000 ); // Hack to detect IME correctly. When IME is running as TIP, ImmIsIME() returns true for CHT US keyboard.
	g_bChineseIME = ( GETPRIMLANG() == LANG_CHINESE ) && bIme;

	HIMC himc;
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

void IME::OnInputLangChange( HWND hWnd )
{
	CheckInputLocale();
	CheckToggleState( hWnd );

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
		HIMC himc = _ImmGetContext( hWnd );
		if( NULL != himc ) 
		{
			_ShowReadingWindow( himc, FALSE );
			_ImmReleaseContext( hWnd, himc );
		}
	}
}

void IME::ResetCompositionString()
{
	m_CandList.strBuffer.empty();
}

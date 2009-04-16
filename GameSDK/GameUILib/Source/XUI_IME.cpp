#include "XUI_IME.h"
#define GETPROCADDRESS( Module, APIName )	*(FARPROC*)&_##APIName = GetProcAddress( Module, #APIName )
namespace UILib
{

	XUI_IME::CCandList	XUI_IME::m_Candlist;	// 输入法绘制结构。
	HINSTANCE			XUI_IME::m_hDllImm32 = NULL;	// IMM32 DLL handle
	HINSTANCE			XUI_IME::m_hDllVer = NULL;		// Version DLL handle
	HIMC				XUI_IME::m_hImcDef = NULL;		// Default input context
	HKL					XUI_IME::m_hklCurrent = NULL;	// Current keyboard layout of the process

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
		strBuffer = "阿萨德";
	}
};
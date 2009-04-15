#include "StdAfx.h"
#include "ClientIME.h"

CClientIME::CClientIME(void)
{
}

CClientIME::~CClientIME(void)
{
}

bool XUI_IME::Initialize()
{
	if( m_hDllImm32 ) // Only need to do once
		return;

	FARPROC Temp;

	WCHAR wszPath[MAX_PATH+1];
	if( !::GetSystemDirectory( wszPath, MAX_PATH+1 ) )
		return;
	wcscat( wszPath, MAX_PATH, "\\imm32.dll" );
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
		return;

	wcscat( wszPath, MAX_PATH, "\\version.dll" );

	m_hDllVer = LoadLibrary( wszPath );
	if( m_hDllVer )
	{
		GETPROCADDRESS( m_hDllVer, VerQueryValueA );
		GETPROCADDRESS( m_hDllVer, GetFileVersionInfoA );
		GETPROCADDRESS( m_hDllVer, GetFileVersionInfoSizeA );
	}
}

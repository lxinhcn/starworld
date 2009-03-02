#include "GuiHeader.h"
#include "XUI_Base.h"

namespace UILib
{
	const UI_MSGMAP* XUI_Base::GetMessageMap() const
	{
		return &XUI_Base::messageMap;
	}

	const UI_MSGMAP	XUI_Base::messageMap = 
	{
		NULL,
			&XUI_Base::_messageEntries[0]
	};

	const UI_MSGMAP_ENTRY XUI_Base::_messageEntries[] =
	{
		{ 0, 0, 0, 0, uiSig_end, 0 }     // nothing here
	};

	XUI_Base::XUI_Base()
	{
	}

	XUI_Base::~XUI_Base()
	{
	}

	// 通过nID和nCode在消息映射表里查询表项，并调用消息处理函数。
	bool XUI_Base::OnCmdMsg( UINT nID, int nCode, void* lpExtra, UI_HANDLE_INFO* pHandler )
	{
		const UI_MSGMAP* pMessageMap;
		const UI_MSGMAP_ENTRY* lpEntry;
		UINT nMsg = HIWORD(nCode);
		nCode = LOWORD(nCode);
		if( nMsg == 0 )	nMsg = WM_COMMAND;
		for( pMessageMap = GetMessageMap(); pMessageMap != NULL; pMessageMap = pMessageMap->pBaseMap )
		{
			lpEntry = FindMessageEntry( pMessageMap->lpEntry, nMsg, nCode, nID );
			if( lpEntry )
			{
				return DispatchCmdMsg( nID, nCode, lpEntry->pfn, lpExtra, lpEntry->nSig );
			}
		}
		return false;
	}

	const UI_MSGMAP_ENTRY*	XUI_Base::FindMessageEntry( const UI_MSGMAP_ENTRY* lpEntry, UINT nMsg, INT nEvent, UINT nID )
	{
		while ( lpEntry->nSig != uiSig_end )
		{
			if (	lpEntry->nMessage == nMsg &&
				lpEntry->nEvent == nEvent &&
				nID >= lpEntry->nID &&
				nID <= lpEntry->nIDLast
				)
			{
				return lpEntry;
			}
			lpEntry++;
		}
		return NULL;    // not found
	}

	HRESULT	XUI_Base::DispatchCmdMsg( UINT nID, int nEvent, UI_PMSG pfn, void* lpExtra, UINT_PTR nSig )
	{
		union	pfnSig	mmf;
		BOOL	bResult = FALSE;
		LRESULT lResult = 0;
		mmf.pfnNormal = pfn;
		switch( nSig )
		{
		case 	uiSig_Cmd:					// VOID	(VOID)
			( this->*mmf.pfn_Cmd )();
			break;

		case	uiSig_Cmd_Range:				// BOOL (UINT)
			( this->*mmf.pfn_Cmd_Range)( nID );
			break;

		case	uiSig_Notify:				// BOOL (HANDLE)
			( this->*mmf.pfn_Notify )( (UILib::NMUIHDR*)lpExtra, lResult );
			break;

		case	uiSig_Notify_Range:
			( this->*mmf.pfn_Notify_Range )( nID, (UILib::NMUIHDR*)lpExtra, lResult );
			break;

			//case	uiSig_b_wl:
			//	( this->*mmf.pfn_b_w_l )( MAKELONG( nID, nEvent ), (LPARAM)lpExtra );

		default:
			return FALSE;
		}
		return TRUE;
	}


	void XUI_Base::AddCommonListener(XUI_Base* pListener)
	{
		// 避免重复添加
		if( std::find( m_pListeners.begin(), m_pListeners.end(), pListener ) != m_pListeners.end() )	return;
		m_pListeners.push_back(pListener);
	}

	void XUI_Base::RemoveCommonListener(XUI_Base* pListener)
	{
		std::vector<XUI_Base*>::iterator itr;
		for(itr=m_pListeners.begin(); itr!=m_pListeners.end(); itr++)
		{
			if (*itr==pListener)
			{
				itr = m_pListeners.erase(itr);
				break;
			}
		}
	}
}
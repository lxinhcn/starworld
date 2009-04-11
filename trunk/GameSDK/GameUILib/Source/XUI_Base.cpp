#include "GuiHeader.h"
#include "XUI_Base.h"
#include "GuiSystem.h"

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
	LRESULT XUI_Base::OnCmdMsg( UINT nID, int nCode, void* lpExtra, UI_HANDLE_INFO* pHandler )
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
				DispatchCmdMsg( nID, nCode, lpEntry->pfn, lpExtra, lpEntry->nSig );
			}
		}
		return true;
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

	LRESULT XUI_Base::DefMsgProc( UINT nMsg, WPARAM wParam, LPARAM lParam )
	{
		CONST UI_MSGMAP* pMessageMap = NULL;
		for( pMessageMap = GetMessageMap(); pMessageMap != NULL; pMessageMap = pMessageMap->pBaseMap )
		{
			CONST UI_MSGMAP_ENTRY* lpEntry = pMessageMap->lpEntry;
			while( lpEntry->nSig != uiSig_end )
			{
				if( lpEntry->nMessage == nMsg )
				{
					union	pfnSig	mmf;
					mmf.pfnNormal = lpEntry->pfn;
					switch( lpEntry->nSig )
					{
					case uiSig_vv:
						( this->*mmf.pfn_vv )();
						break;
					case uiSig_vw:
						( this->*mmf.pfn_vw )( UINT( wParam ) );
						break;
					case uiSig_b_wl:
						( this->*mmf.pfn_b_wl )( wParam, lParam );
						break;
					case uiSig_vwp:
						{
							CPoint point( (uint32) lParam );
							( this->*mmf.pfn_vwp )( ( UINT )wParam, point );
						}
						break;
					case uiSig_v_v_ii:
						( this->*mmf.pfn_v_v_ii)( LOWORD(lParam), HIWORD(lParam) );
						break;
					}
					return 0;
				}
				++lpEntry;
			}
		}

		return XUI_DefWindowProc( GuiSystem::Instance().GetHWND(), nMsg, wParam, lParam );
	}

	// 消息处理，在垂直层次上遍历消息映射表
	LRESULT XUI_Base::OnWndMsg( UINT nMsg, WPARAM wParam, LPARAM lParam )
	{
		HRESULT lResult = 0;
		//for( size_t i = 0; i < m_pListeners.size(); ++i )
		//{
		//	XUI_Base* pTarget = m_pListeners[i];
		//	if( pTarget == m_pParent && !m_bTranslateParent ) continue;
		//	if( pTarget->DefMsgProc( nMsg, wParam, lParam ) )
		//	{
		//		return 0;
		//	}
		//}
		if( nMsg == WM_COMMAND )
		{
			return OnCommand( wParam, lParam );
		}
		else if( nMsg == WM_NOTIFY )
		{
			OnNotify( wParam, lParam, &lResult );
			return lResult;
		}
		return DefMsgProc( nMsg, wParam, lParam );
	}

	// 处理控件消息
	LRESULT XUI_Base::OnCommand( WPARAM wParam, LPARAM lParam )
	{
		UINT nID = LOWORD(wParam);
		XUI_Wnd* pCtrl = (XUI_Wnd*)lParam;
		int nCode = HIWORD(wParam);
		// UPDATE_COMMAND_UI
		// 菜单等单一事件对象的更新消息。
		if( pCtrl )
		{
			OnCmdMsg( nID, UPDATE_COMMAND_UI, NULL, NULL );
			nCode = NC_COMMAND;
		}
		else
		{
			// 反射消息
			// ReflectMsg();
		}

		return OnCmdMsg( nID, nCode, NULL, NULL );
	}

	// 处理自定义通知消息
	VOID XUI_Base::OnNotify( WPARAM, LPARAM lParam, LRESULT* lResult )
	{
		NMUIHDR* pNMHDR = ( NMUIHDR* )lParam;
		if( pNMHDR && pNMHDR->pCtrl )
		{
			int nID = pNMHDR->pCtrl->GetID();
			int nCode = pNMHDR->code;
			//// 调用子控件的消息处理
			//for( UINT i=0; i < m_pChildren.size(); i++ )
			//{
			//	m_pChildren[i]->OnCmdMsg( nID, MAKELONG( nCode, WM_NOTIFY ), (void*)pNMHDR, NULL );
			//}
			*lResult = OnCmdMsg( nID, MAKELONG( nCode, WM_NOTIFY ), (void*)pNMHDR, NULL );
		}
	}

}
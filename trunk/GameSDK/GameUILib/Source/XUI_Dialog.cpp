#include "XUI_Header.h"
#include "XUI_System.h"
#include "XUI_Dialog.h"

namespace XGC
{
	namespace ui
	{
		XUI_Dialog::XUI_Dialog( _lpctstr lpszTemplate /*= NULL*/ )
		{
			m_bTranslateParent = false;
			m_bEnableParent = false;
			m_bModal = false;
			m_nResult = 0;
			if( lpszTemplate )	m_strTemplate = lpszTemplate;
			//BindProperty( _T("Template"), m_strTemplate );
		}

		XUI_Dialog::~XUI_Dialog()
		{
			OnDestroy();
		}

		bool XUI_Dialog::Create( _lpctstr strTemplate, XUI_Window* pParent /* = NULL  */)
		{
			m_strTemplate = strTemplate;
			if( pParent )
			{
				SetParent( pParent );
				pParent->AddChild( this );
			}

			TiXmlDocument Doc;
			const char* p = XT2A( m_strTemplate );
			if( Doc.LoadFile( p ) )
			{
				TiXmlElement* pElement = Doc.FirstChildElement( "DESKTOP" );
				CreateFromXMLNode( pElement );
				iRect rc;

				rc.left		= pElement->IntAttribute( "X" );
				rc.top		= pElement->IntAttribute( "Y" );
				rc.right	= pElement->IntAttribute( "Width" ) + rc.left;
				rc.bottom	= pElement->IntAttribute( "Height" ) + rc.top;

				MoveWindow( rc.left, rc.top, rc.right, rc.bottom );

				ShowWindow( true );
				return OnInitDialog();
			}
			else
			{
				ASSERT( 0 );
			}
			return false;
		}

		XUI_Wnd* XUI_Dialog::PreModal()
		{
			XUI_Wnd* pParent = m_pParent;
			if( pParent == NULL )
			{
				//_assert( _afxCurrentGuiSystem );
				pParent = XUI::Instance().GetRoot();
			}

			while( pParent->GetParent() )
			{
				pParent = pParent->GetParent();
			}
			pParent->EnableWindow( false );
			m_nResult = 0;
			m_bEnableParent = true;
			m_bModal = true;
			XUI::Instance().EnterModaless( this );
			return pParent;
		}

		UINT XUI_Dialog::DoModal( fnModalMsgProc pfn )
		{
			XUI_Wnd* pParent = PreModal();
			//ASSERT( pParent );
			if( !Create( m_strTemplate.c_str(), (XUI_Window*)pParent ) )
			{
				ASSERT( FALSE );
				return -1;
			}

			BeginModalLoop( pfn );
			Release();
			PostModal();
			if( m_bEnableParent )
			{
				pParent->EnableWindow( true );
				m_bEnableParent = false;
			}

			return m_nResult;
		}

		void XUI_Dialog::BeginModalLoop( fnModalMsgProc pfn )
		{
			for(;;)
			{
				MSG msg;
				while( !::PeekMessage( &msg, NULL, NULL, NULL, PM_NOREMOVE ) )
				{
					pfn( TRUE );
					if( !m_bModal ) return;
				}

				while( ::PeekMessage( &msg, NULL, NULL, NULL, PM_NOREMOVE ) )
				{
					if( !pfn( FALSE ) )
					{
						::PostMessage( XUI::Instance().GetHWND(), WM_QUIT, 0, 0 );
						return;
					}

					if( !m_bModal ) return;
				}
			}
		}

		void XUI_Dialog::PostModal()
		{
			XUI::Instance().LeaveModaless();
		}

		void XUI_Dialog::EndModalLoop()
		{
			m_bModal = false;
		}

		void XUI_Dialog::OnOK()
		{
			//UpdateData( TRUE );
			m_nResult = IDOK;
			EndModalLoop();
		}

		void XUI_Dialog::OnCancel()
		{
			m_nResult = IDCANCEL;
			EndModalLoop();
		}

		void XUI_Dialog::CenterWindow()
		{
			if( m_pParent )
			{
				iRect rc = m_pParent->GetWindowRect();
				int nOffsetX = rc.Width()/2 - GetWindowRect().Width()/2;
				int nOffsetY = rc.Height()/2 - GetWindowRect().Height()/2;
				Offset( nOffsetX, nOffsetY );
			}
		}

		void XUI_Dialog::RenderSelf( const iPoint& adjust )
		{
			XUI_Window::RenderSelf( adjust );
		}

		bool XUI_Dialog::onKeyUp(_uint32 keycode, UINT sysKeys)
		{
			// 通过Tab键在控件中切换
			if( m_bFocused && keycode == VK_TAB )
			{
				//for( size_t i= 0; i < m_pChildren.size(); ++i )
				//{
				//	if( m_pChildFocusedOn == m_pChildren[i] )
				//	{
				//		m_pChildFocusedOn->SetFocus( false );
				//		if( i >= m_pChildren.size() - 1 )
				//		{
				//			m_pChildFocusedOn = m_pChildren[0];
				//		}
				//		else
				//		{
				//			m_pChildFocusedOn = m_pChildren[i+1];
				//		}
				//		m_pChildFocusedOn->SetFocus( true );
				//		break;
				//	}
				//}
			}
			return true;
		}

		void XUI_Dialog::SetFocus( UINT nCtrlID )
		{
			XUI_Wnd* pFocus = FindChildByID( nCtrlID );
			pFocus->SetFocus( true );
		}
	}
}
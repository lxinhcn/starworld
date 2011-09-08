#include "XUI_Header.h"
#include "XUI_System.h"
#include "XUI_Dialog.h"

namespace XGC
{
	namespace ui
	{
		XUI_Dialog::XUI_Dialog( _lpctstr lpszTemplate /*= NULL*/, XUI_Window *pParent /*= NULL*/ )
		{
			m_bTranslateParent = false;
			m_bEnableParent = false;
			m_bModal = false;
			m_nResult = 0;
			if( lpszTemplate )	
				m_strTemplate = lpszTemplate;

			m_pParent = pParent;
		}

		XUI_Dialog::~XUI_Dialog()
		{
			OnDestroy();
		}

		bool XUI_Dialog::Create()
		{
			if( m_pParent )
			{
				SetParent( m_pParent );
				m_pParent->AddChild( this );
			}

			TiXmlDocument Doc;
			const char* p = XT2A( m_strTemplate.c_str() );
			if( Doc.LoadFile( p ) )
			{
				TiXmlElement* pElement = Doc.FirstChildElement( "dialog" );
				CreateFromXMLNode( pElement );
				iRect rc;

				rc.left		= pElement->IntAttribute( "x" );
				rc.top		= pElement->IntAttribute( "y" );
				rc.right	= pElement->IntAttribute( "width" ) + rc.left;
				rc.bottom	= pElement->IntAttribute( "height" ) + rc.top;

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

			while( pParent )
			{
				pParent->EnableWindow( false );
				pParent = pParent->GetParent();
			}
			
			m_nResult = 0;
			m_bEnableParent = true;
			m_bModal = true;
			XUI::Instance().EnterModaless( this );
			return m_pParent;
		}

		int XUI_Dialog::DoModal()
		{
			if( !Create() )
			{
				ASSERT( FALSE );
				return -1;
			}

			BeginModalLoop();
			PostModal();
			if( m_bEnableParent )
			{
				m_pParent->EnableWindow( true );
				m_bEnableParent = false;
			}

			return m_nResult;
		}

		void XUI_Dialog::BeginModalLoop()
		{
			for(;;)
			{
				MSG msg;
				while( !::PeekMessage( &msg, NULL, NULL, NULL, PM_REMOVE ) )
				{
					if( !DefMessageProc( msg ) )
					{
						XUI::Instance().HandleMessage( msg.hwnd, msg.message, msg.wParam, msg.lParam );
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

		void XUI_Dialog::SetFocus( UINT nCtrlID )
		{
			XUI_Wnd* pFocus = FindChildByID( nCtrlID );
			pFocus->SetFocus( true );
		}
	}
}
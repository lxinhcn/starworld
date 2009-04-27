#include "GuiHeader.h"
#include "LuaBindClass.h"
#include "UIFactory.h"
#include "XUI_Window.h"
//#include "ScrollBar.h"

namespace UILib
{
	BEGIN_UIMSG_MAP( XUI_Window, XUI_Wnd )
		//UI_REFLECT( NC_SCROLL_CREATE, OnScrollCreate )
		//UI_NOTIFY( EV_NC_POSCHANGED, OnScrollPosChange )
	END_UIMSG_MAP()

	XUI_Window::XUI_Window(void)
		: m_nOffsetX( 0 )
		, m_nOffsetY( 0 )
		, m_nPanelWidth( 0 )
		, m_nPanelHeight( 0 )
	{
	}

	XUI_Window::~XUI_Window()
	{
	}

	x_point XUI_Window::AdjustPoint(const x_point& pt, bool bClientToScreen) const
	{
		x_point ptAdjust( m_nOffsetX, m_nOffsetY );
		if (bClientToScreen)
			return pt-ptAdjust;
		else
			return pt+ptAdjust;
	}

	int XUI_Window::ScrollHorizontal(int nOffset)
	{
		m_nOffsetX += nOffset;
		if( m_nOffsetX < 0 ) m_nOffsetX = 0;
		if( m_nOffsetX + GetWindowRect().Width() > m_nPanelWidth )	m_nOffsetX = m_nPanelWidth - GetWindowRect().Width();
		return m_nOffsetX;
	}

	int XUI_Window::ScrollVerticle(int nOffset)
	{
		m_nOffsetY += nOffset;
		if( m_nOffsetY < 0 ) m_nOffsetY = 0;
		if( m_nOffsetY + GetWindowRect().Height() > m_nPanelWidth )	m_nOffsetY = m_nPanelWidth - GetWindowRect().Height();
		return m_nOffsetY;
	}

	BOOL XUI_Window::OnScrollCreate( WPARAM wParam, LPARAM lParam )
	{
		//CScrollBar* pScroll = reinterpret_cast<CScrollBar*>( lParam );
		//if( !pScroll )	return FALSE;

		//if( pScroll->GetID() == m_nVScrollID )
		//{
		//	m_pVScrollBar = pScroll;
		//	pScroll->AddCommonListener( this );
		//}
		//if( pScroll->GetID() == m_nHScrollID )
		//{
		//	m_pHScrollBar = pScroll;
		//	pScroll->AddCommonListener( this );
		//}
		return TRUE;
	}

	BOOL XUI_Window::OnScrollPosChange( WPARAM wParam, LPARAM lParam )
	{
		int nScrollID	= LOWORD( lParam );
		int nPosition	= HIWORD( lParam );
		if( m_nHScrollID == nScrollID )
		{
			m_nOffsetX = nPosition;
		}
		else if( m_nVScrollID == nScrollID )
		{
			m_nOffsetY = nPosition;
		}
		return TRUE;
	}

	// ForAllChild的回调，用来查看当前的ScrollBar有没有Panel用到的
	bool BindScroll( XUI_Wnd* pElement, void* pPanel )
	{
		//CScrollBar* pScroll = dynamic_cast< CScrollBar* >( pElement );
		//if( pScroll )
		//{
		//	XUI_Window* pPanelEx = reinterpret_cast<XUI_Window*>(pPanel);
		//	if( pPanelEx->m_nHScrollID == pScroll->GetID() )
		//	{
		//		pPanelEx->m_pHScrollBar = pScroll;
		//		pScroll->AddCommonListener( pPanelEx );
		//	}

		//	if( pPanelEx->m_nVScrollID == pScroll->GetID() )
		//	{
		//		pPanelEx->m_pVScrollBar = pScroll;
		//		pScroll->AddCommonListener( pPanelEx );
		//	}
		//}
		return false;
	}

	void XUI_Window::BindScrollBar()
	{
		//XUI_Wnd* pParent = GetParent();
		//if( pParent )
		//{
		//	pParent->ForAllChild( BindScroll, this );
		//}
	}

	// 创建子对象，仅有Panel类才能通过节点创建子对象。
	bool XUI_Window::CreateFromXMLNode( TiXmlNode* pNode )
	{
		// 清除子控件
		DestroyAllChild();
		m_pChildMouseOver	= NULL;
		m_pChildFocusedOn	= NULL;

		TiXmlNode* Attribute = pNode->FirstChild( "Attribute" );
		if( Attribute == NULL ) return false;
		load_file( Attribute->ToElement() );

		TiXmlElement* pNodeChild = pNode->FirstChildElement( "CONTROL" );
		while( pNodeChild )
		{
			TiXmlElement* pXMLChildElement = pNodeChild;
			if( pXMLChildElement )
			{
				if( strcmp( "CONTROL", pXMLChildElement->Value() ) == 0 )
				{
					USES_CONVERSION;
					_lpctstr p = A2T( pXMLChildElement->Attribute( "Lable" ) );
					if( p )
					{
						XUI_Wnd* pElement = CUIFactory::GetInstance().Creator( p );
						ASSERT_MSG( pElement, _T("无法创建控件") );
						if( pElement )
						{
							AddChild( pElement );
							Attribute = pXMLChildElement->FirstChild( "Attribute" );
							pElement->load_file( Attribute->ToElement() );

							pElement->SendUIMessage( UI_CREATE, 0, 0 );

							if( pElement->IsKindOf( TypeWindow ) )
							{
								if( ((XUI_Window*)pElement)->CreateFromXMLNode( pNodeChild ) == false ) return false;
							}
						}
					}
					//const x_rect& rc = pElement->GetRect();
					//if( rc.Width() > m_nPanelWidth )	m_nPanelWidth = rc.Width();
					//if( rc.Height() > m_nPanelHeight )	m_nPanelHeight = rc.Height();
				}
			}
			pNodeChild = pNodeChild->NextSiblingElement( "CONTROL" );
		}
		//BindScrollBar();
		return true;
	}

	bool XUI_Window::SaveToXMLNode( TiXmlNode* pNode )
	{
		TiXmlElement* pElement = pNode->ToElement();
		//if( SavePropertys( pElement ) == false ) return false;;
		TiXmlElement Attribute( "Attribute" );
		if( save_file( &Attribute ) == false ) return false;
		pNode->InsertEndChild( Attribute );


		std::vector<XUI_Wnd*>::const_iterator citer = m_pChildren.begin();
		while( citer != m_pChildren.end() )
		{
			XUI_Wnd* pXUI_Wnd = *citer;
			TiXmlElement XmlElement("CONTROL");
			USES_CONVERSION;
			XmlElement.SetAttribute( "Lable", T2A(pXUI_Wnd->GetLable()) );
			if( pXUI_Wnd->IsKindOf( TypeWindow ) )
				((XUI_Window*)pXUI_Wnd)->SaveToXMLNode( (TiXmlNode*)&XmlElement );
			else
			{
				//if( pXUI_Wnd->SavePropertys( &XmlElement ) == false ) return false;
				TiXmlElement Attribute( "Attribute" );
				if( pXUI_Wnd->save_file( &Attribute ) == false ) return false;
				XmlElement.InsertEndChild( Attribute );
			}

			pNode->InsertEndChild( XmlElement );
			++citer;
		}
		return true;
	}
}

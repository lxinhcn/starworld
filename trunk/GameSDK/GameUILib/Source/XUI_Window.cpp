#include "XUI_Header.h"
#include "XUI_LuaBinder.h"
#include "XUI_Factory.h"
#include "XUI_Window.h"

namespace XGC
{
	namespace ui
	{
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

		void XUI_Window::AdjustPoint( iPoint& pt, bool bClientToScreen )const
		{
			iPoint ptAdjust( m_nOffsetX, m_nOffsetY );
			if( bClientToScreen )
				pt -= ptAdjust;
			else
				pt += ptAdjust;
		}

		void XUI_Window::AdjustPoint( iRect& rc, bool bClientToScreen )const
		{
			iPoint ptAdjust( m_nOffsetX, m_nOffsetY );
			if( bClientToScreen )
				rc -= ptAdjust;
			else
				rc += ptAdjust;
		}

		//int XUI_Window::ScrollHorizontal(int nOffset)
		//{
		//	m_nOffsetX += nOffset;
		//	if( m_nOffsetX < 0 ) m_nOffsetX = 0;
		//	if( m_nOffsetX + GetWindowRect().Width() > m_nPanelWidth )	m_nOffsetX = m_nPanelWidth - GetWindowRect().Width();
		//	return m_nOffsetX;
		//}

		//int XUI_Window::ScrollVerticle(int nOffset)
		//{
		//	m_nOffsetY += nOffset;
		//	if( m_nOffsetY < 0 ) m_nOffsetY = 0;
		//	if( m_nOffsetY + GetWindowRect().Height() > m_nPanelWidth )	m_nOffsetY = m_nPanelWidth - GetWindowRect().Height();
		//	return m_nOffsetY;
		//}

		// 创建子对象，仅有Panel类才能通过节点创建子对象。
		bool XUI_Window::CreateFromXMLNode( TiXmlNode* pNode )
		{
			// 清除子控件
			DestroyAllChild();
			//m_pChildMouseOver	= NULL;
			//m_pChildFocusedOn	= NULL;

			TiXmlNode* Attribute = pNode->FirstChild( "attribute" );
			if( Attribute == NULL ) return false;
			load_file( Attribute->ToElement() );

			TiXmlElement* pNodeChild = pNode->FirstChildElement( "control" );
			while( pNodeChild )
			{
				TiXmlElement* pXMLChildElement = pNodeChild;
				if( pXMLChildElement )
				{
					if( strcmp( "control", pXMLChildElement->Value() ) == 0 )
					{
						_lpcstr lpszLable = pXMLChildElement->Attribute( "lable" );
						XUI_Wnd* pElement = XUI_Factory::GetInstance().Creator( lpszLable );
						ASSERT_MSG( pElement, _T("无法创建控件") );
						if( pElement )
						{
							AddChild( pElement );
							Attribute = pXMLChildElement->FirstChild( "attribute" );
							pElement->load_file( Attribute->ToElement() );

							if( typeid( pElement ) == typeid(XUI_Window) )
							{
								if( ((XUI_Window*)pElement)->CreateFromXMLNode( pNodeChild ) == false ) return false;
							}
						}
					}
				}
				pNodeChild = pNodeChild->NextSiblingElement( "control" );
			}
			return true;
		}

		bool XUI_Window::SaveToXMLNode( TiXmlNode* pNode )
		{
			TiXmlElement* pElement = pNode->ToElement();

			TiXmlElement Attribute( "attribute" );
			if( save_file( &Attribute ) == false ) return false;
			pNode->InsertEndChild( Attribute );


			std::vector<XUI_Wnd*>::const_iterator citer = m_pChildren.begin();
			while( citer != m_pChildren.end() )
			{
				XUI_Wnd* pXUI_Wnd = *citer;
				TiXmlElement XmlElement("control");
				XmlElement.SetAttribute( "lable", typeid( pXUI_Wnd ).name() );
				if( typeid( pXUI_Wnd ) == typeid(XUI_Window) )
					((XUI_Window*)pXUI_Wnd)->SaveToXMLNode( (TiXmlNode*)&XmlElement );
				else
				{
					//if( pXUI_Wnd->SavePropertys( &XmlElement ) == false ) return false;
					TiXmlElement Attribute( "attribute" );
					if( pXUI_Wnd->save_file( &Attribute ) == false ) return false;
					XmlElement.InsertEndChild( Attribute );
				}

				pNode->InsertEndChild( XmlElement );
				++citer;
			}
			return true;
		}
	}
}

#include "header.h"
#include "static.h"
#include "Canvas.h"

namespace UILib
{
	CStatic::CStatic(void)
	{
		m_dwColor	= 0;
		m_dwFormat	= MAKELONG( 0, VTA_CENTER );
	}

	CStatic::~CStatic(void)
	{
	}

	//--------------------------------------------------------------------------
	void CStatic::RenderSelf(const CUIRect& clipper )
	//--------------------------------------------------------------------------
	{
		__super::RenderSelf( clipper );
		CUIPoint pt( 0, 0 );
		pt = ClientToScreen(pt);
		int nWidth	= m_wndRect.Width();
		int nHeight	= m_wndRect.Height();

		// 文字
		if( LOWORD(m_dwFormat) & TA_RIGHT )
			pt.x += nWidth;
		else if( LOWORD(m_dwFormat) &TA_CENTER )
			pt.x += ( nWidth >> 1 );

		pt.y += nHeight/2;
		m_pCanvas->DrawText( m_strText.c_str(), m_pFont, pt.x, pt.y, m_dwColor, clipper );
	}

	//--------------------------------------------------------------------------
	void CStatic::LoadProperty(TiXmlElement* pXMLElement )
	//--------------------------------------------------------------------------
	{
		__super::LoadProperty( pXMLElement );
		m_strText = pXMLElement->Attribute( "Text" );
		pXMLElement->QueryIntAttribute( "TextColor", (int*)&m_dwColor );
		pXMLElement->QueryIntAttribute( "Format", (int*)&m_dwFormat );
		m_dwFormat |= ( VTA_CENTER << 16 );
	}

	//--------------------------------------------------------------------------
	void CStatic::SaveProperty(TiXmlElement* pXMLElement )
	//--------------------------------------------------------------------------
	{
		__super::SaveProperty( pXMLElement );
		pXMLElement->SetAttribute( "Text", m_strText.c_str() );
		pXMLElement->SetAttribute( "TextColor", m_dwColor );
		pXMLElement->SetAttribute( "Format", m_dwFormat );
	}

	// 设置控件属性
	//--------------------------------------------------------------------------
	bool CStatic::SetProperty( const char* pszPropertyName, const char* pszValue )
	//--------------------------------------------------------------------------
	{
		if( stricmp( pszPropertyName, "Text" ) == 0 )
		{
			m_strText = pszValue;
		}
		else if( stricmp( pszPropertyName, "TextColor" ) == 0 )
		{
			int r,g,b;
			sscanf( pszValue, "%d; %d; %d", &r, &g, &b );
			m_dwColor = RGB( r, g, b );
		}
		else if( stricmp( pszPropertyName, "Align" ) == 0 )
		{
			if( stricmp( pszValue, "Left" ) == 0 )
			{
				m_dwFormat &= ~(TA_CENTER|TA_RIGHT);
			}
			else if( stricmp( pszValue, "Center" ) == 0 )
			{
				m_dwFormat |= TA_CENTER;
				m_dwFormat &= ~TA_RIGHT;
			}
			else if( stricmp( pszValue, "Right" ) == 0 )
			{
				m_dwFormat |= TA_RIGHT;
				m_dwFormat &= ~TA_CENTER;
			}
		}
		else
		{
			return __super::SetProperty( pszPropertyName, pszValue );
		}
		return true;
	}

	//--------------------------------------------------------------------------
	bool CStatic::GetProperty( const char* pszPropertyName, char* pValueBuffer, int size )
	//--------------------------------------------------------------------------
	{
		if( stricmp( pszPropertyName, "Text" ) == 0 )
		{
			strncpy( pValueBuffer, m_strText.c_str(), size );
		}
		else if( stricmp( pszPropertyName, "TextColor" ) == 0 )
		{
			_snprintf( pValueBuffer, size, "%d; %d; %d",
				m_dwColor & 0xff0000ff,
				( m_dwColor & 0xff00ff00 ) >> 8,
				( m_dwColor & 0xffff0000 ) >> 16 );
		}
		else if( stricmp( pszPropertyName, "Align" ) == 0 )
		{
			if( m_dwFormat &= TA_CENTER )
			{
				strncpy( pValueBuffer, "Center", size );
			}
			else if( m_dwFormat &= TA_RIGHT )
			{
				strncpy( pValueBuffer, "Right", size );
			}
			else
			{
				strncpy( pValueBuffer, "Left", size );
			}
		}
		else
		{
			return __super::GetProperty( pszPropertyName, pValueBuffer, size );
		}
		return true;
	}
}

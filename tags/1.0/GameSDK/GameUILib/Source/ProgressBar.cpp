#include "header.h"
#include "Texture.h"
#include "Canvas.h"
#include "ProgressBar.h"

namespace UILib
{
	CProgressBar::CProgressBar()
	: m_pTexture( new CTexture )
	, m_nMin( 0 )
	, m_nMax( 100 )
	, m_nValue( 50 )
	, m_nType( 1 )
	, m_fTextureLen( 0.0f )
	{
		SetPos( m_nValue );
	}

	CProgressBar::~CProgressBar()
	{
		UISAFE_DELETE( m_pTexture );
	}

	void CProgressBar::RenderSelf( CONST CUIRect& clipper)
	{
		if( m_pCanvas )
		{

			CUIPoint pt( 0, 0 );
			pt = ClientToScreen(pt);
			int iWidth	= m_wndRect.Width();
			int iHeight	= m_wndRect.Height();

			int nImageWidth = 0;
			int nImageHeight = 0;

 			m_pCanvas->DrawTexture( m_pBkTex, pt.x, pt.y, iWidth, iHeight, clipper );
			switch( m_nType )
			{
			case 0:
				m_pCanvas->DrawTexture( m_pTexture, pt.x, pt.y, ( int )m_fTextureLen, iHeight, clipper );
				break;
			case 1:
				m_pCanvas->DrawTexture( m_pTexture, pt.x, pt.y + ( int )m_fTextureLen, iWidth, iHeight - ( int )m_fTextureLen, clipper );
				break;
			}
		}
	}

	void CProgressBar::SetMax(int nValue)
	{
		m_nMax=nValue;
		m_nMax=max(m_nMax, m_nMin);
		m_nValue=min(m_nValue, m_nMax);
	}

	void CProgressBar::SetMin(int nValue)
	{
		m_nMin=nValue;
		m_nMin=min(m_nMax, m_nMin);
		m_nValue=max(m_nValue, m_nMin);
	}

	void CProgressBar::SetPos( int nValue )
	{
		if( nValue != m_nValue && m_pParent )
		{
			NMHDR_PROCESS hdrProcess;
			hdrProcess.hdr.code		= EV_POSCHANGE;
			hdrProcess.hdr.idFrom	= GetID();
			hdrProcess.hdr.pCtrl	= this;
			hdrProcess.pos			= nValue;
			m_pParent->SendMessage( WM_NOTIFY, 0, (LPARAM)&hdrProcess );
		}

		m_nValue = nValue;
		switch( m_nType )
		{
		case 0:
			{
				float U1 = m_nValue * 1.0f/m_nMax;
				m_pTexture->SetUV( 0.0f, 0.0f, U1, 1.0f );
				m_fTextureLen = GetRect().Width() * U1;
			}
			break;
		case 1:
			{
				float V0 = 1.0f - m_nValue * 1.0f/m_nMax;
				m_pTexture->SetUV( 0.0f, V0, 1.0f, 1.0f );
				m_fTextureLen = GetRect().Height() * V0;
			}
			break;
		case 2:
			break;
		}
	}

	// 设置控件属性
	bool CProgressBar::SetProperty( const char* pszPropertyName, const char* pszValue )
	{
		if( stricmp( pszPropertyName, "Min" ) == 0 )
		{
			m_nMin = atoi( pszValue );
		}
		else if( stricmp( pszPropertyName, "Max" ) == 0 )
		{
			m_nMax = atoi( pszValue );
		}
		else if( stricmp( pszPropertyName, "Value" ) == 0 )
		{
			SetPos( atoi( pszValue ) );
		}
		else if( stricmp( pszPropertyName, "Texture" ) == 0 )
		{
			m_pTexture->SetName( pszValue );
		}
		else if( stricmp( pszPropertyName, "Type" ) == 0 )
		{
			m_nType = atoi( pszValue );
		}
		else
		{
			return __super::SetProperty( pszPropertyName, pszValue );
		}
		return true;
	}

	bool CProgressBar::GetProperty( const char* pszPropertyName, char* pValueBuffer, int size )
	{
		if( stricmp( pszPropertyName, "Min" ) == 0 )
		{
			itoa( m_nMin, pValueBuffer, 10 );
		}
		else if( stricmp( pszPropertyName, "Max" ) == 0 )
		{
			itoa( m_nMax, pValueBuffer, 10 );
		}
		else if( stricmp( pszPropertyName, "Value" ) == 0 )
		{
			itoa( m_nValue, pValueBuffer, 10 );
			SetPos( m_nValue );
		}
		else if( stricmp( pszPropertyName, "Texture" ) == 0 )
		{
			strncpy( pValueBuffer, m_pTexture->GetName(), size );
		}
		else if( stricmp( pszPropertyName, "Type" ) == 0 )
		{
			itoa( m_nType, pValueBuffer, 10 );
		}
		else
		{
			return __super::GetProperty( pszPropertyName, pValueBuffer, size );
		}
		return true;
	}

	void CProgressBar::LoadProperty( TiXmlElement* pXMLElement )
	{
		__super::LoadProperty( pXMLElement );
		m_nType = pXMLElement->IntAttribute( "Type" );
		SetMin( pXMLElement->IntAttribute( "Min" ) );
		SetMax( pXMLElement->IntAttribute( "Max" ) );
		SetPos( pXMLElement->IntAttribute( "Value" ) );
		m_pTexture->SetName( pXMLElement->Attribute( "Texture" ) );
	}

	//存储属性
	void CProgressBar::SaveProperty( TiXmlElement* pXMLElement )
	{
		__super::SaveProperty( pXMLElement );
		pXMLElement->SetAttribute( "Type", m_nType );
		pXMLElement->SetAttribute( "Min", m_nMin );
		pXMLElement->SetAttribute( "Max", m_nMax );
		pXMLElement->SetAttribute( "Value", m_nValue );
		pXMLElement->SetAttribute( "Texture", m_pTexture->GetName() );
	}
}

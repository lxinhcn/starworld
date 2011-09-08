#include "header.h"
#include "RenderTarget.h"
#include "Texture.h"
#include "Canvas.h"

namespace UILib
{
	BEGIN_UIMSG_MAP( CRenderTarget, CUIElement )
		ON_UI_CREATE()
	END_UIMSG_MAP()
	CRenderTarget::CRenderTarget(void)
	{
	}

	CRenderTarget::~CRenderTarget(void)
	{
	}

	void CRenderTarget::SetModelName( LPCSTR lpszModelName )
	{
		m_strModelName = lpszModelName;
	}

	LPCSTR CRenderTarget::GetModelName()
	{	
		return m_strModelName.c_str();
	}

	void CRenderTarget::RenderSelf(const CUIRect& clipper)
	{
		__super::RenderSelf( clipper );
		if( m_pCanvas )
		{
			//±≥æ∞
			CUIPoint pt( 0, 0 );
			pt = ClientToScreen(pt);
			int nWidth	= m_wndRect.Width();
			int nHeight	= m_wndRect.Height();

			// ∞¥≈•±≥æ∞
			CTexture tex;
			tex.SetName( m_strModelName.c_str() );
			m_pCanvas->DrawTexture( &tex, pt.x, pt.y, nWidth, nHeight, clipper );
		}
	}

	BOOL CRenderTarget::OnCreate( WPARAM wParam, LPARAM lParam )
	{
		if( m_pParent )
		{
			NMUIHDR hdr;
			hdr.code = NC_CREATE;
			hdr.pCtrl = this;
			hdr.idFrom = GetID();
			m_pParent->SendMessage( WM_NOTIFY, 0, (LPARAM)&hdr );
		}
		return TRUE;
	}

	void CRenderTarget::LoadProperty(TiXmlElement* pXMLElement )
	{
		__super::LoadProperty( pXMLElement );
		m_strModelName = pXMLElement->Attribute( "ModelName" );
	}

	void CRenderTarget::SaveProperty(TiXmlElement* pXMLElement )
	{
		__super::SaveProperty( pXMLElement );
		pXMLElement->SetAttribute( "ModelName", m_strModelName.c_str() );
	}

	// …Ë÷√øÿº˛ Ù–‘
	bool CRenderTarget::SetProperty( const char* pszPropertyName, const char* pszValue )
	{
		if( stricmp( pszPropertyName, "ModelName" ) == 0 )
		{
			SetModelName( pszValue );
		}
		else
		{
			return __super::SetProperty( pszPropertyName, pszValue );
		}
		return true;
	}

	bool CRenderTarget::GetProperty( const char* pszPropertyName, char* pValueBuffer, int size )
	{
		if( stricmp( pszPropertyName, "ModelName" ) == 0 )
		{
			strncpy( pValueBuffer, GetModelName(), size );
		}
		else
		{
			return __super::GetProperty( pszPropertyName, pValueBuffer, size );
		}
		return true;
	}
}

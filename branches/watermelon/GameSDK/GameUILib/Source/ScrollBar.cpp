#include "header.h"
#include "Canvas.h"
#include "ScrollBar.h"
#include "GuiSystem.h"

namespace UILib
{
	BEGIN_UIMSG_MAP( CScrollBar, CUIElement )
		ON_UI_SIZE()
	END_UIMSG_MAP()
	CScrollBar::CScrollBar(void)
	: m_nMin( 0 )
	, m_nMax( 0 )
	, m_nPosition( 0 )
	, m_nPageSize( 0 )
	, m_bShowThumb( true )
	{
	}

	CScrollBar::~CScrollBar(void)
	{
	}

	void	CScrollBar::SetRange( int nMin, int nMax )
	{
		m_nMax = nMax + m_nPageSize;
		m_nMin = nMin;
		ReCalc();
		UpdateThumb();
	}

	void	CScrollBar::SetPageSize( int nPageSize )
	{
		m_nMax = m_nMax - m_nPageSize + nPageSize;
		m_nPageSize = nPageSize;
		ReCalc();
		UpdateThumb();
	}

	void	CScrollBar::ReCalc()
	{
		if( m_nPosition > m_nMax - m_nPageSize )	m_nPosition = m_nMax - m_nPageSize;
		if( m_nPosition < m_nMin || m_nPageSize > m_nMax - m_nMin )	m_nPosition = m_nMin;
	}

	void	CScrollBar::SetPosition( int nPosition )
	{
		int nOldPosition = m_nPosition;
		m_nPosition = nPosition;
		ReCalc();
		if( nOldPosition != m_nPosition && m_pParent )
		{
			NMHDR_SCROLL hdrScroll;
			hdrScroll.hdr.code		= EV_SCROLLPOSCHANGED;
			hdrScroll.hdr.idFrom	= GetID();
			hdrScroll.hdr.pCtrl		= this;
			hdrScroll.pos			= m_nPosition;
			hdrScroll.oldpos		= nOldPosition;
			SendMessage( WM_NOTIFY, 0, (LPARAM)&hdrScroll );
		}
		UpdateThumb();

	}
	
	void	CScrollBar::Scroll( int nDelta )
	{
		SetPosition( m_nPosition + nDelta );
	}

	void	CScrollBar::UpdateThumb()
	{
		if( m_enDirection == Vertical )
		{
			m_rcThumb.left = m_rcUpButton.left;
			m_rcThumb.right = m_rcUpButton.right;

			if( m_nMax - m_nMin > m_nPageSize )
			{
				int nThumbHeight = max( m_rcTrack.Height() * m_nPageSize / ( m_nMax - m_nMin ), 8 );
				int nMaxPosition = m_nMax - m_nMin - m_nPageSize;
				m_rcThumb.top = m_rcTrack.top + ( m_nPosition - m_nMin ) * ( m_rcTrack.Height() - nThumbHeight )
					/ nMaxPosition;
				m_rcThumb.bottom = m_rcThumb.top + nThumbHeight;
				m_bShowThumb = true;
			}
			else
			{
				m_rcThumb.bottom = m_rcThumb.top;
				m_bShowThumb = false;
			}
		}
		else
		{
			m_rcThumb.top = m_rcUpButton.top;
			m_rcThumb.bottom = m_rcUpButton.bottom;

			if( m_nMax - m_nMin > m_nPageSize )
			{
				int nThumbWidth = max( m_rcTrack.Width() * m_nPageSize / ( m_nMax - m_nMin ), 8 );
				int nMaxPosition = m_nMax - m_nMin - m_nPageSize;
				m_rcThumb.left = m_rcTrack.left + ( m_nPosition - m_nMin ) * ( m_rcTrack.Width() - nThumbWidth )
					/ nMaxPosition;
				m_rcThumb.right = m_rcThumb.left + nThumbWidth;
				m_bShowThumb = true;
			}
			else
			{
				m_rcThumb.right = m_rcThumb.left;
				m_bShowThumb = false;
			}
		}
	}

	bool CScrollBar::onButtonDown(int button, const CUIPoint& pt, UINT sysKeys)
	{
		m_ptLast = pt;
		if( m_bEnable )
		{
			if( m_rcUpButton.PtInRect( pt ) )
			{
				m_enBtnState = StateUp;
			}
			else if( m_rcDownButton.PtInRect( pt ) )
			{
				m_enBtnState = StateDown;
			}
			else if( m_rcThumb.PtInRect( pt ) )
			{
				m_enBtnState = StateThumb;
				if( m_enDirection == Vertical )
				{
					m_nThumbOffset = pt.y - m_rcThumb.top;
				}
				else
				{
					m_nThumbOffset = pt.x - m_rcThumb.left;
				}
			}
		}
		return __super::onButtonDown( button, pt, sysKeys );
	}

	bool CScrollBar::onMouseMove(const CUIPoint& pt, UINT sysKeys)
	{
		if( m_enBtnState == StateThumb && m_bEnable )
		{
			m_ptLast = pt;
			int nPosition = 0;
			if( m_enDirection == Vertical )
			{
				int nThumbPos = pt.y - m_nThumbOffset - m_rcUpButton.Height();
				nPosition = ( m_nMax - m_nMin - m_nPageSize ) * nThumbPos / ( m_rcTrack.Height() - m_rcThumb.Height() ) + m_nMin;
			}
			else
			{
				int nThumbPos = pt.x - m_nThumbOffset - m_rcUpButton.Width();
				nPosition = ( m_nMax - m_nMin - m_nPageSize ) * nThumbPos / ( m_rcTrack.Width() - m_rcThumb.Width() ) + m_nMin;
			}
			SetPosition( nPosition );
		}
		return __super::onMouseMove( pt, sysKeys );
	}

	bool CScrollBar::onButtonUp(int button, const CUIPoint& pt, UINT sysKeys)
	{
		if( m_enBtnState == StateUp )	Scroll( -1 );
		else if( m_enBtnState == StateDown ) Scroll( 1 );

		m_enBtnState = StateNone;
		m_ptLast = pt;
		return __super::onButtonUp( button, pt, sysKeys );
	}

	void CScrollBar::RenderSelf(const CUIRect& clipper)
	{
		int nBtnUpState		= 0; // normal
		int nBtnDownState	= 0; // normal

		if( !m_bEnable )
		{
			nBtnUpState		= 3; // Disable;
			nBtnDownState	= 3; // Disable;
		}

		DWORD dwCurrentTime = GetTickCount();
		switch( m_enBtnState )
		{
		case StateUp:
			if( dwCurrentTime - m_dwBtnTime > SCROLL_BTN_DELAY )
				m_enBtnState = StateHoldUp;
			break;
		case StateHoldUp:
			if( dwCurrentTime - m_dwBtnTime > SCROLL_BTN_DELAY )
			{
				Scroll( -1 );
				m_dwBtnTime = dwCurrentTime;
			}
			break;
		case StateDown:
			if( dwCurrentTime - m_dwBtnTime > SCROLL_BTN_DELAY )
				m_enBtnState = StateHoldDown;
			break;
		case StateHoldDown:
			if( dwCurrentTime - m_dwBtnTime > SCROLL_BTN_DELAY )
			{
				Scroll( 1 );
				m_dwBtnTime = dwCurrentTime;
			}
			break;
		case StateThumb:
			break;
		}

		UpdateThumb();
		if( m_rcUpButton.PtInRect( m_ptLast ) )
		{
			nBtnUpState = 1; // UpHover
		}
		else if( m_rcDownButton.PtInRect( m_ptLast ) )
		{
			nBtnDownState = 1; // DownHover
		}
		m_UpTexture.SetUV( nBtnUpState * 0.25f, 0.0f, nBtnUpState * 0.25f + 0.25f, 1.0f );
		m_DownTexture.SetUV( nBtnDownState * 0.25f, 0.0f, nBtnDownState * 0.25f + 0.25f, 1.0f );

		CUIPoint pt( 0, 0 );
		pt = ClientToScreen(pt);
		m_pCanvas->DrawTexture( &m_UpTexture,
			pt.x + m_rcUpButton.left,
			pt.y + m_rcUpButton.top,
			m_rcUpButton.Width(), m_rcUpButton.Height(),
			clipper );

		m_pCanvas->DrawTexture( &m_DownTexture,
			pt.x + m_rcDownButton.left,
			pt.y + m_rcDownButton.top,
			m_rcDownButton.Width(), m_rcDownButton.Height(),
			clipper );

		m_pCanvas->DrawTexture( &m_ThumbTexture,
			pt.x + m_rcThumb.left,
			pt.y + m_rcThumb.top,
			m_rcThumb.Width(), m_rcThumb.Height(),
			clipper );
	}

	void CScrollBar::OnSize( int nWidth, int nHeight )
	{
		if( m_pCanvas )
		{
			if ( m_enDirection == Vertical )
			{
				int nUpBtnLength	= m_UpTexture.GetHeight();
				int nDownBtnLength	= m_DownTexture.GetHeight();

				m_rcUpButton.SetRect( 0, 0, nWidth, nUpBtnLength );
				m_rcDownButton.SetRect( 0, nHeight - nDownBtnLength, nWidth, nHeight);
				m_rcTrack.SetRect( 0, nUpBtnLength, nWidth, nHeight - nDownBtnLength );
			}
			else
			{
				int nUpBtnLength = m_UpTexture.GetHeight()/4;
				int nDownBtnLength = m_DownTexture.GetWidth()/4;

				m_rcUpButton.SetRect(  0, 0, nUpBtnLength, nHeight );
				m_rcDownButton.SetRect( nWidth - nDownBtnLength, 0, nWidth, nHeight);
				m_rcTrack.SetRect( nUpBtnLength, 0, nWidth - nDownBtnLength, nHeight );
			}
		}
	}

	bool CScrollBar::SetProperty(const char* pszPropertyName, const char* pszValue )
	{
		if( stricmp( pszPropertyName, "UpTexture" ) == 0 )
		{
			m_UpTexture.SetName( pszValue );
			UpdateThumb();
		}
		else if( stricmp( pszPropertyName, "DownTexture" ) == 0 )
		{
			m_DownTexture.SetName( pszValue );
			UpdateThumb();
		}
		else if( stricmp( pszPropertyName, "ThumbTexture" ) == 0 )
		{
			m_ThumbTexture.SetName( pszValue );
			UpdateThumb();
		}
		else if( stricmp( pszPropertyName, "Min" ) == 0 )
		{
			SetRange( atoi( pszValue ), m_nMax );
		}
		else if( stricmp( pszPropertyName, "Max" ) == 0 )
		{
			SetRange( m_nMin, atoi( pszValue ) );
		}
		else if( stricmp( pszPropertyName, "PageSize" ) == 0 )
		{
			SetPageSize( atoi( pszValue ) );
		}
		else if( stricmp( pszPropertyName, "Direction" ) == 0 )
		{
			if( stricmp( pszValue, "Vertical" ) == 0 )
			{
				SetDirection( Vertical );
			}
			else if( stricmp( pszValue, "Horizontal" ) == 0 )
			{
				SetDirection( Horizontal );
			}
		}
		else
		{
			return __super::SetProperty( pszPropertyName, pszValue );
		}
		OnSize( m_wndRect.Width(), m_wndRect.Height() );
		return true;
	}

	bool CScrollBar::GetProperty(const char* pszPropertyName, char* pValueBuffer, int size )
	{
		if( stricmp( pszPropertyName, "UpTexture" ) == 0 )
		{
			strncpy( pValueBuffer, m_UpTexture.GetName(), size );
		}
		else if( stricmp( pszPropertyName, "DownTexture" ) == 0 )
		{
			strncpy( pValueBuffer, m_DownTexture.GetName(), size );
		}
		else if( stricmp( pszPropertyName, "ThumbTexture" ) == 0 )
		{
			strncpy( pValueBuffer, m_ThumbTexture.GetName(), size );
		}
		else if( stricmp( pszPropertyName, "Min" ) == 0 )
		{
			itoa( m_nMin, pValueBuffer, 10 );
		}
		else if( stricmp( pszPropertyName, "Max" ) == 0 )
		{
			itoa( m_nMax, pValueBuffer, 10 );
		}
		else if( stricmp( pszPropertyName, "PageSize" ) == 0 )
		{
			itoa( m_nPageSize, pValueBuffer, 10 );
		}
		else if( stricmp( pszPropertyName, "Direction" ) == 0 )
		{
			if( Vertical == m_enDirection )
			{
				strncpy( pValueBuffer, "Vertical", size );
			}
			else
			{
				strncpy( pValueBuffer, "Horizontal", size );
			}
		}
		else
		{
			return __super::GetProperty( pszPropertyName, pValueBuffer, size );
		}
		return true;
	}

	void CScrollBar::LoadProperty( TiXmlElement* pXMLElement )
	{
		__super::LoadProperty( pXMLElement );
		CTexture Tex;
		m_enDirection = (Direction)pXMLElement->IntAttribute( "Direction" );
		m_nMin = pXMLElement->IntAttribute( "Min" );
		m_nMax = pXMLElement->IntAttribute( "Max" );
		m_nPageSize	= pXMLElement->IntAttribute( "PageSize" );

		m_UpTexture.SetName( pXMLElement->Attribute( "TextureUp" ) );
		m_DownTexture.SetName( pXMLElement->Attribute( "TextureDown" ) );
		m_ThumbTexture.SetName( pXMLElement->Attribute( "TextureThumb" ) );

		ReCalc();
		UpdateThumb();
		OnSize( m_wndRect.Width(), m_wndRect.Height() );
		if( m_pParent )
		{
			NMUIHDR hdr;
			hdr.code	= NC_CREATE;
			hdr.idFrom	= GetID();
			hdr.pCtrl	= this;
			m_pParent->SendMessage( WM_NOTIFY, 0, (LPARAM)&hdr );
		}
	}

	void CScrollBar::SaveProperty( TiXmlElement* pXMLElement )
	{
		__super::SaveProperty( pXMLElement );

		pXMLElement->SetAttribute( "Direction",		(int)m_enDirection );
		pXMLElement->SetAttribute( "Min",			m_nMin );
		pXMLElement->SetAttribute( "Max",			m_nMax );
		pXMLElement->SetAttribute( "PageSize",		m_nPageSize );
		
		pXMLElement->SetAttribute( "TextureUp",		m_UpTexture.GetName() );
		pXMLElement->SetAttribute( "TextureDown",	m_DownTexture.GetName() );
		pXMLElement->SetAttribute( "TextureThumb",	m_ThumbTexture.GetName() );
	}
}
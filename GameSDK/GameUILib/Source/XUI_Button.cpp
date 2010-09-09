#include "XUI_Header.h"
#include "XUI_Button.h"

namespace XGC
{
	namespace ui
	{
		XUI_Button::XUI_Button(void)
			: m_eState(Normal)
			, m_dwColor( 0 )
		{
			memset( m_hButtonSkin, 0, sizeof(m_hButtonSkin) );
		}

		XUI_Button::~XUI_Button(void)
		{
		}

		void XUI_Button::SetText( _lpctstr sText)
		{
			if( sText )	m_strCaption = sText;
		}

		void XUI_Button::SetState( ButtonState enState )
		{
			m_eState = enState;
		}

		void XUI_Button::RenderSelf( const iPoint& adjust )
		{
			//±³¾°
			iPoint pt( m_WindowPosition + adjust );
			int nWidth	= m_WindowSize.cx;
			int nHeight	= m_WindowSize.cy;

			// °´Å¥±³¾°
			// XUI_DrawSprite( m_hButtonSkin[m_eState], pt.x, pt.y, nWidth, nHeight );
		}

		bool XUI_Button::OnMouseEnter()
		{
			SetState( MouseOver );
			return XUI_Wnd::OnMouseEnter();
		}

		bool XUI_Button::OnMouseLeave()
		{
			SetState( Normal );
			return XUI_Wnd::OnMouseLeave();
		}

		bool XUI_Button::OnButtonDown( int nButton, const iPoint& ptMouse, _uint32 nFlags )
		{
			if ( nButton == 0 )
			{
				SetState( ButtonDown );
			}
			return XUI_Wnd::OnButtonDown( nButton, ptMouse, nFlags );
		}

		bool XUI_Button::OnButtonUp(int nButton, const iPoint& ptMouse, _uint32 nFlags )
		{
			if( nButton == 0 )
			{
				SetState( MouseOver );
			}
			else if( nButton == 2 )
			{
				SetState( MouseOver );
			}
			return XUI_Wnd::OnButtonUp( nButton, ptMouse, nFlags );
		}

		bool XUI_Button::OnKeyDown(_uint32 nKey, _uint32 nFlags )
		{
			if( m_bFocused && nKey == VK_RETURN )
			{
				SetState( ButtonDown );
				return true;
			}
			return XUI_Wnd::OnKeyDown( nKey, nFlags );
		}

		void XUI_Button::OnSetFocus()
		{
			SetState( MouseOver );
		}

		void XUI_Button::OnLostFocus()
		{
			SetState( Normal );
		}

		void XUI_Button::OnEnable( bool bEnable )
		{
			if( !bEnable )
				SetState( Disable );
			else
				SetState( Normal );
		}

		void XUI_Button::OnLoadPropertys( const char* name, TiXmlElement* pNode )
		{
			if( strcmp( "skin", name ) == 0 )
			{
				SetState( m_eState );
			}
		}
	}
}
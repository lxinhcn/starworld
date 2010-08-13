#include "XUI_Header.h"
#include "XUI_Button.h"

namespace XGC
{
	namespace ui
	{
		XUI_Button::XUI_Button(void)
			: m_enState(Normal)
			, m_dwColor( 0 )
			, m_pButtonSkin( NULL )
		{
			SetState( Normal );
		}

		XUI_Button::~XUI_Button(void)
		{
			SAFE_DELETE( m_pButtonSkin );
		}

		void XUI_Button::SetText( _lpctstr sText)
		{
			if( sText )	m_strCaption = sText;
		}

		void XUI_Button::SetState( ButtonState enState )
		{
			m_enState = enState;
			if( m_pButtonSkin )
				m_pButtonSkin->SetUV( 0.25f*m_enState, 0.0f, 0.25f*(m_enState+1), 1.0f );
		}

		void XUI_Button::RenderSelf( const iPoint& adjust )
		{
			//±³¾°
			iPoint pt( m_WindowPosition + adjust );
			int nWidth	= m_WindowSize.cx;
			int nHeight	= m_WindowSize.cy;

			// °´Å¥±³¾°
			if( m_pButtonSkin )
			{
				XUI_DrawSprite( m_pButtonSkin, pt.x, pt.y, nWidth, nHeight );
			}
			// ÎÄ×Ö
			XUI_DrawText( m_strCaption.c_str(), m_pFont, pt.x + nWidth/2, pt.y + nHeight/2 );
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

		bool XUI_Button::OnButtonDown(int button, const iPoint& pt, UINT sysKeys)
		{
			if (button==0)
			{
				SetState( ButtonDown );
			}
			return XUI_Wnd::OnButtonDown(button, pt, sysKeys);
		}

		bool XUI_Button::OnButtonUp(int button, const iPoint& pt, UINT sysKeys)
		{
			if ( button == 0 )
			{
				SetState( MouseOver );
			}
			else if( button == 2 )
			{
				SetState( MouseOver );
			}
			return XUI_Wnd::OnButtonUp(button, pt, sysKeys);
		}

		bool XUI_Button::OnKeyDown(_uint32 keycode, UINT sysKeys)
		{
			if( m_bFocused && keycode == VK_RETURN )
			{
				SetState( ButtonDown );
				return true;
			}
			return XUI_Wnd::OnKeyDown( keycode, sysKeys );
		}

		void XUI_Button::OnSetFocus()
		{
			SetState( MouseOver );
		}

		void XUI_Button::OnLostFocus()
		{
			SetState( Normal );
		}

		void XUI_Button::OnEnable( BOOL bEnable )
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
				SetState( m_enState );
			}
		}
	}
}
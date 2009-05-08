#include "GuiHeader.h"
#include "XUI_Button.h"

namespace UILib
{
	BEGIN_UIMSG_MAP( XUI_Button, XUI_Wnd )
		ON_UI_ENABLE()
	END_UIMSG_MAP()

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

	void XUI_Button::RenderSelf()
	{
		//±³¾°
		x_point pt( 0, 0 );
		pt = ClientToScreen(pt);
		int nWidth	= m_WindowRect.Width();
		int nHeight	= m_WindowRect.Height();

		// °´Å¥±³¾°
		if( m_pButtonSkin )
		{
			XUI_DrawSprite( m_pButtonSkin, pt.x, pt.y, nWidth, nHeight );
		}
		// ÎÄ×Ö
		XUI_DrawText( m_strCaption.c_str(), m_pFont, (float)pt.x + nWidth/2, (float)pt.y + nHeight/2 );
	}

	bool XUI_Button::onMouseEnter()
	{
		SetState( MouseOver );
		return XUI_Wnd::onMouseEnter();
	}

	bool XUI_Button::onMouseLeave()
	{
		SetState( Normal );
		return XUI_Wnd::onMouseLeave();
	}

	bool XUI_Button::onButtonDown(int button, const x_point& pt, UINT sysKeys)
	{
		if (button==0)
		{
			SetState( ButtonDown );
		}
		return XUI_Wnd::onButtonDown(button, pt, sysKeys);
	}

	bool XUI_Button::onButtonUp(int button, const x_point& pt, UINT sysKeys)
	{
		if ( button == 0 )
		{
			// doAction();
			//x_point pt2 = ClientToScreen( pt );
			SetState( MouseOver );
			SendUIMessage( WM_COMMAND, MAKELONG( GetID(), EV_BNCLICKED ), 0 );
		}
		else if( button == 2 )
		{
			SetState( MouseOver );
			SendUIMessage( WM_COMMAND, MAKELONG( GetID(), EV_RBNCLICKED ), 0 );
		}
		return XUI_Wnd::onButtonUp(button, pt, sysKeys);
	}

	bool XUI_Button::onKeyDown(uint32 keycode, UINT sysKeys)
	{
		if( m_bFocused && keycode == VK_RETURN )
		{
			SetState( ButtonDown );
			SendUIMessage( WM_COMMAND, MAKELONG( GetID(), EV_BNCLICKED ), 0 );
			return true;
		}
		return XUI_Wnd::onKeyDown( keycode, sysKeys );
	}

	void XUI_Button::onGetFocus()
	{
		SetState( MouseOver );
	}

	void XUI_Button::onLostFocus()
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
}
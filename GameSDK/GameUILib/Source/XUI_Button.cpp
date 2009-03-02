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
	{
		SetState( Normal );
		memset( m_pButtonSkin, 0, sizeof(m_pButtonSkin) );
	}

	XUI_Button::~XUI_Button(void)
	{
		for( int i = 0; i < _countof(m_pButtonSkin); ++i )
		{
			if( m_pButtonSkin[i] )
			{
				m_pButtonSkin[i]->Release();
				m_pButtonSkin[i] = NULL;
			}
		}
	}

	void XUI_Button::SetText( LPCTSTR sText)
	{
		if( sText )	m_strCaption = sText;
	}

	void XUI_Button::SetSkin( const _string& path, float x, float y, float w, float h )
	{
		for( int i = 0; i < _countof(m_pButtonSkin); ++i )
		{
			if( m_pButtonSkin[i] )
			{
				m_pButtonSkin[i]->Release();
				m_pButtonSkin[i] = NULL;
			}
			m_pButtonSkin[i] = XUI_CreateSprite( path.c_str(), x + w*i, y, w, h );
		}
	}
	
	void XUI_Button::SetState( ButtonState enState )
	{
		m_enState = enState;
	}

	void XUI_Button::RenderSelf(const CRect& clipper)
	{
		//背景
		CPoint pt( 0, 0 );
		pt = ClientToScreen(pt);
		int nWidth	= m_wndRect.Width();
		int nHeight	= m_wndRect.Height();

		// 按钮背景
		if( m_pButtonSkin[m_enState] )
		{
			XUI_DrawSprite( m_pButtonSkin[m_enState], pt.x, pt.y, nWidth, nHeight, clipper );
		}
		// 文字
		XUI_DrawText( m_strCaption.c_str(), m_pFont, pt.x + nWidth/2, pt.y + nHeight/2, m_dwColor, clipper );
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

	bool XUI_Button::onButtonDown(int button, const CPoint& pt, UINT sysKeys)
	{
		if (button==0)
		{
			SetState( ButtonDown );
		}
		return XUI_Wnd::onButtonDown(button, pt, sysKeys);
	}

	bool XUI_Button::onButtonUp(int button, const CPoint& pt, UINT sysKeys)
	{
		if ( button == 0 )
		{
			// doAction();
			//CPoint pt2 = ClientToScreen( pt );
			SetState( MouseOver );
			SendMessage( WM_COMMAND, MAKELONG( GetID(), EV_BNCLICKED ), 0 );
		}
		else if( button == 2 )
		{
			SetState( MouseOver );
			SendMessage( WM_COMMAND, MAKELONG( GetID(), EV_RBNCLICKED ), 0 );
		}
		return XUI_Wnd::onButtonUp(button, pt, sysKeys);
	}

	bool XUI_Button::onKeyDown(DWORD keycode, UINT sysKeys)
	{
		if( m_bFocused && keycode == VK_RETURN )
		{
			SetState( ButtonDown );
			SendMessage( WM_COMMAND, MAKELONG( GetID(), EV_BNCLICKED ), 0 );
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

	// 皮肤改变时调用
	void XUI_Button::onSkinChange()
	{
		USES_CONVERSION;
		SetSkin( A2T( m_SpriteAttribute.path.c_str() ), m_SpriteAttribute.x, m_SpriteAttribute.y, m_SpriteAttribute.w, m_SpriteAttribute.h );
	}
}
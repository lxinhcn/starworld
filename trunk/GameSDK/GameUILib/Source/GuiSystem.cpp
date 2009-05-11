#include "GuiHeader.h"
#include "GuiSystem.h"
#include "XUI_Dialog.h"
#include "LuaBindClass.h"

/****************************************************
* CGuiSystem
****************************************************/
namespace UILib
{
	CGuiSystem::CGuiSystem()
	: m_pDesktop(NULL)
	, m_bPointInRoot(false)
	, m_bEditMode( false )
	, m_hWnd(NULL)
	, m_bInitialized( FALSE )
	, m_nowtime( 0.0f )
	, m_timer_anchor( 0.0f )
	, m_pDefaultFont( NULL )
	, m_capture_element( NULL )
	, m_mouseover_element( NULL )
	{
		m_pDesktop = new XUI_Window();
		m_pDesktop->SetID( DEFAULT_DESKTOP );
		m_pDesktop->SetName( _T("root" ) );
		RegistDesktop( m_pDesktop );
	}

	CGuiSystem::~CGuiSystem(void)
	{
		XUI_Window* pDesktop = RemoveDesktop( DEFAULT_DESKTOP );
		delete pDesktop;
		m_pDesktop = NULL;
	}

	bool CGuiSystem::Initialize( HWND hWnd, _lpcstr lpszPath, const XUI_FontAttribute& fontAttr, XUI_IMouse* pCursor )
	{
		if( m_bInitialized )	return TRUE;

		_lpcstr path = _fullpath( NULL, lpszPath, 0 );
		m_resource_path	= path;
		free( (void*)path ); 
		path = NULL;

		// 初始化定时器系统
		m_timer.initialize( 1024, 4096 );

		m_pCursor				= pCursor;
		m_pDefaultFont			= XUI_CreateFont( fontAttr.name.c_str(), fontAttr.size, fontAttr.bold, fontAttr.italic, fontAttr.antialias );	// 设置字体

		// 初始化lua脚本系统
		Lua::Instance().Initialize();

		XUI_IME::Initialize();
		m_hWnd = hWnd;

		x_rect rcWindow;
		GetClientRect( hWnd, rcWindow );
		m_windowsize = rcWindow.Size();


		RECT rect;
		::GetClientRect( hWnd, &rect );

		m_pDesktop->MoveWindow( 0, 0, rect.right - rect.left, rect.bottom - rect.top);
		return TRUE;
	}

	void CGuiSystem::Unitialize()
	{
		XUI_DestroyFont( m_pDefaultFont );
		m_pCursor = NULL;
	}

	void CGuiSystem::Render()
	{
		if ( m_pDesktop )
		{
			m_pDesktop->Render( m_pDesktop->GetWindowRect() );
			XUI_SetClipping( 0, 0, m_windowsize.cx, m_windowsize.cy );

			if( m_bEditMode && m_capture_element )
			{
				//XUI_Wnd* pWnd = GetRoot();
				//while( pWnd->m_pChildFocusedOn ) pWnd = pWnd->m_pChildFocusedOn;
				const x_rect& rc = m_capture_element->GetWindowRect();
				RenderEditFrame( rc );
			}

			XUI_IME::RenderImeWindow();
			if( m_pCursor->IsMouseOver() )
			{
				m_pCursor->RenderMouse();
			}
		}
	}

	void CGuiSystem::RenderEditFrame( const x_rect& rc )
	{
		XUI_DrawRect( rc, XUI_ARGB(0xff,0,0,0), XUI_ARGB(0x40,80,80,80) );

		x_rect rch( -2, -2, 2, 2 );
		rch.OffsetRect( rc.TopLeft() );
		XUI_DrawRect( rch, 0, XUI_ARGB(0x60,0xff,0xff,0xff) );

		rch.OffsetRect( rc.Width()/2, 0 );
		XUI_DrawRect( rch, 0, XUI_ARGB(0x60,0xff,0xff,0xff) );

		rch.OffsetRect( rc.Width()/2, 0 );
		XUI_DrawRect( rch, 0, XUI_ARGB(0x60,0xff,0xff,0xff) );

		rch.OffsetRect( 0, rc.Height()/2 );
		XUI_DrawRect( rch, 0, XUI_ARGB(0x60,0xff,0xff,0xff) );

		rch.OffsetRect( 0, rc.Height()/2 );
		XUI_DrawRect( rch, 0, XUI_ARGB(0x60,0xff,0xff,0xff) );

		rch.OffsetRect( -rc.Width()/2, 0 );
		XUI_DrawRect( rch, 0, XUI_ARGB(0x60,0xff,0xff,0xff) );
		
		rch.OffsetRect( -rc.Width()/2, 0 );
		XUI_DrawRect( rch, 0, XUI_ARGB(0x60,0xff,0xff,0xff) );

		rch.OffsetRect( 0, -rc.Height()/2 );
		XUI_DrawRect( rch, 0, XUI_ARGB(0x60,0xff,0xff,0xff) );
	}

	uint32 CGuiSystem::DetectHandler( const x_rect &rc, const x_point &pt )
	{
		int i = 0;
		x_rect rch( -2, -2, 2, 2 );
		rch.OffsetRect( rc.TopLeft() );
		if( rch.PtInRect( pt ) ) return i++;

		rch.OffsetRect( rc.Width()/2, 0 );
		if( rch.PtInRect( pt ) ) return i++;

		rch.OffsetRect( rc.Width()/2, 0 );
		if( rch.PtInRect( pt ) ) return i++;

		rch.OffsetRect( 0, rc.Height()/2 );
		if( rch.PtInRect( pt ) ) return i++;

		rch.OffsetRect( 0, rc.Height()/2 );
		if( rch.PtInRect( pt ) ) return i++;

		rch.OffsetRect( -rc.Width()/2, 0 );
		if( rch.PtInRect( pt ) ) return i++;

		rch.OffsetRect( -rc.Width()/2, 0 );
		if( rch.PtInRect( pt ) ) return i++;

		rch.OffsetRect( 0, -rc.Height()/2 );
		if( rch.PtInRect( pt ) ) return i++;

		return -1;
	}

	void CGuiSystem::Update( float fDelta )
	{
		m_nowtime += fDelta;
		while( m_nowtime - m_timer_anchor >= 0.1f )
		{
			m_timer_anchor += 0.1f;
			m_timer.timer();
		}
		SLB::LuaCall< void(float, float) >( Lua::Instance().getState(), "UIUpdateEntry" )( m_nowtime, fDelta );
		if( m_pDesktop )
		{
			m_pDesktop->Update( m_nowtime, fDelta );
		}

	}

	bool CGuiSystem::onMouseMove(XUI_Wnd* pElement, const x_point& pt, UINT sysKeys, long_ptr *result )
	{
		if( pElement == NULL ) return false;
		if( !pElement->IsEnable() )	return false;

		XUI_Wnd *pEnterElement = pElement->FindChildInPoint(pt);

		if( m_bEditMode )
		{
			if( m_capture_element )
			{
				int idx = DetectHandler( m_capture_element->GetWindowRect(), pt );
				if( idx != -1 )
				{

				}
				else if( pEnterElement == m_capture_element )
				{
					m_pCursor->SetMouse( XUI_MOUSE_MOVE );
				}
				else
				{
					m_pCursor->SetMouse( XUI_MOUSE_ARROW );
				}
			}
		}

		if( pEnterElement && pEnterElement != m_mouseover_element )
		{
			if( m_mouseover_element ) m_mouseover_element->onMouseLeave();
			pEnterElement->onMouseEnter();
			m_mouseover_element = pEnterElement;
		}
		if( m_mouseover_element->onMouseMove(pt, sysKeys) )
		{
			*result = m_mouseover_element->SendUIMessage( UIM_MOUSEMOVE, MAKELONG(pt.x,pt.y), sysKeys );
			return true;
		}
		return false;
	}

	//bool CGuiSystem::onMouseLeave( XUI_Wnd* pElement )
	//{
	//	//发生leave事件，所有的子控件都会收到一个leave事件
	//	if( pElement && !pElement->onMouseLeave() )
	//	{
	//		return pElement->m_pChildMouseOver?onMouseLeave( pElement->m_pChildMouseOver ):false;
	//	}
	//	return true;
	//}

	bool CGuiSystem::onButtonDown( XUI_Wnd* pElement, uint32 nButton, const x_point& pt, UINT sysKeys, long_ptr *result )
	{
		if( pElement == NULL ) return false;
		if( !pElement->IsEnable() )	return false;

		XUI_Wnd* pChild = pElement->FindChildInPoint(pt);
		if( pChild->onButtonDown( nButton, pt, sysKeys ) )
		{
			*result = pChild->SendUIMessage( UIM_BUTTONDOWN_BEGIN + nButton, MAKELONG(pt.x, pt.y), sysKeys );
			return true;
		}
		return false;
	}

	bool CGuiSystem::onButtonUp( XUI_Wnd* pElement, uint32 nButton, const x_point& pt, UINT sysKeys, long_ptr *result )
	{
		if( pElement == NULL ) return false;
		if( !pElement->IsEnable() )	return false;
		XUI_Wnd* pChild = pElement->FindChildInPoint(pt);

		if( pChild != m_capture_element )
		{
			if( m_capture_element ) m_capture_element->SetFocus( false );
			pChild->SetFocus( true );
			m_capture_element = pChild;
		}
		if( pChild->onButtonUp( nButton, pt, sysKeys ) )
		{
			*result = pChild->SendUIMessage( UIM_BUTTONUP_BEGIN + nButton, MAKELONG(pt.x, pt.y), sysKeys );
			return true;
		}
		return false;
	}

	//void CGuiSystem::SetFocus(XUI_Wnd* pElement)
	//{
	//	if( !pElement ) return;

	//	if( pElement->m_pChildFocusedOn )
	//	{
	//		pElement->m_pChildFocusedOn->SetFocus( false );
	//		pElement->m_pChildFocusedOn = NULL;
	//	}

	//	XUI_Wnd* pParent = pElement->m_pParent;
	//	if (pParent)
	//	{
	//		//去除别的焦点
	//		if( pParent->m_pChildFocusedOn )
	//			pParent->m_pChildFocusedOn->SetFocus(false);

	//		//当前空间的容器应该也是获得焦点的
	//		SetFocus( pParent );
	//		pParent->m_pChildFocusedOn=pElement;
	//	}
	//	pElement->SetFocus(true);
	//}

	LRESULT CGuiSystem::HandleMessage( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		bool ret = false;
		long_ptr result = 0;
		switch( uMsg )
		{
		case WM_NCCREATE:
			m_hWnd = hWnd;
			break;
		case WM_SIZE:
			{
				x_rect rcWindow;
				GetClientRect( m_hWnd, rcWindow );
				m_windowsize = rcWindow.Size();
			}
			break;
		case WM_INPUTLANGCHANGE:
			XUI_IME::OnInputLangChange();
			break;
		case WM_IME_SETCONTEXT:
			lParam = 0;
			break;
		case WM_IME_STARTCOMPOSITION:
			XUI_IME::ResetCompositionString();
			return 0;
			break;
			//输入法
		case WM_IME_COMPOSITION:
			ret = onImeComp( m_pDesktop, (uint32)wParam, (uint32)lParam, &result );
			break;
		case WM_IME_ENDCOMPOSITION:
			ret = onImeEndComp( m_pDesktop, (uint32)wParam, (uint32)lParam, &result );
			break;
		case WM_IME_NOTIFY:
			ret = onImeNotify( m_pDesktop, (uint32)wParam, (uint32)lParam, &result );
			break;
		default:
			if( uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST )
			{
				ret = HandleMouse( uMsg, wParam, lParam, &result );
			}
			else if( uMsg >= WM_KEYFIRST && uMsg <= WM_IME_KEYLAST )
			{
				ret = HandleKeyboard( uMsg, wParam, lParam, &result );
			}
		}

		return ret?result:XUI_DefWindowProc( hWnd, uMsg, wParam, lParam );
	}

	//处理鼠标
	bool CGuiSystem::HandleMouse(UINT uMsg, WPARAM wParam, LPARAM lParam, long_ptr *result )
	{
		// 如果有模态对话框则将消息都发往模态对话框中
		XUI_Window* pDesktop = m_pDesktop;
		if( !m_ModalList.empty() )
		{
			pDesktop = *m_ModalList.begin();
		}

		bool ret = false;
		if ( pDesktop )
		{
			//获取鼠标的坐标
			x_point pt( LOWORD(lParam), HIWORD(lParam) );
			if (pDesktop->IsPointIn(pt))
			{
				pt.x-=pDesktop->m_WindowRect.left;
				pt.y-=pDesktop->m_WindowRect.top;

				//分发消息
				switch (uMsg)
				{
				case WM_MOUSEMOVE:
					ret = onMouseMove( pDesktop, pt, (UINT)wParam, result );
					break;
				case WM_LBUTTONDOWN:
					ret = onButtonDown( pDesktop, XUI_LBUTTON, pt, (UINT)wParam, result );
					break;
				case WM_RBUTTONDOWN:
					ret = onButtonDown( pDesktop, XUI_RBUTTON, pt, (UINT)wParam, result );
					break;
				case WM_MBUTTONDOWN:
					ret = onButtonDown( pDesktop, XUI_MBUTTON, pt, (UINT)wParam, result );
					break;
				case WM_LBUTTONUP:
					ret = onButtonUp( pDesktop, XUI_LBUTTON, pt, (UINT)wParam, result );
					break;
				case WM_RBUTTONUP:
					ret = onButtonUp( pDesktop, XUI_RBUTTON, pt, (UINT)wParam, result );
					break;
				case WM_MBUTTONUP:
					ret = onButtonUp( pDesktop, XUI_MBUTTON, pt, (UINT)wParam, result );
					break;
				}
			}
		}
		return ret;
	}

	bool CGuiSystem::onKeyDown( XUI_Wnd* pElement, uint32 dwVirtualCode, UINT sysKeys, long_ptr *result )
	{
		//if( !pElement->IsEnable() )	return false;

		//if (pElement->m_pChildFocusedOn)
		//{
		//	return onKeyDown(pElement->m_pChildFocusedOn, dwVirtualCode, sysKeys, result );
		//}
		//else if ( pElement->onKeyDown(dwVirtualCode, sysKeys) )

		if( m_capture_element == NULL ) return false;
		if( !m_capture_element->IsEnable() ) return false;
		if( m_capture_element->onKeyDown(dwVirtualCode, sysKeys) )
		{
			*result = m_capture_element->SendUIMessage( UIM_KEYDOWN, dwVirtualCode, sysKeys );
			return true;
		}
		return false;
	}

	bool CGuiSystem::onKeyUp(XUI_Wnd* pElement, uint32 dwVirtualCode, UINT sysKeys, long_ptr *result )
	{
		//if( !pElement->IsEnable() )	return false;

		//if( pElement->m_pChildFocusedOn )
		//{
		//	return onKeyUp(pElement->m_pChildFocusedOn, dwVirtualCode, sysKeys, result );
		//}
		//else if( pElement->onKeyUp( dwVirtualCode, sysKeys ) )
		if( m_capture_element == NULL ) return false;
		if( !m_capture_element->IsEnable() ) return false;
		if( m_capture_element->onKeyUp(dwVirtualCode, sysKeys) )
		{
			*result = pElement->SendUIMessage( UIM_KEYUP, dwVirtualCode, sysKeys );
			return true;
		}
		return false;
	}

	bool CGuiSystem::onChar(XUI_Wnd* pElement, uint32 dwChar, UINT sysKeys, long_ptr *result )
	{
		//if( !pElement->IsEnable() )	return false;

		//if (pElement->m_pChildFocusedOn)
		//{
		//	return onChar( pElement->m_pChildFocusedOn, dwChar, sysKeys, result );
		//}
		//else if ( pElement->onChar(dwChar, sysKeys))
		if( m_capture_element == NULL ) return false;
		if( !m_capture_element->IsEnable() ) return false;
		if( m_capture_element->onChar( dwChar, sysKeys ) )
		{
			*result = pElement->SendUIMessage( UIM_CHAR, dwChar, sysKeys );
			return true;
		}
		return false;
	}

	//处理键盘
	bool CGuiSystem::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam, long_ptr *result )
	{
		XUI_Window* pDesktop = m_pDesktop;
		if( !m_ModalList.empty() )
		{
			pDesktop = *m_ModalList.begin();
		}

		bool ret = false;
		if ( pDesktop )
		{
			//分发消息
			switch(uMsg)
			{
			case WM_KEYDOWN:
				ret = onKeyDown(pDesktop, (uint32)wParam, (UINT)lParam, result );
				break;
			case WM_KEYUP:
				ret = onKeyUp(pDesktop, (uint32)wParam, (UINT)lParam, result );
				break;
			case WM_CHAR:
				ret = onChar(pDesktop, (uint32)wParam, (UINT)lParam, result );
				break;
			}
		}
		return ret;
	}

	bool CGuiSystem::onImeComp(XUI_Wnd* pElement, uint32 wParam, uint32 lParam, long_ptr *result )
	{
		//if (pElement->m_pChildFocusedOn)
		//	return onImeComp( pElement->m_pChildFocusedOn, wParam, lParam, result );
		//else
		//	return pElement->onImeComp(wParam, lParam);
		return ( m_capture_element != NULL )?m_capture_element->onImeComp( wParam, lParam ):false;
	}

	bool CGuiSystem::onImeEndComp(XUI_Wnd* pElement, uint32 wParam, uint32 lParam, long_ptr *result )
	{
		//if( pElement->m_pChildFocusedOn )
		//	return onImeEndComp( pElement->m_pChildFocusedOn, wParam, lParam, result );
		//else
		//	return pElement->onImeEndComp( wParam, lParam );
		return ( m_capture_element != NULL )?m_capture_element->onImeEndComp( wParam, lParam ):false;
	}

	bool CGuiSystem::onImeNotify(XUI_Wnd* pElement, uint32 wParam, uint32 lParam, long_ptr *result )
	{
		//if (pElement->m_pChildFocusedOn)
		//	return onImeNotify( pElement->m_pChildFocusedOn, wParam, lParam, result );
		//else
		//	return pElement->onImeNotify( wParam, lParam );
		return ( m_capture_element != NULL )?m_capture_element->onImeNotify( wParam, lParam ):false;
	}

	//////////////////////////////////////////////////////////////////////////
	// 界面生成
	bool CGuiSystem::LoadFromFile( _lpcstr pszFilename )
	{
		if( m_pDesktop )
		{
			TiXmlDocument Doc;
			if( Doc.LoadFile( (m_resource_path + pszFilename).c_str() ) == false )
				return false;

			TiXmlNode* pNode = Doc.FirstChild( "WINDOW" );
			if( pNode )
			{
				return m_pDesktop->CreateFromXMLNode( pNode->ToElement() );
			}
		}
		return false;
	}

	bool CGuiSystem::SaveToFile( _lpcstr pszFilename )
	{
		if( m_pDesktop )
		{
			TiXmlDocument	Doc;

			TiXmlElement	XmlElement2( "WINDOW" );
			if( m_pDesktop->SaveToXMLNode( &XmlElement2 ) )
			{
				Doc.InsertEndChild( XmlElement2 );

				Doc.SaveFile( (m_resource_path + pszFilename).c_str() );
				return true;
			}
		}
		return false;
	}

	_lpcstr	CGuiSystem::GetResourcePath()
	{
		return m_resource_path.c_str();
	}

	void CGuiSystem::RegistDesktop( XUI_Window* pDesktop )
	{
		if( !pDesktop )	return;

		CDesktopMap::const_iterator citer = m_DesktopMap.find( pDesktop->GetID() );
		if( citer == m_DesktopMap.end() )
		{
			m_DesktopMap[pDesktop->GetID()] = pDesktop;
		}
	}

	void CGuiSystem::SwitchDesktop( int nDesktopID )
	{
		CDesktopMap::const_iterator citer = m_DesktopMap.find( nDesktopID );
		if( citer != m_DesktopMap.end() )
		{
			m_pDesktop->SendUIMessage( UI_SWITCHLEAVE, 0, 0 );
			m_pDesktop = citer->second;
			m_pDesktop->SendUIMessage( UI_SWITCHENTER, 0, 0 );
		}
	}

	XUI_Window* CGuiSystem::RemoveDesktop( int nDesktopID )
	{
		CDesktopMap::iterator iter = m_DesktopMap.find( nDesktopID );
		if( iter != m_DesktopMap.end() )
		{
			XUI_Window* pDesktop = iter->second;
			m_DesktopMap.erase( iter );
			if( nDesktopID != DEFAULT_DESKTOP && m_pDesktop == pDesktop )
			{
				SwitchDesktop( DEFAULT_DESKTOP );
			}
			return pDesktop;
		}
		return NULL;
	}

	XUI_Window* CGuiSystem::GetDesktop( UINT nDesktopID )
	{
		CDesktopMap::iterator iter = m_DesktopMap.find( nDesktopID );
		if( iter != m_DesktopMap.end() )
			return iter->second;
		else
			return NULL;
	}

	bool CGuiSystem::EnterModaless( XUI_Window* pDialog )
	{
		if( pDialog == NULL )	return false;

		m_ModalList.push_front( pDialog );
		return true;
	}

	void CGuiSystem::LeaveModaless()
	{
		m_ModalList.pop_front();
	}

	unsigned int CGuiSystem::SetTimer( event_function function, unsigned short repeat, unsigned short timer )
	{
		return m_timer.insert_event( new xtimer::event_interface( function, repeat, timer ) );
	}

	void CGuiSystem::KillTimer( unsigned int handle )
	{
		m_timer.remove_event( handle );
	}

	bool SetupDebuger()
	{
		return Lua::Instance().SetupDebuger();
	}
};

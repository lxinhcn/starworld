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
	, m_hWnd(NULL)
	, m_bInitialized( FALSE )
	, m_nowtime( 0.0f )
	, m_timer_anchor( 0.0f )
	, m_pDefaultFont( NULL )
	{
	}

	CGuiSystem::~CGuiSystem(void)
	{
		XUI_Window* pDesktop = RemoveDesktop( DEFAULT_DESKTOP );
		delete pDesktop;
		m_pDesktop = NULL;
	}

	bool CGuiSystem::Initialize( HWND w, _lpctstr p, const XUI_FontAttribute& f, XUI_IMouse* pCursor )
	{
		if( m_bInitialized )	return TRUE;

		m_strMediaPath			= p;
		m_pCursor				= pCursor;
		m_pDefaultFont			= XUI_CreateFont( f.name.c_str(), f.size, f.bold, f.italic, f.antialias );	// 设置字体

		m_pDesktop = new XUI_Window();
		m_pDesktop->SetID( DEFAULT_DESKTOP );
		m_pDesktop->SetName( _T("root" ) );
		RegistDesktop( m_pDesktop );

		// 初始化lua脚本系统
		Lua::Instance().Initialize();

		XUI_IME::Initialize();
		m_hWnd = w;

		x_rect rcWindow;
		GetClientRect( w, rcWindow );
		m_windowsize = rcWindow.Size();

		// 初始化定时器系统
		m_timer.initialize( 1024, 4096 );

		RECT rect;
		::GetClientRect( w, &rect );

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

			XUI_IME::RenderImeWindow();
			if( m_pCursor->IsMouseOver() )
			{
				m_pCursor->RenderMouse();
			}
		}
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
	}

	bool CGuiSystem::onMouseMove(XUI_Wnd* pElement, const x_point& pt, UINT sysKeys)
	{
		if( !pElement->IsEnable() )	return false;

		XUI_Wnd *pEnterElement=pElement->FindChildInPoint(pt);
		if (pEnterElement==pElement->m_pChildMouseOver)
		{
			//焦点状态没有发生改变
			if (pEnterElement)
			{
				//在同一个子控件内
				x_point ptTemp(pt);
				ptTemp.x -= pEnterElement->m_WindowRect.left;
				ptTemp.y -= pEnterElement->m_WindowRect.top;
				ptTemp = pEnterElement->AdjustPoint( ptTemp, false );
				if( onMouseMove(pEnterElement, ptTemp, sysKeys) )
					return true;
			}
		}
		else
		{
			//状态发生改变，应该产生一个leave和一个enter事件
			XUI_Wnd* pTemp=pElement->m_pChildMouseOver;
			pElement->m_pChildMouseOver=pEnterElement;

			if (pTemp && pTemp->IsEnable() )
			{
				//if (onMouseLeave(pTemp))
				//    return true;
				onMouseLeave(pTemp);
			}

			if (pEnterElement && pEnterElement->IsEnable() )
			{
				if ( pEnterElement->onMouseEnter() )
					return true;

				//继续细分
				x_point ptTemp(pt);
				ptTemp.x -= pEnterElement->m_WindowRect.left;
				ptTemp.y -= pEnterElement->m_WindowRect.top;
				ptTemp = pEnterElement->AdjustPoint( ptTemp, false );
				if( onMouseMove(pEnterElement, ptTemp, sysKeys) )
					return true;
			}
		}
		return pElement->onMouseMove(pt, sysKeys);
	}

	bool CGuiSystem::onMouseLeave( XUI_Wnd* pElement )
	{
		//发生leave事件，所有的子控件都会收到一个leave事件
		if (pElement->m_pChildMouseOver)
		{
			XUI_Wnd* pTemp=pElement->m_pChildMouseOver;
			pElement->m_pChildMouseOver=NULL;
			if (onMouseLeave(pTemp))
				return true;
		}

		return pElement->onMouseLeave();
	}

	bool CGuiSystem::onMouseHover(XUI_Wnd* pElement, const x_point& pt)
	{
		if ( pElement->onMouseHover(pt) )
			return true;

		XUI_Wnd* pFocusChild=pElement->FindChildInPoint(pt);
		if (pFocusChild)
		{
			x_point ptTemp(pt);
			ptTemp.x -= pFocusChild->m_WindowRect.left;
			ptTemp.y -= pFocusChild->m_WindowRect.top;
			return onMouseHover(pFocusChild, ptTemp);
		}
		return false;
	}

	bool CGuiSystem::onButtonDown(XUI_Wnd* pElement, int iButton, const x_point& pt, UINT sysKeys)
	{
		if( !pElement->IsEnable() )	return false;

		XUI_Wnd* pChild=pElement->FindChildInPoint(pt);
		if (pChild )
		{
			x_point ptTemp(pt);
			ptTemp.x -= pChild->m_WindowRect.left;
			ptTemp.y -= pChild->m_WindowRect.top;
			ptTemp = pChild->AdjustPoint( ptTemp, false );
			if (onButtonDown(pChild, iButton, ptTemp, sysKeys))
				return true;
		}
		else if( pElement->IsEnable() )
		{
			SetFocus(pElement);
		}
		
		return pElement->onButtonDown(iButton, pt, sysKeys);
	}

	bool CGuiSystem::onButtonUp(XUI_Wnd* pElement, int iButton, const x_point& pt, UINT sysKeys)
	{
		if( !pElement->IsEnable() )	return false;

		XUI_Wnd* pChild=pElement->FindChildInPoint(pt);
		if (pChild)
		{
			x_point ptTemp(pt);
			ptTemp.x -= pChild->m_WindowRect.left;
			ptTemp.y -= pChild->m_WindowRect.top;
			ptTemp = pChild->AdjustPoint( ptTemp, false );
			if (onButtonUp(pChild, iButton, ptTemp, sysKeys))
				return true;
		}
		return pElement->onButtonUp(iButton, pt, sysKeys);
	}

	void CGuiSystem::SetFocus(XUI_Wnd* pElement)
	{
		if( !pElement ) return;

		if( pElement->m_pChildFocusedOn )
			pElement->m_pChildFocusedOn->SetFocus( false );

		XUI_Wnd* pParent = pElement->m_pParent;
		if (pParent)
		{
			//去除别的焦点
			if (pParent->m_pChildFocusedOn)
				pParent->m_pChildFocusedOn->SetFocus(false);
			pParent->m_pChildFocusedOn=pElement;

			//当前空间的容器应该也是获得焦点的
			SetFocus(pParent);
		}
		pElement->SetFocus(true);
	}

	LRESULT CGuiSystem::HandleMessage( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		LRESULT ret = 0;
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
			//lParam = 0;
			break;
		case WM_IME_STARTCOMPOSITION:
			XUI_IME::ResetCompositionString();
			return 0;
			break;
		default:
			if( m_pDesktop )
			{
				if( uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST )
				{
					return HandleMouse( uMsg, wParam, lParam );
				}
				else if( uMsg >= WM_KEYFIRST && uMsg <= WM_IME_KEYLAST )
				{
					return HandleKeyboard( uMsg, wParam, lParam );
				}
				else
				{
					return m_pDesktop->SendUIMessage( uMsg, wParam, lParam );
				}
			}
		}
		return XUI_DefWindowProc( hWnd, uMsg, wParam, lParam );
	}

	//处理鼠标
	LRESULT CGuiSystem::HandleMouse(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		// 如果有模态对话框则将消息都发往模态对话框中
		XUI_Window* pDesktop = m_pDesktop;
		if( !m_ModalList.empty() )
		{
			pDesktop = *m_ModalList.begin();
		}

		bool result = false;
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
					result = onMouseMove(pDesktop, pt, (UINT)wParam);
					break;
				case WM_MOUSEHOVER:
					result = onMouseHover(pDesktop, pt);
					break;
				case WM_LBUTTONDOWN:
					result = onButtonDown(pDesktop, 0, pt, (UINT)wParam);
					break;
				case WM_RBUTTONDOWN:
					result = onButtonDown(pDesktop, 1, pt, (UINT)wParam);
					break;
				case WM_MBUTTONDOWN:
					result = onButtonDown(pDesktop, 2, pt, (UINT)wParam);
					break;
				case WM_LBUTTONUP:
					result = onButtonUp(pDesktop, 0, pt, (UINT)wParam);
					break;
				case WM_RBUTTONUP:
					result = onButtonUp(pDesktop, 1, pt, (UINT)wParam);
					break;
				case WM_MBUTTONUP:
					result = onButtonUp(pDesktop, 2, pt, (UINT)wParam);
					break;
				}
			}
		}

		if( !result )
			return XUI_DefWindowProc( m_hWnd, uMsg, wParam, lParam );

		return 0;
	}

	bool CGuiSystem::onKeyDown(XUI_Wnd* pElement, uint32 dwVirtualCode, UINT sysKeys)
	{
		if( !pElement->IsEnable() )	return false;

		if ( pElement->onKeyDown(dwVirtualCode, sysKeys) )
		{
			pElement->SendUIMessage( WM_KEYDOWN, dwVirtualCode, sysKeys );
			return true;
		}

		if (pElement->m_pChildFocusedOn)
		{
			return onKeyDown(pElement->m_pChildFocusedOn, dwVirtualCode, sysKeys);
		}
		else
			return false;
	}

	bool CGuiSystem::onKeyUp(XUI_Wnd* pElement, uint32 dwVirtualCode, UINT sysKeys)
	{
		if( !pElement->IsEnable() )	return false;

		if ( pElement->onKeyUp(dwVirtualCode, sysKeys))
		{
			pElement->SendUIMessage( WM_KEYUP, dwVirtualCode, sysKeys );
			return true;
		}

		if (pElement->m_pChildFocusedOn)
		{
			return onKeyUp(pElement->m_pChildFocusedOn, dwVirtualCode, sysKeys);
		}
		else
			return false;
	}

	bool CGuiSystem::onChar(XUI_Wnd* pElement, uint32 dwChar, UINT sysKeys)
	{
		if( !pElement->IsEnable() )	return false;

		if ( pElement->onChar(dwChar, sysKeys))
		{
			pElement->SendUIMessage( WM_CHAR, dwChar, sysKeys );
			return true;
		}

		if (pElement->m_pChildFocusedOn)
		{
			return onChar(pElement->m_pChildFocusedOn, dwChar, sysKeys);
		}
		else
			return false;
	}

	//处理键盘
	LRESULT CGuiSystem::HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		XUI_Window* pDesktop = m_pDesktop;
		if( !m_ModalList.empty() )
		{
			pDesktop = *m_ModalList.begin();
		}

		bool result = false;
		if ( pDesktop )
		{
			//分发消息
			switch(uMsg)
			{
			case WM_KEYDOWN:
				result = onKeyDown(pDesktop, (uint32)wParam, (UINT)lParam);
				break;
			case WM_KEYUP:
				result = onKeyUp(pDesktop, (uint32)wParam, (UINT)lParam);
				break;
			case WM_CHAR:
				result = onChar(pDesktop, (uint32)wParam, (UINT)lParam);
				break;
			
			//输入法
			case WM_IME_COMPOSITION:
				result = onImeComp(pDesktop, (uint32)wParam, (uint32)lParam);
				break;
			case WM_IME_ENDCOMPOSITION:
				result = onImeEndComp(pDesktop, (uint32)wParam, (uint32)lParam);
				break;
			case WM_IME_NOTIFY:
				result = onImeNotify(pDesktop, (uint32)wParam, (uint32)lParam);
				break;
			}
		}

		if( !result )
			return XUI_DefWindowProc( m_hWnd, uMsg, wParam, lParam );

		return 0;
	}

	bool CGuiSystem::onImeComp(XUI_Wnd* pElement, uint32 wParam, uint32 lParam)
	{
		if (pElement->onImeComp(wParam, lParam))
			return true;
		else if (pElement->m_pChildFocusedOn)
			return onImeComp(pElement->m_pChildFocusedOn, wParam, lParam);
		else
			return false;
	}

	bool CGuiSystem::onImeEndComp(XUI_Wnd* pElement, uint32 wParam, uint32 lParam)
	{
		if (pElement->onImeEndComp(wParam, lParam))
			return true;
		else if (pElement->m_pChildFocusedOn)
			return onImeEndComp(pElement->m_pChildFocusedOn, wParam, lParam);
		else
			return false;
	}

	bool CGuiSystem::onImeNotify(XUI_Wnd* pElement, uint32 wParam, uint32 lParam)
	{
		if (pElement->onImeNotify(wParam, lParam))
			return true;
		else if (pElement->m_pChildFocusedOn)
			return onImeNotify(pElement->m_pChildFocusedOn, wParam, lParam);
		else
			return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// 界面生成
	bool CGuiSystem::LoadFromFile( _lpctstr pszFilename )
	{
		if( m_pDesktop )
		{
			TiXmlDocument Doc;
			USES_CONVERSION;
			if( Doc.LoadFile( T2A(pszFilename) ) == false )
				return false;

			TiXmlNode* pNode = Doc.FirstChild( "WINDOW" );
			if( pNode )
			{
				return m_pDesktop->CreateFromXMLNode( pNode->ToElement() );
			}
		}
		return false;
	}

	bool CGuiSystem::SaveToFile( _lpctstr pszFilename )
	{
		if( m_pDesktop )
		{
			TiXmlDocument	Doc;

			TiXmlElement	XmlElement2( "WINDOW" );
			if( m_pDesktop->SaveToXMLNode( &XmlElement2 ) )
			{
				Doc.InsertEndChild( XmlElement2 );

				USES_CONVERSION;
				Doc.SaveFile( T2A( pszFilename ) );
				return true;
			}
		}
		return false;
	}

	_lpctstr	CGuiSystem::GetImagePath()
	{
		return m_strMediaPath.c_str();
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
};

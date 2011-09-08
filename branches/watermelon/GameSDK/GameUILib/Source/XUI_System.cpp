#include "XUI_Header.h"
#include "XUI_System.h"
#include "XUI_Dialog.h"
#include "XUI_LuaBinder.h"

/****************************************************
* XUI_System
****************************************************/
namespace XGC
{
	namespace ui
	{

		LRESULT WndProc( HWND hWnd, _uint32 uMsg, int_ptr wParam, long_ptr lParam )
		{
			return XUI::Instance().HandleMessage( hWnd, uMsg, wParam, lParam );
		}

		XUI_System::XUI_System()
			: m_bPointInRoot(false)
			, m_bEditMode( false )
			, m_hWnd(NULL)
			, m_bInitialized( FALSE )
			, m_fNowTime( 0.0f )
			, m_timer_anchor( 0.0f )
			, m_hDefaultFont( NULL )
			, m_pCaptureElement( NULL )
			, m_pOverElement( NULL )
			, m_OldProc(NULL)
		{
			// 初始化定时器系统
			m_Timer.initialize( 1024, 4096 );
		}

		XUI_System::~XUI_System(void)
		{
			CTopWindowList::iterator iter = m_TopWindowList.begin();
			while( iter != m_TopWindowList.end() )
			{
				delete *iter;
				iter = m_TopWindowList.erase(iter);
			}
		}

		bool XUI_System::Initialize( HWND hWnd, _lpcstr lpszPath )
		{
			if( m_bInitialized )	return TRUE;

			_lpcstr path = _fullpath( NULL, lpszPath, 0 );
			m_strResourcePath	= path;
			free( (void*)path ); 
			path = NULL;
			m_hWnd = hWnd;

			// 初始化lua脚本系统
			Lua::Instance().Initialize();

			XUI_IME::Initialize();

			m_OldProc = (WNDPROC)SetWindowLong(hWnd, GWL_WNDPROC, (LONG)WndProc);
			RECT rcWindow;
			GetClientRect( hWnd, &rcWindow );
			m_WindowSize.cx = rcWindow.right - rcWindow.left;
			m_WindowSize.cy = rcWindow.bottom - rcWindow.top;

			return TRUE;
		}

		void XUI_System::Unitialize()
		{
			m_hDefaultFont = NULL;
		}

		void XUI_System::SetEditMode( bool bMode )
		{ 
			m_bEditMode = bMode; 
			//if( !m_bEditMode )
				// m_pInput->SetMouse( XUI_MOUSE_APPSTARTING );
		}

		void XUI_System::Render()
		{
			CTopWindowList::iterator iter = m_TopWindowList.begin();
			while( iter != m_TopWindowList.end() )
			{
				(*iter)->Render( iRect( 0, 0, m_WindowSize.cx, m_WindowSize.cy ) );
				++iter;
			}

			// if( m_pInput->IsMouseOver() )
			{
				// XUI_SetClipping( 0, 0, m_WindowSize.cx, m_WindowSize.cy );
				if( m_bEditMode )
				{
					int x = m_ptMouse.x, y = m_ptMouse.y;
					// m_pInput->GetMousePos( &x, &y );
					if( IsButtonPress( XUIK_LBUTTON ) && m_pCaptureElement && !m_pCaptureElement->GetFlags( XUI_Wnd::FLAGS_EDIT ) )
					{
						XUI_DrawRect( iRect(m_ptMouseDown, iPoint(x,y)), XUI_ARGB(0xff,0,0xff,0), XUI_ARGB(0x55,0,0xff,0) );
					}
					XUI_DrawLine( 0, (float)y, (float)m_WindowSize.cx, (float)y, XUI_ARGB(0xff,0,0xff,0) );
					XUI_DrawLine( (float)x, 0, (float)x, (float)m_WindowSize.cy, XUI_ARGB(0xff,0,0xff,0) );
				}
				XUI_IME::RenderImeWindow();
			}
		}

		_uint32 XUI_System::DetectHandler( XUI_Wnd* pElement, const iPoint &pt )
		{
			if( pElement == NULL ) 
				return -1;

			iRect rc = pElement->GetWindowRect();
			iPoint ptt( pt );
			pElement->ClientToScreen( rc );

			int i = 0;
			iRect rch( -2, -2, 2, 2 );
			rch.OffsetRect( rc.TopLeft() );
			if( rch.PtInRect( pt ) ) return i;
			++i;

			rch.OffsetRect( rc.Width()/2, 0 );
			if( rch.PtInRect( pt ) ) return i;
			++i;

			rch.OffsetRect( rc.Width()/2, 0 );
			if( rch.PtInRect( pt ) ) return i;
			++i;

			rch.OffsetRect( 0, rc.Height()/2 );
			if( rch.PtInRect( pt ) ) return i;
			++i;

			rch.OffsetRect( 0, rc.Height()/2 );
			if( rch.PtInRect( pt ) ) return i;
			++i;

			rch.OffsetRect( -rc.Width()/2, 0 );
			if( rch.PtInRect( pt ) ) return i;
			++i;

			rch.OffsetRect( -rc.Width()/2, 0 );
			if( rch.PtInRect( pt ) ) return i;
			++i;

			rch.OffsetRect( 0, -rc.Height()/2 );
			if( rch.PtInRect( pt ) ) 
				return i;
			++i;

			return -1;
		}

		void XUI_System::Update( float fDelta )
		{
			m_fNowTime += fDelta;
			while( m_fNowTime - m_timer_anchor >= 0.1f )
			{
				m_timer_anchor += 0.1f;
				m_Timer.active();
			}

			SLB::LuaCall< void(float, float) >( Lua::Instance().getState(), "UIUpdateEntry" )( m_fNowTime, fDelta );
			CTopWindowList::iterator iter = m_TopWindowList.begin();
			while( iter != m_TopWindowList.end() )
			{
				(*iter)->Update( m_fNowTime, fDelta );
				++iter;
			}
		}

		bool XUI_System::OnMouseMove( const iPoint& pt, _uint32 sysKeys, long_ptr *result )
		{
			iPoint ptOldMouse = m_ptMouse;
			m_ptMouse = pt;

			XUI_Wnd *pEnterElement = NULL;
			CTopWindowList::iterator iter = m_TopWindowList.begin();
			while( iter != m_TopWindowList.end() )
			{
				pEnterElement = (*iter)->FindChildInPoint( pt - (*iter)->GetWindowPosition() );
				++iter;
			}

			if( m_bEditMode )
			{
				if( m_pCaptureElement && m_pCaptureElement->GetFlags(XUI_Wnd::FLAGS_EDIT) && ( sysKeys & MK_LBUTTON ) )
				{
					// 当前悬停对象在捕获列表中 且 按下鼠标右键
					int dx = pt.x - ptOldMouse.x;
					int dy = pt.y - ptOldMouse.y;
					struct move_windows
					{
						int dx, dy;
						int h;
						move_windows( int dx, int dy, int h )
							: dx( dx )
							, dy( dy )
							, h( h )
						{

						}

						void operator()( XUI_Wnd* element )
						{
							const iRect& r = element->GetWindowRect();
							switch( h )
							{
							case -1:
								element->MoveWindow( r.left+dx, r.top+dy, r.right+dx, r.bottom+dy );
								break;
							case 0:
								element->MoveWindow( r.left+dx, r.top+dy, r.right, r.bottom );
								break;
							case 1:
								element->MoveWindow( r.left, r.top+dy, r.right, r.bottom );
								break;
							case 2:
								element->MoveWindow( r.left, r.top+dy, r.right+dx, r.bottom );
								break;
							case 3:
								element->MoveWindow( r.left, r.top, r.right+dx, r.bottom );
								break;
							case 4:
								element->MoveWindow( r.left, r.top, r.right+dx, r.bottom+dy );
								break;
							case 5:
								element->MoveWindow( r.left, r.top, r.right, r.bottom+dy );
								break;
							case 6:
								element->MoveWindow( r.left+dx, r.top, r.right, r.bottom+dy );
								break;
							case 7:
								element->MoveWindow( r.left+dx, r.top, r.right, r.bottom );
								break;
							}
						}
					};
					std::for_each( m_capture_list.begin(), m_capture_list.end(), move_windows( dx, dy, m_nCurrentHandler ) );
				}
				else if( ( m_nCurrentHandler = DetectHandler( m_pCaptureElement, pt ) ) != -1 )
				{
					switch( m_nCurrentHandler )
					{
					case 0:
					case 4:
						SetMouseCursor( XUI_MOUSE_SIZENWSE );
						break;
					case 1:
					case 5:
						SetMouseCursor( XUI_MOUSE_SIZENS );
						break;
					case 2:
					case 6:
						SetMouseCursor( XUI_MOUSE_SIZENESW );
						break;
					case 3:
					case 7:
						SetMouseCursor( XUI_MOUSE_SIZEWE );
						break;
					}
				}
				else if( pEnterElement && pEnterElement->GetFlags(XUI_Wnd::FLAGS_EDIT) )
				{
					SetMouseCursor( XUI_MOUSE_SIZEALL );
				}
				else
				{
					SetMouseCursor( XUI_MOUSE_ARROW );
				}
			}

			if( pEnterElement && pEnterElement != m_pOverElement )
			{
				if( m_pOverElement ) 
					m_pOverElement->OnMouseLeave();
				pEnterElement->OnMouseEnter();
				m_pOverElement = pEnterElement;
			}

			if( m_pOverElement )
				m_pOverElement->OnMouseMove(pt, sysKeys);
			return false;
		}

		bool XUI_System::OnButtonDown( _uint32 nButton, const iPoint& pt, _uint32 sysKeys, long_ptr *result )
		{
			m_ptMouseDown = pt;
			XUI_Wnd * pMouseDownElement = m_pOverElement;
			if( m_bEditMode )
			{
				if( !pMouseDownElement->GetFlags( XUI_Wnd::FLAGS_EDIT ) && !(sysKeys & MK_CONTROL) )
				{
					std::for_each( m_capture_list.begin(), m_capture_list.end(), 
						std::bind2nd( 
						std::mem_fun1< void, XUI_Wnd, _uint16 >( &XUI_Wnd::ClrFlags ), XUI_Wnd::FLAGS_EDIT ) );

					m_capture_list.clear();
				}
				m_nCurrentHandler = DetectHandler( m_pCaptureElement, pt );
			}
			else
			{
				if( m_pCaptureElement && m_pCaptureElement != m_pOverElement) 
				{
					m_pCaptureElement->SetFocus(false);
					pMouseDownElement->SetFocus( true );
				}

				if( pMouseDownElement )
					pMouseDownElement->OnButtonDown( nButton, pt, sysKeys );
			}
			m_pCaptureElement = pMouseDownElement;
			return false;
		}

		bool XUI_System::OnButtonUp( _uint32 nButton, const iPoint& pt, _uint32 sysKeys, long_ptr *result )
		{
			CTopWindowList::iterator iter = m_TopWindowList.begin();
			while( iter != m_TopWindowList.end() )
			{
				XUI_Wnd *pElement = (*iter);
				if( m_bEditMode )
				{
					iRect rcArea( m_ptMouseDown, pt );
					if( rcArea.IsRectEmpty() )
					{
						XUI_Wnd* find_element = pElement->FindChildInPoint( pt - pElement->GetWindowPosition() );

						if( find_element && !find_element->GetFlags( XUI_Wnd::FLAGS_EDIT ) )
						{
							if( std::find_if( m_capture_list.begin(), m_capture_list.end(), 
								std::bind2nd( std::equal_to< XUI_Wnd* >(), find_element ) ) == m_capture_list.end() )
							{
								find_element->SetFlags( XUI_Wnd::FLAGS_EDIT );
								m_capture_list.push_back( find_element );
							}
						}
					}
					else
					{
						CWndList capture_list;
						pElement->FindRectIn( rcArea - pElement->GetWindowPosition(), capture_list );

						CWndList::iterator result = capture_list.begin();
						do
						{
							result = std::find_first_of( result, capture_list.end(), m_capture_list.begin(), m_capture_list.end() );
							if( result != capture_list.end() )
								result = capture_list.erase( result );
						}while( result != capture_list.end() );
						m_capture_list.insert( m_capture_list.end(), capture_list.begin(), capture_list.end() );

						std::for_each( m_capture_list.begin(), m_capture_list.end(), 
							std::bind2nd( std::mem_fun1< void, XUI_Wnd, _uint16 >( &XUI_Wnd::SetFlags ), XUI_Wnd::FLAGS_EDIT ) );
					}
				}
				++iter;
			}
			if( m_pCaptureElement )
				return m_pCaptureElement->OnButtonUp( nButton, pt, sysKeys );
			return false;
		}

		LRESULT XUI_System::HandleMessage( HWND hWnd, _uint32 uMsg, int_ptr wParam, long_ptr lParam )
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
					RECT rcWindow;
					GetClientRect( m_hWnd, &rcWindow );
					m_WindowSize = iSize( rcWindow.right - rcWindow.left, rcWindow.bottom - rcWindow.top );
				}
				break;
			case WM_INPUTLANGCHANGE:
				XUI_IME::OnInputLangChange();
				ret = true;
				break;
			case WM_IME_SETCONTEXT:
				lParam = 0;
				break;
			case WM_IME_STARTCOMPOSITION:
				XUI_IME::ResetCompositionString();
				ret = true;
				break;
				//输入法
			case WM_IME_COMPOSITION:
				ret = OnImeComp( wParam, lParam, &result );
				break;
			case WM_IME_ENDCOMPOSITION:
				ret = OnImeEndComp( wParam, lParam, &result );
				break;
			case WM_IME_NOTIFY:
				ret = OnImeNotify( wParam, lParam, &result );
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

			return ret?result:CallWindowProc( m_OldProc, hWnd, uMsg, wParam, lParam );
		}

		//处理鼠标
		bool XUI_System::HandleMouse(_uint32 uMsg, int_ptr wParam, long_ptr lParam, long_ptr *result )
		{
			// 如果有模态对话框则将消息都发往模态对话框中

			bool ret = false;
			//获取鼠标的坐标
			iPoint pt( LOWORD(lParam), HIWORD(lParam) );

			//分发消息
			switch (uMsg)
			{
			case WM_MOUSEMOVE:
				ret = OnMouseMove( pt, wParam, result );
				break;
			case WM_LBUTTONDOWN:
				ret = OnButtonDown( XUI_LBUTTON, pt, wParam, result );
				break;
			case WM_RBUTTONDOWN:
				ret = OnButtonDown( XUI_RBUTTON, pt, wParam, result );
				break;
			case WM_MBUTTONDOWN:
				ret = OnButtonDown( XUI_MBUTTON, pt, wParam, result );
				break;
			case WM_LBUTTONUP:
				ret = OnButtonUp( XUI_LBUTTON, pt, wParam, result );
				break;
			case WM_RBUTTONUP:
				ret = OnButtonUp( XUI_RBUTTON, pt, wParam, result );
				break;
			case WM_MBUTTONUP:
				ret = OnButtonUp( XUI_MBUTTON, pt, wParam, result );
				break;
			}
			return ret;
		}

		bool XUI_System::OnKeyDown( _uint32 dwVirtualCode, _uint32 sysKeys, long_ptr *result )
		{
			XUI_Wnd *pWindow = NULL;
			if( !m_ModalList.empty() )
			{
				pWindow = *m_ModalList.begin();
			}
			else
			{
				pWindow = m_pCaptureElement;
			}

			if( pWindow && pWindow->IsEnable() && pWindow->IsVisible() )
			{
				return pWindow->OnKeyDown(dwVirtualCode, sysKeys);
			}
			return false;
		}

		bool XUI_System::OnKeyUp( _uint32 dwVirtualCode, _uint32 sysKeys, long_ptr *result )
		{
			XUI_Wnd *pWindow = NULL;
			if( !m_ModalList.empty() )
			{
				pWindow = *m_ModalList.begin();
			}
			else
			{
				pWindow = m_pCaptureElement;
			}

			if( pWindow && pWindow->IsEnable() && pWindow->IsVisible() )
			{
				return pWindow->OnKeyUp(dwVirtualCode, sysKeys);
			}
			return false;
		}

		bool XUI_System::OnChar( _uint32 dwChar, _uint32 sysKeys, long_ptr *result )
		{
			XUI_Wnd *pWindow = NULL;
			if( !m_ModalList.empty() )
			{
				pWindow = *m_ModalList.begin();
			}
			else
			{
				pWindow = m_pCaptureElement;
			}

			if( pWindow && pWindow->IsEnable() && pWindow->IsVisible() )
			{
				return pWindow->OnChar( dwChar, sysKeys);
			}
			return false;
		}

		//处理键盘
		bool XUI_System::HandleKeyboard( _uint32 uMsg, int_ptr wParam, long_ptr lParam, long_ptr *result )
		{
			bool ret = false;
			//分发消息
			switch(uMsg)
			{
			case WM_KEYDOWN:
				ret = OnKeyDown( wParam, lParam, result );
				break;
			case WM_KEYUP:
				ret = OnKeyUp( wParam, lParam, result );
				break;
			case WM_CHAR:
				ret = OnChar( wParam, lParam, result );
				break;
			}
			return ret;
		}

		bool XUI_System::OnImeComp( int_ptr wParam, long_ptr lParam, long_ptr *result )
		{
			XUI_Wnd *pWindow = NULL;
			if( !m_ModalList.empty() )
			{
				pWindow = *m_ModalList.begin();
			}
			else
			{
				pWindow = m_pCaptureElement;
			}

			if( pWindow && pWindow->IsEnable() && pWindow->IsVisible() )
			{
				return pWindow->OnImeComp( wParam, lParam );
			}
			return false;
		}

		bool XUI_System::OnImeEndComp( int_ptr wParam, long_ptr lParam, long_ptr *result )
		{
			XUI_Wnd *pWindow = NULL;
			if( !m_ModalList.empty() )
			{
				pWindow = *m_ModalList.begin();
			}
			else
			{
				pWindow = m_pCaptureElement;
			}

			if( pWindow && pWindow->IsEnable() && pWindow->IsVisible() )
			{
				return pWindow->OnImeEndComp( wParam, lParam );
			}
			return false;
		}

		bool XUI_System::OnImeNotify( int_ptr wParam, long_ptr lParam, long_ptr *result )
		{
			XUI_Wnd *pWindow = NULL;
			if( !m_ModalList.empty() )
			{
				pWindow = *m_ModalList.begin();
			}
			else
			{
				pWindow = m_pCaptureElement;
			}

			if( pWindow && pWindow->IsEnable() && pWindow->IsVisible() )
			{
				return pWindow->OnImeNotify( wParam, lParam );
			}
			return false;
		}

		//////////////////////////////////////////////////////////////////////////
		// 界面生成
		bool XUI_System::LoadFromFile( _lpcstr pszFilename )
		{
			TiXmlDocument Doc;
			if( Doc.LoadFile( (m_strResourcePath + pszFilename).c_str() ) == false )
				return false;

			TiXmlNode* pNode = Doc.FirstChild( "window" );
			while( pNode )
			{
				XUI_Window *pTopWindow = new XUI_Window();
				if( pTopWindow )
				{
					if( pTopWindow->CreateFromXMLNode( pNode->ToElement() ) )
					{
						RegistTopWindow( pTopWindow );
						m_pCaptureElement = pTopWindow;
					}
				}
				pNode = Doc.NextSibling( "window" );
			}
			return false;
		}

		bool XUI_System::SaveToFile( _lpcstr pszFilename )
		{
			TiXmlDocument	Doc;

			CTopWindowList::iterator iter = m_TopWindowList.begin();
			while( iter != m_TopWindowList.end() )
			{
				TiXmlElement	XmlWindow( "window" );
				if( (*iter)->SaveToXMLNode( &XmlWindow ) )
				{
					Doc.InsertEndChild( XmlWindow );
				}
			}

			return Doc.SaveFile( (m_strResourcePath + pszFilename).c_str() );
		}

		_lpcstr	XUI_System::GetResourcePath()
		{
			return m_strResourcePath.c_str();
		}

		void XUI_System::RegistTopWindow( XUI_Window* pTopWindow )
		{
			if( NULL == pTopWindow )
				return;

			CTopWindowList::const_iterator citer = find( m_TopWindowList.begin(), m_TopWindowList.end(), pTopWindow );
			if( citer == m_TopWindowList.end() )
			{
				m_TopWindowList.push_back( pTopWindow );
			}
		}

		XUI_Window* XUI_System::RemoveTopWindow( XUI_Window* pTopWindow )
		{
			CTopWindowList::iterator iter = find( m_TopWindowList.begin(), m_TopWindowList.end(), pTopWindow );
			if( iter != m_TopWindowList.end() )
			{
				m_TopWindowList.erase( iter );
				return pTopWindow;
			}
			return NULL;
		}

		XUI_Window* XUI_System::GetTopWindow( _lpctstr lpszWindowName )
		{
			if( lpszWindowName == NULL )
				return NULL;

			CTopWindowList::iterator iter = m_TopWindowList.begin();
			while( iter != m_TopWindowList.end() );
			{
				if( (*iter)->GetName() == lpszWindowName )
					return *iter;
				++iter;
			}
			return NULL;
		}

		bool XUI_System::EnterModaless( XUI_Window* pDialog )
		{
			if( pDialog == NULL )	return false;

			m_ModalList.push_front( pDialog );
			return true;
		}

		void XUI_System::LeaveModaless()
		{
			m_ModalList.pop_front();
		}

		unsigned int XUI_System::SetTimer( _tfunction function, unsigned short repeat, unsigned short timer )
		{
			return m_Timer.insert_event( function, repeat, timer );
		}

		void XUI_System::KillTimer( unsigned int handle )
		{
			m_Timer.remove_event( handle );
		}

		bool SetupDebuger()
		{
			return Lua::Instance().SetupDebuger();
		}
	}
};

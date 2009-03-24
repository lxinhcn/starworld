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
		m_pDesktop=new XUI_Window();
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

	bool CGuiSystem::Initialize( HWND w, _lpctstr p, const XUI_FontAttribute& f, const XUI_SpriteAttribute& c )
	{
		m_strMediaPath			= p;
		m_pCursor				= XUI_CreateSprite( XA2T(c.path), c.x, c.y, c.w, c.h );	// �������
		m_pDefaultFont			= XUI_CreateFont( XA2T(f.name), f.size, f.bold, f.italic, f.antialias );	// ��������
		if( m_bInitialized )	return TRUE;

		// ��ʼ��lua�ű�ϵͳ
		Lua::Instance().Initialize();

		m_hWnd = w;

		// ��ʼ����ʱ��ϵͳ
		m_timer.initialize( 1024, 4096 );

		RECT rect;
		::GetClientRect( w, &rect );

		m_pDesktop->MoveWindow( 0, 0, rect.right - rect.left, rect.bottom - rect.top);
		return TRUE;
	}

	void CGuiSystem::Unitialize()
	{
		XUI_DestroyFont( m_pDefaultFont );
		XUI_DestroySprite( m_pCursor );
	}

	void CGuiSystem::Render()
	{
		if ( m_pDesktop )
		{
			m_pDesktop->Render( m_pDesktop->GetWindowRect() );
			XUI_DrawSprite( m_pCursor, m_MousePt.x, m_MousePt.y, m_pCursor->GetWidth(), m_pCursor->GetHeight(), m_pDesktop->GetWindowRect() );
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
	}

	bool CGuiSystem::onMouseMove(XUI_Wnd* pElement, const CPoint& pt, UINT sysKeys)
	{
		m_MousePt = pt;
		if( !pElement->IsEnable() )	return false;

		XUI_Wnd *pEnterElement=pElement->FindChildInPoint(pt);
		if (pEnterElement==pElement->m_pChildMouseOver)
		{
			//����״̬û�з����ı�
			if (pEnterElement)
			{
				//��ͬһ���ӿؼ���
				CPoint ptTemp(pt);
				ptTemp.x -= pEnterElement->m_WindowRect.left;
				ptTemp.y -= pEnterElement->m_WindowRect.top;
				ptTemp = pEnterElement->AdjustPoint( ptTemp, false );
				if( onMouseMove(pEnterElement, ptTemp, sysKeys) )
					return true;
			}
		}
		else
		{
			//״̬�����ı䣬Ӧ�ò���һ��leave��һ��enter�¼�
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

				//����ϸ��
				CPoint ptTemp(pt);
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
		//����leave�¼������е��ӿؼ������յ�һ��leave�¼�
		if (pElement->m_pChildMouseOver)
		{
			XUI_Wnd* pTemp=pElement->m_pChildMouseOver;
			pElement->m_pChildMouseOver=NULL;
			if (onMouseLeave(pTemp))
				return true;
		}

		return pElement->onMouseLeave();
	}

	bool CGuiSystem::onMouseHover(XUI_Wnd* pElement, const CPoint& pt)
	{
		if ( pElement->onMouseHover(pt) )
			return true;

		XUI_Wnd* pFocusChild=pElement->FindChildInPoint(pt);
		if (pFocusChild)
		{
			CPoint ptTemp(pt);
			ptTemp.x -= pFocusChild->m_WindowRect.left;
			ptTemp.y -= pFocusChild->m_WindowRect.top;
			return onMouseHover(pFocusChild, ptTemp);
		}
		return false;
	}

	bool CGuiSystem::onButtonDown(XUI_Wnd* pElement, int iButton, const CPoint& pt, UINT sysKeys)
	{
		if( !pElement->IsEnable() )	return false;

		XUI_Wnd* pChild=pElement->FindChildInPoint(pt);
		if (pChild )
		{
			CPoint ptTemp(pt);
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

	bool CGuiSystem::onButtonUp(XUI_Wnd* pElement, int iButton, const CPoint& pt, UINT sysKeys)
	{
		if( !pElement->IsEnable() )	return false;

		XUI_Wnd* pChild=pElement->FindChildInPoint(pt);
		if (pChild)
		{
			CPoint ptTemp(pt);
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
		XUI_Wnd* pParent=pElement->m_pParent;
		if (pParent)
		{
			//ȥ����Ľ���
			if (pParent->m_pChildFocusedOn)
				pParent->m_pChildFocusedOn->SetFocus(false);
			pParent->m_pChildFocusedOn=pElement;

			//��ǰ�ռ������Ӧ��Ҳ�ǻ�ý����
			SetFocus(pParent);
		}
		pElement->SetFocus(true);
	}

	void CGuiSystem::HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		if( m_pDesktop )
		{
			if( uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST )
			{
				HandleMouse( uMsg, wParam, lParam );
			}
			else if( uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST )
			{
				HandleKeyboard( uMsg, wParam, lParam );
			}
			else
			{
				m_pDesktop->SendMessage( uMsg, wParam, lParam );
			}
		}
	}

	//�������
	void CGuiSystem::HandleMouse(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		// �����ģ̬�Ի�������Ϣ������ģ̬�Ի�����
		XUI_Window* pDesktop = m_pDesktop;
		if( !m_ModalList.empty() )
		{
			pDesktop = *m_ModalList.begin();
		}

		if ( pDesktop )
		{
			//��ȡ��������
			int x=LOWORD(lParam);
			int y=HIWORD(lParam);
			CPoint pt(x, y);
			if (pDesktop->IsPointIn(pt))
			{
				pt.x-=pDesktop->m_WindowRect.left;
				pt.y-=pDesktop->m_WindowRect.top;

				//�ַ���Ϣ
				switch (uMsg)
				{
				case WM_MOUSEMOVE:
					onMouseMove(pDesktop, pt, (UINT)wParam);
					break;
				case WM_MOUSEHOVER:
					onMouseHover(pDesktop, pt);
					break;
				case WM_LBUTTONDOWN:
					onButtonDown(pDesktop, 0, pt, (UINT)wParam);
					break;
				case WM_RBUTTONDOWN:
					onButtonDown(pDesktop, 1, pt, (UINT)wParam);
					break;
				case WM_MBUTTONDOWN:
					onButtonDown(pDesktop, 2, pt, (UINT)wParam);
					break;
				case WM_LBUTTONUP:
					onButtonUp(pDesktop, 0, pt, (UINT)wParam);
					break;
				case WM_RBUTTONUP:
					onButtonUp(pDesktop, 1, pt, (UINT)wParam);
					break;
				case WM_MBUTTONUP:
					onButtonUp(pDesktop, 2, pt, (UINT)wParam);
					break;
				}
			}
		}
	}

	bool CGuiSystem::onKeyDown(XUI_Wnd* pElement, uint32 dwVirtualCode, UINT sysKeys)
	{
		if( !pElement->IsEnable() )	return false;

		if ( pElement->onKeyDown(dwVirtualCode, sysKeys) )
		{
			pElement->SendMessage( WM_KEYDOWN, dwVirtualCode, sysKeys );
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
			pElement->SendMessage( WM_KEYUP, dwVirtualCode, sysKeys );
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
			pElement->SendMessage( WM_CHAR, dwChar, sysKeys );
			return true;
		}

		if (pElement->m_pChildFocusedOn)
		{
			return onChar(pElement->m_pChildFocusedOn, dwChar, sysKeys);
		}
		else
			return false;
	}

	//��������
	void CGuiSystem::HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		XUI_Window* pDesktop = m_pDesktop;
		if( !m_ModalList.empty() )
		{
			pDesktop = *m_ModalList.begin();
		}

		if ( pDesktop )
		{
			//�ַ���Ϣ
			switch(uMsg)
			{
			case WM_KEYDOWN:
				onKeyDown(pDesktop, (uint32)wParam, (UINT)lParam);
				break;
			case WM_KEYUP:
				onKeyUp(pDesktop, (uint32)wParam, (UINT)lParam);
				break;
			case WM_CHAR:
				onChar(pDesktop, (uint32)wParam, (UINT)lParam);
				break;
			
			//���뷨
			case WM_IME_COMPOSITION:
				onImeComp(pDesktop, (uint32)wParam, (uint32)lParam);
				break;
			case WM_IME_ENDCOMPOSITION:
				onImeEndComp(pDesktop, (uint32)wParam, (uint32)lParam);
				break;
			case WM_IME_NOTIFY:
				onImeNotify(pDesktop, (uint32)wParam, (uint32)lParam);
				break;
			
			}
		}
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

	//�������뷨
	void CGuiSystem::HandleIMEMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch(uMsg)
		{
		case WM_IME_COMPOSITION:
			onImeComp(m_pDesktop, (uint32)wParam, (uint32)lParam);
			break;
		case WM_IME_ENDCOMPOSITION:
			onImeEndComp(m_pDesktop, (uint32)wParam, (uint32)lParam);
			break;
		case WM_IME_NOTIFY:
			onImeNotify(m_pDesktop, (uint32)wParam, (uint32)lParam);
			break;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// ��������
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
			m_pDesktop->SendMessage( UI_SWITCHLEAVE, 0, 0 );
			m_pDesktop = citer->second;
			m_pDesktop->SendMessage( UI_SWITCHENTER, 0, 0 );
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
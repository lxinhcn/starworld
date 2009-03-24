#include "GuiHeader.h"
#include "XUI_Wnd.h"
#include "tinyxml.h"
#include "GuiSystem.h"
#include "LuaBindClass.h"
#include "SLB\SLB.hpp"
using namespace SLB;

namespace UILib
{
	/*******************************************************
	XUI_Wnd
	*******************************************************/
	XUI_Wnd::XUI_Wnd(void)
	: m_pParent(NULL)
	, m_pChildMouseOver(NULL)
	, m_pChildFocusedOn(NULL)
	, m_bFocused(false)
	, m_bVisible( true )
	, m_bEnable( true )
	, m_bOwnerDraw( false )
	, m_pFont( NULL )
	{
		Move(0,0,0,0);
		m_bTranslateParent = true;
		m_fZ=0.1f;
	}

	XUI_Wnd::~XUI_Wnd(void)
	{
		Destroy();
	}

	void XUI_Wnd::Destroy()
	{
		DestroyAllChild();
		if( m_pParent )
		{
			XUI_Wnd* pParent = m_pParent;
			SetParent( NULL );
			pParent->RemoveChild( this, false );
		}
	}

	void XUI_Wnd::Release()
	{
		delete this;
	}

	void XUI_Wnd::Validate()
	{
		m_WindowRect.NormalizeRect();
	}

	//事件处理
	//鼠标
	//返回 true 则父控件将不继续处理该消息
	bool XUI_Wnd::onMouseMove(const CPoint& pt, UINT sysKeys)
	{
		return false;
	}

	bool XUI_Wnd::onMouseHover(const CPoint& pt)
	{
		return false;
	}

	bool XUI_Wnd::onMouseEnter()
	{
		return false;
	}

	bool XUI_Wnd::onMouseLeave()
	{
		return false;
	}

	bool XUI_Wnd::onButtonDown(int button, const CPoint& pt, UINT sysKeys)
	{
		UINT id=0;
		switch (button)
		{
		case 0:
			id=EV_MOUSELBTDOWN;
			SendMessage( EV_MOUSELBTDOWN, sysKeys, MAKELONG( pt.x, pt.y ) );
			break;
		case 1:
			id=EV_MOUSERBTDOWN;
			SendMessage( EV_MOUSERBTDOWN, sysKeys, MAKELONG( pt.x, pt.y ) );
			break;
		case 2:
			id=EV_MOUSEMBTDOWN;
			break;
		}
		return false;
	}

	bool XUI_Wnd::onButtonUp(int button, const CPoint& pt, UINT sysKeys)
	{
		UINT id=0;
		switch (button)
		{
		case 0:
			id=EV_MOUSELBTUP;
			SendMessage( EV_MOUSELBTUP, sysKeys, MAKELONG( pt.x, pt.y ) );
			break;
		case 1:
			id=EV_MOUSERBTUP;
			SendMessage( EV_MOUSERBTUP, sysKeys, MAKELONG( pt.x, pt.y ) );
			break;
		case 2:
			id=EV_MOUSEMBTUP;
			break;
		}
		return false;
	}

	//键盘
	bool XUI_Wnd::onKeyDown(DWORD keycode, UINT sysKeys)
	{
		return false;
	}

	bool XUI_Wnd::onKeyUp(DWORD keycode, UINT sysKeys)
	{
		return false;
	}

	bool XUI_Wnd::onChar(DWORD c, UINT sysKeys)
	{
		return false;
	}

	//输入法
	bool XUI_Wnd::onImeComp(DWORD wParam, DWORD lParam)
	{
		return false;
	}

	bool XUI_Wnd::onImeEndComp(DWORD wParam, DWORD lParam)
	{
		return false;
	}

	bool XUI_Wnd::onImeNotify(DWORD wParam, DWORD lParam)
	{
		return false;
	}

	//end of 事件处理

	//将一个坐标从控件坐标转换成屏幕坐标
	CPoint XUI_Wnd::ClientToScreen(const CPoint& pt) const
	{
		CPoint rstPt= AdjustPoint(pt, true);
		CPoint ptSelf( m_WindowRect.TopLeft() );
		rstPt+=ptSelf;
		if (m_pParent)
			return m_pParent->ClientToScreen(rstPt);
		else
			return rstPt;
	}

	//将一个坐标从屏幕坐标转换成控件坐标
	CPoint XUI_Wnd::ScreenToClient(const CPoint& pt) const
	{
		CPoint rstPt=AdjustPoint(pt, false);
		if (m_pParent)
			rstPt=m_pParent->ScreenToClient(rstPt);
		CPoint ptSelf( m_WindowRect.TopLeft() );
		rstPt-=ptSelf;
		return rstPt;
	}

	int XUI_Wnd::FindChild(XUI_Wnd* pElement) const
	{
		for( UINT i = 0; i < m_pChildren.size(); i++ )
			if( m_pChildren[i] == pElement )
				return i;
		return -1;
	}

	void XUI_Wnd::AddChild(XUI_Wnd* pElement )
	{
		if (-1==FindChild(pElement))
		{
			pElement->m_pParent=this;
			m_pChildren.push_back(pElement);
			// 把自己作为子元素的观察者,这样，子控件产生的消息，父窗口就可以通过
			// 消息映射表截获到
			pElement->AddCommonListener( this );
		}
	}

	//将控件加入为子控件，并修改其坐标
	void XUI_Wnd::AddChildAt(XUI_Wnd* pElement, int x, int y)
	{
		if (pElement)
		{
			if (pElement->m_WindowRect.left != 0 || pElement->m_WindowRect.top != 0 )
			{
				pElement->Offset(-pElement->m_WindowRect.left, -pElement->m_WindowRect.top );
			}
			pElement->Offset(x, y);
			AddChild(pElement);
		}
	}

	void XUI_Wnd::RemoveChild(XUI_Wnd* pElement, bool bDestroy )
	{
		int i = FindChild( pElement );
		if( i >= 0 )
		{
			std::vector<XUI_Wnd*>::iterator itr = m_pChildren.begin()+i;

			XUI_Wnd* pChild = *itr;
			if( m_pChildMouseOver == pChild )	m_pChildMouseOver = NULL;
			if( m_pChildFocusedOn == pChild )	m_pChildFocusedOn = NULL;

			if( bDestroy ) delete pChild;
			m_pChildren.erase(itr);
		}
	}

	void XUI_Wnd::DestroyAllChild()
	{
		for( size_t i = 0; i < m_pChildren.size(); ++i )
		{
			XUI_Wnd* pXUI_Wnd = m_pChildren[i];
			pXUI_Wnd->SetParent( NULL );
			delete pXUI_Wnd;
		}
		m_pChildren.clear();
		m_pChildMouseOver = NULL;
		m_pChildFocusedOn = NULL;
	}

	//寻找在某个坐标上的控件
	XUI_Wnd* XUI_Wnd::FindChildInPoint(const CPoint& pt) const
	{
		AdjustPoint(pt, true);
		for (int i=(int)m_pChildren.size()-1; i>=0; i--)
		{
			XUI_Wnd* pElement=m_pChildren[i];
			if (pElement->m_bVisible && pElement->IsPointIn(pt))
				return pElement;
		}
		return NULL;
	}

	XUI_Wnd* XUI_Wnd::FindChildByName(LPCTSTR sName) const
	{
		for(size_t i=0; i<m_pChildren.size(); i++)
		{
			XUI_Wnd* pElement=m_pChildren[i];
			if ( !pElement->m_strName.empty() && pElement->m_strName == sName )
				return pElement;
		}
		return NULL;
	}

	XUI_Wnd* XUI_Wnd::FindChildByID( int nID )const
	{
		for(size_t i=0; i<m_pChildren.size(); i++)
		{
			XUI_Wnd* pElement=m_pChildren[i];
			if ( pElement->m_nID == nID )
				return pElement;
		}
		return NULL;
	}

	// 遍历子对象。
	XUI_Wnd* XUI_Wnd::ForAllChild( ElementCallback pfnCallback, void* pData )
	{
		for( size_t i = 0; i < m_pChildren.size(); ++i )
		{
			XUI_Wnd* pElement=m_pChildren[i];
			if( pfnCallback( pElement, pData ) )	return pElement;
		}
		return NULL;
	}

	// 移动对象
	void XUI_Wnd::Move(int left, int top, int right, int bottom)
	{
		CRect oldRect = m_WindowRect;
		m_WindowRect.SetRect( left, top, right, bottom );
		Validate();
		SendMessage( WM_MOVE, 0, MAKELONG( left, top ) );
		SendMessage( WM_SIZE, 0, MAKELONG( m_WindowRect.Width(), m_WindowRect.Height() ) );
		// 发送位置变更消息
		OnMoveWindow( m_WindowRect );
	}

	void XUI_Wnd::Offset(int x, int y)
	{
		m_WindowRect.OffsetRect( x, y );
		SendMessage( WM_MOVE, 0, MAKELONG( m_WindowRect.left, m_WindowRect.top ) );
	}

	void XUI_Wnd::ShowWindow( bool bVisible /* = true  */ )
	{
		SendMessage( WM_SHOWWINDOW, bVisible, 0 );
		m_bVisible=bVisible;
	}

	void XUI_Wnd::EnableWindow( bool bEnable )
	{
		SendMessage( WM_ENABLE, bEnable, 0 );
		m_bEnable = bEnable;
	}

	//--------------------------------------------------------------------------
	void XUI_Wnd::BringToUp()	// 上移一层
	//--------------------------------------------------------------------------
	{
		if( m_pParent )
		{
			int i = m_pParent->FindChild( this );
			if( i > 0 )
				std::swap( m_pParent->m_pChildren[i], m_pParent->m_pChildren[i-1] );
		}
	}

	//--------------------------------------------------------------------------
	void XUI_Wnd::BringToDown() // 下移一层
	//--------------------------------------------------------------------------
	{
		if( m_pParent )
		{
			int i = m_pParent->FindChild( this );
			if( i < ( int )m_pParent->m_pChildren.size() )
				std::swap( m_pParent->m_pChildren[i], m_pParent->m_pChildren[i+1] );
		}
	}

	void XUI_Wnd::BringToFront()
	{
		//将控件移到最后
		if (m_pParent)
		{
			int i=m_pParent->FindChild(this);
			if (i>=0)
			{
				UINT j;
				for (j=i; j<m_pParent->m_pChildren.size()-1; j++)
					m_pParent->m_pChildren[j]=m_pParent->m_pChildren[j+1];
				m_pParent->m_pChildren[j]=this;
			}
		}
	}

	void XUI_Wnd::BringToEnd()
	{
		if( m_pParent )
		{
			int i = m_pParent->FindChild( this );
			if( i > 0 )
			{
				int j;
				for( j = i; j > 0; --j)
					m_pParent->m_pChildren[j]=m_pParent->m_pChildren[j-1];
				m_pParent->m_pChildren[j]=this;
			}
		}
	}

	//设置控件的焦点
	void XUI_Wnd::SetFocus(bool bFocused)
	{
		//触发事件
		if (bFocused && !m_bFocused)
		{
			SendMessage( WM_COMMAND, MAKELONG( GetID(), EV_SETFOCUS ), 0 );
			onGetFocus();
		}
		if (!bFocused && m_bFocused)
		{
			SendMessage( WM_COMMAND, MAKELONG( GetID(), EV_KILLFOCUS ), 0 );
			onLostFocus();
		}
		m_bFocused=bFocused;
	}

	BOOL XUI_Wnd::IsPointIn(const CPoint& pt)
	{
		return m_WindowRect.PtInRect( pt );
	}

	void XUI_Wnd::Render(const CRect& clipper)
	{
		if (!m_bVisible)		return;

		//计算可见区域
		CRect clpSelf( m_WindowRect );
		CPoint pt( 0, 0 );
		if( m_pParent )		pt = m_pParent->ClientToScreen( pt );
		clpSelf += pt;

		if( clpSelf.IntersectRect( clpSelf, clipper ) )
		{
			//绘制自己
			if( m_bVisible )
			{
				if( m_bOwnerDraw )
				{
					DRAWSTRUCT ds;
					ds.rcClient		= m_WindowRect;
					ds.rcClipper	= clpSelf;
					ds.pCtrl		= this;
					SendMessage( UI_OWNERDRAW, GetID(), (LPARAM)&ds );
				}
				else
					RenderSelf(clpSelf);
			}

			//绘制子控件
			for (UINT i=0; i<m_pChildren.size(); i++)
			{
				m_pChildren[i]->Render(clpSelf);
			}
		}
	}

	// 消息处理，在垂直层次上遍历消息映射表
	bool XUI_Wnd::OnWndMsg( UINT nMsg, WPARAM wParam, LPARAM lParam )
	{
		HRESULT lResult = 0;
		for( size_t i = 0; i < m_pListeners.size(); ++i )
		{
			XUI_Base* pTarget = m_pListeners[i];
			if( pTarget == m_pParent && !m_bTranslateParent ) continue;
			if( pTarget->DefMsgProc( nMsg, wParam, lParam ) )
			{
				return TRUE;
			}
		}
		if( nMsg == WM_COMMAND )
		{
			return OnCommand( wParam, lParam );
		}
		else if( nMsg == WM_NOTIFY )
		{
			return OnNotify( wParam, lParam, &lResult );
		}
		else
		{
			// 其他控件消息
			CONST UI_MSGMAP* pMessageMap = NULL;
			for( pMessageMap = GetMessageMap(); pMessageMap != NULL; pMessageMap = pMessageMap->pBaseMap )
			{
				CONST UI_MSGMAP_ENTRY* lpEntry = pMessageMap->lpEntry;
				while( lpEntry->nSig != uiSig_end )
				{
					if( lpEntry->nMessage == nMsg )
					{
						union	pfnSig	mmf;
						mmf.pfnNormal = lpEntry->pfn;
						switch( lpEntry->nSig )
						{
						case uiSig_vv:
							( this->*mmf.pfn_vv )();
							break;
						case uiSig_vw:
							( this->*mmf.pfn_vw )( UINT( wParam ) );
							break;
						case uiSig_b_wl:
							( this->*mmf.pfn_b_wl )( wParam, lParam );
							break;
						case uiSig_vwp:
							{
								CPoint point( (DWORD) lParam );
								( this->*mmf.pfn_vwp )( ( UINT )wParam, point );
							}
							break;
						case uiSig_v_v_ii:
							( this->*mmf.pfn_v_v_ii)( LOWORD(lParam), HIWORD(lParam) );
							break;
						}
						return TRUE;
					}
					++lpEntry;
				}
			}
		}

		return FALSE;
	}

	// 处理控件消息
	bool XUI_Wnd::OnCommand( WPARAM wParam, LPARAM lParam )
	{
		UINT nID = LOWORD(wParam);
		XUI_Wnd* pCtrl = (XUI_Wnd*)lParam;
		int nCode = HIWORD(wParam);
		// UPDATE_COMMAND_UI
		// 菜单等单一事件对象的更新消息。
		if( pCtrl )
		{
			OnCmdMsg( nID, UPDATE_COMMAND_UI, NULL, NULL );
			nCode = NC_COMMAND;
		}
		else
		{
			// 反射消息
			// ReflectMsg();
		}

		return OnCmdMsg( nID, nCode, NULL, NULL );
	}

	// 处理自定义通知消息
	bool XUI_Wnd::OnNotify( WPARAM, LPARAM lParam, HRESULT* lResult )
	{
		if( lParam == 0 )	return FALSE;
		NMUIHDR* pNMHDR = ( NMUIHDR* )lParam;
		if( pNMHDR->pCtrl == NULL )	return FALSE;

		int nID = pNMHDR->pCtrl->GetID();
		int nCode = pNMHDR->code;
		// 调用子控件的消息处理
		for( UINT i=0; i < m_pChildren.size(); i++ )
		{
			m_pChildren[i]->OnCmdMsg( nID, MAKELONG( nCode, WM_NOTIFY ), (void*)pNMHDR, NULL );
		}
		return	OnCmdMsg( nID, MAKELONG( nCode, WM_NOTIFY ), (void*)pNMHDR, NULL );
	}

	unsigned int XUI_Wnd::SendMessage( UINT nMsg, WPARAM wParam, LPARAM lParam )
	{
		OnWndMsg( nMsg, wParam, lParam );
		return 0;
	}

	bool XUI_Wnd::save_file( TiXmlElement* pNode )
	{
		ClassInfo* c = Manager::getInstance().getClass( typeid( *this ) );
		if( c )
		{
			LuaCall< void( XUI_Wnd*, const char*, TiXmlElement* ) > op( Lua::Instance(), "save" );
			std::list< const char* > l;
			c->getMembers( l );
			std::list< const char* >::iterator i = l.begin();
			while( i != l.end() )
			{
				try
				{
					OnSavePropertys( *i, pNode );
					op( this, *i, pNode );
				}
				catch( std::runtime_error& err )
				{
					OutputDebugStringA( err.what() );
					return false;
				}
				++i;
			}
		}
		return true;
	}

	bool XUI_Wnd::load_file( TiXmlElement* pNode )
	{
		ClassInfo* c = Manager::getInstance().getClass( typeid( *this ) );
		if( c )
		{
			LuaCall< void( XUI_Wnd*, const char*, TiXmlElement* ) > op( Lua::Instance(), "load" );
			std::list< const char* > l;
			c->getMembers( l );
			std::list< const char* >::iterator i = l.begin();
			while( i != l.end() )
			{
				try
				{
					TiXmlElement* pChild = pNode->FirstChildElement( *i );
					op( this, *i, pChild );
					OnLoadPropertys( *i, pChild );
				}
				catch( std::runtime_error& err )
				{
					OutputDebugStringA( err.what() );
					return false;
				}
				++i;
			}
		}
		return true;
	}

	// 保存,载入
	void XUI_Wnd::OnSavePropertys( const char* name, TiXmlElement* pNode )
	{
		if( strcmp( name, "window" ) == 0 )
		{
			pNode->SetAttribute( "left",	m_WindowRect.left );
			pNode->SetAttribute( "top",		m_WindowRect.top );
			pNode->SetAttribute( "right",	m_WindowRect.right );
			pNode->SetAttribute( "bottom",	m_WindowRect.bottom );
		}
	}

	void XUI_Wnd::OnLoadPropertys( const char* name, TiXmlElement* pNode )
	{
		if( strcmp( "window", name ) == 0 )
		{
			OnMoveWindow( m_WindowRect );
		}
	}

};

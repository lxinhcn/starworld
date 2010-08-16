#include "XUI_Header.h"
#include "XUI_Wnd.h"
#include "XUI_System.h"
#include "XUI_LuaBinder.h"

#define HANDLE_EDGE	2
namespace XGC
{
	namespace ui
	{
		/*******************************************************
		XUI_Wnd
		*******************************************************/
		XUI_Wnd::XUI_Wnd(void)
		: m_pParent(NULL)
		, m_bFocused(false)
		, m_bVisible( true )
		, m_bEnable( true )
		, m_bOwnerDraw( false )
		, m_pFont( NULL )
		, m_pBackGround( NULL )
		, m_WindowPosition( 0, 0 )
		, m_WindowSize( 100, 100 )
		{
			memset( m_Flags, 0, sizeof(m_Flags ) );
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
			SAFE_DELETE( m_pBackGround );
		}

		void XUI_Wnd::Release()
		{
			delete this;
		}

		//将一个坐标从控件坐标转换成屏幕坐标
		void XUI_Wnd::ClientToScreen( iPoint& pt )const
		{
			for( XUI_Wnd* pParent = m_pParent; pParent; pParent = pParent->GetParent() )
			{
				pt += pParent->GetWindowPosition();
			}
		}

		void XUI_Wnd::ClientToScreen( iRect& rc )const
		{
			for( XUI_Wnd* pParent = m_pParent; pParent; pParent = pParent->GetParent() )
			{
				rc += pParent->GetWindowPosition();
			}
		}

		//将一个坐标从屏幕坐标转换成控件坐标
		void XUI_Wnd::ScreenToClient( iPoint& pt )const
		{
			for( XUI_Wnd* pParent = m_pParent; pParent; pParent = pParent->GetParent() )
			{
				pt -= pParent->GetWindowPosition();
			}
		}

		void XUI_Wnd::ScreenToClient( iRect& rc )const
		{
			for( XUI_Wnd* pParent = m_pParent; pParent; pParent = pParent->GetParent() )
			{
				rc -= pParent->GetWindowPosition();
			}
		}

		int XUI_Wnd::FindChild(XUI_Wnd* pElement) const
		{
			for( _uint32 i = 0; i < m_pChildren.size(); i++ )
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
			}
		}

		//将控件加入为子控件，并修改其坐标
		void XUI_Wnd::AddChildAt(XUI_Wnd* pElement, int x, int y)
		{
			if (pElement)
			{
				if (pElement->m_WindowPosition.x != 0 || pElement->m_WindowPosition.y != 0 )
				{
					pElement->Offset(-pElement->m_WindowPosition.x, -pElement->m_WindowPosition.y );
				}
				pElement->Offset( x, y );
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
				//if( m_pChildMouseOver == pChild )	m_pChildMouseOver = NULL;
				//if( m_pChildFocusedOn == pChild )	m_pChildFocusedOn = NULL;

				if( bDestroy ) pChild->Release();
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
			//m_pChildMouseOver = NULL;
			//m_pChildFocusedOn = NULL;
		}

		//寻找在某个坐标上的控件
		XUI_Wnd* XUI_Wnd::FindChildInPoint(const iPoint &pt )
		{
			if( !IsVisible() )
				return NULL;

			iPoint adjust( pt );
			AdjustPoint( adjust, true );
			for( size_t i = 0; i < m_pChildren.size(); ++i )
			{
				XUI_Wnd* pElement = m_pChildren[i];
				if( pElement->IsPointIn(adjust) )
					return pElement->FindChildInPoint( adjust - m_WindowPosition );
			}

			return this;
		}

		//--------------------------------------------------------//
		//	created:	19:11:2009   18:29
		//	filename: 	d:\Develop\StarGame\GameSDK\GameUILib\Source\XUI_Wnd.cpp
		//	author:		Albert.xu
		//
		//	purpose:	查找能包容整个矩形的最深Wnd
		//--------------------------------------------------------//
		bool XUI_Wnd::FindRectIn( const iRect &rc, std::list< XUI_Wnd* >& l )
		{
			iRect rcAdjust(rc);
			AdjustWindow( rcAdjust, true);

			iRect rcWindow = GetWindowRect();
			if( rcWindow.PtInRect( rc.TopLeft() ) && rcWindow.PtInRect( rc.BottomRight() ) )
			{
				for( size_t i = 0; i < m_pChildren.size(); ++i )
				{
					XUI_Wnd* pElement = m_pChildren[i];
					iRect rcChildren = pElement->GetWindowRect();
					if( rc.PtInRect( rcChildren.TopLeft() ) && rc.PtInRect( rcChildren.BottomRight() ) )
					{
						l.push_back( pElement );
					}
					else if( pElement->FindRectIn( rcAdjust - m_WindowPosition, l ) )
					{
						return true;
					}
				}
				return true;
			}
			return false;
		}

		XUI_Wnd* XUI_Wnd::FindChildByName( const _string& sName) const
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
				if ( pElement->GetID() == nID )
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
		void XUI_Wnd::MoveWindow( int left, int top, int right, int bottom, bool notify )
		{
			m_WindowPosition = iPoint( left, top );
			m_WindowSize = iSize( abs(right - left), abs(bottom - top) );
			if( notify )
			{
				// 发送位置变更消息
				OnMoveWindow( iRect( left, top, right, bottom ) );
			}
		}

		void XUI_Wnd::Offset( int x, int y )
		{
			m_WindowPosition.x += x;
			m_WindowPosition.y += y;
		}

		void XUI_Wnd::ShowWindow( bool bVisible /* = true  */ )
		{
			m_bVisible=bVisible;
		}

		void XUI_Wnd::EnableWindow( bool bEnable )
		{
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
					_uint32 j;
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
				OnSetFocus();
			}
			if (!bFocused && m_bFocused)
			{
				OnLostFocus();
			}
			m_bFocused=bFocused;
		}

		void XUI_Wnd::SetFlags( _uint16 nFlag )
		{
			if( nFlag >= sizeof(m_Flags)*8 )
				return;

			m_Flags[nFlag/8] |= (1<<(nFlag%8));
		}

		void XUI_Wnd::ClrFlags( _uint16 nFlag )
		{
			if( nFlag >= sizeof(m_Flags)*8 )
				return;

			m_Flags[nFlag/8] &= ~(1<<(nFlag%8));
		}

		bool XUI_Wnd::GetFlags( _uint16 nFlag )const
		{
			if( nFlag >= sizeof(m_Flags)*8 )
				return false;

			return (m_Flags[nFlag/8] & (1<<(nFlag%8))) != 0;
		}

		bool XUI_Wnd::IsPointIn(const iPoint& pt)
		{
			return 
				pt.x >= m_WindowPosition.x && 
				pt.x < m_WindowPosition.x + m_WindowSize.cx &&
				pt.y >= m_WindowPosition.y && 
				pt.y < m_WindowPosition.y + m_WindowSize.cy;
		}

		void XUI_Wnd::Render(const iRect& clipper)
		{
			if (!m_bVisible)
				return;

			iRect rcWindow = GetWindowRect();
			ClientToScreen( rcWindow );
			// XUI_SetClipping( rcWindow.left, rcWindow.top, rcWindow.Width(), rcWindow.Height() );
			//绘制自己
			if( m_bOwnerDraw )
			{
			}
			else
			{
				RenderSelf( clipper.TopLeft() );
				if( GetFlags(FLAGS_EDIT) )
				{
					// XUI_SetClipping( rcWindow.left-HANDLE_EDGE, rcWindow.top-HANDLE_EDGE, rcWindow.Width()+HANDLE_EDGE*2, rcWindow.Height()+HANDLE_EDGE*2 );
					RenderEdit( clipper.TopLeft() );
				}
			}

			//绘制子控件
			for (_uint32 i=0; i<m_pChildren.size(); i++)
			{
				m_pChildren[i]->Render(rcWindow);
			}
		}

		void XUI_Wnd::Update( float timer, float delta )
		{
			try
			{
				SLB::LuaCall< void( XUI_Wnd*, float, float ) > LuaUpdate( Lua::Instance(), m_strUpdateFunc.c_str() );
				if( LuaUpdate.valid() )
				{
					LuaUpdate( this, timer, delta );
				}
			}
			catch( std::runtime_error& err )
			{
				puts( err.what() );
			}

			//绘制子控件
			for (_uint32 i=0; i<m_pChildren.size(); i++)
			{
				m_pChildren[i]->Update( timer, delta );
			}
		}

		void XUI_Wnd::RenderSelf( const iPoint& adjust )
		{
			if( m_pBackGround )
			{
				XUI_DrawSprite( m_pBackGround, m_WindowPosition.x - adjust.x, m_WindowPosition.y - adjust.y, m_WindowSize.cx, m_WindowSize.cy );
			}
		}

		void XUI_Wnd::RenderEdit( const iPoint &adjust )
		{
			iRect rc = GetWindowRect() + adjust;

			XUI_DrawRect( rc, XUI_ARGB(0xff,0,0,0), XUI_ARGB(0x40,80,80,80) );

			iRect rch( -HANDLE_EDGE, -HANDLE_EDGE, HANDLE_EDGE, HANDLE_EDGE );
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

		bool XUI_Wnd::save_file( TiXmlElement* pNode )
		{
			SLB::ClassInfo* c = SLB::Manager::getInstance().getClass( typeid( *this ) );
			if( c )
			{
				SLB::LuaCall< void( XUI_Wnd*, const char*, TiXmlElement* ) > op( Lua::Instance(), "save" );
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
			SLB::ClassInfo* c = SLB::Manager::getInstance().getClass( typeid( *this ) );
			if( c )
			{
				SLB::LuaCall< void( XUI_Wnd*, const char*, TiXmlElement* ) > op( Lua::Instance(), "load" );
				std::list< const char* > l;
				c->getMembers( l );
				std::list< const char* >::iterator i = l.begin();
				while( i != l.end() )
				{
					try
					{
						TiXmlElement* pChild = pNode->FirstChildElement( *i );
						if( pChild )
						{
							op( this, *i, pChild );
							OnLoadPropertys( *i, pChild );
						}
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

		void XUI_Wnd::show_members( int indent )
		{
			SLB::ClassInfo* c = SLB::Manager::getInstance().getClass( typeid( *this ) );
			if( c )
			{
				SLB::LuaCall< const char*( const XUI_Wnd*, const char*, int ) > op( Lua::Instance(), "class_attrib" );
				std::list< const char* > l;
				c->getMembers( l );
				std::list< const char* >::iterator i = l.begin();
				while( i != l.end() )
				{
					try
					{
						_putch( '\n' );
						for( int n = 0; n < indent; ++n ) printf( "  " );
						printf( "|-> %s", *i);
						const char* attrib = op( this, *i, indent );
						if( attrib )
						{
							printf( " = %s", attrib );
						}
					}
					catch( std::runtime_error& err )
					{
						OutputDebugStringA( err.what() );
					}
					++i;
				}
			}
			_putch( '\n' );
		}

		// 保存,载入
		void XUI_Wnd::OnSavePropertys( const char* name, TiXmlElement* pNode )
		{
		}

		void XUI_Wnd::OnLoadPropertys( const char* name, TiXmlElement* pNode )
		{
			if( strcmp( "window", name ) == 0 )
			{
				OnMoveWindow( GetWindowRect() );
			}
		}
	}
};

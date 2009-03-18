#include "GuiHeader.h"
#include "GuiSystem.h"
#include "XUI_EditBox.h"

namespace UILib
{
	CEditBuffer::CEditBuffer()
	{
	}

	CEditBuffer::~CEditBuffer()
	{

	}

	//---------------------------------------------------------------------//
	// describe	: 调整缓冲大小
	// return	: 是否成功
	//---------------------------------------------------------------------//
	bool CEditBuffer::increase( size_t inc )
	{
		size_t alloc = ( inc == -1 || inc < 1024 )?( m_capacity?m_capacity+1024:1024 ):m_capacity + inc;
		m_buffer = (unit*)realloc( m_buffer, alloc*sizeof(m_buffer[0]) );
		memset( m_buffer + m_capacity, 0, ( alloc - m_capacity )*sizeof(unit) );
		m_capacity = alloc;
		return true;
	}

	void CEditBuffer::insert( unsigned short pos, unsigned short c )
	{
		unsigned short width = m_pFont->GetCharacterWidth( c );
		if( m_size >= m_capacity )
		{
			increase( -1 );
		}

		if( pos + width >= m_capacity )
		{
			if( increase( m_capacity - pos - width + 1 ) == false ) return;
		}
		else if( pos + width >= m_size )
		{
			m_size = pos + width;
		}
		else
		{
			for( unsigned short p = pos; p < m_size - pos; ++p )
			{
				m_buffer[p + width] = m_buffer[p];
			}
			m_size += width;
		}
		m_buffer[pos].ch = c;
		m_buffer[pos].width = width;
	}

	void CEditBuffer::insert( unsigned short pos, const char* c )
	{
		const wchar_t* w = XA2T( c );
		insert( pos, w );
	}

	void CEditBuffer::insert( unsigned short pos, const wchar_t* c )
	{
		if( pos == -1 )
		{
			pos = m_size;
		}

		unsigned short width = 0;
		const wchar_t *i = c;
		while( i )
		{
			width += m_pFont->GetCharacterWidth( *i );
			++i;
		}
	}

	BEGIN_UIMSG_MAP( XUI_EditBox, XUI_Wnd )
	END_UIMSG_MAP()

	XUI_EditBox::XUI_EditBox(void)
	: UIObjTypeT< XUI_Wnd, TypeEditBox >()
	, m_bControl( false )
	, m_bShift( false )
	, m_CaratPos( 0 )
	, m_FirstLineNumber( 0 )
	, m_nCurLineNumber( 0 )
	, m_bShowCarat( true )
	{
		XUI_IFont* pFont = m_pFont?m_pFont:GuiSystem::Instance().GetDefaultFont();
		if( pFont )
		{
			m_WindowSize.cx		= m_wndRect.Width()/pFont->GetCharacterWidth( _T(' ') );
			m_WindowSize.cy		= m_wndRect.Height()/pFont->GetCharacterHeight();
		}

		m_CaratTimerHandler = GuiSystem::Instance().SetTimer( event_function( this, &XUI_EditBox::CaratTimerUpdate ), 1, TIMER_SECOND(0.5f) );
	}

	XUI_EditBox::~XUI_EditBox(void)
	{
		GuiSystem::Instance().KillTimer( m_CaratTimerHandler );
	}

	bool XUI_EditBox::CaratTimerUpdate( unsigned int handle, unsigned short& repeat, unsigned int& timer )
	{
		m_bShowCarat = !m_bShowCarat;
		repeat = 1;
		return true;
	}

	void XUI_EditBox::RenderCharacter( TCHAR szChar, XUI_IFont* pFont, LONG &x, LONG &y, BOOL bRender )
	{
		if( bRender )
		{
			XUI_DrawCharacter( szChar, pFont, (float)x, (float)y );
		}

		x += pFont->GetCharacterWidth( szChar );
	}

	//重绘，通过实现这个方法来表现空间的外观
	void XUI_EditBox::RenderSelf(const CRect& clipper)
	{
		CPoint pt = m_wndRect.TopLeft();
		AdjustPoint( pt, true );

		XUI_DrawRect( m_wndRect, 0xff00ff00, 0xff00ff00 );

		CRect rc;
		rc.IntersectRect( m_wndRect, clipper );

		CPoint CharPos = pt;
		XUI_IFont* pFont = m_pFont?m_pFont:GuiSystem::Instance().GetDefaultFont();

		_string::size_type begin	= m_LineRecorder.empty()?0:m_LineRecorder[m_FirstLineNumber]+1;
		_string::size_type end		= m_LineRecorder.empty()?m_strText.length():(m_FirstLineNumber<m_LineRecorder.size()?m_LineRecorder[m_FirstLineNumber+1]+1:m_strText.length());

		for( size_t i = begin; i <= end; ++i )
		{
			TCHAR c = m_strText[i];
			if( pFont->GetCharacterWidth( c ) + CharPos.x > rc.right )
			{
				// 字符显示超出宽度，则折行显示
				CharPos.x = rc.left;
				CharPos.y += pFont->GetCharacterHeight();
				if( CharPos.y > rc.bottom - pFont->GetCharacterHeight() )
				{
					return;
				}
			}

			BOOL bRender = rc.PtInRect( CharPos + CPoint( pFont->GetCharacterWidth( c ), pFont->GetCharacterHeight() ) );
			if( _istprint( c ) )
			{
				RenderCharacter( c, pFont, CharPos.x, CharPos.y, bRender );
			}
			else
			{
				switch( c )
				{
				case '\t':
					{
						size_t CharCount = i - begin;
						size_t NextTab = CharPos.x + (4 - CharCount%4)*pFont->GetCharacterWidth( _T(' ') );
						if( (long)NextTab > m_wndRect.right ) NextTab = m_wndRect.right;
						while( CharPos.x < (long)NextTab )
						{
							RenderCharacter( _T(' '), pFont, CharPos.x, CharPos.y, bRender );
						}
					}
					break;
				case '\n':
					{
						CharPos.x = 0;
						CharPos.y += pFont->GetCharacterHeight();
						if( CharPos.y > m_wndRect.bottom - pFont->GetCharacterHeight() )
						{
							return;
						}
					}
					break;
				case '\0':
					RenderCharacter( _T(' '), pFont, CharPos.x, CharPos.y, bRender );
					break;
				default:
					RenderCharacter( _T('?'), pFont, CharPos.x, CharPos.y, bRender );
					break;
				}
			}

			if( i == m_CaratPos && m_bShowCarat )
			{
				long x = CharPos.x - long( pFont->GetCharacterWidth( _T('|') )*1.5f );
				long y = CharPos.y;
				RenderCharacter( _T('|'), pFont, x, y, bRender );
			}
		}
	}

	//鼠标
	//鼠标移动事件
	//参数说明：
	//pt，鼠标的坐标，相对于控件
	//sysKeys，各种重要按键的状态，参见MSDN	
	bool XUI_EditBox::onMouseMove(const CPoint& pt, UINT sysKeys)
	{
		return true;
	}

	bool XUI_EditBox::onMouseHover(const CPoint& pt)
	{
		return true;
	}

	bool XUI_EditBox::onMouseEnter()
	{
		return true;
	}

	bool XUI_EditBox::onMouseLeave()
	{
		return true;
	}

	//鼠标按键事件
	//参数说明：
	//button，按下的键，0-左键，1-右键，2-中键
	//pt，鼠标的坐标
	//sysKeys，各种重要按键的状态，参见MSDN
	bool XUI_EditBox::onButtonDown(int button, const CPoint& pt, UINT sysKeys)
	{
		return true;
	}

	bool XUI_EditBox::onButtonUp(int button, const CPoint& pt, UINT sysKeys)
	{
		return true;
	}

	//键盘
	//参数说明
	//keycode，按下的键
	//sysKeys，各种重要按键的状态，参见MSDN
	bool XUI_EditBox::onKeyDown(DWORD keycode, UINT sysKeys)
	{
		switch( keycode )
		{
		case VK_CONTROL:
			m_bControl = true;
			break;
		case VK_SHIFT:
		case VK_RSHIFT:
			m_bShift = true;
			break;
		case VK_BACK:
			HandleBack( sysKeys );
			break;
		case VK_DELETE:
			HandleDelete( sysKeys );
			break;
		case VK_HOME:
			HandleHome( sysKeys );
			break;
		case VK_END:
			HandleEnd( sysKeys );
			break;
		case VK_LEFT:
			if( m_bControl )
				HandleWordLeft( sysKeys );
			else
				HandleCharLeft( sysKeys );
			break;
		case VK_RIGHT:
			if( m_bControl )
				HandleWordRight( sysKeys );
			else
				HandleCharRight( sysKeys );
			break;
		case VK_RETURN:
			m_strText.insert( m_CaratPos++, 1, _T('\n') );
			HandleReturn( sysKeys );
			break;
		}
		return true;
	}

	void XUI_EditBox::DeleteCharacter( size_t nPos )
	{
		if( m_strText[nPos] == _T('\n') )
		{
			// 从第一个可见行开始查找
			line_recorder::iterator i = m_LineRecorder.begin() + ( m_LineRecorder.empty()?0:m_FirstLineNumber );
			while( i != m_LineRecorder.end() )
			{
				if( *i == nPos )
				{
					m_LineRecorder.erase( i );
					break;
				}
				++i;
			}
		}
		m_strText.erase( nPos, 1 );
	}

	void XUI_EditBox::HandleBack( UINT nSysKey )
	{
		if( m_CaratPos <= 0 ) return;
		DeleteCharacter( --m_CaratPos );
	}

	void XUI_EditBox::HandleDelete( UINT nSysKey )
	{
		if( m_CaratPos >= m_strText.length() ) return;
		DeleteCharacter( m_CaratPos );
	}

	void XUI_EditBox::HandleHome( UINT nSysKey )
	{
		while( m_CaratPos > 0 && m_strText[m_CaratPos] != _T('\n') ) --m_CaratPos;
	}

	void XUI_EditBox::HandleEnd( UINT nSysKey )
	{
		int c = m_strText[m_CaratPos];
		while( m_CaratPos < m_strText.length() && c != _T('\n') && c != _T('\0') ) ++m_CaratPos;
	}

	void XUI_EditBox::HandleWordLeft( UINT nSysKey )
	{
		while( m_CaratPos > 0 && m_strText[m_CaratPos] != _T(' ') ) --m_CaratPos;
	}

	void XUI_EditBox::HandleCharLeft( UINT nSysKey )
	{
		if( m_CaratPos > 0 ) --m_CaratPos; 
	}

	void XUI_EditBox::HandleWordRight( UINT nSysKey )
	{
		while( m_CaratPos < m_strText.length() && m_strText[m_CaratPos] != _T(' ') ) ++m_CaratPos;
	}

	void XUI_EditBox::HandleCharRight( UINT nSysKey )
	{
		if( m_CaratPos < m_strText.length() ) ++m_CaratPos;
	}

	void XUI_EditBox::HandleReturn( UINT nSysKey )
	{
		m_LineRecorder.insert( m_nCurLineNumber, m_CaratPos );
		++m_nCurLineNumber;
		++m_FirstLineNumber;
	}

	bool XUI_EditBox::onKeyUp( DWORD keycode, UINT sysKeys )
	{
		switch( keycode )
		{
		case VK_CONTROL:
			m_bControl = false;
			break;
		case VK_SHIFT:
		case VK_RSHIFT:
			m_bShift = false;
			break;
		}
		return true;
	}

	//参数说明
	//c，输入的字符
	//sysKeys，各种重要按键的状态，参见MSDN
	bool XUI_EditBox::onChar(DWORD c, UINT sysKeys)
	{
		if( _istprint( LOWORD(c) ) )
		{
			m_strText.insert( m_CaratPos++, 1, (wchar_t)c );
		}
		return true;
	}

	//输入法
	//参见MSDN
	bool XUI_EditBox::onImeComp(DWORD wParam, DWORD lParam)
	{
		return true;
	}

	bool XUI_EditBox::onImeEndComp(DWORD wParam, DWORD lParam)
	{
		return true;
	}

	bool XUI_EditBox::onImeNotify(DWORD wParam, DWORD lParam)
	{
		return true;
	}

	unsigned int XUI_EditBox::OnMoveWindow( CRect& rcWindow )
	{
		XUI_IFont* pFont = m_pFont?m_pFont:GuiSystem::Instance().GetDefaultFont();
		if( pFont )
		{
			m_WindowSize.cx		= m_wndRect.Width()/pFont->GetCharacterWidth( _T(' ') );
			m_WindowSize.cy		= m_wndRect.Height()/pFont->GetCharacterHeight();
		}
		return 0;
	}

	//---------------------------------------------------------------------//
	// describe	: 根据字符位置获得行号
	// nPos		: 字符索引
	// return	: 行号
	//---------------------------------------------------------------------//
	size_t XUI_EditBox::GetLineFromCharaterPos( size_t nPos )const
	{
		struct _compare
		{
			explicit _compare( size_t _nPos ) 
				: nPos( _nPos )
			{
			}

			bool operator()( size_t nLineIndex )
			{
				return nPos < nLineIndex;
			}

			const size_t nPos;
		};
		
		line_recorder::const_iterator i = std::find_if( m_LineRecorder.begin(), m_LineRecorder.end(), _compare( nPos ) );
		if( i != m_LineRecorder.end() )
		{
			return i - m_LineRecorder.begin();
		}

		return -1;
	}

}
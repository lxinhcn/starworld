#include "GuiHeader.h"
#include "GuiSystem.h"
#include "XUI_EditBox.h"

namespace UILib
{
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
			m_WindowSize.cx		= m_WindowRect.Width()/pFont->GetCharacterWidth( _T(' ') );
			m_WindowSize.cy		= m_WindowRect.Height()/pFont->GetCharacterHeight();
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

	void XUI_EditBox::SetText( const std::string &t )
	{
		m_text.clear();
		std::string::size_type begin = 0, end = 0;
		while( ( end = t.find( _T('\n'), begin ) ) != std::string::npos )
		{
			m_text.push_back( XA2T( t.substr( begin, end ) ) );
			++begin;
		}
	}

	std::string XUI_EditBox::GetText()const
	{
		std::string t;
		text::const_iterator i = m_text.begin();
		while( i != m_text.end() )
		{
			t.append( XT2A( *i ) );
			++i;
		}

		return t;
	}

	void XUI_EditBox::RenderCharacter( _tchar szChar, XUI_IFont* pFont, LONG &x, LONG &y, BOOL bRender )
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
		CPoint pt = m_WindowRect.TopLeft();
		AdjustPoint( pt, true );

		XUI_DrawRect( m_WindowRect, 0xff00ff00, 0xff00ff00 );

		CRect rc;
		rc.IntersectRect( m_WindowRect, clipper );

		CPoint CharPos = pt;
		XUI_IFont* pFont = m_pFont?m_pFont:GuiSystem::Instance().GetDefaultFont();

		for( Position i = m_FirstLineNumber; i < m_text.size(); ++i )
		{
			line& l = m_text[i];
			end_type t = l.type;

			if( CharPos.y > rc.bottom - pFont->GetCharacterHeight() )
			{
				// 超出高度则跳出
				break;
			}

			for( size_t cursor = 0; cursor < l.size(); ++cursor )
			{
				_tchar c = l[cursor];

				if( pFont->GetCharacterWidth( c ) + CharPos.x > rc.right )
				{
					if( m_bWarpText )
					{
						// 字符显示超出宽度，则折行显示
						// 将没有画的作为独立的串添加到下一行
						m_text.insert( m_text.begin() + i + 1, line( l.substr( cursor, -1 ), type_r ) );
						SetCurLineNumber( m_nCurLineNumber + 1 );
						break;
					}
					else
					{
						// 不折行从下一行开始
						CharPos.x = pt.x;
						break;
					}
				}

				// 判断是否被绘制。
				BOOL bRender = rc.PtInRect( CharPos + CPoint( pFont->GetCharacterWidth( c ), pFont->GetCharacterHeight() ) );
				if( _istprint( c ) )
				{
					// 是显示字符
					RenderCharacter( c, pFont, CharPos.x, CharPos.y, bRender );
				}
				else if( c == _T('\n') )
				{
					break;
				}
				else
				{
					switch( c )
					{
					case '\t':
						{
							// 制表对齐
							size_t NextTab = CharPos.x + (4 - cursor%4)*pFont->GetCharacterWidth( _T(' ') );
							if( (long)NextTab > pt.x + m_WindowRect.Width() ) NextTab = pt.x + m_WindowRect.Width();
							while( CharPos.x < (long)NextTab )
							{
								RenderCharacter( _T(' '), pFont, CharPos.x, CharPos.y, bRender );
							}
						}
						break;
					case '\0':
						// 为绘制光标占位置。
						RenderCharacter( _T(' '), pFont, CharPos.x, CharPos.y, bRender );
						break;
					default:
						RenderCharacter( _T('?'), pFont, CharPos.x, CharPos.y, bRender );
						break;
					}
				}

				if( cursor == m_CaratPos && m_bShowCarat )
				{
					// 是否绘制光标
					long x = CharPos.x - long( pFont->GetCharacterWidth( _T('|') )*1.5f );
					long y = CharPos.y;
					RenderCharacter( _T('|'), pFont, x, y, bRender );
				}

			}

			// 折行。
			CharPos.x = pt.x;
			CharPos.y += pFont->GetCharacterHeight();
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
	bool XUI_EditBox::onKeyDown(uint32 keycode, UINT sysKeys)
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
			HandleBack();
			break;
		case VK_DELETE:
			HandleDelete();
			break;
		case VK_HOME:
			HandleHome();
			break;
		case VK_END:
			HandleEnd();
			break;
		case VK_LEFT:
			if( m_bControl )
				HandleWordLeft();
			else
				HandleCharLeft();
			break;
		case VK_RIGHT:
			if( m_bControl )
				HandleWordRight();
			else
				HandleCharRight();
			break;
		case VK_UP:
			HandleLineUp();
			break;
		case VK_DOWN:
			HandleLineDown();
			break;
		case VK_RETURN:
			HandleReturn();
			break;
		}
		return true;
	}

	void XUI_EditBox::DeleteCharacter( size_t nLine, size_t nPos, size_t nCount )
	{
		Position n = nLine;
		Position p = nPos;
		while( nCount )
		{
			line& l = m_text.at( n );

			size_t del = __min( nCount, l.size() - m_CaratPos );
			l.erase( p, del );
			if( l.empty() )
			{
				// 空行则删除掉
				if( l.type == type_n )
				{
					line& ln = m_text.at( nLine );
					ln.type = type_n;
				}
				m_text.erase( m_text.begin() + n );
			}
			else
			{
				++n;
			}
			nCount -= del;
		}


	}

	void XUI_EditBox::HandleBack()
	{
		if( m_CaratPos <= 0 )
		{
			if( m_nCurLineNumber == 0 ) return;
			SetCurLineNumber( m_nCurLineNumber - 1 );
			m_CaratPos = m_text.at( m_nCurLineNumber ).size() - 1;
		}
		else
		{
			--m_CaratPos;
		}
		DeleteCharacter( m_nCurLineNumber, m_CaratPos, 1 );
	}

	void XUI_EditBox::HandleDelete()
	{
		DeleteCharacter( m_nCurLineNumber, m_CaratPos, 1 );
	}

	void XUI_EditBox::HandleHome()
	{
		m_CaratPos = 0;
	}

	void XUI_EditBox::HandleEnd()
	{
		m_CaratPos = m_text.at( m_nCurLineNumber ).size() - 1;
	}

	void XUI_EditBox::HandleWordLeft()
	{
		//while( m_CaratPos > 0 && m_strText[m_CaratPos] != _T(' ') )
		//{
		//	--m_CaratPos;
		//	if( m_nCurLineNumber > 0 && m_CaratPos < m_LineRecorder[m_nCurLineNumber-1] )
		//	{
		//		SetCurLineNumber( m_nCurLineNumber - 1 );
		//	}
		//}
	}

	void XUI_EditBox::HandleCharLeft()
	{
		if( m_CaratPos > 0 )
		{
			--m_CaratPos; 
		}
		else
		{
			SetCurLineNumber( m_nCurLineNumber - 1 );
			HandleEnd();
		}
	}

	void XUI_EditBox::HandleWordRight()
	{
		//while( m_CaratPos < m_strText.length() && m_strText[m_CaratPos] != _T(' ') )
		//{
		//	++m_CaratPos;
		//	if( m_nCurLineNumber < m_LineRecorder.size() && m_CaratPos >= m_LineRecorder[m_nCurLineNumber] )
		//	{
		//		SetCurLineNumber( m_nCurLineNumber + 1 );
		//	}
		//}
	}

	void XUI_EditBox::HandleCharRight()
	{
		line &l = m_text.at( m_nCurLineNumber );
		if( m_CaratPos < l.size() )
		{
			++m_CaratPos;
		}
		else
		{
			SetCurLineNumber( m_nCurLineNumber + 1 );
			HandleHome();
		}
	}

	void XUI_EditBox::HandleReturn()
	{
		m_text.insert( m_text.begin() + m_nCurLineNumber + 1, _T("") );
		m_CaratPos = 0;
		SetCurLineNumber( m_nCurLineNumber + 1 );
	}

	void XUI_EditBox::HandleLineUp()
	{
		if( m_nCurLineNumber > 0 )
		{
			SetCurLineNumber( m_nCurLineNumber - 1 );
		}
	}

	void XUI_EditBox::HandleLineDown()
	{
		SetCurLineNumber( m_nCurLineNumber + 1 );
	}

	bool XUI_EditBox::onKeyUp( uint32 keycode, UINT sysKeys )
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
	bool XUI_EditBox::onChar(uint32 c, UINT sysKeys)
	{
		if( _istprint( LOWORD(c) ) )
		{
			m_text.at(m_nCurLineNumber).insert( m_CaratPos++, 1, (wchar_t)c );
		}
		return true;
	}

	//输入法
	//参见MSDN
	bool XUI_EditBox::onImeComp(uint32 wParam, uint32 lParam)
	{
		return true;
	}

	bool XUI_EditBox::onImeEndComp(uint32 wParam, uint32 lParam)
	{
		return true;
	}

	bool XUI_EditBox::onImeNotify(uint32 wParam, uint32 lParam)
	{
		return true;
	}

	unsigned int XUI_EditBox::OnMoveWindow( CRect& rcWindow )
	{
		XUI_IFont* pFont = m_pFont?m_pFont:GuiSystem::Instance().GetDefaultFont();
		if( pFont )
		{
			m_WindowSize.cx		= m_WindowRect.Width()/pFont->GetCharacterWidth( _T(' ') );
			m_WindowSize.cy		= m_WindowRect.Height()/pFont->GetCharacterHeight();
		}
		Analyse();
		return 0;
	}

	//---------------------------------------------------------------------//
	// describe	: 分析当前串，刷新LineRecorder对象。
	//---------------------------------------------------------------------//
	void XUI_EditBox::Analyse()
	{

	}

	//---------------------------------------------------------------------//
	// describe	: 设置当前行
	// line		: 行索引
	//---------------------------------------------------------------------//
	void XUI_EditBox::SetCurLineNumber( size_t nLine )
	{
		if( nLine >= m_text.size() ) return;

		line &l = m_text.at( m_nCurLineNumber );
		l.cursor_position = m_CaratPos;

		m_nCurLineNumber = nLine;
		if( m_nCurLineNumber < m_FirstLineNumber )
		{
			m_FirstLineNumber = ( m_FirstLineNumber <= (size_t)m_WindowSize.cy?0:(m_FirstLineNumber - m_WindowSize.cy) );
		}
		else if( m_nCurLineNumber - m_FirstLineNumber >= (size_t)m_WindowSize.cy )
		{
			m_FirstLineNumber = m_nCurLineNumber - (size_t)m_WindowSize.cy + 1;
		}
	}
}
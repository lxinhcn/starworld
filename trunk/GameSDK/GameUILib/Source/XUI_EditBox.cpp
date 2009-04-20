#include "GuiHeader.h"
#include "GuiSystem.h"
#include "XUI_Window.h"
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
	, m_dwBorderColor( XUI_ARGB(0xff,0x80,0x80,0x80) )
	, m_dwBackgroundColor( XUI_ARGB(0x30,0x20,0x20,0x20) )
	{
		XUI_IFont* pFont = m_pFont?m_pFont:GuiSystem::Instance().GetDefaultFont();
		if( pFont )
		{
			m_WindowSize.cx		= m_WindowRect.Width()/pFont->GetCharacterWidth( _T(' ') );
			m_WindowSize.cy		= m_WindowRect.Height()/pFont->GetCharacterHeight();
		}

		m_CaratTimerHandler = GuiSystem::Instance().SetTimer( event_function( this, &XUI_EditBox::CaratTimerUpdate ), 1, TIMER_SECOND(0.5f) );
		m_text.push_back(_T(""));
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

	size_t XUI_EditBox::NaturalLine( size_t nLine )
	{
		if( nLine >= m_text.size() ) return 0;

		line& l = m_text[nLine];
		size_t ret = 0;
		text::iterator i = m_text.begin() + nLine + 1;
		while( l.type == type_r && i != m_text.end() )
		{
			l.append( *i );
			l.type = (*i).type;
			i = m_text.erase( i );
			++ret;
		}

		return ret;
	}

	void XUI_EditBox::SetText( const std::string &t )
	{
		m_text.clear();
		m_text.push_back(_T(""));
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
	void XUI_EditBox::RenderSelf()
	{
		x_point pt = m_WindowRect.TopLeft();
		AdjustPoint( pt, true );

		XUI_DrawRect( m_WindowRect, m_dwBorderColor, m_dwBackgroundColor );

		x_point CharPos = pt;
		x_point CaratPos;
		XUI_IFont* pFont = m_pFont?m_pFont:GuiSystem::Instance().GetDefaultFont();

		for( Position i = m_FirstLineNumber; i < m_text.size(); ++i )
		{
			line& l = m_text[i];
			end_type t = l.type;

			if( CharPos.y > m_WindowRect.bottom - pFont->GetCharacterHeight() )
			{
				// 超出高度则跳出
				break;
			}

			for( size_t cursor = 0; cursor <= l.size(); ++cursor )
			{
				_tchar c = l[cursor];

				if( pFont->GetCharacterWidth( c ) + CharPos.x > m_WindowRect.right )
				{
					if( m_bWarpText )
					{
						// 字符显示超出宽度，则折行显示
						// 将没有画的作为独立的串添加到下一行
						line ll( l.substr( cursor - 1, -1 ), l.type );
						l.erase( cursor - 1, -1 );
						l.type = type_r;
						l.cursor_position = l.size();

						size_t lsize = l.size();
						m_text.insert( m_text.begin() + i + 1, ll );

						// 根据光标位置判断是否要重新设定光标所在行。
						if( m_nCurLineNumber == i && m_CaratPos > lsize )
						{
							SetCurLineNumber( m_nCurLineNumber + 1 );
							m_CaratPos = cursor - lsize;
						}
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
				BOOL bRender = m_WindowRect.PtInRect( CharPos + x_point( pFont->GetCharacterWidth( c ), pFont->GetCharacterHeight() ) );
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

				if( i == m_nCurLineNumber && cursor == m_CaratPos )
				{
					long x = CaratPos.x = CharPos.x - long( pFont->GetCharacterWidth( _T('|') )*1.5f );
					long y = CaratPos.y = CharPos.y;
					if( m_bShowCarat )
					{
						// 是否绘制光标
						RenderCharacter( _T('|'), pFont, x, y, bRender );
					}
				}

			}

			// 折行。
			CharPos.x = pt.x;
			CharPos.y += pFont->GetCharacterHeight();
		}

		if( m_bFocused && !XUI_IME::m_Candlist.strBuffer.empty() )
		{
			XUI_Window* pWnd = GuiSystem::Instance().GetDesktop( DEFAULT_DESKTOP );
			const x_rect& rcWindow = pWnd->GetWindowRect();
			CaratPos.x = pFont->GetCharacterWidth( _T(' ') ) + ( (CaratPos.x + XUI_IME::m_Candlist.rcCandidate.Width() > rcWindow.Width())?rcWindow.Width()-XUI_IME::m_Candlist.rcCandidate.Width()-1:CaratPos.x);
			if( CaratPos.x < 0 ) CaratPos.x = 0;

			CaratPos.y = pFont->GetCharacterHeight()/2 + ( (CaratPos.y + XUI_IME::m_Candlist.rcCandidate.Height() > rcWindow.Height())?rcWindow.Height()-XUI_IME::m_Candlist.rcCandidate.Height()-1:CaratPos.y );
			if( CaratPos.y < 0 ) CaratPos.y = 0;

			XUI_SetClipping( CaratPos.x, CaratPos.y, CaratPos.x + XUI_IME::m_Candlist.rcCandidate.Width(), CaratPos.y + pFont->GetCharacterHeight() + 2 );
			XUI_DrawRect( 
				x_rect( CaratPos.x, CaratPos.y, CaratPos.x + XUI_IME::m_Candlist.rcCandidate.Width(), CaratPos.y + pFont->GetCharacterHeight() + 2 ),
				m_dwBorderColor, 
				m_dwBackgroundColor );

			XUI_DrawTextA( XUI_IME::m_Candlist.strBuffer.c_str(), pFont, float( CaratPos.x + 1 ), float( CaratPos.y + 1 ) );

			if( XUI_IME::m_Candlist.bShowWindow )
			{
				CaratPos.y += pFont->GetCharacterHeight() + 2;
				XUI_DrawRect( 
					x_rect( CaratPos.x, CaratPos.y, CaratPos.x + XUI_IME::m_Candlist.rcCandidate.Width(), CaratPos.y + XUI_IME::m_Candlist.rcCandidate.Height() ),
					m_dwBorderColor, 
					m_dwBackgroundColor );
			}
		}
	}

	//鼠标
	//鼠标移动事件
	//参数说明：
	//pt，鼠标的坐标，相对于控件
	//sysKeys，各种重要按键的状态，参见MSDN	
	bool XUI_EditBox::onMouseMove(const x_point& pt, UINT sysKeys)
	{
		return false;
	}

	bool XUI_EditBox::onMouseHover(const x_point& pt)
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
	bool XUI_EditBox::onButtonDown(int button, const x_point& pt, UINT sysKeys)
	{
		return true;
	}

	bool XUI_EditBox::onButtonUp(int button, const x_point& pt, UINT sysKeys)
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
			if( l.size() - p < nCount )
			{
				l.erase( p, l.size() - p );
				nCount -= ( l.size() - p );

				// 有后续行，将下一行和当前行合并
				if( m_text.size() > n + 1 )
				{
					if( l.type == type_n ) --nCount;
					l.type = type_r;

					NaturalLine( m_nCurLineNumber );
				}
				else
				{
					break;
				}
			}
			else if( l.type == type_r )
			{
				NaturalLine( m_nCurLineNumber );
			}
			else
			{
				l.erase( p, nCount );
				nCount = 0;
			}
		}
	}

	void XUI_EditBox::HandleBack()
	{
		if( m_CaratPos <= 0 )
		{
			if( m_nCurLineNumber == 0 ) return;
			SetCurLineNumber( m_nCurLineNumber - 1 );
			line l = m_text.at( m_nCurLineNumber );
			m_CaratPos = l.size() - (size_t)l.type;
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
		m_CaratPos = m_text.at( m_nCurLineNumber ).size();
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
		line& l = m_text[m_nCurLineNumber];

		line ll = l.substr( m_CaratPos, l.size() - m_CaratPos );
		ll.type = (m_CaratPos==l.size()?type_n:l.type);

		l.erase( l.begin() + m_CaratPos, l.end() );
		l.type = type_n;

		m_text.insert( m_text.begin() + m_nCurLineNumber + 1, ll );
		NaturalLine( m_nCurLineNumber + 1 );

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
			NaturalLine(m_nCurLineNumber);
		}
		return true;
	}

	//输入法
	//参见MSDN
	bool XUI_EditBox::onImeComp(uint32 wParam, uint32 lParam)
	{
		HWND hWnd = GuiSystem::Instance().GetHWND();
		HIMC imc = XUI_IME::_ImmGetContext( hWnd );
		{
			LONG lRet;  // Returned count in CHARACTERS
			WCHAR wszCompStr[MAX_COMPSTRING_SIZE];
			HIMC hImc;

			if( NULL == ( hImc = XUI_IME::_ImmGetContext( hWnd ) ) )
			{
				break;
			}

			// Get the caret position in composition string
			if ( lParam & GCS_CURSORPOS )
			{
				XUI_IME::m_Candlist.nCaretPos = XUI_IME::_ImmGetCompositionStringW( hImc, GCS_CURSORPOS, NULL, 0 );
				if( XUI_IME::m_Candlist.nCaretPos < 0 )
					XUI_IME::m_Candlist.nCaretPos = 0; // On error, set caret to pos 0.
			}

			// ResultStr must be processed before composition string.
			//
			// This is because for some IMEs, such as CHT, pressing Enter
			// to complete the composition sends WM_IME_COMPOSITION with both
			// GCS_RESULTSTR and GCS_COMPSTR.  Retrieving the result string
			// gives the correct string, while retrieving the comp string
			// (GCS_COMPSTR) gives empty string.  GCS_RESULTSTR should be
			// handled first so that the application receives the string.  Then
			// GCS_COMPSTR can be handled to clear the comp string buffer.

			if ( lParam & GCS_RESULTSTR )
			{
				lRet = XUI_IME::_ImmGetCompositionStringW( hImc, GCS_RESULTSTR, wszCompStr, sizeof( wszCompStr ) );
				if( lRet > 0 )
				{
					lRet /= sizeof(WCHAR);
					wszCompStr[lRet] = 0;  // Force terminate
					TruncateCompString( false, (int)wcslen( wszCompStr ) );
					s_CompString.SetText( wszCompStr );
					SendCompString();
					XUI_IME::ResetCompositionString();
				}
			}

			//
			// Reads in the composition string.
			//
			if ( lParam & GCS_COMPSTR )
			{
				//////////////////////////////////////////////////////
				// Retrieve the latest user-selected IME candidates
				lRet = XUI_IME::_ImmGetCompositionStringW( hImc, GCS_COMPSTR, wszCompStr, sizeof( wszCompStr ) );
				if( lRet > 0 )
				{
					lRet /= sizeof(WCHAR);  // Convert size in byte to size in char
					wszCompStr[lRet] = 0;  // Force terminate
					//
					// Remove the whole of the string
					//
					TruncateCompString( false, (int)wcslen( wszCompStr ) );

					s_CompString.SetText( wszCompStr );

					// Older CHT IME uses composition string for reading string
					if ( GetLanguage() == LANG_CHT && !GetImeId() )
					{
						if( lstrlen( s_CompString.GetBuffer() ) )
						{
							s_CandList.dwCount = 4;             // Maximum possible length for reading string is 4
							s_CandList.dwSelection = (DWORD)-1; // don't select any candidate

							// Copy the reading string to the candidate list
							for( int i = 3; i >= 0; --i )
							{
								if( i > lstrlen( s_CompString.GetBuffer() ) - 1 )
									s_CandList.awszCandidate[i][0] = 0;  // Doesn't exist
								else
								{
									s_CandList.awszCandidate[i][0] = s_CompString[i];
									s_CandList.awszCandidate[i][1] = 0;
								}
							}
							s_CandList.dwPageSize = MAX_CANDLIST;
							// Clear comp string after we are done copying
							ZeroMemory( (LPVOID)s_CompString.GetBuffer(), 4 * sizeof(WCHAR) );
							s_bShowReadingWindow = true;
							GetReadingWindowOrientation( 0 );
							if( s_bHorizontalReading )
							{
								s_CandList.nReadingError = -1;  // Clear error

								// Create a string that consists of the current
								// reading string.  Since horizontal reading window
								// is used, we take advantage of this by rendering
								// one string instead of several.
								//
								// Copy the reading string from the candidate list
								// to the reading string buffer.
								s_wszReadingString[0] = 0;
								for( UINT i = 0; i < s_CandList.dwCount; ++i )
								{
									if( s_CandList.dwSelection == i )
										s_CandList.nReadingError = lstrlen( s_wszReadingString );
									StringCchCat( s_wszReadingString, 32, s_CandList.awszCandidate[i] );
								}
							}
						}
						else
						{
							s_CandList.dwCount = 0;
							s_bShowReadingWindow = false;
						}
					}

					//if( s_bInsertOnType )
					//{
					//	// Send composition string to the edit control
					//	SendCompString();
					//	// Restore the caret to the correct location.
					//	// It's at the end right now, so compute the number
					//	// of times left arrow should be pressed to
					//	// send it to the original position.
					//	int nCount = lstrlen( s_CompString.GetBuffer() + s_nCompCaret );
					//	// Send left keystrokes
					//	for( int i = 0; i < nCount; ++i )
					//		SendMessage( DXUTGetHWND(), WM_KEYDOWN, VK_LEFT, 0 );
					//	SendMessage( DXUTGetHWND(), WM_KEYUP, VK_LEFT, 0 );
					//}
				}

				// ResetCaretBlink();
			}

			// Retrieve comp string attributes
			//if( lParam & GCS_COMPATTR )
			//{
			//	lRet = XUI_IME::_ImmGetCompositionStringW( hImc, GCS_COMPATTR, s_abCompStringAttr, sizeof( s_abCompStringAttr ) );
			//	if( lRet > 0 )
			//		s_abCompStringAttr[lRet] = 0;  // ??? Is this needed for attributes?
			//}

			//// Retrieve clause information
			//if( lParam & GCS_COMPCLAUSE )
			//{
			//	lRet = XUI_IME::_ImmGetCompositionStringW(hImc, GCS_COMPCLAUSE, s_adwCompStringClause, sizeof( s_adwCompStringClause ) );
			//	s_adwCompStringClause[lRet / sizeof(DWORD)] = 0;  // Terminate
			//}
			XUI_IME::_ImmReleaseContext( hWnd, imc );
		}
		return true;
	}

	bool XUI_EditBox::onImeEndComp(uint32 wParam, uint32 lParam)
	{
		return true;
	}

	bool XUI_EditBox::onImeNotify(uint32 wParam, uint32 lParam)
	{
		switch( lParam )
		{
		case IMN_PRIVATE:
			{
				int i = 0;
			}
			break;
		}
		return true;
	}


	//--------------------------------------------------------------------------------------
	// Sends the current composition string to the application by sending keystroke
	// messages.
	void XUI_EditBox::SendCompString()
	{
		for( int i = 0; i < lstrlen( s_CompString.GetBuffer() ); ++i )
			GuiSystem::Instance().HandleKeyboard( WM_CHAR, (WPARAM)XUI_IME::m_CompString[i], 0 );
	}

	unsigned int XUI_EditBox::OnMoveWindow( x_rect& rcWindow )
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
		line &curline = m_text.at( m_nCurLineNumber );
		m_CaratPos = curline.cursor_position;


		if( m_nCurLineNumber < m_FirstLineNumber )
		{
			m_FirstLineNumber = ( m_FirstLineNumber <= (size_t)m_WindowSize.cy?0:(m_FirstLineNumber - m_WindowSize.cy) );
		}
		else if( m_nCurLineNumber - m_FirstLineNumber >= (size_t)m_WindowSize.cy )
		{
			m_FirstLineNumber = m_nCurLineNumber;
		}
	}

	//获得焦点
	void XUI_EditBox::onGetFocus()
	{
		XUI_IME::_ImmAssociateContext( GuiSystem::Instance().GetHWND(), XUI_IME::m_hImcDef );
		XUI_Wnd::onGetFocus();
	}

	//失去焦点
	void XUI_EditBox::onLostFocus()
	{
		XUI_IME::_ImmAssociateContext( GuiSystem::Instance().GetHWND(), NULL );
		XUI_Wnd::onGetFocus();
	}

	//LRESULT XUI_EditBox::OnWndMsg( UINT nMsg, WPARAM wParam, LPARAM lParam )
	//{
	//	switch( nMsg )
	//	{
	//	case WM_INPUTLANGCHANGE:
	//		break;
	//	case WM_IME_SETCONTEXT:
	//		break;
	//	case WM_IME_SELECT:
	//		break;
	//	}
	//	return XUI_Wnd::OnWndMsg( nMsg, wParam, lParam );
	//}
}
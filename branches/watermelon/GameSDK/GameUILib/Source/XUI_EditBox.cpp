#include "XUI_Header.h"
#include "XUI_System.h"
#include "XUI_Window.h"
#include "XUI_EditBox.h"

namespace XGC
{
	namespace ui
	{
		XUI_EditBox::XUI_EditBox(void)
			: m_bSingleLine( true )
			, m_bControl( false )
			, m_bShift( false )
			, m_CaratPos( 0 )
			, m_FirstLineNumber( 0 )
			, m_nCurLineNumber( 0 )
			, m_bShowCarat( false )
			, m_dwBorderColor( XUI_ARGB(0xff,0x80,0x80,0x80) )
			, m_dwBackgroundColor( XUI_ARGB(0x30,0x20,0x20,0x20) )
			, m_hFont( NULL )
		{
			xuiFont hFont = m_hFont?m_hFont:XUI::Instance().GetDefaultFont();
			if( hFont )
			{
				m_TextBufferSizeX = m_WindowSize.cx/XUI_GetCharacterWidth( hFont, _T(' ') );
				m_TextBufferSizeY = m_WindowSize.cy/XUI_GetCharacterHeight( hFont );
			}

			m_text.push_back(_T(""));
		}

		XUI_EditBox::~XUI_EditBox(void)
		{
			XUI::Instance().KillTimer( m_CaratTimerHandler );
		}

		bool XUI_EditBox::CaratTimerUpdate( unsigned int handle, unsigned short& repeat, unsigned int& timer )
		{
			m_bShowCarat = !m_bShowCarat;
			return false;
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

		void XUI_EditBox::RenderCharacter( _tchar szChar, xuiFont hFont, int &nX, int &nY, bool bRender )
		{
			if( bRender )
			{
				XUI_DrawCharacter( szChar, hFont, nX, nY );
			}

			nX += XUI_GetCharacterWidth( hFont, szChar );
		}

		//重绘，通过实现这个方法来表现空间的外观
		void XUI_EditBox::RenderSelf( const iPoint& adjust )
		{
			__super::RenderSelf( adjust );
			// 绘制边框
			iRect rcWindow = GetWindowRect() + adjust;
			XUI_DrawRect( rcWindow, m_dwBorderColor, m_dwBackgroundColor );

			iPoint CharPos = rcWindow.TopLeft();
			iPoint CaratPos;
			xuiFont hFont = m_hFont?m_hFont:XUI::Instance().GetDefaultFont();

			int nFontHeight = XUI_GetCharacterHeight( hFont );
			for( Position i = m_FirstLineNumber; i < m_text.size(); ++i )
			{
				line& l = m_text[i];
				end_type t = l.type;

				if( CharPos.y > rcWindow.bottom - nFontHeight )
				{
					// 超出高度则跳出
					break;
				}

				for( size_t cursor = 0; cursor <= l.size(); ++cursor )
				{
					_tchar c = l[cursor];

					if( XUI_GetCharacterWidth( hFont, c ) + CharPos.x > rcWindow.right )
					{
						if( l.type == type_n && m_bWarpText && !m_bSingleLine )
						{
							// 字符显示超出宽度，则折行显示
							// 将没有画的作为独立的串添加到下一行
							line ll( l.subline( cursor, -1 ), l.type );
							l.erase( cursor, -1 );
							l.type = type_r;
							l.cursor_position = l.size();

							size_t lsize = l.size();
							ll.cursor_position = m_CaratPos - lsize;
							m_text.insert( m_text.begin() + i + 1, ll );

							// 根据光标位置判断是否要重新设定光标所在行。
							if( m_nCurLineNumber == i && m_CaratPos >= lsize )
							{
								SetCurLineNumber( m_nCurLineNumber + 1 );
							}
							break;
						}
						else
						{
							// 不折行从下一行开始
							CharPos.x = rcWindow.left;
							break;
						}
					}

					// 判断是否被绘制。
					bool bRender = rcWindow.PtInRect( CharPos + iPoint( XUI_GetCharacterWidth( hFont, c ), nFontHeight ) );
					if( _istprint( c ) )
					{
						// 是显示字符
						RenderCharacter( c, hFont, CharPos.x, CharPos.y, bRender );
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
								int NextTab = CharPos.x + (4 - cursor%4)*XUI_GetCharacterWidth( hFont, _T(' ') );
								if( NextTab > rcWindow.left + rcWindow.Width() ) NextTab = rcWindow.left + rcWindow.Width();
								while( CharPos.x < (long)NextTab )
								{
									RenderCharacter( _T(' '), hFont, CharPos.x, CharPos.y, bRender );
								}
							}
							break;
						case '\0':
							// 为绘制光标占位置。
							RenderCharacter( _T(' '), hFont, CharPos.x, CharPos.y, bRender );
							break;
						default:
							RenderCharacter( _T('?'), hFont, CharPos.x, CharPos.y, bRender );
							break;
						}
					}

					if( i == m_nCurLineNumber && cursor == m_CaratPos )
					{
						int x = CaratPos.x = CharPos.x - ( XUI_GetCharacterWidth( hFont, c ) + (XUI_GetCharacterWidth( hFont, _T('|') )>>1) );
						int y = CaratPos.y = CharPos.y;
						if( m_bShowCarat )
						{
							// 是否绘制光标
							RenderCharacter( _T('|'), hFont, x, y, bRender );
						}
					}
				}

				// 折行。
				CharPos.x = rcWindow.left;
				CharPos.y += nFontHeight;
			}

			if( m_bFocused && XUI_IME::m_CompString[0] )
			{
				iSize siWindow = XUI::Instance().GetClientSize();
				CaratPos.x = XUI_GetCharacterWidth( hFont, _T(' ') ) + ( (CaratPos.x + XUI_IME::m_CandList.rcCandidate.Width() > siWindow.cx)?siWindow.cx-XUI_IME::m_CandList.rcCandidate.Width()-1:CaratPos.x);
				if( CaratPos.x < 0 ) CaratPos.x = 0;

				CaratPos.y = nFontHeight/2 + ( (CaratPos.y + XUI_IME::m_CandList.rcCandidate.Height() > siWindow.cy)?siWindow.cy-XUI_IME::m_CandList.rcCandidate.Height()-1:CaratPos.y );
				if( CaratPos.y < 0 ) CaratPos.y = 0;

				int cx = XUI_GetStringWidth( hFont, XUI_IME::m_CompString );
				int cy = nFontHeight;
				// XUI_SetClipping( CaratPos.x, CaratPos.y, compstring_size.cx ,compstring_size.cy );
				XUI_DrawRect( 
					iRect( CaratPos.x, CaratPos.y, CaratPos.x + cx, CaratPos.y + cy ),
					XUI_ARGB(70,170,170,170), 
					XUI_ARGB(70,190,190,190) );

				XUI_IME::CCandList& Watch = XUI_IME::m_CandList;
				wchar_t* compchar = XUI_IME::m_CompString;
				iPoint imeCaratePos = CaratPos;
				do
				{
					RenderCharacter( *compchar, hFont, imeCaratePos.x, imeCaratePos.y, true );
					if( m_bShowCarat && XUI_IME::m_CandList.nCaretPos == compchar - XUI_IME::m_CompString )
					{
						int x = imeCaratePos.x - ( XUI_GetCharacterWidth( hFont, *compchar ) + (XUI_GetCharacterWidth( hFont, _T('|') )>>1) );
						int y = imeCaratePos.y;
						// 是否绘制光标
						RenderCharacter( _T('|'), hFont, x, y, true );
					}
				}while( *compchar++ );

				if( XUI_IME::m_CandList.bShowWindow )
				{
					CaratPos.y += nFontHeight + 2;
					// XUI_SetClipping( CaratPos.x, CaratPos.y, XUI_IME::m_CandList.rcCandidate.Width(), XUI_IME::m_CandList.rcCandidate.Height() );
					XUI_DrawRect( 
						iRect( CaratPos, XUI_IME::m_CandList.rcCandidate.Size() ),
						XUI_ARGB(70,170,170,170),
						XUI_ARGB(70,190,190,190) );

					wchar_t show[256];
					int idx = 0;
					std::list< std::wstring >::const_iterator citer = XUI_IME::m_CandList.l.begin();
					while( citer != XUI_IME::m_CandList.l.end() )
					{
						_snwprintf( show, _countof(show), _T("%02d %s"), ++idx, citer->c_str() );
						XUI_DrawText( show, hFont, CaratPos.x, CaratPos.y );
						CaratPos.y += nFontHeight;
						++citer;
					}
				}
			}
		}

		//鼠标
		//鼠标移动事件
		//参数说明：
		//pt，鼠标的坐标，相对于控件
		//sysKeys，各种重要按键的状态，参见MSDN	
		bool XUI_EditBox::OnMouseMove(const iPoint& pt, UINT sysKeys)
		{
			return XUI_Wnd::OnMouseMove( pt, sysKeys );
		}

		bool XUI_EditBox::OnMouseEnter()
		{
			return true;
		}

		bool XUI_EditBox::OnMouseLeave()
		{
			return true;
		}

		//鼠标按键事件
		//参数说明：
		//button，按下的键，0-左键，1-右键，2-中键
		//pt，鼠标的坐标
		//sysKeys，各种重要按键的状态，参见MSDN
		bool XUI_EditBox::OnButtonDown(int button, const iPoint& pt, UINT sysKeys)
		{
			return XUI_Wnd::OnButtonDown( button, pt, sysKeys );
		}

		bool XUI_EditBox::OnButtonUp(int button, const iPoint& pt, UINT sysKeys)
		{
			return XUI_Wnd::OnButtonUp( button, pt, sysKeys );
		}

		//键盘
		//参数说明
		//keycode，按下的键
		//sysKeys，各种重要按键的状态，参见MSDN
		bool XUI_EditBox::OnKeyDown(_uint32 keycode, UINT sysKeys)
		{
			switch( keycode )
			{
			case VK_CONTROL:
				m_bControl = true;
				return false;
				break;
			case VK_SHIFT:
			case VK_RSHIFT:
				m_bShift = true;
				return false;
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
			if( m_bSingleLine ) return;

			line& l = m_text[m_nCurLineNumber];

			line ll = l.subline( m_CaratPos, l.size() - m_CaratPos );
			ll.type = (m_CaratPos==l.size()?type_n:l.type);

			l.erase( m_CaratPos, -1 );
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

		bool XUI_EditBox::OnKeyUp( _uint32 keycode, UINT sysKeys )
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
		bool XUI_EditBox::OnChar(_uint32 c, UINT sysKeys)
		{
			if( _istprint( c ) )
			{
				m_text.at(m_nCurLineNumber).insert( m_CaratPos++, 1, (wchar_t)c );
				NaturalLine(m_nCurLineNumber);
			}
			return true;
		}

		//输入法
		//参见MSDN
		bool XUI_EditBox::OnImeComp(_uint32 wParam, _uint32 lParam)
		{
			HWND hWnd = XUI::Instance().GetHWND();
			HIMC imc = XUI_IME::_ImmGetContext( hWnd );
			{
				LONG lRet;  // Returned count in CHARACTERS
				WCHAR wszCompStr[MAX_COMPSTRING_SIZE];
				HIMC hImc;

				if( NULL == ( hImc = XUI_IME::_ImmGetContext( hWnd ) ) )
				{
					return true;
				}

				// Get the caret position in composition string
				if ( lParam & GCS_CURSORPOS )
				{
					XUI_IME::m_CandList.nCaretPos = XUI_IME::_ImmGetCompositionStringW( hImc, GCS_CURSORPOS, NULL, 0 );
					if( XUI_IME::m_CandList.nCaretPos < 0 )
						XUI_IME::m_CandList.nCaretPos = 0; // On error, set caret to pos 0.
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
						lstrcpyn( XUI_IME::m_CompString, wszCompStr, sizeof(XUI_IME::m_CompString) );
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
					if( lRet >= 0 )
					{
						lRet /= sizeof(WCHAR);  // Convert size in byte to size in char
						wszCompStr[lRet] = 0;  // Force terminate

						lstrcpyn( XUI_IME::m_CompString, wszCompStr, sizeof(XUI_IME::m_CompString) );

						// Older CHT IME uses composition string for reading string
						if ( GETLANG() == LANG_CHT )
						{
							if( lstrlen( XUI_IME::m_CompString ) )
							{
								XUI_IME::m_CandList.dwCount = 4;             // Maximum possible length for reading string is 4
								XUI_IME::m_CandList.dwSelection = (DWORD)-1; // don't select any candidate

								//// Copy the reading string to the candidate list
								//for( int i = 3; i >= 0; --i )
								//{
								//	if( i > lstrlen( XUI_IME::m_CompString ) - 1 )
								//		XUI_IME::m_CandList.awszCandidate[i][0] = 0;  // Doesn't exist
								//	else
								//	{
								//		XUI_IME::m_CandList.awszCandidate[i][0] = XUI_IME::m_CompString[i];
								//		XUI_IME::m_CandList.awszCandidate[i][1] = 0;
								//	}
								//}
								//XUI_IME::m_CandList.dwPageSize = MAX_CANDLIST;
								// Clear comp string after we are done copying
								ZeroMemory( (LPVOID)XUI_IME::m_CompString, 4 * sizeof(WCHAR) );
								XUI_IME::m_CandList.bShowWindow = true;
							}
							else
							{
								XUI_IME::m_CandList.dwCount = 0;
								XUI_IME::m_CandList.bShowWindow = false;
							}
						}
					}
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

		bool XUI_EditBox::OnImeEndComp(_uint32 wParam, _uint32 lParam)
		{
			XUI_IME::ResetCompositionString();
			return true;
		}

		bool XUI_EditBox::OnImeNotify(_uint32 wParam, _uint32 lParam)
		{
			switch( wParam )
			{
			case IMN_OPENSTATUSWINDOW:
				break;
			case IMN_CLOSESTATUSWINDOW:
				break;
			case IMN_OPENCANDIDATE:
			case IMN_CHANGECANDIDATE:
				{
					XUI_IME::m_CandList.bShowWindow = true;

					HWND hWnd = XUI::Instance().GetHWND();
					HIMC himc;
					if (NULL == (himc = XUI_IME::_ImmGetContext(hWnd)))
						break;

					LPCANDIDATELIST lpCandList;
					DWORD dwIndex, dwBufLen;

					dwIndex = 0;
					dwBufLen = XUI_IME::_ImmGetCandidateListW( himc, dwIndex, NULL, 0 );
					if ( dwBufLen )
					{
						lpCandList = (LPCANDIDATELIST)malloc(dwBufLen);
						dwBufLen = XUI_IME::_ImmGetCandidateListW( himc, dwIndex, lpCandList, dwBufLen );
					}

					if ( dwBufLen )
					{
						DWORD dwSelection = lpCandList->dwSelection;
						DWORD dwCount = lpCandList->dwCount;

						int startOfPage = 0;

						xuiFont hFont = m_hFont?m_hFont:XUI::Instance().GetDefaultFont();
						XUI_IME::CCandList& Watch = XUI_IME::m_CandList;
						Watch.l.clear();
						Watch.rcCandidate.SetRectEmpty();
						for( _uint32 i = lpCandList->dwSelection; i < __min( lpCandList->dwCount, lpCandList->dwSelection + lpCandList->dwPageSize ); ++i )
						{
							_lpcwstr text_ptr = (_lpwstr)((ulong_ptr)lpCandList + lpCandList->dwOffset[i]);
							Watch.l.push_back( text_ptr );
							int cx = XUI_GetStringWidth( hFont, text_ptr );
							int cy = XUI_GetCharacterHeight( hFont ); 
							Watch.rcCandidate.bottom += cy + 1;
							Watch.rcCandidate.right = __max( Watch.rcCandidate.right, cx + XUI_GetCharacterWidth( hFont,_T('0'))*4 );
						}
						Watch.dwCount = __min( lpCandList->dwCount, MAX_CANDLIST );

						free( (HANDLE)lpCandList );
						XUI_IME::_ImmReleaseContext(hWnd, himc);
					}
				}
				break;
			case IMN_CLOSECANDIDATE:
				XUI_IME::m_CandList.bShowWindow = false;
				break;
			case IMN_SETCONVERSIONMODE:
				break;
			case IMN_SETSENTENCEMODE:
				break;
			case IMN_SETOPENSTATUS:
				break;
			case IMN_SETCANDIDATEPOS:
				break;
			case IMN_SETCOMPOSITIONFONT:
				break;
			case IMN_SETCOMPOSITIONWINDOW:
				break;
			case IMN_SETSTATUSWINDOWPOS:
				break;
			case IMN_GUIDELINE:
				break;
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
			for( int i = 0; i < lstrlen( XUI_IME::m_CompString ); ++i )
				OnChar( (WPARAM)XUI_IME::m_CompString[i], 0 );
		}

		unsigned int XUI_EditBox::OnMoveWindow( iRect& rcWindow )
		{
			xuiFont hFont = m_hFont?m_hFont:XUI::Instance().GetDefaultFont();
			if( hFont )
			{
				m_TextBufferSizeX		= (int)( m_WindowSize.cx/XUI_GetCharacterWidth( hFont, _T(' ') ) );
				m_TextBufferSizeY		= (int)( m_WindowSize.cy/XUI_GetCharacterHeight( hFont ) );
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
				m_FirstLineNumber = ( m_FirstLineNumber <= (size_t)m_TextBufferSizeY?0:(m_FirstLineNumber - m_TextBufferSizeY) );
			}
			else if( m_nCurLineNumber - m_FirstLineNumber >= (size_t)m_TextBufferSizeY )
			{
				m_FirstLineNumber = m_nCurLineNumber;
			}
		}

		//获得焦点
		void XUI_EditBox::OnSetFocus()
		{
			m_CaratTimerHandler = XUI::Instance().SetTimer( _tfunction( bind(&XUI_EditBox::CaratTimerUpdate, this, _1, _2, _3) ), 0, TIMER_SECOND(0.5f) );
			XUI_IME::_ImmAssociateContext( XUI::Instance().GetHWND(), XUI_IME::m_hImcDef );
			XUI_Wnd::OnSetFocus();
		}

		//失去焦点
		void XUI_EditBox::OnLostFocus()
		{
			XUI::Instance().KillTimer( m_CaratTimerHandler );
			m_CaratTimerHandler = INVALID_TIMER_HANDLE;
			m_bShowCarat = false;
			XUI_IME::_ImmAssociateContext( XUI::Instance().GetHWND(), NULL );
			XUI_Wnd::OnLostFocus();
		}
	}
}
//#include <atlbase.h>		//为了能使用宽字符和多字节字符相互转换的宏: USES_CONVERSION,A2W,W2A
//#include <iostream>
//#include <tchar.h>
//#include <windows.h>
//#include "header.h"
//#include "UIElement.h"
//#include "EditBox.h"
//#include "Canvas.h"
//
//namespace UILib
//{
//	#define ISKEYDOWN( vk_code )	( (GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0 )
//	#define MAX_EDIT_WORDS 1024		//不管用户有没有设置MaxLength，都不能超过该定义长度。
//
//	int CEdit::m_sLeftOffset = 4;
//	//-----------------------------------------------------------------------
//	std::string GL_W2A( const std::wstring& wstrValue )
//	{
//		char dest[1024];
//		WideCharToMultiByte( CP_ACP, 0, wstrValue.c_str(), -1,
//			dest, 1024, NULL, NULL );
//		std::string strTest = dest;
//		return strTest;
//	}
//
//	//-----------------------------------------------------------------------
//	std::wstring GL_A2W( const std::string& strValue )
//	{
//		int size = ( int )strValue.size() * 2 + 2;
//		wchar_t* wStr = new wchar_t[ size ];
//		MultiByteToWideChar( CP_ACP, 0, strValue.c_str(), -1, wStr, size );
//
//		std::wstring Result = wStr;
//		delete [] wStr;
//		return Result;
//	}
//
//	//-----------------------------------------------------------------------
//	CEdit::CEdit(void)
//	{
//		m_hOldIMC = NULL;
//		m_nMaxTextLength = -1;
//
//		m_bReadOnly = false;
//		m_bMultiLines = false;
//		m_bMouseDrag = false;
//		
//		m_bPassword = false;
//		m_strPasswordChar = "*";
//
//		m_nFirstVisible = 0;
//		m_nVisibleCount = 0;
//		m_nCaret = 0;
//		m_nSelStart = 0;
//
//		m_nBorderLength = 0;
//
//		m_dwSelectColor = 0xff0000e0;
//		m_dwCursorColor = 0xffffffff;
//		m_dwTextColor	= 0xffffffff;
//	}
//
//	//-----------------------------------------------------------------------
//	CEdit::~CEdit(void)
//	{
//	}	
//
//	//-----------------------------------------------------------------------
//	void CEdit::Clear()
//	{
//		m_nFirstVisible = 0;
//		m_nVisibleCount = 0;
//		m_nCaret = 0;
//		m_nSelStart = 0;
//		m_strText = "";
//	}
//
//
//	//-----------------------------------------------------------------------
//	//这里用多字节字符,然后根据需要转换成宽字符
//	bool CEdit::onChar(DWORD c, UINT sysKeys)
//	{
//		WCHAR wChar = (WCHAR)c;
//		if(wChar >= 0x20 )
//		{
//			DeleteSelectionText();
//			if(m_nCaret <= (int)(GetVisibleText().length()))
//			{	
//				//确保范围合法
//				int nInsertPos = m_nCaret + m_nFirstVisible;
//				if(nInsertPos > (int)m_strText.length())
//					return true;
//
//							
//				std::string strTmp;
//				strTmp += (CHAR)c;				
//				m_strSingleText.append(strTmp);
//
//				
//				std::wstring wstrNewSingle = GL_A2W(m_strSingleText.c_str());
//				if(wstrNewSingle.length() > 0 && TestNotOverMaxLen(wstrNewSingle))	//防止多字节字符被断字
//				{
//					std::wstring wstrNewText = GetWStrText();
//					wstrNewText.insert(nInsertPos,wstrNewSingle);
//					m_strText = GL_W2A(wstrNewText.c_str());					
//					m_strSingleText = "";
//					
//					++m_nCaret;
//					std::wstring wstrFirst2Cursor = wstrNewText.substr(m_nFirstVisible,m_nCaret);
//					int nWidth = GetStringWidth(wstrFirst2Cursor);
//					//如果(光标位置-起始位置)的长度 > 编辑框宽度,则m_nFirstVisible加一
//					if(nWidth > GetInputWidth())
//					{								
//						while (true)
//						{
//							++m_nFirstVisible;
//							--m_nCaret;
//							wstrFirst2Cursor = wstrFirst2Cursor.substr(1,wstrFirst2Cursor.length());
//							nWidth = GetStringWidth(wstrFirst2Cursor );
//							if(nWidth <= GetInputWidth())
//							{
//								break;
//							}
//						}
//					}							
//					RefreshVisibleTextCount();									
//				}
//				//注意当超过最大长度时，需清除记录的字符。
//				if(!TestNotOverMaxLen(wstrNewSingle))
//				{
//					m_strSingleText = "";
//				}
//			}
//			m_nSelStart = m_nFirstVisible + m_nCaret;
//		}
//		return true;
//	}
//
//	//-----------------------------------------------------------------------
//	bool CEdit::onButtonDown(int button, const CUIPoint& pt, UINT sysKeys)
//	{
//		if( m_bEnable )
//		{
//			m_bMouseDrag = true;
//			//CXUIManager::Instance()->SetCurrMouseCapture(this);
//			//CXUIManager::Instance()->SetCurrKeyFocus(this);
//
//			m_nCaret = ParseCaretByMouse( pt );
//			m_nSelStart = m_nFirstVisible + m_nCaret;				
//		}
//		return __super::onButtonDown( button, pt, sysKeys );
//	}
//
//	//-----------------------------------------------------------------------
//	bool CEdit::onButtonUp(int button, const CUIPoint& pt, UINT sysKeys)
//	{
//		if( m_bEnable )
//		{
//			m_bMouseDrag = false;
//			//CXUIManager::Instance()->ReleaseCapture();
//		}
//		return __super::onButtonUp( button, pt, sysKeys );
//	}
//
//	//-----------------------------------------------------------------------
//	bool CEdit::onMouseMove(const CUIPoint& pt, UINT sysKeys)
//	{
//		if( m_bEnable && m_bMouseDrag )
//		{
//			m_nCaret = ParseCaretByMouse(pt);
//		}
//		return __super::onMouseMove( pt, sysKeys );
//	}
//
//
//	//-----------------------------------------------------------------------
//	////获取键盘焦点，如果是Password编辑框，那么DisableIME
//	void CEdit::OnFocusIn()
//	{
//		if( m_bPassword )
//		{
//			//m_hOldIMC = ImmAssociateContext( CXUIManager::Instance()->GetHWnd(), NULL );
//		}
//	}
//
//	//-----------------------------------------------------------------------
//	//失去键盘焦点: a.失去选中, b.EnablePrevIME，
//	void CEdit::OnFocusOut()
//	{
//		m_nSelStart = m_nCaret + m_nFirstVisible;
//
//		if ( m_hOldIMC )
//		{
//			//ImmAssociateContext( CXUIManager::Instance()->GetHWnd(), m_hOldIMC );
//			m_hOldIMC = NULL;   //必须置为NULL,因为用户在其他输入框还可以到各种各样的IME
//		}
//	}
//
//	//-----------------------------------------------------------------------
//	bool CEdit::onKeyDown( DWORD keycode, UINT sysKeys )
//	{
//		switch(keycode)
//		{
//		case VK_UP:
//			{
//				OnUp();break;
//			}
//		case VK_DOWN:
//			{
//				OnDown();break;
//			}
//		case VK_LEFT:                                       // 处理"<-"键
//			{
//				OnLeft();break;
//			}
//		case VK_RIGHT:                                      // 处理"->"键 
//			{
//				OnRight();break;
//			}
//		case VK_RETURN:										// 回车处理                          
//			{
//				OnEntry();break;
//			}
//		case VK_ESCAPE:
//			{
//				OnEscape();break;
//			}
//		case VK_BACK:										// BACK键处理
//			{
//				OnBack();break;
//			}
//		case VK_DELETE:                                     // 处理Del键
//			{
//				OnDelete();break;
//			}   
//		case VK_HOME:										// 处理Home键
//			{
//				OnHome();break;
//			}
//		case VK_END:                                        // 处理End键
//			{  
//				OnEnd();break;
//			}
//		case 'a':                                           // 全选Ctrl + a
//		case 'A':
//			{
//				if(ISKEYDOWN(VK_CONTROL))
//					OnSelectAll();
//				break;;
//			}
//		case 'c':                                           // 拷贝Ctrl + c
//		case 'C':
//			{
//				if(ISKEYDOWN(VK_CONTROL))
//					CopyToClipboard();
//				break;
//			}
//		case 'x':                                           // 剪切Ctrl + x
//		case 'X':
//			{
//				if(ISKEYDOWN(VK_CONTROL))
//				{
//					CopyToClipboard();
//					DeleteSelectionText();
//				}	
//				break;
//			}
//		case 'v':                                           // 粘贴Ctrl + v
//		case 'V':
//			{
//				if(ISKEYDOWN(VK_CONTROL))
//					PasteFromClipboard();
//				break;
//			}
//		}
//		RefreshVisibleTextCount();
//		return __super::onKeyDown( keycode, sysKeys );
//	}
//
//	//-----------------------------------------------------------------------
//	void CEdit::OnEscape(void)
//	{
//
//	}
//
//	//-----------------------------------------------------------------------
//	void CEdit::OnEntry()
//	{
//	}
//
//	//-----------------------------------------------------------------------
//	void CEdit::OnLeft()
//	{
//		if(m_nCaret > 0 && m_nCaret <= m_nVisibleCount)
//			--m_nCaret;
//		else if(m_nCaret == 0 && m_nFirstVisible > 0)
//		{
//			--m_nFirstVisible;
//		}
//		m_nSelStart = m_nFirstVisible + m_nCaret;
//	}
//
//	//-----------------------------------------------------------------------
//	void CEdit::OnRight()
//	{
//		if(m_nCaret >= 0 && m_nCaret < m_nVisibleCount)
//			++m_nCaret;
//		else if(m_nCaret ==m_nVisibleCount  && ((int)GetWStrText().length() > m_nFirstVisible + m_nVisibleCount) )
//		{
//			++m_nFirstVisible;				
//			RefreshVisibleTextCount();
//			m_nCaret = m_nVisibleCount;
//		}
//		m_nSelStart = m_nFirstVisible + m_nCaret;
//	}
//
//	//-----------------------------------------------------------------------
//	void CEdit::OnDelete()
//	{
//		if( (m_nFirstVisible + m_nCaret) != m_nSelStart )
//		{
//			DeleteSelectionText();
//			//SendEvent( EVENT_EDITBOX_CHANGE,this );
//		}
//		else
//		{
//			// Deleting one character
//			std::wstring wstrText = GetWStrText();
//			if( (m_nFirstVisible + m_nCaret) < (int)wstrText.size() )
//			{
//				wstrText.erase(m_nFirstVisible + m_nCaret,1);
//				m_strText = GL_W2A(wstrText.c_str());
//				//SendEvent( EVENT_EDITBOX_CHANGE,this );
//			}			
//		}
//	}
//
//	//-----------------------------------------------------------------------
//	void CEdit::OnBack()
//	{
//		if( (m_nFirstVisible + m_nCaret) != m_nSelStart )	//删除选中区域
//		{
//			DeleteSelectionText();
//			//SendEvent( EVENT_EDITBOX_CHANGE,this );
//		}
//		else if(m_nCaret > 0 && m_nCaret <= m_nVisibleCount)  //要变动光标位置，不变动第一个可见字符
//		{			
//			std::wstring wstrText = GetWStrText();
//			wstrText.erase(m_nFirstVisible + m_nCaret-1,1);
//			m_strText = GL_W2A(wstrText.c_str());			
//			--m_nCaret;
//			RefreshVisibleTextCount(); //重新计算宽度，刷新m_bVisibleCount
//			//SendEvent( EVENT_EDITBOX_CHANGE,this );
//		}
//		else if(m_nCaret == 0 && m_nFirstVisible > 0)	//要变动光标位置，且变动第一个可见字符
//		{
//			/*因为字符不可见，则不删除只是移动可见字符。
//			--m_nFirstVisible;
//			wstring wstrText = GetWStrText();
//			wstrText.erase(m_nFirstVisible,1);
//			USES_CONVERSION;
//			m_strText = W2A(wstrText.c_str());
//			*/
//			std::wstring wstrVisible = GetVisibleTextReverse((int)GetWStrText().length(),&m_nFirstVisible);		
//			m_nCaret = (int)wstrVisible.length();
//			RefreshVisibleTextCount(); //m_nFirstVisible已经改变，需刷新m_bVisibleCount
//			//SendEvent( EVENT_EDITBOX_CHANGE,this );
//		}
//		m_nSelStart = m_nFirstVisible + m_nCaret;
//	}
//
//	//-----------------------------------------------------------------------
//	//不作反应
//	void CEdit::OnUp()
//	{
//	}
//
//	//-----------------------------------------------------------------------
//	//不作反应
//	void CEdit::OnDown()
//	{	
//	}
//
//	//-----------------------------------------------------------------------
//	void CEdit::OnHome()
//	{			
//		//m_nSelStart = KEYDOWN(VK_SHIFT)? m_nFirstVisible + m_nCaret : 0;
//		m_nFirstVisible = 0;
//		m_nCaret = 0;
//		m_nSelStart = ISKEYDOWN(VK_SHIFT)? m_nSelStart : 0;
//	}
//
//	//-----------------------------------------------------------------------
//	void CEdit::OnEnd()
//	{				
//		std::wstring wstrVisible = GetVisibleTextReverse((int)GetWStrText().length(),&m_nFirstVisible);		
//		//m_nSelStart = KEYDOWN(VK_SHIFT)? m_nCaret : m_nFirstVisible + (int)wstrVisible.length();		
//		m_nCaret = (int)wstrVisible.length();		
//		m_nSelStart = ISKEYDOWN(VK_SHIFT)? m_nSelStart : m_nFirstVisible + (int)wstrVisible.length();
//	}
//
//	//-----------------------------------------------------------------------
//	void CEdit::OnSelectAll()
//	{
//		//m_nSelStart = m_nCaret = m_nFirstVisible = 0;
//	}
//
//	//-----------------------------------------------------------------------
//	void CEdit::DeleteSelectionText()
//	{
//		//计算选中范围
//		int nFirst = min( m_nFirstVisible + m_nCaret, m_nSelStart );
//		int nLast = max( m_nFirstVisible + m_nCaret, m_nSelStart );
//		
//		//删除选中字符
//		std::wstring wstrText = GetWStrText();
//		wstrText = wstrText.erase(nFirst,nLast-nFirst);
//		m_strText = GL_W2A(wstrText.c_str());
//
//		m_nSelStart = nFirst;
//		m_nCaret = nFirst - m_nFirstVisible;
//		RefreshVisibleTextCount();
//	}
//	
//	//-----------------------------------------------------------------------
//	void CEdit::CopyToClipboard()
//	{
//		int nFirst = min( m_nFirstVisible + m_nCaret, m_nSelStart );
//		int nLast = max( m_nFirstVisible + m_nCaret, m_nSelStart );
//		if( !m_bPassword && nLast - nFirst > 0 )
//		{
//			std::wstring wstrCopy = GetWStrText().substr(nFirst,nLast-nFirst);
//			std::string strCopy = GL_W2A(wstrCopy.c_str());
//			HGLOBAL hClip = GlobalAlloc(GMEM_ZEROINIT|GMEM_MOVEABLE|GMEM_DDESHARE,MAX_EDIT_WORDS);
//			if(!hClip)
//				return;
//
//			char *temp = (char*)GlobalLock(hClip);
//			if(!temp)
//			{
//				GlobalUnlock(hClip);
//				return;
//			}
//
//			for(int i = 0; i< (int)strCopy.length(); ++i )
//				temp[i] = strCopy[i];
//			temp[(int)strCopy.length()] = 0;
//
//			GlobalUnlock(hClip);
//
//			if(OpenClipboard(NULL))
//			{
//				EmptyClipboard();
//				SetClipboardData( CF_TEXT,hClip);
//				CloseClipboard();
//			}
//		}				
//	}
//
//	//-----------------------------------------------------------------------
//	void CEdit::PasteFromClipboard()
//	{
//		DeleteSelectionText();
//
//		if(OpenClipboard(NULL))
//		{
//			HGLOBAL hClip=NULL;
//
//			if( hClip=GetClipboardData(CF_TEXT) )
//			{
//				char *temp = (char*)GlobalLock(hClip);				
//				std::string strClipboard(temp);
//				int nMaxLength = 0;
//				for(int i=nMaxLength;i<(int)strClipboard.length();i++)
//				{
//					onChar( (DWORD)strClipboard[i],NULL );	//直接发消息，由OnChar计算光标及第一个可见字符
//					if(!TestNotOverMaxLen(L""))	//超过长度则跳出，避免循环次数太多。
//						break;
//				}
//				GlobalUnlock(hClip);				
//			}
//			CloseClipboard();		
//		}
//	}
//
//	//-----------------------------------------------------------------------
//	//这里使用多字节字符计数
//	bool CEdit::TestNotOverMaxLen(const std::wstring& wstrSingleText)
//	{
//		std::string strText = GL_W2A( (GetWStrText() + wstrSingleText).c_str() );
//		return  (m_nMaxTextLength != -1 && (int)strText.length() <= m_nMaxTextLength)
//			|| (m_nMaxTextLength == -1 && (int)strText.length() <= MAX_EDIT_WORDS) ;
//	}
//
//
//	//-----------------------------------------------------------------------
//	//返回可视文本中的光标位置
//	int CEdit::ParseCaretByMouse(const POINT& ptMouse)
//	{
//		int nCaretPos = 0;
//		if(m_strText.length() == 0)
//		{
//			m_nCaret = 0;
//			m_nSelStart = 0;
//			return nCaretPos;
//		}
//
//		//ptCaret = 鼠标位置 - 左边距
//		POINT ptCaret;
//		ptCaret.x = ptMouse.x - m_sLeftOffset;
//		ptCaret.y = ptMouse.y;
//
//		//先取得当前显示的文本
//		std::wstring wstrText = GetVisibleText();
//		int nSingleWCharWidth,nTmpWidth = 0;
//
//		
//		for(int i=0;i<(int)wstrText.length();++i)
//		{								
//			nSingleWCharWidth = GetCharWidth( wstrText[i] );
//			nTmpWidth += nSingleWCharWidth;
//			if(ptCaret.x > nTmpWidth)
//			{
//				//++m_nSelStart;
//				++nCaretPos;
//			}
//			else
//			{
//				if(ptCaret.x > 0 && abs(nTmpWidth - ptCaret.x) < (nSingleWCharWidth / 2) )
//				{				
//					//++m_nSelStart;
//					++nCaretPos;
//				}				
//				break;
//			}
//		}
//		return nCaretPos;
//	}
//
//	//-----------------------------------------------------------------------
//	//获取编辑框内可视文本
//	std::wstring CEdit::GetVisibleText()
//	{
//		std::wstring wstrText = GetWStrText();
//		if( m_nFirstVisible <= (int)wstrText.length() 
//			&& (m_nFirstVisible + m_nVisibleCount) <= (int)wstrText.length() )
//			return wstrText.substr(m_nFirstVisible,m_nVisibleCount);
//		else
//			return L"";
//	}
//
//	//-----------------------------------------------------------------------
//	//根据最后一个可是字符位置,倒推获取编辑框内可视文本
//	std::wstring CEdit::GetVisibleTextReverse(int nReversePos,int* pnFirstVisible)
//	{		
//		std::wstring wstrText = GetWStrText();
//		*pnFirstVisible = 0;
//
//		if(nReversePos <= (int)wstrText.length())
//		{
//			int nSingleWCharWidth, nTmpWidth = 0;
//			for(int i=nReversePos-1;i>=0;--i)
//			{
//				nSingleWCharWidth = GetCharWidth( wstrText[i] );
//				nTmpWidth += nSingleWCharWidth;
//				if( nTmpWidth >= GetInputWidth() )
//				{
//					*pnFirstVisible = i+1;
//					return wstrText.substr(i+1,nReversePos-i-1);
//				}
//			}		 
//			return wstrText;	//如果整个文本宽度<编辑框宽度,则返回整个字符串
//		}
//
//		return L"";
//	}
//
//	//-----------------------------------------------------------------------
//	std::wstring CEdit::GetWStrText()
//	{
//		return GL_A2W(m_strText.c_str());
//	}
//
//	//-----------------------------------------------------------------------
//	int CEdit::GetInputWidth()	//返回编辑框可输入的宽度,(注:编辑框宽度 不一定等于 可输入宽度)
//	{
//		return GetRect().Width() - m_sLeftOffset;
//	}
//
//	//-----------------------------------------------------------------------
//	//刷新m_nVisibleCount
//	void CEdit::RefreshVisibleTextCount()
//	{
//		m_nVisibleCount = 0;
//		int nSingleWCharWidth, nTmpWidth = 0;
//		std::wstring wstrText = GetWStrText();
//		for(int i=m_nFirstVisible;i<(int)wstrText.length();++i)
//		{
//			nSingleWCharWidth = GetCharWidth(wstrText[i] );
//			nTmpWidth += nSingleWCharWidth;
//			if( nTmpWidth <= GetInputWidth() )
//				++m_nVisibleCount;
//			else
//				return;
//		}
//	}
//
//	//-----------------------------------------------------------------------
//	void CEdit::RenderSelf( const CUIRect& clipper )
//	{
//		//CUIPoint Pos( m_wndRect.left, m_wndRect.top );
//		CUIPoint Pos( 0, 0 );
//		Pos = ClientToScreen( Pos );
//
//		////绘制背景
//		m_pCanvas->DrawTexture( m_pBkTex, Pos.x, Pos.y, m_wndRect.Width(), m_wndRect.Height(), clipper );
//
//		//获取客户区域
//		CUIRect rcClient = GetRect();
//
//		Pos.y += rcClient.Height()/2;
//		//绘制选中背景
//		if( (m_nFirstVisible + m_nCaret) != m_nSelStart )
//		{						
//			int nFirst = min( m_nFirstVisible + m_nCaret, m_nSelStart );
//			int nLast = max( m_nFirstVisible + m_nCaret, m_nSelStart );			
//			std::wstring wstrText = GetWStrText();
//			if(nLast >= (int)wstrText.length() + 1)
//				return;
//
//			int nSelLeftWidth,nSelLenght;
//			if ((nFirst-m_nFirstVisible) > 0)
//			{
//				//计算选中字符的左边距
//				nSelLeftWidth = GetStringWidth(wstrText.substr(m_nFirstVisible,nFirst-m_nFirstVisible) );
//			}
//			else
//			{
//				//如果第一个可见字符就已经是选中范围，那么选择左边距为0
//				nSelLeftWidth = 0;
//			}
//			//GetWStrWidth(wstrText.substr(m_nFirstVisible,abs(nFirst-m_nFirstVisible)),&nSelLeftWidth);
//			nSelLenght = GetStringWidth( wstrText.substr( nFirst, nLast-nFirst ) );
//			
//			RECT rcSelection;
//			rcSelection.left = nSelLeftWidth + m_sLeftOffset;
//			rcSelection.right = rcSelection.left + nSelLenght;
//			rcSelection.top = rcClient.top;
//			rcSelection.bottom = rcClient.bottom;
//
//			IntersectRect( &rcSelection, &rcClient, &rcSelection );
//			OffsetRect(&rcSelection,Pos.x,Pos.y);
//
//			m_pCanvas->DrawRect( rcSelection, m_dwSelectColor, NULL );	
//		}
//		
//		
//
//		//绘制文字	
//		std::wstring wstrVisibleText = GetVisibleText();
//		if(m_bPassword)
//		{
//			std::fill(strVisibleText.begin(), strVisibleText.end(), m_strPasswordChar[0]);
//		}
//		m_pCanvas->DrawText(strVisibleText.c_str(), m_pFont, Pos.x + m_sLeftOffset,Pos.y, m_dwTextColor, &rcClient );
//
//		// 有焦点就输出光标		
//		if( m_bFocused )
//		{
//			if((GetTickCount()/500)%2)
//			{
//				//根据m_nCaret计算光标位置
//				if(m_nCaret > (int)wstrVisibleText.length())
//					m_nCaret = (int)wstrVisibleText.length();
//							
//				int nX;
//				nX = GetStringWidth( wstrVisibleText.substr(0,m_nCaret) );
//				nX += (Pos.x + m_sLeftOffset - 4);
//				m_pCanvas->DrawText( _T("|"), m_pFont, nX, Pos.y, m_dwCursorColor, &rcClient );
//			}
//		}
//	}
//
//	void CEdit::SetText( LPCTSTR lpszText ) 
//	{
//		m_nFirstVisible = 0;
//		m_nVisibleCount = 0;
//		m_nCaret = 0;
//		m_nSelStart = 0;
//
//		if( lpszText == NULL )
//		{
//			m_strText = "";
//		}
//		else
//		{
//			int nLen = (int)_tcslen( lpszText ); // 字符长度
//			for( int i = 0; i < nLen; ++i )
//			{
//				onChar( (DWORD)lpszText[i],NULL );	//直接发消息，由OnChar计算光标及第一个可见字符
//				if(!TestNotOverMaxLen(L""))	//超过长度则跳出，避免循环次数太多。
//					break;
//			}
//		}
//	}
//
//	//-----------------------------------------------------------------------
//	int CEdit::GetCharWidth( WCHAR wChar )
//	{		
//		if(m_bPassword)
//		{
//			std::wstring wstrTmp;
//			wstrTmp.append(&wChar,1);
//			std::string strTmp = GL_W2A( wstrTmp );
//			return m_pFont?m_pFont->GetCharWidth( m_strPasswordChar[0] ):0;
//		}
//		return m_pFont?m_pFont->GetCharWidth( m_strPasswordChar[0] ):0;
//	}
//
//	//-----------------------------------------------------------------------
//	int CEdit::GetStringWidth( const std::wstring& strString )
//	{
//		if( m_bPassword )
//		{
//			std::wstring wstrTestTmp = strString;
//			std::string strTest = GL_W2A( strString ) ;
//			fill(strTest.begin(), strTest.end(), m_strPasswordChar[0]);			
//			wstrTestTmp = GL_A2W(strTest.c_str());
//			return m_pFont?m_pFont->GetCharHeight( m_strPasswordChar[0] ):0;
//		}
//		return m_pFont?m_pFont->GetCharHeight( m_strPasswordChar[0] ):0;
//	}
//}
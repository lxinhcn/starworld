//#pragma once
//
//#include "UIElement.h"
//namespace UILib
//{
//	//显示编辑框内容时的大小写风格
//	typedef enum TEditCharCases
//	{
//		XTEC_NORMAL = 0,		
//		XTEC_UPPERCASE,			
//		XTEC_LOWERCASE
//	};
//
//	class CEdit : public XObjTypeT< CUIElement, TypeEdit >
//	{
//	public:
//		CEdit(void);
//	
//		LPCTSTR	GetText() const { return m_strText.c_str(); }
//		void	SetText( LPCTSTR lpszText );
//		
//		int		GetMaxLength() const {return m_nMaxTextLength;};
//		void	SetMaxLength(int nLength){m_nMaxTextLength = nLength;};
//
//		int		GetBorderLength() const {return m_nBorderLength;};
//		void	SetBorderLength(int nLength){m_nBorderLength = nLength;};
//
//		bool	GetPWDStyle() const {return m_bPassword;};
//		void	SetPWDStyle(bool bValue){m_bPassword = bValue;};		 
//
//		LPCTSTR	GetPasswordChar() const {return m_strPasswordChar.c_str();}
//		void	SetPasswordChar(LPCTSTR strValue){m_strPasswordChar = strValue;}
//
//		DWORD	GetCaretColor() const {return m_dwSelectColor;};
//		void	SetCaretColor(DWORD dwValue){m_dwSelectColor = dwValue;};
//
//		DWORD	GetCursorColor() const {return m_dwCursorColor;};
//		void	SetCursortColor(DWORD dwValue){m_dwCursorColor = dwValue;};
//
//		bool	GetReadOnly() const {return m_bReadOnly;};
//		void	SetReadOnly(bool bValue){m_bReadOnly = bValue;};
//
//		bool	GetMultiLines() const {return m_bMultiLines;};
//		void	SetMultiLines(bool bValue){m_bMultiLines = bValue;};
//
//		static int	GetLeftOffset() {return m_sLeftOffset;};
//		static void SetLeftOffset(int nOffset){ m_sLeftOffset = nOffset;};
//		
//		void Clear();
//
//	protected:
//		virtual ~CEdit(void);
//
//		virtual void RenderSelf(const CUIRect& clipper);
//		virtual bool onChar(DWORD c, UINT sysKeys);
//		virtual bool onKeyDown(DWORD keycode, UINT sysKeys);
//
//		virtual	bool onButtonDown(int button, const CUIPoint& pt, UINT sysKeys);
//		virtual	bool onButtonUp(int button, const CUIPoint& pt, UINT sysKeys);
//		virtual	bool onMouseMove(const CUIPoint& pt, UINT sysKeys);
//		//virtual bool onMouseHover(const CUIPoint& pt);
//		//virtual bool onMouseEnter();
//		//virtual bool onMouseLeave();
//
//		virtual void OnEscape(void);   // Escape
//		virtual void OnEntry(void);    // 回车响应
//		virtual void OnLeft(void);     // <-键响应
//		virtual void OnRight(void);    // ->键响应
//		virtual void OnDelete(void);   // Del键响应
//		virtual void OnBack(void);     // Back键响应
//		virtual void OnUp(void);		//VK_UP键响应
//		virtual void OnDown(void);      //VK_DOWN键响应
//		virtual void OnHome(void);     // Home键响应
//		virtual void OnEnd(void);      // End键响应
//		virtual void OnSelectAll(void); // 全选
//
//		virtual void OnFocusIn();	//获取键盘焦点，如果是Password编辑框，那么DisableIME
//		virtual void OnFocusOut();	//失去键盘焦点，a.失去选中, b.EnablePrevIME，
//
//		//控件标签
//		virtual LPCTSTR GetLable(){ return _T("EDITBOX"); }
//
//		void DeleteSelectionText();		
//		void CopyToClipboard();
//		void PasteFromClipboard();
//
//		bool TestNotOverMaxLen(const std::wstring& wstrSingleText);
//
//		//根据鼠标位置,获取m_strText中光标起始位置; 
//		int ParseCaretByMouse(const POINT& ptMouse);	
//
//		TString GetVisibleText();
//		TString GetVisibleTextReverse(int nReversePos,int* pnFirstVisible);
//		void	RefreshVisibleTextCount();
//		int		GetInputWidth();	//返回编辑框可输入的宽度,(注:编辑框宽度 不一定等于 可输入宽度)
//		int		GetCharWidth( WCHAR wChar );
//		int		GetStringWidth( const std::wstring& strString );
//
//	protected:
//		TEditCharCases m_CharCase;
//
//		static int m_sLeftOffset;		//编辑框左边距
//
//		TString	m_strText;
//		TString m_strPasswordChar;
//		TString m_strSingleText;
//		TString m_strFontName;
//
//		int		m_nFirstVisible;			// 当前开始显示位置的索引,针对WStrText
//		int		m_nVisibleCount;			//渲染文本= WStrText.SubStr(m_nFirstVisible,m_nFirstVisible + m_nVisibleCount)
//				
//		int		m_nCaret;					//光标位置,针对strVisibleText,范围: 0 =< m_nCaret =< strVisibleText.length()
//		int		m_nSelStart;				//针对WStrText	范围: 0 =< WStrText =< strVisibleText.length()
//
//		int		m_nMaxTextLength;			//最大限制长度,-1为不限制(程序内部应该有最大长度)
//		int		m_nBorderLength;			//边框框度
//		bool	m_bReadOnly;
//		bool	m_bMultiLines;				//是否多行显示
//		bool	m_bPassword;
//		bool	m_bMouseDrag;
//
//
//		DWORD	m_dwSelectColor;
//		DWORD	m_dwCursorColor;
//		DWORD	m_dwTextColor;
//
//	    HANDLE	m_hOldIMC;
//	};
//}

#pragma once
#include "XUI_Wnd.h"

namespace UILib
{
	class XUI_EditBox :	public UIObjTypeT< XUI_Wnd, TypeEditBox >
	{
		friend class LuaBindClass;
		DECLARE_UIMSG_MAP()
		DECLARE_LABLE( EDIT )
	public:
		XUI_EditBox(void);
		~XUI_EditBox(void);

		//重绘，通过实现这个方法来表现空间的外观
		virtual void RenderSelf(const CRect& clipper);

		//virtual void OnDestroy();
		//鼠标
		//鼠标移动事件
		//参数说明：
		//pt，鼠标的坐标，相对于控件
		//sysKeys，各种重要按键的状态，参见MSDN	
		virtual	bool onMouseMove(const CPoint& pt, UINT sysKeys);
		virtual bool onMouseHover(const CPoint& pt);
		virtual bool onMouseEnter();
		virtual bool onMouseLeave();
		//鼠标按键事件
		//参数说明：
		//button，按下的键，0-左键，1-右键，2-中键
		//pt，鼠标的坐标
		//sysKeys，各种重要按键的状态，参见MSDN
		virtual	bool onButtonDown(int button, const CPoint& pt, UINT sysKeys);
		virtual	bool onButtonUp(int button, const CPoint& pt, UINT sysKeys);

		//键盘
		//参数说明
		//keycode，按下的键
		//sysKeys，各种重要按键的状态，参见MSDN
		virtual	bool onKeyDown(uint32 keycode, UINT sysKeys);
		virtual	bool onKeyUp(uint32 keycode, UINT sysKeys);

		//参数说明
		//c，输入的字符
		//sysKeys，各种重要按键的状态，参见MSDN
		virtual bool onChar(uint32 c, UINT sysKeys);

		//输入法
		//参见MSDN
		virtual bool onImeComp(uint32 wParam, uint32 lParam);
		virtual bool onImeEndComp(uint32 wParam, uint32 lParam);
		virtual bool onImeNotify(uint32 wParam, uint32 lParam);

		virtual unsigned int OnMoveWindow( CRect& rcWindow );

	protected:
		void RenderCharacter( _tchar szChar, XUI_IFont* pFont, LONG &x, LONG &y, BOOL bRender );
		void DeleteCharacter( size_t nPos );
		bool CaratTimerUpdate( unsigned int handle, unsigned short& repeat, unsigned int& timer );

		void HandleBack();
		void HandleDelete();
		void HandleHome();
		void HandleEnd();
		void HandleWordLeft();
		void HandleCharLeft();
		void HandleWordRight();
		void HandleCharRight();
		void HandleReturn();
		void HandleLineUp();
		void HandleLineDown();

		//---------------------------------------------------------------------//
		// describe	: 根据字符位置获得行号
		// nPos		: 字符索引
		// return	: 行号
		//---------------------------------------------------------------------//
		size_t GetLineFromCharaterPos( size_t nPos )const;

		//---------------------------------------------------------------------//
		// describe	: 分析当前串，刷新LineRecorder对象。
		//---------------------------------------------------------------------//
		void Analyse();

		//---------------------------------------------------------------------//
		// describe	: 设置当前行
		// line		: 行索引
		//---------------------------------------------------------------------//
		void SetCurLineNumber( size_t line );
	protected:
		// Attribute
		_string			m_strText;			// 编辑框文字
		bool			m_bWarpText;		// 折行标志

	private:
		typedef _string::size_type Position;
		typedef	std::vector< unsigned int >	line_recorder;
		size_t			m_FirstLineNumber;	// 编辑框里看到的第一个字符的位置
		size_t			m_nCurLineNumber;	// 当前行索引
		size_t			m_CaratPos;			// 光标位置
		line_recorder	m_LineRecorder;		// 换行符位置列表

		bool	m_bControl, m_bShift;
		CSize	m_WindowSize;				// 视窗大小

		unsigned int	m_CaratTimerHandler;
		bool			m_bShowCarat;
	};
}

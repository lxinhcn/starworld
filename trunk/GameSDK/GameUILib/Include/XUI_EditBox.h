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
		virtual	bool onKeyDown(DWORD keycode, UINT sysKeys);
		virtual	bool onKeyUp(DWORD keycode, UINT sysKeys);

		//参数说明
		//c，输入的字符
		//sysKeys，各种重要按键的状态，参见MSDN
		virtual bool onChar(DWORD c, UINT sysKeys);

		//输入法
		//参见MSDN
		virtual bool onImeComp(DWORD wParam, DWORD lParam);
		virtual bool onImeEndComp(DWORD wParam, DWORD lParam);
		virtual bool onImeNotify(DWORD wParam, DWORD lParam);

		virtual unsigned int OnMoveWindow( CRect& rcWindow );

	protected:
		void RenderCharacter( TCHAR szChar, XUI_IFont* pFont, LONG &x, LONG &y, BOOL bRender );
		void DeleteCharacter( size_t nPos );
		bool CaratTimerUpdate( unsigned int handle, unsigned short& repeat, unsigned int& timer );

		void HandleBack( UINT nSysKey );
		void HandleDelete( UINT nSysKey );
		void HandleHome( UINT nSysKey );
		void HandleEnd( UINT nSysKey );
		void HandleWordLeft( UINT nSysKey );
		void HandleCharLeft( UINT nSysKey );
		void HandleWordRight( UINT nSysKey );
		void HandleCharRight( UINT nSysKey );
		void HandleReturn( UINT nSysKey );

		//---------------------------------------------------------------------//
		// describe	: 根据字符位置获得行号
		// nPos		: 字符索引
		// return	: 行号
		//---------------------------------------------------------------------//
		size_t GetLineFromCharaterPos( size_t nPos )const;

	protected:
		// Attribute
		_string			m_strText;			// 编辑框文字
		bool			m_bWarpText;		// 折行标志

	private:
		typedef _string::size_type Position;
		typedef	std::deque< Position >	line_recorder;

		Position		m_FirstVisiblePos;	// 编辑框里看到的第一个字符的位置
		Position		m_CaratPos;			// 光标位置
		line_recorder	m_LineRecorder;		// 换行符位置列表

		bool	m_bControl, m_bShift;
		CSize	m_WindowSize;				// 视窗大小

		struct line_info
		{
			size_t	count;		// 字符数
			size_t	width;		// 占用宽度
			size_t	tag;		// 第一个字符在串中的索引号
		};

		unsigned int	m_CaratTimerHandler;
		bool			m_bShowCarat;
	};
}

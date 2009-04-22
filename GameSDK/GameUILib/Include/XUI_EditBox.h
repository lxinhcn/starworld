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
		virtual void RenderSelf();

		void SetText( const std::string &t );
		std::string GetText()const;

		//鼠标
		//鼠标移动事件
		//参数说明：
		//pt，鼠标的坐标，相对于控件
		//sysKeys，各种重要按键的状态，参见MSDN	
		virtual	bool onMouseMove(const x_point& pt, UINT sysKeys);
		virtual bool onMouseHover(const x_point& pt);
		virtual bool onMouseEnter();
		virtual bool onMouseLeave();
		//鼠标按键事件
		//参数说明：
		//button，按下的键，0-左键，1-右键，2-中键
		//pt，鼠标的坐标
		//sysKeys，各种重要按键的状态，参见MSDN
		virtual	bool onButtonDown(int button, const x_point& pt, UINT sysKeys);
		virtual	bool onButtonUp(int button, const x_point& pt, UINT sysKeys);

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

		//获得焦点
		virtual void onGetFocus();
		//失去焦点
		virtual void onLostFocus();

		virtual unsigned int OnMoveWindow( x_rect& rcWindow );

		// virtual LRESULT OnWndMsg( UINT nMsg, WPARAM wParam, LPARAM lParam );
	protected:
		void RenderCharacter( _tchar szChar, XUI_IFont* pFont, LONG &x, LONG &y, BOOL bRender );
		void DeleteCharacter( size_t nLine, size_t nPos, size_t nCount );
		bool CaratTimerUpdate( unsigned int handle, unsigned short& repeat, unsigned int& timer );
		size_t NaturalLine( size_t nLine );

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
		// describe	: 分析当前串，刷新LineRecorder对象。
		//---------------------------------------------------------------------//
		void Analyse();

		//---------------------------------------------------------------------//
		// describe	: 设置当前行
		// line		: 行索引
		//---------------------------------------------------------------------//
		void SetCurLineNumber( size_t line );
		void SendCompString();

	protected:
		// Attribute
		bool			m_bWarpText;		// 折行标志

	private:
		enum end_type{ type_n, type_r }; // 换行类型 n - 硬回车 r - 软回车
		struct line	:	public _string
		{
			line( const _tchar * string, end_type t = type_n )
				: cursor_position( 0 )
				, select_begin( 0 )
				, select_end( 0 )
				, type( t )
			{
				assign( string );
			}

			line( const _string& string, end_type t = type_n )
				: cursor_position( 0 )
				, select_begin( 0 )
				, select_end( 0 )
				, type( t )
			{
				assign( string );
			}

			size_t		cursor_position;
			size_t		select_begin;
			size_t		select_end;
			end_type	type;
		};

		typedef _string::size_type Position;
		typedef	std::vector< line >	text;
		size_t			m_FirstLineNumber;	// 编辑框里看到的第一个字符的位置
		size_t			m_nCurLineNumber;	// 当前行索引
		size_t			m_CaratPos;			// 光标位置
		text			m_text;				// 行记录
		uint32			m_dwBorderColor, m_dwBackgroundColor;

		bool	m_bControl, m_bShift;
		x_size	m_WindowSize;				// 视窗大小

		unsigned int	m_CaratTimerHandler;
		bool			m_bShowCarat;
	};
}

#ifndef XUI_Wnd_H
#define XUI_Wnd_H
#pragma once

#include "XUI_Base.h"
#define DECLARE_LABLE( lable )\
	virtual LPCTSTR GetLable()const { return _T(#lable); }\

namespace UILib
{
	struct XUI_IFont;
	class XUI_Wnd	:	public UIObjTypeT< XUI_Base, TypeElement >
	{
	friend class CGuiSystem;
	friend class CUIFactory;
	friend class LuaBindClass;

	protected:
		XUI_Wnd(void);
		virtual ~XUI_Wnd(void);

		void Destroy();
	protected:
		//绘制
		void Render(const CRect& clipper);

		//重绘，通过实现这个方法来表现空间的外观
		virtual void RenderSelf(const CRect& clipper)=0;

		//检查
		virtual void Validate();

		// 保存,载入
		virtual void OnSavePropertys( const char* name, TiXmlElement* pNode );
		virtual void OnLoadPropertys( const char* name, TiXmlElement* pNode );

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

		//获得焦点
		virtual void onGetFocus() {}
		//失去焦点
		virtual void onLostFocus() {}

		virtual unsigned int OnMoveWindow( CRect& rcWindow ){ return 0; }
		//////////////////////////////////////////////////////////////////////////
		// 消息处理

		virtual bool OnWndMsg( UINT nMsg, WPARAM wParam, LPARAM lParam );
		virtual bool OnCommand( WPARAM wParam, LPARAM lParam );
		virtual bool OnNotify( WPARAM wParam, LPARAM lParam, HRESULT* lResult );
	public:
		virtual LPCTSTR GetLable()const = 0;

		// 发送消息
		unsigned int SendMessage( UINT nMsg, WPARAM wParam, LPARAM lParam );

		void Release();

		//坐标转换
		CPoint ScreenToClient(const CPoint& pt) const;
		CPoint ClientToScreen(const CPoint& pt) const;

		//坐标修正，个别控件可以滚动，因此需要进行修正
		virtual CPoint AdjustPoint(const CPoint& pt, bool bClientToScreen) const {return pt;}

		//获取空间的显示区域
		const CRect& GetWindowRect()const { return m_wndRect; }

		//--------------------------------------------------------------------------
		//层次关系
		//--------------------------------------------------------------------------
		int  FindChild(XUI_Wnd* pElement) const;
		void AddChild(XUI_Wnd* pElement );
		void AddChildAt(XUI_Wnd* pElement, int x, int y);
		void RemoveChild(XUI_Wnd* pElement, bool bDestroy = true );
		void DestroyAllChild();
		void SetParent( XUI_Wnd* pParent ){ m_pParent = pParent; }
		XUI_Wnd* GetParent(){return m_pParent;}
		XUI_Wnd* FindChildInPoint(const CPoint& pt) const;
		XUI_Wnd* FindChildByName(LPCTSTR sName) const;
		XUI_Wnd* FindChildByID( int nID )const;

		typedef bool (*ElementCallback)( XUI_Wnd*, void* );
		XUI_Wnd* ForAllChild( ElementCallback pfnCallback, void* pData );
		size_t GetChildCount()const{ return m_pChildren.size(); }

		//--------------------------------------------------------------------------
		// 窗口大小的移动和调整
		//--------------------------------------------------------------------------
		
		// 重新设置空间的显示区域
		void Move(int left, int top, int right, int bottom);
		//平移
		void Offset(int x, int y);

		void BringToUp();
		void BringToDown();
		void BringToFront();
		void BringToEnd();

		//检测某一点是否在控件的显示区域内
		virtual BOOL IsPointIn(const CPoint& pt);

		//--------------------------------------------------------------------------
		// 设置控件属性
		//--------------------------------------------------------------------------
		
		//名称
		void	SetName( const _string& lpszName)	{ m_strName = lpszName;	}
		const _string& GetName()const {return m_strName;}

		void	SetID( UINT nID ){ m_nID = nID; }
		UINT	GetID(){ return m_nID; }

		//显示
		bool	IsVisible() {return m_bVisible;}
		void	Show( bool bVisible = true );

		bool	IsEnable() { return m_bEnable; }
		void	Enable( bool bEnable );

		//设置焦点
		void	SetFocus(bool bFocused);

		//---------------------------------------------------------------------//
		// describe	: 保存、载入
		// return	:
		//---------------------------------------------------------------------//
		bool load_file( TiXmlElement* pNode );
		bool save_file( TiXmlElement* pNode );

	//--------------------------------------------------------------------------
	// 控件属性
	//--------------------------------------------------------------------------
	protected:
		typedef std::vector<XUI_Wnd*>	CChildList;
		XUI_Wnd*		m_pParent;			// 父控件指针
		CChildList		m_pChildren;		// 子控件列表，最后的一个最靠前

		//名称，便于查找
		_string			m_strName;
		UINT			m_nID;				// 控件ID用于消息映射

		bool			m_bVisible;			// 是否可见
		bool			m_bEnable;			// 是否有效
		bool			m_bOwnerDraw;		// 是否自绘制

		bool			m_bFocused;			// 是否获取焦点
		bool			m_bTranslateParent;	// 是否将WM_COMMAND消息传给父控件

		float			m_fZ;
		CRect			m_wndRect;			// 位置和大小，相对于父控件

		FontAttribute	m_FontAttribute;	// 字体信息
		XUI_IFont*		m_pFont;

		XUI_Wnd*		m_pChildMouseOver;	// 当前鼠标下的子控件
		XUI_Wnd*		m_pChildFocusedOn;	// 当前获得焦点的子控件
	};

	extern 	CGuiSystem* _afxCurrentGuiSystem;

	typedef struct DrawStruct
	{
		ICanvas*	pCanvas;
		CRect		rcClient;
		CRect		rcClipper;
		XUI_Wnd*	pCtrl;
		LPARAM		lParam1;
		LPARAM		lParam2;
	}*LPDRAWSTRUCT, DRAWSTRUCT;
};
#endif

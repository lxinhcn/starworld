#ifndef XUI_Wnd_H
#define XUI_Wnd_H
#pragma once

#include "XUI_Base.h"
#include "XUI_Canvas.h"
#define DECLARE_LABLE( lable )\
	virtual _lpctstr GetLable()const { return _T(#lable); }\

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
		void Render(const xgcRect& clipper);

		//重绘，通过实现这个方法来表现空间的外观
		virtual void RenderSelf( const xgcPoint& adjust );
		virtual void Update( float timer, float delta );

		// 保存,载入
		virtual void OnSavePropertys( const char* name, TiXmlElement* pNode );
		virtual void OnLoadPropertys( const char* name, TiXmlElement* pNode );

		//virtual void OnDestroy();
		//鼠标
		//鼠标移动事件
		//参数说明：
		//pt，鼠标的坐标，相对于控件
		//sysKeys，各种重要按键的状态，参见MSDN	
		virtual	bool onMouseMove(const xgcPoint& pt, _uint32 sysKeys);
		virtual bool onMouseEnter();
		virtual bool onMouseLeave();

		//鼠标按键事件
		//参数说明：
		//button，按下的键，0-左键，1-右键，2-中键
		//pt，鼠标的坐标
		//sysKeys，各种重要按键的状态，参见MSDN
		virtual	bool onButtonDown(int button, const xgcPoint& pt, _uint32 sysKeys);
		virtual	bool onButtonUp(int button, const xgcPoint& pt, _uint32 sysKeys);

		//键盘
		//参数说明
		//keycode，按下的键
		//sysKeys，各种重要按键的状态，参见MSDN
		virtual	bool onKeyDown(_uint32 keycode, _uint32 sysKeys);
		virtual	bool onKeyUp(_uint32 keycode, _uint32 sysKeys);
		//参数说明
		//c，输入的字符
		//sysKeys，各种重要按键的状态，参见MSDN
		virtual bool onChar(_uint32 c, _uint32 sysKeys);

		//输入法
		//参见MSDN
		virtual bool onImeComp(_uint32 wParam, _uint32 lParam);
		virtual bool onImeEndComp(_uint32 wParam, _uint32 lParam);
		virtual bool onImeNotify(_uint32 wParam, _uint32 lParam);

		//获得焦点
		virtual void onGetFocus() {}
		//失去焦点
		virtual void onLostFocus() {}

		virtual void OnMoveWindow( const xgcRect& rcWindow ){}
	public:
		virtual _lpctstr GetLable()const = 0;

		// 发送消息
		long_ptr SendUIMessage( _uint32 nMsg, uint_ptr wParam, long_ptr lParam );

		void Release();

		//坐标转换
		void ScreenToClient( xgcPoint& pt) const;
		void ClientToScreen( xgcPoint& pt) const;
		void ScreenToClient( xgcRect& rc ) const;
		void ClientToScreen( xgcRect& rc ) const;

		//坐标修正，个别控件可以滚动，因此需要进行修正
		virtual void AdjustPoint( xgcPoint& pt, bool bClientToScreen )const { if( m_pParent ) m_pParent->AdjustPoint( pt, bClientToScreen ); }
		virtual void AdjustWindow( xgcRect& rc, bool bClientToScreen )const { if( m_pParent ) m_pParent->AdjustWindow( rc, bClientToScreen ); }

		//获取空间的显示区域
		const xgcRect	GetWindowRect()const { return xgcRect( m_WindowPosition, m_WindowSize ); }
		const xgcPoint& GetWindowPosition()const{ return m_WindowPosition; }
		const xgcSize&	GetWindowSize()const{ return m_WindowSize; }

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
		XUI_Wnd* FindChildInPoint( const xgcPoint &pt, _uint32* deep = NULL );
		//--------------------------------------------------------//
		//	created:	19:11:2009   18:29
		//	filename: 	d:\Develop\StarGame\GameSDK\GameUILib\Source\XUI_Wnd.cpp
		//	author:		Albert.xu
		//
		//	purpose:	查找能包容整个矩形的最深Wnd
		//--------------------------------------------------------//
		bool FindRectIn( const xgcRect &rc, std::list< XUI_Wnd* >& l );
		XUI_Wnd* FindChildByName( const _string& sName )const;
		XUI_Wnd* FindChildByID( int nID )const;

		typedef bool (*ElementCallback)( XUI_Wnd*, void* );
		XUI_Wnd* ForAllChild( ElementCallback pfnCallback, void* pData );
		size_t GetChildCount()const{ return m_pChildren.size(); }

		//--------------------------------------------------------------------------
		// 窗口大小的移动和调整
		//--------------------------------------------------------------------------
		
		// 重新设置空间的显示区域
		void MoveWindow(int left, int top, int right, int bottom, bool notify = true );
		//平移
		void Offset(int x, int y);

		void BringToUp();
		void BringToDown();
		void BringToFront();
		void BringToEnd();

		//检测某一点是否在控件的显示区域内
		virtual BOOL IsPointIn(const xgcPoint& pt);

		//--------------------------------------------------------------------------
		// 设置控件属性
		//--------------------------------------------------------------------------
		
		//名称
		void	SetName( const _string& lpszName)	{ m_strName = lpszName;	}
		const _string& GetName()const {return m_strName;}

		//显示
		bool	IsVisible()const {return m_bVisible;}
		void	ShowWindow( bool bVisible = true );

		bool	IsEnable()const { return m_bEnable; }
		void	EnableWindow( bool bEnable );

		//设置焦点
		void	SetFocus(bool bFocused);

		//---------------------------------------------------------------------//
		// describe	: 保存、载入
		// return	:
		//---------------------------------------------------------------------//
		bool load_file( TiXmlElement* pNode );
		bool save_file( TiXmlElement* pNode );
		void show_members( int indent );

	//--------------------------------------------------------------------------
	// 控件属性
	//--------------------------------------------------------------------------
	protected:
		typedef std::vector<XUI_Wnd*>	CChildList;
		XUI_Wnd*		m_pParent;			// 父控件指针
		CChildList		m_pChildren;		// 子控件列表，最后的一个最靠前

		//名称，便于查找
		_string			m_strName;
		std::string		m_strUpdateFunc;

		bool			m_bVisible;			// 是否可见
		bool			m_bEnable;			// 是否有效
		bool			m_bOwnerDraw;		// 是否自绘制

		bool			m_bFocused;			// 是否获取焦点
		bool			m_bTranslateParent;	// 是否将WM_COMMAND消息传给父控件

		float			m_fZ;
		xgcPoint		m_WindowPosition;		// 位置和大小，相对于父控件
		xgcSize			m_WindowSize;

		XUI_IFont*		m_pFont;
		XUI_ISprite*	m_pBackGround;			// 背景图
	};

	extern 	CGuiSystem* _afxCurrentGuiSystem;

	typedef struct DrawStruct
	{
		ICanvas*	pCanvas;
		xgcRect		rcClient;
		xgcRect		rcClipper;
		XUI_Wnd*	pCtrl;
		LPARAM		lParam1;
		LPARAM		lParam2;
	}*LPDRAWSTRUCT, DRAWSTRUCT;
};
#endif

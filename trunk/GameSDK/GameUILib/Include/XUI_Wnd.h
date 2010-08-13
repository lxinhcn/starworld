#ifndef XUI_Wnd_H
#define XUI_Wnd_H
#pragma once

#include "XUI_Object.h"
#include "XUI_Canvas.h"

namespace XGC
{
	namespace ui
	{
		struct XUI_Font;
		class XUI_Wnd	:	public XUI_Object
		{
			friend class XUI_System;
			friend class XUI_Factory;
			friend class XUI_Lua;
		public:
			enum GUI_ELEMENT_FLAG
			{
				FLAGS_EDIT = 0,
			};

		protected:
			XUI_Wnd(void);
			virtual ~XUI_Wnd(void);

			void Destroy();
		protected:
			//绘制
			void Render(const iRect& AreaRect );
			void RenderEdit( const iPoint &adjust );

			//重绘，通过实现这个方法来表现空间的外观
			virtual void RenderSelf( const iPoint& adjust );
			virtual void Update( float timer, float delta );

			// 保存,载入
			virtual void OnSavePropertys( const char* name, TiXmlElement* pNode );
			virtual void OnLoadPropertys( const char* name, TiXmlElement* pNode );

			virtual	bool OnMouseMove(const iPoint& pt, _uint32 sysKeys){ return false; }
			virtual bool OnMouseEnter(){ return false; }
			virtual bool OnMouseLeave(){ return false; }

			//鼠标按键事件
			//参数说明：
			//button，按下的键，0-左键，1-右键，2-中键
			//pt，鼠标的坐标
			//sysKeys，各种重要按键的状态，参见MSDN
			virtual	bool OnButtonDown(int button, const iPoint& pt, _uint32 sysKeys){ return false; }
			virtual	bool OnButtonUp(int button, const iPoint& pt, _uint32 sysKeys){ return false; }

			//键盘
			//参数说明
			//keycode，按下的键
			//sysKeys，各种重要按键的状态，参见MSDN
			virtual	bool OnKeyDown(_uint32 keycode, _uint32 sysKeys){ return false; }
			virtual	bool OnKeyUp(_uint32 keycode, _uint32 sysKeys){ return false; }
			//参数说明
			//c，输入的字符
			//sysKeys，各种重要按键的状态，参见MSDN
			virtual bool OnChar(_uint32 c, _uint32 sysKeys){ return false; }

			//输入法
			//参见MSDN
			virtual bool OnImeComp(_uint32 wParam, _uint32 lParam){ return false; }
			virtual bool OnImeEndComp(_uint32 wParam, _uint32 lParam){ return false; }
			virtual bool OnImeNotify(_uint32 wParam, _uint32 lParam){ return false; }

			//获得焦点
			virtual void OnSetFocus() {}
			//失去焦点
			virtual void OnLostFocus() {}

			virtual void OnMoveWindow( const iRect& rcWindow ){}
		public:
			void Release();

			//坐标转换
			void ScreenToClient( iPoint& pt) const;
			void ClientToScreen( iPoint& pt) const;
			void ScreenToClient( iRect& rc ) const;
			void ClientToScreen( iRect& rc ) const;

			//坐标修正，个别控件可以滚动，因此需要进行修正
			virtual void AdjustPoint( iPoint& pt, bool bClientToScreen )const { if( m_pParent ) m_pParent->AdjustPoint( pt, bClientToScreen ); }
			virtual void AdjustWindow( iRect& rc, bool bClientToScreen )const { if( m_pParent ) m_pParent->AdjustWindow( rc, bClientToScreen ); }

			//获取空间的显示区域
			const iRect		GetWindowRect()const { return iRect( m_WindowPosition, m_WindowSize ); }
			const iPoint	GetWindowPosition()const{ return m_WindowPosition; }
			const iSize		GetWindowSize()const{ return m_WindowSize; }

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
			XUI_Wnd* FindChildInPoint( const iPoint &pt );

			//--------------------------------------------------------//
			//	created:	19:11:2009   18:29
			//	filename: 	d:\Develop\StarGame\GameSDK\GameUILib\Source\XUI_Wnd.cpp
			//	author:		Albert.xu
			//
			//	purpose:	查找能包容整个矩形的最深Wnd
			//--------------------------------------------------------//
			bool FindRectIn( const iRect &rc, std::list< XUI_Wnd* >& l );
			XUI_Wnd* FindChildByName( const _string& sName )const;
			XUI_Wnd* FindChildByID( int nID )const;

			typedef bool (*ElementCallback)( XUI_Wnd*, void* );
			XUI_Wnd* ForAllChild( ElementCallback pfnCallback, void* pData );
			_uint32 GetChildCount()const{ return m_pChildren.size(); }

			//--------------------------------------------------------------------------
			// 窗口大小的移动和调整
			//--------------------------------------------------------------------------

			// 重新设置空间的显示区域
			void MoveWindow(int left, int top, int right, int bottom, bool notify = true );
			//平移
			void Offset( int x, int y );

			void BringToUp();
			void BringToDown();
			void BringToFront();
			void BringToEnd();

			//检测某一点是否在控件的显示区域内
			virtual bool IsPointIn(const iPoint& pt);

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

			//--------------------------------------------------------//
			//	created:	25:11:2009   10:48
			//	filename: 	d:\Developed\StarGame\GameSDK\GameUILib\Include\XUI_Wnd.h
			//	author:		Albert.xu
			//
			//	purpose:	标志操作
			//--------------------------------------------------------//
			void SetFlags( _uint16 nFlag );	// 设置标志
			void ClrFlags( _uint16 nFlag );	// 清除标志
			bool GetFlags( _uint16 nFlag )const; // 获取标志

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
			iPoint			m_WindowPosition;		// 位置和大小，相对于父控件
			iSize			m_WindowSize;

			XUI_Font*		m_pFont;
			XUI_ISprite*	m_pBackGround;			// 背景图
			_byte			m_Flags[1];				// 标志
		};

		extern 	XUI_System* _afxCurrentGuiSystem;
	}
};
#endif

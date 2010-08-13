#ifndef GUISYSTEM_H
#define GUISYSTEM_H

#pragma once
#include "XUI_Wnd.h"
#include "timer_manager.h"
#include "singleton.h"

//界面系统，主要负责消息的转发
#define DEFAULT_DESKTOP			0x10000
#define TIMER_PRECISION			(0.1f)
#define TIMER_SECOND(second)	int(second*1.0f/TIMER_PRECISION)
namespace XGC
{
	namespace ui
	{
		struct ICanvas;
		class XUI_Window;
		class XUI_System
		{
			friend struct CreateUsingNew< XUI_System >;
			friend class XUI_Lua;
		private:
			XUI_System();
			XUI_System( CONST XUI_System& src );
			virtual ~XUI_System(void);

		public:
			bool Initialize( HWND hWnd, _lpcstr lpszPath );
			void Unitialize();

			void SetEditMode( bool bMode );
			bool IsEditMode()const{ return m_is_edit_mode; }

			// 设置图片路径
			_lpcstr	GetResourcePath();

			void SetDefaultFont( XUI_Font *pFont ){ m_default_font_ptr	= pFont; }
			void SetDefaultInput( XUI_Input *pInput){ m_pInput = pInput; }

			XUI_Font*	GetDefaultFont()const{ return m_default_font_ptr; }
			XUI_Input*	GetInput()const{ return m_pInput; }

		protected:

			// 是否初始化
			bool m_bInitialized;
			bool m_bPointInRoot;

			//对应的窗口
			HWND		m_hWnd;
			WNDPROC		m_OldProc;
			iSize		m_WindowSize;
			iPoint		m_mouse_old;

		private:
			//根
			typedef std::map< int, XUI_Window* >	CDesktopMap;
			typedef std::list< XUI_Window* >		CModalList;
			typedef std::list< XUI_Wnd* >			CWndList;

			XUI_Window		*m_pDesktop;
			CDesktopMap		m_DesktopMap;
			CModalList		m_ModalList;

			XUI_Font		*m_default_font_ptr;
			XUI_Input		*m_pInput;

			std::string		m_resource_path;
			timer			m_timer;
			float			m_nowtime;
			float			m_timer_anchor;
			bool			m_is_edit_mode;
			int				m_current_handle;
			iPoint			m_mousedown;
			XUI_Wnd			*m_capture_element, *m_mouseover_element, *m_mousedown_element;
			CWndList		m_capture_list;
		protected:
			void SetFocus		( XUI_Wnd* pElement );

			//消息处理，保证消息可以按照预定的方式转发
			bool OnMouseMove	( XUI_Wnd* pElement, const iPoint& pt, _uint32 sysKeys, long_ptr *result );
			bool OnMouseLeave	( XUI_Wnd* pElement );

			bool OnButtonDown	( XUI_Wnd* pElement, _uint32 nButton, const iPoint& pt, _uint32 sysKeys, long_ptr *result );
			bool OnButtonUp		( XUI_Wnd* pElement, _uint32 nButton, const iPoint& pt, _uint32 sysKeys, long_ptr *result );

			bool OnKeyDown		( XUI_Wnd* pElement, _uint32 dwVirtualCode, _uint32 sysKeys, long_ptr *result );
			bool OnKeyUp		( XUI_Wnd* pElement, _uint32 dwVirtualCode, _uint32 sysKeys, long_ptr *result );
			bool OnChar			( XUI_Wnd* pElement, _uint32 dwChar, _uint32 sysKeys, long_ptr *result );

			bool OnImeComp		( XUI_Wnd* pElement, int_ptr wParam, long_ptr lParam, long_ptr *result );
			bool OnImeEndComp	( XUI_Wnd* pElement, int_ptr wParam, long_ptr lParam, long_ptr *result );
			bool OnImeNotify	( XUI_Wnd* pElement, int_ptr wParam, long_ptr lParam, long_ptr *result );

			bool HandleMouse	( _uint32 uMsg, int_ptr wParam, long_ptr lParam, long_ptr *result );
			bool HandleKeyboard	( _uint32 uMsg, int_ptr wParam, long_ptr lParam, long_ptr *result );

		public:
			HWND GetHWND()const {return m_hWnd;}

			XUI_Window* GetRoot() {return m_pDesktop;}

			//渲染
			void Render();
			void RenderEditFrame();
			_uint32 DetectHandler(  XUI_Wnd* pElement, const iPoint &pt );


			//处理外部消息
			LRESULT HandleMessage( HWND hWnd, _uint32 uMsg, int_ptr wParam, long_ptr lParam );

			void Update( float fDelta );
			//////////////////////////////////////////////////////////////////////////
			// 生成界面

			void RegistDesktop( XUI_Window* pDesktop );
			void SwitchDesktop( int nDesktopID );
			XUI_Window* RemoveDesktop( int nDesktopID );
			XUI_Window* GetDesktop( UINT nDesktopID );

			bool EnterModaless( XUI_Window* pDialog );
			void LeaveModaless();

			bool LoadFromFile( _lpcstr pszFilename );
			bool SaveToFile( _lpcstr pszFilename );

			unsigned int SetTimer( _tfunction function, unsigned short repeat, unsigned short timer );
			void KillTimer( unsigned int handle );
		};

		extern bool SetupDebuger();
		inline unsigned int GetLongevity( XUI_System* ){ return 20; }
		typedef SingletonHolder< XUI_System, CreateUsingNew, SingletonWithLongevity > XUI;
	}
}

#endif
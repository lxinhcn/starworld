#ifndef GUISYSTEM_H
#define GUISYSTEM_H

#pragma once
#include "XUI_Wnd.h"
#include "timer_manager.h"
#include <loki/Singleton.h>

//界面系统，主要负责消息的转发
#define DEFAULT_DESKTOP			0x10000
#define TIMER_PRECISION			(0.1f)
#define TIMER_SECOND(second)	int(second*1.0f/TIMER_PRECISION)
namespace UILib
{
	struct ICanvas;
	class XUI_Window;
	class CGuiSystem
	{
	friend struct Loki::CreateUsingNew< CGuiSystem >;
	friend class LuaBindClass;
	private:
		CGuiSystem();
		CGuiSystem( CONST CGuiSystem& src );
		virtual ~CGuiSystem(void);

	public:
		bool Initialize( HWND hWnd, _lpcstr lpszPath );
		void Unitialize();

		void SetEditMode( bool bMode );
		bool IsEditMode()const{ return m_is_edit_mode; }

		// 设置图片路径
		_lpcstr	GetResourcePath();

		void SetDefaultFont( XUI_IFont *pFont ){ m_default_font_ptr	= pFont; }
		void SetDefaultCursor( XUI_IMouse *pCursor ){ m_cursor_ptr = pCursor; }

		XUI_IFont*	GetDefaultFont()const{ return m_default_font_ptr; }
		XUI_IMouse*	GetMouseCursor()const{ return m_cursor_ptr; }

	protected:

		// 是否初始化
		bool m_bInitialized;
		bool m_bPointInRoot;

		//对应的窗口
		HWND		m_hWnd;
		xgcSize		m_windowsize;
		xgcPoint	m_mouse_old;

	private:
		//根
		typedef std::map< int, XUI_Window* >	CDesktopMap;
		typedef std::list< XUI_Window* >		CModalList;
		typedef std::list< XUI_Wnd* >			CWndList;

		XUI_Window		*m_pDesktop;
		CDesktopMap		m_DesktopMap;
		CModalList		m_ModalList;

		XUI_IFont		*m_default_font_ptr;
		XUI_IMouse		*m_cursor_ptr;

		std::string		m_resource_path;
		CTimerManager	m_timer;
		float			m_nowtime;
		float			m_timer_anchor;
		bool			m_is_edit_mode;
		int				m_current_handle;
		xgcPoint		m_mousedown;
		XUI_Wnd*		m_capture_element;
		XUI_Wnd*		m_mouseover_element;
		CWndList		m_capture_list;
	protected:
		void SetFocus		( XUI_Wnd* pElement );

		//消息处理，保证消息可以按照预定的方式转发
		bool onMouseMove	( XUI_Wnd* pElement, const xgcPoint& pt, _uint32 sysKeys, long_ptr *result );
		bool onMouseLeave	( XUI_Wnd* pElement );
		
		bool onButtonDown	( XUI_Wnd* pElement, _uint32 nButton, const xgcPoint& pt, _uint32 sysKeys, long_ptr *result );
		bool onButtonUp		( XUI_Wnd* pElement, _uint32 nButton, const xgcPoint& pt, _uint32 sysKeys, long_ptr *result );

		bool onKeyDown		( XUI_Wnd* pElement, _uint32 dwVirtualCode, _uint32 sysKeys, long_ptr *result );
		bool onKeyUp		( XUI_Wnd* pElement, _uint32 dwVirtualCode, _uint32 sysKeys, long_ptr *result );
		bool onChar			( XUI_Wnd* pElement, _uint32 dwChar, _uint32 sysKeys, long_ptr *result );

		bool onImeComp		( XUI_Wnd* pElement, int_ptr wParam, long_ptr lParam, long_ptr *result );
		bool onImeEndComp	( XUI_Wnd* pElement, int_ptr wParam, long_ptr lParam, long_ptr *result );
		bool onImeNotify	( XUI_Wnd* pElement, int_ptr wParam, long_ptr lParam, long_ptr *result );

		bool HandleMouse	( _uint32 uMsg, int_ptr wParam, long_ptr lParam, long_ptr *result );
		bool HandleKeyboard	( _uint32 uMsg, int_ptr wParam, long_ptr lParam, long_ptr *result );

	public:
		HWND GetHWND()const {return m_hWnd;}
	    
		XUI_Window* GetRoot() {return m_pDesktop;}

		//渲染
		void Render();
		void RenderEditFrame();
		_uint32 DetectHandler(  XUI_Wnd* pElement, const xgcPoint &pt );

		
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

		unsigned int SetTimer( TimerFunction function, unsigned short repeat, unsigned short timer );
		void KillTimer( unsigned int handle );
	};

	extern bool SetupDebuger();
	inline unsigned int GetLongevity( CGuiSystem* ){ return 20; }
	typedef Loki::SingletonHolder< CGuiSystem, Loki::CreateUsingNew, Loki::SingletonWithLongevity > GuiSystem;
}

#endif
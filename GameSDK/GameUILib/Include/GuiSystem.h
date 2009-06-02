#ifndef GUISYSTEM_H
#define GUISYSTEM_H

#pragma once
#include "XUI_Wnd.h"
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
		bool Initialize( HWND hWnd, _lpcstr lpszPath, const XUI_FontAttribute& fontAttr, XUI_IMouse* pCursor );
		void Unitialize();

		void SetEditMode( bool bMode );
		bool IsEditMode()const{ return m_bEditMode; }

		// 设置图片路径
		_lpcstr	GetResourcePath();

		XUI_IFont*	GetDefaultFont()const{ return m_pDefaultFont; }
		XUI_IMouse*	GetMouseCursor()const{ return m_pCursor; }
	protected:

		// 是否初始化
		bool m_bInitialized;
		bool m_bPointInRoot;

		//对应的窗口
		HWND m_hWnd;
		x_size m_windowsize;

	private:
		//根
		typedef std::map< int, XUI_Window* >	CDesktopMap;
		typedef std::list< XUI_Window* >		CModalList;

		XUI_Window		*m_pDesktop;
		CDesktopMap		m_DesktopMap;
		CModalList		m_ModalList;

		XUI_IFont		*m_pDefaultFont;
		XUI_IMouse		*m_pCursor;

		std::string		m_resource_path;
		xtimer			m_timer;
		float			m_nowtime;
		float			m_timer_anchor;
		bool			m_bEditMode;
		x_point			m_mousedown;
		XUI_Wnd*		m_capture_element;
		XUI_Wnd*		m_mouseover_element;
	protected:
		void SetFocus		( XUI_Wnd* pElement );

		//消息处理，保证消息可以按照预定的方式转发
		bool onMouseMove	( XUI_Wnd* pElement, const x_point& pt, uint32 sysKeys, long_ptr *result );
		bool onMouseLeave	( XUI_Wnd* pElement );
		//bool onButtonDbClick( XUI_Wnd* pElement, uint32 nButton, const x_point& pt, uint32 sysKeys, long_ptr *result );
		bool onButtonDown	( XUI_Wnd* pElement, uint32 nButton, const x_point& pt, uint32 sysKeys, long_ptr *result );
		bool onButtonUp		( XUI_Wnd* pElement, uint32 nButton, const x_point& pt, uint32 sysKeys, long_ptr *result );

		bool onKeyDown		( XUI_Wnd* pElement, uint32 dwVirtualCode, uint32 sysKeys, long_ptr *result );
		bool onKeyUp		( XUI_Wnd* pElement, uint32 dwVirtualCode, uint32 sysKeys, long_ptr *result );
		bool onChar			( XUI_Wnd* pElement, uint32 dwChar, uint32 sysKeys, long_ptr *result );

		bool onImeComp		( XUI_Wnd* pElement, uint32 wParam, uint32 lParam, long_ptr *result );
		bool onImeEndComp	( XUI_Wnd* pElement, uint32 wParam, uint32 lParam, long_ptr *result );
		bool onImeNotify	( XUI_Wnd* pElement, uint32 wParam, uint32 lParam, long_ptr *result );

		bool HandleMouse	( UINT uMsg, WPARAM wParam, LPARAM lParam, long_ptr *result );
		bool HandleKeyboard	( UINT uMsg, WPARAM wParam, LPARAM lParam, long_ptr *result );

	public:
		HWND GetHWND()const {return m_hWnd;}
	    
		XUI_Window* GetRoot() {return m_pDesktop;}

		//渲染
		void Render();
		void RenderEditFrame( const x_rect &rc );
		uint32 DetectHandler( const x_rect &rc, const x_point &pt );

		
		//处理外部消息
		LRESULT HandleMessage( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

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

		unsigned int SetTimer( event_function function, unsigned short repeat, unsigned short timer );
		void KillTimer( unsigned int handle );
	};

	extern bool SetupDebuger();
	inline unsigned int GetLongevity( CGuiSystem* ){ return 20; }
	typedef Loki::SingletonHolder< CGuiSystem, Loki::CreateUsingNew, Loki::SingletonWithLongevity > GuiSystem;
}

#endif
#ifndef GUISYSTEM_H
#define GUISYSTEM_H

#pragma once
#include "XUI_Wnd.h"
#include <loki/Singleton.h>

//����ϵͳ����Ҫ������Ϣ��ת��
#define DEFAULT_DESKTOP			0x10000
#define TIMER_PRECISION			(0.1f)
#define TIMER_SECOND(second)	(second*1.0f/TIMER_PRECISION);
namespace UILib
{
	struct ICanvas;
	class XUI_Window;
	class CGuiSystem
	{
	friend struct Loki::CreateStatic< CGuiSystem >;
	private:
		CGuiSystem();
		CGuiSystem( CONST CGuiSystem& src );
		virtual ~CGuiSystem(void);

	public:
		BOOL	Initialize( HWND hWnd, XUI_IFont* pDefaultFont );

		// ����ͼƬ·��
		void	SetImagePath( LPCTSTR lpszImagePath );
		LPCTSTR	GetImagePath();

		XUI_IFont*	GetDefaultFont()const{ return m_pDefaultFont; }

	protected:

		// �Ƿ��ʼ��
		bool m_bInitialized;
		bool m_bPointInRoot;

		//��Ӧ�Ĵ���
		HWND m_hWnd;

	private:
		//��
		typedef std::map< int, XUI_Window* >	CDesktopMap;
		typedef std::list< XUI_Window* >		CModalList;

		XUI_Window*	m_pDesktop;
		CDesktopMap	m_DesktopMap;
		CModalList	m_ModalList;

		XUI_IFont*	m_pDefaultFont;
		_string		m_strMediaPath;
		xtimer		m_timer;
		float		m_nowtime;
		float		m_timer_anchor;
	protected:
		void SetFocus(XUI_Wnd* pElement);

		//��Ϣ��������֤��Ϣ���԰���Ԥ���ķ�ʽת��
		bool onMouseMove(XUI_Wnd* pElement, const CPoint& pt, UINT sysKeys);
		bool onMouseLeave(XUI_Wnd* pElement);
		bool onMouseHover(XUI_Wnd* pElement, const CPoint& pt);
		bool onButtonDown(XUI_Wnd* pElement, int iButton, const CPoint& pt, UINT sysKeys);
		bool onButtonUp(XUI_Wnd* pElement, int iButton, const CPoint& pt, UINT sysKeys);

		bool onKeyDown(XUI_Wnd* pElement, DWORD dwVirtualCode, UINT sysKeys);
		bool onKeyUp(XUI_Wnd* pElement, DWORD dwVirtualCode, UINT sysKeys);
		bool onChar(XUI_Wnd* pElement, DWORD dwChar, UINT sysKeys);

		bool onImeComp(XUI_Wnd* pElement, DWORD wParam, DWORD lParam);
		bool onImeEndComp(XUI_Wnd* pElement, DWORD wParam, DWORD lParam);
		bool onImeNotify(XUI_Wnd* pElement, DWORD wParam, DWORD lParam);

		void HandleMouse(UINT uMsg, WPARAM wParam, LPARAM lParam);
		void HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam);
		void HandleIMEMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);

	public:
		HWND GetHWND() {return m_hWnd;}
	    
		XUI_Window* GetRoot() {return m_pDesktop;}

		//��Ⱦ
		void Render();
		
		//�����ⲿ��Ϣ
		void HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam );

		void Update( float fDelta );
		//////////////////////////////////////////////////////////////////////////
		// ���ɽ���

		void RegistDesktop( XUI_Window* pDesktop );
		void SwitchDesktop( int nDesktopID );
		XUI_Window* RemoveDesktop( int nDesktopID );
		XUI_Window* GetDesktop( UINT nDesktopID );

		bool EnterModaless( XUI_Window* pDialog );
		void LeaveModaless();

		bool LoadFromFile( LPCTSTR pszFilename );
		bool SaveToFile( LPCTSTR pszFilename );

		unsigned int SetTimer( event_function function, unsigned short repeat, unsigned short timer );
		void KillTimer( unsigned int handle );
	};

	typedef Loki::SingletonHolder< CGuiSystem, Loki::CreateStatic > GuiSystem;
}

#endif
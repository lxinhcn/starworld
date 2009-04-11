#pragma once
#include "XUI_Window.h"

namespace UILib
{
	//框架控件，不显示任何内容，只是作为容器
	class CGuiSystem;
	class CXDialog:	public	UIObjTypeT< XUI_Window, TypeDialog >
	{
	DECLARE_UIMSG_MAP()
	DECLARE_LABLE( DIALOG )
	public:
		CXDialog( _lpctstr lpszTemplate = NULL );
		~CXDialog();

		bool Create( _lpctstr strTemplate, XUI_Window* pParent = NULL );

		typedef BOOL (*fnModalMsgProc)( BOOL bIdle );
		UINT DoModal( fnModalMsgProc pfn );

	protected:
		XUI_Wnd*	PreModal();
		void		BeginModalLoop( fnModalMsgProc pfn );
		void		PostModal();
		void		EndModalLoop();
		void		EndDialog( UINT nResult );
		void		OnOK();
		void		OnCancel();
		void		CenterWindow();
		void		SetFocus( XUI_Wnd* pFocus );
		void		SetFocus( UINT nCtrlID );
	protected:
		virtual void	RenderSelf(const CRect& clipper);
		virtual bool	onKeyUp(uint32 keycode, UINT sysKeys);

		virtual bool	OnInitDialog(){ return true;}
		virtual void	OnDestroy(){}
		virtual LRESULT	DefMsgProc( UINT nMsg, WPARAM wParam, LPARAM lParam );

	private:
		bool			m_bModal;
		bool			m_bEnableParent;
		UINT			m_nResult;
		_string			m_strTemplate;
	};
}
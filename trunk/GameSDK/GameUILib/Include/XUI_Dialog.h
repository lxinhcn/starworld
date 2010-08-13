#pragma once
#include "XUI_Window.h"

namespace XGC
{
	namespace ui
	{
		//框架控件，不显示任何内容，只是作为容器
		class XUI_System;
		class XUI_Dialog:	public XUI_Window
		{
		public:
			XUI_Dialog( _lpctstr lpszTemplate = NULL );
			~XUI_Dialog();

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
			void		SetFocus( UINT nCtrlID );
		protected:
			virtual void	RenderSelf( const iPoint& adjust );
			virtual bool	onKeyUp( _uint32 keycode, _uint32 sysKeys );

			virtual bool	OnInitDialog(){ return true;}
			virtual void	OnDestroy(){}

		private:
			bool			m_bModal;
			bool			m_bEnableParent;
			UINT			m_nResult;
			_string			m_strTemplate;
		};
	}
}
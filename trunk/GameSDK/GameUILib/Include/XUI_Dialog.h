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
			XUI_Dialog( _lpctstr lpszTemplate = NULL, XUI_Window *pParent = NULL );
			~XUI_Dialog();

			bool	Create();
			int		DoModal();

		protected:
			XUI_Wnd*	PreModal();
			void		BeginModalLoop();
			void		PostModal();
			void		EndModalLoop();
			void		EndDialog( UINT nResult );
			void		OnOK();
			void		OnCancel();
			void		CenterWindow();
			void		SetFocus( UINT nCtrlID );
		protected:
			virtual bool	OnInitDialog(){ return true;}
			virtual void	OnDestroy(){}

			virtual bool	DefMessageProc( MSG &msg ){ return false; }
		private:
			bool			m_bModal;
			bool			m_bEnableParent;
			int				m_nResult;

			_string			m_strTemplate;
			XUI_Window*		m_pParent;
		};
	}
}
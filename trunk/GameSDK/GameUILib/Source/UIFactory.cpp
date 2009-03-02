#include "GuiHeader.h"
#include "XUI_Button.h"
#include "XUI_Dialog.h"
#include "XUI_Editbox.h"
//#include "XUI_Picture.h"
//#include "ScrollBar.h"
#include "XUI_Window.h"
//#include "XUI_EditBox.h"
//#include "ProgressBar.h"
//#include "MultiPicture.h"
//#include "Static.h"
//#include "RenderTarget.h"
//#include "ListBox.h"
#include "uifactory.h"

namespace UILib
{
	CUIFactory& CUIFactory::GetInstance()
	{
		static CUIFactory factory;
		return factory;
	}

	REGIST_CONTROL( UILib::XUI_Window,		WINDOW		);
	REGIST_CONTROL( UILib::CXDialog,		DIALOG		);
	REGIST_CONTROL( UILib::XUI_Button,		BUTTON		);
	//REGIST_CONTROL( UILib::CPicture,		IMAGE		);
	//REGIST_CONTROL( UILib::CScrollBar,	SCROLLBAR	);
	REGIST_CONTROL( UILib::CXEditBox,		EDITBOX		);
	//REGIST_CONTROL( UILib::CProgressBar,	PROGRESSBAR );
	//REGIST_CONTROL( UILib::CMultiPicture,	MULTIPICTURE );
	//REGIST_CONTROL( UILib::CStatic,		STATIC );
	//REGIST_CONTROL( UILib::CRenderTarget,	RENDERTARGET );
	//REGIST_CONTROL( UILib::CListBox,		LISTBOX );
	// 注册前先包含头文件
};
#include "XUI_Header.h"
#include "XUI_Factory.h"

#include "XUI.h"
#define REGISTER_CONTROL(CLASSNAME)	static XUI_ControlRegister< CLASSNAME > _##CLASSNAME##_

namespace XGC
{
	namespace ui
	{
		REGISTER_CONTROL(XUI_Button);
		REGISTER_CONTROL(XUI_Dialog);
		REGISTER_CONTROL(XUI_EditBox);
		REGISTER_CONTROL(XUI_Window);

		XUI_Factory& XUI_Factory::GetInstance()
		{
			static XUI_Factory factory;
			return factory;
		}

	}
};
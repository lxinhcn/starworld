#pragma once
#include "XUI_Wnd.h"

namespace UILib
{
	struct	XUI_ISprite;
	struct	ICanvas;
	class XUI_Picture : public UIObjTypeT< XUI_Wnd, TypePicture >
	{
		DECLARE_LABLE( PICTURE )
	public:
		XUI_Picture(void);
		~XUI_Picture(void);

	protected:
		virtual void RenderSelf();
	};
};

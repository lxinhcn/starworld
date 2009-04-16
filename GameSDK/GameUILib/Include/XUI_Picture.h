#pragma once
#include "XUI_Wnd.h"

namespace UILib
{
	struct	XUI_ISprite;
	struct	ICanvas;
	class CPicture : public UIObjTypeT< XUI_Wnd, TypePicture >
	{
		DECLARE_LABLE( PICTURE )
	public:
		CPicture(void);
		~CPicture(void);

	protected:
		virtual void RenderSelf();

	private:
		XUI_ISprite*		m_pBkTex;			// ±³¾°ÌùÍ¼
	};
};

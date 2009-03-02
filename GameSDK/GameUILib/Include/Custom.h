#pragma once
#include "UIElement.h"
namespace UILib
{
	class CCustom	:	public	CUIElement
	{
	public:
		CCustom(void);
		~CCustom(void);

		virtual LPCSTR	GetLable(){ return m_strLable.c_str(); }
		virtual void	RenderSelf(const CUIRect& clipper){}
	private:

		std::string		m_strLable;
	};
}

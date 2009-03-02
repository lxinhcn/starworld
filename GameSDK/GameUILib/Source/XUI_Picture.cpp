#include "GuiHeader.h"
#include "XUI_Picture.h"

namespace UILib
{
	CPicture::CPicture(void)
	{
	}

	CPicture::~CPicture(void)
	{
	}

	void CPicture::RenderSelf( const CRect& clipper )
	{
		if( m_pBkTex )
		{
			CPoint pt( 0, 0 );
			pt = ClientToScreen(pt);
			int iWidth	= m_wndRect.Width();
			int iHeight	= m_wndRect.Height();
			int nImageWidth = 0;
			int nImageHeight = 0;

			XUI_DrawSprite( m_pBkTex, pt.x, pt.y, iWidth, iHeight, clipper );
		}
	}
}

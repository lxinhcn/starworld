#ifndef PANEL_H
#define PANEL_H

#include "XUI_Picture.h"
#include "GuiMacroDefine.h"

//////////////////////////////////////////////////////////////////////////
// Panel 为控件容器，同时邦定滚动条控件。
class TiXmlNode;
namespace UILib
{
	class Canvas;
	class XUI_Window :    public UIObjTypeT< XUI_Wnd, TypeWindow >
	{
		DECLARE_UIMSG_MAP()
		DECLARE_LABLE( WINDOW )
		friend class LuaBindClass;
	public:
		XUI_Window(void);
		~XUI_Window(void);

	public:
		virtual x_point AdjustPoint(const x_point& pt, bool bClientToScreen) const;

		// Panel大小
		int		GetWidth()const		{ return m_nPanelWidth; }
		int		GetHeight()const	{ return m_nPanelHeight; }

		// 通过指定的节点创建子控件。
		bool CreateFromXMLNode( TiXmlNode* pNode );

		// 存储到指定节点
		bool SaveToXMLNode( TiXmlNode* pNode );

	protected:
		virtual void RenderSelf(const x_rect& clipper){};
		void	BindScrollBar();
		int		ScrollVerticle(int iOffset);
		int		ScrollHorizontal(int iOffset);

		// uimsg
		BOOL OnScrollCreate( WPARAM wParam, LPARAM lParam );
		BOOL OnScrollPosChange( WPARAM wParam, LPARAM lParam );
	protected:
		int m_nOffsetX;
		int m_nOffsetY;
		int m_nPanelWidth;	// 框架容纳的宽度
		int m_nPanelHeight;	// 框架容纳的高度

	private:
		int m_nVScrollID;
		int m_nHScrollID;

		//CScrollBar*	m_pVScrollBar;	// 垂直方向的滚动条
		//CScrollBar* m_pHScrollBar;	// 水平方向的滚动条
	};
}
#endif

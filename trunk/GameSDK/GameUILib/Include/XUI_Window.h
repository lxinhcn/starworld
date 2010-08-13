#ifndef PANEL_H
#define PANEL_H
#include "XUI_Wnd.h"
//////////////////////////////////////////////////////////////////////////
// Panel 为控件容器，同时邦定滚动条控件。
class TiXmlNode;
namespace XGC
{
	namespace ui
	{
		class XUI_Window :    public XUI_Wnd
		{
			friend class XUI_Lua;
		public:
			XUI_Window(void);
			~XUI_Window(void);

		public:
			virtual void AdjustPoint( iPoint& pt, bool bClientToScreen )const;
			virtual void AdjustPoint( iRect& pt, bool bClientToScreen )const;

			// Panel大小
			int GetWidth()const	{ return m_nPanelWidth; }
			int GetHeight()const	{ return m_nPanelHeight; }

			// 通过指定的节点创建子控件。
			bool CreateFromXMLNode( TiXmlNode* pNode );

			// 存储到指定节点
			bool SaveToXMLNode( TiXmlNode* pNode );

		protected:
			int m_nOffsetX;
			int m_nOffsetY;
			int m_nPanelWidth;	// 框架容纳的宽度
			int m_nPanelHeight;	// 框架容纳的高度
		};
	}
}
#endif

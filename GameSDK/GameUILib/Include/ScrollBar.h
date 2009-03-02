#ifndef SCROLLBAR_H
#define SCROLLBAR_H

#include "UIElement.h"
#include "Texture.h"
#define SCROLL_BTN_DELAY 500
namespace UILib
{
	// class CPanel;
	struct NMHDR_SCROLL
	{
		NMUIHDR hdr;
		int		pos;
		int		oldpos;
	};

	class CButton;
	class CScrollBar	:	public XObjTypeT< CUIElement, TypeScroll >
	{
	DECLARE_UIMSG_MAP();
	public:
		enum	Direction{ Vertical, Horizontal };
		CScrollBar(void);
		~CScrollBar(void);

		// 基础属性设置
		void	SetRange( int nMin, int nMax );
		void	SetPosition( int nPosition );
		void	SetPageSize( int nPageSize );

		int		GetPosition(){ return m_nPosition; }
		int		GetPageSize(){ return m_nPageSize; }

		// 滚动滑块
		void	Scroll( int nDelta );

		// 设置滚动方向
		void		SetDirection( Direction enDirection ){ m_enDirection = enDirection; }
		Direction	GetDirection(){ return m_enDirection; }

		// 重载的虚方法
		virtual const char* GetLable(){ return "SCROLLBAR"; }

		// 从节点创建滚动条对象
		virtual void	LoadProperty( TiXmlElement* pXMLElement );
		virtual void	SaveProperty( TiXmlElement* pXMLElement );

		// 编辑器接口
		virtual bool	SetProperty(const char* pszPropertyName, const char* pszValue );
		virtual bool	GetProperty(const char* pszPropertyName, char* pValueBuffer, int size );

	protected:
		// 更新滑块位置
		void	UpdateThumb();
		// 更新位置信息
		void	ReCalc();

		virtual bool onButtonDown(int button, const CUIPoint& pt, UINT sysKeys);
		virtual bool onMouseMove(const CUIPoint& pt, UINT sysKeys);
		virtual bool onButtonUp(int button, const CUIPoint& pt, UINT sysKeys);
		
		virtual void RenderSelf(const CUIRect& clipper);
		
		uimsg void OnSize( int nWidth, int nHeight );

	protected:
		enum	BtnState{ StateNone, StateUp, StateDown, StateThumb, StateHoldUp, StateHoldDown };

		int		m_nMin;
		int		m_nMax;
		int		m_nPosition;
		int		m_nPageSize;
		bool	m_bShowThumb;

		BtnState	m_enBtnState;
		Direction	m_enDirection;

		CUIRect		m_rcThumb;
		CUIRect		m_rcUpButton;
		CUIRect		m_rcDownButton;
		CUIRect		m_rcTrack;

		ITexture*	m_pUpTexture;
		ITexture*	m_pDownTexture;
		ITexture*	m_pThumbTexture;

	private:
		CUIPoint	m_ptLast;
		int			m_nThumbOffset;
		DWORD		m_dwBtnTime;
	};
}
#endif

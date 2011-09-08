#pragma once
#include "Picture.h"
#include "ScrollBar.h"
#define INVALID_INDEX (-1)
namespace UILib
{
	class CListBox	:	public XObjTypeT< CPicture, TypeListbox >
	{
	DECLARE_UIMSG_MAP()
	public:
		CListBox();
		~CListBox(void);

	//--------------------------------------------------------------------------
		enum enItemState { Normal, Selected, Focused, };
		struct tagItem
		{
			TString		m_string;
			void*		m_pData;
			enItemState	m_enState;
		};
		typedef std::vector< tagItem* > CItemVector;
	//--------------------------------------------------------------------------
		// …Ë÷√øÿº˛ Ù–‘
		virtual LPCTSTR GetLable(){ return _T("LISTBOX"); }

		virtual void LoadProperty( TiXmlElement* pXMLElement );
		virtual void SaveProperty( TiXmlElement* pXMLElement );

		virtual bool SetProperty( LPCTSTR pszPropertyName, LPCTSTR pszValue );
		virtual bool GetProperty( LPCTSTR pszPropertyName, LPTSTR pValueBuffer, INT size );

		virtual void RenderSelf( CONST CUIRect& clipper );
		virtual void DrawItem( INT nX, INT nY, INT nWidth, INT nHeight, tagItem& item, const CUIRect& clipper );

		void		AddItem( LPCTSTR lpszString, void* pData = NULL );
		void		Insert( LPCTSTR lpszString, void* pData, INT nAfter = 0 );
		void		RemoveItem( INT nIndex );
		tagItem*	GetItem( INT nIndex );
		INT			FindItem( INT nStart, enItemState enState );
		INT			GetItemCount()const{ return ( INT )m_ItemVec.size(); }

		void		SetItemHeight( INT nHeight );
		INT			GetItemHeight()const{ return m_nItemHeight; }
		void		SetBorder( INT nBorder ){ m_nBorderWidth = nBorder; }
		INT			GetBorder()const{ return m_nBorderWidth; }
		
		bool		AttachScroll( INT nScrollID );
		bool		AttachScroll( CUIElement* pScrollCtrl );

		void		SetCurSelect( INT nIndex );

	protected:
		virtual BOOL DefMsgProc( UINT nMsg, WPARAM wParam, LPARAM lParam );
		virtual bool onButtonDown(INT button, const CUIPoint& pt, UINT sysKeys);

		uimsg void OnSize( INT cx, INT cy );

	private:
		INT	GetItemByPoint( const CUIPoint& pt)const;

	protected:
		CItemVector	m_ItemVec;
		CTexture*	m_pSelectTex;
		INT			m_nItemHeight;
		INT			m_nScrollID;
		INT			m_nBorderWidth;

	private:
		INT			m_nItemBegin;
		INT			m_nDisplayCount;
		tagItem*	m_pSelectItem;
		CScrollBar*	m_pScroll;
	};
}

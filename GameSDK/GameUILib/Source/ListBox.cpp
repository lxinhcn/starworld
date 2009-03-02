#include "header.h"
#include "Listbox.h"
#include "Canvas.h"

namespace UILib
{
	BEGIN_UIMSG_MAP( CListBox, CUIElement )
		ON_UI_SIZE()
	END_UIMSG_MAP()
	CListBox::CListBox(void)
	: m_pSelectTex( new CTexture )
	{
		m_nItemHeight	= 20;
		m_nScrollID		= 0;
		m_nItemBegin	= 0;
		m_nDisplayCount	= 0;
		m_nBorderWidth	= 3;
		m_pScroll		= NULL;
		m_pSelectItem	= NULL;
		AddItem( "List box item1" );
		AddItem( "List box item2" );
		AddItem( "List box item3" );
	}

	CListBox::~CListBox(void)
	{
		for( size_t i = 0; i < m_ItemVec.size(); ++i )
		{
			delete m_ItemVec[i];
		}
		m_ItemVec.clear();
		delete m_pSelectTex;
	}

	//--------------------------------------------------------------------------
	void CListBox::AddItem( LPCSTR lpszString, void* pData )
	//--------------------------------------------------------------------------
	{
		tagItem* pItem = new tagItem;
		pItem->m_string = lpszString;
		pItem->m_pData	= pData;
		pItem->m_enState= Normal;
		m_ItemVec.push_back( pItem );

		if( m_pScroll )		m_pScroll->SetRange( 0, m_ItemVec.size() );
	}

	void CListBox::Insert( LPCSTR lpszString, void* pData, int nAfter )
	{
		tagItem* pItem = new tagItem;
		pItem->m_string = lpszString;
		pItem->m_pData	= pData;
		pItem->m_enState= Normal;
		if( nAfter < 0 )	nAfter = 0;
		if( nAfter >= ( int )m_ItemVec.size() )
			m_ItemVec.push_back( pItem );
		else
			m_ItemVec.insert( m_ItemVec.begin() + nAfter, pItem );

		if( m_pScroll )		m_pScroll->SetRange( 0, m_ItemVec.size() );
	}

	void CListBox::RemoveItem( int nIndex )
	{
		if( nIndex >=0 && nIndex < GetItemCount() )
		{
			if( m_ItemVec[nIndex] == m_pSelectItem )
				m_pSelectItem = NULL;

			m_ItemVec.erase( m_ItemVec.begin() + nIndex );

			if( m_pScroll )
				m_pScroll->SetRange( 0, m_ItemVec.size() );
		}
	}

	CListBox::tagItem*	CListBox::GetItem( int nIndex )
	{
		if( nIndex < 0 || nIndex >= m_ItemVec.size() )	return NULL;
		return m_ItemVec[nIndex];
	}

	int	CListBox::FindItem( int nStart, enItemState enState )
	{
		for( int i = nStart; i < m_ItemVec.size(); ++i )
		{
			if( m_ItemVec[i]->m_enState == enState )
				return i;
		}
		return INVALID_INDEX;
	}

	void CListBox::RenderSelf( const CUIRect& clipper )
	{
		__super::RenderSelf( clipper );
		int nHeight = m_wndRect.Height();
		int nWidth	= m_wndRect.Width();

		CUIPoint pt( 0, 0 );
		pt = ClientToScreen(pt);

		int nDisplayCount = m_nDisplayCount;
		if( nDisplayCount > GetItemCount() )	nDisplayCount = GetItemCount();
		int nY = pt.y + m_nBorderWidth;
		for( size_t i = m_nItemBegin; i < m_nItemBegin + nDisplayCount; ++i )
		{
			tagItem* item = m_ItemVec[i];
			DrawItem( pt.x + m_nBorderWidth, nY, nWidth, nHeight, *item, clipper );
			nY += m_nItemHeight;
		}
	}

	void CListBox::DrawItem( int nX, int nY, int nWidth, int nHeight, tagItem& item, const CUIRect& clipper )
	{
		CUIRect rc( nX, nY, nX + nWidth - m_nBorderWidth*2, nY + m_nItemHeight );
		if( item.m_enState == Selected )
		{
			m_pCanvas->DrawTexture( m_pSelectTex, nX, nY, nWidth - m_nBorderWidth*2, m_nItemHeight, clipper );
		}
		m_pCanvas->DrawText( item.m_string.c_str(), m_pFont, rc.left, rc.top, 0 );
	}

	bool CListBox::AttachScroll( int nScrollID )
	{
		if( !m_pParent )	return false;

		CUIElement* pCtrl = m_pParent->FindChildByID( nScrollID );
		return AttachScroll( pCtrl );
	}

	bool CListBox::AttachScroll( CUIElement* pScrollCtrl )
	{
		if( pScrollCtrl && pScrollCtrl->IsKindOf( TypeScroll ) )
		{
			m_nScrollID = pScrollCtrl->GetID();
			m_pScroll = reinterpret_cast< CScrollBar* >( pScrollCtrl );
			// 监听ScrollBar的消息
			m_pScroll->AddCommonListener( this );
			if( m_nDisplayCount <= GetItemCount() )
				m_pScroll->SetRange( 0, GetItemCount() - m_nDisplayCount );
			else
				m_pScroll->SetRange( 0, 0 );
			return true;
		}
		return false;
	}

	void CListBox::SetItemHeight( int nHeight )
	{
		if( nHeight > 0 )
		{
			m_nItemHeight = nHeight;
			m_nDisplayCount = ( m_wndRect.Height() - m_nBorderWidth * 2 )/m_nItemHeight;
		}
	}

	void CListBox::SetCurSelect( int nIndex )
	{
		if( nIndex >= 0 && nIndex < GetItemCount() )
		{
			tagItem* pItem = m_ItemVec[nIndex];
			if( pItem && m_pSelectItem != pItem )
			{
				if( m_pSelectItem ) m_pSelectItem->m_enState = Normal;
				pItem->m_enState = Selected;
				m_pSelectItem = pItem;
				SendMessage( WM_COMMAND, MAKELONG( GetID(), EV_SELCHANGE ), 0 );
			}
		}
	}

	int CListBox::GetItemByPoint( const CUIPoint& pt)const
	{
		if(	pt.x < m_nBorderWidth ||
			pt.x < m_nBorderWidth ||
			pt.x > m_wndRect.Width() - m_nBorderWidth ||
			pt.x > m_wndRect.Height() - m_nBorderWidth )
		{
			return INVALID_INDEX;
		}
		int nIndex = ( pt.y - m_nBorderWidth )/m_nItemHeight;
		if( nIndex >= 0 && nIndex < GetItemCount() )
		{
			return nIndex;
		}
		return INVALID_INDEX;
	}
//--------------------------------------------------------------------------
// 窗口消息
//--------------------------------------------------------------------------
	void CListBox::OnSize( int cx, int cy )
	{
		m_nDisplayCount = ( cy - m_nBorderWidth )/m_nItemHeight;
		if( GetItemCount() - m_nItemBegin < m_nDisplayCount ) m_nItemBegin = 0;
		AttachScroll( m_nScrollID );
	}

//--------------------------------------------------------------------------
// 消息处理
//--------------------------------------------------------------------------
	BOOL CListBox::DefMsgProc( UINT nMsg, WPARAM wParam, LPARAM lParam )
	{
		if( nMsg == WM_NOTIFY )
		{
			NMUIHDR* pNMHDR = ( NMUIHDR* )lParam;
			if( pNMHDR &&
				pNMHDR->idFrom == m_nScrollID )
			{
				// BindScroll
				switch( pNMHDR->code )
				{
				case NC_CREATE:
					AttachScroll( pNMHDR->pCtrl );
					break;
				case EV_SCROLLPOSCHANGED:
					m_nItemBegin = ((NMHDR_SCROLL*)pNMHDR)->pos;
					break;
				}
				return TRUE;
			}
		}
		return __super::DefMsgProc( nMsg, wParam, lParam );
	}

	bool CListBox::onButtonDown(int button, const CUIPoint& pt, UINT sysKeys)
	{
		SetCurSelect( GetItemByPoint( pt ) );
		return __super::onButtonDown( button, pt, sysKeys );
	}
//--------------------------------------------------------------------------
// 对象存储
//--------------------------------------------------------------------------
	void CListBox::LoadProperty(TiXmlElement* pXMLElement )
	{
		pXMLElement->QueryIntAttribute( "ScrollID", &m_nScrollID );
		pXMLElement->QueryIntAttribute( "ItemHeight", &m_nItemHeight );
		pXMLElement->QueryIntAttribute( "Border", &m_nBorderWidth );
		m_pSelectTex->SetName( pXMLElement->Attribute( "SelTex" ) );

		AttachScroll( m_nScrollID );
		__super::LoadProperty( pXMLElement );
	}

	void CListBox::SaveProperty(TiXmlElement* pXMLElement )
	{
		pXMLElement->SetAttribute( "ScrollID", m_nScrollID );
		pXMLElement->SetAttribute( "ItemHeight", m_nItemHeight );
		pXMLElement->SetAttribute( "Border", m_nBorderWidth );
		pXMLElement->SetAttribute( "SelTex", m_pSelectTex->GetName() );

		__super::SaveProperty( pXMLElement );
	}

	bool CListBox::SetProperty( const char* pszPropertyName, const char* pszValue )
	{
		if( stricmp( pszPropertyName, "ItemHeight" ) == 0 )
		{
			SetItemHeight( atoi( pszValue ) );
		}
		else if( stricmp( pszPropertyName, "ScrollID" ) == 0 )
		{
			AttachScroll( atoi( pszValue ) );
		}
		else if( stricmp( pszPropertyName, "BorderWidth" ) == 0 )
		{
			SetBorder( atoi( pszValue ) );
		}
		else if( stricmp( pszPropertyName, "SelectTexture" ) == 0 )
		{
			m_pSelectTex->SetName( pszValue );
		}
		else
		{
			return __super::SetProperty( pszPropertyName, pszValue );
		}
		return true;
	}

	bool CListBox::GetProperty( const char* pszPropertyName, char* pValueBuffer, int size )
	{
		if( stricmp( pszPropertyName, "ItemHeight" ) == 0 )
		{
			itoa( GetItemHeight(), pValueBuffer, 10 );
		}
		else if( stricmp( pszPropertyName, "ScrollID" ) == 0 )
		{
			itoa( m_nScrollID, pValueBuffer, 10 );
		}
		else if( stricmp( pszPropertyName, "BorderWidth" ) == 0 )
		{
			itoa( m_nBorderWidth, pValueBuffer, 10 );
		}
		else if( stricmp( pszPropertyName, "SelectTexture" ) == 0 )
		{
			strncpy( pValueBuffer, m_pSelectTex->GetName(), size );
		}
		else
		{
			return __super::GetProperty( pszPropertyName, pValueBuffer, size );
		}
		return true;
	}
}

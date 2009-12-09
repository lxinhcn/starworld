// PropertyList.cpp : implementation file
//

#include "stdafx.h"
//#include "PropListBox.h"
#include "PropertyList.h"
#include "FoldersDialog.h"

#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDC_PROPBTNCTRL		709
#define IDC_PROPEDITBOX		PIT_EDIT + 710
#define IDC_PROPCMBBOX		PIT_COMBO + 710
#define IDC_PROPCMBBOX2		PIT_COMBO2 + 710
#define IDC_PROPCMBBOX3		PIT_COMBO3 + 710
#define IDC_PROPCHECK		PIT_CHECK + 710
#define IDC_IP				PIT_IP + 710
#define IDC_DATE			PIT_DATE + 710
#define IDC_TIME			PIT_TIME + 710
#define IDC_HOTKEY			PIT_HOTKEY + 710

#define WIDTH_PART0 12
#define WIDTH_PART2 20


#define VALIDPOINT(p) \
	(p != NULL && p != (void*)~NULL)


static inline CString MAKEKEY(LPCTSTR catalog, LPCTSTR propname)
{
	CString str = CString(catalog) + "*" + propname;
	str.TrimLeft();
	str.TrimRight();
	str.MakeUpper();
	return str;
}

void SplitString(LPCTSTR src, LPCTSTR delimiter, CStringList& strList)
{
	if(!src || !delimiter)
		return;

	int i0, i1, iLen = _tcslen(src);

	i0 = i1 = 0;
	LPTSTR p = (LPTSTR) (long) src;

	while( i0 < iLen && (i1 = _tcscspn(p + i0, delimiter)) >= 0)
	{
		CString tmp = p + i0;
		if(iLen > i0 + i1)
			tmp.SetAt(i1, NULL);
		tmp.ReleaseBuffer();

		strList.AddTail(tmp);

		i0 += i1 + 1;
	}

	if(i0 < iLen)
	{
		CString tmp = p + i0;
		strList.AddTail(tmp);
	}
}

///////////////////////////////////////////////////////////////////////////////
// Function Name:	GetDateTime
// Parameters:
//					szDateTime, LPCTSTR, time string
//					[out]tm, CTime, Time Struct
// Return Value:
//					True, Get a valid time, or False
// Remarks:
//					convert time string to CTime
//						like:
//							YYYY/m/d H:M:S
//							YYYY-m-d H:M:S
//							m/d/YYYY H:M:S
//							m-d-YYYY H:M:S
//
//					this function has fault.
//
// Author:			Runming Yan
// Date: 
// Mender: 
// Date: 
///////////////////////////////////////////////////////////////////////////////
BOOL GetDateTime(LPCTSTR szDateTime, CTime& tm)
{
	int y, m, d, H, M, S;

	y=2000;
	m=d=1;
	H=M=S=0;

	LPCTSTR deli = _T("-/");
	CString strTime = szDateTime;
	int i0, i1;

	strTime.TrimLeft();
	strTime.TrimRight();
	
	if((i0 = strTime.FindOneOf(deli)) >= 0)
	{
		y = _ttoi(strTime.Mid(0, i0));

		i1 = i0+1;
		strTime = strTime.GetBuffer(0) + i1;
		
		if((i0 = strTime.FindOneOf(deli)) >= 0)
		{
			m = _ttoi(strTime.Mid(0, i0));
			
			i1 = i0+1;
			strTime = strTime.GetBuffer(0) + i1;
			
			if((i0 = strTime.GetLength()) >= 0)
			{
				d = _ttoi(strTime.Mid(0, i0>1?2:i0));
			}
		}

		if(d > 31 && y < 13)
		{
			std::swap(y, m);
			std::swap(m, d);
		}

		y=y<1970?1970:y;
		y=y>3000?3000:y;
		m=m<1?1:m;
		m=m>12?12:m;
		d=d<1?1:d;
		d=d>31?31:d;
	}
	
	strTime = szDateTime;
	if((i0 = strTime.FindOneOf(_T(" "))) >= 0)
	{
		i1 =i0+1;
		strTime = strTime.GetBuffer(0) + i1;
	}

	if((i0 = strTime.FindOneOf(_T(":"))) >= 0)
	{
		H = _ttoi(strTime.Mid(0, i0));
		
		i1 = i0+1;
		strTime = strTime.GetBuffer(0) + i1;
		
		if((i0 = strTime.FindOneOf(_T(":"))) >= 0)
		{
			M = _ttoi(strTime.Mid(0, i0));
			
			i1 = i0+1;
			strTime = strTime.GetBuffer(0) + i1;
			
			if((i0 = strTime.GetLength()) >= 0)
			{
				S = _ttoi(strTime.Mid(0, i0));
				
				i1 = i0+1;
				strTime = strTime.GetBuffer(0) + i1;
			}
		}
	}
	
	tm = CTime(y, m, d, H, M, S);
	
	strTime = szDateTime;
	return strTime.FindOneOf(_T("-/:"))>0;
}

//////////////////////////////////////////////////////////////////////////
//convert string like "255 255 255" to RGB
//////////////////////////////////////////////////////////////////////////
static COLORREF GetColor(LPCTSTR src)
{
	int RVal, GVal, BVal;
	RVal=GVal=BVal=0;
	
	LPCTSTR delimiter = _T(" ");
	
	CString currClr = src;
	currClr.TrimLeft();
	currClr.TrimRight();
	
	if (currClr.GetLength() > 0)
	{
		int j = currClr.Find(delimiter, 0);
		
		if(j > 0)
		{
			CString bufr = currClr.Mid(0, j);
			RVal = _ttoi(bufr);
			int j2 = currClr.Find(delimiter, j+1);
			if(j2 > j)
			{
				bufr = currClr.Mid(j+1, j2-(j+1));
				GVal = _ttoi(bufr);
				int j3 = currClr.GetLength();
				if(j3 > j2)
				{
					bufr = currClr.Mid(j2+1, j3-(j2+1));
					BVal = _ttoi(bufr);
				}
			}
		}
	}

	return RGB(RVal, GVal, BVal);
}

/////////////////////////////////////////////////////////////////////////////
// CPropertyList

CPropertyList::CPropertyList() : m_pfnOnSelChanged(NULL)
{
	m_bmpChk.LoadBitmap(IDB_CHECKBOX);
}

CPropertyList::~CPropertyList()
{
	m_bmpChk.DeleteObject();
}


BEGIN_MESSAGE_MAP(CPropertyList, CListBox)
	//{{AFX_MSG_MAP(CPropertyList)
	ON_WM_CREATE()
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelchange)
	ON_WM_LBUTTONUP()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
	ON_CBN_KILLFOCUS(IDC_PROPCMBBOX, OnKillfocusCmbBox)
	ON_CBN_KILLFOCUS(IDC_PROPCMBBOX2, OnKillfocusCmbBox2)
	ON_CBN_KILLFOCUS(IDC_PROPCMBBOX3, OnKillfocusChkCmbBox)
	ON_EN_KILLFOCUS(IDC_PROPEDITBOX, OnKillfocusEditBox)
	ON_EN_CHANGE(IDC_PROPEDITBOX, OnChangeEditBox)
	ON_BN_CLICKED(IDC_PROPBTNCTRL, OnButton)
	ON_NOTIFY(NM_KILLFOCUS, IDC_TIME, OnKillfocusTime)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyList message handlers

BOOL CPropertyList::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CListBox::PreCreateWindow(cs))
		return FALSE;

	cs.style &= ~(LBS_OWNERDRAWVARIABLE | LBS_SORT);
	cs.style |= LBS_OWNERDRAWFIXED;

	m_bTracking = FALSE;
	m_nDivider = 0;
	m_bDivIsSet = FALSE;

	return TRUE;
}

void CPropertyList::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	lpMeasureItemStruct->itemHeight = 20; //pixels
}


void CPropertyList::DrawItem(LPDRAWITEMSTRUCT lpDIS) 
{
//	AfxTrace("DrawItem=%d,%d,%d,%x", 
//		lpDIS->itemID, 
//		lpDIS->itemAction, 
//		lpDIS->itemState, 
//		lpDIS->itemData);
	
	UINT nIndex = lpDIS->itemID;
	
	if (nIndex == (UINT) -1 || (lpDIS->itemAction|ODA_DRAWENTIRE) != ODA_DRAWENTIRE)
		return;

	//get the CPropertyItem for the current row
	CPropertyItem* pItem = static_cast<CPropertyItem*>(GetItemDataPtr(nIndex));
	if(!VALIDPOINT(pItem))
		return;

//	AfxTrace(",%x\n", (DWORD)pItem);
	if(PIT_CATALOG != pItem->m_nItemType && pItem->IsHidden())
	{
		return;
	}
	
	//
	CRect rectFullItem, rectPart0, rectPart1, rectPart2, rectPart3;

	rectFullItem = lpDIS->rcItem;

	if (m_nDivider==0)
		m_nDivider = rectFullItem.Width() / 2;

	rectPart0 = rectFullItem;
	rectPart0.right = rectPart0.left + WIDTH_PART0;

	rectPart1 = rectFullItem;
	rectPart1.left = rectPart0.right;
	rectPart1.right = m_nDivider;

	rectPart2 = rectFullItem;
	rectPart2.left = m_nDivider + (PIT_CATALOG==pItem->m_nItemType?0:1);
	if(PIT_COLOR == pItem->m_nItemType || PIT_CHECK == pItem->m_nItemType)
	{
		rectPart2.right = rectPart2.left + WIDTH_PART2;
	}
	else
		rectPart2.right = rectPart2.left;

	rectPart3 = rectFullItem;
	rectPart3.left = rectPart2.right;
	
	//
	//begin to draw this item
	//
	CDC dc;
	dc.Attach(lpDIS->hDC);
	
	//first part of item
	if(PIT_CATALOG == pItem->m_nItemType)
	{
		dc.FillSolidRect(rectPart0, RGB(150,150,150));

		CRect rect2;
		rect2.left = rectPart0.left+2;
		rect2.top = rectPart0.top+3;
		rect2.right = rectPart0.right;
		rect2.bottom = rectPart0.bottom-5;
		
		CDC m_dcCross;
		
		m_dcCross.CreateCompatibleDC(&dc);
		m_dcCross.SelectObject(&m_bmpChk);
		
		dc.BitBlt(rect2.left, 
			rect2.top, 
			10, 
			10, 
			&m_dcCross, 
			pItem->IsCollapsed()?64:48, 
			0, 
			SRCCOPY);
	}else
	{
		dc.DrawEdge(rectPart0,EDGE_SUNKEN,BF_BOTTOM);
		dc.FillSolidRect(rectPart0, RGB(190,190,190));
	}
	
	//second part
	//if it is the selected item, set its background-color
	if(PIT_CATALOG == pItem->m_nItemType)
	{
		dc.FillSolidRect(rectPart1, RGB(150,150,150));
		
		//write the property name
		dc.DrawText(pItem->m_catalog,
			CRect(rectPart1.left+3,rectPart1.top+1,rectPart1.right-3,rectPart1.bottom-1),
			DT_LEFT | DT_SINGLELINE | DT_VCENTER );
	}
	else
	{
		dc.SetBkMode(TRANSPARENT);
		dc.FillSolidRect(rectPart1, nIndex==(UINT)m_curSel?RGB(190,190,190):RGB(255,255,255));
	
		//write the property name
		dc.DrawText(pItem->m_propName,
			CRect(rectPart1.left+3,rectPart1.top+1,rectPart1.right-3,rectPart1.bottom-1),
			DT_LEFT | DT_SINGLELINE | DT_VCENTER );

		dc.DrawEdge(rectPart1,EDGE_SUNKEN,BF_BOTTOMRIGHT);
	}

	//third part
	dc.DrawEdge(rectPart2,EDGE_SUNKEN,BF_BOTTOM);
	
	if(PIT_CATALOG == pItem->m_nItemType)
	{
		dc.FillSolidRect(rectPart2, RGB(150,150,150));
	}
	else if(PIT_COLOR == pItem->m_nItemType)
	{
		CRect rect3 = rectPart2;
		rect3.left += 2;
		rect3.top += 1;
		rect3.bottom -= 4;
		rect3.right = rect3.left + rect3.bottom - rect3.top;
		dc.FillSolidRect(rect3, GetColor(pItem->m_curValue));

		//edge of color frame
		CBrush br;
		br.CreateSolidBrush(RGB(0, 0, 0));
		dc.FrameRect(&rect3, &br);
	}
	else if(PIT_CHECK == pItem->m_nItemType)
	{
		CDC m_dcChk;
		
		m_dcChk.CreateCompatibleDC(&dc);
		m_dcChk.SelectObject(&m_bmpChk);
		
		dc.BitBlt(rectPart2.left+2, 
			rectPart2.top+3, 
			16, 
			16, 
			&m_dcChk, 
			pItem->m_curValue.CompareNoCase(_T("Y"))==0?32:16, 
			0, 
			SRCCOPY);
	}

	
	//fourth
	//write the initial property value in the second rectangle
	dc.DrawEdge(rectPart3,EDGE_SUNKEN,BF_BOTTOM);
	
	if(PIT_CATALOG == pItem->m_nItemType)
	{
		dc.FillSolidRect(rectPart3, RGB(150,150,150));
	}
	else if(PIT_CHECK != pItem->m_nItemType)
	{
		dc.DrawText(pItem->m_curValue,
			CRect(rectPart3.left+3, rectPart3.top+1, rectPart3.right+3, rectPart3.bottom-3),
			DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	}
	//
	//the end of drawing this item
	//

	dc.Detach();
}

BOOL CPropertyList::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	dwStyle |= LBS_OWNERDRAWVARIABLE|LBS_HASSTRINGS;
	
	return CListBox::Create(dwStyle, rect, pParentWnd, nID);
}

BOOL CPropertyList::AddPropItem(CString catalog, CString propName, CString curValue, int nItemType, CString cmbItems, CString propDesc)
{
	CString str = MAKEKEY(catalog, propName);
	if(m_mapData.find(str) != m_mapData.end())
	{
		return FALSE;
	}
	
	InsertCatalog(catalog.GetBuffer(0));
	
	CPropertyItem propItem(catalog, propName, curValue, nItemType, cmbItems, propDesc);
	
	m_mapData[str] = propItem;
	
	if(!IsCollapsedCatalog(catalog))
	{
		SaveCurrentPropertyValue();

		int nIndex = GetPos4NewItem(catalog.GetBuffer(0), propName.GetBuffer(0));
		
		nIndex = InsertString(nIndex, propName.GetBuffer(0));
		
		SetItemDataPtr(nIndex, &(m_mapData[str]) );
	}
	
	return TRUE;
}

BOOL CPropertyList::GetPropItem(LPCTSTR catalog, LPCTSTR szPropName, CPropertyItem& item)
{
	SaveCurrentPropertyValue();

	CString str = MAKEKEY(catalog, szPropName);
	
	std::map<CString, CPropertyItem>::iterator mit;
	m_mapData.size();
	if( (mit = m_mapData.find(str.GetBuffer(0))) != m_mapData.end() )
	{
		//memcpy(&item, &(mit->second), sizeof(CPropertyItem));	//dangerous
		item = mit->second;
		return TRUE;
	}
	
	return FALSE;
}

CString CPropertyList::GetPropValue(LPCTSTR catalog, LPCTSTR szPropName)
{
	CPropertyItem item;
	if( GetPropItem(catalog, szPropName, item) )
	{
		return item.m_curValue;
	}

	return _T("");
}

int CPropertyList::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	lpCreateStruct->style |= LBS_OWNERDRAWVARIABLE|LBS_HASSTRINGS;

	if (CListBox::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_bDivIsSet = FALSE;
	m_nDivider = 0;
	m_bTracking = FALSE;

	m_hCursorSize = AfxGetApp()->LoadStandardCursor(IDC_SIZEWE);
	m_hCursorArrow = AfxGetApp()->LoadStandardCursor(IDC_ARROW);

	m_SSerif8Font.CreatePointFont(80,_T("MS Sans Serif"));

	return 0;
}

void CPropertyList::OnSelchange() 
{
	CRect rect;
	CString lBoxSelText;

	CPropertyItem* pItem = static_cast<CPropertyItem*>(GetItemDataPtr(m_curSel));
	if(!VALIDPOINT(pItem))
		return;
	
	//m_curSel = GetCurSel();

	//Caculate the grid width
	GetItemRect(m_curSel,rect);
	if(PIT_COLOR == pItem->m_nItemType || PIT_CHECK == pItem->m_nItemType)
	{
		rect.left = m_nDivider + 1 + WIDTH_PART2;
	}
	else
		rect.left = m_nDivider + 1;

	if (m_btnCtrl)
		m_btnCtrl.ShowWindow(SW_HIDE);

	switch(pItem->m_nItemType) 
	{
	case PIT_CATALOG:
		OnSelectCatalog(pItem);
		break;
	case PIT_EDIT:
		OnSelectEdit(rect, pItem);
		break;
	case PIT_COMBO:
		OnSelectComboBox(m_cbxDropDownList, rect, pItem);
		break;
	case PIT_COMBO2:
		OnSelectComboBox(m_cbxDropDown, rect, pItem);
		break;
	case PIT_COMBO3:
		OnSelectChkComboBox(rect, pItem);
		break;
	case PIT_IP:
		OnSelectIP(rect, pItem);
		break;
	case PIT_CHECK:
		OnSelectCheck(pItem);
		break;
	case PIT_DATE:
	case PIT_TIME:
	case PIT_DATETIME:
		OnSelectDateTime(rect, pItem);
		break;
//	case PIT_HOTKEY:
//		OnSelectHotKey(rect, pItem);
//		break;
	default:
		DisplayButton(rect);
		break;
	}

//	if(PIT_CATALOG == pItem->m_nItemType)
//	{
//		OnSelectCatalog(pItem);
//	}
//	else if (PIT_COMBO == pItem->m_nItemType || PIT_COMBO2 == pItem->m_nItemType)
//	{
//		OnSelectComboBox(PIT_COMBO2 == pItem->m_nItemType?m_cbxDropDown:m_cbxDropDownList, rect, pItem);
//	}
//	else if(PIT_EDIT == pItem->m_nItemType)
//	{
//		OnSelectEdit(rect, pItem);
//	}
//	else if(PIT_IP == pItem->m_nItemType)
//	{
//		OnSelectIP(rect, pItem);
//	}
//	else if(PIT_CHECK==pItem->m_nItemType)
//	{
//		OnSelectCheck(pItem);
//	}
//	else if(PIT_DATE==pItem->m_nItemType||PIT_TIME==pItem->m_nItemType||PIT_DATETIME==pItem->m_nItemType)
//	{
//		OnSelectDateTime(rect, pItem);
//	}
//	else if(PIT_COMBO3 == pItem->m_nItemType)
//	{
//		OnSelectChkComboBox(rect, pItem);
//	}
//	else
//		DisplayButton(rect);
	
	if(m_pfnOnSelChanged)
		m_pfnOnSelChanged(pItem);
	
	Invalidate();
}

void CPropertyList::OnSelectCatalog(CPropertyItem* pItem)
{
	if(!VALIDPOINT(pItem) && PIT_CATALOG != pItem->m_nItemType)
		return;

	pItem->SetCollapsed(!pItem->IsCollapsed());
	
	if(pItem->IsCollapsed())
	{
		for(int i=GetCount()-1;i>=0;i--)
		{
			CPropertyItem* p = (CPropertyItem*) GetItemData(i);
			if(p && p->m_catalog.CompareNoCase(pItem->m_catalog) == 0)
			{
				if(PIT_CATALOG != p->m_nItemType)
				{
					p->SetHidden(pItem->IsCollapsed());
					DeleteString(i);
				}
			}
		}
	}else
	{
		std::map<CString, CPropertyItem>::iterator mit;
		
		for(mit = m_mapData.begin(); mit!=m_mapData.end(); mit++)
		{
			CPropertyItem* p = &mit->second;
			
			if(VALIDPOINT(p)
				&& PIT_CATALOG != p->m_nItemType
				&& p->m_catalog.CompareNoCase(pItem->m_catalog)==0)// && p->IsHidden()
			{
				p->SetHidden(FALSE);
				
				int nIndex = GetPos4NewItem(p->m_catalog.GetBuffer(0), p->m_propName.GetBuffer(0));
				nIndex = InsertString(nIndex, p->m_propName.GetBuffer(0));
				SetItemDataPtr(nIndex, p);
			}
		}
	}
}

void CPropertyList::OnSelectComboBox(CComboBox& cbx, CRect& rect, CPropertyItem* pItem)
{
	//display the combo box.  If the combo box has already been
	//created then simply move it to the new location, else create it
	m_nLastBox = 0;
	if (cbx)
		cbx.MoveWindow(rect);
	else
	{
		rect.bottom += 100;
		if(PIT_COMBO2 == pItem->m_nItemType)
		{
			cbx.Create(CBS_DROPDOWN | CBS_NOINTEGRALHEIGHT | WS_VISIBLE | WS_CHILD | WS_BORDER,
				rect, this, IDC_PROPCMBBOX2);
		}else
		{
			cbx.Create(CBS_DROPDOWNLIST | CBS_NOINTEGRALHEIGHT | WS_VISIBLE | WS_CHILD | WS_BORDER,
				rect, this, IDC_PROPCMBBOX);
		}
		cbx.SetFont(&m_SSerif8Font);
	}
	
	//add the choices for this particular property
	CString cmbItems = pItem->m_cmbItems;
	CString lBoxSelText = pItem->m_curValue;
	
	cbx.ResetContent();
	if(PIT_CHECK == pItem->m_nItemType)
	{
		cmbItems = "Y|N";
	}
	else
	{
		cbx.AddString(_T(""));
	}

//	int i,i2;
//	i=0;
//	while ((i2=cmbItems.Find('|',i)) != -1)
//	{
//		cbx.AddString(cmbItems.Mid(i,i2-i));
//		i=i2+1;
//	}

	CStringList strList;
	SplitString(cmbItems, _T("|"), strList);
	for(POSITION pos = strList.GetHeadPosition(); pos != NULL; )
	{
		CString s = strList.GetNext(pos);
		if(cbx.FindString(0, s) == CB_ERR)
			cbx.AddString(s);
	}
	
	cbx.ShowWindow(SW_SHOW);
	cbx.SetFocus();
	
	//jump to the property's current value in the combo box
	int j = cbx.FindStringExact(0, lBoxSelText);
	if (j != CB_ERR)
		cbx.SetCurSel(j);
	else
	{
		cbx.SetCurSel(0);
		cbx.SetWindowText(lBoxSelText);
		cbx.SetEditSel(0, -1);
	}
}

void CPropertyList::OnSelectChkComboBox(CRect& rect, CPropertyItem* pItem)
{
	//display the combo box.  If the combo box has already been
	//created then simply move it to the new location, else create it
	m_nLastBox = 0;
	if (m_chkCbx)
		m_chkCbx.MoveWindow(rect);
	else
	{
		rect.bottom += 100;
		m_chkCbx.Create(CBS_DROPDOWNLIST | CBS_NOINTEGRALHEIGHT | WS_VISIBLE | WS_CHILD | WS_BORDER,
			rect, this, IDC_PROPCMBBOX3);
		m_chkCbx.SetFont(&m_SSerif8Font);
	}
	
	//add the choices for this particular property
	CString cmbItems = pItem->m_cmbItems;
	CString lBoxSelText = pItem->m_curValue;
	
	m_chkCbx.ResetContent();
	m_chkCbx.SetOptions(cmbItems);
	m_chkCbx.SetValues(lBoxSelText);
	m_chkCbx.ShowWindow(SW_SHOW);
	m_chkCbx.SetFocus();
}

void CPropertyList::OnSelectEdit(CRect& rect, CPropertyItem* pItem)
{
	//display edit box
	m_nLastBox = 1;
	m_prevSel = m_curSel;
	rect.bottom -= 3;
	if (m_editBox)
		m_editBox.MoveWindow(rect);
	else
	{
		m_editBox.Create(ES_LEFT | ES_AUTOHSCROLL | WS_VISIBLE | WS_CHILD | WS_BORDER,
			rect, this, IDC_PROPEDITBOX);
		m_editBox.SetFont(&m_SSerif8Font);
	}
	
	//set the text in the edit box to the property's current value
	m_editBox.SetWindowText(pItem->m_curValue);
	m_editBox.ShowWindow(SW_SHOW);
	m_editBox.SetFocus();
	m_editBox.SetSel(0, -1);
}

void CPropertyList::OnSelectIP(CRect& rect, CPropertyItem* pItem)
{
	m_nLastBox = 11;
	m_prevSel = m_curSel;

	if (m_ipCtrl)
	{
		m_ipCtrl.ShowWindow(SW_HIDE);
		m_ipCtrl.ShowWindow(SW_SHOW);
		m_ipCtrl.MoveWindow(rect);
	}
	else
	{
		m_ipCtrl.Create(WS_VISIBLE | WS_CHILD | WS_BORDER,
			rect, this, IDC_IP);
		m_ipCtrl.SetFont(&m_SSerif8Font);
	}
	
	BYTE ip[4] = {0, 0, 0, 0};
	CStringList ipList;
	SplitString(pItem->m_curValue, _T("."), ipList);
	int i = 3;
	for(POSITION pos = ipList.GetHeadPosition(); pos != NULL && i >= 0; i--)
	{
		ip[i] = _ttoi(ipList.GetNext(pos).GetBuffer(0));
	}
	
	//set the text in the edit box to the property's current value
	m_ipCtrl.SetAddress(*(long*)ip);
	m_ipCtrl.ShowWindow(SW_SHOW);
	m_ipCtrl.SetFocus();
}

void CPropertyList::OnSelectCheck(CPropertyItem* pItem)
{
	if(pItem->m_curValue.CompareNoCase(_T("Y")) == 0)
	{
		pItem->m_curValue = _T("N");
	}else
	{
		pItem->m_curValue = _T("Y");
	}
}

void CPropertyList::OnSelectDateTime(CRect& rect, CPropertyItem* pItem)
{
	m_nLastBox = 1;
	m_prevSel = m_curSel;

	if(PIT_DATE == pItem->m_nItemType)
	{
		if (m_date)
		{
			m_date.MoveWindow(rect);
		}
		else
		{
			m_date.Create(WS_VISIBLE | WS_CHILD | DTS_SHORTDATEFORMAT, //WS_TABSTOP | DTS_SHOWNONE | 
				rect, this, IDC_DATE);
		}

		CTime tm;
		if(pItem) GetDateTime(pItem->m_curValue, tm);
		m_date.SetTime(&tm);

		m_date.ShowWindow(TRUE);
		m_date.SetFocus();
	}
	else if(PIT_TIME == pItem->m_nItemType)
	{
		if (m_time)
		{
			m_time.MoveWindow(rect);
		}
		else
		{
			m_time.Create(WS_VISIBLE | WS_CHILD | DTS_TIMEFORMAT,
				rect, this, IDC_TIME);
		}
		
		CTime tm;
		if(pItem) GetDateTime(pItem->m_curValue, tm);
		m_time.SetTime(&tm);
		
		m_time.ShowWindow(TRUE);
		m_time.SetFocus();
	}
	else if(PIT_DATETIME == pItem->m_nItemType)
	{
		CRect rect0, rect1;
		rect0 = rect1 = rect;
		rect0.right = rect0.left + (rect0.right - rect0.left)/2;
		rect1.left = rect1.left + (rect1.right - rect1.left)/2 + 1;

		if (m_date)
		{
			m_date.MoveWindow(rect0);
		}
		else
		{
			m_date.Create(WS_VISIBLE | WS_CHILD | DTS_SHORTDATEFORMAT, 
				rect0, this, IDC_DATE);
		}

		if (m_time)
		{
			m_time.MoveWindow(rect1);
		}
		else
		{
			m_time.Create(WS_VISIBLE | WS_CHILD | DTS_TIMEFORMAT,
				rect1, this, IDC_TIME);
		}
		
		CTime tm;
		if(pItem) GetDateTime(pItem->m_curValue, tm);
		m_date.SetTime(&tm);
		m_time.SetTime(&tm);
		
		m_date.ShowWindow(TRUE);
		m_time.ShowWindow(TRUE);
		m_date.SetFocus();
	}
}

void CPropertyList::OnSelectHotKey(CRect& rect, CPropertyItem* pItem)
{
	if(!VALIDPOINT(pItem) && PIT_HOTKEY != pItem->m_nItemType)
		return;

	if(::IsWindow(m_hotKey.m_hWnd))
	{
		m_hotKey.ShowWindow(SW_HIDE);
		m_hotKey.MoveWindow(&rect);
		m_hotKey.ShowWindow(SW_NORMAL);
	}
	else
	{
		m_hotKey.Create(WS_VISIBLE|WS_CHILD, rect, this, IDC_HOTKEY);
		m_hotKey.SetFont(&m_SSerif8Font);
	}

	CString strHotKey = pItem->m_curValue;
	//BOOL 
	m_hotKey.SetWindowText(strHotKey);
}

void CPropertyList::DisplayButton(CRect region)
{
	//displays a button if the property is a file/color/font chooser
	m_nLastBox = 2;
	m_prevSel = m_curSel;

	if (region.Width() > 25)
		region.left = region.right - 25;
	region.bottom -= 3;

	if (m_btnCtrl)
		m_btnCtrl.MoveWindow(region);
	else
	{	
		m_btnCtrl.Create(_T("..."),BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD,
						region,this,IDC_PROPBTNCTRL);
		m_btnCtrl.SetFont(&m_SSerif8Font);
	}

	m_btnCtrl.ShowWindow(SW_SHOW);
	m_btnCtrl.SetFocus();
}

void CPropertyList::OnKillFocus(CWnd* pNewWnd) 
{
	//m_btnCtrl.ShowWindow(SW_HIDE);

	CListBox::OnKillFocus(pNewWnd);
}

void CPropertyList::OnKillfocusCmbBox() 
{
	CString selStr;
	if (m_cbxDropDownList && m_cbxDropDownList.IsWindowVisible())
	{
		m_cbxDropDownList.GetWindowText(selStr);
		m_cbxDropDownList.ShowWindow(SW_HIDE);
		
		CPropertyItem* pItem = static_cast<CPropertyItem*>(GetItemDataPtr(GetItemIndex(&m_cbxDropDownList)));
		if(!VALIDPOINT(pItem))
			return;

		if(PIT_COMBO != pItem->m_nItemType)
		{
			_ASSERTE(FALSE);
			return;
		}
		pItem->m_curValue = selStr;
	}

	Invalidate();
}

void CPropertyList::OnKillfocusCmbBox2() 
{
	CString selStr;
	if (m_cbxDropDown && m_cbxDropDown.IsWindowVisible())
	{
		m_cbxDropDown.GetWindowText(selStr);
		m_cbxDropDown.ShowWindow(SW_HIDE);
		
		CPropertyItem* pItem = static_cast<CPropertyItem*>(GetItemDataPtr(GetItemIndex(&m_cbxDropDown)));
		if(!VALIDPOINT(pItem))
			return;

		if(PIT_COMBO2 != pItem->m_nItemType)
		{
			_ASSERTE(FALSE);
			return;
		}
		pItem->m_curValue = selStr;
	}
	
	Invalidate();
}

void CPropertyList::OnKillfocusChkCmbBox()
{
	CString selStr;
	if (m_chkCbx && m_chkCbx.IsWindowVisible())
	{
		m_chkCbx.GetWindowText(selStr);
		m_chkCbx.ShowWindow(SW_HIDE);
		
		UINT curIndex = GetItemIndex(&m_chkCbx);
		CPropertyItem* pItem = static_cast<CPropertyItem*>(GetItemDataPtr(curIndex));
		if(!VALIDPOINT(pItem))
			return;

		if(PIT_COMBO3 != pItem->m_nItemType)
		{
			_ASSERTE(FALSE);
			return;
		}
		pItem->m_curValue = selStr;
	}
	
	Invalidate();
}

void CPropertyList::OnKillfocusEditBox()
{
	CString newStr;
	if (m_editBox && m_editBox.IsWindowVisible())
	{
		m_editBox.ShowWindow(SW_HIDE);
	}

	Invalidate();
}

///////////////////////////////////////////////////////////////////////////////
// Function Name:
//		CPropertyList::OnKillFocusIpCtrl
// Parameters:
// 
//
// Return Value:
// 
// Remarks:
//		Attention Please! CIPAddresCtrl has not actually the KillFocus Event.
//
// Author: Hankersyan@msn.com	(Eastman Kodak company)
// Date: 2005/6/30
// Mender: 
// Date: 
///////////////////////////////////////////////////////////////////////////////
void CPropertyList::OnKillFocusIpCtrl()
{
	if(m_ipCtrl && m_ipCtrl.IsWindowVisible())
	{
		UINT iItemIndex = GetItemIndex(&m_ipCtrl);
		if(LB_ERR == iItemIndex)
			return;

		CPropertyItem* pItem = static_cast<CPropertyItem*>(GetItemDataPtr(iItemIndex));
		if(!VALIDPOINT(pItem))
			return;
		
		if(pItem && (int) pItem != -1 && PIT_IP == pItem->m_nItemType)
		{
			BYTE b[4] = {0,0,0,0};
			m_ipCtrl.GetAddress(b[0], b[1], b[2], b[3]);
			
			CString strIP;
			strIP.Format(_T("%d.%d.%d.%d"), b[0], b[1], b[2], b[3]);
			
			pItem->m_curValue = strIP;

			m_ipCtrl.ShowWindow(FALSE);
			Invalidate();
		}
	}
}

//void CPropertyList::OnKillfocusDate(NMHDR* pNMHDR, LRESULT* pResult)
void CPropertyList::OnKillfocusDate()
{
	CPropertyItem* pItem = static_cast<CPropertyItem*>(GetItemDataPtr(m_curSel));
	if(!VALIDPOINT(pItem))
		return;
	
	if(pItem && PIT_DATE == pItem->m_nItemType
		&& m_date && m_date.IsWindowVisible() )
	{
		CTime t;
		m_date.GetTime(t);
		CString str;
		str.Format(_T("%d-%02d-%02d"), t.GetYear(), t.GetMonth(), t.GetDay());
		
		pItem->m_curValue = str;
		
		m_date.ShowWindow(FALSE);
		
		Invalidate();
	}
}

void CPropertyList::OnKillfocusTime(NMHDR* pNMHDR, LRESULT* pResult)
{
	CPropertyItem* pItem = static_cast<CPropertyItem*>(GetItemDataPtr(m_curSel));
	if(!VALIDPOINT(pItem))
		return;
	
	if(pItem && PIT_TIME == pItem->m_nItemType
		&& m_time && m_time.IsWindowVisible() )
	{
		CTime t;
		m_time.GetTime(t);
		CString str;
		str.Format(_T("%d:%d:%d"), t.GetHour(), t.GetMinute(), t.GetSecond());
		
		pItem->m_curValue = str;
		
		m_time.ShowWindow(FALSE);
		
		Invalidate();
	}
}

void CPropertyList::OnKillfocusDatetime()
{
	CString str;

	CPropertyItem* pItem = static_cast<CPropertyItem*>(GetItemDataPtr(m_curSel));
	if(!VALIDPOINT(pItem))
		return;
	
	if(pItem && PIT_DATETIME == pItem->m_nItemType
		&& m_date && m_date.IsWindowVisible() 
		&& m_time && m_time.IsWindowVisible() )
	{
		CTime t, t1;
		CString str;

		m_date.GetTime(t);
		str.Format(_T("%d-%02d-%02d "), t.GetYear(), t.GetMonth(), t.GetDay());
		
		m_time.GetTime(t1);
		CString tmp1;
		tmp1.Format(_T("%d:%d:%d"), t1.GetHour(), t1.GetMinute(), t1.GetSecond());
		str += tmp1;

		m_date.ShowWindow(FALSE);
		m_time.ShowWindow(FALSE);

		pItem->m_curValue = str;
		
		Invalidate();
	}
}

void CPropertyList::OnChangeEditBox()
{
	CString newStr;
	m_editBox.GetWindowText(newStr);
	
	CPropertyItem* pItem = static_cast<CPropertyItem*>(GetItemDataPtr(m_curSel));
	if(!VALIDPOINT(pItem))
		return;

	pItem->m_curValue = newStr;
}

void CPropertyList::OnButton()
{
	CPropertyItem* pItem = static_cast<CPropertyItem*>(GetItemDataPtr(m_curSel));
	if(!VALIDPOINT(pItem))
		return;
	
	//display the appropriate common dialog depending on what type
	//of chooser is associated with the property
	if (pItem->m_nItemType == PIT_COLOR)
	{
		CColorDialog ClrDlg(GetColor(pItem->m_curValue));
		
		if (IDOK == ClrDlg.DoModal())
		{
			COLORREF selClr = ClrDlg.GetColor();
			CString clrStr;
			clrStr.Format(_T("%d %d %d"),GetRValue(selClr),
						GetGValue(selClr),GetBValue(selClr));
			m_btnCtrl.ShowWindow(SW_HIDE);

			pItem->m_curValue = clrStr;
			Invalidate();
		}
	}
	else if (pItem->m_nItemType == PIT_FILE)
	{
		CString SelectedFile; 
	
		CFileDialog FileDlg(TRUE, NULL, NULL, NULL,	NULL);
		
		CString currPath = pItem->m_curValue;
		FileDlg.m_ofn.lpstrTitle = _T("Select file");
		if (currPath.GetLength() > 0)
			FileDlg.m_ofn.lpstrInitialDir = currPath.Left(
				currPath.GetLength() - currPath.ReverseFind(_T('\\')));

		if(IDOK == FileDlg.DoModal())
		{
			SelectedFile = FileDlg.GetPathName();
			
			m_btnCtrl.ShowWindow(SW_HIDE);
			
			pItem->m_curValue = SelectedFile;
			Invalidate();
		}
	}
	else if (pItem->m_nItemType == PIT_FONT)
	{	
		CFontDialog FontDlg(NULL,CF_EFFECTS | CF_SCREENFONTS,NULL,this);
		
		if(IDOK == FontDlg.DoModal())
		{
			CString faceName = FontDlg.GetFaceName();
			
			m_btnCtrl.ShowWindow(SW_HIDE);
			
			pItem->m_curValue = faceName;
			Invalidate();
		}
	}
	else if(PIT_FOLDER == pItem->m_nItemType)
	{
		CFoldersDialog dlgFolder;

		if( dlgFolder.BrowseFolder(m_hWnd, pItem->m_curValue) )
			pItem->m_curValue = dlgFolder.GetFolder();
		
		m_btnCtrl.ShowWindow(SW_HIDE);
		
		Invalidate();
	}
}

void CPropertyList::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_bTracking)
	{
		//if columns were being resized then this indicates
		//that mouse is up so resizing is done.  Need to redraw
		//columns to reflect their new widths.
		
		m_bTracking = FALSE;
		//if mouse was captured then release it
		if (GetCapture()==this)
			::ReleaseCapture();

		::ClipCursor(NULL);

		CClientDC dc(this);
		InvertLine(&dc,CPoint(point.x,m_nDivTop),CPoint(point.x,m_nDivBtm));
		//set the divider position to the new value
		m_nDivider = point.x;

		//redraw
		Invalidate();
	}
	else
	{
		BOOL loc;
		int i = ItemFromPoint(point,loc);
		if(m_curSel != i)
		{
			//before changing Selection
			SaveCurrentPropertyValue();
		}
		m_curSel = i;
		CListBox::OnLButtonUp(nFlags, point);
	}
}

void CPropertyList::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if ((point.x>=m_nDivider-5) && (point.x<=m_nDivider+5))
	{
		//if mouse clicked on divider line, then start resizing

		::SetCursor(m_hCursorSize);

		CRect windowRect;
		GetWindowRect(windowRect);
		windowRect.left += 10; windowRect.right -= 10;
		//do not let mouse leave the list box boundary
		::ClipCursor(windowRect);
		
		if (m_cbxDropDownList)
			m_cbxDropDownList.ShowWindow(SW_HIDE);
		if (m_editBox)
			m_editBox.ShowWindow(SW_HIDE);

		CRect clientRect;
		GetClientRect(clientRect);

		m_bTracking = TRUE;
		m_nDivTop = clientRect.top;
		m_nDivBtm = clientRect.bottom;
		m_nOldDivX = point.x;

		CClientDC dc(this);
		InvertLine(&dc,CPoint(m_nOldDivX,m_nDivTop),CPoint(m_nOldDivX,m_nDivBtm));

		//capture the mouse
		SetCapture();
	}
	else
	{
		m_bTracking = FALSE;
		CListBox::OnLButtonDown(nFlags, point);
	}
}

void CPropertyList::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bTracking)
	{
		//move divider line to the mouse pos. if columns are
		//currently being resized
		CClientDC dc(this);
		//remove old divider line
		InvertLine(&dc,CPoint(m_nOldDivX,m_nDivTop),CPoint(m_nOldDivX,m_nDivBtm));
		//draw new divider line
		InvertLine(&dc,CPoint(point.x,m_nDivTop),CPoint(point.x,m_nDivBtm));
		m_nOldDivX = point.x;
	}
	else if ((point.x >= m_nDivider-5) && (point.x <= m_nDivider+5))
		//set the cursor to a sizing cursor if the cursor is over the row divider
		::SetCursor(m_hCursorSize);
	else
		CListBox::OnMouseMove(nFlags, point);
}

void CPropertyList::InvertLine(CDC* pDC,CPoint ptFrom,CPoint ptTo)
{
	int nOldMode = pDC->SetROP2(R2_NOT);
	
	pDC->MoveTo(ptFrom);
	pDC->LineTo(ptTo);

	pDC->SetROP2(nOldMode);
}

void CPropertyList::PreSubclassWindow()
{
	m_bDivIsSet = FALSE;
	m_nDivider = 0;
	m_bTracking = FALSE;
	m_curSel = 1;

	m_hCursorSize = AfxGetApp()->LoadStandardCursor(IDC_SIZEWE);
	m_hCursorArrow = AfxGetApp()->LoadStandardCursor(IDC_ARROW);

	m_SSerif8Font.CreatePointFont(80,_T("MS Sans Serif"));
}

/*
int CPropertyList::GetCount()
{
	return m_mapData.size();
}

BOOL CPropertyList::GetItem(int index, CPropertyItem& item)
{
	if(index >= 0 && index < (int)m_mapData.size())
	{
		std::map<std::string, CPropertyItem>::iterator mit = m_mapData.begin();
		while(index-- > 0)
		{
			if( mit++ == m_mapData.end() )
				return FALSE;
		}

		item = mit->second;

		return TRUE;
	}

	return FALSE;
}
*/

POSITION CPropertyList::GetHead()
{
	SaveCurrentPropertyValue();
	
	return (m_mit = m_mapData.begin()) == m_mapData.end() ? NULL : (POSITION) (long) &m_mit;
}

const CPropertyItem* CPropertyList::GetNext(POSITION& pos)
{
	if(pos == NULL)
	{
		return NULL;
	}

	CPropertyItem* p = &m_mit->second;
	
	pos = m_mit++ == m_mapData.end() ? NULL : (POSITION) (long) &m_mit;

	if(pos == NULL || m_mit == m_mapData.end())
	{
		pos = NULL;
	}
	
	return p;
}

BOOL CPropertyList::DeleteCurrentProperty()
{
	int iSel = GetCurSel();

	CPropertyItem* pItem = static_cast<CPropertyItem*>(GetItemDataPtr(iSel));
	if(!VALIDPOINT(pItem))
		return FALSE;
	
	if(iSel >= 0 && pItem)
	{
		CString strKey = MAKEKEY(pItem->m_catalog, pItem->m_propName);
		
		std::map<CString, CPropertyItem>::iterator mit;
		if((mit = m_mapData.find(strKey.GetBuffer(0))) != m_mapData.end())
		{
			DeleteString(iSel);
			
			m_mapData.erase(mit);
			
			return TRUE;
		}
	}
	
	return FALSE;
}

void CPropertyList::DeleteAll()
{
	SaveCurrentPropertyValue();
	
	ResetContent();
	m_mapData.clear();
}

void CPropertyList::SaveCurrentPropertyValue()
{
	//¶¼ÊÇCIPAddressCtrlÈÇµÄ»ö
	OnKillFocusIpCtrl();
	OnKillfocusDate();
	OnKillfocusDatetime();

	OnKillfocusCmbBox();
	OnKillfocusCmbBox2();
	OnKillfocusChkCmbBox();
	OnKillfocusDate();
	NMHDR a;
	LRESULT b;
	OnKillfocusTime(&a, &b);

	if(IsWindow(m_btnCtrl.m_hWnd))
		m_btnCtrl.ShowWindow(SW_HIDE);
}

UINT CPropertyList::GetItemIndex(CWnd* pWnd)
{
	if(!pWnd)
		return LB_ERR;
	
	CRect rectList, rectItem;
	GetWindowRect(rectList);
	pWnd->GetWindowRect(&rectItem);
	CPoint point;
	point.x = rectItem.left - rectList.left;
	point.y = rectItem.top - rectList.top;
	BOOL bOut;
	UINT iItemIndex = ItemFromPoint(point, bOut);
	if(bOut)
	{
		iItemIndex = LB_ERR;
	}

	return iItemIndex;
}

int CPropertyList::GetPos4Catalog(LPCTSTR szCatalog)
{
	int iCnt = GetCount(), iPos=-1;
	
	for(int i=0; i<iCnt;i++)
	{
		CPropertyItem* pItem = (CPropertyItem*)GetItemDataPtr(i);

		if(VALIDPOINT(pItem) 
			&& PIT_CATALOG == pItem->m_nItemType 
			&& pItem->m_catalog.CompareNoCase(szCatalog) == 0)
		{
			iPos = i;
			break;
		}
	}
	
	return iPos;
}

int CPropertyList::GetPos4NewItem(LPCTSTR szCatalog, LPCTSTR szPropName)
{
	int iCnt = GetCount(), iPos=GetPos4Catalog(szCatalog)+1;

	while(iPos<iCnt)
	{
		CPropertyItem* pItem = (CPropertyItem*)GetItemDataPtr(iPos);

		if(VALIDPOINT(pItem) 
			&& (pItem->m_catalog.CompareNoCase(szCatalog) != 0
			|| pItem->m_propName.CompareNoCase(szPropName) > 0) )
		{
			break;
		}

		++iPos;
	}

	iPos=iPos<0?0:iPos;
	iPos=iPos>iCnt?iCnt:iPos;

	return iPos;
}

int CPropertyList::GetPos4NewCata(LPCTSTR szCatalog)
{
	int iCnt = GetCount(), iPos=0;
	
	while(iPos<iCnt)
	{
		CPropertyItem* pItem = (CPropertyItem*)GetItemDataPtr(iPos);

		if(VALIDPOINT(pItem) 
			&& pItem->m_catalog.CompareNoCase(szCatalog) > 0)
		{
			break;
		}
		
		++iPos;
	}
	
	return iPos;
}

int CPropertyList::InsertCatalog(LPCTSTR szCatalog)
{
	CString sKey = MAKEKEY(szCatalog, _T(""));
	int iIndex = -1;
	
	if(m_mapData.find(sKey) == m_mapData.end())
	{
		iIndex = GetPos4NewCata(szCatalog);

		iIndex = InsertString(iIndex, szCatalog);

		CPropertyItem item(szCatalog, _T(""), _T(""), PIT_CATALOG, _T(""), _T(""));
		
		m_mapData[sKey] = item;
		
		SetItemDataPtr(iIndex, &(m_mapData[sKey]) );
	}

	return iIndex;
}

BOOL CPropertyList::IsCollapsedCatalog(LPCTSTR szCatalog)
{
	CString strKey = MAKEKEY(szCatalog, _T(""));

	if(m_mapData.find(strKey) != m_mapData.end())
	{
		CPropertyItem* p = &m_mapData[strKey];
		if(VALIDPOINT(p) && PIT_CATALOG == p->m_nItemType)
		{
			return p->IsCollapsed();
		}
	}

	return TRUE;
}
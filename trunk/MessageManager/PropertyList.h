#if !defined(AFX_PROPERTYLIST_H__74205380_1B56_11D4_BC48_00105AA2186F__INCLUDED_)
#define AFX_PROPERTYLIST_H__74205380_1B56_11D4_BC48_00105AA2186F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyList.h : header file
//

//////////////////////////////////////////////////
// CPropertyList - Property List
//
// Author: Runming yan
// Email:  HankersYan@msn.com
// Copyright 2005, Runming yan
//
// You may freely use or modify this code provided this
// Copyright is included in all derived versions.
//
// History - 2005/7/3
//
// Remarks:
// This class implements a properties List like vb
//
//	Attention, please! 
//	checkboxes.bmp is neccessary, then it must be added into resources
//	and it's resourceID must be "IDB_CHECKBOX"
//	                            ..............
//
//
//	the five following files also are neccessary.
//	ChkComboBox.cpp (Refered to "http://www.codeproject.com/combobox/checkcombo.asp")
//	ChkComboBox.h	(CheckComboBox Control By Magnus Egelberg, Lundalogik)
//	FoldersDialog.cpp 
//	FoldersDialog.h
//	MemDC.h
//
//




#pragma warning(disable:4786)

#include <map>
#include <string>
#include <list>


#include "ChkComboBox.h"

//
//PIT = property item type
#define PIT_EDIT		0	//Edit
#define PIT_COMBO		1	//Dropdownlist
#define PIT_COMBO2		2	//Dropdown(editable)
#define PIT_COMBO3		3	//CheckComboBox (multiSel)	e.g. "option1|option2"
#define PIT_COLOR		4	//Color						e.g. "128 128 128"
#define PIT_FONT		5	//Font
#define PIT_FILE		6	//File Select Dialog
#define PIT_FOLDER		7	//Folder Select Dialog
#define PIT_CHECK		8	//BOOL						e.g. "Y"
#define PIT_IP			9	//IP Address
#define PIT_DATE		10	//Date						e.g. "2005/7/3"
#define PIT_TIME		11	//Time						e.g. "12:32:45"
#define PIT_DATETIME	12	//Date & Time				e.g. "2005/7/3 12:32:45"
#define PIT_HOTKEY		13	//Hot Key	Temporarily unavailable
#define PIT_CATALOG		99	//Catalog

inline int GetSafePITType(int iType)
{
	switch(iType)
	{
	case PIT_COMBO:
	case PIT_COMBO2:
	case PIT_COMBO3:
	case PIT_COLOR:
	case PIT_FONT:
	case PIT_FILE:
	case PIT_CHECK:
	case PIT_FOLDER:
	case PIT_IP:
	case PIT_DATE:
	case PIT_TIME:
	case PIT_DATETIME:
//	case PIT_HOTKEY:
	case PIT_CATALOG:
		return iType;
	default:
		return PIT_EDIT;
	}

	return PIT_EDIT;
};


//////////////////////////////////////////////////////////////////////////
// Class Name:	CPropertyItem
// Abstract:
// Exception:
// Remarks:		CPropertyList Item data struct
//				m_catalog + m_propName is the Keyword
//
// Author:		Runming yan
// Date:
//////////////////////////////////////////////////////////////////////////
class CPropertyItem
{
public:
	CString m_catalog;
	CString m_propName;
	CString m_curValue;
	CString m_cmbItems;
	CString m_propDesc;
	int m_nItemType;

	CPropertyItem() : m_iData(0)
	{
	};
	CPropertyItem(const CPropertyItem& item)
	{
		m_catalog	= item.m_catalog;
		m_propName  = item.m_propName;
		m_curValue  = item.m_curValue;
		m_nItemType = item.m_nItemType;
		m_cmbItems  = item.m_cmbItems;
		m_propDesc	= item.m_propDesc;
		m_iData		= item.m_iData;
	};
	CPropertyItem(CString catalog, CString propName, CString curValue,
				  int nItemType, CString cmbItems, CString propDesc)
	{
		m_catalog	= catalog;
		m_propName  = propName;
		m_curValue  = curValue;
		m_nItemType = nItemType;
		m_cmbItems  = cmbItems;
		m_propDesc	= propDesc;
		m_iData		= 0;
	};

	BOOL IsCollapsed()
	{
		return GetBit(0);
	};
	void SetCollapsed(BOOL bCollapsed)
	{
		SetBit(0, !bCollapsed);
	};

	BOOL IsHidden()
	{
		return GetBit(1);
	};
	void SetHidden(BOOL bHidden)
	{
		SetBit(1, !bHidden);
	};

	BOOL operator==(const CPropertyItem& item)
	{
		return m_catalog.CompareNoCase(item.m_catalog) == 0 && m_propName.CompareNoCase(item.m_propName) == 0;
	};

private:
	BOOL GetBit(int index)
	{
		if(index < 0 && index >= sizeof(m_iData))
		{
			_ASSERTE(FALSE);
			return FALSE;
		}
		
		return (m_iData>>index)&0x1;
	};
	void SetBit(int index, BOOL bZero)
	{
		if(index < 0 && index >= sizeof(m_iData))
		{
			_ASSERTE(FALSE);
			return;
		}
		
		if(bZero)
			m_iData &= ~(1<<index);
		else
			m_iData |= 1<<index;
	};
	
	UINT m_iData;	//record the private data of this item
};

//
//Function point that is occured when the index of Selected Item changes
//
typedef void (*PFN_OnSelChanged)(const CPropertyItem* pCurItem);


/////////////////////////////////////////////////////////////////////////////
// Class Name:	CPropertyList
// Abstract:	CListBox
// Exception:
// Remarks:		CPropertyList Item data struct
//
//
// Author:		Runming yan
// Date:
//////////////////////////////////////////////////////////////////////////
class CPropertyList : public CListBox
{
// Construction
public:
	CPropertyList();
	virtual ~CPropertyList();

// Attributes
public:
	
// Operations
public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	BOOL AddPropItem(CString catalog, CString propName, CString curValue, int nItemType, CString cmbItems, CString propDesc = _T(""));

	BOOL GetPropItem(LPCTSTR catalog, LPCTSTR szPropName, CPropertyItem& item);
	CString GetPropValue(LPCTSTR catalog, LPCTSTR szPropName);
	POSITION GetHead();
	const CPropertyItem* GetNext(POSITION& pos);
//	int GetCount();
//	BOOL GetItem(int index, CPropertyItem& item);

	void SetPFNOnSelChanged(PFN_OnSelChanged pfn){m_pfnOnSelChanged = pfn;};

	BOOL DeleteCurrentProperty();
	void DeleteAll();
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyList)
protected:
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	//{{AFX_MSG(CPropertyList)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSelchange();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg void OnKillfocusCmbBox();
	afx_msg void OnKillfocusCmbBox2();
	afx_msg void OnKillfocusChkCmbBox();
	afx_msg void OnKillfocusEditBox();
	afx_msg void OnChangeEditBox();
	afx_msg void OnButton();
	afx_msg void OnKillFocusIpCtrl();
	//afx_msg void OnKillfocusDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusDate();
	afx_msg void OnKillfocusTime(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusDatetime();

	void OnSelectCatalog(CPropertyItem* pItem);
	void OnSelectComboBox(CComboBox& cbx, CRect& rect, CPropertyItem* pItem);
	void OnSelectEdit(CRect& rect, CPropertyItem* pItem);
	void OnSelectIP(CRect& rect, CPropertyItem* pItem);
	void OnSelectCheck(CPropertyItem* pItem);
	void OnSelectDateTime(CRect& rect, CPropertyItem* pItem);
	void OnSelectChkComboBox(CRect& rect, CPropertyItem* pItem);
	void OnSelectHotKey(CRect& rect, CPropertyItem* pItem);
	
	DECLARE_MESSAGE_MAP()
private:
	void InvertLine(CDC* pDC,CPoint ptFrom,CPoint ptTo);
	void DisplayButton(CRect region);
	void SaveCurrentPropertyValue();
	UINT GetItemIndex(CWnd* pWnd);
	BOOL IsCollapsedCatalog(LPCTSTR szCatalog);
		
	int GetPos4NewCata(LPCTSTR szCatalog);
	int GetPos4NewItem(LPCTSTR szCatalog, LPCTSTR szPropName);
	int GetPos4Catalog(LPCTSTR szCatalog);
	int InsertCatalog(LPCTSTR szCatalog);

	CComboBox m_cbxDropDownList, m_cbxDropDown;
	CEdit m_editBox;
	CButton m_btnCtrl, m_chk;
	CFont m_SSerif8Font;
	CIPAddressCtrl m_ipCtrl;
	CDateTimeCtrl m_date, m_time;
	CChkComboBox m_chkCbx;
	CHotKeyCtrl m_hotKey;
	
	int m_curSel,m_prevSel;
	int m_nDivider;
	int m_nDivTop;
	int m_nDivBtm;
	int m_nOldDivX;
	int m_nLastBox;
	BOOL m_bTracking;
	BOOL m_bDivIsSet;
	HCURSOR m_hCursorArrow;
	HCURSOR m_hCursorSize;
	CBitmap m_bmpChk;
	//CDC m_dcChk;

	std::map<CString, CPropertyItem> m_mapData;
	std::map<CString, CPropertyItem>::iterator m_mit;
	PFN_OnSelChanged m_pfnOnSelChanged;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYLIST_H__74205380_1B56_11D4_BC48_00105AA2186F__INCLUDED_)

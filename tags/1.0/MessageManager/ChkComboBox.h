#if !defined(AFX_CHKCOMBOBOX_H__60B47B7F_99F3_4CAE_AB54_531B55C1DAA5__INCLUDED_)
#define AFX_CHKCOMBOBOX_H__60B47B7F_99F3_4CAE_AB54_531B55C1DAA5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChkComboBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChkComboBox dialog

class CChkComboBox : public CComboBox
{
	DECLARE_DYNAMIC(CChkComboBox)

public:
    CChkComboBox(void);
    virtual ~CChkComboBox(void);
	
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	
	// Selects all/unselects the specified item
	INT SetCheck(INT nIndex, BOOL bFlag);
	
	// Returns checked state
	BOOL GetCheck(INT nIndex);
	
	// Selects all/unselects all
	void SelectAll(BOOL bCheck = TRUE);
	
    void SetMe();
	
	void ClearText(){
		m_strText.Empty();
	}

	void SetOptions(LPCTSTR szOptions);
	void SetValues(LPCTSTR szValue);
	
    void SetCallback(LONG fnCallback);
    
protected:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCheckComboBox)
protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_VIRTUAL
	
	//{{AFX_MSG(CCheckComboBox)
	afx_msg LRESULT OnCtlColorListBox(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetText(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetTextLength(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDropDown();
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
		
protected:
	// Routine to update the text
	void RecalcText();
	
	// The subclassed COMBOLBOX window (notice the 'L')
	HWND m_hListBox;
	
	// The string containing the text to display
	CString m_strText;
	BOOL m_bTextUpdated;
	
	// A flag used in MeasureItem, see comments there
	BOOL m_bItemHeightSet;
    
    LONG m_fnComboBoxCallBack;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHKCOMBOBOX_H__60B47B7F_99F3_4CAE_AB54_531B55C1DAA5__INCLUDED_)

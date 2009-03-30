#pragma once
#include "afxwin.h"


// CNamepipeSelectDlg dialog

class CNamepipeSelectDlg : public CDialog
{
	DECLARE_DYNAMIC(CNamepipeSelectDlg)

public:
	CNamepipeSelectDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNamepipeSelectDlg();

	void GetPipeName( CString& str )const;

// Dialog Data
	enum { IDD = IDD_DIALOG_SELECT_DEBUG_NAMEPIPE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	CComboBox m_NamepipeListCtrl;
};

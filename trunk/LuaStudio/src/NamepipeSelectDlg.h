#pragma once
#include "afxwin.h"


// CNamepipeSelectDlg dialog

class CNamepipeSelectDlg : public CDialog
{
	DECLARE_DYNAMIC(CNamepipeSelectDlg)

public:
	CNamepipeSelectDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNamepipeSelectDlg();

	CString GetPipeName()const;

// Dialog Data
	enum { IDD = IDD_SELECT_DEBUGER };

protected:
	CListCtrl	m_NamepipeListCtrl;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	CString m_strPipename;
};

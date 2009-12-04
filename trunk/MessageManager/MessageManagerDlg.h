
// MessageManagerDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "PropertyList.h"

// CMessageManagerDlg 对话框
struct Property
{
	enum type { I8, I16, I32, I64, U8, U16, U32, U64, FLT, LFLT, ASTR, USTR, SUB_DEF, CONTAINER };
	type	m_type;				// 属性类型
	CString m_name;				// 属性名
};

struct PropertyContainer	:	public Property
{
	CString m_object_name;	// 容纳对象名
};

class CMessageManagerDlg : public CDialog
{
// 构造
public:
	CMessageManagerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MESSAGEMANAGER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CBitmap			m_Bitmap;
	CImageList		m_ImageList;
	CTreeCtrl		m_MessageTreeCtrl;
	CPropertyList	m_PropertyListCtrl;
	afx_msg void OnTreeAppend();
	afx_msg void OnTreeDelete();
	afx_msg void OnNMRClickMessageTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnEndlabeleditMessageTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnBeginlabeleditMessageTree(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnAppendProperty( UINT nID );
};

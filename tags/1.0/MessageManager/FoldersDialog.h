// FoldersDialog.h: interface for the CFoldersDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FOLDERSDIALOG_H__4697F64B_AE4D_4345_9704_C43AD7C788C1__INCLUDED_)
#define AFX_FOLDERSDIALOG_H__4697F64B_AE4D_4345_9704_C43AD7C788C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdlib.h>
#include <tchar.h>
#include <Shlobj.h>

class CFoldersDialog  
{
public:
	CFoldersDialog();
	virtual ~CFoldersDialog();
	BOOL BrowseFolder(HWND, CString curPath, CString rootPath = _T(""));
	CString GetFolder();
};

#endif // !defined(AFX_FOLDERSDIALOG_H__4697F64B_AE4D_4345_9704_C43AD7C788C1__INCLUDED_)

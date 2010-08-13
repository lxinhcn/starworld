// FoldersDialog.cpp: implementation of the CFoldersDialog class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "FoldersDialog.h"

TCHAR szSelectedDir[MAX_PATH];

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFoldersDialog::CFoldersDialog()
{

}

CFoldersDialog::~CFoldersDialog()
{

}

LPITEMIDLIST ConvertPathToLpItemIdList(const char *pszPath)
{
	LPITEMIDLIST  pidl;
	LPSHELLFOLDER pDesktopFolder;
	OLECHAR       olePath[MAX_PATH];
	ULONG         chEaten;
	ULONG         dwAttributes;
	HRESULT       hr;

	if (SUCCEEDED(SHGetDesktopFolder(&pDesktopFolder)))
	{
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pszPath, -1, olePath, MAX_PATH);
		hr = pDesktopFolder->ParseDisplayName(NULL, NULL, olePath, &chEaten, &pidl, &dwAttributes);
		pDesktopFolder->Release();
	}
	return pidl;
}

int CALLBACK BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lp, LPARAM pData) 
{
	switch(uMsg) 
	{
	case BFFM_INITIALIZED:
		{
			/* change the selected folder. */
			SendMessage(hwnd, BFFM_SETSELECTION, TRUE, pData);
			break;
		}
	case BFFM_SELCHANGED: 
		{
		   // Set the status window to the currently selected path.
		   if (SHGetPathFromIDList((LPITEMIDLIST) lp , szSelectedDir)) 
			  SendMessage(hwnd,BFFM_SETSTATUSTEXT, 0, (LPARAM)szSelectedDir);
		   break;
		}
	default:
		   break;
	}
	return 0;
}

BOOL CFoldersDialog::BrowseFolder(HWND hWnd, CString curPath, CString rootPath)
{
	BROWSEINFO bi;
	TCHAR szDir[MAX_PATH];
	LPITEMIDLIST pidl;
	LPMALLOC pMalloc;
	BOOL bRet;

	if (SUCCEEDED(SHGetMalloc(&pMalloc))) 
	{
		bRet = TRUE;
		ZeroMemory(&bi,sizeof(bi));
		bi.hwndOwner = hWnd;
		bi.pszDisplayName = 0;
		bi.lpszTitle = 0;
		USES_CONVERSION;
		bi.pidlRoot = ConvertPathToLpItemIdList(T2A(rootPath));
		bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;
		bi.lpfn = BrowseCallbackProc;
		bi.lParam = (long) curPath.GetBuffer(0);

		pidl = SHBrowseForFolder(&bi);
		if (pidl)
		   SHGetPathFromIDList(pidl,szDir);
		else
			bRet = FALSE;
		pMalloc->Free(pidl); 
		pMalloc->Release();
	}
	return bRet;
}

CString CFoldersDialog::GetFolder()
{
	return szSelectedDir;
}
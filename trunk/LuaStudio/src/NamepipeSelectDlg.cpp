// NamepipeSelectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NamepipeSelectDlg.h"


// CNamepipeSelectDlg dialog

IMPLEMENT_DYNAMIC(CNamepipeSelectDlg, CDialog)

CNamepipeSelectDlg::CNamepipeSelectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNamepipeSelectDlg::IDD, pParent)
{

}

CNamepipeSelectDlg::~CNamepipeSelectDlg()
{
}

BEGIN_MESSAGE_MAP(CNamepipeSelectDlg, CDialog)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

void CNamepipeSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control( pDX, IDC_LIST_LUAPIPE, m_NamepipeListCtrl );
}

// CNamepipeSelectDlg message handlers
CString CNamepipeSelectDlg::GetPipeName()const
{	
	return m_strPipename;
}

#define FileDirectoryInformation 1
#define STATUS_NO_MORE_FILES 0x80000006L

typedef struct
{
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct
{
	LONG Status;
	ULONG Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

typedef struct {
	ULONG NextEntryOffset;
	ULONG FileIndex;
	LARGE_INTEGER CreationTime;
	LARGE_INTEGER LastAccessTime;
	LARGE_INTEGER LastWriteTime;
	LARGE_INTEGER ChangeTime;
	LARGE_INTEGER EndOfFile;
	LARGE_INTEGER AllocationSize;
	ULONG FileAttributes;
	ULONG FileNameLength;
	union {
		struct {
			WCHAR FileName[1];
		} FileDirectoryInformationClass;

		struct {
			DWORD dwUknown1;
			WCHAR FileName[1];
		} FileFullDirectoryInformationClass;

		struct {
			DWORD dwUknown2;
			USHORT AltFileNameLen;
			WCHAR AltFileName[12];
			WCHAR FileName[1];
		} FileBothDirectoryInformationClass;
	};
} FILE_QUERY_DIRECTORY, *PFILE_QUERY_DIRECTORY;


// ntdll!NtQueryDirectoryFile (NT specific!)
//
// The function searches a directory for a file whose name and attributes
// match those specified in the function call.
//
// NTSYSAPI
// NTSTATUS
// NTAPI
// NtQueryDirectoryFile(
//    IN HANDLE FileHandle,                      // handle to the file
//    IN HANDLE EventHandle OPTIONAL,
//    IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
//    IN PVOID ApcContext OPTIONAL,
//    OUT PIO_STATUS_BLOCK IoStatusBlock,
//    OUT PVOID Buffer,                          // pointer to the buffer to receive the result
//    IN ULONG BufferLength,                     // length of Buffer
//    IN FILE_INFORMATION_CLASS InformationClass,// information type
//    IN BOOLEAN ReturnByOne,                    // each call returns info for only one file
//    IN PUNICODE_STRING FileTemplate OPTIONAL,  // template for search
//    IN BOOLEAN Reset                           // restart search
// );
typedef LONG (WINAPI *PROCNTQDF)( HANDLE,HANDLE,PVOID,PVOID,PIO_STATUS_BLOCK,PVOID,ULONG,
								 UINT,BOOL,PUNICODE_STRING,BOOL );

PROCNTQDF NtQueryDirectoryFile;

BOOL CNamepipeSelectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	DWORD dwStyle = ListView_GetExtendedListViewStyle( m_NamepipeListCtrl );
	//Add the full row select and grid line style to the existing extended styles
	dwStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP | LVS_EX_TRACKSELECT;
	ListView_SetExtendedListViewStyle ( m_NamepipeListCtrl, dwStyle );

	m_NamepipeListCtrl.InsertColumn( 0, _T("Name"), LVCFMT_LEFT, 100 );
	m_NamepipeListCtrl.InsertColumn( 1, _T("Process"), LVCFMT_LEFT, 80 );
	m_NamepipeListCtrl.InsertColumn( 2, _T("Thread"), LVCFMT_LEFT, 80 );

	LONG ntStatus;
	IO_STATUS_BLOCK IoStatus;
	HANDLE hPipe;
	BOOL bReset = TRUE;
	PFILE_QUERY_DIRECTORY DirInfo,
		TmpInfo;

	NtQueryDirectoryFile = (PROCNTQDF)GetProcAddress(
		GetModuleHandle("ntdll"),
		"NtQueryDirectoryFile"
		);

	if (!NtQueryDirectoryFile)
		return TRUE;

	hPipe = CreateFile("\\\\.\\Pipe\\",GENERIC_READ,
		FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
		NULL,OPEN_EXISTING,0,NULL);

	if(hPipe == INVALID_HANDLE_VALUE)
		return TRUE;

	DirInfo = (PFILE_QUERY_DIRECTORY) new BYTE[0x1000];
	
	USES_CONVERSION;
	while(1)
	{
		ntStatus = NtQueryDirectoryFile(hPipe,NULL,NULL,NULL,&IoStatus,DirInfo,0x1000,
			FileDirectoryInformation,FALSE,NULL,bReset);

		if (ntStatus!=NO_ERROR)
		{
			if (ntStatus == STATUS_NO_MORE_FILES)
				break;

			return TRUE;
		}

		TmpInfo = DirInfo;
		while(1)
		{
			if( wcsnicmp( L"lua\\", TmpInfo->FileDirectoryInformationClass.FileName, 4 ) == 0 )
			{
				LPWSTR name = (wchar_t*)malloc( ( TmpInfo->FileNameLength + 1 )* sizeof(wchar_t) );
				wcsncpy( name, TmpInfo->FileDirectoryInformationClass.FileName, TmpInfo->FileNameLength );
				name[TmpInfo->FileNameLength/2] = 0;
				LPSTR tok = _tcstok( W2T(name), _T(".") );

				if( tok == NULL ) break;
				int i = m_NamepipeListCtrl.InsertItem( 0, tok );

				tok = _tcstok( NULL, _T(".") );
				if( tok == NULL ) break;
				m_NamepipeListCtrl.SetItemText( i, 1, tok );

				tok = _tcstok( NULL, _T(".") );
				if( tok == NULL ) break;
				m_NamepipeListCtrl.SetItemText( i, 2, tok );
				free( name );
			}
			
			if(TmpInfo->NextEntryOffset==0)
				break;
			else
				TmpInfo = (PFILE_QUERY_DIRECTORY)((DWORD)TmpInfo+TmpInfo->NextEntryOffset);
		}

		bReset = FALSE;
	}

	delete DirInfo;
	CloseHandle(hPipe);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CNamepipeSelectDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	POSITION pos = m_NamepipeListCtrl.GetFirstSelectedItemPosition();
	if( pos )
	{
		int sel = m_NamepipeListCtrl.GetNextSelectedItem( pos );
		if( sel < 0 )
		{
			MessageBox( _T("必须选择一个调试管道。"), _T("错误") );
			return;
		}
		m_strPipename.Empty();
		CString item[3];
		for( int i = 0; i < 3; ++i )
			item[i] = m_NamepipeListCtrl.GetItemText( sel, i );
		m_strPipename = item[0] + _T(".") + item[1] + _T(".") + item[2];
	}
	else
	{
		MessageBox( _T("必须选择一个调试管道。"), _T("错误") );
		return;
	}
	CDialog::OnOK();
}

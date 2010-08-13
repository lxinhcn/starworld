
// MessageManagerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MessageManager.h"
#include "MessageManagerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

LPCTSTR Property::TypeName[] = 
{
	_T("int8"),
	_T("int16"),
	_T("int32"),
	_T("int64"),
	_T("uint8"),
	_T("uint16"),
	_T("uint32"),
	_T("uint64"),
	_T("float"),
	_T("double"),
	_T("astring"),
	_T("wstring"),
	_T("tstring"),
	_T("buffer"),
	_T("{}"),
	_T("[]"),
	_T("<>"),
	_T("message"),
	_T("namespace"),
};

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CMessageManagerDlg 对话框
CMessageManagerDlg::CMessageManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMessageManagerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMessageManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MESSAGE_TREE, m_MessageTreeCtrl);
	DDX_Control(pDX, IDC_PROPERTY_LIST, m_PropertyListCtrl);
	DDX_Control(pDX, IDC_EDIT_COMMAND, m_CommandEditCtrl);
}

BEGIN_MESSAGE_MAP(CMessageManagerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_TREE_APPEND, &CMessageManagerDlg::OnTreeAppend)
	ON_COMMAND(ID_TREE_DELETE, &CMessageManagerDlg::OnTreeDelete)
	ON_COMMAND_RANGE(ID_INT8, ID_CONTAINER, &CMessageManagerDlg::OnAppendProperty )
	ON_NOTIFY(NM_RCLICK, IDC_MESSAGE_TREE, &CMessageManagerDlg::OnNMRClickMessageTree)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_MESSAGE_TREE, &CMessageManagerDlg::OnTvnEndlabeleditMessageTree)
	ON_NOTIFY(TVN_BEGINLABELEDIT, IDC_MESSAGE_TREE, &CMessageManagerDlg::OnTvnBeginlabeleditMessageTree)
	ON_NOTIFY(TVN_GETDISPINFO, IDC_MESSAGE_TREE, &CMessageManagerDlg::OnTvnGetdispinfoMessageTree)
	ON_NOTIFY(TVN_DELETEITEM, IDC_MESSAGE_TREE, &CMessageManagerDlg::OnTvnDeleteitemMessageTree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_MESSAGE_TREE, &CMessageManagerDlg::OnTvnSelchangedMessageTree)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CMessageManagerDlg 消息处理程序

BOOL CMessageManagerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	ShowWindow(SW_NORMAL);

	m_PropertyListCtrl.ShowWindow( SW_HIDE );
	m_PropertyListCtrl.EnableWindow( FALSE );

	// TODO: 在此添加额外的初始化代码
	m_Bitmap.LoadBitmap( IDB_TREEIMAGE );
	m_ImageList.Create( 16, 16, ILC_COLOR24, 7, 1 );
	m_ImageList.Add( &m_Bitmap, RGB(0,0,0) );
	m_MessageTreeCtrl.SetImageList( &m_ImageList, TVSIL_NORMAL );

	AdjustLayout();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMessageManagerDlg::AdjustLayout()
{
	CRect rcWindow;
	GetClientRect( rcWindow );
	rcWindow.InflateRect( -5, -5 );

	CRect rcPart[3] = { rcWindow, rcWindow, rcWindow };

	if( m_MessageTreeCtrl.GetSafeHwnd() )
	{
		rcPart[0].right = __max( rcWindow.Width()/2, 150 );
		m_MessageTreeCtrl.MoveWindow( rcPart[0] );
	}

	if( m_PropertyGridCtrl.GetSafeHwnd() )
	{
		rcPart[1].left = __max( rcWindow.Width()/2, 150 ) + 5;
		rcPart[1].bottom -= 50;

		m_PropertyGridCtrl.MoveWindow( rcPart[1] );
	}

	if( m_CommandEditCtrl.GetSafeHwnd() )
	{
		rcPart[2].left = __max( rcWindow.Width()/2, 150 ) + 5;
		rcPart[2].top = rcPart[2].bottom - 45;

		m_CommandEditCtrl.MoveWindow( rcPart[2] );
	}
}

void CMessageManagerDlg::ExecuteCommand( CString strCmd )
{

}

void CMessageManagerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMessageManagerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMessageManagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMessageManagerDlg::OnTreeAppend()
{
	// TODO: 在此添加命令处理程序代码
	HTREEITEM hItem = m_MessageTreeCtrl.GetSelectedItem();

	HTREEITEM hInsert = m_MessageTreeCtrl.InsertItem( LPSTR_TEXTCALLBACK, 3, 3, hItem?hItem:TVI_ROOT );
	Property *pProperty = new PropertyMessage();
	m_MessageTreeCtrl.SetItemData( hInsert, (DWORD_PTR)pProperty );
	m_MessageTreeCtrl.SelectItem( hInsert );
	m_MessageTreeCtrl.EditLabel( hInsert );
}

void CMessageManagerDlg::OnTreeDelete()
{
	// TODO: 在此添加命令处理程序代码
	m_MessageTreeCtrl.DeleteItem( m_MessageTreeCtrl.GetSelectedItem() );
}

void CMessageManagerDlg::OnNMRClickMessageTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	
	CMenu Menu;

	CPoint ptCursor, ptTreeHit;
	GetCursorPos( &ptCursor );

	ptTreeHit = ptCursor;
	m_MessageTreeCtrl.ScreenToClient( &ptTreeHit );
	HTREEITEM hItem = m_MessageTreeCtrl.HitTest( ptTreeHit );
	m_MessageTreeCtrl.SelectItem( hItem );

	Menu.LoadMenu( IDR_MENU_TREEPOP );
	CMenu *pSub = Menu.GetSubMenu( 0 );

	pSub->EnableMenuItem( ID_TREE_DELETE, hItem?MF_ENABLED:MF_DISABLED|MF_GRAYED );
	pSub->EnableMenuItem( ID_TREE_APPEND, hItem?MF_DISABLED|MF_GRAYED:MF_ENABLED );

	pSub->TrackPopupMenu( TPM_LEFTALIGN, ptCursor.x, ptCursor.y, this );

}

void CMessageManagerDlg::OnTvnEndlabeleditMessageTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);

	// TODO: 在此添加控件通知处理程序代码
	if( pTVDispInfo->item.pszText )
	{
		Property *pProperty = (Property *)pTVDispInfo->item.lParam;
		pProperty->m_name = pTVDispInfo->item.pszText;
	}
	*pResult = FALSE;
}

void CMessageManagerDlg::OnTvnBeginlabeleditMessageTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	Property *pProperty = (Property*)m_MessageTreeCtrl.GetItemData( pTVDispInfo->item.hItem );
	if( pProperty )
	{
		CEdit *pEdit = m_MessageTreeCtrl.GetEditControl();
		pEdit->SetWindowText( pProperty->m_name );
	}
	*pResult = 0;
}

BOOL CMessageManagerDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if( pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST )
	{
		if( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN )
		{
			CWnd* pWnd = GetFocus();
			if( pWnd == m_MessageTreeCtrl.GetEditControl() )
				m_MessageTreeCtrl.EndEditLabelNow(FALSE);
			else if( pWnd->GetSafeHwnd() == m_CommandEditCtrl.GetSafeHwnd() )
			{
				CString strCmd;
				m_CommandEditCtrl.GetWindowText(strCmd);
				ExecuteCommand( strCmd );
				m_CommandEditCtrl.SetWindowText( _T("") );
			}

			return TRUE;
		}
		else if( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE )
		{
			CWnd* pWnd = GetFocus();
			if( pWnd == m_MessageTreeCtrl.GetEditControl() )
				m_MessageTreeCtrl.EndEditLabelNow(TRUE);
			else if( pWnd->GetSafeHwnd() == m_CommandEditCtrl.GetSafeHwnd() )
				m_CommandEditCtrl.SetWindowText( _T("") );
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CMessageManagerDlg::OnAppendProperty( UINT nID )
{
	// TODO: 在此添加命令处理程序代码
	HTREEITEM hItem = m_MessageTreeCtrl.GetSelectedItem();

	HTREEITEM hInsert = NULL;
	switch( nID )
	{
	case ID_STRUCT:
		hInsert = m_MessageTreeCtrl.InsertItem( LPSTR_TEXTCALLBACK, 4, 4, hItem?hItem:TVI_ROOT );
		break;
	default:
		hInsert = m_MessageTreeCtrl.InsertItem( LPSTR_TEXTCALLBACK, 5, 5, hItem?hItem:TVI_ROOT );
		break;
	}
	
	Property *pProperty = new Property();
	m_MessageTreeCtrl.SetItemData( hInsert, (DWORD_PTR)pProperty );
	pProperty->m_type = (Property::type)(nID - ID_INT8);
	m_MessageTreeCtrl.SelectItem( hInsert );
	m_MessageTreeCtrl.EditLabel( hInsert );
}

void CMessageManagerDlg::OnTvnGetdispinfoMessageTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	Property *pProperty = (Property*)m_MessageTreeCtrl.GetItemData( pTVDispInfo->item.hItem );
	if( pProperty && pProperty->m_type < Property::MESSAGE )
	{
		_sntprintf( pTVDispInfo->item.pszText, pTVDispInfo->item.cchTextMax, _T("%s:%s"), pProperty->m_name, Property::TypeName[pProperty->m_type] );
	}
	else if( pProperty )
	{
		_sntprintf( pTVDispInfo->item.pszText, pTVDispInfo->item.cchTextMax, _T("[%s]"), pProperty->m_name );
	}

	*pResult = 0;
}

void CMessageManagerDlg::OnTvnDeleteitemMessageTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	Property *pProperty = (Property*)pNMTreeView->itemOld.lParam;
	delete pProperty;
	*pResult = 0;
}

void CMessageManagerDlg::OnTvnSelchangedMessageTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	m_PropertyListCtrl.DeleteAll();
	Property *pProperty = (Property*)pNMTreeView->itemNew.lParam;
	if( pProperty )
	{
		switch( pProperty->m_type )
		{
		case Property::MESSAGE:
			{
				m_PropertyListCtrl.AddPropItem( _T("Message"), _T("消息类型"), _T("0"), PIT_EDIT, _T("0") );
				m_PropertyListCtrl.AddPropItem( _T("Message"), _T("消息编码"), _T("0"), PIT_EDIT, _T("0") );
			}
			break;
		case Property::SUB_DEF:
			break;
		case Property::ARRAY:
			m_PropertyListCtrl.AddPropItem( _T("Array"), _T("数组下标"), _T("1"), PIT_EDIT, _T("1") );
			break;
		case Property::CONTAINER:
			{
				m_PropertyListCtrl.AddPropItem( _T("Container"), _T("嵌套类型"), _T(""), PIT_COMBO, _T("1|2|3") );
			}
			break;
		default:
			break;
		}
	}
	*pResult = 0;
}

int CMessageManagerDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	CRect rc;
	if( !m_PropertyGridCtrl.Create( WS_VISIBLE | WS_CHILD, rc, this, 1 ) )
		return -1;

	m_PropertyGridCtrl.EnableHeaderCtrl(FALSE);
	m_PropertyGridCtrl.SetVSDotNetLook();
	m_PropertyGridCtrl.MarkModifiedProperties();

	//CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("外观"));

	//pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("三维外观"), (_variant_t) false, _T("指定窗口的字体不使用粗体，并且控件将使用三维边框")));

	//CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("边框"), _T("Dialog Frame"), _T("其中之一: 无(None)、细(Thin)、可调整大小(Resizable)、对话框外框(Dialog Frame)"));
	//pProp->AddOption(_T("None"));
	//pProp->AddOption(_T("Thin"));
	//pProp->AddOption(_T("Resizable"));
	//pProp->AddOption(_T("Dialog Frame"));
	//pProp->AllowEdit(FALSE);

	//pGroup1->AddSubItem(pProp);
	//pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("标题"), (_variant_t) _T("关于"), _T("指定窗口标题栏中显示的文本")));

	//m_PropertyGridCtrl.AddProperty(pGroup1);

	//CMFCPropertyGridProperty* pSize = new CMFCPropertyGridProperty(_T("窗口大小"), 0, TRUE);

	//pProp = new CMFCPropertyGridProperty(_T("高度"), (_variant_t) 250l, _T("指定窗口的高度"));
	//pProp->EnableSpinControl(TRUE, 50, 300);
	//pSize->AddSubItem(pProp);

	//pProp = new CMFCPropertyGridProperty( _T("宽度"), (_variant_t) 150l, _T("指定窗口的宽度"));
	//pProp->EnableSpinControl(TRUE, 50, 200);
	//pSize->AddSubItem(pProp);

	//m_PropertyGridCtrl.AddProperty(pSize);

	//CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty(_T("字体"));

	//LOGFONT lf;
	//CFont* font = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	//font->GetLogFont(&lf);

	//lstrcpy(lf.lfFaceName, _T("宋体, Arial"));

	//pGroup2->AddSubItem(new CMFCPropertyGridFontProperty(_T("字体"), lf, CF_EFFECTS | CF_SCREENFONTS, _T("指定窗口的默认字体")));
	//pGroup2->AddSubItem(new CMFCPropertyGridProperty(_T("使用系统字体"), (_variant_t) true, _T("指定窗口使用“MS Shell Dlg”字体")));

	//m_PropertyGridCtrl.AddProperty(pGroup2);

	//CMFCPropertyGridProperty* pGroup3 = new CMFCPropertyGridProperty(_T("杂项"));
	//pProp = new CMFCPropertyGridProperty(_T("(名称)"), _T("应用程序"));
	//pProp->Enable(FALSE);
	//pGroup3->AddSubItem(pProp);

	//CMFCPropertyGridColorProperty* pColorProp = new CMFCPropertyGridColorProperty(_T("窗口颜色"), RGB(210, 192, 254), NULL, _T("指定默认的窗口颜色"));
	//pColorProp->EnableOtherButton(_T("其他..."));
	//pColorProp->EnableAutomaticButton(_T("默认"), ::GetSysColor(COLOR_3DFACE));
	//pGroup3->AddSubItem(pColorProp);

	//static TCHAR BASED_CODE szFilter[] = _T("图标文件(*.ico)|*.ico|所有文件(*.*)|*.*||");
	//pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("图标"), TRUE, _T(""), _T("ico"), 0, szFilter, _T("指定窗口图标")));

	//pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("文件夹"), _T("c:\\")));

	//m_PropertyGridCtrl.AddProperty(pGroup3);

	//CMFCPropertyGridProperty* pGroup4 = new CMFCPropertyGridProperty(_T("层次结构"));

	//CMFCPropertyGridProperty* pGroup41 = new CMFCPropertyGridProperty(_T("第一个子级"));
	//pGroup4->AddSubItem(pGroup41);

	//CMFCPropertyGridProperty* pGroup411 = new CMFCPropertyGridProperty(_T("第二个子级"));
	//pGroup41->AddSubItem(pGroup411);

	//pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("项 1"), (_variant_t) _T("值 1"), _T("此为说明")));
	//pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("项 2"), (_variant_t) _T("值 2"), _T("此为说明")));
	//pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("项 3"), (_variant_t) _T("值 3"), _T("此为说明")));

	//pGroup4->Expand(FALSE);
	//m_PropertyGridCtrl.AddProperty(pGroup4);

	return 0;
}

void CMessageManagerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, __max( cx, 300 ), __max( cy, 300 ) );

	// TODO: 在此处添加消息处理程序代码
	AdjustLayout();
}

#include "stdafx.h"
#include "ListCtrlBar.h"


ListCtrlBar::ListCtrlBar()
{}

ListCtrlBar::~ListCtrlBar()
{}


const int LIST_ID= 1000;

BEGIN_MESSAGE_MAP(ListCtrlBar, CSizingControlBarCF)
	ON_WM_CREATE()
	ON_NOTIFY(LVN_GETDISPINFO, LIST_ID, OnGetDispInfo)
	ON_NOTIFY(LVN_ITEMCHANGED, LIST_ID, OnItemChanged)
END_MESSAGE_MAP()

int ListCtrlBar::OnCreate( LPCREATESTRUCT cs )
{
	if (CSizingControlBarCF::OnCreate(cs) == -1)
		return -1;

	SetSCBStyle(GetSCBStyle() | SCBS_SIZECHILD);

	if (!list_.Create(WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL | LVS_OWNERDATA, CRect(0,0,0,0), this, LIST_ID))
		return -1;

	list_.SetExtendedStyle( LVS_EX_FULLROWSELECT );

	CreateColumns(list_);

	return 0;
}

void ListCtrlBar::OnGetDispInfo(NMHDR* nmhdr, LRESULT* result)
{
	LV_DISPINFO* disp_info = reinterpret_cast<LV_DISPINFO*>(nmhdr);
	static std::string buffer;

	if (disp_info->item.mask & LVIF_TEXT)
	{
		size_t index= disp_info->item.iItem;
		buffer.clear();
		GetDispInfo(index, disp_info->item.iSubItem, buffer);
		disp_info->item.pszText = const_cast<char*>(buffer.c_str());
	}
}


void ListCtrlBar::ResetItems(size_t count)
{
	list_.SetItemCount(static_cast<int>(count));
}

void ListCtrlBar::OnItemChanged(NMHDR* pNmhdr, LRESULT* result)
{
	LPNMLISTVIEW pNmListView = (LPNMLISTVIEW)pNmhdr;
	ItemChanged( pNmListView->iItem );
}
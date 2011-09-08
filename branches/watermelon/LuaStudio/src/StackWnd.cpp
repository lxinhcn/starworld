#include "stdafx.h"
#include "StackWnd.h"
#include "MainFrm.h"

StackWnd::StackWnd()
{

}

StackWnd::~StackWnd()
{
	Clear();
}

void StackWnd::Clear()
{
	CallStackVec::const_iterator c = m_callstack.begin();
	while( c != m_callstack.end() )
	{
		delete *c;
		++c;
	}

	m_callstack.clear();
}

void StackWnd::Notify( buffer* callstack )
{
	Clear();
	while( callstack )
	{
		Params* p = new Params;
		PraseString( callstack->data, *p );
		if( p->size() == 5 )
		{
			m_callstack.push_back( p );
		}
		callstack = callstack->next;
	}
	ResetItems( m_callstack.size() );

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

	buffer* ret = Debug_Command( pFrame->GetDebuger(), "stack 0" );
	pFrame->local_vars_wnd_.Notify( ret );

}

void StackWnd::GetDispInfo(size_t item, int column, std::string& buffer)
{
	if( item < m_callstack.size() )
	{
		buffer = m_callstack[item]->at(column);
	}
}

void StackWnd::CreateColumns(CListCtrl& ctrl)
{
	ctrl.InsertColumn(0, "Index", LVCFMT_LEFT, 80);
	ctrl.InsertColumn(1, "Name", LVCFMT_LEFT, 100);
	ctrl.InsertColumn(2, "What", LVCFMT_LEFT, 60);
	ctrl.InsertColumn(3, "Line", LVCFMT_LEFT, 40);
	ctrl.InsertColumn(4, "File", LVCFMT_LEFT, 200);
}

void StackWnd::ItemChanged( int nItem )
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if( nItem >= 0 && nItem < m_callstack.size() )
	{
		buffer* ret = Debug_Command( pFrame->GetDebuger(), "stack %s", m_callstack[nItem]->at(0).c_str() );
		pFrame->local_vars_wnd_.Notify( ret );
	}
}

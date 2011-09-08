#include "stdafx.h"
#include "LocalVarsWnd.h"


LocalVarsWnd::LocalVarsWnd()
{

}

LocalVarsWnd::~LocalVarsWnd()
{

}

void LocalVarsWnd::Clear()
{
	LocalVec::const_iterator c = m_locals.begin();
	while( c != m_locals.end() )
	{
		delete *c;
		++c;
	}

	m_locals.clear();
}

void LocalVarsWnd::Notify( buffer* local )
{
	Clear();
	while( local )
	{
		Params* p = new Params;
		PraseString( local->data, *p );
		if( p->size() == 3 )
		{
			m_locals.push_back( p );
		}
		local = local->next;
	}
	ResetItems( m_locals.size() );
}

void LocalVarsWnd::GetDispInfo(size_t item, int column, std::string& buffer)
{
	if( item < m_locals.size() )
	{
		buffer = m_locals[item]->at(column);
	}
}

void LocalVarsWnd::CreateColumns(CListCtrl& ctrl)
{
	ctrl.InsertColumn(0, "Idx", LVCFMT_LEFT, 60);
	ctrl.InsertColumn(1, "Name", LVCFMT_LEFT, 100);
	ctrl.InsertColumn(2, "Value", LVCFMT_LEFT, 200);
}

#pragma once
#include "ListCtrlBar.h"

#include "DebugerInterface.h"
class StackWnd : public ListCtrlBar
{
public:
	StackWnd();

	virtual ~StackWnd();

	void Notify( buffer* stackframe );

protected:
	void Clear();

private:
	virtual void GetDispInfo( size_t item, int column, std::string& buffer );
	virtual void CreateColumns( CListCtrl& ctrl );
	virtual void ItemChanged( int nItem );

	typedef std::vector< Params* >	 CallStackVec;
	CallStackVec m_callstack;
};

#pragma once
#include "ListCtrlBar.h"
#include "DebugerInterface.h"

class LocalVarsWnd : public ListCtrlBar
{
public:
	LocalVarsWnd();
	virtual ~LocalVarsWnd();

	void Notify( buffer* local );

private:
	virtual void GetDispInfo(size_t item, int column, std::string& buffer);
	virtual void CreateColumns(CListCtrl& ctrl);

	void Clear();

private:
	typedef std::vector< Params* >	 LocalVec;
	LocalVec m_locals;
};

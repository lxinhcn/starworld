#pragma once
#include "ListCtrlBar.h"

class GlobalVarsWnd : public ListCtrlBar
{
public:
	GlobalVarsWnd();

	virtual ~GlobalVarsWnd();

private:
	virtual void GetDispInfo(size_t item, int column, std::string& buffer);
	virtual void CreateColumns(CListCtrl& ctrl);
};

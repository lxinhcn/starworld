#pragma once
#include "xobjectai.h"

class CPersonAI :	public CXObjectAI
{
public:
	CPersonAI(void);
	~CPersonAI(void);

	virtual BOOL Think( CGameObject* pObj, int nEvent, LPCSTR lpBuf, size_t nSize ) = 0;
};

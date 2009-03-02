#pragma once
#include "GameScene.h"

class CClientMap :	public CMap
{
public:
	CClientMap(void);
	~CClientMap(void);

	BOOL CreateMap( LPCTSTR lpszMapFile );
};

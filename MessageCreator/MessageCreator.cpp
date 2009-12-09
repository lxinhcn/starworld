// MessageCreator.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

void Analyse( _lpctstr filename );

int _tmain(int argc, _TCHAR* argv[])
{
	for( int i = 1; i < argc; ++i )
	{
		Analyse( argv[argc] );
	}
	return 0;
}

static _lpctstr keyword[] = 
{
	_T("char"),
	_T("short"),
	_T("int"),
	_T("long"),
	_T("float"),
	_T("double"),
	_T("unsigned"),
	_T("vector"),
};

void Analyse( _lpctstr filename )
{
	
}

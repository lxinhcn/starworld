// MessageCreator.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <fstream>
void Analyse( _lpctstr filename );
using namespace std;
int _tmain(int argc, _TCHAR* argv[])
{
	fstream file;
	file.open( "1.txt" );
	file.seekp( 1024, iostream::end );
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
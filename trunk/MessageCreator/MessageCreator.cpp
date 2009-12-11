// MessageCreator.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "bufstream.h"
#include <set>
#include <map>
#include <queue>
#include <deque>

void Analyse( _lpctstr filename );
using namespace std;
int _tmain(int argc, _TCHAR* argv[])
{
	bufstream buf( 1024 );
	int n[11] = {1,2,3,4,5,6,7,8,9,0,1};
	buf << 1 << 0.5f << 43l << short(558);
	buf << "1234656";

	buf << bufstream( (char*)n, sizeof(n), bufstream::in, sizeof(n) );
	
	std::vector< int > vec;
	buf << vec;

	std::list<int> listt;
	buf << listt;

	std::deque<int> dequee;
	buf << dequee;

	std::set<int> sett;
	buf << sett;

	std::map<int, int> mapp;
	buf << mapp;


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
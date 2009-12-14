// MessageCreator.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "bufstream.h"
#include "AnalyseFile.h"

using namespace std;
int _tmain(int argc, _TCHAR* argv[])
{
	root r;
	for( int i = 1; i < argc; ++i )
	{
		analysefile( &r, argv[i] );
	}
	return 0;
}
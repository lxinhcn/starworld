#pragma once
#include <list>
#include <map>
using namespace std;
struct param
{
	string	tname;
	string	pname;
	bool	_array;	// 是否数组
	bool	_point; // 是否指针
};

struct node
{
	list< param >	params;
	list< node >	sub;

	int container_num; // 容器个数
};

struct message
{
	node sub;
	string messagename;
	string stype;
	string scode;
};

struct root
{
	char	*buf;
	size_t	size;
	node	*pnode;
	map< string, string >	definemap;	// 预定义表
	map< string, string >	typedefmap;	// 类型定义表
	map< string, int >		mcode;
	list< node >	snode;	// 结构定义
	list< message >	mnode;	// 消息定义
};
struct command;

//--------------------------------------------------------//
//	created:	14:12:2009   16:53
//	filename: 	AnalyseFile
//	author:		Albert.xu
//
//	purpose:	过滤字符
//--------------------------------------------------------//
char* trim( char *buf, size_t size, char *trimstring );

//--------------------------------------------------------//
//	created:	14:12:2009   14:51
//	filename: 	AnalyseFile
//	author:		Albert.xu
//
//	purpose:	分析文件
//--------------------------------------------------------//
int analysefile( root *proot, _lpctstr filename );

//--------------------------------------------------------//
//	created:	14:12:2009   15:01
//	filename: 	AnalyseFile
//	author:		Albert.xu
//
//	purpose:	查找列表中的第一个字符串
//--------------------------------------------------------//
int findkeywork( char **buf, size_t *size, command commands[], int count );

//--------------------------------------------------------//
//	created:	14:12:2009   18:14
//	filename: 	AnalyseFile
//	author:		Albert.xu
//
//	purpose:	构造参数
//--------------------------------------------------------//
size_t makeparam( root *proot, char *buf, size_t size );

//--------------------------------------------------------//
//	created:	14:12:2009   14:51
//	filename: 	AnalyseFile
//	author:		Albert.xu
//
//	purpose:	构造树结构
//--------------------------------------------------------//
size_t maketree( root *proot, char *buf, size_t size );

//--------------------------------------------------------//
//	created:	14:12:2009   17:23
//	filename: 	AnalyseFile
//	author:		Albert.xu
//
//	purpose:	构造消息
//--------------------------------------------------------//
size_t makemessage( root *proot, char *buf, size_t size );
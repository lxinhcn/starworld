#pragma once
#include <list>
#include <stack>
#include <fstream>
#include <strstream>
using namespace std;
struct param
{
	string	tline;
	string	comment;
	string	tname;
	string	pname;
	string	pvalue;
	bool	_immediately;	// 是否立即数
	bool	_array;			// 是否数组
	bool	_point;			// 是否指针
	bool	_basic;			// 是否基本类型
	bool	_container;		// 是否容器
};

struct node
{
	string			name;
	string			spacename;
	list< param* >	params;
	list< node* >	sub;
};

struct message
{
	node sub;
	string stype;
	string scode;
	string filename;
};

struct root
{
	size_t	size;
	char	*buf;
	string	filename;
	string	bindir;		// 库文件输出目录
	string	outdir;		// 包含文件输出目录
	string	libdir;		// 库文件输出路径
	fstream	hfile;		// .h 文件
	fstream	cfile;		// .cpp 文件
	map< string, list< string > >	mcode;		// 消息结构
	stack< node * >		pnode;
	list< node* >		snode;		// 结构定义
	list< message* >	mnode;		// 消息定义
	list< string >		defines;	// 预定义宏
	list< string >		files;
	struct
	{
		bool	_debug;
		bool	_runtime;
		bool	_library;
		string	version;		// 版本
		string	runtime;		// 运行库
		string	linktar;		// 连接目标 dll or lib
	}opt;
};
struct command;

//--------------------------------------------------------//
//	created:	18:12:2009   16:37
//	filename: 	AnalyseFile
//	author:		Albert.xu
//
//	purpose:	销毁语法树
//--------------------------------------------------------//
void destroyall( root *proot );

//--------------------------------------------------------//
//	created:	18:12:2009   16:37
//	filename: 	AnalyseFile
//	author:		Albert.xu
//
//	purpose:	销毁消息
//--------------------------------------------------------//
void destroymessage( message *pmessage );

//--------------------------------------------------------//
//	created:	18:12:2009   16:37
//	filename: 	AnalyseFile
//	author:		Albert.xu
//
//	purpose:	销毁节点
//--------------------------------------------------------//
void destroynode( node *pnode );

//--------------------------------------------------------//
//	created:	18:12:2009   16:37
//	filename: 	AnalyseFile
//	author:		Albert.xu
//
//	purpose:	销毁参数
//--------------------------------------------------------//
void destroyparam( param *pparam );

//--------------------------------------------------------//
//	created:	14:12:2009   14:51
//	filename: 	AnalyseFile
//	author:		Albert.xu
//
//	purpose:	分析文件
//--------------------------------------------------------//
int analysefile( root *proot, _lpcstr filename );

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
//	purpose:	构造消息定义
//--------------------------------------------------------//
size_t makedefine( root *proot, char *buf, size_t size, void* pdata );

//--------------------------------------------------------//
//	created:	14:12:2009   18:14
//	filename: 	AnalyseFile
//	author:		Albert.xu
//
//	purpose:	构造结构定义
//--------------------------------------------------------//
size_t makenode( root *proot, char *buf, size_t size, void* pdata );

//--------------------------------------------------------//
//	created:	14:12:2009   18:14
//	filename: 	AnalyseFile
//	author:		Albert.xu
//
//	purpose:	构造参数
//--------------------------------------------------------//
size_t makeparam( root *proot, char *buf, size_t size, void* pdata );

//--------------------------------------------------------//
//	created:	14:12:2009   18:14
//	filename: 	AnalyseFile
//	author:		Albert.xu
//
//	purpose:	构造修饰词
//--------------------------------------------------------//
size_t makemodifier( root *proot, char *buf, size_t size, void* pdata );

//--------------------------------------------------------//
//	created:	14:12:2009   18:14
//	filename: 	AnalyseFile
//	author:		Albert.xu
//
//	purpose:	构造参数
//--------------------------------------------------------//
size_t makebasic( root *proot, char *buf, size_t size, void* pdata );

//--------------------------------------------------------//
//	created:	14:12:2009   18:14
//	filename: 	AnalyseFile
//	author:		Albert.xu
//
//	purpose:	构造参数
//--------------------------------------------------------//
size_t makecontainer( root *proot, char *buf, size_t size, void* pdata );

//--------------------------------------------------------//
//	created:	14:12:2009   14:51
//	filename: 	AnalyseFile
//	author:		Albert.xu
//
//	purpose:	构造树结构
//--------------------------------------------------------//
size_t maketree( root *proot, char *buf, size_t size, void* pdata );

//--------------------------------------------------------//
//	created:	14:12:2009   17:23
//	filename: 	AnalyseFile
//	author:		Albert.xu
//
//	purpose:	构造消息
//--------------------------------------------------------//
size_t makemessage( root *proot, char *buf, size_t size, void* pdata );

//--------------------------------------------------------//
//	created:	17:12:2009   14:28
//	filename: 	AnalyseFile
//	author:		Albert.xu
//
//	purpose:	写一个消息
//--------------------------------------------------------//
void writemessage( root *proot, message *pmessage );

//--------------------------------------------------------//
//	created:	17:12:2009   14:29
//	filename: 	AnalyseFile
//	author:		Albert.xu
//
//	purpose:	写一个结构
//--------------------------------------------------------//
void writenode_def( root *proot, node *pnode, string pix );
void writenode_imp( root *proot, node *pnode, string pix );

//--------------------------------------------------------//
//	created:	18:12:2009   15:24
//	filename: 	AnalyseFile
//	author:		Albert.xu
//
//	purpose:	生成头文件
//--------------------------------------------------------//
void writeheader( root *proot );

//--------------------------------------------------------//
//	created:	17:12:2009   14:27
//	filename: 	AnalyseFile
//	author:		Albert.xu
//
//	purpose:	将生成的消息树转换成C++文件。
//--------------------------------------------------------//
void writefile( root *proot );
#include "StdAfx.h"
#include "AnalyseFile.h"

struct command
{
	const char *keyword;
	size_t (*pcommand)( root *proot, char *buf, size_t size, void* pdata );
};

static command typelist[] = 
{
	{ "unsigned",	makemodifier },
	{ "const",		makemodifier },
	{ "char",		makebasic },
	{ "wchar_t",	makebasic },
	{ "short",		makebasic },
	{ "int",		makebasic },
	{ "long",		makebasic },
	{ "float",		makebasic },
	{ "double",		makebasic },
	{ "string",		makebasic },
	{ "vector",		makecontainer },
	{ "list",		makecontainer },
	{ "deque",		makecontainer },
	{ "set",		makecontainer },
	{ "map",		makecontainer },
};

static command commands[] = 
{
	//{ "#define", makedefine },
	//{ "typedef", maketype },
	{ "message", makemessage },
	//{ "enum", makeenum },
};

int analysefile( root *proot, _lpctstr filename )
{
	FILE *fp;
	_tfopen_s( &fp, filename, _T("r") );
	if( fp )
	{
		struct _stat teststat;
		_tstat( filename, &teststat );
		size_t size = proot->size = teststat.st_size;
		char *buf = proot->buf = (char*)malloc( size );

		if( fread( buf, size, 1, fp ) != 1 && !feof(fp) )
			return -1;

		while( size )
		{
			int idx = findkeywork( &buf, &size, commands, _countof(commands) );
			if( idx != -1 )
			{
				size_t read = commands[idx].pcommand( proot, buf, size, NULL );
				size -= read;
				buf += read;
			}
			else
			{
				break;
			}
		}
	}
	return 0;
}

//--------------------------------------------------------//
//	created:	14:12:2009   17:29
//	filename: 	AnalyseFile
//	author:		Albert.xu
//
//	purpose:	获取行号
//--------------------------------------------------------//
int countline( char *buf, char* cur )
{
	int n = 1;
	while( buf < cur )
	{
		while( buf < cur && *buf++ != '\n' );
		++n;
	}
	return n;
}

//--------------------------------------------------------//
//	created:	14:12:2009   18:29
//	filename: 	AnalyseFile
//	author:		Albert.xu
//
//	purpose:	匹配中括号
//--------------------------------------------------------//
char* matchclose( char *buf, size_t size, char lch = '{', char rch = '}' )
{
	//if( *buf != lch )
	//	return NULL;

	int n = 1;
	char *cur = buf;
	while( cur < buf + size )
	{
		if( (*cur == lch?++n:*cur == rch?--n:n) == 0 )
		{
			return ++cur;
		}
		++cur;
	}
	return NULL;
}

//--------------------------------------------------------//
//	created:	14:12:2009   15:01
//	filename: 	AnalyseFile
//	author:		Albert.xu
//
//	purpose:	查找列表中的第一个字符串
//--------------------------------------------------------//
int findkeywork( char **buf, size_t *size, command commands[], int count )
{
	while( *size )
	{
		for( int i = 0; i < count; ++i )
		{
			int len = strlen(commands[i].keyword);
			if( strncmp( *buf, commands[i].keyword, len ) == 0 )
			{
				*buf += len;
				return i;
			}
		}
		++*buf;
		--*size;
	}
	return -1;
}

//--------------------------------------------------------//
//	created:	15:12:2009   17:39
//	filename: 	AnalyseFile
//	author:		Albert.xu
//
//	purpose:	是否是列表中的串
//--------------------------------------------------------//
int iskeywork( root *proot, char *buf, size_t size, command commands[], int count )
{
	char *next = NULL;
	char *token = strntok( buf, buf+size, " ;()<>[]{}\t\r\n", &next );
	if( token )
	{
		for( int i = 0; i < count; ++i )
		{
			if( strncmp( token, commands[i].keyword, strlen(commands[i].keyword) ) == 0 )
			{
				return i;
			}
		}
	}


	return -1;
}

//--------------------------------------------------------//
//	created:	14:12:2009   18:14
//	filename: 	AnalyseFile
//	author:		Albert.xu
//
//	purpose:	构造参数
//--------------------------------------------------------//
size_t makeparam( root *proot, char *buf, size_t size, void* pdata )
{
	char seps[] = " <>\t\r\n";

	int ret = iskeywork( proot, buf, size, typelist, _countof(typelist) );
	if( ret != -1 )
	{
		if( pdata == NULL )
		{
			node *pnode = proot->pnode.top();
			param *param_ = new param;
			param_->tline.append( buf, size );
			param_->_array = false;
			param_->_container = false;
			param_->_point = false;

			pnode->params.push_back( param_ );
			pdata = (void*)param_;
		}

		return typelist[ret].pcommand( proot, buf, size, pdata );
	}
	return -1;
}

//--------------------------------------------------------//
//	created:	14:12:2009   18:14
//	filename: 	AnalyseFile
//	author:		Albert.xu
//
//	purpose:	构造修饰词
//--------------------------------------------------------//
size_t makemodifier( root *proot, char *buf, size_t size, void* pdata )
{
	char *next = NULL;
	char *token = strntok( buf, buf+size, " ;()<>[]{}\t\r\n", &next );
	param *param_ = (param*)pdata;
	if( param_ )
	{
		param_->tname.append( token, next+1 );
	}

	size_t read = next - token;
	read += makeparam( proot, next, size - read, pdata );
	return read;
}

//--------------------------------------------------------//
//	created:	14:12:2009   18:14
//	filename: 	AnalyseFile
//	author:		Albert.xu
//
//	purpose:	构造参数
//--------------------------------------------------------//
size_t makebasic( root *proot, char *buf, size_t size, void* pdata )
{
	param *param_ = (param*)pdata;
	vector< string > part;
	int n = 0, nary = 0;

	for( char *cur = buf, *beg = buf; cur != buf+size; ++cur )
	{
		switch( *cur )
		{
		case '[':
			if( cur != beg )
			{
				part.push_back( string( beg, cur - beg ) );
			}
			beg = cur+1;
			break;
		case ']':
			param_->_array = true;
			part.push_back( string( beg, cur - beg ) );
			beg = cur+1;
			++nary;
			break;
		case ' ':
		case ';':
		case '\n':
		case '\r':
		case '\t':
			if( cur != beg ) 
				part.push_back( string( beg, cur - beg ) );
			beg = cur+1;
			break;
		}
	}

	if( part.size() < 2 )
		return -1;

	param_->tname += part[0];
	param_->pname = part[1];
	return size;
}

//--------------------------------------------------------//
//	created:	14:12:2009   18:14
//	filename: 	AnalyseFile
//	author:		Albert.xu
//
//	purpose:	构造参数
//--------------------------------------------------------//
size_t makecontainer( root *proot, char *buf, size_t size, void* pdata )
{
	param *param_ = (param*)pdata;
	vector< string > part;
	int n = 0, ntpl = 0, nary = 0;

	for( char *cur = buf, *beg = buf; cur != buf+size; ++cur )
	{
		switch( *cur )
		{
		case '<':
			if( ntpl == 0 )
			{
				part.push_back( string( beg, cur - beg ) );
				beg = cur+1;
			}
			++ntpl;
			break;
		case '>':
			--ntpl;
			if( ntpl == 0 )
			{
				param_->_container = true;
				part.push_back( string( beg, cur - beg ) );
				beg = cur+1;
			}
			break;
		case '[':
			if( cur != beg )
			{
				part.push_back( string( beg, cur - beg ) );
			}
			beg = cur+1;
			++nary;
			break;
		case ']':
			param_->_array = true;
			part.push_back( string( beg, cur - beg ) );
			beg = cur+1;
			--nary;
			break;
		case ' ':
		case ';':
		case '\n':
		case '\r':
		case '\t':
			if( ntpl == 0 && nary == 0 )
			{
				if( cur != beg ) 
					part.push_back( string( beg, cur - beg ) );
				beg = cur+1;
			}
			break;
		}
	}
	param_->tname += part[0] + "< " + part[1] + " >";
	param_->pname = part[2];
	return size;
}

//--------------------------------------------------------//
//	created:	14:12:2009   14:51
//	filename: 	AnalyseFile
//	author:		Albert.xu
//
//	purpose:	构造树结构
//--------------------------------------------------------//
size_t maketree( root *proot, char *buf, size_t size, void* pdata )
{
	char *bseg = buf, *cur = buf;

	char *match = matchclose( buf, size );
	if( match )
		size = match - buf;
		
	size_t read = 0;
	do
	{
		switch( *cur++ )
		{
		case ';':
			read = makeparam( proot, bseg, cur - bseg, NULL );
			if( read == -1 )
			{
				printf( "format error at line %d\n", countline( proot->buf, bseg ) );
				cur = match;
			}
			bseg = cur;
			break;
		case '/':
			if( *cur == '/' )
			{
				strntok( cur, match, "\n\r", &bseg );
				cur = bseg;
			}
			break;
		case '{':
			{
				node *sub = new node;
				node *pnode = proot->pnode.top();
				pnode->sub.push_back( sub );
				char *next = NULL, seps[] = " ;{()\n\r\t";
				char *token = strntok( bseg, cur, seps, &next );
				char *part[2][2];

				for( int i = 0; i < _countof(part); ++i )
				{
					part[i][0] = token;
					part[i][1] = next;
					token = strntok( NULL, cur, seps, &next );
				}
				sub->name.append( part[1][0], part[1][1] );

				proot->pnode.push( sub );
				cur += maketree( proot, cur, size - ( cur - buf ), NULL );
				bseg = cur;
			}
			break;
		case '}':
			break;
		}
	}while( cur < match );

	proot->pnode.pop();
	return size;
}

size_t makemessage( root *proot, char *buf, size_t size, void* pdata )
{
	message *msg = new message;

	char seps[] = " ()\n\r\t";
	char *part[3][2];
	char *next = NULL, *end = strchr( buf, '{' );
	size_t total = 0;
	if( end )
	{
		int i = 0;
		char *token = strntok( buf, end, seps, &next );
		for( int i = 0; token && i < _countof(part)/2; token = strntok( NULL, end, seps, &next ), ++i )
		{
			part[i][0] = token;
			part[i][1] = next;
		}

		if( i == _countof(part) )
		{
			msg->sub.name.append( part[0][0], part[0][1] );
			msg->stype.append( part[1][0], part[1][1] );
			msg->scode.append( part[2][0], part[2][1] );
			if( proot->mcode.find( msg->stype ) == proot->mcode.end() )
				proot->mcode[msg->stype] = 0;
		}
		else
		{
			printf( "syntax error at %d", countline( proot->buf, part[i][0] ) );
			return false;
		}
		proot->pnode.push( &msg->sub );
		size_t read = maketree( proot, end+1, size - ( end + 1 - buf ), NULL );
		total += end - buf;
		total += read;
	}
	proot->mnode.push_back( msg );
	return total;
}
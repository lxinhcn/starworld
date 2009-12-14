#include "StdAfx.h"
#include "AnalyseFile.h"

struct command
{
	const char *keyword;
	size_t (*pcommand)( root *proot, char *buf, size_t size );
};

static command typelist[] = 
{
	{ "char",		makeparam },
	{ "wchar_t",	makeparam },
	{ "short",		makeparam },
	{ "int",		makeparam },
	{ "long",		makeparam },
	{ "float",		makeparam },
	{ "double",		makeparam },
	{ "vector",		makeparam },
	{ "list",		makeparam },
	{ "deque",		makeparam },
	{ "set",		makeparam },
	{ "map",		makeparam },
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

		int idx = findkeywork( &buf, &size, commands, _countof(commands) );
		if( idx != -1 )
		{
			size_t read = commands[idx].pcommand( proot, buf, size );
			size -= read;
			buf += read;
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
char* matchclose( char *buf, size_t size )
{
	if( *buf != '{' )
		return NULL;

	int n = 1;
	char *cur = buf;
	while( cur < buf + size )
	{
		if( (*cur == '{'?++n:*cur == '}'?--n:n) == 0 )
			return cur;
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
//	created:	14:12:2009   18:14
//	filename: 	AnalyseFile
//	author:		Albert.xu
//
//	purpose:	构造参数
//--------------------------------------------------------//
size_t makeparam( root *proot, char *buf, size_t size )
{
	return 0;
}

//--------------------------------------------------------//
//	created:	14:12:2009   14:51
//	filename: 	AnalyseFile
//	author:		Albert.xu
//
//	purpose:	构造树结构
//--------------------------------------------------------//
size_t maketree( root *proot, char *buf, size_t size )
{
	char *match = matchclose( buf, size );
	if( match == NULL )
	{
		printf( "'}' not match at %d", countline( proot->buf, buf ) );
	}

	char *bseg = buf+1;
	do
	{
		char *eseg = strchr( bseg, ';' );
		if( eseg && eseg < match )
		{
			size_t sseg( eseg - bseg );
			int idx = findkeywork( &bseg, &sseg, typelist, _countof(typelist) );
			if( idx != -1 )
			{
				typelist[idx].pcommand( proot, bseg, eseg - bseg );
			}
			else
			{
				printf( "warning unknowe type on %d", countline( proot->buf, bseg ) );
			}
			bseg = eseg + 1;
		}
	}while( bseg < match );

	return match - buf + 1;
}

size_t makemessage( root *proot, char *buf, size_t size )
{
	message msg;

	char seps[] = " ()\n\r";
	char *part[2];
	char *end = strchr( buf, '{' );
	size_t total = 0;
	if( end )
	{
		int i = 0;
		char *token = strtok( buf, seps );
		while( token && token < end && size_t(end - buf) < size )
		{
			part[i++] = token;
			token = strtok( NULL, seps );
		}

		if( i == _countof(part) )
		{
			msg.messagename = part[0];
			msg.stype = part[1];
			if( proot->mcode.find( msg.stype ) == proot->mcode.end() )
				proot->mcode[msg.stype] = 0;
			char szcode[256];
			if( _snprintf( szcode, _countof(szcode), "0x%02x", proot->mcode[msg.stype]++ ) != -1 )
				msg.scode = szcode;
			else
			{
				printf( "error on message %s", part[0] );
				return false;
			}
		}
		else
		{
			printf( "syntax error at %d", countline( proot->buf, part[i] ) );
			return false;
		}
		proot->pnode = &msg.sub;
		total += end - buf;
		total += maketree( proot, end, size - ( end - buf ) );
	}
	proot->mnode.push_back( msg );
	return total;
}
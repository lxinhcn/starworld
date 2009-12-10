#include "StdAfx.h"
//#include "Analyse.h"
//#include "resource.h"
//#include <map>
//
//typedef std::list< TString >				CMsgIDList;
//typedef std::map< TString, CMsgIDList >		CMsgMap;
//typedef std::map< TString, TString >		CMsgValueMap;
//
//CMsgMap			g_msg_map;
//CMsgValueMap	msg_value_map;
//
//template< > const char*		Type2Name< class type >::name = "unknowe";
//template< > const wchar_t*	Type2Name< class type >::wname = L"unknowe";
//template< > const TCHAR*	Type2Name< class type >::tname = _T("unknowe");
//const char*		Type2Name< const char* >::name 	= "LPCSTR";
//const wchar_t*	Type2Name< const char* >::wname	= L"LPCSTR";
//const TCHAR*	Type2Name< const char* >::tname = _T("LPCSTR");
//const char*		Type2Name< const wchar_t* >::name 	= "LPCWSTR";
//const wchar_t*	Type2Name< const wchar_t* >::wname	= L"LPCWSTR";
//const TCHAR*	Type2Name< const wchar_t* >::tname = _T("LPCWSTR");
//
//bool get_params( LPCTSTR lpszCommand, CParams& params, TCHAR split/* = _T(' ')*/ )
//{
//	ASSERT( lpszCommand );
//	if( lpszCommand == NULL ) return false;
//	CONST TCHAR* begin	= lpszCommand;
//	while( *begin == split ) ++begin;
//	while( begin )
//	{
//		CONST TCHAR* end = _tcschr( begin, split );
//		if( end )
//		{
//			TString str;
//			str.append( begin, end - begin );
//			params.push_back(str);
//			while( *(++end) == split );
//		}
//		else if( *begin != 0 )
//		{
//			params.push_back( begin );
//		}
//		begin = end;
//	}
//	return true;
//}
//
//typedef bool (*pfnWrite)( CParams& params, LPCTSTR in_name, FILE* fp, LPCTSTR prefix );
//typedef bool (*pfnRead)( CParams& params, LPCTSTR out_name, FILE* fp, LPCTSTR prefix );
//struct type_command
//{
//	LPCTSTR		lpType;
//	pfnWrite	write;
//	pfnRead		read;
//};
//
//type_command cmd_table[] = 
//{
//	{ _T("_string"), write_TString, read_TString,	},
//	{ _T("std::string"),  write_String< LPCSTR >, read_String< LPCSTR >,	},
//	{ _T("std::wstring"), write_String< LPCWSTR >, read_String< LPCWSTR >,	},
//};
//
//// 分析消息定义文件
//bool Analyse( LPCTSTR lpszSrcName, LPCTSTR lpszDestName, HWND hProgressWnd )
//{
//	FILE* fp;
//	_tfopen_s( &fp, lpszSrcName, _T("r") );
//	if( fp == NULL )	return false;
//
//	TCHAR szFileName[256];
//	FILE* fp_header;
//	FILE* fp_cpp;
//
//	fseek( fp, 0, SEEK_END );
//	size_t file_size = ftell( fp );
//	fseek( fp, 0, SEEK_SET );
//
//	::SendMessage( hProgressWnd, WM_USER + 0x4021, 0, MAKELONG( 0, 1000 ) );
//	// 打开.h文件
//	_sntprintf_s( szFileName, countof( szFileName ), countof( szFileName ), _T("%s.h"), lpszDestName );
//	_tfopen_s( &fp_header, szFileName, _T("w+") );
//	if( fp_header == NULL )	return false;
//
//	// 写.h文件的文件头
//	_ftprintf_s( fp_header, _T("#ifndef %s_MESSAGE_HEADER\n"), lpszDestName );
//	_ftprintf_s( fp_header, _T("#define %s_MESSAGE_HEADER\n"), lpszDestName );
//	_ftprintf_s( fp_header, _T("#include \"MessageHead.h\"\n") );
//	_ftprintf_s( fp_header, _T("#include \"netstream.h\"\n") );
//
//
//	//打开.cpp文件
//	_sntprintf_s( szFileName, countof( szFileName ), countof( szFileName ), _T("%s.cpp"), lpszDestName );
//	_tfopen_s( &fp_cpp, szFileName, _T("w") );
//	if( fp_header == NULL )	return false;
//
//	// 写.cpp文件的文件头
//	//_ftprintf_s( fp_cpp, _T("#include \"stdafx.h\"\n") );
//	_ftprintf_s( fp_cpp, _T("#include \"%s.h\"\n"), lpszDestName );
//
//	MessageDefine	msgbody;
//	TCHAR			szLine[1024];
//	int				deep = 0;
//	int				line = 0;
//	// 开始分析结构
//	while( !feof( fp ) )
//	{
//		_fgetts( szLine, sizeof( szLine ), fp );
//		size_t cur_pos = ftell( fp );
//		::SendMessage( hProgressWnd, WM_USER + 0x4021, 1, cur_pos*1000/file_size );
//		++line;
//		LPTSTR p = szLine;
//		if( _tcsncmp( p, _T("//"), 2 ) == 0 )
//		{
//			msgbody.strComment.append( szLine );
//			continue;
//		}
//		while( ( *p = (*p == _T('\t'))?_T(' '):(*p == _T('\n'))?_T(' '):*p )!= 0 )
//		{
//			if( *p == _T(';') ) *p = _T(' ');
//			if( *p == _T('{') )
//			{
//				if( deep == 0 ) *p = _T(' ');
//				++deep;
//			}
//			else if( *p == _T('}') )
//			{
//				--deep;
//				if( deep == 0 )
//				{
//					::SendMessage( hProgressWnd, WM_USER + 0x4022, IDS_CREATE_CPP, (LPARAM)msgbody.strName.c_str() );
//					AnalyseBody( msgbody, fp_header, fp_cpp, hProgressWnd );
//					msgbody.strComment.clear();
//				}
//			}
//			++p;
//		}
//		CParams params;
//		get_params( szLine, params );
//		if( params.empty() ) continue;
//		if( params[0] == _T("#define") )
//		{
//			CMsgValueMap::iterator iter = msg_value_map.find( params[1] );
//
//			if( iter != msg_value_map.end() )
//			{
//				TCHAR szLog[1024];
//				_sntprintf_s( szLog, countof(szLog), countof(szLog),_T("发现重复定义的宏。%s [%d]"), params[0].c_str(), line );
//				AfxMessageBox( szLog );
//				continue;
//			}
//			msg_value_map[params[1]] = params[2];
//			continue;
//		}
//
//		if( params[0] == _T("struct") && deep == 0 )
//		{
//			msgbody.strName = params[1];
//			msgbody.Body.clear();
//		}
//		else if( deep > 0 )
//		{
//			msgbody.Body.push_back( params );
//		}
//	}
//	_ftprintf_s( fp_header, _T("#endif %s_MESSAGE_HEADER\n"), lpszDestName );
//
//	fclose( fp );
//	fclose( fp_header );
//	fclose( fp_cpp );
//	return true;
//}
//
//// 分析完成的结构
//bool AnalyseBody( MessageDefine& msg, FILE* fp_header, FILE* fp_cpp, HWND hProgressWnd )
//{
//	_ftprintf( fp_header, _T("%s"), msg.strComment.c_str() );
//	_ftprintf( fp_header, _T("struct %s\t: public MessageHeader\n{\n"), msg.strName.c_str() );
//	_fputts( _T("\tvirtual size_t read( const char* data, size_t size );\n"), fp_header );
//	_fputts( _T("\tvirtual size_t write( char* data, size_t size ) const;\n"), fp_header );
//	CParamsList::iterator iter = msg.Body.begin();
//	int deep = 1;
//	typedef std::map< TString, cmd_for_each >	cmd_map;
//	cmd_map	struct_for_each_map;
//	enum struct_status{ en_none, en_struct, en_enum };
//	std::stack< struct_status > status_stack;
//	struct_status cur_status = en_struct;
//	status_stack.push( en_struct );
//	//////////////////////////////////////////////////////////////////////////
//	// 写头文件
//	while( iter != msg.Body.end() )
//	{
//		CParams& params = *iter;
//		++iter;
//		LPCTSTR p = params[0].c_str();
//		for( int i = 0; i < deep - 1; ++i ) _ftprintf( fp_header, _T("\t") );
//		// 注释则全部打印
//		if( p[0] == _T('/') && p[1] == _T('/') )
//		{
//			_fputts( _T("\t"), fp_header );
//			for( size_t i = 0; i < params.size(); ++i )	_ftprintf( fp_header, _T("%s "), params[i].c_str() );
//			_fputts( _T("\n"), fp_header );
//
//			if( params.size() <= 1 ) continue;
//			// 将脚本命令提取出来。
//			if( params[1] == _T("for_each") )
//			{
//				ASSERT( params.size() == 4 );
//				if( params.size() != 4 )
//				{
//					// log here
//					continue;
//				}
//				cmd_for_each cmd;
//				cmd.struct_name = params[2];
//				cmd.count_name	= params[2] + _T("_count");
//				cmd.variant_name= params[3];
//				struct_for_each_map[params[2]] = cmd;
//				_ftprintf( fp_header, _T("\tstd::vector< %s > %s;\n"), cmd.struct_name.c_str(), cmd.variant_name.c_str() );
//			}
//			else if( params[1] == _T("msgtype") )
//			{
//				msg.msgtype = params[2];
//				_ftprintf( fp_header, _T("\tenum { type = %s };\n"), msg.msgtype.c_str() );
//			}
//			else if( params[1] == _T("msgid") )
//			{
//				msg.msgid = params[2];
//				_ftprintf( fp_header, _T("\tenum { id = %s };\n"), msg.msgid.c_str() );
//			}
//			continue;
//		}
//
//		while( *p != 0 )
//		{
//			if( *p == _T('{') )
//			{
//				++deep;
//				_fputts( _T("\t{\n"), fp_header );
//				status_stack.push( cur_status );
//			}
//			if( *p == _T('}') )
//			{
//				--deep;
//				_fputts( _T("};\n"), fp_header );
//				if( status_stack.size() == 0 )
//				{
//					AfxMessageBox( _T("括号不匹配.") );
//				}
//				else
//					status_stack.pop();
//				if( status_stack.size() == 0 ) cur_status = en_none;
//			}
//			++p;
//		}
//
//		if( params[0] == _T("struct") )
//		{
//			_ftprintf( fp_header, _T("\t%s\t\t%s\n"), params[0].c_str(), params[1].c_str() );
//			cur_status = en_struct;
//		}
//		else if( params[0] == _T("enum") )
//		{
//			_ftprintf( fp_header, _T("\t%s\n"), params[0].c_str() );
//			cur_status = en_enum;
//		}
//		else if( status_stack.size() )
//		{
//			switch( status_stack.top() )
//			{
//			case en_struct:
//				if( params.size() < 2 )	continue;
//				_ftprintf( fp_header, _T("\t%s\t\t%s;\n"), params[0].c_str(), params[1].c_str() );
//				break;
//			case en_enum:
//				if( params.size() < 1 || params[0] == _T("{") || params[0] == _T("}") ) continue;
//				_ftprintf( fp_header, _T("\t") );
//				for( size_t i = 0; i < params.size(); ++i )
//				{
//					_ftprintf( fp_header, _T("%s\t"), params[i].c_str() );
//				}
//				_ftprintf( fp_header, _T("\n") );
//			}
//		}
//	}
//	_fputts( _T("};\n\n"), fp_header );
//
//	//////////////////////////////////////////////////////////////////////////
//	// 写CPP文件 write
//	_ftprintf( fp_cpp, _T("//////////////////////////////////////////////////////////////////////////\n") );
//	_ftprintf( fp_cpp, _T("// %s write\n"), msg.strName.c_str() );
//
//	_ftprintf( fp_cpp, _T("size_t %s::write( char* data, size_t size ) const\n"), msg.strName.c_str() );
//	_ftprintf( fp_cpp, 
//		_T("{\n")
//		_T("\ttry\n")
//		_T("\t{\n")
//		_T("\t\tCNetInStream in( (char*)data, size );\n")
//		_T("\t\tin.write< _byte >(%s);\n")
//		_T("\t\tin.write< _byte >(%s);\n")
//		, msg.msgtype.c_str()
//		, msg.msgid.c_str()
//		);
//	deep = 1;
//	iter = msg.Body.begin();
//	typedef std::list< TString >	CStructWriteStack;
//	CStructWriteStack	struct_write_stack;
//
//	while( iter != msg.Body.end() )
//	{
//		CParams& params = *iter;
//		++iter;
//		
//		LPCTSTR p = params[0].c_str();
//		// 遇到结构则认为该结构需要在消息内连续存储多个。
//		if( params[0] == _T("struct") )
//		{
//			struct_write_stack.push_back( params[1] );
//			continue;
//		}
//
//		for( int i = 0; i < deep; ++i ) _ftprintf( fp_cpp, _T("\t") );
//		// 注释则全部打印
//		if( p[0] == _T('/') && p[1] == _T('/') )
//		{
//			_fputts( _T("\t"), fp_cpp );
//			for( size_t i = 0; i < params.size(); ++i )	_ftprintf( fp_cpp, _T("%s "), params[i].c_str() );
//			_fputts( _T("\n"), fp_cpp );
//			continue;
//		}
//
//		// 搜索 {} 匹配
//		while( *p != 0 )
//		{
//			if( *p == _T('{') )
//			{
//				if( struct_write_stack.size() )
//				{
//					TString& struct_name = struct_write_stack.back();
//					cmd_map::iterator iter = struct_for_each_map.find( struct_name );
//					if( iter != struct_for_each_map.end() )
//					{
//						_ftprintf( fp_cpp, 
//							_T("\tstruct _for_each_%s\n") 
//							, struct_name.c_str() 
//							);
//						for( int i = 0; i < deep; ++i ) _ftprintf( fp_cpp, _T("\t") );
//						_ftprintf( fp_cpp, 
//							_T("\t{\n") 
//							);
//						for( int i = 0; i < deep; ++i ) _ftprintf( fp_cpp, _T("\t") );
//						_ftprintf( fp_cpp, 
//							_T("\t\tCNetInStream& in;\n")
//							);
//						for( int i = 0; i < deep; ++i ) _ftprintf( fp_cpp, _T("\t") );
//						_ftprintf( fp_cpp, 
//							_T("\t\t_for_each_%s( CNetInStream& _in ): in( _in ){}\n") 
//							, struct_name.c_str()
//							);
//						for( int i = 0; i < deep; ++i ) _ftprintf( fp_cpp, _T("\t") );
//						// 变量定义使用全类型路径
//						CStructWriteStack::iterator iter = struct_write_stack.begin();
//						TString struct_all_name = *iter++;
//						while( struct_write_stack.end() != iter )
//						{
//							struct_all_name += _T("::") + *iter;
//							++iter;
//						}
//						_ftprintf( fp_cpp, 
//							_T("\t\tvoid operator()( const %s& iter )\n") 
//							, struct_all_name.c_str()
//							);
//						for( int i = 0; i < deep; ++i ) _ftprintf( fp_cpp, _T("\t") );
//						_ftprintf( fp_cpp, 
//							_T("\t\t{\n")
//							);
//					}
//					++deep;
//				}
//				++deep;
//			}
//			if( *p == _T('}') )
//			{
//				--deep;
//				if( struct_write_stack.size() )
//				{
//					--deep;
//					TString& struct_name = struct_write_stack.back();
//					cmd_map::iterator iter = struct_for_each_map.find( struct_name );
//					if( iter != struct_for_each_map.end() )
//					{
//						_ftprintf( fp_cpp, _T("}\n") );
//						for( int i = 0; i < deep; ++i ) _ftprintf( fp_cpp, _T("\t") );
//						_ftprintf( fp_cpp, _T("\t};\n") );
//						for( int i = 0; i < deep; ++i ) _ftprintf( fp_cpp, _T("\t") );
//						_ftprintf( fp_cpp,
//							_T("\t_for_each_%s op_%s(in);\n")
//							, struct_name.c_str(), struct_name.c_str()
//							);
//						// 对于operator()() 内的变量引用，应该使用iter作为前导。
//						if( struct_write_stack.size() > 1 )
//						{
//							for( int i = 0; i < deep; ++i ) _ftprintf( fp_cpp, _T("\t") );
//							_ftprintf( fp_cpp, _T("\tin.write< _uint16 >( (_uint16)iter.%s.size() );\n"), iter->second.variant_name.c_str() );
//							for( int i = 0; i < deep; ++i ) _ftprintf( fp_cpp, _T("\t") );
//							_ftprintf( fp_cpp,
//								_T("\tstd::for_each( iter.%s.begin(), iter.%s.end(), op_%s );\n")
//								, iter->second.variant_name.c_str()
//								, iter->second.variant_name.c_str()
//								, struct_name.c_str()
//								);
//						}
//						else
//						{
//							for( int i = 0; i < deep; ++i ) _ftprintf( fp_cpp, _T("\t") );
//							_ftprintf( fp_cpp, _T("\tin.write< _uint16 >( (_uint16)%s.size() );\n"), iter->second.variant_name.c_str() );
//							for( int i = 0; i < deep; ++i ) _ftprintf( fp_cpp, _T("\t") );
//							_ftprintf( fp_cpp,
//								_T("\tstd::for_each( %s.begin(), %s.end(), op_%s );\n")
//								, iter->second.variant_name.c_str()
//								, iter->second.variant_name.c_str()
//								, struct_name.c_str()
//								);
//						}
//					}
//					struct_write_stack.pop_back();
//				}
//			}
//			++p;
//		}
//
//		if( struct_write_stack.size() && params.size() > 1 )
//		{
//			//_ftprintf( fp_cpp, _T("\t") );
//			write_type( params, _T("in"), fp_cpp, _T("iter.") );
//		}
//		else if( params.size() > 1 )
//		{
//			write_type( params, _T("in"), fp_cpp );
//		}
//	}
//
//	_fputts( 
//		_T("\t\tin.end();\n")
//		_T("\t\treturn in.length();\n")
//		_T("\t}\n")
//		_T("\tcatch( LPCTSTR error )\n")
//		_T("\t{\n")
//		_T("\t\tCLogger::GetInstance( MESSAGE_LOG )->WriteLog( _T(\"%s %s\"), error, __FUNCTION__ );\n")
//		_T("\t\treturn -1;\n")
//		_T("\t}\n")
//		, fp_cpp );
//
//	_ftprintf( fp_cpp, _T("}\n\n") );
//
//	//////////////////////////////////////////////////////////////////////////
//	// 写CPP文件 read
//	_ftprintf( fp_cpp, _T("//////////////////////////////////////////////////////////////////////////\n") );
//	_ftprintf( fp_cpp, _T("// %s read\n"), msg.strName.c_str() );
//
//	_ftprintf( fp_cpp, _T("size_t %s::read( const char* data, size_t size )\n"), msg.strName.c_str() );
//	_ftprintf( fp_cpp, 
//		_T("{\n")
//		_T("\ttry\n")
//		_T("\t{\n")
//		_T("\t\tCNetOutStream out( (char*)data, size );\n")
//		);
//	iter = msg.Body.begin();
//	typedef std::list< TString >	CStructReadStack;
//	CStructReadStack struct_read_stack;
//	deep = 1;
//	while( iter != msg.Body.end() )
//	{
//		CParams& params = *iter;
//		++iter;
//
//		LPCTSTR p = params[0].c_str();
//		// 遇到结构则认为该结构需要在消息内连续存储多个。
//		if( params[0] == _T("struct") )
//		{
//			struct_read_stack.push_back( params[1] );
//			continue;
//		}
//		for( int i = 0; i < deep; ++i ) _ftprintf( fp_cpp, _T("\t") );
//		// 注释则全部打印
//		if( p[0] == _T('/') && p[1] == _T('/') )
//		{
//			_fputts( _T("\t"), fp_cpp );
//			for( size_t i = 0; i < params.size(); ++i )	_ftprintf( fp_cpp, _T("%s "), params[i].c_str() );
//			_fputts( _T("\n"), fp_cpp );
//			continue;
//		}
//
//		// 搜索 {} 匹配
//		while( *p != 0 )
//		{
//			if( *p == _T('{') )
//			{
//				if( struct_read_stack.size() )
//				{
//					TString& struct_name = struct_read_stack.back();
//					cmd_map::iterator iter = struct_for_each_map.find( struct_name );
//					if( iter != struct_for_each_map.end() )
//					{
//						// 如果循环发生嵌套，则变量名需要使用上一层嵌套中的临时变量名作为前缀。
//						TString strTmpName;
//						if( struct_read_stack.size() > 1 )
//						{
//							CStructReadStack::reverse_iterator parent_name_iter = struct_read_stack.rbegin();
//							++parent_name_iter;
//							TString& parent_struct_name = *parent_name_iter;
//							strTmpName += _T("tmp_") + parent_struct_name + _T(".");
//						}
//
//						_ftprintf( fp_cpp, _T("\tsize_t %s%s = out.readtype( _uint16 );\n"), strTmpName.c_str(), iter->second.count_name.c_str() );
//						for( int i = 0; i < deep; ++i ) _ftprintf( fp_cpp, _T("\t") );
//						_ftprintf( fp_cpp, 
//							_T("\t%s%s.resize( %s%s );\n")
//							, strTmpName.c_str()
//							, iter->second.variant_name.c_str()
//							, strTmpName.c_str()
//							, iter->second.count_name.c_str()
//							);
//						for( int i = 0; i < deep; ++i ) _ftprintf( fp_cpp, _T("\t") );
//						_ftprintf( fp_cpp, 
//							_T("\tfor(unsigned int i = 0; i < (%s%s); ++i )\n")
//							, strTmpName.c_str()
//							, iter->second.count_name.c_str()
//							);
//						for( int i = 0; i < deep; ++i ) _ftprintf( fp_cpp, _T("\t") );
//						_ftprintf( fp_cpp, 
//							_T("\t{\n")
//							);
//						for( int i = 0; i < deep; ++i ) _ftprintf( fp_cpp, _T("\t") );
//						// 变量定义使用全类型路径
//						CStructReadStack::iterator struct_name_iter = struct_read_stack.begin();
//						TString struct_all_name = *struct_name_iter++;
//						while( struct_read_stack.end() != struct_name_iter )
//						{
//							struct_all_name += _T("::") + *struct_name_iter;
//							++struct_name_iter;
//						}
//						_ftprintf( fp_cpp, 
//							_T("\t\t%s& tmp_%s = %s%s.at(i);\n")
//							, struct_all_name.c_str()
//							, struct_name.c_str()
//							, strTmpName.c_str()
//							, iter->second.variant_name.c_str()
//							);
//					}
//				}
//				++deep;
//			}
//			if( *p == _T('}') )
//			{
//				--deep;
//				if( struct_read_stack.size() )
//				{
//					TString& struct_name = struct_read_stack.back();
//					cmd_map::iterator iter = struct_for_each_map.find( struct_name );
//					if( iter != struct_for_each_map.end() )
//					{
//						_ftprintf( fp_cpp, 
//							_T("}\n")
//							);
//					}
//					struct_read_stack.pop_back();
//				}
//			}
//			++p;
//		}
//
//		if( struct_read_stack.size() && params.size() > 1 )
//		{
//			TString strTmpName(_T("tmp_") );
//			strTmpName += struct_read_stack.back() + _T(".");
//			read_type( params, _T("out"), fp_cpp, strTmpName.c_str() );
//		}
//		else if( params.size() > 1 )
//		{
//			read_type( params, _T("out"), fp_cpp );
//		}
//	}
//
//	_fputts( 
//		_T("\t\treturn out.position();\n")
//		_T("\t}\n")
//		_T("\tcatch( LPCTSTR error )\n")
//		_T("\t{\n")
//		_T("\t\tCLogger::GetInstance( MESSAGE_LOG )->WriteLog( _T(\"%s %s\"), error, __FUNCTION__ );\n")
//		_T("\t\treturn -1;\n")
//		_T("\t}\n")
//		, fp_cpp );
//	_ftprintf( fp_cpp, _T("}\n\n") );
//
//	if( g_msg_map.find( msg.msgtype ) != g_msg_map.end() )
//	{
//		CMsgIDList& msg_list = g_msg_map[msg.msgtype];
//		if( std::find( msg_list.begin(), msg_list.end(), msg.msgid ) == msg_list.end() )
//		{
//			g_msg_map[msg.msgtype].push_back( msg.msgid );
//		}
//		//else
//		//{
//		//	AfxMessageBox( _T("Found same id.") );
//		//}
//	}
//	else
//	{
//		g_msg_map[msg.msgtype].push_back( msg.msgid );
//	}
//
//	return true;
//}
//
//bool is_array( TString& param, TString& array_size, TString& variant_name )
//{
//	size_t pos = param.find( _T('[') );
//	if( pos == TString::npos ) return false;
//	size_t end = param.find( _T(']'), pos + 1 );
//	
//	array_size.append( param, pos+1, end - pos - 1 );
//	variant_name.append( param, 0, pos );
//	return true;
//}
//
//bool write_type( CParams& params, LPCTSTR in_name, FILE* fp, LPCTSTR prefix )
//{
//	for( size_t i = 0; i < countof( cmd_table ); ++i )
//	{
//		if( params[0] == cmd_table[i].lpType )
//		{
//			return cmd_table[i].write( params, in_name, fp, prefix );
//		}
//	}
//
//	if( prefix == NULL ) return false;
//	TString array_size;
//	TString variant_name;
//	if( is_array( params[1], array_size, variant_name ) )
//	{
//		_ftprintf( fp, _T("\t%s.write( (char*)%s%s, sizeof(%s%s) );\n"), in_name, prefix, variant_name.c_str(), prefix, variant_name.c_str() );
//	}
//	else
//	{
//		_ftprintf( fp, _T("\t%s.write( %s%s );\n"), in_name, prefix, params[1].c_str() );
//	}
//	return true;
//}
//
//bool read_type( CParams& params, LPCTSTR out_name, FILE* fp, LPCTSTR prefix )
//{
//	for( size_t i = 0; i < countof( cmd_table ); ++i )
//	{
//		if( params[0] == cmd_table[i].lpType )
//		{
//			return cmd_table[i].read( params, out_name, fp, prefix );
//		}
//	}
//
//	TString array_size;
//	TString variant_name;
//	if( is_array( params[1], array_size, variant_name ) )
//	{
//		_ftprintf( fp, _T("\t%s.read( (char*)%s%s, sizeof(%s%s) );\n"), out_name, prefix, variant_name.c_str(), prefix, variant_name.c_str() );
//	}
//	else
//	{
//		_ftprintf( fp, _T("\t%s.read( %s%s );\n"), out_name, prefix, params[1].c_str() );
//	}
//	return true;
//}
//
//bool write_TString( CParams& params, LPCTSTR in_name, FILE* fp, LPCTSTR prefix )
//{
//	if( prefix == NULL ) return false;
//	TString array_size;
//	TString variant_name;
//	if( is_array( params[1], array_size, variant_name ) )
//	{
//		_ftprintf( fp,
//			_T("\tfor( int i = 0; i < %s; ++i ){%s.write( %s%s[i].c_str() );}\n"),
//			array_size.c_str(),
//			in_name,
//			prefix,
//			variant_name.c_str()
//			);
//	}
//	else
//	{
//		_ftprintf( fp, _T("\t%s.write( %s%s.c_str() );\n"), in_name, prefix, params[1].c_str() );
//	}
//	return true;
//}
//
//bool read_TString( CParams& params, LPCTSTR out_name, FILE* fp, LPCTSTR prefix )
//{
//	TString array_size;
//	TString variant_name;
//	if( is_array( params[1], array_size, variant_name ) )
//	{
//		_ftprintf( fp,
//			_T("\tfor( int i = 0; i < %s; ++i ){LPCTSTR p;\t%s.read( p );\t%s%s[i] = p;}\n"),
//			array_size.c_str(),
//			out_name,
//			prefix,
//			variant_name.c_str()
//			);
//	}
//	else
//	{
//		_ftprintf( fp, 
//			_T("\t{LPCTSTR p = NULL;%s.read( p );%s%s = p;}\n"),
//			out_name,
//			prefix,
//			params[1].c_str()
//			);
//	}
//	return true;
//}
//bool WriteMessageDefine( LPCTSTR lpszDefFilename, LPCTSTR lpszExt )
//{
//	CString strFilename( lpszDefFilename );
//	strFilename += lpszExt;
//	FILE* fp_header = _tfopen( strFilename, _T("w") );
//	if( !fp_header )	return false;
//
//	// 写.h文件中的消息类型枚举定义 
//	_ftprintf_s( fp_header, _T("#ifndef %s_H\n#define %s_H\n"), lpszDefFilename, lpszDefFilename );
//	_ftprintf_s( fp_header, _T("enum MSG_TYPE\n{\n") );
//
//	CMsgMap::iterator iter = g_msg_map.begin();
//	while( g_msg_map.end() != iter )
//	{
//		// 已经定义过的则不进行自动定义
//		CMsgValueMap::iterator valIter = msg_value_map.find(iter->first);
//		if( valIter != msg_value_map.end() )
//		{
//			_ftprintf_s( fp_header, _T("\t%s = %s,\n"), iter->first.c_str(), valIter->second.c_str() );
//			TRACE(_T("\t%s = %s,\n"), iter->first.c_str(), valIter->second.c_str());
//		}
//		else
//		{
//			_ftprintf_s( fp_header, _T("\t%s,\n"), iter->first.c_str() );
//			TRACE(_T("\t%s,\n"), iter->first.c_str());
//		}
//		++iter;
//	}
//	_ftprintf_s( fp_header, _T("};\n\n") );
//	TRACE(_T("};\n\n"));
//
//	// 写.h文件中的消息枚举定义
//	iter = g_msg_map.begin();
//	while( g_msg_map.end() != iter )
//	{
//		_ftprintf_s( fp_header, _T("enum ENUM_%s\n{\n"), iter->first.c_str() );
//		TRACE( _T("enum ENUM_%s\n{\n"), iter->first.c_str() );
//		CMsgIDList::iterator idIter = iter->second.begin();
//		while( idIter != iter->second.end() )
//		{
//			// 已经定义过的则不进行自动定义
//			CMsgValueMap::iterator valIter = msg_value_map.find(*idIter);
//			if( valIter != msg_value_map.end() )
//			{
//				_ftprintf_s( fp_header, _T("\t%s = %s,\n"), idIter->c_str(), valIter->second.c_str() );
//				TRACE(_T("\t%s = %s,\n"), idIter->c_str(), valIter->second.c_str());
//			}
//			else
//			{
//				_ftprintf_s( fp_header, _T("\t%s,\n"), idIter->c_str() );
//				TRACE(_T("\t%s,\n"), idIter->c_str() );
//			}
//			++idIter;
//		}
//
//		_ftprintf_s( fp_header, _T("\t%s_COUNT,\n"), iter->first.c_str() );
//		TRACE(_T("\t%s_COUNT,\n"), iter->first.c_str());
//		_ftprintf_s( fp_header, _T("};\n\n") );
//		TRACE(_T("};\n\n"));
//		++iter;
//	}
//
//	time_t t = time(NULL);
//	_ftprintf_s( fp_header, _T("static DWORD GetNetMessageVersion(){ return %u; }\n"), time(NULL) );
//	_ftprintf_s( fp_header, _T("#endif\n") );
//	fclose( fp_header );
//	return true;
//}
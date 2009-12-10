//#pragma once
//
//typedef std::vector< TString >	CParams;
//typedef std::list< CParams >	CParamsList;
//
//struct MessageDefine
//{
//	TString		strComment;
//	TString		strName;
//	TString		msgtype;
//	TString		msgid;
//	CParamsList	Body;
//};
//
//struct cmd_for_each
//{
//	TString	struct_name;
//	TString count_name;
//	TString	variant_name;
//};
//
//bool get_params( LPCTSTR lpszCommand, CParams& params, TCHAR split = _T(' ') );
//
//bool Analyse( LPCTSTR lpszSrcName, LPCTSTR lpszDestName, HWND hProgressWnd );
//bool AnalyseBody( MessageDefine& msg, FILE* fp_header, FILE* fp_cpp, HWND hProgressWnd );
//bool WriteMessageDefine( LPCTSTR lpszDefFilename, LPCTSTR lpszExt );
//
//bool is_array( LPCTSTR lpParam, TString& array_size, TString& variant_name );
//
//bool write_type( CParams& params, LPCTSTR in_name, FILE* fp, LPCTSTR prefix = _T("")  );
//bool read_type( CParams& params, LPCTSTR out_name, FILE* fp, LPCTSTR prefix = _T("") );
//
//template< class type >
//struct Type2Name
//{
//	static const char		*name;
//	static const wchar_t	*wname;
//	static const TCHAR		*tname;
//};
//
//template<>
//struct Type2Name< const char* >
//{
//	static const char		*name;
//	static const wchar_t	*wname;
//	static const TCHAR		*tname;
//};
//
//template<>
//struct Type2Name< const wchar_t * >
//{
//	static const char		*name;
//	static const wchar_t	*wname;
//	static const TCHAR		*tname;
//};
//
//template< class STRTYPE >
//bool write_String( CParams& params, LPCTSTR in_name, FILE* fp, LPCTSTR prefix = _T("") )
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
//template< class STRTYPE >
//bool read_String( CParams& params, LPCTSTR out_name, FILE* fp, LPCTSTR prefix = _T("") )
//{
//	TString array_size;
//	TString variant_name;
//	if( is_array( params[1], array_size, variant_name ) )
//	{
//		_ftprintf( fp,
//			_T("\tfor( int i = 0; i < %s; ++i ){%s p;\t%s.read( p );\t%s%s[i] = p;}\n"),
//			Type2Name< STRTYPE >::tname,
//			array_size.c_str(),
//			out_name,
//			prefix,
//			variant_name.c_str()
//			);
//	}
//	else
//	{
//		_ftprintf( fp, 
//			_T("\t{%s p = NULL;%s.read( p );%s%s = p;}\n"),
//			Type2Name< STRTYPE >::tname,
//			out_name,
//			prefix,
//			params[1].c_str()
//			);
//	}
//	return true;
//}
//
//bool write_TString( CParams& params, LPCTSTR in_name, FILE* fp, LPCTSTR prefix = _T("") );
//bool read_TString( CParams& params, LPCTSTR out_name, FILE* fp, LPCTSTR prefix = _T("") );

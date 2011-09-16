// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 DATABASE_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// DATABASE_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#pragma once
#ifndef __DATABASE_H
#define __DATABASE_H

#ifdef DATABASE_EXPORTS
#define DATABASE_API __declspec(dllexport)
#else
#define DATABASE_API __declspec(dllimport)
#endif

#ifndef _USE_BOOST
#include <functional>
using namespace std::tr1;
#else
#include "boost/function.hpp"
using namespace boost;
#endif

namespace XGC
{
	namespace db
	{
		typedef intptr_t recordset;
		typedef intptr_t command;
		typedef intptr_t connection;
		typedef intptr_t database_t;

		typedef function< void() > respcall;
		typedef function< respcall( db::connection conn ) >	postcall;

		enum db_type{ mssql, mysql };
		enum CommandType
		{
			dbCmdUnspecified = -1,
			dbCmdUnknown = 8,
			dbCmdText = 1,
			dbCmdTable = 2,
			dbCmdStoredProc = 4,
			dbCmdFile = 256,
			dbCmdTableDirect = 512
		};

		enum ParameterDirection
		{
			dbParamUnknown = 0,
			dbParamInput = 1,
			dbParamOutput = 2,
			dbParamInputOutput = 3,
			dbParamReturnValue = 4
		};

		extern "C"
		{
			DATABASE_API bool initial( db_type type );
			DATABASE_API void final();
			DATABASE_API extern unsigned int(*get_error_code)();
			DATABASE_API extern const char*	(*get_error_info)();

			DATABASE_API extern connection (*connect)( const char* str );
			DATABASE_API extern void (*disconnect)( connection conn );
			DATABASE_API extern bool (*execute_cmd)( connection conn, command cmd );
			DATABASE_API extern bool (*execute_cmdex)( connection conn, command cmd, recordset &rs );
			DATABASE_API extern bool (*execute_sql)( connection conn, const char *cmd );
			DATABASE_API extern bool (*execute_sqlex)( connection conn, const char *cmd, recordset &rs );
			DATABASE_API extern bool (*connected)( connection conn );

			//////////////////////////////////////////////////////////////////////////
			// recordset
			//////////////////////////////////////////////////////////////////////////
			DATABASE_API extern void (*movefirst)( recordset rs );
			DATABASE_API extern void (*movenext)( recordset rs );
			DATABASE_API extern void (*moveprev)( recordset rs );
			DATABASE_API extern void (*movelast)( recordset rs );
			DATABASE_API extern bool (*eof)( recordset rs );
			DATABASE_API extern bool (*bof)( recordset rs );
			DATABASE_API extern unsigned int (*recordcount)( recordset rs );
			DATABASE_API extern unsigned int (*fieldcount)( recordset rs );

			DATABASE_API extern int		(*field_integer)( recordset rs, const char* name );
			DATABASE_API extern char*	(*field_string)	( recordset rs, const char* name );
			DATABASE_API extern bool	(*field_bool)	( recordset rs, const char* name );
			DATABASE_API extern float	(*field_float)	( recordset rs, const char* name );
			DATABASE_API extern double	(*field_decimal)( recordset rs, const char* name );
			DATABASE_API extern long	(*field_binary)	( recordset rs, const char* name, char *data, long length );
			DATABASE_API extern tm		(*field_datetime)( recordset rs, const char* name );

			//////////////////////////////////////////////////////////////////////////
			// command
			//////////////////////////////////////////////////////////////////////////
			DATABASE_API extern command (*create_command)	( int type, const char *content );
			DATABASE_API extern void	(*destroy_command)	( command cmd );
			DATABASE_API extern void	(*reset_command)	( command cmd, int type, const char *content );
			DATABASE_API extern void	(*delete_param)		( command cmd, const char *name );

			DATABASE_API extern void (*insert_tinyint)	( command cmd, const char *name, char d, int v );
			DATABASE_API extern void (*insert_smallint)	( command cmd, const char *name, char d, int v );
			DATABASE_API extern void (*insert_integer)	( command cmd, const char *name, char d, int v );
			DATABASE_API extern void (*insert_bigint)		( command cmd, const char *name, char d, __int64 v );

			DATABASE_API extern void (*insert_utinyint)	( command cmd, const char *name, char d, unsigned int v );
			DATABASE_API extern void (*insert_usmallint)	( command cmd, const char *name, char d, unsigned int v );
			DATABASE_API extern void (*insert_uinteger)	( command cmd, const char *name, char d, unsigned int v );
			DATABASE_API extern void (*insert_ubigint)	( command cmd, const char *name, char d, unsigned __int64 v );

			DATABASE_API extern void (*insert_string)		( command cmd, const char *name, char d, const char *s, size_t n );
			DATABASE_API extern void (*insert_vstring)	( command cmd, const char *name, char d, const char *s, size_t n );
			DATABASE_API extern void (*insert_lvstring)	( command cmd, const char *name, char d, const char *s, size_t n );
			DATABASE_API extern void (*insert_wstring)	( command cmd, const char *name, char d, const wchar_t *s, size_t n );
			DATABASE_API extern void (*insert_vwstring)	( command cmd, const char *name, char d, const wchar_t *s, size_t n );
			DATABASE_API extern void (*insert_lvwstring)	( command cmd, const char *name, char d, const wchar_t *s, size_t n );
			DATABASE_API extern void (*insert_binary)		( command cmd, const char *name, char d, void *v, size_t n );
			DATABASE_API extern void (*insert_vbinary)	( command cmd, const char *name, char d, void *v, size_t n );
			DATABASE_API extern void (*insert_lvbinary)	( command cmd, const char *name, char d, void *v, size_t n );

			DATABASE_API extern void (*insert_bool)		( command cmd, const char *name, char d, bool v );
			DATABASE_API extern void (*insert_float)		( command cmd, const char *name, char d, float v );
			DATABASE_API extern void (*insert_double)		( command cmd, const char *name, char d, double v );
			DATABASE_API extern void (*insert_datetime)	( command cmd, const char *name, char d, tm v );

			DATABASE_API extern __int64		(*get_bigint)		( command cmd, const char *name );
			DATABASE_API extern unsigned int(*get_uinteger)	( command cmd, const char *name );
			DATABASE_API extern int			(*get_integer)	( command cmd, const char *name );
			DATABASE_API extern char*		(*get_string)		( command cmd, const char *name );
			DATABASE_API extern bool		(*get_bool)		( command cmd, const char *name );
			DATABASE_API extern float		(*get_float)		( command cmd, const char *name );
			DATABASE_API extern double		(*get_double)		( command cmd, const char *name );
			DATABASE_API extern char*		(*get_binary)		( command cmd, const char *name );
			DATABASE_API extern tm			(*get_datetime)	( command cmd, const char *name );
		};

		//////////////////////////////////////////////////////////////////////////
		// service
		//////////////////////////////////////////////////////////////////////////
		// 创建数据库服务
		DATABASE_API database_t create_database( const char* connstring );

		// 销毁数据库服务
		DATABASE_API void destroy_database( database_t s );

		// 投递数据库请求
		DATABASE_API void put_post( database_t s, const postcall &call );

		// 获取数据库回应
		DATABASE_API respcall get_resp( database_t s );

	}
}
#endif //__DATABASE_H

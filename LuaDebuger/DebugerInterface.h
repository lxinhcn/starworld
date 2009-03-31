
#ifndef _DEBUGER_INTERFACE
#define _DEBUGER_INTERFACE

typedef void (*ProcessRetCmd)( LPBYTE lpszCommand );

enum run_mode { lua_run, lua_debug, lua_stop };

typedef class LuaDebugCommander LuaDebugCommander;

extern LuaDebugCommander* Create_Commander( const char* pipe, ProcessRetCmd fn );
extern void Debug_Command( LuaDebugCommander* Debuger, const char* Cmd );
extern bool Debug_CheckMode( LuaDebugCommander* pDebuger, run_mode m );
extern void Destroy_Commander( LuaDebugCommander* Debuger );
#endif;
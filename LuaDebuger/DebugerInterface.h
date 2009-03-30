
#ifndef _DEBUGER_INTERFACE
#define _DEBUGER_INTERFACE
typedef void (*ProcessRetCmd)( LPBYTE lpszCommand );
class LuaDebugCommander;
typedef LuaDebugCommander LuaDebugCommander;
extern LuaDebugCommander* Create_Commander( const char* pipe, ProcessRetCmd fn );
extern void Debug_Command( LuaDebugCommander* Debuger, const char* Cmd );
extern void Destroy_Commander( LuaDebugCommander* Debuger );
#endif;
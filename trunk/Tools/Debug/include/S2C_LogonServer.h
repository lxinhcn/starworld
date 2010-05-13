#include "messagedef.h"
#include "structsdef.h"
#ifdef _USRDLL
	#ifdef MESSAGE_API
		#define MESSAGE_EXPORT __declspec(dllexport)
	#else
		#define MESSAGE_EXPORT __declspec(dllimport)
	#endif
#else
	#define MESSAGE_EXPORT
#endif

struct MESSAGE_EXPORT MMO_LogonReplay
{
	_byte mtype;
	_byte mcode;
	char token[256];
	unsigned int	address;
	unsigned short	port;
};

MESSAGE_EXPORT bufstream& operator << ( bufstream& stream, const MMO_LogonReplay& c );
MESSAGE_EXPORT bufstream& operator >> ( bufstream& stream, MMO_LogonReplay& c );

/////////////////////////////////////////////////////////////////////////////////
struct MESSAGE_EXPORT MMO_LogonFailed
{
	_byte mtype;
	_byte mcode;
	int failed_reason;
};

MESSAGE_EXPORT bufstream& operator << ( bufstream& stream, const MMO_LogonFailed& c );
MESSAGE_EXPORT bufstream& operator >> ( bufstream& stream, MMO_LogonFailed& c );

/////////////////////////////////////////////////////////////////////////////////
struct MESSAGE_EXPORT MMO_RegistReplay
{
	_byte mtype;
	_byte mcode;
	int failed_reason;
};

MESSAGE_EXPORT bufstream& operator << ( bufstream& stream, const MMO_RegistReplay& c );
MESSAGE_EXPORT bufstream& operator >> ( bufstream& stream, MMO_RegistReplay& c );

/////////////////////////////////////////////////////////////////////////////////

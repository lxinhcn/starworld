#include "StdAfx.h"
#include "DispatchDatabase.h"

#define REGISTER_USER 0

BEGIN_DISPATCHER_TABLE( CDispatcherDatabase, database )
	DECLARE_DISPATCH( REGISTER_USER, registerUser )
END_DISPATCHER_TABLE( CDispatcherDatabase, database )
CDispatcherDatabase::CDispatcherDatabase(void)
{
}

CDispatcherDatabase::~CDispatcherDatabase(void)
{
}

size_t CDispatcherDatabase::Process( dbMsgHeader &header )
{
	func pFunc = DISPATCHER_GET( database, header.mid );
	if( pFunc )
		return (this->*pFunc)( header );

	return -1;
}

//--------------------------------------------------------//
//	created:	3:12:2009   13:11
//	filename: 	DispatchDatabase
//	author:		Albert.xu
//
//	purpose:	×¢²áÒ»¸öÕËºÅ
//--------------------------------------------------------//
size_t CDispatcherDatabase::registerUser( dbMsgHeader &header )
{
	return -1;
}
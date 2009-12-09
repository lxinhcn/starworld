#include "StdAfx.h"
#include "DatabaseService.h"

#define REGISTER_USER 0

BEGIN_DISPATCHER_TABLE( CDatabaseService, database )
	DECLARE_DISPATCH( REGISTER_USER, registerUser )
END_DISPATCHER_TABLE( CDatabaseService, database )
CDatabaseService::CDatabaseService(void)
{
}

CDatabaseService::~CDatabaseService(void)
{
}

size_t CDatabaseService::Process( dbMsgHeader &header )
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
size_t CDatabaseService::registerUser( dbMsgHeader &header )
{
	return -1;
}
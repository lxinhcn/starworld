#include "StdAfx.h"
#include "DispatchDatabase.h"

#define REGISTER_USER 0

struct CDispatcherDatabase::message_table
{
	message_table()
	{
		CDispatcherDatabase::Impl.regist( REGISTER_USER, &CDispatcherDatabase::registerUser );
	}
};

DispatcherBase::functionImpl DispatcherBase::Impl;
CDispatcherDatabase::message_table CDispatcherDatabase::message_initialize;

CDispatcherDatabase::CDispatcherDatabase(void)
{
}

CDispatcherDatabase::~CDispatcherDatabase(void)
{
}

size_t CDispatcherDatabase::ProcessDBRequest( dbMsgHeader &header )
{
	functionImpl::proc func = Impl.get( header.mid );
	if( func )
		return (this->*func)( header );

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
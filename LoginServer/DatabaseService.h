#pragma once

class CDatabaseService;
typedef message_dispatcher< CDatabaseService, size_t (CDatabaseService::*)( dbMsgHeader &header ), short > DispatcherBase;

class CDatabaseService	:	public DispatcherBase
{
	IMPLEMENT_DISPATCHER_TABLE( database );
public:
	CDatabaseService(void);
	~CDatabaseService(void);

	size_t Process( dbMsgHeader &header );
protected:
	//--------------------------------------------------------//
	//	created:	3:12:2009   13:11
	//	filename: 	DispatchDatabase
	//	author:		Albert.xu
	//
	//	purpose:	×¢²áÒ»¸öÕËºÅ
	//--------------------------------------------------------//
	size_t registerUser( dbMsgHeader &header );
};

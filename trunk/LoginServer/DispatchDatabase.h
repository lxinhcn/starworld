#pragma once
struct dbMsgHeader
{
	size_t	size;	// 消息长度
	size_t	uid;	// 用户id
	size_t	mid;	// 消息号
};

class CDispatcherDatabase;
typedef message_dispatcher< CDispatcherDatabase, size_t (CDispatcherDatabase::*)( dbMsgHeader &header ), short > DispatcherBase;

class CDispatcherDatabase	:	public DispatcherBase
{
	IMPLEMENT_DISPATCHER_TABLE( database );
public:
	CDispatcherDatabase(void);
	~CDispatcherDatabase(void);

	size_t Process( dbMsgHeader &header );
protected:
	//--------------------------------------------------------//
	//	created:	3:12:2009   13:11
	//	filename: 	DispatchDatabase
	//	author:		Albert.xu
	//
	//	purpose:	注册一个账号
	//--------------------------------------------------------//
	size_t registerUser( dbMsgHeader &header );
};

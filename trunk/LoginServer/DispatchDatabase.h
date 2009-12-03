#pragma once
struct dbMsgHeader
{
	size_t	size;	// 消息长度
	size_t	uid;	// 用户id
	size_t	mid;	// 消息号
};

template< class handler >
struct message_dispatcher_db
{
	virtual size_t ProcessDBRequest( dbMsgHeader &header ) = 0;
	typedef size_t ( handler::* proc )( dbMsgHeader &header );
};

class CDispatcherDatabase;
typedef message_dispatcher< CDispatcherDatabase, short, message_dispatcher_db< CDispatcherDatabase > > DispatcherBase;

class CDispatcherDatabase	:	public DispatcherBase
{
public:
	CDispatcherDatabase(void);
	~CDispatcherDatabase(void);

protected:
	virtual size_t ProcessDBRequest( dbMsgHeader &header );

protected:
	//--------------------------------------------------------//
	//	created:	3:12:2009   13:11
	//	filename: 	DispatchDatabase
	//	author:		Albert.xu
	//
	//	purpose:	注册一个账号
	//--------------------------------------------------------//
	size_t registerUser( dbMsgHeader &header );

private:
	struct message_table;
	static message_table	message_initialize;
};

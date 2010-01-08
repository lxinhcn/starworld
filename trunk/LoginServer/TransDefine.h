#ifndef _TRANSACTIME_H
#define _TRANSACTIME_H

#define TRANS_BUF_SIZE	4096;
struct transaction
{
	size_t	size;	// 消息长度
	size_t	uid;	// 用户id
	_byte	mid;	// 消息号
	_byte	pid;	// 流水线号

	char	msg[1];	// 消息缓冲
};

enum
{
	db_user_regist,
	db_user_logon,
};

enum
{
	lg_logon_failed,
	lg_logon_success,
};

struct dbRegistUser
{
	char username[128];
	char password[128];
};

struct dbUserLogon
{
	char username[128];
	char password[128];
};

#endif // _TRANSACTIME_H
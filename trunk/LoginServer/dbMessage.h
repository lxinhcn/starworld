struct dbMsgHeader
{
	size_t	size;	// 消息长度
	size_t	uid;	// 用户id
	size_t	mid;	// 消息号
};

struct dbRegistUser	:	public dbMsgHeader
{
	_string username;
	_string password;
};
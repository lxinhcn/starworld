#pragma once

class CLogicService;
typedef message_dispatcher< CLogicService, size_t (CLogicService::*)( transaction &header ), short > LogicDispatcherBase;

class CLogicService	: public LogicDispatcherBase
{
public:
	CLogicService(void);
	~CLogicService(void);
};

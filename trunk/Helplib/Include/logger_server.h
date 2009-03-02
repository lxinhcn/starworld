#pragma once
#ifndef _LOGGERSERVER_H
#define _LOGGERSERVER_H
#include "NetEvent.h"
#include "NetPackage.h"

class CLoggerClient	:	public ISocketBase
{
public:
	CLoggerClient();
	~CLoggerClient();
private:
	virtual void OnAccept();
	virtual void OnRead();
	virtual void OnWrite();
	virtual void OnClose();
	virtual void OnConnect();

	virtual void Release();

	void ProcessPackage();
private:
	CNetPackage* m_pHeadPackage;
	CNetPackage* m_pPackage;

	_string m_strLogger;
	_string m_strWatcher;
};

class TOOLLIB_API CLoggerServer	:	public CNetEvent
{
public:
	CLoggerServer(){}
	~CLoggerServer(){}

	virtual ISocketBase* AllocClient();
	static bool	InitUserTable( LPCTSTR lpszTableFile );
	static bool CheckTable( LPCTSTR pUser, LPCTSTR pPass );
};

#endif // _LOGGERSERVER_H
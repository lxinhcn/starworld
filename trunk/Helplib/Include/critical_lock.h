#pragma once

#ifdef TOOLLIB_EXPORTS
#define TOOLLIB_API __declspec(dllexport)
#else
#define TOOLLIB_API __declspec(dllimport)
#endif	//TOOLLIB_EXPORTS

class TOOLLIB_API CCritical
{
public:
	CCritical();
	~CCritical();

	void Lock();
	void UnLock();

	operator CRITICAL_SECTION&(){ return m_lock; }

private:
	CRITICAL_SECTION m_lock;
};

class TOOLLIB_API CCriticalLock
{
public:
	explicit CCriticalLock( CRITICAL_SECTION& Critical );
	virtual ~CCriticalLock(void)throw();

	void operator=( CONST CCriticalLock& lock );
	void operator=( CONST CRITICAL_SECTION& Critical );
private:
	CRITICAL_SECTION& m_CriticalRef;
};

class TOOLLIB_API CCriticalTryLock
{
public:
	explicit CCriticalTryLock( CRITICAL_SECTION& Critical );
	virtual ~CCriticalTryLock(void);

	bool IsLock(){ return m_bLocked; }
private:
	CRITICAL_SECTION& m_CriticalRef;
	bool m_bLocked;
};

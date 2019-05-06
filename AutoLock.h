#pragma once
#include "windows.h"
#define AUTOLOCK(lock) CAutoLock autolock(lock)
class CLock
{
public:
	CLock()
	{
		InitializeCriticalSection(&m_cs);
	}
	~CLock()
	{
		DeleteCriticalSection(&m_cs);
	}
public:
	void Lock(void)
	{
		EnterCriticalSection(&m_cs);
	}
	void unLock(void)
	{
		LeaveCriticalSection(&m_cs);
	}
private:
	CRITICAL_SECTION m_cs;
};

class CAutoLock
{
public:
	CAutoLock(CLock &lock):m_Lock(lock)
	{
		m_Lock.Lock();
	}
	~CAutoLock()
	{
		m_Lock.unLock();
	}
private:
	CLock &m_Lock;
};
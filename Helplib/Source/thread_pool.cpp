// Filename		: ThreadPool.cpp
// Author		: Siddharth Barman
// Date			: 18 Sept 2005
// Description	: Implementation file for CThreadPool class. 
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "thread_pool.h"
//------------------------------------------------------------------------------
/* Parameters	: pointer to a _threadData structure.
   Description	: This is the internal thread function which will run 
				  continuously till the Thread Pool is deleted. Any user thread
				  functions will run from within this function.
*/
#ifdef USE_WIN32API_THREAD
DWORD WINAPI CThreadPool::_ThreadProc(LPVOID pParam)
#else
UINT __stdcall CThreadPool::_ThreadProc(LPVOID pParam)
#endif
{
	DWORD					dwWait;
	CThreadPool*			pool;
  	HANDLE					hThread = GetCurrentThread();
	LPTHREAD_START_ROUTINE  proc;
	LPVOID					data;
	DWORD					dwThreadId = GetCurrentThreadId();
	HANDLE					hWaits[2];
	IRunObject*				runObject;
	bool					bAutoDelete;

	ASSERT(pParam != NULL);
	if(NULL == pParam)
	{
		return -1;
	}

	pool = static_cast<CThreadPool*>(pParam);
	hWaits[0] = pool->GetWaitHandle(dwThreadId);
	hWaits[1] = pool->GetShutdownHandle();

	loop_here:

	dwWait = WaitForMultipleObjects(2, hWaits, FALSE, INFINITE);

	if(dwWait - WAIT_OBJECT_0 == 0)
	{
		// a new function was added, go and get it
		pool->BusyNotify(dwThreadId);
		while(pool->GetThreadProc(dwThreadId, proc, &data, &runObject))
		{
			if(proc == NULL)
			{
				// a function object is being used instead of 
				// a function pointer.
				bAutoDelete = runObject->AutoDelete();
				runObject->Run();

				// see if we need to free this object
				if(bAutoDelete)
				{
					delete runObject;
				}
			}
			else
			{
				proc(data);
			}
		}
		pool->FinishNotify(dwThreadId); // tell the pool, i am now free

		goto loop_here;
	}

	return 0;
}

#ifdef USE_WIN32API_THREAD
DWORD WINAPI CThreadPool::_CheckThreadProc(LPVOID pParam)
#else
UINT __stdcall CThreadPool::_CheckThreadProc(LPVOID pParam)
#endif
{
	CThreadPool* pThis = (CThreadPool*)pParam;

	while( WaitForSingleObject( pThis->m_hNotifyCheckdown, 1000 ) == WAIT_TIMEOUT )
	{
		CCriticalLock _lock( pThis->m_cs );
		ThreadMap::iterator iter = pThis->m_threads.begin();
		while( iter != pThis->m_threads.end() )
		{
			_ThreadData data = iter->second;
			if( !data.bFree && timeGetTime() - data.dwLastUpdateTime > pThis->m_dwThreadTimeOut )
			{
				// 监测出超时线程
				pThis->m_threads.erase( iter );

				TerminateThread( data.hThread, -1 );

				DWORD	dwThreadId;
				HANDLE	hThread;

#ifdef USE_WIN32API_THREAD
				hThread = CreateThread(NULL, 0, CThreadPool::_ThreadProc, 
					pThis, CREATE_SUSPENDED, &dwThreadId);
#else
				UINT uThreadId;
				hThread = (HANDLE)_beginthreadex(NULL, 0, CThreadPool::_ThreadProc, pThis, 
					CREATE_SUSPENDED, (UINT*)&uThreadId);
				dwThreadId = uThreadId;
#endif
				ASSERT(NULL != hThread);

				if(hThread)
				{
					// add the entry to the map of threads
					data.bFree		= true;
					data.hThread	= hThread;
					data.dwThreadId	= dwThreadId;

					// 下面这一句我也不想加,因为创建的时候已经设置为未通知状态了,本来不需要再Reset一次的。
					// 但是，如果没有下面这句WaitSignleObjects 就会不阻塞直接返回。[4/27/2007]
					ResetEvent( data.WaitHandle );
					ResumeThread(hThread);

					pThis->m_threads.insert(ThreadMap::value_type(dwThreadId, data));
				}
			}
			++iter;
		}
	}
	return 0;
}

//------------------------------------------------------------------------------
/* Parameters	: Pool size, indicates the number of threads that will be 
				  available in the queue.
*******************************************************************************/
CThreadPool::CThreadPool(int nPoolSize, bool bCreateNow)
: m_nMaxListCount( 100 )
, m_dwTimeOut( 0 )
, m_nActiveSize( 0 )
, m_dwThreadTimeOut( 10000 )
{
	m_nPoolSize = nPoolSize;
	
	if(bCreateNow)
	{
		if(!Create())
		{
			throw 1;
		}
	}
}
//------------------------------------------------------------------------------

/* Description	: Use this method to create the thread pool. The constructor of
				  this class by default will create the pool. Make sure you 
				  do not call this method without first calling the Destroy() 
				  method to release the existing pool.
   Returns		: true if everything went fine else returns false.
  *****************************************************************************/
bool CThreadPool::Create()
{
	HANDLE		hThread;
	DWORD		dwThreadId;
	_ThreadData ThreadData;
	TCHAR		szEvtName[20];
	UINT		uThreadId;	
	
	// create the event which will signal the threads to stop
	m_hNotifyShutdown = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hNotifyCheckdown = CreateEvent(NULL, TRUE, FALSE, NULL);
	ASSERT(m_hNotifyShutdown != NULL);
	if(!m_hNotifyShutdown)
	{
		return false;
	}

	// create the threads
	for(int nIndex = 0; nIndex < m_nPoolSize; nIndex++)
	{
		_stprintf_s(szEvtName, _countof(szEvtName), _T("Thread%d"), nIndex);
				
		#ifdef USE_WIN32API_THREAD
		hThread = CreateThread(NULL, 0, CThreadPool::_ThreadProc, 
							   this, CREATE_SUSPENDED, &dwThreadId);
		#else
		hThread = (HANDLE)_beginthreadex(NULL, 0, CThreadPool::_ThreadProc, this,  
								 CREATE_SUSPENDED, (UINT*)&uThreadId);
		dwThreadId = uThreadId;
		#endif
		ASSERT(NULL != hThread);
		
		if(hThread)
		{
			// add the entry to the map of threads
			ThreadData.bFree		= true;
			ThreadData.WaitHandle	= CreateEvent(NULL, TRUE, FALSE, NULL);
			ThreadData.hThread		= hThread;
			ThreadData.dwThreadId	= dwThreadId;
		
			// 下面这一句我也不想加,因为创建的时候已经设置为未通知状态了,本来不需要再Reset一次的。
			// 但是，如果没有下面这句WaitSignleObjects 就会不阻塞直接返回。[4/27/2007]
			ResetEvent( ThreadData.WaitHandle );

			m_threads.insert(ThreadMap::value_type(dwThreadId, ThreadData));		

			ResumeThread(hThread); 
		
		}
		else
		{
			return false;
		}
	}

	return true;
}
//------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------//
bool CThreadPool::EnableUpdateCheck( bool bEnable )	// enable threads update checking
{
	if( bEnable )
	{
		// 启动监控线程
#ifdef USE_WIN32API_THREAD
		m_hCheckThread = CreateThread(NULL, 0, CThreadPool::_CheckThreadProc, 
			this, 0, NULL);
#else
		m_hCheckThread = (HANDLE)_beginthreadex(NULL, 0, CThreadPool::_CheckThreadProc, this,
			0, NULL);
#endif
		if( !m_hCheckThread ) return false;
	}
	else
	{
		SetEvent( m_hNotifyCheckdown );
		WaitForSingleObject( m_hCheckThread, 5000 );
		CloseHandle( m_hCheckThread );
		m_hCheckThread = NULL;
	}
	return true;
}

CThreadPool::~CThreadPool()
{
	Destroy();
}
//------------------------------------------------------------------------------

/* Description	: Use this method to destory the thread pool. The destructor of
				  this class will destory the pool anyway. Make sure you 
				  this method before calling a Create() when an existing pool is 
				  already existing.
   Returns		: void
  *****************************************************************************/
void CThreadPool::Destroy()
{
	// tell all threads to shutdown.
	SetEvent(m_hNotifyShutdown);
	SetEvent(m_hNotifyCheckdown);

	// lets give the thread one second atleast to terminate
	Sleep(1000);
	
	ThreadMap::iterator iter;
	_ThreadData ThreadData;
	
	// walk through the events and threads and close them all
	for(iter = m_threads.begin(); iter != m_threads.end(); iter++)
	{
		ThreadData = (*iter).second;		
		CloseHandle(ThreadData.WaitHandle);
		CloseHandle(ThreadData.hThread);
	}

	// close the shutdown event
	CloseHandle(m_hNotifyShutdown);
	CloseHandle(m_hNotifyCheckdown);
	CloseHandle(m_hCheckThread);

	// empty all collections
	FunctionList::iterator iterList = m_functionList.begin();
	while( iterList != m_functionList.end() )
	{
		if( (*iterList).runObject && (*iterList).runObject->AutoDelete() )
		{
			delete (*iterList).runObject;
			(*iterList).runObject = NULL;
		}
		++iterList;
	}

	m_functionList.clear();
	m_threads.clear();
}
//------------------------------------------------------------------------------

int CThreadPool::GetPoolSize()
{
	return m_nPoolSize;
}
//------------------------------------------------------------------------------

/* Parameters	: nSize - number of threads in the pool.   
   ****************************************************************************/
void CThreadPool::SetPoolSize(int nSize)
{
	ASSERT(nSize > 0);
	if(nSize <= 0)
	{
		return;
	}

	m_nPoolSize = nSize;
}

int	CThreadPool::GetWaitCount()const
{
	return (int)m_functionList.size();
}

//--------------------------------------------------------------------------------------------------------//
// 设置最大队列长度
// nMax		:	队列最大值
//--------------------------------------------------------------------------------------------------------//
void CThreadPool::SetMaxWaitCount( size_t nMax )
{
	m_nMaxListCount = nMax;
}

//--------------------------------------------------------------------------------------------------------//
// 设置超时时间
// dwSecond		:	超时时间
//--------------------------------------------------------------------------------------------------------//
void CThreadPool::SetTimeOut( DWORD dwSecond )
{
	m_dwTimeOut = dwSecond;
}

//--------------------------------------------------------------------------------------------------------//
// 设置线程超时时间
// dwSecond		:	超时时间
//--------------------------------------------------------------------------------------------------------//
void	CThreadPool::SetThreadTimeOut( DWORD dwSecond )
{
	m_dwThreadTimeOut = dwSecond;
}

//--------------------------------------------------------------------------------------------------------//
// 更新线程时间
// dwThreadID	:	线程标示
//--------------------------------------------------------------------------------------------------------//
void	CThreadPool::UpdateThreadTime()
{
	DWORD dwThreadID = GetCurrentThreadId();
	CCriticalLock _lock( m_cs );
	ThreadMap::iterator iter = m_threads.find( dwThreadID );
	if( iter != m_threads.end() )
	{
		iter->second.dwLastUpdateTime = timeGetTime();
	}
}

//------------------------------------------------------------------------------
HANDLE CThreadPool::GetShutdownHandle()
{
	return m_hNotifyShutdown;
}
//------------------------------------------------------------------------------

/* Parameters	: hThread - Handle of the thread that is invoking this function.
   Return		: A ThreadProc function pointer. This function pointer will be 
			      executed by the actual calling thread.
				  NULL is returned if no functions list is empty.
																			  */
bool CThreadPool::GetThreadProc(DWORD dwThreadId, LPTHREAD_START_ROUTINE& Proc, 
								LPVOID* Data, IRunObject** runObject)
{
	LPTHREAD_START_ROUTINE  lpResult = NULL;
	_FunctionData			FunctionData;
	FunctionList::iterator	iter;

	// get the first function info in the function list
	CCriticalLock lock(m_cs);
	
	// 抛弃所有超时的数据
	while( m_dwTimeOut && m_functionList.size() )
	{
		_FunctionData& FunctionData = *m_functionList.begin();
		if( ( clock() - FunctionData.dwTimeStab )/CLOCKS_PER_SEC < m_dwTimeOut )
		{
			break;
		}
		if( FunctionData.runObject && FunctionData.runObject->AutoDelete() )
		{
			delete FunctionData.runObject;
		}
		m_functionList.pop_front();
	}

	iter = m_functionList.begin();

	if(iter != m_functionList.end())
	{
		FunctionData = (*iter);

		Proc = FunctionData.lpStartAddress;
		
		if(NULL == Proc) // is NULL for function objects
		{		
			*runObject = static_cast<IRunObject*>(FunctionData.pData);
		}
		else		
		{
			// this is a function pointer
			*Data		= FunctionData.pData;
			runObject	= NULL;
		}

		m_functionList.pop_front(); // remove the function from the list

		return true;
	}
	else
	{
		return false;
	}
}
//------------------------------------------------------------------------------

/* Parameters	: hThread - Handle of the thread that is invoking this function.
   Description	: When ever a thread finishes executing the user function, it 
				  should notify the pool that it has finished executing.      
																			  */
void CThreadPool::FinishNotify(DWORD dwThreadId)
{
	ThreadMap::iterator iter;
	
	CCriticalLock lock(m_cs);
	iter = m_threads.find(dwThreadId);

	if(iter == m_threads.end())	// if search found no elements
	{			
		ASSERT(!_T("No matching thread found."));
		return;
	}
	else
	{	
		m_threads[dwThreadId].bFree = true;

		if(!m_functionList.empty())
		{
			// there are some more functions that need servicing, lets do that.
			// By not doing anything here we are letting the thread go back and
			// check the function list and pick up a function and execute it.
			return;
		}		
		else
		{
			// back to sleep, there is nothing that needs servicing.
			ResetEvent(m_threads[dwThreadId].WaitHandle);
		}
	}	
}
//------------------------------------------------------------------------------

void CThreadPool::BusyNotify(DWORD dwThreadId)
{
	ThreadMap::iterator iter;
	
	CCriticalLock lock(m_cs);

	iter = m_threads.find(dwThreadId);

	if(iter == m_threads.end())	// if search found no elements
	{
		ASSERT(!_T("No matching thread found."));
	}
	else
	{		
		m_threads[dwThreadId].bFree = false;		
	}
}
//------------------------------------------------------------------------------

/* Parameters	: pFunc - function pointer of type ThreadProc
				  pData - An LPVOID pointer
   Decription	: This function is to be called by clients which want to make 
				  use of the thread pool.
  *****************************************************************************/
bool CThreadPool::Run(LPTHREAD_START_ROUTINE pFunc, LPVOID pData, 
					  ThreadPriority priority)
{
	CCriticalLock lock(m_cs);
	if( m_functionList.size() >= m_nMaxListCount )
	{
		return false;
	}

	_FunctionData funcdata;

	funcdata.lpStartAddress = pFunc;
	funcdata.pData			= pData;
	funcdata.dwTimeStab		= clock();

	// add it to the list
	if(priority == Low)
	{
		m_functionList.push_back(funcdata);
	}
	else
	{
		m_functionList.push_front(funcdata);
	}

	// See if any threads are free
	ThreadMap::iterator iter;
	_ThreadData ThreadData;

	for(iter = m_threads.begin(); iter != m_threads.end(); iter++)
	{
		ThreadData = (*iter).second;
		
		if(ThreadData.bFree)
		{
			// here is a free thread, put it to work
			// m_threads[ThreadData.dwThreadId].bFree = false;
			m_threads[ThreadData.dwThreadId].dwLastUpdateTime = timeGetTime();
			SetEvent(ThreadData.WaitHandle); 
			// this thread will now call GetThreadProc() and pick up the next
			// function in the list.
			break;
		}
	}
	return true;
}
//------------------------------------------------------------------------------

/* Parameters	: runObject - Pointer to an instance of class which implements
							  IRunObject interface.
				  priority  - Low or high. Based on this the object will be
							  added to the front or back of the list.
	return		: true - function has been push to queue
				  false - queue was full;
   Decription	: This function is to be called by clients which want to make 
				  use of the thread pool.
  *****************************************************************************/
bool CThreadPool::Run(IRunObject* runObject, ThreadPriority priority)
{
	ASSERT(runObject != NULL);
	
	CCriticalLock lock(m_cs);
	if( m_functionList.size() >= m_nMaxListCount )
	{
		return false;
	}

	_FunctionData funcdata;

	funcdata.lpStartAddress = NULL; // NULL indicates a function object is being
									// used instead.
	funcdata.pData			= runObject; // the function object

	funcdata.dwTimeStab		= clock();
	// add it to the list
	if(priority == Low)
	{
		m_functionList.push_back(funcdata);
	}
	else
	{
		m_functionList.push_front(funcdata);
	}

	// See if any threads are free
	ThreadMap::iterator iter;
	_ThreadData ThreadData;

	for(iter = m_threads.begin(); iter != m_threads.end(); iter++)
	{
		ThreadData = (*iter).second;
		
		if(ThreadData.bFree)
		{
			// here is a free thread, put it to work
			// m_threads[ThreadData.dwThreadId].bFree = false;
			m_threads[ThreadData.dwThreadId].dwLastUpdateTime = timeGetTime();
			SetEvent(ThreadData.WaitHandle); 
			// this thread will now call GetThreadProc() and pick up the next
			// function in the list.
			break;
		}
	}
	return true;
}
//------------------------------------------------------------------------------

/* Parameters	: ThreadId - the id of the thread for which the wait handle is 
							 being requested.
   Returns		: NULL if no mathcing thread id is present.
				  The HANDLE which can be used by WaitForXXXObject API.
  *****************************************************************************/
HANDLE CThreadPool::GetWaitHandle(DWORD dwThreadId)
{
	HANDLE hWait;
	ThreadMap::iterator iter;
	
	CCriticalLock lock(m_cs);
	iter = m_threads.find(dwThreadId);
	
	if(iter == m_threads.end())	// if search found no elements
	{
		return NULL;
	}
	else
	{
		ASSERT( iter->second.dwThreadId == dwThreadId );
		hWait = iter->second.WaitHandle;
	}
	return hWait;
}
//------------------------------------------------------------------------------
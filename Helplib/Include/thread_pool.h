// Filename		: ThreadPool.h
// Author		: Siddharth Barman
// Date			: 18 Sept 2005
/* Description	: Defines CThreadPool class. How to use the Thread Pool. First
				  create a CThreadPool object. The default constructor will 
				  create 10 threads in the pool. To defer creation of the pool
				  pass the pool size and false to the sonstructor. 
				  
				  You can use two approaches while working with the thread 
				  pool. 

				  1. To make use of the thread pool, you will need to first 
				  create a function having the following signature
				  DWORD WINAPI ThreadProc(LPVOID); Check the CreateThread 
				  documentation in MSDN to get details. Add this function to	
				  the pool by calling the Run() method and pass in the function 
				  name and a void* pointer to any object you want. The pool will
				  pick up the function passed into Run() method and execute as 
				  threads in the pool become free. 

				  2. Instead of using a function pointer, you can use an object
				  of a class which derives from IRunObject. Pass a pointer to 
				  this object in the Run() method of the thread pool. As threads
				  become free, the thread pool will call the Run() method of you
				  r class. You will also need to write a body for AutoDelete() f
				  unction. If the return value is true, the thread pool will use
				  'delete' to free the object you pass in. If it returns false,  
				  the thread pool will not do anything else to the object after
				  calling the Run() function.
	
				  It is possible to destroy the pool whenever you want by 
				  calling the Destroy() method. If you want to create a new pool
				  call the Create() method. Make sure you have destoryed the 
				  existing pool before creating a new one.
				  
				  By default, the pool uses _beginthreadex() function to create
				  threads. To have the pool use CreateThread() Windows API, just
				  define USE_WIN32API_THREAD. Note: it is better to use the defa
				  ult _beginthreadex(). 
				  
				  If this code works, it was written by Siddharth Barman, email 
				  siddharth_b@yahoo.com. 
				   _____ _                        _  ______           _  
				  |_   _| |                      | | | ___ \         | | 
				    | | | |__  _ __ ___  __ _  __| | | |_/ /__   ___ | | 
				    | | | '_ \| '__/ _ \/ _` |/ _` | |  __/ _ \ / _ \| | 
				    | | | | | | | |  __/ (_| | (_| | | | | (_) | (_) | | 
				    \_/ |_| |_|_|  \___|\__,_|\__,_| \_|  \___/ \___/|_|  
------------------------------------------------------------------------------*/
#ifndef THREAD_POOL_CLASS
#define THREAD_POOL_CLASS
#pragma warning( disable : 4786) // remove irritating STL warnings

#ifdef TOOLLIB_EXPORTS
#define TOOLLIB_API __declspec(dllexport)
#else
#define TOOLLIB_API __declspec(dllimport)
#endif	//TOOLLIB_EXPORTS

struct TOOLLIB_API IRunObject
{
	virtual void Run() = 0;
	virtual bool AutoDelete() = 0;
};

#define POOL_SIZE		  10
#define SHUTDOWN_EVT_NAME _T("PoolEventShutdown")
#define THREADPOOL		_T("THREADPOOL")

class TOOLLIB_API CThreadPool
{
public:
	enum ThreadPriority{ High,	Low	};
	// info about functions which require servicing will be saved using this struct.
	typedef struct tagFunctionData
	{
		LPTHREAD_START_ROUTINE lpStartAddress;
		union 
		{
			IRunObject* runObject;
			LPVOID pData;
		};	
		DWORD	dwTimeStab;
	} _FunctionData;

	// // info about threads in the pool will be saved using this struct.
	typedef struct tagThreadData
	{
		bool	bFree;
		HANDLE	WaitHandle;
		HANDLE	hThread;
		DWORD	dwThreadId;
		DWORD	dwLastUpdateTime;
	} _ThreadData;

	// info about all threads belonging to this pool will be stored in this map
	typedef std::map<DWORD, _ThreadData, std::less<DWORD>, std::allocator<_ThreadData> > ThreadMap;

	// all functions passed in by clients will be initially stored in this list.
	typedef std::list<_FunctionData, std::allocator<_FunctionData> > FunctionList;

	// this decides whether a function is added to the front or back of the list.
private:
	#ifdef USE_WIN32API_THREAD
		static DWORD WINAPI _ThreadProc(LPVOID);
		static DWORD WINAPI _CheckThreadProc(LPVOID);
	#else
		static UINT __stdcall _ThreadProc(LPVOID pParam);
		static UINT __stdcall _CheckThreadProc(LPVOID pParam);
	#endif
	
	FunctionList m_functionList;
	ThreadMap m_threads;

	HANDLE	m_hCheckThread;		// 监控线程句柄
	int		m_nPoolSize;
	int		m_nActiveSize;		// Active thread size
	HANDLE	m_hNotifyShutdown;	// notifies threads that a new function 
								// is added
	HANDLE	m_hNotifyCheckdown;
	size_t	m_nMaxListCount;	// max list count
	DWORD	m_dwTimeOut;		// time out setting. using second
	DWORD	m_dwThreadTimeOut;	// thread time out;

	CCritical m_cs;

	bool	GetThreadProc(DWORD dwThreadId, LPTHREAD_START_ROUTINE&, 
						  LPVOID*, IRunObject**); 
	
	void	FinishNotify(DWORD dwThreadId);
	void	BusyNotify(DWORD dwThreadId);
	
	HANDLE	GetWaitHandle(DWORD dwThreadId);
	HANDLE	GetShutdownHandle();

public:
	CThreadPool(int nPoolSize = POOL_SIZE, bool bCreateNow = false);
	virtual ~CThreadPool();
	bool	Create();	// creates the thread pool
	void	Destroy();	// destroy the thread pool
	bool	EnableUpdateCheck( bool bEnable );	// enable threads update checking

	int		GetPoolSize();
	void	SetPoolSize(int);

	//--------------------------------------------------------------------------------------------------------//
	// 得到队列中等待处理的业务个数
	//--------------------------------------------------------------------------------------------------------//
	int		GetWaitCount()const;

	//--------------------------------------------------------------------------------------------------------//
	// 设置最大队列长度
	// nMax		:	队列最大值
	//--------------------------------------------------------------------------------------------------------//
	void	SetMaxWaitCount( size_t nMax );

	//--------------------------------------------------------------------------------------------------------//
	// 设置超时时间
	// dwSecond		:	超时时间
	//--------------------------------------------------------------------------------------------------------//
	void	SetTimeOut( DWORD dwSecond );

	//--------------------------------------------------------------------------------------------------------//
	// 设置线程超时时间
	// dwSecond		:	超时时间
	//--------------------------------------------------------------------------------------------------------//
	void	SetThreadTimeOut( DWORD dwSecond );

	//--------------------------------------------------------------------------------------------------------//
	// 更新调用该函数的线程的线程时间
	//--------------------------------------------------------------------------------------------------------//
	void	UpdateThreadTime();

	bool	Run(LPTHREAD_START_ROUTINE pFunc, LPVOID pData, ThreadPriority priority = Low);

	bool	Run(IRunObject*, ThreadPriority priority = Low);
};
//------------------------------------------------------------------------------
#endif

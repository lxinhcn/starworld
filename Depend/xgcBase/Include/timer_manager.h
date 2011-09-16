#pragma once
//////////////////////////////////////////////////////////////////////////
// Loki 头文件

#ifdef EXPORT_API
#define EXPORT_LIB __declspec(dllexport)
#else
#define EXPORT_LIB __declspec(dllimport)
#endif
#ifndef _USE_BOOST
	#include <functional>
	using namespace std;
	using namespace std::tr1;
	using namespace std::tr1::placeholders;
#else
	#include "boost\function.hpp"
	#include "boost\bind.hpp"
	using namespace boost;
#endif
namespace XGC
{
	namespace common
	{
		//////////////////////////////////////////////////////////////////////////
		// 定时器管理类
		// base		:	基准时间
		//////////////////////////////////////////////////////////////////////////
		typedef unsigned int timer_t;
		class EXPORT_LIB timer
		{
		public:
			timer(void);
			~timer(void);

		public:
			void active();
			//////////////////////////////////////////////////////////////////////////
			// 定时器事件接口
			//////////////////////////////////////////////////////////////////////////
			typedef function< bool ( unsigned int ) > _efunction;
			struct _tevent	/*:	public CPoolAllocator*/
			{
				friend class timer;
			protected:
				//------------------------------------------------//
				// [7/28/2008] Write by Albert.xu
				// Description		: 构造时间事件对象
				// _function	: 函数指针或者重载operator()的结构体
				// _repeat		: 重复次数, 至少执行一次.
				// _timer		: 事件的触发时间间隔
				//------------------------------------------------//
				_tevent( _efunction &_function, unsigned int _repeat, unsigned int _timer, uintptr_t _user_ptr = 0 )
					: function( _function )
					, handle(-1)
					, repeat(_repeat)
					, interval(_timer)
					, index(0)
					, user_ptr( _user_ptr )
				{

				};

				virtual ~_tevent()
				{
					function = NULL;
					handle	= -1;
					repeat	= 0;
					interval= 0;
					index	= 0;
					user_ptr = 0;
				}

			public:
				__inline uintptr_t&		get_user_ptr()		{ return user_ptr; }
				__inline timer_t		get_handle()const	{ return handle; } 
				__inline unsigned int	get_invent()const	{ return interval; } 
				__inline unsigned int	get_repeat()const	{ return repeat; }

				__inline void set_interval( unsigned int _interval ){ interval = _interval; } 
				__inline void set_repeat( unsigned int _repeat ){ repeat = _repeat; }

			private:
				void release(){ delete this; }

				_efunction		function;
				timer_t			handle;	// 句柄
				unsigned int	interval;	// 时间间隔, 基准之间的倍数
				unsigned int	repeat;	// 执行次数, 执行次数为降为0 则被销毁
				unsigned int	index;	// 索引号, 代表在那个数组中
				uintptr_t		user_ptr;	// 用户数据

				size_t	round;	// 循环编号,编号和TimerManager的循环编号相等时,该对象才被执行.
			};

		public:
			//------------------------------------------------//
			// [7/24/2008] Write by Albert.xu
			// Description	:	初始化时间事件管理器
			// length	:	容器大小,该值越大效率越高
			// return	:
			//------------------------------------------------//
			bool initialize( unsigned int length, unsigned int max_event_object );

			//------------------------------------------------//
			// [7/24/2008] Write by Albert.xu
			// Description	:	插入事件
			// event	:	事件对象
			// return	:	事件句柄
			//------------------------------------------------//
			__inline timer_t insert_event( _tevent* event, unsigned int delay = 0 );

			//---------------------------------------------//
			// [11/21/2008 write by albert]
			// 函数用途	:	insert event
			// function	:	回调函数,可以是成员函数,仿函数或函数指针
			// repeat	:	重复次数
			// timer	:	间隔时间
			// return	:	定时器句柄
			//---------------------------------------------//
			timer_t insert_event( _efunction function, unsigned int repeat, unsigned int interval, unsigned int delay = 0 );

			//---------------------------------------------------//
			// [9/17/2009 Albert]
			// Description:	延迟处理定时器事件
			// handle	:	事件句柄
			// delay	:	延迟多少滴答
			//---------------------------------------------------//
			bool delay_event( unsigned int handle, unsigned int delay );

			//------------------------------------------------//
			// [7/24/2008] Write by Albert.xu
			// Description	:	得到事件对象
			// handle	:	事件句柄
			// return	:	事件对象,未找到返回NULL
			//------------------------------------------------//
			_tevent* get_event( unsigned int handle );

			//------------------------------------------------//
			// [7/24/2008] Write by Albert.xu
			// Description	:	删除事件
			// event	:	事件对象
			//------------------------------------------------//
			unsigned int remove_event( _tevent* event );

			//------------------------------------------------//
			// [7/24/2008] Write by Albert.xu
			// Description	:	删除事件
			// handle	:	事件句柄
			//------------------------------------------------//
			__inline unsigned int remove_event( timer_t handle );

			//---------------------------------------------------//
			// [11/29/2010 Albert]
			// Description:	暂停更新 
			//---------------------------------------------------//
			void pause_event( timer_t handle );

			//---------------------------------------------------//
			// [11/29/2010 Albert]
			// Description:	恢复更新 
			//---------------------------------------------------//
			void resume_event( timer_t handle );
		protected:
			//------------------------------------------------//
			// [7/24/2008] Write by Albert.xu
			// Description	:	插入对象到定时器列表中
			//	handle	:	时间事件对象句柄
			//------------------------------------------------//
			void inner_insert( timer_t handle, unsigned int delay = 0 );

		private:
			struct Impl;
			Impl* m_Impl;

			struct timer_tandle
			{
				unsigned int	position:24;	// 位置位域
				unsigned int	round:8;		// 轮询位域
			};

			// 定时器变量
			size_t	m_next_list;	// 下一个列表
			size_t	m_timer_round;	// 下一个循环

			// 对象池变量
			size_t	m_object_count;	// 事件对象个数
			size_t	m_cur_pos;		// 当前空闲的单元
			size_t	m_round;		// 循环计数
		};

		#define INVALID_TIMER_HANDLE (-1)
		#define TIMER_IMMEDIATELY (0)
		typedef timer::_tevent		_tevent;
		typedef timer::_efunction	_tfunction;
		// #include "TimerManager.inl"
	}
}
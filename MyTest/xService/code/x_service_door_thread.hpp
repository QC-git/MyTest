
#ifdef X_SERVICE_USE_PRAGMAONCE
#pragma once
#endif

#ifndef _SERVOCE_DOOR_THREAD_HEADER
#define _SERVOCE_DOOR_THREAD_HEADER

#if X_SERVICE_USE_SIGNAL > 0
#ifdef _WIN32
#	include <windows.h>
#else
#endif
#endif

#if X_SERVICE_USE_THREAD > 0
#ifdef _WIN32
#	include <process.h>
#else
#	include <pthread.h>
#endif
#endif

//////////////////////////////////////////////////////////////////////////
_SERVOCE_DOOR_BEGIN
	
#if X_SERVICE_USE_SIGNAL > 0

#ifdef _WIN32
#	define _SERVICE_HMUTEX   CRITICAL_SECTION
#else
#	define _SERVICE_HMUTEX   pthread_mutex_t
#endif
#define HMUTEX_D  _SERVICE_HMUTEX

/*
************************************************************************************************************************
*                                                    CLock
*
* Description: 测试
*
* Arguments  : 无
*
* Returns    : 随机
*
* Note(s)    :
*
************************************************************************************************************************
*/
class _SERVICE_CLASS CLock
{
public:
	CLock();
	~CLock();

	VOID_T Lock();
	VOID_T UnLock();

private:
	HMUTEX_D m_hLock;
};

/*
************************************************************************************************************************
*                                                    CAutoLock
*
* Description: 测试
*
* Arguments  : 无
*
* Returns    : 随机
*
* Note(s)    :
*
************************************************************************************************************************
*/
class _SERVICE_CLASS CAutoLock
{
public:
	CAutoLock(CLock* pLock);
	~CAutoLock();

private:
	CLock* m_pLock;
};

#endif



#if X_SERVICE_USE_THREAD > 0

#ifdef _WIN32
#	define _SERVICE_HTHREAD  uintptr_t
# else
#	define _SERVICE_HTHREAD  pthread_t
# endif
#define HTHREAD_D _SERVICE_HTHREAD

typedef void TaskFunc_T(void*);

/*
************************************************************************************************************************
*                                                    CRoundThread
*
* Description: 测试
*
* Arguments  : 无
*
* Returns    : 随机
*
* Note(s)    :
*
************************************************************************************************************************
*/
class _SERVICE_CLASS CRoundThread  // 循环线程类
{
public:
	CRoundThread();
	virtual ~CRoundThread();

	BOOL_T SetThreadName(const CHAR_T* sName);  // 一般放在继承函数构造函数里面

	BOOL_T TStartup();
	BOOL_T TClose();

protected:
	static VOID_T* ThreadProc(VOID_T* hThis);
	VOID_T ProRun();

	virtual BOOL_T DoOncePro();    // 只需执行一次 则重载该函数 并返回true
	virtual VOID_T DoRoundPro() = 0; //  不断循环
	virtual VOID_T DoEndPro();    // 程序结束通知

	BOOL_T WaitForThreadClose(TIME_T lMs);

private:
	HTHREAD_D m_hThread;

	BOOL_T  m_bRunEnable;
	BOOL_T  m_bThreadOpen;
	CHAR_T* m_sThreadName;

};

/*
************************************************************************************************************************
*                                                    CThreadCreater
*
* Description: 测试
*
* Arguments  : 无
*
* Returns    : 随机
*
* Note(s)    :
*
************************************************************************************************************************
*/
class _SERVICE_CLASS CThreadCreater  // 创建线程类，代替传统的创建线程函数
{
public:
	CThreadCreater();
	virtual ~CThreadCreater();

	VOID_T SetThreadName(const CHAR_T* sName);  // 一般放在继承函数构造函数里面

public:
	// 创建成功 返回0， 上个线程还在运行， 返回1， 其它 返回 < 0
	ERR_T CreateThread(TaskFunc_T* pTaskFunc, VOID_T* hTaskFuncUser);

	// 创建线程之前先调用该函数 避免意外失败
	BOOL_T IsHasThreadRun() { return m_bThreadOpen; } 

private: 
	HTHREAD_D m_hThread;

	BOOL_T  m_bThreadOpen;
	CHAR_T* m_sThreadName;

	TaskFunc_T* m_pTaskFunc;
	VOID_T* m_hTaskFuncUser;

	static VOID_T* ThreadProc(VOID_T* hThis);
	VOID_T ThreadProc2();

	// 堵塞函数 会一直等待线程退出
	VOID_T WaitForDestroy(/*long lOutOfTime*/);  

};

/*
************************************************************************************************************************
*                                                    CThreadPool
*
* Description: 线程池，内部为一整组的线程列表，可静态或动态开辟
*
* Arguments  : 无
*
* Returns    : 随机
*
* Note(s)    :
*
************************************************************************************************************************
*/

class _SERVICE_CLASS CThreadPool 
{
public:
	// GetThread() // 获取较为空闲线程

protected:
	// m_pThreadList;  // 线程列表
};

/*
************************************************************************************************************************
*                                                    CThreadThreadEx
*
* Description: 高级线程创建类，派遣，分配任务，内部为线程池，为引擎服务
*
* Arguments  : 无
*
* Returns    : 随机
*
* Note(s)    :
*
************************************************************************************************************************
*/
class _SERVICE_CLASS CThreadThreadEx 
{
public:
	// 派遣，分配单个任务，并返回任务当前所在句柄，

	// 派遣，分配任务引擎

protected:
	// 线程池
};

/*
************************************************************************************************************************
*                                                    CTask
*
* Description: 简单任务
*
* Arguments  : 无
*
* Returns    : 随机
*
* Note(s)    :
*
************************************************************************************************************************
*/
class _SERVICE_CLASS CTask 
{
public:

protected:
	// 任务类型
	
};

/*
************************************************************************************************************************
*                                                    CMessageTask
*
* Description: 消息任务，可发送消息
*
* Arguments  : 无
*
* Returns    : 随机
*
* Note(s)    :
*
************************************************************************************************************************
*/
class _SERVICE_CLASS CMessageTask // : public CTask
{
public:
	//  发送消息

protected:

};

/*
************************************************************************************************************************
*                                                    CTimeTask
*
* Description: 时间任务，可设置延迟时间或下一次执行时间
*
* Arguments  : 无
*
* Returns    : 随机
*
* Note(s)    :
*
************************************************************************************************************************
*/
class _SERVICE_CLASS CTimeTask // : public CTask
{
public:

protected:
	// 延迟时间

};

/*
************************************************************************************************************************
*                                                    CEventTask
*
* Description: 事件任务，可设置延迟时间或下一次执行时间
*
* Arguments  : 无
*
* Returns    : 随机
*
* Note(s)    :
*
************************************************************************************************************************
*/
class _SERVICE_CLASS CEventTask // : public CTask
{
public:

protected:
	// 时间id

};

/*
************************************************************************************************************************
*                                                    CTimeEventTask
*
* Description: 时间事件任务，同时拥有时间和事件的属性
*
* Arguments  : 无
*
* Returns    : 随机
*
* Note(s)    :
*
************************************************************************************************************************
*/
class _SERVICE_CLASS CTimeEventTask // : public CTimeTask, public CEventTask
{
public:

protected:
	// 时间id

	// 延迟时间

};

/*
************************************************************************************************************************
*                                                    CGroupTask
*
* Description: 特殊任务，组合任务， 可将多个任务属性组合
*
* Arguments  : 无
*
* Returns    : 随机
*
* Note(s)    :
*
************************************************************************************************************************
*/
// class _SERVICE_CLASS CGroupTask // : public CTask  // 可能不合理，待评估
// {
// public:
// 
// protected:
// 	// 任务列表
// 
// 	// 任务属性 mask值，只能添加同种属性任务一个
// 
// };

/*
************************************************************************************************************************
*                                                    CTaskEngine
*
* Description: 简单单循环任务引擎
*
* Arguments  : 无
*
* Returns    : 随机
*
* Note(s)    :
*
************************************************************************************************************************
*/
class _SERVICE_CLASS CTaskEngine 
{
public:
	// 为任务列表 添加任务及用户指针

	// virtual 执行任务列表任务

protected:
	// 运行一次任务列表中所有的任务

	// 任务列表
};

/*
************************************************************************************************************************
*                                                    CRoundTaskEngine
*
* Description: 任务引擎，循环执行任务
*
* Arguments  : 无
*
* Returns    : 随机
*
* Note(s)    :
*
************************************************************************************************************************
*/
class _SERVICE_CLASS CRoundTaskEngine // : public CTaskEngine
{
public:
	// 为任务列表 添加任务及用户指针

	// virtual 执行任务列表任务, 循环执行

protected:
	// virtual 运行一次任务列表中所有的任务

	// 任务列表
};

/*
************************************************************************************************************************
*                                                    CTaskRoundEngineEx
*
* Description: 高级循环任务引擎，可自由添加删除任务
*
* Arguments  : 无
*
* Returns    : 随机
*
* Note(s)    :
*
************************************************************************************************************************
*/
class _SERVICE_CLASS CRoundTaskEngineEx // : public CRoundTaskEngine
{
public:
	// 为任务列表 添加任务及用户指针

	// 删除任务

	// virtual 执行任务列表任务, 循环执行

protected:
	// virtual 运行一次任务列表中所有的任务

	// 任务列表
};

/*
************************************************************************************************************************
*                                                    CTimeTaskEngine
*
* Description: 时间任务引擎，可为任务设置，并可以在任务结束时设定下次执行时间
*
* Arguments  : 无
*
* Returns    : 随机
*
* Note(s)    :
*
************************************************************************************************************************
*/
class _SERVICE_CLASS CTimeTaskEngine // : public CRoundTaskEngineEx
{
public:
	// 特殊任务句柄，可以通过返回值确定下次任务执行时间

	// 为任务列表 添加任务及用户指针，及延迟时间

	// 删除任务

	// virtual 执行任务列表任务, 循环执行

protected:
	// virtual 运行一次任务列表中所有的任务

	// 任务列表

	// 任务时间堆

};

/*
************************************************************************************************************************
*                                                    CEventEngine
*
* Description: 异步事件引擎， proactor模型
*
* Arguments  : 无
*
* Returns    : 随机
*
* Note(s)    :
*
************************************************************************************************************************
*/
class _SERVICE_CLASS CEventEngine 
{
public:
	// 添加任务

	// 删除任务

	// virtual 启动 事件循环

protected:
	// 初始事件任务

	// 任务列表，任务指针，用户指针，事件id

	// virtual 事件执行，判断事件是否触发，触发则执行对应任务
};

/*
************************************************************************************************************************
*                                                    CSynEventEngine
*
* Description: 同步事件引擎， reactor模型
*
* Arguments  : 无
*
* Returns    : 随机
*
* Note(s)    :
*
************************************************************************************************************************
*/
class _SERVICE_CLASS CSynEventEngine // : public  CEventEngine
{
public:
	// 添加任务

	// 删除任务

	// virtual 启动 事件循环

protected:
	// 初始事件任务

	// 任务列表，任务指针，用户指针，事件id

	// 高级线程管理句柄

	// virtual 事件执行，判断事件是否触发，触发则利用 线程管理句柄，选择一个线程并执行
};

/*
************************************************************************************************************************
*                                                    CGroupEngine
*
* Description: 特殊引擎，可以将其它引擎组合在一起
*
* Arguments  : 无
*
* Returns    : 随机
*
* Note(s)    :
*
************************************************************************************************************************
*/
class _SERVICE_CLASS CGroupEngine 
{
public:
	// 添加 任务引擎

	// 添加 事件引擎

	// virtual 启动 事件循环， 循环执行各个引擎中的 事件执行

protected:
	// 任务引擎列表

	// 事件引擎列表

};

#endif

/*
************************************************************************************************************************
*                                                    ThreadTest
*
* Description: 测试
*
* Arguments  : 无
*
* Returns    : 随机
*
* Note(s)    :
*
************************************************************************************************************************
*/
_SERVICE_EXPORT ERR_T ThreadTest();

_SERVOCE_DOOR_END
//////////////////////////////////////////////////////////////////////////

#ifdef _SERVICE_DOOR_LOCK
#error  "this file can not include the door"
#endif

#endif
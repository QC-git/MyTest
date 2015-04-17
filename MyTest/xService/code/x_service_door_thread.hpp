
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
* Description: ����
*
* Arguments  : ��
*
* Returns    : ���
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
* Description: ����
*
* Arguments  : ��
*
* Returns    : ���
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
* Description: ����
*
* Arguments  : ��
*
* Returns    : ���
*
* Note(s)    :
*
************************************************************************************************************************
*/
class _SERVICE_CLASS CRoundThread  // ѭ���߳���
{
public:
	CRoundThread();
	virtual ~CRoundThread();

	BOOL_T SetThreadName(const CHAR_T* sName);  // һ����ڼ̳к������캯������

	BOOL_T TStartup();
	BOOL_T TClose();

protected:
	static VOID_T* ThreadProc(VOID_T* hThis);
	VOID_T ProRun();

	virtual BOOL_T DoOncePro();    // ֻ��ִ��һ�� �����ظú��� ������true
	virtual VOID_T DoRoundPro() = 0; //  ����ѭ��
	virtual VOID_T DoEndPro();    // �������֪ͨ

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
* Description: ����
*
* Arguments  : ��
*
* Returns    : ���
*
* Note(s)    :
*
************************************************************************************************************************
*/
class _SERVICE_CLASS CThreadCreater  // �����߳��࣬���洫ͳ�Ĵ����̺߳���
{
public:
	CThreadCreater();
	virtual ~CThreadCreater();

	VOID_T SetThreadName(const CHAR_T* sName);  // һ����ڼ̳к������캯������

public:
	// �����ɹ� ����0�� �ϸ��̻߳������У� ����1�� ���� ���� < 0
	ERR_T CreateThread(TaskFunc_T* pTaskFunc, VOID_T* hTaskFuncUser);

	// �����߳�֮ǰ�ȵ��øú��� ��������ʧ��
	BOOL_T IsHasThreadRun() { return m_bThreadOpen; } 

private: 
	HTHREAD_D m_hThread;

	BOOL_T  m_bThreadOpen;
	CHAR_T* m_sThreadName;

	TaskFunc_T* m_pTaskFunc;
	VOID_T* m_hTaskFuncUser;

	static VOID_T* ThreadProc(VOID_T* hThis);
	VOID_T ThreadProc2();

	// �������� ��һֱ�ȴ��߳��˳�
	VOID_T WaitForDestroy(/*long lOutOfTime*/);  

};

/*
************************************************************************************************************************
*                                                    CThreadPool
*
* Description: �̳߳أ��ڲ�Ϊһ������߳��б��ɾ�̬��̬����
*
* Arguments  : ��
*
* Returns    : ���
*
* Note(s)    :
*
************************************************************************************************************************
*/

class _SERVICE_CLASS CThreadPool 
{
public:
	// GetThread() // ��ȡ��Ϊ�����߳�

protected:
	// m_pThreadList;  // �߳��б�
};

/*
************************************************************************************************************************
*                                                    CThreadThreadEx
*
* Description: �߼��̴߳����࣬��ǲ�����������ڲ�Ϊ�̳߳أ�Ϊ�������
*
* Arguments  : ��
*
* Returns    : ���
*
* Note(s)    :
*
************************************************************************************************************************
*/
class _SERVICE_CLASS CThreadThreadEx 
{
public:
	// ��ǲ�����䵥�����񣬲���������ǰ���ھ����

	// ��ǲ��������������

protected:
	// �̳߳�
};

/*
************************************************************************************************************************
*                                                    CTask
*
* Description: ������
*
* Arguments  : ��
*
* Returns    : ���
*
* Note(s)    :
*
************************************************************************************************************************
*/
class _SERVICE_CLASS CTask 
{
public:

protected:
	// ��������
	
};

/*
************************************************************************************************************************
*                                                    CMessageTask
*
* Description: ��Ϣ���񣬿ɷ�����Ϣ
*
* Arguments  : ��
*
* Returns    : ���
*
* Note(s)    :
*
************************************************************************************************************************
*/
class _SERVICE_CLASS CMessageTask // : public CTask
{
public:
	//  ������Ϣ

protected:

};

/*
************************************************************************************************************************
*                                                    CTimeTask
*
* Description: ʱ�����񣬿������ӳ�ʱ�����һ��ִ��ʱ��
*
* Arguments  : ��
*
* Returns    : ���
*
* Note(s)    :
*
************************************************************************************************************************
*/
class _SERVICE_CLASS CTimeTask // : public CTask
{
public:

protected:
	// �ӳ�ʱ��

};

/*
************************************************************************************************************************
*                                                    CEventTask
*
* Description: �¼����񣬿������ӳ�ʱ�����һ��ִ��ʱ��
*
* Arguments  : ��
*
* Returns    : ���
*
* Note(s)    :
*
************************************************************************************************************************
*/
class _SERVICE_CLASS CEventTask // : public CTask
{
public:

protected:
	// ʱ��id

};

/*
************************************************************************************************************************
*                                                    CTimeEventTask
*
* Description: ʱ���¼�����ͬʱӵ��ʱ����¼�������
*
* Arguments  : ��
*
* Returns    : ���
*
* Note(s)    :
*
************************************************************************************************************************
*/
class _SERVICE_CLASS CTimeEventTask // : public CTimeTask, public CEventTask
{
public:

protected:
	// ʱ��id

	// �ӳ�ʱ��

};

/*
************************************************************************************************************************
*                                                    CGroupTask
*
* Description: ��������������� �ɽ���������������
*
* Arguments  : ��
*
* Returns    : ���
*
* Note(s)    :
*
************************************************************************************************************************
*/
// class _SERVICE_CLASS CGroupTask // : public CTask  // ���ܲ�����������
// {
// public:
// 
// protected:
// 	// �����б�
// 
// 	// �������� maskֵ��ֻ�����ͬ����������һ��
// 
// };

/*
************************************************************************************************************************
*                                                    CTaskEngine
*
* Description: �򵥵�ѭ����������
*
* Arguments  : ��
*
* Returns    : ���
*
* Note(s)    :
*
************************************************************************************************************************
*/
class _SERVICE_CLASS CTaskEngine 
{
public:
	// Ϊ�����б� ��������û�ָ��

	// virtual ִ�������б�����

protected:
	// ����һ�������б������е�����

	// �����б�
};

/*
************************************************************************************************************************
*                                                    CRoundTaskEngine
*
* Description: �������棬ѭ��ִ������
*
* Arguments  : ��
*
* Returns    : ���
*
* Note(s)    :
*
************************************************************************************************************************
*/
class _SERVICE_CLASS CRoundTaskEngine // : public CTaskEngine
{
public:
	// Ϊ�����б� ��������û�ָ��

	// virtual ִ�������б�����, ѭ��ִ��

protected:
	// virtual ����һ�������б������е�����

	// �����б�
};

/*
************************************************************************************************************************
*                                                    CTaskRoundEngineEx
*
* Description: �߼�ѭ���������棬���������ɾ������
*
* Arguments  : ��
*
* Returns    : ���
*
* Note(s)    :
*
************************************************************************************************************************
*/
class _SERVICE_CLASS CRoundTaskEngineEx // : public CRoundTaskEngine
{
public:
	// Ϊ�����б� ��������û�ָ��

	// ɾ������

	// virtual ִ�������б�����, ѭ��ִ��

protected:
	// virtual ����һ�������б������е�����

	// �����б�
};

/*
************************************************************************************************************************
*                                                    CTimeTaskEngine
*
* Description: ʱ���������棬��Ϊ�������ã����������������ʱ�趨�´�ִ��ʱ��
*
* Arguments  : ��
*
* Returns    : ���
*
* Note(s)    :
*
************************************************************************************************************************
*/
class _SERVICE_CLASS CTimeTaskEngine // : public CRoundTaskEngineEx
{
public:
	// ����������������ͨ������ֵȷ���´�����ִ��ʱ��

	// Ϊ�����б� ��������û�ָ�룬���ӳ�ʱ��

	// ɾ������

	// virtual ִ�������б�����, ѭ��ִ��

protected:
	// virtual ����һ�������б������е�����

	// �����б�

	// ����ʱ���

};

/*
************************************************************************************************************************
*                                                    CEventEngine
*
* Description: �첽�¼����棬 proactorģ��
*
* Arguments  : ��
*
* Returns    : ���
*
* Note(s)    :
*
************************************************************************************************************************
*/
class _SERVICE_CLASS CEventEngine 
{
public:
	// �������

	// ɾ������

	// virtual ���� �¼�ѭ��

protected:
	// ��ʼ�¼�����

	// �����б�����ָ�룬�û�ָ�룬�¼�id

	// virtual �¼�ִ�У��ж��¼��Ƿ񴥷���������ִ�ж�Ӧ����
};

/*
************************************************************************************************************************
*                                                    CSynEventEngine
*
* Description: ͬ���¼����棬 reactorģ��
*
* Arguments  : ��
*
* Returns    : ���
*
* Note(s)    :
*
************************************************************************************************************************
*/
class _SERVICE_CLASS CSynEventEngine // : public  CEventEngine
{
public:
	// �������

	// ɾ������

	// virtual ���� �¼�ѭ��

protected:
	// ��ʼ�¼�����

	// �����б�����ָ�룬�û�ָ�룬�¼�id

	// �߼��̹߳�����

	// virtual �¼�ִ�У��ж��¼��Ƿ񴥷������������� �̹߳�������ѡ��һ���̲߳�ִ��
};

/*
************************************************************************************************************************
*                                                    CGroupEngine
*
* Description: �������棬���Խ��������������һ��
*
* Arguments  : ��
*
* Returns    : ���
*
* Note(s)    :
*
************************************************************************************************************************
*/
class _SERVICE_CLASS CGroupEngine 
{
public:
	// ��� ��������

	// ��� �¼�����

	// virtual ���� �¼�ѭ���� ѭ��ִ�и��������е� �¼�ִ��

protected:
	// ���������б�

	// �¼������б�

};

#endif

/*
************************************************************************************************************************
*                                                    ThreadTest
*
* Description: ����
*
* Arguments  : ��
*
* Returns    : ���
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
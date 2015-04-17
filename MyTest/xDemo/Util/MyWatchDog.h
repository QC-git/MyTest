#pragma once

#define RK_RF_SERVER         "RF_Server2.1.exe"
#define RK_RF_SERVER_MUTEX   RKGUI_TEXTW("RF_Server_Mutex")

#define RK_WATCHDOG_ENABLE  0   // ʹ���������Ź�
#define RK_NOFEEDDOG_ENABLE 1   // ʹ�ܲ���Ҫι��
#if !RK_NOFEEDDOG_ENABLE
#define RK_FEEDDOG_INTERVAL 10   // ι������С���ʱ��
#endif 

class CRkWatchDog;


class CWatchDogParameters
{
public:
	CWatchDogParameters();
	~CWatchDogParameters();

	struct SData
	{
		char sDogName[64];    // ���Ź�����
		char sServerName[64]; // ��ʹ��

		BOOL bLastProStatus;
		BOOL bIsEverLogin;         // �Ƿ�������¼
		BOOL bIsEverStartService;  // �Ƿ�������������

		BOOL bEnterProStatus;      // ��������־��־Ϊ1, �˳�ʱ��Ϊ0��ÿ�����г���ʱ���ñ�־
		BOOL bLogin;
		BOOL bStartService;
	};

public:
	int Read();
	int Write();

// 	bool IsEverLogin() { return bIsEverLogin }
// 
 	SData* GetDataPtr() { return &m_stData; }

private:
	SData m_stData;

	bool m_bIsReadFileSuc;

	void ResetData(); 

	friend class CRkWatchDog;

};

class CRkWatchDog
{
public:
	static CRkWatchDog* Create();  // һ������ֻ�ܴ���һ���� �ظ��������������٣������ʧ��
	~CRkWatchDog();

public:
	BOOL_T CheckIsOnlyOneApp();

	int  EnterPro();  // 0�������������� 1���쳣�������� <0�����ʧ��

	void LeavePro();  // ��������رճ���

	void Feed();  // ����ι������

	BOOL IsEverLogin() { return m_cParams.m_stData.bIsEverLogin; }

	BOOL IsEverStartSevice() { return m_cParams.m_stData.bIsEverStartService; }

	BOOL IsLastCloseError() { return m_cParams.m_stData.bLastProStatus; }

	void SaveLoginStatus(BOOL bStatus) { m_cParams.m_stData.bLogin = bStatus; m_cParams.Write(); }

	void SaveServiceStatus(BOOL bStatus) { m_cParams.m_stData.bStartService = bStatus; m_cParams.Write(); }

protected:
	CRkWatchDog();

	BOOL RegisterServer(BOOL bRegister);

	BOOL_T StartWatchThread();

	void StopWatchThread();

private:
	static int s_nCreateCount;

	CWatchDogParameters m_cParams;

};


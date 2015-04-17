#pragma once

#define RK_RF_SERVER         "RF_Server2.1.exe"
#define RK_RF_SERVER_MUTEX   RKGUI_TEXTW("RF_Server_Mutex")

#define RK_WATCHDOG_ENABLE  0   // 使能启动看门狗
#define RK_NOFEEDDOG_ENABLE 1   // 使能不需要喂狗
#if !RK_NOFEEDDOG_ENABLE
#define RK_FEEDDOG_INTERVAL 10   // 喂狗的最小间隔时间
#endif 

class CRkWatchDog;


class CWatchDogParameters
{
public:
	CWatchDogParameters();
	~CWatchDogParameters();

	struct SData
	{
		char sDogName[64];    // 看门狗名字
		char sServerName[64]; // 不使用

		BOOL bLastProStatus;
		BOOL bIsEverLogin;         // 是否曾经登录
		BOOL bIsEverStartService;  // 是否曾经启动服务

		BOOL bEnterProStatus;      // 进入程序标志标志为1, 退出时设为0，每次运行程序时检查该标志
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
	static CRkWatchDog* Create();  // 一个程序只能创建一个， 重复创建必须先销毁，否则会失败
	~CRkWatchDog();

public:
	BOOL_T CheckIsOnlyOneApp();

	int  EnterPro();  // 0：正常启动程序， 1：异常启动程序， <0：检查失败

	void LeavePro();  // 标记正常关闭程序

	void Feed();  // 进行喂狗操作

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


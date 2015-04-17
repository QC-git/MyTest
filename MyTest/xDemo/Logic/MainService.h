#pragma once

#include "RkUtil.h"
#include "DataBaseOperater.h"
#include "ServerParameters.h"
#include "RfServiceClientTest.h"
#include "RfServece.h"
#include "MyWatchDog.h"


class CMainService
{
public:
	CMainService();
	~CMainService();

public:
	int OnInit();        // 放在CWinApp::InitInstance();

	void OnUnInit();    //  放在CWinApp::ExitInstance(); 

	void OnCreate();    //  放在CDialog::OnInitDialog();

	void OnDestroy();   //  放在CDialog::OnDestroy();

	void OnMonitor();   //  放在CDialog::OnTimer() 500ms 一次

	int DoStart();  // 一键启动 

public:
	DataBaseLogin* m_pLogin;

	DataBaseReader* m_pDataBaseReader;

	RfService* m_pRfService;

	CRfServiceClient* m_pTestClient;

	RfDevVarsInFile_RW* m_pFileDevInfoReader;

	CRkWatchDog* m_pDog; 

private:

	bool m_bIsReadDbConfig;

	bool m_bIsNeedConnect1;
	bool m_bIsNeedConnect2;

	int  m_bConnectFailCount1;
	int  m_bConnectFailCount2;

	int m_nMonitorCount;

	bool m_bIsFatalError;

	bool m_bIsAutoConnectDb;

};

extern CMainService g_cService;
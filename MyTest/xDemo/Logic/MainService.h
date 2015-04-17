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
	int OnInit();        // ����CWinApp::InitInstance();

	void OnUnInit();    //  ����CWinApp::ExitInstance(); 

	void OnCreate();    //  ����CDialog::OnInitDialog();

	void OnDestroy();   //  ����CDialog::OnDestroy();

	void OnMonitor();   //  ����CDialog::OnTimer() 500ms һ��

	int DoStart();  // һ������ 

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
#pragma once

#include "RkUtil.h"
/*#include "ServerDevice.h"*/
#include "ServerParameters.h"
#include "RfServiceClientTest.h"
#include "MyWatchDog.h"
#include "ListAdapter.h"

extern bool g_bIsUserMfcMsgBox;
#ifndef MY_MSGBOX_F
#define MY_MSGBOX_F(str) { if (g_bIsUserMfcMsgBox) MessageBox(str, _T("��ʾ")); }
#endif

#define MY_SUPER_USERNAME  "ROPEOK"
#define MY_SUPER_PASSWORD  "ROPEOK"

class DataBaseLogin
{
public:
	DataBaseLogin();
	~DataBaseLogin();

	friend class RfService;

	static enum LOGIN_STATUS
	{
		LOGIN_STATUS_NO_CONNECTED,
		LOGIN_STATUS_CONECTING,
		LOGIN_STATUS_CONECTED
	};

	static class LoginInfoCallBack
	{
	public:
		virtual void OnLogionStatus(bool bIsLogionSuc, void* dummy) = 0;   // �첽�ص�
	};

	LOGIN_STATUS LoginStatus() const { return m_emLoginStatus; }

	//void SetLoginThreadEnable(bool bIsLoginUseThread = true) { m_bIsLoginUseThread = bIsLoginUseThread; };

	// ʹ��ǰ�ȵ��� LoginStatus()  �ж�״̬
	int Login(const char* sUser, const char* sPwd, bool bIsUseThread = true);  

	int Logout(const char* sUser, const char* sPwd);
	int Logout(bool bIsCheckUser = true);

	int Login2(const char* sUser, const char* sPwd);
	int Logout2(const char* sUser, const char* sPwd);
	bool IsLogin() { return LOGIN_STATUS_CONECTED == m_emLoginStatus ? true : false; }

	int ReadDbConfig() { return m_cDataBaseInfo.Read(); }

	int ConnectToDb(int nIndex);

	bool IsAlive(int nIndex);

	BOOL Ping();

	void SetLoginCallBack(LoginInfoCallBack* pInfoCallBack) { m_pLoginInfoCallBack = pInfoCallBack; }

	DataBaseInfo_RW* DataBaseInfo() { return &m_cDataBaseInfo; }

private:
	LOGIN_STATUS m_emLoginStatus;

	//bool m_bIsLoginUseThread;
	int Login(bool bIsUseThread);

	int CheckUserInfo(const char* sUser, const char* sPwd);

	char* m_sUser;
	char* m_sPwd;
	int AssignUserInfo(const char* sUser, const char* sPwd);
	void ResetUserInfo();

	int AssignDbInfo();
	SAString m_sastrDBInfo, m_sastrDbUser, m_sastrDbPassword;  // ���ݿ�������Ϣ
	SAString m_sastrDBInfo2, m_sastrDbUser2, m_sastrDbPassword2;  // ��������ݿ�

	//HANDLE m_hConSerThread;
	bool m_bThreadExit;
	//bool m_bThreadBeenExit;

	X::CThreadCreater* m_pThreadCreater;

	X::CLock m_lock;

	// �ڲ�����Ϊ�Ͽ������������Ƿ������������ⲿ������Ҫ������Ծ�����ݿ����ӣ�������øú���
	static void ConnectThread(void* hUser) { ((DataBaseLogin *)hUser)->ConnectSerThread2(); }
	void ConnectSerThread2(); 

	//static DWORD ConnectSerThread3(LPVOID lpParam) { ((DataBaseLogin *)lpParam)->ConnectSerThread2(); return NULL; }

	LoginInfoCallBack* m_pLoginInfoCallBack;

	DataBaseInfo_RW m_cDataBaseInfo;
};

class DataBaseReader
{
public:
	DataBaseReader(DataBaseLogin* pLogin);
	~DataBaseReader();

	friend class RfService;

	static class ReadDataInfoCallBack
	{
	public:
		virtual void OnReadReaderData(int nCount, const VEC_ITER_PREADER_T& vecReaderInfos) = 0;   // ͬ���ص�  nCount > 0

		virtual void OnReadRegionData(int nCount, const VEC_ITER_PREGION_T& vecRegionInfos) = 0;

	};

	typedef X::CValueHashMap<int, RF_INFO_READER_IN> READER_MAP_T;

	typedef X::CValueHashMap<int, RF_INFO_REGION_POINT_IN> REGION_POINT_MAP_T;
	typedef X::CValueHashMap<int, RF_INFO_REGION_LINE_IN>  REGION_LINE_MAP_T;
	typedef X::CValueHashMap<int, RF_INFO_REGION_RECT_IN>  REGION_RECT_MAP_T;

	static class ReadDataInfoCallBack2
	{
	public:
		virtual void OnReadData(READER_MAP_T& cMap1, REGION_POINT_MAP_T& cMap2, REGION_LINE_MAP_T& cMap3, REGION_RECT_MAP_T& cMap4) = 0;   // ͬ���ص�  nCount > 0
	};

public:
	//DataBaseLogin* GetDataBaseLoginPtr() { return m_pLogin; }

	int ReadFromDataBase();

	int ReadFromDataBase2();

	void SetReadDataCallBack(ReadDataInfoCallBack* pReadDataCallBack) { m_pReadDataCallBack = pReadDataCallBack; }

	void SetReadDataCallBack2(ReadDataInfoCallBack2* pReadDataCallBack) { m_pReadDataCallBack2 = pReadDataCallBack; }

	void PrintData(int nIndex);

	//RfDevVarsInFile_RW* DeviceInfoInFile() { return &m_cDevPramas; }

protected:
	VEC_PREADER_T m_vec_Readers;
	VEC_PREGION_T m_vec_Regions;

	void FreeReadersAndRegions();

	int ReadServerId(int* pOutServerId);
	int ReadReaders();
	int ReadRegions(int nServerId);
	int ReadRegionsForLink();

	//RfDevVarsInFile_RW m_cDevPramas;

	ReadDataInfoCallBack* m_pReadDataCallBack;

	DataBaseLogin* m_pLogin;

public:
	READER_MAP_T m_cReaderMap;

	REGION_POINT_MAP_T m_cRegionPointMap;
	REGION_LINE_MAP_T  m_cRegionLineMap;
	REGION_RECT_MAP_T  m_cRegionRectMap;

	ReadDataInfoCallBack2* m_pReadDataCallBack2;

	void FreeAllMap();

};

#define RFDEV_PARAMS_FILEPATH    "rf_dev_parameters.ini"
#define RFDEV_PARAMS_BASE_SEC        "rf_base"
#define RFDEV_PARAMS_READER_SEC      "rf_reader"

#define RFDEV_PARAMS_BASE_SEC_READER_NUM          "reader_num"                // �豸������
#define RFDEV_PARAMS_BASE_SEC_USE_READER_PONINT   "reader_point_use"          // �Ƿ�ʹ�ý��������������� 
#define RFDEV_PARAMS_BASE_SEC_USE_ALL_125K_PONINT "reader_point_125k_use_all" // �Ƿ��Զ��������125k ID

#define RFDEV_PARAMS_READER_SEC_ID               "id"
#define RFDEV_PARAMS_READER_SEC_IP               "ip"
#define RFDEV_PARAMS_READER_SEC_PORT             "port"
#define RFDEV_PARAMS_READER_SEC_POINT_X          "point_x"
#define RFDEV_PARAMS_READER_SEC_POINT_Y          "point_y"

//typedef short BOOL16_T;

class RfDevVarsInFile_RW  // ���ڶ�ȡ�ļ��� ������������������Ϣ
{
public:
	RfDevVarsInFile_RW();
	virtual ~RfDevVarsInFile_RW();

	struct SData
	{
		int nReaderNum;
		int  bIsUseReaderPoint;
		int  bIsAutoAdd125kPoint;

		RF_INFO_READER_IN* pReaderList;
	};

	static class ReadDataInfoCallBack
	{
	public:
		virtual void OnReadReaderData(const SData* pDataList) = 0;   // ͬ���ص�  nCount > 0

	};

	void SetReadDataCallBack(ReadDataInfoCallBack* pReadDataCallBack) { m_pCallBack = pReadDataCallBack; }

public:
	virtual int Read(); 

	virtual bool IsDataReadSuc() { return m_bIsReadSuc; }  // ����Ƿ��ȡ�ɹ� �ɹ�����true�� 

	SData* GetDataPtr() { return m_bIsReadSuc ? &m_stData : NULL; }

private:
	void FreeData();

	int ReadBaseData(const char* pFilePath);  // ��ȡ�������ݣ�������ȫ���

	int ReadReaderData(const char* pFilePath, int nIndex, RF_INFO_READER_IN* pSink);  // ��ȡ���������ݣ�������ȫ���

private:
	bool m_bIsReadSuc;

	SData m_stData;

	ReadDataInfoCallBack* m_pCallBack;

};

// class CDataBaseConnecter
// {
// public:
// 	CDataBaseConnecter();
// 
// 
// public:
// 
// 
// };




#pragma once

#include "RkUtil.h"

#include "RF_Position_API_v2.h"
#include "DataBaseOperater.h"
#include "ListAdapter.h"
#include <set>
#include <map>
#include <queue>
#include "x_thread.h"

extern bool g_bIsUserMfcMsgBox;
#ifndef MY_MSGBOX_F
#define MY_MSGBOX_F(str) { if (g_bIsUserMfcMsgBox) MessageBox(str); }
#endif

#ifndef MY_MSGBOX_ENABLE_F
#define MY_MSGBOX_ENABLE_F(bFlag) { g_bIsUserMfcMsgBox = bFlag; }
#endif

#ifndef MY_RFSERVER_DEBUG
#define MY_RFSERVER_DEBUG
#endif

class RfTransport;
class RfXmlHanlder;
class RfExportTagFilter;
class RfDataBaseWriter;

class RfService
{

public:
	static RfService* CreateService();   // ֻ�ܴ���һ�����ظ��������������Ѵ����ģ������ʧ��; 
	static void DestoryService(RfService* pRfService);

public:
	int StartCheck(DataBaseReader* pDataReader);  // ʹ�����ݿ�����
	int StartCheck(RfDevVarsInFile_RW* pDataReader);  // ʹ���ļ�����

	int StartCheck2(DataBaseReader* pDataBaseReader);  // ʹ����������

	//����
	int StartTestCheck();  // ʹ����������
	int StartTestCheck2();  // ʹ����������, ��ʵ�òο���
	int StartTestCheck3();  // ʹ����������, ʹ��125k
	

	void StopCheck();

	bool InitDefautInfo();

	void SetTagInfoCallBack(RF_CALLBACK_TAG pTagInfoCallBack, DWORD hTagInfoCallBackUser);

	RfTransport* GetTransport() { return m_pTrans; }
	void TestXmlInfo();

	int StartTransService();
	void StopTransService();

	RfExportTagFilter* GetTagFilter() { return m_pFilter; }
	RfDataBaseWriter* GetDbWriter() { return m_pDbWriter; }

	bool IsStart() { return m_bIsRfStart; }

protected:
	RfService();
	~RfService();

private:
	bool m_bIsRfStart;

	RF_CALLBACK_TAG m_pTagInfoCallBack;
	DWORD m_hTagInfoCallBackUser;

	RfTransport* m_pTrans;

	RfExportTagFilter* m_pFilter;

	RfDataBaseWriter* m_pDbWriter;

private:
	static int s_nCreateCount;

};

class RfXmlHanlder
{
public:
	RfXmlHanlder(RfTransport* pTrans);
	~RfXmlHanlder();

public:
	int InsertTagInfo(RF_INFO_TAG* pTagInfoList, int nTagNum);
	int InsertReaInfo(RF_INFO_READER* pReaderInfoList, int nReaderNum);

private:
	CSendingQueue* m_pSentQue;

};

#define MAX_OBJ_NUM 20   

class RfTransport: protected X::CRoundThread
{
public:
	RfTransport();
	~RfTransport();

	struct STagPack 
	{
		int nLen;
		int nType;      //��Ϣ����,TRANS_READER_NUMS
		int nCheckA;
		int nCheckB;

		long lMovingTime;   //ʱ���

		int  nObjNum;   //��λ����������
		RF_INFO_TAG Objs[MAX_OBJ_NUM];	//Ŀ�������Ϣ
	};

	struct SReaPack
	{
		int nLen;
		int nType;      //��Ϣ����,TRANS_READER_NUMS
		int nCheckA;
		int nCheckB;

		long lMovingTime;   //ʱ���

		int  nObjNum;   //��λ����������
		RF_INFO_READER Objs[MAX_OBJ_NUM];	//Ŀ�������Ϣ
	};

	int  Init(TRANS_METHOD transMethod, char* pszIPOwner, unsigned short nPortOwner);
	void UnInit();

	//void SetTransHandle(CRopeokTrans* pRpkTrans) { m_pRpkTrans = pRpkTrans; }

	int  StarTrans(int nQLen = 10,int nQNodeSize = 1024);
	void StopTrans();

public:
	//Override
	void DoRoundPro();

	int InsertTagInfoToSendQueue(RF_INFO_TAG* pTagInfoList, int nTagNum);
	int InsertReaInfoToSendQueue(RF_INFO_READER* pReaderInfoList, int nReaderNum);

	int InsertTagInfoToSendQueueByXml(RF_INFO_TAG* pTagInfoList, int nTagNum);
	int InsertReaInfoToSendQueueByXml(RF_INFO_READER* pReaderInfoList, int nReaderNum);

	static void ServerReceiveProc(void *pUserData, SOCKET sockCon, char *pDataBuf, int nLen)
	{
		return ((RfTransport*)pUserData)->ServerReceiveProc2(sockCon, pDataBuf, nLen);
	}

	void ServerReceiveProc2(SOCKET sockCon, char *pDataBuf, int nLen);

	static void ServerReceiveProc_Control(void *pUserData, SOCKET sockCon, char *pDataBuf, int nLen);

private:
	std::set<SOCKET>		m_setConSockets;
	std::map<SOCKET, int>	m_mConSktFailCount; // ͳ�Ʒ�������ʧ�ܴ����������ʹ�������һ��ֵʱ��Ϊ��Socket��Ч

	CSendingQueue	m_SendingQ;
	CSendingQueue	m_SendingQEmergent;

	CRopeokTrans* m_pRpkTrans;
	int           m_nDurSend;
	char*		  m_pTransBuf;

	RfXmlHanlder m_cXmlHanlder;

	friend class RfXmlHanlder;

	//bool m_bInitialize;

	//bool m_bIsStart;
};

class RfTagGoodsToPeopleBindList
{
public:
	RfTagGoodsToPeopleBindList() {;}
	~RfTagGoodsToPeopleBindList() {;}

	struct SBindInfo
	{
		long lTagId;     
		long lBindTagId;   
	};

	typedef X::CValueHashMap<long, SBindInfo> BingTagMap;

	BingTagMap* GetList() { return &m_cMap; }

public:
	int Updata();  // �����б���Ϣ

protected:
	BingTagMap m_cMap;

	int LoadVirtualData();

};

typedef X::CValueHashMap<long, RF_INFO_TAG*> ExportTagInfoMap_T;
typedef X::CValueSet<long> ExportTagIdSet_T;

typedef X::CValueHashMap<int, RF_INFO_READER*> ExportReaderInfoMap_T;

class RfExportTagFilter
{
public:
	RfExportTagFilter();
	~RfExportTagFilter();

	void Filter(RF_INFO_TAG* tagList, int tagListNum, ExportTagInfoMap_T& cDataMap);

protected:
	void HandleSingleTagInfo();

	RfTagGoodsToPeopleBindList m_cBindList;
};


class RfDataBaseWriter
{
public:
	RfDataBaseWriter();
	virtual ~RfDataBaseWriter();


public:
	void UpdataTagInfoToDb(RF_INFO_TAG* tagList, int tagListNum, ExportTagInfoMap_T& cDataMap);

	void UpdataReaderInfoToDb(RF_INFO_READER* readerList, int readerListNum, ExportReaderInfoMap_T& cDataMap);

};






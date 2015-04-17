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
	static RfService* CreateService();   // 只能创建一个，重复创建必须销毁已创建的，否则会失败; 
	static void DestoryService(RfService* pRfService);

public:
	int StartCheck(DataBaseReader* pDataReader);  // 使用数据库数据
	int StartCheck(RfDevVarsInFile_RW* pDataReader);  // 使用文件数据

	int StartCheck2(DataBaseReader* pDataBaseReader);  // 使用配置数据

	//调试
	int StartTestCheck();  // 使用虚拟数据
	int StartTestCheck2();  // 使用虚拟数据, 不实用参考卡
	int StartTestCheck3();  // 使用虚拟数据, 使用125k
	

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
		int nType;      //信息类型,TRANS_READER_NUMS
		int nCheckA;
		int nCheckB;

		long lMovingTime;   //时间戳

		int  nObjNum;   //定位接收器数量
		RF_INFO_TAG Objs[MAX_OBJ_NUM];	//目标具体信息
	};

	struct SReaPack
	{
		int nLen;
		int nType;      //信息类型,TRANS_READER_NUMS
		int nCheckA;
		int nCheckB;

		long lMovingTime;   //时间戳

		int  nObjNum;   //定位接收器数量
		RF_INFO_READER Objs[MAX_OBJ_NUM];	//目标具体信息
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
	std::map<SOCKET, int>	m_mConSktFailCount; // 统计发送数据失败次数，当发送次数超过一定值时认为该Socket无效

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
	int Updata();  // 更新列表信息

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






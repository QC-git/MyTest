#pragma once

#include "RF_Constant.h"
#include "RF_Algorithm.h"
#include "RF_NetTransmit.h"
#include "RF_Position_API_v2.h"

//////////////////////////////////////////////////////////////////////////


struct RF_SDataUnit  //���������� ������  ��Ϣ��Ԫ
{
	int nUpdataCount;   //  ��ǰ�����µĴ��� Max 10000

	long lTime; // ��ȡʱ��ʱ��

	UCHAR tyPower;  //���ĵ���  ���Ϊ3�� Ϊ1ʱ����
	     
	UCHAR Rssi[5]; // ��¼��ȥ5��ǿ��ֵ
	float fRelRssi;  //���˺��ǿ��ֵ

	int nAntID;     // 125k����id
	int nAntRssi;   // 125k����ǿ��
};

struct RF_DataBase_AntEx_M
{
	int nAntID;

	int nAntRssi;

	long nAntTime;
};

struct RF_DataBase_TagEx_M
{
	int nTagId;

	RF_DataBase_AntEx_M stAntInfo;

};

class RF_CDataPool  // ���ݳ�
{
public:
	RF_CDataPool();
	~RF_CDataPool();

	static const int REA_ROOM_NUM=100;     //�������
	static const int TAG_ROOM_NUM=1000;   
	  
	float ReadRssi(UINT ReaID, long TagID);
	float ReadRssi(UINT ReaID, long TagID, bool& bIsOutIfTime);  // �Ƿ��ǳ�ʱ���� 
	bool  ReadCompletelyData(UINT ReaID, long TagID, RF_SDataUnit* pData_out);
	UINT  ReadTagForStrongestRssis(long TagID, UINT uiInfoNum, UINT* pReaderID_out, float* pRssi_out=NULL);    

	UINT Get_TagIDList(long* lTagID_out);
	UINT Get_ReaIDList(UINT* uiReaID_out);
	
	void test();
	void test2();

	int ReadAntId(int nTagId);
	int ReadAntRssi(int nTagId);
	int ReadAntData(int nTagId, RF_DataBase_AntEx_M& stInfo_out, bool& bIsOutTime);

	void  Clear();  //return  remove count

protected:
	bool SaveData(
		UINT uiReaID, 
		long lTagID,
		UCHAR byRssi,
		UCHAR byPower,
		int nAntID,
		int nAntRssi);

	bool SaveAntData(int nTagId, int nAntId, int nAntRssi);

private:
	bool m_bPoolInit;
	CRITICAL_SECTION m_csData;

	RF_SDataUnit* m_pDataBase_List;
	UINT*          m_pReaID_List;
	long*          m_pTagID_List;

	RF_DataBase_TagEx_M* m_pTagAntInfoList;  //������ ��Ϣ�б�

	long GetRoomIndex(UINT uiReaID, long lTagID, bool bIsAutoAdd = false); // true Ϊ�Զ�����ID

	bool GetPriData(RF_SDataUnit* pData_out);

};

//////////////////////////////////////////////////////////////////////////

#define READERMANAGER_NORMALMODE  0
#define READERMANAGER_SERVERMODE  1
#define READERMANAGER_CLIENTMODE  2

#define READERMANAGER_SOCKET_TESTMODE    9


struct RF_ReaderBase_M
{
	UINT ReaderID;

	char IP[16];
	UINT Port;

	floatP Location;

	bool IsOpen;
	bool IsHavData;

	long gotDataTime; // seconds
};

class RF_CReaderManager:public RF_CDataPool
	,protected CTCP_ServerToListen_M  
	,protected CTCP_Client_M
	,protected MDoubleThread
{
public:
	static RF_CReaderManager* CreateNew(int nManagerMode);

public:
	~RF_CReaderManager();

	bool IsInitSuc() const { return m_bManagerInit; }

	static const int READER_MAX_NUM=100;
	const int READER_MANAGE_MODE;

	bool Add_ReaderInfo(UINT uiID, const char* sIP, UINT uiPort, float fPos_X, float fPos_Y);
	int  Get_ReaderInfo(RF_ReaderBase_M* pData_out);  //all  return count

	bool IsReaderNomal(UINT uiID);

	//-------------------------------

	int  StartThread();  // �ɹ����ؿ��ٵ��߳��� >0
	bool CloseThread();

	void SetUserHandle(DWORD hHandle) { m_hUserHandle = hHandle; }
	void SetAllReaderInfoCallBack(RF_CALLBACK_READER cb) { m_cbAllReaderInfo = cb; }

	void ClearAll(); //����ֹͣ�߳� ���������

	int GetReadersInfo(RF_CALLBACK_READER pAfterGetFunc, DWORD hUser);

	int SetRssiInfoCallBack(RF_GET_RSSI_INFO_FUNC pAfterGetFunc, DWORD hUser);

protected:
	RF_CReaderManager(int nManagerMode);

	bool m_bIsThreadStart;  //�����д���һ���߳�ʱ��

	bool Startup_TReaderOpen();  //����������
	void  Close_TReaderOpen();

	bool Startup_TDataTransmit(); //���ݴ����߳�
	void  Close_TDataTransmit();

private:
	RF_ReaderBase_M* m_pReaderList;   // �������б�
	int m_nReaderCount;
	CRITICAL_SECTION m_csManager;
	
	bool m_bManagerInit;

	RF_INFO_READER* m_pReaderStatusInfo; // ���ڻ��棬ͨ���ص����������״̬��Ϣ
	int m_nReaderStatusInfoNum;
	//--------------------------------------

	int m_nSticks1;
	//Override
	void roundPro1();

	bool OpenReader(UINT uiID,const char* sIP, UINT uiPort);
	void OpenNextReader();
	int  OpenAllReader();

	int m_nNextOpenIndex;

	//---------------------------------------

	char* m_pRfPackList;           //1000 
	RfPacketEx* m_pRfPackBuff;     //500    

	char* m_psentBuff;

	int m_nSticks2;
	//Override
	void roundPro2();

	int DataTransmit();

	//---------------------------------------
	bool OnRecieved(  //����
		UINT uiReaID, 
		long lTagID,
		UCHAR byRssi,
		UCHAR byPower);

	DWORD m_hUserHandle;
	RF_CALLBACK_READER m_cbAllReaderInfo; 

	RF_GET_RSSI_INFO_FUNC m_RssiInfoCallBack;
	DWORD m_RssiInfoCallBackUser;
};
